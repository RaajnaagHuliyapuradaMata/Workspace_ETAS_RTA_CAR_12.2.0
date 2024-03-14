/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "ComM_Priv.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/


#if (COMM_PNC_ENABLED != STD_OFF)
LOCAL_INLINE ComM_ModeType ComM_Prv_GetPncMode (ComM_PncModeType pncState_en);
#endif /* COMM_PNC_ENABLED != STD_OFF */

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/


#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

/********************************************************************************************
 * Function name    : ComM_GetCurrentPNCComMode
 * Syntax           : Std_ReturnType ComM_GetCurrentPNCComMode ( ComM_UserHandleType User, ComM_ModeType* ComMode )
 * Description      : The function returns the current Communication Mode of the corresponding PNC the affected
 *                    user is assigned to.
 * Parameters       : User -> Handle of the user who requests a mode
 *                    ComMode -> ComM_ModeType
 * Return value     : E_OK:Succesfully returned Communication Mode from Bus State Manager
 *                    E_NOT_OK: Return of Communication Mode from Bus State Manager failed
 *                    COMM_E_MULTIPLE_PNC_ASSIGNED : Function could not provide the current mode of the PNC,
 *                    since multiple PNCs are assigned to the affected user
 *                    COMM_E_NO_PNC_ASSIGNED : Function could not provide the current mode of the PNC, since no
 *                    PNC is assigned to the affected user
 *                    COMM_E_UNINIT: ComM not initialized
 **********************************************************************************************/

/* MR12 RULE 8.3, 8.13 VIOLATION: 8.3 Two declarations exists for this api. One is in ComM.h &
 * another generated by RTE.
 * Hence, the difference in identifiers.
 * 8.13 AUTOSAR SWS specification signature is as below, hence suppressed */
Std_ReturnType ComM_GetCurrentPNCComMode(ComM_UserHandleType User, ComM_ModeType * ComMode)
{
    /* Local variable declaration */
#if (COMM_PNC_ENABLED != STD_OFF)
    const ComM_UsersType_tst *          userConfigPtr_pcst;
    uint8                               userId_internal_u8;
    uint8                               pncId_u8;
    uint8                               numPncs_u8;
    ComM_PncRamStructType_tst *         pncRamPtr_pst;
    ComM_PncModeType                    pncState_en;
#endif /* COMM_PNC_ENABLED != STD_OFF */

    /***********************************************DET STARTS HERE****************************************/
    /* DET check for ComM not initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_CUR_PNCCOMMODE_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }

    /* DET check for pointer passed to be a NULL pointer */
    if (ComMode == NULL_PTR)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_CUR_PNCCOMMODE_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }

    /* DET  for Invalid user */
    if ( ComM_Prv_ValidateUserId(User) == FALSE)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_CUR_PNCCOMMODE_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }
    /*************************************DET ENDS HERE*************************************************/

    /* Local variable initialization */
#if (COMM_PNC_ENABLED != STD_OFF)
    userId_internal_u8 = ComM_UserId_MappingTable_acst[User];
    userConfigPtr_pcst = &(COMM_GET_USER_LIST[userId_internal_u8]);
    numPncs_u8         = userConfigPtr_pcst->NumPnc_u8;

    /* Return COMM_E_MULTIPLE_PNC_ASSIGNED if multiple PNCs are assigned to the affected user */
    if(numPncs_u8 > 0x01u)
    {
        return COMM_E_MULTIPLE_PNC_ASSIGNED;
    }

    /*  Return COMM_E_NO_PNC_ASSIGNED if no PNC is assigned to the affected user */
    else if(numPncs_u8 < 0x01u)
#endif /* COMM_PNC_ENABLED != STD_OFF */
    {
        return COMM_E_NO_PNC_ASSIGNED;
    }
#if (COMM_PNC_ENABLED != STD_OFF)
    else
    {
        /* Update the current Communication Mode of the corresponding PNC the affected user is assigned to*/
        pncId_u8 = (userConfigPtr_pcst->Pncs_pcu8[0]);
        pncRamPtr_pst = &ComM_PncRamStruct[pncId_u8];
        pncState_en = pncRamPtr_pst->PncState_en;

        *ComMode = ComM_Prv_GetPncMode(pncState_en);

        return E_OK;
    }
#endif /* COMM_PNC_ENABLED != STD_OFF */
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"


#if (COMM_PNC_ENABLED != STD_OFF)
/* ComM_Prv_GetPncMode */
/**************************************************************************************
 * Function name    : ComM_Prv_GetPncMode
 * Syntax           : ComM_Prv_GetPncMode
 *                    (
 *                    ComM_PncModeType pncState_en
 *                    )
 * Description      : Function to give mode corresponding to a state
 * Parameters       : pncState_en
 * Return value     : ComM_ModeType
 **************************************************************************************/

LOCAL_INLINE ComM_ModeType ComM_Prv_GetPncMode (ComM_PncModeType pncState_en)
{
    ComM_ModeType pncMode_u8;
    pncMode_u8 = COMM_NO_COMMUNICATION;

    switch(pncState_en)
    {
        case COMM_PNC_NO_COMMUNICATION :
        {
            pncMode_u8 = COMM_NO_COMMUNICATION;
        }
        break;

        case COMM_PNC_REQUESTED :
        case COMM_PNC_READY_SLEEP :
        case COMM_PNC_PREPARE_SLEEP :
        {
            pncMode_u8 = COMM_FULL_COMMUNICATION;
        }
        break;

        default:
        {
            /*Invalid mode. Control should not come here */
        }
        break;
    }

    return pncMode_u8;
}
#endif /* COMM_PNC_ENABLED != STD_OFF */
