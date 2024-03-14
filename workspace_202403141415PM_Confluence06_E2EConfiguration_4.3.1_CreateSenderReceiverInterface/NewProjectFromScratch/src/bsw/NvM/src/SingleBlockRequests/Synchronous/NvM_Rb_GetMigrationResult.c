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
#include "NvM.h"

#include "MemIf.h"
// TRACE[NVM089] Check version compatibility of included header files
#if (!defined(MEMIF_AR_RELEASE_MAJOR_VERSION) || (MEMIF_AR_RELEASE_MAJOR_VERSION != NVM_AR_RELEASE_MAJOR_VERSION))
# error "AUTOSAR major version undefined or mismatched"
#endif

#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"
/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

Std_ReturnType NvM_Rb_GetMigrationResult(NvM_BlockIdType BlockId, NvM_Rb_MigrationResult_ten *MigrationResultPtr)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3052] Service to get the result of migration operation
    Std_ReturnType stReturn_uo = E_NOT_OK;
    MemIf_Rb_MigrationResult_ten migrationResult_en;

    // TRACE[NVM635] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
    if (NvM_Prv_ErrorDetection_IsBlockIdValid(NVM_SERVICE_ID_RB_GET_MIGRATION_RESULT, BlockId, FALSE))
    {
        NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
        ptr_un.ptrMigrationResult_pen = MigrationResultPtr;
        if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_RB_GET_MIGRATION_RESULT,
                                              BlockId,
                                              NVM_E_PARAM_DATA,
                                              &ptr_un))
        {
            migrationResult_en = MemIf_Rb_GetMigrationResult(NvM_Prv_BlkDesc_GetIdDevice(BlockId),
                                                             NvM_Prv_BlkDesc_GetIdMemIf(BlockId));
            // If migration feature is disabled then function MemIf_Rb_GetMigrationResult will be replaced
            // by MEMIF_RB_MIGRATION_RESULT_DEACTIVATED_E and compiler generates a warning about "unused parameter".
            // Following line was added to avoid this warning
            (void)BlockId;

            switch(migrationResult_en)
            {
                case MEMIF_RB_MIGRATION_RESULT_INIT_E:
                    *MigrationResultPtr = NVM_RB_MIGRATION_RESULT_INIT_E;
                break;

                case MEMIF_RB_MIGRATION_RESULT_NOT_NECESSARY_E:
                    *MigrationResultPtr = NVM_RB_MIGRATION_RESULT_NOT_NECESSARY_E;
                break;

                case MEMIF_RB_MIGRATION_RESULT_TO_SMALLER_SIZE_E:
                    *MigrationResultPtr = NVM_RB_MIGRATION_RESULT_TO_SMALLER_SIZE_E;
                break;

                case MEMIF_RB_MIGRATION_RESULT_TO_BIGGER_SIZE_E:
                    *MigrationResultPtr = NVM_RB_MIGRATION_RESULT_TO_BIGGER_SIZE_E;
                break;

                case MEMIF_RB_MIGRATION_RESULT_NOT_DONE_E:
                    *MigrationResultPtr = NVM_RB_MIGRATION_RESULT_NOT_DONE_E;
                break;

                case MEMIF_RB_MIGRATION_RESULT_DEACTIVATED_E:
                    *MigrationResultPtr = NVM_RB_MIGRATION_RESULT_DEACTIVATED_E;
                break;

                default:
                    /* MR12 RULE 16.4 VIOLATION: Default can not be reached and has no effect here */
                break;
            }

            stReturn_uo = E_OK;
        }
    }

    return stReturn_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

