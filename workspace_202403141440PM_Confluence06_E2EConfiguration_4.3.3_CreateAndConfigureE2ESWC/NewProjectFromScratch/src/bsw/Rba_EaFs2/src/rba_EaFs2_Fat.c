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

#include "Ea.h"

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#include "Ea_Prv.h"

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

/* Index of the RamTable will be the blockNumber from Cfg*/
rba_EaFs2_Prv_RamTable_tst rba_EaFs2_Prv_RamTable_st[EA_NUM_BLOCKS];
#if (EA_WRITE_SPREAD != FALSE)
rba_EaFs2_Prv_WriteSpreadRamTable_tst rba_EaFs2_Prv_WriteSpreadRamTable_st[EA_WRITE_SPREADBLOCKS];
#endif /*(EA_WRITE_SPREAD != FALSE)*/
rba_EaFs2_Prv_DefragBlk_tst            rba_EaFs2_Prv_DefragBlk_st;
rba_EaFs2_Prv_FsInfo_tst               rba_EaFs2_Prv_FsInfo_st;
static rba_EaFs2_Prv_FatWrite_tst      rba_EaFs2_Prv_FatWrite_st;
static rba_EaFs2_Prv_DefragBlkOper_tst rba_EaFs2_Prv_DefragBlkOper_st;

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

static MemIf_JobResultType rba_EaFs2_Prv_FatBuild(void);

static void rba_EaFs2_Prv_Block2Fat(rba_EaFs2_Prv_Block_tst const * block_pcst,rba_EaFs2_Prv_FatEntry_tst * fatEntry_pst);
static boolean rba_EaFs2_Prv_IsFatValid(rba_EaFs2_Prv_FatEntry_tst * fatEntry_pst);
static boolean rba_EaFs2_Prv_FatSelector(uint32 index_u32, rba_EaFs2_Prv_FatEntry_tst * fat_st);
static boolean rba_EaFs2_Prv_IsRamTableUpdateNeeded(uint16 blockNumber_u16);

