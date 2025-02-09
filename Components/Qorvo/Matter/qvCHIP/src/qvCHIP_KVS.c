/*
 * Copyright (c) 2021-2022, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 */

/** @file "qvCHIP_KVS.c"
 *
 *  CHIP KVS functionality
 *
 *  Implementation of qvCHIP KVS
 *
 *  This implementation supports theoretical data sizes of up to 255 * 255 bytes in one tag. This is
 *  achieved by extending the tag space by one byte and in case the data to be stored is larger than
 *  one tag value buffer (MAX_KVS_VALUE_LEN = 255 bytes), a new tag is allocated, using the same
 *  original tag name, plus an index at the end. In this way, up to 255 tags can be created starting
 *  from the original key:
 *
 *  |-----------------------------------------------------| <- original buffer, key={'T','A','G'}
 *
 *  Extended_Key_0 = {'T','A','G','0'},
 *  Extended_Key_1 = {'T','A','G','1'}, ...
 *
 *  |__0__|__1__|__2__|__3__|__4__|__5__|__6__|__7__|__8__|
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
/* <CodeGenerator Placeholder> Includes */

#define GP_COMPONENT_ID GP_COMPONENT_ID_QVCHIP

#include "qvCHIP.h"

#include "hal.h"
#include "gpLog.h"
#include "gpNvm.h"


#ifdef QVCHIP_DIVERSITY_KVS_HASH_KEYS
#include "mbedtls/sha256.h"
#endif // QVCHIP_DIVERSITY_KVS_HASH_KEYS
/* </CodeGenerator Placeholder> Includes */

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> Macro */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* Pool ID for KVS is the same as pool id for NVN implementation - they share the same pool */
#define KVS_POOL_ID (0)

/* Maximum length for values stored in KVS - reserve last byte for keys larger than maximum value size */
#define MAX_KVS_KEY_LEN (GP_NVM_MAX_TOKENLENGTH - 2)

/* Maximum length for the token mask when looking for a specific token */
#define MAX_KVS_TOKENMASK_LEN (GP_NVM_MAX_TOKENLENGTH - 1)

/* Maximum length for values stored in KVS */
#ifdef GP_DIVERSITY_ROMUSAGE_FOR_MATTER
#define MAX_KVS_VALUE_LEN (251)
#else //GP_DIVERSITY_ROMUSAGE_FOR_MATTER
#define MAX_KVS_VALUE_LEN (255)
#endif //GP_DIVERSITY_ROMUSAGE_FOR_MATTER

typedef PACKED_PRE struct qvCHIP_KVS_Tag_ {
    uint8_t componentId;
    uint8_t key[MAX_KVS_KEY_LEN];
    uint8_t idExt;
} PACKED_POST qvCHIP_KVS_Tag;


/* Variable settings definitions */
#ifndef GP_NVM_NBR_OF_UNIQUE_TOKENS
#define GP_NVM_NBR_OF_UNIQUE_TOKENS GP_NVM_NBR_OF_UNIQUE_TAGS
#endif //GP_NVM_NBR_OF_UNIQUE_TOKENS
#if GP_NVM_NBR_OF_POOLS > 1
#error CHIP glue layer only built for use with 1 pool currently.
#endif //GP_NVM_NBR_OF_POOLS

/* </CodeGenerator Placeholder> Macro */

/*****************************************************************************
 *                    Static Data
 *****************************************************************************/

HAL_CRITICAL_SECTION_DEF(qvCHIP_KvsMutex)

// Start of NVM area - linkerscript defined
extern const UIntPtr gpNvm_Start;

/*****************************************************************************
 *                    Static Component Function Definitions
 *****************************************************************************/
#ifdef QVCHIP_DIVERSITY_KVS_HASH_KEYS
static qvStatus_t qvCHIP_KvsHashKey(uint8_t length, uint8_t* key, uint8_t* hash)
{
    int ret;
    mbedtls_sha256_context qvCHIP_Kvs_HashContext;
    mbedtls_sha256_init(&qvCHIP_Kvs_HashContext);
    ret = mbedtls_sha256_starts_ret(&qvCHIP_Kvs_HashContext, 0);
    if(ret)
    {
        goto exit;
    }
    ret = mbedtls_sha256_update_ret(&qvCHIP_Kvs_HashContext, key, length);
    if(ret)
    {
        goto exit;
    }
    ret = mbedtls_sha256_finish_ret(&qvCHIP_Kvs_HashContext, hash);
    if(ret)
    {
        goto exit;
    }
exit:
    mbedtls_sha256_free(&qvCHIP_Kvs_HashContext);
    return (ret) ? QV_STATUS_INVALID_DATA : QV_STATUS_NO_ERROR;
}
#endif // QVCHIP_DIVERSITY_KVS_HASH_KEYS

