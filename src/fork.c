#include "sched.h"
#include "entry.h"
#include "printf.h"
#include "mm.h"
#include "entry.h"
 /* int copy_process(unsigned long fn, unsigned long arg)
{
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) get_free_page();
	if (!p)
		return 1;
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

	p->cpu_context.x19 = fn;
	p->cpu_context.x20 = arg;
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;
	int pid = nr_tasks++;
	//task[pid] = p;	
	preempt_enable();
	return 0;
}*/


 int copy_process(unsigned long fn, unsigned long arg)
{
    preempt_disable(&cpu0_rq.cfs);
	struct task_struct *p;
	//struct sched_entity * se;
	struct cfs_rq * cfs_rq = &cpu0_rq.cfs;
	p = (struct task_struct *) get_free_page();
	//cfs_rq -> curr = se;

    p->se.load.weight = 1024;
	//cfs_rq -> curr = &se;
    p->se.cfs_rq =    cfs_rq;
    p->se.on_rq			= 0;
	p->se.exec_start		= 0;
	p->se.sum_exec_runtime		= 0;
	p->se.prev_sum_exec_runtime	= 0;
	//task->se.nr_migrations		= 0;
	p->se.vruntime			= 0;
	p->state =TASK_RUNNING;
	p->preempt_count 		= 1;
	//task->stack					= 0;
	p->flags					= PF_KTHREAD;
	p->cpu_context.x19 = fn;
	p->cpu_context.x20 = arg;
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;
   	int pid = ++tasknum;
	p->pid = pid;
	task[pid] = p;
    printf("fork pid: %d  fork arg:%d\n",pid,arg);
	task_fork_fair(task[pid],pid);
	//printf("while fork preempt_count:%d",curr_task->preempt_count);
	//cfs_rq->curr = &task->se;
    preempt_enable(&cpu0_rq.cfs);
	
	//place_entity(&cpu0_rq.cfs,&task->se,1);
	
	return 0;
}

 
