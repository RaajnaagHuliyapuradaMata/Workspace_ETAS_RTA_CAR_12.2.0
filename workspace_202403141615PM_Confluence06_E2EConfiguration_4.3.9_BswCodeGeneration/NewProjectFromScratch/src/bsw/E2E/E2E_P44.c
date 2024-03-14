

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_P44.h"

/**
 ***********************************************************************************************************************
 * E2E_P44ProtectInit
 *
 * \brief End to end communication protection state initialization
 *
 * Description: Protection initialization function of E2E profile P44
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
 * \param   E2E_P44ProtectStateType  *StatePtr       Pointer to the protect status structure of of E2E profile P44
 * \return  Std_ReturnType                           Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P44ProtectInit(E2E_P44ProtectStateType *StatePtr)
{
    /* TRACE[SWS_E2E_00373], TRACE[SWS_E2E_00377] */
    Std_ReturnType error = E2E_E_INTERR;


    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* Initialize Protect state */
        StatePtr->Counter = 0U;

        /* no error occurred */
        error = E2E_E_OK;
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P44Protect
 *
 * \brief End to end communication protection routine corresponding to the profile P44 specified by AUTOSAR
 *
 * Description: Protection function of E2E profile P44
 * - Checks the pointer of the input variables
 * - Checks the range of the input variables
 * - Writes the current counter value (message sequence number) into the data stream
 * - Calculates a crc32 over the data stream to protect and writes the result into the data stream
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
 * \param   E2E_P44ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P44
 * \param   E2E_P44ProtectStateType *StatePtr   Pointer to the protect status structure of of E2E profile P44
 * \param   uint8                   *DataPtr    Pointer to the byte sequence within the Data to protect
 * \param   uint16                  Length      Length of the data in bytes
 * \return  Std_ReturnType                      Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P44Protect(const E2E_P44ConfigType *ConfigPtr, E2E_P44ProtectStateType *StatePtr,
                              uint8 *DataPtr, uint16 Length)
{
    /* TRACE[SWS_E2E_00338], TRACE[SWS_E2E_00362], TRACE[SWS_E2E_00363], TRACE[SWS_E2E_00376], TRACE[SWS_E2E_00364],
       TRACE[SWS_E2E_00365], TRACE[SWS_E2E_00366], TRACE[SWS_E2E_00367], TRACE[SWS_E2E_00368], TRACE[SWS_E2E_00369] */
    /* Initialize internal error */
    Std_ReturnType error = E2E_E_INTERR;
    uint32 crc_u32;
    uint32 offset_u32;          /* offset in bytes */


    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr) || (NULL_PTR == DataPtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input variables */
    else if (
             (Length < (ConfigPtr->MinDataLength / 8U)) ||
             (Length > (ConfigPtr->MaxDataLength / 8U)))
    {
        error = E2E_E_INPUTERR_WRONG;
    }
    else
    {
        /* convert offset in bits to offset in bytes */
        offset_u32 = ConfigPtr->Offset / 8U;

        /* Write length of data into the data stream with respect to the configured offset (big endian order) */
        E2E_Prv_P44WriteU16(DataPtr, offset_u32, Length);
        
        /* Write counter into the data stream (big endian order) */
        E2E_Prv_P44WriteU16(DataPtr, offset_u32 + E2E_P44_COUNTER_OFFSET, StatePtr->Counter);
                   
        /* Write DataID of data into the data stream with respect to the configured offset (big endian order) */
        E2E_Prv_P44WriteU32(DataPtr, offset_u32 + E2E_P44_DATAID_OFFSET, ConfigPtr->DataID);
        /* Calculate CRC over data using CRC32 (standard version of the CRC AUTOSAR library) */
        crc_u32 = E2E_Prv_P44CalcCRC32P4(ConfigPtr, DataPtr, Length);

        /* Write CRC into the specified region of data (big endian order with respect to bit order) */
        E2E_Prv_P44WriteU32(DataPtr, offset_u32 + E2E_P44_CRC_OFFSET, crc_u32);

        /* increment the counter (overflow means: counter restarted) */
        StatePtr->Counter++;

        /* no error occurred */
        error = E2E_E_OK;
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P44CheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E profile P44
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
 * \param   E2E_P44CheckStateType     *StatePtr       Pointer to the check status structure of of E2E profile P44
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P44CheckInit(E2E_P44CheckStateType *StatePtr)
{
    /* TRACE[SWS_E2E_00350], TRACE[SWS_E2E_00378] */
    Std_ReturnType error = E2E_E_INTERR;


    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* Initialize Check state */
        StatePtr->Counter = 0xFFFFU;
        StatePtr->Status = E2E_P44STATUS_ERROR;

        /* no error occurred */
        error = E2E_E_OK;
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P44Check
 *
 * \brief End to end communication check routine corresponding to the profile P44 specified by AUTOSAR
 *
 * Description: Check function of E2E profile P44
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
 * \param   E2E_P44ConfigType       *ConfigPtr      Pointer to the configuration structure of E2E profile P44
 * \param   E2E_P44CheckStateType   *StatePtr       Pointer to the check status structure of of E2E profile P44
 * \param   uint8                   *DataPtr        Pointer to the byte sequence within the Data to be checked
 * \param   uint16                  Length          Length of the data in bytes
 * \return  Std_ReturnType                          Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P44Check(const E2E_P44ConfigType *ConfigPtr, E2E_P44CheckStateType *StatePtr,
                            const uint8 *DataPtr, uint16 Length)
{
    /* TRACE[SWS_E2E_00355], TRACE[SWS_E2E_00356], TRACE[SWS_E2E_00376], TRACE[SWS_E2E_00367], TRACE[SWS_E2E_00357],
       TRACE[SWS_E2E_00358], TRACE[SWS_E2E_00359], TRACE[SWS_E2E_00360], TRACE[SWS_E2E_00361], TRACE[SWS_E2E_00339] */
    Std_ReturnType error = E2E_E_INTERR;
    uint16 recLength_u16;       /* length value received via data stream */
    uint16 recCounter_u16;      /* counter value received via data stream */
    uint16 deltaCounter_u16;    /* difference between received counter and last valid received counter */
    uint32 recDataID_u32;       /* dataId value received via data stream */
    uint32 recCrc_u32;          /* crc32 value received via data stream */
    uint32 crc_u32;             /* calculated crc result from data stream */
    uint32 offset_u32;          /* offset in bytes */
    boolean IsDataOk;

    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input data */
    else 
    {
        IsDataOk = E2E_Prv_P44CheckData(ConfigPtr, DataPtr, Length);
        
        if (!IsDataOk)
        {
            error = E2E_E_INPUTERR_WRONG;
        }
        else
        {
            /* Check for new data available */
            if (NULL_PTR != DataPtr)
            {
                /* convert offset in bits to offset in bytes */
                offset_u32 = ConfigPtr->Offset / 8U;
    
                /* Read Length from Data (big endian order) */
                recLength_u16 = E2E_Prv_P44ReadU16(DataPtr, offset_u32);
                
                /* Read Counter from Data (big endian order) */
                recCounter_u16 = E2E_Prv_P44ReadU16(DataPtr, offset_u32 + E2E_P44_COUNTER_OFFSET);
                                 
                /* Read DataID from Data (big endian order) */
                recDataID_u32 = E2E_Prv_P44ReadU32(DataPtr, offset_u32 + E2E_P44_DATAID_OFFSET);
                               
                /* Read Crc32 from Data (big endian order) */
                recCrc_u32 = E2E_Prv_P44ReadU32(DataPtr, offset_u32 + E2E_P44_CRC_OFFSET);
                
                /* Calculate crc over data stream and DataID */
                crc_u32 = E2E_Prv_P44CalcCRC32P4(ConfigPtr, DataPtr, Length);
    
                /* Check for correctness of crc, length and dataID transmitted with the data */
                if ((crc_u32 != recCrc_u32) || (Length != recLength_u16) || (ConfigPtr->DataID != recDataID_u32))
                {
                    StatePtr->Status = E2E_P44STATUS_ERROR;
                }
                /* Crc OK: Received and calculated Crc is the same */
                else
                {
                    /* Calculate DeltaCounter */
                    deltaCounter_u16 = E2E_Prv_P44MaxDeltaCounter(recCounter_u16, StatePtr->Counter);
                    
                    /* Correct data */
                    if (E2E_P44_DELTACOUNTER_OK == deltaCounter_u16)
                    {
                        StatePtr->Status = E2E_P44STATUS_OK;
                    }
                    /* Repeated data */
                    else if (E2E_P44_DELTACOUNTER_REPEATED == deltaCounter_u16)
                    {
                        StatePtr->Status = E2E_P44STATUS_REPEATED;
                    }
                    /* Some data lost, but still ok */
                    else if (deltaCounter_u16 <= ConfigPtr->MaxDeltaCounter)
                    {
                        StatePtr->Status = E2E_P44STATUS_OKSOMELOST;
                    }
                    /* Wrong sequence or to many data lost */
                    else
                    {
                        StatePtr->Status = E2E_P44STATUS_WRONGSEQUENCE;
                    }
    
                    /* set counter value in state */
                    StatePtr->Counter = recCounter_u16;
                }
            }
            /* no new data available */
            else
            {
                StatePtr->Status = E2E_P44STATUS_NONEWDATA;
            }
    
            /* no error occurred */
            error = E2E_E_OK;
        }
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P44MapStatusToSM
 *
 * \brief Map profile P44 check status to generic state machine status
 *
 * Description:
 * The function maps the check status of profile P44 to a generic check status, which can be used by E2E state machine
 * check function. The E2E profile P44 delivers a more fine-granular status, but this is not relevant for the E2E
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
 * \param   Std_ReturnType            CheckReturn     Return value of the E2E_P44Check function
 * \param   E2E_P44CheckStatusType    Status          Status determined by E2E_P44Check function
 * \return  E2E_PCheckStatusType                      Profile-independent status of the reception on one single Data
 *                                                    in one cycle
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
E2E_PCheckStatusType E2E_P44MapStatusToSM(Std_ReturnType CheckReturn, E2E_P44CheckStatusType Status)
{
    /* TRACE[SWS_E2E_00349], TRACE[SWS_E2E_00351], TRACE[SWS_E2E_00352] */
    /* report an ERROR as default */
    E2E_PCheckStatusType checkStatus_en = E2E_P_ERROR;


    /* Check if return value is correct */
    if (E2E_E_OK == CheckReturn)
    {
        /* Evaluate Profile status */
        switch (Status)
        {
            case E2E_P44STATUS_OK:
            case E2E_P44STATUS_OKSOMELOST:
            {
                checkStatus_en = E2E_P_OK;
            }
            break;

            case E2E_P44STATUS_NONEWDATA:
            {
                checkStatus_en = E2E_P_NONEWDATA;
            }
            break;

            case E2E_P44STATUS_WRONGSEQUENCE:
            {
                checkStatus_en = E2E_P_WRONGSEQUENCE;
            }
            break;

            case E2E_P44STATUS_ERROR:
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;

            case E2E_P44STATUS_REPEATED:
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

    return (checkStatus_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
