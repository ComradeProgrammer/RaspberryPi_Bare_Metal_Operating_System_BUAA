#include<helpfunct.h>
#include<mmu.h>
#include<os_printf.h>
#include<env.h>
#include<syscall_lib.h>
int fork(){
    return syscall_env_alloc();
}