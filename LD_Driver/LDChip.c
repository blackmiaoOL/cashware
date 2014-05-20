/***************************乐声电子科技有限公司****************************
**  文件名称：语音模块底层驱动
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
//#include "DEMOSOUND.h"//    zarien...
#include "usart.h"
#include "debug.h"
//#include "flash.h"
//#include "spi.h"
//#include "body.h"

uint32 nMp3StartPos=0; //	 zarien...
uint32 nMp3Size=0;	   //	 zarien...
uint32 nMp3Pos=0;	   //	 zarien...
uint32 nCurMp3Pos=0;   //	 zarien...

uint8 nLD_Mode = LD_MODE_IDLE;		//	用来记录当前是在进行ASR识别还是在播放MP3

//********************************************************************************
uint8 bMp3Play=0;		//	用来记录播放MP3的状态			  zarien...
//uint8 idata ucRegVal;	 //
uint8  ucHighInt;	 //
uint8  ucLowInt;	 //
uint8  ucStatus;	 //
uint8  ucSPVol=15; // MAX=15 MIN=0		//	Speaker喇叭输出的音量
//*********************************************************************************
uint8 ucRegVal;
extern uint8  nAsrStatus;
extern uint8  nDemoFlag;  //zarien...
struct sttsound sound[35];
void sound_Init()
{
	u8 i=0;
sound[0].lenth=7556L;//C:/FFOutput/1.mp3
sound[1].lenth=34305L;//C:/FFOutput/2.mp3
sound[2].lenth=19259L;//C:/FFOutput/3.mp3
sound[3].lenth=22811L;//C:/FFOutput/4(????????????).mp3
sound[4].lenth=12989L;//C:/FFOutput/5.mp3
sound[5].lenth=15079L;//C:/FFOutput/6(??????,???????).mp3
sound[6].lenth=11735L;//C:/FFOutput/7(????).mp3
sound[7].lenth=31588L;//C:/FFOutput/8.mp3
sound[8].lenth=17378L;//C:/FFOutput/9(??????).mp3
sound[9].lenth=11735L;//C:/FFOutput/14(?????).mp3
sound[10].lenth=6720L;//C:/FFOutput/17(????).mp3
sound[11].lenth=12780L;//C:/FFOutput/18.mp3
sound[12].lenth=11526L;//C:/FFOutput/19.mp3
sound[13].lenth=29290L;//C:/FFOutput/20--1.mp3
sound[14].lenth=20304L;//C:/FFOutput/21.mp3
sound[15].lenth=6302L;//C:/FFOutput/??????.mp3
sound[16].lenth=76310L;//C:/FFOutput/???.mp3
sound[17].lenth=169097L;//C:/FFOutput/?? ddd.mp3
sound[18].lenth=11526L;//C:/FFOutput/???????.mp3
sound[19].lenth=7347L;//C:/FFOutput/???????.mp3
sound[20].lenth=11735L;//C:/FFOutput/???????.mp3
sound[21].lenth=140676L;//C:/FFOutput/??ccc.mp3
sound[22].lenth=61055L;//C:/FFOutput/????.mp3
sound[23].lenth=12780L;//C:/FFOutput/???????.mp3
sound[24].lenth=3167;//C:/FFOutput/aaa.mp3
sound[25].lenth=5884;//C:/FFOutput/aaaay.mp3
sound[26].lenth=8810;//C:/FFOutput/hei ya hu.mp3
sound[27].lenth=30586;//C:/FFOutput/????.mp3
sound[28].lenth=5466;//C:/FFOutput/??.mp3
sound[29].lenth=9228;//C:/FFOutput/???.mp3
sound[30].lenth=9646;//C:/FFOutput/??,???.mp3
sound[31].lenth=7765;//C:/FFOutput/??.mp3
sound[32].lenth=12153;//C:/FFOutput/??.mp3
sound[33].lenth=7556;//C:/FFOutput/1.mp3LDInit_finished
sound[34].lenth=127092;//C:/FFOutput/Psy??? - Style - ?? Dj 00_02_26-00_02_57.mp3
//	sound[0].lenth=7556;//who is he
//	sound[1].lenth=34305;//nihaodehuidayu
//	sound[2].lenth=18005;//niyoushenmebenling
//	sound[3].lenth=76310L;//kaijiyu
//	sound[4].lenth=14034;//xianghewozuogepnegyouma
//	sound[5].lenth=12780;//jianggegushi
//	sound[6].lenth=43082;
	sound[0].addr_start=0;
	for(i=1;i<35;i++)
	{
		sound[i].addr_start=sound[i-1].addr_start+sound[i-1].lenth;
	}
	
}
extern u8 isplaying;
//void sound_play(u8 index)
//{
//	u8 i=index;
//	isplaying=1;
//	
//	LD_reset();
//		LD_reset();
//index=24;
//nDemoFlag = 1;
//nMp3StartPos = 0;
//	nMp3Size =sound[index].lenth;
////	gesture(index,0);
//LD_Init_MP3();
//LD_AdjustMIX2SPVolume(1);
//LD_play(index);
//delay_ms(300);
//	
//	index=i;
//	nDemoFlag = 1;
//	nMp3StartPos = 0;
//		nMp3Size =sound[index].lenth;
////		gesture(index,0);
//	LD_Init_MP3();
//	LD_AdjustMIX2SPVolume(7);
//	LD_play(index);
//	
//	
//	
//	delay_ms(600);
//		nAsrStatus = LD_ASR_NONE;
//		LD3320_Init();
//	isplaying=0;
//}

void  delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	for (j=0;j<5;j++)
	{
		for (g=0;g<uldata;g++)
		{

		}
	}
}
/***********************************************************
* 名    称：void LD_reset(void)
* 功    能：LD芯片硬件初始化
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
#define LD_RST_H() IO1(IOCB,PIN3)
#define LD_RST_L() IO0(IOCB,PIN3)
void LD_reset(void)
{
	LD_RST_H();		//		zarien...
	LD3320_delay(500);	//	zarien...
	LD_RST_L();			//	zarien...
	LD3320_delay(500);//		zarien...
	LD_RST_H();		///		zarien...
	LD3320_delay(500);	//	zarien...
	LD_CS_L();
	LD3320_delay(500);
	LD_CS_H();		
	LD3320_delay(2000);
}

/***********************************************************
* 名    称： void LD_Init_Common(void)
* 功    能： 初始化命令
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD_Init_Common(void)
{

	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	LD3320_delay(5);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);   
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);       
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19); 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);
	LD_WriteReg(0x17, 0x4c); 
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}

//************************************************************
/***********************************************************
* 名    称：void LD_Init_MP3()
* 功    能：初始化MP3
* 入口参数：  
* 出口参数：							zarien...
* 说    明：
* 调用方法： 
**********************************************************/
//*************************************************************
void LD_Init_MP3(void)
{
	nLD_Mode = LD_MODE_MP3;
	LD_Init_Common();

	LD_WriteReg(0xBD,0x02);
	LD_WriteReg(0x17, 0x48);
	delay(10);

	LD_WriteReg(0x85, 0x52); 
	LD_WriteReg(0x8F, 0x00);  
	LD_WriteReg(0x81, 0x00);
	LD_WriteReg(0x83, 0x00);
	LD_WriteReg(0x8E, 0xff);
	LD_WriteReg(0x8D, 0xff);
    delay(1);
	LD_WriteReg(0x87, 0xff);
	LD_WriteReg(0x89, 0xff);
	delay(1);
	LD_WriteReg(0x22, 0x00);    
	LD_WriteReg(0x23, 0x00);
	LD_WriteReg(0x20, 0xef);    
	LD_WriteReg(0x21, 0x07);
	LD_WriteReg(0x24, 0x77);          
    LD_WriteReg(0x25, 0x03);
    LD_WriteReg(0x26, 0xbb);    
    LD_WriteReg(0x27, 0x01); 
}

