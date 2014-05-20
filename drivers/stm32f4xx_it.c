///**
//  ******************************************************************************
//  * @file    IO_Toggle/stm32f4xx_it.c
//  * @author  MCD Application Team
//  * @version V1.0.0
//  * @date    19-September-2011
//  * @brief   Main Interrupt Service Routines.
//  *          This file provides template for all exceptions handler and
//  *          peripherals interrupt service routine.
//  ******************************************************************************
//  * @attention
//  *
//  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
//  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
//  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
//  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
//  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
//  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
//  *
//  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
//  ******************************************************************************
//  */

///* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
//#include <rtthread.h>
//#include "board.h"

///** @addtogroup STM32F4_Discovery_Peripheral_Examples
//  * @{
//  */

///** @addtogroup IO_Toggle
//  * @{
//  */

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
///* Private macro -------------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/
///* Private function prototypes -----------------------------------------------*/
///* Private functions ---------------------------------------------------------*/

///******************************************************************************/
///*            Cortex-M4 Processor Exceptions Handlers                         */
///******************************************************************************/

///**
//  * @brief   This function handles NMI exception.
//  * @param  None
//  * @retval None
//  */
//void NMI_Handler(void)
//{
//}

///**
//  * @brief  This function handles Hard Fault exception.
//  * @param  None
//  * @retval None
//  */
////void HardFault_Handler(void)
////{
////    // definition in libcpu/arm/cortex-m4/context_*.S
////}

///**
//  * @brief  This function handles Memory Manage exception.
//  * @param  None
//  * @retval None
//  */
//void MemManage_Handler(void)
//{
//  /* Go to infinite loop when Memory Manage exception occurs */
//  while (1)
//  {
//  }
//}

///**
//  * @brief  This function handles Bus Fault exception.
//  * @param  None
//  * @retval None
//  */
//void BusFault_Handler(void)
//{
//  /* Go to infinite loop when Bus Fault exception occurs */
//  while (1)
//  {
//  }
//}

///**
//  * @brief  This function handles Usage Fault exception.
//  * @param  None
//  * @retval None
//  */
//void UsageFault_Handler(void)
//{
//  /* Go to infinite loop when Usage Fault exception occurs */
//  while (1)
//  {
//  }
//}

///**
//  * @brief  This function handles SVCall exception.
//  * @param  None
//  * @retval None
//  */
//void SVC_Handler(void)
//{
//}

///**
//  * @brief  This function handles Debug Monitor exception.
//  * @param  None
//  * @retval None
//  */
//void DebugMon_Handler(void)
//{
//}

///**
//  * @brief  This function handles PendSVC exception.
//  * @param  None
//  * @retval None
//  */
////void PendSV_Handler(void)
////{
////    // definition in libcpu/arm/cortex-m4/context_*.S
////}

///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
////void SysTick_Handler(void)
////{
////    // definition in boarc.c
////}

///******************************************************************************/
///*                 STM32F4xx Peripherals Interrupt Handlers                   */
///*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
///*  available peripheral interrupt handler's name please refer to the startup */
///*  file (startup_stm32f4xx.s).                                               */
///******************************************************************************/

///**
//  * @brief  This function handles PPP interrupt request.
//  * @param  None
//  * @retval None
//  */
///*void PPP_IRQHandler(void)
//{
//}*/

//void USART1_IRQHandler(void)
//{
//#ifdef RT_USING_UART1
//    extern struct rt_device uart1_device;
//	extern void rt_hw_serial_isr(struct rt_device *device);

//    /* enter interrupt */
//    rt_interrupt_enter();

//    rt_hw_serial_isr(&uart1_device);

//    /* leave interrupt */
//    rt_interrupt_leave();
//#endif
//}

//void USART2_IRQHandler(void)
//{
//#ifdef RT_USING_UART2
//    extern struct rt_device uart2_device;
//	extern void rt_hw_serial_isr(struct rt_device *device);

//    /* enter interrupt */
//    rt_interrupt_enter();

//    rt_hw_serial_isr(&uart2_device);

//    /* leave interrupt */
//    rt_interrupt_leave();
//#endif
//}

//void USART3_IRQHandler(void)
//{
//#ifdef RT_USING_UART3
//    extern struct rt_device uart3_device;
//	extern void rt_hw_serial_isr(struct rt_device *device);

//    /* enter interrupt */
//    rt_interrupt_enter();

//    rt_hw_serial_isr(&uart3_device);

//    /* leave interrupt */
//    rt_interrupt_leave();
//#endif
//}

///**
//  * @}
//  */

///**
//  * @}
//  */
//  #include "usb_bsp.h"
//#include "usb_hcd_int.h"
//#include "usbh_core.h"
//  extern USB_OTG_CORE_HANDLE          USB_OTG_Core_dev;
//extern USBH_HOST                    USB_Host;

//void OTG_FS_IRQHandler(void) {

//	
//  USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
//}
///******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/





















/**
*****************************************************************************
**
**  File        : stm32f4xx_it.c
**
**  Abstract    : Main Interrupt Service Routines.
**                This file provides template for all exceptions handler and
**                peripherals interrupt service routine.
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
**
*****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "stm32f4xx_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern USB_OTG_CORE_HANDLE          USB_OTG_Core_dev;
extern USBH_HOST                    USB_Host;
/* Private function prototypes -----------------------------------------------*/

extern void TimingDelay_Decrement(void);
extern void USB_OTG_BSP_TimerIRQ (void);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
//void HardFault_Handler(void)
//{
//  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
//}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//    TimingDelay_Decrement();
//}
/**
  * @brief  TIM2_IRQHandler
  *         This function handles Timer2 Handler.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  //USB_OTG_BSP_TimerIRQ(&USB_OTG_Core_dev);
	USB_OTG_BSP_TimerIRQ();
}

/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void) {
#else
void OTG_HS_IRQHandler(void) {
#endif
	
  USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/















