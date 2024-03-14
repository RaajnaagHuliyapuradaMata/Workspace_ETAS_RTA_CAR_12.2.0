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

#ifndef RBA_EAFS2_PRV_FAT_H
#define RBA_EAFS2_PRV_FAT_H

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

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
#define EA_MARKER_PATTERN                     0xCAFE  /* Start value for CRC-32,CRC-16 operations         */
#define RBA_EAFS2_PRV_FAT_SIZE                     (uint16)(sizeof(rba_EaFs2_Prv_FatEntry_tst))
#define RBA_EAFS2_PRV_FATS_PER_READ             (uint16)((EA_DATA_BUFFER_SIZE/RBA_EAFS2_PRV_FAT_SIZE)* RBA_EAFS2_PRV_FAT_SIZE)
#define RBA_EAFS2_PRV_FAT_ADDR_PATTERN_CLR        0x00FFFFFFuL
#define RBA_EAFS2_PRV_FAT_ADDR_PATTERN_MASK        0xFF000000uL
#define RBA_EAFS2_PRV_FAT_ADDR_PATTERN            0x5A000000uL
#define RBA_EAFS2_PRV_FAT_STATUS_PATTERN_CLR    (uint16)0x00FFu
#define RBA_EAFS2_PRV_FAT_STATUS_PATTERN_MASK    (uint16)0xFF00u
#define RBA_EAFS2_PRV_FAT_STATUS_PATTERN        (uint16)0x5A00u

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

typedef enum
{
    RBA_EAFS2_PRV_DEFRAGBLK_IDLE_E = 0,
    RBA_EAFS2_PRV_DEFRAGBLK_READ_E,
    RBA_EAFS2_PRV_DEFRAGBLK_WRITE_E,
    RBA_EAFS2_PRV_DEFRAGBLK_VERIFY_E,
    RBA_EAFS2_PRV_DEFRAGBLK_FINAL_E
}rba_EaFs2_Prv_DefragBlkState_en;

/* States when writing a FAT entry */
typedef enum
{
    RBA_EAFS2_PRV_FAT_FATWRITE_IDLE_E = 0, /* Nothing to do                                */
    RBA_EAFS2_PRV_FAT_FATWRITE_INIT_E,     /* Set up the FAT entry for writing             */
    RBA_EAFS2_PRV_FAT_FATWRITE_WRITING_E,  /* Waiting for the flash to write the FAT entry */
    RBA_EAFS2_PRV_FAT_FATWRITE_COMPARE_E,  /* Compare the data written in EEPROM           */
    RBA_EAFS2_PRV_FAT_FATWRITE_FINAL_E
} rba_EaFs2_Prv_FatWrite_ten;

/* enums used to represent task performed in background*/
typedef enum
{
    RBA_EAFS2_PRV_BGOTASK_NOOP_E = 0,       /* No operation is performed in this state    */
    RBA_EAFS2_PRV_BGOTASK_IDLE_E,           /* State to trigger selection by BGO selector */
    RBA_EAFS2_PRV_BGOTASK_TEMPMARKER_E,     /* Writing and comparing of temporary marker   */
    RBA_EAFS2_PRV_BGOTASK_BGOALLOC_E,       /* State to do Background FAT allocation      */
    RBA_EAFS2_PRV_BGOTASK_DEFRAG_E,         /* State to do Defragmentation                */
    RBA_EAFS2_PRV_BGOTASK_PROPERTYCHANGE_E, /* State to do Property Change SLx > SLy                */
    RBA_EAFS2_PRV_BGOTASK_FATREPAIR_E       /* State to do FAT repair                     */
}rba_EaFs2_Prv_BgoTask_ten;

typedef struct
{
    rba_EaFs2_Prv_BgoTask_ten mode_en;  /* Mode of the background operatin under progress */
    uint32  currDataPtr_u32;            /* Pointer to the end of data region in the memory */
    uint16  currFatIndex_u16;           /* Index to the Fat entry to add new FAT entry*/
    uint16  totalGaps_u16;              /* Indicates the number of FAT entry gaps available in the medium*/
    uint16  allotedblocks_u16;          /* Number of blocks allocated in the EEPROM */
    uint16  allocIter_u16;              /* Iteration holder for background allocation of blocks */
    uint16  propertyChangeGaps_u16;
    uint16  securityLevelChangedBlocks_u16;
    boolean bgoEnable_b;                /* Flag to enable the background operations after EndInit */
    boolean isPreAllocReq_b;            /* Flag to control the allocation of survival blocks in Init*/
    boolean isInitFailed_b;             /* Flag to indicate intialization status. TRUE: init failed*/
    boolean suspendBgo_b;               /* Flag to suspend the background operations to service user request*/
    boolean unsupportedPropertyChange_b; /* Flag to indicate unsupported block property change */
    boolean updateMigragationMarker_b;  /* Flag to indicate migration marker is not yet written in case of block property change*/
}rba_EaFs2_Prv_FsInfo_tst;

typedef struct
{
    uint32  address_u32;         /* Address of the block in EEPROM         */
    uint16  persistentId_u16;    /* Persistnet ID of the block in EEPROM   */
    uint16  length_u16;          /* Config length of block in EEPROM       */
    uint16  blockNumber_u16;     /* Block number is known, else 0xFFFF     */
    boolean doublestorage_b;     /* Indicate if block is double security   */
    boolean survival_b;          /* Indicate if the block is survival      */
    boolean isInConfig_b;        /* The block is present in current config */
#if (EA_WRITE_SPREAD != FALSE)
    boolean writeSpread_b;      /*Indicate if write spread is required for the block*/
#endif /*(EA_WRITE_SPREAD != FALSE)*/
}rba_EaFs2_Prv_Block_tst;

