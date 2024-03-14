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
#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobStateMachine.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#if (NVM_CALC_NV_CRC == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_JobCalcNvCrc_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobCalcNvCrc_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobCalcNvCrc_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                           NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobCalcNvCrc_DoDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobCalcNvCrc_DoBlkHdrCrc(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobCalcNvCrc_Data_cst =
{
        &NvM_Prv_JobCalcNvCrc_GetStateFct,
        &NvM_Prv_JobCalcNvCrc_IsEnabled,
        NvM_Prv_stJob_CalcNvCrc_Start_e,
        NvM_Prv_stJob_CalcNvCrc_Completed_e,
        NvM_Prv_JobResource_nrCluster_e
};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#endif  // (NVM_CALC_NV_CRC == STD_ON)

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobCalcNvCrc_GetData(void)
{
#if (NVM_CALC_NV_CRC == STD_ON)
    return &NvM_Prv_JobCalcNvCrc_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_CALC_NV_CRC == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_JobCalcNvCrc_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    // Nv CRC is enabled only if configured for current block
    return (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_NV_CRC));
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobCalcNvCrc_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobCalcNvCrc_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_CalcNvCrc_Start_e:
            JobCalcNvCrc_State_pfct = &NvM_Prv_JobCalcNvCrc_Start;
        break;

        case NvM_Prv_stJob_CalcNvCrc_DoDataCrc_e:
            JobCalcNvCrc_State_pfct = &NvM_Prv_JobCalcNvCrc_DoDataCrc;
        break;

        case NvM_Prv_stJob_CalcNvCrc_DoBlkHdrCrc_e:
            JobCalcNvCrc_State_pfct = &NvM_Prv_JobCalcNvCrc_DoBlkHdrCrc;
        break;

        default:
            JobCalcNvCrc_State_pfct = NULL_PTR;
        break;
    }

    return JobCalcNvCrc_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobCalcNvCrc_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                           NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCalcNvCrc_e;
    NvM_BlockIdType idBlock_uo = JobData_pcst->idBlock_uo;
    NvM_Prv_Job_CrcCalculation_tst* Calculation_pst = &JobResult_pst->CrcData_st.Calculation_st;

    boolean isIdxSubBlock_b = NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo,
                                                               JobData_pcst->idxDataset_u8);
    boolean isBlockSplit_b = (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo));

    uint16 OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(idBlock_uo,
                                                                         JobData_pcst->idxDataset_u8);

    if (   JobData_pcst->isEncryptionEnabled_b
        || (isBlockSplit_b && !isIdxSubBlock_b)
        || (JobResult_pst->CrcData_st.CrcRamBlk_un.Crc32_u32 == 0u))
    {
        // CRC over user data has not been recalculated
        // (block is encrypted or master-block of a split-block is in progress or RAM CRC disabled)
        // -> start CRC calculation over all data in internal buffer
        Calculation_pst->isFirstCall_b      = TRUE;
        Calculation_pst->Length_u16         = *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16;
        Calculation_pst->Crc_un.Crc32_u32   = 0u;
        Calculation_pst->Buffer_pu8         = &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16];
    }
    else
    {
        // CRC over user data is already in internal buffer
        // -> continue CRC calculation over remaining data in internal buffer
        Calculation_pst->isFirstCall_b      = FALSE;
        Calculation_pst->Length_u16         =   *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16
                                              - NvM_Prv_BlkDesc_GetSize(idBlock_uo);
        Calculation_pst->Crc_un.Crc32_u32   = JobResult_pst->CrcData_st.CrcRamBlk_un.Crc32_u32;
        Calculation_pst->Buffer_pu8         = &JobData_pcst->IntBuffer_st.Buffer_pu8[  OffsetToSubBlock_u16
                                                                                     + NvM_Prv_BlkDesc_GetSize(idBlock_uo)];
    }

    *stJob_pen = NvM_Prv_stJob_CalcNvCrc_DoDataCrc_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCrc_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobCalcNvCrc_DoDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCalcNvCrc_e;

#if (NVM_RB_BLOCK_HEADER > 0u)
    // block header is already in the internal buffer
    // -> continue with CRC calculation over the block header
    JobResult_pst->CrcData_st.Calculation_st.isFirstCall_b      = FALSE;
    JobResult_pst->CrcData_st.Calculation_st.Buffer_pu8         = JobData_pcst->IntBuffer_st.BlockHeader_pu8;
    JobResult_pst->CrcData_st.Calculation_st.Length_u16         = NVM_RB_BLOCK_HEADER_LENGTH;

    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCrc_e;
    *stJob_pen = NvM_Prv_stJob_CalcNvCrc_DoBlkHdrCrc_e;
#else
    (void)JobData_pcst;
    *stJob_pen = NvM_Prv_stJob_CalcNvCrc_Completed_e;
#endif

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobCalcNvCrc_DoBlkHdrCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCalcNvCrc_e;

    (void)JobData_pcst;
    *stJob_pen = NvM_Prv_stJob_CalcNvCrc_Completed_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

#endif  // (NVM_CALC_NV_CRC == STD_ON)
