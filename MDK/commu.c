#include "commu.h"
#include "debug.h"
#include "rtthread.h"
#include "app_interface.h"
#include "mouse_gesture.h"

//PIN0~PIN7:info
//PIN8:CLK
//PIN9:endflag(high enable)
//#define CLK_H IO1(IOEB,PIN11)
//#define CLK_L IO0(IOEB,PIN11)
//#define END_H IO1(IOEB,PIN13)
//#define END_L IO0(IOEB,PIN13)
//#define DATA_SET(a) do{GPIOE->ODR&=0XFC03;GPIOE->ODR|=((((u16)a)<<2)&0X3fc);}while(0)
rt_mq_t mq_commu;
rt_sem_t sem_commu;
rt_sem_t sem_app_init;
extern  u32 flash_addr;
extern u8 blue_choose;
void commu_blue_send(u8* buf,u32 lenth);
void key_cap_Init(void);
bool key_capture(u8 *buf);
extern rt_mq_t mq_commu;
void blue_tooth_Init(void);
ALIGN(RT_ALIGN_SIZE)
char thread_commu_stack[1024];
struct rt_thread thread_commu;
extern u8 flash_buf[512];
void rt_thread_entry_commu(void* parameter)
{
    u8 mail[9];

    rt_sem_take(sem_app_init,RT_WAITING_FOREVER);
    while(1)
    {
        rt_mq_recv (mq_commu, mail,9, RT_WAITING_FOREVER);
        switch(mail[0])
        {
        case 0://keyborad send with cap
        {
//        	u8 i=0;
//			for(i=0;i<9;i++)	
//            DBG("i=%dmail=%d\r\n",i,mail[i]);
            if(key_capture(mail+1))
            {
                mail[8]=92;
                if(blue_choose)
                {
                    mail[0]=6;
                    commu_blue_send(mail,9);
                }
                
                else
                {
                   commu_send(mail,9); 
                }
            }
            break;
        }
        case 1://mouse 
        {

            static u8 release_flag=0;
//            u8 i=0;
//			for(i=0;i<9;i++)	
//               DBG("i=%dmail=%d\r\n",i,mail[i]);
            mail[8]=90;
            if((mail[3]==124||mail[5]==240)&&mail[7]==255)
            {
                DBG("mouse error");
                goto mouseend;
            }
            if(mail[1]==6)
            {
                mail[1]=1;
                commu_blue_send(mail,9);
                commu_send(mail,9); 
                blue_choose=1-blue_choose;
                
                if(blue_choose)
                    cmd("Blue remote");
                else
                {
                    cmd("Blue closed");
                }
                release_flag=1;
            }
            else if(release_flag)
            {
                if(mail[1]==0)
                {
                    release_flag=0;
                }   
            }
            else
            {
                if(mouse_capture(mail+1))
                {
                    
                    if(blue_choose)
                    {     
                        commu_blue_send(mail,9);
                    }
                    else
                    {
                        mouse_code((mail+1));
                        commu_send(mail,9); 
                    }
                }
            }
            mouseend:
            
            break;
        }
        case 3://read flash addr
        {
            u8 buf[4];
            commu_recv(buf,4);
            flash_addr=*((u32*)buf);
            rt_sem_release(sem_commu);
            break;
        }
        case 4://send flash content
        {
            commu_send(flash_buf,512);
            rt_sem_release(sem_commu);
            break;
        }
        case 5://receive flash content 
        {
            commu_recv(flash_buf,512);
            rt_sem_release(sem_commu);
            break;
        }
        case 6://keyborad send immediately
        {
            mail[8]=92;
            if(blue_choose)
            {
                mail[0]=6;
                commu_blue_send(mail,9);
            }
            else
            {
                mail[0]=0;
               commu_send(mail,9); 
            }
            rt_thread_delay(10);
            break;
        }
        case 10:
        {
            mail[8]=92;
            mail[0]=6;
            commu_blue_send(mail,9);
            mail[0]=0;
            commu_send(mail,9);
            rt_thread_delay(10);  
            break;
        }
        case 11://mouse direct
        {
//            u8 i=0;
//			for(i=0;i<9;i++)	
//               DBG("i=%dmail=%d\r\n",i,mail[i]);
            mail[0]=1;
            mouse_code((mail+1));
            if(blue_choose)
            {     
                commu_blue_send(mail,9);
            }
            else
            {
                commu_send(mail,9); 
            }
            break;
        }
        }
        rt_thread_delay(1);
    }


}

u8 commu_send_byte(u8 info);

