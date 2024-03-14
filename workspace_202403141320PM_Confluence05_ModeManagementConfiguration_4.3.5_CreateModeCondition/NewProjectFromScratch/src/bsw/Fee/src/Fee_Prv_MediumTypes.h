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
 * The medium unit is the lowest layer in the Fee architecture.
 * It abstracts direct flash access and Fls_Read flash access.
 * The Fee assumes that it is the only flash user.
 * To reduce the number of error paths it is not visible to higher layers,
 * whether starting a flash job returned with E_NOT_OK or
 * whether the flash job finished with MEMIF_JOB_FAILED.
 * Additionally it contains a big buffer which is used for various operations,
 * mainly the buffer is used for writing internal management data and during the data sector change.
 * The medium unit does also provide blocking flash access,
 * this means polling until Fls_GetJobResult != MEMIF_JOB_PENDING
 * or asynchronous flash access.
*/

#ifndef FEE_PRV_MEDIUMTYPES_H
#define FEE_PRV_MEDIUMTYPES_H


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "MemIf_Types.h"
#include "Fls.h"
#include "Fee_Cfg_SchM.h"
#include "Fee_Prv_LibTypes.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* in order to avoid creating a new Paramdef,
 * the parameter FEE_RB_WRITE_PADDING_REQUIRED should be defined in Fee_Cfg_Schm.h only to test this feature */
#  ifndef FEE_RB_WRITE_PADDING_REQUIRED
#   if(defined(FLS_RB_VENDOR_ID) && (0x06 == FLS_RB_VENDOR_ID))
/* MemStack FLS is used */
#define FEE_RB_WRITE_PADDING_REQUIRED  (FALSE)
#   else
/* 3rd-party FLS is used */
#define FEE_RB_WRITE_PADDING_REQUIRED  (TRUE)
#   endif
#  endif

/* This solution is chosen as this is needed for testing purpose when direct memory access is not possible. */
#  ifdef RBA_FLSVECU_FLASH
/* Since VECU is being used, direct memory access is not possible (ignore the setting from the external user). */
#define FEE_PRV_MEDIUM_DIRECT_READ_ALLOWED  (FALSE)
#  endif


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* Prefetch modes */
typedef enum
{
    FEE_PRV_PRELOAD_E = 0,
    FEE_PRV_POSTLOAD_E
} Fee_Prv_MediumLoadMode_ten;

/* States for loading data from flash to RAM */
typedef enum
{
    FEE_PRV_LOAD_IDLE_E = 0,    /* Nothing to do        */
    FEE_PRV_LOAD_BUSY_E         /* Currently loading    */
} Fee_Prv_MediumLoad_ten;

/* Behaviour on buffer hit */
typedef enum
{
    FEE_PRV_MEDIUM_BUF_CUT_E = 0,   /* Data of all buffers above the passed start address is cut off */
    FEE_PRV_MEDIUM_BUF_CLR_E        /* Data of all buffers is cleared                                */
} Fee_Prv_Medium_BufBhvr_ten;

/* Sequences for programming data from RAM to flash */
typedef enum
{
    FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COPY_COMP_E = 0,       /* Write sequence: blank check              -> copy -> program -> compare */
    FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_WLBCKP_COPY_COMP_E,    /* Write sequence: blank check -> wl backup -> copy -> program -> compare */
    FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COMP_E,                /* Write sequence: blank check                      -> program -> compare */
    FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_WLBCKP_COMP_E,         /* Write sequence: blank check -> wl backup         -> program -> compare */
    FEE_PRV_MEDIUM_SEQ_WRITE_COPY_COMP_E,                   /* Write sequence:                             copy -> program -> compare */
    FEE_PRV_MEDIUM_SEQ_WRITE_COMP_E,                        /* Write sequence:                                     program -> compare */
    FEE_PRV_MEDIUM_SEQ_WRITE_ONLY_E                         /* Write sequence:                                     program            */
} Fee_Prv_MediumSeqWrite_ten;

/* State for programming data from RAM to flash */
typedef enum
{
    FEE_PRV_MEDIUM_WRITE_IDLE_E = 0,    /* Nothing to do                                        */
    FEE_PRV_MEDIUM_WRITE_BLANKCHECK_E,  /* Make sure the target space is not already programmed */
    FEE_PRV_MEDIUM_WRITE_BACKUPWL_E,    /* Backup the wordline in case of wordline error        */
    FEE_PRV_MEDIUM_WRITE_COPYDATA_E,    /* copy data from user buffer to internal buffer        */
    FEE_PRV_MEDIUM_WRITE_PADDING_E,     /* Prepare padded data in internal buffer               */
    FEE_PRV_MEDIUM_WRITE_PROGRAM_E,     /* Program data from RAM to flash                       */
    FEE_PRV_MEDIUM_WRITE_COMPARE_E,     /* Compare RAM to flash                                 */
    FEE_PRV_MEDIUM_WRITE_DETNWLERR_E,   /* Detect a possible wordline error                     */
    FEE_PRV_MEDIUM_WRITE_FINAL_E        /* Programming finished successfully                    */
} Fee_Prv_MediumWrite_ten;

