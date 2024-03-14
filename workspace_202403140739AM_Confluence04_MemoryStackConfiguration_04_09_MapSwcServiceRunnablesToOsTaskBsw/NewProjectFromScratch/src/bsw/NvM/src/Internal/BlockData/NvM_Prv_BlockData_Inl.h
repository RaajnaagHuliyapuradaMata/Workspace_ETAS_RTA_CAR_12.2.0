/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_BLOCKDATA_INL_H
#define NVM_PRV_BLOCKDATA_INL_H

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
# include "NvM_Prv.h"
#endif

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"
/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/

/**
 * This function returns whether a RAM Block CRC recalculation for the block is needed.
 *
 * \param[in] idBlock_uo
 * ID of the block
 *
 * \return
 * - TRUE = a RAM block CRC recalculation for the block is needed
 * - FALSE = a RAM block CRC recalculation for the block is not needed
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRamCrcRecalcRequired(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ) != 0u);
}

/**
 * This function returns whether a RAM Block CRC recalculation for the block is currently ongoing.
 *
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operations
 *
 * \param[in] idBlock_uo
 * ID of the block
 *
 * \return
 * - TRUE = a RAM block CRC recalculation for the block is currently ongoing
 * - FALSE = there is no RAM Block CRC recalculation for the block currently ongoing
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRamCrcRecalcOngoing(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_RAM_CRC_RECALC_ONGOING) != 0u);
}

/**
 * This function returns whether a write of the data in the Permanent RAM Block is needed.
 *
 * \param[in] idBlock_uo
 * ID of the block
 *
 * \return
 * - TRUE = write is needed
 * - FALSE = no write is needed
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsWriteRequired(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ) != 0u);
}

/**
 * This function returns the information whether the data in permanent RAM block is valid or not.
 *
 * \param[in] idBlock_uo
 * ID of the block for which the status of the data in RAM block is required
 *
 * \return
 * Validness of the data in permanent RAM block:
 * - TRUE = data is valid
 * - FALSE = data is invalid
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsPRamBlockValid(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_RAM_VALID) != 0u);
}

/**
 * This function returns the information whether the data in permanent RAM block is changed or not.
 *
 * \param[in] idBlock_uo
 * ID of the block for which the status of the data in RAM block is required
 *
 * \return
 * Changed status of the data in permanent RAM block:
 * - TRUE = data is changed
 * - FALSE = data is not changed
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsChanged(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_RAM_CHANGED) != 0u);
}

/**
 * This function returns the trigger status for WriteAll service.
 *
 * \param[in] idBlock_uo
 * ID of the block for which the trigger status for WriteAll service is required
 *
 * \return
 * trigger status for WriteAll service:
 * - TRUE = passed block will participate on next WriteAll service regardless of its changed status
 * - FALSE = Participation of the passed block on next WriteAll service depends on its changed status
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsTrgWriteAllActive(NvM_BlockIdType idBlock_uo)
{
    boolean isTrgWriteAllActive_b = FALSE;
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
#if (NVM_RB_SET_WRITEALL_TRIGGER_API == STD_ON)
    isTrgWriteAllActive_b = (NvM_Prv_stBlock_auo[idBlock_uo] & NVM_RB_BLOCK_STATE_MASK_TRG_WRITEALL) != 0u;
#else
    (void)idBlock_uo;
#endif
    return isTrgWriteAllActive_b;
}

/**
 * This function returns the information whether the ROM data is ambiguous or not.
 *
 * Normally data in configured ROM block can be used as default data. But if a block is of type DATASET and
 * at least one ROM block is configured for it then data in ROM block cannot be used as default data because
 * it is not clear which ROM block instance to take (the case of exactly one ROM block is not specially treated
 * by the NvM for simplicity reasons).
 *
 * \param[in] idBlock_uo
 * ID of the block for which ambiguity status of ROM data is required
 *
 * \return
 * Ambiguity status of ROM data:
 * - TRUE = given block is of type DATASET and at least one ROM block is configured
 * - FALSE = for given block unique ROM data is configured
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRomDataAmbiguous(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    return ((NULL_PTR != NvM_Prv_BlkDesc_GetRomBlockAddress(idBlock_uo)) &&
            (NVM_BLOCK_DATASET == NvM_Prv_BlkDesc_GetType(idBlock_uo)) &&
            (NvM_Prv_idxDataSet_au8[idBlock_uo] < NvM_Prv_BlkDesc_GetNrNonVolatileBlocks(idBlock_uo))
           );
}
/**
 * This function returns the information whether the given instance of a DATASET block is in ROM or not.
 *
 * \param[in] idBlock_uo
 * ID of the block for which storage location is required
 * \param[in] idxDataset_u8
 * dataset index for which storage location is required
 *
 * \return
 * Storage location:
 * - TRUE = given instance of a DATASET block is stored in ROM (non-dataset blocks are never stored in ROM)
 * - FALSE = given instance of a DATASET block is stored in non-volatile memory
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsInRom(NvM_BlockIdType idBlock_uo, uint8 idxDataset_u8)
{
    return ((NVM_BLOCK_DATASET == NvM_Prv_BlkDesc_GetType(idBlock_uo)) &&
            (idxDataset_u8 >= NvM_Prv_BlkDesc_GetNrNonVolatileBlocks(idBlock_uo)));
}

/**
 * This function returns the write-protection status for a given block.
 *
 * \param[in] idBlock_uo
 * ID of the block for which write-protection status is required
 *
 * \return
 * Write-protection status:
 * - TRUE = write-protection is enabled
 * - FALSE = write-protection is disabled
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsWriteProtected(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_WRITE_PROT) != 0u);
}

/**
 * This function returns the read done status for a given block.
 *
 * \param[in] idBlock_uo
 * ID of the block for which read done status is required
 *
 * \return
 * Read done status:
 * - TRUE = A read has been performed regardless of whether it has been successful or not.
 * - FALSE = No read has been performed.
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsReadDone(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_READ_DONE) != 0u);
}

/**
 * This function returns the state of the job currently ongoing for a given block.
 *
 * \param[in] idBlock_uo
 * ID of the block for which job status is required
 *
 * \return
 * Status of the job currently ongoing:
 * - TRUE = a job is currently ongoing
 * - FALSE = NO job is currently ongoing
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsJobOngoing(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return ((NvM_Prv_stBlock_auo[idBlock_uo] & NVM_BLOCK_STATE_JOB_ONGOING) != 0u);
}

/**
 * This function returns the information whether the given block is writeable or not.
 *
 * It is not possible to write a block if one of following conditions is true:
 * - write-protection is enabled (see NvM_Prv_Block_IsWriteProtected())
 * - required instance of a DATASET block is stored in ROM (seeNvM_Prv_Block_IsInRom())
 * - write-once-status-unknown
 *   (No prior read done for write once blocks, regardless whether the read has been successful or not)
 *
 * If it is not possible to write a block then this function sets the corresponding bit in the passed error bit-field
 * for later processing.
 * Possible errors:
 * - NvM_Prv_Service_InitiationError_MaskBlkWriteProtected_e, see mask description
 * - NvM_Prv_Service_InitiationError_MaskBlkWriteOnceStatusUnknown_e, see mask description
 *
 * \param[in] idBlock_uo
 * ID of the block for which writeability status is required
 * \param[in] idxDataset_u8
 * Dataset index to check whether the block is located in ROM
 * \param[inout] Errors_puo
 * Pointer to the bit-field for possible errors
 *
 * \return
 * Writability status:
 * - TRUE = block is writeable
 * - FALSE = block is not writeable
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsWriteable(NvM_BlockIdType idBlock_uo,
                                                          uint8 idxDataset_u8,
                                                          NvM_Prv_Service_InitiationErrors_tuo *Errors_puo)
{
    boolean IsBlockInRom_b = NvM_Prv_Block_IsInRom(idBlock_uo, idxDataset_u8);
    boolean isWriteOnceBlockWriteable_b = TRUE;
    boolean isBlockWriteProtected_b = NvM_Prv_Block_IsWriteProtected(idBlock_uo);

    if (isBlockWriteProtected_b || IsBlockInRom_b)
    {
        *Errors_puo |= (NvM_Prv_Service_InitiationErrors_tuo)
                       NvM_Prv_Service_InitiationError_MaskBlkWriteProtected_e;
    }
#if (NVM_AR_RELEASE_MAJOR_VERSION == 4) && (NVM_AR_RELEASE_MINOR_VERSION >=5)
    /* MR12 RULE 13.5 VIOLATION: both function are pure getter, no unexpected side-effects */
    if (   NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo, NVM_PRV_BLOCK_FLAG_WRITE_ONCE)
        && !NvM_Prv_Block_IsReadDone(idBlock_uo))
    {
        *Errors_puo |= (NvM_Prv_Service_InitiationErrors_tuo)
                       NvM_Prv_Service_InitiationError_MaskBlkWriteOnceStatusUnknown_e;
        isWriteOnceBlockWriteable_b = FALSE;
    }