/***********************************************************
* 名    称：void LD_Init_ASR(void)
* 功    能：初始化ASR
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 	
void LD_Init_ASR(void)
{
	nLD_Mode=LD_MODE_ASR_RUN;
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);	
	LD3320_delay( 5 );
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
	LD3320_delay( 1 );
}

/***********************************************************
* 名    称：void ProcessInt0(void)
* 功    能：识别处理函数
* 入口参数：  
* 出口参数：
* 说    明：可以利用查询方式，或中断方式进行调用
* 调用方法： 
**********************************************************/ 
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);

		// 语音识别产生的中断
			// （有声音输入，不论识别成功或失败都有中断）
		if(nLD_Mode == LD_MODE_ASR_RUN)		 //zaien...添加判断条件
		{
			LD_WriteReg(0x29,0) ;
			LD_WriteReg(0x02,0) ;
	
			if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
				{	 
						nAsrResCount = LD_ReadReg(0xba);
			
						if(nAsrResCount>0 && nAsrResCount<=4) 
						{
							nAsrStatus=LD_ASR_FOUNDOK; 				
						}
						else
						{
							nAsrStatus=LD_ASR_FOUNDZERO;							
						}	
				}
			else{
					nAsrStatus=LD_ASR_FOUNDZERO;	//执行没有识别
				}

		  LD_WriteReg(0x2b, 0);	//中断请求编号=0
		  LD_WriteReg(0x1C,0);/*写0:ADC不可用*/

		  	LD_WriteReg(0x29,0) ;	//FIFO，同步中断=0，不允许
			LD_WriteReg(0x02,0) ;	//FIFO中断允许位=0
			LD_WriteReg(0x2B,  0);	//中断请求编号=0
			LD_WriteReg(0xBA, 0);	 //中断辅助=0
			LD_WriteReg(0xBC,0);	 //ASR,MP3结束控制=0
			LD_WriteReg(0x08,1);	 /*清除FIFO_DATA*/
			LD_WriteReg(0x08,0);	/*清除FIFO_DATA后 再次写0*/
		 }
		