/*
 * #----------------------------------!!ATTENTION!!------------------------------------#
 * #---------------Do not modify this structure, written in Layout---------------------#
 * #-----------------------------------------------------------------------------------#
 */
typedef struct
{
    uint32 address_u32;         /* Address of the block in EEPROM         */
    uint16 length_u16;          /* Config length of block in EEPROM       */
    uint16 ID_u16;              /* Persistnet ID of the block in EEPROM   */
    uint16 status_u16;          /* Holds the property of the FAT (Survival, Double storage) */
    uint16 crc_u16;             /* CRC of the fat entry   */
}rba_EaFs2_Prv_FatEntry_tst;

/*
 * #----------------------------------!!ATTENTION!!------------------------------------#
 * #---------------Do not modify this structure, written in Layout---------------------#
 * #-----------------------------------------------------------------------------------#
 */
typedef struct
{
    uint32 resumeAddr_u32;
    uint16 srcIdx_u16;
    uint16 destIdx_u16;
    uint16 lengthInSrc_u16;
    uint16 movingId_u16;
    uint16 status_u16;
    uint16 crc_u16;
}rba_EaFs2_Prv_DefragBlk_tst;

typedef struct
{
    rba_EaFs2_Prv_DefragBlkState_en state_en;
    uint8                           copies_u8;
    uint8                           addrOffset_u8;
    boolean                         entry_b;
}rba_EaFs2_Prv_DefragBlkOper_tst;


typedef struct
{
    rba_EaFs2_Prv_FatWrite_ten        state_en;       /* State of defrag state machine           */
    rba_EaFs2_Prv_Block_tst   const * block_pcst;     /* pointer to the FAT block structure      */
    rba_EaFs2_Prv_FatEntry_tst        fatEntry_st;    /* FAT entry as such in EEPROM             */
    uint32                            fatAdr_u32;     /* Address of the FAT entry                */
    uint16                            fatIndex_u16;   /* index position of the FAT               */
    uint8                             retry_u8;       /* Number of retries for FAT write         */
    boolean                           entry_b;        /* indicate entry status for state machine */
}rba_EaFs2_Prv_FatWrite_tst;


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

extern rba_EaFs2_Prv_DefragBlk_tst     rba_EaFs2_Prv_DefragBlk_st;
extern rba_EaFs2_Prv_FsInfo_tst     rba_EaFs2_Prv_FsInfo_st;

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
 * \brief   Clears all the fields in FAT entry structure.
 *
 * \param   fat_pst    pointer to the FAT entry to be cleared
 *
 */
LOCAL_INLINE void rba_EaFs2_Prv_FatBlockClear(rba_EaFs2_Prv_Block_tst * block_pst)
{
    block_pst->address_u32      = 0x00FFFFFFUL;
    block_pst->persistentId_u16 = 0xFFFFu;
    block_pst->length_u16       = 0xFFFFu;
    block_pst->blockNumber_u16  = 0xFFFFu;
    block_pst->doublestorage_b  = (boolean)FALSE;
    block_pst->survival_b       = (boolean)FALSE;
    block_pst->isInConfig_b     = (boolean)FALSE;
}

/**
 * \brief   Calculates the physical address in eeprom from the FAT index.
 *
 * \param   index_u16    fat index to convert into address
 *
 */
LOCAL_INLINE uint32 rba_EaFs2_Prv_FatIndex2Address(uint16 index_u16)
{
    return (EA_EEP_START_OFFSET + (uint32)(index_u16*sizeof(rba_EaFs2_Prv_FatEntry_tst)) );
}

/**
 * \brief   Function to verify if the defrag block has a valid CRC.
 *
 * \param   db_pcst    pointer to the defrag block
 *
 */
LOCAL_INLINE boolean rba_EaFs2_Prv_IsDefragBlkValid(rba_EaFs2_Prv_DefragBlk_tst const * const db_cpcst)
{
    return (db_cpcst->crc_u16 == Crc_CalculateCRC16((uint8 const *)db_cpcst,
            sizeof(rba_EaFs2_Prv_DefragBlk_tst)-sizeof(uint16),
            EA_MARKER_PATTERN,
            (boolean)FALSE) );
}

extern boolean rba_EaFs2_Prv_IsBlockIgnore    (rba_EaFs2_Prv_Block_tst * block_pst);

extern void rba_EaFs2_Prv_Fat2Block            (rba_EaFs2_Prv_Block_tst * const block_cpst,rba_EaFs2_Prv_FatEntry_tst const * fatEntry_pcst);
extern void rba_EaFs2_Prv_ParseReservedArea    (void);
extern void rba_EaFs2_Prv_FatWrite            (rba_EaFs2_Prv_Block_tst const * block_pcst,uint16 fatIndex_u16);
extern void rba_EaFs2_Prv_FatInit            (void);
extern void rba_EaFs2_Prv_PreAllocation        (void);
extern void rba_EaFs2_Prv_defragBlkRead        (void);
extern void rba_EaFs2_Prv_defragBlkWrite    (void);

extern MemIf_JobResultType rba_EaFs2_Prv_FatWriteDo        (void);
extern MemIf_JobResultType rba_EaFs2_Prv_defragBlkOperDo(void);
extern MemIf_JobResultType rba_EaFs2_Prv_readFatUp      (rba_EaFs2_Prv_FatEntry_tst * fatEntry_pst);
extern MemIf_JobResultType rba_EaFs2_Prv_readFatDown    (rba_EaFs2_Prv_FatEntry_tst * fatEntry_pst);
extern MemIf_JobResultType rba_EaFs2_Prv_readFatEEPROM  (uint8 * corruptHeader, uint32 adrFat_u32);
#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif
#endif
