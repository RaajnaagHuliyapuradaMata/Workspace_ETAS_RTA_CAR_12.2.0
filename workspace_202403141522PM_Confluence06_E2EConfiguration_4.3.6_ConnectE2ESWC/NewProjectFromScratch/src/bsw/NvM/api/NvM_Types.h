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
// TRACE[NVM550] Header file specifying all public NvM types
#ifndef NVM_TYPES_H
#define NVM_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
// TRACE[SWS_NvM_00553] Make pre-compile time configuration accessible via public header
#include "NvM_Cfg.h"

// This header file controls centrally all definitions and declarations which will be provided by RTE generator,
// for details please see coding guidelines Rule BSW_ServiceRTE_002.
#include "rba_MemLib_UseRte.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */


/* Rte is always generating typedefs even if no port is configured for Rte usage
 * Therefore following check DOES NOT consider NVM_RB_RTE_USED */
#if (defined(RBA_MEMLIB_USE_RTE))

#include "Rte_NvM_Type.h"

#else
// Following type definitions will be used only of RTE generator is not used
/**
 * Definition of the implementation data type for the pointer to a void used in public NvM APIs.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 */
typedef void* NvM_Rb_VoidPtr;

/**
 * Definition of the implementation data type for the pointer to a constant void used in public NvM APIs.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 */
typedef const void* NvM_Rb_ConstVoidPtr;

/**
 * Definition of the implementation data type for block ID used in public NvM APIs.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 * TRACE[SWS_NvM_00471]
 */
typedef uint16 NvM_BlockIdType;

/**
 * Definition of the implementation data type for an array with reserved bytes used in the structure
 * to store write request counters in persistant way.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3127]
 */
typedef uint8 NvM_Rb_WriteMon_Reserved_au8[22];

/**
 * Definition of the implementation data type for a structure to store write request counters in persistant way.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 * TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3127]
 */
typedef struct
{
    /// Maximum number of bytes written since last power-on
    uint32 nrBytesWrittenMax_u32;
    /// Accumulated number of all write requests
    uint32 cntrWritesAbs_u32;
    /// Maximum number of write requests since last power-on
    uint16 cntrWritesMax_u16;

    /// Reserved bytes
    NvM_Rb_WriteMon_Reserved_au8 Reserved_au8;

} NvM_Rb_WriteMon_tst;

/**
 * Definition of the implementation data type for possible results of asynchronous requests used in public NvM APIs.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 * TRACE[SWS_NvM_00083]
 * TRACE[SWS_NvM_00470]
 */
 // If the NvM_RequestResultType is changed here, keep the measurement definition of it(NvM_Prv_stRequestResult_au8) in NvMArxmlGen.java in sync
typedef uint8 NvM_RequestResultType;

/**
 * Definition of the implementation data type for callback functions used in public NvM APIs.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 * TRACE[SWS_NvM_91002]
 */
typedef uint8 NvM_BlockRequestType;

/**
 * Definition of the implementation data type for init callback functions used in public NvM APIs.
 * If RTE generator is used then this type definition is located in Rte_Type.h
 * TRACE[SWS_NvM_91123]
 */
typedef uint8 NvM_InitBlockRequestType;


#endif

/***************************************************************************************************************/
/* Rte is always generating defines only if a port is configured for Rte usage
 * Therefore following check DOES consider NVM_RB_RTE_USED */
#if (defined(RBA_MEMLIB_USE_RTE) && defined(NVM_RB_RTE_USED) && (NVM_RB_RTE_USED == STD_ON))

/* Rte_NvM_Type.h is already included abvove */

