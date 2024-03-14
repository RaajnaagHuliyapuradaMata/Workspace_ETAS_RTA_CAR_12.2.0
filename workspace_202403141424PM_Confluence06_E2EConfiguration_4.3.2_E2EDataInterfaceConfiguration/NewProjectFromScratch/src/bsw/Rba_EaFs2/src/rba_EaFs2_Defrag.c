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
#include "Ea_Cfg_SchM.h"
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

rba_EaFs2_Prv_DefragInfo_tst rba_EaFs2_Prv_DefragInfo_st;      /* Structure variable to hold the Defrag related internal variables*/
rba_EaFs2_Prv_Split_tst rba_EaFs2_Prv_Split_st[3];             /* Structure to hold split data inforamtion */

#if((EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE) && (EA_RB_ENTER_STOP_MODE != FALSE))
rba_EaFs2_Prv_UnAllocBlkSpace_tst rba_EaFs2_Prv_UnAllocBlkSpace_st; /* Structure to handle Unallocblkspce calculation */
#endif

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)

#define EA_START_SEC_VAR_CLEARED_8
#include "Ea_MemMap.h"

uint8 Ea_Rb_MigrationStaus_u8;                    /* Global variable to hold the Defragmentation status(Migration for backward compatability) */

#define EA_STOP_SEC_VAR_CLEARED_8
#include "Ea_MemMap.h"

