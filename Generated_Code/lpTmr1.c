/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : lpTmr1.c
**     Project     : Lora
**     Processor   : MKL26Z128VLH4
**     Component   : fsl_lptmr
**     Version     : Component 1.3.0, Driver 01.00, CPU db: 3.00.000
**     Repository  : KSDK 1.3.0
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-02-02, 13:13, # CodeGen: 75
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file lpTmr1.c
** @version 01.00
*/         
/*!
**  @addtogroup lpTmr1_module lpTmr1 module documentation
**  @{
*/         

/* MODULE lpTmr1. */

#include "Events.h"
#include "lpTmr1.h"

const lptmr_user_config_t lpTmr1_lptmrCfg0 = {
  .timerMode = kLptmrTimerModeTimeCounter,
  .pinSelect = kLptmrPinSelectInput0,
  .pinPolarity = kLptmrPinPolarityActiveHigh,
  .freeRunningEnable = false,
  .prescalerEnable = true,
  .prescalerClockSource = kClockLptmrSrcLpoClk,
  .prescalerValue = kLptmrPrescalerDivide2,
  .isInterruptEnabled = true,
};




/* Driver state structure */
lptmr_state_t lpTmr1_State;

/******************************************************************************
 * Code
 *****************************************************************************/
/* LPTMR IRQ handler */
void LPTMR0_IRQHandler(void)
{
    LPTMR_DRV_IRQHandler(0U);
}

/* END lpTmr1. */

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