static MemIf_JobResultType rba_EaFs2_Prv_DefragBlkOperRead(rba_EaFs2_Prv_DefragBlkOper_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DefragBlkOperWrite(rba_EaFs2_Prv_DefragBlkOper_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DefragBlkOperVerify(rba_EaFs2_Prv_DefragBlkOper_tst * fsm_pst);

static MemIf_JobResultType rba_EaFs2_Prv_FatWriteInit    (rba_EaFs2_Prv_FatWrite_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatWriteWrite   (rba_EaFs2_Prv_FatWrite_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatWriteCompare (rba_EaFs2_Prv_FatWrite_tst * fsm_pst);
static uint8 rba_EaFs2_Prv_FindHeaderCorrupt(rba_EaFs2_Prv_FatEntry_tst * fms_pst,uint32 adrFat_u32);
static void rba_EaFs2_Prv_RecordBlockPropertyChanges(rba_EaFs2_Prv_Block_tst const * block_pst);

#if(FALSE == EA_RB_ACT_UNKNOWN_BLOCK_MIG)
static void rba_EaFs2_Prv_MarkerRead (void);
#endif


/**
 * \brief    Reads FAT entries from the iterator value to upper limit into eeprom buffer. The number of FAT entries read
 *           depends on the buffer size.  Once the buffer is filled this function picks out one FAT entry at a time and
 *           passes the reference to it.
 *           If an FAT is detected as inconistent based on CRC or other criteria used to match a valid FAT a cleared FAT
 *           is returned. MEMIF_JOB_FAILED is retured only in case of eep faliures
 *
 * \param    fatEntry_pst  pointer to the FAT entry inside eeprom buffer.
 *
 * \return   MEMIF_JOB_OK       - Low level read is completed and buffer has the FAT entry.
 *           MEMIF_JOB_FAILED   - Eep_Read has failed.
 *           MEMIF_JOB_PENDING  - Eep_Read is ongoing.
 *
 *\attention rba_EaFs2_Prv_PrepareEeprom must be called before using this function.
 *
 * \usedIn   Init, Defrag
 */

MemIf_JobResultType rba_EaFs2_Prv_readFatDown(rba_EaFs2_Prv_FatEntry_tst * fatEntry_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    result_en = rba_EaFs2_Prv_EepromLoad(
            rba_EaFs2_Prv_Eeprom_st.iterator_u32,
            (uint16)(2u*sizeof(rba_EaFs2_Prv_FatEntry_tst)),
            RBA_EAFS2_PRV_NXTLOAD_E
    );

    if(MEMIF_JOB_OK == result_en)
    {
        /* Inside 256 byte Ea buffer,get the address location to read next FAT to validate*/
        uint32 index_u32 = rba_EaFs2_Prv_Eeprom_st.iterator_u32 - rba_EaFs2_Prv_Eeprom_st.lowLimit_u32;

        if((boolean)FALSE == rba_EaFs2_Prv_FatSelector(index_u32, fatEntry_pst))
        {
            /* if both copies are corrupted a cleared FAT must be returned */
            fatEntry_pst->address_u32 = 0x00FFFFFFuL;
            fatEntry_pst->ID_u16      = 0xFFFFu;
            fatEntry_pst->crc_u16     = 0xFFFFu;
            fatEntry_pst->length_u16  = 0x0u;
            fatEntry_pst->status_u16  = 0x0000u; /* all config bits are in FALSE state*/
        }

        rba_EaFs2_Prv_Eeprom_st.iterator_u32 += 2u*sizeof(rba_EaFs2_Prv_FatEntry_tst);
    }



    return result_en;
}

/**
 * \brief    Reads FAT entries from the iterator value to upper limit into eeprom buffer. The number of FAT entries read
 *           depends on the buffer size.  Once the buffer is filled this function picks out one FAT entry at a time and
 *           passes the reference to it.
 *
 * \param    fatEntry_pst  pointer to the FAT entry inside eeprom buffer.
 *
 * \return   MEMIF_JOB_OK       - Low level read is completed and buffer has the FAT entry.
 *           MEMIF_JOB_FAILED   - Eep_Read has failed.
 *           MEMIF_JOB_PENDING  - Eep_Read is ongoing.
 *
 *\attention rba_EaFs2_Prv_PrepareEeprom must be called before using this function.
 *
 * \usedIn   Defrag (Forced)
 */

MemIf_JobResultType rba_EaFs2_Prv_readFatUp(rba_EaFs2_Prv_FatEntry_tst * fatEntry_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    result_en = rba_EaFs2_Prv_EepromLoad(
            rba_EaFs2_Prv_Eeprom_st.iterator_u32,
            (uint16)(2u*sizeof(rba_EaFs2_Prv_FatEntry_tst)),
            RBA_EAFS2_PRV_PRELOAD_E
    );

    if(MEMIF_JOB_OK == result_en)
    {
        uint32 index_u32 = rba_EaFs2_Prv_Eeprom_st.iterator_u32 - rba_EaFs2_Prv_Eeprom_st.lowLimit_u32;

        if((boolean)FALSE == rba_EaFs2_Prv_FatSelector(index_u32, fatEntry_pst))
        {
            /* if both copies are corrupted a cleared FAT must be returned */
            fatEntry_pst->address_u32 = 0x00FFFFFFuL;
            fatEntry_pst->ID_u16      = 0xFFFFu;
            fatEntry_pst->crc_u16     = 0xFFFFu;
            fatEntry_pst->length_u16  = 0x0u;
            fatEntry_pst->status_u16  = 0x0000u; /* all config bits are in FALSE state*/
        }

        rba_EaFs2_Prv_Eeprom_st.iterator_u32 -= 2u*sizeof(rba_EaFs2_Prv_FatEntry_tst);
    }

    return result_en;
}

static boolean rba_EaFs2_Prv_FatSelector(uint32 index_u32, rba_EaFs2_Prv_FatEntry_tst * fat_st)
{
    boolean result_b = (boolean)TRUE;

    /*get copy1 from the buffer into the local structure*/
    rba_EaFs2_Prv_Memcpy(
            &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[index_u32],
            (uint8 *)fat_st,
            sizeof(rba_EaFs2_Prv_FatEntry_tst)
    );

    if((boolean)FALSE == rba_EaFs2_Prv_IsFatValid(fat_st))
    {
        /* Copy1 CRC is inconsistent*/

        index_u32 += sizeof(rba_EaFs2_Prv_FatEntry_tst);

        /*get copy2 from the buffer into the local structure*/
        rba_EaFs2_Prv_Memcpy(
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[index_u32],
                (uint8 *)fat_st,
                sizeof(rba_EaFs2_Prv_FatEntry_tst)
        );

        if((boolean)FALSE == rba_EaFs2_Prv_IsFatValid(fat_st))
        {
            /* Copy2 CRC is inconsistent, both copies of fat are not valid*/
            result_b = (boolean)FALSE;
        }
    }

    return result_b;
}
/**
 * \brief    The RAM Table is build up by scanning all the FAT entries from the start of FAT.
 *
 * \return   MEMIF_JOB_OK       - RAM Table formation is complete.
 *           MEMIF_JOB_FAILED   - Eep failure.
 *
 */

void rba_EaFs2_Prv_FatInit(void)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    rba_EaFs2_Prv_SetIterator(EA_EEP_START_OFFSET);
    rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32   = EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE;
    rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16  = 0u;

    /* RAM Table is cleared on reset*/

    result_en = rba_EaFs2_Prv_FatBuild();

    if(MEMIF_JOB_FAILED == result_en)
    {
        rba_EaFs2_Prv_FsInfo_st.isInitFailed_b = (boolean)TRUE;
    }
}

/**
 * \brief    FAT entries are read from the start of FAT Area till 4 succesive entries are found to be invalid.
 *           once a valid FAT entry is found the corresponding RAM Table index is updated. Once the scan is complete
 *           the current data pointer will point to the start of the last written block and the current fat index will
 *           point to the next free FAT entry slot.
 *
 *
 * \return   MEMIF_JOB_OK       - FAT scan is completed.
 *           MEMIF_JOB_FAILED   - Eep_Read has failed.
 *
 */

static MemIf_JobResultType rba_EaFs2_Prv_FatBuild(void)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    uint8 fatEndCtr_u8 = 0u;
    rba_EaFs2_Prv_FatEntry_tst fe_st;
    rba_EaFs2_Prv_Block_tst block_st;
    boolean prevFatValid_b = (boolean)TRUE; /* flag used to find first gap index*/


    rba_EaFs2_Prv_PrepareEeprom();

    do
    {
        result_en = rba_EaFs2_Prv_readFatDown(&fe_st);

        if(result_en == MEMIF_JOB_OK)
        {
            /*FAT entry is valid*/
            rba_EaFs2_Prv_Fat2Block(&block_st, &fe_st);

            if(block_st.persistentId_u16 == rba_EaFs2_Prv_DefragBlk_st.movingId_u16)
            {
                rba_EaFs2_Prv_UpdateSplitAccess(&block_st);
            }

            if((boolean)FALSE == rba_EaFs2_Prv_IsBlockIgnore(&block_st))
            {
                /* Persistent ID is present in the current cfg*/
                fatEndCtr_u8=0;
                rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 = block_st.address_u32;

                if((boolean)FALSE != rba_EaFs2_Prv_IsRamTableUpdateNeeded(block_st.blockNumber_u16))
                {
                    /* ID not alloted yet to ram table, Valid ID take to ram table*/

                    /* Flag to decide if pre-allocation is required or not */
                    rba_EaFs2_Prv_FsInfo_st.isPreAllocReq_b = (
                            (rba_EaFs2_Prv_FsInfo_st.isPreAllocReq_b && block_st.survival_b)
                    );

                    rba_EaFs2_Prv_FsInfo_st.allotedblocks_u16++;
                    rba_EaFs2_Prv_RamTable_st[block_st.blockNumber_u16].fatIndex_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;
                    rba_EaFs2_Prv_RamTable_st[block_st.blockNumber_u16].address_u32  = block_st.address_u32;

                    rba_EaFs2_Prv_RecordBlockPropertyChanges(&block_st);

                }
                else
                {
                    rba_EaFs2_Prv_FsInfo_st.totalGaps_u16++;

                    if((boolean)FALSE != prevFatValid_b)
                    {
                        prevFatValid_b = (boolean)FALSE;
                        rba_EaFs2_Prv_DefragInfo_st.firstGapIdx_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;
                    }
                }
            }
            else
            {

                /* FAT entry is ignored, i.e.
                 * a) its not in current config, check to find if its a survival block
                 * b) there is a mismatch wrt config
                 * c) unwritten area is read*/
                if((boolean)FALSE == block_st.survival_b)
                {
                    /* Block is deleted and its not a survival block, take slot to gap table Add logic to find recoverable space */
                    rba_EaFs2_Prv_FsInfo_st.totalGaps_u16++;

                    if((boolean)FALSE != prevFatValid_b)
                    {
                        prevFatValid_b = (boolean)FALSE;
                        rba_EaFs2_Prv_DefragInfo_st.firstGapIdx_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;
                    }
                }
                else
                {
                    /* unknown block is survival, must be preserved*/
                    rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 = block_st.address_u32;
                }

                /* If both the copies of FAT are invalid, length is set as 0 by fat selector function */
                if(0u == block_st.length_u16)
                {
                    fatEndCtr_u8++;
                }
                else
                {
                    fatEndCtr_u8 = 0;
                }
            }
        }

        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 += 2u;

    }while( (MEMIF_JOB_FAILED != result_en) &&
            (2u > fatEndCtr_u8)
    );

    if(MEMIF_JOB_FAILED != result_en)
    {
        /* Decrement the FAT Index and total gaps only if the fat build is successful*/
        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 -= 4u;
        rba_EaFs2_Prv_FsInfo_st.totalGaps_u16-=2u;
    }
    else
    {
        /* If Fat build failed, prevent preallocation and total gaps should not be decremented */
        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 -= 2u;
        rba_EaFs2_Prv_FsInfo_st.isPreAllocReq_b = (boolean)FALSE;
    }

    return result_en;
}

/**
 * \brief    Check if block property is changed to survival.
 *
 *
 * \return   TRUE  - Block is changed to survival block.
 *           FALSE - No change in survival flag.
 *
 */
static void rba_EaFs2_Prv_RecordBlockPropertyChanges(rba_EaFs2_Prv_Block_tst const * block_pst)
{
    rba_EaFs2_Prv_BlockPropertyInfor_tst * fsm_pst = &rba_EaFs2_Prv_BlockPropertyInfo_st;

    if( rba_EaFs2_Prv_IsBlockSurvival(block_pst->blockNumber_u16) != block_pst->survival_b)
    {
        /* Block identified as change in survival property */
        if(block_pst->survival_b != (boolean)FALSE )
        {
            /* retain as survival block */
            rba_EaFs2_Prv_RamTable_st[block_pst->blockNumber_u16].isSurvivalFlagChanged_b = (boolean)TRUE;
        }
        else
        {
            /* update the as survival block */
            fsm_pst->blockToUpdate_st = *block_pst;
            if(rba_EaFs2_Prv_BlockPropertyUpdateDo(fsm_pst) != MEMIF_JOB_OK)
            {
                rba_EaFs2_Prv_FsInfo_st.isInitFailed_b = (boolean)TRUE;
            }
        }
    }

    if( rba_EaFs2_Prv_IsBlockDblStorage(block_pst->blockNumber_u16) != block_pst->doublestorage_b)
    {
        rba_EaFs2_Prv_FsInfo_st.securityLevelChangedBlocks_u16++;
        rba_EaFs2_Prv_RamTable_st[block_pst->blockNumber_u16].isSecurityLevelChanged_b = (boolean)TRUE;
        /* report DET error - Length Mismatch */
        RBA_EAFS2_DET_REPORTERROR(EA_SID_INIT,EA_E_RB_BLOCK_LENGTH_MISMATCH);
    }
}


/**
 * \brief    Check if update on RAM table is needed.
 *
 *
 * \return   TRUE  - RAM Table must be updated.
 *           FALSE - FAT entry is already in RAM Table or should not be considered.
 *
 */
static boolean rba_EaFs2_Prv_IsRamTableUpdateNeeded(uint16 blockNumber_u16)
{
    boolean result_b = (boolean)FALSE;

    if(0x0000uL == rba_EaFs2_Prv_RamTable_st[blockNumber_u16].address_u32)
    {
        /* FAT entry not present in RAM Table*/
        result_b = (boolean)TRUE;

        if(
                (rba_EaFs2_Prv_DefragBlk_st.movingId_u16 != 0x0)                                    &&
                (rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 > rba_EaFs2_Prv_DefragBlk_st.destIdx_u16) &&
                (rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 < rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16)
        )
        {
            /* The FAT under conisderation is between the source and destination address of an ongoing defragmentation,
             * Hence these FAT entries must not be considered.
             */
            result_b = (boolean)FALSE;
        }
    }

    return result_b;
}

/**
 * \brief    Marker pattern at location 0 is read to check if defragmentation was ongoing or not in previous power cycle.
 */
#if(FALSE == EA_RB_ACT_UNKNOWN_BLOCK_MIG)
static void rba_EaFs2_Prv_MarkerRead(void)
{
    MemIf_JobResultType result_en;

    rba_EaFs2_Prv_EepromRead(0x0u,
            &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
            sizeof(uint32));

    result_en = rba_EaFs2_Prv_EepromStatus();
    if((result_en == MEMIF_JOB_OK) && (rba_EaFs2_Prv_Eeprom_st.buffer_u.u32[0] == RBA_EAFS2_PRV_TEMPMARKER))
    {
        rba_EaFs2_Prv_TempMarker_st.wrfinish_b= (boolean)TRUE;
    }
    else
    {
        rba_EaFs2_Prv_TempMarker_st.wrfinish_b= (boolean)FALSE;
        if(rba_EaFs2_Prv_Eeprom_st.buffer_u.u32[0] != EA_MIGRATION_MARKER)
        {
            rba_EaFs2_Prv_FsInfo_st.updateMigragationMarker_b = (boolean)TRUE;
        }
    }
}
#endif

/**
 * \brief    Reserved area is read to update the defragmentation status. Repair is done for the defrag block
 *           if its required
 *
 *
 * \return   MEMIF_JOB_OK       - Defrag block is read.
 *           MEMIF_JOB_FAILED   - eep failure.
 *
 */
void rba_EaFs2_Prv_ParseReservedArea(void)
{
    MemIf_JobResultType result_en;

#if(FALSE == EA_RB_ACT_UNKNOWN_BLOCK_MIG)
    (void)rba_EaFs2_Prv_MarkerRead();
#endif

    rba_EaFs2_Prv_defragBlkRead();

    result_en = rba_EaFs2_Prv_defragBlkOperDo();

    rba_EaFs2_Prv_DefragInfo_st.lastGapIdx_u16 = 0xFFFFu;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_DefragInfo_st,RBA_EAFS2_PRV_DEFRAG_INIT_E);

    if(MEMIF_JOB_FAILED == result_en)
    {
        rba_EaFs2_Prv_FsInfo_st.isInitFailed_b = (boolean)TRUE;
    }
}

/**
 * \brief    Sets the state machine to read the defrag block
 *
 */
void rba_EaFs2_Prv_defragBlkRead(void)
{
    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_DefragBlkOper_st,RBA_EAFS2_PRV_DEFRAGBLK_READ_E);
}

