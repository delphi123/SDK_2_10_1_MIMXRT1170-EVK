/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Generated by erpcgen 1.8.1 on Fri Apr 16 14:22:27 2021.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#if !defined(_erpc_two_way_rpc_Core1Interface_h_)
#define _erpc_two_way_rpc_Core1Interface_h_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "erpc_version.h"
#include "erpc_two_way_rpc_Core0Interface.h"
#include "erpc_two_way_rpc_Core1Interface.h"

#if 10801 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

#if !defined(ERPC_TYPE_DEFINITIONS)
#define ERPC_TYPE_DEFINITIONS

// Aliases data types declarations
/*! callback type */
typedef void (*getNumberCallback_t)(uint32_t * param1);

#endif // ERPC_TYPE_DEFINITIONS

/*! @brief Core1Interface identifiers */
enum _Core1Interface_ids
{
    kCore1Interface_service_id = 2,
    kCore1Interface_increaseNumber_id = 1,
    kCore1Interface_getGetCallbackFunction_id = 2,
    kCore1Interface_getNumberFromCore0_id = 3,
};

#if defined(__cplusplus)
extern "C" {
#endif

/*! Core1 interface */
//! @name Core1Interface
//@{
/*! Function for increasing given number by 1. Implementation on Core0 */
void increaseNumber(uint32_t * number);

/*! Gets callback function. Implementation on Core0  */
void getGetCallbackFunction(getNumberCallback_t * getNumberCallbackParam);

/*! Callback function. Implementation on Core0 */
void getNumberFromCore0(uint32_t * number);
//@}

#if defined(__cplusplus)
}
#endif

#endif // _erpc_two_way_rpc_Core1Interface_h_
