/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_BLOCKDESCRIPTOR_TYPES_H
#define NVM_PRV_BLOCKDESCRIPTOR_TYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Types.h"
#include "NvM_Prv_JobResource_Types.h"

#if (NVM_CRYPTO_USED == STD_ON)
#include "NvM_Prv_Crypto_Types.h"
#endif

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
/**
 * Definition of the immediate job priority for a NVRAM block.
 * TRACE[ECUC_NvM_00477]
 */
#define NVM_PRV_JOB_PRIORITY_IMMEDIATE      (0u)

// Definitions for giving special block IDs some semantics
// ------------------------------------------------------------------------------------------------
/**
 * Definition of the reserved block ID used for multi block operations.
 * TRACE[NVM471]
 */
#define NVM_PRV_MULTI_BLOCK         (0u)

#if (NVM_RB_AUX_INTERFACE == STD_ON)
/**
 * Definition of the block ID used for administrative information related to operations triggered by auxiliary users
 * (for this use case, an additional admin block has been reserved behind all "regular" admin blocks).
 */
# define NVM_PRV_AUX_ADMIN_BLOCK    NVM_CFG_NR_ALL_BLOCKS
#endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * Definition of the union for different signatures of callback function to copy data from the NvM mirror
 * to the application's RAM block.
 */
typedef union
{
    /// callback signature with void const* parameter
    Std_ReturnType (*ptrReadRamBlockFromNvmConst_pfct)(void const* NvMBuffer);
    /// callback signature with void* parameter
    Std_ReturnType (*ptrReadRamBlockFromNvm_pfct)(void* NvMBuffer);

} NvM_Prv_ReadRamBlockFromNvm_tun;

/**
 * \typedef
 * Type definition for copy functions used for the explicit synchronization.
 *
 * This type makes it possible for the explicit synchronization to write user data into the NvM internal buffer and
 * to read user data from the NvM internal buffer in a common way.
 */
typedef Std_ReturnType (*NvM_Prv_ExplicitSync_Copy_tpfct)(void* NvMBuffer_pv);

/**
 * Definition of different bit masks to access different configuration flags withis block descriptors
 */
