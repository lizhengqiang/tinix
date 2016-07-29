
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               tty.c
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
#include "keyboard.h"
#include "proto.h"


#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)


/* 本文件内函数声明 */
PRIVATE void	init_tty(TTY* p_tty);
PRIVATE void	tty_do_read(TTY* p_tty);
PRIVATE void	tty_do_write(TTY* p_tty);
PRIVATE void	put_key(TTY* p_tty, t_32 key);


/*======================================================================*
                           task_tty
 *======================================================================*/
PUBLIC void task_tty()
{
	TTY*	p_tty;
	MSG 	msg;

	init_keyboard();

	for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		init_tty(p_tty);
	}

	select_console(0);

	//清屏
	clear(TTY_FIRST);

	//欢迎信息
	printf("X-Tinix.\n");
	printf("X-Tinix: TTY(TASK) loaded.\n");
	
	//就绪消息
	_send(PID_SERVICE_PROC ,MSG_TYPE_TTY_READY);
	_recv(MSG_TYPE_PROC_READY);
	_send(PID_SERVICE_SHELL,MSG_TYPE_TTY_READY);

	//监听消息
	while (1) {

		if(recv(ANY_MSG_SRC,&msg)!=-1){
			SHELL_MSG shell_msg;
			memcpy(&shell_msg,msg.msg,sizeof(SHELL_MSG));
			switch(msg.type){
				case MSG_TYPE_SHELL:
					tty_write(TTY_FIRST+shell_msg.tty,shell_msg.command,strlen(shell_msg.command));
					break;
				case MSG_TYPE_TTY_CLEAR:
					p_tty = TTY_FIRST + shell_msg.tty;
					clear(p_tty);
					break;
				default:
					break;
			}
		}
		for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
			tty_do_read(p_tty);
			//tty_do_write(p_tty);
		}
		//clean work
		memset(&msg,0x0,sizeof(MSG));
	}
}

/*======================================================================*
                           init_tty
 *======================================================================*/
PRIVATE void init_tty(TTY* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

	init_screen(p_tty);
}

/*======================================================================*
                           handle command
 *======================================================================*/
PUBLIC void handle_command(TTY* p_tty){
	
	MSG msg;
	SHELL_MSG shell_msg;
	char command[TTY_IN_BYTES];
	char *p=command;
	int len=p_tty->inbuf_count-1;
	int r;
	
	out_char(p_tty->p_console,'\n');

	while (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;
		*p=ch;
		p++;
	}
	p--;//消除换行符
	*p=0;
	msg.type=MSG_TYPE_SHELL;
	shell_msg.tty=p_tty-TTY_FIRST;
	memcpy(&shell_msg.command,command,len+1);
	memcpy(&msg.msg,&shell_msg,sizeof(SHELL_MSG));
	send(PID_SERVICE_SHELL,&msg);
}

/*======================================================================*
                           in_process
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, t_32 key)
{
	if (!(key & FLAG_EXT)) {
		put_key(p_tty, key);
		out_char(p_tty->p_console, key);
	}
	else {
		int raw_code = key & MASK_RAW;
		switch(raw_code) {
		case ENTER:

			put_key(p_tty, '\n');
			handle_command(p_tty);
			break;
		case BACKSPACE:
			if(p_tty->inbuf_count>0){
				out_char(p_tty->p_console, '\b');
			}
			put_key(p_tty, '\b');
			
			break;
		case UP:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {	/* Shift + Up */
				scroll_screen(p_tty->p_console, SCROLL_SCREEN_UP);
			}
			break;
		case DOWN:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {	/* Shift + Down */
				scroll_screen(p_tty->p_console, SCROLL_SCREEN_DOWN);
			}
			break;
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
		case F12:
			if (1) {	/* Alt + F1~F12 */
				select_console(raw_code - F1);
			}
			break;
		default:
			break;
		}
	}
}


/*======================================================================*
                              put_key
*======================================================================*/
PRIVATE void put_key(TTY* p_tty, t_32 key)
{
	if ( key == '\b'){
		if(p_tty -> inbuf_count == 0){
			return ;
		}else{
			p_tty->inbuf_count--;
			*(p_tty->p_inbuf_head)=0;
			p_tty->p_inbuf_head--;
			return ;
		}
	}
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key;
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}


/*======================================================================*
                              tty_do_read
*======================================================================*/
PRIVATE void tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		keyboard_read(p_tty);
	}
}


/*======================================================================*
                              tty_do_write
*======================================================================*/
PRIVATE void tty_do_write(TTY* p_tty)
{
	if (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;

		//disp_int(ch);
		out_char(p_tty->p_console, ch);
	}
}


/*======================================================================*
                              tty_write
*======================================================================*/
PUBLIC void tty_write(TTY* p_tty, char* buf, int len)
{
	char* p = buf;
	int i = len;

	while (i) {
		out_char(p_tty->p_console, *p++);
		i--;
	}
}

/*======================================================================*
                              sys_write
*======================================================================*/
PUBLIC int sys_write(char* buf, int len, PROCESS* p_proc)
{
	tty_write(&tty_table[p_proc->nr_tty], buf, len);
	return 0;
}


