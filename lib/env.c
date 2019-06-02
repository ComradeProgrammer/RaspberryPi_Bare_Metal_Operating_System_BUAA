#include<env.h>
#include<mmu.h>
struct Env *envs = NULL;		// All environments
struct Env *curenv = NULL;	        // the current env
