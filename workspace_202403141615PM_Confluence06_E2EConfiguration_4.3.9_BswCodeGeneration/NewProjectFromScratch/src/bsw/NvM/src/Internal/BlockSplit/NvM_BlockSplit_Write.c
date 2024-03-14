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

#include "rba_MemLib.h"
#include "rba_MemLib_Special.h"

#include "NvM_Prv_Crc.h"
#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_InternalBuffer.h"
#include "NvM_Prv_BlockDescriptor.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_BlockSplit_Write_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_BlockSplit_Write_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_BlockSplit_Write_DoSubBlocks(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_BlockSplit_Write_Data_cst =
{
        &NvM_Prv_BlockSplit_Write_GetStateFct,
        &NvM_Prv_BlockSplit_Write_IsEnabled,
        NvM_Prv_stJob_BlockSplit_Write_DoSubBlocks_e,
        NvM_Prv_stJob_BlockSplit_Write_Completed_e,
        NvM_Prv_JobResource_nrCluster_e
};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_BlockSplit_Write_GetData(void)
{
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    return &NvM_Prv_BlockSplit_Write_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_BlockSplit_Write_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    // Write job for split blocks is enabled only for configured split blocks if master-block is in use
    boolean isIdxMasterBlock = !NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8);

    return ((NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo)) && isIdxMasterBlock);
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_BlockSplit_Write_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct Write_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_BlockSplit_Write_DoSubBlocks_e:
            Write_State_pfct = &NvM_Prv_BlockSplit_Write_DoSubBlocks;
        break;

        default:
            Write_State_pfct = NULL_PTR;
        break;
    }

    return Write_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_BlockSplit_Write_DoSubBlocks(NvM_Prv_stJob_ten* stJob_pen,
                                                                                     NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                     NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoBlockSplit_Write_e;
    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo);

    uint8* ArrayIdxSubBlockCurrent_pu8  = CfgBlockSplit_pcst->BlockSplit_Master_pcst->idxSubBlock_pu8;
    uint8 nrSubBlocks_u8                = CfgBlockSplit_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8;
    uint8 idxMaster_u8                  = 2u * nrSubBlocks_u8;

    uint8 idxSubBlock_u8;

    // The following union is necessary to remove an otherwise unavoidable compiler warning
    // due to a discarded const qualifier
    union
    {
        NvM_Prv_JobData_tst const* JobData_pcst;
        NvM_Prv_JobData_tst* JobData_pst;
    } JobData_un;
    JobData_un.JobData_pcst = JobData_pcst;

    // DoSubBlocks-StateMachine starts always with dataset index for master-block
    if (JobData_pcst->idxDataset_u8 == idxMaster_u8)
    {
        // no sub-block has been processed yet
        // -> set sub-block-index for first sub-block
        idxSubBlock_u8 = 0u;
    }
    else
    {
        // a sub-block has been processed
        // -> get sub-block-index from dataset-index
        //    (hint: dataset-index is static variable and sub-block-index is local variable)
        idxSubBlock_u8 = JobData_pcst->idxDataset_u8 % nrSubBlocks_u8;
        // -> set sub-block-index for next sub-block
        ++idxSubBlock_u8;
    }

    // check result of DoWrite-StateMachine for current sub-block
    if (NvM_Prv_JobResult_Failed_e == JobResult_pst->Result_en)
    {
        // writing of current sub-block has failed -> complete DoSubBlocks-StateMachine
        *stJob_pen = NvM_Prv_stJob_BlockSplit_Write_Completed_e;
    }
    else
    {
        if (idxSubBlock_u8 < nrSubBlocks_u8)
        {
            // -> set dataset-index for current sub-block from current master-block
            JobData_un.JobData_pst->idxDataset_u8 =   (ArrayIdxSubBlockCurrent_pu8[idxSubBlock_u8] * nrSubBlocks_u8)
                                                    + idxSubBlock_u8;

            // start DoMemIf_Write-StateMachine for current sub-block
            *stJob_pen = NvM_Prv_stJob_BlockSplit_Write_DoSubBlocks_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
        }
        else
        {
            // all required sub-blocks written -> complete job
            *stJob_pen = NvM_Prv_stJob_BlockSplit_Write_Completed_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            JobData_un.JobData_pst->idxDataset_u8 = idxMaster_u8;
            // update size of internal buffer
            *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = nrSubBlocks_u8 + (nrSubBlocks_u8 * (uint16)sizeof(NvM_Prv_Crc_tun));
        }
    }

    return idJobStateMachineNext_en;
}

#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

