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

#ifndef RBA_EAFS2_PRV_H
#define RBA_EAFS2_PRV_H

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#if(EA_DEV_ERROR_DETECT != FALSE) || (EA_DEV_RUNTIME_ERROR_DETECT != FALSE)
#include "Det.h"
#endif
#include "Crc.h"
#include "rba_EaFs2_Prv_Order.h"
#include "rba_EaFs2_Prv_Fat.h"
#include "rba_EaFs2_Prv_Data.h"
#include "rba_EaFs2_Prv_Eeprom.h"
#include "rba_EaFs2_Prv_Defrag.h"

#include "rba_EaFs2_Prv_BlockProperty.h"

#if(FALSE != EA_PRV_DEBUGGING)
#include "rba_EaFs2_Prv_Dbg.h"
#endif


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

#define RBA_EAFS2_PRV_DEFRAGBLK_ADDR             ((EA_EEP_START_OFFSET)- (2u*sizeof(rba_EaFs2_Prv_DefragBlk_tst)))

#define RBA_EAFS2_PRV_CLIPBOARD_SIZE             (256u)
#define RBA_EAFS2_PRV_BLKCHECKLIMIT              (500u)

#define RBA_EAFS2_PRV_MAX_CRC_CALC_PER_CALL      (512u)
#define RBA_EAFS2_PRV_MIN_GAP_IN_EEPROM          (4u*sizeof(rba_EaFs2_Prv_FatEntry_tst))
#if (EA_WRITE_SPREAD != FALSE)
#define RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE   (8u) /* size of memory area of write counter in Eeprom*/
#define RBA_EAFS2_PRV_WRITESPREAD_COUNTER_LENGTH (4u) /* length of write counter*/
#endif /*(EA_WRITE_SPREAD != FALSE)*/

#define RBA_EAFS2_PRV_SECURITY_LEVEL_FLAG        0x01u   /* Set flag for security level 2, store block twice. Bit 0 */
#define RBA_EAFS2_PRV_SURVIVAL_FLAG              0x10u   /* Set flag for survival data. Bit 5                       */
#if (EA_WRITE_SPREAD != FALSE)
#define RBA_EAFS2_PRV_WRITESPREAD_FLAG           0x02u   /* Set flag for write spread. Bit 1                        */
#endif /*(EA_WRITE_SPREAD != FALSE)*/
#define RBA_EAFS2_PRV_REPAIR_DONE                0x01u   /* Flag to check repair status                             */
#define SECOND_COPY_CORRUPTED                    (2u)
#define FIRST_COPY_CORRUPTED                     (1u)


#define RBA_EAFS2_PRV_LIBTRANS(FSM_STRUCT, FSM_STATE)    \
        {                                         \
    (FSM_STRUCT).state_en   = (FSM_STATE);  \
    (FSM_STRUCT).entry_b    = ((boolean)TRUE);       \
        }

#define RBA_EAFS2_PRV_LIBSC(FSM_STATE)             RBA_EAFS2_PRV_LIBTRANS((*fsm_pst), (FSM_STATE))
#define RBA_EAFS2_PRV_LIBENTRY                     (rba_EaFs2_Prv_LibEnter(&fsm_pst->entry_b))


#if(EA_DEV_ERROR_DETECT != FALSE)
#define RBA_EAFS2_DET_REPORTERROR(APID, ERRID)    (void)Det_ReportError(EA_MODULE_ID, EA_INSTANCE_ID, APID, ERRID)
#else
#define RBA_EAFS2_DET_REPORTERROR(APID, ERRID)    (void)APID
#endif

#if(EA_DEV_RUNTIME_ERROR_DETECT != FALSE)
#define RBA_EAFS2_DET_REPORTRUNTIMEERROR(APID, ERRID)    (void)Det_ReportRuntimeError(EA_MODULE_ID, EA_INSTANCE_ID, APID, ERRID)
#else
#define RBA_EAFS2_DET_REPORTRUNTIMEERROR(APID, ERRID)    (void)APID
#endif

#if(FALSE == EA_RB_ACT_UNKNOWN_BLOCK_MIG)
#define RBA_EAFS2_PRV_TEMPMARKER                0xDEADBEEFuL   /* Temp Marker pattern in EaFs2*/
#endif