typedef enum
{
    /// Bit mask definition to get information whether a block shall be processed during NvM_ReadAll.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM117_Conf] Parameter NvMBlockDescriptor:NvMSelectBlockForReadAll
    NVM_PRV_BLOCK_FLAG_SELECT_FOR_READ_ALL              = 0x00000001u,
    /// Bit mask definition to get information whether the block shall be processed during NvM_WriteAll.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM549_Conf] Parameter NvMBlockDescriptor:NvMSelectBlockForWriteAll
    NVM_PRV_BLOCK_FLAG_SELECT_FOR_WRITE_ALL             = 0x00000002u,
    /// Bit mask definition to get information whether the block shall be processed during NvM_Rb_FirstInitAll.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2828] Parameter NvMBlockDescriptor:NvMRbSelectBlockForFirstInitAll
    NVM_PRV_BLOCK_FLAG_SELECT_FOR_FIRST_INIT_ALL        = 0x00000004u,
    /// Bit mask definition to get information whether the block shall be initialized when a layout change is detected.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2881] Parameter NvMBlockDescriptor:NvMRbInitBlockAtLayoutChange
    NVM_PRV_BLOCK_FLAG_SELECT_FOR_INIT_AT_LAYOUT_CHANGE = 0x00000008u,
    /// Bit mask definition to get information whether the block is initially write protected.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM033_Conf] Parameter NvMBlockDescriptor:NvMBlockWriteProt
    NVM_PRV_BLOCK_FLAG_WRITE_PROTECTED                  = 0x00000010u,
    /// Bit mask definition to get information whether the block shall be written only once.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM072_Conf] Parameter NvMBlockDescriptor:NvMWriteBlockOnce
    NVM_PRV_BLOCK_FLAG_WRITE_ONCE                       = 0x00000020u,
    /// Bit mask definition to get information whether the block shall be resistant to block layout changes.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM483_Conf] ParameterNvMBlockDescriptor:NvMResistantToChangedSw
    NVM_PRV_BLOCK_FLAG_RESISTANT_TO_CHANGED_SW          = 0x00000040u,
    /// Bit mask definition to get information whether the block shall use explicit synchronization mechanism.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM519_Conf] Parameter NvMBlockDescriptor:NvMBlockUseSyncMechanism
    NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM               = 0x00000080u,
    /// Bit mask definition to get information whether the block shall be processed during NvM_ValidateAll.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM557_Conf] Parameter NvMBlockDescriptor:NvMBlockUseAutoValidation
    NVM_PRV_BLOCK_FLAG_USE_AUTO_VALIDATION              = 0x00000100u,
    /// Bit mask definition to get information whether the block is using a variable block length.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2996] Parameter NvMBlockDescriptor:NvMRbNvBlockLengthString
    NVM_PRV_BLOCK_FLAG_USE_VARIABLE_BLOCK_LENGTH        = 0x00000200u,
    /// Bit mask definition to get information whether the block is configured for migration.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3027] Parameter NvMBlockDescriptor:NvMRbFirstReadDataMigration
    NVM_PRV_BLOCK_FLAG_SELECT_FOR_MIGRATION             = 0x00000400u,
    /// Bit mask definition to get information whether the block shall be initialized unconditionally
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3175] Parameter NvMBlockDescriptor:NvMRbRamInitUnconditional
    NVM_PRV_BLOCK_FLAG_RAM_INIT_UNCONDITIONAL           = 0x00000800u,
    /// Bit mask definition to get information whether the block shall use CRC compare mechanism.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[ECUC_NvM_00556] Parameter NvMBlockDescriptor:NvMBlockUseCRCCompMechanism
    NVM_PRV_BLOCK_FLAG_USE_CRC_COMP_MECHANISM           = 0x00001000u,
    /// Bit mask definition to get information whether RAM CRC shall be calculated for the (permanent RAM) block.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM119_Conf] Parameter NvMBlockDescriptor:NvMCalcRamBlockCrc
    NVM_PRV_BLOCK_FLAG_USE_RAM_CRC                      = 0x00002000u,
    /// Bit mask definition to get information whether block consistency is protected via a CRC in NV media.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    /// TRACE[NVM036_Conf] Parameter NvMBlockDescriptor:NvMBlockUseCrc
    NVM_PRV_BLOCK_FLAG_USE_NV_CRC                       = 0x00004000u,
    /// Bit mask definition to get information whether the block shall use cryptographic services.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    NVM_PRV_BLOCK_FLAG_USE_CRYPTO                       = 0x00008000u,
    /// Bit mask definition to get information whether the block shall have a counter for write requests on it.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    NVM_PRV_BLOCK_FLAG_CNTR_WRITE                       = 0x00010000u,
    /// Bit mask definition to get information whether the block shall have a block version.
    /// This information is stored in the NVRAM block descriptor, see NvM_Prv_BlockDescriptor_tst.stFlags_uo
    NVM_PRV_BLOCK_FLAG_BLOCK_VERSION                    = 0x00020000u

} NvM_Prv_BlockConfiguration_ten;

/**
 * \typedef
 * Type definition ID of memory device defined by parameter NvMBlockDescriptor:NvMNvramDeviceId.
 */
typedef uint8   NvM_Prv_IdMemoryDevice_tuo;

/**
 * Definition of the structure for common configuration options of the NvM used during the runtime.
 * TRACE[NVM028_Conf]
 */
typedef struct
{
    /**
     * Pointer to a callback function which shall be invoked on termination of each asynchronous multi-block request.
     * TRACE[NVM468]
     * TRACE[NVM331]
     * TRACE[NVM500_Conf] Parameter NvMCommon:NvMMultiBlockCallback
     */
    void (*MultiBlockCallback_pfct)(NvM_Prv_MultiCallbackReq_tuo MultiCallbackReqId, NvM_RequestResultType JobResult);

    /**
     * Pointer to a callback function which shall be invoked on start of each asynchronous multi-block request.
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2762] Parameter NvMCommon:NvMRbMultiBlockStartCallback
     */
    void (*RbMultiBlockStartCallback_pfct)(NvM_Prv_MultiCallbackReq_tuo MultiCallbackReqId);
    /**
     * Pointer to a callback function which shall be invoked on termination of each asynchronous single-block request.
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2842]
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2843] Parameter NvMCommon:NvMRbObserverCallback
     */
    Std_ReturnType (*ObserverCallback_pfct)(NvM_BlockIdType BlockId,
                                            NvM_Prv_SnglCallbackReq_tuo  SnglCallbackReqId,
                                            NvM_RequestResultType JobResult);

} NvM_Prv_Common_tst;

/**
 * Definition of the structure for a NVRAM block descriptor.
 * A NVRAM block descriptor contains all information about a single NVRAM block needed during the runtime.
 * The NVRAM block descriptors are condensed in the NVRAM block descriptor table.
 * TRACE[NVM061_Conf]
 */
typedef struct
{
#if (NVM_RB_BLOCK_VERSION == STD_ON)
    /**
     * Block version, it contains the configured block version information
     */
    uint8 BlockVersion_u8;
#endif
    /**
     * Block ID to be passed to the MemIf.
     * Do not confuse this ID with NVRAM block ID used within NvM.
     */
    uint16 idBlockMemIf_u16;
    /**
     * Pointer to the block legth in bytes.
     * If runtime calculation of block lengths is enabled by the NvM configuration then block lengths will be located
     * in RAM so pointer points to a variable with block length calculated during runtime.
     * Otherwise if runtime calculation is disabled by the NvM configuration then block legths will be located
     * in ROM so pointer points to a constant with block length configured during compile time.
     * TRACE[NVM479_Conf] Parameter NvMBlockDescriptor:NvMNvBlockLength
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3028] Parameter NvMCommon:NvMRbRuntimeRamBlockConfiguration
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2996] Parameter NvMBlockDescriptor:NvMRbNvBlockLengthString
     */
#if (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON)
    uint16* nrBlockBytes_pu16;
#else
    uint16 const* nrBlockBytes_pu16;

#endif

    /**
     * Block length in bytes stored on the medium.
     * Dependent on the block configuration it is possible that not only plain user data will be stored.
     * Following configuration options are available:
     * - only user data is stored (normal use case)
     * - user data + [signature] + [CRC] = encryption disabled + signing on/off + NV CRC on/off
     * - encrypted user data + [signature] + [CRC] = encryption enabled + signing on/off + NV CRC on/off
     */
    uint16 nrBlockBytesStored_u16;

    /**
     * Device ID where the block is located (Fee / Ea).
     * TRACE[NVM035_Conf] Parameter NvMBlockDescriptor:NvMNvramDeviceId
     */
    NvM_Prv_IdMemoryDevice_tuo idDevice_uo;
    /**
     * Number of non-volatile blocks in a contiguous area according to the given block management type.
     * TRACE[NVM480_Conf] Parameter NvMBlockDescriptor:NvMNvBlockNum
     */
    uint8 nrNvBlocks_u8;
    /**
     * Number of multiple ROM blocks in a contiguous area according to the given block management type.
     * TRACE[NVM485_Conf] Parameter NvMBlockDescriptor:NvMRomBlockNum
     */
    uint8 nrRomBlocks_u8;
    /**
     * Pointer to the permanent RAM block data.
     * If this pointer is NULL_PTR then no permanent RAM block data has been configured by the user.
     * If runtime calculation of RAM block data addresses is enabled by the NvM configuration
     * then RAM block data addresses will be located in RAM and pointer points to a void pointer
     * calculated during runtime.
     * Otherwise if runtime calculation is disabled by the NvM configuration then RAM block data addresses
     * will be located in ROM and pointer points to a constant void pointer configured during compile time.
     * TRACE[NVM126]
     * TRACE[NVM482_Conf] Parameter NvMBlockDescriptor:NvMRamBlockDataAddress
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3028] Parameter NvMCommon:NvMRbRuntimeRamBlockConfiguration
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3086] Parameter NvMBlockDescriptor:NvMRbRamBlockDataAddressString
     */
#if (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON)
    void** adrRamBlock_ppv;
#else
    void* const* adrRamBlock_ppv;
#endif
    /**
     * Pointer to the ROM block data.
     * If this pointer is NULL_PTR then no ROM block data has been configured by the user.
     * TRACE[NVM020]
     * TRACE[NVM484_Conf] Parameter NvMBlockDescriptor:NvMRomBlockDataAddress
     */
    void const* adrRomBlock_pcv;
    /**
     * Pointer to the block specific callback function which shall be invoked on termination
     * of each asynchronous single-block request.
     * If this pointer is NULL_PTR then no callback has been configured by the user.
     * TRACE[NVM113]
     * TRACE[NVM467]
     * TRACE[NVM330]
     * TRACE[NVM506_Conf] Parameter NvMBlockDescriptor:NvMSingleBlockCallback
     */
    Std_ReturnType (*SingleBlockCallback_pfct)(NvM_Prv_SnglCallbackReq_tuo  SnglCallbackReqId, NvM_RequestResultType JobResult);
    /**
     * Pointer to the block specific callback function which shall be invoked on start
     * of each asynchronous single-block request.
     * If this pointer is NULL_PTR then no callback has been configured by the user.
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3056]
     * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3062] Parameter NvMBlockDescriptor:NvMRbSingleBlockStartCallback
     */
    Std_ReturnType (*SingleBlockStartCallback_pfct)(NvM_Prv_SnglCallbackReq_tuo  SnglCallbackReqId);
    /**
     * Pointer to the block specific callback function which shall be invoked if no ROM block data is available
     * for initialization of the block.
     * If this pointer is NULL_PTR then no callback has been configured by the user.
     * TRACE[NVM469]
     * TRACE[NVM352]
     * TRACE[NVM116_Conf] Parameter NvMBlockDescriptor:NvMInitBlockCallback
     */
    Std_ReturnType (*InitBlockCallback_pfct)(NVM_RB_INIT_CALLBACK_TYP_PARAM);
    /**
     * Pointer to the block specific callback function to let the application copy data from the NvM mirror
     * to application's RAM block.
     * If this pointer is NULL_PTR then no callback has been configured by the user.
     * TRACE[NVM540]
     * TRACE[NVM542]
     * TRACE[NVM521_Conf] Parameter NvMBlockDescriptor:NvMReadRamBlockFromNvCallback
     */
#if (NVM_RB_EXPLICIT_SYNC_READ_WITH_CONST == STD_ON)
    Std_ReturnType (*ReadRamBlockFromNvm_pfct)(void const* NvMBuffer);
#else
    Std_ReturnType (*ReadRamBlockFromNvm_pfct)(void* NvMBuffer);
#endif
    /**
     * Pointer the block specific callback function to let the application copy data from the application's RAM block
     * to the NvM mirror.
     * If this pointer is NULL_PTR then no callback has been configured by the user.
     * TRACE[NVM539]
     * TRACE[NVM541]
     * TRACE[NVM520_Conf] Parameter NvMBlockDescriptor:NvMWriteRamBlockToNvCallback
     */
    Std_ReturnType (*WriteRamBlockToNvm_pfct)(void* NvMBuffer);
    /**
     * Block management type.
     * TRACE[NVM062_Conf] Parameter NvMBlockDescriptor:NvMBlockManagementType
     */
    NvM_BlockManagementType BlockManagementType_en;
    /**
     * Block job priority.
     * TRACE[NVM477_Conf] Parameter NvMBlockDescriptor:NvMBlockJobPriority
     */
    uint8 JobPriority_u8;
    /**
     * Bit-field containing several configuration parameter which can be represented by a bit.
     * NvM provides special bit mask to access individual bits from this bit-field:
     * - NVM_PRV_BLOCK_FLAG_SELECT_FOR_READ_ALL
     *   TRACE[NVM117_Conf] Parameter NvMBlockDescriptor:NvMSelectBlockForReadAll
     * - NVM_PRV_BLOCK_FLAG_SELECT_FOR_WRITE_ALL
     *   TRACE[NVM549_Conf] Parameter NvMBlockDescriptor:NvMSelectBlockForWriteAll
     * - NVM_PRV_BLOCK_FLAG_SELECT_FOR_FIRST_INIT_ALL
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2828] Parameter NvMBlockDescriptor:NvMRbSelectBlockForFirstInitAll
     * - NVM_PRV_BLOCK_FLAG_SELECT_FOR_INIT_AT_LAYOUT_CHANGE
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2881] Parameter NvMBlockDescriptor:NvMRbInitBlockAtLayoutChange
     * - NVM_PRV_BLOCK_FLAG_WRITE_PROTECTED
     *   TRACE[NVM033_Conf] Parameter NvMBlockDescriptor:NvMBlockWriteProt
     * - NVM_PRV_BLOCK_FLAG_WRITE_ONCE
     *   TRACE[NVM072_Conf] Parameter NvMBlockDescriptor:NvMWriteBlockOnce
     * - NVM_PRV_BLOCK_FLAG_RESISTANT_TO_CHANGED_SW
     *   TRACE[NVM483_Conf] Parameter NvMBlockDescriptor:NvMResistantToChangedSw
     * - NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM
     *   TRACE[NVM519_Conf] Parameter NvMBlockDescriptor:NvMBlockUseSyncMechanism
     * - NVM_PRV_BLOCK_FLAG_USE_AUTO_VALIDATION
     *   TRACE[NVM557_Conf] Parameter NvMBlockDescriptor:NvMBlockUseAutoValidation
     * - NVM_PRV_BLOCK_FLAG_USE_VARIABLE_BLOCK_LENGTH
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2996] Parameter NvMBlockDescriptor:NvMRbNvBlockLengthString
     * - NVM_PRV_BLOCK_FLAG_SELECT_FOR_MIGRATION
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3027] Parameter NvMBlockDescriptor:NvMRbFirstReadDataMigration
     * - NVM_PRV_BLOCK_FLAG_RAM_INIT_UNCONDITIONAL
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3175] Parameter NvMBlockDescriptor:NvMRbRamInitUnconditional
     * - NVM_PRV_BLOCK_FLAG_USE_CRC_COMP_MECHANISM
     *   TRACE[ECUC_NvM_00556] Parameter NvMBlockDescriptor:NvMBlockUseCRCCompMechanism
     * - NVM_PRV_BLOCK_FLAG_USE_RAM_CRC
     *   TRACE[NVM119_Conf] Parameter NvMBlockDescriptor:NvMCalcRamBlockCrc
     * - NVM_PRV_BLOCK_FLAG_USE_NV_CRC
     *   TRACE[NVM036_Conf] Parameter NvMBlockDescriptor:NvMBlockUseCrc
     * - NVM_PRV_BLOCK_FLAG_USE_CRYPTO
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3249] Parameter NvMRbSecServices:NVMRbAddEncryption
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3251] Parameter NvMRbSecServices:NvMRbAddSignature
     * - NVM_PRV_BLOCK_FLAG_USE_WRITE_CNTR
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3295] Parameter NvMCommon:NvMRbCommonWriteCounter
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3296] Parameter NvMBlockDescriptor:NvMRbWriteCounter
     *   NVM_PRV_BLOCK_FLAG_BLOCK_VERSION
     *   TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3343] Parameter NvMBlockDescriptor:NvMRbBlockVersion
     */
    uint32 stFlags_uo;

    NvM_Prv_idJobResource_tuo idJobResource_auo[NvM_Prv_JobResource_nrCluster_e];

#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
    /**
     * CRC type.
     * TRACE[ECUC_NvM_00476] Parameter NvMBlockDescriptor:NvMBlockCrcType
     */
    NvM_Prv_Crc_Type_ten TypeCrc_en;
    /**
     * Index of the RAM block CRC in the array containing all RAM block CRCs of the same CRC type.
     */
    uint16 idxRamBlockCrc_u16;
#endif

#if (NVM_CRYPTO_USED == STD_ON)
    uint16 PersistentId_u16;   // Persistent Id
    /**
     * Block configuration for cryptographic services
     */
    NvM_Prv_Crypto_BlockConfig_tst CryptoConfig_st;
#endif

#if (NVM_RB_BLOCK_HEADER > 0u)
    uint8 BlockHeader_au8[NVM_RB_BLOCK_HEADER_LENGTH];
#endif
#if (NVM_RB_RAM_INIT_CHECK == NVM_RB_RAM_INIT_CHECK_QUICK)
    uint8 idxSavedZone_u8;
#endif
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    /**
     * Index of the split block configuation in the array containing configuration data for all blocks configured
     * to use block splitting
     */
    uint16 idxCfgSplitBlocks_u16;
#endif

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    boolean isHighSpeedModeProcessable_b;
#endif
} NvM_Prv_BlockDescriptor_tst;

/**
 * Definition of the structure to assign the persistent ID to the block ID for a NVRAM block.
 */
typedef struct
{
    uint16          PersistentId_u16;   // Persistent Id
    NvM_BlockIdType BlockId_u16;        // Block Id
} NvM_Prv_PersId_BlockId_tst;


/* NVM_PRV_BLOCKDESCRIPTOR_H */
#endif
