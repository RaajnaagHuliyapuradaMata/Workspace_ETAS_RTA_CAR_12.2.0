

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_P06.h"

E2E_INLINE void E2E_Prv_P06DoCheck(E2E_P06CheckStateType *StatePtr, const E2E_P06ConfigType * const ConfigPtr, uint8 recCounter_u8)
{
    uint8 deltaCounter_u8;    /* difference between received counter and last valid received counter */
                /* Calculate DeltaCounter, respect the limitation to value (E2E_P06_MAXCOUNTER + 1U) */
                deltaCounter_u8 = E2E_Prv_MaxDeltaCounter(recCounter_u8, StatePtr->Counter,
                                                          ((uint16)E2E_P06_MAXCOUNTER + 1U));

                /* Correct data */
                if (E2E_P06_DELTACOUNTER_OK == deltaCounter_u8)
                {
                    StatePtr->Status = E2E_P06STATUS_OK;
                }
                /* Repeated data */
                else if (E2E_P06_DELTACOUNTER_REPEATED == deltaCounter_u8)
                {
                    StatePtr->Status = E2E_P06STATUS_REPEATED;
                }
                /* Some data lost, but still ok */
                else if (deltaCounter_u8 <= ConfigPtr->MaxDeltaCounter)
                {
                    StatePtr->Status = E2E_P06STATUS_OKSOMELOST;
                }
                /* Wrong sequence or to many data lost */
                else
                {
                    StatePtr->Status = E2E_P06STATUS_WRONGSEQUENCE;
                }

                /* set counter value in state */
                StatePtr->Counter = recCounter_u8;
}