/*
 * Terminology
 *
 * DataLengthInMedium       :  Data Length Aligned to ECC
 * BlkLengthInMedium        :  DataLengthInMedium + Metadata length (8)
 * SizeInMedium             :  BlkLengthInMedium * Security Level (1 or 2)
 *
 * Block                    :  FAT entry details manipulated for ease of computation
 * Metadata                 :  Data validity information, stored along with data (CRC, Validity, WIP)
 * Fat Entry                :  Block Header formatted to write in the medium
 * */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

typedef enum
{
    RBA_EAFS2_PRV_READ_IDLE_E = 0,        /* Idle state for read state machine                             */
    RBA_EAFS2_PRV_READ_INIT_E,            /* Initializes structures needed for read operation              */
    RBA_EAFS2_PRV_READ_READMETA_E,        /* Reads meta data                                               */
    RBA_EAFS2_PRV_READ_CALCCRC_E,         /* Computes the CRC from the user buffer                         */
    RBA_EAFS2_PRV_READ_VERIFYCRC_E,       /* Computes CRC by reading from EEPROM                           */
    RBA_EAFS2_PRV_READ_READPAYLOAD_E,     /* Reads the data                                                */
    RBA_EAFS2_PRV_READ_FAILHANDLE_E,      /* Checks for Security level in case of corrupt copies           */
#if (EA_WRITE_SPREAD != FALSE)
    RBA_EAFS2_PRV_READ_GETLOCATION_E,     /* Finds the current location of the write spread block          */
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    RBA_EAFS2_PRV_READ_FINAL_E            /* Final state for read state machine                            */
}rba_EaFs2_Prv_Read_ten;



typedef enum
{
    RBA_EAFS2_PRV_WRITE_IDLE_E = 0,       /* Idle state for write state machine                            */
    RBA_EAFS2_PRV_WRITE_INIT_E,           /* Initializes structures needed for read operation              */
    RBA_EAFS2_PRV_WRITE_REPAIR_E,         /* Repairs SL2 blocks if needs or Reads Wip for first time read  */
    RBA_EAFS2_PRV_WRITE_ALLOCSPACE_E,     /* Performs block allocation                                     */
    RBA_EAFS2_PRV_WRITE_GETCRC_E,         /* Calculates block data CRC                                     */
    RBA_EAFS2_PRV_WRITE_READMETA_E,       /* Read meta data of the block to write                          */
    RBA_EAFS2_PRV_WRITE_COMPARE_E,        /* Compare contents of user buffer with eeprom data              */
    RBA_EAFS2_PRV_WRITE_OPENMETADATA_E,   /* Writes meta data part1                                        */
    RBA_EAFS2_PRV_WRITE_CLOSEMETADATA_E,  /* Writes meta data part2                                        */
    RBA_EAFS2_PRV_WRITE_VERIFY_E,         /* Verify if the data written in EEPROM is as expected           */
    RBA_EAFS2_PRV_WRITE_INVALIDATE_E,     /* Writes an invalidated meta data                               */
    RBA_EAFS2_PRV_WRITE_DATAWRITE_E,      /* performs data write                                           */
    RBA_EAFS2_PRV_WRITE_FATWRITE_E,       /* Writes FAT entry                                              */
#if (EA_WRITE_SPREAD != FALSE)
    RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E,  /* Increment the counter of the block after write                */
    RBA_EAFS2_PRV_WRITE_CLEARCOUNTER_E,   /* Clear all the counters in the allocated write spread locations*/
    RBA_EAFS2_PRV_WRITE_GETLOCATION_E,    /* Finds the current location of the write spread block          */
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    RBA_EAFS2_PRV_WRITE_FINAL_E           /* Final state for write state machine                           */
}rba_EaFs2_Prv_Write_ten;

typedef enum
{
    RBA_EAFS2_PRV_BGO_IDLE_E = 0,         /* Idle state for bgo operations                                 */
    RBA_EAFS2_PRV_BGO_INIT_E,             /* Initializes structures needed for bgo operations              */
    RBA_EAFS2_PRV_BGO_GETSPACE_E,         /* Checks if space is available in eeprom                        */
    RBA_EAFS2_PRV_BGO_WRITEFAT_E,         /* Writes FAT entry                                              */
    RBA_EAFS2_PRV_BGO_EMPTYMETA_E,        /* Writes an empty meta data i.e WIP 0xFFFF                      */
#if (EA_WRITE_SPREAD != FALSE)
    RBA_EAFS2_PRV_BGO_CLEARCOUNTER_E,     /* Clears the block counter in all allocated locations           */
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    RBA_EAFS2_PRV_BGO_FINAL_E             /* Final state for bgo state machine                             */
}rba_EaFs2_Prv_Bgo_ten;

