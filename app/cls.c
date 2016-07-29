
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               recv.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

void _start(){
	MSG msg;
	SHELL_MSG shell_msg;
	char  	arg[256];
	delay(100);
	_recv_arg(arg);
	printf("\nHello World\n");
	msg.type=MSG_TYPE_SHELL;
	shell_msg.tty=0;
	memcpy(shell_msg.command,"clear",6);
	memcpy(msg.msg,&shell_msg,sizeof(SHELL_MSG));
	send(PID_SERVICE_SHELL,&msg);

	
	_send_kill_me();
	while(1){
		
		delay(1000);
	}
}