//***********************************************************************		
		// 声音播放产生的中断，有三种：
	// A. 声音数据已全部播放完。
	// B. 声音数据已发送完毕。
	// C. 声音数据暂时将要用完，需要放入新的数据。	
	ucHighInt = LD_ReadReg(0x29); 
	ucLowInt=LD_ReadReg(0x02); 
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
    if(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
    {
	// A. 声音数据已全部播放完。

		LD_WriteReg(0x2B, 0);
      	LD_WriteReg(0xBA, 0);	
		LD_WriteReg(0xBC,0x0);	
		bMp3Play=0;					// 声音数据全部播放完后，修改bMp3Play的变量
		LD_WriteReg(0x08,1);
		delay(5);
      	LD_WriteReg(0x08,0);
		LD_WriteReg(0x33, 0);

		return ;
     }

	 if(nMp3Pos>=nMp3Size)
	{
	// B. 声音数据已发送完毕。

		LD_WriteReg(0xBC, 0x01);
		LD_WriteReg(0x29, 0x10);
//		bMp3Play=0;				//	此时，只是主控MCU把所有MP3数据发送到LD3320芯片内，但是还没有把送入的数据全部播放完毕
//		EX0=1;		 //zarien...

		return;	
	}

	// C. 声音数据暂时将要用完，需要放入新的数据。	

//	LD_ReloadMp3Data_2();		 //zarien...此处是从flash中调用MP3数据重载函数
		
	LD_WriteReg(0x29,ucHighInt); 
	LD_WriteReg(0x02,ucLowInt) ;

//	delay_2(10);	 //zarien...
//	EX0=1;			 //zarien...
			//LD3320_delay(1);

}

//******************************************************************
 /***********************************************************
* 名    称：LD_play()
* 功    能：播放音乐函数
* 入口参数：  
* 出口参数：						   zarien...
* 说    明：
* 调用方法：
**********************************************************/ 
//void LD_play(u8 index)
//{
//	nMp3Pos=0;
//	bMp3Play=1;

//	if (nMp3Pos >=  nMp3Size)
//		return ; 

//	LD_ReloadMp3Data(index);

//    LD_WriteReg(0xBA, 0x00);
//	LD_WriteReg(0x17, 0x48);
//	LD_WriteReg(0x33, 0x01);
//	LD_WriteReg(0x29, 0x04);
//	
//	LD_WriteReg(0x02, 0x01); 
//	LD_WriteReg(0x85, 0x5A);