#endif

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
/* Static Declarations */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_Init        (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatReadUp   (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatReadPrev (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatReadNxt  (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_PreErase    (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_MoveInit    (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_DataRead    (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_DataWrite   (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_DataVerify  (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_UpdateHdr   (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatOverWrite(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatClear    (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_UpdateGap   (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_NxtSteps    (rba_EaFs2_Prv_DefragInfo_tst * fsm_pst);
static rba_EaFs2_Prv_DefragStatus_ten rba_EaFs2_Prv_GetDefragStatus ( void );
static uint16 rba_EaFs2_Prv_EncodeDefragStatus ( void );

#if(EA_RB_ENTER_STOP_MODE != FALSE)
static uint32 rba_EaFs2_Prv_UnallocBlkSpaceCalc(uint16 BlkNumoffset, uint16 BlkNumMax);
#endif


/**
 * \brief   Worker function to Move a block.
 *
 * \return  MEMIF_JOB_PENDING   Moving the block is still ongoing
 *          MEMIF_JOB_OK        Movement of Block finished successfully
 *          MEMIF_JOB_FAILED    During Defrag an undefined error occurred
 */
MemIf_JobResultType rba_EaFs2_Prv_DefragDo(void)
{
    MemIf_JobResultType result_en;
    rba_EaFs2_Prv_DefragInfo_tst * fsm_pst = &rba_EaFs2_Prv_DefragInfo_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_DEFRAG_INIT_E:               result_en = rba_EaFs2_Prv_Defrag_Init           (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_FATREADUP_E:          result_en = rba_EaFs2_Prv_Defrag_FatReadUp      (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_FATREADPRV_E:         result_en = rba_EaFs2_Prv_Defrag_FatReadPrev    (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_FATREADNXT_E:         result_en = rba_EaFs2_Prv_Defrag_FatReadNxt     (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_PREERASE_E:           result_en = rba_EaFs2_Prv_Defrag_PreErase       (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_MOVEINIT_E:           result_en = rba_EaFs2_Prv_Defrag_MoveInit       (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_DATAREAD_E:           result_en = rba_EaFs2_Prv_Defrag_DataRead       (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_DATAWRITE_E:          result_en = rba_EaFs2_Prv_Defrag_DataWrite      (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_DATAVERIFY_E:         result_en = rba_EaFs2_Prv_Defrag_DataVerify     (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E:          result_en = rba_EaFs2_Prv_Defrag_UpdateHdr      (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_FATOVERWRITE_E:       result_en = rba_EaFs2_Prv_Defrag_FatOverWrite   (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_FATCLEAR_E:           result_en = rba_EaFs2_Prv_Defrag_FatClear       (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_UPDATEGAP_E:          result_en = rba_EaFs2_Prv_Defrag_UpdateGap      (fsm_pst);  break;
            case RBA_EAFS2_PRV_DEFRAG_NXTSTEPS_E:           result_en = rba_EaFs2_Prv_Defrag_NxtSteps       (fsm_pst);  break;
            default:                                        result_en = MEMIF_JOB_FAILED; break;
        }
    }while(
            (MEMIF_JOB_OK == result_en) &&
            (RBA_EAFS2_PRV_DEFRAG_FINAL_E != fsm_pst->state_en) &&
            (FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
    );

    if(
            (FALSE != rba_EaFs2_Prv_FsInfo_st.suspendBgo_b) &&
            (MEMIF_JOB_PENDING != result_en)
    )
    {
        /* Defrag is interrupted by User Request, Cancel accepted, clear cancel flag */
        rba_EaFs2_Prv_FsInfo_st.suspendBgo_b = FALSE;
        rba_EaFs2_Prv_PrepareEeprom();
    }

    if((RBA_EAFS2_PRV_DEFRAG_FINAL_E == fsm_pst->state_en) || (result_en == MEMIF_JOB_FAILED))
    {
         /* Reset state machine*/
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_INIT_E);
    }

    return result_en;
}

/**
 * \brief   Defrag Init Function to prepare the gap information
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  MemIf_JobResultType - Always returns MEMIF_JOB_OK
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_Init(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    if(FALSE != fsm_pst->isPwrInterrupted_b)
    {
        if(FALSE != fsm_pst->isDefragHdrRepairNeeded_b)
        {
            /* Defrag Header needs to be repaired */
            if(RBA_EAFS2_PRV_LIBENTRY)
            {
                rba_EaFs2_Prv_defragBlkWrite();
            }

            result_en = rba_EaFs2_Prv_defragBlkOperDo();

        }

        if(MEMIF_JOB_OK == result_en)
        {
            fsm_pst->isClipUsed_b = FALSE;
            fsm_pst->isDataWritteninResume_b = FALSE;
            /* Path to handle the power interrupted defrag */
            fsm_pst->workingGapIdx_u16 = rba_EaFs2_Prv_DefragBlk_st.destIdx_u16;

            fsm_pst->workingNxtValidIdx_u16 = rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16;

            fsm_pst->srcLength_u16 = rba_EaFs2_Prv_SizeInMedium(&(fsm_pst->nextValidBlock_st));

            fsm_pst->gapLength_u16 = (
                    (uint16)(rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 - fsm_pst->nextValidBlock_st.address_u32) -
                    rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16
            );

            if((rba_EaFs2_Prv_DefragBlk_st.status_u16 & RBA_EAFS2_PRV_DEFRAG_STATUS_CLIP_VALID) != 0u)
            {
                fsm_pst->isClipUsed_b = TRUE;
            }

            if((rba_EaFs2_Prv_DefragBlk_st.status_u16 & RBA_EAFS2_PRV_DEFRAG_STATUS_RESUME_VALID) != 0u)
            {
                fsm_pst->isDataWritteninResume_b = TRUE;
            }

            fsm_pst->status_u8 = (uint8)((rba_EaFs2_Prv_DefragBlk_st.status_u16 >> 2) & 0x03u);

            fsm_pst->isPwrInterrupted_b = FALSE;
            fsm_pst->isDefragHdrRepairNeeded_b = FALSE;

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_MOVEINIT_E);
        }
    }
    else
    {
        fsm_pst->blockUnderRead_pst = &(fsm_pst->previousValidBlock_st);

        if(FALSE != fsm_pst->isForcedDefrag_b)
        {

            /* Forced handling to free up space for new blocks to be allocated */
            if(FALSE != fsm_pst->hasLastGapInfo_b)
            {
                /* Last gap is known, continue to freeup space */
                fsm_pst->workingGapIdx_u16 = fsm_pst->lastGapIdx_u16;

                rba_EaFs2_Prv_Eeprom_st.iterator_u32 = rba_EaFs2_Prv_FatIndex2Address(
                        (uint16)rba_EaFs2_Prv_LibMin((uint32)fsm_pst->lastGapIdx_u16,
                                (uint32)(fsm_pst->lastGapIdx_u16-2u))
                );

                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FATREADPRV_E);
            }
            else
            {
                /* Last Gap is Unknown, scan from the current fat index */
                rba_EaFs2_Prv_Eeprom_st.iterator_u32 = rba_EaFs2_Prv_FatIndex2Address(
                        (uint16)rba_EaFs2_Prv_LibMin((uint32)rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16,
                                (uint32)(rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16-2u))
                );

                fsm_pst->lastGapIdx_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;
                fsm_pst->workingNxtValidIdx_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;
                fsm_pst->readFatUpGapFound_b = FALSE;

                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FATREADUP_E);
            }
        }
        else
        {
            /* First gap is known from Init, continue to freeup space */
            fsm_pst->workingGapIdx_u16 = fsm_pst->firstGapIdx_u16;

            rba_EaFs2_Prv_Eeprom_st.iterator_u32 = rba_EaFs2_Prv_FatIndex2Address(
                    (uint16)rba_EaFs2_Prv_LibMin((uint32)fsm_pst->firstGapIdx_u16,
                            (uint32)(fsm_pst->firstGapIdx_u16-2u))
            );

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FATREADPRV_E);
        }


        /* Reset Defrag Header structure to known values */

        fsm_pst->isClipUsed_b = FALSE;
        fsm_pst->isDataWritteninResume_b = FALSE;

        rba_EaFs2_Prv_DefragBlk_st.crc_u16         = 0xFFFFu;
        rba_EaFs2_Prv_DefragBlk_st.destIdx_u16     = 0xFFFFu;
        rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16      = 0xFFFFu;
        rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16 = 0x0000u;
        rba_EaFs2_Prv_DefragBlk_st.movingId_u16    = 0x0000u;
        rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32  = 0x00FFFFFFUL;
        rba_EaFs2_Prv_DefragBlk_st.status_u16      = 0x0000;

        rba_EaFs2_Prv_Split_st[1].addr_u32 = EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE;
        rba_EaFs2_Prv_Split_st[1].len_u16 = 0;
        rba_EaFs2_Prv_Split_st[2].len_u16 = 0;
    }

    rba_EaFs2_Prv_PrepareEeprom();

    return result_en;
}

/**
 * \brief   Function to scan the Fat entries from Current Fat Index to find the last gap
 *          This function will be used only for forced defrag, when no space available for new block allocation
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the FAT read operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatReadUp(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    /* Read Fat Up: returns the Fat structure from currFatIndex*/
    result_en = rba_EaFs2_Prv_readFatUp(&fsm_pst->fatUnderRead_st);

    if(result_en == MEMIF_JOB_OK)
    {
        /* This is the FAT under Read */
        fsm_pst->lastGapIdx_u16 -= (uint16)2;

        rba_EaFs2_Prv_Fat2Block(fsm_pst->blockUnderRead_pst, &fsm_pst->fatUnderRead_st);

        if(
                (FALSE == rba_EaFs2_Prv_IsBlockIgnore(fsm_pst->blockUnderRead_pst))||
                (FALSE != fsm_pst->blockUnderRead_pst->survival_b)
        )
        {
            if(FALSE != fsm_pst->readFatUpGapFound_b)
            {
                /* Previously read FAT was a gap, take its index into last gap*/
                fsm_pst->readFatUpGapFound_b = FALSE;
                fsm_pst->lastGapIdx_u16 += 2u;
                fsm_pst->workingGapIdx_u16 = fsm_pst->lastGapIdx_u16;

                if(fsm_pst->workingNxtValidIdx_u16 == rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16)
                {
                    /* Only gap at the end, update variables */
                    /*workingNxtValidIdx_u16 was initialized to current fat index, if it still remains at it even after
                     * finidng gaps the gaps are found at the end, no data movement is required
                     * */
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEGAP_E);
                }
                else
                {
                    /* Resume address is the start of the previous valid block*/
                    rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 = fsm_pst->previousValidBlock_st.address_u32;
                    rba_EaFs2_Prv_DefragBlk_st.destIdx_u16    = fsm_pst->workingGapIdx_u16;

                    /* Gap length is calculated based on start of the previous and the next valid FATs and
                     * length of the previous FAT*/
                    fsm_pst->gapLength_u16 = (
                            (uint16)(fsm_pst->previousValidBlock_st.address_u32 - fsm_pst->nextValidBlock_st.address_u32) -
                            rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16
                    );
                    /* Valid block just above the gap is found, proceed for data move */
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E);
                }
            }
            else
            {
                /* From bottom scan, this is the topmost known consecutive valid block */
                fsm_pst->nextValidBlock_st = fsm_pst->previousValidBlock_st;
                fsm_pst->workingNxtValidIdx_u16 = fsm_pst->lastGapIdx_u16;
                /* Stay in the same state till a gap is found*/
            }
        }
        else
        {
            if(FALSE == fsm_pst->readFatUpGapFound_b)
            {
                /* First Gap found from the bottom */
                fsm_pst->readFatUpGapFound_b = TRUE;

                /* Found the valid block, update required information */
                fsm_pst->srcLength_u16 = rba_EaFs2_Prv_SizeInMedium(&(fsm_pst->nextValidBlock_st));

                rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16 = fsm_pst->srcLength_u16;
                rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16 = fsm_pst->workingNxtValidIdx_u16;
                rba_EaFs2_Prv_DefragBlk_st.movingId_u16 = fsm_pst->nextValidBlock_st.persistentId_u16;

                rba_EaFs2_Prv_Split_st[0].addr_u32 = fsm_pst->nextValidBlock_st.address_u32;

                rba_EaFs2_Prv_Split_st[0].len_u16 = rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16;

            }
            /* Only Gap found, stay in the same state till a valid block is found*/

            /* Handle top most gap in this scenario */
            if(0u == fsm_pst->lastGapIdx_u16)
            {
                fsm_pst->readFatUpGapFound_b = FALSE;
                fsm_pst->workingGapIdx_u16 = fsm_pst->lastGapIdx_u16;

                rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 = EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE;
                rba_EaFs2_Prv_DefragBlk_st.destIdx_u16 = 0u;

                fsm_pst->gapLength_u16 = (
                        (uint16)(rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 - fsm_pst->nextValidBlock_st.address_u32) -
                        rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16
                );

                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E);
            }
        }
    }

    return result_en;
}

/**
 * \brief   Function to read the fat entry of the previous valid FAT required to determine the gap
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the fat read operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatReadPrev(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(0u == fsm_pst->workingGapIdx_u16)
    {
        /* First index is a gap, proceed for search of next valid block */
        fsm_pst->workingNxtValidIdx_u16 = fsm_pst->workingGapIdx_u16;
        rba_EaFs2_Prv_Eeprom_st.iterator_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->workingNxtValidIdx_u16);
        fsm_pst->blockUnderRead_pst = &(fsm_pst->nextValidBlock_st);

        rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 = EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE;
        rba_EaFs2_Prv_DefragBlk_st.destIdx_u16 = 0u;

        result_en = MEMIF_JOB_OK;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FATREADNXT_E);
    }
    else
    {
        /* Read Fat Down: returns the Fat structure from workingGapIdx*/
        result_en = rba_EaFs2_Prv_readFatDown(&fsm_pst->fatUnderRead_st);

        if(result_en == MEMIF_JOB_OK)
        {
            rba_EaFs2_Prv_Fat2Block(fsm_pst->blockUnderRead_pst, &fsm_pst->fatUnderRead_st);

            if(
                    (FALSE == rba_EaFs2_Prv_IsBlockIgnore(fsm_pst->blockUnderRead_pst))||
                    (FALSE != fsm_pst->blockUnderRead_pst->survival_b)
            )
            {
                /* Found the previous valid block, search for the next valid block */
                fsm_pst->workingNxtValidIdx_u16 = fsm_pst->workingGapIdx_u16;
                fsm_pst->blockUnderRead_pst = &(fsm_pst->nextValidBlock_st);

                rba_EaFs2_Prv_Eeprom_st.iterator_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->workingNxtValidIdx_u16);

                rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 = fsm_pst->previousValidBlock_st.address_u32;
                rba_EaFs2_Prv_DefragBlk_st.destIdx_u16 = fsm_pst->workingGapIdx_u16;

                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FATREADNXT_E);
            }
            else
            {
                /* Gap, look for the previous valid block*/
                fsm_pst->workingGapIdx_u16 -= 2u;
                rba_EaFs2_Prv_Eeprom_st.iterator_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->workingGapIdx_u16-2u);
            }
        }
    }

    return result_en;
}

/**
 * \brief   Function to read the fat entry of the block under movement, valid block below the gap
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the fat read operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatReadNxt(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 == fsm_pst->workingNxtValidIdx_u16)
    {
        result_en = MEMIF_JOB_OK;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEGAP_E);
    }
    else
    {
        /* Read Fat Down: returns the Fat structure from workingGapIdx*/
        result_en = rba_EaFs2_Prv_readFatDown(&fsm_pst->fatUnderRead_st);

        if(result_en == MEMIF_JOB_OK)
        {
            rba_EaFs2_Prv_Fat2Block(fsm_pst->blockUnderRead_pst, &fsm_pst->fatUnderRead_st);

            if(
                    (FALSE == rba_EaFs2_Prv_IsBlockIgnore(fsm_pst->blockUnderRead_pst))||
                    (FALSE != fsm_pst->blockUnderRead_pst->survival_b)
            )
            {
                /* Found the valid block, start data movement */
                fsm_pst->srcLength_u16 = rba_EaFs2_Prv_SizeInMedium(&(fsm_pst->nextValidBlock_st));

                rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16 = fsm_pst->srcLength_u16;
                rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16 = fsm_pst->workingNxtValidIdx_u16;
                rba_EaFs2_Prv_DefragBlk_st.movingId_u16 = fsm_pst->nextValidBlock_st.persistentId_u16;

                rba_EaFs2_Prv_Split_st[0].addr_u32 = fsm_pst->nextValidBlock_st.address_u32;

                rba_EaFs2_Prv_Split_st[0].len_u16 = rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16;

                fsm_pst->gapLength_u16 = (
                        (uint16)(rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 - fsm_pst->nextValidBlock_st.address_u32) -
                        rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16
                );

                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E);
            }
            else
            {
                /* Still a gap find the next valid block */
                fsm_pst->workingNxtValidIdx_u16 += 2u;
                /*Look the behaviour for workingGap =0 case*/
                rba_EaFs2_Prv_Eeprom_st.iterator_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->workingNxtValidIdx_u16);
            }

        }
    }

    return result_en;
}

/**
 * \brief   Router function for a single block movement
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - MEMIF_JOB_OK for all valid cases
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_MoveInit(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    rba_EaFs2_Prv_DefragStatus_ten status_en = rba_EaFs2_Prv_GetDefragStatus();

    switch(status_en)
    {
        case RBA_EAFS2_PRV_DEFRAGSTATUS_ERASEFAT_E:
        {
            rba_EaFs2_Prv_FatBlockClear(&fsm_pst->blockToUpdate_st);

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_PREERASE_E);
        }
        break;
        case RBA_EAFS2_PRV_DEFRAGSTATUS_READSOURCE_E:
        {
            /* Calculate the length to move in this cycle */
            fsm_pst->lengthToMove_u16 = (uint16)rba_EaFs2_Prv_LibMin(
                    rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16,
                    RBA_EAFS2_PRV_CLIPBOARD_SIZE
            );

            /* Find the write address (destination/Clipboard)*/
            if(fsm_pst->lengthToMove_u16 <= fsm_pst->gapLength_u16)
            {
                /* Source to Resume */
                rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 -= (uint32)fsm_pst->lengthToMove_u16;
                fsm_pst->writeAddress_u32 = rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32;
                fsm_pst->isDataWritteninResume_b = TRUE;
            }
            else
            {
                /* Source to Clip */
                fsm_pst->isClipUsed_b = TRUE;
                fsm_pst->writeAddress_u32 = EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE;
            }

            /* Calculate the read address */
            fsm_pst->readAddress_u32 = fsm_pst->nextValidBlock_st.address_u32 +
                    (uint32)(rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16 - fsm_pst->lengthToMove_u16);

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_DATAREAD_E);
        }
        break;

        case RBA_EAFS2_PRV_DEFRAGSTATUS_READCLIP_E:
        {
            /* Clip to destination */
            fsm_pst->isClipUsed_b = FALSE;
            fsm_pst->isDataWritteninResume_b = TRUE;
            fsm_pst->readAddress_u32 = EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE;

            fsm_pst->lengthToMove_u16 = (
                    (fsm_pst->srcLength_u16 - rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16) % RBA_EAFS2_PRV_CLIPBOARD_SIZE
            );

            if(fsm_pst->lengthToMove_u16 == 0u)
            {
                fsm_pst->lengthToMove_u16 = RBA_EAFS2_PRV_CLIPBOARD_SIZE;
            }

            rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 -= (uint32)fsm_pst->lengthToMove_u16;
            fsm_pst->writeAddress_u32 = rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32;

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_DATAREAD_E);

        }
        break;

        case RBA_EAFS2_PRV_DEFRAGSTATUS_WRITEFAT_E:
        {
            fsm_pst->blockToUpdate_st.address_u32      = rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32;
            fsm_pst->blockToUpdate_st.length_u16       = fsm_pst->nextValidBlock_st.length_u16;
            fsm_pst->blockToUpdate_st.persistentId_u16 = fsm_pst->nextValidBlock_st.persistentId_u16;
            fsm_pst->blockToUpdate_st.doublestorage_b  = fsm_pst->nextValidBlock_st.doublestorage_b;
            fsm_pst->blockToUpdate_st.survival_b       = fsm_pst->nextValidBlock_st.survival_b;
            fsm_pst->blockToUpdate_st.blockNumber_u16  = fsm_pst->nextValidBlock_st.blockNumber_u16;
            fsm_pst->blockToUpdate_st.isInConfig_b     = fsm_pst->nextValidBlock_st.isInConfig_b;

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FATOVERWRITE_E);
        }
        break;

        case RBA_EAFS2_PRV_DEFRAGSTATUS_CLEARFAT_E:
        {
            rba_EaFs2_Prv_FatBlockClear(&fsm_pst->blockToUpdate_st);

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FATCLEAR_E);
        }
        break;

        default:
            result_en = MEMIF_JOB_FAILED;
            break;

    }

    return result_en;
}

