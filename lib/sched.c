#include<env.h>
#include<os_printf.h>
int remaining_time;
void sched_yield(){
    static int pos = -1;
	//printf("sched!\n");
	while(1){
		pos = (pos+1)%NENV;
		if(envs[pos].env_status==ENV_RUNNABLE){
			env_run(&envs[pos]);
		}
	}
}