////	EX0=1;			//zarien...

//}
//******************************************************************
 /***********************************************************
* 名    称：LD_AdjustMIX2SPVolume(uint8 val)
* 功    能：喇叭音量调节函数
* 入口参数：  
* 出口参数：						   zarien...
* 说    明：
* 调用方法：
**********************************************************/
void LD_AdjustMIX2SPVolume(uint8 val)
{
	ucSPVol = val;
	val = ((15-val)&0x0f) << 2;
	LD_WriteReg(0x8E, val | 0xc3); 
	LD_WriteReg(0x87, 0x78); 
}
//******************************************************************
 /***********************************************************
* 名    称：void LD_ReloadMp3Data()
* 功    能：重载MP3数据
* 入口参数：  
* 出口参数：						   zarien...
* 说    明：
* 调用方法：
**********************************************************/
//static 	u8 temp[1000];
//void LD_ReloadMp3Data(u8 index)
//{
////	uint32 nCurMp3Pos;
//	uint8 val;
//	u32 count=0;

//	u32 lenth=0;
//	u32 start_addr=0;
//	lenth=sound[index].lenth;
//	start_addr=sound[index].addr_start;
//	if (nDemoFlag)
//	{
//		DBG("play_start\r\n");
//		ucStatus = LD_ReadReg(0x06);
//		printf("ucStatus=%d\r\n",ucStatus);
//		printf("lenth=%d\r\n",lenth);
//		printf("start_addr=%d\r\n",start_addr);
//		while ( !(ucStatus&MASK_FIFO_STATUS_AFULL) && (nMp3Pos<lenth) )
//		{
//			if(count%400==0)
//			{
//				//SPIx_Init();
//				SPI_Flash_Read(temp,start_addr+(count),400);
//			//	printf("sec_test=%d\r\n",temp[0]);				
//				SPI2->CR1&=~(1<<6); 
//				SPI2->CR1&=~(1<<1); 		//空闲模式下SCK为1 CPOL=1  //no
//				SPI2->CR1&=~(1<<0); 		//数据采样从第二个时间边沿开始,CPHA=1  //no
//				SPI2->CR1|=1<<6; 		//SPI设备使能	
//				if(index==34)gesture(index,count/400+1);
//			}
//			
//			val = temp[count%400];
//			count++;
//			if(index==6)gesture(index,count);
//			
//			nMp3Pos++;
//			LD_WriteReg(0x01,val);
//		//	printf("%d ",val);
////			if (nMp3Pos == lenth)
////				nMp3Pos = 0;

//			ucStatus = LD_ReadReg(0x06);
//			delay_us(125);
//		}
//		printf("play_end---%d\r\n",nMp3Pos);
//		
//	}

