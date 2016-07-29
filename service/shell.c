
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              shell.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Edward	2013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	回车键:	把光标移到第一列
	换行键:	把光标前进到下一行
*/


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
#include "shell.h"
#include "klib.h"


PRIVATE void jobs(int tty);
PRIVATE void welcome(int tty);
PRIVATE void tty_clear(int tty);
PRIVATE void ret_shell(int tty);

/*======================================================================*
                           service_shell
 *======================================================================*/
PUBLIC void service_shell()
{
	TTY *p_tty;
	MSG msg;
	int tty;
	int i, r;
	
	//等待TTY启动
	while(1){
		if(recv(ANY_MSG_SRC,&msg)!=-1){
			if(msg.type==MSG_TYPE_TTY_READY){
				break;
			}
		}
	}
	
	//发送欢迎信息
	printf("X-Tinix: SHELL(SERVICE) loaded.\n");
	
	for(i=0;i<NR_CONSOLES;i++) {
		welcome(i);
	}
	//监听SHELL
	while(1){
		if(recv(ANY_MSG_SRC,&msg)!=-1){
			if(msg.type==MSG_TYPE_SHELL){
				SHELL_MSG shell_msg;
				char *str;
				char cmd[64];
				char arg[8][64];
				int  argv;
				
				memcpy(&shell_msg,msg.msg,sizeof(SHELL_MSG));
				memset(cmd, 0x0, 64);
				memset(arg, 0x0, 8 * 64);
				str  = shell_msg.command ;
				argv = explode(shell_msg.command,cmd,arg[0],arg[1],arg[2],arg[3],arg[4]);
				tty  = shell_msg.tty;

				if(strcmp(cmd,"jobs")==0){
					jobs(tty);
				}else if(strcmp(cmd,"ls")==0){
					ls(tty);
				}else if(strcmp(cmd,"clear")==0){
					tty_clear(tty);
					welcome(tty);
					continue;
				}else if(strcmp(cmd,"load")==0){
					if(strcmp(arg[1],"show")==0){
						r = load_file(tty,arg[0],str,1);
					}else{
						r = load_file(tty,arg[0],str,0);
					}
					if( r == 1){
						continue;
					}
				}else if(strcmp(cmd,"kill")==0){
					r = kill(atoi(arg[0]));
					if(r>=0){
						tty_printf(tty,"PID:%x has been killed.\n",r);
					}else{
						tty_printf(tty,"can't kill the process ErrCode:%x.\n",r);
					}
				}else{
					r = load_file(tty,cmd,str,-1);
					if( r == 1){
						continue;
					}
				}
				ret_shell(tty);	
			}else if(msg.type==MSG_TYPE_PROC_RET){
				tty = ( proc_table + msg.src_pid ) -> nr_tty;
				ret_shell(tty);
			}	
		}

	}
}
/*======================================================================*
                           ret_shell
 *======================================================================*/
PRIVATE void ret_shell(int tty){
	tty_printf(tty,".%x$:",tty);
}

/*======================================================================*
                           welcome
 *======================================================================*/
PRIVATE void welcome(int tty){
	tty_printf(tty,"Welcome to X-Tinix TTY:%x\n.%x$:",tty,tty);
}
/*======================================================================*
                           jobs
 *======================================================================*/
PRIVATE void jobs(int tty){
	PROCESS* p_proc = proc_table;
	int len = 0;
	for(; p_proc < proc_table + MAX_PROCS; p_proc ++){
		if(p_proc -> status == ST_PROC_RUN ){
			tty_printf(tty,"PID:%x \tNAME:%s \tPDE:%x \n",p_proc->pid,p_proc->name,p_proc->nr_pde);
			len ++;
		}
	}
	tty_printf(tty, "%d processes is active\n",len);
	return ;

}
/*======================================================================*
                           ls
 *======================================================================*/