typedef enum
{
    RBA_EAFS2_PRV_FATREPAIR_INIT_E = 0,          /* Initial state to decide if marker update is needed or not   */
    RBA_EAFS2_PRV_FATREPAIR_FATREADINIT_E,       /* Fat read initial state to check header copy corrupt or not  */
    RBA_EAFS2_PRV_FATREPAIR_FATREAD_E,           /* Fat read state if one of Fats is corrupted                  */
    RBA_EAFS2_PRV_FATREPAIR_FATWRITE_E,          /* Fat write state if one of Fats is corrupted                 */
    RBA_EAFS2_PRV_FATREPAIR_FATWIRTECOMPARE_E,   /* verify data state after write Fat                           */
    RBA_EAFS2_PRV_FATREPAIR_WRITEMARKER_E,       /* Write config marker                                         */
    RBA_EAFS2_PRV_FATREPAIR_COMPAREMARKER_E,     /* Compare config marker                                       */
    RBA_EAFS2_PRV_FATREPAIR_ERASEFAT_E,          /* Erase FAT to trigger end of FAT                             */
    RBA_EAFS2_PRV_FATREPAIR_FINAL_E              /* Final state FAT repair state machine                        */
}rba_EaFs2_Prv_FatRepair_ten;

#if(FALSE == EA_RB_ACT_UNKNOWN_BLOCK_MIG)
typedef enum
{
    RBA_EAFS2_PRV_TEMPMARKER_INIT_E = 0,   /* Initial state                                             */
    RBA_EAFS2_PRV_TEMPMARKER_WRITE_E,      /* Write temp marker                                         */
    RBA_EAFS2_PRV_TEMPMARKER_COMPARE_E,    /*Compare temp marker                                        */
    RBA_EAFS2_PRV_TEMPMARKER_FINAL_E       /*Final state                                                */
}rba_EaFs2_Prv_TempMarker_ten;
#endif

#if (EA_RB_ENTER_STOP_MODE != FALSE)
typedef enum
{
    RBA_EAFS2_PRV_STOPMODE_NOTREQUESTED = 0,    /* Stop Mode Not Requested   */
    RBA_EAFS2_PRV_STOPMODE_INPROCESS,           /* Stop Mode Request Processing ongoing    */
    RBA_EAFS2_PRV_STOPMODE_ACTIVATED            /* Ea module entered into stop mode  */
}rba_EaFs2_Prv_StopMode_ten;
#endif

typedef enum
{
    EA_PRV_DRIVERSTATE_UNINIT_E            = 0,
    EA_PRV_DRIVERSTATE_INIT_E              = 1,
    EA_PRV_DRIVERSTATE_BACKGROUND_E        = 2,
    EA_PRV_DRIVERSTATE_IDLE_E              = 3,
    EA_PRV_DRIVERSTATE_READ_E              = 4,
    EA_PRV_DRIVERSTATE_WRITE_E             = 5,
    EA_PRV_DRIVERSTATE_INVALIDATEBLOCK_E   = 6,
    EA_PRV_DRIVERSTATE_BLOCKMAINTENANCE_E  = 7
} rba_EaFs2_Prv_DriverState_ten;

