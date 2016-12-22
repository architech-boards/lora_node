/*
 * flash.c
 *
 *  Created on: 05/dic/2016
 *      Author: Roberto
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <string.h>
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "flash1.h"

#include "SSD_FTFx.h"

#define BUFFER_SIZE_BYTE          0x80
#define ONE_KB                    1024
#define LAUNCH_CMD_SIZE           0x100

#define FTFx_REG_BASE             0x40020000
#define P_FLASH_BASE              0x00000000
#define DEBUGENABLE               0x00

// Program flash IFR
#if (FSL_FEATURE_FLASH_IS_FTFE == 1)
#define PFLASH_IFR                0x3C0
#else // FSL_FEATURE_FLASH_IS_FTFL == 1 or FSL_FEATURE_FLASH_IS_FTFA = =1
#define PFLASH_IFR                0xC0
#endif
// Program Flash block information
#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
#define P_BLOCK_NUM             FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT
#define P_SECTOR_SIZE           FSL_FEATURE_FLASH_PFLASH_BLOCK_SECTOR_SIZE
// Data Flash block information
#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS
#define FLEXNVM_SECTOR_SIZE     FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SECTOR_SIZE
#define FLEXNVM_BLOCK_SIZE      FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SIZE
#define FLEXNVM_BLOCK_NUM       FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_COUNT

// Flex Ram block information
#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS
#define EERAM_SIZE              FSL_FEATURE_FLASH_FLEX_RAM_SIZE

extern void flash1_Callback(void);
///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
uint8_t buffer[BUFFER_SIZE_BYTE]; /*! Buffer for program */
uint8_t buffer_r[BUFFER_SIZE_BYTE]; /*! Buffer for program */
pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

// Array to copy __Launch_Command func to RAM.
uint16_t ramFunc[LAUNCH_CMD_SIZE/2];

// Flash Standard Software Driver Structure.
extern FLASH_SSD_CONFIG flash1_InitConfig0;
#if 0
FLASH_SSD_CONFIG flash1_InitConfig0 =
{
    FTFx_REG_BASE,          /*! FTFx control register base */
    P_FLASH_BASE,           /*! Base address of PFlash block */
    P_FLASH_SIZE,           /*! Size of PFlash block */
    FLEXNVM_BASE,           /*! Base address of DFlash block */
    0,                      /*! Size of DFlash block */
    EERAM_BASE,             /*! Base address of EERAM block */
    0,                      /*! Size of EEE block */
    DEBUGENABLE,            /*! Background debug mode enable bit */
    NULL_CALLBACK           /*! Pointer to callback function */
};
#endif
///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Use Standard Software Drivers (SSD) to modify flash.
 *
 * This function uses SSD to demonstrate flash mode:
 *      + Check flash information.
 *      + Erase a sector and verify.
 *      + Program a sector and verify.
 */
int mainFlash(void)
{
    uint32_t result;               /*! Return code from each SSD function */
    uint32_t destAdrss;            /*! Address of the target location */
    uint8_t  securityStatus = 0;   /*! Return protection status */
    uint16_t number;               /*! Number of longword or phrase to be verified */
    uint32_t marginReadLevel;      /*! 0=normal, 1=user, 2=factory - margin read for reading */
    uint32_t i, failAddr;
#if 0
    // Init hardware
    //hardware_init();

    // Setup flash SSD structure for device and initialize variables.
    result = FlashInit(&flash1_InitConfig0);
    if (FTFx_OK != result)
    {
        error_trap();
    }
    // print welcome message
    PRINTF("\r\n Flash Example Start \r\n");
    // Print flash information - PFlash.
    PRINTF("\r\n Flash Information: ");
    PRINTF("\r\n Total Flash Size:\t%d KB, Hex: (0x%x)", (P_FLASH_SIZE/ONE_KB), P_FLASH_SIZE);
    PRINTF("\r\n Flash Sector Size:\t%d KB, Hex: (0x%x) ", (FTFx_PSECTOR_SIZE/ONE_KB), FTFx_PSECTOR_SIZE);
    // Check if DFlash exist on this device.
    if (flash1_InitConfig0.DFlashSize)
    {
        PRINTF("\r\n Data Flash Size:\t%d KB,\tHex: (0x%x)", (int)(flash1_InitConfig0.DFlashSize/ONE_KB), (unsigned int)flash1_InitConfig0.DFlashSize);
        PRINTF("\r\n Data Flash Base Address:\t0x%x", (unsigned int)flash1_InitConfig0.DFlashBase);
    }
    else
    {
      PRINTF("\r\n There is no D-Flash (FlexNVM) on this Device.");
    }
    // Check if FlexMemory exist on this device.
    if (flash1_InitConfig0.EEESize)
    {
        PRINTF("\r\n Enhanced EEPROM (EEE) Block Size:\t%d KB,\tHex: (0x%x)", (int)(flash1_InitConfig0.EEESize/ONE_KB), (unsigned int)flash1_InitConfig0.EEESize);
        PRINTF("\r\n Enhanced EEPROM (EEE) Base Address:\t0x%x", (unsigned int)flash1_InitConfig0.EERAMBase);
    }
    else
    {
      PRINTF("\r\n There is no Enhanced EEPROM (EEE) on this Device.");
    }
#endif
    //result = FlashInit(&flash1_InitConfig0);
    // Check security status.
    result = FlashGetSecurityState(&flash1_InitConfig0, &securityStatus);
    if (FTFx_OK != result)
    {
        error_trap();
    }
    // Print security status.
    switch(securityStatus)
    {
        case FLASH_NOT_SECURE:
            PRINTF("\r\n Flash is UNSECURE!");
            break;
        case FLASH_SECURE_BACKDOOR_ENABLED:
            PRINTF("\r\n Flash is SECURE, BACKDOOR is ENABLED!");
            break;
        case FLASH_SECURE_BACKDOOR_DISABLED:
            PRINTF("\r\n Flash is SECURE, BACKDOOR is DISABLED!");
            break;
        default:
            break;
    }
    PRINTF("\r\n");

    // Debug message for user.
    // Erase several sectors on upper pflash block where there is no code
    // because we are running from Flash.
    PRINTF("\r\n Erase a sector of flash");

    // Set command to RAM.
    g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)ramFunc , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);

    // Erase a sector from destAdrss.
    destAdrss = flash1_InitConfig0.PFlashBase + (flash1_InitConfig0.PFlashSize - 2*FTFx_PSECTOR_SIZE);
    result = FlashEraseSector(&flash1_InitConfig0, destAdrss, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand);
    if (FTFx_OK != result)
    {
        return 0;
    }

    // Verify sector if it's been erased.
    // Number of long to verify.
    number = FTFx_PSECTOR_SIZE/FSL_FEATURE_FLASH_PFLASH_SECTION_CMD_ADDRESS_ALIGMENT;
    for(marginReadLevel = 0; marginReadLevel <=0x2; marginReadLevel++)
    {
        result = FlashVerifySection(&flash1_InitConfig0, destAdrss, number, marginReadLevel, g_FlashLaunchCommand);
        if (FTFx_OK != result)
        {
            return 0;
        }
    }
#if 0
    // Print message for user.
    PRINTF("\r\n Successfully Erased Sector 0x%x -> 0x%x\r\n", (unsigned int)destAdrss, (unsigned int)(destAdrss+FTFx_PSECTOR_SIZE));

    // Print message for user.
    PRINTF("\r\n Program a buffer to a sector of flash ");
#endif

    // Prepare buffer.
    for (i = 0; i < BUFFER_SIZE_BYTE; i++)
    {
        buffer[i] = i;
    }
    destAdrss = flash1_InitConfig0.PFlashBase + (flash1_InitConfig0.PFlashSize - 0x60*FTFx_PSECTOR_SIZE);
    result = FlashProgram(&flash1_InitConfig0, destAdrss, BUFFER_SIZE_BYTE, \
                                   buffer, g_FlashLaunchCommand);
    if (FTFx_OK != result)
    {
        return 0;
    }

    // Program Check user margin levels
    for (marginReadLevel = 1; marginReadLevel <=0x2; marginReadLevel++)
    {
        result = FlashProgramCheck(&flash1_InitConfig0, destAdrss, BUFFER_SIZE_BYTE, buffer, \
                                    &failAddr, marginReadLevel, g_FlashLaunchCommand);
        if (FTFx_OK != result)
        {
            return 0;
        }
    }

    FlashReadOnce( &flash1_InitConfig0, destAdrss, buffer_r, BUFFER_SIZE_BYTE, g_FlashLaunchCommand );
#if 0
    PRINTF("\r\n Successfully Programmed and Verified Location 0x%x -> 0x%x \r\n", (unsigned int)destAdrss, (unsigned int)(destAdrss + BUFFER_SIZE_BYTE));

    // Print finished message.
    PRINTF("\r\n Flash Example End \r\n");
    while(1);
#endif

    return 1;
}

/*
* @brief Gets called when an error occurs.
* Print error message and trap forever.
*/
void error_trap(void)
{
    PRINTF("\r\n\r\n\r\n\t---- HALTED DUE TO FLASH ERROR! ----");
    while (1)
    {}
}


