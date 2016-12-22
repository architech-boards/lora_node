/*********************************************************************
 * Lora-node demo by Silica
 * Project     : BAEVTSS002
 * Processor   : MKL26Z128VLH4
 * Tuorial     : http://lora-node.readthedocs.io
 * -------------------------------------------------------------------
 * CHANGELOG
 * Release 1.5c:
 * - fix resetAvnet
 * - added console
 * Release 1.5b:
 * - avnetReset saves the devaddr without adding 8
 * Release 1.5:
 * - the board sends a message of 8 bytes like "0x18, X-acc, Y-acc, Z-acc, 0x48, Ambient light, 0x00, 0x00"
 * Release 1.4:
 * - added menu via console with functions for change values in the RN2483 registers
 * - now the lED1 and LED2 are turned off
 * - now there is only one version of the firmware
 * Release 1.3:
 * - added setDevaddr function
 * - modified loraConfiguration function
 * Release 1.2:
 * - reset is setted only if there is a problem with the RN
 * Release 1.1:
 * - first release completed with deepsleep
 *********************************************************************/
/*!
** @file main.c
** @version 01.01
** @brief
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "lpTmr1.h"
#include "gpio1.h"
#include "i2cCom1.h"
#include "adConv1.h"
#include "uartCom1.h"
#include "pwrMan1.h"
#include "DbgCs1.h"
#include "flash1.h"
#if CPU_INIT_CONFIG
  #include "Init_Config.h"
#endif
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdio.h>
#include <string.h>
#include "drivers.h"

#define TIMEOUT_JOIN 5		// Join tries every 5 seconds
#define TIMEOUT_SEND 30		// Send data every 30 seconds

#define FIRMWARE_VERSION "\r\nBAEVTSS002 firmware version 1.5obj\r\nObjenious Network FR\r\n"

#define SLEEP_1_SEC		1000000
#define SLEEP_5_SEC 	5000000
#define SLEEP_30_SEC	30000000

#define LED5_ON	 0
#define LED5_OFF 1

#define RN2483_ON  1
#define RN2483_OFF 0

#define SUCCESS 0
#define FAILED  1

struct AccelSensor Accel;	// accelerometer
struct MagSensor Mag;		// magnetometer

#define MAX_UART_BUFFER 100
uint8_t uart_buffer[MAX_UART_BUFFER], do_join = 1, sleep, wakeup;
extern unsigned char timer_wait;

extern int mainFlash(void);

static void FormatVoltage(char * str)
{
	char tmpstr[64];
	tmpstr [0] = str[0];
	tmpstr [1] = '.';
	tmpstr [2] = str[1];
	tmpstr [3] = str[2];
	tmpstr [4] = str[3];
	tmpstr [5] = 0;
	strcpy(str,tmpstr);
}

static void formatMacStr(char * str)
{
	char tmpstr[64];
	uint8_t j=0;
	for(uint8_t i=0;i<strlen(str);i+=2)
	{
		tmpstr [j++] = str[i];
		tmpstr [j++] = str[i+1];
		tmpstr [j++] = ':';
	}
	tmpstr[j-1]=0;
	strcpy(str,tmpstr);
}

static void FormatPowerStr(char * str)
{
	switch(str[0])
	{
	case '1':
		sprintf(str, "+14dBm");
		break;
	}
}

/*
 * setTimer - set LPTimer
 */
static void setTimer( uint32_t time )
{
	LPTMR_DRV_Stop( lpTmr1_IDX );
	LPTMR_DRV_SetTimerPeriodUs( lpTmr1_IDX, time );
	LPTMR_DRV_Start( lpTmr1_IDX );
}

/*
 * isOk - return 1 if there is OK chars
 */
static int isOk( uint8_t *s1 )
{
	while( *s1 ) {
		if( (*s1 == 'O' || *s1 == 'o') &&
			(*(s1+1) == 'K' || *(s1+1) == 'k') )
			return 1;
		s1++;
	}
	return 0;
}

