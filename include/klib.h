
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            klib.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Edward		2013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef	_TINIX_KLIB_H_
#define	_TINIX_KLIB_H_

/* FDC Const*/
#define DMA_READ 0x46
#define DMA_WRITE 0x4A

/* FDC i/o port & const */
#define FLOPPY_REG_DOR 	0x3F2
#define FD_DCR 			0x3F7
#define FD_DATA 		0x3f5
#define FD_SPECIFY		0x03
#define FD_SEEK 		0x0F
#define FD_READ 		0xE6
#define FD_RECALIBRATE 	0x07
#define FD_STATUS 		0x3f4		
#define STATUS_READY 	0x80
#define STATUS_CBUSY 	0x10
#define STATUS_DIR 		0x40

/* File const for FAT12 */
#define SECTOR_NO_OF_ROOT_DIR	19
#define ROOT_DIR_SECTORS		1

#define ATTR_FILE_NORMAL 		0x20
#define CHAR_FILE_SPACE  		0x20

#define FIRST_DATA_SECTOR 		33-2

#define immoutb_p(val,port) \
__asm__("outb %0,%1\n\tjmp 1f\n1:\tjmp 1f\n1:"::"a" ((char) (val)),"i" (port))                                                    

typedef struct 
{
	unsigned int size, sect, head, track, stretch;
	unsigned char gap, rate, spec1;
}floppy_strucy ;

typedef struct{
	char 	name[11];
	t_8		attr;
	char	n[10];
	t_16	wrt_time;
	t_16	wrt_date;
	t_16 	fst_clus;
	t_32	file_size;
}fat_entry;





#endif /* _TINIX_KLIB_H_ */