/**
 * \brief   Function writes the FAT entry with Pre-Erase patterns to clear the Gap index
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the Fat write operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_PreErase(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_FatWrite(&fsm_pst->blockToUpdate_st,fsm_pst->workingGapIdx_u16);
    }

    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        /*pre-erase FATs from destination to source index*/
        if(fsm_pst->workingGapIdx_u16 < (rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16-2u))
        {
            fsm_pst->workingGapIdx_u16+=2u;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_PREERASE_E);
        }
        else
        {
            /* PreErase completed, proceed for Data move */
            fsm_pst->status_u8 = RBA_EAFS2_PRV_DEFRAG_DATAMOVE_ONGOING;
            /* Reset worrking gap index back to destination index*/
            fsm_pst->workingGapIdx_u16 = rba_EaFs2_Prv_DefragBlk_st.destIdx_u16;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_MOVEINIT_E);
        }
    }

    return result_en;
}

/**
 * \brief   Function to read the data of the block under movement (max 256 bytes/cycle)
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the Eeprom read operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_DataRead(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromRead(
                fsm_pst->readAddress_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
                fsm_pst->lengthToMove_u16
        );
    }

    /* Call Eeprom status function to know the status of Eeprom Read */
    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        if(FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
        {
            /* Read Completed, write the data, do not update the info variables here */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_DATAWRITE_E);
        }
        else
        {
            /* Redo the read operation in case cancel is requested,
             * Possible Ea internal buffer collision, reset state to Data Read before Data Write*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_DATAREAD_E);
        }
    }

    return result_en;
}

/**
 * \brief   Function to write data of the block under movement
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - Result of the Eeprom write operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_DataWrite(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromWrite(
                fsm_pst->writeAddress_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                fsm_pst->lengthToMove_u16
        );
    }

    /* Call Eeprom status function to know the status of Eeprom Write */
    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        if(FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
        {
            /* Write Completed, verify the data, do not update the info variables here */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_DATAVERIFY_E);
        }
        else
        {
            /* User request interrupted, redo the operation from Read */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_DATAREAD_E);
        }
    }

    return result_en;
}

