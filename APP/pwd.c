#include "pwd.h"
ALIGN(RT_ALIGN_SIZE)
static char thread_pwd_stack[1024];
rt_thread_t thread_pwd;

static void rt_thread_entry_pwd(void* parameter)
{

}
void pwd_start(struct st_key_cap* key_cap)
{
    u8 key_value;
       thread_pwd= rt_thread_create(
                   "app",
                   rt_thread_entry_pwd,
                   RT_NULL,
                   1024,10,5);
    rt_thread_startup(thread_pwd);
//    key_value=getkey();
    if(key_value==ascii2usb['g'])
    {

    }
    else if(key_value==ascii2usb['g'])
    {

    }    
}

