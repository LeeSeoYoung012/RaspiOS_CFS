#include "sched.h"
#include "irq.h"
#include "printf.h"
#include "rbtree.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}




static void update_curr(struct cfs_rq * cfs_rq)
{
	struct sched_entity * curr = cfs_rq->curr;
	u64 now = rq_of(cfs_rq)->clock;
}

void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

/* void insert_rbnode(struct task_struct * t ){
     struct sched_entity * t_se = t->se;
	 struct rb_node * trb = t_se->run_node;
	 t_se->run_node.color = BLACK;
	 t_se->run_node.parent = NULL;
	 t_se->right = NULL;
	 t_se->left = NULL;
	 rb_entry ()
}*/
void _schedule(void)
{
	preempt_disable();
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	switch_to(task[next]);
	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}


void schedule_tail(void) {
	preempt_enable();
}


void timer_tick()
{
	--current->counter;
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	current->counter=0;
	enable_irq();
	_schedule();
	disable_irq();
}
