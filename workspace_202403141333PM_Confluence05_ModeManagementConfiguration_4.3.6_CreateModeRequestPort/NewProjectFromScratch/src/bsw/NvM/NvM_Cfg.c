/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 */
// TRACE[NVM321]
// NvM file containing all configuration parameters which are to be implemented as constants

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "NvM_Cfg.h"
#include "NvM_Prv_HideRteApi.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
/* prevent inclusion of TestCd_NvM.h to avoid warnings */
# define TESTCD_NVM_H
# define TESTCD_NO_INLINE
#endif

#include "MemIf.h"
// TRACE[NVM089]
// Check version compatibility of included header files
#if (!defined(MEMIF_AR_RELEASE_MAJOR_VERSION) || (MEMIF_AR_RELEASE_MAJOR_VERSION != NVM_AR_RELEASE_MAJOR_VERSION))
    #error "AUTOSAR major version undefined or mismatched"
#endif

#if (NVM_CRYPTO_USED == STD_ON)
# include "Csm.h"
// TRACE[NVM089] Check version compatibility of included header files
# if (!defined(CSM_AR_RELEASE_MAJOR_VERSION) || (CSM_AR_RELEASE_MAJOR_VERSION != NVM_AR_RELEASE_MAJOR_VERSION))
#  error "AUTOSAR major version undefined or mismatched"
# endif
# if (!defined(CSM_AR_RELEASE_MINOR_VERSION) || (CSM_AR_RELEASE_MINOR_VERSION < 3))
#  error "AUTOSAR minor version undefined or mismatched"
# endif
#endif

#include "NvM_Prv.h"
#include "NvM_Prv_BlockData_Var.h"
#include "NvM_Prv_InternalBuffer.h"
#include "NvM_Prv_BlockDescriptor.h"


// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2997]
// Include customer/user specific declarations
#include "EcuM.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/
// RAM block of NVRAM block NvM_NativeBlock_2
/* MR12 RULE 8.5, 8.6, 8.11 DIR 1.1 VIOLATION: If we declared this in NvM_Cfg.h,
   we would effectively re-export this to all NvM users.
   Either use short identifier or configure the compiler to differentiate more than 63 chars. */
extern uint8 Rte_CPim_MyReceivingSWC_PIM_01[];

// RAM block of NVRAM block NvM_NativeBlock_3
/* MR12 RULE 8.5, 8.6, 8.11 DIR 1.1 VIOLATION: If we declared this in NvM_Cfg.h,
   we would effectively re-export this to all NvM users.
   Either use short identifier or configure the compiler to differentiate more than 63 chars. */
extern uint8 Rte_CPim_MyReceivingSWC_PIM_02[];


/*
 **********************************************************************************************************************
 * Assertions
 **********************************************************************************************************************
*/
/* Block length check for Ram block data address of NVRAM block ECUM_CFG_NVM_BLOCK
   Checks the configured block length against the actual data size determined by the included header-file. */
/* MR12 DIR 1.1, RULE 1.1, 8.6, 14.3 VIOLATION: RB compiler assert is using mechanisms which makes the violations necessary. */
COMPILER_RB_ASSERT_GLOBAL(sizeof(EcuM_Rb_dataShutdownInfo_st)==(4u), ECUM_CFG_NVM_BLOCK_CheckRamBlockLength)

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/

#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

// Array containing (persistent Id, block Id) couples sorted by ascendant persistent Ids
const NvM_Prv_PersId_BlockId_tst NvM_Prv_PersId_BlockId_acst[NVM_PRV_NR_PERSISTENT_IDS] =
{
    //{PersId, BlockId}
    {42697u, 3u},        // NvMConf_NvMBlockDescriptor_NvM_NativeBlock_2
    {46912u, 4u},        // NvMConf_NvMBlockDescriptor_NvM_NativeBlock_3
    {62199u, 2u},        // NvMConf_NvMBlockDescriptor_ECUM_CFG_NVM_BLOCK
};

// TRACE[NVM028_Conf]
// Structure containing common configuration options
const NvM_Prv_Common_tst NvM_Prv_Common_cst =
{
    NULL_PTR,
    NULL_PTR,
    NULL_PTR
};

