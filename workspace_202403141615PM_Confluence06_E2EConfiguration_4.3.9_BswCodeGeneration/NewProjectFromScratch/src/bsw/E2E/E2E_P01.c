

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_P01.h"

/**
 ***********************************************************************************************************************
 * E2E_P01ProtectInit
 *
 * \brief End to end communication protection state initialization
 *
 * Description: Protection initialization function of E2E profile P01
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
 * \param   E2E_P01ProtectStateType  *StatePtr       Pointer to the protect status structure of of E2E profile P01
 * \return  Std_ReturnType                           Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType *StatePtr)
{
    Std_ReturnType error = E2E_E_INTERR;

    
    /* TRACE[SWS_E2E_00386] */
    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* TRACE[SWS_E2E_00386] */
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
 * E2E_P01Protect
 *
 * \brief End to end communication protection routine corresponding to the profile P01 specified by AUTOSAR
 *
 * Description: Protection function of E2E profile P01
 * - Checks the pointer of the input variables
 * - Checks the range of the input variables
 * - Writes the current counter value (message sequence number) into the data stream
 * - Calculates a crc over the data stream to protect and writes the result into the data stream
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
 * \param   E2E_P01ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P01
 * \param   E2E_P01ProtectStateType *StatePtr   Pointer to the protect status structure of of E2E profile P01
 * \param   uint8                   *DataPtr    Pointer to the byte sequence within the Data to protect
 * \return  Std_ReturnType                      Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"

Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType *ConfigPtr, E2E_P01ProtectStateType *StatePtr,
                              uint8 *DataPtr)
{
    /* TRACE[SWS_E2E_00195]*/
    /* Initialize internal error */
    Std_ReturnType error = E2E_E_INTERR;
    boolean IsStateOk;


    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr) || (NULL_PTR == DataPtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input variables */
    else 
    {
        IsStateOk = E2E_Prv_P01CheckProtectStateType(StatePtr);
        
        if (!IsStateOk)
        {
            error = E2E_E_INPUTERR_WRONG;
        }
        else
        {
            /* Increase of the counter in data with respect to the configured offset */
            E2E_Prv_P01WriteCounter(ConfigPtr, StatePtr, DataPtr);
    
            /* add the low nibble of the dataID in case of Profile 1c */
            E2E_Prv_P01SetDataIdNibble(ConfigPtr, DataPtr);
            
            /* Calculate CRC over data using CRC-8-SAE J1850 (standard version of the CRC AUTOSAR library and
               write CRC into the specified region of data */
            DataPtr[ConfigPtr->CRCOffset / 8U] = E2E_Prv_P01CalcCRC8(ConfigPtr, StatePtr->Counter, DataPtr);
    
            /* Increments the sender status counter for the next message to send */
            if (StatePtr->Counter < E2E_P01_MAXCOUNTER)
            {
                StatePtr->Counter++;
            }
            else
            {
                /* Max counter value already reached reset counter */
                StatePtr->Counter = 0U;
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
 * E2E_P01CheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E profile P01
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
 * \param   E2E_P01CheckStateType     *StatePtr       Pointer to the check status structure of of E2E profile P01
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType *StatePtr)
{
    /* TRACE[SWS_E2E_00389] */
    Std_ReturnType error = E2E_E_INTERR;


    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* Initialize Check state */
        StatePtr->LastValidCounter = 0U;
        StatePtr->MaxDeltaCounter = 0U;
        StatePtr->WaitForFirstData = TRUE;
        StatePtr->NewDataAvailable = TRUE;
        StatePtr->LostData = 0U;
        StatePtr->Status = E2E_P01STATUS_NONEWDATA;
        StatePtr->SyncCounter = 0U;
        StatePtr->NoNewOrRepeatedDataCounter = 0U;

        /* no error occurred */
        error = E2E_E_OK;
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_P01Check
 *
 * \brief End to end communication check routine corresponding to the profile P01 specified by AUTOSAR
 *
 * Description: Check function of E2E profile P01
 * - Checks the pointer of the input values for NULL,
 * - Checks the range of the input values
 *   These two first check lead to a interrupt of the check routine when the result is FALSE.
 *   Otherwise the next checks are performed:
 * - Calculates the tolerable maximum loss of messages
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
 * \param   E2E_P01ConfigType       *ConfigPtr      Pointer to the configuration structure of E2E profile P01
 * \param   E2E_P01CheckStateType   *StatePtr       Pointer to the check status structure of of E2E profile P01
 * \param   uint8                   *DataPtr        Pointer to the byte sequence within the Data to be checked
 * \return  Std_ReturnType                          Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"

Std_ReturnType E2E_P01Check(const E2E_P01ConfigType *ConfigPtr, E2E_P01CheckStateType *StatePtr,
                            const uint8 *DataPtr)
{
    /* TRACE[SWS_E2E_00196] */
    Std_ReturnType error;
    uint8          recCounter_u8;           /* counter value received via data stream */
    uint8          recCrc_u8;               /* crc result received via data stream */
    uint8          recDataIDNibble_u8 = 0;  /* data Id lower nibble get from the data stream */
    uint8          crc_u8;                  /* calculated crc result from data stream */

    error = E2E_Prv_P01VerifyCheckInput(ConfigPtr, StatePtr, DataPtr);
    
    if(error == E2E_E_OK)
    {
        /* Increases StatePtr->MaxDeltaCounter until the maximum is reached */
        E2E_Prv_IncreaseCounter(&StatePtr->MaxDeltaCounter, E2E_P01_MAXCOUNTER);

        /* Check for new data available */
        if (FALSE != StatePtr->NewDataAvailable)
        {
            /* Reads Counter in the data stream with respect to the configured CounterOffset */
            recCounter_u8 = E2E_Prv_P01ReadCounter(ConfigPtr, DataPtr);

            /* Check the received counter value */
            if (recCounter_u8 < (E2E_P01_MAXCOUNTER + 1U))
            {
                /* Reads the crc result in the data stream with the configured CRCOffset */
                recCrc_u8 = DataPtr[ConfigPtr->CRCOffset / 8U];

                /* get the low nibble of the dataID in case of Profile 1c */
                E2E_Prv_P01GetDataIdNibble(ConfigPtr, &recDataIDNibble_u8, DataPtr);
                
                /* Calculate the crc result of the data stream using CRC-8-SAE J1850 polynomial */
                crc_u8 = E2E_Prv_P01CalcCRC8(ConfigPtr, recCounter_u8, DataPtr);
    

                /* Check for Correctness of CRC */
                /* bug in AUTOSAR specification (additional mask added) */
                if ((crc_u8 != recCrc_u8) ||
                    ((E2E_P01_DATAID_NIBBLE == ConfigPtr->DataIDMode) &&
                     (recDataIDNibble_u8 != ((uint8)(ConfigPtr->DataID >> 8U) & (uint8)E2E_MASK_L_NIBBLE))))
                {
                    /* CRC calculated over data is not identical to the Received CRC */
                    StatePtr->Status = E2E_P01STATUS_WRONGCRC;
                }
                /* Crc OK: Received and calculated Crc is the same / DataIdNibble is correct */
                else
                {
                    E2E_Prv_P01DoCheck(ConfigPtr, StatePtr, recCounter_u8);
                }
            }
            /* invalid received counter value */
            else
            {
                error = E2E_E_INPUTERR_WRONG;
            }
        }
        /* no new data available */
        else
        {
            E2E_Prv_IncreaseCounter(&StatePtr->NoNewOrRepeatedDataCounter, E2E_P01_MAXCOUNTER);

            StatePtr->Status = E2E_P01STATUS_NONEWDATA;
        }
   }
    
    return error;
}

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"


/**
 ***********************************************************************************************************************
 * E2E_P01MapStatusToSM
 *
 * \brief Map profile P01 check status to generic state machine status
 *
 * Description:
 * The function maps the check status of profile P01 to a generic check status, which can be used by E2E state machine
 * check function. The E2E profile P01 delivers a more fine-granular status, but this is not relevant for the E2E
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
 * \param   Std_ReturnType            CheckReturn     Return value of the E2E_P01Check function
 * \param   E2E_P01CheckStatusType    Status          Status determined by E2E_P01Check function
 * \param   boolean                   profileBehavior FALSE: check has the legacy behavior, before R4.2
 *                                                    TRUE: check behaves like new P4/P5/P6 profiles introduced in R4.2
 * \return  E2E_PCheckStatusType                      Profile-independent status of the reception on one single Data
 *                                                    in one cycle
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
E2E_PCheckStatusType E2E_P01MapStatusToSM(Std_ReturnType CheckReturn, E2E_P01CheckStatusType Status,
                                          boolean profileBehavior)
{
    /* report an ERROR as default */
    E2E_PCheckStatusType checkStatus_en = E2E_P_ERROR;

 /* TRACE[SWS_E2E_00383], TRACE[SWS_E2E_00476] */
    /* Check if return value is correct */    
    if (E2E_E_OK == CheckReturn)
    {
        /* Evaluate Profile status */
        switch (Status)
        {
            case E2E_P01STATUS_OK:
            case E2E_P01STATUS_OKSOMELOST:
            {
                checkStatus_en = E2E_P_OK;
            }
            break;
                
            case E2E_P01STATUS_WRONGCRC:
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;
                
            case E2E_P01STATUS_REPEATED:
            {
                checkStatus_en = E2E_P_REPEATED;
            }
            break;
                
            case E2E_P01STATUS_NONEWDATA:
            {
                checkStatus_en = E2E_P_NONEWDATA;
            }
            break;
                
            case E2E_P01STATUS_WRONGSEQUENCE:
            {
                checkStatus_en = E2E_P_WRONGSEQUENCE;
            }
            break;
            
            case E2E_P01STATUS_INITIAL:
            {
                checkStatus_en = (TRUE == profileBehavior) ? E2E_P_WRONGSEQUENCE : E2E_P_OK;
            }
            break;

            case E2E_P01STATUS_SYNC:
            {
                checkStatus_en = (FALSE == profileBehavior) ? E2E_P_WRONGSEQUENCE : E2E_P_OK;
            }
            break;
    
            default: /* Status has an invalid value */
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;
        }
    }

    return (checkStatus_en); /* TRACE[SWS_E2E_00384] */
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