typedef struct
{
    rba_EaFs2_Prv_Block_tst         block_st;            /* FAT block structure                                  */
    rba_EaFs2_Prv_metaBlock_tst     metaBlock_st;        /* structure to hold metadata entry                     */
    rba_EaFs2_Prv_Read_ten          state_en;            /* current state of state machine                       */
    Ea_Prv_Order_tst        const * order_pcst;          /* Order structure as updated from user request         */
    uint32                          calculatedCrc_u32;   /* CRC of the data calculated from EEPROM               */
    uint32                          addr_u32;            /* Physical address of the block in EEPROM              */
#if (EA_WRITE_SPREAD != FALSE)
    uint32                          blockWriteCounter_u32[2]; /* Number of times write spread block is written. used only when write spread is enabled */
    uint32                          blockWriteCounterT1_u32;/*Number of times write spread block is written at previous location. used only when write spread is enabled*/
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    uint16                          fatIndex_u16;        /* Index position of the FAT entry of the block         */
    uint16                          dataBlockLength_u16; /* Length of the block in EEPROM (incl metadata)        */
    uint8                           currCopy_u8;         /* Inidcates the current working copy for a SL2 block   */
    uint8                           retry_u8;            /* Number of retires for a read operation               */
#if (EA_WRITE_SPREAD != FALSE)
    uint8                           CurrentLocation_u8;    /* Indicate the current Location to clear all counters for write spread blocks*/
    boolean                         CurrentLocationFound_b;
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    boolean                         entry_b;             /* Entry indicator for the state machine                */
}rba_EaFs2_Prv_Read_tst;

typedef struct
{
    rba_EaFs2_Prv_Block_tst         block_st;              /* FAT block structure                                  */
    rba_EaFs2_Prv_metaBlock_tst     metaBlock_st;          /* structure to hold metadata entry                     */
    rba_EaFs2_Prv_Write_ten         state_en;              /* current state of state machine                       */
    Ea_Prv_Order_tst        const * order_pcst;            /* Order structure as updated from user request         */
    uint32                          addr_u32;              /* Physical address of the block in EEPROM              */
    uint32                          calculatedCrc_u32;     /* CRC of the data calculated from user buffer          */
#if (EA_WRITE_SPREAD != FALSE)
    uint32                          blockWriteCounter_u32[2]; /* Number of times write spread block is written. used only when write spread is enabled */
    uint32                          blockWriteCounterT1_u32;/*Number of times write spread block is written at previous location. used only when write spread is enabled*/
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    uint16                          crcCalcLengthDone_u16; /* Length remaining to calculate CRC                    */
    uint16                          dataBlockLength_u16;   /* Length of the block in EEPROM (incl metadata)        */
    uint16                          fatIndex_u16;          /* Index position of the FAT entry of the block         */
    uint8                           currCopy_u8;           /* Inidcates the current working copy for a SL2 block   */
    uint8                           retry_u8;              /* Number of retires for a write/invalidate operation   */
#if (EA_WRITE_SPREAD != FALSE)
    uint8                           CurrentLocation_u8;    /* Indicate the current Location to clear all counters for write spread blocks*/
    uint8                           currCopyCounter_u8;    /* Inidcates the current copy of counter of the write spread block*/
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    boolean                         doInvalidate_b;        /* Flag to indicate invalidate/write operation          */
    boolean                         needFatWrite_b;        /* Flag to indicate if FAT update is needed             */
    boolean                         entry_b;               /* Entry indicator for the state machine                */
#if (EA_WRITE_SPREAD != FALSE)
    boolean                         locationChange_b;      /* Flag to indicate if location change is needed. used only when write spread is enabled*/
    boolean                         CurrentLocationFound_b;
#endif /*(EA_WRITE_SPREAD != FALSE)*/
}rba_EaFs2_Prv_Write_tst;

typedef struct
{
    rba_EaFs2_Prv_Block_tst         block_st;            /* FAT block structure                                  */
    rba_EaFs2_Prv_metaBlock_tst     metaBlock_st;        /* structure to hold metadata entry                     */
    rba_EaFs2_Prv_Bgo_ten           state_en;            /* current state of state machine                       */
    uint32                          addr_u32;            /* Physical address of the block in EEPROM              */
#if (EA_WRITE_SPREAD != FALSE)
    uint32                          blockWriteCounter_u32[2]; /* Number of times write spread block is written. used only when write spread is enabled */
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    uint16                          fatIndex_u16;        /* Index position of the FAT entry of the block         */
    uint16                          dataBlockLength_u16; /* Length of the block in EEPROM (incl metadata)        */
    uint8                           currCopy_u8;         /* Inidcates the current working copy for a SL2 block   */
#if (EA_WRITE_SPREAD != FALSE)
    uint8                           CurrentLocation_u8;    /* Indicate the current Location to clear all counters for write spread blocks*/
#endif /*(EA_WRITE_SPREAD != FALSE)*/
boolean                         entry_b;             /* Entry indicator for the state machine                */
}rba_EaFs2_Prv_Bgo_tst;

