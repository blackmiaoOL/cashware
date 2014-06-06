#include "lua_app.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
void delay_us2(int us)
{
    volatile int a,b;
    for(b=0;b<us;b++)
    for(a=0;a<84;a++)
    {
        
    }
}
int lua_delay_ms(lua_State *L)
{
    rt_thread_delay(lua_tonumber(L,1));
    return 0;
    
}
int ultrasonic_read(lua_State *L)
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
            return 0;
        }
            
    }
    while(PAin(1))
    {
        time2++;
        if(time2>35000)
        {
            rt_exit_critical();
            return 0;
        }
    }
    rt_exit_critical();
    //DBG("ddd%d\r\n",time2);
    lua_pushnumber(L, time2);
    return 1;
}
int putskey(lua_State *L)
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
    lua_register(L, "putskey", putskey); 
    lua_register(L, "ultrasonic_read",ultrasonic_read);
    lua_register(L, "lua_delay_ms",lua_delay_ms);
    thread_lua= rt_thread_create(
                   "lua",
                   rt_thread_entry_lua,
                   L,
                   40024L,17,5);
    rt_thread_startup(thread_lua);
    
    
    
    
    
}


