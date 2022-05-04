/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Description: This file implements the ISSDK FXAS21002 and FXOS8700 sensor
   drivers. It is used for reading data from FXAS21002 and FXOS8700 sensors.
 */

#ifndef GET_SENSOR_DATA_H_
#define GET_SENSOR_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "fsl_common.h"

/* ISSDK includes */
#include "fxas21002_drv.h"
#include "fxos8700_drv.h"

status_t SENSOR_Init(void);
status_t SENSOR_Run(fxas21002_gyrodata_t *rawDataGyro, fxos8700_accelmagdata_t *rawDataAccel);

#ifdef __cplusplus
}
#endif

#endif /* GET_SENSOR_DATA_H_ */
