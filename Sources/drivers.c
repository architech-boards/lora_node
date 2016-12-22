/*
 * drivers.c
 *
 *  Created on: Sep 14, 2015
 *      Author: Owner
 */
#include "drivers.h"
#include "i2cCom1.h"
#include "fsl_i2c_hal.h"
#include "fsl_i2c_master_driver.h"

static uint8 I2C_Buf[I2C_BUF_LEN];

// initialize FXOS8700 accelerometer plus magnetometer sensor
i2c_status_t FXOS8700_Init( struct AccelSensor *pthisAccel, struct MagSensor *pthisMag )
{
	I2C_Buf[0] = FXOS8700_CTRL_REG1;
	I2C_Buf[1] = 0x00;

	if( I2C_DRV_MasterSendDataBlocking( i2cCom1_IDX, &i2cCom1_MasterConfig0, &I2C_Buf[0], 1, &I2C_Buf[1], 1, 1000 ) != kStatus_I2C_Success )
		return kStatus_I2C_Fail;

	I2C_Buf[0] = FXOS8700_M_CTRL_REG1;
	I2C_Buf[1] = 0x1F;
	if( I2C_DRV_MasterSendDataBlocking( i2cCom1_IDX, &i2cCom1_MasterConfig0, &I2C_Buf[0], 1, &I2C_Buf[1], 1, 1000) != kStatus_I2C_Success )
		return kStatus_I2C_Fail;

	I2C_Buf[0] = FXOS8700_M_CTRL_REG2;
	I2C_Buf[1] = 0x20;
	if( I2C_DRV_MasterSendDataBlocking( i2cCom1_IDX, &i2cCom1_MasterConfig0, &I2C_Buf[0], 1, &I2C_Buf[1], 1, 1000 ) != kStatus_I2C_Success )
		return kStatus_I2C_Fail;

	I2C_Buf[0] = FXOS8700_XYZ_DATA_CFG;
	I2C_Buf[1] = 0x01;
	if( I2C_DRV_MasterSendDataBlocking( i2cCom1_IDX, &i2cCom1_MasterConfig0, &I2C_Buf[0], 1, &I2C_Buf[1], 1, 1000 ) != kStatus_I2C_Success )
		return kStatus_I2C_Fail;

	I2C_Buf[0] = FXOS8700_CTRL_REG2;
	I2C_Buf[1] = 0x02;
	if( I2C_DRV_MasterSendDataBlocking( i2cCom1_IDX, &i2cCom1_MasterConfig0, &I2C_Buf[0], 1, &I2C_Buf[1], 1, 1000 ) != kStatus_I2C_Success )
		return kStatus_I2C_Fail;

	I2C_Buf[0] = FXOS8700_CTRL_REG1;
	I2C_Buf[1] = 0x0D;
	if( I2C_DRV_MasterSendDataBlocking( i2cCom1_IDX, &i2cCom1_MasterConfig0, &I2C_Buf[0], 1, &I2C_Buf[1], 1, 1000 ) != kStatus_I2C_Success )
		return kStatus_I2C_Fail;

	// store the gain terms in the accelerometer and magnetometer sensor structures
#define FXOS8700_UTPERCOUNT 0.1F      				// fixed range for FXOS8700 magnetometer
#define FXOS8700_COUNTSPERUT 10.0F					// must be reciprocal of FUTPERCOUNT
#define FXOS8700_GPERCOUNT 0.0001220703125F			// equal to 1/8192
	pthisAccel->fgPerCount = FXOS8700_GPERCOUNT;
	pthisMag->fuTPerCount = FXOS8700_UTPERCOUNT;
	pthisMag->fCountsPeruT = FXOS8700_COUNTSPERUT;

	return kStatus_I2C_Success;
}

// read FXOS8700 accelerometer and magnetometer data over I2C
i2c_status_t FXOS8700_ReadData( struct AccelSensor *pthisAccel, struct MagSensor *pthisMag )
{
	// set up the address of the first output register
	I2C_Buf[0] = FXOS8700_OUT_X_MSB;

	// read the 12 bytes of sequential sensor data
	if( I2C_DRV_MasterReceiveDataBlocking(i2cCom1_IDX, &i2cCom1_MasterConfig0, I2C_Buf, 1, I2C_Buf, 12, 1000) != kStatus_I2C_Success )
		return kStatus_I2C_Fail;

	// place the 12 bytes read into the 16 bit accelerometer and magnetometer structures
	pthisAccel->iGpFast[X] = (I2C_Buf[0] << 8) | I2C_Buf[1];
	pthisAccel->iGpFast[Y] = (I2C_Buf[2] << 8) | I2C_Buf[3];
	pthisAccel->iGpFast[Z] = (I2C_Buf[4] << 8) | I2C_Buf[5];
	pthisMag->iBpFast[X] = (I2C_Buf[6] << 8) | I2C_Buf[7];
	pthisMag->iBpFast[Y] = (I2C_Buf[8] << 8) | I2C_Buf[9];
	pthisMag->iBpFast[Z] = (I2C_Buf[10] << 8) | I2C_Buf[11];

	// finally check for -32768 in the accelerometer and magnetometer data since
	// this value cannot be negated in a later HAL operation
	if (pthisAccel->iGpFast[X] == -32768) pthisAccel->iGpFast[X]++;
	if (pthisAccel->iGpFast[Y] == -32768) pthisAccel->iGpFast[Y]++;
	if (pthisAccel->iGpFast[Z] == -32768) pthisAccel->iGpFast[Z]++;
	if (pthisMag->iBpFast[X] == -32768) pthisMag->iBpFast[X]++;
	if (pthisMag->iBpFast[Y] == -32768) pthisMag->iBpFast[Y]++;
	if (pthisMag->iBpFast[Z] == -32768) pthisMag->iBpFast[Z]++;

	return kStatus_I2C_Success;
}

