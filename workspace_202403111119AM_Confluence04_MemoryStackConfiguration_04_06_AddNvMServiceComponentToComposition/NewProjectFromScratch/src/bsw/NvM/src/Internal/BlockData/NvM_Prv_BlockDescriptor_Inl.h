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
#ifndef NVM_PRV_BLOCKDESCRIPTOR_INL_H
#define NVM_PRV_BLOCKDESCRIPTOR_INL_H

#include "NvM_Types.h"
#include "rba_MemLib.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/
/**
 * This function returns information whether the block header is configured or not.
 *
 * The purpose of this function is to simplify unit-testing of the NvM.
 *
 * \return
 * - TRUE = block header is enabled
 * - FALSE = block header disabled
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsBlockHeaderEnabled(void)
{
    return (NVM_RB_BLOCK_HEADER > 0u);
}

/**
 * This function appends the block header to the given buffer.
 *
 * The caller of this function has to make sure that given buffer points exactly where the block header
 * has to be appended.
 * If user has not configured the block header then this function appends nothing.
 *
 * \param idBlock_uo
 * ID of the block for which the block header will be appended
 * \param Buffer_pu8
 * Pointer to the buffer where block header will be appended
 */
/* MR12 RULE 5.9, 8.13 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files.
 * The statements within the function can be disabled via a compiler-switch but the parameter is still required
 * to be changeable if the statements within the function are active. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_AppendBlockHeader(NvM_BlockIdType idBlock_uo,
                                                               uint8* Buffer_pu8)
{
#if (NVM_RB_BLOCK_HEADER > 0u)

    rba_MemLib_MemCopy(NvM_Prv_BlockDescriptors_acst[idBlock_uo].BlockHeader_au8,
                       Buffer_pu8,
                       NVM_RB_BLOCK_HEADER_LENGTH);

#else

    (void)idBlock_uo;
    (void)Buffer_pu8;

#endif  // NVM_RB_BLOCK_HEADER > 0u
}

/**
 * \brief
 * This NvM private function returns for given block ID the job resource ID to be used for given job resource cluster.
 *
 * \param idBlock_uo
 * ID of the block for which device ID is required.
 * \param Cluster_en
 * Job resource cluster for which the job resource ID is required
 *
 * \return
 * ID of the job resource to be used for given job resource cluster
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJobResource_tuo NvM_Prv_BlkDesc_GetIdJobResource(NvM_BlockIdType idBlock_uo,
                                                                                   NvM_Prv_JobResource_Cluster_ten Cluster_en)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].idJobResource_auo[Cluster_en];
}

/**
 * \brief
 * This NvM private function provides information whether a block is configured for the given feature.
 *
 * \param idBlock_uo
 * ID of the block for which the configuration information will be provided.
 * \param SelectionMask_en
 * Bit mask for the configuration information to be provided.
 *
 * \return
 * - TRUE = block is configured for the given feature
 * - FALSE = block is not configured for the given feature
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsBlockSelected(NvM_BlockIdType idBlock_uo,
                                                                NvM_Prv_BlockConfiguration_ten SelectionMask_en)
{
    return ((NvM_Prv_BlockDescriptors_acst[idBlock_uo].stFlags_uo & ((uint32)SelectionMask_en)) != 0u);
}

/**
 * \brief
 * This NvM private function provides the information whether default data is available for the given block.
 *
 * \param idBlock_uo
 * ID of the block to provide the availability of the default data.
 *
 * \return
 * - TRUE = default data is available
 * - FALSE = default data is not available
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsDefaultDataAvailable(NvM_BlockIdType idBlock_uo)
{
    return (((NULL_PTR != NvM_Prv_BlockDescriptors_acst[idBlock_uo].adrRomBlock_pcv) ||
             (NULL_PTR != NvM_Prv_BlockDescriptors_acst[idBlock_uo].InitBlockCallback_pfct)));
}

/**
 * \brief
 * This NvM private function checks for the given block whether the configured block length is valid.
 *
 * \param idBlock
 * ID of the block for which the block length will be checked.
 *
 * \return
 * - TRUE = block length is valid
 * - FALSE = block length is invalid
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsLengthValid(NvM_BlockIdType idBlock_uo)
{
    return (0u != *NvM_Prv_BlockDescriptors_acst[idBlock_uo].nrBlockBytes_pu16);
}

/**
 * \brief
 * This NvM private function returns the ID of the internal user request queue to be used for services
 * which modify a block.
 *
 * \details
 * Modifying services are:
 * - write block
 * - erase block
 * - invalidate block
 *
 * \attention
 * The caller of this function has to ensure that passed block ID is valid.
 *
 * \param idBlock_uo
 * ID of the block for which the ID of the internal user request queue is required.
 *
 * \return
 * ID of the internal user request queue
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idQueue_tuo NvM_Prv_BlkDesc_GetIdQueueForModifyingServices(NvM_BlockIdType idBlock_uo)
{
    NvM_Prv_idQueue_tuo idQueue_uo = NvM_Prv_idQueue_Standard_e;
#if (NVM_JOB_PRIORITIZATION == STD_ON)
    if (NVM_PRV_JOB_PRIORITY_IMMEDIATE == NvM_Prv_BlockDescriptors_acst[idBlock_uo].JobPriority_u8)
    {
        // TRACE[SWS_NvM_00378] Only single block write requests for immediate blocks are queued in the immediate queue
        //                      All other single block requests are queued in the standard queue.
        idQueue_uo = NvM_Prv_idQueue_Immediate_e;
    }
#else
    (void)idBlock_uo;
#endif

    return idQueue_uo;
}

/**
 * \brief
 * This NvM private function checks whether the given block is configured with immediate priority.
 *
 * \param idBlock_uo
 * ID of the block for which the priority will be checked.
 *
 * \return
 * - TRUE = block is configured with immediate priority
 * - FALSE = block is configured with standard priority
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_HasBlockImmediateJobPriority(NvM_BlockIdType idBlock_uo)
{
    boolean HasBlockImmediateJobPriority_b = FALSE;

#if (NVM_JOB_PRIORITIZATION == STD_ON)
    if ((NVM_PRV_JOB_PRIORITY_IMMEDIATE == NvM_Prv_BlockDescriptors_acst[idBlock_uo].JobPriority_u8))
    {
        HasBlockImmediateJobPriority_b = TRUE;
    }
#else
    (void)idBlock_uo;
#endif
    return HasBlockImmediateJobPriority_b;
}

/**
 * \brief
 * This NvM private function returns the configured block size for the given block.
 *
 * \param idBlock_uo
 * ID of the block for which the configured block size will be returned.
 *
 * \return
 * Configured block size
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetSize(NvM_BlockIdType idBlock_uo)
{
    return *NvM_Prv_BlockDescriptors_acst[idBlock_uo].nrBlockBytes_pu16;
}

/**
 * \brief
 * This NvM private function returns the configured block size stored on the medium for the given block.
 *
 * \param idBlock_uo
 * ID of the block for which the configured block size stored on the medium will be returned.
 *
 * \return
 * Configured block size stored on the medium
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetBlockSizeStored(NvM_BlockIdType idBlock_uo)
{
    uint16 BlockSizeStored_u16 = 0u;

#if (defined(NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG) && (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON))
        BlockSizeStored_u16 = *NvM_Prv_BlockDescriptors_acst[idBlock_uo].nrBlockBytes_pu16;
#else
        BlockSizeStored_u16 = NvM_Prv_BlockDescriptors_acst[idBlock_uo].nrBlockBytesStored_u16;
#endif

    return BlockSizeStored_u16;
}

/**
 * \brief
 * This NvM private function returns the configured block management type for the given block.
 *
 * \param idBlock_uo
 * ID of the block for which the configured block management type will be returned.
 *
 * \return
 * Configured block management type
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_BlockManagementType NvM_Prv_BlkDesc_GetType(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].BlockManagementType_en;
}

/**
 * \brief
 * This NvM private function returns the configured number of non-volatile data sets for the given block.
 *
 * \param idBlock_uo
 * ID of the block for which the configured number of non-volatile data sets will be returned.
 *
 * \return
 * Configured number of non-volatile data sets
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_BlkDesc_GetNrNonVolatileBlocks(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].nrNvBlocks_u8;
}

/**
 * \brief
 * This NvM private function returns the configured overall number of data sets for the given block.
 *
 * \param idBlock_uo
 * ID of the block for which the configured overall number of data sets will be returned.
 *
 * \return
 * Configured number of data sets
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetNrDataIndexes(NvM_BlockIdType idBlock_uo)
{
    uint8 nrDataIndexes = (NvM_Prv_BlockDescriptors_acst[idBlock_uo].nrNvBlocks_u8 +
                           NvM_Prv_BlockDescriptors_acst[idBlock_uo].nrRomBlocks_u8);

    return nrDataIndexes;
}

/**
 * \brief
 * This NvM private function returns the pointer to the configured permanent RAM block for the given block.
 *
 * \details
 * If no permanent RAM block is configured for a block then this function returns a NULL pointer.
 *
 * \param idBlock_uo
 * ID of the block for which the pointer to the configured permanent RAM block will be returned.
 *
 * \return
 * Pointer to the configured permanent RAM block
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void * NvM_Prv_BlkDesc_GetPRamBlockAddress(NvM_BlockIdType idBlock_uo)
{
    return *NvM_Prv_BlockDescriptors_acst[idBlock_uo].adrRamBlock_ppv;
}

/**
 * \brief
 * This NvM private function returns the pointer to the configured ROM block for the given block.
 *
 * \details
 * If no ROM block is configured for a block then this function returns a NULL pointer.
 *
 * \param idBlock_uo
 * ID of the block for which the pointer to the configured ROM block will be returned.
 *
 * \return
 * Pointer to the configured ROM block
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void const* NvM_Prv_BlkDesc_GetRomBlockAddress(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].adrRomBlock_pcv;
}

/**
 * \brief
 * This NvM private function returns the configured block ID used by the mem interface for the given block.
 *
 * \param idBlock_uo
 * ID of the block for which the block ID used by the mem interface will be returned.
 *
 * \return
 * Configured block ID used by the mem interface
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetIdMemIf(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].idBlockMemIf_u16;
}

/**
 * \brief
 * This NvM private function returns the configured device index where the given block is located (Fee / Ea).
 *
 * \param idBlock_uo
 * ID of the block for which the configured device index will be returned.
 *
 * \return
 * Configured device ID where the given block is located
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_IdMemoryDevice_tuo NvM_Prv_BlkDesc_GetIdDevice(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].idDevice_uo;
}

/**
 * \brief
 * This NvM private function returns the configured CRC type for the given block.
 *
 * \param idBlock_uo
 * ID of the block for which the configured device index will be returned.
 *
 * \return
 * Configured CRC type
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_Crc_Type_ten NvM_Prv_BlkDesc_GetCrcType(NvM_BlockIdType idBlock_uo)
{
    NvM_Prv_Crc_Type_ten TypeCrc_en = NvM_Prv_Crc_Type_NoCrc_e;
#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
    TypeCrc_en = NvM_Prv_BlockDescriptors_acst[idBlock_uo].TypeCrc_en;
#else
    (void)idBlock_uo;
#endif
    return TypeCrc_en;
}

/**
 * \brief
 * This NvM private function returns the configured index of the RAM block CRC for the given block.
 *
 * If CRC is generally disabled by configuration then this function always returns 0.
 *
 * \attention
 * The user shall make sure that return value of this function is used only if RAM block CRC is configured.
 *
 * \param[in] idBlock_uo
 * ID of the block for which the configured index of the RAM block CRC will be returned.
 *
 * \return
 * Configured index of the RAM block CRC
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetIdxRamBlockCrc(NvM_BlockIdType idBlock_uo)
{
    uint16 idxRamBlockCrc_u16 = 0u;
#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
    idxRamBlockCrc_u16 = NvM_Prv_BlockDescriptors_acst[idBlock_uo].idxRamBlockCrc_u16;
#else
    (void)idBlock_uo;
#endif
    return idxRamBlockCrc_u16;
}
/**
 * \brief
 * This NvM private function returns the index of the savedzone which is used for the PRAM and savedzone marker
 *
 * \param[in] idBlock_uo
 * ID of the block for which the configured index of the RAM block CRC will be returned.
 *
 * \return
 * Configured index of the index of the savedzone
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_BlkDesc_GetIdxSavedZone(NvM_BlockIdType idBlock_uo)
{
#if (NVM_RB_RAM_INIT_CHECK == NVM_RB_RAM_INIT_CHECK_QUICK)
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].idxSavedZone_u8;
#else
    (void)idBlock_uo;
    return 0u;
#endif
}
/**
 * \brief
 * This NvM private function returns the configured persistent ID of a block identified by index for persistent IDs.
 *
 * \param idxPersistentId_u16
 * Index of the required persistent ID.
 *
 * \return
 * Configured persistent ID of a block identified by index for persistent IDs
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetIdPersistent(uint16 idxPersistentId_u16)
{
    uint16 PersistentId_u16 = 0u;
    if (idxPersistentId_u16 < NVM_PRV_NR_PERSISTENT_IDS)
    {
        PersistentId_u16 = NvM_Prv_PersId_BlockId_acst[idxPersistentId_u16].PersistentId_u16;
    }
    return PersistentId_u16;
}

/**
 * \brief
 * This NvM private function returns the configured NvM ID of a block identified by index for persistent IDs.
 *
 * \param idxPersistentId_u16
 * Index of the required persistent ID.
 *
 * \return
 * Configured NvM ID of a block identified by index for persistent IDs
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_BlockIdType NvM_Prv_BlkDesc_GetIdBlock(uint16 idxPersistentId_u16)
{
    uint16 BlockId_u16 = 0u;
    if (idxPersistentId_u16 < NVM_PRV_NR_PERSISTENT_IDS)
    {
        BlockId_u16 = NvM_Prv_PersId_BlockId_acst[idxPersistentId_u16].BlockId_u16;
    }
    return BlockId_u16;
}

/**
 * \brief
 * This NvM private function returns the configured pointer to the block specific callback function
 * to copy data from the NvM mirror to the application's RAM block. Using union to allow support for
 * read callback function using void* and void const* parameter.
 *
 * \param idBlock_uo
 * ID of the block for which the configured copy function will be returned.
 *
 * \return
 * Configured pointer to the copy function
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_ExplicitSync_Copy_tpfct NvM_Prv_BlkDesc_GetCopyFctForRead(NvM_BlockIdType idBlock_uo)
{
    NvM_Prv_ReadRamBlockFromNvm_tun readFct_un;

#if (NVM_RB_EXPLICIT_SYNC_READ_WITH_CONST == STD_ON)
    readFct_un.ptrReadRamBlockFromNvmConst_pfct =  NvM_Prv_BlockDescriptors_acst[idBlock_uo].ReadRamBlockFromNvm_pfct;
#else
    readFct_un.ptrReadRamBlockFromNvm_pfct =  NvM_Prv_BlockDescriptors_acst[idBlock_uo].ReadRamBlockFromNvm_pfct;
#endif

    return readFct_un.ptrReadRamBlockFromNvm_pfct;
}

/**
 * \brief
 * This NvM private function returns the configured pointer to the block specific callback function
 * to copy data from the application's RAM block to the NvM mirror.
 *
 * \attention
 * The user shall make sure that this function is used only if explicit synchronization is configured.
 *
 * \param idBlock_uo
 * ID of the block for which the configured copy function will be returned.
 * \param InternalBuffer_pu8
 * Pointer to the internal buffer, if NvMRbInitBufferBeforeSyncWrite is enabled, the internal buffer is set to 0.
 *
 * \return
 * Configured pointer to the copy function
 */