#else
/*****************  NvM_RequestResultType - TRACE[SWS_NvM_00470] *****************/
/**
 * Last asynchronous request has been finished successfully.
 * This is the default value after reset.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_OK
# define NVM_REQ_OK                 (0u)
# endif
/**
 * Last asynchronous request has been finished unsuccessfully.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_NOT_OK
# define NVM_REQ_NOT_OK             (1u)
# endif
/**
 * Last asynchronous request is currently pending.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_PENDING
# define NVM_REQ_PENDING            (2u)
# endif
/**
 * Result of the last asynchronous read request is a data integrity failure.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_INTEGRITY_FAILED
# define NVM_REQ_INTEGRITY_FAILED   (3u)
# endif
/**
 * The referenced block was skipped during execution of NvM_ReadAll or NvM_WriteAll.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_BLOCK_SKIPPED
# define NVM_REQ_BLOCK_SKIPPED      (4u)
# endif
/**
 * The referenced NV block is invalidated.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_NV_INVALIDATED
# define NVM_REQ_NV_INVALIDATED     (5u)
# endif
/**
 * The multi block request NvM_WriteAll was canceled by calling NvM_CancelWriteAll.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_CANCELED
# define NVM_REQ_CANCELED           (6u)
# endif
/**
 * The required redundancy of the referenced NV block is lost (obsolete).
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_REDUNDANCY_FAILED
# define NVM_REQ_REDUNDANCY_FAILED  (7u)
# endif
/**
 * The referenced NV block had the default values copied to the RAM image.
 * If RTE generator is used then this type definition is located in Rte_NvM_Type.h
 * TRACE[SWS_NvM_00470]
 */
# ifndef NVM_REQ_RESTORED_FROM_ROM
# define NVM_REQ_RESTORED_FROM_ROM  (8u)
# endif

#endif // (defined(RBA_MEMLIB_USE_RTE) && defined(NVM_RB_RTE_USED) && (NVM_RB_RTE_USED == STD_ON))

#if (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
/*****************  NvM_InitBlockRequestType - TRACE[SWS_NvM_91123] *****************/
# ifndef NVM_INIT_READ_BLOCK
#define NVM_INIT_READ_BLOCK                0u      // NvM_ReadBlock/ NvM_ReadPRAMBlock is requested on the block
# endif
# ifndef NVM_INIT_RESTORE_BLOCK_DEFAULTS
#define NVM_INIT_RESTORE_BLOCK_DEFAULTS    1u      // NvM_RestoreBlockDefaults/ NvM_RestorePRAMBlockDefaults is requested on the block
# endif
# ifndef NVM_INIT_READ_ALL_BLOCK
#define NVM_INIT_READ_ALL_BLOCK            2u      // NvM_ReadAll is processing this block
# endif
# ifndef NVM_INIT_FIRST_INIT_ALL
#define NVM_INIT_FIRST_INIT_ALL            3u      // NvM_FirstInitAll is processing this block
# endif
# ifndef NVM_RB_NO_INIT_CALLBACK
#define NVM_RB_NO_INIT_CALLBACK            255u    // ServiceID has no InitBlockRequestType value in AR Spec
# endif

/*****************  NvM_BlockRequestType - TRACE[SWS_NvM_91002]*****************/
# ifndef NVM_READ_BLOCK
#define NVM_READ_BLOCK                 0u   // NvM_ReadBlock/ NvM_ReadPRAMBlock was performed on the block
# endif
# ifndef NVM_WRITE_BLOCK
#define NVM_WRITE_BLOCK                1u   // NvM_WriteBlock/ NvM_WritePRAMBlock was performed on the block
# endif
# ifndef NVM_RESTORE_BLOCK_DEFAULTS
#define NVM_RESTORE_BLOCK_DEFAULTS     2u   // NvM_RestoreBlockDefaults/ NvM_RestorePRAMBlockDefaults was performed on the block
# endif
# ifndef NVM_ERASE_NV_BLOCK
#define NVM_ERASE_NV_BLOCK             3u   // NvM_EraseNvBlock was performed on the block
# endif
# ifndef NVM_INVALIDATE_NV_BLOCK
#define NVM_INVALIDATE_NV_BLOCK        4u   // NvM_InvalidateNvBlock was performed on the block
# endif
# ifndef NVM_READ_ALL_BLOCK
#define NVM_READ_ALL_BLOCK             5u   // NvM_ReadAll has finished processing this block
# endif
# ifndef NVM_RB_WRITE_ALL_BLOCK
#define NVM_RB_WRITE_ALL_BLOCK         200u // NvM_WriteAll has finished processing this block
# endif
# ifndef NVM_RB_NO_BLOCK_CALLBACK
#define NVM_RB_NO_BLOCK_CALLBACK       255u // ServiceID has no BlockRequestType value in AR Spec
# endif

