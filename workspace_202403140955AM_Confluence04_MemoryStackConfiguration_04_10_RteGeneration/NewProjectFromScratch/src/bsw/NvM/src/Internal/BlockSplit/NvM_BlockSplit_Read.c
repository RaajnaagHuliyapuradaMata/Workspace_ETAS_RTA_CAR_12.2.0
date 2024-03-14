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

#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_BlockDescriptor.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_BlockSplit_Read_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_BlockSplit_Read_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_BlockSplit_Read_DoSubBlocks(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_BlockSplit_Read_Data_cst =
{
        &NvM_Prv_BlockSplit_Read_GetStateFct,
        &NvM_Prv_BlockSplit_Read_IsEnabled,
        NvM_Prv_stJob_BlockSplit_Read_DoSubBlocks_e,
        NvM_Prv_stJob_BlockSplit_Read_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_BlockSplit_Read_GetData(void)
{
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    return &NvM_Prv_BlockSplit_Read_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_BlockSplit_Read_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    // Read job for split blocks is enabled only for configured split blocks if master-block is in use
    boolean isIdxMasterBlock = !NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8);

    return ((NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo)) && isIdxMasterBlock);
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_BlockSplit_Read_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct Read_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_BlockSplit_Read_DoSubBlocks_e:
            Read_State_pfct = &NvM_Prv_BlockSplit_Read_DoSubBlocks;
        break;

        default:
            Read_State_pfct = NULL_PTR;
        break;
    }

    return Read_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_BlockSplit_Read_DoSubBlocks(NvM_Prv_stJob_ten* stJob_pen,
                                                                                    NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                    NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoBlockSplit_Read_e;
    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo);

    // get offset to the master-block within internal buffer
    uint16 OffsetToMasterBlock_u16      = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);
    // get start address of dataset-index-array within read master-block within internal buffer
    uint8* ArrayIdxSubBlockNew_pu8      = &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToMasterBlock_u16];
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

    // a sub-block have been processed
    // -> check result of DoRead-StateMachine for current sub-block
    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // reading of current sub-block has succeeded

        // check if last sub-block has been processed
        if (idxSubBlock_u8 < nrSubBlocks_u8)
        {
            JobData_un.JobData_pst->idxDataset_u8 =   (ArrayIdxSubBlockNew_pu8[idxSubBlock_u8] * nrSubBlocks_u8)
                                                    + (idxSubBlock_u8);
            // start DoRead-StateMachine for next sub-block
            *stJob_pen = NvM_Prv_stJob_BlockSplit_Read_DoSubBlocks_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;
        }
        else
        {
            // all sub-blocks written -> complete job
            *stJob_pen = NvM_Prv_stJob_BlockSplit_Read_Completed_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            JobData_un.JobData_pst->idxDataset_u8 = idxMaster_u8;
        }
    }
    else
    {
        // reading of current sub-block has failed -> complete DoSubBlocks-StateMachine
        *stJob_pen = NvM_Prv_stJob_BlockSplit_Read_Completed_e;
    }

    return idJobStateMachineNext_en;
}

#endif  // (NVM_BLOCK_COMPRESSION_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

