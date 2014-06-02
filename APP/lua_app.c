#include "lua_app.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
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
    luaL_dostring(L, (const char *)read_buf);
    
}

