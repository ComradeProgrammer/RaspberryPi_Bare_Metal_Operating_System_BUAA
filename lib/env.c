#include<env.h>
#include<mmu.h>
#include<pmap.h>
#include<helpfunct.h>
#include<os_printf.h>
struct Env *envs = NULL;		// All environments
struct Env *curenv = NULL;	        // the current env.

static struct Env_list env_free_list;
struct Env_list env_sched_list[2];
int cur_sched=0;
extern int remaining_time;

unsigned long mkenvid(struct Env *e)
{
	static unsigned long next_env_id = 0;
    /*Hint: lower bits of envid hold e's position in the envs array. */
	unsigned long idx = e - envs;

    /*Hint:  high bits of envid hold an increasing number. */
	return (++next_env_id << (1 + LOG2NENV)) | idx;
}

int envid2env(unsigned long  envid, struct Env **penv, int checkperm)
{
    struct Env *e;
    /* Hint: If envid is zero, return the current environment.*/
    /*Step 1: Assign value to e using envid. */
	if(envid==0){
		*penv=curenv;
		return 0;
		
	}
	e = envs+ENVX(envid);

        if (e->env_status == ENV_FREE || e->env_id != envid) {
                *penv = 0;
                return -E_BAD_ENV;
        }
    /* Hint:
	Check that the calling environment has legitimate permissions to manipulate the specified environment.
	If checkperm is set, the specified environmentmust be either the current environment.or an immediate child of the current environment.If not, error! */
    /*Step 2: Make a check according to checkperm. */
	if(checkperm){
		if(e!=curenv&&e->env_parent_id!=curenv->env_id){
			*penv=0;
			return -E_BAD_ENV;
		}
	}
        *penv = e;
        return 0;
}
void env_init(void)
{
	int i;
	struct Env_list a,b; 
    /*Step 1: Initial env_free_list. */
	LIST_INIT(&env_free_list);

    /*Step 2: Travel the elements in 'envs', init every element(mainly initial its status, mark it as free)
     * and inserts them into the env_free_list as reverse order. */
	for(i=NENV-1;i>=0;i--){
		(envs[i]).env_status=ENV_FREE;
		LIST_INSERT_HEAD(&env_free_list,&(envs[i]),env_link);
	}
	LIST_INIT(&(a));
	LIST_INIT(&(b));

	env_sched_list[0]=a;
	env_sched_list[1]=b;
	cur_sched=0;
	remaining_time=0;

}

static int env_setup_vm(struct Env *e)
{

	int i, r;
	struct Page *p = NULL;
	unsigned long *pgdir;

    /*Step 1: Allocate a page for the page directory and add its reference.
     *pgdir is the page directory of Env e. */
	if ((r = page_alloc(&p)) < 0) {
		panic("env_setup_vm - page_alloc error\n");
		return r;
	}
	p->pp_ref++;
	pgdir = (unsigned long *)page2pa(p);
	/*copy the kernel to the lower address space*/
	for (i = 0; i < 512; i++) {
		pgdir[i] = ((unsigned long*)_pg_dir)[i];
	}

	e->env_pgdir = pgdir;
	return 0;
}
int env_alloc(struct Env **new, unsigned long parent_id)
{
	//int r;
	struct Env *e;
    //struct Page* pp;
    /*Step 1: Get a new Env from env_free_list*/
	if((e=LIST_FIRST(&env_free_list))==NULL){
		printf("Sorry,alloc env failed!\n");
		return -E_NO_FREE_ENV;
	}
	env_setup_vm(e);

    /*Step 3: Initialize every field of new Env with appropriate values*/
	e->env_parent_id = parent_id;
	e->env_status = ENV_RUNNABLE;
	e->env_runs = 0;
	e->env_id = mkenvid(e);

	e->env_tf.sp = USTACKTOP;
	
	*new = e;
	LIST_REMOVE(e,env_link);
	return 0;
}

 void env_create_priority(unsigned char *binary, int size, int priority)
{
    struct Env *e;
	int r;
    /*Step 1: Use env_alloc to alloc a new env. */
	r=env_alloc(&e,0);
	if(r<0){
		panic("fuck,env_create_priority:env_alloc failed");
		return;
	}
	/*stack*/
	struct Page *p = NULL;
	r=page_alloc(&p);
	p->pp_ref++;
	if(r<0){
		printf("env_create:page_alloc failed\n");
		return;
	}	
	r=page_insert(e->env_pgdir,p,USTACKTOP-BY2PG,PTE_V);
    /*Step 2: assign priority to the new env. */
	e->env_pri=priority;
    /*Step 3: Use load_icode() to load the named elf binary. */
	//load_icode(e,binary,size);//----------------
	e->env_tf.pc=(unsigned long)binary;
	e->env_tf.sp=USTACKTOP;

	LIST_INSERT_HEAD(&(env_sched_list[cur_sched]),e,env_sched_link);
}
/* Overview:
 * Allocates a new env with default priority value.
 * 
 * Hints:
 *  this function warp the env_create_priority function/
 */

