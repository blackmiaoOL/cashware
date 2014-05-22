/***************************乐声电子科技有限公司****************************
**  工程名称：YS-V0.4语音识别开发板程序
**	CPU: STM32f103RCT6
**	晶振：8MHZ
**	波特率：9600 bit/S
**	配套产品信息：YS-V0.4语音识别开发板
**                http://yuesheng001.taobao.com
**  作者：zdings
**  联系：751956552@qq.com
**  修改日期：2012.4.11
**  说明：本程序 具备语音识别、串口通信、开发板基本功能演示。
***************************乐声电子科技有限公司******************************/
#include "includes.h"
#include <rtthread.h>
//#include "usart.h"
#include "debug.h"
//#include "stm32f10x.h"
//#include "stm32f10x_spi.h"
//#include "stm32f10x_exti.h"
//#include "stm32f10x_nvic.h"
//#include "core_cm3_m.h"
//#include "sys.h"
//#include "MOTO.h"
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)
/*************端口信息********************
 * 接线说明

 * CS   	PB12
 * P2/SDCK  PB13
 * P1/SDO   PB14
 * P0/SDI   PB15
 * IRQ      PB11

 * CLK      PA8
*****************************************/



/************************************************************************************
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
*********************************************************************************/
	uint8 nDemoFlag=0;
	uint8 nAsrStatus=0;
	uint8 nAsrRes;
	uint8 end_loop=0;
	u8 isplaying=0;
	extern void (*exe_func)(void);
void LD3320_Init(void);

uint8 RunASR(void);
void ProcessInt0(void);
void LD3320_EXTI_Cfg(void);
void LD3320_Spi_cfg(void);
void LD3320_GPIO_Cfg(void);
void LED_gpio_cfg(void);

