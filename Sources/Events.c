/* ###################################################################
**     Filename    : Events.c
**     Project     : Lora
**     Processor   : MKL26Z128VLH4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-01-27, 12:00, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : lpTmr1_OnTimerCompare
**     Description : This callback is called when timer interrupt
**     occurs.
**     Parameters  : None
**     Returns : Nothing
** ===================================================================
*/
extern uint8_t wakeup;
unsigned char timer_wait;
void lpTmr1_OnTimerCompare(void)
{
	if( timer_wait )
		timer_wait--;
}

#ifdef i2cCom1_IDX
/*
** ===================================================================
**     Interrupt handler : I2C0_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void I2C0_IRQHandler(void)
{
  I2C_DRV_IRQHandler(i2cCom1_IDX);
  /* Write your code here ... */
}
#else
  /* This IRQ handler is not used by i2cCom1 component. The purpose may be
   * that the component has been removed or disabled. It is recommended to 
   * remove this handler because Processor Expert cannot modify it according to 
   * possible new request (e.g. in case that another component uses this
   * interrupt vector). */
  #warning This IRQ handler is not used by i2cCom1 component.\
           It is recommended to remove this because Processor Expert cannot\
           modify it according to possible new request.
#endif

/*
** ===================================================================
**     Interrupt handler : ADC0_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void ADC0_IRQHandler(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Callback    : lpsciCom1_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The LPSCI instance number.
**       lpsciState - A pointer to the LPSCI driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/
/*
** ===================================================================
**     Interrupt handler : LLWU_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LLWU_IRQHandler(void)
{
  /* Write your code here. For example clear LLWU wake up flags ... */
	POWER_SYS_ClearWakeupPinFlag(kPowerManagerWakeupPin7);
}

/*
** ===================================================================
**     Interrupt handler : PORTC_PORTD_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PORTC_PORTD_IRQHandler(void)
{
  /* Clear interrupt flag.*/
  PORT_HAL_ClearPortIntFlag(PORTC_BASE_PTR);
  /* Write your code here ... */
  wakeup = 1;
}

/*
** ===================================================================
**     Callback    : flash1_Callback
**     Description : Callback function to service the time critical
**     events.
**     Parameters  : None
**     Returns : Nothing
** ===================================================================
*/
void flash1_Callback(void)
{
    /* Write your code here ... */
}
/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