/**
 * \brief    Sets the state machine to write the defrag block
 *
 */
void rba_EaFs2_Prv_defragBlkWrite(void)
{
    if((boolean)FALSE == rba_EaFs2_Prv_DefragInfo_st.isDefragHdrRepairNeeded_b)
    {
        rba_EaFs2_Prv_DefragBlkOper_st.copies_u8  = 2u;
        rba_EaFs2_Prv_DefragBlkOper_st.addrOffset_u8 = 0u;
    }

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_DefragBlkOper_st,RBA_EAFS2_PRV_DEFRAGBLK_WRITE_E);
}

/**
 * \brief    Worker function to perform defrag block operations. Both read and write operations are processed.
 *
 *\return    MEMIF_JOB_OK     - Defrag read/write operation completed
 *\return    MEIF_JOB_FAILED  - eep failure
 */
MemIf_JobResultType rba_EaFs2_Prv_defragBlkOperDo(void)
{
    MemIf_JobResultType result_en;
    rba_EaFs2_Prv_DefragBlkOper_tst * fsm_pst = &rba_EaFs2_Prv_DefragBlkOper_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_DEFRAGBLK_READ_E   : result_en = rba_EaFs2_Prv_DefragBlkOperRead   (fsm_pst); break;
            case RBA_EAFS2_PRV_DEFRAGBLK_WRITE_E  : result_en = rba_EaFs2_Prv_DefragBlkOperWrite  (fsm_pst); break;
            case RBA_EAFS2_PRV_DEFRAGBLK_VERIFY_E : result_en = rba_EaFs2_Prv_DefragBlkOperVerify (fsm_pst); break;
            default: result_en = MEMIF_JOB_FAILED; break;
        }
    }while ((MEMIF_JOB_OK == result_en) && (RBA_EAFS2_PRV_DEFRAGBLK_FINAL_E != fsm_pst->state_en));

    if(MEMIF_JOB_PENDING != result_en)
    {
        rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 &= RBA_EAFS2_PRV_FAT_ADDR_PATTERN_CLR;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAGBLK_IDLE_E);
    }

    return result_en;
}