// TRACE[NVM061_Conf]
// Array containing block descriptors
// TRACE[NVM140]
// The block descriptor contents are completely determined by configuration
const NvM_Prv_BlockDescriptor_tst NvM_Prv_BlockDescriptors_acst[NVM_CFG_NR_ALL_BLOCKS] =
{
    {
#if (defined(NVM_RB_BLOCK_VERSION) && (STD_ON == NVM_RB_BLOCK_VERSION))
        0u, // block specific version
#endif
        // Block descriptor of NVRAM block NvM_MultiBlock (NvM block ID: 0):
        0u, // MemIf block ID (this block is not stored on any memory device)
        (const uint16 *) &(NvM_Prv_BlockLengths_acu16[0]), // Block length calculated on compile time
        1u, // block length in bytes stored on the medium
        0u, // Device index (here: not applicable)
        1u, // Number of NV blocks
        0u, // Number of ROM blocks
        &(NvM_Prv_RamBlockAdr_acpv[0]), // RAM block data address calculated on compile time
        NULL_PTR, // ROM block data address
        NULL_PTR, // Single block callback
        NULL_PTR, // Single block start callback
        NULL_PTR, // Initialization callback
        NULL_PTR, // Explicit sync read callback
        NULL_PTR, // Explicit sync write callback
        NVM_BLOCK_NATIVE, // Block management type
        1u, // Job priority (0: Immediate, 1: Standard)
        // Block flags
        0u,
        // Definition of job resource IDs used for different job resource clusters
        {
            // ID of the job resource used for MemIf jobs
            NvM_Prv_idJobResource_MemIf_0_e,
        },
#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
        NvM_Prv_Crc_Type_NoCrc_e, // CRC type used for this block
        0u, // Index of the RAM block CRC in the corresponding array with same CRC type
#endif
#if (NVM_CRYPTO_USED == STD_ON)
        0u, // Persistent ID
        {
            /* MR12 DIR 1.1, 11.6, RULE 11.5 VIOLATION: Casting to byte pointer can always be done safely */
            (uint8 const*)NULL_PTR,                               // Pointer to associated data used for AEAD encryption
            {
                0u,                                               // Csm job ID to encrypt user data
                0u,                                               // Csm job ID to decrypt user data
                0u,                                               // Csm job ID to generate signature
                0u,                                               // Csm job ID to verify signature
                0u,                                               // Csm job ID to generate MAC
                0u,                                               // Csm job ID to verify MAC
                0u,                                               // Csm key ID to be used for encryption / authentication
            },
            {
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of the signature
                0u,                                               // Length of the signature
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the key initialization vector used for data encryption / authentication
            },
            0u,                                                   // Length of associated data used for data authentication
            0u,                                                   // Length of the tag stored for for data authentication
            0u,                                                   // Position of the key initialization vector within internal buffer
        },
#endif
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
        NVM_CFG_NR_SPLIT_BLOCKS,    // this block is not configured to use block splitting
#endif
#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
        FALSE,  // Definition if block is processable in high-speed-mode
#endif
    },
    {
#if (defined(NVM_RB_BLOCK_VERSION) && (STD_ON == NVM_RB_BLOCK_VERSION))
        0u, // block specific version
#endif
        // Block descriptor of NVRAM block NvM_ConfigId (NvM block ID: 1):
        0u, // MemIf block ID (this block is not stored on any memory device)
        (const uint16 *) &(NvM_Prv_BlockLengths_acu16[1]), // Block length calculated on compile time
        2u, // block length in bytes stored on the medium
        0u, // Device index (here: not applicable)
        1u, // Number of NV blocks
        0u, // Number of ROM blocks
        &(NvM_Prv_RamBlockAdr_acpv[1]), // RAM block data address calculated on compile time
        NULL_PTR, // ROM block data address
        NULL_PTR, // Single block callback
        NULL_PTR, // Single block start callback
        NULL_PTR, // Initialization callback
        NULL_PTR, // Explicit sync read callback
        NULL_PTR, // Explicit sync write callback
        NVM_BLOCK_NATIVE, // Block management type
        1u, // Job priority (0: Immediate, 1: Standard)
        // Block flags
        0u,
        // Definition of job resource IDs used for different job resource clusters
        {
            // ID of the job resource used for MemIf jobs
            NvM_Prv_idJobResource_MemIf_0_e,
        },
#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
        NvM_Prv_Crc_Type_NoCrc_e, // CRC type used for this block
        0u, // Index of the RAM block CRC in the corresponding array with same CRC type
#endif
#if (NVM_CRYPTO_USED == STD_ON)
        0u, // Persistent ID
        {
            /* MR12 DIR 1.1, 11.6, RULE 11.5 VIOLATION: Casting to byte pointer can always be done safely */
            (uint8 const*)NULL_PTR,                               // Pointer to associated data used for AEAD encryption
            {
                0u,                                               // Csm job ID to encrypt user data
                0u,                                               // Csm job ID to decrypt user data
                0u,                                               // Csm job ID to generate signature
                0u,                                               // Csm job ID to verify signature
                0u,                                               // Csm job ID to generate MAC
                0u,                                               // Csm job ID to verify MAC
                0u,                                               // Csm key ID to be used for encryption / authentication
            },
            {
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of the signature
                0u,                                               // Length of the signature
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the key initialization vector used for data encryption / authentication
            },
            0u,                                                   // Length of associated data used for data authentication
            0u,                                                   // Length of the tag stored for for data authentication
            0u,                                                   // Position of the key initialization vector within internal buffer
        },
#endif
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
        NVM_CFG_NR_SPLIT_BLOCKS,    // this block is not configured to use block splitting
#endif
#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
        FALSE,  // Definition if block is processable in high-speed-mode
#endif
    },
    {
#if (defined(NVM_RB_BLOCK_VERSION) && (STD_ON == NVM_RB_BLOCK_VERSION))
        0u, // block specific version
#endif
        // Block descriptor of NVRAM block ECUM_CFG_NVM_BLOCK (NvM block ID: 2, persistent ID: 62199):
        FeeConf_FeeBlockConfiguration_ECUM_CFG_NVM_BLOCK, // MemIf block ID
        (const uint16 *) &(NvM_Prv_BlockLengths_acu16[2]), // Block length calculated on compile time
        4u, // block length in bytes stored on the medium
        0u, // Device index (here: Fee)
        1u, // Number of NV blocks
        0u, // Number of ROM blocks
        &(NvM_Prv_RamBlockAdr_acpv[2]), // RAM block data address calculated on compile time
        NULL_PTR, // ROM block data address
        &EcuM_Rb_NvMSingleBlockCallbackFunction, // Single block callback
        NULL_PTR, // Single block start callback
        NULL_PTR, // Initialization callback
        NULL_PTR, // Explicit sync read callback
        NULL_PTR, // Explicit sync write callback
        NVM_BLOCK_NATIVE, // Block management type
        1u, // Job priority (0: Immediate, 1: Standard)
        // Block flags
        (uint32)NVM_PRV_BLOCK_FLAG_SELECT_FOR_READ_ALL |
        (uint32)NVM_PRV_BLOCK_FLAG_SELECT_FOR_WRITE_ALL,
        // Definition of job resource IDs used for different job resource clusters
        {
            // ID of the job resource used for MemIf jobs
            NvM_Prv_idJobResource_MemIf_0_e,
        },
#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
        NvM_Prv_Crc_Type_NoCrc_e, // CRC type used for this block
        0u, // Index of the RAM block CRC in the corresponding array with same CRC type
#endif
#if (NVM_CRYPTO_USED == STD_ON)
        62199u, // Persistent ID
        {
            /* MR12 DIR 1.1, 11.6, RULE 11.5 VIOLATION: Casting to byte pointer can always be done safely */
            (uint8 const*)NULL_PTR,                               // Pointer to associated data used for AEAD encryption
            {
                0u,                                               // Csm job ID to encrypt user data
                0u,                                               // Csm job ID to decrypt user data
                0u,                                               // Csm job ID to generate signature
                0u,                                               // Csm job ID to verify signature
                0u,                                               // Csm job ID to generate MAC
                0u,                                               // Csm job ID to verify MAC
                0u,                                               // Csm key ID to be used for encryption / authentication
            },
            {
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of the signature
                0u,                                               // Length of the signature
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the key initialization vector used for data encryption / authentication
            },
            0u,                                                   // Length of associated data used for data authentication
            0u,                                                   // Length of the tag stored for for data authentication
            0u,                                                   // Position of the key initialization vector within internal buffer
        },
#endif
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
        NVM_CFG_NR_SPLIT_BLOCKS,    // this block is not configured to use block splitting
#endif
#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
        FALSE,  // Definition if block is processable in high-speed-mode
#endif
    },
    {
#if (defined(NVM_RB_BLOCK_VERSION) && (STD_ON == NVM_RB_BLOCK_VERSION))
        0u, // block specific version
#endif
        // Block descriptor of NVRAM block NvM_NativeBlock_2 (NvM block ID: 3, persistent ID: 42697):
        FeeConf_FeeBlockConfiguration_NvM_NativeBlock_2, // MemIf block ID
        (const uint16 *) &(NvM_Prv_BlockLengths_acu16[3]), // Block length calculated on compile time
        2u, // block length in bytes stored on the medium
        0u, // Device index (here: Fee)
        1u, // Number of NV blocks
        0u, // Number of ROM blocks
        &(NvM_Prv_RamBlockAdr_acpv[3]), // RAM block data address calculated on compile time
        NULL_PTR, // ROM block data address
        NULL_PTR, // Single block callback
        NULL_PTR, // Single block start callback
        NULL_PTR, // Initialization callback
        NULL_PTR, // Explicit sync read callback
        NULL_PTR, // Explicit sync write callback
        NVM_BLOCK_NATIVE, // Block management type
        1u, // Job priority (0: Immediate, 1: Standard)
        // Block flags
        (uint32)NVM_PRV_BLOCK_FLAG_SELECT_FOR_READ_ALL |
        (uint32)NVM_PRV_BLOCK_FLAG_SELECT_FOR_WRITE_ALL,
        // Definition of job resource IDs used for different job resource clusters
        {
            // ID of the job resource used for MemIf jobs
            NvM_Prv_idJobResource_MemIf_0_e,
        },
#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
        NvM_Prv_Crc_Type_NoCrc_e, // CRC type used for this block
        0u, // Index of the RAM block CRC in the corresponding array with same CRC type
#endif
#if (NVM_CRYPTO_USED == STD_ON)
        42697u, // Persistent ID
        {
            /* MR12 DIR 1.1, 11.6, RULE 11.5 VIOLATION: Casting to byte pointer can always be done safely */
            (uint8 const*)NULL_PTR,                               // Pointer to associated data used for AEAD encryption
            {
                0u,                                               // Csm job ID to encrypt user data
                0u,                                               // Csm job ID to decrypt user data
                0u,                                               // Csm job ID to generate signature
                0u,                                               // Csm job ID to verify signature
                0u,                                               // Csm job ID to generate MAC
                0u,                                               // Csm job ID to verify MAC
                0u,                                               // Csm key ID to be used for encryption / authentication
            },
            {
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of the signature
                0u,                                               // Length of the signature
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the key initialization vector used for data encryption / authentication
            },
            0u,                                                   // Length of associated data used for data authentication
            0u,                                                   // Length of the tag stored for for data authentication
            0u,                                                   // Position of the key initialization vector within internal buffer
        },
#endif
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
        NVM_CFG_NR_SPLIT_BLOCKS,    // this block is not configured to use block splitting
#endif
#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
        FALSE,  // Definition if block is processable in high-speed-mode
#endif
    },
    {
#if (defined(NVM_RB_BLOCK_VERSION) && (STD_ON == NVM_RB_BLOCK_VERSION))
        0u, // block specific version
#endif
        // Block descriptor of NVRAM block NvM_NativeBlock_3 (NvM block ID: 4, persistent ID: 46912):
        FeeConf_FeeBlockConfiguration_NvM_NativeBlock_3, // MemIf block ID
        (const uint16 *) &(NvM_Prv_BlockLengths_acu16[4]), // Block length calculated on compile time
        2u, // block length in bytes stored on the medium
        0u, // Device index (here: Fee)
        1u, // Number of NV blocks
        0u, // Number of ROM blocks
        &(NvM_Prv_RamBlockAdr_acpv[4]), // RAM block data address calculated on compile time
        NULL_PTR, // ROM block data address
        NULL_PTR, // Single block callback
        NULL_PTR, // Single block start callback
        NULL_PTR, // Initialization callback
        NULL_PTR, // Explicit sync read callback
        NULL_PTR, // Explicit sync write callback
        NVM_BLOCK_NATIVE, // Block management type
        1u, // Job priority (0: Immediate, 1: Standard)
        // Block flags
        (uint32)NVM_PRV_BLOCK_FLAG_SELECT_FOR_READ_ALL |
        (uint32)NVM_PRV_BLOCK_FLAG_SELECT_FOR_WRITE_ALL,
        // Definition of job resource IDs used for different job resource clusters
        {
            // ID of the job resource used for MemIf jobs
            NvM_Prv_idJobResource_MemIf_0_e,
        },
#if (defined(NVM_CALC_CRC) && (NVM_CALC_CRC == STD_ON))
        NvM_Prv_Crc_Type_NoCrc_e, // CRC type used for this block
        0u, // Index of the RAM block CRC in the corresponding array with same CRC type
#endif
#if (NVM_CRYPTO_USED == STD_ON)
        46912u, // Persistent ID
        {
            /* MR12 DIR 1.1, 11.6, RULE 11.5 VIOLATION: Casting to byte pointer can always be done safely */
            (uint8 const*)NULL_PTR,                               // Pointer to associated data used for AEAD encryption
            {
                0u,                                               // Csm job ID to encrypt user data
                0u,                                               // Csm job ID to decrypt user data
                0u,                                               // Csm job ID to generate signature
                0u,                                               // Csm job ID to verify signature
                0u,                                               // Csm job ID to generate MAC
                0u,                                               // Csm job ID to verify MAC
                0u,                                               // Csm key ID to be used for encryption / authentication
            },
            {
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of encrypted user data
                0u,                                               // Length of the signature
                0u,                                               // Length of the signature
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the MAC in bytes
                0u,                                               // Length of the key initialization vector used for data encryption / authentication
            },
            0u,                                                   // Length of associated data used for data authentication
            0u,                                                   // Length of the tag stored for for data authentication
            0u,                                                   // Position of the key initialization vector within internal buffer
        },
#endif
#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
        NVM_CFG_NR_SPLIT_BLOCKS,    // this block is not configured to use block splitting
#endif
#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
        FALSE,  // Definition if block is processable in high-speed-mode
#endif
    },
};