/* MR12 RULE 5.9, 8.13 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files.
 * The statements within the function can be disabled via a compiler-switch but the parameter is still required
 * to be changeable if the statements within the function are active. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_ExplicitSync_Copy_tpfct NvM_Prv_BlkDesc_GetCopyFctForWrite(NvM_BlockIdType idBlock_uo,
                                                                                           uint8* InternalBuffer_pu8)
{
#if (NVM_RB_INIT_BUFFER_BEFORE_SYNC_WRITE == STD_ON)
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3305] Set internal buffer to 0 if NvMRbInitBufferBeforeSyncWrite is enabled
    rba_MemLib_MemSet(InternalBuffer_pu8, 0x00u, NvM_Prv_BlkDesc_GetSize(idBlock_uo));
#else
    (void)*InternalBuffer_pu8;
#endif

    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].WriteRamBlockToNvm_pfct;
}

/**
 * \brief
 * This NvM private function returns the configured block specific initializaton callback function.
 * \details
 * If no initializaton callback function is configured for the passed block then this function does nothing and
 * returns E_NOT_OK.
 * If the no initialization callback if configured ROM block feature is enabled and the passed block has a configured
 * ROM block then this function does nothing and returns E_NOT_OK.
 *
 * \param idBlock_uo
 * ID of the block for which the configured initialization callback function will be invoked.
 *
 * \return
 * E_OK = no initialization callback function is configured or has returned E_OK
 * E_NOT_OK = initialization callback function is configured and has returned E_NOT_OK
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE Std_ReturnType NvM_Prv_BlkDesc_InvokeInitBlockCallback(NvM_BlockIdType idBlock_uo,
                                                                               NvM_Prv_idService_tuo idService_uo)
{
    Std_ReturnType RetValue = E_OK;
#if (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    NvM_InitBlockRequestType NVM_RB_INIT_CALLBACK_PARAM = NvM_Prv_BlkDesc_ServiceIdToInitBlockRequestType(idService_uo);
#endif

    if ( (NULL_PTR != NvM_Prv_BlockDescriptors_acst[idBlock_uo].InitBlockCallback_pfct)
#if (NVM_RB_NO_INIT_CALLBACK_ROM_BLOCK_CONFIGURED == STD_ON)
        && (NULL_PTR == NvM_Prv_BlkDesc_GetRomBlockAddress(idBlock_uo))
#endif
       )
    {
        RetValue = NvM_Prv_BlockDescriptors_acst[idBlock_uo].InitBlockCallback_pfct(NVM_RB_INIT_CALLBACK_PARAM);
    }
    (void)idService_uo;  // avoid unused warning in case of NVM_RB_CALLBACK_SIGNATURE_AR45 != STD_ON
    return RetValue;
}

#if (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
/**
 * This function converts the Service ID to the NvM_InitBlockRequestType.
 * NvM_InitBlockRequestType is defined here :[SWS_NvM_91123]]
 *
 * \param idBlock_uo
 * Service ID
 *
 * \return
 * ID of the NvM_InitBlockRequestType
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_InitBlockRequestType NvM_Prv_BlkDesc_ServiceIdToInitBlockRequestType(NvM_Prv_idService_tuo idService_uo)
{
    NvM_InitBlockRequestType InitBlockRequest;

    switch (idService_uo)
    {
        case NVM_SERVICE_ID_RB_FIRST_INIT_ALL:
        {
            InitBlockRequest = NVM_INIT_FIRST_INIT_ALL;
        }
        break;
        case NVM_SERVICE_ID_READ_BLOCK:
        {
            InitBlockRequest = NVM_INIT_READ_BLOCK;
        }
        break;
        case NVM_SERVICE_ID_RESTORE_BLOCK_DEFAULTS:
        {
            InitBlockRequest = NVM_INIT_RESTORE_BLOCK_DEFAULTS;
        }
        break;
        case NVM_SERVICE_ID_READ_ALL:
        {
            InitBlockRequest = NVM_INIT_READ_ALL_BLOCK;
        }
        break;
        default:
        {
            InitBlockRequest = NVM_RB_NO_INIT_CALLBACK;
        }
        break;
    }

    return InitBlockRequest;
}
#endif  // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
/**
 * \brief
 * This NvM private function invokes the configured block specific single request start callback function.
 * \details
 * If no single request start callback function is configured for the passed block then this function does nothing.
 *
 * \param idBlock_uo
 * ID of the block for which the configured single request start callback function will be invoked.
 * \param idService_uo
 * Id of the request for which the start callback function will be invoked.
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeSingleBlockStartCallback(NvM_BlockIdType idBlock_uo,
                                                                            NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_SnglCallbackReq_tuo SnglCallbackReqId = NvM_Prv_BlkDesc_ServiceIdToBlockRequestType(idService_uo);
    if (NULL_PTR != NvM_Prv_BlockDescriptors_acst[idBlock_uo].SingleBlockStartCallback_pfct)
    {
        // Start callback function returns allways E_OK so return value can be dropped safely
        (void)(NvM_Prv_BlockDescriptors_acst[idBlock_uo].SingleBlockStartCallback_pfct)(SnglCallbackReqId);
    }
}

/**
 * \brief
 * This NvM private function invokes the configured block specific single request termination callback function.
 * \details
 * If no single request termination callback function is configured for the passed block then this function does nothing.
 *
 * \param idBlock_uo
 * ID of the block for which the configured termination callback function will be invoked.
 * \param idService_uo
 * Id of the request for which the termination callback function will be invoked.
 * \param Result_uo
 * Result of the terminated request.
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeSingleBlockCallback(NvM_BlockIdType idBlock_uo,
                                                                       NvM_Prv_idService_tuo idService_uo,
                                                                       NvM_RequestResultType Result_uo)
{
    NvM_Prv_SnglCallbackReq_tuo SnglCallbackReqId = NvM_Prv_BlkDesc_ServiceIdToBlockRequestType(idService_uo);

    if (NULL_PTR != NvM_Prv_BlockDescriptors_acst[idBlock_uo].SingleBlockCallback_pfct)
    {
        // Termination callback function returns allways E_OK so return value can be dropped safely
        (void)(NvM_Prv_BlockDescriptors_acst[idBlock_uo].SingleBlockCallback_pfct)(SnglCallbackReqId, Result_uo);
    }
}

/**
 * \brief
 * This NvM private function invokes the configured observer callback function.
 * \details
 * If no observer callback function is configured for the NvM then this function does nothing.
 *
 * \param idBlock_uo
 * ID of the block to be passed to the configured observer callback function.
 * \param idService_uo
 * Id of the request to be passed to the configured observer callback function.
 * \param Result_uo
 * Result of the request to be passed to the configured observer callback function.
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeObserverCallback(NvM_BlockIdType idBlock_uo,
                                                                    NvM_Prv_idService_tuo idService_uo,
                                                                    NvM_RequestResultType Result_uo)
{
    NvM_Prv_SnglCallbackReq_tuo SnglCallbackReqId = NvM_Prv_BlkDesc_ServiceIdToBlockRequestType(idService_uo);

    if (NULL_PTR != NvM_Prv_Common_cst.ObserverCallback_pfct)
    {
        // Observer callback function returns allways E_OK so return value can be dropped safely
        (void)(NvM_Prv_Common_cst.ObserverCallback_pfct)(idBlock_uo, SnglCallbackReqId, Result_uo);
    }
}

/**
 * This function converts the Service ID to the NvM_BlockRequestType.
 * NvM_BlockRequestType is defined here: [SWS_NvM_91002].
 *
 * \param idBlock_uo
 * Service ID
 *
 * \return
 * ID of the NvM_BlockRequestType
 */

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_SnglCallbackReq_tuo NvM_Prv_BlkDesc_ServiceIdToBlockRequestType(NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_SnglCallbackReq_tuo  SnglCallbackReqId;
#if (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    switch (idService_uo)
    {
        case NVM_SERVICE_ID_READ_BLOCK:
        {
            SnglCallbackReqId = NVM_READ_BLOCK;
        }
        break;
        case NVM_SERVICE_ID_WRITE_BLOCK:
        {
            SnglCallbackReqId = NVM_WRITE_BLOCK;
        }
        break;
        case NVM_SERVICE_ID_RESTORE_BLOCK_DEFAULTS:
        {
            SnglCallbackReqId = NVM_RESTORE_BLOCK_DEFAULTS;
        }
        break;
        case NVM_SERVICE_ID_ERASE_NV_BLOCK:
        {
            SnglCallbackReqId = NVM_ERASE_NV_BLOCK;
        }
        break;
        case NVM_SERVICE_ID_INVALIDATE_NV_BLOCK:
        {
            SnglCallbackReqId = NVM_INVALIDATE_NV_BLOCK;
        }
        break;
        case NVM_SERVICE_ID_READ_ALL:
        {
            SnglCallbackReqId = NVM_READ_ALL_BLOCK;
        }
        break;
        case NVM_SERVICE_ID_WRITE_ALL:
        {
            SnglCallbackReqId = NVM_RB_WRITE_ALL_BLOCK;
        }
        break;
        default:
        {
            SnglCallbackReqId = NVM_RB_NO_BLOCK_CALLBACK;
        }
        break;
    }
#else  // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    SnglCallbackReqId = idService_uo;
#endif // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    return SnglCallbackReqId;
}