/**
 * \brief    Low level function to read the defrag block. Based on the 2 copies of the defrag block read decide
 *           if repair if required.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_DefragBlkOperRead(rba_EaFs2_Prv_DefragBlkOper_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromRead(RBA_EAFS2_PRV_DEFRAGBLK_ADDR,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
                2u*sizeof(rba_EaFs2_Prv_DefragBlk_tst)
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        rba_EaFs2_Prv_DefragBlk_tst copy1_st;
        rba_EaFs2_Prv_DefragBlk_tst copy2_st;

        rba_EaFs2_Prv_Memcpy(&rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
                (uint8 *)&copy1_st,
                sizeof(rba_EaFs2_Prv_DefragBlk_tst)
        );

        rba_EaFs2_Prv_Memcpy(&rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[sizeof(rba_EaFs2_Prv_DefragBlk_tst)],
                (uint8 *)&copy2_st,
                sizeof(rba_EaFs2_Prv_DefragBlk_tst)
        );

        if((boolean)FALSE != rba_EaFs2_Prv_IsDefragBlkValid(&copy1_st))
        {
            rba_EaFs2_Prv_DefragBlk_st = copy1_st;

            if((boolean)FALSE != rba_EaFs2_Prv_IsDefragBlkValid(&copy2_st) )
            {
                if(copy1_st.crc_u16 == copy2_st.crc_u16)
                {
                    /* Case VV Both copies are valid and equal, safe to update defrag block info*/
                }
                else
                {
                    /* Case V+V Write copy1 into copy2
                     * if both copies are equal an different take copy1 as valid since update is done on it first*/
                    fsm_pst->copies_u8 = 1;
                    fsm_pst->addrOffset_u8 = sizeof(rba_EaFs2_Prv_DefragBlk_tst);
                    rba_EaFs2_Prv_DefragInfo_st.isDefragHdrRepairNeeded_b = (boolean)TRUE;
                }
            }
            else
            {
                /* Case VC Write copy1 into copy2*/
                fsm_pst->copies_u8 = 1;
                fsm_pst->addrOffset_u8 = sizeof(rba_EaFs2_Prv_DefragBlk_tst);
                rba_EaFs2_Prv_DefragInfo_st.isDefragHdrRepairNeeded_b = (boolean)TRUE;
            }
        }
        else
        {
            /* Copy1 corrupted, validate copy2 and write 2 into 1*/
            if((boolean)FALSE != rba_EaFs2_Prv_IsDefragBlkValid(&copy2_st) )
            {
                /* Case CV write copy2 into copy1*/
                /* The structure assignment should not be done if structure contains a pointer */
                rba_EaFs2_Prv_DefragBlk_st = copy2_st;
                fsm_pst->copies_u8 = 1;
                fsm_pst->addrOffset_u8 = 0u;
                rba_EaFs2_Prv_DefragInfo_st.isDefragHdrRepairNeeded_b = (boolean)TRUE;
            }
            else
            {
                /* Case CC Both copies are broken*/
                /* Write default value for Defrag Block */
                rba_EaFs2_Prv_DefragBlk_st.movingId_u16 = 0x0000u;
            }
        }

        if(0x0000u == rba_EaFs2_Prv_DefragBlk_st.movingId_u16)
        {
            /* For Cleared Defrag header, ID is set as 0, also for CC scenario ID is set as 0*/
            rba_EaFs2_Prv_DefragInfo_st.isDefragHdrRepairNeeded_b = (boolean)FALSE;
        }

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAGBLK_FINAL_E);
    }

    return result_en;
}


