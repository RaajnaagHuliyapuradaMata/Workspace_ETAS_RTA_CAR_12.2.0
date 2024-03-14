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
#include "Crc.h"
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

rba_EaFs2_Prv_Bgo_tst              rba_EaFs2_Prv_Bgo_st;
rba_EaFs2_Prv_Read_tst      rba_EaFs2_Prv_Read_st;
rba_EaFs2_Prv_Write_tst     rba_EaFs2_Prv_Write_st;

MemIf_Rb_DetailedBlkInfo_ten    rba_EaFs2_Prv_ReadJobResultDetailed_aen[EA_NUM_BLOCKS];

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
rba_EaFs2_Prv_FatRepair_tst rba_EaFs2_Prv_FatRepair_st;
rba_EaFs2_Prv_TempMarker_tst rba_EaFs2_Prv_TempMarker_st;
#endif

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

static MemIf_JobResultType rba_EaFs2_Prv_WriteInit            (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteRepair          (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteAllocSpace      (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteGetCRC          (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteReadMeta        (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteCompare         (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteOpenMetaData    (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteCloseMetaData   (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteVerify          (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteInvalidate      (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteDataWrite       (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteFatWrite        (rba_EaFs2_Prv_Write_tst * fsm_pst);
static void                rba_EaFs2_Prv_Write2ndCopy         (rba_EaFs2_Prv_Write_tst * fsm_pst);
static void                rba_EaFs2_Prv_CopyDataToBuffer     (rba_EaFs2_Prv_Read_tst const * fsm_pst);

#if (EA_WRITE_SPREAD != FALSE)
static void                rba_EaFs2_Prv_WriteCounter2ndCopy  (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteUpdateCounter   (rba_EaFs2_Prv_Write_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_WriteGetLocation     (rba_EaFs2_Prv_Write_tst * fsm_pst);
#endif /*(EA_WRITE_SPREAD != FALSE)*/

static MemIf_JobResultType rba_EaFs2_Prv_ReadInit             (rba_EaFs2_Prv_Read_tst  * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_ReadReadMetaData     (rba_EaFs2_Prv_Read_tst  * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_ReadVerifyCrc        (rba_EaFs2_Prv_Read_tst  * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_ReadPayLoad          (rba_EaFs2_Prv_Read_tst  * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_ReadFailHandle       (rba_EaFs2_Prv_Read_tst  * fsm_pst);
#if (EA_WRITE_SPREAD != FALSE)
static MemIf_JobResultType rba_EaFs2_Prv_ReadGetLocation       (rba_EaFs2_Prv_Read_tst  * fsm_pst);
#endif /*(EA_WRITE_SPREAD != FALSE)*/

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
static MemIf_JobResultType rba_EaFs2_Prv_BgoIdle              (rba_EaFs2_Prv_Bgo_tst   * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_BgoInit              (rba_EaFs2_Prv_Bgo_tst   * fsm_pst);
#endif
static MemIf_JobResultType rba_EaFs2_Prv_BgoGetSpace          (rba_EaFs2_Prv_Bgo_tst   * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_BgoEmptyMeta         (rba_EaFs2_Prv_Bgo_tst   * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_BgoWriteFat          (rba_EaFs2_Prv_Bgo_tst   * fsm_pst);
#if (EA_WRITE_SPREAD != FALSE)
static MemIf_JobResultType rba_EaFs2_Prv_BgoClearCounter      (rba_EaFs2_Prv_Bgo_tst   * fsm_pst);
#endif /*(EA_WRITE_SPREAD != FALSE)*/

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)

static void rba_EaFs2_Prv_BgoSelector( void );

static MemIf_JobResultType rba_EaFs2_Prv_BgFatAlloc(void);

static MemIf_JobResultType rba_EaFs2_Prv_FatRepairInit       (rba_EaFs2_Prv_FatRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairEraseFat  (rba_EaFs2_Prv_FatRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairWriteMarker(rba_EaFs2_Prv_FatRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairCompareMarker(rba_EaFs2_Prv_FatRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairDo(void);

static MemIf_JobResultType rba_EaFs2_Prv_TempMarker(void);
static MemIf_JobResultType rba_EaFs2_Prv_TempMarker_Init(rba_EaFs2_Prv_TempMarker_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_TempMarker_Write(rba_EaFs2_Prv_TempMarker_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_TempMarker_Compare(rba_EaFs2_Prv_TempMarker_tst * fsm_pst);

static MemIf_JobResultType rba_EaFs2_Prv_FatRepairReadInit(rba_EaFs2_Prv_FatRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairRead(rba_EaFs2_Prv_FatRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairWrite(rba_EaFs2_Prv_FatRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairCompareWrite(rba_EaFs2_Prv_FatRepair_tst * fsm_pst);


#endif

/**
 * \brief    Trigger function to start the write/invalidate state machine.
 *
 * \param    isInvalidate_b  flag to distinguish invalidate/write operations.
 *
 */
void rba_EaFs2_Write(boolean isInvalidate_b)
{
#if(FALSE != EA_PRV_DEBUGGING)
    rba_EaFs2_Prv_DebugTimeStart(&rba_EaFs2_Debug_st.writeTime_st);
#endif

    rba_EaFs2_Prv_Write_st.order_pcst            = &Ea_Prv_Order_st;
    rba_EaFs2_Prv_Write_st.retry_u8              = 3;
    rba_EaFs2_Prv_Write_st.doInvalidate_b        = isInvalidate_b;
    rba_EaFs2_Prv_Write_st.calculatedCrc_u32     = EA_MARKER_PATTERN;
    rba_EaFs2_Prv_Write_st.crcCalcLengthDone_u16 = 0u;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Write_st,RBA_EAFS2_PRV_WRITE_INIT_E);
}

/**
 * \brief    Trigger function to start the read state machine.
 *
 *
 */
void rba_EaFs2_Read(void)
{
#if(FALSE != EA_PRV_DEBUGGING)
    rba_EaFs2_Prv_DebugTimeStart(&rba_EaFs2_Debug_st.readTime_st);
#endif

    rba_EaFs2_Prv_Read_st.order_pcst = &Ea_Prv_Order_st;
    rba_EaFs2_Prv_Read_st.retry_u8   = 3;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Read_st,RBA_EAFS2_PRV_READ_INIT_E);
}

/**
 * \brief   Trigger function to start the read state machine.
 *
 * \return  MEMIF_JOB_OK        Read operation is completed.
 *          MEMIF_JOB_PENDING   Read operation is ongoing.
 *          MEMIF_JOB_FAILED    Read operation failed.
 */
MemIf_JobResultType rba_EaFs2_Prv_ReadDo(void)
{
    rba_EaFs2_Prv_Read_tst * fsm_pst = &rba_EaFs2_Prv_Read_st;

    /*
        1. Check if address is in Ram table
        2. Yes -> Check if item is invalidated
                  21. Yes -> return MEMIF_BLOCK_INCONSISTENT
                  22. No  -> read Data from address and check eep result status
                             221. Failed ->  Check if backup is configured.
                                             2211. No  -> return MEMIF_BLOCK_INVALID/MEMIF_BLOCK_INCONSISTENT ???
                                             2212. Yes -> read from backup address and check eep result status
                                                        22121. Success -> compute CRC for DATA and check its status
                                                                          21211. CRC Valid   ->  return Data and MEMIF_JOB_OK
                                                                          21212. CRC Invalid ->  return MEMIF_BLOCK_INCONSISTENT
                                                        22122. Failed  -> return MEMIF_BLOCK_INVALID/MEMIF_BLOCK_INCONSISTENT ???
                             222. Success -> compute CRC for DATA and check its status
                                            2221. CRC Valid   ->  return Data and MEMIF_JOB_OK
                                            2222. CRC Invalid ->  return MEMIF_BLOCK_INCONSISTENT
           No -> return  MEMIF_BLOCK_INCONSISTENT

        Discrete actions:
        1. Checking in ram table
        2. Checking if ID invalidated
        3. read from address
        4. compute crc
     */
    MemIf_JobResultType result_en;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_READ_INIT_E:        result_en = rba_EaFs2_Prv_ReadInit         (fsm_pst); break;
            case RBA_EAFS2_PRV_READ_READMETA_E:    result_en = rba_EaFs2_Prv_ReadReadMetaData (fsm_pst); break;
            case RBA_EAFS2_PRV_READ_VERIFYCRC_E:   result_en = rba_EaFs2_Prv_ReadVerifyCrc    (fsm_pst); break;
            case RBA_EAFS2_PRV_READ_READPAYLOAD_E: result_en = rba_EaFs2_Prv_ReadPayLoad      (fsm_pst); break;
            case RBA_EAFS2_PRV_READ_FAILHANDLE_E:  result_en = rba_EaFs2_Prv_ReadFailHandle   (fsm_pst); break;
#if (EA_WRITE_SPREAD != FALSE)
            case RBA_EAFS2_PRV_READ_GETLOCATION_E: result_en = rba_EaFs2_Prv_ReadGetLocation  (fsm_pst); break;
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            default:                               result_en = rba_EaFs2_Prv_LibResetRetry(&fsm_pst->retry_u8); break;
        }
    }while( (MEMIF_JOB_OK == result_en) && (fsm_pst->state_en != RBA_EAFS2_PRV_READ_FINAL_E) );

    if((MEMIF_JOB_FAILED == result_en) && (0 < fsm_pst->retry_u8))
    {
        fsm_pst->retry_u8--;
        result_en = MEMIF_JOB_PENDING;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_INIT_E);
    }

    if(MEMIF_JOB_PENDING != result_en)
    {
#if(FALSE != EA_PRV_DEBUGGING)
        rba_MemLib_UpdateTimerUs();
        rba_EaFs2_Prv_DebugTimeStop(&rba_EaFs2_Debug_st.readTime_st);
#endif

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_IDLE_E);
    }

    return result_en;
}

/**
 * \brief   Initialized the read state machine.
 *
 */

static MemIf_JobResultType rba_EaFs2_Prv_ReadInit(rba_EaFs2_Prv_Read_tst * fsm_pst)
{
    MemIf_JobResultType result_en =  MEMIF_JOB_OK;

    fsm_pst->block_st            = rba_EaFs2_Prv_BlockFromOrder(fsm_pst->order_pcst);
    fsm_pst->dataBlockLength_u16 = rba_EaFs2_Prv_BlkLengthInMedium(fsm_pst->order_pcst->blockNumber_u16);

    if(rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].isSecurityLevelChanged_b != (boolean)FALSE)
    {
        fsm_pst->block_st.doublestorage_b =   !fsm_pst->block_st.doublestorage_b;
    }
    if((boolean)FALSE != fsm_pst->block_st.doublestorage_b)
    {
        fsm_pst->currCopy_u8 = 2;
    }
    else
    {
        fsm_pst->currCopy_u8 = 1;
    }

    /* Address is found in Ram table, take this address to read from Eeprom*/
    fsm_pst->block_st.address_u32 = rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].address_u32;
    fsm_pst->fatIndex_u16         = rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].fatIndex_u16;

#if (EA_WRITE_SPREAD != FALSE)
    if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
    {
        if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32==0u)
        {
            fsm_pst->CurrentLocationFound_b = (boolean)FALSE;
            fsm_pst->CurrentLocation_u8 = 1; /* Iterate from 1 to n locations */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_GETLOCATION_E);
        }
        else
        {
            fsm_pst->block_st.address_u32 = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->block_st.blockNumber_u16)].currAddress_u32;
            if(2 == fsm_pst->currCopy_u8)
            {
                fsm_pst->addr_u32 = fsm_pst->block_st.address_u32 + fsm_pst->dataBlockLength_u16;
            }
            else
            {
                fsm_pst->addr_u32 = fsm_pst->block_st.address_u32;
            }

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_READMETA_E);
        }
     }
     else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
     {
         if(2 == fsm_pst->currCopy_u8)
         {
            fsm_pst->addr_u32 = fsm_pst->block_st.address_u32 + fsm_pst->dataBlockLength_u16;
         }
         else
         {
            fsm_pst->addr_u32 = fsm_pst->block_st.address_u32;
         }

         RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_READMETA_E);
     }

     return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_ReadReadMetaData(rba_EaFs2_Prv_Read_tst *fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataReadMetaData(
                &fsm_pst->metaBlock_st,
                fsm_pst->addr_u32,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        rba_EaFs2_Prv_DecodeMetaData(&fsm_pst->metaBlock_st);

        if((boolean)FALSE != fsm_pst->metaBlock_st.isConsistent_b)
        {
            /* Comes here if WIP is proper, Check the block validity*/
            if(RBA_EAFS2_PRV_BLOCK_INVALIDATED_E == fsm_pst->metaBlock_st.status_en)
            {
                /* Block is invalidated*/
                result_en = MEMIF_BLOCK_INVALID;
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_FINAL_E);
            }
            else
            {
                result_en = MEMIF_JOB_OK;
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_VERIFYCRC_E);
            }
        }
        else
        {
            /* WIP is set the block is partially programmed*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_FAILHANDLE_E);
        }
    }

    return result_en;
}
#if (EA_WRITE_SPREAD != FALSE)
static MemIf_JobResultType rba_EaFs2_Prv_ReadGetLocation(rba_EaFs2_Prv_Read_tst  * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint8 NumberOfwriteLocations_u8 = rba_EaFs2_Prv_WriteSpreadRequired(fsm_pst->block_st.blockNumber_u16);

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataBlockCounterRead(
                (uint32 *)&fsm_pst->blockWriteCounter_u32[0],
                (fsm_pst->block_st.address_u32 + ((fsm_pst->dataBlockLength_u16*fsm_pst->currCopy_u8)*fsm_pst->CurrentLocation_u8) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1))),
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();
    if(MEMIF_JOB_OK == result_en)
    {
        if((fsm_pst->blockWriteCounter_u32[0] <= EA_EEP_ALLOWED_WRITE_CYCLES)&&(fsm_pst->blockWriteCounter_u32[1] <= EA_EEP_ALLOWED_WRITE_CYCLES))
        {
            if(fsm_pst->blockWriteCounter_u32[0] < fsm_pst->blockWriteCounter_u32[1])
            {
                fsm_pst->blockWriteCounter_u32[0] = fsm_pst->blockWriteCounter_u32[1];
            }
            else
            {
                fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
            }
            if((fsm_pst->blockWriteCounter_u32[0] >= EA_EEP_ALLOWED_WRITE_CYCLES)&&( NumberOfwriteLocations_u8 > fsm_pst->CurrentLocation_u8))
            {
                fsm_pst->CurrentLocation_u8++;
                fsm_pst->blockWriteCounterT1_u32 = fsm_pst->blockWriteCounter_u32[0]; /* Store the write counter of the previous location*/
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_GETLOCATION_E);
            }
            else
            {
                fsm_pst->CurrentLocationFound_b = (boolean)TRUE;
                if((fsm_pst->blockWriteCounter_u32[0] == 0u)&&(fsm_pst->CurrentLocation_u8!=1)) /*Read from previous location if this location counter is 0*/
                {
                    fsm_pst->CurrentLocation_u8--;
                    fsm_pst->blockWriteCounter_u32[0] = fsm_pst->blockWriteCounterT1_u32;
                    fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
                }
                fsm_pst->block_st.address_u32 = (fsm_pst->block_st.address_u32 + (((fsm_pst->dataBlockLength_u16*fsm_pst->currCopy_u8)*(fsm_pst->CurrentLocation_u8-1)) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1))));
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32 = fsm_pst->block_st.address_u32; /*Store the current address for subsequent read operation*/
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 = fsm_pst->blockWriteCounter_u32[0];
                if(2 == fsm_pst->currCopy_u8)
                {
                    fsm_pst->addr_u32 = fsm_pst->block_st.address_u32 + fsm_pst->dataBlockLength_u16;
                }
                else
                {
                    fsm_pst->addr_u32 = fsm_pst->block_st.address_u32;
                }
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_READMETA_E);
            }
        }
        else
        {
            result_en = MEMIF_JOB_FAILED;
        }
    }

    return result_en;
}

#endif /*(EA_WRITE_SPREAD != FALSE)*/

static MemIf_JobResultType rba_EaFs2_Prv_ReadVerifyCrc(rba_EaFs2_Prv_Read_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataCrcVerify(
                fsm_pst->addr_u32+sizeof(rba_EaFs2_Prv_metaDataEntry_tst),
                Ea_BlockProperties_st[fsm_pst->block_st.blockNumber_u16].Ea_BlkLength_u16,
                fsm_pst->block_st.blockNumber_u16,
                &fsm_pst->calculatedCrc_u32
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        if(fsm_pst->calculatedCrc_u32 == fsm_pst->metaBlock_st.crc_u32)
        {
            /* Reuse Eeprom buffer for reads that are less than buffer size hence avoid re-read */
            if(Ea_BlockProperties_st[fsm_pst->block_st.blockNumber_u16].Ea_BlkLength_u16 > EA_DATA_BUFFER_SIZE){
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_READPAYLOAD_E);
            }else{
                rba_EaFs2_Prv_CopyDataToBuffer(fsm_pst);
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_FINAL_E);
            }
        }
        else
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_FAILHANDLE_E);
        }
    }

    return result_en;
}

/* the function to copy data from rba_EaFs2_Prv_Eeprom_st.buffer_u.u8 to Ea_Prv_Order_st.bfr_pu8 */
static void rba_EaFs2_Prv_CopyDataToBuffer(rba_EaFs2_Prv_Read_tst const * fsm_pst)
{
    uint16 i=0;
    uint8 *p;
    p = fsm_pst->order_pcst->bfr_pu8;
    for(i=0;i<fsm_pst->order_pcst->length_u16;i++){
         *(p+i) = rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[i];
    }
}

static MemIf_JobResultType rba_EaFs2_Prv_ReadPayLoad(rba_EaFs2_Prv_Read_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataRead(
                fsm_pst->addr_u32 + sizeof(rba_EaFs2_Prv_metaDataEntry_tst),
                fsm_pst->order_pcst->bfr_pu8,
                fsm_pst->block_st.length_u16,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        /* Read completed succesfully*/
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_FINAL_E);
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_ReadFailHandle(rba_EaFs2_Prv_Read_tst *fsm_pst)
{
    MemIf_JobResultType result_en;

    fsm_pst->currCopy_u8--;

    if(0 < fsm_pst->currCopy_u8)
    {
        /* Read the redundant copy*/
        result_en = MEMIF_JOB_PENDING;
        fsm_pst->addr_u32 -= fsm_pst->dataBlockLength_u16;

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_READMETA_E);
    }
    else
    {
        result_en = MEMIF_BLOCK_INCONSISTENT;

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_READ_FINAL_E);

        /* distinguish between EMPTY and CORRUPTED */
        if((boolean)FALSE == fsm_pst->metaBlock_st.isConsistent_b){
            /* block EMPTY */
            rba_EaFs2_Prv_ReadJobResultDetailed_aen[fsm_pst->block_st.blockNumber_u16] = MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E;
        }else{
            if(fsm_pst->calculatedCrc_u32 != fsm_pst->metaBlock_st.crc_u32){
                /* block CORRUPTED */
                rba_EaFs2_Prv_ReadJobResultDetailed_aen[fsm_pst->block_st.blockNumber_u16] = MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_DATA_E;
            }
        }
    }

    return result_en;
}

MemIf_JobResultType rba_EaFs2_Prv_BgoDo(void)
{
    /* If it exists and its valid set interrupted defrag flag and complete the operation then proceed with normal defrag
     * Repair FAT entries when idle, including Defrag block*/
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

#if (EA_RB_ENTER_STOP_MODE != FALSE)
    if(RBA_EAFS2_PRV_STOPMODE_ACTIVATED == rba_EaFs2_Prv_StopMode_en)
    {
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
        if(EA_RB_MIGRATION_FORCEDRUNNING == Ea_Rb_MigrationStaus_u8)
        {
            if(RBA_EAFS2_PRV_TRIGGERFORCEDDEFRAG_E == rba_EaFs2_Prv_UnAllocBlkSpace_st.unAllocBlkSpace_en)
            {
                rba_EaFs2_Prv_UnallocBlkSpaceCheck();
                if(EA_RB_MIGRATION_FORCEDRUNNING == Ea_Rb_MigrationStaus_u8)
                {
                    rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_IDLE_E;
                    rba_EaFs2_Prv_FsInfo_st.suspendBgo_b = (boolean)FALSE;
                }
                else
                {
                    rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;
                }
            }
            else
            {
                rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;
            }
        }
        else
#endif/* EA_RB_ACT_UNKNOWN_BLOCK_MIG */
        {
            /* Stop mode called do nothing and exit*/
            rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;
        }
    }
#endif /* (EA_RB_ENTER_STOP_MODE != FALSE) */

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
    rba_EaFs2_Prv_BgoSelector();

    /* Select the background task based on the mode in FsInfo*/
    switch(rba_EaFs2_Prv_FsInfo_st.mode_en)
    {
        /* Idle mode, exit without processing*/
        case RBA_EAFS2_PRV_BGOTASK_NOOP_E       :
        case RBA_EAFS2_PRV_BGOTASK_IDLE_E       :                                           break;
        case RBA_EAFS2_PRV_BGOTASK_TEMPMARKER_E : result_en = rba_EaFs2_Prv_TempMarker  (); break;
        case RBA_EAFS2_PRV_BGOTASK_BGOALLOC_E   : result_en = rba_EaFs2_Prv_BgFatAlloc  (); break;
        case RBA_EAFS2_PRV_BGOTASK_DEFRAG_E     : result_en = rba_EaFs2_Prv_DefragDo    (); break;
        case RBA_EAFS2_PRV_BGOTASK_PROPERTYCHANGE_E : result_en = rba_EaFs2_Prv_PropertyChangeDo    (); break;
        case RBA_EAFS2_PRV_BGOTASK_FATREPAIR_E  : result_en = rba_EaFs2_Prv_FatRepairDo (); break;
        default                                 : result_en =  MEMIF_JOB_FAILED;            break;
    }

    if(MEMIF_JOB_FAILED == result_en)
    {
        /*In future when Fat repair is implemented any failure during Fat
         *repair should not lead to calling Ea_Rb_CheckDefragErr function or RBA_EAFS2_PRV_BGOTASK_NOOP_E*/
        /* No background operation should be run if any of the background operation fails */
        rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;
        Ea_Rb_CheckDefragErr(EA_SID_RB_MIGRATION,EA_E_RB_MIGRATION_FAILED);
    }

#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE */
    return result_en;
}

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)

static void rba_EaFs2_Prv_BgoSelector( void )
{
    if((RBA_EAFS2_PRV_BGOTASK_NOOP_E != rba_EaFs2_Prv_FsInfo_st.mode_en) && (Ea_Rb_MigrationStaus_u8 != EA_RB_MIGRATION_ERROR))
    {
        if (((boolean)FALSE == rba_EaFs2_Prv_TempMarker_st.wrfinish_b) && ((EA_RB_MIGRATION_RUNNING==Ea_Rb_MigrationStaus_u8)||(EA_RB_MIGRATION_FORCEDRUNNING == Ea_Rb_MigrationStaus_u8)))
        {
            rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_TEMPMARKER_E;
        }
        else if(0 < rba_EaFs2_Prv_FsInfo_st.totalGaps_u16)
        {
            /* gaps exist in layout, perform defragmentation*/
            rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_DEFRAG_E;
        }
        else if(EA_NUM_BLOCKS > rba_EaFs2_Prv_FsInfo_st.allotedblocks_u16)
        {
            /* unallocated blocks are present in eeprom, preform allocation*/
            rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_BGOALLOC_E;
        }
        else if(rba_EaFs2_Prv_FsInfo_st.securityLevelChangedBlocks_u16 > 0)
        {
            /* perform SL changes by moving the block to free space */
            rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_PROPERTYCHANGE_E;
        }
        else
        {
            rba_EaFs2_Prv_FsInfo_st.mode_en =RBA_EAFS2_PRV_BGOTASK_FATREPAIR_E;
        }
    }
}

/*Funtion for temporary marker operations*/
static MemIf_JobResultType rba_EaFs2_Prv_TempMarker( void )
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    rba_EaFs2_Prv_TempMarker_tst * fsm_pst = &rba_EaFs2_Prv_TempMarker_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_TEMPMARKER_INIT_E:    result_en = rba_EaFs2_Prv_TempMarker_Init    (fsm_pst); break;
            case RBA_EAFS2_PRV_TEMPMARKER_WRITE_E:   result_en = rba_EaFs2_Prv_TempMarker_Write   (fsm_pst); break;
            case RBA_EAFS2_PRV_TEMPMARKER_COMPARE_E: result_en = rba_EaFs2_Prv_TempMarker_Compare (fsm_pst); break;
            default:                                 result_en = MEMIF_JOB_FAILED;                           break;
        }
    }while(
            (MEMIF_JOB_OK == result_en) &&
            (RBA_EAFS2_PRV_TEMPMARKER_FINAL_E != fsm_pst->state_en)
    );

    if(MEMIF_JOB_PENDING != result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_TEMPMARKER_INIT_E);
    }
    return result_en;
}

