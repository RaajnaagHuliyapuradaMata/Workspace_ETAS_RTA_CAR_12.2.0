

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_P05.h"

E2E_INLINE void E2E_Prv_P05DoCheck(E2E_P05CheckStateType *StatePtr, const E2E_P05ConfigType * const ConfigPtr, uint8 recCounter_u8)
{
    uint8 deltaCounter_u8;    /* difference between received counter and last valid received counter */
                /* Calculate DeltaCounter, respect the limitation to value (E2E_P05_MAXCOUNTER + 1U) */
                deltaCounter_u8 = E2E_Prv_MaxDeltaCounter(recCounter_u8, StatePtr->Counter,
                                                          ((uint16)E2E_P05_MAXCOUNTER + 1U));

                /* Correct data */
                if (E2E_P05_DELTACOUNTER_OK == deltaCounter_u8)
                {
                    StatePtr->Status = E2E_P05STATUS_OK;
                }
                /* Repeated data */
                else if (E2E_P05_DELTACOUNTER_REPEATED == deltaCounter_u8)
                {
                    StatePtr->Status = E2E_P05STATUS_REPEATED;
                }
                /* Some data lost, but still ok */
                else if (deltaCounter_u8 <= ConfigPtr->MaxDeltaCounter)
                {
                    StatePtr->Status = E2E_P05STATUS_OKSOMELOST;
                }
                /* Wrong sequence or to many data lost */
                else
                {
                    StatePtr->Status = E2E_P05STATUS_WRONGSEQUENCE;
                }

                /* set counter value in state */
                StatePtr->Counter = recCounter_u8;
}

/**
 ***********************************************************************************************************************
 * E2E_P05ProtectInit
 *
 * \brief End to end communication protection state initialization
 *
 * Description: Protection initialization function of E2E profile P05
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
 * \param   E2E_P05ProtectStateType  *StatePtr       Pointer to the protect status structure of of E2E profile P05
 * \return  Std_ReturnType                           Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P05ProtectInit(E2E_P05ProtectStateType *StatePtr)
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
 * E2E_P05Protect
 *
 * \brief End to end communication protection routine corresponding to the profile P05 specified by AUTOSAR
 *
 * Description: Protection function of E2E profile P05
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
 * \param   E2E_P05ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P05
 * \param   E2E_P05ProtectStateType *StatePtr   Pointer to the protect status structure of of E2E profile P05
 * \param   uint8                   *DataPtr    Pointer to the byte sequence within the Data to protect
 * \param   uint16                  Length      Length of the data in bytes
 * \return  Std_ReturnType                      Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P05Protect(const E2E_P05ConfigType *ConfigPtr, E2E_P05ProtectStateType *StatePtr,
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
             (Length != (ConfigPtr->DataLength / 8U)))
    {
        error = E2E_E_INPUTERR_WRONG;
    }
    else
    {
        /* convert offset in bits to offset in bytes */
        offset_u16 = ConfigPtr->Offset / 8U;

        /* Write counter into the data stream */
        E2E_Prv_P05WriteU8(DataPtr, offset_u16 + E2E_P05_COUNTER_OFFSET, StatePtr->Counter);
        
        /* Calculate CRC over data using CRC16 (standard version of the CRC AUTOSAR library */
        crc_u16 = E2E_Prv_P05CalcCRC16(ConfigPtr, DataPtr, Length);

        /* Write CRC into the specified region of data */
        E2E_Prv_P05WriteU16(DataPtr, offset_u16, crc_u16);

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
 * E2E_P05CheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E profile P05
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
 * \param   E2E_P05CheckStateType     *StatePtr       Pointer to the check status structure of of E2E profile P05
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P05CheckInit(E2E_P05CheckStateType *StatePtr)
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
        StatePtr->Status = E2E_P05STATUS_ERROR;

        /* no error occurred */
        error = E2E_E_OK;
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P05Check
 *
 * \brief End to end communication check routine corresponding to the profile P05 specified by AUTOSAR
 *
 * Description: Check function of E2E profile P05
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
 * \param   E2E_P05ConfigType       *ConfigPtr      Pointer to the configuration structure of E2E profile P05
 * \param   E2E_P05CheckStateType   *StatePtr       Pointer to the check status structure of of E2E profile P05
 * \param   uint8                   *DataPtr        Pointer to the byte sequence within the Data to be checked
 * \param   uint16                  Length          Length of the data in bytes
 * \return  Std_ReturnType                          Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P05Check(const E2E_P05ConfigType *ConfigPtr, E2E_P05CheckStateType *StatePtr,
                            const uint8 *DataPtr, uint16 Length)
{
    Std_ReturnType error = E2E_E_INTERR;
    boolean IsDataOk;
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
        IsDataOk = E2E_Prv_P05CheckData(ConfigPtr, DataPtr, Length);
        
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
    
                /* Read Counter from Data */
                recCounter_u8 = E2E_Prv_P05ReadU8(DataPtr, offset_u16 + E2E_P05_COUNTER_OFFSET); 
    
                /* Read Crc16 from Data */
                recCrc_u16 = E2E_Prv_P05ReadU16(DataPtr, offset_u16);
    
                 /* Calculate crc over data stream and DataID */
                 crc_u16 = E2E_Prv_P05CalcCRC16(ConfigPtr, DataPtr, Length); 
    
                /* Check for correctness of crc and length (profile 06 only) transmitted with the data */
                if ((crc_u16 != recCrc_u16))
                {
                    StatePtr->Status = E2E_P05STATUS_ERROR;
                }
                /* Crc OK: Received and calculated Crc is the same */
                else
                {
                    E2E_Prv_P05DoCheck(StatePtr, ConfigPtr, recCounter_u8);
                }
            }
            /* no new data available */
            else
            {
                StatePtr->Status = E2E_P05STATUS_NONEWDATA;
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
 * E2E_P05MapStatusToSM
 *
 * \brief Map profile P05 check status to generic state machine status
 *
 * Description:
 * The function maps the check status of profile P05 to a generic check status, which can be used by E2E state machine
 * check function. The E2E profile P05 delivers a more fine-granular status, but this is not relevant for the E2E
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
 * \param   Std_ReturnType            CheckReturn     Return value of the E2E_P05Check function
 * \param   E2E_P05CheckStatusType    Status          Status determined by E2E_P05Check function
 * \return  E2E_PCheckStatusType                      Profile-independent status of the reception on one single Data
 *                                                    in one cycle
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
E2E_PCheckStatusType E2E_P05MapStatusToSM(Std_ReturnType CheckReturn, E2E_P05CheckStatusType Status)
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
            case E2E_P05STATUS_OK:
            case E2E_P05STATUS_OKSOMELOST:
            {
                checkStatus_en = E2E_P_OK;
            }
            break;

            case E2E_P05STATUS_NONEWDATA:
            {
                checkStatus_en = E2E_P_NONEWDATA;
            }
            break;

            case E2E_P05STATUS_WRONGSEQUENCE:
            {
                checkStatus_en = E2E_P_WRONGSEQUENCE;
            }
            break;

            case E2E_P05STATUS_ERROR:
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;

            case E2E_P05STATUS_REPEATED:
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
