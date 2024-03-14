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

#ifndef FEE_RB_TYPES_H
#define FEE_RB_TYPES_H


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* Error codes for API used for DET module (FEE010) */
#define FEE_E_UNINIT                                 0x01u    /* API service called when module was not initialized */
#define FEE_E_INVALID_BLOCK_NO                       0x02u    /* API service called with invalid block number */
#define FEE_E_INVALID_BLOCK_OFS                      0x03u    /* API service called with invalid block offset */
#define FEE_E_PARAM_POINTER                          0x04u    /* API service called with invalid data pointer */
#define FEE_E_INVALID_DATA_PTR                       FEE_E_PARAM_POINTER  /* Backward Compatibility for AR 4.0 */
#define FEE_E_INVALID_BLOCK_LEN                      0x05u    /* API service called with invalid length information */
#define FEE_E_BUSY                                   0x06u    /* API service called while module still busy */
#define FEE_E_BUSY_INTERNAL                          0x07u    /* API service called while module is busing doing internal management operation */
#define FEE_E_INVALID_CANCEL                         0x08u    /* API Fee_Cancel called while no job was pending or being processed*/
#define FEE_E_INIT_FAILED                            0x09u    /* API service called when Fee_Init fails */
#define FEE_E_INVALID_VAR_BLK_LEN_CFG                0xFFu    /* API service called with invalid variable block length configuration */
#define FEE_E_INVALID_MIGRATION_CFG                  0xFEu    /* API service called with invalid block migration configuration */
#define FEE_E_INVALID_BLOCK_CFG                      0xFDu    /* API service called with invalid block configuration */
#define FEE_E_INVALID_DEVICE_NAME                    0xFCu    /* API service called with invalid device name */
#define FEE_E_INVALID_USER                           0xFBu    /* API service called by an invalid user */
#define FEE_E_INVALID_SEQUENCE                       0xFAu    /* API service called in a wrong sequence */
#define FEE_E_INVALID_BUF_LEN                        0xF9u    /* API service called with invalid buffer length */
#define FEE_E_INVALID_MNGT_DATA                      0xF8u    /* API service called with invalid management data */
#define FEE_E_INVALID_JOB_MODE_REQUESTED             0xF7u    /* API service called with invalid job mode */
#define FEE_E_INVALID_SECT_NR                        0xF6u    /* API service called with invalid logical/physical sector or row number */
#define FEE_E_INVALID_SYSTEM_JOB_REQUESTED           0xF5u    /* API service called with invalid system job */

