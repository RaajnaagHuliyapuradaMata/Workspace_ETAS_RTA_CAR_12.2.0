

#ifndef E2E_PRV_SM_H
#define E2E_PRV_SM_H

/*
 **********************************************************************************************************************
 E2E STATEMACHINE LOCAL DEFINES
 **********************************************************************************************************************
*/


/**
 ***********************************************************************************************************************
 * E2E_Prv_SMCheckConfigAndState
 *
 * \brief Tests the input parameter E2E_SMConfigType and E2E_SMCheckStateType for correctness
 *
 * Description: Tests the input parameter E2E_SMConfigType and E2E_SMCheckStateType for correctness
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
 * \param   E2E_SMConfigType       *ConfigPtr  pointer to the configuration structure of the state machine
 * \param   E2E_SMCheckStateType       *StatePtr  pointer to the state
 * \return  Std_ReturnType                      returns E2E_E_OK if the check is ok.
 *
 ***********************************************************************************************************************
 */

E2E_INLINE Std_ReturnType E2E_Prv_SMCheckConfigAndState(const E2E_SMConfigType * const ConfigPtr,
                                                   const E2E_SMCheckStateType * const StatePtr)
{
    Std_ReturnType error;
    
    if ((NULL_PTR == ConfigPtr) || (NULL_PTR == StatePtr))
    {
        error = E2E_E_INPUTERR_NULL;
    }
    /* Check for range of input data */
    else if (!(((ConfigPtr->WindowSize > 0U) &&
                      ((uint8)StatePtr->SMState <= (uint8)E2E_SM_INVALID) &&
                      (NULL_PTR != StatePtr->ProfileStatusWindow) &&
                      (StatePtr->WindowTopIndex < ConfigPtr->WindowSize))))
    {
        // a valid window size has to be at least 1
        // the state has to be initialized to valid value
        // the window needs to be on valid buffer pointer
        // window index counter must be in valid range
        error = E2E_E_INPUTERR_WRONG;
    }
    else
    {
        error = E2E_E_OK;
    }
                      
    return error;
}

/* E2E_PRV_SM_H */
#endif
