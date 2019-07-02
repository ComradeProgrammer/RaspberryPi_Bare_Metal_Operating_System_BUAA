# raspi bare metal operating system
北京航空航天大学操作系统课程设计实验 树莓派挑战性任务

将在课程中完成的MIPS小操作系统移植到树莓派3b中

使用qemu模拟器进行仿真

由于时间受限，该任务仅完成了4/6，在aarch64体系结构下实现了boot,基于三级页表的页式内存管理，基于时钟中断的进程管理，系统调用和一个fork函数

但是通过了挑战性任务验收答辩

Repository of challenge task of OS Experiment of SCSE of BUAA(Summer 2019)

Challenge Task: transplant the mini MIPS Operating System to the Raspberry Pi 3b with the help of simulator qemu

Due to the extremely limited time, only 4/6 of the challenge task is finished.

This project contains a bare metal aarch64 operating system ,on which virtual page memory managment, process scheduling, system calls and a fork is implemented

This project passed the final acceptance check of this course
