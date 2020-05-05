#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__

#define THREAD_SIZE				4096

#define NR_TASKS				64 

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

#define TASK_RUNNING				0
#define TASK_INTERRUPTIBLE			1
#define TASK_UNINTERRUPTIBLE        3
#define TASK_ZOMBIE				4
#define NR_TASKS				64 
#define PF_KTHREAD		            	0x00000002	//clone_flags인지 아닌지 확인
#define size_t		unsigned long
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

#define rb_entry(ptr, type, member) container_of(ptr,type,member)


#if NR_CPUS > 1
#define CONFIG_SMP
#else
#undef CONFIG_SMP
#endif

#define NR_CPUS	1
#define nr_cpu_ids		1U 
#define NICE_0_LOAD	 (1L<<10)
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	INIT_SCHED_ENTITY,0,0,0,0, PF_KTHREAD \
}


#define INIT_SCHED_ENTITY \ 
				{ {1024,0}, \
				0,0,0,0,0,0,&cpu0_rq.cfs,NULL,NULL \
}


struct list_head {
	struct list_head *next, *prev;
};



#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list->prev = list;
}

static inline void
__list_add(struct list_head *entry,
                struct list_head *prev, struct list_head *next)
{
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

static inline void list_add(struct list_head *entry, struct list_head *head)
{
    __list_add(entry, head, head->next);
}



#include "rbtree.h"
#include "types.h"
//#include "llist.h"


struct cpu_context {
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;
	unsigned long sp;
	unsigned long pc;
};

struct load_weight{
	unsigned long weight;
	int inv_weight;
};

struct sched_entity{
	struct load_weight	load;
	unsigned int		on_rq;
	u64			vruntime;
	u64			exec_start;
    u64			sum_exec_runtime;
	u64 		prev_sum_exec_runtime;
	u64			start_runtime;
	struct cfs_rq			*cfs_rq;
	struct cfs_rq			*my_q;
	struct sched_entity		*parent;
};


extern const int		sched_prio_to_weight[40];
//extern struct task_struct *current;

//extern int nr_tasks;

struct cfs_rq {
    struct load_weight	load;
	unsigned long		runnable_weight;
	unsigned int		nr_running;
	//unsigned int		h_nr_running;      /* SCHED_{NORMAL,BATCH,IDLE} */
	unsigned int		idle_h_nr_running; /* SCHED_IDLE */
    
	u64			exec_clock;
	u64			min_vruntime;
#ifndef CONFIG_64BIT
	u64			min_vruntime_copy;
#endif

    struct rq		*rq;
	struct rb_root_cached tasks_timeline;
	struct list_head	leaf_cfs_rq_list;
	struct task_group	*tg;	/* group that "owns" this runqueue */
	struct sched_entity	*curr;
	struct sched_entity	*next;
	struct sched_entity	*last;
	struct sched_entity	*skip;
};



extern struct list_head task_groups;

struct task_group {
	struct sched_entity	**se;
	struct cfs_rq		**cfs_rq;
	unsigned long		shares;
	//struct rcu_head		rcu;
	struct list_head	list;

	struct task_group	*parent;
	struct list_head	siblings;
	struct list_head	children;
};
struct task_group root_task_group;

struct rq{
		//raw_spinlock_t		lock;
		unsigned int nr_running;
		
		struct load_weight load;
		unsigned long nr_load_updates;
		u64 nr_switches ;
        int     cpu;
		struct cfs_rq cfs;
		//struct rt_rq rt;  //실시간 런큐
		//struct dl_rq dl;
	    unsigned long nr_uninterruptible;//task중 uninterruptible개수
		struct task_struct *curr;
		struct task_struct *idle; 
		struct task_struct *stop;	
		u64			clock;
		u64			clock_task;
		struct list_head	cfs_tasks;	
};
struct rq root_rq;

//#ifdef CONFIG_FAIR_GROUP_SCHED

/* CPU runqueue to which this cfs_rq is attached */
static inline struct rq *rq_of(struct cfs_rq *cfs_rq)
{
	return cfs_rq->rq;
}

/* #else

static inline struct rq *rq_of(struct cfs_rq *cfs_rq)
{
	return container_of(cfs_rq, struct rq, cfs);
}
#endif*/

int scheduler_running;


//extern void switch_to(struct task_struct * next, int index);
extern void sched_init(void);
extern void schedule(void);
void switch_to (struct task_struct * next, struct task_struct * prev);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern void exit_process(void);
void preempt_enable(struct cfs_rq * cfs_rq );
void preempt_disable(struct cfs_rq * cfs_rq );
static inline struct task_struct *task_of(struct sched_entity *se);
void timer_tick(struct cfs_rq * cfs_rq, struct sched_entity *curr);
static inline u64 calc_delta_fair(u64 delta, struct sched_entity *se);
static u64 sched_vslice(struct cfs_rq *cfs_rq, struct sched_entity *se);
static void __update_inv_weight(struct load_weight *lw);
static inline u64 mul_u64_u32_shr(u64 a, u32 mul, unsigned int shift);
static inline u64 mul_u32_u32(u32 a, u32 b);
static u64 __calc_delta(u64 delta_exec, unsigned long weight, struct load_weight* lw);
static u64 sched_slice(struct cfs_rq *cfs_rq, struct sched_entity *se);
static u64 __sched_period(unsigned long nr_running);
void _schedule(void);
static void update_curr(struct cfs_rq *cfs_rq);
static int update_min_vruntime(struct cfs_rq *cfs_rq);
void place_entity(struct cfs_rq*cfs_rq, struct sched_entity *se, int initial);
void set_next_entity(struct cfs_rq * cfs_rq, struct sched_entity *se);
void init_tg_cfs_entry(struct task_group*tg, struct cfs_rq* cfs_rq, struct rq* rq, struct sched_entity *se, int cpu, struct sched_entity *parent);
static inline void update_load_set(struct load_weight *lw, unsigned long w);
void init_cfs_rq(struct cfs_rq *cfs_rq);
void cpu0_init_rq(struct rq * rq);
void init_root_task_group(struct task_group * root_task_group);
unsigned long max_vruntime(u64 max_vruntime, u64 vruntime);
void task_fork_fair(struct task_struct * p, int pid);
void init_task_all(struct task_struct *task[]);
void exit_all_process();



int tasknum;
struct task_struct {
	struct cpu_context cpu_context;
    struct sched_entity se;
	long state;	
	long counter;
	long preempt_count;
    int pid;
    unsigned long flags;
	
};
//struct task_struct * task_arr[NR_TASKS];
struct task_group root_task_group;
struct rq cpu0_rq;
struct task_struct * task[NR_TASKS];



#endif
#endif

