#include "app_interface.h"
#include "string.h"
#include "ahk_analyser.h"
#include "key_remap.h"
rt_device_t OLED_dev;
extern rt_sem_t sem_flash;
u8 read_buf[9000];
u8 buf_out[9];

void buf_clear(void);
extern rt_mailbox_t mb_commu;
cap key_cap_free[key_cap_cnt_all];
//st_key_cap key_cap_use[200];
cap* cap_use_head=RT_NULL;
cap* cap_free_head=key_cap_free;
rt_uint16_t key_cap_cnt=0;
const u8  ascii2usb[]={
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,40,0,0, \
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0, \
    44,46,0,0,  0,0,0,0,  0,0,0,0,  54,0,55,0, \
    39,30,31,32,  33,34,35,36,  37,38,51,0,  0,0,0,56, \

    31,4,5,6,  7,8,9,10,  11,12,13,14,  15,16,17,18, \
    19,20,21,22,  23,24,25,26,  27,28,29,0,  0,0,0,0, \
    0,4,5,6,  7,8,9,10,  11,12,13,14,  15,16,17,18, \
    19,20,21,22,  23,24,25,26,  27,28,29,0,  50,0,53,0, \
};
const u8  *const p_ascii2usb=ascii2usb;
const u8 shift_table[]={
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,1,0,    0,0,0,1,

    1,1,1,1,    1,1,1,1,    1,1,1,1,    1,1,1,1,
    1,1,1,1,    1,1,1,1,    1,1,1,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    1,0,1,0,

};



#define min(a,b) (a>b?b:a)
void cmd_line(u8* content);
void cmd(u8* content)
{
    u32 content_char_cnt=0;
    u8 content_line_cnt=0;
    u8 i=0;
    while(content[content_char_cnt]!=0)
    {
        content_char_cnt++;
    }
    content_line_cnt=(content_char_cnt-1)/16+1;
    for(i=0;i<content_line_cnt;i++)
    {
        cmd_line(content+i*16);
    }
}
void buf_clear()
{
    u8 i=0;
    for(i=2;i<9;i++)
    {
        buf_out[i]=0;
    }
    buf_out[0]=6;
    press;

}
void cmd_line(u8* content)
{
    u32 i=0;
    u32 content_char_cnt=0;
    //	u32 content_line_cnt;
    static u32 line_cnt=0;
    static u8 OLED_buf[4][16];
    while(content[content_char_cnt]!=0)
    {
        content_char_cnt++;
        if(content_char_cnt==16)
        {
            break;
        }

    }
    if(line_cnt<4)
    {
        u8 j=0;
        for(j=0;j<min(16,content_char_cnt);j++)
        {
            OLED_buf[line_cnt][j]=content[j];
        }
    }
    else
    {
        u8 i=0;
        for(i=0;i<3;i++)
        {
            u8 j=0;
            for(j=0;j<16;j++)
            {
                OLED_buf[i][j]=OLED_buf[i+1][j];
            }
        }

        for(i=0;i<16;i++)
        {
            OLED_buf[3][i]=0;
        }
        for(i=0;i<min(16,content_char_cnt);i++)
        {
            OLED_buf[3][i]=content[i];
        }

    }


    OLED_dev->control(OLED_dev,0,0);
    for(i=0;i<4;i++)
    {
        //		u8 k=0;
        OLED_dev->write(OLED_dev,(i*2),OLED_buf[i],16);
        //		for(k=0;k<16;k++)
        //		DBG("OLED:%d",OLED_buf[i][k]);
    }


    line_cnt++;
}


const u8 key_string[key_string_num][key_stinrg_max_word]={
    "lctrl",
    "lshift",
    "lalt",
    "lgui",
    "rctrl",
    "rshift",
    "ralt",
    "rgui"};
u8  key_changetable[]={
    0,1,2,3,    4,5,6,7
};
s8 control_key_index(const char buf[])
{
    u8 i=0;
    for(i=0;i<8;i++)
    {
        if(!strcmp((const char *)buf,(const char *)key_string[i]))
            return i;
    }
    return -1;
}

u8 key_process(u8 control_key)
{
    u8 i=0;
    u8 return_key=0;
    for(i=0;i<8;i++)
    {
        if(control_key&(1<<i))
        {
            return_key|=(1<<key_changetable[i]);
        }
    }
    //key_changetable[]
    return return_key;
}
u8 control_key_filt(u8 key,ctrl_filter* filter)
{
    u8 i=0;
    for(i=0;i<filter->control_filter_cnt;i++)
    {
       // printf("filter%d",filter->control_filter[i]);
        if(key&filter->control_filter[i])
        {
            key&=(~filter->control_filter[i]);
        }
        else
            return 1;
    }
    if(key==0)
        return 0;
    else
        return 1;
}
bool key_capture(u8 *buf)
{
    u8 i=0;
    static  u8 hotkey_flag=0;
    static  u8 hotkey_value=0;
    DBG("CNT=%d",key_cap_cnt);
    //    for(i=0;i<8;i++)
    //    {
    //        DBG("%d\r\n",buf[i]);
    //    }
    //    DBG("%c",HID_KEYBRD_Key[HID_KEYBRD_Codes[buf[2]]]);
    //    if(buf[0]&(1<<6))
    //    {
    //        buf[0]&=(0xff-(1<<6));W
    //        buf[0]|=(1<<4) ;
    //    }
    //printf("buf[0]_raw=%d\r\n",buf[0]);
    buf[0]=key_process(buf[0]);//TODO
    if(hotkey_flag)
    {

        if(buf[2]==hotkey_value)
        {
            DBG("I block it!\r\n");

            return false;
        }
        else
        {
            hotkey_flag=0;
        }
    }
    for(i=0;i<key_cap_cnt;i++)
    {
        cap* cap_this;

        if(i==0)
        {
            cap_this=cap_use_head;
        }
        else
            cap_this=cap_this->next;


        if(control_key_filt(buf[0],&(cap_this->filter))==0)
        {
            DBG("(%d----%d)",key_cap_cnt,cap_this->filter.key);
            if( buf[2]==cap_this->filter.key)
            {
                DBG("I got it!\r\n");
                (cap_this->key_exe)(cap_this);
                hotkey_flag=1;
                hotkey_value=buf[2];

                return false;
            }
        }
    }

    return true;
}