/**
 * \brief   Function to verify the data written as part of block data movement
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - Result of the Eeprom verfiy operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_DataVerify(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromCompare(
                fsm_pst->writeAddress_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                fsm_pst->lengthToMove_u16
        );
    }

    /* Call Eeprom status function to know the status of Eeprom Compare */
    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        /* Written data is verified sucessfully, proceed for defrag header update */
        if(FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
        {
            if(fsm_pst->readAddress_u32 != (EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE))
            {
                /* Read was done from source, decrement the length available in source*/
                rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16 -= fsm_pst->lengthToMove_u16;
            }

            if(
                    (0u == rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16) &&
                    (FALSE == fsm_pst->isClipUsed_b)
            )
            {
                /* Data Move is complete, set the state to fat overwrite */
                fsm_pst->status_u8 = RBA_EAFS2_PRV_DEFRAG_FATWRITE_ONGOING;
            }

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E);
        }
        else
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_DATAREAD_E);
        }
    }

    return result_en;
}

/**
 * \brief   Function to update the Defrag block header in the Medium
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the Defrag block write operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_UpdateHdr(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Encode the Defrag status before updating */
        rba_EaFs2_Prv_DefragBlk_st.status_u16 = rba_EaFs2_Prv_EncodeDefragStatus();
        rba_EaFs2_Prv_defragBlkWrite();
    }

    /* Call Defrag Operation worker function to know the status of Defrag Block Write */
    result_en = rba_EaFs2_Prv_defragBlkOperDo();

    if(MEMIF_JOB_OK == result_en)
    {
        if(fsm_pst->status_u8 <= RBA_EAFS2_PRV_DEFRAG_FATWRITE_ONGOING)
        {
            /* The Split structure will have 3 indices
             * 0 : Corresponds to Source information (Start Addess and Length of valid data)
             * 1 : Corresponds to Clip information   (Address of Clip and Length of valid data in Clip)
             * 2 : Corresponds to Resume information (resume address and length of valid data in resume)
             * */
            /* Split information is required untill FAT write operation*/
            rba_EaFs2_Prv_Split_st[0].addr_u32 = fsm_pst->nextValidBlock_st.address_u32;

            rba_EaFs2_Prv_Split_st[0].len_u16 = rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16;

            rba_EaFs2_Prv_Split_st[1].addr_u32 = EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE;

            /* Update the length of valid data available in Clipboard area*/
            if(fsm_pst->isClipUsed_b != (boolean)FALSE)
            {
                rba_EaFs2_Prv_Split_st[1].len_u16 = fsm_pst->lengthToMove_u16;
            }
            else
            {
                rba_EaFs2_Prv_Split_st[1].len_u16 = 0;
            }

            rba_EaFs2_Prv_Split_st[2].addr_u32 = rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32;

            /* Update the length of valid data available in Resume area*/
            if(fsm_pst->isDataWritteninResume_b != (boolean)FALSE)
            {
                if(fsm_pst->isClipUsed_b != (boolean)FALSE)
                {
                    /*calculate length moved to resume based on length in clip and in source*/
                    rba_EaFs2_Prv_Split_st[2].len_u16 = (
                            (fsm_pst->srcLength_u16 - rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16) -
                            fsm_pst->lengthToMove_u16
                    );
                }
                else
                {
                    rba_EaFs2_Prv_Split_st[2].len_u16 = fsm_pst->srcLength_u16 - rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16;
                }
            }
            else
            {
                rba_EaFs2_Prv_Split_st[2].len_u16 = 0;
            }
        }

        if(rba_EaFs2_Prv_DefragBlk_st.movingId_u16 == 0x0000u)
        {
            rba_EaFs2_Prv_Split_st[0].len_u16  = 0u;
            rba_EaFs2_Prv_Split_st[0].addr_u32 = 0u;
            rba_EaFs2_Prv_Split_st[1].len_u16  = 0u;
            rba_EaFs2_Prv_Split_st[1].addr_u32 = 0u;
            rba_EaFs2_Prv_Split_st[2].len_u16  = 0u;
            rba_EaFs2_Prv_Split_st[2].addr_u32 = 0u;

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEGAP_E);
        }
        else
        {
            /* Block move is still ongoing */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_MOVEINIT_E);
        }
    }

    return result_en;
}