/***********************************************************
* 名    称： LD3320_
*
* (void)
* 功    能： 主函数LD3320程序入口
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
extern rt_mq_t mq_commu;
void LD_loop()
{
    
    u8 mail[9];
	u8 i=0;
loop:if(nAsrStatus!=LD_ASR_RUNING)
     {
        DBG("***%d****",nAsrStatus);
        if(nAsrStatus==0x31)
        rt_thread_delay(1000);
     }
        
    switch(nAsrStatus)
    {
        case LD_ASR_RUNING:
        case LD_ASR_ERROR:
                            break;
        case LD_ASR_NONE:

                    nAsrStatus=LD_ASR_RUNING;
                    if (RunASR()==0)	//	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算
                    {
                        nAsrStatus = LD_ASR_ERROR;
                        DBG("init_false");
                    }
                    break;

        case LD_ASR_FOUNDOK:
                             nAsrRes = LD_GetResult( );	//	一次ASR识别流程结束，去取ASR最佳识别结果
                             DBG("\r\n识别码:");			 /*text.....*/
                             putchar(nAsrRes+0x30); /*text.....*/
							mail[i]=0; 
                            switch(nAsrRes)		   /*对结果执行相关操作,客户修改*/
                            {
								
								//ase CODE_XK:
								#define clear_mail for(i=0;i<9;i++){mail[i]=0;}
 
								case CODE_YMD:
									DBG("“xiaoxi识别成功\r\n"); 
                                    clear_mail;	
                                    mail[1]=1;
                                    rt_mq_send (mq_commu, mail,9);
                                    clear_mail;	
                                    mail[1]=3;
                                    rt_mq_send (mq_commu, mail,9);
                                    clear_mail;	
                                    mail[1]=3;
                                    mail[3]=38;
                                    rt_mq_send (mq_commu, mail,9);
                                    clear_mail;	

                                    rt_mq_send (mq_commu, mail,9);

                                    //	Key_LED();								 zarien...
                                  
								break;
                                case CODE_QJ:			/*命令“前进”*/
                                    DBG("“前进”命令识别成功\r\n"); /*text.....*/
                                    //	Glide_LED();		//实现流水灯功能	zarien...
                                    //qianxing();
									mail[0]=1;
									mail[1]=0;
									mail[2]=0;
									mail[3]=0;
									mail[4]=252;
									mail[5]=0;
									mail[6]=0;
									mail[7]=0;
									mail[8]=0;
									rt_mq_send (mq_commu, mail,9);
                                    break;
								//case CODE_SK:
                                case CODE_HT:	 /*命令“后退”*/
                                    DBG("“后退”命令识别成功\r\n"); /*text.....*/
//											Flicker_LED();		//实现灯闪烁功能	  zarien...
									mail[0]=1;
									mail[1]=0;
									mail[2]=0;
									mail[3]=0;
									mail[4]=3;
									mail[5]=0;
									mail[6]=0;
									mail[7]=0;
									mail[8]=0;
									rt_mq_send (mq_commu, mail,9);
                                   // houtui();
                                    break;

                                case CODE_KNM:		/*命令“举手”*/
                                    DBG("kannimei命令识别成功\r\n"); /*text.....*/
                                    for(i=0;i<9;i++)
									{
										mail[i]=0;
									}	
									mail[1]=8;
									rt_mq_send (mq_commu, mail,9);
									for(i=0;i<9;i++)
									{
										mail[i]=0;
									}	
									mail[1]=8;
									mail[3]=7;
									rt_mq_send (mq_commu, mail,9);
									for(i=0;i<9;i++)
									{
										mail[i]=0;
									}	
									mail[1]=8;
									rt_mq_send (mq_commu, mail,9);
									for(i=0;i<9;i++)
									{
										mail[i]=0;
									}	
									rt_mq_send (mq_commu, mail,9);

                                    //	Key_LED();								 zarien...
                                    break;
#define toString(a) #a
								case CODE_A:
								{
									DBG(toString(CODE_A)"命令识别成功\r\n");
									break;
								}
								case CODE_B:
								{
									DBG(toString(CODE_B)"命令识别成功\r\n");
									break;
								}
								case CODE_C:
								{
									DBG(toString(CODE_C)"命令识别成功\r\n");
									break;
								}
								case CODE_D:
								{
									DBG(toString(CODE_D)"命令识别成功\r\n");
									break;
								}
								case CODE_E:
								{
									DBG(toString(CODE_E)"命令识别成功\r\n");
									break;
								}
								case CODE_F:
								{
									DBG(toString(CODE_F)"命令识别成功\r\n");
									break;
								}
								case CODE_G:
								{
									DBG(toString(CODE_G)"命令识别成功\r\n");
									break;
								}
							
                                default:break;
                            }
                                nAsrStatus = LD_ASR_NONE;
                                break;

        case LD_ASR_FOUNDZERO:
        default:
                            nAsrStatus = LD_ASR_NONE;
                            break;
        }
    if(nAsrStatus!=LD_ASR_RUNING)
        goto loop;
    //	else exe_func= free_func;


}\

