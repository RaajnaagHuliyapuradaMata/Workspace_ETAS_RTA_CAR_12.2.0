/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
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

#include "NvM_Prv_HideRteApi.h"
#include "NvM_Prv_ErrorDetection.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * This function returns the block errors.
 *
 * \param[in] idBlock_uo
 * ID of the block for which production error is reported. *
 * \param[in] BlockErrors_puo
 * Variable saving blocks' errors.
 *
 * \return
 * Initiation status:
 * - E_OK = read request into the configured permanent RAM block has been initiated successfully
 * - E_NO_OK = initiation of the read request into the configured permanent RAM block has failed,
 *             i.e. at least one of the condition for successful initiation is not fulfilled (see detailed description)
 *
 */
Std_ReturnType NvM_Rb_GetBlockErrors(NvM_BlockIdType idBlock_uo, NvM_Rb_BlockErrors_tuo* BlockErrors_puo )
{
    Std_ReturnType stReturn_uo = E_NOT_OK;
    NvM_Prv_ErrorDetection_Ptr_tun ptr_un;

    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_RB_GET_BLOCK_ERRORS, idBlock_uo))
    {
        if (NvM_Prv_ErrorDetection_IsBlockIdValid(NVM_SERVICE_ID_RB_GET_BLOCK_ERRORS, idBlock_uo, TRUE))
        {
            ptr_un.ptrBlockErr_puo = BlockErrors_puo;
            if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_RB_GET_BLOCK_ERRORS, idBlock_uo, NVM_E_PARAM_DATA, &ptr_un))
            {
                stReturn_uo = E_OK;
                *BlockErrors_puo = NvM_Prv_ErrorDetection_GetBlockErrors(idBlock_uo);
            }
        }
    }
    return(stReturn_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