/* API Service IDs used for DET module */
#define FEE_SID_INIT                                 0x00u    /* Service ID for the FEE Init function (unused) */
#define FEE_SID_SETMODE                              0x01u    /* Service ID for the FEE set mode function */
#define FEE_SID_READ                                 0x02u    /* Service ID for the FEE read function */
#define FEE_SID_WRITE                                0x03u    /* Service ID for the FEE write function */
#define FEE_SID_CANCEL                               0x04u    /* Service ID for the FEE job cancel function (unused) */
#define FEE_SID_GETSTATUS                            0x05u    /* Service ID for the FEE get status function (unused) */
#define FEE_SID_GETJOBRESULT                         0x06u    /* Service ID for the FEE get job result function for NvM requests */
#define FEE_SID_INVALIDATE                           0x07u    /* Service ID for the FEE invalidate function */
#define FEE_SID_GETVERSIONINFO                       0x08u    /* Service ID for the FEE get version info function */
#define FEE_SID_ERASEIMMEDIATEBLOCK                  0x09u    /* Service ID for the FEE erase immediate block function (unused) */
#define FEE_SID_JOBENDNOTIFICATION                   0x10u    /* Service ID for the FEE job end notification (unused) */
#define FEE_SID_JOBERRORNOTIFICATION                 0x11u    /* Service ID for the FEE job error notification (unused) */
#define FEE_SID_MAINFUNCTION                         0x12u    /* Service ID for the FEE main function (unused) */
#define FEE_SID_RB_GET_STRESS_MEASMT_RESULT          0xDAu    /* Service ID for the FEE get stress measurement result API */
#define FEE_SID_RB_RESTART_STRESS_MEASMT             0xDBu    /* Service ID for the FEE restart stress measurement API */
#define FEE_SID_RB_EXIT_HIGH_SPEED_MODE              0xDCu    /* Service ID for the FEE exit high speed execution mode API */
#define FEE_SID_RB_ENTER_HIGH_SPEED_MODE             0xDDu    /* Service ID for the FEE enter high speed execution mode API */
#define FEE_SID_RB_GET_SECT_STATE                    0xDEu    /* Service ID for the FEE get sector or row state API */
#define FEE_SID_RB_SYSTEM_JOB                        0xDFu    /* Service ID for the FEE system job */
#define FEE_SID_RB_SYSTEM_JOB_GETRESULT              0xE0u    /* Service ID for the FEE get job result function for system requests */
#define FEE_SID_RB_GETDETAILEDBLKINFO                0xE1u    /* Service ID for the FEE to read the detailed block info */
#define FEE_SID_RB_GET_NR_OF_SUPRTD_ERASES           0xE2u    /* Service ID for the FEE get number of supported erases function */
#define FEE_SID_RB_GETJOBMODE                        0xE4u    /* Service ID for the FEE get job mode function */
#define FEE_SID_RB_SETJOBMODE                        0xE5u    /* Service ID for the FEE set job mode function */
#define FEE_SID_RB_CHUNK_READ_FIRST                  0xE6u    /* Service ID for the FEE read first chunk function */
#define FEE_SID_RB_CHUNK_READ_NEXT                   0xE7u    /* Service ID for the FEE read next chunk function */
#define FEE_SID_RB_CHUNK_WRITE_FIRST                 0xE8u    /* Service ID for the FEE write first chunk function */
#define FEE_SID_RB_CHUNK_WRITE_NEXT                  0xE9u    /* Service ID for the FEE write next chunk function */
#define FEE_SID_RB_CHUNK_GET_RESULT                  0xEAu    /* Service ID for the FEE get chunk result function */
#define FEE_SID_RB_CHUNK_GET_NR_BYT_PROC             0xEBu    /* Service ID for the FEE get number of processed chunk bytes function */
/*                                                   0xECu       Service ID for the FEE terminate chunk function, do not reuse */
#define FEE_SID_RB_ISBLOCKDOUBLESTORAGE              0xEDu    /* Service ID for the FEE is block double storage function */
#define FEE_SID_RB_GETMODE                           0xEEu    /* Service ID for the FEE get mode function */
#define FEE_SID_RB_WRITE_PERSISTENT_DATA             0xEFu    /* Service ID for the FEE write persistent data API */
#define FEE_SID_RB_INTERNAL_GETJOBRESULT             0xF0u    /* Service ID for the FEE get job result function for internal layer requests */
#define FEE_SID_RB_GET_SECT_ERASE_CNTR               0xF1u    /* Service ID for the FEE get sector erase counter API */
#define FEE_SID_RB_GET_WORKING_STATE                 0xF2u    /* Service ID for the FEE get working state API */
#define FEE_SID_RB_GET_NR_FREE_BYTES_AND_FAT_ENTRIES 0xF3u    /* Service ID for the FEE get free space and free FAT entries API */
#define FEE_SID_RB_END_INIT                          0xF4u    /* Service ID for the FEE end init function */
#define FEE_SID_RB_GET_NR_FREE_BYTES                 0xF5u    /* Service ID for the FEE get free space API */
#define FEE_SID_RB_GET_SECT_CHNG_CNT                 0xF6u    /* Service ID for the FEE get sector change counter API */
#define FEE_SID_RB_ENTER_STOP_MODE                   0xF7u    /* Service ID for the FEE stop mode function */
/*                                                   0xF8u       Service ID formerly used for the FEE forced sector reorganisation, do not reuse */
#define FEE_SID_RB_ENABLE_BG                         0xF9u    /* Service ID for the FEE enable background operations function */
#define FEE_SID_RB_DISABLE_BG                        0xFAu    /* Service ID for the FEE disable background operations function */
#define FEE_SID_RB_MAINTAIN                          0xFBu    /* Service ID for the FEE maintenance function */
#define FEE_SID_RB_ADAPTERGETJOBRESULT               0xFCu    /* Service ID for the FEE get job result function for adapter layer requests */
#define FEE_SID_RB_VARLENWRITE                       0xFDu    /* Service ID for the FEE Rb variable length write function */
#define FEE_SID_RB_VARLENREAD                        0xFEu    /* Service ID for the FEE Rb variable length read function */
#define FEE_SID_RB_GETMIGRATIONRESULT                0xFFu    /* Service ID for the FEE to read the migration result */


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* Global Fee_MainFunction state machine type
 * Necessary in public header because it is needed by the Mx17 Adapter for the Eep_GetState function */
