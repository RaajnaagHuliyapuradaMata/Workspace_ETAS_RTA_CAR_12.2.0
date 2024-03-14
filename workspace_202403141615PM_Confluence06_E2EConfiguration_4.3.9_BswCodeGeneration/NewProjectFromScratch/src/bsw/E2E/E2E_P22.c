

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_P22.h"

/**
 ***********************************************************************************************************************
 * E2E_P22ProtectInit
 *
 * \brief End to end communication protection state initialization
 *
 * Description: Protection initialization function of E2E profile P22
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
 * \param   E2E_P22ProtectStateType  *StatePtr       Pointer to the protect status structure of of E2E profile P22
 * \return  Std_ReturnType                      Error Flags
 *
 ***********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_P22ProtectInit(E2E_P22ProtectStateType *StatePtr)
{
    /* TRACE[SWS_E2E_00581] */
    Std_ReturnType error_u8;

    /* TRACE[SWS_E2E_00559] */
    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error_u8 = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* TRACE[SWS_E2E_00559] */
        /* Initialize Protect state */
        StatePtr->Counter = 0U;

        /* no error occurred */
        error_u8 = E2E_E_OK;
    }
    return (error_u8);
}

/**
 ***********************************************************************************************************************
 * E2E_P22Protect
 *
 * \brief End to end communication protection routine corresponding to the profile P22 specified by AUTOSAR
 *
 * Description: Protection function of E2E profile P22
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
 * \param   E2E_P22ConfigType       *ConfigPtr  Pointer to the configuration structure of E2E profile P22
 * \param   E2E_P22ProtectStateType *StatePtr   Pointer to the protect status structure of of E2E profile P22
 * \param   uint8                   *DataPtr    Pointer to the byte sequence within the Data to protect
 * \return  Std_ReturnType                      Error Flags
 *
 ***********************************************************************************************************************
 */

Std_ReturnType E2E_P22Protect(const E2E_P22ConfigType *ConfigPtr, E2E_P22ProtectStateType *StatePtr,
                              uint8 *DataPtr, uint16 Length)
{
    /* TRACE[SWS_E2E_00528] ,  TRACE[SWS_E2E_00580] */
    /* Initialize internal error */
    Std_ReturnType error_u8 = E2E_E_INTERR;
    uint16 offset_u16;          /* offset in bytes */
    uint8 crc_u8;

    /* Check for NULL pointer TRACE[SWS_E2E_00529]*/
    if ((ConfigPtr == NULL_PTR) || (StatePtr == NULL_PTR) || (DataPtr == NULL_PTR))
    {
        error_u8 = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input variables */
    else if(           (Length != (ConfigPtr->DataLength/8U)))
    {
        error_u8 = E2E_E_INPUTERR_WRONG; 
    }          
    else
    {
        /* offset in bytes */
        offset_u16 = ConfigPtr->Offset/8U;          
        
        /* TRACE[SWS_E2E_00530] */
        /* Write counter into the data stream */
        E2E_Prv_P22WriteU8(ConfigPtr, DataPtr, StatePtr->Counter);
        
        /* TRACE[SWS_E2E_00531] */
        /* Calculate CRC over data using CRC8H2F (standard version of the CRC AUTOSAR library */
        crc_u8 = E2E_Prv_P22CalcCRC8H2F(ConfigPtr, DataPtr, Length, StatePtr->Counter);
    
        /* TRACE[SWS_E2E_00532] */
        /* Write CRC into the specified region of data (big endian order with respect to bit order) */
        E2E_Prv_P22WriteCrcU8(DataPtr, offset_u16, crc_u8);
    
        /* TRACE[SWS_E2E_00533], TRACE[SWS_E2E_00523] */
        /* increment the counter (overflow means: counter restarted) */
        /* MR12 Rule 10.3,10.4 Violation: Type casting does not lead to any functional issues */
        StatePtr->Counter = (StatePtr->Counter + 1) % E2E_P22_BEYONDMAXCOUNTER;
        
        /* no error occurred */
        error_u8 = E2E_E_OK;
    }
    
    return (error_u8);
}

/**
 ***********************************************************************************************************************
 * E2E_P22CheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E profile P22
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
 * \param   E2E_P22CheckStateType     *StatePtr       Pointer to the check status structure of of E2E profile P22
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/

Std_ReturnType E2E_P22CheckInit(E2E_P22CheckStateType *StatePtr)
{
    /* TRACE[SWS_E2E_00578] */
    Std_ReturnType error_u8;
    
    /* TRACE[SWS_E2E_00560] */
    /* Check for NULL pointer */
    if (NULL_PTR == StatePtr)
    {
        error_u8 = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* TRACE[SWS_E2E_00560] */
        /* Initialize Check state */
        StatePtr->Counter = 0xF0U;
        StatePtr->Status = E2E_P22STATUS_ERROR;

        /* no error occurred */
        error_u8 = E2E_E_OK;
    }

    return (error_u8);
}