/*Init function for temporary marker*/
static MemIf_JobResultType rba_EaFs2_Prv_TempMarker_Init(rba_EaFs2_Prv_TempMarker_tst * fsm_pst)
{
    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_TEMPMARKER_WRITE_E);
    return (MEMIF_JOB_OK);
}

/* Function to write temporary configration marker*/
static MemIf_JobResultType rba_EaFs2_Prv_TempMarker_Write(rba_EaFs2_Prv_TempMarker_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Place write request*/
        rba_EaFs2_Prv_Eeprom_st.buffer_u.u32[0] = RBA_EAFS2_PRV_TEMPMARKER;

        rba_EaFs2_Prv_EepromWrite(
                0u,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                sizeof(uint32)
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_TEMPMARKER_COMPARE_E);
    }

    return result_en;
}

/* Function to compare temporary configration marker*/
static MemIf_JobResultType rba_EaFs2_Prv_TempMarker_Compare(rba_EaFs2_Prv_TempMarker_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Place compare request*/
        rba_EaFs2_Prv_Eeprom_st.buffer_u.u32[0] = RBA_EAFS2_PRV_TEMPMARKER;

        rba_EaFs2_Prv_EepromCompare(
                0u,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                sizeof(uint32)
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_TEMPMARKER_FINAL_E);
        fsm_pst->wrfinish_b=(boolean)TRUE;
        rba_EaFs2_Prv_FsInfo_st.mode_en =RBA_EAFS2_PRV_BGOTASK_IDLE_E;
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_BgFatAlloc( void )
{
    /* noUnallocFat_u16 is updated in init during Fat Build
     * Current Allocation index is set to zero first time but persists till power off
     * remember to update noUnallocFat_u16 after every new fat entry update even from new user write request*/
    MemIf_JobResultType       result_en;
    rba_EaFs2_Prv_Bgo_tst *   fsm_pst = &rba_EaFs2_Prv_Bgo_st;

    if (fsm_pst->state_en == RBA_EAFS2_PRV_BGO_IDLE_E)
    {
        result_en = rba_EaFs2_Prv_BgoIdle(fsm_pst);

        if (fsm_pst->state_en == RBA_EAFS2_PRV_BGO_INIT_E)
        {
            result_en = rba_EaFs2_Prv_BgoInit(fsm_pst);
        }
    }

    result_en = rba_EaFs2_Prv_FatAllocDo();

    return result_en;
}

#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE */

void rba_EaFs2_Prv_PrepFatInfo( void )
{
    rba_EaFs2_Prv_Bgo_tst *   fsm_pst = &rba_EaFs2_Prv_Bgo_st;

    /* Block unallocated, form the block structure*/
    fsm_pst->block_st.doublestorage_b  = rba_EaFs2_Prv_IsBlockDblStorage(rba_EaFs2_Prv_FsInfo_st.allocIter_u16);
    fsm_pst->block_st.survival_b       = rba_EaFs2_Prv_IsBlockSurvival  (rba_EaFs2_Prv_FsInfo_st.allocIter_u16);
    fsm_pst->dataBlockLength_u16       = rba_EaFs2_Prv_BlkLengthInMedium(rba_EaFs2_Prv_FsInfo_st.allocIter_u16);
    fsm_pst->block_st.length_u16       = Ea_BlockProperties_st[rba_EaFs2_Prv_FsInfo_st.allocIter_u16].Ea_BlkLength_u16;
    fsm_pst->block_st.persistentId_u16 = Ea_BlockProperties_st[rba_EaFs2_Prv_FsInfo_st.allocIter_u16].Ea_BlkPersistentID_u16;
    fsm_pst->block_st.blockNumber_u16  = rba_EaFs2_Prv_FsInfo_st.allocIter_u16;
    fsm_pst->block_st.isInConfig_b     = (boolean)TRUE;
#if (EA_WRITE_SPREAD != FALSE)
    fsm_pst->block_st.writeSpread_b    = rba_EaFs2_Prv_IsBlockWriteSpread(rba_EaFs2_Prv_FsInfo_st.allocIter_u16);
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    if(fsm_pst->block_st.doublestorage_b)
    {
        fsm_pst->currCopy_u8 = 2;
    }
    else
    {
        fsm_pst->currCopy_u8 = 1;
    }

    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_GETSPACE_E);
}