/**
 ***********************************************************************************************************************
 * E2E_P06ProtectInit
 *
 * \brief End to end communication protection state initialization
 *
 * Description: Protection initialization function of E2E profile P06
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
 * \param   E2E_P06ProtectStateType  *StatePtr       Pointer to the protect status structure of of E2E profile P06
 * \return  Std_ReturnType                           Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P06ProtectInit(E2E_P06ProtectStateType *StatePtr)
{
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
 * E2E_P06Protect
 *
 * \brief End to end communication protection routine corresponding to the profile P06 specified by AUTOSAR
 *
 * Description: Protection function of E2E profile P06
 * - Checks the pointer of the input variables
 * - Checks the range of the input variables
 * - Writes the current counter value (message sequence number) into the data stream
 * - Calculates a crc16 over the data stream to protect and writes the result into the data stream
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
 * \param   E2E_P06ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P06
 * \param   E2E_P06ProtectStateType *StatePtr   Pointer to the protect status structure of of E2E profile P06
 * \param   uint8                   *DataPtr    Pointer to the byte sequence within the Data to protect
 * \param   uint16                  Length      Length of the data in bytes
 * \return  Std_ReturnType                      Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P06Protect(const E2E_P06ConfigType *ConfigPtr, E2E_P06ProtectStateType *StatePtr,
                              uint8 *DataPtr, uint16 Length)
{
    /* Initialize internal error */
    Std_ReturnType error = E2E_E_INTERR;
    uint16 crc_u16;
    uint16 offset_u16;          /* offset in bytes */


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
        offset_u16 = ConfigPtr->Offset / 8U;

        /* Write counter into the data stream */
        E2E_Prv_P06WriteU8(DataPtr, offset_u16 + E2E_P06_COUNTER_OFFSET, StatePtr->Counter);
        
        /* Write length of data into the data stream with respect to the configured offset (big endian order) */
        E2E_Prv_P06WriteU16(DataPtr, offset_u16 + E2E_P06_LENGTH_OFFSET, Length);

        /* Calculate CRC over data using CRC16 (standard version of the CRC AUTOSAR library */
        crc_u16 = E2E_Prv_P06CalcCRC16(ConfigPtr, DataPtr, Length);

        /* Write CRC into the specified region of data */
        E2E_Prv_P06WriteU16(DataPtr, offset_u16, crc_u16);

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
 * E2E_P06CheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E profile P06
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
 * \param   E2E_P06CheckStateType     *StatePtr       Pointer to the check status structure of of E2E profile P06
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P06CheckInit(E2E_P06CheckStateType *StatePtr)
{
    Std_ReturnType error = E2E_E_INTERR;


    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* Initialize Check state */
        StatePtr->Counter = 0xFFU;
        StatePtr->Status = E2E_P06STATUS_ERROR;

        /* no error occurred */
        error = E2E_E_OK;
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P06Check
 *
 * \brief End to end communication check routine corresponding to the profile P06 specified by AUTOSAR
 *
 * Description: Check function of E2E profile P06
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
 * \param   E2E_P06ConfigType       *ConfigPtr      Pointer to the configuration structure of E2E profile P06
 * \param   E2E_P06CheckStateType   *StatePtr       Pointer to the check status structure of of E2E profile P06
 * \param   uint8                   *DataPtr        Pointer to the byte sequence within the Data to be checked
 * \param   uint16                  Length          Length of the data in bytes
 * \return  Std_ReturnType                          Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P06Check(const E2E_P06ConfigType *ConfigPtr, E2E_P06CheckStateType *StatePtr,
                            const uint8 *DataPtr, uint16 Length)
{
    Std_ReturnType error = E2E_E_INTERR;
    boolean IsDataOk;
    uint16 recLength_u16;       /* length value received via data stream */
    uint8  recCounter_u8;       /* counter value received via data stream */
    uint16 recCrc_u16;          /* crc16 value received via data stream */
    uint16 crc_u16;             /* calculated crc result from data stream */
    uint16 offset_u16;          /* offset in bytes */


    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input data */
    else 
    {
        IsDataOk = E2E_Prv_P06CheckData(ConfigPtr, DataPtr, Length);
        
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
                offset_u16 = ConfigPtr->Offset / 8U;
    
                /* Read Length from Data (big endian order) */
                recLength_u16 = E2E_Prv_P06ReadU16(DataPtr, offset_u16 + E2E_P06_LENGTH_OFFSET); 
    
                /* Read Counter from Data */
                recCounter_u8 = E2E_Prv_P06ReadU8(DataPtr, offset_u16 + E2E_P06_COUNTER_OFFSET); 
    
                /* Read Crc16 from Data */
                recCrc_u16 = E2E_Prv_P06ReadU16(DataPtr, offset_u16);
    
                 /* Calculate crc over data stream and DataID */
                 crc_u16 = E2E_Prv_P06CalcCRC16(ConfigPtr, DataPtr, Length); 
    
                /* Check for correctness of crc and length (profile 06 only) transmitted with the data */
                if ((crc_u16 != recCrc_u16) || (Length != recLength_u16))
                {
                    StatePtr->Status = E2E_P06STATUS_ERROR;
                }
                /* Crc OK: Received and calculated Crc is the same */
                else
                {
                    E2E_Prv_P06DoCheck(StatePtr, ConfigPtr, recCounter_u8);
                }
            }
            /* no new data available */
            else
            {
                StatePtr->Status = E2E_P06STATUS_NONEWDATA;
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
 * E2E_P06MapStatusToSM
 *
 * \brief Map profile P06 check status to generic state machine status
 *
 * Description:
 * The function maps the check status of profile P06 to a generic check status, which can be used by E2E state machine
 * check function. The E2E profile P06 delivers a more fine-granular status, but this is not relevant for the E2E
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
 * \param   Std_ReturnType            CheckReturn     Return value of the E2E_P06Check function
 * \param   E2E_P06CheckStatusType    Status          Status determined by E2E_P06Check function
 * \return  E2E_PCheckStatusType                      Profile-independent status of the reception on one single Data
 *                                                    in one cycle
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
E2E_PCheckStatusType E2E_P06MapStatusToSM(Std_ReturnType CheckReturn, E2E_P06CheckStatusType Status)
{
    /* report an ERROR as default */
    E2E_PCheckStatusType checkStatus_en = E2E_P_ERROR;


    /* TRACE[SWS_E2E_00461] */
    /* Check if return value is correct */
    if (E2E_E_OK == CheckReturn)
    {
        /* Evaluate Profile status */
        switch (Status)
        {
            case E2E_P06STATUS_OK:
            case E2E_P06STATUS_OKSOMELOST:
            {
                checkStatus_en = E2E_P_OK;
            }
            break;

            case E2E_P06STATUS_NONEWDATA:
            {
                checkStatus_en = E2E_P_NONEWDATA;
            }
            break;

            case E2E_P06STATUS_WRONGSEQUENCE:
            {
                checkStatus_en = E2E_P_WRONGSEQUENCE;
            }
            break;

            case E2E_P06STATUS_ERROR:
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;

            case E2E_P06STATUS_REPEATED:
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
    /* TRACE[SWS_E2E_00462] */
    return (checkStatus_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
