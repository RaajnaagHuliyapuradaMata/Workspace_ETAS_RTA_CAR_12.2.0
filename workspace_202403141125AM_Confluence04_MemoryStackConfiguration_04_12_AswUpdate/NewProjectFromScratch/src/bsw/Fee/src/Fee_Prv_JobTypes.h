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

/*
 * The Job unit stores the received orders in internal job slots.
 * The main function will poll jobs from the Job unit and inform the Job unit if a job is finished.
 */

#ifndef FEE_PRV_JOBTYPES_H
#define FEE_PRV_JOBTYPES_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "Fee_Prv_ConfigTypes.h"
#include "Fee_Rb_Types.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* Every job is stored in such a job struct */
typedef struct
{
    /* Note: Don't combine bfr_pu8 and bfr_pcu8 in a union (e.g. using Fee_Prv_LibBufferU8_tun), they must be kept
     * separate to identify situations where we are waiting for the next chunk of a chunk-wise read or write job!   */
    uint8  *           bfr_pu8;                /* Pointer to a variable buffer - for read jobs  (see note above)    */
    uint8  const *     bfr_pcu8;               /* Pointer to a constant buffer - for write jobs (see note above)    */
    /* Union is used below to ease the understanding of the variable handling. This is used only for chunk job. In the
     * "Chunk user object" element nrBytProc_u32 is used and in "Working object" element cntrPayloadBytDone_u32 is used.
     * Since nrBytProc_u32 should also include chunk header size, the max value can be more than uint16,so it is defined
     * as uint32. However, max value for cntrPayloadBytDone_u32 can be 65535 (max payload size that Fee block can have).
     * To avoid padding issues, both the elements of the union are made of same type. */
    union
    {
        uint32         cntrPayloadBytDone_u32; /* Payload bytes that have been processed so far (in all chunks)     */
        uint32         nrBytProc_u32;          /* Bytes processed in last/current chunkwise job, incl. chunk header */
    } bytDone_un;
    uint16             blockNumber_u16;        /* Blocknumber as defined in Fee.h - this is NOT the persistent ID ! or 0xFFFF for unknown block (isUnknownBlk_b is also set to TRUE) */
    uint16             length_u16;             /* Length of block operation (in current chunk if job is chunk-wise) */
    uint16             offset_u16;             /* Offset of block operation                                         */
    uint16             nrBytTot_u16;           /* Total block length to process (in all chunks)                     */
    uint16             idPers_u16;             /* Block's persistent ID (unique block identifier)                   */
    uint16             statusFlag_u16;         /* Status flag (used for unknown block handling)                     */
    boolean            isChunkJob_b;           /* TRUE if the job is coming from chunk API, or FALSE otherwise      */
    boolean            isUnknownBlk_b;         /* TRUE if the job is for an unknown block, or FALSE otherwise       */
    Fee_Rb_JobType_ten type_en;                /* Read/Write/Invalidate/Maintain job                                */
    Fee_Rb_JobMode_ten jobMode_en;             /* Mode in which current job has to be executed                      */
} Fee_Prv_JobDesc_tst;

/* RAM variable for the unit. Here all the data objects of the unit are collected */
typedef struct
{
    Fee_Prv_JobDesc_tst      jobs_ast[FEE_RB_USER_MAX_E];     /* Details of accepted jobs for each user */
    Fee_Rb_JobResultType_ten results_aen[FEE_RB_USER_MAX_E];  /* Results of the last jobs for each user */

    Fee_Rb_JobMode_ten       jobMode_en[FEE_RB_JOBTYPE_MAX_E];/* Job mode for different jobs */

    Fee_Prv_JobDesc_tst      actvJobDesc_st;                  /* Details of currently active job being processed */
    Fee_Rb_User_ten          actvUser_en;                     /* Currently active user which is being processed */
    /*allwdUser_en = FEE_RB_USER_MAX_E: all jobs processed, else only user mentioned here is allowed to be processed
     * (ex. process only NvM blocks during crash data storage) */
    Fee_Rb_User_ten          allwdUser_en;                    /* See comments above */
    boolean                  isChunkJobCncldByOthUser_b;      /* = TRUE, chunk job was canceled by another user (ex NvM user during crash) */
}Fee_Prv_JobData_tst;

# endif

/* FEE_PRV_JOBTYPES_H */
#endif
