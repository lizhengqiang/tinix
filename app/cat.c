

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
	t_8 	buf[16*1024];
	char 	tmp[9];
	char  	arg[256];
	char 	args[3];
	char 	name[9];
	char 	ext[4];
	int		len ; 
	int 	i   ;
	delay(1000);
	_recv_arg(arg);
	explode(arg,args[0],args[1],args[2]);

	printf("\nRead File:%s\n",args[1]);
	upper_name(args[1],strlen(args[1]));
	len = read_file(args[1],"TXT",buf);
	printf("File %s is a %dKB file\n",args[1],len/1024);

	//printf("\nsize is %dKB \n",len/1024);
	for(i=0;i<len;i+=8){
		memcpy(tmp,&buf[i],8);
		tmp[9]=0;
		printf("%s",tmp);
	}
	printf("\n");
	_send_ret();
	_send_kill_me();
	while(1){
		delay(1000);
	}
}
