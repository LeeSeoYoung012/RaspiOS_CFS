#include "sched.h"
#include "printf.h"
#include "timer.h"
#include "irq.h"
unsigned int sysctl_sched_latency = 6000000ULL;
unsigned int sysctl_sched_min_granularity = 750000ULL;
static unsigned int sched_nr_latency =8;
static struct task_struct init_task = INIT_TASK;

tasknum=0;
#define WMULT_CONST (~0U)

void init_task_all(struct task_struct *task[])
{
  for(int i=0; i<=NR_TASKS; i++)
  {
    task[i]= &init_task;
  }
}

void init_root_task_group(struct task_group * root_task_group){

  unsigned long ptr =0;
  ptr += 2 * nr_cpu_ids * sizeof(void **);
  
  root_task_group->se = (struct sched_entity **)ptr;
  ptr += nr_cpu_ids * sizeof(void **);

  root_task_group->cfs_rq = (struct cfs_rq **)ptr;
  ptr += nr_cpu_ids * sizeof(void **);

  root_task_group->shares = NICE_0_LOAD; //share: enqueue 된 cfs 런큐에서 cpu 시간을 할당받을때의 상대적인 비율을 결정하는 필드
  LIST_HEAD(task_groups); //prev와 next를 자기 자신이 가르킴

  list_add(&root_task_group->list, &task_groups); //task groups와 root_task_group 안에 list를 연결
  INIT_LIST_HEAD(&root_task_group->children); // 자식 task group 초기화(prev와 next를 자기 자신이 가르킴)
  INIT_LIST_HEAD(&root_task_group->siblings); //형제 task group 초기화(prev와 next를 자기자신이 가르킴)

  return;

}

void cpu0_init_rq(struct rq * rq)
{
	rq->nr_running=0;
	rq->nr_switches=0; 
	rq->nr_load_updates =0; //cpu_load 필드를 갱신한 횟수를 나타내는 필드 
	rq->nr_uninterruptible=0;
	init_cfs_rq(&rq->cfs); 
	rq->cpu = 0; //cpu id
	INIT_LIST_HEAD(&rq->cfs_tasks);//enqueue 된 태스크가 모두 연결되어있는 cfs_tasks를 초기화
}

void init_cfs_rq(struct cfs_rq *cfs_rq){
	cfs_rq->tasks_timeline = RB_ROOT_CACHED;//레드블랙 트리의 root노드를 나타내는 필드
	//cfs_rq->min_vruntime = (u64)(-(1LL << 20));//
  update_load_set(&cfs_rq->load,0);   
  cfs_rq->min_vruntime = 0;
  place_entity(cfs_rq,&task[0]->se,1);
  
}


static inline void update_load_set(struct load_weight *lw, unsigned long w)
{
	lw->weight = w;
	lw->inv_weight = 0;
}

/* cpu0번만 고려  */
void init_tg_cfs_entry(struct task_group*tg, struct cfs_rq* cfs_rq, struct rq* rq, struct sched_entity *se, int cpu, struct sched_entity *parent)
{
cfs_rq->tg = tg;
cfs_rq->rq = rq;
tg->cfs_rq[cpu] = cfs_rq;
tg->se[cpu]=se;
if(!se) return; //root_task_group일 때는 se는 NULL이 된다. 

se->my_q = cfs_rq;
update_load_set(&se->load, NICE_0_LOAD);
se->parent = parent;

}


void sched_init()
{
  init_task_all(task);
  init_root_task_group(&root_task_group);
  cpu0_init_rq(&cpu0_rq);
  init_tg_cfs_entry(&root_task_group, &cpu0_rq.cfs,&cpu0_rq,NULL,0,NULL);
  scheduler_running =1;
 }


void set_next_entity(struct cfs_rq * cfs_rq, struct sched_entity *se)
{
  u64 now;

  asm volatile("mrs %x0, CNTPCT_EL0" : "=r" (now));
  se->exec_start = now; 
  cfs_rq->curr =se;
  se->prev_sum_exec_runtime = se->sum_exec_runtime;
}

