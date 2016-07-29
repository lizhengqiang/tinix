
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            mem.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Edward		2013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_TINIX_MEM_H_
#define	_TINIX_MEM_H_

#define MALLOC_MEM_BASE 	0x3000000
#define MALLOC_MEM_LIMIT 	0x0EF0000


typedef struct s_mem 
{
	unsigned int is_valid	;
	unsigned int size		;
	unsigned int next		;
}mem_head;

PUBLIC int initMem(unsigned int base , unsigned int limit);






#endif /* _TINIX_MEM_H_ */