/**
 * \brief    Low level function to write the defrag block.
 *           This function writes both the copies one after other
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_DefragBlkOperWrite(rba_EaFs2_Prv_DefragBlkOper_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Freeup First Byte of resume address*/
        rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 &= RBA_EAFS2_PRV_FAT_ADDR_PATTERN_CLR;
        /* Add the known pattern 0x5A to the first byte of resume address */
        rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 |= RBA_EAFS2_PRV_FAT_ADDR_PATTERN;

        rba_EaFs2_Prv_DefragBlk_st.crc_u16 = Crc_CalculateCRC16(
                (uint8 const *)&rba_EaFs2_Prv_DefragBlk_st,
                sizeof(rba_EaFs2_Prv_DefragBlk_tst)-sizeof(uint16),
                EA_MARKER_PATTERN,
                (boolean)FALSE
        );

        rba_EaFs2_Prv_EepromWrite(RBA_EAFS2_PRV_DEFRAGBLK_ADDR + fsm_pst->addrOffset_u8,
                (uint8 *)&rba_EaFs2_Prv_DefragBlk_st,
                sizeof(rba_EaFs2_Prv_DefragBlk_tst)
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->copies_u8--;
        fsm_pst->addrOffset_u8 = sizeof(rba_EaFs2_Prv_DefragBlk_tst);

        if(0 < fsm_pst->copies_u8)
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAGBLK_WRITE_E);
        }
        else
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAGBLK_VERIFY_E);
        }
    }

    return result_en;
}

/**
 * \brief    Low level function to Verify the defrag block write.
 *           This function Verifies both the copies together
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_DefragBlkOperVerify(rba_EaFs2_Prv_DefragBlkOper_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_Memcpy(
                (uint8 *)&rba_EaFs2_Prv_DefragBlk_st,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                sizeof(rba_EaFs2_Prv_DefragBlk_tst)
        );

        rba_EaFs2_Prv_Memcpy(
                (uint8 *)&rba_EaFs2_Prv_DefragBlk_st,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[sizeof(rba_EaFs2_Prv_DefragBlk_tst)],
                sizeof(rba_EaFs2_Prv_DefragBlk_tst)
        );

        rba_EaFs2_Prv_EepromCompare(RBA_EAFS2_PRV_DEFRAGBLK_ADDR,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                (2u*sizeof(rba_EaFs2_Prv_DefragBlk_tst))
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAGBLK_FINAL_E);
    }

    return result_en;
}

/**
 * \brief    Funtion to allocate FAT entries for survival blocks.
 *
 * \return   MEMIF_JOB_OK      - allocation of survival/permanent blocks done.
 *           MEMIF_JOB_FAILED  - allocation failed. Eep failure
 *
 * \attention This function is synchornous and must be used only in Init
 */
void rba_EaFs2_Prv_PreAllocation(void)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint16 blockNr_u16            = 0;

    for(blockNr_u16 = 0; blockNr_u16 < EA_NUM_BLOCKS; blockNr_u16++)
    {
        /* Allocate survival block if not done, typically wont be done*/
        if( ((boolean)FALSE != rba_EaFs2_Prv_IsBlockSurvival(blockNr_u16) ) &&
                (0u == rba_EaFs2_Prv_RamTable_st[blockNr_u16].address_u32) )
        {

            /*Survival block take for preallocation*/
            rba_EaFs2_Prv_FsInfo_st.allocIter_u16 = blockNr_u16;

            /* Create a Block entry and proceed for allocation of space*/
            rba_EaFs2_Prv_PrepFatInfo();

            result_en = rba_EaFs2_Prv_FatAllocDo();

            if(MEMIF_JOB_OK == result_en)
            {
                /* Nothing to do continue */
            }
            else
            {
                /* Eep failure, exit*/
                break;
            }
        }
    }

    if(MEMIF_JOB_FAILED == result_en)
    {
        rba_EaFs2_Prv_FsInfo_st.isInitFailed_b = (boolean)TRUE;
    }
}


