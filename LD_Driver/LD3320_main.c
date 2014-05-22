/***************************�������ӿƼ����޹�˾****************************
**  �������ƣ�YS-V0.4����ʶ�𿪷������
**	CPU: STM32f103RCT6
**	����8MHZ
**	�����ʣ�9600 bit/S
**	���ײ�Ʒ��Ϣ��YS-V0.4����ʶ�𿪷���
**                http://yuesheng001.taobao.com
**  ���ߣ�zdings
**  ��ϵ��751956552@qq.com
**  �޸����ڣ�2012.4.11
**  ˵���������� �߱�����ʶ�𡢴���ͨ�š����������������ʾ��
***************************�������ӿƼ����޹�˾******************************/
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
/*************�˿���Ϣ********************
 * ����˵��

 * CS   	PB12
 * P2/SDCK  PB13
 * P1/SDO   PB14
 * P0/SDI   PB15
 * IRQ      PB11

 * CLK      PA8
*****************************************/



/************************************************************************************
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
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
* ��    �ƣ� LD3320_
*
* (void)
* ��    �ܣ� ������LD3320�������
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
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
                    if (RunASR()==0)	//	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
                    {
                        nAsrStatus = LD_ASR_ERROR;
                        DBG("init_false");
                    }
                    break;

        case LD_ASR_FOUNDOK:
                             nAsrRes = LD_GetResult( );	//	һ��ASRʶ�����̽�����ȥȡASR���ʶ����
                             DBG("\r\nʶ����:");			 /*text.....*/
                             putchar(nAsrRes+0x30); /*text.....*/
							mail[i]=0; 
                            switch(nAsrRes)		   /*�Խ��ִ����ز���,�ͻ��޸�*/
                            {
								
								//�case CODE_XK:
								#define clear_mail for(i=0;i<9;i++){mail[i]=0;}
 
								case CODE_YMD:
									DBG("��xiaoxiʶ��ɹ�\r\n"); 
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
                                case CODE_QJ:			/*���ǰ����*/
                                    DBG("��ǰ��������ʶ��ɹ�\r\n"); /*text.....*/
                                    //	Glide_LED();		//ʵ����ˮ�ƹ���	zarien...
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
                                case CODE_HT:	 /*������ˡ�*/
                                    DBG("�����ˡ�����ʶ��ɹ�\r\n"); /*text.....*/
//											Flicker_LED();		//ʵ�ֵ���˸����	  zarien...
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

                                case CODE_KNM:		/*������֡�*/
                                    DBG("kannimei����ʶ��ɹ�\r\n"); /*text.....*/
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
									DBG(toString(CODE_A)"����ʶ��ɹ�\r\n");
									break;
								}
								case CODE_B:
								{
									DBG(toString(CODE_B)"����ʶ��ɹ�\r\n");
									break;
								}
								case CODE_C:
								{
									DBG(toString(CODE_C)"����ʶ��ɹ�\r\n");
									break;
								}
								case CODE_D:
								{
									DBG(toString(CODE_D)"����ʶ��ɹ�\r\n");
									break;
								}
								case CODE_E:
								{
									DBG(toString(CODE_E)"����ʶ��ɹ�\r\n");
									break;
								}
								case CODE_F:
								{
									DBG(toString(CODE_F)"����ʶ��ɹ�\r\n");
									break;
								}
								case CODE_G:
								{
									DBG(toString(CODE_G)"����ʶ��ɹ�\r\n");
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
	
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
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
* ��    �ƣ�LD3320_Init(void)
* ��    �ܣ�ģ�������˿ڳ�ʼ����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
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
* ��    �ƣ� void Delay_( int i)
* ��    �ܣ� ����ʱ
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void Delay_( int i)
 {     
    while( i--)
       {	

        }
 }	
/***********************************************************
* ��    �ƣ�	LD3320_delay(unsigned long uldata)
* ��    �ܣ�	����ʱ����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
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
* ��    �ƣ�	RunASR(void)
* ��    �ܣ�	����ASR
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;

    rt_thread_delay(2);
	DBG("\r\nstart_init");
	for (i=0; i<5; i++)			//	��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_AsrStart();			//��ʼ��ASR
		rt_thread_delay(3);
		if (LD_AsrAddFixed()==0)	//��ӹؼ����ﵽLD3320оƬ��
		{
			DBG("\r\nerror0");
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
		//	LD3320_delay(10);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}

		LD3320_delay(10);
		
		if (LD_AsrRun() == 0)
		{
			DBG("\r\nerror1");
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			rt_thread_delay(10);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}

		asrflag=1;
		break;					//	ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}

	return asrflag;
}

/***********************************************************
* ��    �ƣ�LD3320_GPIO_Cfg(void)
* ��    �ܣ���ʼ����Ҫ�õ���IO��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
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
* ��    �ƣ�LD3320_Spi_cfg(void)
* ��    �ܣ�����SPI���ܺͶ˿ڳ�ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD3320_Spi_cfg(void)
{
//	SPI_InitTypeDef  SPI_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//  //spi�˿�����
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB2Periph_GPIOB,ENABLE);	   //ʹ��SPI2����ʱ��
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

//	//P0/P1/P2
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOB,&GPIO_InitStructure);

////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			//spis Ƭѡ	WR		 zarien...
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						 zarien...
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						 zarien...
////	GPIO_Init(GPIOB, &GPIO_InitStructure);									 zarien...
//  
//	LD_CS_H();
//	
//	//spi��������
//	SPI_Cmd(SPI2, DISABLE);
//	/* SPI2 ���� */
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //ȫ˫��
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //��ģʽ
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8λ
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						   //ʱ�Ӽ��� ����״̬ʱ��SCK���ֵ͵�ƽ
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //ʱ����λ ���ݲ����ӵ�һ��ʱ�ӱ��ؿ�ʼ
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //�������NSS
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;   //�����ʿ��� SYSCLK/128
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //���ݸ�λ��ǰ
//	SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC����ʽ�Ĵ�����ʼֵΪ7
//	SPI_Init(SPI2, &SPI_InitStructure);
//	/* ʹ��SPI2 */
//	SPI_Cmd(SPI2, ENABLE);

}
/***********************************************************
* ��    �ƣ� LD3320_EXTI_Cfg(void) 
* ��    �ܣ� �ⲿ�жϹ������ú���ض˿�����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD3320_EXTI_Cfg(void)
{
//  EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//  GPIO_InitTypeDef GPIO_InitStructure;

//  //�ж���������
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//	//�ⲿ�ж������� 
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//  EXTI_GenerateSWInterrupt(EXTI_Line11);
//		
//	GPIO_SetBits(GPIOB,GPIO_Pin_11);	 //Ĭ�������ж�����

//	EXTI_ClearFlag(EXTI_Line11);
//	EXTI_ClearITPendingBit(EXTI_Line11);
//	//�ж�Ƕ������
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure)                                                                             ;
}


/***********************************************************
* ��    �ƣ�  EXTI1_IRQHandler(void)
* ��    �ܣ� �ⲿ�жϺ���
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 

void EXTI15_10_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line11)!= RESET ) 
	{
 		if(isplaying==0)
		{
			ProcessInt0(); 
		printf("�����ж�\r\n");	
		}
		/*text........................*/
		EXTI_ClearFlag(EXTI_Line11);
        EXTI_ClearITPendingBit(EXTI_Line11);
        LD_loop();
		//end_loop=1;
	}
	
}
/***********************************************************
* ��    �ƣ�void LED_gpio_cfg(void)
* ��    �ܣ�LED�˿�����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 