static int isStatusError( uint8_t *s1 )
{
	while( *(s1+3) )
	{
		if( *s1 == '0' && *(s1+1) == '0' && *(s1+2) == '0' && *(s1+3) == '1' )
			return 0;
		s1++;
	}
	return 1;
}

/*
 * errorRN: inifinte loop with LED5 blinking every 1 second
 */
void errorRN( void )
{
	while(1) {
		GPIO_DRV_WritePinOutput(J1_15_LED5, LED5_ON );
		setTimer( SLEEP_1_SEC );
		timer_wait = 1;
		while( timer_wait );
		GPIO_DRV_WritePinOutput(J1_15_LED5, LED5_OFF );
		setTimer( SLEEP_1_SEC );
		timer_wait = 1;
		while( timer_wait );
	}
}

/*
 * resetHwRN - Hardware Reset of the RN
 */
static void resetHwRN( void )
{
	GPIO_DRV_WritePinOutput( J2_2_RESET, RN2483_OFF );
	setTimer( SLEEP_1_SEC );
	timer_wait = 1;
	while( timer_wait );
	GPIO_DRV_WritePinOutput( J2_2_RESET, RN2483_ON );
}

/*
 * receiveUart - Data received from RN
 * buffer: buffer where are store data received from RN
 * timeout: timeout in milliseconds
 *
 */
static int receiveUart(  uint8_t *buffer, int timeout )
{
	uint8_t *start = buffer;
	uart_status_t ret;

	*buffer = 0;
	do
	{
		ret = UART_DRV_ReceiveDataBlocking( uartCom1_IDX, buffer, 1, timeout );
		if( ret == kStatus_UART_Success ) {
			if( *buffer == 0x00 )
				ret = kStatus_UART_Timeout;
			buffer++;
		}
	} while( ret != kStatus_UART_Timeout);
	*buffer = 0;

	return (buffer - start);
}

/*
 * turnOffLeds - turn off LED1 & LED2
 */
static void turnOffLeds( void )
{
	UART_DRV_SendDataBlocking( uartCom1_IDX, "sys set pinmode GPIO13 digout\r\n", 31, 1000 );
	receiveUart( uart_buffer, 250 );
	UART_DRV_SendDataBlocking( uartCom1_IDX, "sys set pindig GPIO13 1\r\n", 25, 1000 );
	receiveUart( uart_buffer, 250 );

	UART_DRV_SendDataBlocking( uartCom1_IDX, "sys set pinmode GPIO12 digout\r\n", 31, 1000 );
	receiveUart( uart_buffer, 250 );
	UART_DRV_SendDataBlocking( uartCom1_IDX, "sys set pindig GPIO12 1\r\n", 25, 1000 );
	receiveUart( uart_buffer, 250 );
}

/*
 * getRegister - read data from a register
 * register_name: name of the register to read
 * lenght: how many bytes to read
 * buffer: where to store the data read
 */
static int getRegister( char *register_name, int lenght, char *buffer )
{
	if( (strcmp( register_name, "hweui") == 0) || (strcmp( register_name, "vdd") == 0))
		sprintf( uart_buffer, "sys get %s\r\n", register_name );
	else
		sprintf( uart_buffer, "mac get %s\r\n", register_name );

	if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, strlen(uart_buffer), 1000 ) == kStatus_UART_Success ) {
		if( receiveUart( uart_buffer, 250 ) ) {
			uart_buffer[lenght] = 0;
			strncpy( buffer, uart_buffer, lenght );
			buffer[lenght] = 0;
			return SUCCESS;
		}
	}
	return FAILED;
}

/*
 * setRadioRegister - set radio the register
 * input: data to write
 * register_name: name of the register
 * lenght: value to write
 */