/**
 * \brief   Function writes the FAT entry with updated Address of the block under movement
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the FAT Write Operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatOverWrite(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Caller function to initiate the FAT entry write */
        rba_EaFs2_Prv_FatWrite(&fsm_pst->blockToUpdate_st,fsm_pst->workingGapIdx_u16);
    }
    /* Worker function to do the FAT entry write in the Medium */
    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        /* FAT of the moving ID written in dst index, Proceed for Fat Clear */
        fsm_pst->status_u8 = RBA_EAFS2_PRV_DEFRAG_FATCLEAR_ONGOING;

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E);
    }

    return result_en;
}

/**
 * \brief   Clears the FAT entry of the block which is just moved
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  result_en - result of the FAT Write Operation
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_FatClear(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Caller function to initiate the FAT entry write */
        rba_EaFs2_Prv_FatWrite(&fsm_pst->blockToUpdate_st,fsm_pst->workingNxtValidIdx_u16);
    }

    /* Worker function to do the FAT entry write in the Medium */
    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        /* Fat Clear completed, proceed for Clearing of Defrag Block */
        fsm_pst->status_u8 = RBA_EAFS2_PRV_DEFRAG_PREERASE_ONGOING;
        fsm_pst->isClipUsed_b = FALSE;
        fsm_pst->isDataWritteninResume_b = FALSE;
        /* Clearing Defrag Header, Single Block movement completed */
        rba_EaFs2_Prv_DefragBlk_st.destIdx_u16 = 0xFFFF;
        rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16 = 0x0000;
        rba_EaFs2_Prv_DefragBlk_st.movingId_u16 = 0x0000;
        rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32 = 0x00FFFFFFUL;
        rba_EaFs2_Prv_DefragBlk_st.srcIdx_u16 = 0xFFFF;

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_UPDATEHDR_E);
    }

    return result_en;
}

/**
 * \brief   Function updates the gap info for movement of each block during defrag
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  MemIf_JobResultType - Always returns MEMIF_JOB_OK
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_UpdateGap(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    if(fsm_pst->workingNxtValidIdx_u16 == rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16)
    {
        rba_EaFs2_Prv_FsInfo_st.totalGaps_u16 -= ((fsm_pst->workingNxtValidIdx_u16 - fsm_pst->workingGapIdx_u16)/ 2u);
        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 = fsm_pst->workingGapIdx_u16;
        rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 = fsm_pst->previousValidBlock_st.address_u32;
        fsm_pst->lastGapIdx_u16 = 0xFFFFu;
    }

    if((fsm_pst->workingNxtValidIdx_u16+2u) == rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16)
    {
        /* last valid block before the current fat index is moved, space is freed up! */
        rba_EaFs2_Prv_FsInfo_st.totalGaps_u16 -= ((fsm_pst->workingNxtValidIdx_u16 - fsm_pst->workingGapIdx_u16)/ 2u);
        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 = fsm_pst->workingGapIdx_u16 + 2u;
        rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 += fsm_pst->gapLength_u16;
        fsm_pst->lastGapIdx_u16 = 0xFFFFu;
    }

    if(fsm_pst->workingGapIdx_u16 == fsm_pst->firstGapIdx_u16)
    {
        if(fsm_pst->workingGapIdx_u16 == fsm_pst->lastGapIdx_u16)
        {
            fsm_pst->lastGapIdx_u16 = fsm_pst->workingGapIdx_u16+2u;
        }

        fsm_pst->firstGapIdx_u16 = fsm_pst->workingGapIdx_u16+2u;
    }
    else
    {
        if(fsm_pst->workingGapIdx_u16 == fsm_pst->lastGapIdx_u16)
        {
            fsm_pst->lastGapIdx_u16 = fsm_pst->workingGapIdx_u16+2u;
        }
    }
    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_NXTSTEPS_E);

    return MEMIF_JOB_OK;
}

/**
 * \brief   Function that tracks the completion of Defrag
 *
 * \param   rba_EaFs2_Prv_DefragInfo_st - pointer to Defrag info structure
 *
 * \return  MemIf_JobResultType - Always returns MEMIF_JOB_OK
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_Defrag_NxtSteps(rba_EaFs2_Prv_DefragInfo_tst * fsm_pst)
{
    if(rba_EaFs2_Prv_FsInfo_st.totalGaps_u16 == 0u)
    {
        /* Defrag Completed */
        fsm_pst->hasLastGapInfo_b = FALSE;
    }
    else
    {
        /* Last Gap index is known only for forced defrag */
        if(fsm_pst->lastGapIdx_u16 >= rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16)
        {
            fsm_pst->hasLastGapInfo_b = FALSE;
        }
        else
        {
            fsm_pst->hasLastGapInfo_b = TRUE;
        }
    }

    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DEFRAG_FINAL_E);

    return MEMIF_JOB_OK;
}

