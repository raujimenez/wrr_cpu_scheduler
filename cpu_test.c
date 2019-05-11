#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <sched.h>
#include <pthread.h>


#define __NR_set_wrr_scheduler 338
#define __NR_get_wrr_weight 337
#define SCHED_WRR 6

void bind_thread_to_cpu(int cpuid) {
     cpu_set_t mask;
     CPU_ZERO(&mask);

     CPU_SET(cpuid, &mask);
     if (sched_setaffinity(0, sizeof(cpu_set_t), &mask)) {
         fprintf(stderr, "sched_setaffinity");
         exit(EXIT_FAILURE);
     }
}

void main(){

    int pid_child;
    struct sched_param weight;
    
    pid_child = fork();
    
    if(pid_child > 0){ //parent
        weight.sched_priority = 1;
        printf("Process %d has weight: %d \n" , getpid(), weight.sched_priority);
    }else{
        weight.sched_priority = 2;
        printf("Process %d has weight: %d \n" , getpid(), weight.sched_priority);
    }
    fflush(stdout);
   
    bind_thread_to_cpu(1); 
    syscall(__NR_set_wrr_scheduler, getpid(), SCHED_WRR,  &weight );
    int weight_k = syscall(__NR_get_wrr_weight, getpid());
   
    printf("%d has started its job\n weight: %d \n ", getpid(), weight_k);
    int i;
    int job = 0;
    for(i = 0; i < 10000000; i++){
	job = 0;
	while(1)job++;
	//printf("%d",getpid());
    }
    printf("%d has finished its job \n",getpid());

}
