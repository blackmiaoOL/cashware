#include "w25q16.h"
#include "spi.h"

u16 SPI_FLASH_TYPE=W25Q64;//????25Q64

void SPI_Flash_Init(void)
{	

	SCPE(PERIOB);
	IOConfig(IOBB,PIN12,tuiwanshuchu);
	IO1(IOBB,PIN12);
	SPI2_Init();		   	//???SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//???18M??,????
	SPI_FLASH_TYPE=SPI_Flash_ReadID();//??FLASH ID.  
}  

u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_CS_L;                            //????   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);    //???????????    
	byte=SPI2_ReadWriteByte(0Xff);             //??????  
	SPI_FLASH_CS_H;                            //????     
	return byte;   
} 

void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS_L;                            //????   
	SPI2_ReadWriteByte(W25X_WriteStatusReg);   //???????????    
	SPI2_ReadWriteByte(sr);               //??????  
	SPI_FLASH_CS_H;                            //????     	      
}   
//SPI_FLASH???	
//?WEL??   
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_WriteEnable);      //?????  
	SPI_FLASH_CS_H;                            //????     	      
} 
//SPI_FLASH???	
//?WEL??  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_WriteDisable);     //???????    
	SPI_FLASH_CS_H;                            //????     	      
} 			    
//????ID W25X16?ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS_L;				    
	SPI2_ReadWriteByte(0x90);//????ID??	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS_H;				    
	return Temp;
}   		    
//??SPI FLASH  
//????????????????
//pBuffer:?????
//ReadAddr:???????(24bit)
//NumByteToRead:???????(??65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_ReadData);         //??????   
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  //??24bit??    
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //????  
    }
	SPI_FLASH_CS_H;                            //????     	      
}  
//SPI???(0~65535)?????256??????
//???????????256?????
//pBuffer:?????
//WriteAddr:???????(24bit)
//NumByteToWrite:???????(??256),???????????????!!!	 
void SPI_Flash_Write_Page( u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_PageProgram);      //??????   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //??24bit??    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI2_ReadWriteByte(pBuffer[i]);//????  
	SPI_FLASH_CS_H;                            //???? 
	SPI_Flash_Wait_Busy();					   //??????
} 
//????SPI FLASH 
//??????????????????0XFF,????0XFF?????????!
//???????? 
//????????????????,??????????!
//pBuffer:?????
//WriteAddr:???????(24bit)
//NumByteToWrite:???????(??65535)
//CHECK OK
void SPI_Flash_Write_NoCheck( u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //????????		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//???256???
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//?????
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //???????????
			if(NumByteToWrite>256)pageremain=256; //??????256???
			else pageremain=NumByteToWrite; 	  //??256????
		}
	};	    
} 
//?SPI FLASH  
//????????????????
//????????!
//pBuffer:?????
//WriteAddr:???????(24bit)
//NumByteToWrite:???????(??65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write( u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;//???? 0~511 for w25x16
	secoff=WriteAddr%4096;//???????
	secremain=4096-secoff;//????????   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//???4096???
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//?????????
		for(i=0;i<secremain;i++)//????
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//????  	  
		}
		if(i<secremain)//????
		{
			SPI_Flash_Erase_Sector(secpos);//??????
			for(i=0;i<secremain;i++)	   //??
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//??????  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//???????,??????????. 				   
		if(NumByteToWrite==secremain)break;//?????
		else//?????
		{
			secpos++;//?????1
			secoff=0;//?????0 	 

		   	pBuffer+=secremain;  //????
			WriteAddr+=secremain;//?????	   
		   	NumByteToWrite-=secremain;				//?????
			if(NumByteToWrite>4096)secremain=4096;	//??????????
			else secremain=NumByteToWrite;			//??????????
		}	 
	};	 	 
}
//??????
//??????:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//??????...
void SPI_Flash_Erase_Chip(void)   
{                                             
    SPI_FLASH_Write_Enable();                  //SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_ChipErase);        //???????  
	SPI_FLASH_CS_H;                            //????     	      
	SPI_Flash_Wait_Busy();   				   //????????
}   
//??????
//Dst_Addr:???? 0~511 for w25x16
//???????????:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_SectorErase);      //???????? 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //??24bit??    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS_H;                            //????     	      
    SPI_Flash_Wait_Busy();   				   //??????
}  
//????
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // ??BUSY???
}  
//??????
void SPI_Flash_PowerDown(void)   
{ 
  	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_PowerDown);        //??????  
	SPI_FLASH_CS_H;                            //????     	      
    delay_us(3);                               //??TPD  
}   
//??
void SPI_Flash_WAKEUP(void)   
{  
  	SPI_FLASH_CS_L;                            //????   
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	SPI_FLASH_CS_H;                            //????     	      
    delay_us(3);                               //??TRES1
}   


