/* Sequences for copying data from one flash location to another (using internal buffer) */
typedef enum
{
    FEE_PRV_MEDIUM_SEQ_COPY_BLNKCHK_COMP_E = 0,     /* Copy sequence:  blank check              -> copy -> compare */
    FEE_PRV_MEDIUM_SEQ_COPY_BLNKCHK_WLBCKP_COMP_E,  /* Copy sequence:  blank check -> wl backup -> copy -> compare */
    FEE_PRV_MEDIUM_SEQ_COPY_COMP_E,                 /* Copy sequence:                           -> copy -> compare */
    FEE_PRV_MEDIUM_SEQ_COPY_ONLY_E                  /* Yet unsupported copy sequence: copy                         */
} Fee_Prv_MediumSeqCopy_ten;

/* State for copying data from one flash location to another (using internal buffer) */
typedef enum
{
    FEE_PRV_MEDIUM_COPY_IDLE_E = 0,     /* Nothing to do                            */
    FEE_PRV_MEDIUM_COPY_READ_E,         /* Read data from flash to internal buffer  */
    FEE_PRV_MEDIUM_COPY_WRITE_E,        /* Write data from internal buffer to flash */
    FEE_PRV_MEDIUM_COPY_FINAL_E         /* Copy finished succcessfully              */
} Fee_Prv_MediumCopy_ten;

/* State for comparing data from one flash location to another (using internal buffer) */
typedef enum
{
    FEE_PRV_MEDIUM_COMPAREFLS2FLS_IDLE_E = 0,     /* Nothing to do                                              */
    FEE_PRV_MEDIUM_COMPAREFLS2FLS_READ_E,         /* Read data from flash to internal buffer                    */
    FEE_PRV_MEDIUM_COMPAREFLS2FLS_COMPARE_E,      /* Compare data from internal buffer to 2nd flash location    */
    FEE_PRV_MEDIUM_COMPAREFLS2FLS_FINAL_E         /* Compare finished succcessfully                             */
} Fee_Prv_MediumCompareFls2Fls_ten;

/* Fls jobs triggered by Fee_Medium */
typedef enum
{
    FEE_PRV_MEDIUM_FLS_NONE_E = 0,
    FEE_PRV_MEDIUM_FLS_READ_E,
    FEE_PRV_MEDIUM_FLS_WRITE_E,
    FEE_PRV_MEDIUM_FLS_ERASE_E,
    FEE_PRV_MEDIUM_FLS_COMPARE_E,
    FEE_PRV_MEDIUM_FLS_BLANKCHECK_E
} Fee_Prv_MediumFlsJob_ten;

typedef struct
{
    uint32    maxPrefetchLen_u32;                           /* Maximum amount of data to load including prefetch     */
    uint32    iterator_u32;                                 /* Iterator used to iterate through the flash in steps   */
    uint32    reloadAdr_u32;                                /* Address to reload from                                */
    uint32    nrBytes2Load_u32;                             /* Number of bytes to load                               */
    uint32    limitLo_u32;                                  /* Upper limit of data from flash in the medium buffer   */
    uint32    limitHi_u32;                                  /* Lower limit of data from flash in the medium buffer   */
    uint8     nrPageShift_u8;                               /* Data shift used when loading buffer (in no. of pages) */
} Fee_Prv_MediumBufData_tst;

typedef struct
{
    Fee_Prv_LibBufferU8_tun     dataBuf_un;            /* Buffer for used when programming to or loading from medium */
    uint32                      flsAdr_u32;            /* Address for Fls request (also needed to get the result)    */
    uint32                      nrBytToProc_u32;       /* No. of bytes to process                                    */
    uint16                      adrOffs_u16;           /* Address offset to consider for Fls job                     */
    Fee_Prv_MediumFlsJob_ten    idFlsJobType_en;       /* Selected Fls job type                                      */
    Fee_Rb_DeviceName_ten       deviceName_en;         /* Device name for which the job has to be performed          */
    Fls_Rb_FlsDevice_ten        flsDevId_en;           /* Name of the device index that needs to be used for Fls job */
    Std_ReturnType              lastFlsResult_en;      /* Result of the last Fls job                                 */
    boolean                     isTraErrSuppReqd_b;    /* Suppress transient error reporting on Fls level            */
    boolean                     sync_b;                /* Polling = TRUE or Asynchronous = FALSE flash access        */
}  Fee_Prv_Medium_tst;

typedef struct
{
    Fee_Prv_MediumLoad_ten  state_en;                                                               /* Current state */
} Fee_Prv_MediumLoad_tst;

typedef struct
{
    uint32      nrBytesDone_u32;                                             /* Already considered number of bytes   */
    uint32      length_u32;                                                  /* Total number of bytes to consider    */
    uint32 *    crc_pu32;                                                    /* Pointer to the result                */
    boolean     firstCall_b;                                                 /* First CRC call yes or no             */
} Fee_Prv_MediumCalcCrc_tst;