typedef struct
{
    rba_EaFs2_Prv_FatRepair_ten   state_en;
    uint8    currMarkerCopy_u8;                           /* Indicates current copy of layout marker wrote and compared*/
    boolean  entry_b;
    uint8    corruptedHeader;                             /* variable indicates current copy is corrupted              */
    uint32   iterator;                                    /* address Fat is calculated by index                        */
    uint16   indexFat;                                    /* index Fat which is processing                             */
    uint32   fatAdrRead_u32;                              /* variable store address of valid header copy               */
    uint32   fatAdrWrite_u32;                             /* variable store address of invalid header copy             */
    boolean  isFatRepair;                                 /* variable indicates the process repair done or not         */
    boolean  reCheck;                                     /* after read write, need recheck all fat one again          */
}rba_EaFs2_Prv_FatRepair_tst;


#if(FALSE == EA_RB_ACT_UNKNOWN_BLOCK_MIG)
typedef struct
{
    rba_EaFs2_Prv_TempMarker_ten state_en;                  /* Stores state of temp marker state machine*/
    boolean wrfinish_b;                                     /* Will be set to values corresponding to presence of temp marker*/
    boolean entry_b;
}rba_EaFs2_Prv_TempMarker_tst;
#endif

typedef struct
{
    uint32 address_u32;                                     /* Address pointing to the Metadata of the block, Points to Copy 1 and not Copy 2*/
    uint16 fatIndex_u16;                                    /* Index pointing to the First FAT entry */
    uint16 status_u16;                                      /* Holds the information, if the block is reapired or not */
    boolean isSecurityLevelChanged_b;
    boolean isSurvivalFlagChanged_b;
}rba_EaFs2_Prv_RamTable_tst;


#if (EA_WRITE_SPREAD != FALSE)
typedef struct
{
    uint32 currAddress_u32;     /* Address pointing to the Metadata of the block, Points to Copy 1 and not Copy 2*/
    uint32 currCounter_u32;     /* Current counter value of the block in the current location*/
    uint16 blockNumber_u16;     /* Block number is known, else 0xFFFF     */
}rba_EaFs2_Prv_WriteSpreadRamTable_tst;
#endif /*(EA_WRITE_SPREAD != FALSE)*/

typedef struct
{
    void (*Ea_Rb_ErrorHandling_CallBack_pfct)(uint8 ErrorId);   /* Pointer to error handling callback function */
} Ea_Prv_Common_tst;

typedef struct
{
    uint16 Ea_BlkPersistentID_u16; /* Presistent ID of the block from configuration */
    uint16 Ea_BlkLength_u16;       /* Block length from configuration               */
    uint16 Ea_BlkStatus_u16;       /* Block status                                  */
#if (EA_WRITE_SPREAD != FALSE)
    uint32 Ea_NumberOfWriteCycles;   /* Number of write cycles required for the block */
#endif /*(EA_WRITE_SPREAD != FALSE)*/
}Ea_BlockProperties_tst;

#if(FALSE != EA_PRV_DEBUGGING)
typedef struct
{
    rba_EaFs2_Prv_Write_tst * write_pst;
    rba_EaFs2_Prv_Read_tst  * read_pst;
    rba_EaFs2_Prv_Bgo_tst   * bgo_pst;
    rba_EaFs2_Prv_time_tst    mainTime_st;
    rba_EaFs2_Prv_time_tst    readTime_st;
    rba_EaFs2_Prv_time_tst    writeTime_st;
    rba_EaFs2_Prv_time_tst    initTime_st;
}rba_EaFs2_Debug_tst;
#endif
/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#define EA_START_SEC_CONST_UNSPECIFIED
#include "Ea_MemMap.h"

extern const Ea_Prv_Common_tst             Ea_Prv_Common_cst;
extern const Std_VersionInfoType        Ea_VersionInfo;
extern Ea_BlockProperties_tst const     Ea_BlockProperties_st[EA_NUM_BLOCKS];

#define EA_STOP_SEC_CONST_UNSPECIFIED
#include "Ea_MemMap.h"

/***********************************/

#define EA_START_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"

extern rba_EaFs2_Prv_DriverState_ten    rba_EaFs2_Prv_DriverState_en;

#define EA_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"

