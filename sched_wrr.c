/*
 * Weighted Round Robin Scheduling Class (mapped to the SCHED_WRR
 * policy)
*/


/*
	This function will find the process to use in round robin scheduling
	it scans the linked list to find the process.
	@returns a pointer to the entity group if found else it return a null pointer
*/
static struct sched_wrr_entity_group* find_wrr_user(struct list_head *user_list, int id) {
	struct sched_wrr_entity_group *selected_user_process;
	struct sched_wrr_entity_group *next_user_process;

	//loop through linked llist in a safe way
	list_for_each_entry_safe(selected_user_process, next_user_process, user_list, run_list_users){
		if(selected_user_process->id == id)
			return selected_user_process; //found
	}
	return NULL; //not found
}

/*
	This adds the entity group to the run queue
*/
static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int wakeup) {
	struct sched_wrr_entity_group *entity;
	
	//find the user process note the rq and p attributes were defined by rao in sched.c
	entity = find_wrr_user(&(rq->wrr.ready_tasks), p->wrr_group);
	
	//if process exists
	if(!entity) {
		entity = kmalloc(sizeof(struct sched_wrr_entity_group), GFP_ATOMIC);
		if(!entity) { printk(KERN_INFO "FAIL: process not found\n"); return; }
	
		//initialize new list
		INIT_LIST_HEAD(&entity->run_list_users);
		INIT_LIST_HEAD(&entity->run_list);

		//initialize values
		entity->id = p->wrr_group;
		entity->curr_time_slice = DEF_TIMESLICE * 8; //this number can change
		entity->num_processes = 0;
		rq->wrr.wrr_nr_users++;
	
		//place entity into rq
		list_add_tail(&(entity->run_list_users), &(rq->wrr.ready_tasks));
	}
	//insert task struct of process into group
	list_add_tail(&(p->wrr.run_list), &(entity->run_list));

	//increment counters
	rq->wrr.wrr_nr_running++;
	entity->num_processes++;
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int sleep) {
	struct sched_wrr_entity_group *entity;

	//find the user process note the rq and p attributes were defined by rao in sched.c
	entity = find_wrr_user(&(rq->wrr.ready_tasks), p->wrr_group);	
	if(!entity) { printk(KERN_INFO "FAIL: process not found\n"); return; }

	//delete task from ready processes
	list_del_init(&(p->wrr.run_list));
	entity->num_processes--;
	
	//check if group is empty
	if(entity->num_processes == 0){
		//get rid of group from runqueu
		rq->wrr.wrr_nr_users--;
		list_del_init(&(entity->run_list_users));
		kfree(entity);
	}

	//remove from running tasks
	rq->wrr.wrr_nr_running--;
}

static void requeue_wrr_entity(struct wrr_rq *wrr_rq, struct sched_wrr_entity_group *wrr_se, int head){
	if(wrr_rq->wrr_nr_users <= 1)
		return; //return if  more than one already exost
	
	//move from run_list_usersa and readd to ready_tasks
	list_move_tail(&(wrr_se->run_list_users), &(wrr_rq->ready_tasks)); 
}

static void requeue_task_wrr(struct rq *rq, struct task_struct *p, int head){
	struct sched_wrr_entity_group *entity;

	//find the user process note the rq and p attributes were defined by rao in sched.c
	entity = find_wrr_user(&(rq->wrr.ready_tasks), p->wrr_group);	
	if(!entity) {
		printk(KERN_INFO "FAIL: process not found\n"); 
		return; 
	}

	//determine if group is empty
	if(entity->num_processes <= 1) 
		return;
	
	//move task to runable processses
	list_move_tail(&(p->wrr.run_list), &(entity->run_list));
}

static void yield_task_wrr(struct rq *rq) { /*do nothing*/ }
static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags) { /*do nothing*/}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
	struct sched_wrr_entity *new_entity;
	struct sched_wrr_entity_group *new_entity_group;
	struct list_head *head;
	
	//determine if there are processes to run
	if(rq->wrr.wrr_nr_running <= 0)
		return NULL;
	
	//find entry for new entity group (groups)
	head = &(rq->wrr.ready_tasks); //give it reference to ready tasks 
	new_entity_group = list_entry(head->next, struct sched_wrr_entity_group, run_list_users);

	//find entry for entity (process)
	head = &(new_entity_group->run_list);
	new_entity = list_entry(head->next, struct sched_wrr_entity, run_list);
	
	//return value
	if(new_entity)
		return entity->process;
	return NULL;
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p) {/*do nothing*/}

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

static void set_curr_task_wrr(struct rq *rq) {
	struct task_struct *p = rq->curr;
	p->se.exec_start = rq->clock;
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued) {
	struct sched_wrr_entity_group *entity;

	//find the user process note the rq and p attributes were defined by rao in sched.c
	entity = find_wrr_user(&(rq->wrr.ready_tasks), p->wrr_group);	
	if(!entity) {
		printk(KERN_INFO "FAIL: process not found\n"); 
		return; 
	}
	//decrement the time allcated for the process
	p->wrr.time_slice--;

	//decrement time allocated for the the group
	entity->curr_time_slice--;

	if(p->wrr.time_slice > 0 && entity->curr_time_slice > 0)
		return;
	
	//check if time has expired
	if(entity->curr_time_slice == 0) {
		//reset time slice and place back in queue
		entity->curr_time_slice = DEF_TIMESLICE * 8;
		requeue_wrr_entity(&rq->wrr, entity, 0);
	}

	//check if process has expired in time
	if(p->wrr.time_slice == 0)
	{
		//reset process time and place back in quee
		p->wrr.time_slice = DEF_TIMESLICE * p->wrr.weight;
		requeue_task_wrr(rq, p, 0);
	}

	//let OS know know we need to reschedule the process
	resched_task(p);
}

unsigned int get_rr_interval_wrr(struct task_struct *task) {
    	//Time slice is 0 for SCHED_FIFO tasks
        if (task->policy == SCHED_WRR) return DEF_TIMESLICE;
        else return 0;
}

/* added by Jia Rao: No preemption, so we leave this function empty */
static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio, int running) { /*nothing*/ }
static void switched_to_wrr(struct rq *rq, struct task_struct *p, int running) {/*nothing*/}

static const struct sched_class wrr_sched_class = {
	.next			= &idle_sched_class,
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

