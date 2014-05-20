#include "mouse_gesture.h"
#include "debug.h"
#include "usbh_hid_mouse.h"
#include "rtthread.h"
#include "math.h"
#include "app_interface.h"
struct mouse_pos{
    s16 x;
    s16 y;
};
static struct mouse_pos track[200];
static void mouse_ana(HID_MOUSE_Data_TypeDef *mouse);
static u16 key_cnt=0;
static double qiufangcha(struct mouse_pos *track, int count) ;
double qiufangcha(struct mouse_pos *track, int count) {
	double d1, d2, d3;
	double mx, my;
	int i;

	// ??????
	d1 = d2 = d3 = mx = my = 0.0;

	// ??x?y????
	for (i = 0; i < count; i++) {
		mx += track[i].x;
		my += track[i].y;
	}
	mx = mx / count;
	my = my / count;

	// ?????????????
	for (i = 0; i < count; i++) {
		d1 += (track[i].x - mx) * (track[i].y - my);
		d2 += (track[i].x - mx) * (track[i].x - mx);
		d3 += (track[i].y - my) * (track[i].y - my);
	}
	d2 = sqrt(d2 * d3);
	if (d2 == 0) {
		d1 = -1;
	} else {
		d1 = d1 / d2;
	}

	return (d1);
}
bool mouse_capture(u8 *mail)
{
  //  u8 i=0;
    HID_MOUSE_Data_TypeDef * mouse=(HID_MOUSE_Data_TypeDef *)mail;
    static u8 key_state=0;
    
//    for(i=0;i<8;i++)
//    {
//        DBG("%d",mail[i]);
//    }
    if(mouse->button==2)
    {
        
        track[key_cnt].x=(key_cnt>0?track[key_cnt-1].x:0)+mouse->x;
        track[key_cnt].y=(key_cnt>0?track[key_cnt-1].y:0)+mouse->y;
        mouse->button=0;
        if(key_cnt<200)
        {
            key_cnt++;
        }
        if(key_state==0)
        {
            
            key_state=1;
            return false;
        }      
    }
    else
    {
        if(key_state==1)
        {
            mouse_ana(mouse);
            
                key_cnt=0;
                key_state=0;
                return false;
        }
    }
    return true;
}
extern rt_mq_t mq_commu;
extern const u8  ascii2usb[128];
static void mouse_ana(HID_MOUSE_Data_TypeDef *mouse)
{
    
    if(key_cnt<=5)
    {
        u8 buf[9];
        u8 i=0;
        buf[0]=11;
        mouse->button=2;
        for(i=1;i<9;i++)
        {
            buf[i]=((u8*)mouse)[i-1];
        }  
        rt_mq_send(mq_commu,buf,9);
        rt_thread_delay(10);
        mouse->button=0;
        mouse->x=0;
        mouse->y=0;
        mouse->z=0;
        for(i=1;i<9;i++)
        {
            buf[i]=((u8*)mouse)[i-1];
        } 
        rt_mq_send(mq_commu,buf,9);
        
    }
    else
    {
        double fangcha;
        fangcha=qiufangcha(track,key_cnt);
        DBG("====%f",fangcha);
        if(fangcha>0.9||fangcha<-0.9)
        {   
           u16 buf[6];
           buf[0]=(1<<9)+(1<<control_key_index("lalt"));//press alt
           buf[1]=ascii2usb[32];
           buf[2]=(1<<10)+(1<<2);//release alt
           buf[3]=(1<<9)+(1<<control_key_index("lctrl"));//press ctrl
           buf[4]=ascii2usb['n'];
           buf[5]=(1<<10)+(1<<control_key_index("lctrl"));
           //DBG("wwww%dwwww",buf[1]);
           press_string_pure(buf,6);
        }
    }
}


