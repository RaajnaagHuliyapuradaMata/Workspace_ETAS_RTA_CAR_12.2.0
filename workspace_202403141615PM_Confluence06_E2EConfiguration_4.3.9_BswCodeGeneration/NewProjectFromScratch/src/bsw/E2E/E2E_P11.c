

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_P11.h"

/**
 ***********************************************************************************************************************
 * E2E_P11ProtectInit
 *
 * \brief End to end communication protection state initialization
 *
 * Description: Protection initialization function of E2E profile P11
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
 * \param   E2E_P11ProtectStateType  *StatePtr       Pointer to the protect status structure of of E2E profile P11
 * \return  Std_ReturnType                           Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P11ProtectInit(E2E_P11ProtectStateType *StatePtr)
{
    Std_ReturnType error_u8 = E2E_E_OK;
    
    
    /* TRACE [SWS_E2E_00555] */
    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error_u8 = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* Initialize Protect state */
        StatePtr->Counter = 0U;

    }

    return (error_u8);
}


/**
 ***********************************************************************************************************************
 * E2E_P11Protect
 *
 * \brief End to end communication protection routine corresponding to the profile P11 specified by AUTOSAR
 *
 * Description: Protection function of E2E profile P11
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
 * \param   E2E_P11ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P11
 * \param   E2E_P11ProtectStateType *StatePtr   Pointer to the protect status structure of of E2E profile P11
 * \param   uint8                   *DataPtr    Pointer to the byte sequence within the Data to protect
 * \param   uint16                   Length     Length of data. 
 * 
 ***********************************************************************************************************************
 */
/* TRACE [SWS_E2E_00509] */
Std_ReturnType E2E_P11Protect(const E2E_P11ConfigType *ConfigPtr, E2E_P11ProtectStateType *StatePtr,
                              uint8 *DataPtr, uint16 Length)
{
    
    Std_ReturnType error_u8 = E2E_E_OK;

    /* TRACE [SWS_E2E_00510] */
    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr) || (NULL_PTR == DataPtr))
    {
            error_u8 = E2E_E_INPUTERR_NULL;
    }
    else if (
                 (Length != (ConfigPtr->DataLength / 8U)))
    {
            error_u8 = E2E_E_INPUTERR_WRONG;
    }
    else
    {    
         /* TRACE [SWS_E2E_00511] , TRACE [SWS_E2E_00505], TRACE [SWS_E2E_00506]  */
        /* add the low nibble of the dataID */
        E2E_Prv_P11WriteDataIdNibble(ConfigPtr, DataPtr);
        
        /* TRACE [SWS_E2E_00512] */
        /* Increase of the counter in data with respect to the configured offset */
        E2E_Prv_P11WriteCounter(ConfigPtr, StatePtr, DataPtr);
         /* TRACE [SWS_E2E_00513] , TRACE [SWS_E2E_00514] */
         /* Calculate CRC over data using CRC-8-SAE J1850 (standard version of the CRC AUTOSAR library and
          write CRC into the specified region of data */
         DataPtr[ConfigPtr->CRCOffset / 8U] = E2E_Prv_P11CalcCRC8(ConfigPtr, DataPtr);
         /* TRACE [SWS_E2E_00515] , TRACE [SWS_E2E_00504] */
         /* Increments the sender status counter for the next message to send */
         /* MR12 Rule 10.3,10.4 Violation: Both sides of the expression deals with
            the same struct element. Does not lead to any functional issue */
         StatePtr->Counter = (StatePtr->Counter + 1) % E2E_P11_BEYONDMAXCOUNTER;
        
    }
    
    return (error_u8);
        
}

/**
 ***********************************************************************************************************************
 * E2E_P11CheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E profile P11
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
 * \param   E2E_P11CheckStateType     *StatePtr       Pointer to the check status structure of of E2E profile P11
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/

Std_ReturnType E2E_P11CheckInit(E2E_P11CheckStateType *StatePtr)
{

    Std_ReturnType error_u8 = E2E_E_OK;

     /* TRACE[SWS_E2E_00556] */
    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error_u8 = E2E_E_INPUTERR_NULL;
    }
    else
    {
        StatePtr->Counter = 0xEU;
        StatePtr->Status = E2E_P11STATUS_ERROR;

    }

    return (error_u8);
}

/**
 ***********************************************************************************************************************
 * E2E_P11Check
 *
 * \brief End to end communication check routine corresponding to the profile P11 specified by AUTOSAR
 *
 * Description: Check function of E2E profile P11
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
 * \param   E2E_P11ConfigType       *ConfigPtr      Pointer to the configuration structure of E2E profile P11
 * \param   E2E_P11CheckStateType   *StatePtr       Pointer to the check status structure of of E2E profile P11
 * \param   uint8                   *DataPtr        Pointer to the byte sequence within the Data to be checked
 * \param   uint16                   Length         Length of data
 * \return  Std_ReturnType                          Error Flags
 *
 ***********************************************************************************************************************
*/