/***********************************/

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

extern rba_EaFs2_Prv_RamTable_tst       rba_EaFs2_Prv_RamTable_st[EA_NUM_BLOCKS];        /* Index of the RamTable will be the blockNumber from Cfg*/
extern MemIf_Rb_DetailedBlkInfo_ten     rba_EaFs2_Prv_ReadJobResultDetailed_aen[EA_NUM_BLOCKS];  /* array contain detailed block infor */
#if (EA_WRITE_SPREAD != FALSE)
extern rba_EaFs2_Prv_WriteSpreadRamTable_tst rba_EaFs2_Prv_WriteSpreadRamTable_st[EA_WRITE_SPREADBLOCKS];
#endif /*(EA_WRITE_SPREAD != FALSE)*/
extern rba_EaFs2_Prv_Bgo_tst            rba_EaFs2_Prv_Bgo_st;
extern rba_EaFs2_Prv_Read_tst           rba_EaFs2_Prv_Read_st;
extern rba_EaFs2_Prv_Write_tst          rba_EaFs2_Prv_Write_st;
extern rba_EaFs2_Prv_FatRepair_tst      rba_EaFs2_Prv_FatRepair_st;



#if(FALSE != EA_PRV_DEBUGGING)
extern rba_EaFs2_Debug_tst rba_EaFs2_Debug_st;
#endif

#if(FALSE == EA_RB_ACT_UNKNOWN_BLOCK_MIG)
extern rba_EaFs2_Prv_TempMarker_tst rba_EaFs2_Prv_TempMarker_st;
#endif

#if(FALSE != EA_RB_ENTER_STOP_MODE)
extern rba_EaFs2_Prv_StopMode_ten  rba_EaFs2_Prv_StopMode_en;
#endif

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define EA_START_SEC_CODE
#include "Ea_MemMap.h"
/**
 * \brief   Check whether a state of a state machine is entered the 1. time or the >1. time
 *
 * \param   entry_pb    Pointer to the entry_b variable of the state machine
 */
LOCAL_INLINE boolean rba_EaFs2_Prv_LibEnter(boolean * entry_pb)
{
    boolean result_b = (boolean)FALSE;

    /* If not already entered then set the entry_b to FALSE and return TRUE (= state is entered) */
    if(*entry_pb == (boolean)TRUE)
    {
        result_b    = (boolean)TRUE;
        *entry_pb   = (boolean)FALSE;
    }

    return(result_b);
}

LOCAL_INLINE void rba_EaFs2_Prv_Memcpy(uint8 const * src_pcu8,uint8 *dst_pu8,uint32 const length_u32)
{
    uint32 i_u32 = 0;

    for(i_u32 = 0;i_u32 < length_u32;i_u32++)
    {
        dst_pu8[i_u32]= src_pcu8[i_u32];
    }
}

LOCAL_INLINE MemIf_JobResultType rba_EaFs2_Prv_LibResetRetry(uint8 * retry_u8)
{
    *(retry_u8) = 0U;
    return MEMIF_JOB_FAILED;
}

/* Returns the length taken by data in EEPROM including ecc alignment*/
LOCAL_INLINE uint16 rba_EaFs2_Prv_DataLengthInMedium(uint16 blockNumber_u16)
{
    return RBA_EAFS2_PRV_DATLENGTH(Ea_BlockProperties_st[blockNumber_u16].Ea_BlkLength_u16);
}

/* Returns the length taken by a block in EEPROM including metadata size and ecc alignment*/
LOCAL_INLINE uint16 rba_EaFs2_Prv_BlkLengthInMedium(uint16 blockNumber_u16)
{
    return (rba_EaFs2_Prv_DataLengthInMedium(blockNumber_u16) + (uint16)sizeof(rba_EaFs2_Prv_metaDataEntry_tst) );
}
/*
 * currWip_u8 : value of part 2 WIP read from EEP
 * If the second part of WIP is 55 - new pattern should be AAAA
 * If the second part of WIP is not 55 - new pattern should be 5555
 *
 */
LOCAL_INLINE uint16 rba_EaFs2_Prv_GetWipToUse(uint8 currWip_u8)
{
    uint16 wip_u16 = 0x5555;

    if(currWip_u8 == 0x55)
    {
        wip_u16 = 0xAAAA;
    }

    return wip_u16;
}

