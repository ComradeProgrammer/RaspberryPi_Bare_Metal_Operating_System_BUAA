#ifndef _IPC_H_
#define _IPC_H_
void ipc_send(unsigned long whom, unsigned long val, unsigned long srcva, unsigned long perm);
unsigned long ipc_recv(unsigned long *whom, unsigned long dstva, unsigned long *perm);
#endif