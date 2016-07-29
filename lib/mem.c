
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
#include "mem.h"                                         


PUBLIC int initMem(unsigned int base , unsigned int limit){
	mem_head *head  	= (mem_head*)base;
	mem_head *first 	= (mem_head*)(base + sizeof(mem_head));
	head -> is_valid 	= FALSE;
	head -> size 		= 0;
	head -> next 		= (unsigned int)first;
	first -> is_valid   = TRUE;
	first -> size 		= limit - sizeof(mem_head) * 2;
	first -> next  		= (unsigned int)head;
	return first -> size;
}

/*======================================================================*
                               is_alphanumeric
 *======================================================================*/
PUBLIC char* malloc(unsigned int size)
{

	mem_head *p;
	p = ((mem_head*)(MALLOC_MEM_BASE)) ;
	p = (mem_head*)p-> next;
	while( p != (mem_head*)MALLOC_MEM_BASE){

		if( p->is_valid==TRUE &&  p->size >= (size+2*sizeof(mem_head)) ){
			mem_head* m_free 	= p + 1;
			mem_head* m_r  		= (mem_head*)((char*)p + sizeof(mem_head) + size);

			m_r->is_valid	= FALSE;
			m_r->size 		= size;
			m_r->next 		= (unsigned int)m_free;

			m_free->is_valid= TRUE;
			m_free->size 	= p->size - size;
			m_free->next 	= p->next;

			p->is_valid 	= FALSE;
			p->size 		= 0;
			p->next 		= (unsigned int)m_r;

			memset((char*)m_r + sizeof(mem_head),0x0,size);

			//printf("M_R:%x SIZE:%x ADDR:%x \n",m_r,m_r->size,m_r->next);

			return (char*)m_r + sizeof(mem_head);

		}
		p = (mem_head*)p -> next;
	}
	return (char*)0;
	//没有合适的内存块
	
}
