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

static rba_EaFs2_Prv_Data_tst        rba_EaFs2_Prv_Data_st;
static rba_EaFs2_Prv_DataRepair_tst  rba_EaFs2_Prv_DataRepair_st;

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

static MemIf_JobResultType rba_EaFs2_Prv_Data_Init       (rba_EaFs2_Prv_Data_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Data_SplitStart (rba_EaFs2_Prv_Data_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Data_Check      (rba_EaFs2_Prv_Data_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_Data_Call       (rba_EaFs2_Prv_Data_tst * fsm_pst);

static void rba_EaFs2_Prv_EncodeMetaData(rba_EaFs2_Prv_metaBlock_tst * metaData_pst);

static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_Init      (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_ReadMeta  (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_VerifyCRC (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_ReadData  (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_Process   (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_WriteData (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_OpenMeta  (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_CloseMeta (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_InvMeta   (rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
#if (EA_WRITE_SPREAD != FALSE)
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_GetLocation(rba_EaFs2_Prv_DataRepair_tst  * fsm_pst);
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_UpdateCounter(rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
static void                rba_EaFs2_Prv_DataRepair_Counter2ndCopy(rba_EaFs2_Prv_DataRepair_tst * fsm_pst);
#endif /*(EA_WRITE_SPREAD != FALSE)*/
static rba_EaFs2_Prv_wipState_en rba_EaFs2_Prv_GetMetaDataStatus(uint16 wipPattern_u16);


/**
 *********************************************************************
 * rba_EaFs2_Prv_DataWrite: Entry call to process a DataWrite
 *
 * This function initializes the data structure and state to start data operation
 *
 * \param    addr_u32:      address to write the data
 * \param    * bfr_pcu8:      ptr to buffer which holds the data to be written
 * \param    length_u16:      length of the data to be written
 * \param    blocknumber_u16:      block ID of the data to be written
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_DataWrite(uint32 addr_u32,uint8 const * bfr_pcu8,uint16 length_u16,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_WRITE_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = length_u16;
    rba_EaFs2_Prv_Data_st.bfr_pcu8        = bfr_pcu8;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRead: Entry call to process a DataRead
 *
 * This function initializes the data structure and state to start data operation
 *
 * \param    addr_u32:      address to Read the data
 * \param    * bfr_pcu8:      ptr to buffer to which the data is to be Read
 * \param    length_u16:      length of the data to be Read
 * \param    blocknumber_u16:      block ID of the data to be Read
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_DataRead(uint32 addr_u32,uint8 * bfr_pu8,uint16 length_u16,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_READ_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = length_u16;
    rba_EaFs2_Prv_Data_st.bfr_pu8         = bfr_pu8;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

/*Function to read data from eeprom and compute crc
 * This function will read the data split across multiple locations.
 *
 * */
void rba_EaFs2_Prv_DataCrcVerify(uint32 addr_u32,uint16 length_u16,uint16 blocknumber_u16,uint32 * crc_u32)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_CRC_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = length_u16;/* complete length of data as per config*/
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;
    rba_EaFs2_Prv_Data_st.crc_pu32        = crc_u32;
    *(rba_EaFs2_Prv_Data_st.crc_pu32)     = EA_MARKER_PATTERN;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataCompare: Entry call to process a DataCompare
 *
 * This function initializes the data structure and state to start data operation
 *
 * \param    addr_u32:      address to Compare the data with
 * \param    * bfr_pcu8:      ptr to buffer which holds the data that needs to be Compared
 * \param    length_u16:      length of the data block to be Compared
 * \param    blocknumber_u16:      block ID of the block to be Compared
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_DataCompare(uint32 addr_u32,uint8 const * bfr_pcu8,uint16 length_u16,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_COMPARE_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = length_u16;
    rba_EaFs2_Prv_Data_st.bfr_pcu8        = bfr_pcu8;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

/**
 * \brief    Function to write part1 of meta data by first encoding it into the metadata format
 *
 *\params    metaBlock_pst  pointer to the metadata entry to write
 *\params    addr_u32       start address of the metadata block
 *
 */
void rba_EaFs2_Prv_DataOpenMetaData(rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_EncodeMetaData(metaBlock_pst);

    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_WRITE_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = sizeof(metaBlock_pst->metaDataEntry_st.part1_st);
    rba_EaFs2_Prv_Data_st.bfr_pcu8        = (uint8 *)&metaBlock_pst->metaDataEntry_st.part1_st;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

/**
 * \brief    Function to write part2 of meta data.
 *
 *\params    metaBlock_pst  pointer to the metadata entry to write
 *\params    addr_u32       start address of the metadata block
 *
 *\attention This function must be called only after rba_EaFs2_Prv_OpenMetaData is called and meta block should
 *           remain consistent between the two calls
 */
void rba_EaFs2_Prv_DataCloseMetaData(rba_EaFs2_Prv_metaBlock_tst const * metaBlock_pcst,uint32 addr_u32,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_WRITE_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32 + sizeof(metaBlock_pcst->metaDataEntry_st.part1_st);
    rba_EaFs2_Prv_Data_st.length_u16      = sizeof(metaBlock_pcst->metaDataEntry_st.part2_st);
    rba_EaFs2_Prv_Data_st.bfr_pcu8        = (uint8 const *)&metaBlock_pcst->metaDataEntry_st.part2_st;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_ReadMetaData(): Place EEPROM Request to read MetaData
 *
 * This function place an EEPROM request to read the metadata
 *
 * \param    *metaBlock_pst:    ptr to MetaData to be read
 * \param    addr_u32:          Address from where to read metadata
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_DataReadMetaData(rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_READ_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = sizeof(metaBlock_pst->metaDataEntry_st);
    rba_EaFs2_Prv_Data_st.bfr_pu8        = (uint8 *)&metaBlock_pst->metaDataEntry_st;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

#if (EA_WRITE_SPREAD != FALSE)
/**
 *********************************************************************
 * rba_EaFs2_Prv_DataBlockCounterRead(): Place EEPROM Request to read WriteCounter for read/write operation
 *
 * This function place an EEPROM request to read the WriteCounter
 *
 * \param    *WriteCounterBlock_u32:    ptr to WriteCounter to be read
 * \param    addr_u32:          Address from where to read metadata
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_DataBlockCounterRead(uint32 * WriteCounterBlock_u32,uint32 addr_u32,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_READ_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = sizeof(WriteCounterBlock_u32)*2u;
    rba_EaFs2_Prv_Data_st.bfr_pu8        = (uint8 *)WriteCounterBlock_u32;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

#endif /*(EA_WRITE_SPREAD != FALSE)*/

/**
 * \brief    Function to invalidate block by writing both metadata part1 and part2 together. block status is
 *           markerd with 0xBADD and CRC is filled with 0.
 *
 *\params    metaBlock_pst  pointer to the metadata entry to write
 *\params    addr_u32       start address of the metadata block
 *
 */
void rba_EaFs2_Prv_DataInvalidateMetaData(rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_EncodeMetaData(metaBlock_pst);

    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_WRITE_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = sizeof(metaBlock_pst->metaDataEntry_st);
    rba_EaFs2_Prv_Data_st.bfr_pcu8        = (uint8 *)&metaBlock_pst->metaDataEntry_st;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EmptyMetaData(): Place EEPROM Request to write Empty MetaData
 *
 * This function encodes the metadata and place an EEPROM request to write the metadata
 *
 * \param    *metaBlock_pst:    ptr to MetaData to be written
 * \param    addr_u32:          Address to write empty meta
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_DataEmptyMetaData(rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16)
{
    metaBlock_pst->crc_u32 = 0;
    rba_EaFs2_Prv_EncodeMetaData(metaBlock_pst);

    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_WRITE_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = sizeof(metaBlock_pst->metaDataEntry_st);
    rba_EaFs2_Prv_Data_st.bfr_pcu8        = (uint8 const *)&metaBlock_pst->metaDataEntry_st;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}
#if (EA_WRITE_SPREAD != FALSE)
/**
 *********************************************************************
 * rba_EaFs2_Prv_BlockCounterWrite: Entry call to process a Block counter write
 *
 * This function initializes the data structure and state to start data operation
 *
 * \param    addr_u32:      address to write the data
 * \param    * bfr_pcu8:      ptr to buffer which holds the data to be written
 * \param    length_u16:      length of the data to be written
 * \param    blocknumber_u16:      block ID of the data to be written
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_BlockCounterWrite(uint32 addr_u32,uint8 const * bfr_pcu8,uint16 length_u16,uint16 blocknumber_u16)
{
    rba_EaFs2_Prv_Data_st.func_en         = RBA_EAFS2_PRV_DAT_FUNC_WRITE_E;
    rba_EaFs2_Prv_Data_st.adrNow_u32      = addr_u32;
    rba_EaFs2_Prv_Data_st.length_u16      = length_u16;
    rba_EaFs2_Prv_Data_st.bfr_pcu8        = bfr_pcu8;
    rba_EaFs2_Prv_Data_st.blocknumber_u16 = blocknumber_u16;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_Data_st,RBA_EAFS2_PRV_DATA_INIT_E);
}
#endif /*(EA_WRITE_SPREAD != FALSE)*/
/**
 *********************************************************************
 * rba_EaFs2_Prv_DataDo: Data process iterator
 *
 * This function iterates the internal states involved in data processing
 *
 * \param    void
 * \return   MemIf_JobResultType
 *********************************************************************
 */
MemIf_JobResultType rba_EaFs2_Prv_DataDo(void)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;
    rba_EaFs2_Prv_Data_tst * fsm_pst = &rba_EaFs2_Prv_Data_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_DATA_INIT_E:       result_en = rba_EaFs2_Prv_Data_Init       (fsm_pst); break;
            case RBA_EAFS2_PRV_DATA_SPLITSTART_E: result_en = rba_EaFs2_Prv_Data_SplitStart (fsm_pst); break;
            case RBA_EAFS2_PRV_DATA_CALL_E:       result_en = rba_EaFs2_Prv_Data_Call       (fsm_pst); break;
            case RBA_EAFS2_PRV_DATA_CHECK_E:      result_en = rba_EaFs2_Prv_Data_Check      (fsm_pst); break;
            default:                              result_en = MEMIF_JOB_FAILED;                        break;
        }
    }while((MEMIF_JOB_OK == result_en)&&(RBA_EAFS2_PRV_DATA_FINAL_E != fsm_pst->state_en));

    if(MEMIF_JOB_PENDING != result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_IDLE_E);
    }

    return result_en;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataDo_Init: Checks if the data block is split or not
 *
 * This function checks if the data block is split or not
 * SplitIndex 0 -> source
 * SplitIndex 1 -> clipboard
 * SplitIndex 2 -> Resume Location
 *
 * \param    * fsm_pst:      ptr to Data Structure
 * \return   MemIf_JobResultType : Always OK
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_Data_Init(rba_EaFs2_Prv_Data_tst * fsm_pst)
{
    rba_EaFs2_Prv_Data_st.nrBytesDone_u16 = 0u;

    if(rba_EaFs2_Prv_DefragBlk_st.movingId_u16 != rba_EaFs2_Prv_GetPersistentID(fsm_pst->blocknumber_u16))
    {
        /* Block to be accessed is not split*/
        fsm_pst->nrBytesToConsider_u16 = fsm_pst->length_u16;

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_CALL_E);
    }
    else
    {
        fsm_pst->splitIter_u8  = 0;
        /* Initial offset required to access data
         * addrDelta_u32 = 0 : metadata copy1
         * addrDelta_u32 = 8 : data     copy1
         * addrDelta_u32 = block length in medium     : meta data copy2
         * addrDelta_u32 = block length in medium + 8 : data      copy2
         * */
        fsm_pst->addrDelta_u32 = (fsm_pst->adrNow_u32 - rba_EaFs2_Prv_RamTable_st[fsm_pst->blocknumber_u16].address_u32);

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_SPLITSTART_E);
    }

    return MEMIF_JOB_OK;
}


/**
 *********************************************************************
 * rba_EaFs2_Prv_Data_SplitStart: Find the access address required for blocks which are split
 *
 * This function calls the EEPROM function for the respective data operation
 *
 * \param    * fsm_pst:      ptr to Data Structure
 * \return   MemIf_JobResultType : Always OK
 *********************************************************************
 */

static MemIf_JobResultType rba_EaFs2_Prv_Data_SplitStart(rba_EaFs2_Prv_Data_tst * fsm_pst)
{
    /* Add the done bytes to keep relativity with start address*/
    uint16 offset_u16 = (uint16)(fsm_pst->addrDelta_u32 + fsm_pst->nrBytesDone_u16);

    /*Iterate trough each of the possible split locations and find the part of the data in it*/
    for(; fsm_pst->splitIter_u8<=2 ; fsm_pst->splitIter_u8++)
    {
        /* Check if offset lies in each of the split regions */
        if(offset_u16 < rba_EaFs2_Prv_Split_st[fsm_pst->splitIter_u8].len_u16)
        {
            /* part of data is available in this, location data range to access is in the current iterator region*/
            fsm_pst->nrBytesToConsider_u16 = (uint16)rba_EaFs2_Prv_LibMin(
                    (rba_EaFs2_Prv_Split_st[fsm_pst->splitIter_u8].len_u16 - offset_u16),
                    fsm_pst->length_u16 - fsm_pst->nrBytesDone_u16);

            fsm_pst->adrNow_u32 = rba_EaFs2_Prv_Split_st[fsm_pst->splitIter_u8].addr_u32 + offset_u16;
            break;
        }
        else
        {
            /* Decrement offset when changing regions as the start address gets modified*/
            offset_u16 -= rba_EaFs2_Prv_Split_st[fsm_pst->splitIter_u8].len_u16;
        }
    }

    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_CALL_E);

    return MEMIF_JOB_OK;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataCall: Calls the Eeprom operation
 *
 * This function calls the EEPROM function for the respective data operation
 *
 * \param    * fsm_pst:      ptr to Data Structure
 * \return   MemIf_JobResultType : Always OK
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_Data_Call(rba_EaFs2_Prv_Data_tst * fsm_pst)
{
    switch(rba_EaFs2_Prv_Data_st.func_en)
    {
        case RBA_EAFS2_PRV_DAT_FUNC_READ_E:
            rba_EaFs2_Prv_EepromRead(
                    fsm_pst->adrNow_u32,
                    fsm_pst->bfr_pu8 + rba_EaFs2_Prv_Data_st.nrBytesDone_u16,
                    fsm_pst->nrBytesToConsider_u16
            );
            break;

        case RBA_EAFS2_PRV_DAT_FUNC_WRITE_E:
            rba_EaFs2_Prv_EepromWrite(
                    fsm_pst->adrNow_u32,
                    fsm_pst->bfr_pcu8 + rba_EaFs2_Prv_Data_st.nrBytesDone_u16,
                    fsm_pst->nrBytesToConsider_u16
            );
            break;

        case RBA_EAFS2_PRV_DAT_FUNC_COMPARE_E:
            rba_EaFs2_Prv_EepromCompare(
                    fsm_pst->adrNow_u32,
                    fsm_pst->bfr_pcu8 + rba_EaFs2_Prv_Data_st.nrBytesDone_u16,
                    fsm_pst->nrBytesToConsider_u16
            );
            break;

        case RBA_EAFS2_PRV_DAT_FUNC_CRC_E:
        {
            fsm_pst->nrBytesToConsider_u16 = (uint16)rba_EaFs2_Prv_LibMin(
                    fsm_pst->nrBytesToConsider_u16,
                    EA_DATA_BUFFER_SIZE
            );

            rba_EaFs2_Prv_EepromRead(
                    fsm_pst->adrNow_u32,
                    &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
                    fsm_pst->nrBytesToConsider_u16
            );
            break;
        }

        default:  /* Intentionally Left Blank - No Operation */
            break;
    }

    RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_CHECK_E);

    return MEMIF_JOB_OK;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataDo_Check: Checks if the data operation is completed
 *
 * This function checks if the data operation is completed for all bytes considered
 *
 * \param    * fsm_pst:      ptr to Data Structure
 * \return   MemIf_JobResultType : Eeprom Job Status
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_Data_Check(rba_EaFs2_Prv_Data_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;

    result_en = rba_EaFs2_Prv_EepromStatus();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->nrBytesDone_u16 += fsm_pst->nrBytesToConsider_u16;

        if(RBA_EAFS2_PRV_DAT_FUNC_CRC_E == fsm_pst->func_en)
        {
            *(fsm_pst->crc_pu32) = Crc_CalculateCRC32(&rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
                    fsm_pst->nrBytesToConsider_u16,
                    *(fsm_pst->crc_pu32),
                    (boolean)FALSE
            );
        }

        if(fsm_pst->nrBytesDone_u16 == fsm_pst->length_u16)
        {
            /* Data operation is complete*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_FINAL_E);
        }
        else
        {
            /* data access did not finish in one cycle*/
            rba_EaFs2_Prv_Data_st.adrNow_u32 += fsm_pst->nrBytesToConsider_u16;

            if(rba_EaFs2_Prv_DefragBlk_st.movingId_u16 != rba_EaFs2_Prv_GetPersistentID(fsm_pst->blocknumber_u16))
            {
                fsm_pst->nrBytesToConsider_u16 = (fsm_pst->length_u16 - fsm_pst->nrBytesDone_u16);
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_CALL_E);
            }
            else
            {
                fsm_pst->splitIter_u8 = 0;
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATA_SPLITSTART_E);
            }
        }
    }

    return result_en;
}

/*
 *
 *********************************************************************
 * rba_EaFs2_Prv_DecodeMetaData: Decodes the metadata in userreadable format
 *
 * This function converts the metadata from layout pattern to userreadable format
 *
 * \param    * metaData_pst:      ptr to MetaData Structure to decode
 * \return   void
 *
 * Meta data is made up of two parts : Part1 and Part2
 * crc_u16 of part1 contains the upper two bytes of the crc and part2 contains the lower half
 * The function calling this API must use the isConsistent_b flag to ensure the metadata is valid before processing
 *
 * | 31..16 |          | 15..8  | 15...0  |           | 7..0   |
 * |  crc   | validity |   wip  |   crc   |  validity |   wip  |
 * | 2 byte |  1 byte  | 1 byte |  2 byte |   1 byte  | 1 byte |
 *
 * <------------Part1----------><-----------Part2-------------->
 *
 **********************************************************************
 * */
void rba_EaFs2_Prv_DecodeMetaData(rba_EaFs2_Prv_metaBlock_tst * metaData_pst)
{
    uint16 validity_u16;

    /* 1. Metadata decode begins with parsing the MSByte and LSByte of the CRC
     * 2. Check if the Write in progress is in correct state
     *     2a. if   Yes, Check the validity status
     *         2aa. if    its either valid or invalidated, update the consistency flag
     *         2ab. else  set the consistency flag as false
     *     2b. else set the consistency flag as false
     * */

    metaData_pst->crc_u32 = (
            ((uint32)metaData_pst->metaDataEntry_st.part1_st.crc_u16 << 16) |
            (metaData_pst->metaDataEntry_st.part2_st.crc_u16)
    );

    metaData_pst->wipPattern_u16 = (
            ((uint16)metaData_pst->metaDataEntry_st.part1_st.wipState_u8 << 8) +
            (metaData_pst->metaDataEntry_st.part2_st.wipState_u8)
    );

    metaData_pst->wip_en = rba_EaFs2_Prv_GetMetaDataStatus(metaData_pst->wipPattern_u16);

    if(RBA_EAFS2_PRV_WIP_COMPLETE_E == metaData_pst->wip_en)
    {
        metaData_pst->isConsistent_b = (boolean)TRUE;

        validity_u16 = (
                ((uint16)metaData_pst->metaDataEntry_st.part1_st.validity_u8 << 8) +
                (metaData_pst->metaDataEntry_st.part2_st.validity_u8)
        );

        if(RBA_EAFS2_PRV_VALIDPATTERN == validity_u16)
        {
            metaData_pst->status_en = RBA_EAFS2_PRV_BLOCK_VALID_E;
        }
        else if(RBA_EAFS2_PRV_INVALIDPATTERN == validity_u16)
        {
            metaData_pst->status_en = RBA_EAFS2_PRV_BLOCK_INVALIDATED_E;
        }
        else
        {
            metaData_pst->isConsistent_b = (boolean)FALSE;
            metaData_pst->status_en = RBA_EAFS2_PRV_BLOCK_VALID_E;
        }
    }
    else
    {
        metaData_pst->isConsistent_b = (boolean)FALSE;
        metaData_pst->status_en = RBA_EAFS2_PRV_BLOCK_VALID_E;
    }

}

/**
 *********************************************************************
 * rba_EaFs2_Prv_EncodeMetaData: Encodes the metadata in layout format
 *
 * This function converts the metadata from userreadable format to layout pattern
 *
 * \param    * metaData_pst:      ptr to MetaData Structure to encode
 * \return   void
 *********************************************************************
 */
static void rba_EaFs2_Prv_EncodeMetaData(rba_EaFs2_Prv_metaBlock_tst * metaData_pst)
{
    metaData_pst->metaDataEntry_st.part1_st.crc_u16 = (uint16)((metaData_pst->crc_u32 >> 16));
    metaData_pst->metaDataEntry_st.part2_st.crc_u16 = (uint16)(metaData_pst->crc_u32);

    if(RBA_EAFS2_PRV_BLOCK_VALID_E == metaData_pst->status_en)
    {
        metaData_pst->metaDataEntry_st.part1_st.validity_u8 = (uint8)(RBA_EAFS2_PRV_VALIDPATTERN >> 8);
        metaData_pst->metaDataEntry_st.part2_st.validity_u8 = (uint8)(RBA_EAFS2_PRV_VALIDPATTERN & 0xFFuL);
    }
    else
    {
        metaData_pst->metaDataEntry_st.part1_st.validity_u8 = (uint8)(RBA_EAFS2_PRV_INVALIDPATTERN >> 8);
        metaData_pst->metaDataEntry_st.part2_st.validity_u8 = (uint8)(RBA_EAFS2_PRV_INVALIDPATTERN & 0xFFuL);
    }

    if(0xAAAAuL == metaData_pst->wipPattern_u16)
    {
        metaData_pst->metaDataEntry_st.part1_st.wipState_u8 = (uint8)(0xAA);
        metaData_pst->metaDataEntry_st.part2_st.wipState_u8 = (uint8)(0xAA);
    }
    else if(0x5555 == metaData_pst->wipPattern_u16)
    {
        metaData_pst->metaDataEntry_st.part1_st.wipState_u8 = (uint8)(0x55);
        metaData_pst->metaDataEntry_st.part2_st.wipState_u8 = (uint8)(0x55);
    }
    else
    {
        metaData_pst->metaDataEntry_st.part1_st.wipState_u8 = (uint8)(0xFF);
        metaData_pst->metaDataEntry_st.part2_st.wipState_u8 = (uint8)(0xFF);
    }
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair: Entry call to process a repair request
 *
 * This function initializes the repair structure and state to start repair
 *
 * \param    blocknumber_u16:      block ID of the block to be repaired
 * \return   void
 *********************************************************************
 */
void rba_EaFs2_Prv_DataRepair(void)
{
    rba_EaFs2_Prv_DataRepair_st.order_pcst         = &Ea_Prv_Order_st;
    rba_EaFs2_Prv_DataRepair_st.copyState_u8       = 0;
    rba_EaFs2_Prv_DataRepair_st.calculatedCRC_u32  = 0;

    RBA_EAFS2_PRV_LIBTRANS(rba_EaFs2_Prv_DataRepair_st,RBA_EAFS2_PRV_DATAREPAIR_INIT_E);
}

/*
 *  Repair function to ensure that both copies for a redundant blocks are same
 *  Reads the first page from the 2nd copy compare against the 1st copy
 *  NextRelease: Merge the medium to medium copy inside the wrapper function for reuse
 *  */
MemIf_JobResultType rba_EaFs2_Prv_DataRepairDo( void )
{
    MemIf_JobResultType result_en;
    rba_EaFs2_Prv_DataRepair_tst * fsm_pst = &rba_EaFs2_Prv_DataRepair_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case RBA_EAFS2_PRV_DATAREPAIR_INIT_E:        result_en = rba_EaFs2_Prv_DataRepair_Init       (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_READMETA_E:    result_en = rba_EaFs2_Prv_DataRepair_ReadMeta   (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_VERIFYCRC_E:   result_en = rba_EaFs2_Prv_DataRepair_VerifyCRC  (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_READDATA_E:    result_en = rba_EaFs2_Prv_DataRepair_ReadData   (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_PROCESS_E:     result_en = rba_EaFs2_Prv_DataRepair_Process    (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_WRITEDATA_E:   result_en = rba_EaFs2_Prv_DataRepair_WriteData  (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_OPENMETA_E:    result_en = rba_EaFs2_Prv_DataRepair_OpenMeta   (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_CLOSEMETA_E:   result_en = rba_EaFs2_Prv_DataRepair_CloseMeta  (fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_INVMETA_E:     result_en = rba_EaFs2_Prv_DataRepair_InvMeta    (fsm_pst); break;
#if (EA_WRITE_SPREAD != FALSE)
            case RBA_EAFS2_PRV_DATAREPAIR_GETLOCATION_E:  result_en = rba_EaFs2_Prv_DataRepair_GetLocation(fsm_pst); break;
            case RBA_EAFS2_PRV_DATAREPAIR_UPDATECOUNTER_E:result_en = rba_EaFs2_Prv_DataRepair_UpdateCounter(fsm_pst); break;
#endif /*(EA_WRITE_SPREAD != FALSE)*/
            default:                                   result_en = MEMIF_JOB_FAILED;                             break;
        }
    } while( (result_en == MEMIF_JOB_OK) && (RBA_EAFS2_PRV_DATAREPAIR_FINAL_E != fsm_pst->state_en) );

    if(MEMIF_JOB_PENDING != result_en)
    {
        if(MEMIF_JOB_OK == result_en)
        {
            rba_EaFs2_Prv_UpdateRepairStatus(fsm_pst->block_st.blockNumber_u16);
        }
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_IDLE_E);
    }

    return result_en;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_Init: Initialize the Repair structure
 *
 * This function initializes the repair structure
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of metadata read
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_Init(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    fsm_pst->block_st             = rba_EaFs2_Prv_BlockFromOrder(fsm_pst->order_pcst);
    fsm_pst->block_st.address_u32 = rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].address_u32;
    fsm_pst->blockLength_u16      = rba_EaFs2_Prv_BlkLengthInMedium(fsm_pst->block_st.blockNumber_u16);
    fsm_pst->currCopy_u8    = 1;
    fsm_pst->metaBlock_pst  = &fsm_pst->metaBlock1_st;

#if (EA_WRITE_SPREAD != FALSE)
    if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
    {
        if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32==0u)
        {
            fsm_pst->CurrentLocationFound_b = (boolean)FALSE;
            fsm_pst->CurrentLocation_u8 = 1; /* Iterate from 1 to n locations */
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_GETLOCATION_E);
        }
        else
        {
            fsm_pst->block_st.address_u32       = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32;
            fsm_pst->blockWriteCounter_u32[0]   = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
            fsm_pst->blockWriteCounter_u32[1]   = fsm_pst->blockWriteCounter_u32[0];
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_READMETA_E);
        }
    }
    else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_READMETA_E);
    }

    if(rba_EaFs2_Prv_RamTable_st[fsm_pst->block_st.blockNumber_u16].isSecurityLevelChanged_b != (boolean)FALSE)
    {
        /* Block Marked for SL1 to SL2 property change, Skip Block Maintain with positive response until property change */
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_FINAL_E);
    }

    return (MEMIF_JOB_OK);
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_ReadMeta: Read Block MetaData
 *
 * This function reads the metadata for the block under repair
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of metadata read
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_ReadMeta(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataReadMetaData(
                fsm_pst->metaBlock_pst,
                fsm_pst->block_st.address_u32,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        rba_EaFs2_Prv_DecodeMetaData(fsm_pst->metaBlock_pst);

        if((boolean)FALSE != fsm_pst->metaBlock_pst->isConsistent_b)
        {
            /* Set the bit for block status i.e. valid or invalidated*/
            fsm_pst->copyState_u8 |= ((uint8)fsm_pst->metaBlock_pst->status_en<<(fsm_pst->currCopy_u8 + 1uL));

            if(RBA_EAFS2_PRV_BLOCK_VALID_E == fsm_pst->metaBlock_pst->status_en)
            {
                /* Block Valid proceed for CRC check */
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_VERIFYCRC_E);
            }
            else
            {
                /* Set the block as consitent for invalidated copies*/
                fsm_pst->copyState_u8 |= ((uint8)1 <<(fsm_pst->currCopy_u8 - 1uL));
            }
        }
        /* copy state is not updated in case of inconsistent metadata, default state being corrupted*/

        if(RBA_EAFS2_PRV_DATAREPAIR_VERIFYCRC_E != fsm_pst->state_en)
        {
            if(1 == fsm_pst->currCopy_u8)
            {
                fsm_pst->currCopy_u8           = 2;
                fsm_pst->block_st.address_u32 += fsm_pst->blockLength_u16;
                fsm_pst->metaBlock_pst         = &fsm_pst->metaBlock2_st;

                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_READMETA_E);
            }
            else
            {
                /* Both the copies evaluated proceed for processing */
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_PROCESS_E);
            }
        }
    }

    return result_en;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_VerifyCRC: Validate the CRC against Data
 *
 * This function computes CRC from EEPROM data and validate the CRC written in Metadata
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of CRC validation
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_VerifyCRC(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataCrcVerify(
                (fsm_pst->block_st.address_u32 + sizeof(rba_EaFs2_Prv_metaDataEntry_tst)),
                fsm_pst->block_st.length_u16,
                fsm_pst->block_st.blockNumber_u16,
                &fsm_pst->calculatedCRC_u32
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        if (fsm_pst->metaBlock_pst->crc_u32 != fsm_pst->calculatedCRC_u32)
        {
            fsm_pst->metaBlock_pst->isConsistent_b = (boolean)FALSE;
        }
        else
        {
            /* Set the bit for consistency*/
            fsm_pst->copyState_u8 |= ((uint8)1 <<(fsm_pst->currCopy_u8 - 1uL));
        }

        /* Start repair operation for SL2 blocks*/
        if(1 == fsm_pst->currCopy_u8)
        {
            /* Copy1 read continue reading Copy2, Store the next meta data read into block2 */
            fsm_pst->block_st.address_u32     += fsm_pst->blockLength_u16;
            fsm_pst->currCopy_u8   = 2;
            fsm_pst->metaBlock_pst = &fsm_pst->metaBlock2_st;

            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_READMETA_E);
        }
        else
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_PROCESS_E);
        }

    }

    return result_en;
}

/*
 * V  ->  Valid
 * C  ->  Corrupt
 * I  ->  Invalidated
 * V+ ->  Valid but different Data
 *
 *    Scenario    I   II  III IV  V   VI  VII VIII IX  X
 *    Copy1       V   V   V   C   V   C   I   I    I   C
 *    Copy2       V   V+  C   V   I   I   V   C    I   C
 *
 *    currCopy should be updated to the copy to be written to
 *
 *    addr_u32 now points to start address of copy2 block
 *    After processing read and write points to start of corresponding blocks
 **/
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_Process(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    switch(fsm_pst->copyState_u8)
    {
        case RBA_EAFS2_PRV_DATACOPYSTATE_VV:
        {
            if(fsm_pst->metaBlock1_st.crc_u32 == fsm_pst->metaBlock2_st.crc_u32)
            {
                /* VV*/
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_FINAL_E);
            }
            else
            {
                /* VV+ - copy from copy2 to 1, addr is currently at copy2*/
                fsm_pst->currCopy_u8                    = 1;
                fsm_pst->metaBlock_pst                 = &fsm_pst->metaBlock2_st;
                fsm_pst->metaBlock2_st.wipPattern_u16  = rba_EaFs2_Prv_GetWipToUse(
                        fsm_pst->metaBlock1_st.metaDataEntry_st.part2_st.wipState_u8);
                fsm_pst->readAddr_u32                   = fsm_pst->block_st.address_u32;
                fsm_pst->writeAddr_u32                  = fsm_pst->block_st.address_u32 - fsm_pst->blockLength_u16;
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_OPENMETA_E);
            }
        }
        break;
        case RBA_EAFS2_PRV_DATACOPYSTATE_IV:
        case RBA_EAFS2_PRV_DATACOPYSTATE_CV:
        {
            /* IV, CV - copy from copy2 to 1, addr is currently at copy2*/
            fsm_pst->currCopy_u8                    = 1;
            fsm_pst->metaBlock_pst                  = &fsm_pst->metaBlock2_st;
            fsm_pst->metaBlock2_st.wipPattern_u16  = rba_EaFs2_Prv_GetWipToUse(
                    fsm_pst->metaBlock1_st.metaDataEntry_st.part2_st.wipState_u8);
            fsm_pst->readAddr_u32                   = fsm_pst->block_st.address_u32;
            fsm_pst->writeAddr_u32                  = fsm_pst->block_st.address_u32 - fsm_pst->blockLength_u16;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_OPENMETA_E);
        }
        break;
        case RBA_EAFS2_PRV_DATACOPYSTATE_VI:
        case RBA_EAFS2_PRV_DATACOPYSTATE_CI:
        {
            /* VI, CI Invalidate copy1 */
            fsm_pst->currCopy_u8   = 1;
            fsm_pst->metaBlock_pst = &fsm_pst->metaBlock1_st;
            fsm_pst->writeAddr_u32 = fsm_pst->block_st.address_u32 - fsm_pst->blockLength_u16;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_INVMETA_E);
        }
        break;
        case RBA_EAFS2_PRV_DATACOPYSTATE_VC:
        {
            /* VC - copy from copy1 to 2, addr is currently at copy2 */
            fsm_pst->currCopy_u8                    = 2;
            fsm_pst->metaBlock_pst                 = &fsm_pst->metaBlock1_st;
            fsm_pst->metaBlock1_st.wipPattern_u16  = rba_EaFs2_Prv_GetWipToUse(
                    fsm_pst->metaBlock2_st.metaDataEntry_st.part2_st.wipState_u8);
            fsm_pst->readAddr_u32                   = fsm_pst->block_st.address_u32 - fsm_pst->blockLength_u16;
            fsm_pst->writeAddr_u32                  = fsm_pst->block_st.address_u32;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_OPENMETA_E);
        }
        break;
        case RBA_EAFS2_PRV_DATACOPYSTATE_IC:
        {
            /* IC Invalidate copy2 */
            fsm_pst->currCopy_u8   = 2;
            fsm_pst->metaBlock_pst = &fsm_pst->metaBlock2_st;
            fsm_pst->writeAddr_u32 = fsm_pst->block_st.address_u32;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_INVMETA_E);
        }
        break;
        case RBA_EAFS2_PRV_DATACOPYSTATE_CC:
        case RBA_EAFS2_PRV_DATACOPYSTATE_II:
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_FINAL_E);
            break;
        }

        default:
            /* Should not reach this place - if reached go to FINAL*/
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_FINAL_E);
            break;

    }

    return MEMIF_JOB_OK;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_OpenMeta: Write part1 of Metadata
 *
 * This function writes the part1 of Metadata and returns the job result
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of metadata write
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_OpenMeta(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataOpenMetaData(fsm_pst->metaBlock_pst,
                fsm_pst->writeAddr_u32,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->remainingLength_u16   =  fsm_pst->block_st.length_u16;
        fsm_pst->writeAddr_u32        +=  sizeof(rba_EaFs2_Prv_metaDataEntry_tst);
        fsm_pst->readAddr_u32         +=  sizeof(rba_EaFs2_Prv_metaDataEntry_tst);

        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_READDATA_E);
    }

    return result_en;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_CloseMeta: Write part2 of Metadata
 *
 * This function writes the part2 of Metadata and returns the job result
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of metadata write
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_CloseMeta(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataCloseMetaData(fsm_pst->metaBlock_pst,
                fsm_pst->writeAddr_u32,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
#if (EA_WRITE_SPREAD != FALSE)
        if(fsm_pst->block_st.writeSpread_b != (boolean)FALSE)
        {
            if(rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32<EA_EEP_ALLOWED_WRITE_CYCLES)
            {
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 +=1u;
            }
            fsm_pst->blockWriteCounter_u32[0] = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32;
            fsm_pst->blockWriteCounter_u32[1] = fsm_pst->blockWriteCounter_u32[0];
            fsm_pst->currCopyCounter_u8 = 2;
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_UPDATECOUNTER_E);
        }
        else
#endif /*(EA_WRITE_SPREAD != FALSE)*/
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_FINAL_E);
        }
    }

    return result_en;
}


/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_ReadData: Read a copy of the block data
 *
 * This function reads the block data and returns the job result
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of Read Data
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_ReadData(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        if(fsm_pst->remainingLength_u16 > EA_DATA_BUFFER_SIZE)
        {
            fsm_pst->lengthToConsider_u16 = EA_DATA_BUFFER_SIZE;
        }
        else
        {
            fsm_pst->lengthToConsider_u16 = fsm_pst->remainingLength_u16;
        }

        rba_EaFs2_Prv_DataRead(
                fsm_pst->readAddr_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
                fsm_pst->lengthToConsider_u16,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_WRITEDATA_E);
    }

    return result_en;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_WriteData: Write the block data
 *
 * This function writes the block data and returns the job result
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of Write Data
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_WriteData(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataWrite(
                fsm_pst->writeAddr_u32,
                &rba_EaFs2_Prv_Eeprom_st.buffer_u.u8[0u],
                fsm_pst->lengthToConsider_u16,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->remainingLength_u16 -= fsm_pst->lengthToConsider_u16;
        fsm_pst->readAddr_u32   += fsm_pst->lengthToConsider_u16;
        fsm_pst->writeAddr_u32  += fsm_pst->lengthToConsider_u16;

        if(0 < fsm_pst->remainingLength_u16)
        {
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_READDATA_E);
        }
        else
        {
            /* Reset Write address to start of block to write close metadata*/
            fsm_pst->writeAddr_u32 -= (fsm_pst->block_st.length_u16 + (uint16)sizeof(rba_EaFs2_Prv_metaDataEntry_tst));
            RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_CLOSEMETA_E);
        }
    }

    return result_en;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_InvMeta: Invalidata the block metadata
 *
 * This function invalidates the block metadata and returns the job result
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of Invalidata MetaData
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_InvMeta(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        fsm_pst->metaBlock_pst->wipPattern_u16  = rba_EaFs2_Prv_GetWipToUse(
                fsm_pst->metaBlock_pst->metaDataEntry_st.part2_st.wipState_u8
        );
        fsm_pst->metaBlock_pst->crc_u32         = 0;
        fsm_pst->metaBlock_pst->status_en       = RBA_EAFS2_PRV_BLOCK_INVALIDATED_E;

        rba_EaFs2_Prv_DataInvalidateMetaData(
                fsm_pst->metaBlock_pst,
                fsm_pst->writeAddr_u32,
                fsm_pst->block_st.blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_FINAL_E);
    }

    return result_en;
}

#if (EA_WRITE_SPREAD != FALSE)
/**
 *********************************************************************
 * rba_EaFs2_Prv_DataRepair_GetLocation: Get the current location of write spread block
 *
 * \param    *fsm_pst:      ptr to DataRepair Internal Structure
 * \return   MemIf_JobResultType: Job Result of rba_EaFs2_Prv_DataRepair_GetLocation
 *********************************************************************
 */
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_GetLocation(rba_EaFs2_Prv_DataRepair_tst  * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint8 NumberOfwriteLocations_u8;

    if(RBA_EAFS2_PRV_LIBENTRY)
    {
        rba_EaFs2_Prv_DataBlockCounterRead(
                (uint32 *)&fsm_pst->blockWriteCounter_u32[0],
                (fsm_pst->block_st.address_u32 + ((fsm_pst->blockLength_u16*2)*fsm_pst->CurrentLocation_u8) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1))),
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
            NumberOfwriteLocations_u8 = rba_EaFs2_Prv_WriteSpreadRequired(fsm_pst->block_st.blockNumber_u16);
            if((fsm_pst->blockWriteCounter_u32[0] >= EA_EEP_ALLOWED_WRITE_CYCLES)&&( NumberOfwriteLocations_u8 > fsm_pst->CurrentLocation_u8))
            {
                fsm_pst->CurrentLocation_u8++;
                fsm_pst->blockWriteCounterT1_u32 = fsm_pst->blockWriteCounter_u32[0]; /* Store the write counter of the previous location*/
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_GETLOCATION_E);
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
                fsm_pst->block_st.address_u32 = fsm_pst->block_st.address_u32 + (((fsm_pst->blockLength_u16*2)*(fsm_pst->CurrentLocation_u8-1)) + (RBA_EAFS2_PRV_WRITESPREAD_COUNTER_SIZE *(fsm_pst->CurrentLocation_u8 -1)));
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32 = fsm_pst->block_st.address_u32; /*Store the current address for subsequent read operation*/
                rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currCounter_u32 = fsm_pst->blockWriteCounter_u32[0];
                RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_READMETA_E);
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
static MemIf_JobResultType rba_EaFs2_Prv_DataRepair_UpdateCounter(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    uint8 currCopy_u8 =0;
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
        fsm_pst->block_st.address_u32 = rba_EaFs2_Prv_WriteSpreadRamTable_st[rba_EaFs2_Prv_WriteSpreadRamTableIndex(fsm_pst->order_pcst->blockNumber_u16)].currAddress_u32;
        rba_EaFs2_Prv_BlockCounterWrite(
                fsm_pst->block_st.address_u32 + (fsm_pst->blockLength_u16*currCopy_u8) + ((fsm_pst->currCopyCounter_u8-1)*RBA_EAFS2_PRV_WRITESPREAD_COUNTER_LENGTH),
                (uint8 *)&fsm_pst->blockWriteCounter_u32,
                RBA_EAFS2_PRV_WRITESPREAD_COUNTER_LENGTH,
                fsm_pst->order_pcst->blockNumber_u16
        );
    }

    result_en = rba_EaFs2_Prv_DataDo();

    if(MEMIF_JOB_OK == result_en)
    {
        rba_EaFs2_Prv_DataRepair_Counter2ndCopy(fsm_pst);
    }

    return result_en;
}

static void rba_EaFs2_Prv_DataRepair_Counter2ndCopy(rba_EaFs2_Prv_DataRepair_tst * fsm_pst)
{
    fsm_pst->currCopyCounter_u8--;

    if(0u < fsm_pst->currCopyCounter_u8)
    {
        /* Write the redundant copy*/
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_UPDATECOUNTER_E);
    }
    else
    {
        RBA_EAFS2_PRV_LIBSC(RBA_EAFS2_PRV_DATAREPAIR_FINAL_E);
    }
}
#endif /*(EA_WRITE_SPREAD != FALSE)*/
/**
 *********************************************************************
 * rba_EaFs2_Prv_GetMetaDataStatus(): Return if MetaData is complete or ongoing
 *
 * This function checks the wippattern and return if the block is written complete
 *
 * \param    wipPattern_u16:       Write in Progress pattern of a block
 * \return   WIP State
 * \retval   RBA_EAFS2_PRV_WIP_COMPLETE_E:      WIP is complete
 * \retval   RBA_EAFS2_PRV_WIP_ONGOING_E:       WIP is ongoing
 *********************************************************************
 */
static rba_EaFs2_Prv_wipState_en rba_EaFs2_Prv_GetMetaDataStatus(uint16 wipPattern_u16)
{
    rba_EaFs2_Prv_wipState_en result_en = RBA_EAFS2_PRV_WIP_ONGOING_E;

    if((0xAAAAuL == wipPattern_u16) || (0x5555uL == wipPattern_u16) )
    {
        result_en = RBA_EAFS2_PRV_WIP_COMPLETE_E;
    }

    return result_en;
}

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