#endif
    return (!isBlockWriteProtected_b && !IsBlockInRom_b && isWriteOnceBlockWriteable_b);
}

/**
 * This function returns currently set dataset index.
 *
 * It is possible to invoke this function for non-DATASET blocks. In this case this function always returns 0.
 *
 * \param[in] idBlock_uo
 * ID of the block for which set data index is required
 *
 * \return
 * Currently set data index
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_Block_GetIdxDataset(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    return NvM_Prv_idxDataSet_au8[idBlock_uo];
}

/**
 * This function returns the result of the last asynchronous user request.
 *
 * If no asynchronous user requests has been started for the given block then this function returns
 * the initialization value NVM_REQ_OK.
 *
 * \param[in] idBlock_uo
 * ID of the block for which result of the last asynchronous user request is required
 *
 * \return
 * Result of the last asynchronous user request, for possible values see NvM_RequestResultType
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_RequestResultType NvM_Prv_Block_GetRequestResult(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    return NvM_Prv_stRequestResult_au8[idBlock_uo];
}

/**
 * This function returns detailed error information on the last read request
 *
 * If no request has yet been started then NvM_Rb_ErrorInfoDetail_NotAvailable_e is returned
 *
 * \param[in] idBlock_uo
 * ID of the block for which error details are required
 *
 * \return
 * Error details on the last read, for possible values see NvM_Rb_ErrorInfoDetail_ten
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Rb_ErrorInfoDetail_ten NvM_Prv_Block_GetErrorInfoDetail(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
#if (NVM_RB_ERROR_INFO_DETAIL == STD_ON)
    return NvM_Prv_stDetailedErrorInfo_aen[idBlock_uo];
#else
    (void)idBlock_uo;
    return NvM_Rb_ErrorInfoDetail_NotAvailable_e;
#endif
}

/**
 * This function returns the information whether an asynchronous user request is pending or not.
 *
 * \param[in] idBlock_uo
 * ID of the block for which pending status of an asynchronous user request is required
 * \param[in] maskService_u16
 * Mask of the asynchronous user request for which pending status is required
 *
 * \return
 * Pending status of an asynchronous user request identified by the passed service mask:
 * - TRUE = asynchronous user request is pending
 * - FALSE = asynchronous user request is not pending
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsRequestPending(NvM_BlockIdType idBlock_uo, uint16 maskService_u16)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    return ((NvM_Prv_stRequests_au16[idBlock_uo] & maskService_u16) != 0u);
}

/**
 * This function returns the information whether any asynchronous user request is pending or not.
 *
 * If any asynchronous user request is currently pending then this function returns the block pending error
 * to be reported to the DET, see parameter Errors_puo.
 *
 * If no asynchronous user request is currently pending but NvM is enqueuing a multi-block request for the given block
 * then this function will also return the blaok pending error to be reported to the DET,
 * i.e. NvM cannot accept another user request for the given block, see NvM_Prv_Block_IsNvmEnqueuingForMulti().
 *
 * \attention
 * The user shall make sure that the call of this function is done only with active scheduler lock
 * If called with enabled scheduler lock the run-time of 4 can be used (14 in worst case, but this is rather improbable):
 * - 2 for internal copy-operation
 * - NvM_Prv_Block_IsNvmEnqueuingForMulti = normally run-time of this function is 2;
 *                                          in worst case this function is called while a multi-block request
 *                                          is updating status for individual blocks, in worst case it is
 *                                          NvM_Prv_WriteAll_IsBlockParticipating and run-time of this function is 12
 *                                          (see NvM_Prv_Block_IsNvmEnqueuingForMulti)
 *
 * \attention
 * This function considers only \b user requests. If the only pending request is the NvM internal maintenance request
 * then this function will return FALSE, i.e. \b no user requests are pending.
 *
 * \param[in] idBlock_uo
 * ID of the block for which pending status of any asynchronous user request is required
 * \param[inout] Errors_puo
 * Errors which can occur if any asynchronous user request is pending.
 *
 * \return
 * Pending status of any asynchronous \b user request:
 * - TRUE = any asynchronous user request is pending
 * - FALSE = no asynchronous user request is pending
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_IsPending(NvM_BlockIdType idBlock_uo,
                                                        NvM_Prv_Service_InitiationErrors_tuo *Errors_puo)
{
    boolean isBlockPending_b;
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif

    isBlockPending_b = (   NvM_Prv_Block_IsNvmEnqueuingForMulti(idBlock_uo, Errors_puo)
                                || (   (NvM_Prv_stRequests_au16[idBlock_uo] != 0u)
#if (NVM_PRV_MAINTAIN_REDUNDANT_BLOCKS)
                                    && (NvM_Prv_stRequests_au16[idBlock_uo] != (1u << NvM_Prv_ServiceBit_Maintain_e))
#endif
                                   )
                               );

    if (isBlockPending_b)
    {
        *Errors_puo |= (NvM_Prv_Service_InitiationErrors_tuo)NvM_Prv_Service_InitiationError_MaskBlkPEnding_e;
    }

    return isBlockPending_b;
}

/**
 * This function sets the dataset index as required.
 *
 * \attention
 * This function does not check if passed block is of type DATASET.
 * The user of this function shall make sure that he calls it only for blocks of type DATASET.
 * Also the user shall make sure that the dataset index can be set safely.
 * I.e. the user shall use this function with enabled scheduler lock.
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operations
 *
 * \param[in] idBlock_uo
 * ID of the block for which data index is set
 * \param[in] idxData_u8
 * Required dataset index
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetIdxData(NvM_BlockIdType idBlock_uo, uint8 idxData_u8)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    NvM_Prv_idxDataSet_au8[idBlock_uo] = idxData_u8;
}

/**
 * This function sets the result of the last asynchronous user request as required.
 *
 * \attention
 * The user of this function shall make sure that the result can be set safely.
 * I.e. the user shall use this function with enabled scheduler lock.
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operations
 *
 * \param[in] idBlock_uo
 * ID of the block for which result of the last asynchronous user request is set
 * \param[in] Result_uo
 * Required result
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetRequestResult(NvM_BlockIdType idBlock_uo, NvM_RequestResultType Result_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    NvM_Prv_stRequestResult_au8[idBlock_uo] = Result_uo;
}

/**
 * This function sets the detailed error information
 *
 * \attention
 * The user of this function shall make sure that the result can be set safely.
 * I.e. the user shall use this function with enabled scheduler lock.
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operations
 *
 * \param[in] idBlock_uo
 * ID of the block for which detailed error information shall be set
 * \param[in] errorInfoDetail_en
 * Error information detail
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetErrorInfoDetail(NvM_BlockIdType idBlock_uo,
                                                              NvM_Rb_ErrorInfoDetail_ten errorInfoDetail_en)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
#if (NVM_RB_ERROR_INFO_DETAIL == STD_ON)
    NvM_Prv_stDetailedErrorInfo_aen[idBlock_uo] = errorInfoDetail_en;
#else
    (void)idBlock_uo;
    (void)errorInfoDetail_en;
#endif
}

/**
 * This function sets an asynchronous request as pending.
 *
 * \attention
 * This function does not check if other asynchronous requests are pending for the passed block.
 * The user of this function shall make sure that required asynchronous request can be set safely as pending.
 * I.e. the user shall use this function with enabled scheduler lock.
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operations
 *
 * \param[in] idBlock_uo
 * ID of the block for which an asynchronous request is set as pending
 * \param[in] ServiceBit_uo
 * Service bit which identifies the required asynchronous request
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetRequest(NvM_BlockIdType idBlock_uo, NvM_Prv_ServiceBit_tuo ServiceBit_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    NvM_Prv_stRequests_au16[idBlock_uo] |= (1u << ServiceBit_uo);
}

/**
 * This function clears pending asynchronous requests.
 *
 * \attention
 * The user of this function shall make sure that pending asynchronous requests can be cleared safely.
 * I.e. the user shall use this function with enabled scheduler lock.
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operations
 *
 * \param[in] idBlock_uo
 * ID of the block for which pending status is cleared as required
 * \param[in] ServiceBit_uo
 * Mask of the asynchronous requests for which pending status is cleared
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_ClearRequests(NvM_BlockIdType idBlock_uo, uint16 maskRequests_u16)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_PRV_NR_BLOCKS_ADMIN);
#endif
    NvM_Prv_stRequests_au16[idBlock_uo] &= (~maskRequests_u16);
}

/**
 * This function sets status informations as required.
 *
 * For available status informations see the definition of NvM_Prv_stBlock_au8.
 *
 * \attention
 * The user of this function shall make sure that status informations can be set safely.
 * I.e. the user shall use this function with enabled scheduler lock.
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operations
 *
 * \param[in] idBlock_uo
 * ID of the block for which status informations is set as required
 * \param[in] maskBitsToChange_u8
 * Mask which defines status informations to be changed
 * \param[in] maskBitsNewValue_u8
 * Mask which defines new values for status informations defined by parameter maskBitsToChange_u8
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetState(NvM_BlockIdType idBlock_uo,
                                                    uint16 maskBitsToChange_uo,
                                                    uint16 maskBitsNewValue_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    NvM_Prv_stBlock_auo[idBlock_uo] = ((NvM_Prv_stBlock_auo[idBlock_uo] & (~maskBitsToChange_uo)) |
                                       (maskBitsNewValue_uo & maskBitsToChange_uo));
}

/**
 * This function checks whether the configuration ID has changed.
 *
 * \return
 * Change status of the configuration ID:
 * - TRUE = configuration ID has changed
 * - FALSE = configuration ID has not changed
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_HasIdConfigChanged(void)
{
    return (NVM_COMPILED_CONFIG_ID != NvM_Prv_idConfigStored_u16);
}

/**
 * This function sets the configuration ID as required.
 *
 * \param[in] idConfigNew_u16
 * New value for the configuration ID
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Block_SetIdConfig(uint16 idConfigNew_u16)
{
    NvM_Prv_idConfigStored_u16 = idConfigNew_u16;
}

/**
 * This function returns the information whether the configuration ID will be written
 * during the WriteAll-service.
 *
 * There are three use cases where NvM writes configuration ID:
 * - Dynamic configuration as defined by AR is enabled:
 *   - if configuration ID has changed then NvM writes it at the end of the WriteAll-service
 * - Extended dynamic configuration is enabled:
 *   - if configuration ID has changed then NvM writes it at the end of the ReadAll-service
 * - Initialization at layout change is enabled:
 *   - if configuration ID has changed then NvM writes it at the end of the ReadAll-service
 *
 * \return
 * Participation status of the configuration ID during WriteAll-service:
 * - TRUE = NvM writes configuration ID during next WriteAll-service
 * - FALSE = NvM writes configuration ID at the end of the ReadAll-service
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_Block_InitIdConfigDuringWriteAll(void)
{
    boolean InitIdConfigDuringWriteAll_b = FALSE;

#if ((NVM_DYNAMIC_CONFIGURATION == STD_ON) && (NVM_RB_REMOVE_NON_RESISTANT_BLOCKS == STD_OFF))
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2901] Signal the writing of the Config ID block in WriteAll
    //                                          when dynamic conf is active and extended dynamic conf is inactive
    InitIdConfigDuringWriteAll_b = NvM_Prv_Block_HasIdConfigChanged();
#endif
    return InitIdConfigDuringWriteAll_b;
}

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

/**
 * This function returns the value of the variable storing all flags (attribute/status information) for the
 * NVRAM-block.
 *
 * \param[in] idBlock_uo
 * ID of the block
 *
 * \return
 * Value of block status
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_Block_GetState(NvM_BlockIdType idBlock_uo)
{
#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_CntOob(idBlock_uo, NVM_CFG_NR_ALL_BLOCKS);
#endif
    return NvM_Prv_stBlock_auo[idBlock_uo];
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_BLOCKDATA_INL_H */
#endif