#if (defined(NVM_CFG_CRC_NR_RAM_BLOCKS) && (NVM_CFG_CRC_NR_RAM_BLOCKS > 0u))
NvM_BlockIdType const NvM_Prv_idBlockRamCrc_cauo[NVM_CFG_CRC_NR_RAM_BLOCKS] =
{
};
#endif


// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3028]
// Runtime Calculation feature disabled
#if (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_OFF)
// Used to calculate the NV block lengths on compile time
// This variable is mapped into the block descriptor NvM_Prv_BlockDescriptors_acst
const uint16 NvM_Prv_BlockLengths_acu16[NVM_CFG_NR_ALL_BLOCKS] =
{
    // Block length of NVRAM block NvM_MultiBlock (NvM block ID: 0)
    1u,
    // Block length of NVRAM block NvM_ConfigId (NvM block ID: 1)
    2u,
    // Block length of NVRAM block ECUM_CFG_NVM_BLOCK (NvM block ID: 2)
    4u,
    // Block length of NVRAM block NvM_NativeBlock_2 (NvM block ID: 3)
    2u,
    // Block length of NVRAM block NvM_NativeBlock_3 (NvM block ID: 4)
    2u,
};
// Used to calculate the RAM block data addresses on compile runtime
// This variable is mapped into the block descriptor NvM_Prv_BlockDescriptors_acst
void * const NvM_Prv_RamBlockAdr_acpv[NVM_CFG_NR_ALL_BLOCKS] =
{
    // Permanent RAM address of NVRAM block NvM_MultiBlock (NvM block ID: 0)
    /* MR12 DIR 1.1, 11.6 VIOLATION: Casting to void pointer can always be done safely */
    (void *)NULL_PTR,
    // Permanent RAM address of NVRAM block NvM_ConfigId (NvM block ID: 1)
    /* MR12 DIR 1.1, 11.6 VIOLATION: Casting to void pointer can always be done safely */
    (void *)NULL_PTR,
    // Permanent RAM address of NVRAM block ECUM_CFG_NVM_BLOCK (NvM block ID: 2)
    /* MR12 DIR 1.1, 11.6 VIOLATION: Casting to void pointer can always be done safely */
    (void *)&(EcuM_Rb_dataShutdownInfo_st),
    // Permanent RAM address of NVRAM block NvM_NativeBlock_2 (NvM block ID: 3)
    /* MR12 DIR 1.1, 11.6 VIOLATION: Casting to void pointer can always be done safely */
    (void *)&(Rte_CPim_MyReceivingSWC_PIM_01),
    // Permanent RAM address of NVRAM block NvM_NativeBlock_3 (NvM block ID: 4)
    /* MR12 DIR 1.1, 11.6 VIOLATION: Casting to void pointer can always be done safely */
    (void *)&(Rte_CPim_MyReceivingSWC_PIM_02),
};
#endif

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"
/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3028]
// Runtime Calculation feature enabled
#if (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON)
# define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
# include "NvM_MemMap.h"
// Used to calculate the NV block lengths and RAM block data addresses on runtime
// These variables are mapped into the block descriptor NvM_Prv_BlockDescriptors_acst
uint16 NvM_Prv_BlockLengths_au16[NVM_CFG_NR_ALL_BLOCKS];
void *NvM_Prv_RamBlockAdr_apv[NVM_CFG_NR_ALL_BLOCKS];
# define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
# include "NvM_MemMap.h"
#endif

/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/
# define NVM_START_SEC_CODE
# include "NvM_MemMap.h"
/********************************************************************************************
 * Initialization of NV block length and RAM block data address (permanent RAM address)     *
 *                                                                                          *
 * In this case NvMRbRuntimeRamBlockConfiguration is enabled                                *
 * + NV block length is defined either by NvMRbNvBlockLengthString or NvMNvBlockLength      *
 * + RAM block data address is still defined by NvMRamBlockDataAddress but now              *
 *   NvMRamBlockDataAddress can also contain C expressions                                  *
 *                                                                                          *
 * Furthermore if explicit sync feature is enabled the explicit sync buffer is defined here *
 * by setting the start address and calculating the buffer size                             *
 * Start address and end address is defined by user in common options with the parameters   *
 * + NvMRbRuntimeRamBufferAddressStart                                                      *
 * + NvMRbRuntimeRamBufferAddressEnd                                                        *
 *                                                                                          *
 * ******************************************************************************************
*/
/* HIS METRIC STMT VIOLATION IN NvM_Prv_InitRamBlockProperties: Due to the configuration this generated function may be empty */
void NvM_Prv_InitRamBlockProperties(void)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3028]
    // Runtime Calculation feature enabled
#if (NVM_PRV_RUNTIME_RAM_BLOCK_CONFIG == STD_ON)
# if (NVM_PRV_EXPLICIT_SYNC == STD_ON)
    // Calculate explicit synchronization RAM buffer size
    /* MR12 RULE 11.4 VIOLATION: Cast to an integral type is necessary to calculate the size of the object*/
    uint32 RuntimeRamMirrorSize_u32 = (uint32)(0) - (uint32)(0);

    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3029] Calculate explicit synchronization RAM buffer
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3030] Calculate explicit synchronization RAM buffer
    // Set explicit synchronization RAM buffer start address and its size
    NvM_Prv_InitRuntimeRamMirror((uint8 *)(0),
                                 RuntimeRamMirrorSize_u32);
# endif

#endif
}
# define NVM_STOP_SEC_CODE
# include "NvM_MemMap.h"