static int setRadioRegister( char *input, char *register_name, int lenght )
{
	if( register_name[0] == 'd' ) {
		getRegister( register_name, lenght, input );
		debug_printf( "Changing %s register, current value: %s\r\n", register_name, input );
	}
	debug_printf( "Enter new %s value, %d hex numbers lenght (press ENTER to abort):\r\n", register_name, lenght );


	if( inputConsole( input, lenght ) == 0 ) {

		sprintf( uart_buffer, "radio set %s %s\r\n", register_name, input );
		if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, strlen(uart_buffer), 1000 ) != kStatus_UART_Success ) {
			debug_printf(" Procedure failed: UART TX error, register not changed\n\r");
			return FAILED;
		}
		receiveUart( uart_buffer, 250 );
		if( !isOk(uart_buffer) ) {
				debug_printf(" Procedure failed: 'radio set' command not accepted, register not changed\n\r");
				return FAILED;
		}
	} else {
		debug_printf(" Procedure aborted\r\n" );
		return FAILED;
	}

	debug_printf(" value changed\r\n" );
	return SUCCESS;
}



/*
 * setRegister - set the register
 * input: data to write
 * register_name: name of the register
 * lenght: value to write
 */
static int setRegister( char *input, char *register_name, int lenght )
{
	if( register_name[0] == 'd' ) {
		getRegister( register_name, lenght, input );
		debug_printf( "Changing %s register, current value: %s\r\n", register_name, input );
	}
	debug_printf( "Enter new %s value, %d hex numbers lenght (press ENTER to abort):\r\n", register_name, lenght );


	if( inputConsole( input, lenght ) == 0 ) {

		sprintf( uart_buffer, "mac set %s %s\r\n", register_name, input );
		if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, strlen(uart_buffer), 1000 ) != kStatus_UART_Success ) {
			debug_printf(" Procedure failed: UART TX error, register not changed\n\r");
			return FAILED;
		}
		receiveUart( uart_buffer, 250 );
		if( !isOk(uart_buffer) ) {
				debug_printf(" Procedure failed: 'mac set' command not accepted, register not changed\n\r");
				return FAILED;
		}
	} else {
		debug_printf(" Procedure aborted\r\n" );
		return FAILED;
	}

	debug_printf(" value changed\r\n" );
	return SUCCESS;
}

/*
 * saveRegisters - it saves the registers modified
 */
static int saveRegisters( void )
{
	debug_printf("Saving modifies, please don't turn off the device..." );
	if( UART_DRV_SendDataBlocking( uartCom1_IDX, "mac save\r\n", 10, 1000 ) != kStatus_UART_Success ) {
		debug_printf(" procedure failed: UART TX error, registers not saved\n\r");
		return FAILED;
	}
	receiveUart( uart_buffer, 5000 );
	if( !isOk(uart_buffer) ) {
		debug_printf(" procedure failed: 'mac save' command not accepted, registers not saved\n\r");
		return FAILED;
	}
	debug_printf(" saved successfully\r\n" );
	return SUCCESS;
}

/*
 * avnetReset - Avnet RN2483 reset
 */
static int avnetReset( void )
{
        char deveui[20];

        debug_printf( "Starting Avnet default reset, please don't turnoff the board\r\n" );

        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "sys factoryRESET\r\n", 18, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 5000 );

        // 1. sys get hweui
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "sys get hweui\r\n", 15, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        strncpy( deveui, uart_buffer, 16 );
        deveui[16] = 0x00;

        // 2. mac set deveui
        sprintf( uart_buffer, "mac set deveui %s\r\n", deveui );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 33, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 3. mac set devaddr
        sprintf( uart_buffer, "mac set devaddr %s\r\n", &deveui[8] );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 26, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 4. mac set appskey
        debug_printf( "mac set appskey AFBECD56473829100192837465FAEBDC\r\n" );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "mac set appskey AFBECD56473829100192837465FAEBDC\r\n", sizeof("mac set appskey AFBECD56473829100192837465FAEBDC\r\n"), 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 5. mac set nwkskey
        sprintf( uart_buffer, "mac set nwkskey %s%s\r\n", deveui, deveui );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 50, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 6. mac save
        sprintf( uart_buffer, "mac save\r\n" );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 10, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 5000 );

        if( !isOk(uart_buffer) )
        	goto error;

        debug_printf( "success\r\n" );
        turnOffLeds();
        return SUCCESS;

        error:
		debug_printf( "failed\r\n" );
		turnOffLeds();
		return FAILED;
}

