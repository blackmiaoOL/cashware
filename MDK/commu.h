#ifndef _COMMU_H_
#define _COMMU_H_
#include "stm32f4lib.h"
#include "usbh_hid_mouse.h"
void commu_send(u8* buf,u32 lenth);
void commu_recv(u8* buf,u32 lenth);
void commu_Init(void);
u8 commu_send_byte(u8 info);
extern char thread_commu_stack[1024];
extern struct rt_thread thread_commu;
void rt_thread_entry_commu(void* parameter);

extern char thread_commu_read_stack[1024];
extern struct rt_thread thread_commu_read;
void rt_thread_entry_commu(void* parameter);
	
	
#endif//_COMMU_H_

