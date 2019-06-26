#include<syscall_lib.h>
#include<os_printf.h>
#include<ipc.h>
void user_main_fktest2(){
	int a=0;
	int id=0;

	if((id=fork())==0)
	{
		if ((id=fork())==0)
		{
			a+=3;
			for(;;) printf("\t\tthis is child2 :a:%d\n",a);
		}
		a+=2;
		for(;;) printf("\tthis is child :a:%d\n",a);
	}
	a++;
	for(;;) printf("this is father: a:%d\n",a);
}
void user_main_fktest(){
	
	unsigned long  r=0;
	r=fork();
	printf("%x===\n",r);
	if(r<0){
		panic("failed");
	}
	if(r==0){
		while(1){
			printf("i am son\n");
		}
	}
	else if(r>0){
		while(1){
			printf("i am father\n");
		}
	}
}
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