/*
 * swisscomReset - Swisscom RN2483 reset
 */
static int swisscomReset( void )
{
        char deveui[20], temp[2];

        debug_printf( "Starting Swisscom default reset, please don't turnoff the board\r\n" );

        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "sys factoryRESET\r\n", 18, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 5000 );

        // 1. sys get hweui
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "sys get hweui\r\n", 15, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        strncpy( deveui, uart_buffer, 16 );

        // 2. mac set deveui
        sprintf( uart_buffer, "mac set deveui %s\r\n", deveui );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 33, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 3. mac set devaddr
        temp[0] = deveui[8];
        temp[1] = deveui[9];
        deveui[8] = '0';
        deveui[9] = '8';
        sprintf( uart_buffer, "mac set devaddr %s\r\n", &deveui[8] );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 26, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;
        deveui[8] = temp[0];
		deveui[9] = temp[1];

        // 4. mac set appskey
        debug_printf( "mac set appskey AFBECD56473829100192837465FAEBDC\r\n" );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "mac set appskey AFBECD56473829100192837465FAEBDC\r\n", sizeof("mac set appskey AFBECD56473829100192837465FAEBDC\r\n"), 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 5. mac set nwkskey
        sprintf( uart_buffer, "mac set nwkskey %s%s\r\n", deveui, deveui );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 50, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 6. mac save
        sprintf( uart_buffer, "mac save\r\n" );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 10, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 5000 );

        if( !isOk(uart_buffer) )
        	goto error;

        debug_printf( "success\r\n" );
        turnOffLeds();
        return SUCCESS;

        error:
		debug_printf( "failed\r\n" );
		turnOffLeds();
		return FAILED;
}

/*
 * geniusReset - Genius RN2483 reset
 */
static int geniusReset( void )
{
        char deveui[20], temp[2];

        debug_printf( "Starting Genius default reset, please don't turnoff the board\r\n" );

        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "sys factoryRESET\r\n", 18, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 5000 );

        // 1. sys get hweui
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "sys get hweui\r\n", 15, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        strncpy( deveui, uart_buffer, 16 );

        // 2. mac set deveui
        sprintf( uart_buffer, "mac set deveui %s\r\n", deveui );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 33, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 3. mac set devaddr
        temp[0] = deveui[8];
        temp[1] = deveui[9];
        deveui[8] = '0';
        deveui[9] = '8';
        sprintf( uart_buffer, "mac set devaddr %s\r\n", &deveui[8] );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 26, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;
        deveui[8] = temp[0];
		deveui[9] = temp[1];

        // 4. mac set appskey
        debug_printf( "mac set appskey AFBECD56473829100192837465FAEBDC\r\n" );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, "mac set appskey AFBECD56473829100192837465FAEBDC\r\n", sizeof("mac set appskey AFBECD56473829100192837465FAEBDC\r\n"), 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 5. mac set nwkskey
        sprintf( uart_buffer, "mac set nwkskey %s%s\r\n", deveui, deveui );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 50, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 250 );
        if( !isOk(uart_buffer) )
        	goto error;

        // 6. mac save
        sprintf( uart_buffer, "mac save\r\n" );
        debug_printf( uart_buffer );
        if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 10, 1000 ) != kStatus_UART_Success )
        	goto error;
        receiveUart( uart_buffer, 5000 );

        if( !isOk(uart_buffer) )
        	goto error;

        debug_printf( "success\r\n" );
        turnOffLeds();
        return SUCCESS;

        error:
		debug_printf( "failed\r\n" );
		turnOffLeds();
		return FAILED;
}


/*
 * readLight - read light sensor
 */
static unsigned short readLight( void )
{
	unsigned short read;

	ADC16_DRV_ConfigConvChn(adConv1_IDX, 0U, &adConv1_ChnConfig0);
	GPIO_DRV_WritePinOutput(J1_8_LIGHT, 1);  // LIGHT ON
	ADC16_DRV_WaitConvDone(adConv1_IDX, 0);
	read = ADC16_DRV_GetConvValueRAW(adConv1_IDX, 0);
	GPIO_DRV_WritePinOutput(J1_8_LIGHT, 1);  // LIGHT OFF
	return read;
}