/**
 * \brief   Converts the uint8 status to defragStatus enum for hassle free processing
 *
 * \param   void
 *
 * \return  status_en : used as input for Defrag_MoveInit.
 *
 */
static rba_EaFs2_Prv_DefragStatus_ten rba_EaFs2_Prv_GetDefragStatus ( void )
{
    rba_EaFs2_Prv_DefragStatus_ten status_en = RBA_EAFS2_PRV_DEFRAGSTATUS_FINAL_E;

    if(rba_EaFs2_Prv_DefragInfo_st.status_u8 == RBA_EAFS2_PRV_DEFRAG_PREERASE_ONGOING)
    {
        status_en=RBA_EAFS2_PRV_DEFRAGSTATUS_ERASEFAT_E;
    }
    else if(rba_EaFs2_Prv_DefragInfo_st.status_u8 == RBA_EAFS2_PRV_DEFRAG_DATAMOVE_ONGOING)
    {
        if(FALSE != rba_EaFs2_Prv_DefragInfo_st.isClipUsed_b)
        {
            status_en=RBA_EAFS2_PRV_DEFRAGSTATUS_READCLIP_E;
        }
        else
        {
            status_en=RBA_EAFS2_PRV_DEFRAGSTATUS_READSOURCE_E;
        }
    }
    else if(rba_EaFs2_Prv_DefragInfo_st.status_u8 == RBA_EAFS2_PRV_DEFRAG_FATWRITE_ONGOING)
    {
        status_en=RBA_EAFS2_PRV_DEFRAGSTATUS_WRITEFAT_E;
    }
    else if(rba_EaFs2_Prv_DefragInfo_st.status_u8 == RBA_EAFS2_PRV_DEFRAG_FATCLEAR_ONGOING)
    {
        status_en=RBA_EAFS2_PRV_DEFRAGSTATUS_CLEARFAT_E;
    }
    else
    {
        status_en=RBA_EAFS2_PRV_DEFRAGSTATUS_FINAL_E;
    }

    return status_en;
}

/**
 * \brief       Encodes the Defrag Status byte to store in H/W
 *              +---------------+-------+-------+-------+-------+
 *              | Bits 15 - 4   | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
 *              +---------------+-------+-------+-------+-------+
 *              | Reserved Bits |  Defrag State | Resume| Clip  |
 *              +---------------+---------------+-------+-------+
 *
 * Defrag State: 00 - PREERASE_ONGOING,
 *               01 - DATAMOVE_ONGOING,
 *               10 - FATWRITE_ONGOING,
 *               11 - FATCLEAR_ONGOING
 * Resume :      0 - Resume Address doesn't have Valid Data
 *               1 - Resume address has a valid data
 * Clip :        0 - Clip Area doesn't have valid data
 *               1 - Clip Area has a valid data
 *
 * \return       status_u16 : Encoded Status to be written in Defrag Header
 *
 */
static uint16 rba_EaFs2_Prv_EncodeDefragStatus ( void )
{
    /* Reset Status Word to zero */
    uint16 status_u16 = 0u;

    if(FALSE != rba_EaFs2_Prv_DefragInfo_st.isClipUsed_b)
    {
        /* Clip has valid data set Bit 0 */
        status_u16 |= RBA_EAFS2_PRV_DEFRAG_STATUS_CLIP_VALID;
    }

    if(FALSE != rba_EaFs2_Prv_DefragInfo_st.isDataWritteninResume_b)
    {
        /* Resume address has valid data set Bit 1 */
        status_u16 |= RBA_EAFS2_PRV_DEFRAG_STATUS_RESUME_VALID;
    }

    status_u16 |= ((uint16)(rba_EaFs2_Prv_DefragInfo_st.status_u8) << 2u);

    /* Masking to leave the reserved bits unused */
    status_u16 &= 0x000Fu;

    return status_u16;
}

#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE */

/**
 * \brief       This function extracts the information about the split block due to interrupted Defrag
 *              Sets the interrupted Defrag Flag to TRUE, so that a special handling is ensured
 *
 * \param       block_pcst  -  Pointer to the block which is interrupted during a movement
 */
void rba_EaFs2_Prv_UpdateSplitAccess(rba_EaFs2_Prv_Block_tst const * block_pcst)
{
    /* The Split structure will have 3 indices
     * 0 : Corresponds to Source information (Start Addess and Length of valid data)
     * 1 : Corresponds to Clip information   (Address of Clip and Length of valid data in Clip)
     * 2 : Corresponds to Resume information (resume address and length of valid data in resume)
     * */
    /* Update Source address, source length, clip address and resume address */
    rba_EaFs2_Prv_Split_st[0].len_u16  = rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16;
    rba_EaFs2_Prv_Split_st[0].addr_u32 = block_pcst->address_u32;
    rba_EaFs2_Prv_Split_st[1].addr_u32 = (EA_RB_EEP_SIZE - RBA_EAFS2_PRV_CLIPBOARD_SIZE);
    rba_EaFs2_Prv_Split_st[2].addr_u32 = rba_EaFs2_Prv_DefragBlk_st.resumeAddr_u32;

    if(0u != (rba_EaFs2_Prv_DefragBlk_st.status_u16 & RBA_EAFS2_PRV_DEFRAG_STATUS_CLIP_VALID))
    {
        rba_EaFs2_Prv_Split_st[1].len_u16 = rba_EaFs2_Prv_SizeInMedium(block_pcst)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       - rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16;

        if(0u != (rba_EaFs2_Prv_DefragBlk_st.status_u16 & RBA_EAFS2_PRV_DEFRAG_STATUS_RESUME_VALID))
        {
            /* Valid data is available in both Clip and Resume address */
            rba_EaFs2_Prv_Split_st[1].len_u16 %= RBA_EAFS2_PRV_CLIPBOARD_SIZE;

            if(0u == rba_EaFs2_Prv_Split_st[1].len_u16)
            {
                rba_EaFs2_Prv_Split_st[1].len_u16 = RBA_EAFS2_PRV_CLIPBOARD_SIZE;
            }

            rba_EaFs2_Prv_Split_st[2].len_u16 = rba_EaFs2_Prv_SizeInMedium(block_pcst)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    - rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16 -  rba_EaFs2_Prv_Split_st[1].len_u16 ;
        }
        else
        {
            /* Clip Falg is Set, Resume is not set, Valid data is available in the Clip region only */

            rba_EaFs2_Prv_Split_st[2].len_u16  = 0u;
        }
    }
    else
    {
        /* Only Resume Address has valid data*/
        rba_EaFs2_Prv_Split_st[1].len_u16  = 0u;

        rba_EaFs2_Prv_Split_st[2].len_u16 = rba_EaFs2_Prv_SizeInMedium(block_pcst)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                - rba_EaFs2_Prv_DefragBlk_st.lengthInSrc_u16;
    }

    /* Power Interrupted De-frag */
    rba_EaFs2_Prv_DefragInfo_st.isPwrInterrupted_b = (boolean)TRUE;

    /* Saving the block details of block under movement to resume normal defrag operation */
    rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.address_u32       = block_pcst->address_u32;
    rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.length_u16        = block_pcst->length_u16;
    rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.persistentId_u16  = block_pcst->persistentId_u16;
    rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.doublestorage_b   = block_pcst->doublestorage_b;
    rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.survival_b        = block_pcst->survival_b;
    rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.blockNumber_u16   = block_pcst->blockNumber_u16;
    rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.isInConfig_b      = block_pcst->isInConfig_b;

}