/**
 * \brief
 * This NvM private function invokes the configured multi-block request start callback function.
 * \details
 * If no multi-block request start callback function is configured then this function does nothing.
 *
 * \param idService_uo
 * Id of the request for which the start callback function will be invoked.
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeMultiStartCallback(NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_MultiCallbackReq_tuo MultiCallbackReqId = NvM_Prv_BlkDesc_ServiceIdToMultiBlockRequestType(idService_uo);

    if (NULL_PTR != NvM_Prv_Common_cst.RbMultiBlockStartCallback_pfct)
    {
        NvM_Prv_Common_cst.RbMultiBlockStartCallback_pfct(MultiCallbackReqId);
    }
}

/**
 * \brief
 * This NvM private function invokes the configured multi-block request termination callback function.
 * \details
 * If no multi-block request termination callback function is configured then this function does nothing.
 *
 * \param idService_uo
 * Id of the request to be passed to the configured multi-block request termination callback function.
 * \param Result_uo
 * Result of the request to be passed to the configured multi-block request termination callback function.
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeMultiCallback(NvM_Prv_idService_tuo idService_uo,
                                                                 NvM_RequestResultType Result_uo)
{
    NvM_Prv_MultiCallbackReq_tuo MultiCallbackReqId = NvM_Prv_BlkDesc_ServiceIdToMultiBlockRequestType(idService_uo);

    if (NULL_PTR != NvM_Prv_Common_cst.MultiBlockCallback_pfct)
    {
        NvM_Prv_Common_cst.MultiBlockCallback_pfct(MultiCallbackReqId, Result_uo);
    }
}
/**
 * This function converts the Service ID to the NvM_MultiBlockRequestType.
 * NvM_MultiBlockRequestType is defined here :[SWS_NvM_91003]
 *
 * \param idBlock_uo
 * Service ID
 *
 * \return
 * ID of the NvM_MultiBlockRequestType
 */

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_MultiCallbackReq_tuo NvM_Prv_BlkDesc_ServiceIdToMultiBlockRequestType(NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_MultiCallbackReq_tuo MultiCallbackReqId;
#if (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    switch (idService_uo)
    {
        case NVM_SERVICE_ID_INIT:
        case NVM_SERVICE_ID_RB_FIRST_INIT_ALL:
        {
            MultiCallbackReqId = NVM_FIRST_INIT_ALL;
        }
        break;
        case NVM_SERVICE_ID_CANCEL_WRITE_ALL:
        {
            MultiCallbackReqId = NVM_CANCEL_WRITE_ALL;
        }
        break;
        case NVM_SERVICE_ID_READ_ALL:
        {
            MultiCallbackReqId = NVM_READ_ALL;
        }
        break;
        case NVM_SERVICE_ID_WRITE_ALL:
        {
            MultiCallbackReqId = NVM_WRITE_ALL;
        }
        break;
        case NVM_SERVICE_ID_VALIDATE_ALL:
        {
            MultiCallbackReqId = NVM_VALIDATE_ALL;
        }
        break;
        default:
        {
            MultiCallbackReqId = NVM_RB_NO_ALL_CALLBACK;
        }
        break;
    }
#else   // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    MultiCallbackReqId = idService_uo;
#endif  // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    return MultiCallbackReqId;
}