/*
 * deepSleep - it sets vlps mode
 */
static void deepSleep( void )
{
	GPIO_DRV_WritePinOutput(J1_15_LED5, LED5_OFF);
	sleep = wakeup = 0;
	do
	{
		setTimer( SLEEP_30_SEC );
		POWER_SYS_SetMode( kPowerManagerVlps, kPowerManagerPolicyAgreement );
		CLOCK_SYS_UpdateConfiguration(4, kPowerManagerPolicyAgreement);
		POWER_SYS_SetMode( kPowerManagerRun, kPowerManagerPolicyAgreement );
		sleep++;
		if( wakeup )
			break;
	}while(sleep < 4);

	GPIO_DRV_WritePinOutput(J1_15_LED5, LED5_ON);

	if( UART_DRV_SendDataBlocking( uartCom1_IDX, "mac get status\r\n", 16, 1000  ) == kStatus_UART_Success )
	{
		receiveUart( uart_buffer, 250 );
		if( isStatusError( uart_buffer ) )
		{
			resetHwRN();
			receiveUart( uart_buffer, 2000 );
			debug_printf("%s\r\n",uart_buffer);
			turnOffLeds();
			do_join = 1;
		}
	}
}

/*
 * sendMessage - message sent to the server. Max timeout is 70 seconds.
 */
static void sendMessage( void )
{
	char response_msg[10], voltage_str[5];
	//getRegister( "vdd", 5, voltage_str );
	sprintf( (char*)uart_buffer, "mac tx cnf 4 18%02x%02x%02x48%02x0000\r\n", (unsigned char)((Accel.iGpFast[X] * 100L) / 8192L), (unsigned char)((Accel.iGpFast[Y] * 100L) / 8192L), (unsigned char)((Accel.iGpFast[Z] * 100L) / 8192L), (readLight()>>8) );
	debug_printf("Message to send: %s\r\n", uart_buffer );
	UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, 31, 1000  );
	receiveUart( uart_buffer, 1000 );
	debug_printf("%s\r\n",uart_buffer);
}

#define JOIN_MODE_ABP 0
#define JOIN_MODE_OTAA 1


static int ForceDutyCycle( uint8_t channel )
{
	sprintf( (char*)uart_buffer, "mac set ch dcycle %d 0\r\n", channel );
	if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, strlen(uart_buffer), 1000  ) == kStatus_UART_Success )
	{
		receiveUart( uart_buffer, 1000 );

		if( isOk(uart_buffer) )
		{
			debug_printf("Forced Duty Cycle OK on channel %d.\n\r", channel);
			receiveUart( uart_buffer, 10000 );
			debug_printf("%s\r\n",uart_buffer);
			return SUCCESS;
		}
	}
	else
	{
		debug_printf("UART error\r\n");
	}


	return FAILED;
}

static int IsNetworkJoined(void)
{
	char str[4],tmpstr[7];
	uint32_t regval;
	getRegister( "status", 4, str);
	debug_printf("Status Reg: 0x%s\r\n", str);
	sprintf(tmpstr, "0x%s", str);
	regval = strtoul(tmpstr, NULL,0);
	if(regval & 0x00000001 )
	{
		debug_printf("network joined.:)\r\n");
		return SUCCESS;
	}

	debug_printf("network not joined.:(\r\n");

	return FAILED;
}


/*
 * joinNetwork - join network procedure
 */