/**
 * \brief   Starts a FAT Write operation
 *
 * \param   block_pcst   pointer to the fat entry block
 * \parm    fatIndex_u16 fat index position to write the fat entry
 *
 */
void rba_EaFs2_Prv_FatWrite(rba_EaFs2_Prv_Block_tst const * block_pcst,uint16 fatIndex_u16)
{
    rba_EaFs2_Prv_FatWrite_st.block_pcst   = block_pcst;
    rba_EaFs2_Prv_FatWrite_st.fatAdr_u32   = rba_EaFs2_Prv_FatIndex2Address(fatIndex_u16);
    rba_EaFs2_Prv_FatWrite_st.fatIndex_u16 = fatIndex_u16;

    rba_EaFs2_Prv_FatWrite_st.retry_u8 = 3;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_FatWrite_st,RBA_EAFS2_PRV_FAT_FATWRITE_INIT_E);
}

/**
 * \brief   Worker function to write a FAT entry.
 *
 * \return  MEMIF_JOB_PENDING   Writing the FAT entry is still ongoing
 *          MEMIF_JOB_OK        Writing the FAT entry finished successfully
 *          MEMIF_JOB_FAILED    During writing an undefined error occurred
 */
MemIf_JobResultType rba_EaFs2_Prv_FatWriteDo(void)
{
    MemIf_JobResultType result_en;
    rba_EaFs2_Prv_FatWrite_tst * fsm_pst = &rba_EaFs2_Prv_FatWrite_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_FAT_FATWRITE_INIT_E:    result_en = rba_EaFs2_Prv_FatWriteInit    (fsm_pst);  break;
            case RBA_EAFS2_PRV_FAT_FATWRITE_WRITING_E: result_en = rba_EaFs2_Prv_FatWriteWrite   (fsm_pst);  break;
            case RBA_EAFS2_PRV_FAT_FATWRITE_COMPARE_E: result_en = rba_EaFs2_Prv_FatWriteCompare (fsm_pst);  break;
            default:                                   result_en = rba_EaFs2_Prv_LibResetRetry(&fsm_pst->retry_u8); break;
        }
    }while((MEMIF_JOB_OK == result_en) &&(RBA_EAFS2_PRV_FAT_FATWRITE_FINAL_E != fsm_pst->state_en));

    if( (MEMIF_JOB_FAILED == result_en)&& (0 < fsm_pst->retry_u8) )
    {
        /* Retry again Fat write */
        fsm_pst->retry_u8--;
        result_en = MEMIF_JOB_PENDING;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FAT_FATWRITE_INIT_E);
    }

    if(MEMIF_JOB_PENDING != result_en)
    {
        /* Removing the pattern added while writing into the medium*/
        fsm_pst->fatEntry_st.address_u32 &= RBA_EAFS2_PRV_FAT_ADDR_PATTERN_CLR;
        fsm_pst->fatEntry_st.status_u16  &= RBA_EAFS2_PRV_FAT_STATUS_PATTERN_CLR;
        /* Reset state machine*/
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FAT_FATWRITE_IDLE_E);
    }

    return result_en;
}

/**
 * \brief  Fills the eeprom buffer with the two copies of the FAT entry to be written.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_FatWriteInit(rba_EaFs2_Prv_FatWrite_tst * fsm_pst)
{
    rba_EaFs2_Prv_Block2Fat(fsm_pst->block_pcst,&fsm_pst->fatEntry_st);

    rba_EaFs2_Prv_Memcpy(
            (uint8 *)&fsm_pst->fatEntry_st,
            &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
            sizeof(rba_EaFs2_Prv_FatEntry_tst)
    );

    rba_EaFs2_Prv_Memcpy(
            (uint8 *)&fsm_pst->fatEntry_st,
            &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[sizeof(rba_EaFs2_Prv_FatEntry_tst)],
            sizeof(rba_EaFs2_Prv_FatEntry_tst)
    );

    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FAT_FATWRITE_WRITING_E);

    return MEMIF_JOB_OK;
}

/**
 * \brief  Performs the low level write of the FAT entries.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_FatWriteWrite(rba_EaFs2_Prv_FatWrite_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromWrite(
                fsm_pst->fatAdr_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                (2u*sizeof(rba_EaFs2_Prv_FatEntry_tst))
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FAT_FATWRITE_COMPARE_E);
    }

    return result_en;
}

/**
 * \brief  Compares the written FAT entry to validate.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_FatWriteCompare(rba_EaFs2_Prv_FatWrite_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromCompare(fsm_pst->fatAdr_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                (2u*sizeof(rba_EaFs2_Prv_FatEntry_tst))
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        if((boolean)FALSE != fsm_pst->block_pcst->isInConfig_b)
        {
            /* For blocks in config if FAT is done RAM table must be updated*/
            rba_EaFs2_Prv_RamTable_st[fsm_pst->block_pcst->blockNumber_u16].fatIndex_u16 = fsm_pst->fatIndex_u16;
            rba_EaFs2_Prv_RamTable_st[fsm_pst->block_pcst->blockNumber_u16].address_u32  = fsm_pst->block_pcst->address_u32;
        }
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FAT_FATWRITE_FINAL_E);
    }

    return result_en;
}

/**
 * \brief   Convert a block entry into a FAT entry.
 *
 * \param   block_pcst      Pointer to the block that shall be converted
 * \param   fatEntry_pst    Buffer where the resulting FAT entry shall be stored
 */