MemIf_JobResultType rba_EaFs2_Prv_FatAllocDo( void )
{

    MemIf_JobResultType       result_en = MEMIF_JOB_OK;
    rba_EaFs2_Prv_Bgo_tst *   fsm_pst = &rba_EaFs2_Prv_Bgo_st;

    while( (result_en == MEMIF_JOB_OK) && (fsm_pst->state_en != RBA_EAFS2_PRV_BGO_FINAL_E) )
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_BGO_GETSPACE_E :    result_en = rba_EaFs2_Prv_BgoGetSpace     (fsm_pst); break;
            case RBA_EAFS2_PRV_BGO_EMPTYMETA_E:    result_en = rba_EaFs2_Prv_BgoEmptyMeta    (fsm_pst); break;
            case RBA_EAFS2_PRV_BGO_WRITEFAT_E:     result_en = rba_EaFs2_Prv_BgoWriteFat     (fsm_pst); break;
#if (EA_WRITE_SPREAD != FALSE)
            case RBA_EAFS2_PRV_BGO_CLEARCOUNTER_E: result_en = rba_EaFs2_Prv_BgoClearCounter (fsm_pst); break;
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            default:                               result_en = MEMIF_JOB_FAILED;                        break;
        }
    }

    if(MEMIF_JOB_PENDING != result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_IDLE_E);
    }
    return result_en;
}

#if (EA_WRITE_SPREAD != FALSE)
/*Function to check if Write spread is required for a block*/
uint8 rba_EaFs2_Prv_WriteSpreadRequired(uint16 blockNumber_u16)
{
    uint8 WriteLocationsRequired_u8 = 0;
    WriteLocationsRequired_u8 = (uint8)(Ea_BlockProperties_st[blockNumber_u16].Ea_NumberOfWriteCycles/EA_EEP_ALLOWED_WRITE_CYCLES);
    if ((Ea_BlockProperties_st[blockNumber_u16].Ea_NumberOfWriteCycles%EA_EEP_ALLOWED_WRITE_CYCLES)!= 0u)
    {
        WriteLocationsRequired_u8+=1;
    }
    return WriteLocationsRequired_u8;
}
#endif /*(EA_WRITE_SPREAD != FALSE)*/

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)