static int joinNetwork( uint8_t mode )
{
	if(mode == JOIN_MODE_ABP)
		sprintf( (char*)uart_buffer, "mac join abp\r\n" );
	else
		sprintf( (char*)uart_buffer, "mac join otaa\r\n" );
	debug_printf("connection : %s", uart_buffer);
	if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, strlen(uart_buffer), 1000  ) == kStatus_UART_Success )
	{
		receiveUart( uart_buffer, 1000 );
		debug_printf("%s\r\n",uart_buffer);
		if( isOk(uart_buffer) )
		{
			receiveUart( uart_buffer, 10000 );
			debug_printf("connection : %s", uart_buffer);
			if(strcmp(uart_buffer, "denied\r\n" ) == 0)
			{
				return FAILED;
			}
			else
			{
				return SUCCESS;
			}
			// return IsNetworkJoined();


		}

	}
	else
	{
		debug_printf("UART error\r\n");
	}


	return FAILED;
}



static int joinNetworkMenu( uint8_t mode )
{
	if( joinNetwork( mode) == SUCCESS)
	{
		debug_printf("Network Joined.\r\n");
		return SUCCESS;
	}
	else
	{
		debug_printf("Join Failure.\r\n");
		return FAILED;
	}
}

/*
 * console Menu
 */
static void showMenu( void )
{
	debug_printf("*** MENU CONFIG RN2483 ***\n\r");
	debug_printf("Enter 0 to print this menu\n\r");
	debug_printf("Enter 1 to see configuration values\n\r");
	debug_printf("Enter 2 to modify the globally unique device identifier\n\r");
	debug_printf("Enter 3 to modify the module with a 4-byte unique network device address\n\r");
	debug_printf("Enter 4 to modify the application session key\n\r");
	debug_printf("Enter 5 to modify NWKSKEY\n\r");
	debug_printf("Enter 6 to save changes\n\r");
	debug_printf("Enter 7 to Microchip default reset\n\r");
	debug_printf("Enter 8 to Avnet default reset\n\r");
	debug_printf("Enter 9 to exit\n\r");
	debug_printf("Enter a to modify the application identifier\n\r");
	debug_printf("Enter k to modify the application key\n\r");
	debug_printf("Enter f to Force Duty Cycle\n\r");
	debug_printf("Enter j to Join Network by personalization\n\r");
	debug_printf("Enter J to Join Network Over The Air\n\r");
	debug_printf("Enter p to modify the TX power\n\r");
	debug_printf("Enter s to Swisscom default reset\n\r");
	debug_printf("Enter g to Genius default reset\n\r");
	debug_printf("Enter c to send command to the RN2483 manually\n\r");
	debug_printf("Enter m to select ABP or OTEE mode\n\r");
}

/*
 * inputConsole - read data from the serial
 * input: buffer where store data
 * lenght: number of bytes to read
 */
int inputConsole( char *input, int lenght )
{
	int i = 0;

	debug_printf(">> ");

	while( lenght ) {
		input[i] = debug_getchar();
		if( input[i] == '\r' ) {
			debug_printf("\r\n");
			return 1;
		}
//		if( (input[i] < 'A' || input[i] > 'F') && (input[i] < 'a' || input[i] > 'f') && (input[i] < '0' || input[i] > '9') )
//			continue;
		debug_putchar( input[i] );
		i++;
		lenght--;
	}
	input[i] = 0;
	debug_printf("\r\n");

	return 0;
}

/*
 * getRadioRegister - read data from a register
 * register_name: name of the register to read
 * length: how many bytes to read
 * buffer: where to store the data read
 */
static int getRadioRegister( char *register_name, int lenght, char *buffer )
{
	sprintf( uart_buffer, "radio get %s\r\n", register_name );

	if( UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, strlen(uart_buffer), 1000 ) == kStatus_UART_Success ) {
		if( receiveUart( uart_buffer, 250 ) ) {
			uart_buffer[lenght] = 0;
			strncpy( buffer, uart_buffer, lenght );
			buffer[lenght] = 0;
			return SUCCESS;
		}
	}
	return FAILED;
}



/*
 * microchipReset - Microchip factory reset default of the RN2483
 */
static int microchipReset( void )
{
    debug_printf("Performing Microchip default reset, don't turn off the device\r\nplease wait...");
    if( UART_DRV_SendDataBlocking( uartCom1_IDX, "sys factoryRESET\r\n", 18, 1000 ) != kStatus_UART_Success ) {
    	debug_printf("failed\r\n");
    	return FAILED;
    }
    receiveUart( uart_buffer, 5000 );
    turnOffLeds();
    debug_printf("success\r\n");

    return SUCCESS;
}