/**
 * This function appends the block version to the given buffer and returns its size
 *
 * The caller of this function has to make sure that given buffer points exactly
 * where the block version has to be appended.
 *
 * \param idBlock_uo
 * ID of the block for which the block version will be appended
 * \param Buffer_pu8
 * Pointer to the buffer where the block version will be appended
 *
 * \return
 * Size of the appended block version
 */
/* MR12 RULE 5.9, 8.13 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files.
 * The statements within the function can be disabled via a compiler-switch but the parameter is still required
 * to be changeable if the statements within the function are active. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_AppendBlockVersion(NvM_BlockIdType idBlock_uo,
                                                                  uint8* Buffer_pu8)
{
    uint16 BlockVersionSize_u16 = 0u;
#if (STD_ON == NVM_RB_BLOCK_VERSION)
    if (NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo, NVM_PRV_BLOCK_FLAG_BLOCK_VERSION))
    {
        BlockVersionSize_u16 = sizeof(NvM_Prv_BlockDescriptors_acst[idBlock_uo].BlockVersion_u8);
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON)) && !defined(TESTCD_NO_INLINE)
        if ( TestCd_NvM_st.Arguments.EnablePatchBlockVersion_b &&
            (TestCd_NvM_st.Arguments.PatchedVersionBlockId_uo == idBlock_uo) )
        {
            *Buffer_pu8 = TestCd_NvM_st.Arguments.PatchedBlockVersion_u8;
            TestCd_NvM_st.Arguments.EnablePatchBlockVersion_b = FALSE;
        }
        else
        {
            *Buffer_pu8 = NvM_Prv_BlockDescriptors_acst[idBlock_uo].BlockVersion_u8;
        }

# else
        *Buffer_pu8 = NvM_Prv_BlockDescriptors_acst[idBlock_uo].BlockVersion_u8;
# endif
    }
#else
    (void)idBlock_uo;
    (void)*Buffer_pu8;
#endif // NVM_RB_BLOCK_VERSION
    return BlockVersionSize_u16;
}

/**
 * This function extracts the block version from the given buffer and returns whether it is valid or not
 * by checking if the extracted block version matches the configured block version or not.
 *
 * The caller of this function has to make sure that given buffer contains the block version at the end
 * of the given buffer.
 * If user has not configured the block version for the given block then this function extracts nothing
 * and returns TRUE.
 *
 * \param idBlock_uo
 * ID of the block for which the block version will be extracted
 * \param Buffer_pu8
 * Pointer to the buffer with the block version
 * \param SizeInBytes_pu16
 * Size of the buffer with the block version in bytes
 *
 * \return
 * - TRUE = block version is valid (matches with the configured one) or the block does not have a block version.
 * - FALSE = block version is invalid (missmatches with the configured one)
 */
