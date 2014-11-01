#include "lua_app.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "usbh_hid_mouse.h"
#include "ahk_analyser.h"
static int lua_wait_event(lua_State *L);
static int lua_flag_set(lua_State *L);
rt_mq_t mq_lua;
u32 lua_flag=0;//bit0:keyboard enable bit1:mouse enable 
static int lua_key_register(lua_State *L);
static void delay_us2(int us)
{
    volatile int a,b;
    for(b=0;b<us;b++)
    for(a=0;a<84;a++)
    {
        
    }
}
static int lua_delay_ms(lua_State *L)
{
    rt_thread_delay(lua_tonumber(L,1));
    return 0;
    
}
static int ultrasonic_read(lua_State *L)
{
    u32 time2=0;
    u32 cnt=0;
    rt_enter_critical();
    IO1(IOAB,PIN0);
    delay_us2(30);
    IO0(IOAB,PIN0);
    time2=0;
    
    delay_us2(3);
    
    while(!PAin(1));
    {
        cnt++;
        if(cnt>14000)
        {
            rt_exit_critical();
            lua_pushnumber(L, 0);
            return 1;
        }
            
    }
    while(PAin(1))
    {
        time2++;
        if(time2>35000)
        {
            rt_exit_critical();
            lua_pushnumber(L, 0);
            return 1;
        }
    }
    rt_exit_critical();
    //DBG("ddd%d\r\n",time2);
    lua_pushnumber(L, time2);
    return 1;
}
int lua_mouse_send(lua_State *L)
{
    mouse_data a;
    u8 i=0;
    u8 buf[9];
    buf[0]=1;
    a.button=lua_tonumber(L,1);
    a.x=lua_tonumber(L,2);
    a.y=lua_tonumber(L,3);
    a.z=lua_tonumber(L,4);
    for(i=0;i<8;i++)
    {
        buf[i+1]=((u8 *)&a)[i];
    }
    rt_mq_send(mq_commu,buf,9);
    return 0;
}
static int key_put(lua_State *L)
{
    int num=lua_gettop(L);
    int lenth=0;
    u8 i=0;
    u16 key;
    u16* string;
    u8* str_ptr;
    DBG("num=%d\r\n",num);
    key=ascii2usb[(*lua_tostring(L,1))];
    DBG("key=%d\r\n",key);
    str_ptr=(u8*)lua_tostring(L,1);
    while((str_ptr[lenth])!=0)
    {
        lenth++;
        if(lenth>100)
            break;
    }
    string=(u16*)rt_malloc(lenth<<1);
    for(i=0;i<lenth;i++)
    {
        string[i]=ascii2usb[str_ptr[i]];
    }
    DBG("lenth=%d\r\n",lenth);
    press_string_pure(string,lenth);
    buf_clear();
    return true;
}
static void rt_thread_entry_lua(void* parameter)
{
    lua_State *L=(lua_State *)parameter;
    luaL_dostring(L, (const char *)read_buf);
}
void lua_init()
{   
    FIL file;
    rt_thread_t thread_lua;
    u32 cnt=0;
    lua_State *L  ;
    L = lua_open();      
    luaopen_base(L);  
    
    printf("open:%d",f_open(&file,"/profile.lua",FA_OPEN_EXISTING|FA_WRITE|FA_READ|FA_OPEN_ALWAYS|FA__WRITTEN));
    printf("read:%d",f_read(&file,read_buf,file.fsize,&cnt));
    printf("size=%d\r\n",file.fsize);
    f_close(&file);
    read_buf[file.fsize]=0;
    lua_register(L, "flag_set",lua_flag_set);
    lua_register(L, "key_register", lua_key_register); 
    lua_register(L, "key_put", key_put); 
    lua_register(L, "ultrasonic_read",ultrasonic_read);
    lua_register(L, "delay_ms",lua_delay_ms);
    lua_register(L, "mouse_send",lua_mouse_send);
    lua_register(L, "wait_event",lua_wait_event);
    thread_lua= rt_thread_create(
                   "lua",
                   rt_thread_entry_lua,
                   L,
                   30024L,17,5);
    rt_thread_startup(thread_lua);
   
}
static int lua_wait_event(lua_State *L)
{
    u8 mail[10];
    u8 i=0;
    rt_mq_recv (mq_lua, mail,10, RT_WAITING_FOREVER);
    for(i=0;i<10;i++)
    {
        lua_pushnumber(L, mail[i]);  
    }
    return 10;
}
static int lua_flag_set(lua_State *L)
{    
    int pos=lua_tointeger(L,1);
    int value=lua_tointeger(L,2);
    lua_flag&=(~(value<<pos));
    lua_flag|=(value<<pos);
    return 0;
}    
static void lua_send_mail(struct st_key_cap* key_cap)
{
    u8 buf[10];

    buf[9]=key_cap->flag;
    rt_mq_send (mq_lua, (void*)buf, 10);
    DBG("lua_send_mail");
}
static int lua_key_register(lua_State *L)
{
    ctrl_filter filter;
    cap  cap_this;
    int num=lua_gettop(L);
    int i=0;
    int event=lua_tointeger(L,1);
    DBG("EVENT=%d\n",event);
    filter_Init(&filter);
    filter.key=ascii2usb[lua_tostring(L, 2)[0]];
    filter_add_string(&filter,lua_tostring(L, 3));
    cap_this.filter=filter;
    cap_this.key_exe=lua_send_mail;
    cap_this.flag=(u8)event;
    key_cap_add(&cap_this);
    return 0;
   // for(i=0;i<num;i)
    
//    key_temp[0]=2;//right ctrl
//    key_temp[1]='^';
    
//    filter_add(&filter,key_temp);
//    filter.key=ascii2usb['i'];
//    block.filter=filter;
//    cap_this2.filter=block.filter;
//    cap_this2.key_exe=macro_play_prepare;
//    key_cap_add(&cap_this2);
   // 1<<control_key_index("lgui")
}

