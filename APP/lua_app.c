#include "lua_app.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
int putskey(lua_State *L)
{
    int num=lua_gettop(L);
    int curpos;
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
void lua_init()
{   
    FIL file;
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
    
    luaL_dostring(L, (const char *)read_buf);
    
}