static MemIf_JobResultType rba_EaFs2_Prv_BgoIdle(rba_EaFs2_Prv_Bgo_tst * fsm_pst)
{
    if((boolean)FALSE != rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
    {
        /* User request triggered, stop doing bgo*/
        rba_EaFs2_Prv_FsInfo_st.suspendBgo_b = (boolean)FALSE;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_FINAL_E);
    }
    else if(rba_EaFs2_Prv_FsInfo_st.allocIter_u16 >= EA_NUM_BLOCKS)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_FINAL_E);
    }
    else
    {
        /* Start bg allocation*/
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_INIT_E);
    }

    return MEMIF_JOB_OK;
}

/* Time out ?*/
static MemIf_JobResultType rba_EaFs2_Prv_BgoInit(rba_EaFs2_Prv_Bgo_tst * fsm_pst)
{
    boolean doFatWrite_b = (boolean)FALSE;
    while(rba_EaFs2_Prv_FsInfo_st.allocIter_u16 < EA_NUM_BLOCKS)
    {
        if(0u == rba_EaFs2_Prv_RamTable_st[rba_EaFs2_Prv_FsInfo_st.allocIter_u16].address_u32)
        {
            rba_EaFs2_Prv_PrepFatInfo();
            doFatWrite_b = (boolean)TRUE;
            break;
        }
        else
        {
            rba_EaFs2_Prv_FsInfo_st.allocIter_u16++;
        }
    }
    if((boolean)FALSE != doFatWrite_b)
    {
        /* Nothing to do */
    }
    else
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_FINAL_E);
    }

    return MEMIF_JOB_OK;
}

#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE */

static MemIf_JobResultType rba_EaFs2_Prv_BgoGetSpace(rba_EaFs2_Prv_Bgo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint32 freespace_u32;
    uint32 requiredlength_u32;
#if (EA_WRITE_SPREAD != FALSE)
    uint8 writespreadLocations_u8 = 0;
#endif /*(EA_WRITE_SPREAD != FALSE)*/

    freespace_u32 = rba_EaFs2_Prv_GetFreeSpace();
#if (EA_WRITE_SPREAD != FALSE)
    if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
    {
        writespreadLocations_u8 = rba_EaFs2_Prv_WriteSpreadRequired(fsm_pst->block_st.blockNumber_u16);
        requiredlength_u32 = ((((fsm_pst->dataBlockLength_u16)*(fsm_pst->currCopy_u8)) + RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE)*writespreadLocations_u8)+(2u*sizeof(rba_EaFs2_Prv_FatEntry_tst));
    }
    else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    {
        requiredlength_u32 = ((fsm_pst->dataBlockLength_u16)*(fsm_pst->currCopy_u8)) + (2u*sizeof(rba_EaFs2_Prv_FatEntry_tst));
    }

    if (freespace_u32 > requiredlength_u32)
    {
#if (EA_WRITE_SPREAD != FALSE)
        if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
        {
            if(fsm_pst->block_st.doublestorage_b != (boolean)FALSE)
            {
                fsm_pst->addr_u32     = rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 - ((((fsm_pst->dataBlockLength_u16*(fsm_pst->currCopy_u8)) + RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE)*writespreadLocations_u8)- fsm_pst->dataBlockLength_u16);
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->block_st.blockNumber_u16)].currAddress_u32 = fsm_pst->addr_u32-fsm_pst->dataBlockLength_u16; /*Store the current address for subsequent write operation*/
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->block_st.blockNumber_u16)].currCounter_u32 = 0x00u;
            }
            else
            {
                fsm_pst->addr_u32     = rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 - (((fsm_pst->dataBlockLength_u16*(fsm_pst->currCopy_u8)) + RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE)*writespreadLocations_u8);
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->block_st.blockNumber_u16)].currAddress_u32 = fsm_pst->addr_u32; /*Store the current address for subsequent write operation*/
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->block_st.blockNumber_u16)].currCounter_u32 = 0x00u;
            }
        }
        else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
        {
            fsm_pst->addr_u32     = rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 - fsm_pst->dataBlockLength_u16;
        }
        fsm_pst->fatIndex_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_EMPTYMETA_E);
    }
    else
    {
        Ea_Rb_CheckDefragErr(EA_SID_RB_MIGRATION,EA_E_RB_MIGRATION_NOT_POSSIBLE);
        rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;

        result_en = MEMIF_JOB_FAILED;
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_BgoEmptyMeta(rba_EaFs2_Prv_Bgo_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        fsm_pst->metaBlock_st.wipPattern_u16 = 0xFFFFu;
        fsm_pst->metaBlock_st.status_en = RBA_EAFS2_PRV_BLOCK_VALID_E;

        rba_EaFs2_Prv_DataEmptyMetaData(
                &fsm_pst->metaBlock_st,
                fsm_pst->addr_u32,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->currCopy_u8--;

        if(0 < fsm_pst->currCopy_u8)
        {
            /* Write the redundant copy*/
            fsm_pst->addr_u32 -= fsm_pst->dataBlockLength_u16;

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_EMPTYMETA_E);
        }
        else
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_WRITEFAT_E);
        }
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_BgoWriteFat(rba_EaFs2_Prv_Bgo_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        fsm_pst->block_st.address_u32 = fsm_pst->addr_u32;
        rba_EaFs2_Prv_FatWrite(&fsm_pst->block_st,fsm_pst->fatIndex_u16);
    }

    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        /* Update RamTable*/
        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 += (uint16)2;
        rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 = fsm_pst->addr_u32;

        rba_EaFs2_Prv_FsInfo_st.allocIter_u16++;
        rba_EaFs2_Prv_FsInfo_st.allotedblocks_u16++;
#if (EA_WRITE_SPREAD != FALSE)
        if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
        {
            fsm_pst->CurrentLocation_u8 = 1; /* Set to 1 to clear all counters starting from first location*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_CLEARCOUNTER_E);
        }
        else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_FINAL_E);
        }
    }

    return result_en;
}
#if (EA_WRITE_SPREAD != FALSE)
static MemIf_JobResultType rba_EaFs2_Prv_BgoClearCounter(rba_EaFs2_Prv_Bgo_tst   * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint8 NumberOfwriteLocations_u8 = rba_EaFs2_Prv_WriteSpreadRequired(fsm_pst->block_st.blockNumber_u16);
    uint8 currCopy_u8=0;
    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        fsm_pst->blockWriteCounter_u32[0] =0x00u;
        fsm_pst->blockWriteCounter_u32[1] =0x00u;
        if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
        {
            currCopy_u8 = 1;
        }
        else
        {
            currCopy_u8 = 2;
        }
        rba_EaFs2_Prv_BlockCounterWrite(
                fsm_pst->addr_u32 + ((fsm_pst->dataBlockLength_u16*currCopy_u8)*fsm_pst->CurrentLocation_u8) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1)) ,
                (uint8 *)&fsm_pst->blockWriteCounter_u32,
                RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->CurrentLocation_u8++;
        if(NumberOfwriteLocations_u8 >= fsm_pst->CurrentLocation_u8)
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_CLEARCOUNTER_E);
        }
        else
        {
            fsm_pst->CurrentLocation_u8 = 0;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_BGO_FINAL_E);
        }
    }
    return result_en;
}
#endif /*(EA_WRITE_SPREAD != FALSE)*/

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)

/* */
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairDo(void)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    rba_EaFs2_Prv_FatRepair_tst * fsm_pst = &rba_EaFs2_Prv_FatRepair_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_FATREPAIR_INIT_E:              result_en = rba_EaFs2_Prv_FatRepairInit              (fsm_pst); break;
            case RBA_EAFS2_PRV_FATREPAIR_FATREADINIT_E:       result_en = rba_EaFs2_Prv_FatRepairReadInit          (fsm_pst); break;
            case RBA_EAFS2_PRV_FATREPAIR_FATREAD_E:           result_en = rba_EaFs2_Prv_FatRepairRead              (fsm_pst); break;
            case RBA_EAFS2_PRV_FATREPAIR_FATWRITE_E:          result_en = rba_EaFs2_Prv_FatRepairWrite             (fsm_pst); break;
            case RBA_EAFS2_PRV_FATREPAIR_FATWIRTECOMPARE_E:   result_en = rba_EaFs2_Prv_FatRepairCompareWrite      (fsm_pst); break;
            case RBA_EAFS2_PRV_FATREPAIR_WRITEMARKER_E:       result_en = rba_EaFs2_Prv_FatRepairWriteMarker       (fsm_pst); break;
            case RBA_EAFS2_PRV_FATREPAIR_COMPAREMARKER_E:     result_en = rba_EaFs2_Prv_FatRepairCompareMarker     (fsm_pst); break;
            case RBA_EAFS2_PRV_FATREPAIR_ERASEFAT_E:          result_en = rba_EaFs2_Prv_FatRepairEraseFat          (fsm_pst); break;
            default:                                          result_en = MEMIF_JOB_FAILED;                               break;
        }
    }while(
            (MEMIF_JOB_OK == result_en) &&
            (RBA_EAFS2_PRV_FATREPAIR_FINAL_E != fsm_pst->state_en) &&
            ((boolean)FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
    );

    if(     ((boolean)FALSE != rba_EaFs2_Prv_FsInfo_st.suspendBgo_b) &&
            (MEMIF_JOB_PENDING != result_en)
    )
    {
        rba_EaFs2_Prv_FsInfo_st.suspendBgo_b = (boolean)FALSE;
    }
    if((RBA_EAFS2_PRV_FATREPAIR_FINAL_E == fsm_pst->state_en) || (MEMIF_JOB_FAILED == result_en))
    {
        /*Reset state machine*/
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_INIT_E);
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_FatRepairInit(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    /* Persist the config marker once all the blocks from config are allocated */
    if((EA_RB_MIGRATION_RUNNING == Ea_Rb_MigrationStaus_u8)||(EA_RB_MIGRATION_FORCEDRUNNING == Ea_Rb_MigrationStaus_u8)||(!fsm_pst->isFatRepair))
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREADINIT_E);
        fsm_pst->currMarkerCopy_u8 =0u;
        fsm_pst->indexFat =0;
        fsm_pst->isFatRepair = FALSE;
        fsm_pst->reCheck = FALSE;
        rba_EaFs2_Prv_Eeprom_st.upLimit_u32 = 0;
        rba_EaFs2_Prv_Eeprom_st.lowLimit_u32 = 0;
    }
    else
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
    }
    return MEMIF_JOB_OK;

}