#if((EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE) && (EA_RB_ENTER_STOP_MODE != FALSE))

/**
 *********************************************************************
 * rba_EaFs2_Prv_UnallocblkSpaceCheck
 *
 * This function checks the unallocated block space based on the
 * value triggers the Forced Defragmentation
 * \param    None
 * \return   None
 * \seealso
 * \usedresources
 *********************************************************************
 */

void rba_EaFs2_Prv_UnallocBlkSpaceCheck(void)
{
    uint16 BlkNumoffset,tempEablkUnchecked_u16;
    uint32 tempUnAllocblkSpace_u32, freeSpace_u32;

    switch(rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocBlkSpace_en)
    {
        case RBA_EAFS2_PRV_UNALLOCBLKSPACE_CALC_E:
            BlkNumoffset = rba_EaFs2_Prv_UnAllocBlkSpace_st.blkChecklimit_u16;

            tempEablkUnchecked_u16 = (EA_NUM_BLOCKS - rba_EaFs2_Prv_UnAllocBlkSpace_st.blkChecklimit_u16);

            if( tempEablkUnchecked_u16 <= RBA_EAFS2_PRV_BLKCHECKLIMIT)
            {
                rba_EaFs2_Prv_UnAllocBlkSpace_st.blkChecklimit_u16 += tempEablkUnchecked_u16;
                rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocBlkSpace_en = RBA_EAFS2_PRV_TRIGGERFORCEDDEFRAG_E;
            }
            else
            {
                rba_EaFs2_Prv_UnAllocBlkSpace_st.blkChecklimit_u16 += RBA_EAFS2_PRV_BLKCHECKLIMIT ;
                rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocBlkSpace_en = RBA_EAFS2_PRV_UNALLOCBLKSPACE_CALC_E;
            }/* Blk Number starts from zero it is not necessary to calculate space when blk number value equals to EA_NUM_BLOCKS */
            tempUnAllocblkSpace_u32 = rba_EaFs2_Prv_UnallocBlkSpaceCalc(BlkNumoffset, rba_EaFs2_Prv_UnAllocBlkSpace_st.blkChecklimit_u16);
            rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocblkSpace_u32 += tempUnAllocblkSpace_u32;
            break;

        case RBA_EAFS2_PRV_TRIGGERFORCEDDEFRAG_E:
            freeSpace_u32 = rba_EaFs2_Prv_GetFreeSpace();
            if(freeSpace_u32 >= rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocblkSpace_u32)
            {
                SchM_Enter_Ea_Order();
                if(Ea_Rb_MigrationStaus_u8 == EA_RB_MIGRATION_FORCEDRUNNING)
                {
                    /* Reset the used variables and Setting status as EA_RB_MIGRATION_RUNNING will
                    End the forced Migration Operation.
                    Further defragmentation should not be continued if continued it will leads to corruption
                    because rba_FeeAddOnAS and Ea module both will access Eep module which will leads to corruption */
                    Ea_Rb_MigrationStaus_u8 = EA_RB_MIGRATION_RUNNING;
                    rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocBlkSpace_en = RBA_EAFS2_PRV_UNALLOCBLKSPACE_CALC_E;
                    rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocblkSpace_u32 = 0;
                    rba_EaFs2_Prv_UnAllocBlkSpace_st.blkChecklimit_u16 = 0;
                    rba_EaFs2_Prv_DefragInfo_st.isForcedDefrag_b = FALSE;
                    rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_IDLE_E;
                }
                SchM_Exit_Ea_Order();
            }
            else
            {
                if(rba_EaFs2_Prv_FsInfo_st.totalGaps_u16 > 0u)
                {
                    rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_BACKGROUND_E;
                    rba_EaFs2_Prv_DefragInfo_st.isForcedDefrag_b = TRUE;
                }
                else
                {
                    /* Enough space is not there in Eeprom to complete the defrag.
                      This will set Migration status to Error which will stop forced Defrag */
                    Ea_Rb_CheckDefragErr(EA_SID_RB_FORCEDMIGRATION,EA_E_RB_MIGRATION_NOT_POSSIBLE);
                }
            }
            break;

        default:
        {
            SchM_Enter_Ea_Order();
            Ea_Rb_MigrationStaus_u8 = EA_RB_MIGRATION_ERROR;
            SchM_Exit_Ea_Order();
        }   break;
    }
}


/**
 *********************************************************************
 * rba_EaFs2_Prv_UnallocblkSpaceCalc
 *
 * This function Calculates and returns the space needed for unallocated
 * blocks
 * \param    BlkNumoffset : Initial value of the block number
 * \param    BlkNumMax : Max value of the block number
 * \retval   returns the calculated space
 * \seealso
 * \usedresources
 *********************************************************************
 */

static uint32 rba_EaFs2_Prv_UnallocBlkSpaceCalc(uint16 BlkNumoffset, uint16 BlkNumMax)
{
    uint16 blockNr_u16;
    uint32 unAllocblkSpace_u32 = 0, blkRealLength_u32,blkLengthTemp_u32;
    /*  rba_EaFs2_Prv_RamTable_st array starts with zero due to that we are not checking blockNr_u16 == BlkNumMax case */
    for(blockNr_u16 = BlkNumoffset;blockNr_u16 < BlkNumMax; blockNr_u16++)
    {
        if(0u == rba_EaFs2_Prv_RamTable_st[blockNr_u16].address_u32)
        {
            blkLengthTemp_u32 = rba_EaFs2_Prv_BlkLengthInMedium(blockNr_u16);
            if(FALSE != rba_EaFs2_Prv_IsBlockDblStorage(blockNr_u16))
            {
                blkLengthTemp_u32 = blkLengthTemp_u32 * 2u;
            }
            blkRealLength_u32 = blkLengthTemp_u32 + (2u * RBA_EAFS2_PRV_FAT_SIZE);
            unAllocblkSpace_u32 += blkRealLength_u32;
        }
    }
    return unAllocblkSpace_u32;
}