typedef struct
{
    uint8 const *    adr_pcu8;                                   /* Address where to read from                       */
    uint8 *          bfr_pu8;                                    /* Buffer where to write into (= user buffer)       */
    uint32           length_u32;                                 /* Bytes to read in total                           */
    uint32           nrBytesDone_u32;                            /* Number of bytes already processed                */
    boolean          isBufferHit_b;                              /* Flag indicates if src address is in Flash or RAM */
} Fee_Prv_MediumRead_tst;

typedef struct
{
    void const *               bfr_pcv;             /* Buffer where to program from (= user buffer)                  */
    void const *               data2Program_pcv;    /* Pointer to data, which is programmed in current chunk         */
    uint32                     adrDest_u32;         /* Address where to program to                                   */
    uint32                     length_u32;          /* Bytes to program in total                                     */
    uint32                     done_u32;            /* Bytes already programmed                                      */
    uint32                     bytes2Program_u32;   /* Bytes to program in current chunk                             */
    Fee_Prv_MediumWrite_ten    state_en;            /* State of the programming operation                            */
    Fee_Rb_DeviceName_ten      deviceName_en;       /* Device name for which the job has to be performed             */
    uint8                      nrPageShift_u8;      /* Data shift to consider (in no. of pages)                      */
    boolean                    isMedBfrByp_b;       /* Bypass internal medium buffer (i.e. program from user buffer) */
    boolean                    isTraErrSuppReqd_b;  /* Suppress transient error reporting on Fls level               */
    boolean                    isCmpSkip_b;         /* Skip compare operation after write                            */
    boolean                    isWlBckpReqd_b;      /* Is a backup requested by user for current write               */
    boolean                    hasWlError_b;        /* Has wordline error occured during write                       */
    boolean                    entry_b;             /* Next state shall be entered                                   */
} Fee_Prv_MediumWrite_tst;

typedef struct
{
    uint32                        adrSrc_u32;                /* Source address in flash                              */
    uint32                        adrDest_u32;               /* Destination address in flash                         */
    uint32                        length_u32;                /* Bytes to copy in total                               */
    uint32                        done_u32;                  /* Bytes already copied to flash                        */
    uint32                        bytes2Copy_u32;            /* Bytes to copy in current chunk                       */
    Fee_Prv_MediumCopy_ten        state_en;                  /* State of the copy operation                          */
    Fee_Prv_MediumSeqWrite_ten    writeSeq_en;               /* Requested write sequence for copy operation          */
    Fee_Rb_DeviceName_ten         deviceName_en;             /* Device name for which the job has to be performed    */
    uint8                         nrPageShiftSrc_u8;         /* Source data shift to consider (in no. of pages)      */
    uint8                         nrPageShiftDest_u8;        /* Destination data shift to consider (in no. of pages) */
    boolean                       entry_b;                   /* Next state shall be entered                          */
} Fee_Prv_MediumCopy_tst;

typedef struct
{
    uint32                            adrFls1_u32;              /* Address of 1st flash location                     */
    uint32                            adrFls2_u32;              /* Address of 2nd flash location                     */
    uint32                            length_u32;               /* Bytes to compare in total                         */
    uint32                            nrBytesDone_u32;          /* Bytes already compared in flash                   */
    uint32                            nrBytesCurrentChunk_u32;  /* Bytes to compare in current chunk                 */
    Fee_Prv_MediumCompareFls2Fls_ten  state_en;                 /* State of the compare operation                    */
    Fee_Rb_DeviceName_ten             deviceName_en;            /* Device name for which the job has to be performed */
    uint8                             nrPageShift1_u8;          /* 1st data shift to consider (in no. of pages)      */
    uint8                             nrPageShift2_u8;          /* 2nd data shift to consider (in no. of pages)      */
    boolean                           entry_b;                  /* Next state shall be entered                       */
} Fee_Prv_MediumCompareFls2Fls_tst;

/* RAM variable for the unit. Here all the data objects of the unit are collected */
typedef struct
{
    Fee_Prv_MediumBufData_tst           mediumBufData_ast[FEE_PRV_CFG_RB_NR_PAGE_BUFFER];
    Fee_Prv_Medium_tst                  medium_st;
    Fee_Prv_MediumLoad_tst              mediumLoad_st;
    Fee_Prv_MediumCalcCrc_tst           mediumCalcCrc_st;
    Fee_Prv_MediumRead_tst              mediumRead_st;
    Fee_Prv_MediumWrite_tst             mediumWrite_st;
    Fee_Prv_MediumCopy_tst              mediumCopy_st;
    Fee_Prv_MediumCompareFls2Fls_tst    mediumCompareFls2Fls_st;
}Fee_Prv_MediumData_tst;

# endif
/* FEE_PRV_MEDIUMTYPES_H */
#endif