/* Function to write configration marker*/
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairWriteMarker(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Place write request*/
        rba_EaFs2_Prv_Eeprom_st.buffer_u.u32[0] = (uint32)EA_MIGRATION_MARKER;

        rba_EaFs2_Prv_EepromWrite(
                (0u +((fsm_pst->currMarkerCopy_u8)*sizeof(uint32))),
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                sizeof(uint32)
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_COMPAREMARKER_E);
    }

    return result_en;
}

/* Function to compare configration marker*/
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairCompareMarker(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Place compare request*/
        rba_EaFs2_Prv_Eeprom_st.buffer_u.u32[0] = (uint32)EA_MIGRATION_MARKER;

        rba_EaFs2_Prv_EepromCompare(
                (0u +((fsm_pst->currMarkerCopy_u8)*sizeof(uint32))),
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                sizeof(uint32)
        );
        fsm_pst->currMarkerCopy_u8++;
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        if(fsm_pst->currMarkerCopy_u8==2u)
        {
            SchM_Enter_Ea_Order();
            if((EA_RB_MIGRATION_RUNNING == Ea_Rb_MigrationStaus_u8) || (EA_RB_MIGRATION_FORCEDRUNNING == Ea_Rb_MigrationStaus_u8))
            {
                 Ea_Rb_MigrationStaus_u8 = EA_RB_MIGRATION_FINISHED;
            }

            SchM_Exit_Ea_Order();
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
        }
        else
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_WRITEMARKER_E);
        }
    }

    return result_en;
}
/* Function to erase two consecutive FAT entries to trigger end of fat*/
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairEraseFat(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Place erase request*/
        rba_EaFs2_Prv_EepromErase(
                rba_EaFs2_Prv_FatIndex2Address(rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16),
                4u*sizeof(rba_EaFs2_Prv_FatEntry_tst)
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_WRITEMARKER_E);
    }

    return result_en;
}

#endif

/**
 * \brief    Worker function write/invalidate blocks into EEPROM.
 *
 *           1. Get Address of block to write.
 *           1a. If  block is unallocated perform allocation.
 *           2. else Calculate data CRC from user buffer.
 *           3.      Read metadata and compare CRC.
 *           3a. if  CRC different write metadata part1
 *                   Write Data
 *                   Write metadata part2
 *           4. else compare data in eeprom and buffer
 *                if data is same exit
 *           5. else go to step 3
 *
 *           Blocks are always written/invalidated in the order Copy2 > Copy1. Before a write request is handled it is
 *           ensured that both the copies are equalized by the repair function.
 *
 * \return   MEMIF_JOB_OK      - Block write completed.
 *           MEMIF_JOB_FAILED  - Block write failure.
 *           MEMIF_JOB_PENDING - Block write ongoing.
 *
 * \attention This function is synchornous and must be used only in Init
 */
MemIf_JobResultType rba_EaFs2_Prv_WriteDo(void)
{
    MemIf_JobResultType         result_en;
    rba_EaFs2_Prv_Write_tst *   fsm_pst = &rba_EaFs2_Prv_Write_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_WRITE_INIT_E :         result_en = rba_EaFs2_Prv_WriteInit            (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_REPAIR_E:        result_en = rba_EaFs2_Prv_WriteRepair          (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_ALLOCSPACE_E:    result_en = rba_EaFs2_Prv_WriteAllocSpace      (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_GETCRC_E:        result_en = rba_EaFs2_Prv_WriteGetCRC          (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_READMETA_E:      result_en = rba_EaFs2_Prv_WriteReadMeta        (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_COMPARE_E:       result_en = rba_EaFs2_Prv_WriteCompare         (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_OPENMETADATA_E:  result_en = rba_EaFs2_Prv_WriteOpenMetaData    (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_CLOSEMETADATA_E: result_en = rba_EaFs2_Prv_WriteCloseMetaData   (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_VERIFY_E:        result_en = rba_EaFs2_Prv_WriteVerify          (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_INVALIDATE_E:    result_en = rba_EaFs2_Prv_WriteInvalidate      (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_DATAWRITE_E:     result_en = rba_EaFs2_Prv_WriteDataWrite       (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_FATWRITE_E:      result_en = rba_EaFs2_Prv_WriteFatWrite        (fsm_pst); break;
#if (EA_WRITE_SPREAD != FALSE)
            case RBA_EAFS2_PRV_WRITE_CLEARCOUNTER_E:
            case RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E: result_en = rba_EaFs2_Prv_WriteUpdateCounter   (fsm_pst); break;
            case RBA_EAFS2_PRV_WRITE_GETLOCATION_E:   result_en = rba_EaFs2_Prv_WriteGetLocation     (fsm_pst); break;
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            default:                                  result_en = rba_EaFs2_Prv_LibResetRetry(&fsm_pst->retry_u8); break;
        }
    }while( (MEMIF_JOB_OK == result_en) && (RBA_EAFS2_PRV_WRITE_FINAL_E != fsm_pst->state_en) );

    if(MEMIF_JOB_FAILED == result_en)
    {
        /* Block repair status must be cleared for any failures during write */
        rba_EaFs2_Prv_ClearRepairStatus(fsm_pst->order_pcst->blockNumber_u16);

        if(0 < fsm_pst->retry_u8)
        {
            /* Retry again write operation*/
            fsm_pst->retry_u8--;
            result_en = MEMIF_JOB_PENDING;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_INIT_E);
        }
    }

    if(MEMIF_JOB_PENDING != result_en)
    {
#if(FALSE != EA_PRV_DEBUGGING)
        rba_MemLib_UpdateTimerUs();
        rba_EaFs2_Prv_DebugTimeStop(&rba_EaFs2_Debug_st.writeTime_st);
#endif

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_IDLE_E);
    }

    return result_en;
}

/**
 * \brief   Function to initialize write opearation. Converts order from user level into block format
 *          Computes address of the block from RAM Table. If the block is unallocated a defrag opeartion is triggered
 *          For SL2 blocks a repair operation is done once per drive cycle prior to every write.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteInit(rba_EaFs2_Prv_Write_tst * fsm_pst)
{

     /* Indicate write state in a global variable
         * write order is 2nd copy first for redundant blocks, it will further be changed based on wip/crc state*/

     fsm_pst->block_st = rba_EaFs2_Prv_BlockFromOrder(fsm_pst->order_pcst);

     if(rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].isSecurityLevelChanged_b != (boolean)FALSE)
     {
         /* Block has a security level change, until the change is implemented by the defrag process use the old property for user operations */
         fsm_pst->block_st.doublestorage_b =   !fsm_pst->block_st.doublestorage_b;
     }

     if(rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].isSurvivalFlagChanged_b != (boolean)FALSE)
     {
         /* Block converted from survival to non-survival in config, treat as survival block forever
          * Once Survival block, stays survival for lifetime */
         fsm_pst->block_st.survival_b =   !fsm_pst->block_st.survival_b;
     }

     if(fsm_pst->block_st.doublestorage_b)
     {
          fsm_pst->currCopy_u8 = 2;
     }
     else
     {
          fsm_pst->currCopy_u8 = 1;
     }

     fsm_pst->dataBlockLength_u16 = rba_EaFs2_Prv_BlkLengthInMedium(fsm_pst->order_pcst->blockNumber_u16);

     /*
     * This function looks up the address of Id from ram table or starts the allocation function
     * Write should be done to a corrupted block in case of double storage, so check WIP status of blocks
     */


    if(0u != rba_EaFs2_Prv_RamTable_st[fsm_pst->order_pcst->blockNumber_u16].address_u32)
    {
        /* Address is found in Ram table, take this address to read from Eeprom*/
        fsm_pst->addr_u32       = rba_EaFs2_Prv_RamTable_st[fsm_pst->order_pcst->blockNumber_u16].address_u32;
        fsm_pst->fatIndex_u16   = rba_EaFs2_Prv_RamTable_st[fsm_pst->order_pcst->blockNumber_u16].fatIndex_u16;
        fsm_pst->needFatWrite_b = (boolean)FALSE;
#if (EA_WRITE_SPREAD != FALSE)
        if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
        {
            if((rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32==0u)||
                    (rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 >= EA_EEP_ALLOWED_WRITE_CYCLES))
            {
                fsm_pst->CurrentLocationFound_b = (boolean)FALSE;
                fsm_pst->CurrentLocation_u8 = 1; /* Iterate from 1 to n locations */
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETLOCATION_E);
            }
            else
            {
                fsm_pst->addr_u32       = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32;
                if((boolean)FALSE != fsm_pst->block_st.doublestorage_b)
                {
                    /* Repair required only for SL2 Blocks */
                    if((boolean)FALSE == rba_EaFs2_Prv_IsRepairDone(fsm_pst->order_pcst->blockNumber_u16))
                    {
                        #if(FALSE != EA_RB_FASTMODE_CCPS)
                        if(MEMIF_MODE_FAST == EA_RB_EEP_MODE)
                        {
                           /* Repair is not needed for Crash data writing */
                            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                        }
                        else
                        #endif
                        {
                            /* Repair not done yet, proceed with repair */
                            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_REPAIR_E);
                        }
                    }
                    else
                    {
                        /* Repair already done, proceed with write */
                        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                    }
                }
                else
                {
                    /* SL1 Block proceed with write request */
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                }
            }
        }
        else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
        {
            if((boolean)FALSE != fsm_pst->block_st.doublestorage_b)
            {
                /* Repair required only for SL2 Blocks */
                if((boolean)FALSE == rba_EaFs2_Prv_IsRepairDone(fsm_pst->order_pcst->blockNumber_u16))
                {
                    #if(FALSE != EA_RB_FASTMODE_CCPS)
                    if(MEMIF_MODE_FAST == EA_RB_EEP_MODE)
                    {
                       /* Repair is not needed for Crash data writing */
                        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                    }
                    else
                    #endif
                    {
                        /* Repair not done yet, proceed with repair */
                        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_REPAIR_E);
                    }
                }
                else
                {
                    /* Repair already done, proceed with write */
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                }
            }
            else
            {
                /* SL1 Block proceed with write request */
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
            }
        }
    }
    else
    {
        fsm_pst->needFatWrite_b = (boolean)TRUE;
        /* Id not yet written in Eeprom, perform block allocation */
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_ALLOCSPACE_E);
    }

    return MEMIF_JOB_OK;

}

/**
 * \brief   Trigger repair for an SL2 block.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteRepair(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        /* Start repair operation for SL2 blocks*/
        rba_EaFs2_Prv_DataRepair();
    }

    result_en = rba_EaFs2_Prv_DataRepairDo();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
    }

    return result_en;
}

