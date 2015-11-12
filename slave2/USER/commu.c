#include "commu.h"
//#include "exti.h"
#include "usart.h"
#include "w25q16.h"
#include "stm32lib.h"




u8 commu_buf[514];

/************************config***********************************/
#define SPI_COMMU_CS_H do{IOout(COMMU_BASE,COMMU_PIN,1);}while(0);   	//选中FLASH	
#define SPI_COMMU_CS_L do{IOout(COMMU_BASE,COMMU_PIN,0);IOout(FLASH_BASE,FLASH_PIN,1);}while(0); 	//选中FLASH	
/************************config end***********************************/
void commu_write(char *buf,u32 len){
	SPI_COMMU_CS_L;
	for(int i=0;i<len;i++){
		printf("got %x \r\n",SPIx_ReadWriteByte(buf[i]));
	}
	
	SPI_COMMU_CS_H;
}
u8 *commu_read(u32 *len){
	printf("init ");
	while(!IOin(IRQ_BASE,IRQ_PIN));
	printf("stage1");
	u32 cnt=0;
	while(IOin(IRQ_BASE,IRQ_PIN)){
		commu_buf[cnt++]=SPIx_ReadWriteByte(0xa8);
		if(cnt==514)
			cnt=513;
		printf("data%X %x",commu_buf[cnt-1],cnt);		
		
	}	
	printf("state2");
	*len=cnt;
	return commu_buf;
}
void commu_init()
{
	SPIx_Init();
	SPIx_SetSpeed(SPI_SPEED_256);
	SCPE(PERIOA);
	IOConfig(COMMU_BASE,COMMU_PIN>7,1<<(COMMU_PIN%8*4),3);
	IOout(COMMU_BASE,COMMU_PIN,1);
	IOConfig(IRQ_BASE,IRQ_PIN>7,1<<(IRQ_PIN%8*4),8);
	IOout(IRQ_BASE,IRQ_PIN,0);
	
}


void USART1_IRQHandler(void)
{
//	u8 res;	
//	
	if(USART1->SR&(1<<5));//接收到数据
//	{	 
//		res=USART1->DR; 
//  	
//		if(commu_flag==2)
//		{
//			if(res=='s')
//			{
//				commu_flag=0;
//			}
//		}
//		else 
//		{
//			if(commu_lenth==8)
//			{
//				commu_buf[commu_lenth++]=res;
//				commu_flag=1;
//			}
//			else
//			{
//				commu_buf[commu_lenth++]=res;
//			}
//			
//			
//		}
//	}
	//putchar('s');
} 

u8 SD_ReadDisk(u8* buf,u32 sector,u32 cnt)		//读块  512
{
	SPI_Flash_Read(buf,sector*512,cnt*512);
	return 0;
}
u8 SD_WriteDisk(u8*buf,u32 sector,u32 cnt)		//写块   512 
{
	SPI_Flash_Write(buf,sector*512,cnt*512);
	return 0;
}