void place_entity(struct cfs_rq*cfs_rq, struct sched_entity *se, int initial)
{
  u64 vruntime = cfs_rq -> min_vruntime;
  vruntime+=sched_vslice(cfs_rq,se); //task 0 일 때 6ms 가 됨
  cfs_rq->load.weight += se->load.weight;
  se->vruntime = max_vruntime(se->vruntime,vruntime);
  printf("place_entity se's vruntime: %d\n",se->vruntime);
    if(initial == 1 ){
     u64 now;
     asm volatile("mrs %x0, CNTPCT_EL0" : "=r" (now));
     se->exec_start =  now;
     cfs_rq->curr =se;
    }
}

unsigned long max_vruntime(u64 max_vruntime, u64 vruntime)
{
	s64 delta = (s64)(vruntime - max_vruntime);
	if (delta > 0)
		max_vruntime = vruntime;

	return max_vruntime;
}

static int update_min_vruntime(struct cfs_rq *cfs_rq)
{
    struct sched_entity * task0_se = &task[0]->se;
    struct task_struct * p;
    int next=0;
    int min = task0_se->vruntime;

  for (int i = 1; i <=tasknum; i++){
			p = task[i];

			if (p && p->state == TASK_RUNNING && (min >(p->se.vruntime))) {
             
				     min = p->se.vruntime;
				     next = i;
			}
		}
    cfs_rq -> min_vruntime = min;

    return next;
}

static void update_curr(struct cfs_rq *cfs_rq)
{
  struct sched_entity * curr = cfs_rq->curr;
  u64 now; 
  asm volatile("mrs %x0, CNTPCT_EL0" : "=r" (now));
  if(now>18000000){  generic_timer_stop();  exit_all_process();}
  u64 delta_exec;
  if(!curr)return;
  delta_exec = now - curr->exec_start;
  if((s64)delta_exec<=0) return;
  curr->exec_start = now;
  curr->sum_exec_runtime += delta_exec;
  curr->vruntime += calc_delta_fair(delta_exec, curr);
  update_min_vruntime(cfs_rq);

}

void switch_to (struct task_struct * next, struct task_struct * prev) 
{
  printf("\n\n\n\r---------Task switch ----------\r\n");
  printf("\n\rtask[%d] vruntime = %d\n\r" ,next->pid , next->se.vruntime);
  printf("task[%d] sum_exec_runtime = %d\n\r" ,next->pid , next->se.sum_exec_runtime);
  printf("task[%d] prev_sum_exec_runtime = %d\n\r",next->pid ,next->se.prev_sum_exec_runtime);
  printf("task[%d] exec_start = %d\n\r",next->pid ,next->se.exec_start);
  printf("\n\r---------------------------\r\n");

	cpu_switch_to(prev, next);
}

void schedule(void)
{
  _schedule();
}

void _schedule(void)
{
  struct cfs_rq * cfs_rq = &cpu0_rq.cfs;
  preempt_disable(cfs_rq);
  int next;
  struct sched_entity * prev = cfs_rq->curr;
  next = update_min_vruntime(cfs_rq);

  if(task_of(prev)->pid==next){preempt_enable(cfs_rq);return;}
  set_next_entity(cfs_rq,&task[next]->se);
  switch_to(task[next],task_of(prev));
  cpu0_rq.nr_switches++;
  cfs_rq->curr->on_rq =1;

  preempt_enable(cfs_rq);
}


void task_fork_fair(struct task_struct * p, int pid){
  struct cfs_rq *cfs_rq = &cpu0_rq.cfs;
  struct sched_entity *se = &p->se, *curr;
  curr = cfs_rq->curr;
  place_entity(cfs_rq,se,0);
 
     se->vruntime -=cfs_rq->min_vruntime;
    
}
 
static u64 __sched_period(unsigned long nr_running)
{
    if(nr_running>sched_nr_latency)
        return nr_running * sysctl_sched_min_granularity;
    else
        return sysctl_sched_latency;
    
}

static u64 sched_slice(struct cfs_rq *cfs_rq, struct sched_entity *se)
{
  u64 slice = __sched_period(cfs_rq->nr_running + !se->on_rq);
  
    struct load_weight *load;
    struct load_weight  lw;
  
  cfs_rq = &cpu0_rq.cfs;
  load = &cfs_rq->load;
  if(!se->on_rq){
    lw = cfs_rq->load;
    lw.weight += se->load.weight;
   // printf("before on_rq cfs total weight: %d \n",lw.weight);
    lw.inv_weight = 0;
    load = &lw;
  }
  slice = __calc_delta(slice,se->load.weight,load);
  return slice;
}

