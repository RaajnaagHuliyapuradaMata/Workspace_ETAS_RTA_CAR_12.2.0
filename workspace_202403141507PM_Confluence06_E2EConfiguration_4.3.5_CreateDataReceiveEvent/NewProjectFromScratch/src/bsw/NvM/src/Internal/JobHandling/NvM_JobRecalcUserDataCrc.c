/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
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

#include "rba_MemLib_Special.h"

#include "NvM_Prv_Crc.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_JobStateMachine.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#if (NVM_CALC_RAM_BLOCK_CRC == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRecalcUserDataCrc_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRecalcUserDataCrc_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcUserDataCrc_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcUserDataCrc_DoUserDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                           NvM_Prv_JobData_tst const* JobData_pcst);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobRecalcUserDataCrc_Data_cst =
{
        &NvM_Prv_JobRecalcUserDataCrc_GetStateFct,
        &NvM_Prv_JobRecalcUserDataCrc_IsEnabled,
        NvM_Prv_stJob_RecalcUserDataCrc_Start_e,
        NvM_Prv_stJob_RecalcUserDataCrc_Completed_e,
        NvM_Prv_JobResource_nrCluster_e
};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#endif  // (NVM_CALC_RAM_BLOCK_CRC == STD_ON)

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRecalcUserDataCrc_GetData(void)
{
#if (NVM_CALC_RAM_BLOCK_CRC == STD_ON)
    return &NvM_Prv_JobRecalcUserDataCrc_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_CALC_RAM_BLOCK_CRC == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRecalcUserDataCrc_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    boolean isEnabled_b;

    // RAM CRC is enabled only if configured for current block and no master-block for a split-block is in progress
    boolean isIdxSubBlock_b      = NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo,
                                                                    JobData_pcst->idxDataset_u8);
    boolean idRamCrcConfigured_b = NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo,
                                                                   NVM_PRV_BLOCK_FLAG_USE_RAM_CRC);

    isEnabled_b = idRamCrcConfigured_b || isIdxSubBlock_b;

    return isEnabled_b;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRecalcUserDataCrc_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobRecalcUserDataCrc_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_RecalcUserDataCrc_Start_e:
            JobRecalcUserDataCrc_State_pfct = &NvM_Prv_JobRecalcUserDataCrc_Start;
        break;

        case NvM_Prv_stJob_RecalcUserDataCrc_DoUserDataCrc_e:
            JobRecalcUserDataCrc_State_pfct = &NvM_Prv_JobRecalcUserDataCrc_DoUserDataCrc;
        break;

        default:
            JobRecalcUserDataCrc_State_pfct = NULL_PTR;
        break;
    }

    return JobRecalcUserDataCrc_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcUserDataCrc_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRecalcUserDataCrc_e;
    NvM_BlockIdType idBlock_uo = JobData_pcst->idBlock_uo;
    NvM_Prv_Job_CrcCalculation_tst* Calculation_pst = &JobResult_pst->CrcData_st.Calculation_st;

    boolean isIdxSubBlock_b = NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo,
                                                               JobData_pcst->idxDataset_u8);
    boolean isBlockSplit_b = (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo));

    uint16 OffsetToSubBlock_u16;
    uint16 SizeUserData_u16;

    if (!isIdxSubBlock_b && isBlockSplit_b)
    {
        SizeUserData_u16 = NvM_Prv_BlkDesc_GetSize(idBlock_uo);
        OffsetToSubBlock_u16 = 0u;
    }
    else
    {
        SizeUserData_u16 = NvM_Prv_BlockSplit_GetSizeSubBlock(idBlock_uo, JobData_pcst->idxDataset_u8);

        if (JobData_pcst->isEncryptionEnabled_b)
        {
            OffsetToSubBlock_u16 = JobData_pcst->IntBuffer_st.OffsetPlainData_u16;
        }
        else
        {
            OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(idBlock_uo,
                                                                          JobData_pcst->idxDataset_u8);
        }
    }

    // Prepare CRC recalculation over the user data
    Calculation_pst->isFirstCall_b      = TRUE;
    Calculation_pst->Length_u16         = SizeUserData_u16;
    Calculation_pst->Crc_un.Crc32_u32   = 0u;
    Calculation_pst->Buffer_pu8         = &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16];

    *stJob_pen = NvM_Prv_stJob_RecalcUserDataCrc_DoUserDataCrc_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCrc_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcUserDataCrc_DoUserDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                           NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRecalcUserDataCrc_e;

    (void)JobData_pcst;
    *stJob_pen = NvM_Prv_stJob_RecalcUserDataCrc_Completed_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

#endif  // (NVM_CALC_RAM_BLOCK_CRC == STD_ON)