static void sendCommands2RN2483( void )
{
	char loop = 1, i = 0;

	debug_printf( "Send commands to RN2483. Write 'quit' to exit.\r\n>> " );
	while(1) {
		uart_buffer[i++] = debug_getchar();
		debug_putchar( uart_buffer[i-1] );
		if( uart_buffer[i-1] == '\r' ) {
			uart_buffer[i++] = '\n';
			uart_buffer[i] = 0;
			debug_printf( "\r\n" );

			if( strncmp( uart_buffer, "quit", 4 ) == 0) {
				debug_printf( "quitted\r\n" );
				return;
			}
			UART_DRV_SendDataBlocking( uartCom1_IDX, uart_buffer, i, 100 );
			memset( uart_buffer, 0x00, MAX_UART_BUFFER );
			if( receiveUart( uart_buffer, 250 ) )
				debug_printf( uart_buffer );
			debug_printf( ">> " );
			i = 0;
		}
	}
}

/*
 * loopMenu
 */
static int8_t loopMenu( void )
{
	char input[50];
	uint8_t mode = JOIN_MODE_OTAA;

	showMenu();

	do {
		if( !inputConsole( input, 1 ) ) {
			switch( input[0] ) {
				case '0': {
					showMenu();
				} break;
				case '1': {
					getRegister( "hweui", 16, input );
					debug_printf( "hweui = %s [read only register]\r\n", input );
					getRegister( "deveui", 16, input );
					debug_printf( "deveui = %s\r\n", input );
					getRegister( "appeui", 16, input );
					debug_printf( "appeui = %s\r\n", input );
					getRegister( "devaddr", 8, input );
					debug_printf( "devaddr = %s\r\n", input );
					getRegister( "band", 3, input );
					debug_printf( "Frequency Band: %s MHz\t\t", input );
					getRegister( "pwridx", 1, input );

					debug_printf( "Output Power: %s\r\n", input );
					getRadioRegister( "pwr", 2, input );
					debug_printf( "Radio Output Power: %s\r\n", input );

				} break;
				case '2': {
					setRegister( input, "deveui", 16 );
				} break;
				case '3': {
					setRegister( input, "devaddr", 8 );
				} break;
				case '4': {
					setRegister( input, "appskey", 32 );
				} break;
				case '5': {
					setRegister( input, "nwkskey", 32 );
				} break;
				case '6': {
					saveRegisters();
				} break;
				case '7': {
					microchipReset();
				} break;
				case '8': {
					avnetReset();
				} break;
				case '9': {
					debug_printf("Exit\n\r");
					return mode;
				} break;
				case 'a': {
					setRegister( input, "appeui", 16 );
				} break;
				debug_printf("Enter k to modify the application key\n\r");
				case 'k': {
					setRegister( input, "appkey", 32 );
				} break;
				case 'j': {
					debug_printf("Join Network... activation by personalization in process\r\n");
					joinNetworkMenu( JOIN_MODE_ABP );
				} break;
				case 'p': {
					debug_printf("Output Power \r\n");
					setRadioRegister( input, "pwr", 2 );
				} break;
				case 'f': {
					debug_printf("Force Duty Cycle on all channels...\r\n");
					for(uint8_t i=0;i<16;i++)
					{
						ForceDutyCycle(i);
					}
				}break;
				case 'J': {
					//ForceNetwork();
					debug_printf("Join Network JOIN_MODE_OTAA... over-the-air activation in process\r\n");
					do_join = joinNetworkMenu(JOIN_MODE_OTAA);
				} break;
				case 's': {
					swisscomReset();
				} break;
				case 'g': {
					geniusReset();
				} break;
				case 'c': {
					sendCommands2RN2483();
				} break;
				case 'm': {
					if( mode == JOIN_MODE_OTAA ) {
						mode = JOIN_MODE_ABP;
						debug_printf( "mode ABP selected, tring to join...\n\r" );
						do_join = joinNetworkMenu(JOIN_MODE_ABP);
					} else {
						mode = JOIN_MODE_OTAA;
						debug_printf( "mode OTAA selected, tring to join...\n\r" );
						do_join = joinNetworkMenu(JOIN_MODE_OTAA);
					}
				} break;
				default: {
					debug_printf("Wrong command\n\r");
					showMenu();
				}
			}
		}
	}while( 1 );
}