/* return TRUE if repair is completed in the current power cycle */
LOCAL_INLINE boolean rba_EaFs2_Prv_IsRepairDone(uint16 blockNumber_u16)
{
    boolean result_b = (boolean)FALSE;

    if(EA_NUM_BLOCKS > blockNumber_u16)
    {
        if( 0uL != (rba_EaFs2_Prv_RamTable_st[blockNumber_u16].status_u16 & RBA_EAFS2_PRV_REPAIR_DONE) )
        {
            result_b = (boolean)TRUE;
        }
    }

    return result_b;
}

LOCAL_INLINE void rba_EaFs2_Prv_UpdateRepairStatus(uint16 blockNumber_u16)
{
    rba_EaFs2_Prv_RamTable_st[blockNumber_u16].status_u16 |= 0x0001u;
}

LOCAL_INLINE void rba_EaFs2_Prv_ClearRepairStatus(uint16 blockNumber_u16)
{
    rba_EaFs2_Prv_RamTable_st[blockNumber_u16].status_u16 &= 0xFFFEu;
}

/* This API should not be used for unknown blocks, i.e. during defrag*/
LOCAL_INLINE uint16 rba_EaFs2_Prv_GetPersistentID(uint16 blocknumber_u16)
{
    return Ea_BlockProperties_st[blocknumber_u16].Ea_BlkPersistentID_u16;
}

LOCAL_INLINE uint32 rba_EaFs2_Prv_GetFreeSpace(void)
{
    return (
            rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 -
            rba_EaFs2_Prv_FatIndex2Address(rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16) -
            RBA_EAFS2_PRV_MIN_GAP_IN_EEPROM
    );
}

/* Returns TRUE if the block is configured as a double storage block*/
LOCAL_INLINE boolean rba_EaFs2_Prv_IsBlockDblStorage(uint16 blockNumber_u16)
{
    boolean result_b = (boolean)FALSE;

    if(EA_NUM_BLOCKS > blockNumber_u16)
    {
        if(0u != (Ea_BlockProperties_st[blockNumber_u16].Ea_BlkStatus_u16 & RBA_EAFS2_PRV_SECURITY_LEVEL_FLAG) )
        {
            result_b = (boolean)TRUE;
        }
    }
    return result_b;
}

LOCAL_INLINE boolean rba_EaFs2_Prv_IsBlockSurvival(uint16 blockNumber_u16)
{
    boolean result_b = (boolean)FALSE;

    if(EA_NUM_BLOCKS > blockNumber_u16)
    {
        if(0u != (Ea_BlockProperties_st[blockNumber_u16].Ea_BlkStatus_u16 & RBA_EAFS2_PRV_SURVIVAL_FLAG) )
        {
            result_b = (boolean)TRUE;
        }
    }
    return result_b;
}

#if (EA_WRITE_SPREAD != FALSE)
LOCAL_INLINE boolean rba_EaFs2_Prv_IsBlockWriteSpread(uint16 blockNumber_u16)
{
    boolean result_b = (boolean)FALSE;

    if(EA_NUM_BLOCKS > blockNumber_u16)
    {
        if(0u != (Ea_BlockProperties_st[blockNumber_u16].Ea_BlkStatus_u16 & RBA_EAFS2_PRV_WRITESPREAD_FLAG) )
        {
            result_b = (boolean)TRUE;
        }
    }
    return result_b;
}
LOCAL_INLINE void rba_EaFs2_Prv_WriteSpreadRamTableInit(void)
{
    uint16 blockNumber_u16 = 0;
    uint16 WriteSpreadRamTableIndex =0;
    for(blockNumber_u16 = 0; blockNumber_u16 < EA_NUM_BLOCKS; blockNumber_u16++)
    {
        if (rba_EaFs2_Prv_IsBlockWriteSpread (blockNumber_u16) != (boolean)FALSE)
        {
            rba_EaFs2_Prv_WriteSpreadRamTable_st[WriteSpreadRamTableIndex].blockNumber_u16 = blockNumber_u16;
            WriteSpreadRamTableIndex ++;
        }
        if(WriteSpreadRamTableIndex >= EA_WRITE_SPREADBLOCKS)
        {
            break;
        }
    }
}
LOCAL_INLINE uint16 rba_EaFs2_Prv_WriteSpreadRamTableIndex(uint16 blockNumber_u16)
{
    uint16 blockNr_u16 = 0;
    uint16 ret_blockNr_u16 = 0;

    for (blockNr_u16=0; blockNr_u16 < EA_WRITE_SPREADBLOCKS; blockNr_u16++)
    {
        if (rba_EaFs2_Prv_WriteSpreadRamTable_st[blockNr_u16].blockNumber_u16 == blockNumber_u16)
        {
            ret_blockNr_u16 = blockNr_u16;
        }
    }
    return ret_blockNr_u16;
}
#endif /*(EA_WRITE_SPREAD != FALSE)*/