PUBLIC int ls(int tty){
	fat_entry 	entries[256];
	fat_entry 	*p = entries;
	int 		count = 0;
	read_sectors(SECTOR_NO_OF_ROOT_DIR, ROOT_DIR_SECTORS, (char*)entries);
	while(p->attr != 0){
		if(p->attr==ATTR_FILE_NORMAL && p->name[0]>='A' && p->name[0]<='Z'){
			char name[9];
			char ext[4];
			read_name(p->name,name,ext);
			tty_printf(tty, "%s.%s  \tattr:%c\tclus:%x\tsize:%dKB\n",name,ext,p->attr,p->fst_clus,p->file_size/1024);
			count ++;
		}
		p ++;
	}
	tty_printf(tty, "Root dir has %d files.\n",count);
}
/*======================================================================*
                           load_file
 *======================================================================*/
PUBLIC int load_file(int tty,char *name,char* str,int mode){
	char* 		buff;
	int 		i;
	int 		len ;
	PROCESS* 	proc;
	TTY* 		p_tty = tty_table;
	Elf32_Ehdr 	hdr	;
	Elf32_Phdr 	phdr;
	
	buff = (char*)0x100000;
	upper_name( name, 8 );
	len = read_file(name,"BIN",buff);

	if(len>0){
		memcpy(&hdr,buff,sizeof(Elf32_Ehdr));
		if(mode==0){
			tty_printf(tty,"(SHELL)found file:%s.bin(%dKB).\n",name,len/1024);
		}
		for(i=0;i<hdr.e_phnum;i++){
			memcpy(&phdr,&buff[hdr.e_phoff+i*hdr.e_phentsize],sizeof(Elf32_Phdr));
			if(mode==1){
				tty_printf(tty,"(SHELL)PT:%x off:%x size:%x\n",phdr.p_type,phdr.p_offset,phdr.p_filesz);
			}else{
				if(phdr.p_type==0x1){
					//执行新进程
					p_tty += tty;
					proc = pre_execute(name,p_tty);
					char *desc=(char*)(proc -> nr_pde * 0x400000+ 0x1000000);
					memcpy(desc,&buff[phdr.p_offset],phdr.p_filesz);
					execute(proc);
					//发送arg
					_send_str(proc->pid,MSG_TYPE_PROC_ARG,str);
					if(mode==0){
						tty_printf(tty,"(SHELL)process %s loaded. PID:%x PDE:%x \n",name,proc->pid,proc->nr_pde);
					}
					return 1;
				}
			}
		}
	}else{
		if(mode==0 || mode ==1){
			tty_printf(tty,"(SHELL)can't find file:%s.bin\n",name);
		}else if(mode == -1){
			tty_printf(tty,"undefined command.\n");
		}
	}
	return 0;

}
/*======================================================================*
                           tty_printf
 *======================================================================*/
PUBLIC int tty_printf(int tty,const char *fmt, ...)
{
	int i;
	char buf[256];
	MSG msg;
	SHELL_MSG shell_msg;

	memset(buf,0x0,256);
	memset(&msg,0x0,sizeof(MSG));
	memset(&shell_msg,0x0,sizeof(SHELL_MSG));

	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */

	i = vsprintf(buf, fmt, arg);

	msg.type=MSG_TYPE_SHELL;
	shell_msg.tty=tty;
	memcpy(shell_msg.command,buf,i+1);
	memcpy(msg.msg,&shell_msg,sizeof(SHELL_MSG));
	send(PID_TASK_TTY,&msg);

	return i;
}
/*======================================================================*
                           tty_clear
 *======================================================================*/
PRIVATE void tty_clear(int tty){

	MSG msg;
	
	SHELL_MSG shell_msg;

	memset(&msg,0,sizeof(MSG));
	memset(&shell_msg,0,sizeof(SHELL_MSG));

	msg.type=MSG_TYPE_TTY_CLEAR;
	shell_msg.tty=tty;

	memcpy(msg.msg,&shell_msg,sizeof(SHELL_MSG));

	send(PID_TASK_TTY,&msg);

	return ;
}




