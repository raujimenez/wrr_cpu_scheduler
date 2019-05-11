#include <linux/kernel.h>
#include <linux/sched.h>

asmlinkage int set_wrr_scheduler(int pid, int weight){
	//find task struct given pid
    struct task_struct *process_ts;
    process_ts = pid_task(find_vpid(pid), PIDTYPE_PID);
	
    //if not present return a negative weight 
    if(process_ts == NULL)
        return -1;
	process_ts->wrr.weight = weight;
    process_ts->policy = SCHED_WRR;
    return 0;
}