/* TRACE [SWS_E2E_00516] */
Std_ReturnType E2E_P11Check(const E2E_P11ConfigType *ConfigPtr, E2E_P11CheckStateType *StatePtr,
                            const uint8 *DataPtr, uint16 Length)
{
    Std_ReturnType error_u8 = E2E_E_OK;
    E2E_P11ContextType Context;
    Context.recDataIDNibble_u8 = 0;  /* data Id lower nibble get from the data stream */
    
    /* TRACE [SWS_E2E_00517] */
    /* Check for NULL pointer */
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr))
    {
        error_u8 = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input data */
    else if ((((DataPtr != NULL_PTR) && (Length != 0U)) || ((DataPtr == NULL_PTR) && (Length == 0U))))
    {
        if (NULL_PTR != DataPtr)
        {
    /* MR12 Rule 10.4 Violation: Both Length and DataLength variables are of type uint16. Does not lead to any functionality loss */
            if (Length == (ConfigPtr->DataLength/8))
            {
                /* TRACE [SWS_E2E_00582] */
                /* Read DataID from Data */
                E2E_Prv_P11GetDataIdNibble(ConfigPtr, &Context.recDataIDNibble_u8, DataPtr);
                
                /* TRACE [SWS_E2E_00518] */
                /* Reads Counter in the data stream with respect to the configured CounterOffset*/
                Context.recCounter_u8 = E2E_Prv_P11ReadCounter(ConfigPtr, DataPtr);
                
                /* TRACE [SWS_E2E_00519] */
                /* Read Crc from Data  */
                Context.recCrc_u8 = E2E_Prv_P11ReadCrc(DataPtr,ConfigPtr);
                /* TRACE [SWS_E2E_00513] */
                /* Calculate crc over data stream and DataID*/
                Context.crc_u8 = E2E_Prv_P11CalcCRC8(ConfigPtr, DataPtr);

                 /* TRACE [SWS_E2E_00521] */
                 E2E_Prv_P11DoCheck(StatePtr, ConfigPtr, &Context);

            }
            else
            {
                error_u8 = E2E_E_INPUTERR_WRONG;
            }
        }
         /* no new data available */
        else
        {
            StatePtr->Status = E2E_P11STATUS_NONEWDATA;
        }

    }
    else
    {
        error_u8 = E2E_E_INPUTERR_WRONG;
    }

    return (error_u8);
}

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"


/**
 ***********************************************************************************************************************
 * E2E_P11MapStatusToSM
 *
 * \brief Map profile P11 check status to generic state machine status
 *
 * Description:
 * The function maps the check status of profile P11 to a generic check status, which can be used by E2E state machine
 * check function. The E2E profile P11 delivers a more fine-granular status, but this is not relevant for the E2E
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
 * \param   Std_ReturnType            CheckReturn     Return value of the E2E_P11Check function
 * \param   E2E_P11CheckStatusType    Status          Status determined by E2E_P11Check function
 * 
 * \return  E2E_PCheckStatusType                      Profile-independent status of the reception on one single Data
 *                                                    in one cycle
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
E2E_PCheckStatusType E2E_P11MapStatusToSM(Std_ReturnType CheckReturn, E2E_P11CheckStatusType Status)
{
    /* report an ERROR as default */
    E2E_PCheckStatusType checkStatus_en = E2E_P_ERROR;

 /* TRACE[SWS_E2E_00574], TRACE[SWS_E2E_00557] */
    /* Check if return value is correct */    
    if (E2E_E_OK == CheckReturn)
    {
        /* Evaluate Profile status */
        switch (Status)
        {
            case E2E_P11STATUS_OK:
            case E2E_P11STATUS_OKSOMELOST:
            {
                checkStatus_en = E2E_P_OK;
            }
            break;
                
            case E2E_P11STATUS_ERROR:
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;
                
            case E2E_P11STATUS_REPEATED:
            {
                checkStatus_en = E2E_P_REPEATED;
            }
            break;
                
            case E2E_P11STATUS_NONEWDATA:
            {
                checkStatus_en = E2E_P_NONEWDATA;
            }
            break;
                
            case E2E_P11STATUS_WRONGSEQUENCE:
            {
                checkStatus_en = E2E_P_WRONGSEQUENCE;
            }
            break;
    
            default: /* Status has an invalid value */
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;
        }
     }
       
/* TRACE[SWS_E2E_00558] */
    return (checkStatus_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
