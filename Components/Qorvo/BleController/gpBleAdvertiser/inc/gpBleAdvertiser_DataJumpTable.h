/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by\
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header: //depot/release/Embedded/Components/Qorvo/BleController/v2.10.2.0/comps/gpBleAdvertiser/inc/gpBleAdvertiser_DataJumpTable.h#1 $
 * $Change: 187624 $
 * $DateTime: 2021/12/20 10:58:50 $
 *
 */

#ifndef _GPBLEADVERTISER_DATAJUMPTABLE_H_
#define _GPBLEADVERTISER_DATAJUMPTABLE_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpBleAdvertiser_defs.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GPBLEADVERTISER_DATAJUMPTABLE_INIT_LIST                               \
    .gpBleAdvertiser_GlobalVars_ptr = (void*) & gpBleAdvertiser_GlobalVars,

#endif // _GPBLEADVERTISER_DATAJUMPTABLE_H_