/* Check if space is there to allocate data if not start defrag if space recovery is possible*/
static MemIf_JobResultType rba_EaFs2_Prv_WriteAllocSpace(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint32 freespace_u32;
    uint32 requiredlength_u32;
#if (EA_WRITE_SPREAD != FALSE)
    uint8 writespreadLocations_u8 = 0;
#endif /*(EA_WRITE_SPREAD != FALSE)*/

    freespace_u32 = rba_EaFs2_Prv_GetFreeSpace();
#if (EA_WRITE_SPREAD != FALSE)
    if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
    {
        writespreadLocations_u8 = rba_EaFs2_Prv_WriteSpreadRequired(fsm_pst->block_st.blockNumber_u16);
        requiredlength_u32 = ((((fsm_pst->dataBlockLength_u16)*(fsm_pst->currCopy_u8)) + RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE)*writespreadLocations_u8)+(2u*sizeof(rba_EaFs2_Prv_FatEntry_tst));
    }
    else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    {
        requiredlength_u32 = ((fsm_pst->dataBlockLength_u16)*(fsm_pst->currCopy_u8)) + (2u*sizeof(rba_EaFs2_Prv_FatEntry_tst));
    }

    if (freespace_u32 > requiredlength_u32)
    {
        fsm_pst->fatIndex_u16 = rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16;
#if (EA_WRITE_SPREAD != FALSE)
        if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
        {
            fsm_pst->addr_u32     = (rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 - (((fsm_pst->dataBlockLength_u16*(fsm_pst->currCopy_u8)) + RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE)*writespreadLocations_u8));
            rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32 = fsm_pst->addr_u32; /*Store the current address for subsequent write operation*/
            rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 = 0x00u;
            fsm_pst->CurrentLocation_u8 = 1; /* Set to 1 to clear all counters starting from first location*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_CLEARCOUNTER_E);
        }
        else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
        {
            fsm_pst->addr_u32     = rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 - (fsm_pst->dataBlockLength_u16*(fsm_pst->currCopy_u8));
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
        }

    }
    else
    {
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
        if(rba_EaFs2_Prv_FsInfo_st.totalGaps_u16 > 0u)
        {
            /* Start defrag state machine */
            rba_EaFs2_Prv_DefragInfo_st.isForcedDefrag_b = (boolean)TRUE;
            result_en = rba_EaFs2_Prv_DefragDo();
            /* Forced defrag is cleared once its accepted by the defrag state machine*/
            rba_EaFs2_Prv_DefragInfo_st.isForcedDefrag_b = (boolean)FALSE;
        }
        else
        {
            Ea_Rb_CheckDefragErr(EA_SID_WRITE,EA_E_RB_MIGRATION_NOT_POSSIBLE);
            rba_EaFs2_Prv_FsInfo_st.mode_en = RBA_EAFS2_PRV_BGOTASK_NOOP_E;
            result_en = MEMIF_JOB_FAILED;
        }

#else
        result_en = MEMIF_JOB_FAILED;
#endif
    }

    if(result_en == MEMIF_JOB_FAILED)
    {
        /*Avoid retries if no space is available in EEPROM*/
        fsm_pst->retry_u8 = 0u;
    }

    return result_en;
}

/**
 * \brief   Writes part1 of metadata for a block.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteOpenMetaData(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        fsm_pst->metaBlock_st.status_en      = RBA_EAFS2_PRV_BLOCK_VALID_E;
        fsm_pst->metaBlock_st.wipPattern_u16 = rba_EaFs2_Prv_GetWipToUse(fsm_pst->metaBlock_st.metaDataEntry_st.part2_st.wipState_u8);

        rba_EaFs2_Prv_DataOpenMetaData(&fsm_pst->metaBlock_st,fsm_pst->addr_u32,fsm_pst->order_pcst->blockNumber_u16);
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_DATAWRITE_E);
    }

    return result_en;
}

/**
 * \brief   Writes part2 of metadata for a block.
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteCloseMetaData(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataCloseMetaData(&fsm_pst->metaBlock_st,fsm_pst->addr_u32,fsm_pst->order_pcst->blockNumber_u16);
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        #if(FALSE != EA_RB_FASTMODE_CCPS)
        if(MEMIF_MODE_FAST == EA_RB_EEP_MODE)
        {
#if (EA_WRITE_SPREAD != FALSE)
            if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
            {
                if((fsm_pst->block_st.doublestorage_b != (boolean)FALSE) && (fsm_pst->currCopy_u8==2))
                {
                    if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32<EA_EEP_ALLOWED_WRITE_CYCLES)
                    {
                        rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 +=1u;
                    }
                    fsm_pst->blockWriteCounter_u32[0] = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
                    fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
                    fsm_pst->currCopyCounter_u8 = 2;
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E);
                }
                else if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
                {
                    if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32<EA_EEP_ALLOWED_WRITE_CYCLES)
                    {
                        rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 +=1u;
                    }
                    fsm_pst->blockWriteCounter_u32[0] = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
                    fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
                    fsm_pst->currCopyCounter_u8 = 2;
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E);
                }
                else
                {
                    rba_EaFs2_Prv_Write2ndCopy(fsm_pst);
                }
            }
            else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            {
                rba_EaFs2_Prv_Write2ndCopy(fsm_pst);
            }
        }
        else
        #endif
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_VERIFY_E);
        }
    }

    return result_en;
}

/**
 * \brief   Function to verify if the data written into eeprom is same as the intended data from the user buffer
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteVerify(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataCompare(
                fsm_pst->addr_u32 + sizeof(rba_EaFs2_Prv_metaDataEntry_tst),
                fsm_pst->order_pcst->bfr_pcu8,
                fsm_pst->order_pcst->length_u16,
                fsm_pst->order_pcst->blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_PENDING != result_en)
    {
        /* Check if data compare is succesfull*/
        if(MEMIF_JOB_OK == result_en)
        {
#if (EA_WRITE_SPREAD != FALSE)
            if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
            {
                if((fsm_pst->block_st.doublestorage_b != (boolean)FALSE) && (fsm_pst->currCopy_u8==2))
                {
                    if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32<EA_EEP_ALLOWED_WRITE_CYCLES)
                    {
                        rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 +=1u;
                    }
                    fsm_pst->blockWriteCounter_u32[0] = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
                    fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
                    fsm_pst->currCopyCounter_u8 = 2;
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E);
                }
                else if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
                {
                    if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32<EA_EEP_ALLOWED_WRITE_CYCLES)
                    {
                        rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 +=1u;
                    }
                    fsm_pst->blockWriteCounter_u32[0] = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
                    fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
                    fsm_pst->currCopyCounter_u8 = 2;
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E);
                }
                else
                {
                    rba_EaFs2_Prv_Write2ndCopy(fsm_pst);
                }
            }
            else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            {
                rba_EaFs2_Prv_Write2ndCopy(fsm_pst);
            }
        }
        else
        {
            /* Verification failure, dont retry again. Possibly because of changing user buffer/ Hw failure*/
            fsm_pst->retry_u8 = 0;
            result_en = MEMIF_JOB_FAILED;
        }
    }

    return result_en;
}

