/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "ComM_Priv.h"

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Variables
 ***************************************************************************************************
 */

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"


/**************************************************************************************
 * Function name    : ComM_GetMaxComMode
 * Syntax           : Std_ReturnType ComM_GetMaxComMode
 *                   (
 *                     ComM_UserHandleType User, ComM_ModeType* ComMode
 *                     )
 * Description      : Function to query the maximum allowed Communication Mode of the corresponding
 *                    user.
 * Parameters       : User -> Handle of the user who requests a mode
 *                    ComMode ->  ComM_ModeType
 * Return value     : E_OK: Successfully returned maximum allowed Communication Mode
 *                    E_NOT_OK: Return of maximum allowed Communication Mode failed
 *                    COMM_E_UNINIT: ComM not initialized
 **************************************************************************************/
/* MR12 RULE 8.3 VIOLATION:Two declarations exists for this api. One is in ComM.h & another generated by RTE.
 * Hence,the difference in identifiers.*/
Std_ReturnType ComM_GetMaxComMode(ComM_UserHandleType User, ComM_ModeType * ComMode)
{
    /* Local variables declaration */
    #if (COMM_INHIBITION_ENABLED)
    ComM_UserVarType_tst *  userRamPtr_pst;
    #endif  /*  if (COMM_INHIBITION_ENABLED)  */

    /******************************************DET STARTS HERE***************************************************/
    /* DET check for ComM initialization */
    if (ComM_GlobalStruct.ComM_InitStatus_en!= COMM_INIT)
    {
        /* Report DET with ComM not initialized status*/
        COMM_DET_REPORT_ERROR(COMM_GET_MAX_COMMODE_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }

    /* Check if pointer used is NULL. If so raise DET and return status as E_NOT_OK */
    if (ComMode == NULL_PTR)
    {
        /* Report DET with NULL parameter*/
        COMM_DET_REPORT_ERROR(COMM_GET_MAX_COMMODE_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }

    /* DET check for valid user */
    if (ComM_Prv_ValidateUserId(User) == FALSE)
    {
        /* Report DET with Invalid Mode parameter*/
        COMM_DET_REPORT_ERROR(COMM_GET_MAX_COMMODE_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }

    /******************************************DET ENDS HERE***************************************************/

    /* Local variable initialization */

    User = ComM_UserId_MappingTable_acst[User];
    #if (COMM_INHIBITION_ENABLED)
    userRamPtr_pst = &ComM_UserStruct[User];

    if(((userRamPtr_pst->WakeUpInhibitionCtr_u16) != C_ZERO) || ((userRamPtr_pst->LimitToNoComCtr_u16) != C_ZERO))
    {
        *ComMode = COMM_NO_COMMUNICATION;
    }
    else
    #endif  /* (COMM_INHIBITION_ENABLED)  */
    {
        *ComMode = COMM_FULL_COMMUNICATION;
    }
    return E_OK;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

