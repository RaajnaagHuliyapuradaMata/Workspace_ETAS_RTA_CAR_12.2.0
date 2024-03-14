

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_P07.h"

/**
 ***********************************************************************************************************************
 * E2E_P07ProtectInit
 *
 * \brief End to end communication protection state initialization
 *
 * Description: Protection initialization function of E2E profile P07
 * - Checks the pointer of the input variables
 * - Initializes the current counter value (message sequence number) to zero
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P07ProtectStateType  *StatePtr       Pointer to the protect status structure of of E2E profile P07
 * \return  Std_ReturnType                           Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P07ProtectInit(E2E_P07ProtectStateType *StatePtr)
{
/* TRACE[SWS_E2E_00547] */
    Std_ReturnType error_en = E2E_E_INTERR;

    /* TRACE[SWS_E2E_00551] */
    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error_en = E2E_E_INPUTERR_NULL;
    }
    else
    {
         /* TRACE[SWS_E2E_00551] */
        /* Initialize Protect state */
        StatePtr->Counter = 0U;

        /* no error occurred */
        error_en = E2E_E_OK;
    }

    return (error_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P07Protect
 *
 * \brief End to end communication protection routine corresponding to the profile P07 specified by AUTOSAR
 *
 * Description: Protection function of E2E profile P07
 * - Checks the pointer of the input variables
 * - Checks the range of the input variables
 * - Writes the current counter value (message sequence number) into the data stream
 * - Calculates a crc64 over the data stream to protect and writes the result into the data stream
 * - Updates the counter of the sender for the next message to send
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P07ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P07
 * \param   E2E_P07ProtectStateType *StatePtr   Pointer to the protect status structure of of E2E profile P07
 * \param   uint8                   *DataPtr    Pointer to the byte sequence within the Data to protect
 * \param   uint32                  Length      Length of the data in bytes
 * \return  Std_ReturnType                      Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P07Protect(const E2E_P07ConfigType *ConfigPtr, E2E_P07ProtectStateType *StatePtr,
                              uint8 *DataPtr, uint32 Length)
{
    /* TRACE[SWS_E2E_00546] , TRACE[SWS_E2E_00486], TRACE[SWS_E2E_00487] */
    /* Initialize internal error */
    Std_ReturnType error_en = E2E_E_INTERR;
    uint64 crc_u64;
    uint32 offset_u32;          /* offset in bytes */


    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr) || (NULL_PTR == DataPtr))
    {
        error_en = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input variables */
    else if (
             (Length < (ConfigPtr->MinDataLength / 8U)) ||
             (Length > (ConfigPtr->MaxDataLength / 8U)))
    {
        error_en = E2E_E_INPUTERR_WRONG;
    }
    else
    {
        /* convert offset in bits to offset in bytes */
        offset_u32 = ConfigPtr->Offset / 8U;
        
        /* TRACE[[SWS_E2E_00490]] */
        /* Write counter into the data stream (big endian order and bit order is not changed) */
        E2E_Prv_P07WriteU32(DataPtr, offset_u32 + E2E_P07_COUNTER_OFFSET, StatePtr->Counter);
        
        /* TRACE[SWS_E2E_00489] */       
        /* Write length of data into the data stream with respect to the configured offset (big endian order) */
        E2E_Prv_P07WriteU32(DataPtr, offset_u32 + E2E_P07_LENGTH_OFFSET, Length);
        
        /* TRACE[SWS_E2E_00491] */      
        /* Write DataID of data into the data stream with respect to the configured offset (big endian order) */
        E2E_Prv_P07WriteU32(DataPtr, offset_u32 + E2E_P07_DATAID_OFFSET, ConfigPtr->DataID);
        
        /* TRACE[SWS_E2E_00492] */
        /* Calculate CRC over data using CRC64 (standard version of the CRC AUTOSAR library */
         crc_u64 = E2E_Prv_P07CalcCRC64(ConfigPtr, DataPtr, Length);
        
        /* TRACE[SWS_E2E_00493] */
        /* Write CRC into the specified region of data (big endian order with respect to bit order) */
        E2E_Prv_P07WriteU64(DataPtr, offset_u32, crc_u64);
        
        /* TRACE[SWS_E2E_00494] */
        /* increment the counter (overflow means: counter restarted) */
        StatePtr->Counter++;

        /* no error occurred */
        error_en = E2E_E_OK;
    }

    return (error_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P07CheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E profile P07
 * - Checks the pointer of the input values for NULL,
 * - Sets all variables in check state structure to default values
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P07CheckStateType     *StatePtr       Pointer to the check status structure of of E2E profile P07
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P07CheckInit(E2E_P07CheckStateType *StatePtr)
{
/* TRACE[SWS_E2E_00549] */
    Std_ReturnType error_en = E2E_E_INTERR;

    /* TRACE[SWS_E2E_00552] */
    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error_en = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* TRACE[SWS_E2E_00552] */
        /* Initialize Check state */
        StatePtr->Counter = 0xFFFFFFFFU;
        StatePtr->Status = E2E_P07STATUS_ERROR;

        /* no error occurred */
        error_en = E2E_E_OK;
    }

    return (error_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P07Check
 *
 * \brief End to end communication check routine corresponding to the profile P07 specified by AUTOSAR
 *
 * Description: Check function of E2E profile P07
 * - Checks the pointer of the input values for NULL,
 * - Checks the range of the input values
 *   These two first check lead to a interrupt of the check routine when the result is FALSE.
 *   Otherwise the next checks are performed.
 * - Gets the current Counter from the data stream
 * - Gets the crc result, calculated by the protect function from the data stream
 * - Calculates the crc result of the received data stream and compares it with the received crc result
 * - Checks the disposition of the receiver
 * - Checks the correctness of the received data stream
 * - Checks the sequence of the received message
 * - Evaluates the status of the receiver
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   E2E_P07ConfigType       *ConfigPtr      Pointer to the configuration structure of E2E profile P07
 * \param   E2E_P07CheckStateType   *StatePtr       Pointer to the check status structure of of E2E profile P07
 * \param   uint8                   *DataPtr        Pointer to the byte sequence within the Data to be checked
 * \param   uint32                  Length          Length of the data in bytes
 * \return  Std_ReturnType                          Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"

Std_ReturnType E2E_P07Check(const E2E_P07ConfigType *ConfigPtr, E2E_P07CheckStateType *StatePtr,
                            const uint8 *DataPtr, uint32 Length)
{

 /* TRACE[SWS_E2E_00495] , TRACE[SWS_E2E_00501] , TRACE[SWS_E2E_00548]  */
    Std_ReturnType error_en = E2E_E_INTERR;
    boolean IsDataOk;
    uint32 recLength_u32;       /* length value received via data stream */
    uint32 recCounter_u32;      /* counter value received via data stream */
    uint32 deltaCounter_u32;    /* difference between received counter and last valid received counter */
    uint32 recDataID_u32;       /* dataId value received via data stream */
    uint64 recCrc_u64;          /* crc64 value received via data stream */
    uint64 crc_u64;             /* calculated crc result from data stream */
    uint32 offset_u32;          /* offset in bytes */

    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr))
    {
        error_en = E2E_E_INPUTERR_NULL;
    }
    else
    {
        IsDataOk = E2E_Prv_P07CheckData(ConfigPtr, DataPtr, Length);

        if ( !IsDataOk)
        {
            error_en = E2E_E_INPUTERR_WRONG;
        }
        else
        {
            /* TRACE[SWS_E2E_00496] */
            /* Check for new data available */
            if (NULL_PTR != DataPtr)
            {
                /* convert offset in bits to offset in bytes */
                offset_u32 = ConfigPtr->Offset / 8U;
                
                /* TRACE[SWS_E2E_00497] */
                /* Read Length from Data (big endian order) */
                recLength_u32 = E2E_Prv_P07ReadU32(DataPtr, offset_u32 + E2E_P07_LENGTH_OFFSET);
                
                /* TRACE[SWS_E2E_00498] */
                /* Read Counter from Data (big endian order) */
                recCounter_u32 = E2E_Prv_P07ReadU32(DataPtr, offset_u32 + E2E_P07_COUNTER_OFFSET);
                 
                 /* TRACE[SWS_E2E_00499] */               
                /* Read DataID from Data (big endian order) */
                recDataID_u32 = E2E_Prv_P07ReadU32(DataPtr, offset_u32 + E2E_P07_DATAID_OFFSET);
                 
                 /* TRACE[SWS_E2E_00500] */              
                /* Read Crc64 from Data (big endian order) */
                recCrc_u64 = E2E_Prv_P07ReadU64(DataPtr, offset_u32);
                
                 /* Calculate crc over data stream and DataID */
                 crc_u64 = E2E_Prv_P07CalcCRC64(ConfigPtr, DataPtr, Length);
    
                /* Check for correctness of crc, length and dataID transmitted with the data */
                if ((crc_u64 != recCrc_u64) || (Length != recLength_u32) || (ConfigPtr->DataID != recDataID_u32))
                {
                    StatePtr->Status = E2E_P07STATUS_ERROR;
                }
                /* Crc OK: Received and calculated Crc is the same */
                else
                {
                    /* Calculate DeltaCounter */
                    deltaCounter_u32 = E2E_Prv_P07MaxDeltaCounter(recCounter_u32, StatePtr->Counter);
                    
                    StatePtr->Status = E2E_Prv_P07GetCheckStatus(deltaCounter_u32, ConfigPtr->MaxDeltaCounter);
    
                    /* set counter value in state */
                    StatePtr->Counter = recCounter_u32;
                }
            }
            /* no new data available */
            else
            {
                StatePtr->Status = E2E_P07STATUS_NONEWDATA;
            }
    
            /* no error occurred */
            error_en = E2E_E_OK;
        }
    }

    return (error_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P07MapStatusToSM
 *
 * \brief Map profile P07 check status to generic state machine status
 *
 * Description:
 * The function maps the check status of profile P07 to a generic check status, which can be used by E2E state machine
 * check function. The E2E profile P07 delivers a more fine-granular status, but this is not relevant for the E2E
 * state machine.
 *
 * Restrictions:
 *   -
 *
 * Dependencies:
 *   -
 *
 * Resources:
 *   -
 *
 * \param   Std_ReturnType            CheckReturn     Return value of the E2E_P07Check function
 * \param   E2E_P07CheckStatusType    Status          Status determined by E2E_P07Check function
 * \return  E2E_PCheckStatusType                      Profile-independent status of the reception on one single Data
 *                                                    in one cycle
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
E2E_PCheckStatusType E2E_P07MapStatusToSM(Std_ReturnType CheckReturn, E2E_P07CheckStatusType Status)
{
/* TRACE[SWS_E2E_00550] */
    /* report an ERROR as default */
    E2E_PCheckStatusType checkStatus_en = E2E_P_ERROR;

/* TRACE[SWS_E2E_00553]*/
    /* Check if return value is correct */
    if (E2E_E_OK == CheckReturn)
    {
        /* Evaluate Profile status */
        switch (Status)
        {
            case E2E_P07STATUS_OK:
            case E2E_P07STATUS_OKSOMELOST:
            {
                checkStatus_en = E2E_P_OK;
            }
            break;

            case E2E_P07STATUS_NONEWDATA:
            {
                checkStatus_en = E2E_P_NONEWDATA;
            }
            break;

            case E2E_P07STATUS_WRONGSEQUENCE:
            {
                checkStatus_en = E2E_P_WRONGSEQUENCE;
            }
            break;

            case E2E_P07STATUS_ERROR:
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;

            case E2E_P07STATUS_REPEATED:
            {
                checkStatus_en = E2E_P_REPEATED;
            }
            break;

            default: /* Status has an invalid value*/
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;
        }
    }
/* TRACE[SWS_E2E_00554] */
    return (checkStatus_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