//}
/***********************************************************
* 名    称：uint8 LD_Check_ASRBusyFlag_b2(void)
* 功    能：检测 ASR 是否忙状态
* 入口参数：flag ---1：空闲状态  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 LD_Check_ASRBusyFlag_b2(void)
{
	uint8 j;
	uint8 flag = 0;
	char mm[2];
	mm[1]=0;
	
	for (j=0; j<10; j++)
	{

		mm[0]=LD_ReadReg(0xb2);

		//DBG("MM=%d",mm[0]);
		if (mm[0] == 0x21)
		{
			flag = 1;
			break;
		}
		LD3320_delay(10);		
	}
	return flag;
}
/***********************************************************
* 名    称： void LD_AsrStart(void)
* 功    能：
* 入口参数：  ASR初始化
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
void LD_AsrStart(void)
{
	LD_Init_ASR();
}
/***********************************************************
* 名    称： uint8 LD_AsrRun(void)
* 功    能： ASR执行函数
* 入口参数：  
* 出口参数：
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 LD_AsrRun(void)
{
	nAsrStatus=LD_ASR_RUNING;//zarien 12.10
	LD_WriteReg(0x35, MIC_VOL);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);
	LD_WriteReg(0x37, 0x06);
	LD3320_delay( 5);
	LD_WriteReg(0x1C, 0x0b);
	LD_WriteReg(0x29, 0x10);
	
	LD_WriteReg(0xBD, 0x00);   
	return 1;
}

/***********************************************************
* 名    称：uint8 LD_AsrAddFixed(void)
* 功    能：添加识别关键词语
* 入口参数：  
* 出口参数： flag-- 1：success
* 说    明：开发者可以学习"语音识别芯片LD3320高阶秘籍.pdf"中
						关于垃圾词语吸收错误的用法，提高识别率。
* 调用方法： 
**********************************************************/ 
//uint8 LD_AsrAddFixed(void)
//{
//	uint8 k, flag;
//	uint8 nAsrAddLength;
//	#define DATE_A 4    /*数组二维数值*/
//	#define DATE_B 20		/*数组一维数值*/
//	 uint8  sRecog[DATE_A][DATE_B] = {
//	 									"qian jin",\
//										"hou tui",\
//										"ju shou",\
//									    "tai tui"\
//									};	/*添加关键词，用户修改*/
//	 uint8  pCode[DATE_A] = {
//	 									CODE_QJ,\
//										CODE_HT,\
//									    CODE_JS,\
//										CODE_TT\
//							};	/*添加识别码，用户修改*/
//	flag = 1;
//	for (k=0; k<DATE_A; k++)
//	{			
//		if(LD_Check_ASRBusyFlag_b2() == 0)
//		{
//			flag = 0;
//			break;
//		}
//		
//		LD_WriteReg(0xc1, pCode[k] );
//		LD_WriteReg(0xc3, 0 );
//		LD_WriteReg(0x08, 0x04);
//		LD3320_delay(1);
//		LD_WriteReg(0x08, 0x00);
//		LD3320_delay(1);

//		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
//		{
//			if (sRecog[k][nAsrAddLength] == 0)
//				break;
//			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
//		}
//		LD_WriteReg(0xb9, nAsrAddLength);
//		LD_WriteReg(0xb2, 0xff);
//		LD_WriteReg(0x37, 0x04);
////		LD_WriteReg(0x37, 0x04);
//	}	 

//    return flag;
//}
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 14       /*数组二维数值*/
	#define DATE_B 21		/*数组一维数值*/
	 uint8  sRecog[DATE_A][DATE_B] = {
	 									"fan ye",\
										"shang ke",\
										"kan ni mei",\
									    "xiao xi",\
										"pu run te",\
										"bi li",\
										"se ba",\
									    "di ya",\
										"yi na",\
										"ai fu",\
										"ji lo",\
										"ei chi",\
										"ai bi",\
										"zhi mi",\
//										"kei",\
//									    "lai ao",\
//										"ai mu",\
//										"en",\
//										"ou",\
//										"pi",\
//										"kiu",\
//										"a",\
//										"ai si",\
//										"ti" \

									};	/*添加关键词，用户修改*/
	 uint8  pCode[DATE_A] = {
										 CODE_QJ,\
										 CODE_HT,\
										 CODE_KNM,\
										 CODE_YMD,\
										 CODE_A,\
										 CODE_B,\
										 CODE_C,\
										 CODE_D,\
										 CODE_E,\
										 CODE_F,\
										 CODE_G,\
										 CODE_H,\
										 CODE_I,\
										 CODE_J,\
//										 CODE_K,\
//										 CODE_L,\
//										 CODE_M,\
//										 CODE_N,\
//										 CODE_O,\
//										 CODE_P,\
//										 CODE_Q,\
//										 CODE_R,\
//										 CODE_S,\
//										 CODE_T \
										 										 									
							};	/*添加识别码，用户修改*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k] );
		LD_WriteReg(0xc3, 0 );
		LD_WriteReg(0x08, 0x04);
		LD3320_delay(1);		  
		LD_WriteReg(0x08, 0x00);
		LD3320_delay(1);		  

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);
//		LD_WriteReg(0x37, 0x04);	//zarien 12.10
	}	 

    return flag;
}

/***********************************************************
* 名    称： uint8 LD_GetResult(void)
* 功    能： 获取识别结果
* 入口参数：  
* 出口参数： 返回识别码 LD_ReadReg(0xc5 );
* 说    明：
* 调用方法： 
**********************************************************/ 
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5 );
}



