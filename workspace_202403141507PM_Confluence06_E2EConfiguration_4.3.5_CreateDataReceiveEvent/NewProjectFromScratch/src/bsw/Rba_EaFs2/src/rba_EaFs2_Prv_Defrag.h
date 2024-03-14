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

#ifndef RBA_EAFS2_PRV_DEFRAG_H
#define RBA_EAFS2_PRV_DEFRAG_H

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#include "Std_Types.h"
#include "rba_EaFs2_Prv.h"
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#define RBA_EAFS2_PRV_DEFRAG_PREERASE_ONGOING   0x00u
#define RBA_EAFS2_PRV_DEFRAG_DATAMOVE_ONGOING   0x01u
#define RBA_EAFS2_PRV_DEFRAG_FATWRITE_ONGOING   0x02u
#define RBA_EAFS2_PRV_DEFRAG_FATCLEAR_ONGOING   0x03u

#define RBA_EAFS2_PRV_DEFRAG_STATUS_CLIP_VALID      0x0001u
#define RBA_EAFS2_PRV_DEFRAG_STATUS_RESUME_VALID    0x0002u


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
typedef enum
{
    RBA_EAFS2_PRV_DEFRAG_INIT_E=0 ,
    RBA_EAFS2_PRV_DEFRAG_FATREADUP_E,
    RBA_EAFS2_PRV_DEFRAG_FATREADPRV_E,
    RBA_EAFS2_PRV_DEFRAG_FATREADNXT_E,
    RBA_EAFS2_PRV_DEFRAG_PREERASE_E,
    RBA_EAFS2_PRV_DEFRAG_MOVEINIT_E,
    RBA_EAFS2_PRV_DEFRAG_DATAREAD_E,
    RBA_EAFS2_PRV_DEFRAG_DATAWRITE_E,
    RBA_EAFS2_PRV_DEFRAG_DATAVERIFY_E,
    RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E,
    RBA_EAFS2_PRV_DEFRAG_FATOVERWRITE_E,
    RBA_EAFS2_PRV_DEFRAG_FATCLEAR_E,
    RBA_EAFS2_PRV_DEFRAG_UPDATEGAP_E,
    RBA_EAFS2_PRV_DEFRAG_NXTSTEPS_E,
    RBA_EAFS2_PRV_DEFRAG_FINAL_E
}rba_EaFs2_Prv_DefragState_ten;


typedef enum
{
    RBA_EAFS2_PRV_DEFRAGSTATUS_ERASEFAT_E=0,
    RBA_EAFS2_PRV_DEFRAGSTATUS_READSOURCE_E,
    RBA_EAFS2_PRV_DEFRAGSTATUS_READCLIP_E,
    RBA_EAFS2_PRV_DEFRAGSTATUS_WRITEFAT_E,
    RBA_EAFS2_PRV_DEFRAGSTATUS_CLEARFAT_E,
    RBA_EAFS2_PRV_DEFRAGSTATUS_FINAL_E
}rba_EaFs2_Prv_DefragStatus_ten;

#if((EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE) && (EA_RB_ENTER_STOP_MODE != FALSE))
typedef enum
{
    RBA_EAFS2_PRV_UNALLOCBLKSPACE_CALC_E = 0,
    RBA_EAFS2_PRV_TRIGGERFORCEDDEFRAG_E
}rba_EaFs2_Prv_UnAllocBlkSpace_ten;

typedef struct
{
    rba_EaFs2_Prv_UnAllocBlkSpace_ten unAllocBlkSpace_en;       /* Enum to handle state machine on UnAllocblockSpace calc */
    uint32 unAllocblkSpace_u32;                                 /* Variable to hold the unallocated block Size */
    uint16 blkChecklimit_u16;                                   /* Variable to hold the blk number checked per cycle */
}rba_EaFs2_Prv_UnAllocBlkSpace_tst;
#endif