/* MR12 RULE 5.9, 8.13 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files.
 * The statements within the function can be disabled via a compiler-switch but the parameter is still required
 * to be changeable if the statements within the function are active. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_ExtractCheckBlockVersion(NvM_BlockIdType idBlock_uo,
                                                                         uint8 const* Buffer_pu8,
                                                                         uint16* SizeInBytes_pu16)
{
    boolean isBlockVersionValid_b = TRUE;
#if (STD_ON == NVM_RB_BLOCK_VERSION)

    if (NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo, NVM_PRV_BLOCK_FLAG_BLOCK_VERSION))
    {
        uint16 SizeBlockVersion_u16 = sizeof(NvM_Prv_BlockDescriptors_acst[idBlock_uo].BlockVersion_u8);
        *SizeInBytes_pu16 -= SizeBlockVersion_u16;

        if (NvM_Prv_BlockDescriptors_acst[idBlock_uo].BlockVersion_u8 != Buffer_pu8[*SizeInBytes_pu16])
        {
            isBlockVersionValid_b = FALSE;
        }
    }

#else

    (void)idBlock_uo;
    (void)*Buffer_pu8;
    (void)*SizeInBytes_pu16;

#endif  // NVM_RB_BLOCK_VERSION

    return isBlockVersionValid_b;
}

/**
 * This function returns split-block configuration data for a given block ID.
 *
 * If given block ID refers to a block which does not use block splitting then this function returns NULL_PTR.
 *
 * \param idBlock_uo
 * ID of the block for which the split-block configuration data will be returned
 *
 * \return
 * Pointer to the split-block configuration data
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Cfg_BlockSplit_tst const* NvM_Prv_BlkDesc_GetCfgBlockSplit(NvM_BlockIdType idBlock_uo)
{
    NvM_Cfg_BlockSplit_tst const* CfgSplitBlocks_pcst = NULL_PTR;

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    uint16 idxCfgSplitBlocks_u16 = NvM_Prv_BlockDescriptors_acst[idBlock_uo].idxCfgSplitBlocks_u16;

    if (idxCfgSplitBlocks_u16 < NVM_CFG_NR_SPLIT_BLOCKS)
    {
        CfgSplitBlocks_pcst = &NvM_Cfg_SplitBlocks_acst[idxCfgSplitBlocks_u16];
    }
#else
    (void)idBlock_uo;
#endif

    return CfgSplitBlocks_pcst;
}


/**
 * This function returns pointer to the array device IDs configured for high-speed-mode.
 *
 * If no device ID is configured for high-speed-mode then this function returns NULL_PTR.
 *
 * \return
 * Pointer to device IDs configured for high-speed-mode
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_IdMemoryDevice_tuo const* NvM_Prv_BlkDesc_GetIdDeviceHighSpeedMode(void)
{
#if (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u)
    return NvM_Prv_idDevicesHighSpeedMode_cauo;
#else
    return NULL_PTR;
#endif
}

/**
 * \brief
 * This NvM private function provides the information whether the given block can be processed in high-speed-mode.
 *
 * \param[in] idBlock_uo
 * ID of the block to get the processability
 *
 * \return
 * - TRUE = block can be processed in high-speed-mode
 * - FALSE = block can NOT be processed in high-speed-mode
 */
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsHighSpeedModeProcessable(NvM_BlockIdType idBlock_uo)
{
    boolean isHighSpeedModeProcessable_b = FALSE;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    isHighSpeedModeProcessable_b = NvM_Prv_BlockDescriptors_acst[idBlock_uo].isHighSpeedModeProcessable_b;
#else
    (void)idBlock_uo;
#endif

    return isHighSpeedModeProcessable_b;
}

