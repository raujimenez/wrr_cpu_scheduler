/*
 * Weighted Round Robin Scheduling Class (mapped to the SCHED_WRR
 * policy)
 */


static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int wakeup)
{
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int sleep)
{
}

static void
requeue_wrr_entity(struct wrr_rq *wrr_rq, struct sched_wrr_entity *wrr_se, int head)
{
}

static void requeue_task_wrr(struct rq *rq, struct task_struct *p, int head)
{
}

static void yield_task_wrr(struct rq *rq)
{
}
static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags)
{
}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p)
{
}

#ifdef CONFIG_SMP
static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags)
{
	return task_cpu(p);
}

static unsigned long
load_balance_wrr(struct rq *this_rq, int this_cpu, struct rq *busiest,
                unsigned long max_load_move,
                struct sched_domain *sd, enum cpu_idle_type idle,
                int *all_pinned, int *this_best_prio)
{
        /* don't touch WRR tasks */
        return 0;
}

static int
move_one_task_wrr(struct rq *this_rq, int this_cpu, struct rq *busiest,
                 struct sched_domain *sd, enum cpu_idle_type idle)
{
        return 0;
}


#endif
static void set_curr_task_wrr(struct rq *rq)
{
	struct task_struct *p = rq->curr;

	p->se.exec_start = rq->clock;
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
}

unsigned int get_rr_interval_wrr(struct task_struct *task)
{
        /*
 *          * Time slice is 0 for SCHED_FIFO tasks
 *                   */
        if (task->policy == SCHED_WRR)
                return DEF_TIMESLICE;
        else
                return 0;
}
/* added by Jia Rao: No preemption, so we leave this function empty */
static void prio_changed_wrr(struct rq *rq, struct task_struct *p,
                              int oldprio, int running)
{
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p,
                           int running)
{
}

static const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,
	.yield_task		= yield_task_wrr,

	.check_preempt_curr	= check_preempt_curr_wrr,

	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr,

	.load_balance		= load_balance_wrr,
	.move_one_task		= move_one_task_wrr,
#endif

	.set_curr_task          = set_curr_task_wrr,
	.task_tick		= task_tick_wrr,

	.get_rr_interval	= get_rr_interval_wrr,

//	.prio_changed		= prio_changed_wrr,
	.switched_to		= switched_to_wrr,
};

