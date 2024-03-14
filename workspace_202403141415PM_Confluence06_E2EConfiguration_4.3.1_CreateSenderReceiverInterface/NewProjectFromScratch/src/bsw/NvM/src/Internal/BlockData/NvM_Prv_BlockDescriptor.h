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
#ifndef NVM_PRV_BLOCKDESCRIPTOR_H
#define NVM_PRV_BLOCKDESCRIPTOR_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_MemLib_Special.h"

#include "NvM_Prv_BlockDescriptor_Types.h"
#include "NvM_Prv_BlockSplit_Types.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
// Declaration of different configuration-dependent data.
// Definition of all this data is done in NvM_Cfg.c.
// ------------------------------------------------------------------------------------------------
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/**
 * Declaration of the container with all common configuration data.
 * TRACE[NVM028_Conf]
 */
extern const NvM_Prv_Common_tst             NvM_Prv_Common_cst;
/**
 * Declaration of the NVRAM block descriptor table.
 * TRACE[NVM061_Conf]
 */
extern const NvM_Prv_BlockDescriptor_tst    NvM_Prv_BlockDescriptors_acst[NVM_CFG_NR_ALL_BLOCKS];
/**
 * Declaratiion of the table to get the NvM block ID using the persistent ID.
 */
extern const NvM_Prv_PersId_BlockId_tst     NvM_Prv_PersId_BlockId_acst[NVM_PRV_NR_PERSISTENT_IDS];

/**
 * Declaration of the table with blocks IDs which are configured for RAM block CRC.
 */
#if (defined(NVM_CFG_CRC_NR_RAM_BLOCKS) && (NVM_CFG_CRC_NR_RAM_BLOCKS > 0u))
extern NvM_BlockIdType const NvM_Prv_idBlockRamCrc_cauo[NVM_CFG_CRC_NR_RAM_BLOCKS];
#endif

#if (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u)
extern NvM_Prv_IdMemoryDevice_tuo const NvM_Prv_idDevicesHighSpeedMode_cauo[NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE];
#endif

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
extern NvM_Cfg_BlockSplit_tst const NvM_Cfg_SplitBlocks_acst[NVM_CFG_NR_SPLIT_BLOCKS];
#endif

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#if (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON)
# define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
# include "NvM_MemMap.h"

extern uint16 NvM_Prv_BlockLengths_au16[NVM_CFG_NR_ALL_BLOCKS];
extern void *NvM_Prv_RamBlockAdr_apv[NVM_CFG_NR_ALL_BLOCKS];

# define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
# include "NvM_MemMap.h"
#else
# define NVM_START_SEC_CONST_UNSPECIFIED
# include "NvM_MemMap.h"

extern uint16 const NvM_Prv_BlockLengths_acu16[NVM_CFG_NR_ALL_BLOCKS];
extern void * const NvM_Prv_RamBlockAdr_acpv[NVM_CFG_NR_ALL_BLOCKS];

# define NVM_STOP_SEC_CONST_UNSPECIFIED
# include "NvM_MemMap.h"
#endif

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

// Init-related functions (defined in NvM_Cfg.c)
extern void NvM_Prv_InitRamBlockProperties(void);