/**
 * \brief   Writes both the metadata part1 and 2 together with invalidate state
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteInvalidate(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        fsm_pst->metaBlock_st.wipPattern_u16 = rba_EaFs2_Prv_GetWipToUse(fsm_pst->metaBlock_st.metaDataEntry_st.part2_st.wipState_u8);

        rba_EaFs2_Prv_DataInvalidateMetaData(&fsm_pst->metaBlock_st,fsm_pst->addr_u32,fsm_pst->order_pcst->blockNumber_u16);
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
#if (EA_WRITE_SPREAD != FALSE)
            if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
            {
                if((fsm_pst->block_st.doublestorage_b != (boolean)FALSE) && (fsm_pst->currCopy_u8==2))
                {
                    if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32<EA_EEP_ALLOWED_WRITE_CYCLES)
                    {
                        rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 +=1u;
                    }
                    fsm_pst->blockWriteCounter_u32[0] = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
                    fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
                    fsm_pst->currCopyCounter_u8 = 2;
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E);
                }
                else if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
                {
                    if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32<EA_EEP_ALLOWED_WRITE_CYCLES)
                    {
                        rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 +=1u;
                    }
                    fsm_pst->blockWriteCounter_u32[0] = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
                    fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
                    fsm_pst->currCopyCounter_u8 = 2;
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E);
                }
                else
                {
                    rba_EaFs2_Prv_Write2ndCopy(fsm_pst);
                }
            }
            else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            {
                rba_EaFs2_Prv_Write2ndCopy(fsm_pst);
            }
    }

    return result_en;
}

/**
 * \brief   Computes CRC of the data from the user buffer
 *
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteGetCRC(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if((boolean)FALSE != fsm_pst->doInvalidate_b)
    {
        /* Skip CRC computation for Invalidate Request - Proceed for Read Meta*/
        result_en = MEMIF_JOB_OK;
    }
    else
    {
        /* Write Request - Compute CRC */
        uint16 crcLengthToCalc_u16 = 0u;

        crcLengthToCalc_u16 = (uint16)rba_EaFs2_Prv_LibMin(
                fsm_pst->block_st.length_u16 - fsm_pst->crcCalcLengthDone_u16,
                RBA_EAFS2_PRV_MAX_CRC_CALC_PER_CALL
        );

        fsm_pst->calculatedCrc_u32 = Crc_CalculateCRC32(fsm_pst->order_pcst->bfr_pcu8+fsm_pst->crcCalcLengthDone_u16,
                crcLengthToCalc_u16,
                fsm_pst->calculatedCrc_u32,
                (boolean)FALSE
        );

        fsm_pst->crcCalcLengthDone_u16 += crcLengthToCalc_u16;

        if(fsm_pst->crcCalcLengthDone_u16 == fsm_pst->block_st.length_u16)
        {
            /* CRC calculated for complete data length */
            fsm_pst->metaBlock_st.status_en = RBA_EAFS2_PRV_BLOCK_VALID_E;
            result_en = MEMIF_JOB_OK;
        }
    }

    if(MEMIF_JOB_PENDING != result_en)
    {
        if(2 == fsm_pst->currCopy_u8)
        {
            /* SL2 - Write to the second copy first*/
            fsm_pst->addr_u32 += fsm_pst->dataBlockLength_u16;
        }

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_READMETA_E);
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_WriteReadMeta(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;
#if (EA_WRITE_SPREAD != FALSE)
    uint8 currCopy_u8 =0;
#endif /*(EA_WRITE_SPREAD != FALSE)*/

    if((boolean)FALSE == fsm_pst->needFatWrite_b)
    {

        if(RBA_EAFS2_PRV_LIBENTRY)
        {
#if (EA_WRITE_SPREAD != FALSE)
            if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
            {
                currCopy_u8 = 1;
            }
            else
            {
                currCopy_u8 = 2;
            }
            if(fsm_pst->locationChange_b != (boolean)FALSE) /*Location change is ongoing. Read metadata from previous location*/
            {
                rba_EaFs2_Prv_DataReadMetaData(&fsm_pst->metaBlock_st,(fsm_pst->addr_u32 - ((fsm_pst->dataBlockLength_u16*currCopy_u8) + RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE)),
                        fsm_pst->order_pcst->blockNumber_u16);
                if (fsm_pst->currCopy_u8 == 1)
                {
                    fsm_pst->locationChange_b = (boolean)FALSE;
                }
            }
            else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            {
                rba_EaFs2_Prv_DataReadMetaData(&fsm_pst->metaBlock_st,fsm_pst->addr_u32,fsm_pst->order_pcst->blockNumber_u16);
            }
        }

        result_en = rba_EaFs2_Prv_DataDo();
    }
    else
    {
        /* New allocation, Read meta is not required */
        fsm_pst->metaBlock_st.metaDataEntry_st.part1_st.wipState_u8 = 0xFFu;
        fsm_pst->metaBlock_st.metaDataEntry_st.part2_st.wipState_u8 = 0xFFu;

        result_en = MEMIF_JOB_OK;
    }

    if(MEMIF_JOB_OK == result_en)
    {
        rba_EaFs2_Prv_DecodeMetaData(&fsm_pst->metaBlock_st);

        if((boolean)FALSE != fsm_pst->doInvalidate_b)
        {
            fsm_pst->metaBlock_st.status_en = RBA_EAFS2_PRV_BLOCK_INVALIDATED_E;
            fsm_pst->metaBlock_st.crc_u32 = 0;

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_INVALIDATE_E);
        }
        else
        {
            #if(FALSE != EA_RB_FASTMODE_CCPS)
            if(MEMIF_MODE_FAST == EA_RB_EEP_MODE)
            {
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_OPENMETADATA_E);
            }
            else
            #endif
            {
                if(
                    ((boolean)FALSE                       != fsm_pst->metaBlock_st.isConsistent_b) &&
                    (RBA_EAFS2_PRV_BLOCK_VALID_E == fsm_pst->metaBlock_st.status_en     ) &&
                    (fsm_pst->calculatedCrc_u32  == fsm_pst->metaBlock_st.crc_u32)
                )
                {
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_COMPARE_E);
                }
                else
                {
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_OPENMETADATA_E);
                }
            }
            fsm_pst->metaBlock_st.crc_u32 = fsm_pst->calculatedCrc_u32;
        }

    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_WriteCompare(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataCompare(
                fsm_pst->addr_u32 + sizeof(rba_EaFs2_Prv_metaDataEntry_tst),
                fsm_pst->order_pcst->bfr_pcu8,
                fsm_pst->order_pcst->length_u16,
                fsm_pst->order_pcst->blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_PENDING != result_en)
    {
        if(MEMIF_JOB_OK == result_en)
        {
            /* Buffer and Eeprom contents are same, no write required*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_FINAL_E);
        }
        else
        {
            /* result modified so that state machine does not do retries for comapre failure cases*/
            result_en = MEMIF_JOB_OK;
            /*Mismatch between Eeprom and user buffer*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_OPENMETADATA_E);
        }
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_WriteDataWrite(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataWrite(
                fsm_pst->addr_u32 + sizeof(rba_EaFs2_Prv_metaDataEntry_tst),
                fsm_pst->order_pcst->bfr_pcu8,
                fsm_pst->order_pcst->length_u16,
                fsm_pst->order_pcst->blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_CLOSEMETADATA_E);
    }

    return result_en;
}

static MemIf_JobResultType rba_EaFs2_Prv_WriteFatWrite(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        fsm_pst->block_st.address_u32 = fsm_pst->addr_u32;
        rba_EaFs2_Prv_FatWrite(&fsm_pst->block_st,rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16);
    }

    result_en = rba_EaFs2_Prv_FatWriteDo();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->needFatWrite_b = (boolean)FALSE;

        rba_EaFs2_Prv_FsInfo_st.allotedblocks_u16++;
        rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16 += (uint16)2;
        rba_EaFs2_Prv_FsInfo_st.currDataPtr_u32 = fsm_pst->addr_u32;
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_FINAL_E);
    }

    return result_en;
}

static void rba_EaFs2_Prv_Write2ndCopy(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    /* First copy already written, Checks and updates the second copy */
    fsm_pst->currCopy_u8--;

    if(0u < fsm_pst->currCopy_u8)
    {
        /* Write the redundant copy*/
        fsm_pst->addr_u32 -= fsm_pst->dataBlockLength_u16;

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_READMETA_E);
    }
    else
    {
        if((boolean)FALSE != fsm_pst->needFatWrite_b)
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_FATWRITE_E);
        }
        else
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_FINAL_E);
        }
    }
}

#if (EA_WRITE_SPREAD != FALSE)
static MemIf_JobResultType rba_EaFs2_Prv_WriteGetLocation(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint8 NumberOfwriteLocations_u8 = rba_EaFs2_Prv_WriteSpreadRequired(fsm_pst->block_st.blockNumber_u16);

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataBlockCounterRead(
                (uint32 *)&fsm_pst->blockWriteCounter_u32[0],
                (fsm_pst->addr_u32 + ((fsm_pst->dataBlockLength_u16*fsm_pst->currCopy_u8)*fsm_pst->CurrentLocation_u8) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1))),
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();
    if(MEMIF_JOB_OK == result_en)
    {
        if((fsm_pst->blockWriteCounter_u32[0] <= EA_EEP_ALLOWED_WRITE_CYCLES)&&(fsm_pst->blockWriteCounter_u32[1] <= EA_EEP_ALLOWED_WRITE_CYCLES))
        {
            if(fsm_pst->blockWriteCounter_u32[0] < fsm_pst->blockWriteCounter_u32[1])
            {
                fsm_pst->blockWriteCounter_u32[0] = fsm_pst->blockWriteCounter_u32[1];
            }
            else
            {
                fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
            }
            if((fsm_pst->blockWriteCounter_u32[0] >= EA_EEP_ALLOWED_WRITE_CYCLES)&&( NumberOfwriteLocations_u8 > fsm_pst->CurrentLocation_u8))
            {
                fsm_pst->blockWriteCounterT1_u32 = fsm_pst->blockWriteCounter_u32[0]; /* Store the write counter of the previous location*/
                fsm_pst->CurrentLocation_u8++;
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETLOCATION_E);
            }
            else
            {
                fsm_pst->CurrentLocationFound_b = (boolean)TRUE;
                fsm_pst->addr_u32 = (fsm_pst->addr_u32 + (((fsm_pst->dataBlockLength_u16*fsm_pst->currCopy_u8)*(fsm_pst->CurrentLocation_u8-1)) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1))));
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32 = fsm_pst->addr_u32; /*Store the current address for subsequent write operation*/
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 = fsm_pst->blockWriteCounter_u32[0];
                if((fsm_pst->blockWriteCounter_u32[0] == 0u)&&(fsm_pst->blockWriteCounterT1_u32 !=0u)) /*Location change is required*/
                {
                    fsm_pst->locationChange_b = (boolean)TRUE;
                }
                if((boolean)FALSE != fsm_pst->block_st.doublestorage_b)
                {
                    /* Repair required only for SL2 Blocks */
                    if((boolean)FALSE == rba_EaFs2_Prv_IsRepairDone(fsm_pst->order_pcst->blockNumber_u16))
                    {
                        #if(FALSE != EA_RB_FASTMODE_CCPS)
                        if(MEMIF_MODE_FAST == EA_RB_EEP_MODE)
                        {
                           /* Repair is not needed for Crash data writing */
                            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                        }
                        else
                        #endif
                        {
                            /* Repair not done yet, proceed with repair */
                            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_REPAIR_E);
                        }
                    }
                    else
                    {
                        /* Repair already done, proceed with write */
                        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                    }
                }
                else
                {
                    /* SL1 Block proceed with write request */
                    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
                }
            }
        }
        else
        {
            result_en = MEMIF_JOB_FAILED;
        }
    }

    return result_en;
}

/**
 * \brief    Trigger function to update the bock counter
 */
static MemIf_JobResultType rba_EaFs2_Prv_WriteUpdateCounter(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint8 currCopy_u8 =0;
    uint8 NumberOfwriteLocations_u8;
    if(fsm_pst->state_en == RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E)
    {
        if(RBA_EAFS2_PRV_LIBENTRY)
        {
            if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
            {
                currCopy_u8 = 1;
            }
            else
            {
                currCopy_u8 = 2;
            }
            rba_EaFs2_Prv_BlockCounterWrite(
                    rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32 + (fsm_pst->dataBlockLength_u16*currCopy_u8) + ((fsm_pst->currCopyCounter_u8-1)*RBA_EAFS2_PRV_WRITESPREAD_COUNTER_LENGTH) ,
                    (uint8 *)&fsm_pst->blockWriteCounter_u32,
                    RBA_EAFS2_PRV_WRITESPREAD_COUNTER_LENGTH,
                    fsm_pst->order_pcst->blockNumber_u16
            );
        }

        result_en = rba_EaFs2_Prv_DataDo();

        if(MEMIF_JOB_OK == result_en)
        {
            rba_EaFs2_Prv_WriteCounter2ndCopy(fsm_pst);
        }
    }
    else
    {
    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        if(fsm_pst->block_st.doublestorage_b == (boolean)FALSE)
        {
            currCopy_u8 = 1;
        }
        else
        {
            currCopy_u8 = 2;
        }
        fsm_pst->blockWriteCounter_u32[0] =0x00u;
        fsm_pst->blockWriteCounter_u32[1] =0x00u;
        rba_EaFs2_Prv_BlockCounterWrite(
        fsm_pst->addr_u32 + ((fsm_pst->dataBlockLength_u16*currCopy_u8)*fsm_pst->CurrentLocation_u8) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1)) ,
        (uint8 *)&fsm_pst->blockWriteCounter_u32,
        RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE,
        fsm_pst->order_pcst->blockNumber_u16
        );
        }
        result_en = rba_EaFs2_Prv_DataDo();

        if(MEMIF_JOB_OK == result_en)
        {
            fsm_pst->CurrentLocation_u8++;
            NumberOfwriteLocations_u8 = rba_EaFs2_Prv_WriteSpreadRequired(fsm_pst->block_st.blockNumber_u16);
            if(NumberOfwriteLocations_u8 >= fsm_pst->CurrentLocation_u8)
            {

                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_CLEARCOUNTER_E);
            }
            else
            {
                fsm_pst->CurrentLocation_u8 = 0;
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_GETCRC_E);
            }
        }
    }
    return result_en;
}
static void rba_EaFs2_Prv_WriteCounter2ndCopy(rba_EaFs2_Prv_Write_tst * fsm_pst)
{
    fsm_pst->currCopyCounter_u8--;

    if(0u < fsm_pst->currCopyCounter_u8)
    {
        /* Write the redundant copy*/

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_WRITE_UPDATECOUNTER_E);
    }
    else
    {
        rba_EaFs2_Prv_Write2ndCopy(fsm_pst);
    }
}
#endif /*(EA_WRITE_SPREAD != FALSE)*/
rba_EaFs2_Prv_Block_tst rba_EaFs2_Prv_BlockFromOrder(Ea_Prv_Order_tst const *order_pcst)
{
    rba_EaFs2_Prv_Block_tst block_st;

    block_st.persistentId_u16   = rba_EaFs2_Prv_GetPersistentID   (order_pcst->blockNumber_u16);
    block_st.doublestorage_b    = rba_EaFs2_Prv_IsBlockDblStorage (order_pcst->blockNumber_u16);
    block_st.survival_b         = rba_EaFs2_Prv_IsBlockSurvival   (order_pcst->blockNumber_u16);
    block_st.length_u16         = order_pcst->length_u16;
    block_st.blockNumber_u16    = order_pcst->blockNumber_u16;
    block_st.address_u32        = 0;
    block_st.isInConfig_b       = (boolean)TRUE; /* only known blocks can be written*/
#if (EA_WRITE_SPREAD != FALSE)
    block_st.writeSpread_b      = rba_EaFs2_Prv_IsBlockWriteSpread(order_pcst->blockNumber_u16);
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    return block_st;
}

