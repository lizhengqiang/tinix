
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            klib.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
                                                 Appending by Edward
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
#include "klib.h"



/*======================================================================*
                               is_alphanumeric
 *======================================================================*/
PUBLIC t_bool is_alphanumeric(char ch)
{
	return ((ch >= ' ') && (ch <= '~'));
}
/*======================================================================*
                               atoi
 *======================================================================*/
PUBLIC int atoi(char * str){
	int 	r = 0;
	int 	len = strlen(str);
	int		b = 1;
	char *	p;
	for(p=&str[len-1];p>=str;p--){
		r+=((*p-'0')*b);
		b*=10;
	}
	return r;
}

PUBLIC char * asc(char * str, char c){
	char *	p = str;
	char	ch;
	int	i;
	t_bool	flag = FALSE;

	*p++ = '0';
	*p++ = 'x';

	if(c == 0){
		*p++ = '0';
	}
	else{	
		for(i=4;i>=0;i-=4){
			ch = (c >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = TRUE;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}
/*======================================================================*
                               itoa
 *======================================================================*/
PUBLIC char* _itoa(char* str, int num){
	char* 	p 	= str;
	int 	tmp = num;
	int 	len = 1 ;
	int 	b   = 1 ;
	//计算长度
	while(tmp > 10){
		b 	*= 10;
		tmp /= 10;
	}
	
	while(b > 0 ){
		*p = ((num - (num / (b*10) * (b*10))) / b) + '0';
		p ++;
		b /= 10;
	}
	*p='\0';

	return str;
}
/*======================================================================*
                               itoa
 *======================================================================*/
PUBLIC char * itoa(char * str, int num)
{
	char *	p = str;
	char	ch;
	int	i;
	t_bool	flag = FALSE;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = TRUE;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}



/*======================================================================*
                               delay
 *======================================================================*/
PUBLIC void delay(int time)
{
	int i, j, k;
	for(k=0;k<time;k++){
		/*for(i=0;i<10000;i++){	for Virtual PC	*/
		for(i=0;i<10;i++){/*	for Bochs	*/
			for(j=0;j<10000;j++){}
		}
	}
}

/*======================================================================*
                               simple_send
 *======================================================================*/

 PUBLIC int _send(int pid, int msg){
 	MSG m;
 	memset(&m,0,sizeof(MSG));
 	m.type = msg;
 	return send(pid,&m);
 }

 /*======================================================================*
                               simple_send_str
 *======================================================================*/

 PUBLIC int _send_str(int pid, int type, char *msg){
 	MSG m;
 	memset(&m,0,sizeof(MSG));
 	m.type = type;
 	strcpy(m.msg,msg);
 	return send(pid,&m);
 }
 /*======================================================================*
                               simple_recv(block)
 *======================================================================*/
PUBLIC int _recv(int msg){
	MSG m;
	int r;
	while(m.type!=msg){
		while((r=recv(ANY_MSG_SRC,&msg))==-1){
			delay(100);
		}
	}
	return r;
}

 /*======================================================================*
                               simple_recv_arg(block)
 *======================================================================*/
PUBLIC char* _recv_arg(char *arg){
	MSG m;
	int r;

	while(m.type!=MSG_TYPE_PROC_ARG){
		while((r=recv(ANY_MSG_SRC,&m))==-1){
			delay(100);
		}
	}
	
	return strcpy(arg,m.msg);
}                               
/*======================================================================*
                               simple_send_kill
 *======================================================================*/
PUBLIC int _send_kill_me(){
	_send(PID_SERVICE_PROC,MSG_TYPE_PROC_KILL);
}
/*======================================================================*
                               simple_send_return
 *======================================================================*/
PUBLIC int _send_ret(){
	_send(PID_SERVICE_SHELL,MSG_TYPE_PROC_RET);
}
/*======================================================================*
                               fdc
 *======================================================================*/
PUBLIC void fdc(){
	int i;
	out_byte(FLOPPY_REG_DOR, 0x08);//重启
	for(i=0 ;i <100 ;i++){

	}
	out_byte(FLOPPY_REG_DOR,0x1c);//选择DMA模式，选择A
	out_byte(FD_DCR,0);//设置速度
	output_byte(FD_SPECIFY);
	output_byte(0x0cf);//马达速度 卸载时间32ms
	output_byte(6);//磁头加载时间6ms DMA
}
/*======================================================================*
                               output_byte
 *======================================================================*/
PUBLIC void output_byte(char byte){
	int counter;
	unsigned char status;
	for(counter = 0 ; counter < 10000 ; counter++){
		status = in_byte(FD_STATUS) & (STATUS_READY | STATUS_DIR );
		if( status==STATUS_READY ){
			out_byte(FD_DATA, byte);
			return ;
		}
	}
	printf("Unable to send byte toFDC\n");	
}
/*======================================================================*
                               FloppyReadSector
 *======================================================================*/
PUBLIC void FloppyReadSector(t_32 sectNo, t_8 *buf){
	floppy_strucy floppy_type={2800,18,2,80,0,0x1B,0x00,0xCF};//1.44MB
	t_32 current_dev = 0;//Disk ID
	t_32 head, track, block, sector, seek_track;
	if(NULL == buf){
		printf("\nFloppyReadSector para error\n");
		return ;
	}
	if(sectNo >= (floppy_type.head * floppy_type.track * floppy_type.sect)){
		printf("\nFloppyReadSector sectNo error %d %d %d %d \n",sectNo,floppy_type.head , floppy_type.track , floppy_type.sect);
		return ;
	}

	sector = sectNo % floppy_type.sect + 1;
	block = sectNo / floppy_type.sect;
    track = block / floppy_type.head;
    head = block % floppy_type.head;
    seek_track = track << floppy_type.stretch;

    /* 软盘重新校正 */

    output_byte(FD_RECALIBRATE);
    output_byte(current_dev);

    /* 寻找磁道 */
    output_byte(FD_SEEK);
    output_byte(current_dev);
    output_byte(seek_track);

     /* 设置DMA，准备传送数据 */
    SetDMA(buf, FD_READ); 

    /* 发送读扇区命令 */
    output_byte(FD_READ); /* command */
    output_byte(current_dev); /* driver no. */
    output_byte(track); /* track no. */
    output_byte(head); /* head */
    output_byte(sector); /* start sector */
    output_byte(2); /* sector size = 512 */
    output_byte(floppy_type.sect); /* Max sector */
    output_byte(floppy_type.gap); /* sector gap */
    output_byte(0xFF); /* sector size (0xff when n!=0 ?) */
    return;
}

/*======================================================================*
                               SetDMA
 *======================================================================*/
PUBLIC void SetDMA(t_8 *buf, t_8 cmd){
	
    long addr = (long)buf;
    disable_int();

    /* mask DMA 2 */
    immoutb_p(4|2,10);
    /* output command byte. I don't know why, but everyone (minix, */
    /* sanches & canton) output this twice, first to 12 then to 11 */
    __asm__("outb %%al,$12\n\tjmp 1f\n1:\tjmp 1f\n1:\t"
    "outb %%al,$11\n\tjmp 1f\n1:\tjmp 1f\n1:"::
    "a" ((char) ((cmd == FD_READ)?DMA_READ:DMA_WRITE)));
    /* 8 low bits of addr */
    immoutb_p(addr,4);
    addr >>= 8;
    /* bits 8-15 of addr */
    immoutb_p(addr,4);
    addr >>= 8;
    /* bits 16-19 of addr */
    immoutb_p(addr,0x81);
    /* low 8 bits of count-1 (1024-1=0x3ff) */
    immoutb_p(0xff,5);
    /* high 8 bits of count-1 */
    immoutb_p(3,5);
    /* activate DMA 2 */
    immoutb_p(0|2,10);

    enable_int();
    return ;
   
}

/*======================================================================*
                           get_fat_entry
 *======================================================================*/
PUBLIC t_16 get_fat_entry(t_16 s){
    int i;
    t_32 sector=1;//FAT表区
    t_8 fat[512*12];//FAT表缓存
    t_8 *p=fat;//FAT表缓存指针
    t_16 r;//result
    for(i=1;i<10;i++){
        fdc();
        FloppyReadSector(sector,p);
        sector++;
        p+=512;
    }
    if(s%2==0){
        
        r=fat[s/2*3]+((fat[s/2*3+1] & 0xF) <<8);

    }else{
        r=(fat[s/2*3+1]>>4)+(fat[s/2*3+2]<<4);
    }
    return r;

}

/*======================================================================*
                           read_sectors
 *======================================================================*/
PUBLIC void read_sectors(t_32 start,t_32 size,t_8 *buf){
    int i;
    t_8 *dst = buf;
    memset(buf, '\0', size * 0xFF);
    for(i = 0; i < size; i ++){
        fdc();

        FloppyReadSector(start+i, dst);
        dst += 0xFF;
    }
    return ;
}
/*======================================================================*
                           read_name
 *======================================================================*/
PUBLIC char* read_name(char *data, char *name, char *ext){
    int i, j;
    memcpy(name,data,8);
    memcpy(ext,data+8,3);
    for(j=0;j<8;j++){

        if(name[j]==0x20){
            name[j]=0;
        }
    }
    name[8]=0;
    for(j=0;j<8;j++){
        if(ext[j]==0x20){
            ext[j]=0;
        }
    }
    ext[3]=0;
    return name;
}


/*======================================================================*
                               read_file
 *======================================================================*/
PUBLIC int read_file(char* n, char* e,char* buf){
    char*       buff = buf;
    fat_entry   entries[256];
    fat_entry   *p = entries;
    int         count = 0;
    int         r = 0;
    int         i = 0;
    read_sectors(SECTOR_NO_OF_ROOT_DIR, ROOT_DIR_SECTORS, (char*)entries);
    while(p->attr != 0){
        if(p->attr==ATTR_FILE_NORMAL){
            char name[9];
            char ext[4];
            char bign[9];
            char bige[4];
            read_name(p->name,name,ext);
            //对比
            if(strcmp(name,n)==0 && strcmp(ext,e)==0){
                //找到文件
                int file_sector;
                t_16 clus=p->fst_clus;
                
                while(clus<0xFF7){
                    file_sector=clus+FIRST_DATA_SECTOR;
                    fdc();
                    FloppyReadSector(file_sector,buff);
                    clus=get_fat_entry(clus);
                    buff += 512;
                    r+=512;
                }
                return r;
            }
        }
        p ++;
    }
    return -1;
}
/*======================================================================*
                               upper_name
 *======================================================================*/
PUBLIC int upper_name(char *buf,int len){
    char* p;
    len = strlen(buf);
    for(p=buf;p<buf+len;p++){
        if(*p>='a' && *p<='z'){
            *p = *p-'a'+'A';
        }
    }
}