/**
 * Autosar Definition of enumeration for the multi block request type.
 * Identifies the type of request performed on multi block when signaled via the
 * callback function or when reporting to BswM
 */
// TRACE[SWS_NvM_91003] Enumeration for the multi block request type
typedef enum
{
    NVM_READ_ALL     = 0,       // NvM_ReadAll was performed
    NVM_WRITE_ALL,              // NvM_WriteAll was performed
    NVM_VALIDATE_ALL,           // NvM_ValidateAll was performed
    NVM_FIRST_INIT_ALL,         // NvM_FirstInitAll was performed
    NVM_CANCEL_WRITE_ALL,       // NvM_CancelWriteAll was performed
    NVM_RB_NO_ALL_CALLBACK      // ServiceID has no MultiBlockRequestType value in AR Spec
} NvM_MultiBlockRequestType;

    typedef NvM_BlockRequestType               NvM_Prv_SnglCallbackReq_tuo;        // contains NvM_BlockRequestType ID
    typedef NvM_MultiBlockRequestType          NvM_Prv_MultiCallbackReq_tuo;   // contains NvM_MultiBlockRequestType ID
    typedef NvM_InitBlockRequestType           NvM_Prv_InitCallbackReq_tuo;    // contains NvM_InitBlockRequestType ID
    #define NVM_RB_INIT_CALLBACK_PARAM         InitBlockRequest
    #define NVM_RB_INIT_CALLBACK_TYP_PARAM     NvM_Prv_InitCallbackReq_tuo NVM_RB_INIT_CALLBACK_PARAM
    #define NVM_RB_INIT_CALLBACK_PARAM_UNUSED  (void)InitBlockRequest
#else // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
    typedef uint8                              NvM_Prv_SnglCallbackReq_tuo;  // contains ServiceID
    typedef uint8                              NvM_Prv_MultiCallbackReq_tuo; // contains ServiceID
    #define NVM_RB_INIT_CALLBACK_PARAM
    #define NVM_RB_INIT_CALLBACK_TYP_PARAM     void NVM_RB_INIT_CALLBACK_PARAM
    #define NVM_RB_INIT_CALLBACK_PARAM_UNUSED
#endif // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_ON)
/********************************************************************************************************************/
/* From here only definitions are included which are not generated by the Rte */


// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3050] Enumeration for NvM migration status information
typedef enum
{
    NVM_RB_MIGRATION_RESULT_INIT_E              = 0,    // Initialization value, result not yet available
    NVM_RB_MIGRATION_RESULT_NOT_NECESSARY_E     = 1,    // RAM size == size on medium (migration not necessary)
    NVM_RB_MIGRATION_RESULT_TO_SMALLER_SIZE_E   = 2,    // RAM size < size on medium (remaining data discarded)
    NVM_RB_MIGRATION_RESULT_TO_BIGGER_SIZE_E    = 3,    // RAM size > size on medium (remaining data filled with zero)
    NVM_RB_MIGRATION_RESULT_NOT_DONE_E          = 4,    // Migration not done, because data were not read
    NVM_RB_MIGRATION_RESULT_DEACTIVATED_E       = 5     // Migrations is deactivated by configuration
}NvM_Rb_MigrationResult_ten;

// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3403] Enumeration for NvM detailed error information
typedef enum
{
    NvM_Rb_ErrorInfoDetail_NotAvailable_e           = 0, // No more details available
    NvM_Rb_ErrorInfoDetail_BlockNotFoundOnMedium_e  = 1, // Block was not found on the medium
    NvM_Rb_ErrorInfoDetail_InconsistentData_e       = 2, // Block was found, but data is inconsistent to checksum
    NvM_Rb_ErrorInfoDetail_VersionMismatch_e        = 3, // Version counter of the block does not match
    NvM_Rb_ErrorInfoDetail_CryptoFailed_e           = 4, // Error happened during cryptographic operations
    NvM_Rb_ErrorInfoDetail_BlockSplitFailed_e       = 5  // Error happened during handling of split block
} NvM_Rb_ErrorInfoDetail_ten;

// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2751] Enumeration for general NvM status information
typedef enum
{
    NVM_RB_STATUS_UNINIT = 0,   // NvM is not initialized yet
    NVM_RB_STATUS_IDLE = 1,     // NvM is currently idle
    NVM_RB_STATUS_BUSY = 2      // NvM is currently busy
} NvM_Rb_StatusType;

// TRACE[NVM137] Enumeration of block management types (public because the enum literal names are defined by AUTOSAR)
typedef enum
{
    NVM_BLOCK_NATIVE,     // TRACE[NVM557] Block is a standard block (no redundant storage, no dataset features)
    NVM_BLOCK_REDUNDANT,  // TRACE[NVM558] Like a native block, but with redundant storage on the memory media
    NVM_BLOCK_DATASET     // TRACE[NVM559] Allows several NV blocks and ROM blocks for one RAM block
} NvM_BlockManagementType;

// Enumeration of NvM activities
// TRACE[NVM507] Private type added to public NvM_Types.h to allow AUTOSAR debugging
typedef enum
{
    NVM_PRV_ACTIVITY_NOT_INIT = 0,  // NvM not initialized, job processing prohibited
    NVM_PRV_ACTIVITY_IDLE,          // Arbitrate incoming requests (last arbitration did not find any request)
    NVM_PRV_ACTIVITY_BUSY,          // NvM process user requests
    NVM_PRV_ACTIVITY_RAM_BLOCK_CRC  // Check PRAM block state and recalculate CRC if required
} NvM_Prv_Activities_ten;

typedef uint8 NvM_Rb_BlockErrors_tuo;

/**
 * This macro generates lists where each entry corresponds to a CRC type provided by NvM.
 *
 * Each entry of the macro shall look like follows:
 *
 * ENTRY(Type)
 *
 * with:
 * Type     CRC type for which the entry is generated
 */
#define LIST_CRC_TYPE(ENTRY)    \
        ENTRY(NoCrc)            \
        ENTRY(8_Bit)            \
        ENTRY(16_Bit)           \
        ENTRY(32_Bit)           \

#define LIST_ENTRY_CRC_TYPE(Type)     NvM_Prv_Crc_Type_##Type##_e,

/**
 * \brief
 * Definition of different CRC types provided by NvM.
 */
typedef enum
{
    /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
    LIST_CRC_TYPE(LIST_ENTRY_CRC_TYPE)

    //-------------------------------------------------------------------------
    // This is actually no CRC type,
    // in fact this enum value defines count of CRC types available in NvM and
    // can be used for array definition
    //-------------------------------------------------------------------------
    NvM_Prv_Crc_Type_Count_e

} NvM_Prv_Crc_Type_ten;

/**
 * This macro generates lists where each entry corresponds to a service bit provided by NvM.
 *
 * Each entry of the macro shall look like follows:
 *
 * ENTRY(ServiceBit, JobName)
 *
 * with:
 * ServiceBit   Service bit for which the entry is generated
 * JobName      Name of the job for which the entry is generated
 */