//#define RW_H    IO1(IOCB,PIN12)//H:R L:W
//#define RW_L    IO0(IOCB,PIN12)
#define CLK_H   IO1(IOBB,PIN9)
#define CLK_L   IO0(IOBB,PIN9)
#define FINISH_FLAG    PBin(8)

#define DATA_W(data)    do{IOBB->ODR&=0XFF00;IOBB->ODR|=(0X00ff&data);}while(0)
#define DATA_R  (IOBB->IDR&=0XFF)
void commu_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure0;
	SPI_InitTypeDef  SPI_InitStructure;

  
    
	SCPE(PERIOB);
	SCPE(PERIOA);
    IOConfig(IOBB,PIN2|PIN6|PIN7,fukongshuru);
	GPIO_PinAFConfig(IOBB, 3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IOBB, 4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IOBB, 5, GPIO_AF_SPI1);

	GPIO_InitStructure0.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure0.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure0.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure0.GPIO_PuPd  = GPIO_PuPd_DOWN;



	GPIO_InitStructure0.GPIO_Pin=PIN3;
	GPIO_Init(GPIOB, &GPIO_InitStructure0);
	GPIO_InitStructure0.GPIO_Pin=PIN4;
	//IOConfig(IOBB,PIN4,fukongshuru);
	GPIO_Init(GPIOB, &GPIO_InitStructure0);
	GPIO_InitStructure0.GPIO_Pin=PIN5;
	GPIO_Init(GPIOB, &GPIO_InitStructure0);
	

	
	IOConfig(IOAB,PIN15,tuiwanshuchu);
	IO1(IOAB,PIN15);

	SCPE(PERSPI1);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	



}
#define FPGA_CS_L IO0(IOAB,PIN15)
#define FPGA_CS_H IO1(IOAB,PIN15)
// void commu_send_byte_half (u8 info);

u8 commu_send_byte(u8 info)
{

//    commu_send_byte_half(info);
//	commu_send_byte_half(info<<4);

	int j;
	u8 data=0;

   FPGA_CS_L;
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send Half Word through the sFLASH peripheral */
  SPI_I2S_SendData(SPI1, info);

  /*!< Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
   FPGA_CS_H;
  /*!< Return the Half Word read from the SPI bus */

		for(j=0;j<100;j++);

  data=SPI_I2S_ReceiveData(SPI1);
 // DBG("%c-%c",info,data);
  return data;

	
}
// void commu_send_byte_half(u8 info)
//{
//    volatile int cnt=0;
//	u32 i=0;
//    begin:
//	DATA_W(info);
//    CLK_L;
//    while(!FINISH_FLAG)
//	{
//		i++;
//		if(i>3000000)
//		{
//			CLK_H;
//			 for(cnt=0;cnt<100;cnt++);
//			goto begin;
//		}
//			
//	}
//    CLK_H;
//    for(cnt=0;cnt<100;cnt++);
//	
//}
void blue_putchar(u8 ch);
void commu_blue_send(u8* buf,u32 lenth)
{
    u32 i=0;
   // blue_putchar(7<<4);
    for(i=0;i<lenth;i++)
	{
		blue_putchar(buf[i]);
        rt_thread_delay(1);
	}
}
void commu_send(u8* buf,u32 lenth)
{
	u32 i=0;
	//END_L;
//    RW_L;//W
//    CLK_H;
	//commu_send_byte_half(7<<4);
	if(lenth==512)
	{
		commu_send_byte(11<<4);
	}
	else
		commu_send_byte(7<<4);
	for(i=0;i<lenth;i++)
	{
		commu_send_byte(buf[i]);
		//delay_ms2(10);
	}	
}
void commu_recv_byte_half(u8* data);
 void commu_recv_byte(u8* data)
{
	*data=commu_send_byte(0x12);

}

//void commu_recv_byte_half(u8* data)
//{
//   volatile int cnt=0;
//	u32 i=0;
//    begin:
//    CLK_L;
//    while(!FINISH_FLAG)
//	{
//		i++;
//		if(i>30000)
//		{
//			CLK_H;
//			 for(cnt=0;cnt<100;cnt++);
//			goto begin;
//		}
//			
//	}
//	*data=IOBB->IDR&0XFF;
//	//DBG("%d,",IOBB->IDR&0XFF);
//    CLK_H;
//	 while(FINISH_FLAG);
//    for(cnt=0;cnt<100;cnt++);
//}



void commu_recv(u8* buf,u32 lenth)
{
    u32 i=0;

	if(lenth==512)
	{
		commu_send_byte(12<<4);
	}
	else
    commu_send_byte(9<<4);
commu_send_byte(12);
    for(i=0;i<lenth;i++)
    {
        commu_recv_byte(buf+i);
		
    }

}