static qvStatus_t qvCHIP_KvsCreateExtTag(const char* key, qvCHIP_KVS_Tag* extTag)
{
    qvStatus_t qv_status = QV_STATUS_NO_ERROR;
    uint8_t keyLen = strlen(key);
#ifdef QVCHIP_DIVERSITY_KVS_HASH_KEYS
    uint8_t hash[32];
    qv_status = qvCHIP_KvsHashKey(keyLen, (uint8_t*)key, hash);
    if(QV_STATUS_NO_ERROR != qv_status)
    {
        return qv_status;
    }
    MEMCPY(extTag->key, hash, MAX_KVS_KEY_LEN);
#else
    if(keyLen > MAX_KVS_KEY_LEN)
    {
        GP_LOG_PRINTF("WARNING: Key is too long");
        return QV_STATUS_INVALID_DATA;
    }
    /* make sure we only use the string part of the key and not the rest that
       follows after the string terminator */
    MEMSET(extTag->key, 0x00, MAX_KVS_KEY_LEN);
    MEMCPY(extTag->key, key, MIN(keyLen, MAX_KVS_KEY_LEN));
#endif // QVCHIP_DIVERSITY_KVS_HASH_KEYS
    extTag->componentId = GP_COMPONENT_ID;

    return qv_status;
}

/*****************************************************************************
 *                    Public Component Function Definitions
 *****************************************************************************/