/**
 ***********************************************************************************************************************
 * E2E_P22Check
 *
 * \brief End to end communication check routine corresponding to the profile P22 specified by AUTOSAR
 *
 * Description: Check function of E2E profile P22
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
 * \param   E2E_P22ConfigType       *ConfigPtr      Pointer to the configuration structure of E2E profile P22
 * \param   E2E_P22CheckStateType   *StatePtr       Pointer to the check status structure of of E2E profile P22
 * \param   uint8                   *DataPtr        Pointer to the byte sequence within the Data to be checked
 * \param   uint16                   Length         Length of data
 * \return  Std_ReturnType                          Error Flags
 *
 ***********************************************************************************************************************
*/

Std_ReturnType E2E_P22Check(const E2E_P22ConfigType *ConfigPtr, E2E_P22CheckStateType *StatePtr,
                            const uint8 *DataPtr, uint16 Length)
{
    /* TRACE[SWS_E2E_00534] , TRACE[SWS_E2E_00539] , TRACE[SWS_E2E_00577]  */
    Std_ReturnType error_u8 = E2E_E_OK;
    uint16 offset_u16;          /* offset in bytes */
    E2E_P22ContextType Context;

    /* TRACE[SWS_E2E_00535] */
    /* Check for NULL pointer */
    if ((ConfigPtr == NULL_PTR) || (StatePtr == NULL_PTR))
    {
        error_u8 = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input data */
    else if ((( (DataPtr != NULL_PTR) && (Length != 0) ) || ( (DataPtr == NULL_PTR) && (Length == 0) )))
    {
        if (NULL_PTR != DataPtr)
        {
            if (Length == (ConfigPtr->DataLength/8U))
            {
                /* convert offset in bits to offset in bytes */
                offset_u16 = ConfigPtr->Offset / 8U;
            
                /* TRACE[SWS_E2E_00536] */
                /* Read Counter from Data  */
                Context.recCounter_u8 = E2E_Prv_P22ReadU8(DataPtr, ConfigPtr->Offset);
                
                /* TRACE[SWS_E2E_00537] */ 
                /* Read Crc8H2F from Data */
                Context.recCrc_u8 = E2E_Prv_P22ReadCrcU8(DataPtr, offset_u16);
                /* TRACE[SWS_E2E_00531] */
                /* Calculate crc over data stream */
                Context.crc_u8 = E2E_Prv_P22CalcCRC8H2F(ConfigPtr, DataPtr, Length, StatePtr->Counter);
                
                /* TRACE[SWS_E2E_00539] */
                E2E_Prv_P22DoCheck(StatePtr, ConfigPtr, &Context);

            }
            else
            {
                error_u8 = E2E_E_INPUTERR_WRONG;
            }
        }
        else
        {
            StatePtr->Status = E2E_P22STATUS_NONEWDATA;
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
 * E2E_P22MapStatusToSM
 *
 * \brief Map profile P22 check status to generic state machine status
 *
 * Description:
 * The function maps the check status of profile P22 to a generic check status, which can be used by E2E state machine
 * check function. The E2E profile P22 delivers a more fine-granular status, but this is not relevant for the E2E
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
 * \param   Std_ReturnType            CheckReturn     Return value of the E2E_P22Check function
 * \param   E2E_P22CheckStatusType    Status          Status determined by E2E_P22Check function
 * \return  E2E_PCheckStatusType                      Profile-independent status of the reception on one single Data
 *                                                    in one cycle
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
E2E_PCheckStatusType E2E_P22MapStatusToSM(Std_ReturnType CheckReturn, E2E_P22CheckStatusType Status)
{
/* TRACE[SWS_E2E_00561] */
    /* report an ERROR as default */
    E2E_PCheckStatusType checkStatus_en = E2E_P_ERROR;

/* TRACE[SWS_E2E_00562]*/
    /* Check if return value is correct */
    if (E2E_E_OK == CheckReturn)
    {
        /* Evaluate Profile status */
        switch (Status)
        {
            case E2E_P22STATUS_OK:
            case E2E_P22STATUS_OKSOMELOST:
            {
                checkStatus_en = E2E_P_OK;
            }
            break;

            case E2E_P22STATUS_NONEWDATA:
            {
                checkStatus_en = E2E_P_NONEWDATA;
            }
            break;

            case E2E_P22STATUS_WRONGSEQUENCE:
            {
                checkStatus_en = E2E_P_WRONGSEQUENCE;
            }
            break;

            case E2E_P22STATUS_ERROR:
            {
                checkStatus_en = E2E_P_ERROR;
            }
            break;

            case E2E_P22STATUS_REPEATED:
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
/* TRACE[SWS_E2E_00562] */
    return (checkStatus_en);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
