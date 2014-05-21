#include "stm32f4xx.h"
#include <board.h>
#include <rtthread.h>

void rt_app_entry(void *parameter)
{
//    while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET);
//        rt_kprintf("fuck\n");  
    while(1)
    {
        rt_thread_delay(2);
    }
}



void Jacob_appinit()
{
    rt_thread_t tid;

    tid = rt_thread_create("jacob",
                           rt_app_entry, RT_NULL,
                           4048, 10, 20);
    
    
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}