#define LIST_SERVICE_BIT(ENTRY)                                            \
        ENTRY(ReadAll,                  ReadRecovery                   )   \
        ENTRY(RemoveNonResistant,       InvalidateForRemoveNonResistant)   \
        ENTRY(WriteAll,                 WriteAll                       )   \
        ENTRY(FirstInitAll,             Idle                           )   \
        ENTRY(Maintain,                 Maintain                       )   \
        ENTRY(InitAtLayoutChange,       Write                          )   \
        ENTRY(ValidateAll,              Validate                       )   \
        ENTRY(ReadAllExtPreparation,    ReadAllExtPreparation          )   \
        ENTRY(Read,                     ReadRecovery                   )   \
        ENTRY(Write,                    Write                          )   \
        ENTRY(Invalidate,               Invalidate                     )   \
        ENTRY(Erase,                    Erase                          )   \
        ENTRY(Restore,                  Restore                        )   \
        ENTRY(NotUsed_1,                Invalid                        )   \
        ENTRY(NotUsed_2,                Invalid                        )   \
        ENTRY(NotUsed_3,                Invalid                        )   \
        ENTRY(Unspecified,              Invalid                        )

#define LIST_ENTRY_SERVICE_BIT(ServiceBit, Unused)      NvM_Prv_ServiceBit_##ServiceBit##_e,

/**
 * \brief
 * Definition of different bits for all asynchronous services provided by NvM.
 * \details
 * These bits are used to define corresponding masks for service flags in the administrative blocks.
 * A set bit in the administrative block signals that the corresponding service has been requested.
 *
 * Definition of bit masks as anonymous enumeration instead of several prepocessor defines is done
 * to get better structuring to show that all these bit masks belongs together.
 */
enum
{
    /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
    LIST_SERVICE_BIT(LIST_ENTRY_SERVICE_BIT)

    NvM_Prv_ServiceBit_nr_e
};

/// Type definition of different bits for all services provided by NvM.
// TRACE[NVM507] Private type added to public NvM_Types.h to allow AUTOSAR debugging
typedef uint16 NvM_Prv_ServiceBit_tuo;

/// Type definition for services provided by NvM.
/// NvM defines individual service IDs within NvM.h since these IDs are part of the public interface.
// TRACE[NVM507] Private type added to public NvM_Types.h to allow AUTOSAR debugging
typedef uint8 NvM_Prv_idService_tuo;

/// Type definition for DET errors which can be reported by NvM.
/// NvM defines individual DET error IDs within NvM.h since these IDs are part of the public interface.
typedef uint8 NvM_Prv_idDetError_tuo;

/**
 * \brief
 * Definition of different queue IDs used by NvM.
 * \details
 * The order of IDs defines also the priority of the corresponding queue in ascending order .
 */
enum
{
    /// ID of the queue for multi-block requests
    NvM_Prv_idQueue_Multi_e,

    /// ID of the queue for single block requests with standard priority
    NvM_Prv_idQueue_Standard_e,

#if (NVM_RB_AUX_INTERFACE == STD_ON)
    /// ID of the queue for auxiliary single block requests
    NvM_Prv_idQueue_Aux_e,
#endif

// TRACE[NVM564] NvMJobPrioritization enables/disables immediate job processing
#if (NVM_JOB_PRIORITIZATION == STD_ON)
    /// ID of the queue for suspended jobs
    NvM_Prv_idQueue_Suspended_e,

    /// ID of the queue for single block requests with immediate priority
    NvM_Prv_idQueue_Immediate_e,
#endif

    //-------------------------------------------------------------------------
    // Add IDs of new queues before this line
    /// Number of queues available in NvM.
    /// Additionally this value can be used as special ID to avoid enqueuing a request into a queue or
    /// as invalid queue ID.
    NvM_Prv_idQueue_nrQueues_e
};

/// Type definition for queues used by NvM.
typedef uint8 NvM_Prv_idQueue_tuo;

/// Type definition for indexes used in the queues by NvM.
/// The NvM uses this type also for counters and sizes in a queue.
typedef uint8 NvM_Prv_Queue_idx_tuo;

/// Definition of the value for an invalid value for indexes used in the queues by NvM.
#define NVM_PRV_QUEUE_IDX_INVALID   (0xFFu)