qvStatus_t qvCHIP_KvsInit(void)
{
#ifdef GP_NVM_DIVERSITY_VARIABLE_SETTINGS
    UInt8 availableMaxTokenLength;
    UIntPtr currentNvmStart;
    gpNvm_KeyIndex_t currentNumberOfUniqueTokens;

    gpNvm_GetVariableSettings(&currentNvmStart, &currentNumberOfUniqueTokens, &availableMaxTokenLength);
    GP_LOG_PRINTF("Settings old/new: Nvm Start:%lx/%lx #Uniq Token:%u/%u Token Length:%u/%u", 0,
                  (unsigned long)currentNvmStart, (unsigned long)&gpNvm_Start,
                  currentNumberOfUniqueTokens, GP_NVM_NBR_OF_UNIQUE_TOKENS,
                  availableMaxTokenLength, GP_NVM_MAX_TOKENLENGTH);

    if(availableMaxTokenLength < GP_NVM_MAX_TOKENLENGTH)
    {
        GP_LOG_SYSTEM_PRINTF("Max token length %u < %u", 0, availableMaxTokenLength, GP_NVM_MAX_TOKENLENGTH);
        return QV_STATUS_BUFFER_TOO_SMALL;
    }

    gpNvm_SetVariableSettings((UIntPtr)&gpNvm_Start, GP_NVM_NBR_OF_UNIQUE_TOKENS);
#endif //GP_NVM_DIVERSITY_VARIABLE_SETTINGS
#ifdef GP_NVM_DIVERSITY_VARIABLE_SIZE
    UInt16 currentNrOfSectors;
    UInt8 currentNrOfPools;
    UInt8 currentSectorsPerPool[4]; // Max from NVM implementation

    UInt8 sectorsPerPool[] = {GP_NVM_POOL_1_NBR_OF_PHY_SECTORS};

    gpNvm_GetVariableSize(&currentNrOfSectors, &currentNrOfPools, currentSectorsPerPool);
    GP_LOG_PRINTF("Sizes old/new: #sectors:%lx/%lx #pools:%u/%u", 0,
                  currentNrOfSectors, sectorsPerPool[0],
                  currentNrOfPools, GP_NVM_NBR_OF_POOLS);

    gpNvm_SetVariableSize(sectorsPerPool[0], GP_NVM_NBR_OF_POOLS, sectorsPerPool);
#endif //GP_NVM_DIVERSITY_VARIABLE_SIZE

    hal_MutexCreate(&qvCHIP_KvsMutex);
    if(!hal_MutexIsValid(qvCHIP_KvsMutex))
    {
        return QV_STATUS_NVM_ERROR;
    }

    return QV_STATUS_NO_ERROR;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
qvStatus_t qvCHIP_KvsPut(const char* key, const void* value, size_t valueSize)
{
    /* <CodeGenerator Placeholder> Implementation_qvCHIP_KvsPut */
    qvStatus_t qv_status;
    gpNvm_Result_t nvm_result;

    qvCHIP_KVS_Tag extTag;
    uint8_t idExt;

    size_t totalBytesWritten;

    if((key == NULL) || (value == NULL))
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    qv_status = qvCHIP_KvsCreateExtTag(key, &extTag);
    if(QV_STATUS_NO_ERROR != qv_status)
    {
        return qv_status;
    }

    hal_MutexAcquire(qvCHIP_KvsMutex);


    idExt = 0;
    totalBytesWritten = 0;
    while(totalBytesWritten < valueSize)
    {
        uint8_t bytesToWrite;

        bytesToWrite = ((valueSize - totalBytesWritten) > MAX_KVS_VALUE_LEN) ? MAX_KVS_VALUE_LEN : (valueSize - totalBytesWritten);
        /* idExt is incrementing to create unique tags for value sizes more than
            the maximum size of one KVS entry */
        extTag.idExt = idExt;

        nvm_result = gpNvm_Write(KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, GP_NVM_MAX_TOKENLENGTH, (uint8_t*)&extTag,
                                 bytesToWrite, (unsigned char*)value + totalBytesWritten);
        if((nvm_result != gpNvm_Result_DataAvailable) && (nvm_result != gpNvm_Result_NoDataAvailable))
        {
            hal_MutexRelease(qvCHIP_KvsMutex);
            return QV_STATUS_INVALID_DATA;
        }

        idExt += 1;
        totalBytesWritten += bytesToWrite;
    }

    hal_MutexRelease(qvCHIP_KvsMutex);
    return QV_STATUS_NO_ERROR;
    /* </CodeGenerator Placeholder> Implementation_qvCHIP_KvsPut */
}

qvStatus_t qvCHIP_KvsGet(const char* key, void* value, size_t valueSize, size_t* readBytesSize,
                         size_t offsetBytes)
{
    /* <CodeGenerator Placeholder> Implementation_qvCHIP_KvsGet */
    gpNvm_LookupTable_Handle_t handle;
    gpNvm_Result_t nvm_result;
    uint8_t nrOfMatches;
    qvCHIP_KVS_Tag extTag;

    uint8_t tempTagData[MAX_KVS_VALUE_LEN];
    uint8_t idExt;

    size_t totalBytesRead;

    qvStatus_t qv_status = QV_STATUS_NO_ERROR;

    /* check parameters*/
    if((key == NULL) || (value == NULL) || (readBytesSize == NULL))
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    /* initialize variables */
    *readBytesSize = 0;

    qv_status = qvCHIP_KvsCreateExtTag(key, &extTag);
    if(QV_STATUS_NO_ERROR != qv_status)
    {
        return qv_status;
    }

    hal_MutexAcquire(qvCHIP_KvsMutex);

    /* build the lookup table based on the key - could return multiple results */
    nvm_result = gpNvm_BuildLookup(&handle, KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore,
                                   MAX_KVS_TOKENMASK_LEN, (uint8_t*)&extTag,
                                   GP_NVM_NBR_OF_UNIQUE_TOKENS, &nrOfMatches);
    if((nvm_result != gpNvm_Result_DataAvailable) || (nrOfMatches == 0))
    {
        qv_status = QV_STATUS_INVALID_DATA;
        goto _cleanup;
    }

    /*
        Note: due to offseting, we need to read the tag into a temporary buffer and rebuild the
              data afterwards
    */
    idExt = 0;
    totalBytesRead = 0;
    while(nrOfMatches > 0)
    {
        uint8_t bytesRead;

        /* idExt is incrementing to create unique tags for value sizes more than
            the maximum size of one KVS entry */
        extTag.idExt = idExt;

        nvm_result = gpNvm_ReadUnique(handle, KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, NULL,
                                      GP_NVM_MAX_TOKENLENGTH, (uint8_t*)&extTag, MAX_KVS_VALUE_LEN, &bytesRead,
                                      tempTagData);
        if(nvm_result == gpNvm_Result_Truncated)
        {
            //Note: this error should not appear, as the maximum tag value size is MAX_KVS_VALUE_LEN
            //      and we provide a buffer this size for reading
            qv_status = QV_STATUS_BUFFER_TOO_SMALL;
            goto _cleanup;
        }
        if(nvm_result != gpNvm_Result_DataAvailable)
        {
            qv_status = QV_STATUS_INVALID_DATA;
            goto _cleanup;
        }

        totalBytesRead += bytesRead;

        /* only copy data into the provided buffer if we are past the specified offset */
        if(totalBytesRead > offsetBytes)
        {
            Bool bufferTooSmall = false;

            /* if we read more than fits the data buffer, trim length to fit and return BUFFER_TOO_SMALL */
            if(totalBytesRead - offsetBytes > valueSize)
            {
                totalBytesRead = offsetBytes + valueSize;
                bufferTooSmall = true;
            }
            /* use old value of readBytesSize as index into the destination buffer */
            MEMCPY((unsigned char*)value + *readBytesSize,
                   ((totalBytesRead - offsetBytes) > MAX_KVS_VALUE_LEN) ? tempTagData : &tempTagData[offsetBytes % MAX_KVS_VALUE_LEN],
                   totalBytesRead - offsetBytes - *readBytesSize);
            *readBytesSize = totalBytesRead - offsetBytes;

            if(bufferTooSmall == true)
            {
                qv_status = QV_STATUS_BUFFER_TOO_SMALL;
                goto _cleanup;
            }
        }

        idExt += 1;
        nrOfMatches -= 1;
    }


_cleanup:

    gpNvm_FreeLookup(handle);
    hal_MutexRelease(qvCHIP_KvsMutex);
    return qv_status;
    /* </CodeGenerator Placeholder> Implementation_qvCHIP_KvsGet */
}

qvStatus_t qvCHIP_KvsDelete(const char* key)
{
    /* <CodeGenerator Placeholder> Implementation_qvCHIP_KvsDelete */
    gpNvm_LookupTable_Handle_t handle;
    gpNvm_Result_t nvm_result;

    uint8_t nrOfMatches;
    qvCHIP_KVS_Tag extTag;
    uint8_t idExt;

    qvStatus_t qv_status = QV_STATUS_NO_ERROR;

    if(key == NULL)
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    qv_status = qvCHIP_KvsCreateExtTag(key, &extTag);
    if(QV_STATUS_NO_ERROR != qv_status)
    {
        return qv_status;
    }
    hal_MutexAcquire(qvCHIP_KvsMutex);

    nvm_result = gpNvm_BuildLookup(&handle, KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore,
                                   MAX_KVS_TOKENMASK_LEN, (uint8_t*)&extTag,
                                   GP_NVM_NBR_OF_UNIQUE_TOKENS, &nrOfMatches);
    if((nvm_result != gpNvm_Result_DataAvailable) || (nrOfMatches == 0))
    {
        qv_status = QV_STATUS_INVALID_DATA;
        goto _cleanup;
    }

    /* delete all extended tags matching the specified tag - the algorithm assumes the extended tag
       ids are generated linearly starting from 0 */
    idExt = 0;
    while(nrOfMatches > 0)
    {
        extTag.idExt = idExt;

        nvm_result = gpNvm_Remove(KVS_POOL_ID, gpNvm_UpdateFrequencyIgnore, GP_NVM_MAX_TOKENLENGTH, (uint8_t*)&extTag);
        if(nvm_result != gpNvm_Result_DataAvailable)
        {
            qv_status = QV_STATUS_INVALID_DATA;
            goto _cleanup;
        }

        idExt += 1;
        nrOfMatches -= 1;
    }

_cleanup:

    gpNvm_FreeLookup(handle);
    hal_MutexRelease(qvCHIP_KvsMutex);
    return qv_status;
    /* </CodeGenerator Placeholder> Implementation_qvCHIP_KvsDelete */
}

qvStatus_t qvCHIP_KvsErasePartition(void)
{
    /* <CodeGenerator Placeholder> Implementation_qvCHIP_KvsErasePartition */
    // When using this function, advise to trigger a reset in a higher lever afterwards to make sure everything
    // is cleaned up. [qvCHIP_ResetSystem API can be used to trigger the reset].
    // see <matter repo>/src/platform/qpg/ConfigurationManagerImpl.cpp - ConfigurationManagerImpl::DoFactoryReset

    qvStatus_t res = QV_STATUS_NO_ERROR;

    gpNvm_ErasePool(gpNvm_PoolId_AllPoolIds);

    return res;
    /* </CodeGenerator Placeholder> Implementation_qvCHIP_KvsErasePartition */
}
