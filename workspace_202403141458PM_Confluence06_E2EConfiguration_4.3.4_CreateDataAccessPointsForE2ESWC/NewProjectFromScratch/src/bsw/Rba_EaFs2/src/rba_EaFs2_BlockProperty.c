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
#include "Ea_Prv.h"

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
rba_EaFs2_Prv_BlockPropertyInfor_tst rba_EaFs2_Prv_BlockPropertyInfo_st;
rba_EaFs2_Prv_PropertyChange_tst rba_EaFs2_Prv_PropertyChange_st;
#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"


#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

static MemIf_JobResultType rba_EaFs2_Prv_BlockPropertyUpdate_Init(rba_EaFs2_Prv_BlockPropertyInfor_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_BlockPropertyUpdate_WriteFAT(rba_EaFs2_Prv_BlockPropertyInfor_tst * fsm_pst);

static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_Init(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_FindNextBlock (rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_GetFreeSpace (rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_MetaRead (rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_DataRead(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_DataWrite(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_DataVerify(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_WriteFat(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_ClearFat(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst);
/**
 * \brief   The function perform is called to perform block property update -> Only non survival to survival is possible.
 *
 * \param   rba_EaFs2_Prv_BlockPropertyInfor_tst - pointer to block property info structure
 *
 * \return  MemIf_JobResultType - returns result job (MEM_IF_OK or MEM_IF_FAILED)
 *
 */
MemIf_JobResultType rba_EaFs2_Prv_BlockPropertyUpdateDo(rba_EaFs2_Prv_BlockPropertyInfor_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    fsm_pst->state_en = RBA_EAFS2_PRV_BLOCKPROPERTY_INIT_E;
    do
    {
        switch(fsm_pst->state_en)
        {
         case RBA_EAFS2_PRV_BLOCKPROPERTY_INIT_E:          result_en = rba_EaFs2_Prv_BlockPropertyUpdate_Init           (fsm_pst);  break;
         case RBA_EAFS2_PRV_BLOCKPROPERTY_WRITE_FAT_E:     result_en = rba_EaFs2_Prv_BlockPropertyUpdate_WriteFAT       (fsm_pst);  break;
         default:                                          result_en = MEMIF_JOB_FAILED;                                      break;
        }
    }while(
       (MEMIF_JOB_OK == result_en) &&
       (RBA_EAFS2_PRV_BLOCKPROPERTY_FINAL_E != fsm_pst->state_en)
    );

    return result_en;
}

/**
 * \brief   The function perform is called to initialize the information before moving the property changed block
 *
 * \param   rba_EaFs2_Prv_BlockPropertyInfor_tst - pointer to block property info structure
 *
 * \return  MemIf_JobResultType - returns result job (MEM_IF_OK or MEM_IF_FAILED)
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_BlockPropertyUpdate_Init(rba_EaFs2_Prv_BlockPropertyInfor_tst * fsm_pst)
{
    uint16 blockNr_u16 = fsm_pst->blockToUpdate_st.blockNumber_u16;

    fsm_pst->srcIdx_u16                          = rba_EaFs2_Prv_RamTable_st[blockNr_u16].fatIndex_u16;
    fsm_pst->blockToUpdate_st.survival_b         = TRUE; /* only allow change from non-survival to survival */
    fsm_pst->entry_b = TRUE;
    fsm_pst->state_en = RBA_EAFS2_PRV_BLOCKPROPERTY_WRITE_FAT_E;

    return MEMIF_JOB_OK;
}


/**
 * \brief   this function is called to write FAT
 *          because block is moved then the FAT must also updated
 *
 * \param   rba_EaFs2_Prv_BlockPropertyInfor_tst - pointer to block property info structure
 *
 * \return  MemIf_JobResultType - returns result job (MEM_IF_OK or MEM_IF_FAILED)
 *
 */

static MemIf_JobResultType rba_EaFs2_Prv_BlockPropertyUpdate_WriteFAT(rba_EaFs2_Prv_BlockPropertyInfor_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
       /* update fat for block property change to available space */
       rba_EaFs2_Prv_FatWrite(&fsm_pst->blockToUpdate_st, fsm_pst->srcIdx_u16);
    }

    /* Worker function to do the FAT entry write in the Medium */
    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->state_en = RBA_EAFS2_PRV_BLOCKPROPERTY_FINAL_E;
    }
    return result_en;
}


MemIf_JobResultType rba_EaFs2_Prv_PropertyChangeDo ( void )
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;
    rba_EaFs2_Prv_PropertyChange_tst * fsm_pst = &rba_EaFs2_Prv_PropertyChange_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_PROPERTYCHANGE_INIT_E:               result_en = rba_EaFs2_Prv_PropertyChange_Init(fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_FINDNEXTBLOCK_E:      result_en = rba_EaFs2_Prv_PropertyChange_FindNextBlock (fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_GETFREESPACE_E:       result_en = rba_EaFs2_Prv_PropertyChange_GetFreeSpace (fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_METAREAD_E:           result_en = rba_EaFs2_Prv_PropertyChange_MetaRead (fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_DATAREAD_E:           result_en = rba_EaFs2_Prv_PropertyChange_DataRead(fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_DATAWRITE_E:          result_en = rba_EaFs2_Prv_PropertyChange_DataWrite(fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_DATAVERIFY_E:         result_en = rba_EaFs2_Prv_PropertyChange_DataVerify(fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_WRITEFAT_E:           result_en = rba_EaFs2_Prv_PropertyChange_WriteFat(fsm_pst); break;
            case RBA_EAFS2_PRV_PROPERTYCHANGE_CLEARFAT_E:           result_en = rba_EaFs2_Prv_PropertyChange_ClearFat(fsm_pst); break;
            default:                                                result_en = MEMIF_JOB_FAILED;                           break;
        }
    }while(
            (MEMIF_JOB_OK == result_en) &&
            (RBA_EAFS2_PRV_PROPERTYCHANGE_FINAL_E != fsm_pst->state_en) &&
            (FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
    );

    if(
            (FALSE != rba_EaFs2_Prv_FsInfo_st.suspendBgo_b) &&
            (MEMIF_JOB_PENDING != result_en)
    )
    {
        /* Interrupted by User Request, Cancel accepted, clear cancel flag */
        rba_EaFs2_Prv_FsInfo_st.suspendBgo_b = FALSE;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_INIT_E);
    }

    if((RBA_EAFS2_PRV_PROPERTYCHANGE_FINAL_E == fsm_pst->state_en) || (result_en == MEMIF_JOB_FAILED))
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_INIT_E);
    }
    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_Init (rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
{
    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_FINDNEXTBLOCK_E);
    return MEMIF_JOB_OK;
}

static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_FindNextBlock (rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
{
    /* iterate through the RAM table and find the next security level changed block */
    uint16 blockNr_u16            = 0;

    for(blockNr_u16 = 0; blockNr_u16 < EA_NUM_BLOCKS; blockNr_u16++)
    {
        if(rba_EaFs2_Prv_RamTable_st[blockNr_u16].isSecurityLevelChanged_b != (boolean)FALSE)
        {
            fsm_pst->dataBlockAddress_u32       = rba_EaFs2_Prv_RamTable_st[blockNr_u16].address_u32;
            fsm_pst->workingFatIndex_u16        = rba_EaFs2_Prv_RamTable_st[blockNr_u16].fatIndex_u16;
            fsm_pst->block_st.blockNumber_u16   = blockNr_u16;
            fsm_pst->block_st.persistentId_u16  = rba_EaFs2_Prv_GetPersistentID   (blockNr_u16);
            fsm_pst->block_st.doublestorage_b   = rba_EaFs2_Prv_IsBlockDblStorage (blockNr_u16);
            fsm_pst->block_st.survival_b        = rba_EaFs2_Prv_IsBlockSurvival   (blockNr_u16);
            fsm_pst->block_st.length_u16        = Ea_BlockProperties_st[blockNr_u16].Ea_BlkLength_u16;
            fsm_pst->dataBlockLength_u16        = rba_EaFs2_Prv_BlkLengthInMedium(blockNr_u16);
            fsm_pst->block_st.isInConfig_b      = (boolean)FALSE;  /* No need to update the RAM table centrally */
            fsm_pst->curCopy_u8                 = (uint8)1;

            if(rba_EaFs2_Prv_RamTable_st[blockNr_u16].isSurvivalFlagChanged_b != (boolean)FALSE)
            {
                /* Block converted from survival to non-survival in config, treat as survival block forever
                 * Once Survival block, stays survival for lifetime */
                fsm_pst->block_st.survival_b =   !fsm_pst->block_st.survival_b;
            }
            break;
        }
    }

    fsm_pst->readAddress_u32 = fsm_pst->dataBlockAddress_u32;
    fsm_pst->srcLength_u16 = fsm_pst->dataBlockLength_u16;
    /* Calculate the length to move in this cycle */
    fsm_pst->lengthToMove_u16 = (uint16)rba_EaFs2_Prv_LibMin(
            fsm_pst->srcLength_u16,
            EA_DATA_BUFFER_SIZE
    );

    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_GETFREESPACE_E);

    return MEMIF_JOB_OK;
}

static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_GetFreeSpace (rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint32 freespace_u32;
    uint32 requiredlength_u32;
    uint8 numberOfCopiesToWrite_u8;

    if(fsm_pst->block_st.doublestorage_b != (boolean)FALSE)
    {
        numberOfCopiesToWrite_u8 = 2;
    }
    else
    {
        numberOfCopiesToWrite_u8 = 1;
    }

    freespace_u32 = rba_EaFs2_Prv_GetFreeSpace();
    requiredlength_u32 = ((fsm_pst->dataBlockLength_u16)*(numberOfCopiesToWrite_u8)) + (2u*sizeof(rba_EaFs2_Prv_FatEntry_tst));

    if (freespace_u32 > requiredlength_u32)
    {
        /* Space available for block alocation, proceed for property change */
        fsm_pst->targetFatIndex_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;
        fsm_pst->writeAddress_u32     = rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 - (fsm_pst->dataBlockLength_u16*(numberOfCopiesToWrite_u8));
        fsm_pst->block_st.address_u32 = fsm_pst->writeAddress_u32;
        if(fsm_pst->block_st.doublestorage_b != (boolean)FALSE)
        {
            /* SL1 to SL2 Start moving, no need to validate the Meta */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_DATAREAD_E);
        }
        else
        {
            /* SL2 to SL1 read meta for valid copy and then start moving */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_METAREAD_E);
        }
    }
    else
    {
        /* Space not available, check if defrag could help to freeup space */
        if(rba_EaFs2_Prv_FsInfo_st.propertyChangeGaps_u16 > 0u)
        {
            /* Update Total Gaps, reset current state machine */
            rba_EaFs2_Prv_FsInfo_st.totalGaps_u16 = rba_EaFs2_Prv_FsInfo_st.propertyChangeGaps_u16;
            rba_EaFs2_Prv_FsInfo_st.propertyChangeGaps_u16 = 0u;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_FINAL_E);
        }
        else
        {
            /* Space not available, defrag not possible, report error */
            result_en = MEMIF_JOB_FAILED;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_FINAL_E);
        }
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_MetaRead (rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataReadMetaData(
                &fsm_pst->metaBlock_st,
                fsm_pst->readAddress_u32,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        rba_EaFs2_Prv_DecodeMetaData(&fsm_pst->metaBlock_st);

        if((boolean)FALSE == fsm_pst->metaBlock_st.isConsistent_b)
        {
            /* SL2 to SL1 use second copy, since first copy is invalid */
            fsm_pst->readAddress_u32 += fsm_pst->dataBlockLength_u16;
        }

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_DATAREAD_E);
    }

    return result_en;
}


static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_DataRead(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
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
        /* Read completed, proceed for data write */
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_DATAWRITE_E);
    }

    return result_en;
}


static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_DataWrite(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
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
        /* Write completed proceed for data Verify */
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_DATAVERIFY_E);
    }

    return result_en;
}


static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_DataVerify(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
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
        /* Current chunk has been moved successfully */
        fsm_pst->srcLength_u16 -= fsm_pst->lengthToMove_u16;
        fsm_pst->writeAddress_u32 += fsm_pst->lengthToMove_u16;

        if(fsm_pst->srcLength_u16 == 0)
        {
            /* if SL1 to SL2 initiate second copy move operation */
            if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
            {
                /* SL2 to SL1 - proceed for FAT write */
                /* Data movement completed, proceed for FAT operations */
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_WRITEFAT_E);
            }
            else
            {
                /* SL1 to SL2 - Check if a second copy needs to be written */
                if(fsm_pst->curCopy_u8 == 1)
                {
                    fsm_pst->curCopy_u8++;

                    /* re-initate data movement process
                     * prepare read Address
                     * srcLength_u16
                     * lengthToMove
                     * Write address alread incremented */
                    fsm_pst->srcLength_u16 = fsm_pst->dataBlockLength_u16;
                    fsm_pst->readAddress_u32 = fsm_pst->dataBlockAddress_u32;
                    fsm_pst->lengthToMove_u16 = (uint16)rba_EaFs2_Prv_LibMin(
                            fsm_pst->srcLength_u16,
                            EA_DATA_BUFFER_SIZE
                    );

                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_DATAREAD_E);
                }
                else
                {
                    /* Data movement completed, reset copy and proceed for FAT operations */
                    fsm_pst->curCopy_u8 = 1;
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_WRITEFAT_E);
                }
            }
        }
        else
        {
            /* continue data movement process
             * prepare read Address
             * lengthToMove
             * Write address alread incremented */
            fsm_pst->readAddress_u32 += fsm_pst->lengthToMove_u16;
            fsm_pst->lengthToMove_u16 = (uint16)rba_EaFs2_Prv_LibMin(
                    fsm_pst->srcLength_u16,
                    EA_DATA_BUFFER_SIZE
            );

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_DATAREAD_E);
        }
    }

    return result_en;
}


static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_WriteFat(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_FatWrite(&fsm_pst->block_st,fsm_pst->targetFatIndex_u16);
    }

    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        /* New Fat written to the medium */
        rba_EaFs2_Prv_FatBlockClear(&fsm_pst->blockToUpdate_st);
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_CLEARFAT_E);
    }

    return result_en;
}



static MemIf_JobResultType rba_EaFs2_Prv_PropertyChange_ClearFat(rba_EaFs2_Prv_PropertyChange_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Caller function to initiate the FAT entry write */
        rba_EaFs2_Prv_FatWrite(&fsm_pst->blockToUpdate_st,fsm_pst->workingFatIndex_u16);
    }

    /* Worker function to do the FAT entry write in the Medium */
    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        /* RAM table must be updated*/
        rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].fatIndex_u16 = fsm_pst->targetFatIndex_u16;
        rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].address_u32  = fsm_pst->block_st.address_u32;
        rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].isSecurityLevelChanged_b = FALSE;

        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 += (uint16)2;
        rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 = fsm_pst->block_st.address_u32;

        if(rba_EaFs2_Prv_FsInfo_st.propertyChangeGaps_u16 == 0)
        {
            rba_EaFs2_Prv_DefragInfo_st.firstGapIdx_u16 = fsm_pst->workingFatIndex_u16;
        }
        else
        {
            /* Look for the least gap index */
            rba_EaFs2_Prv_DefragInfo_st.firstGapIdx_u16 = (uint16)rba_EaFs2_Prv_LibMin(
                    rba_EaFs2_Prv_DefragInfo_st.firstGapIdx_u16,
                    fsm_pst->workingFatIndex_u16
            );
        }

        rba_EaFs2_Prv_FsInfo_st.propertyChangeGaps_u16++;
        rba_EaFs2_Prv_FsInfo_st.securityLevelChangedBlocks_u16--;

        if(rba_EaFs2_Prv_FsInfo_st.securityLevelChangedBlocks_u16 == 0)
        {
            /* Property change handling completed, trigger defrag to free up the gaps */
            rba_EaFs2_Prv_FsInfo_st.totalGaps_u16 = rba_EaFs2_Prv_FsInfo_st.propertyChangeGaps_u16;
            rba_EaFs2_Prv_FsInfo_st.propertyChangeGaps_u16 = 0;
        }

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_PROPERTYCHANGE_FINAL_E);

    }

    return result_en;
}

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif
