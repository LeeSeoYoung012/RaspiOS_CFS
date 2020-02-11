
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

#define PF_KTHREAD		            	0x00000002	//clone_flags인지 아닌지 확인
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#define container_of(ptr, type, member) ({	\
		const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
		(type *)( (char *)__mptr - offsetof(type,member) );})

#define rb_entry(ptr, type, member) container_of(ptr,type,member)


#include "rbtree.h"
#include "types.h"
#include "llist.h"



extern const int		sched_prio_to_weight[40];
extern struct task_struct *current;
extern struct task_struct * task[NR_TASKS];
extern int nr_tasks;




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

struct rq{
		//raw_spinlock_t		lock;
		unsigned int nr_running;
		
		struct load_weight load;
		unsigned long nr_load_updates;
		u64 nr_switches ;

		struct cfs_rq cfs;
		//struct rt_rq rt;  //실시간 런큐
		//struct dl_rq dl;
	    unsigned long nr_uninterruptible;//task중 uninterruptible개수
		struct task_struct *curr;
		struct task_struct *idle; 
		struct task_struct *stop;	
		u64			clock;
		struct list_head	cfs_tasks;	
};

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


struct cfs_rq {
    struct load_weight	load;
	unsigned long		runnable_weight;
	unsigned int		nr_running;
	unsigned int		h_nr_running;      /* SCHED_{NORMAL,BATCH,IDLE} */
	unsigned int		idle_h_nr_running; /* SCHED_IDLE */

	u64			exec_clock;
	u64			min_vruntime;
#ifndef CONFIG_64BIT
	u64			min_vruntime_copy;
#endif

//	struct rb_root_cached tasks_timeline;

	/*
	 * 'curr' points to currently running entity on this cfs_rq.
	 * It is set to NULL otherwise (i.e when none are currently running).
	 */
	struct sched_entity	*curr;
	struct sched_entity	*next;
	struct sched_entity	*last;
	struct sched_entity	*skip;
};




struct sched_entity{
	struct load_weight	load;
	struct rb_node	run_node;
//	struct list_head 	group_node;
	unsigned int		on_rq;
	u64			exec_start;
    u64			sum_exec_runtime;
	u64			last_wakeup;
	u64			avg_overlap;
	u64			nr_migrations;
	u64			start_runtime;
	u64			avg_wakeup;	

};

struct task_struct {
	struct cpu_context cpu_context;
	long state;	
	long counter;
	long priority;
	long preempt_count;
	unsigned long stack;
	unsigned long flags;
	struct sched_entity se;
};

//extern void switch_to(struct task_struct * next, int index);
extern void sched_init(void);
extern void schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct * next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern void exit_process(void);

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0, 0, PF_KTHREAD \
}

#endif
#endif