void blue_set(cap * cap_this)
{
    u8 buf[9];
    u8 i=0;
    buf[0]=10;
    for(i=1;i<9;i++)
    {
        buf[i]=0;
    }
    rt_mq_send(mq_commu,(void *)buf,9);
    blue_choose=1-blue_choose;
    if(blue_choose)
        cmd("Blue remote");
    else
    {
        cmd("Blue closed");
    }
}
void  key_cap_add(cap* cap_this)
{
    u16 i=0;
    cap * cap_new;
    //DBG("++++++++++");
    if(cap_free_head->next==RT_NULL||key_cap_cnt==key_cap_cnt_all)
    {
        return ;
    }
    else
    {
        cap_new=cap_free_head;
        cap_free_head=cap_free_head->next;
    }

    cap_new->filter=cap_this->filter;
    cap_new->key_exe=cap_this->key_exe;
    cap_new->string=cap_this->string;
    cap_new->string_lenth=cap_this->string_lenth;

    if(key_cap_cnt==0)
    {
        cap_use_head=cap_new;
    }
    else
    {
        cap** cap_current=&cap_use_head;
        for(i=0;i<key_cap_cnt;i++)
        {
            cap_current=&((*cap_current)->next);
        }
        *cap_current=cap_new;
    }
    
    key_cap_cnt++;

}
void rt_thread_entry_app(void* parameter)
{
    static FATFS fs;  		//逻辑磁盘工作区.
    static FIL file;	  		//文件1
    u8 key_temp[2];
    cap  cap_this2;
    ctrl_filter filter;
    block_info block;
    //	static FIL ftemp;	  		//文件2.
    //	static UINT br,bw;			//读写变量
    //	static FILINFO fileinfo;	//文件信息
    //	static DIR dir;

    u32 cnt=0;
    u32 i=0;


    key_cap_Init();
    OLED_dev=rt_device_find("OLED");

    if(OLED_dev!=RT_NULL)
    {
        rt_kprintf("OLED dev found\n\r");
        //    rt_device_write(OLED_dev,0,&led_value,1);
        //  led_value++;
    }
    cmd("Initializing~");
	rt_device_open(OLED_dev,RT_DEVICE_FLAG_ACTIVATED);

    rt_sem_take(sem_flash,RT_WAITING_FOREVER);
    f_mount(&fs,"/",1);
    f_mkdir("/sks");
    //	f_opendir(&dir,"/");

    printf("open:%d",f_open(&file,"/key_t",FA_OPEN_EXISTING|FA_WRITE|FA_READ|FA_OPEN_ALWAYS|FA__WRITTEN));
    printf("read:%d",f_read(&file,read_buf,file.fsize,&cnt));
    printf("size=%d\r\n",file.fsize);
    for(i=0;i<file.fsize;i++)
    {
        putchar(read_buf[i]);
    }
    f_close(&file);
    
    if(key_table_process(read_buf,file.fsize))
    {
        cmd("Key table error");
    }
    else
    {
        cmd("Key table done");
    }

    printf("open:%d",f_open(&file,"/mode_1",FA_OPEN_EXISTING|FA_WRITE|FA_READ|FA_OPEN_ALWAYS|FA__WRITTEN));
    printf("read:%d",f_read(&file,read_buf,file.fsize,&cnt));
    printf("size=%d\r\n",file.fsize);
    rt_sem_release(sem_flash);
    for(i=0;i<file.fsize;i++)
    {
        putchar(read_buf[i]);
    }
    f_close(&file);

    if(key_mode_process(read_buf,file.fsize))
    {
        cmd("Key mode error");
    }
    else
    {
        cmd("Key mode done");
    }
   // rt_kprintf("cap cnt:%d\r\n",);
    cmd("Done");
    
    key_temp[0]=0;
    key_temp[1]='^';
    filter_Init(&filter);
    filter_add(&filter,key_temp);
    filter.key=53;
    block.filter=filter;
    cap_this2.filter=block.filter;
    cap_this2.key_exe=blue_set;
    key_cap_add(&cap_this2);
    
    key_temp[0]=0;
    key_temp[1]='!';
    filter_Init(&filter);
    filter_add(&filter,key_temp);
    filter.key=53;
    block.filter=filter;
    cap_this2.filter=block.filter;
    cap_this2.key_exe=blue_set;
    key_cap_add(&cap_this2);
}