void env_create(unsigned char *binary, int size)
{
	env_create_priority(binary,size,1);
}


 
void env_free(struct Env* e){
	unsigned long pudno,pmdno,pteno;
	unsigned long* pud_entry,*pmd_entry;
	printf("[%08x] free env %08x\n", curenv ? curenv->env_id : 0, e->env_id);
	for(pudno=PUDX(0xc0000000);pudno<512;pudno++){
		if(!((e->env_pgdir[pudno])&PTE_V)){
			continue;
		}
		pud_entry=(unsigned long* )PTE_ADDR(e->env_pgdir[pudno]);
		for(pmdno=0;pmdno<512;pmdno++){
			if(!((pud_entry[pmdno])&PTE_V)){
				continue;
			}
			pmd_entry=(unsigned long*)PTE_ADDR(pud_entry[pmdno]);
			for(pteno=0;pteno<512;pteno++){
				if(!((pmd_entry[pteno])&PTE_V)){
					page_remove(e->env_pgdir,(pudno<<30)+(pmdno<<21)+(pteno<<12));
				}
			}
			page_remove(e->env_pgdir,(unsigned long)pmd_entry);
		}
		page_remove(e->env_pgdir,(unsigned long)pud_entry);

	}
	e->env_pgdir=NULL;
	e->env_status=ENV_FREE;
	LIST_INSERT_HEAD(&env_free_list, e, env_link);
	LIST_REMOVE(e, env_sched_link);
}
extern void sched_yield();
void env_destroy(struct Env* e){
	env_free(e);
	if(curenv==e){
		curenv=NULL;
		bcopy((void *)KERNEL_SP - sizeof(struct Trapframe),
			  (void *)TIMESTACK - sizeof(struct Trapframe),
			  sizeof(struct Trapframe));
		printf("i am killed ... \n");
		remaining_time=0;
		sched_yield();
	}
}
void env_run(struct Env *e)
{
	extern void tlb_invalidate();
	struct Trapframe *old=(struct Trapframe*)(TIMESTACK-sizeof(struct Trapframe));
	/*Step 1: save register state of curenv. */
    /* Hint: if there is a environment running,you should do
    *  context switch.You can imitate env_destroy() 's behaviors.*/
	if(curenv!=NULL){
		bcopy((void*)old,(void*)(&(curenv->env_tf)),sizeof(struct Trapframe));
		curenv->env_tf.pc=old->elr_el1;
	}

    /*Step 2: Set 'curenv' to the new environment. */
	curenv=e;
	curenv->env_runs++;
    
    /*Step 4: Use env_pop_tf() to restore the environment's
     * environment   registers and drop into user mode in the
     * the   environment.
     */
	tlb_invalidate();//essential
	env_pop_tf(&(curenv->env_tf),curenv->env_pgdir);//---------

}