#if (NVM_CRYPTO_USED == STD_ON)

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetPersistantId(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].PersistentId_u16;
}

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetLengthJobCsm(NvM_BlockIdType idBlock_uo,
                                                               NvM_Prv_Crypto_idService_ten idServiceCrypto_en)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].CryptoConfig_st.LengthJobCsm_auo[idServiceCrypto_en];
}

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint32 NvM_Prv_BlkDesc_GetIdJobCsm(NvM_BlockIdType idBlock_uo,
                                                           NvM_Prv_Crypto_idService_ten idServiceCrypto_en)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].CryptoConfig_st.idJobCsm_auo[idServiceCrypto_en];
}

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 const* NvM_Prv_BlkDesc_GetDataJobCsmAssociated(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].CryptoConfig_st.dataJobCsmAssociated_pcu8;
}

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetLengthJobCsmAssociated(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].CryptoConfig_st.LengthJobCsmAssociated_uo;
}

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetLengthJobCsmTag(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].CryptoConfig_st.LengthJobCsmTag_uo;
}

/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetPositionJobCsmInitVector(NvM_BlockIdType idBlock_uo)
{
    return NvM_Prv_BlockDescriptors_acst[idBlock_uo].CryptoConfig_st.PositionKeyInitVector;
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_BLOCKDESCRIPTOR_INL_H */
#endif