#define PrintCom(A,B) printf(B)
#define Delay(A) delay_us2(A)
void end_handle(void);
extern struct sttsound sound[35];
void delay_ms2(u32 ms);
void  LD3320_main_Init(void)
{
	nAsrStatus=0;
	LD3320_Init();
    end_loop=0;
	LD_reset();
	LD_reset();
    rt_thread_delay(100);
	
	nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR
	//printf("LDInit_finished\r\n");

    LD_reset();
    rt_thread_delay(60);
	//sound_play(28);
//	delay_ms(600);
//	sound_play(28);
	//delay_ms(600);
//	exe_func=body_stop;
//		sound_play(14);
//	delay_ms(600);
//	SPIx_Init();
//	//while(1);
//	for(i=0;i<sound[3].lenth;i++)
//	{
//		char *temp;
//		SPI_Flash_Read(temp,i,1);
//		printf("%d ",*temp);
//	}
	
         

//while(nAsrStatus!=LD_ASR_RUNING)

	



	
}
/***********************************************************
* 名    称：LD3320_Init(void)
* 功    能：模块驱动端口初始配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_Init(void)
{
	LD3320_GPIO_Cfg();	
	LD3320_EXTI_Cfg();
    //LD3320_Spi_cfg();
	//LED_gpio_cfg();
    //sound_Init();
	LD_reset();
}

/***********************************************************
* 名    称： void Delay_( int i)
* 功    能： 短延时
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void Delay_( int i)
 {     
    while( i--)
       {	

        }
 }	
/***********************************************************
* 名    称：	LD3320_delay(unsigned long uldata)
* 功    能：	长延时函数
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
 void  LD3320_delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	for (j=0;j<50;j++)
	{
		for (g=0;g<uldata;g++)
		{
			Delay_(30);
		}
	}
}

/***********************************************************
* 名    称：	RunASR(void)
* 功    能：	运行ASR
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;

    rt_thread_delay(2);
	DBG("\r\nstart_init");
	for (i=0; i<5; i++)			//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD_AsrStart();			//初始化ASR
		rt_thread_delay(3);
		if (LD_AsrAddFixed()==0)	//添加关键词语到LD3320芯片中
		{
			DBG("\r\nerror0");
			LD_reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
		//	LD3320_delay(10);			//	并从初始化开始重新ASR识别流程
			continue;
		}

		LD3320_delay(10);
		
		if (LD_AsrRun() == 0)
		{
			DBG("\r\nerror1");
			LD_reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			rt_thread_delay(10);			//	并从初始化开始重新ASR识别流程
			continue;
		}

		asrflag=1;
		break;					//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
	}

	return asrflag;
}

/***********************************************************
* 名    称：LD3320_GPIO_Cfg(void)
* 功    能：初始化需要用到的IO口
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
*
*
**********************************************************/ 
extern rt_sem_t sem_ld3320;
void LD3320_GPIO_Cfg(void)
{	
    SCPE(PERIOC);
    SCPE(PERIOA);
    IOConfig(IOCB,PIN1|PIN3,tuiwanshuchu);
    IOConfig(IOCB,PIN2,fukongshuru);
    IOConfig(IOAB,PIN6,fukongshuru);
    LD_CS_H();


}
/***********************************************************
* 名    称：LD3320_Spi_cfg(void)
* 功    能：配置SPI功能和端口初始化
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_Spi_cfg(void)
{
//	SPI_InitTypeDef  SPI_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//  //spi端口配置
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB2Periph_GPIOB,ENABLE);	   //使能SPI2外设时钟
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

//	//P0/P1/P2
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOB,&GPIO_InitStructure);

////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			//spis 片选	WR		 zarien...
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						 zarien...
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						 zarien...
////	GPIO_Init(GPIOB, &GPIO_InitStructure);									 zarien...
//  
//	LD_CS_H();
//	
//	//spi功能配置
//	SPI_Cmd(SPI2, DISABLE);
//	/* SPI2 配置 */
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //全双工
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //主模式
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8位
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						   //时钟极性 空闲状态时，SCK保持低电平
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //时钟相位 数据采样从第一个时钟边沿开始
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //软件产生NSS
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;   //波特率控制 SYSCLK/128
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据高位在前
//	SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC多项式寄存器初始值为7
//	SPI_Init(SPI2, &SPI_InitStructure);
//	/* 使能SPI2 */
//	SPI_Cmd(SPI2, ENABLE);

}
/***********************************************************
* 名    称： LD3320_EXTI_Cfg(void) 
* 功    能： 外部中断功能配置和相关端口配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD3320_EXTI_Cfg(void)
{
//  EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//  GPIO_InitTypeDef GPIO_InitStructure;

//  //中断引脚配置
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//	//外部中断线配置 
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//  EXTI_GenerateSWInterrupt(EXTI_Line11);
//		
//	GPIO_SetBits(GPIOB,GPIO_Pin_11);	 //默认拉高中断引脚

//	EXTI_ClearFlag(EXTI_Line11);
//	EXTI_ClearITPendingBit(EXTI_Line11);
//	//中断嵌套配置
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure)                                                                             ;
}


/***********************************************************
* 名    称：  EXTI1_IRQHandler(void)
* 功    能： 外部中断函数
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 

void EXTI15_10_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line11)!= RESET ) 
	{
 		if(isplaying==0)
		{
			ProcessInt0(); 
		printf("进入中断\r\n");	
		}
		/*text........................*/
		EXTI_ClearFlag(EXTI_Line11);
        EXTI_ClearITPendingBit(EXTI_Line11);
        LD_loop();
		//end_loop=1;
	}
	
}
/***********************************************************
* 名    称：void LED_gpio_cfg(void)
* 功    能：LED端口配置
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 


