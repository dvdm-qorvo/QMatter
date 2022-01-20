/*
 * Copyright (c) 2020, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
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
 */

/** @file "qorvo_config.h"
 *
 */

#ifndef _QORVO_CONFIG_H_
#define _QORVO_CONFIG_H_

/*
 * Version info
 */

#define GP_CHANGELIST                                                            189048
#define GP_VERSIONINFO_APP                                                       MatterQorvoGlue_qpg6105_libbuild
#define GP_VERSIONINFO_BASE_COMPS                                                2,10,2,1
#define GP_VERSIONINFO_BLE_COMPS                                                 2,10,2,0
#define GP_VERSIONINFO_DATE                                                      2022-01-20
#define GP_VERSIONINFO_GLOBAL_VERSION                                            0,7,0,0
#define GP_VERSIONINFO_HOST                                                      UNKNOWN
#define GP_VERSIONINFO_PROJECT                                                   P345_Matter_DK_Endnodes
#define GP_VERSIONINFO_USER                                                      UNKNOWN@UNKNOWN


/*
 * Component: gpBle
 */

/* WcBleHost will be calling gpBle_ExecuteCommand */
#define GP_DIVERSITY_BLE_EXECUTE_CMD_WCBLEHOST


/*
 * Component: gpBleComps
 */

/* The amount of dedicated connection complete buffers */
#define GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS                           0


/*
 * Component: gpBleConfig
 */

/* The amount of LLCP procedures that are supported */
#define GP_BLE_NR_OF_SUPPORTED_PROCEDURES                                        0


/*
 * Component: gpBsp
 */

#define GP_BSP_FILENAME                                                          "gpBsp_Smart_Home_and_Lighting_CB_1_x_QPG6105.h"

/* UART baudrate */
#define GP_BSP_UART_COM_BAUDRATE                                                 115200

/* Support for A25L080 SPI flash chip */
#define GP_DIVERSITY_A25L080_SPIFLASH


/*
 * Component: gpCom
 */

/* Multiple gpComs were specified - defined in code */
#define GP_COM_DIVERSITY_MULTIPLE_COM

/* Enable SYN datastream encapsulation */
#define GP_COM_DIVERSITY_SERIAL

#define GP_COM_MAX_NUMBER_OF_MODULE_IDS                                          2

#define GP_COM_MAX_PACKET_PAYLOAD_SIZE                                           250

/* Use UART for COM - defined as default in code */
#define GP_DIVERSITY_COM_UART


/*
 * Component: gphal
 */

/* Enable Multistandard Listening: support 802.15.4 RxOnWhenIdle + BLE Observer/Central */
#define GP_HAL_DIVERSITY_MULTISTANDARD_LISTENING_MODE


/*
 * Component: gpLog
 */

/* overrule log string length from application code */
#define GP_LOG_MAX_LEN                                                           256


/*
 * Component: gpNvm
 */

/* Size of reserved section for NVM */
#define GP_DATA_SECTION_SIZE_NVM                                                 0x4000

/* maximal length of a token used */
#define GP_NVM_MAX_TOKENLENGTH                                                   13

/* set working range of gpNvm_PoolId_t, requires setting number of phy sectors for each pool */
#define GP_NVM_NBR_OF_POOLS                                                      1

/* Maximum number of unique tags in each pool. Used for memory allocation at Tag level API */
#define GP_NVM_NBR_OF_UNIQUE_TAGS                                                20

/* Maximum number of tokens tracked by token API */
#define GP_NVM_NBR_OF_UNIQUE_TOKENS                                              111

/* number of sectors of pool 1 */
#define GP_NVM_POOL_1_NBR_OF_PHY_SECTORS                                         16

#define GP_NVM_TYPE                                                              6


/*
 * Component: gpSched
 */

/* Don't include the implementation for our mainloop MAIN_FUNCTION_NAME */
#define GP_SCHED_EXTERNAL_MAIN

/* Callback after every main loop iteration. */
#define GP_SCHED_NR_OF_IDLE_CALLBACKS                                            0

/* Change the name of the main eventloop implementation */
#define MAIN_FUNCTION_NAME                                                       main


/*
 * Component: halCortexM4
 */

/* set custom stack size */
#define GP_KX_STACK_SIZE                                                         512

/* Set if hal has real mutex capability */
#define HAL_MUTEX_SUPPORTED


/*
 * Component: silexCryptoSoc
 */

#define AES_GCM_EMABLED                                                          0

#define AES_HW_KEYS_ENABLED                                                      0

#define AES_MASK_ENABLED                                                         0


#include "qorvo_internals.h"

#endif //_QORVO_CONFIG_H_