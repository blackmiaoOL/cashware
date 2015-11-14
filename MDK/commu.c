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
rt_sem_t sem_commu_self;
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
#define RECV_BUF_SIZE 600
static u8 recv_buf[RECV_BUF_SIZE];
struct rt_thread thread_commu;
u8 commu_send_byte(u8 info);


#define IRQ_L IO0(IOCB,PIN9)
#define IRQ_H IO1(IOCB,PIN9)

#define SPI_STATE_WAIT 0
#define SPI_STATE_SEND_WAIT 1
#define SPI_STATE_SEND_CONTENT 2


static u8 spi_state;
static u32 spi_send_len;
static u8 *spi_send_buf;
void commu_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStruct;
    
    SCPE(PERIOA);
    SCPE(PERIOC);
    SCPE(PERSPI3);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    // enable clock for used IO pins
    
    /* configure pins used by SPI3
     * PC10 = SCK
     * PC11 = MISO
     * PC12 = MOSI
	 * PA15 = NSS
	 * PC9  = slave has info
     */
    IOConfig(IOCB,PIN9,tuiwanshuchu);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    IRQ_L;
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    
    
    
    
    
    // connect SPI3 pins to SPI alternate function
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15,GPIO_AF_SPI3);
    
    
    SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);
    SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);
    
    
    
    // enable clock for used IO pins
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//refer platform_config.h
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex
    SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;     // transmit in slave mode, NSS pin has
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
    SPI_InitStruct.SPI_NSS = SPI_NSS_Hard; // set the NSS HARD
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
    SPI_Init(SPI3, &SPI_InitStruct);
    SPI_Cmd(SPI3, ENABLE); // enable SPI3
    
    
}
#define SPI_STATE_WAIT 0
#define SPI_STATE_SEND_WAIT 1
#define SPI_STATE_SEND_CONTENT 2
#define SPI_STATE_RECVING 3
#define SPI_SEND_HEAD 0x3E
#define SPI_SEND_NOTHING 0x8c
#define SPI_RECV_HEAD 0x1D
#define SPI_RECV_HEAD_MASTER_RECV 0XA8
#define SPI_RECV_HEAD_MASTER_SEND 0X8a
void commu_send(u8* buf,u32 len)
{

//	spi_state=SPI_STATE_SEND_WAIT;
	spi_send_buf=buf;
	spi_send_len=len;
	IRQ_H;
	
	while(spi_state!=SPI_STATE_SEND_WAIT&&spi_state!=SPI_STATE_SEND_CONTENT);
	while(spi_state!=SPI_STATE_WAIT);
	
}
char thread_commu_read_stack[1024];
struct rt_thread thread_commu_read;


void rt_thread_entry_commu(void* parameter){
	while(1){
		rt_kprintf("S");   
		commu_send("miaowu",6);
		if(!rt_sem_take(sem_commu_self,100)){
			
			u32 len=(recv_buf[0]<<8)+(recv_buf[1]);
			rt_kprintf("get%d",len);
			for(u32 i=0;i<len;i++){
				rt_kprintf("%c",recv_buf[i+2]);
			}
		}
	}
}


u8 test_data;

void SPI3_IRQHandler(void)
{
    static u32  cnt = 0;
    if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_TXE) == SET)
    {
		if(spi_state==SPI_STATE_SEND_WAIT)
		{
			SPI3->DR=SPI_SEND_HEAD;// start
			spi_state=SPI_STATE_SEND_CONTENT;
			cnt=0;
		}else if(spi_state==SPI_STATE_SEND_CONTENT)
		{
			if(cnt==0)
			{
				SPI3->DR=spi_send_len>>8;
				
			}else if(cnt==1)
			{
				SPI3->DR=spi_send_len%8;
			}else
			{
				SPI3->DR=spi_send_buf[cnt-2];
			}
			
			cnt++;
			if(cnt==spi_send_len+3)
			{
				spi_state=SPI_STATE_WAIT;
				IRQ_L;
			}
		}else{
			SPI3->DR=SPI_SEND_NOTHING;
		}
    }
	static u32 len=0;
    if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == SET)
    {
		u8 data= SPI3->DR;
//		rt_kprintf("%X ,spi_state %d\r\n",data,spi_state);
		if(spi_state==SPI_STATE_WAIT){
			if(data==SPI_RECV_HEAD_MASTER_RECV){
				spi_state=SPI_STATE_SEND_WAIT;
			}else if(data==SPI_RECV_HEAD_MASTER_SEND){
				spi_state=SPI_STATE_RECVING;
				cnt=0;
				len=0;
			}
		}else if(spi_state==SPI_STATE_RECVING){
			u32 pos=cnt>RECV_BUF_SIZE+1?RECV_BUF_SIZE-1:cnt;
			recv_buf[pos]=data;
			if(cnt==0){
				len=data<<8;
			}else if(cnt==1){
				len|=data;
			}else if(cnt<len+1){
			}else if(cnt>=len+1){
				rt_sem_release(sem_commu_self);
				spi_state=SPI_STATE_WAIT;
			}
			
			cnt++;	
		}else{
			//do nothing		
		}
    }    
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

void commu_recv_byte_half(u8* data);
void commu_recv_byte(u8* data)
{
//    *data=commu_send_byte(0x12);
    
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
//    u32 i=0;
//    
//    if(lenth==512)
//    {
//        commu_send_byte(12<<4);
//    }
//    else
//        commu_send_byte(9<<4);
//    commu_send_byte(12);
//    for(i=0;i<lenth;i++)
//    {
//        commu_recv_byte(buf+i);
//        
//    }
    
}