LOCAL_INLINE void rba_EaFs2_Prv_RestOrder(void)
{
    Ea_Prv_Order_st.OrderType_en    = EA_RB_NO_ORDER_E;
    Ea_Prv_Order_st.bfr_pu8         = NULL_PTR;
    Ea_Prv_Order_st.bfr_pcu8        = NULL_PTR;
    Ea_Prv_Order_st.blockNumber_u16 = 0u;
    Ea_Prv_Order_st.length_u16      = 0u;
    Ea_Prv_Order_st.offset_u16      = 0u;
}

/**
 * \brief   Simple maximum chooser.
 *
 * \param   a_u32   First value
 * \param   b_u32   Second value
 *
 * \return  The biggest of both values
 */
LOCAL_INLINE uint32 rba_EaFs2_Prv_LibMax(uint32 a_u32, uint32 b_u32)
{
    uint32 result_u32 = a_u32;
    if(a_u32 < b_u32)
    {
        result_u32 = b_u32;
    }
    return(result_u32);
}

/**
 * \brief   Simple minimum chooser.
 *
 * \param   a_u32   First value
 * \param   b_u32   Second value
 *
 * \return  The smallest of both values
 */
LOCAL_INLINE uint32 rba_EaFs2_Prv_LibMin(uint32 a_u32, uint32 b_u32)
{
    uint32 result_u32 = a_u32;
    if(a_u32 > b_u32)
    {
        result_u32 = b_u32;
    }
    return(result_u32);
}

extern Std_ReturnType rba_EaFs2_Prv_CheckBlkLength        (uint8 ApiId_u8,uint16 blkNum_u16, uint16 blkoffset_u16, uint16 blklength_u16);
extern Std_ReturnType Ea_Rb_CheckBusy                    (uint8 ApiId_u8);
extern Std_ReturnType Ea_Rb_CheckInit                    (uint8 ApiId_u8);
extern Std_ReturnType Ea_Rb_CheckBlkNr                    (uint8 ApiId_u8, uint16 BlockNumber_u16);
extern Std_ReturnType Ea_Rb_CheckDataPtr                (uint8 ApiId_u8,uint8 const * DataBufferPtr);
extern MemIf_Rb_DetailedBlkInfo_ten rba_EaFs2_Prv_GetDetailedBlkInfo(uint16 blockNumber_u16);
extern void Ea_Rb_CheckDefragErr                        (uint8 ApiId_u8, uint8 ErrTyp_u8);

extern MemIf_JobResultType rba_EaFs2_Prv_FatAllocDo(void);

#if (EA_WRITE_SPREAD != FALSE)
extern uint8 rba_EaFs2_Prv_WriteSpreadRequired(uint16 blockNumber_u16);
#endif /*(EA_WRITE_SPREAD != FALSE)*/

extern rba_EaFs2_Prv_Block_tst rba_EaFs2_Prv_BlockFromOrder   (Ea_Prv_Order_tst const *order_pcst);
extern uint16  rba_EaFs2_Prv_GetBlockID                    (uint16 persistentID_u16);
extern boolean rba_EaFs2_Prv_isBlockneedtobePreserved_b    (rba_EaFs2_Prv_FatEntry_tst * fatentry_pst,boolean * isValid_b);

extern void rba_EaFs2_Prv_PrepFatInfo(void);
extern void rba_EaFs2_Write(boolean isInvalidate_b);
extern void rba_EaFs2_Read(void);

#if (EA_RB_ENTER_STOP_MODE != FALSE)
extern void rba_EaFs2_Prv_StopModeActivate(void);
#endif

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */

#endif /* RBA_EAFS2_PRV_H */
