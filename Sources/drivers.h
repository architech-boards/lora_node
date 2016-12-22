/*
 * drivers.h
 *
 *  Created on: Sep 14, 2015
 *      Author: Owner
 */

#ifndef DRIVERS_H_
#define DRIVERS_H_

#include "fsl_i2c_hal.h"

#define uint8 unsigned char
#define int16 short int
#define int32 int

// FXOS8700 registers and constants
#define FXOS8700_OUT_X_MSB       	  	0x01
#define FXOS8700_WHO_AM_I      			0x0D
#define FXOS8700_XYZ_DATA_CFG       	0x0E
#define FXOS8700_CTRL_REG1        	 	0x2A
#define FXOS8700_CTRL_REG2        	 	0x2B
#define FXOS8700_M_CTRL_REG1         	0x5B
#define FXOS8700_M_CTRL_REG2        	0x5C
#define FXOS8700_WHO_AM_I_VALUE     	0xC7
#define FXOS8700_I2C_ADDR				0x1E
#define OVERSAMPLE_RATIO 				8       	// int32: 8x: 3DOF, 6DOF, 9DOF run at SENSORFS / OVERSAMPLE_RATIO Hz
#define I2C_BUF_LEN           			16
#define I2C_SENT_FLAG      				0x01
#define I2C_RCVD_FLAG       			0x02

// vector components
#define X 0
#define Y 1
#define Z 2


// accelerometer sensor structure definition
struct AccelSensor
{
	int32 iSumGpFast[3];	// sum of fast measurements
	float fGpFast[3];		// fast (typically 200Hz) readings (g)
	float fGp[3];			// slow (typically 25Hz) averaged readings (g)
	float fgPerCount;		// initialized to FGPERCOUNT
	int16 iGpFast[3];		// fast (typically 200Hz) readings
	int16 iGp[3];			// slow (typically 25Hz) averaged readings (counts)
};

// magnetometer sensor structure definition
struct MagSensor
{
	int32 iSumBpFast[3];	// sum of fast measurements
	float fBpFast[3];		// fast (typically 200Hz) raw readings (uT)
	float fBp[3];			// slow (typically 25Hz) averaged raw readings (uT)
	float fBcFast[3];		// fast (typically 200Hz) calibrated readings (uT)
	float fBc[3];			// slow (typically 25Hz) averaged calibrated readings (uT)
	float fuTPerCount;		// initialized to FUTPERCOUNT
	float fCountsPeruT;		// initialized to FCOUNTSPERUT
	int16 iBpFast[3];		// fast (typically 200Hz) raw readings (counts)
	int16 iBp[3];			// slow (typically 25Hz) averaged raw readings (counts)
	int16 iBc[3];			// slow (typically 25Hz) averaged calibrated readings (counts)
};

// gyro sensor structure definition
struct GyroSensor
{
	int32 iSumYpFast[3];					// sum of fast measurements
	float fYp[3];							// raw gyro sensor output (deg/s)
	float fDegPerSecPerCount;				// initialized to FDEGPERSECPERCOUNT
	int16 iYpFast[OVERSAMPLE_RATIO][3];		// fast (typically 200Hz) readings
	int16 iYp[3];							// averaged gyro sensor output (counts)
};

extern i2c_status_t FXOS8700_Init( struct AccelSensor *pthisAccel, struct MagSensor *pthisMag);
extern i2c_status_t FXOS8700_ReadData( struct AccelSensor *pthisAccel, struct MagSensor *pthisMag );

#endif /* DRIVERS_H_ */