uint16 rba_EaFs2_Prv_GetBlockID(uint16 persistentID_u16)
{

    uint16 low_u16    = 0;
    uint16 high_u16   = EA_NUM_BLOCKS;
    uint16 mid_u16    = 0;
    uint16 result_u16 = 0xFFFF;

    while(low_u16 != high_u16)
    {
        mid_u16 = low_u16 + ((high_u16-low_u16)/(uint16)2);

        if(Ea_BlockProperties_st[mid_u16].Ea_BlkPersistentID_u16 < persistentID_u16)
        {
            if(mid_u16 == low_u16)
            {
                high_u16 = low_u16;
            }
            else
            {
                low_u16 = mid_u16;
            }
        }
        else if(Ea_BlockProperties_st[mid_u16].Ea_BlkPersistentID_u16 > persistentID_u16)
        {
            high_u16 = mid_u16;
        }
        else
        {
            high_u16 = low_u16;
            result_u16 = mid_u16;
        }
    }
    return result_u16;
}

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
/* rba_EaFs2_Prv_FatRepairReadInit is function that check first header copy or second header copy corrupted or not  */
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairReadInit(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    /* find address from index input */
    fsm_pst->iterator = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->indexFat);

    if(fsm_pst->indexFat < rba_EaFs2_Prv_FsInfo_st.currFatIndex_u16)
    {
       /* load Fat into buffer to check */
       result_en = rba_EaFs2_Prv_readFatEEPROM(&fsm_pst->corruptedHeader,fsm_pst->iterator);

       if(result_en == MEMIF_JOB_OK)
       {
          /* suspend background task if user trigger read or write */
          if(FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
          {
              if(fsm_pst->corruptedHeader == (uint8)SECOND_COPY_CORRUPTED)
              {
                 if(!fsm_pst->reCheck)
                 {
                   fsm_pst->fatAdrRead_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->indexFat);
                   fsm_pst->fatAdrWrite_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->indexFat)+sizeof(rba_EaFs2_Prv_FatEntry_tst);
                   RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREAD_E);
                 }else{
                   RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
                 }
              }else if (fsm_pst->corruptedHeader == (uint8)FIRST_COPY_CORRUPTED)
              {
                 if(!fsm_pst->reCheck)
                 {
                   fsm_pst->fatAdrRead_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->indexFat)+sizeof(rba_EaFs2_Prv_FatEntry_tst);
                   fsm_pst->fatAdrWrite_u32 = rba_EaFs2_Prv_FatIndex2Address(fsm_pst->indexFat);
                   RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREAD_E);
                 }else{
                   RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
                 }
              }else
              {
                  fsm_pst->indexFat += (uint16)2;
                  RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREADINIT_E);
              }
          }else
          {
              RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREADINIT_E);
          }
       }else
       {
           if(result_en == MEMIF_JOB_FAILED)
           {
               fsm_pst->isFatRepair = TRUE;
               RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
           }
       }
    }else
    {
        /* finish checking and fixing all Fat */
        fsm_pst->isFatRepair = TRUE;
        if(!fsm_pst->reCheck){
            /* recheck all Fat after fix to make sure there are no Fat is corrupted after fixing */
            fsm_pst->reCheck = TRUE;
            fsm_pst->indexFat = 0;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREADINIT_E);
        }else{
            /* completely done fixing Fat */
            result_en = MEMIF_JOB_OK;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_ERASEFAT_E);
        }
    }

    return result_en;
}

/* function to read header copy  */
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairRead(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;
    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromRead(
                fsm_pst->fatAdrRead_u32,
            &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
            sizeof(rba_EaFs2_Prv_FatEntry_tst)
        );
    }

    /* Call Eeprom status function to know the status of Eeprom Read */
    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        if(FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
        {
           /* Read Completed, write the data, do not update the info variables here */
           RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATWRITE_E);
        }else
        {
           /* Redo the read operation in case cancel is requested,
            * Possible Ea internal buffer collision, reset state to Data Read before Data Write*/
           RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREAD_E);
        }

    }else{
        if(result_en == MEMIF_JOB_FAILED)
        {
           fsm_pst->isFatRepair = TRUE;
           RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
        }
    }
    return result_en;
}

/* function to write Fat header copy */
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairWrite(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
      /* Place write request*/
      rba_EaFs2_Prv_EepromWrite(
            fsm_pst->fatAdrWrite_u32,
            &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
            sizeof(rba_EaFs2_Prv_FatEntry_tst)
      );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        if(FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
        {
            /* Read Completed, write the data, do not update the info variables here */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATWIRTECOMPARE_E);
        }else
        {
            /* Redo the read operation in case cancel is requested,
            * Possible Ea internal buffer collision, reset state to Data Read before Data Write*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREAD_E);
         }

    }else
    {
        if(result_en == MEMIF_JOB_FAILED)
        {
           fsm_pst->isFatRepair = TRUE;
           RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
        }
    }

    return result_en;
}

/* Function to verify data after write Fat */
static MemIf_JobResultType rba_EaFs2_Prv_FatRepairCompareWrite(rba_EaFs2_Prv_FatRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_EepromCompare(
                fsm_pst->fatAdrWrite_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0],
                sizeof(rba_EaFs2_Prv_FatEntry_tst)
        );
    }

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        if(FALSE == rba_EaFs2_Prv_FsInfo_st.suspendBgo_b)
        {
           /* increase index to continue check next Fat */
           fsm_pst->indexFat +=(uint16)2;
           RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREADINIT_E);
        }else
        {
           RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FATREAD_E);
        }
    }else
    {
        if(result_en == MEMIF_JOB_FAILED)
        {
           fsm_pst->isFatRepair = TRUE;
           RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_FATREPAIR_FINAL_E);
        }
    }

    return result_en;
}
#endif

#if(FALSE != EA_PRV_DEBUGGING)
/**
 *********************************************************************
 * rba_EaFs2_Prv_Test_Initialize_EaFs2():
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
void rba_EaFs2_Prv_Test_Initialize_EaFs2(void)
{
    rba_EaFs2_Prv_Bgo_st.block_st.address_u32=0u;
    rba_EaFs2_Prv_Bgo_st.block_st.persistentId_u16=0u;
    rba_EaFs2_Prv_Bgo_st.block_st.length_u16=0u;
    rba_EaFs2_Prv_Bgo_st.block_st.blockNumber_u16=0u;
    rba_EaFs2_Prv_Bgo_st.block_st.doublestorage_b=FALSE;
    rba_EaFs2_Prv_Bgo_st.block_st.survival_b=FALSE;

    rba_EaFs2_Prv_Bgo_st.metaBlock_st.metaDataEntry_st.part1_st.crc_u16=0u;
    rba_EaFs2_Prv_Bgo_st.metaBlock_st.metaDataEntry_st.part1_st.validity_u8=0u;
    rba_EaFs2_Prv_Bgo_st.metaBlock_st.metaDataEntry_st.part1_st.wipState_u8=0u;

    rba_EaFs2_Prv_Bgo_st.metaBlock_st.metaDataEntry_st.part2_st.crc_u16=0u;
    rba_EaFs2_Prv_Bgo_st.metaBlock_st.metaDataEntry_st.part2_st.validity_u8=0u;
    rba_EaFs2_Prv_Bgo_st.metaBlock_st.metaDataEntry_st.part2_st.wipState_u8=0u;

    rba_EaFs2_Prv_Bgo_st.metaBlock_st.wip_en=RBA_EAFS2_PRV_WIP_COMPLETE_E;
    rba_EaFs2_Prv_Bgo_st.metaBlock_st.status_en=RBA_EAFS2_PRV_BLOCK_VALID_E;

    rba_EaFs2_Prv_Bgo_st.metaBlock_st.crc_u32=0u;
    rba_EaFs2_Prv_Bgo_st.metaBlock_st.wipPattern_u16=0u;
    rba_EaFs2_Prv_Bgo_st.metaBlock_st.isConsistent_b=FALSE;

    rba_EaFs2_Prv_Bgo_st.state_en=RBA_EAFS2_PRV_BGO_IDLE_E;
    rba_EaFs2_Prv_Bgo_st.addr_u32=0u;
    rba_EaFs2_Prv_Bgo_st.fatIndex_u16=0u;
    rba_EaFs2_Prv_Bgo_st.dataBlockLength_u16=0u;
    rba_EaFs2_Prv_Bgo_st.currCopy_u8=0u;
    rba_EaFs2_Prv_Bgo_st.entry_b=FALSE;

#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
    rba_EaFs2_Prv_TempMarker_st.state_en=RBA_EAFS2_PRV_TEMPMARKER_INIT_E;
    rba_EaFs2_Prv_TempMarker_st.wrfinish_b=FALSE;
    rba_EaFs2_Prv_TempMarker_st.entry_b=FALSE;

#endif


}
#endif

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