#endif /* ((EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE) && (EA_RB_ENTER_STOP_MODE != FALSE)) */

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
/**
 *********************************************************************
 *  Ea_Rb_MigrationStatus(): Returns the Migration status to appliation layer.
 *  This is Not applicable to boot mode.
 * \param    None
 * \return  return Migration Status
 * \retval  EA_RB_MIGRATION_NOTNEEDED 0x00
 *          EA_RB_MIGRATION_RUNNING   0x01
 *          EA_RB_MIGRATION_FINISHED  0x02
 *          EA_RB_MIGRATION_ERROR     0x03
 *          EA_RB_MIGRATION_FORCEDRUNNING  0x04
 *
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint8 Ea_Rb_MigrationStatus(void)
{
    return Ea_Rb_MigrationStaus_u8;
}

#if (EA_RB_ENTER_STOP_MODE != FALSE)
/**
 *********************************************************************
 *  Ea_Rb_ForcedMigration(): This function triggers a Forced Defragmentation
 *  to create space for unallocated block.
 *  This request should be accepted only in Stop Mode
 * \param    None
 * \return   Order acceptance status
 * \retval   E_OK:      Order was accepted
 * \retval   E_NOT_OK:  Order was not accepted
 * \seealso
 * \usedresources
 *********************************************************************
 */

Std_ReturnType Ea_Rb_ForcedMigration(void)
{
    Std_ReturnType result_en = E_NOT_OK;

    if(E_OK == Ea_Rb_CheckInit(EA_SID_RB_FORCEDMIGRATION))
    {
        if(RBA_EAFS2_PRV_STOPMODE_ACTIVATED == rba_EaFs2_Prv_StopMode_en)
        {
            result_en = E_OK;
            SchM_Enter_Ea_Order();
            if(Ea_Rb_MigrationStaus_u8 == EA_RB_MIGRATION_RUNNING)
            {
                /* Locking is done since accessing the variable in User request and in Background task.
                   Technically Interrupt lock not need for migration status variable for forced migration
                   use case, Locking is added to avoid unnecessary analysis if we update migration status */
                Ea_Rb_MigrationStaus_u8 = EA_RB_MIGRATION_FORCEDRUNNING;
            }
            SchM_Exit_Ea_Order();
        }
    }
    return result_en;
}
#endif /* EA_RB_ENTER_STOP_MODE != FALSE */
#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE */


#if(FALSE != EA_PRV_DEBUGGING)
/**
 *********************************************************************
 * rba_EaFs2_Prv_Initialize_Defrag():
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
void rba_EaFs2_Prv_Initialize_Defrag(void)
{
     uint8 index_u8 = 0u;
     rba_EaFs2_Prv_DefragInfo_st.state_en = RBA_EAFS2_PRV_DEFRAG_INIT_E;
     rba_EaFs2_Prv_DefragInfo_st.previousValidBlock_st.address_u32 =0u;
     rba_EaFs2_Prv_DefragInfo_st.previousValidBlock_st.persistentId_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.previousValidBlock_st.length_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.previousValidBlock_st.blockNumber_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.previousValidBlock_st.doublestorage_b =FALSE;
     rba_EaFs2_Prv_DefragInfo_st.previousValidBlock_st.survival_b =FALSE;
     rba_EaFs2_Prv_DefragInfo_st.previousValidBlock_st.isInConfig_b =FALSE;

     rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.address_u32 =0u;
     rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.persistentId_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.length_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.blockNumber_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.doublestorage_b =FALSE;
     rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.survival_b =FALSE;
     rba_EaFs2_Prv_DefragInfo_st.nextValidBlock_st.isInConfig_b =FALSE;

     rba_EaFs2_Prv_DefragInfo_st.fatUnderRead_st.address_u32= 0u;
     rba_EaFs2_Prv_DefragInfo_st.fatUnderRead_st.length_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.fatUnderRead_st.ID_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.fatUnderRead_st.status_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.fatUnderRead_st.crc_u16=0u;

     rba_EaFs2_Prv_DefragInfo_st.blockUnderRead_pst = NULL_PTR;


     rba_EaFs2_Prv_DefragInfo_st.blockToUpdate_st.address_u32 =0u;
     rba_EaFs2_Prv_DefragInfo_st.blockToUpdate_st.persistentId_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.blockToUpdate_st.length_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.blockToUpdate_st.blockNumber_u16 =0u;
     rba_EaFs2_Prv_DefragInfo_st.blockToUpdate_st.doublestorage_b =FALSE;
     rba_EaFs2_Prv_DefragInfo_st.blockToUpdate_st.survival_b =FALSE;
     rba_EaFs2_Prv_DefragInfo_st.blockToUpdate_st.isInConfig_b =FALSE;

     rba_EaFs2_Prv_DefragInfo_st.readAddress_u32=0u;
     rba_EaFs2_Prv_DefragInfo_st.writeAddress_u32=0u;
     rba_EaFs2_Prv_DefragInfo_st.lengthToMove_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.gapLength_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.srcLength_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.workingGapIdx_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.workingNxtValidIdx_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.firstGapIdx_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.lastGapIdx_u16=0u;
     rba_EaFs2_Prv_DefragInfo_st.status_u8=0u;
     rba_EaFs2_Prv_DefragInfo_st.isClipUsed_b=FALSE;
     rba_EaFs2_Prv_DefragInfo_st.isDataWritteninResume_b=FALSE;
     rba_EaFs2_Prv_DefragInfo_st.isPwrInterrupted_b=FALSE;
     rba_EaFs2_Prv_DefragInfo_st.isDefragHdrRepairNeeded_b=FALSE;
     rba_EaFs2_Prv_DefragInfo_st.readFatUpGapFound_b=FALSE;
     rba_EaFs2_Prv_DefragInfo_st.isForcedDefrag_b=FALSE;
     rba_EaFs2_Prv_DefragInfo_st.hasLastGapInfo_b=FALSE;
     rba_EaFs2_Prv_DefragInfo_st.entry_b=FALSE;

     for(index_u8 = 0;index_u8 < 3 ; index_u8 ++){
         rba_EaFs2_Prv_Split_st[index_u8].addr_u32= 0u;
         rba_EaFs2_Prv_Split_st[index_u8].len_u16=0u;
     }

#if((EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE) && (EA_RB_ENTER_STOP_MODE != FALSE))
    rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocBlkSpace_en=RBA_EAFS2_PRV_UNALLOCBLKSPACE_CALC_E;
    rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocblkSpace_u32=0u;
    rba_EaFs2_Prv_UnAllocBlkSpace_st.blkChecklimit_u16=0u;
#endif

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
    Ea_Rb_MigrationStaus_u8 = 0u;
#endif

}
#endif

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* EA_RB_CFG_SELECTED_FS */