/// Type definition of the structure for indexes used for queue handling by NvM.
/// Main purpose of this structure is to simplify dequeuing of an element in a linked queue if used.
typedef struct
{
    /// Index of a element in the queue.
    NvM_Prv_Queue_idx_tuo idx_uo;
    /// Index of the previos element in the queue.
    NvM_Prv_Queue_idx_tuo idxPrev_uo;
} NvM_Prv_Queue_idx_tst;

/// Type definition for largest CRC used by NvM.
// TRACE[SWS_NvM_00544] Define data type to allocate memory for largest CRC used by NvM.
typedef union
{
    uint8   Crc8_u8;
    uint16  Crc16_u16;
    uint32  Crc32_u32;

} NvM_Prv_Crc_tun;

typedef union
{
    void*   ptrRamBlock_pv;
    uint8*  ptrRamBlock_pu8;

} NvM_Prv_ptrRamBlock_tun;

typedef union
{
    void const*   ptrRomBlock_pcv;
    uint8 const*  ptrRomBlock_pcu8;

} NvM_Prv_ptrRomBlock_tun;

/// Structure for holding one queue entry.
/// This structure is used for all services (single-block and multi-block).
// TRACE[NVM507] Private type added to public NvM_Types.h to allow AUTOSAR debugging
typedef struct
{
    /// ID of the requested block service
    NvM_Prv_idService_tuo idService_uo;

    /// ID of the queue where this request has been enqueued originally
    /// (Required to get the original queue in case of request suspension)
    NvM_Prv_idQueue_tuo idQueueOriginal_uo;

    /// ID of the block for which a service is requested
    NvM_BlockIdType idBlock_uo;

    /// Service bit for the requested block service
    NvM_Prv_ServiceBit_tuo ServiceBit_uo;

    /// Pointer to the buffer holding the block data
    // TRACE[NVM088] No assumptions can be made about alignment, hence a void pointer
    NvM_Prv_ptrRamBlock_tun BlockData_un;

} NvM_Prv_QueueEntry_tst;

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
typedef uint8 NvM_Prv_InitVectorCryptoKey_tauo[NVM_PRV_LENGTH_RANDOM_NR];
#endif

// Structure for holding all state information which is not block-specific
// (to have these variables gathered in one struct both optimizes resources and eases debugging)
// TRACE[NVM507] Private type added to public NvM_Types.h to allow AUTOSAR debugging
// If the NvM_Prv_MainStates_tst is changed here, keep the measurement definition of it in NvMArxmlGen.java in sync
typedef struct
{
    // General NvM state
    // TRACE[NVM399] Flag indicating whether the NvM already has been initialized
    // TRACE[NVM510] This flag is available for debugging
    NvM_Prv_Activities_ten  Activity_rAMwM_en;
    // ID of the queue with currently ongoing request (NvM_Prv_idQueue_nrQueues_e if no request is currently ongoing)
    NvM_Prv_idQueue_tuo idQueueActive_uo;
    /// ID of the currently active block service
    NvM_Prv_idService_tuo idServiceActive_uo;
    /// Information whether both ReadAll and InitAfterReadAll are completely done.
    boolean isReadAllDone_b;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    /// Information whether high speed mode is required
    boolean isHighSpeedModeRequired_b;
    /// Information whether high speed mode is active
    boolean isHighSpeedModeActive_b;
#endif

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
    /// Status of the initialization vector to be used for next encryption of user data
    boolean isInitVectorAvailable_b;
    /// Initialization vector to be used for next encryption of user data
    NvM_Prv_InitVectorCryptoKey_tauo InitVectorCryptoKey_auo;
#endif

} NvM_Prv_MainStates_tst;

// Type for array to hold the pointer to the saved zone marker
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3438] Savedzone marker for NvM ramblock init
#if (NVM_RB_RAM_INIT_CHECK == NVM_RB_RAM_INIT_CHECK_QUICK)
typedef uint32* const NvM_Prv_SavedZoneMarker_tcpu32;
#endif

/* NVM_TYPES_H */
#endif
