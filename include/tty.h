
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                tty.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
                                                    Edward		2013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_TINIX_TTY_H_
#define	_TINIX_TTY_H_


#define TTY_IN_BYTES	256	/* tty input queue size */

struct s_tty;
struct s_console;

/* TTY */
typedef struct s_tty
{
	t_32	in_buf[TTY_IN_BYTES];	/* TTY 输入缓冲区 */
	t_32*	p_inbuf_head;		/* 指向缓冲区中下一个空闲位置 */
	t_32*	p_inbuf_tail;		/* 指向键盘任务应处理的键值 */
	int	inbuf_count;		/* 缓冲区中已经填充了多少 */

	struct s_console *	p_console;
	
}TTY;

/* TTY message*/
typedef struct s_shell_msg
{
	int 	tty;
	char	command[64];
}SHELL_MSG;



#endif /* _TINIX_TTY_H_ */