typedef enum
{
    FEE_RB_IDLE_E = 0,               /* Nothing to do, check order queue */
    FEE_RB_WRITE_MODE_E,             /* A write order is currently being proceeded */
    FEE_RB_READ_MODE_E,              /* A read order is currently being proceeded */
    FEE_RB_INVALIDATE_MODE_E,        /* An invalidation is requested */
    FEE_RB_MAINTAIN_MODE_E,          /* An block maintenance is requested */
    FEE_RB_SOFT_SECTOR_REORG_MODE_E, /* Perform a sector reorganization in the background */
    FEE_RB_HARD_SECTOR_REORG_MODE_E, /* Perform a sector reorganization without allowing interruptions */
    FEE_RB_SECTOR_ERASE_E,           /* The sector will be erased */
    FEE_RB_STOPMODE_E                /* Stop mode requested */
}Fee_Rb_WorkingStateType_ten;

/* Possible order types */
/* Note: To be removed when Fs1 is removed - keep it now for compatibility */
typedef enum
{
    FEE_NO_ORDER = 0,           /* there is no order active */
    FEE_READ_ORDER,             /* order-entry belongs to an read job */
    FEE_WRITE_ORDER,            /* order-entry belongs to an write job */
    FEE_INVALIDATE_ORDER,       /* order-entry belongs to an invalidate job */
    FEE_MAINTAIN_ORDER,         /* order-entry belongs to an maintenance job */
    FEE_FORCED_READ_ORDER       /* order-entry belongs to an forced read job */
}Fee_HlMode_ten;

/* Different kind of Fee jobs. If needed there could be more jobs added in future like MainFunction and so on */
typedef enum
{
    /* Block oriented jobs */
    FEE_RB_JOBTYPE_READ_E = 0,             /* Read a block */
    FEE_RB_JOBTYPE_WRITE_E,                /* Write a block */
    FEE_RB_JOBTYPE_INVALIDATE_E,           /* Invalidate a block */
    FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E,     /* Maintain blocks */

    /* System jobs */
    FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORG_E,    /* FS1x: Perform a sector reorganization    */
    FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGFAT_E, /* FS2: Perform a FAT sector reorganization */
    FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGDAT_E, /* FS2: Perform a DAT sector reorganization */
    FEE_RB_SYSTEM_JOBTYPE_STOP_MODE_E,       /* Stop driver                              */

    FEE_RB_JOBTYPE_MAX_E,                  /* This enum should not be used by users. It is used internally to define the size of the job type array. */
    FEE_RB_JOBTYPE_WAIT_NEXT_CHUNK_E       /* This enum is no real job and shall therefore be behind FEE_RB_JOBTYPE_MAX_E */
}Fee_Rb_JobType_ten;