void env_check()
{
        struct Env *temp, *pe, *pe0, *pe1, *pe2;
        struct Env_list fl;
        int re = 0;
 	// should be able to allocate three envs
	pe0 = 0;
        pe1 = 0;
        pe2 = 0;
        assert(env_alloc(&pe0, 0) == 0);
        assert(env_alloc(&pe1, 0) == 0);
        assert(env_alloc(&pe2, 0) == 0);

        assert(pe0);
        assert(pe1 && pe1 != pe0);
        assert(pe2 && pe2 != pe1 && pe2 != pe0);

 	// temporarily steal the rest of the free envs
 	fl = env_free_list;
	// now this env_free list must be empty!!!!
	LIST_INIT(&env_free_list);

	// should be no free memory
	 assert(env_alloc(&pe, 0) == -E_NO_FREE_ENV);

	// recover env_free_list
	env_free_list = fl;

        printf("pe0->env_id %d\n",pe0->env_id);
        printf("pe1->env_id %d\n",pe1->env_id);
        printf("pe2->env_id %d\n",pe2->env_id);

        assert(pe0->env_id == 2048);
        assert(pe1->env_id == 4097);
        assert(pe2->env_id == 6146);
        printf("env_init() work well!\n");

	 /* check envid2env work well */
	 pe2->env_status = ENV_FREE;
        re = envid2env(pe2->env_id, &pe, 0);

        assert(pe == 0 && re == -E_BAD_ENV);

        pe2->env_status = ENV_RUNNABLE;
        re = envid2env(pe2->env_id, &pe, 0);

        assert(pe->env_id == pe2->env_id &&re == 0);

        temp = curenv;
        curenv = pe0;
        re = envid2env(pe2->env_id, &pe, 1);
        assert(pe == 0 && re == -E_BAD_ENV);
        curenv = temp;
        printf("envid2env() work well!\n");

	/* check env_setup_vm() work well */
        printf("env_check() succeeded!\n");
}
/* 
static int load_icode_mapper(unsigned long va, unsigned int sgsize,
							 unsigned char *bin, unsigned int bin_size, void *user_data)
{
	struct Env *env = (struct Env *)user_data;
	struct Page *p = NULL;
	unsigned long kva=0;
	unsigned long range=0;
	unsigned long i;
	int r;
	unsigned long offset = va - ROUNDDOWN(va, BY2PG);
	//printf("load_icode_mapper:binsize:%x,sgsize:%x,va:%x\n",bin_size,sgsize,va);
	//Step 1: load all content of bin into memory. 
	for (i = 0; i < bin_size;) {
	// Hint: You should alloc a page and increase the reference count of it. 
		r=page_alloc(&p);
	//	p->pp_ref++;
		if(r<0){
			printf("load_icode_mapper:page_alloc failed\n");
			return -E_NO_MEM;
		}
		kva=page2pa(p);
		if(i==0&&offset!=0){
			range=(offset+bin_size>=BY2PG)?BY2PG-offset:bin_size;
				bcopy((void*)(bin),kva+offset,range);
		//	printf("from,to,range%x,%x,%x\n",bin,kva+offset,range);
			i+=BY2PG-offset;
		}
		else{
			range=(i+BY2PG<bin_size)?BY2PG:bin_size-i;
				bcopy((void*)(bin+i),kva,range);
		//	printf("from,to,range%x,%x,%x\n",bin+i,kva,range);

			i+=BY2PG;
		}
		(p->pp_ref)++;
		r=page_insert(env->env_pgdir,p,va+i-BY2PG,PTE_V);
		if(r<0){
			printf("load_icode_mapper:page_insert failed\n");
			return -E_NO_MEM;
		}

	}
	//Step 2: alloc pages to reach `sgsize` when `bin_size` < `sgsize`. * i has the value of `bin_size` now. 
	while (i < sgsize) {
		r=page_alloc(&p);
		p->pp_ref++;
		if(r<0){
			printf("load_icode_mapper:page_alloc failedi\n");
			return -E_NO_MEM;
		}
		r=page_insert(env->env_pgdir,p,va+i,PTE_V);
		if(r<0){
			printf("load_icode_mapper:page_insert failed\n");
			return -E_NO_MEM;
		}
		i+=BY2PG;
	}
	return 0;
}

static void
load_icode(struct Env *e, unsigned char *binary, unsigned int size)
{
	// Hint:
	 //  You must figure out which permissions you'll need
	 //  for the different mappings you create.
	 //  Remember that the binary image is an a.out format image,
	 //  which contains both text and data.
	struct Page *p = NULL;
	unsigned long entry_point;
	unsigned long r;
        unsigned long perm;
     //Step 1: alloc a page. 
	r=page_alloc(&p);
	p->pp_ref++;
	if(r<0){
		printf("load_icode:page_alloc failed\n");
		return;
	}	
	perm=PTE_V;
    //Step 2: Use appropriate perm to set initial stack for new Env. 
    //Hint: The user-stack should be writable? 
	r=page_insert(e->env_pgdir,p,USTACKTOP-BY2PG,perm);
	if(r<0){
		printf("fuck,load_icode:page_insert failed\n");
		return;
	}
    //Step 3:load the binary by using elf loader. 
//	r=load_elf(binary,size,&entry_point,e,load_icode_mapper);//--------------
	if(r<0){
		printf("fuck,load_icode: load_elf failed\n");
		return;	
	}
    //Your Question Here
    //Step 4:Set CPU's PC register as appropriate value. 
	e->env_tf.pc = entry_point;
	return;
 }*/