typedef struct
{
    rba_EaFs2_Prv_DefragState_ten   state_en;                   /* State of the Defrag state machine                    */
    rba_EaFs2_Prv_Block_tst         previousValidBlock_st;      /* Adjacent FAT details just above the gap              */
    rba_EaFs2_Prv_Block_tst         nextValidBlock_st;          /* FAT details just below the gap; Block under movement */
    rba_EaFs2_Prv_FatEntry_tst      fatUnderRead_st;
    rba_EaFs2_Prv_Block_tst *       blockUnderRead_pst;         /* ptr : points to the current read FAT                 */
    rba_EaFs2_Prv_Block_tst         blockToUpdate_st;           /* FAT Block used to erase/overwrite/clear FAT entries  */
    uint32                          readAddress_u32;            /* address to read from                                 */
    uint32                          writeAddress_u32;           /* address to write to                                  */
    uint16                          lengthToMove_u16;           /* length to move                                       */
    uint16                          gapLength_u16;              /* length of the working gap                            */
    uint16                          srcLength_u16;              /* block length in medium of the current moving block   */
    uint16                          workingGapIdx_u16;          /* Fat Index of working gap; destination index          */
    uint16                          workingNxtValidIdx_u16;     /* Fat Index of current moving block; source index      */
    uint16                          firstGapIdx_u16;            /* Holds the index of the first gap in the layout       */
    uint16                          lastGapIdx_u16;             /* Holds the index of last gap in the layout            */
    uint8                           status_u8;                  /* Holds the current status of defrag;0-Erase Fat;1-Data Move;2-Write Fat;3-Clear Fat */
    boolean                         isClipUsed_b;               /* TRUE: If Clip has valid data                         */
    boolean                         isDataWritteninResume_b;    /* TRUE: If Resume address holds valid data             */
    boolean                         isPwrInterrupted_b;         /* Flag indicates if the defrag is power interrupted    */
    boolean                         isDefragHdrRepairNeeded_b;  /* Flag to indicate if defrag header repair is required */
    boolean                         readFatUpGapFound_b;
    boolean                         isForcedDefrag_b;           /* Indicates if Defrag is triggerd by Write Request processing */
    boolean                         hasLastGapInfo_b;           /* Indicates if Last Gap is known. FALSE: Default value */
    boolean                         entry_b;                    /* State to be entered or is already entered            */
}rba_EaFs2_Prv_DefragInfo_tst;

/*
 * Structure to hold the location of the block under movement
 */
typedef struct
{
    uint32 addr_u32;    /* address of the data */
    uint16 len_u16;     /* length of data in the particular address */
}rba_EaFs2_Prv_Split_tst;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
/* The Split structure will have 3 indices
 * 0 : Corresponds to Source information (Start Addess and Length of valid data)
 * 1 : Corresponds to Clip information   (Address of Clip and Length of valid data in Clip)
 * 2 : Corresponds to Resume information (resume address and length of valid data in resume)
 * */
extern rba_EaFs2_Prv_Split_tst             rba_EaFs2_Prv_Split_st[3];
extern rba_EaFs2_Prv_DefragInfo_tst     rba_EaFs2_Prv_DefragInfo_st;

#if((EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE) && (EA_RB_ENTER_STOP_MODE != FALSE))
extern rba_EaFs2_Prv_UnAllocBlkSpace_tst rba_EaFs2_Prv_UnAllocBlkSpace_st;      /* Structure to handle Unallocblkspce calculation */
#endif

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)

#define EA_START_SEC_VAR_CLEARED_8
#include "Ea_MemMap.h"

extern uint8    Ea_Rb_MigrationStaus_u8;                    /* Global variable to hold the Defragmentation status */

#define EA_STOP_SEC_VAR_CLEARED_8
#include "Ea_MemMap.h"

#endif /* (EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE) */
/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

/* Returns the length taken by data in EEPROM including ecc alignment*/
LOCAL_INLINE uint16 rba_EaFs2_Prv_SizeInMedium(rba_EaFs2_Prv_Block_tst const * block_pcst)
{
    uint16 return_u16;

    return_u16 = (((RBA_EAFS2_PRV_DATLENGTH(block_pcst->length_u16))+ (uint16)sizeof(rba_EaFs2_Prv_metaDataEntry_tst)));

    if((boolean)FALSE != block_pcst->doublestorage_b)
    {
        return_u16 = return_u16*2u;
    }

    return return_u16;
}

extern void rba_EaFs2_Prv_UpdateSplitAccess            (rba_EaFs2_Prv_Block_tst const * block_pcst);

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
extern MemIf_JobResultType rba_EaFs2_Prv_DefragDo    (void);
#if(EA_RB_ENTER_STOP_MODE != FALSE)
extern void rba_EaFs2_Prv_UnallocBlkSpaceCheck(void);
#endif /* EA_RB_ENTER_STOP_MODE */
#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG */

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */

#endif /* RBA_EAFS2_PRV_DEFRAG_H */