/* Different possible job return values */
typedef enum
{
    FEE_RB_JOB_OK = 0,                  /* Processing a job has finished successfully */
    FEE_RB_JOB_FAILED,                  /* Processing a job has failed */
    FEE_RB_JOB_PENDING,                 /* Processing a job hasn't finished yet */
    FEE_RB_JOB_CANCELED,                /* Processing a job is canceled */
    FEE_RB_BLOCK_INCONSISTENT,          /* Block is inconsistent */
    FEE_RB_BLOCK_INVALID,               /* Block is invalid */
    FEE_RB_BLOCK_PART_OK                /* Processing of last chunk has fishished successfully, ready for next chunk */
}Fee_Rb_JobResultType_ten;

/* Different possible modes that could be set for the jobs */
typedef enum
{
    FEE_RB_ALLJOBS_ALLSTEPS_E = 0,                    /* Default mode setting for all jobs. All standard steps are performed for each job */
    FEE_RB_WRITEJOB_WRITE_VERIFY_E,                   /* Only write and verify, skip all other steps */
    FEE_RB_WRITEJOB_WRITE_ONLY_E,                     /* Fastest write mode: Only write the data without any further steps */
    FEE_RB_INVALIDATEJOB_WRITE_VERIFY_E,              /* Only invalidate and verify, skip all other steps */
    FEE_RB_INVALIDATEJOB_WRITE_ONLY_E,                /* Fastest invalidate mode: Only invalidate block without any further steps */
    FEE_RB_WRITEJOB_SKIP_HARDREORG_WRITE_ONLY_E,      /* Allow writes during hard reorg + fastest write mode without any further steps */
    FEE_RB_INVALIDATEJOB_SKIP_HARDREORG_WRITE_ONLY_E, /* Allow invalidate during hard reorg + fastest invalidate mode without any further steps */
    FEE_RB_BLOCKMAINTENANCE_DISABLE_E,                /* Disable block maintaenance feature -> return E_NOT_OK when new request is placed */

    FEE_RB_JOBMODE_MAX_E                              /* This enum should not be used by users. It is used internally to valdiate the parameter passed by the user. */
}Fee_Rb_JobMode_ten;

typedef struct
{
    uint8    dummy_u8;    /* Postcompile configuration isn't supported; Fee_Init always has to get a NULL_PTR */
} Fee_ConfigType;

typedef struct
{   /*
     * In future it may be possible that Fee_Rb_Idx_SystemJob requires additional parameters for certain job types
     * This can be done by adding new elements to this structure.
     * Advantage: Prototype of Fee_Rb_Idx_SystemJob has not to be changed

     * If the first relevant parameter has been added to this structure dummy_u8 should be removed.
     * It is just added to prevent compiler warnings due to an empty structure
     */
    uint8    dummy_u8;
}Fee_Rb_SystemJobParam_tst;

/* Simplified state of the sector in Fee1x, Fee2 or row state in Fee3 */
typedef enum
{
    FEE_RB_SECT_STATE_UNDEFD_E = 0,    /* Sector or row is undefined state. Has no valid markers and user data yet. */
    FEE_RB_SECT_STATE_ERASED_E,        /* Sector or row has valid marker but no user data yet. */
    FEE_RB_SECT_STATE_USED_E,          /* Sector or row has valid marker + valid user data */
    FEE_RB_SECT_STATE_DEFECTIVE_E      /* Sector or row is defective and cannot be used (ex. read, erase, program failure) */
}Fee_Rb_SectState_ten;

/* The users are handled with descending priority of this enumerator.
 * If a new user is added, it has to be placed inside this enumerator according to its priority. */
typedef enum
{
    FEE_RB_USER_SYSTEM_E = 0,
    FEE_RB_USER_NVM_E,
    FEE_RB_USER_ADAPTER_E,
    FEE_RB_USER_CHUNK_E,
    FEE_RB_USER_DEBUG_E,
    FEE_RB_USER_MAX_E
} Fee_Rb_User_ten;

/* #ifndef FEE_RB_TYPES_H */
#endif