static u64 __calc_delta(u64 delta_exec, unsigned long weight, struct load_weight* lw)
{
  u64 fact = weight;
  int shift = 32;
  __update_inv_weight(lw);
  if(fact>>32){
    while(fact>>32)
         fact>>=1;
         shift --;
  }
    fact = mul_u32_u32(fact, lw->inv_weight);//weight * lw.inv_weight
   while (fact >> 32) {
		fact >>= 1;
		shift--;
	}
   return mul_u64_u32_shr(delta_exec, fact, shift);//del
}

static inline u64 mul_u32_u32(u32 a, u32 b)
{
	return (u64)a * b;
}

static inline u64 mul_u64_u32_shr(u64 a, u32 mul, unsigned int shift)
{
	u32 ah, al;
	u64 ret;

	al = a;
	ah = a >> 32;

	ret = mul_u32_u32(al, mul) >> shift;
	if (ah)
		ret += mul_u32_u32(ah, mul) << (32 - shift);

	return ret;
}

static void __update_inv_weight(struct load_weight *lw)
{
    unsigned long w;
    w = lw->weight;
    if(lw->inv_weight)
         return;
    
    if(w>=WMULT_CONST)
         lw->inv_weight =1;
    else if(!w)
         lw->inv_weight = WMULT_CONST;
    else
         lw->inv_weight = WMULT_CONST/w;
}

static u64 sched_vslice(struct cfs_rq *cfs_rq, struct sched_entity *se)
{
	return calc_delta_fair(sched_slice(cfs_rq, se), se);
}

static inline u64 calc_delta_fair(u64 delta, struct sched_entity *se)
{
    if(se->load.weight != NICE_0_LOAD)
        delta = __calc_delta(delta,NICE_0_LOAD,&se->load);
    
    return delta;
//NICE_0_LOAD를 기준으로 엔티티의 load weight가 크다면 delta는 작아지고 load weight가 작다면 delta는 커짐
}



void timer_tick(struct cfs_rq * cfs_rq, struct sched_entity *curr)
{
  unsigned long ideal_runtime, delta_exec;
  struct task_struct *curr_task = task_of(curr); 
  u64 delta;
  update_curr(cfs_rq);
  ideal_runtime = sched_slice(cfs_rq,curr);
  delta_exec = curr->sum_exec_runtime - curr->prev_sum_exec_runtime;
  printf("curr_task sum_exec_runtime:%d\n",curr->sum_exec_runtime);
  printf(" task0 vruntime:%d task1 vruntime: %d, task2 vruntime: %d\n",task[0]->se.vruntime,task[1]->se.vruntime, task[2]->se.vruntime);
  printf("ideal_runtime: %d\n",ideal_runtime);
  if(delta_exec > ideal_runtime && curr_task->preempt_count<=0 ){
   //    printf("preempt_count:%d",curr_task->preempt_count);
       enable_irq();
       printf("timer_tick resched\n");
       	_schedule();
	     disable_irq();    
  }
}

static inline struct task_struct *task_of(struct sched_entity *se)
{
	return container_of(se, struct task_struct, se);
}

void preempt_disable(struct cfs_rq * cfs_rq )
{
  struct task_struct * curr = task_of(cfs_rq -> curr);
  curr->preempt_count++;
}

void preempt_enable(struct cfs_rq * cfs_rq )
{
  struct task_struct * curr = task_of(cfs_rq -> curr);
	curr->preempt_count--;
}


void schedule_tail(void) {
	preempt_enable(&cpu0_rq.cfs);
}

void exit_all_process(){
  struct cfs_rq * cfs_rq = &cpu0_rq.cfs;
  preempt_disable(cfs_rq);
  for (int i=0; i<=tasknum; i++){
      task[i]->state = TASK_ZOMBIE;
      free_page(task[i]);
  }
  preempt_enable(cfs_rq);
  schedule();

}