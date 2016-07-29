

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               app.c
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

	char tmp[10];
	char  	arg[256];
	_recv_arg(arg);
	
	while(1){
		time(tmp);
		int year 	= 2000 + ( tmp[5] >> 4 ) * 10 + ( tmp[5] & 0x0F );
		int month 	= ( tmp[4] >> 4 ) * 10 + ( tmp[4] & 0x0F );
		int day 	= ( tmp[3] >> 4 ) * 10 + ( tmp[3] & 0x0F );
		int h	 	= ( tmp[2] >> 4 ) * 10 + ( tmp[2] & 0x0F );
		int min 	= ( tmp[1] >> 4 ) * 10 + ( tmp[1] & 0x0F );
		int sec 	= ( tmp[0] >> 4 ) * 10 + ( tmp[0] & 0x0F );
		printf("TIME: %d-%d-%d %d:%d:%d\n",year,month,day,h,min,sec);
		delay(1000);
	}
	
	//_send_ret();
	_send_kill_me();
	

}

