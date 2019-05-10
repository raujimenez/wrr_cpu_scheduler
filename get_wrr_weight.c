#include <linux/kernel.h>
#include <linux/sched.h>

asmlinkage int get_wrr_weight(int pid){
	//find task struct given pid
    struct task_struct *process_ts;
    process_ts = pid_task(find_vpid(pid), PIDTYPE_PID);
	
    //if not present return a negative weight else return the weight
    if(process_ts == NULL)
        return -1;
	return process_ts->wrr.weight;
}
