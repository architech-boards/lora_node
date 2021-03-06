/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : pwrMan1.c
**     Project     : Lora
**     Processor   : MKL26Z128VLH4
**     Component   : fsl_power_manager
**     Version     : Component 1.3.0, Driver 01.00, CPU db: 3.00.000
**     Repository  : KSDK 1.3.0
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-02-03, 18:10, # CodeGen: 95
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
** @file pwrMan1.c
** @version 01.00
*/         
/*!
**  @addtogroup pwrMan1_module pwrMan1 module documentation
**  @{
*/         
/* pwrMan1. */
#include "Events.h"
#include "pwrMan1.h"
const power_manager_user_config_t pwrMan1_InitConfig0 = {
  .mode = kPowerManagerRun,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig1 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig2 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig3 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig4 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig5 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig6 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig7 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};

const power_manager_user_config_t pwrMan1_InitConfig8 = {
  .mode = kPowerManagerVlps,
  .sleepOnExitValue = false,
  .powerOnResetDetectionValue = kSmcPorEnabled,
  .partialStopOptionValue = kSmcPstopStop,
};


/*! @brief Array of pointers to User configuration structures */
power_manager_user_config_t const * powerConfigsArr[] = {
    &pwrMan1_InitConfig0,
    &pwrMan1_InitConfig1,
    &pwrMan1_InitConfig2,
    &pwrMan1_InitConfig3,
    &pwrMan1_InitConfig4,
    &pwrMan1_InitConfig5,
    &pwrMan1_InitConfig6,
    &pwrMan1_InitConfig7,
    &pwrMan1_InitConfig8
};
/*! @brief Array of pointers to User defined Callbacks configuration structures */
power_manager_callback_user_config_t * powerStaticCallbacksConfigsArr[] = {NULL};



/* END pwrMan1. */
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
