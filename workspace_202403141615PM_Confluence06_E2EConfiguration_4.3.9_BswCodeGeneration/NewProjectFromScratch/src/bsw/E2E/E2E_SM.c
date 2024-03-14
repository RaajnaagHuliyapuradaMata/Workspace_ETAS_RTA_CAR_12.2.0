

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "E2E.h"
#include "E2E_Prv.h"
#include "E2E_Prv_SM.h"
/**
 ***********************************************************************************************************************
 * E2E_SMCheckInit
 *
 * \brief End to end communication check state initialization
 *
 * Description: Check initialization function of E2E state machine
 * - Checks the pointer of the input values for NULL,
 * - Sets all variables in state structure with a default value
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
 * \param   E2E_SMCheckStateType     *StatePtr        Pointer to communication state structure
 * \param   E2E_SMConfigType         *ConfigPtr       Pointer to configuration of the state machine
 * \return  Std_ReturnType                            Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
/* TRACE[SWS_E2E_00354] , TRACE[SWS_E2E_00375] , TRACE[SWS_E2E_00353] */
Std_ReturnType E2E_SMCheckInit(E2E_SMCheckStateType *StatePtr, const E2E_SMConfigType * const ConfigPtr)
{
    uint32_least   i;
    Std_ReturnType error = E2E_E_INTERR;

    /* TRACE[SWS_E2E_00370] */
    /* Check for NULL pointer */
    if ((NULL_PTR == StatePtr) || (NULL_PTR == ConfigPtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check if status window pointer is a valid pointer */
    else if (NULL_PTR == StatePtr->ProfileStatusWindow)
    {
        error = E2E_E_INPUTERR_WRONG;
    }
    else
    {
        /* Clear buffer used as window for Profile status storage */
        for (i = 0U; i < ConfigPtr->WindowSize; i++)
        {
            StatePtr->ProfileStatusWindow[i] = (uint8)E2E_P_NOTAVAILABLE;
        }

        /* Initialize Check state structure parameters */
        StatePtr->WindowTopIndex = 0U;
        StatePtr->OkCount = 0U;
        StatePtr->ErrorCount = 0U;
        StatePtr->SMState = E2E_SM_NODATA;

        /* no error occurred */
        error = E2E_E_OK;
    }

    return (error);
}
#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/**
 ***********************************************************************************************************************
 * E2E_SMCheck
 *
 * \brief End to end communication generic state check with status history handling
 *
 * Description:
 * Checks the communication channel. It determines if the data can be used for safety-related application, based on
 * history of checks performed by a corresponding E2E_P0XCheck() function.
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
 * \param   E2E_PCheckStatusType    ProfileStatus       Profile-independent status  of  the  reception on  one  single
 *                                                      Data in one cycle
 * \param   E2E_SMConfigType        *ConfigPtr          Pointer to static configuration.
 * \param   E2E_SMCheckStateType    *StatePtr           Pointer to port/data communication state.

 * \return  Std_ReturnType                              Error Flags
 *
 ***********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"
Std_ReturnType E2E_SMCheck(E2E_PCheckStatusType ProfileStatus, const E2E_SMConfigType * const ConfigPtr,
                           E2E_SMCheckStateType *StatePtr)
{
    Std_ReturnType error;

    /* TRACE[SWS_E2E_00354] , TRACE[SWS_E2E_00345] , TRACE[SWS_E2E_00340] , TRACE[SWS_E2E_00371]*/
    error = E2E_Prv_SMCheckConfigAndState(ConfigPtr, StatePtr);
    /* Check for NULL pointer */
    if (error == E2E_E_OK)
    {
        /* if in a state where window used then process received profile status */
        if ((E2E_SM_VALID == StatePtr->SMState) ||
            (E2E_SM_INIT == StatePtr->SMState) ||
            (E2E_SM_INVALID == StatePtr->SMState))
        {
            /* check the actual state and do something if the state to be shifted out of the window is different */
            if ((uint8)E2E_P_OK == (uint8)ProfileStatus)
            {
                /* do nothing if actual state and the shifted out state are equal */
                StatePtr->OkCount+= ((uint8)E2E_P_OK != StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex]) ? 1U : 0U;
                StatePtr->ErrorCount-= ((uint8)E2E_P_ERROR == StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex]) ? 1U : 0U; 
            }
            else if ((uint8)E2E_P_ERROR == (uint8)ProfileStatus)
            {
                /* do nothing if actual state and the shifted out state are equal */
                StatePtr->OkCount-= ((uint8)E2E_P_OK == StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex]) ? 1U : 0U;
                StatePtr->ErrorCount+= ((uint8)E2E_P_ERROR != StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex]) ? 1U : 0U; 
            }
            else
            {
                /* check if OkCount or ErrorCount has to be decreased (depends on the last state within the ringbuffer
                   because the new state is not E2E_P_OK and not E2E_P_ERROR */
                StatePtr->OkCount-= ((uint8)E2E_P_OK == StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex]) ? 1U : 0U;
                StatePtr->ErrorCount-= ((uint8)E2E_P_ERROR == StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex]) ? 1U : 0U; 
            }
            /* Store current profile status in Profile Status Window (overwrite the last state) */
            StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex] = (uint8)ProfileStatus;
            /* Increment window index, if end reached then reset index back to zero */
            /* WindowTopIndex is now a pointer to the value we have to shift out of the window in the next step */
            StatePtr->WindowTopIndex = (StatePtr->WindowTopIndex < (ConfigPtr->WindowSize - 1U)) ? (StatePtr->WindowTopIndex + 1U) : 0U;
        }
        /* Based on the statuses and SM state update SM state */
        switch (StatePtr->SMState)
        {
            case E2E_SM_VALID:
            {
                /* Check error and ok counts against limits */
                StatePtr->SMState = ((StatePtr->ErrorCount > ConfigPtr->MaxErrorStateValid) ||
                                    (StatePtr->OkCount < ConfigPtr->MinOkStateValid)) ? E2E_SM_INVALID : E2E_SM_VALID;
                /* no error occurred */
                error = E2E_E_OK;
                break;
            }
            case E2E_SM_DEINIT:
            {
                /* E2E is in a deinitialized state report state error */
                error = E2E_E_WRONGSTATE;
                break;
            }
            case E2E_SM_NODATA:
            {
                StatePtr->SMState = ((E2E_P_ERROR != ProfileStatus) && (E2E_P_NONEWDATA != ProfileStatus)) ? 
                E2E_SM_INIT : E2E_SM_NODATA;
                /* no error occurred */
                error = E2E_E_OK;
                break;
            }
            case E2E_SM_INIT:
            {
                /* Check error and ok counts against limits */
                if ((StatePtr->ErrorCount <= ConfigPtr->MaxErrorStateInit) &&
                    (StatePtr->OkCount >= ConfigPtr->MinOkStateInit))
                {
                    /* Ok limit reached enter valid state */
                    StatePtr->SMState = E2E_SM_VALID;
                }
                else
                {
                    StatePtr->SMState = (StatePtr->ErrorCount > ConfigPtr->MaxErrorStateInit) ? E2E_SM_INVALID : E2E_SM_INIT;
                }
                /* no error occurred */
                error = E2E_E_OK;
                break;
            }
            default: /* This state E2E_SM_INVALID is only remaining, because of previous checks */
            {
                StatePtr->SMState = ((StatePtr->ErrorCount <= ConfigPtr->MaxErrorStateInvalid) &&
                    (StatePtr->OkCount >= ConfigPtr->MinOkStateInvalid)) ? E2E_SM_VALID : E2E_SM_INVALID;
                error = E2E_E_OK; /* no error occurred */
                break;
            }
        }
    }
    return (error);
}

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

