
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
                               进程管理，调度、创建
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
                                                    Edward		2013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
                              schedule
                              进程调度
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS*	p;
	int		greatest_ticks = 0;
	
	while (!greatest_ticks) {
		for (p=proc_table; p<proc_table+MAX_PROCS; p++) {
			if (p->status == ST_PROC_NULL){
				continue;
			}
			if (p->ticks > greatest_ticks) {
				greatest_ticks = p->ticks;
				p_proc_ready = p;

			}
		}
	
		if (!greatest_ticks) {
			for (p=proc_table; p<proc_table+MAX_PROCS; p++) {
				if (p->status == ST_PROC_NULL){
					continue;
				}
				p->ticks = p->priority;
			}
		}
	}
}
/*======================================================================*
                              proc_service
                              
 *======================================================================*/
PUBLIC void service_proc(){
	MSG msg;
	_recv(MSG_TYPE_TTY_READY);
	_send(PID_TASK_TTY,MSG_TYPE_PROC_READY);
	printf("X-Tinix: PROC(SERVICE) loaded\n");
	while(1){
		if(recv(ANY_MSG_SRC,&msg)!=-1){
			if(msg.type==MSG_TYPE_PROC_KILL){
				kill(msg.src_pid);
			}
		}
		memset(&msg,0,sizeof(MSG));
		delay(100);
	}

}

/*======================================================================*
                              mallocProc
                              获取空闲进程体
 *======================================================================*/
PUBLIC PROCESS* mallocProc(){
	PROCESS* p;
	for (p = proc_table; p < proc_table + MAX_PROCS; p ++ ){
		if (p->status != ST_PROC_RUN) {
			p->pid = p-proc_table;
			return p;
		}
	}
}
/*======================================================================*
                              mallocPDE
                              获取空闲PDE
 *======================================================================*/
PUBLIC PDE* mallocPDE(){
	PDE* p;
	for (p = pde_table; p < pde_table + MAX_PDES; p ++) {
		if (p->status != ST_PDE_USE) {
			p->status = ST_PDE_USE;
			return p;
		}
	}
}
/*======================================================================*
                              pre_excute
                              获取空闲进程体,准备填充内存
 *======================================================================*/
PUBLIC PROCESS* pre_execute(char* name,TTY* p_tty){
	
	PROCESS* 	p_proc 			= mallocProc();
	PDE* 		p_pde 			= mallocPDE();
	/* TODO 验证是否有空闲的PROC以及PDE*/
	int 		pid				= p_proc-proc_table;
	char* 		p_task_stack 	= task_stack + STACK_SIZE_TOTAL - ( pid * STACK_SIZE_PROC );
	t_16		selector_ldt	= SELECTOR_LDT_FIRST + pid*(1<<3);
	t_8			privilege 		= PRIVILEGE_USER;
	t_8			rpl				= RPL_USER;			
	int			eflags			= 0x3202; /* IOPL = 3 */

	strcpy(p_proc->name, name);	/* name of the process */
	p_proc->ldt_sel	= selector_ldt;
	memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
	p_proc->ldts[0].attr1 = DA_C | privilege << 5;	/* change the DPL */
	memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
	p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;/* change the DPL */
	p_proc->regs.cs		= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	p_proc->regs.ds		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	p_proc->regs.es		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	p_proc->regs.fs		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	p_proc->regs.ss		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	p_proc->regs.gs		= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
	p_proc->regs.eip	= 0x1000000;
	p_proc->regs.esp	= (t_32)p_task_stack;
	p_proc->regs.eflags	= eflags;
	p_proc->nr_tty		= p_tty - tty_table;
	p_proc->nr_pde 		= p_pde -> i;
	p_proc->ticks 		= p_proc->priority = 5;
	p_proc->pde 		= p_pde ->addr;
	p_proc->status  	= ST_PROC_NULL;
	return p_proc;
}

/*======================================================================*
                              excute
                              激活进程
 *======================================================================*/
PUBLIC PROCESS* execute(PROCESS *p){
	p->status = ST_PROC_RUN;
}

/*======================================================================*
                              kill
                              终止进程
 *======================================================================*/
PUBLIC int kill(int pid){
	if(pid<=2){
		return -1;
	}else{
		PROCESS* 	p_proc 	= proc_table + pid;
		PDE* 		p_pde 	= pde_table  + p_proc->nr_pde;
		if(p_proc->status == ST_PROC_RUN){
			p_proc -> status = ST_PROC_NULL;
			p_pde  -> status = ST_PDE_NULL;
			return pid;
		}else{
			return -2;
		}
		
	}	
}

/*======================================================================*
                           sys_send
                           向内核发送消息
                           send_message
 *======================================================================*/
PUBLIC int sys_send(t_32 pid, MSG* msg, PROCESS* p_proc){
	MSG* m;
	for(m=msg_table; m<msg_table+MAX_MSGS; m++){
		if(m->status!=ST_MSG_NORMAL){
			memset(m,0,sizeof(MSG));//clean work
			msg->des_pid = pid;
			msg->src_pid = p_proc->pid;
			msg->status  = ST_MSG_NORMAL;
			memcpy(m, msg, sizeof(MSG));
			nr_msgs++;
			return nr_msgs;
		}
	}
	return -1;
	
}
/*======================================================================*
                           sys_recv
                           从内核接收消息
                           recv_message
 *======================================================================*/
PUBLIC int sys_recv(t_32 pid, MSG* msg,PROCESS* p_proc){
	MSG* m;
	if(nr_msgs>0){
		for(m=msg_table; m<msg_table+MAX_MSGS; m++){
			if(m->status==ST_MSG_NORMAL && m->des_pid==p_proc->pid && (pid==ANY_MSG_SRC || m->src_pid==pid)){
				nr_msgs--;
				memcpy(msg,m,sizeof(MSG));
				m->status = ST_MSG_NULL;
				return nr_msgs;
			}
		}
		return -1;
	}else{
		return -1;
	}
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}