/*
**********************************************************************************************************************
* Inline functions declarations
**********************************************************************************************************************
*/
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsBlockHeaderEnabled(void);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_AppendBlockHeader(NvM_BlockIdType idBlock_uo,
                                                               uint8* Buffer_pu8);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJobResource_tuo NvM_Prv_BlkDesc_GetIdJobResource(NvM_BlockIdType idBlock_uo,
                                                                                   NvM_Prv_JobResource_Cluster_ten Cluster_en);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsBlockSelected(NvM_BlockIdType idBlock_uo,
                                                                NvM_Prv_BlockConfiguration_ten SelectionMask_en);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsDefaultDataAvailable(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsLengthValid(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idQueue_tuo NvM_Prv_BlkDesc_GetIdQueueForModifyingServices(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_HasBlockImmediateJobPriority(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetSize(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetBlockSizeStored(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_BlockManagementType NvM_Prv_BlkDesc_GetType(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_BlkDesc_GetNrNonVolatileBlocks(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetNrDataIndexes(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void * NvM_Prv_BlkDesc_GetPRamBlockAddress(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void const* NvM_Prv_BlkDesc_GetRomBlockAddress(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetIdMemIf(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_IdMemoryDevice_tuo NvM_Prv_BlkDesc_GetIdDevice(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_Crc_Type_ten NvM_Prv_BlkDesc_GetCrcType(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetIdxRamBlockCrc(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_BlkDesc_GetIdxSavedZone(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetIdPersistent(uint16 idxPersistentId_u16);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_BlockIdType NvM_Prv_BlkDesc_GetIdBlock(uint16 idxPersistentId_u16);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_ExplicitSync_Copy_tpfct NvM_Prv_BlkDesc_GetCopyFctForRead(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_ExplicitSync_Copy_tpfct NvM_Prv_BlkDesc_GetCopyFctForWrite(NvM_BlockIdType idBlock_uo,
                                                                                           uint8* InternalBuffer_pu8);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE Std_ReturnType NvM_Prv_BlkDesc_InvokeInitBlockCallback(NvM_BlockIdType idBlock_uo,
                                                                               NvM_Prv_idService_tuo idService_uo);
#if (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_InitBlockRequestType NvM_Prv_BlkDesc_ServiceIdToInitBlockRequestType(NvM_Prv_idService_tuo idService_uo);
#endif
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeSingleBlockStartCallback(NvM_BlockIdType idBlock_uo,
                                                                            NvM_Prv_idService_tuo idService_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeSingleBlockCallback(NvM_BlockIdType idBlock_uo,
                                                                       NvM_Prv_idService_tuo idService_uo,
                                                                       NvM_RequestResultType Result_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeObserverCallback(NvM_BlockIdType idBlock_uo,
                                                                    NvM_Prv_idService_tuo idService_uo,
                                                                    NvM_RequestResultType Result_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeMultiStartCallback(NvM_Prv_idService_tuo idService_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_BlkDesc_InvokeMultiCallback(NvM_Prv_idService_tuo idService_uo,
                                                                 NvM_RequestResultType Result_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_SnglCallbackReq_tuo NvM_Prv_BlkDesc_ServiceIdToBlockRequestType(NvM_Prv_idService_tuo idService_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_MultiCallbackReq_tuo NvM_Prv_BlkDesc_ServiceIdToMultiBlockRequestType(NvM_Prv_idService_tuo idService_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_AppendBlockVersion(NvM_BlockIdType idBlock_uo,
                                                                  uint8* Buffer_pu8);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_ExtractCheckBlockVersion(NvM_BlockIdType idBlock_uo,
                                                                         uint8 const* Buffer_pu8,
                                                                         uint16* SizeInBytes_pu16);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Cfg_BlockSplit_tst const* NvM_Prv_BlkDesc_GetCfgBlockSplit(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_IdMemoryDevice_tuo const* NvM_Prv_BlkDesc_GetIdDeviceHighSpeedMode(void);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_BlkDesc_IsHighSpeedModeProcessable(NvM_BlockIdType idBlock_uo);

#if (NVM_CRYPTO_USED == STD_ON)
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetPersistantId(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetLengthJobCsm(NvM_BlockIdType idBlock_uo,
                                                               NvM_Prv_Crypto_idService_ten idServiceCrypto_en);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint32 NvM_Prv_BlkDesc_GetIdJobCsm(NvM_BlockIdType idBlock_uo,
                                                           NvM_Prv_Crypto_idService_ten idServiceCrypto_en);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint8 const* NvM_Prv_BlkDesc_GetDataJobCsmAssociated(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetLengthJobCsmAssociated(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetLengthJobCsmTag(NvM_BlockIdType idBlock_uo);
/* MR12 RULE 5.9 VIOLATION: To simplify unit test the declaration and definition of this inline functioin is located
 * in two different header files. */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_BlkDesc_GetPositionJobCsmInitVector(NvM_BlockIdType idBlock_uo);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline function definitions
 **********************************************************************************************************************
 */
/* Include the actual definitions of the provided inline functions */
#include "NvM_Prv_BlockDescriptor_Inl.h"

/* NVM_PRV_BLOCKDESCRIPTOR_H */
#endif
