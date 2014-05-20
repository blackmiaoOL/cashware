/***************************�������ӿƼ����޹�˾****************************
**  �ļ����ƣ�����ģ�� ��д�ļ�
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
u8 LD3320_WrDat(unsigned char dat);
//#include "stm32f10x_spi.h"
/***********************************************************
* ��    �ƣ�static uint8 spi_send_byte(uint8 byte)
* ��    �ܣ� дSPI
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
 uint8 spi_send_byte(uint8 byte)
{
////	PrintCom(USART1," NO\r\n"); /*text.....*/
//	/* ѭ����ⷢ�ͻ������Ƿ��ǿ� */
//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
////	PrintCom(USART1," NO\r\n"); /*text.....*/

//	/*ͨ��SPI2���跢������*/
//	SPI_I2S_SendData(SPI2,byte);

//	/* �ȴ��������ݣ�ѭ�����������ݻ����� */
//	while (SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET);

//	/* ���ض��������� */
//	return SPI_I2S_ReceiveData(SPI2);

return LD3320_WrDat(byte);


}
/***********************************************************
* ��    �ƣ�void LD_WriteReg(uint8 data1,uint8 data2)
* ��    �ܣ� дld3320�Ĵ���
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_WriteReg(uint8 data1,uint8 data2)
{
	LD_CS_L();

//	LD_SPIS_L();	   zarien...

	spi_send_byte(0x04);

	spi_send_byte(data1);

	spi_send_byte(data2);

	LD_CS_H();

}
/***********************************************************
* ��    �ƣ�uint8 LD_ReadReg(uint8 reg_add)
* ��    �ܣ���ld3320�Ĵ���
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_ReadReg(uint8 reg_add)
{
	uint8 i;

	LD_CS_L();

//	LD_SPIS_L();	   zarien...

	spi_send_byte(0x05);

	spi_send_byte(reg_add);

	i=spi_send_byte(0x00);	/*��SPI*/

	LD_CS_H();

	return(i);
}



