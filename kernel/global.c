
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
                                                    Edward 		2013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"                                           
#include "global.h"
#include "proto.h"


PUBLIC  t_32	nr_msgs = 0;

PUBLIC 	int		nr_procs = NR_PROCS;

PUBLIC	PROCESS	proc_table[MAX_PROCS];

PUBLIC 	PDE 	pde_table[MAX_PDES];

PUBLIC	MSG		msg_table[MAX_MSGS];

PUBLIC	TASK	task_table[NR_TASKS] = {{task_tty, STACK_SIZE_PROC, "tty"},
										{service_shell, STACK_SIZE_PROC, "shell"},
										{service_proc, 	STACK_SIZE_PROC, "proc"}};
PUBLIC	TASK	user_proc_table[NR_PROCS] = {	
						{TestA, STACK_SIZE_PROC, "TestA"}};

PUBLIC	char	task_stack[STACK_SIZE_TOTAL];

PUBLIC	TTY			tty_table[NR_CONSOLES];

PUBLIC	CONSOLE			console_table[NR_CONSOLES];

PUBLIC	t_pf_irq_handler	irq_table[NR_IRQ];

PUBLIC	t_sys_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_write, sys_send, sys_recv};