static void rba_EaFs2_Prv_Block2Fat(rba_EaFs2_Prv_Block_tst const * block_pcst,rba_EaFs2_Prv_FatEntry_tst *fatEntry_pst)
{
    uint16 status_u16 = 0;

    if(block_pcst->doublestorage_b)
    {
        status_u16 |= RBA_EAFS2_PRV_SECURITY_LEVEL_FLAG;
    }

    if(block_pcst->survival_b)
    {
        status_u16 |= RBA_EAFS2_PRV_SURVIVAL_FLAG;
    }
#if (EA_WRITE_SPREAD != FALSE)
    if(block_pcst->writeSpread_b)
    {
        status_u16 |= RBA_EAFS2_PRV_WRITESPREAD_FLAG;
    }
#endif /*(EA_WRITE_SPREAD != FALSE)*/

    fatEntry_pst->ID_u16      = block_pcst->persistentId_u16;
    fatEntry_pst->length_u16  = block_pcst->length_u16;
    fatEntry_pst->address_u32 = ((block_pcst->address_u32 & RBA_EAFS2_PRV_FAT_ADDR_PATTERN_CLR) |
            RBA_EAFS2_PRV_FAT_ADDR_PATTERN);    /* Adds the pattern 0xA5 to the unused Address byte */
    fatEntry_pst->status_u16  = ((status_u16 & RBA_EAFS2_PRV_FAT_STATUS_PATTERN_CLR) |
            RBA_EAFS2_PRV_FAT_STATUS_PATTERN);                /* Adds the pattern 0x5A to the unused Status byte */

    /* MR12 RULE 11.3 VIOLATION: API takes only this pointer */
    fatEntry_pst->crc_u16 = Crc_CalculateCRC16(
            (uint8 const *)fatEntry_pst,
            sizeof(rba_EaFs2_Prv_FatEntry_tst) - sizeof(uint16),
            EA_MARKER_PATTERN,
            (boolean)FALSE
    );
}

/**
 * \brief   Convert a FAT entry into a Block format.
 *
 * \param   block_pcst      Pointer to the block that shall be converted
 * \param   fatEntry_pst    Buffer where the resulting FAT entry shall be stored
 */
void rba_EaFs2_Prv_Fat2Block(rba_EaFs2_Prv_Block_tst * const block_cpst,rba_EaFs2_Prv_FatEntry_tst const * fatEntry_pcst)
{
    block_cpst->address_u32      = fatEntry_pcst->address_u32;
    block_cpst->length_u16       = fatEntry_pcst->length_u16;
    block_cpst->persistentId_u16 = fatEntry_pcst->ID_u16;

    block_cpst->blockNumber_u16  = rba_EaFs2_Prv_GetBlockID(fatEntry_pcst->ID_u16);
    block_cpst->doublestorage_b  = (boolean)FALSE;
    block_cpst->survival_b       = (boolean)FALSE;
    block_cpst->isInConfig_b     = (boolean)FALSE;
#if (EA_WRITE_SPREAD != FALSE)
    block_cpst->writeSpread_b    = (boolean)FALSE;
#endif /*(EA_WRITE_SPREAD != FALSE)*/


    if(0u != (fatEntry_pcst->status_u16 & RBA_EAFS2_PRV_SURVIVAL_FLAG) )
    {
        block_cpst->survival_b = (boolean)TRUE;
    }

    if(0u != (fatEntry_pcst->status_u16 & RBA_EAFS2_PRV_SECURITY_LEVEL_FLAG) )
    {
        block_cpst->doublestorage_b = (boolean)TRUE;
    }
#if (EA_WRITE_SPREAD != FALSE)
    if(0u != (fatEntry_pcst->status_u16 & RBA_EAFS2_PRV_WRITESPREAD_FLAG) )
    {
        block_cpst->writeSpread_b = (boolean)TRUE;
    }
#endif /*(EA_WRITE_SPREAD != FALSE)*/

    if(0xFFFFu != block_cpst->blockNumber_u16)
    {
        /*block is present in current config*/
        block_cpst->isInConfig_b     = (boolean)TRUE;
    }
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG != FALSE)
    else
    {
        /* In Boot mode all unknown blocks are treated as survival to retain all blocks */
        if(block_cpst->persistentId_u16 != 0xFFFFu)
        {
            block_cpst->survival_b = (boolean)TRUE;
        }
    }
#endif
}

/**
 * \brief   Function to verify if the FAT entry has a valid CRC.
 *
 * \param   fatEntry_pst    pointer to the FAT entry to validate
 */

static boolean rba_EaFs2_Prv_IsFatValid(rba_EaFs2_Prv_FatEntry_tst * fatEntry_pst)
{
    uint16 crc_u16;
    boolean result_b = (boolean)FALSE;

    crc_u16 = Crc_CalculateCRC16(
            (uint8 const *)fatEntry_pst,
            sizeof(rba_EaFs2_Prv_FatEntry_tst) - sizeof(uint16),
            EA_MARKER_PATTERN,
            (boolean)FALSE
    );


    /* Check whether both CRC and Pattern(0x5A) are matching or Not */
    if((crc_u16 == fatEntry_pst->crc_u16) &&
            (RBA_EAFS2_PRV_FAT_ADDR_PATTERN == (fatEntry_pst->address_u32 & RBA_EAFS2_PRV_FAT_ADDR_PATTERN_MASK)) &&
            (RBA_EAFS2_PRV_FAT_STATUS_PATTERN == (fatEntry_pst->status_u16 & RBA_EAFS2_PRV_FAT_STATUS_PATTERN_MASK)))
    {
        /* Clear the Pattern written at Addr and Status location of FAT */
        fatEntry_pst->address_u32 &= RBA_EAFS2_PRV_FAT_ADDR_PATTERN_CLR;
        fatEntry_pst->status_u16 &= RBA_EAFS2_PRV_FAT_STATUS_PATTERN_CLR;

        /* Older FAT entries will point to higher data address*/
        if(fatEntry_pst->address_u32 >= rba_EaFs2_Prv_FatIndex2Address(rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16))
        {
            result_b = (boolean)TRUE;
        }

    }

    return result_b;
}

