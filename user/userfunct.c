#include<syscall_lib.h>
#include<os_printf.h>
#include<ipc.h>
void user_main1(){
	while(1){
		syscall_putchar('1');
	}
}
void user_main2(){
	while(1){
		syscall_putchar('1');
	}
}
void user_main_pingpong(){
      
	unsigned long me,other,i,who;
	i=0;
	me=syscall_getenvid();
	if(me==0x800){
		other=0x1001;
	
		printf("\n@@@@@send %d from %x to %x\n",i,me, other);
		ipc_send(other, i++, 0, 0);

	}
	else if(me==0x1001)
		other=0x800;
	for(;;){
		printf("%x am waiting.....\n", me);
		i = ipc_recv(&who, 0, 0);

		printf("%x got %d from %x\n", me, i, who);

		if (i == 10) {
			syscall_env_destroy(me);
		}

		i++;
		printf("\n@@@@@send %d from %x to %x\n",i,me, who);
		ipc_send(who, i, 0, 0);
		if (i == 10) {
			syscall_env_destroy(me);
		}
	}

}
