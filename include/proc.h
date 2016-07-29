
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
                                                    Edward		2013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


typedef struct s_stackframe {	/* proc_ptr points here				↑ Low			*/
	t_32	gs;		/* ┓						│			*/
	t_32	fs;		/* ┃						│			*/
	t_32	es;		/* ┃						│			*/
	t_32	ds;		/* ┃						│			*/
	t_32	edi;		/* ┃						│			*/
	t_32	esi;		/* ┣ pushed by save()				│			*/
	t_32	ebp;		/* ┃						│			*/
	t_32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	t_32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	t_32	edx;		/* ┃						│			*/
	t_32	ecx;		/* ┃						│			*/
	t_32	eax;		/* ┛						│			*/
	t_32	retaddr;	/* return address for assembly code save()	│			*/
	t_32	eip;		/*  ┓						│			*/
	t_32	cs;		/*  ┃						│			*/
	t_32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	t_32	esp;		/*  ┃						│			*/
	t_32	ss;		/*  ┛						┷High			*/
}STACK_FRAME;

typedef struct s_msg {
	t_32	src_pid;//the source of the message
	t_32    des_pid;//the destination of the message
	int 	type;
	int 	status;
	char    msg[255];
}MSG;

typedef struct s_proc {
	STACK_FRAME			regs;			/* process' registers saved in stack frame */

	t_16				ldt_sel;		/* selector in gdt giving ldt base and limit*/
	DESCRIPTOR			ldts[LDT_SIZE];		/* local descriptors for code and data */
								/* 2 is LDT_SIZE - avoid include protect.h */
	t_32 			pde; 		/* 内存分页 */

	int				ticks;			/* remained ticks */
	int				priority;
	t_32			pid;			/* process id passed in from MM */
	char			name[16];		/* name of the process */
	int 			msgs_count;
	int				nr_tty;
	int 			nr_pde;
	int    			status;
	
}PROCESS;

typedef struct s_pde{
	t_32			addr;
	int 			i;
	int 			status;
	int 			type;
}PDE;

typedef struct s_task {
	t_pf_task	initial_eip;
	int			stacksize;
	char		name[32];
}TASK;

/* message status*/
#define ST_MSG_NORMAL	100
#define ST_MSG_NULL		0

/* process status*/
#define ST_PROC_NULL	0
#define ST_PROC_RUN		100
#define ST_PROC_STOP	200

/* pde status */
#define ST_PDE_NULL 	0
#define ST_PDE_USE		1

/* pde type */
#define TYPE_PDE_KERNEL 1
#define TYPE_PDE_PROC 	0

/* message type */
#define MSG_TYPE_SHELL 		0x10
#define MSG_TYPE_SH_READY 	0x12
#define MSG_TYPE_TTY_PRINT 	0x20
#define MSG_TYPE_TTY_CLEAR 	0x21
#define MSG_TYPE_TTY_READY 	0x22
#define MSG_TYPE_PROC_KILL 	0x31
#define MSG_TYPE_PROC_ARG	0x30
#define MSG_TYPE_PROC_READY 0x32
#define MSG_TYPE_PROC_RET 	0x33
/* PID of processes */
#define ANY_MSG_SRC 	0xFFFF
#define PID_TASK_TTY 		0
#define PID_SERVICE_SHELL 	1
#define PID_SERVICE_PROC 	2

/* table limit */
#define MAX_PROCS 	(NR_PROCS + NR_TASKS + 7)
#define MAX_PDES  	8
#define MAX_MSGS	256

/* Number of tasks & processes */
#define NR_TASKS		3
#define NR_PROCS		1

/* stacks of tasks */

#define STACK_SIZE_PROC		0x4000

#define STACK_SIZE_TOTAL	(STACK_SIZE_PROC * MAX_PROCS)

