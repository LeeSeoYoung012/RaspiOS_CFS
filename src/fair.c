#include "sched.h"
#include "types.h"

unsigned int sysctl_sched_latency = 6000000ULL; // 6ms 기본 스케줄링 레이턴시
unsigned int sysctl_sched_min_granularity = 750000ULL; // 0.75ms 태스크에 보장되는 최소 실행시간
static unsigned int sched_nr_latency =8; //ssl/ssmg 스케줄링 레이턴시를 사용할 수 있는 태스크개수
                                            //2.25*8 = 18
#define __verify_pcpu_ptr(ptr)						\
do {									\
	const void __percpu *__vpp_verify = (typeof((ptr) + 0))NULL;	\
	(void)__vpp_verify;						\
} while (0)

#define VERIFY_PERCPU_PTR(__p)						\
({									\
	__verify_pcpu_ptr(__p);						\
	(typeof(*(__p)) __kernel __force *)(__p);			\
})
#define per_cpu_ptr(ptr, cpu)	({ (void)(cpu); VERIFY_PERCPU_PTR(ptr); })
#define per_cpu(var, cpu)	(*per_cpu_ptr(&(var), cpu))
#define cpu_rq(cpu)		(&per_cpu(runqueues, (cpu)))
#define task_rq(p)		cpu_rq(task_cpu(p))
#define for_each_sched_entity(se) \
		for (; se; se = NULL)


static inline struct task_struct *task_of(struct sched_entity *se)
{
	return container_of(se, struct task_struct, se);
}

static inline struct cfs_rq *task_cfs_rq(struct task_struct *p)
{
	return &task_rq(p)->cfs;
}

static inline struct cfs_rq *cfs_rq_of(struct sched_entity *se)
{
	struct task_struct *p = task_of(se);
	struct rq *rq = task_rq(p);

	return &rq->cfs;
}

static inline void update_load_add(struct load_weight *lw, unsigned long inc)
{
    lw->weight += inc;
    lw->inv_weight =0;
}

static inline void update_load_sub(struct load_weight *lw, unsigned long dec)
{
	lw->weight -= dec;
	lw->inv_weight = 0;
}

static inline void update_load_set(struct load_weight *lw, unsigned long w)
{
	lw->weight = w;
	lw->inv_weight = 0;
}

static u64 __sched_period(unsigned long nr_running)
{
    if(unlikely(nr_running>sched_nr_latency))
        return nr_running * sysctl_sched_min_granularity;
    else
        return sysctl_sched_latency;
    
}

static u64 sched_slice(struct cfs_rq *cfs_rq, struct sched_entity *se)
{
 u64 slice = __sched_period( cfs_rq->nr_running + !se->on_rq );
 //cfs 런큐에서 동작중인 태스크들+1(스케줄 엔티티가 런큐에 있지 않는 경우)
for_each_sched_entity(se){
      struct load_weight *load;
      struct load_weight lw;

      cfs_rq = cfs_rq_of(se);
      load = &cfs_rq->load;
      if(unlikely(!se->on_rq)){//만약 런큐에 없다면
          lw = cfs_rq->load;
          update_load_add(&lw, se->load.weight);
          load = &lw;
      }
      slice = __calc_delta(slice, se->load.weight, load);
}
      return slice;
}