static void getModuleInfo( void)
{

}

/*
 * mainLoop
 */
static void mainLoop( void )
{
	char input[50];
	uint8_t mode = JOIN_MODE_OTAA;
	GPIO_DRV_WritePinOutput(J1_15_LED5, LED5_OFF );

	while( FXOS8700_Init( &Accel, &Mag ) != kStatus_I2C_Success );

	resetHwRN();
	if( receiveUart( uart_buffer, 2000 ) != 35 )
		errorRN();

	debug_printf(FIRMWARE_VERSION);
	debug_printf("Module Version : %s - ONLY FOR TEST\r\n", uart_buffer);
	if( strncmp( &uart_buffer[7], "1.0.1", 5 ) ) {
		debug_printf("*** Warning: this software is developed for RN2483 firmware version 1.0.1 ***\n\r");
	}

	getRegister( "hweui", 16, input);
	formatMacStr(input);
	debug_printf( "EUI node address: %s \r\n", input);
	getRegister( "deveui", 16, input );
	formatMacStr(input);
	debug_printf( "device EUI:  %s\r\n", input );
	getRegister( "devaddr", 8, input );
	debug_printf( "device address: %s\r\n", input );
	getRegister( "band", 3, input );
	debug_printf( "Frequency Band: %s MHz\t\t", input );
	getRegister( "pwridx", 1, input );
	FormatPowerStr(input);
	debug_printf( "Output Power: %s\r\n", input );
	getRegister( "vdd", 4, input );
	FormatVoltage(input);
	debug_printf( "Module Voltage: %sV\r\n",input);
/*
	getRegister( "ch dcycle 0", 5, input );
	if(input[0] != '0' && input[1] != '\r')
	{
		for(uint8_t i=0;i<16;i++)
		{
			ForceDutyCycle(i);
		}
	}
*/


	if (GPIO_DRV_ReadPinInput( BUTTON_SW1 ) == 0 ) {
		debug_printf("Please release the button S2\n\r");
		while( GPIO_DRV_ReadPinInput( BUTTON_SW1 ) == 0);
		mode = loopMenu();
	}

	do_join = IsNetworkJoined();
	turnOffLeds();


	//CLOCK_SYS_UpdateConfiguration( 4, kPowerManagerPolicyAgreement );
	//POWER_SYS_SetMode( kPowerManagerRun, kPowerManagerPolicyAgreement );
	//mainFlash();
	for(;;) {

		GPIO_DRV_WritePinOutput(J1_15_LED5, LED5_OFF );
		//CLOCK_SYS_UpdateConfiguration( 4, kPowerManagerPolicyAgreement );
		//POWER_SYS_SetMode( kPowerManagerRun, kPowerManagerPolicyAgreement );
		//deepSleep();

		GPIO_DRV_WritePinOutput(J1_15_LED5, LED5_ON );
		readLight();
		if( FXOS8700_ReadData( &Accel, &Mag ) == kStatus_I2C_Success )
		{
			debug_printf( "Press S2 in order to try to send a message \r\n");
			while( GPIO_DRV_ReadPinInput( BUTTON_SW1 ) == 1);

			if( do_join )
			{
				debug_printf( "Join \r\n");
				do_join = joinNetworkMenu( mode );
			}
			if( do_join == 0 )
			{
				debug_printf( "Send message\r\n");
				sendMessage();
			}
		}
	}
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{

	/* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  CLOCK_SYS_Init(g_clockManConfigsArr, 5, g_clockManCallbacksArr, FSL_CLOCK_MANAGER_CALLBACK_CNT );
  mainLoop();
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
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