/*
 * \breief Function to decide if the FAT Block should be conisdered for processing into RAM table
 *
 * A FAT Block is considered as ignored under the following creiteria
 *                  - Not in current config (survival blocks)
 *                  - Property changed
 *                  - Length changed
 *
 * \retval          - TRUE  FAT is ignored
 *                  - FALSE FAT should be considered
 * */
boolean rba_EaFs2_Prv_IsBlockIgnore(rba_EaFs2_Prv_Block_tst * block_pst)
{
    boolean result_b = (boolean)TRUE;

    if((boolean)FALSE != block_pst->isInConfig_b)
    {
        /* The FAT entry is for a known block*/
        if(( Ea_BlockProperties_st[block_pst->blockNumber_u16].Ea_BlkLength_u16 != block_pst->length_u16     )
#if (EA_WRITE_SPREAD != FALSE)
                ||( rba_EaFs2_Prv_IsBlockWriteSpread(block_pst->blockNumber_u16)       != block_pst->writeSpread_b)
#endif /*(EA_WRITE_SPREAD != FALSE)*/
        )
        {
            /* Configured Block length or Block status and FAT entry block length or status are different*/
            /* Report error to DET about block length , Security level mismatch
             * isInConfig is reset so that defrag does not allocate this wrongly configured block into ramtable*/
            block_pst->isInConfig_b = (boolean)FALSE;

            RBA_EAFS2_DET_REPORTERROR(EA_SID_INIT,EA_E_RB_BLOCK_LENGTH_MISMATCH);
        }
        else
        {
            /* Block should not be ignored! */
            result_b = (boolean)FALSE;
        }
    }

    return result_b;
}

/* this function used to load 256 byte to buffer to check */
MemIf_JobResultType rba_EaFs2_Prv_readFatEEPROM(uint8 * corruptHeader, uint32 adrFat_u32)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    rba_EaFs2_Prv_FatEntry_tst fatEnty_st;

    result_en = rba_EaFs2_Prv_EepromLoad(
            adrFat_u32,
            (uint16)(2u*sizeof(rba_EaFs2_Prv_FatEntry_tst)),
            RBA_EAFS2_PRV_NXTLOAD_E
    );

    if(MEMIF_JOB_OK == result_en)
    {
        *corruptHeader = rba_EaFs2_Prv_FindHeaderCorrupt(&fatEnty_st,adrFat_u32);
    }
    return result_en;
}

/* this function to return header copy which corrupted */
static uint8 rba_EaFs2_Prv_FindHeaderCorrupt(rba_EaFs2_Prv_FatEntry_tst * fms_pst,uint32 adrFat_u32)
{
    uint8 result_u = 0;
    /* Inside 256 byte Ea buffer,get the address location to read next FAT to validate*/
    uint32 index_u32 = adrFat_u32 - rba_EaFs2_Prv_Eeprom_st.lowLimit_u32;
    /*get copy1 from the buffer into the local structure*/
    rba_EaFs2_Prv_Memcpy(
           &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[index_u32],
           (uint8 *)fms_pst,
           sizeof(rba_EaFs2_Prv_FatEntry_tst)
    );

    if((boolean)FALSE != rba_EaFs2_Prv_IsFatValid(fms_pst))
    {
         /* Copy1 CRC is consistent*/
         index_u32 += sizeof(rba_EaFs2_Prv_FatEntry_tst);
         /*get copy2 from the buffer into the local structure*/
         rba_EaFs2_Prv_Memcpy(
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[index_u32],
                (uint8 *)fms_pst,
                sizeof(rba_EaFs2_Prv_FatEntry_tst)
         );

         if((boolean)FALSE == rba_EaFs2_Prv_IsFatValid(fms_pst))
         {
           result_u = SECOND_COPY_CORRUPTED;
         }
    }else
    {
        result_u = FIRST_COPY_CORRUPTED;
    }
    return result_u;
}

#if(FALSE != EA_PRV_DEBUGGING)
/**
 *********************************************************************
 * rba_EaFs2_Prv_Initialize_Fat():
 *
 * This function used as initialzation for global variable
 *
 * \param    None
 *
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_EaFs2_Prv_Initialize_Fat(void)
{
    uint16 count = 0;
    for(count =0; count<EA_NUM_BLOCKS;count++)
    {
        rba_EaFs2_Prv_RamTable_st[count].address_u32 =0u;
        rba_EaFs2_Prv_RamTable_st[count].fatIndex_u16 =0u;
        rba_EaFs2_Prv_RamTable_st[count].status_u16 =0u;
    }

    rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 =0u;
    rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16=0u;
    rba_EaFs2_Prv_DefragBlk_st.destIdx_u16=0u;
    rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16=0u;
    rba_EaFs2_Prv_DefragBlk_st.movingId_u16=0u;
    rba_EaFs2_Prv_DefragBlk_st.status_u16=0u;
    rba_EaFs2_Prv_DefragBlk_st.crc_u16=0u;

    rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32=0u;
    rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16=0u;
    rba_EaFs2_Prv_FsInfo_st.totalGaps_u16=0u;
    rba_EaFs2_Prv_FsInfo_st.allotedblocks_u16=0u;
    rba_EaFs2_Prv_FsInfo_st.allocIter_u16=0u;
    rba_EaFs2_Prv_FsInfo_st.bgoEnable_b= FALSE;
    rba_EaFs2_Prv_FsInfo_st.isPreAllocReq_b = FALSE;
    rba_EaFs2_Prv_FsInfo_st.isInitFailed_b = FALSE;
    rba_EaFs2_Prv_FsInfo_st.suspendBgo_b = FALSE;
    rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;

}
#endif

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif
