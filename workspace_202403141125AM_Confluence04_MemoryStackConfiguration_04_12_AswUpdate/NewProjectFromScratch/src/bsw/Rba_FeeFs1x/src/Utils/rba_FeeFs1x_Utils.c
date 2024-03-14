/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

#if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x.h"
#include "rba_FeeFs1x_Prv_Cfg.h"

#include "rba_FeeFs1x_Prv_Utils.h"
#include "rba_FeeFs1x_Prv_UtilsTypes.h"

#include "Crc.h"
#include "rba_MemLib.h"
#include "rba_FeeFs1x_Prv_Sector.h"
#include "rba_FeeFs1x_Prv_SectorTypes.h"
#include "rba_FeeFs1x_Prv_SectorReorg.h"
#include "rba_FeeFs1x_Prv_PAMap.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"


static rba_FeeFs1x_Utils_compCrc_data_tst rba_FeeFs1x_Utils_compCrc_data_st;


#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/**
 *********************************************************************
 * rba_FeeFs1x_Utils_computeUserBufferCrc
 * This function calculates the CRC over user buffer. This is asynchronous and
 * would calculate the CRC in chunks.
 *
 * \param   *bufferPtr_pcu8 : pointer to user buffer
 * \param   length_u32      : length of the complete area over which CRC must be calculated (i.e., block length)
 * \param   *crc_pu32       : pointer in which CRC is returned
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Utils_computeUserBufferCrc(uint8 const * bufferPtr_pcu8, uint32 length_u32, uint32 * crc_pu32)
{
    rba_FeeFs1x_Utils_compCrc_data_st.bufferPtr_pcu8     = bufferPtr_pcu8;
    rba_FeeFs1x_Utils_compCrc_data_st.crc_pu32           = crc_pu32;
    rba_FeeFs1x_Utils_compCrc_data_st.bytesDone_u16      = 0u;
    rba_FeeFs1x_Utils_compCrc_data_st.bytesRemaining_u16 = (uint16)length_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Utils_computeUserBufferCrcDo
 * This is the Do function that calculates the CRC over user buffer.
 *
 * \param   void
 * \return  rba_FeeFs1x_Pending_e   : CRC calculation is pending
 * \return  rba_FeeFs1x_JobOK_e     : CRC calculation completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Utils_computeUserBufferCrcDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint16 bytesInCurrentChunk_u16;

    // limit the chunk size calculated within one cycle
    bytesInCurrentChunk_u16 = (uint16) rba_MemLib_Min
            (
                    rba_FeeFs1x_Utils_compCrc_data_st.bytesRemaining_u16,
                    RBA_FEEFS1X_UTILS_RAMCRC_CALC_RESTRICTION
            );

    // calculate the CRC over the given chunk
    *(rba_FeeFs1x_Utils_compCrc_data_st.crc_pu32) =
            Crc_CalculateCRC32(
                    &(rba_FeeFs1x_Utils_compCrc_data_st.bufferPtr_pcu8[rba_FeeFs1x_Utils_compCrc_data_st.bytesDone_u16]),
                    bytesInCurrentChunk_u16,
                    *(rba_FeeFs1x_Utils_compCrc_data_st.crc_pu32),
                    FALSE
            );

    // prepare the next cycle
    rba_FeeFs1x_Utils_compCrc_data_st.bytesRemaining_u16 -= bytesInCurrentChunk_u16;
    rba_FeeFs1x_Utils_compCrc_data_st.bytesDone_u16      += bytesInCurrentChunk_u16;

    // end condition: If everything is processed, return job ok
    if(rba_FeeFs1x_Utils_compCrc_data_st.bytesRemaining_u16 > 0u)
    {
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
            retVal_en = rba_FeeFs1x_JobOK_e;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_getSectChngCnt
 * Function returns the current sector change counter value
 * \return  sector change counter
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_GetSectorChgCntr(void)
{
    return (rba_FeeFs1x_Sector_getSectChngCnt());
}

/**
 *********************************************************************
 * rba_FeeFs1x_getFreeSpace
 * Function returns the amount of free space in the DFLASH. this is only the area where user blocks are stored and
 * does not include the area reserved for sector headers.
 *
 * \return  amount of free space in bytes
 * \seealso
 * \usedresources
 *********************************************************************
 */
#  if(RBA_FEEFS1X_PRV_CFG_GET_NR_FREE_BYTES != FALSE)
uint32 rba_FeeFs1x_GetFreeSpace(void)
{
    rba_FeeFs1x_Sector_SectorState_ten sectorState_en;
    uint32 nrPagesPerSector = rba_FeeFs1x_Sector_getNrOfLogPagesPerSector();
    uint32 nrLogPagesInDFLASH_u32 = 0u;
    uint8 i_u8;

    for(i_u8 = 0; i_u8 < rba_FeeFs1x_Sector_getTotalNrOfSectors(); i_u8++)
    {
        sectorState_en = rba_FeeFs1x_Sector_getState(i_u8);

        if(rba_FeeFs1x_Sector_SectorState_USED_e == sectorState_en)
        {
            nrLogPagesInDFLASH_u32 += ((rba_FeeFs1x_Sector_getLogStartPage(i_u8) + nrPagesPerSector)
                                        - rba_FeeFs1x_PAMap_getCurrWrPage());
        }
        else if(rba_FeeFs1x_Sector_SectorState_ERASED_e == sectorState_en)
        {
            nrLogPagesInDFLASH_u32 += nrPagesPerSector;
        }
        else
        {
            // Do not consider sectors in FULL, UNDEF, R2E states
        }
    }

    return (nrLogPagesInDFLASH_u32 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);
}
#endif
/**
 *********************************************************************
 *
 * Get current state of Soft reorg
 *
 * \return  TRUE = Soft reorg is running, FALSE =  Soft reorg not running
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_IsSoftReorgOngoing(void)
{
    return (rba_FeeFs1x_Reorg_isSoftReorgOngoing());
}

/**
 *********************************************************************
 *
 * Get current state of Hard reorg
 *
 * \return  TRUE = Hard reorg is running, FALSE =  Hard reorg not running
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_IsHardReorgOngoing(void)
{
    return (rba_FeeFs1x_Reorg_isHardReorgOngoing());
}

/**
 *********************************************************************
 *
 * Get current state of Sector erase
 *
 * \return  TRUE = Sector erase is running, FALSE = Sector erase not running
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_IsSectorEraseOngoing(void)
{
    return (rba_FeeFs1x_Sector_isSectorEraseOngoing());
}

/**
 *********************************************************************
 * Get current state of Sector
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   idxFeeSectPhys_u8       Physical sector of which the sector state shall be returned
 *
 * \return  Fee_Rb_SectState_ten    Map sector state to Fee high level simplifed sector state
 * \seealso
 * \usedresources
 *********************************************************************
 */
#  if(RBA_FEEFS1X_PRV_CFG_GET_SECT_STATE_API != FALSE)
Fee_Rb_SectState_ten rba_FeeFs1x_GetSectorState(uint8 idxFeeSectPhys_u8)
{
    Fee_Rb_SectState_ten stRet_en;
    uint8                idxSectLog_u8 = rba_FeeFs1x_Sector_getLogIdxByPhysIdx(idxFeeSectPhys_u8);

    switch(rba_FeeFs1x_Sector_getState(idxSectLog_u8))
    {
        case rba_FeeFs1x_Sector_SectorState_ERASED_e:   {stRet_en = FEE_RB_SECT_STATE_ERASED_E;} break;

        case rba_FeeFs1x_Sector_SectorState_USED_e:
        case rba_FeeFs1x_Sector_SectorState_FULL_e:
        case rba_FeeFs1x_Sector_SectorState_CLONE_e:    {stRet_en = FEE_RB_SECT_STATE_USED_E;} break;

        case rba_FeeFs1x_Sector_SectorState_UNDEF_e:
        default:                                        {stRet_en = FEE_RB_SECT_STATE_UNDEFD_E;} break;
    }

    return(stRet_en);
}
#  endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif
