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
#include "rba_FeeFs1x_Prv_Sector.h"
#include "rba_FeeFs1x_Prv_SectorTypes.h"

#include "MemIf_Types.h"
#include "Fee_Prv_Medium.h"
#include "Fee_Prv_Config.h"
#include "Fee_Prv_Lib.h"
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "Crc.h"
#include "rba_FeeFs1x_Prv.h"


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

RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_Sector_SectorListElement_tst  rba_FeeFs1x_Sector_sectorInfo_ast[RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE])
static rba_FeeFs1x_Sector_data_tst                              rba_FeeFs1x_Sector_data_st;
RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_Sector_markSector_tst         rba_FeeFs1x_Sector_markSector_st)
RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_Sector_erase_tst              rba_FeeFs1x_Sector_erase_st)

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"


#define FEE_START_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"


static const rba_FeeFs1x_Sector_const_tst rba_FeeFs1x_Sector_const_st =
{
        RBA_FEEFS1X_SECTOR_LOG_SIZE / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE,                                   /* number of logical pages per sector */
        RBA_FEEFS1X_SECTOR_M_PATTERN,                                                          /* marker pattern */
        (RBA_FEEFS1X_SECTOR_M_MIN_MEM_SIZE * ((uint8)(RBA_FEEFS1X_SECTOR_NUM_MARKER_E) - 1u)), /* offset to start address of the logical sector area*/
        (uint8)RBA_FEEFS1X_SECTOR_NUM_MARKER_E - 2u,                                           /* number of available sector header markers */
        RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE,                                                         /* number of available sectors */
        RBA_FEEFS1X_SECTOR_M_SIZE,                                                             /* size of sector header marker[bytes] */
        RBA_FEEFS1X_SECTOR_M_SIZE - RBA_FEEFS1X_SECTOR_M_CHECKSUM_SIZE,                        /* size of sector header marker data without CRC value[bytes] */
        ((RBA_FEEFS1X_SECTOR_M_MIN_MEM_SIZE / RBA_FEEFS1X_PRV_CFG_PHY_PAGE_LEN) - 1u) * RBA_FEEFS1X_PRV_CFG_PHY_PAGE_LEN,      /* address offset to the last physical page in the sector header marker */
        RBA_FEEFS1X_PRV_CFG_PHY_PAGE_LEN,                                                                      /* length of a physical page[bytes] */
        RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE,                                                                 /* length of a logical page[bytes] */
        RBA_FEEFS1X_SECTOR_M_MIN_MEM_SIZE,                                                     /* minimum required memory space for the sector header marker */
        RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES,                                                            /* number of read retries */
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT                                                 /* complement erase pattern */
};

#define FEE_STOP_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_detectStatesOfAllSectors(void);

LOCAL_INLINE void    rba_FeeFs1x_Sector_init_bubblesortSectors(rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfoSource_ast);
LOCAL_INLINE boolean rba_FeeFs1x_Sector_init_bubblesortSectors_isSwapRequired(
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst * currentSector_pst,
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst const * nextSector_pcst);

LOCAL_INLINE void rba_FeeFs1x_Sector_init_calculateMaxSectCngCnt(rba_FeeFs1x_Sector_unsortedSectorInfo_tst const * sectorInfo_cast);

LOCAL_INLINE void rba_FeeFs1x_Sector_init_cpySectorListToPersistentArray(
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst const * sectorInfoSource_cast);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_determineInfoOfSingleSector(uint8 xPhysSector_u8, rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfo_ast);

LOCAL_INLINE void rba_FeeFs1x_Sector_init_detInfoSingleMarker(
        rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst *detInfoSingleMarker_data_pst,
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfo_ast,
        uint32 sectorMarkerAddr_u32, uint8 markerPos_u8);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_readMarker(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_blankCheckMarker(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_evaluateMarker(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_blankCheckLastPage(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_detInfoSect(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst);

LOCAL_INLINE void rba_FeeFs1x_Sector_init_eraseStartFlagCheck(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst *detInfoSingleMarker_data_pst, uint32 sectorMarkerAddr_u32);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_eraseStartFlagCheckDo(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst *fsm_pst);

LOCAL_INLINE void rba_FeeFs1x_Sector_init_determineSectorState(rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfo_pst, rba_FeeFs1x_Sector_markerStates_ten markerState_en);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerErased(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerUsed(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerFull(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerEraseRequest(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerClone(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen);


LOCAL_INLINE void rba_FeeFs1x_Sector_init_determineSectorCngCnt(uint32 *sectorCngCnt_pu32,
        uint32 markerContent_u32,
        rba_FeeFs1x_Sector_CngCntStates_ten *SectorCngCntState_pen,
        rba_FeeFs1x_Sector_markerStates_ten markerState_en);
LOCAL_INLINE void rba_FeeFs1x_Sector_init_determineStartAddress(uint32 *startAddr_pu32,
        uint32 markerContent_u32,
        rba_FeeFs1x_Sector_markerStates_ten markerState_en);

LOCAL_INLINE void rba_FeeFs1x_Sector_init_restoreUsedOrErasedBeforeFullOrUsedToFull(rba_FeeFs1x_Sector_SectorListElement_tst *sectorList_ast);
LOCAL_INLINE void rba_FeeFs1x_Sector_init_restoreErasedAfterFullToUsed(rba_FeeFs1x_Sector_SectorListElement_tst *sectorList_ast);
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_checkInitializationOfAllSectors(const rba_FeeFs1x_Sector_SectorListElement_tst *sectorList_ast);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_markSectorDo_write(rba_FeeFs1x_Sector_markSector_tst *fsm_pst);
static uint32 rba_FeeFs1x_Sector_calcSectCngCnt(uint8 logSectIdx_u8, boolean preserveSectCngCnt_b);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_eraseDo_writeEraseStartFlag(rba_FeeFs1x_Sector_erase_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_eraseDo_erase(rba_FeeFs1x_Sector_erase_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_eraseDo_wrEraseMarker(rba_FeeFs1x_Sector_erase_tst *fsm_pst);

LOCAL_INLINE void rba_FeeFs1x_Sector_init_restoreUsedOrErasedBeforeFullToFull_markFull(uint8 idxSector_u8);
LOCAL_INLINE uint16 rba_FeeFs1x_Sector_calcCRC(uint16 markerPattern_u16, rba_FeeFs1x_Sector_markerStates_ten markerState_en, uint32 content_u32);
LOCAL_INLINE void rba_FeeFs1x_Sector_createMarkerSectorHeader(uint8 *writeBfr_pu8, const rba_FeeFs1x_Sector_marker_tst *marker_pcst);
LOCAL_INLINE void rba_FeeFs1x_Sector_init_createMarkerInfo(const uint8 *readBfr_pu8, rba_FeeFs1x_Sector_marker_tst *marker_pst);
LOCAL_INLINE rba_FeeFs1x_Sector_markerStates_ten rba_FeeFs1x_Sector_init_mapMarkerID2Enum(uint8 id_u8);
LOCAL_INLINE void rba_FeeFs1x_Sector_setMemComplementErasePattern(uint8 *bfr_pu8, uint8 startIdx_u8, uint8 endIdx_u8);

static void rba_FeeFs1x_Sector_markSector(rba_FeeFs1x_Sector_SectorState_ten SectorState_en,rba_FeeFs1x_Sector_markerStates_ten MarkerState_en, uint8 logSectIdx_u8, boolean isReerasing_b);
LOCAL_INLINE void rba_FeeFs1x_Sector_markErased(uint8 logSectIdx_u8, boolean isReerasing_b);
/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */


/**
 *********************************************************************
 *
 * \param   void
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e: initialization finished without any errors
 *          rba_FeeFs1x_ErrorInternal_e: initialization failed
 * \seealso
 * \usedresources
 *********************************************************************
 */

Std_ReturnType rba_FeeFs1x_Sector_init(void)
{
    Std_ReturnType retVal;

    rba_FeeFs1x_Sector_cancel();

    // inits unconditionally: rba_FeeFs1x_Sector_sectorInfo_ast and rba_FeeFs1x_Sector_data_st
    retVal = rba_FeeFs1x_Sector_init_detectStatesOfAllSectors();

    return retVal;
}

/**
 *********************************************************************
 *
 * \param   void
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */

void rba_FeeFs1x_Sector_cancel(void)
{
    rba_FeeFs1x_Sector_markSector_st.state_en   = rba_FeeFs1x_Sector_markSectorDoStm_idle_e;
    rba_FeeFs1x_Sector_erase_st.state_en = rba_FeeFs1x_Sector_eraseStm_idle_e;
}

/**
 *********************************************************************
 *
 * detects the state of all sectors and sort them in the way of
 * ascending sector change counters
 *
 * \param   void
 * \return  rba_FeeFs1x_JobOK_e: initialization was succesful, continue normal program flow
 *          rba_FeeFs1x_ErrorInternal_e: initialization failed -> stop
 *          rba_FeeFs1x_ErrorExternal_e: initialization failed due to FLS is not initialized -> stop
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_detectStatesOfAllSectors(void)
{
    uint8 idxSector_u8;
    Std_ReturnType retVal;
    rba_FeeFs1x_RetVal_ten retValDetInfo_en;

    rba_FeeFs1x_Sector_unsortedSectorInfo_tst rba_FeeFs1x_Sector_unsortedSectorInfo_ast[RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE];

    /* loop over all available sectors in physical sector order to determine their state */
    retValDetInfo_en = rba_FeeFs1x_JobOK_e;
    for(idxSector_u8 = 0u;
        ((idxSector_u8 < rba_FeeFs1x_Sector_const_st.xSectors_u8) && (rba_FeeFs1x_ErrorInternal_e != retValDetInfo_en) && (rba_FeeFs1x_ErrorExternal_e != retValDetInfo_en));
        idxSector_u8++)
    {
        /* determine the state of the actual sector */
        /* rba_FeeFs1x_Sector_unsortedSectorInfo_ast elements are initialized inside rba_FeeFs1x_Sector_init_determineInfoOfSingleSector function */
        retValDetInfo_en = rba_FeeFs1x_Sector_init_determineInfoOfSingleSector(idxSector_u8, &rba_FeeFs1x_Sector_unsortedSectorInfo_ast[idxSector_u8]);
    }

    if((rba_FeeFs1x_ErrorInternal_e != retValDetInfo_en) && (rba_FeeFs1x_ErrorExternal_e != retValDetInfo_en))
    {
        /* rearrange sectors according to their ascending sector change counters */
        rba_FeeFs1x_Sector_init_bubblesortSectors(rba_FeeFs1x_Sector_unsortedSectorInfo_ast);

        /* calculate the max. sector change counter */
        rba_FeeFs1x_Sector_init_calculateMaxSectCngCnt(rba_FeeFs1x_Sector_unsortedSectorInfo_ast);

        rba_FeeFs1x_Sector_init_cpySectorListToPersistentArray(rba_FeeFs1x_Sector_unsortedSectorInfo_ast);

        /* set ERASED or USED sector before a FULL sector, to FULL */
        rba_FeeFs1x_Sector_init_restoreUsedOrErasedBeforeFullOrUsedToFull(rba_FeeFs1x_Sector_sectorInfo_ast);

        /* set ERASED sector to USED, if ERASED sector is immediately after a FULL sector without a USED sector */
        rba_FeeFs1x_Sector_init_restoreErasedAfterFullToUsed(rba_FeeFs1x_Sector_sectorInfo_ast);


        /* final check of correct initialization of all sectors */
        retVal = rba_FeeFs1x_Sector_init_checkInitializationOfAllSectors(rba_FeeFs1x_Sector_sectorInfo_ast);
    }
    else
    {
        /* Report execution error */
        retVal = E_NOT_OK;
    }

    return retVal;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_init_bubblesortSectors
 * sorts the sector array by rising sector change counter.
 * Sectors without valid or invalid counter get sorted behind the defined ones.
 * UNDEFINED sectors shall be sorted before R2E sectors.
 *
 * The function directly works on the given array.
 *
 * \param   sectorInfoSource_ast: array of sectors to be sorted (INOUT)
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_bubblesortSectors(rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfoSource_ast)
{
    uint8 idxOuter_u8;
    uint8 idxInner_u8;

    // outer loop of the bubble sort
    for(idxOuter_u8 = (rba_FeeFs1x_Sector_const_st.xSectors_u8 - 1u) ; idxOuter_u8 >  0u ; idxOuter_u8--)
    {
        // inner loop for the bubble sort
        for(idxInner_u8 = 0u; idxInner_u8 < idxOuter_u8 ; idxInner_u8++)
        {
            // check whether a swap of current and right neighbor is required
            if(
                    rba_FeeFs1x_Sector_init_bubblesortSectors_isSwapRequired(
                            &(sectorInfoSource_ast[idxInner_u8]) ,
                            &(sectorInfoSource_ast[idxInner_u8 + 1u])
                    )
            )
            {
                // exchange the sectors
                rba_FeeFs1x_Sector_unsortedSectorInfo_tst tmpSectStorage_st;

                tmpSectStorage_st = sectorInfoSource_ast[idxInner_u8];

                sectorInfoSource_ast[idxInner_u8] = sectorInfoSource_ast[idxInner_u8 + 1u];

                sectorInfoSource_ast[idxInner_u8 + 1u] = tmpSectStorage_st;
            }
            else
            {
                // don't exchange the sectors
            }
        }
    }
}
/**
 *********************************************************************
 * rba_FeeFs1x_Sector_init_bubblesortSectors_isSwapRequired
 * determines whether the two sectors should be swapped during bubbleSort
 *
 * main criterion is the sector change counter. All sectors with valid sector
 * change counters are sorted by rising counters.
 *
 * Second priority for the sectors without valid change counters: is it an invalid
 * counter(not yet found one within this sector --> undefined sector) is to be sorted before explicitly
 * invalidated counters (R2E sector) to achieve the round robin usage.
 *
 * \param   currentSector_pst: reference to current sector to be compared
 * \param   nextSector_pcst:    reference to the right neighbor of the current sector
 * \return  Is the swap during bubble sort of current and right neighbor to be performed?
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_Sector_init_bubblesortSectors_isSwapRequired(
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst * currentSector_pst,
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst const * nextSector_pcst)
{
    boolean retVal_b;
    // don't consider invalid and invalidated sector change counters during sorting by sector change counter
    switch(currentSector_pst->stateSectorCngCnt_en)
    {
        case rba_FeeFs1x_Sector_CngCnt_Valid_e:
        {
            // the current sector has a valid change counter, check the right neighbor
            if(rba_FeeFs1x_Sector_CngCnt_Valid_e == nextSector_pcst->stateSectorCngCnt_en)
            {
                // both current and right neighbor have valid sector change counters
                // decide about order by looking at the change counter
                if(currentSector_pst->sectorCngCnt_u32 > nextSector_pcst->sectorCngCnt_u32)
                {
                    // current sector has a higher counter --> switch sectors
                    retVal_b = TRUE;
                }
                else
                {
                    // current's counter is smaller or equal to the right's one --> don't switch
                    retVal_b = FALSE;
                }
            }
            else
            {
                // current has a valid change counter, but right neibor not --> don't switch
                // unknown state of right, increase the error counter(gets marked as suspicious in the next swap check)
                retVal_b = FALSE;
            }
        }break;
        case rba_FeeFs1x_Sector_CngCnt_Invalid_e:
        {
            // current has no valid change counter
            if(rba_FeeFs1x_Sector_CngCnt_Valid_e == nextSector_pcst->stateSectorCngCnt_en)
            {
                // valid counters have to be before the invalid ones --> switch
                retVal_b = TRUE;
            }
            else
            {
                // keep the physical order between undefined sectors --> don't switch
                // invalidated (R2E markers) --> behind invalid (undefined sectors, physical order) --> don't switch
                // known sector states to the left --> don't switch
                retVal_b = FALSE;
            }

        }break;
        case rba_FeeFs1x_Sector_CngCnt_Invalidated_e:
        {
            if(     (rba_FeeFs1x_Sector_CngCnt_Valid_e   == nextSector_pcst->stateSectorCngCnt_en) ||
                    (rba_FeeFs1x_Sector_CngCnt_Invalid_e == nextSector_pcst->stateSectorCngCnt_en) )
            {
                // valid counters have to be before the invalidated ones --> switch
                // invalidated (R2E markers) --> behind invalid (undefined sectors, physical order) --> switch
                retVal_b = TRUE;
            }
            else
            {
                // keep the physical order between invalidated sectors --> don't switch
                // unknown state of the next sector, gets the error counter increased at the next's marker switch
                retVal_b = FALSE;
            }
        }break;
        default:
        {
            // unknown state, increase the error counter
            currentSector_pst->xMarkerSuspicious_u8++;

            // known sector states to the left --> switch the current sector further right
            retVal_b = TRUE;
        }break;
    }

    return retVal_b;
}

/**
 *********************************************************************
 * calculate the maximum sector change counter
 *
 * \param   sectorInfo_cast: sorted array of sector information after detection
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_calculateMaxSectCngCnt(rba_FeeFs1x_Sector_unsortedSectorInfo_tst const * sectorInfo_cast)
{
    uint32 maxSectCngCnt_u32;
    uint8 idxSector_u8;

    /* detect the highest sector change counter */
    maxSectCngCnt_u32 = 0uL;
    for(idxSector_u8 = 0u ; idxSector_u8 < rba_FeeFs1x_Sector_const_st.xSectors_u8 ; idxSector_u8++)
    {
        if(rba_FeeFs1x_Sector_CngCnt_Valid_e == sectorInfo_cast[idxSector_u8].stateSectorCngCnt_en)
        {
            if(maxSectCngCnt_u32 < sectorInfo_cast[idxSector_u8].sectorCngCnt_u32)
            {
                maxSectCngCnt_u32 = sectorInfo_cast[idxSector_u8].sectorCngCnt_u32;
            }
        }
    }
    /* store the maximum value of the sector change counter */
    rba_FeeFs1x_Sector_data_st.maxSectCngCnt_u32 = maxSectCngCnt_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_init_cpySectorListToPersistentArray
 * copies the already sorted "rba_FeeFs1x_Sector_unsortedSectorInfo_tst"
 * sector info array into the rba_FeeFs1x_Sector_sectorInfo_ast
 *
 * \param   sectorInfoSource_ast: sorted array of sector information after detection
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_cpySectorListToPersistentArray(
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst const * sectorInfoSource_cast)
{
    uint8 idxCurrSect_u8;

    for(idxCurrSect_u8 = 0u ; idxCurrSect_u8 < RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE ; idxCurrSect_u8++)
    {
        rba_FeeFs1x_Sector_sectorInfo_ast[idxCurrSect_u8].physSectIdx_u8 =
                sectorInfoSource_cast[idxCurrSect_u8].idxPhySect_u8;

        rba_FeeFs1x_Sector_sectorInfo_ast[idxCurrSect_u8].sectorState_en =
                sectorInfoSource_cast[idxCurrSect_u8].sectorState_en;

        if(rba_FeeFs1x_Sector_CngCnt_Valid_e == sectorInfoSource_cast[idxCurrSect_u8].stateSectorCngCnt_en)
        {
            rba_FeeFs1x_Sector_sectorInfo_ast[idxCurrSect_u8].sectorCngCnt_u32 =
                    sectorInfoSource_cast[idxCurrSect_u8].sectorCngCnt_u32;
        }


        rba_FeeFs1x_Sector_sectorInfo_ast[idxCurrSect_u8].sectorMarkerWriteIdx_u8 =
                sectorInfoSource_cast[idxCurrSect_u8].sectorMarkerWriteIdx_u8;

        rba_FeeFs1x_Sector_sectorInfo_ast[idxCurrSect_u8].startMarkerContent_u32 =
                sectorInfoSource_cast[idxCurrSect_u8].startMarkerContent_u32;
    }
}




/**
 *********************************************************************
 *
 * determines state, change counter and start address of a single sector
 *
 * \param   xPhysSector_u8: sector, which shall be analyzed
 * \param   *sectorInfo_ast: pointer to buffer, where determined information of a single sector is stored
 * \return  rba_FeeFs1x_JobOK_e: initialization was succesful, continue normal program flow
 *          rba_FeeFs1x_ErrorInternal_e: initialization failed -> stop
 *          rba_FeeFs1x_ErrorExternal_e: initialization failed due to FLS is not initialized -> stop
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_determineInfoOfSingleSector(uint8 xPhysSector_u8,
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfo_ast)
{
    uint8 i;
    uint32 sectorMarkerAddr_u32;
    uint32 sectorHeaderAddr_u32;
    rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_st;
    rba_FeeFs1x_RetVal_ten retVal_en = rba_FeeFs1x_JobOK_e;

    /* initialize sector data */
    sectorInfo_ast->idxPhySect_u8 = xPhysSector_u8;

    sectorInfo_ast->sectorState_en = rba_FeeFs1x_Sector_SectorState_UNDEF_e;
    sectorInfo_ast->xMarkerSuspicious_u8 = 0;
    sectorInfo_ast->startMarkerContent_u32 = RBA_FEEFS1X_MAXUINT32;
    sectorInfo_ast->stateSectorCngCnt_en = rba_FeeFs1x_Sector_CngCnt_Invalid_e;
    sectorInfo_ast->sectorMarkerWriteIdx_u8 = 0;
    sectorInfo_ast->sectorCngCnt_u32 = 0;

    /* set the address to the first sector header marker */
    sectorHeaderAddr_u32 = Fee_Prv_ConfigGetPhysStartAddress(xPhysSector_u8, rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast);
    sectorMarkerAddr_u32 = sectorHeaderAddr_u32;

    /* loop over all sector header markers */
    for(i=0;
       (i<rba_FeeFs1x_Sector_const_st.xSectorMarkers_u8) && (rba_FeeFs1x_ErrorInternal_e != retVal_en) && (rba_FeeFs1x_ErrorExternal_e != retVal_en);
       i++)
    {
        rba_FeeFs1x_Sector_init_detInfoSingleMarker(&rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_st, sectorInfo_ast, sectorMarkerAddr_u32, i);
        do
        {
            retVal_en = rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo(&rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_st);
        } while(rba_FeeFs1x_Pending_e == retVal_en);
        /* Evaluation of while loop: The function is called only during init phase where Fee operates in synchronous mode.
         * The while loop is required to go through the different states of marker evaluation. The state transitions
         * are checked and would not lead to infinite loops, as long as Fee_Medium has proper exit criteria
         * (Refer handling of while loop in Fee_Medium) */

        /* increase the read address to the next sector header marker */
        sectorMarkerAddr_u32 += rba_FeeFs1x_Sector_const_st.markerMemLen_u8;
    }

    /* Check if we already have a execution error */
   if((rba_FeeFs1x_ErrorInternal_e != retVal_en) && (rba_FeeFs1x_ErrorExternal_e != retVal_en))
    {
        /* Evaluate presence of Erase Start Flag */
        rba_FeeFs1x_Sector_init_eraseStartFlagCheck(&rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_st, sectorMarkerAddr_u32);
        retVal_en = rba_FeeFs1x_Sector_init_eraseStartFlagCheckDo(&rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_st);

        if(RBA_FEEFS1X_MAXUINT32 != sectorInfo_ast->startMarkerContent_u32)
        {
            /* In the START marker, only the 3 low-bytes of the address are stored.
               The start address is then the physical start address of the sector + relative address
               In case of sector-size > 2^24 = 16,7MByte, this mechanisms will not work anymore. */
            sectorInfo_ast->startMarkerContent_u32 = sectorInfo_ast->startMarkerContent_u32 | (sectorHeaderAddr_u32 & 0xFF000000uL);
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 *
 * init function for state machine rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo
 *
 * \param *detInfoSingleMarker_data_pst: pointer to struct containing the state machine data
 * \param *sectorInfo_ast: pointer to buffer, where determined information of a single sector is stored
 * \param sectorMarkerAddr_u32: address of the sector header marker to be analyzed
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_detInfoSingleMarker(
        rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst *detInfoSingleMarker_data_pst,
        rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfo_ast,
        uint32 sectorMarkerAddr_u32, uint8 markerPos_u8)
{
    detInfoSingleMarker_data_pst->sectorInfo_ast = sectorInfo_ast;
    detInfoSingleMarker_data_pst->readRetries_u8 = rba_FeeFs1x_Sector_const_st.readWriteRetries_u8;
    detInfoSingleMarker_data_pst->sectorMarkerAddr_u32 = sectorMarkerAddr_u32;
    detInfoSingleMarker_data_pst->markerPos_u8 = markerPos_u8;

    detInfoSingleMarker_data_pst->state_en = rba_FeeFs1x_Sector_init_stm_readMarker_e;
    detInfoSingleMarker_data_pst->entry_b = TRUE;
}

/**
 *********************************************************************
 *
 * determines state, change counter and start address of a single sector header marker.
 * If the sector header marker contains any data (first page not blank or CRC check passed or preamble check passed)
 * then the write position index is set to the next sector header marker position.
 *
 * If the sector header marker contains valid data (first page not blank and CRC check passed and preamble check passed)
 * then the write position index is set to the next sector header marker position
 * and the data is evaluated to determine the state of the sector, sector change counter or start address
 *
 * \param *fsm_pst: pointer to struct containing the state machine data
 * \return rba_FeeFs1x_RetVal_ten
 * \retval rba_FeeFs1x_Pending_e: pending
 * \retval rba_FeeFs1x_JobFailed_e: sector header marker info is corrupted and not evaluated
 * \retval rba_FeeFs1x_JobOK_e: sector header marker info is evaluated
 * \retval rba_FeeFs1x_ErrorInternal_e: an internal error occured
 * \retval rba_FeeFs1x_ErrorExternal_e: reading out the sector header marker or blank check failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst *fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(fsm_pst->state_en)
    {
        case rba_FeeFs1x_Sector_init_stm_readMarker_e:
        {
            retVal_en = rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_readMarker(fsm_pst);
            break;
        }
        case rba_FeeFs1x_Sector_init_stm_blankCheckMarker_e:
        {
            retVal_en = rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_blankCheckMarker(fsm_pst);
            break;
        }
        case rba_FeeFs1x_Sector_init_stm_evaluateMarker_e:
        {
            retVal_en = rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_evaluateMarker(fsm_pst);
            break;
        }
        case rba_FeeFs1x_Sector_init_stm_blankCheckLastPage_e:
        {
            retVal_en = rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_blankCheckLastPage(fsm_pst);
            break;
        }
        case rba_FeeFs1x_Sector_init_stm_detInfoSect_e:
        {
            retVal_en = rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_detInfoSect(fsm_pst);
            break;
        }
        default:
        {
            // unexpected state
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            fsm_pst->state_en = rba_FeeFs1x_Sector_init_stm_idle_e;
            fsm_pst->entry_b = TRUE;
            break;
        }
    }

    return(retVal_en);
}

/**
 *********************************************************************
 *
 * state function reading out the sector header marker and creating infos for further processing
 *
 * \param *fsm_pst: pointer to struct containing the state machine data
 * \return rba_FeeFs1x_RetVal_ten
 * \retval rba_FeeFs1x_Pending_e: pending
 * \retval rba_FeeFs1x_ErrorInternal_e: an internal error occured
 * \retval rba_FeeFs1x_ErrorExternal_e: reading out the sector header marker failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_readMarker(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    MemIf_JobResultType retValMediumRead_en;
    uint8 readBfr_au8[RBA_FEEFS1X_SECTOR_M_SIZE] = {0};

    if(FEE_PRV_LIBENTRY)
    {
        /* read and buffer the sector header marker */
        Fee_Prv_MediumRead(
                Fee_Rb_DeviceName,
                fsm_pst->sectorMarkerAddr_u32,
                0u,
                (void*)readBfr_au8,
                rba_FeeFs1x_Sector_const_st.markerSize_u8);
    }
    retValMediumRead_en = Fee_Prv_MediumReadDo(Fee_Rb_DeviceName);

    switch (retValMediumRead_en)
    {
        case MEMIF_JOB_OK:
        {
            /* extract header info from readBfr */
            rba_FeeFs1x_Sector_init_createMarkerInfo(readBfr_au8, &(fsm_pst->marker_st));
            /* reading of the header was successful, so proceed with blank check of the sector header marker */
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_blankCheckMarker_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            if(fsm_pst->readRetries_u8 > 0)
            {
                /* read retry */
                FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_readMarker_e);
                fsm_pst->readRetries_u8--;
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                /* a read error occured, so eventually there is written data
                 * -> increase the write index */
                fsm_pst->sectorInfo_ast->sectorMarkerWriteIdx_u8 ++;
                FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_idle_e);
                retVal_en = rba_FeeFs1x_ErrorExternal_e;
            }
            break;
        }
        // MEMIF_JOB_PENDING will be never returned from Fee_Prv_MediumReadDo during init here because:
        // 1) Fee is operating in synchronous mode
        //    --> Jobs will be finished before return - Pending return is not possible
        // 2) Fee effort limitation is still disabled
        //    --> Jobs will be finished in one step to initialize as fast as possible
        case MEMIF_JOB_PENDING:
        default:
        {
            /* something unexpected occured, switch to next marker */
            fsm_pst->sectorInfo_ast->sectorMarkerWriteIdx_u8 ++;
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_idle_e);
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }
    return(retVal_en);
}

/**
 *********************************************************************
 *
 * performing a blank check on the complete header marker
 *
 * \param *fsm_pst: pointer to struct containing the state machine data
 * \return rba_FeeFs1x_RetVal_ten
 * \retval rba_FeeFs1x_Pending_e: pending
 * \retval rba_FeeFs1x_ErrorInternal_e: doing the blank check failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_blankCheckMarker(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst)
{
    MemIf_JobResultType retValBlankCheck_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* do the blank check on the complete header marker */
        Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName, fsm_pst->sectorMarkerAddr_u32, 0u, rba_FeeFs1x_Sector_const_st.markerMemLen_u8);
    }
    retValBlankCheck_en = Fee_Prv_MediumBlankCheckDo(Fee_Rb_DeviceName);

    switch(retValBlankCheck_en)
    {
        case MEMIF_JOB_OK:
        {
            /* complete header marker is blank */
            fsm_pst->isMarkerBlank_b = TRUE;
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_evaluateMarker_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            /* complete header marker is not blank */
            fsm_pst->isMarkerBlank_b = FALSE;
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_evaluateMarker_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        // MEMIF_JOB_PENDING will be never returned from Fee_Prv_MediumBlankCheckDo during init here because:
        // 1) Fee is operating in synchronous mode
        //    --> Jobs will be finished before return - Pending return is not possible
        case MEMIF_JOB_PENDING:
        default:
        {
            /* something unexpected occured, check the next marker */
            fsm_pst->sectorInfo_ast->sectorMarkerWriteIdx_u8 ++;
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_idle_e);
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 *
 * state function for preamble check and CRC check and to decide urther processing
 *
 * \param *fsm_pst: pointer to struct containing the state machine data
 * \return rba_FeeFs1x_RetVal_ten
 * \retval rba_FeeFs1x_Pending_e: pending
 * \retval rba_FeeFs1x_JobFailed_e: preamble or CRC check failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_evaluateMarker(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint16 xCalc_crc_u16;

    /* check CRC */
    xCalc_crc_u16 = rba_FeeFs1x_Sector_calcCRC(fsm_pst->marker_st.xPattern_u16,
            fsm_pst->marker_st.sectorMarkerState_en, fsm_pst->marker_st.xContent_u32);

    if(!fsm_pst->isMarkerBlank_b)
    {
        /* marker is not empty -> extract the write index on the current marker position */
        fsm_pst->sectorInfo_ast->sectorMarkerWriteIdx_u8 = fsm_pst->markerPos_u8 + 1u;
    }

    if((rba_FeeFs1x_Sector_const_st.markerPattern_u16 == fsm_pst->marker_st.xPattern_u16) &&
            (xCalc_crc_u16 == fsm_pst->marker_st.xChecksum_u16))
    {
        /* header content and CRC calculation is valid */
        if(rba_FeeFs1x_Prv_isBlankChkRequired())
        {
            /* blank check is meaningful -> do the blank check of the last marker page */
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_blankCheckLastPage_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            /* blank check is not meaningful -> determine sector info */
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_detInfoSect_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
    }
    else
    {
        /* preamble or CRC check failed -> switch to next sector header marker */
        FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_idle_e);
        retVal_en = rba_FeeFs1x_JobFailed_e;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 *
 * state function doing a blank check on the last sector header marker page
 *
 * \param *fsm_pst: pointer to struct containing the state machine data
 * \return rba_FeeFs1x_RetVal_ten
 * \retval rba_FeeFs1x_Pending_e: pending
 * \retval rba_FeeFs1x_JobFailed_e: last sector header marker page is blank
 * \retval rba_FeeFs1x_ErrorInternal_e: an internal error occured
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_blankCheckLastPage(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst)
{
    MemIf_JobResultType retValBlankCheck_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* do the blank check on the last physical page of the header */
        Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName,
                (fsm_pst->sectorMarkerAddr_u32 + rba_FeeFs1x_Sector_const_st.addrOffsetToLastPageInMarker_u8) ,
                0u,
                rba_FeeFs1x_Sector_const_st.physPageLen_u8);
    }
    retValBlankCheck_en = Fee_Prv_MediumBlankCheckDo(Fee_Rb_DeviceName);

    switch (retValBlankCheck_en)
    {
        case MEMIF_JOB_OK:
        {
            /* last sector header marker page is blank -> check the next header marker */
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_idle_e);
            retVal_en = rba_FeeFs1x_JobFailed_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            /* page is not blank, preamble and CRC check passed -> determine the sector info */
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_detInfoSect_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        // MEMIF_JOB_PENDING will be never returned from Fee_Prv_MediumBlankCheckDo during init here because:
        // 1) Fee is operating in synchronous mode
        //    --> Jobs will be finished before return - Pending return is not possible
        case MEMIF_JOB_PENDING:
        default:
        {
            /* something unexpected occured, switch to next marker */
            fsm_pst->sectorInfo_ast->sectorMarkerWriteIdx_u8 ++;
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_idle_e);
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }
    return (retVal_en);
}

/**
 *********************************************************************
 *
 * state function sector header marker is valid, so determine the new sector state,
 * calculate the sector change counter or the start address
 *
 * \param *fsm_pst: pointer to struct containing the state machine data
 * \return rba_FeeFs1x_RetVal_ten
 * \retval rba_FeeFs1x_JobOK_e: blank check on the last sector header marker page is meaningless or last page is not blank,
 *                              preamble is OK, CRC check is passed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_detInfoSingleMarkerDo_detInfoSect(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst)
{
    /* determine sector state by evaluation of the header marker */
    rba_FeeFs1x_Sector_init_determineSectorState(fsm_pst->sectorInfo_ast,
            fsm_pst->marker_st.sectorMarkerState_en);

    /* determine the sector change counter */
    rba_FeeFs1x_Sector_init_determineSectorCngCnt(&(fsm_pst->sectorInfo_ast->sectorCngCnt_u32),
            fsm_pst->marker_st.xContent_u32,
            &(fsm_pst->sectorInfo_ast->stateSectorCngCnt_en),
            fsm_pst->marker_st.sectorMarkerState_en);

    /* determine the start address */
    rba_FeeFs1x_Sector_init_determineStartAddress(&(fsm_pst->sectorInfo_ast->startMarkerContent_u32),
            fsm_pst->marker_st.xContent_u32,
            fsm_pst->marker_st.sectorMarkerState_en);

    FEE_PRV_LIBSC(rba_FeeFs1x_Sector_init_stm_idle_e);
    return (rba_FeeFs1x_JobOK_e);
}

/**
 *********************************************************************
 *
 * evaluates the header marker and determines the sector change counter
 *
 * \param   sectorCngCnt_pu32: pointer to the sectorCngCnt
 * \param   markerContent_u32: content of the marker
 * \param   SectorCngCntState_pen: pointer to validity info of the sectorCngCnt
 * \param   markerState_en: state of the marker
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_determineSectorCngCnt(uint32 *sectorCngCnt_pu32,
        uint32 markerContent_u32,
        rba_FeeFs1x_Sector_CngCntStates_ten *SectorCngCntState_pen,
        rba_FeeFs1x_Sector_markerStates_ten markerState_en)
{

    switch(markerState_en)
    {
        case rba_FeeFs1x_Sector_Marker_ERASED_e:
        case rba_FeeFs1x_Sector_Marker_USED_e:
        case rba_FeeFs1x_Sector_Marker_FULL_e:
        {
            /* only ERASED, USED, FULL marker contain the sector change counter as marker content */
            if(rba_FeeFs1x_Sector_CngCnt_Invalid_e == *SectorCngCntState_pen)
            {
                /* first sector change counter found */
                *sectorCngCnt_pu32 = markerContent_u32;
                *SectorCngCntState_pen = rba_FeeFs1x_Sector_CngCnt_Valid_e;
            }
            else
            {
                /* in a previous marker a sector change counter is already found, so compare the two and take the highest one */
                if(*sectorCngCnt_pu32 < markerContent_u32)
                {
                    *sectorCngCnt_pu32 = markerContent_u32;
                }
            }
        }break;

        case rba_FeeFs1x_Sector_Marker_ERASE_REQUEST_e:

        {
            /* Request to erase is marking the sector to be sorted to the end
             * and not to consider the sector change counter.
             * Ensure that no further found sector change counter gets considered*/

            *SectorCngCntState_pen = rba_FeeFs1x_Sector_CngCnt_Invalidated_e;
            *sectorCngCnt_pu32 = 0;
        }break;

        case rba_FeeFs1x_Sector_Marker_CLONE_e:
        {
            /* if clone marker is detected, state is reset to UNDEF => reset also the sector change counter info */
            *SectorCngCntState_pen = rba_FeeFs1x_Sector_CngCnt_Invalid_e;
            *sectorCngCnt_pu32 = 0;
        }break;

        default:
        {
            /* Marker doesn't have an influence onto the sector change counter */
        }break;
    }

}

/**
 *********************************************************************
 *
 * evaluates the header marker and determines the start address of the sector
 *
 * \param   startAddr_pu32: pointer to the startAddress
 * \param   markerContent_u32: content of the marker
 * \param   markerState_en: state of the marker
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_determineStartAddress(uint32 *startAddr_pu32,
        uint32 markerContent_u32,
        rba_FeeFs1x_Sector_markerStates_ten markerState_en)
{
    /* only the START marker contains the start address as marker content */
    if(rba_FeeFs1x_Sector_Marker_START_e == markerState_en)
    {
        /* start marker found, so store the start address */
        *startAddr_pu32 = markerContent_u32;
    }

    /* if clone marker is detected, state is reset to UNDEF => reset also the start address info */
    if(rba_FeeFs1x_Sector_Marker_CLONE_e == markerState_en)
    {
        *startAddr_pu32 = RBA_FEEFS1X_MAXUINT32;
    }
}

/**
 *********************************************************************
 *
 * evaluates the sector header marker state and out of that determines the actual sector state
 * store the sector state to the passed pointer to buffer
 *
 * \param   *sectorInfo_pst: pointer to struct containing intermediate determined sector state
 * \param   markerState_en: state of the next read marker
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_determineSectorState(rba_FeeFs1x_Sector_unsortedSectorInfo_tst *sectorInfo_pst,
        rba_FeeFs1x_Sector_markerStates_ten markerState_en)
{
    Std_ReturnType retVal;

    switch (markerState_en)
    {
        /* first marker found, set initial state */
        case rba_FeeFs1x_Sector_Marker_ERASED_e:
        {
            retVal = rba_FeeFs1x_Sector_init_determineSectorState_MarkerErased(&(sectorInfo_pst->sectorState_en));
            break;
        }
        case rba_FeeFs1x_Sector_Marker_USED_e:
        {
            retVal = rba_FeeFs1x_Sector_init_determineSectorState_MarkerUsed(&(sectorInfo_pst->sectorState_en));
            break;
        }
        case rba_FeeFs1x_Sector_Marker_FULL_e:
        {
            retVal = rba_FeeFs1x_Sector_init_determineSectorState_MarkerFull(&(sectorInfo_pst->sectorState_en));
            break;
        }
        case rba_FeeFs1x_Sector_Marker_ERASE_REQUEST_e:
        {
            retVal = rba_FeeFs1x_Sector_init_determineSectorState_MarkerEraseRequest(&(sectorInfo_pst->sectorState_en));
            break;
        }
        case rba_FeeFs1x_Sector_Marker_CLONE_e:
        {
            retVal = rba_FeeFs1x_Sector_init_determineSectorState_MarkerClone(&(sectorInfo_pst->sectorState_en));
            break;
        }
        case rba_FeeFs1x_Sector_Marker_START_e:
        {
            if((sectorInfo_pst->sectorState_en) == rba_FeeFs1x_Sector_SectorState_USED_e)
            {
                /* START marker after USED marker -> OK */
                retVal = E_OK;
            }
            else
            {
                retVal = E_NOT_OK;
            }
            break;
        }
        default:
        {
            /* undefined marker */
            /* -> set debug info and keep state UNDEF */
            retVal = E_NOT_OK;
            break;
        }
    }

    if(retVal != E_OK)
    {
        /* last evaluated marker is suspicious */
        sectorInfo_pst->xMarkerSuspicious_u8 ++;
    }
}

/**
 *********************************************************************
 *
 * determines the next state of the sector
 *
 * \param   *sectorState_pen: pointer to actual determined state of the sector
 * \return  E_OK: *sectorState_pen is fitting to previous determined sector state
 *          E_NOT_OK: *sectorState_pen is not fitting to previous determined sector state
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerErased(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen)
{
    Std_ReturnType retVal;

    switch(*sectorState_pen)
    {
        case rba_FeeFs1x_Sector_SectorState_UNDEF_e:
            /* ERASED marker -> OK */
            *sectorState_pen = rba_FeeFs1x_Sector_SectorState_ERASED_e;
            retVal = E_OK;
            break;
        default:
            /* ERASED marker after ERASED marker -> should not occur */
            /* ERASED marker after USED marker -> error */
            /* ERASED marker after FULL marker -> error */
            /* ERASED marker after ERASE_REQUEST marker -> error */
            /* -> set debug info and keep state */
            retVal = E_NOT_OK;
            break;
    }

    return retVal;
}

/**
 *********************************************************************
 *
 * determines the next state of the sector
 *
 * \param   *sectorState_pen: pointer to actual determined state of the sector
 * \return  E_OK: *sectorState_pen is fitting to previous determined sector state
 *          E_NOT_OK: *sectorState_pen is not fitting to previous determined sector state
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerUsed(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen)
{
    Std_ReturnType retVal;

    switch(*sectorState_pen)
    {
        case rba_FeeFs1x_Sector_SectorState_UNDEF_e:
            /* ERASED marker expected -> ERASED marker is missing
               or previous marker was a CLONE marker. Therefore state is UNDEF -> USED marker is expected */
        case rba_FeeFs1x_Sector_SectorState_ERASED_e:
            /* USED marker after ERASED marker -> OK */
            /* -> set state to USED */
            *sectorState_pen = rba_FeeFs1x_Sector_SectorState_USED_e;
            retVal = E_OK;
            break;
        case rba_FeeFs1x_Sector_SectorState_USED_e:
        case rba_FeeFs1x_Sector_SectorState_FULL_e:
        case rba_FeeFs1x_Sector_SectorState_R2E_e:
        default:
            /* USED marker after USED marker -> should not occur */
            /* USED marker after FULL marker -> error */
            /* USED marker after ERASE_REQUEST marker -> error */
            /* -> set debug info and keep state */
            retVal = E_NOT_OK;
            break;
    }

    return retVal;
}

/**
 *********************************************************************
 *
 * determines the next state of the sector
 *
 * \param   *sectorState_pen: pointer to actual determined state of the sector
 * \return  E_OK: *sectorState_pen is fitting to previous determined sector state
 *          E_NOT_OK: *sectorState_pen is not fitting to previous determined sector state
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerFull(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen)
{
    Std_ReturnType retVal;

    switch(*sectorState_pen)
    {
        case rba_FeeFs1x_Sector_SectorState_UNDEF_e:
            /* ERASED marker expected -> ERASED and USED marker are missing */
        case rba_FeeFs1x_Sector_SectorState_ERASED_e:
            /* FULL marker after ERASED marker -> USED marker is missing */
            /* -> set debug info and set state to FULL */
            *sectorState_pen = rba_FeeFs1x_Sector_SectorState_FULL_e;
            retVal = E_NOT_OK;
            break;
        case rba_FeeFs1x_Sector_SectorState_USED_e:
            /* FULL marker after USED marker -> OK */
        case rba_FeeFs1x_Sector_SectorState_FULL_e:
            /* FULL marker after FULL marker -> OK */
            /* -> set state to FULL */
            *sectorState_pen = rba_FeeFs1x_Sector_SectorState_FULL_e;
            retVal = E_OK;
            break;
        case rba_FeeFs1x_Sector_SectorState_R2E_e:
        default:
            /* FULL marker after ERASE_REQUEST marker -> error
               -> set debug info and keep state */
            retVal = E_NOT_OK;
            break;
    }

    return retVal;
}

/**
 *********************************************************************
 *
 * determines the next state of the sector
 *
 * \param   *sectorState_pen: pointer to actual determined state of the sector
 * \return  E_OK: *sectorState_pen is fitting to previous determined sector state
 *          E_NOT_OK: *sectorState_pen is not fitting to previous determined sector state
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerEraseRequest(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen)
{
    Std_ReturnType retVal;

    switch(*sectorState_pen)
    {
        case rba_FeeFs1x_Sector_SectorState_UNDEF_e:
            /* ERASED marker expected -> ERASED marker is missing */
        case rba_FeeFs1x_Sector_SectorState_ERASED_e:
            /* ERASE_REQUEST marker after ERASED marker -> FULL marker is missing */
        case rba_FeeFs1x_Sector_SectorState_USED_e:
            /* ERASE_REQUEST marker after USED marker -> FULL marker is missing */
            /* -> set debug info and state to ERASE_REQUEST */
            *sectorState_pen = rba_FeeFs1x_Sector_SectorState_R2E_e;
            retVal = E_NOT_OK;
            break;
        case rba_FeeFs1x_Sector_SectorState_FULL_e:
            /* ERASE_REQUEST marker after FULL marker -> OK */
            /* -> set state to ERASE_REQUEST */
            *sectorState_pen = rba_FeeFs1x_Sector_SectorState_R2E_e;
            retVal = E_OK;
            break;
        case rba_FeeFs1x_Sector_SectorState_R2E_e:
        default:
            /* ERASE_REQUEST marker after ERASE_REQUEST marker -> should not occur */
            /* -> set debug info and keep state ERASE_REQUEST */
            retVal = E_NOT_OK;
            break;
    }

    return retVal;
}

/**
 *********************************************************************
 *
 * determines the next state of the sector
 *
 * \param   *sectorState_pen: pointer to actual determined state of the sector
 * \return  E_OK: *sectorState_pen is fitting to previous determined sector state
 *          E_NOT_OK: *sectorState_pen is not fitting to previous determined sector state
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_determineSectorState_MarkerClone(rba_FeeFs1x_Sector_SectorState_ten *sectorState_pen)
{
    Std_ReturnType retVal;

    switch(*sectorState_pen)
    {
        case rba_FeeFs1x_Sector_SectorState_ERASED_e:
            /* CLONE marker after ERASED marker -> OK
               set state to UNDEF */
            *sectorState_pen = rba_FeeFs1x_Sector_SectorState_UNDEF_e;
            retVal = E_OK;
            break;
        default:
            /* UNDEF: ERASED marker expected -> ERASED marker is missing */
            /* CLONE marker after USED marker -> should not occur */
            /* CLONE marker after FULL marker -> should not occur */
            /* CLONE marker after ERASE_REQUEST marker -> should not occur */
            /* -> set debug info and keep state */
            retVal = E_NOT_OK;
            break;
    }

    return retVal;
}

/**
 *********************************************************************
 *
 * init function for state machine rba_FeeFs1x_Sector_init_eraseStartFlagCheck
 *
 * \param *detInfoSingleMarker_data_pst : pointer to struct containing the state machine data
 * \param sectorMarkerAddr_u32          : address of the sector header marker to be analyzed
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_eraseStartFlagCheck(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst *detInfoSingleMarker_data_pst, uint32 sectorMarkerAddr_u32)
{
    /* All other information is already copied to the internal working variable, only set the flag to trigger first entry in the do function
     * and update the address where the blank check must be performed. */
    detInfoSingleMarker_data_pst->sectorMarkerAddr_u32 = sectorMarkerAddr_u32;
    detInfoSingleMarker_data_pst->entry_b = TRUE;
}

/**
 *********************************************************************
 * Check if Erase Start Flag is programmed
 *
 * \param *fsm_pst                      : pointer to struct containing the state machine data
 * \return rba_FeeFs1x_RetVal_ten
 * \retval rba_FeeFs1x_Pending_e        : check for erase start flag ongoing
 * \retval rba_FeeFs1x_JobOK_e          : check for erase start flag completed successfully
 * \retval rba_FeeFs1x_ErrorInternal_e  : error during the blank check failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_init_eraseStartFlagCheckDo(rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst * fsm_pst)
{
    MemIf_JobResultType retValBlankCheck_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* do the blank check on the complete header marker */
        Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName, fsm_pst->sectorMarkerAddr_u32, 0u, rba_FeeFs1x_Sector_const_st.markerMemLen_u8);
    }
    retValBlankCheck_en = Fee_Prv_MediumBlankCheckDo(Fee_Rb_DeviceName);

    switch(retValBlankCheck_en)
    {
        case MEMIF_JOB_OK:
        {
            /* complete header marker is blank */
            retVal_en = rba_FeeFs1x_JobOK_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            /* complete header marker is not blank, ignore the sector and mark it as UNDEF */
            /* some markers might be valid, since the erase flag is set, complete sector must be ignored and nothing must be trusted. */
            fsm_pst->sectorInfo_ast->sectorState_en = rba_FeeFs1x_Sector_SectorState_UNDEF_e;
            fsm_pst->sectorInfo_ast->xMarkerSuspicious_u8 = 0;
            fsm_pst->sectorInfo_ast->startMarkerContent_u32 = RBA_FEEFS1X_MAXUINT32;
            fsm_pst->sectorInfo_ast->stateSectorCngCnt_en = rba_FeeFs1x_Sector_CngCnt_Invalid_e;
            fsm_pst->sectorInfo_ast->sectorMarkerWriteIdx_u8 = 0;
            fsm_pst->sectorInfo_ast->sectorCngCnt_u32 = 0;

            retVal_en = rba_FeeFs1x_JobOK_e;
            break;
        }

        /* The function Fee_Prv_MediumBlankCheckDo() should not return _pending, as this is called only during
         * init phase, where the Fee_Medium operates in synchronous mode and calls Fls_MainFunction till the requested
         * operation is completed. */
        case MEMIF_JOB_PENDING:
        default:
        {
            /* something unexpected occured, ignore the flag and continue with next steps */
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_init_restoreUsedOrErasedBeforeFullOrUsedToFull
 * This function heals the sector marker loss.
 * When multiple markers are lost and an ERASED or USED sector is found before a FULL or USED sector,
 * it will then write a FULL Marker in such sectors and also update the RAM state.
 *
 * \param   *sectorList_ast: pointer to an array containing the sector info
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_restoreUsedOrErasedBeforeFullOrUsedToFull(rba_FeeFs1x_Sector_SectorListElement_tst *sectorList_ast)
{
    sint32 idxSector_s32;
    boolean isFirstFullFound_b;

    isFirstFullFound_b = FALSE;

    /* loop backwards over all sectors */
    for(idxSector_s32 = rba_FeeFs1x_Sector_const_st.xSectors_u8-1; idxSector_s32 >= 0; idxSector_s32--)
    {
        if (isFirstFullFound_b)
        {
            if((rba_FeeFs1x_Sector_SectorState_USED_e == sectorList_ast[idxSector_s32].sectorState_en) ||
               (rba_FeeFs1x_Sector_SectorState_ERASED_e == sectorList_ast[idxSector_s32].sectorState_en))
            {

                rba_FeeFs1x_Sector_init_restoreUsedOrErasedBeforeFullToFull_markFull((uint8)idxSector_s32);
                /* In case the writing of Full Marker fails, the state is not updated by the rba_FeeFs1x_Sector_markSectorDo(),
                 * so the state is updated here explicity, so that the software cotinues to run with status update in RAM */
                sectorList_ast[idxSector_s32].sectorState_en = rba_FeeFs1x_Sector_SectorState_FULL_e;
            }
        }
        else
        {
            if( (rba_FeeFs1x_Sector_SectorState_FULL_e == sectorList_ast[idxSector_s32].sectorState_en) ||
                (rba_FeeFs1x_Sector_SectorState_USED_e == sectorList_ast[idxSector_s32].sectorState_en) )
            {
                isFirstFullFound_b = TRUE;
            }
        }
    }
}

/**
 *********************************************************************
 * performing write of the full marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   idxSector_u8: sector to be marked as full
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_restoreUsedOrErasedBeforeFullToFull_markFull(uint8 idxSector_u8)
{
    rba_FeeFs1x_RetVal_ten retValMarkSector_en;

    /* write the Full marker to RAM and DFlash */
    rba_FeeFs1x_Sector_markFull(idxSector_u8);
    do
    {
        retValMarkSector_en = rba_FeeFs1x_Sector_markSectorDo();
    }while(rba_FeeFs1x_Pending_e == retValMarkSector_en);
    /* Evaluation of while loop: The function is called only during init phase where Fee_Medium operates in synchronous mode.
     * The while loop is required for retry handling of marker write. As long as Fee_Medium has proper exit criteria,
     * the while loop here is safe (Refer handling of while loop in Fee_Medium) */
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_init_restoreErasedAfterFullToUsed
 * The function checks if there is a ERASED sector after a FULL sector,
 * if found it will write a USED marker in this sector and change the state of the sector to USED.
 *
 * \param  *sectorList_ast: pointer to array with sector information
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_restoreErasedAfterFullToUsed(rba_FeeFs1x_Sector_SectorListElement_tst *sectorList_ast)
{
    /* Workaround or robustness for the Kammerer/Daimler problem in the CB. If there is data programmed in a sector, but there is no USED
     * marker in the sector header, data was overprogrammed by the FEE. For this case it is not critical to mark the ERASED sector as USED.
     * To be consistent in RAM and FLASH, the marker will be written directly into the DFLASH. */

    uint8 sectorCtr_u8;
    rba_FeeFs1x_RetVal_ten retValMarkSector;

    for(sectorCtr_u8 = 1; sectorCtr_u8 < rba_FeeFs1x_Sector_const_st.xSectors_u8; sectorCtr_u8++)
    {
        if ((rba_FeeFs1x_Sector_SectorState_ERASED_e == sectorList_ast[sectorCtr_u8].sectorState_en) &&
                (rba_FeeFs1x_Sector_SectorState_FULL_e == sectorList_ast[sectorCtr_u8-1].sectorState_en))
        {
            /* write the USED marker to RAM and DFlash */
            rba_FeeFs1x_Sector_markUsed(sectorCtr_u8);
            do
            {
                retValMarkSector = rba_FeeFs1x_Sector_markSectorDo();
            }while(rba_FeeFs1x_Pending_e == retValMarkSector);
            /* Evaluation of while loop: The function is called only during init phase where Fee_Medium operates in synchronous mode.
             * The while loop is required for retry handling of marker write. As long as Fee_Medium has proper exit criteria,
             * the while loop here is safe (Refer handling of while loop in Fee_Medium) */

            /* In case the writing of Used Marker fails, the state is not updated by the rba_FeeFs1x_Sector_markSectorDo(),
             * so the state is updated here explicity, so that the software cotinues to run with status update in RAM. */
            sectorList_ast[sectorCtr_u8].sectorState_en = rba_FeeFs1x_Sector_SectorState_USED_e;

            /* Also, its ok to not write the Start Marker, as default value will be taken and other software parts are
             * designed to work without valid start marker. Not writing start marker saves start up time.  */
        }
    }
}

/**
 *********************************************************************
 *
 * checks if any sector before a USED or FULL sector is in state CLONE, UNDEF, ERASED or ERASE_REQUEST
 *
 * \param   *sectorList_ast: pointer to struct, containing the secot information
 * \return
 * \retval  E_OK: initialization was successful
 * \retval  E_NOT_OK: initialization failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_Sector_init_checkInitializationOfAllSectors(const rba_FeeFs1x_Sector_SectorListElement_tst *sectorList_ast)
{
    uint8 sectorCtr_u8;
    boolean isSectorFullOrUsedReached_b;
    Std_ReturnType retVal;

    sectorCtr_u8 = rba_FeeFs1x_Sector_const_st.xSectors_u8;
    isSectorFullOrUsedReached_b = FALSE;
    retVal = E_OK;

    /* loop backwards over all sectors */
    /* Evaluation of while loop: The loop will exit after all sectors are scanned. The number of sectors to be scanned
     * is a constant and fixed. Hence, there is no infinite loop possible here. */
    while(sectorCtr_u8 > 0)
    {
        sectorCtr_u8 --;

        if((rba_FeeFs1x_Sector_SectorState_FULL_e == sectorList_ast[sectorCtr_u8].sectorState_en) ||
           (rba_FeeFs1x_Sector_SectorState_USED_e == sectorList_ast[sectorCtr_u8].sectorState_en))
        {
            isSectorFullOrUsedReached_b = TRUE;
        }
        else /* any other state then USED or FULL is found */
        {
            if(isSectorFullOrUsedReached_b)
            {
                /* another state then FULL or USED is found before another FULL or USED sector
                   This is critical and initialization failed */
                retVal = E_NOT_OK;
            }
        }
    }
    return retVal;
}

/**
 *********************************************************************
 *
 * returns the logical sector number of the sector which is active directly after initialization
 * if a USED sector is found, this is the active sector
 * if a FULL sector is found the active sector is the one behind the full sector
 * !!! be aware that if the last sector is FULL there is no valid active sector. In that case the active sector is not in the range of available sectors !!!
 *
 * \param   void
 * \return  uint8: logical sector number of the active sector
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint8 rba_FeeFs1x_Sector_getActiveSector(void)
{
    uint8 sectorCtr_u8;
    uint8 activeSector_u8;
    boolean isFullOrUsedReached_b;

    isFullOrUsedReached_b = FALSE;
    sectorCtr_u8 = rba_FeeFs1x_Sector_const_st.xSectors_u8;
    activeSector_u8 = sectorCtr_u8;

    /* loop backwards over all sectors */
    /* Evaluation of while loop: The loop will exit after all sectors are scanned. The number of sectors to be scanned
     * is a constant and fixed. Hence, there is no infinite loop possible here. */
    while(sectorCtr_u8 > 0)
    {
        sectorCtr_u8 --;
        if(!isFullOrUsedReached_b)
        {
            if(rba_FeeFs1x_Sector_SectorState_FULL_e == rba_FeeFs1x_Sector_sectorInfo_ast[sectorCtr_u8].sectorState_en)
            {
                /* sector state is FULL; so the active sector points to the sector behind, so don't update the active sector anymore */
                isFullOrUsedReached_b = TRUE;
            }
            else
            {
                /* sector state is UNDEF, USED, R2E, ERASED or CLONE, so consider this sector as the active sector */
                activeSector_u8 = sectorCtr_u8;
                if(rba_FeeFs1x_Sector_SectorState_USED_e == rba_FeeFs1x_Sector_sectorInfo_ast[sectorCtr_u8].sectorState_en)
                {
                    /* sector state is USED */
                    isFullOrUsedReached_b = TRUE;
                }
            }
        }
    }
    return activeSector_u8;
}


/**
 *********************************************************************
 *
 * calculates the start address of the physical sector indicated by logSectIdx_u8
 *
 * \param   logSectIdx_u8: logical sector from which the logical sector start address shall be calculated
 * \param   *logStartAddress_pu32: logical start address (physical start address + header marker address space)
 * \return  E_OK: logSectIdx_u8 is in the expected range   E_NOT_OK: logSectIdx_u8 is out of bounds
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_Sector_getLogStartAddress(uint8 logSectIdx_u8, uint32* logStartAddress_pu32)
{
    Std_ReturnType retVal;
    uint8 physSector_u8;
    uint32 physStartAddress_u32;
    uint32 AddressOffset2LogSectorStart_u32;

    retVal = E_NOT_OK;
    if(logSectIdx_u8 < rba_FeeFs1x_Sector_const_st.xSectors_u8)
    {
        physSector_u8 = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].physSectIdx_u8;
        /* calculate the physical start address of the given logical sector */
        physStartAddress_u32 = Fee_Prv_ConfigGetPhysStartAddress(physSector_u8, rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast);
        /* calculate the address offset from physical start of a sector to logical start of a sector */
        AddressOffset2LogSectorStart_u32 = rba_FeeFs1x_Sector_const_st.logSectorStartOffset_u8;
        /* logical start address of the sector is the physical start address of the sector  + address offset to the
           first page in the sector */
        *logStartAddress_pu32 = physStartAddress_u32 + AddressOffset2LogSectorStart_u32;
        retVal = E_OK;
    }
    return retVal;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_getLogStartAddressPrechecked
 * calculates the start address of the physical sector indicated by logSectIdx_u8.
 * \attention: In comparison to rba_FeeFs1x_Sector_getLogStartAddress, this function doesn't perform safetychecks.
 * The inboundness of the sector index has to be ensured by the caller
 *
 * \param   logSectIdx_u8: logical sector from which the logical sector start address shall be calculated
 *
 * \return  log. start address of the sector
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_Sector_getLogStartAddressPrechecked(uint8 logSectIdx_u8)
{
    uint8 physSector_u8;
    uint32 physStartAddress_u32;
    uint32 AddressOffset2LogSectorStart_u32;

    physSector_u8 = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].physSectIdx_u8;
    /* calculate the physical start address of the given logical sector */
    physStartAddress_u32 = Fee_Prv_ConfigGetPhysStartAddress(physSector_u8, rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast);
    /* calculate the address offset from physical start of a sector to logical start of a sector */
    AddressOffset2LogSectorStart_u32 = rba_FeeFs1x_Sector_const_st.logSectorStartOffset_u8;
    /* logical start address of the sector is the physical start address of the sector  + address offset to the
               first page in the sector */
    return (physStartAddress_u32 + AddressOffset2LogSectorStart_u32);
}


/**
 *********************************************************************
 *
 * calculates the logical start page of the sector indicated by logSectIdx_u8
 *
 * \param   logSectIdx_u8: logical sector from which the logical start page shall be calculated
 * \return  logical start page
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_Sector_getLogStartPage(uint8 logSectIdx_u8)
{
    uint32  logStartPage_u32;
    /* logical start page of given logical sector:
       amount of logical pages per sector * logSectIdx_u8
       in the design limitations it is mentioned that the overall number of logical sector pages does not exceed 32 bit */
    logStartPage_u32 = rba_FeeFs1x_Sector_const_st.logSectorPages_u32 * logSectIdx_u8;

    return logStartPage_u32;
}

/**
 *********************************************************************
 *
 * calculates the logical end page of the sector indicated by logSectIdx_u8
 *
 * \param   logSectIdx_u8: logical sector from which the logical end page shall be calculated
 * \return  uint32: logical end page
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_Sector_getLogEndPage(uint8 logSectIdx_u8)
{
    uint32 logStartPage_u32;

    /* logical end page of given logical sector:
       (amount of logical pages per sector * logSectIdx_u8) + amount of logical pages per sector - 1)
       in the design limitations it is mentioned that the overall number of logical sector pages does not exceed 32 bit */
    logStartPage_u32 = rba_FeeFs1x_Sector_const_st.logSectorPages_u32 * logSectIdx_u8;
    return (logStartPage_u32 + rba_FeeFs1x_Sector_const_st.logSectorPages_u32 - 1u);
}

/**
 *********************************************************************
 *rba_FeeFs1x_Sector_isMemFull
 * Checks if the memory would be full if the given block is written
 *
 * \param   lastPageOfNewBlock_u32: logical end page of the block that would be written
 * \return  returns whether write will lead to memory full
 * \retval  TRUE  : write will result in Mem full
 * \retval  FALSE : write will not result in mem full
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_Sector_isMemFull(uint32 lastPageOfNewBlock_u32)
{
    boolean retVal_b;
    uint32 totalNrLogPagesInMem_u32;

    totalNrLogPagesInMem_u32 = rba_FeeFs1x_Sector_const_st.logSectorPages_u32 * rba_FeeFs1x_Sector_const_st.xSectors_u8;

    if(lastPageOfNewBlock_u32 >= totalNrLogPagesInMem_u32)
    {
        // Memory is full
        retVal_b = TRUE;
    }
    else
    {
        // Memory is not full
        retVal_b = FALSE;
    }

    return (retVal_b);
}

/**
 *********************************************************************
 * returns the physical sector index of a logical sector
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: index of logical sector from which the physical sector index shall be returned
 * \return  uint8: value of the physical sector index
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint8 rba_FeeFs1x_Sector_getPhysIdxByLogIdx(uint8 logSectIdx_u8)
{
    return (rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].physSectIdx_u8);
}

/**
 *********************************************************************
 * returns the logical sector index of the passed physical sector index
 *
 * \param   idxPhysSect_u8: index of physical sector from which the logical sector index shall be returned
 * \return  uint8: value of the logical sector index
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint8 rba_FeeFs1x_Sector_getLogIdxByPhysIdx(uint8 idxPhysSect_u8)
{
    uint8 idxLogSect_u8 = RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE;
    uint8_least idxCurrSect_qu8;

    for(idxCurrSect_qu8 = 0u ; idxCurrSect_qu8 < RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE ; idxCurrSect_qu8++)
    {
        if(idxPhysSect_u8 == rba_FeeFs1x_Sector_sectorInfo_ast[idxCurrSect_qu8].physSectIdx_u8)
        {
            idxLogSect_u8 = (uint8)idxCurrSect_qu8;
            break;
        }
    }

    return (idxLogSect_u8);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_getSectCngCnt
 * returns the sector change counter of the requested sector
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   idxLogSector_u8: logical sector from which the sector state shall be returned
 * \return  uint32: Value of the sector change counter
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32   rba_FeeFs1x_Sector_getSectCngCnt(uint8 idxLogSector_u8)
{
    return(rba_FeeFs1x_Sector_sectorInfo_ast[idxLogSector_u8].sectorCngCnt_u32);
}


/**
 *********************************************************************
 * rba_FeeFs1x_Sector_getState
 * returns the state of the sector indicated by logSectIdx_u8
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: logical sector from which the sector state shall be returned
 * \return  enumerator returning the state of the sector
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_Sector_SectorState_ten rba_FeeFs1x_Sector_getState(uint8 logSectIdx_u8)
{
    /* return the state of the requested sector */
    return (rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].sectorState_en);
}

/**
 *********************************************************************
 *
 * from a given start marker address the page number is calculated
 *
 * \param   logSectIdx_u8: logical sector from which the sector state shall be returned
 * \param   *page_pu32: calculated page number
 * \return  Std_ReturnType
 * \retval  E_OK: initialization finished without any errors
 * \retval  E_NOT_OK: logSectIdx out of bounds, no start address available, start address invalid or misaligned
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_Sector_getStartMarkerContentAsPageNo(uint8 logSectIdx_u8 , uint32* page_pu32)
{
    Std_ReturnType retVal;
    Std_ReturnType retVal_getLogStartAddress;
    uint32 startAddrSect_u32;
    uint32 addrOffsetSect_u32;
    uint32 logStartPage_u32;

    retVal = E_NOT_OK;

    if((logSectIdx_u8 < rba_FeeFs1x_Sector_const_st.xSectors_u8) &&
       (RBA_FEEFS1X_MAXUINT32 != rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].startMarkerContent_u32))
    {
        retVal_getLogStartAddress = rba_FeeFs1x_Sector_getLogStartAddress(logSectIdx_u8, &startAddrSect_u32);

        if((E_OK == retVal_getLogStartAddress) &&
                (rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].startMarkerContent_u32 >= startAddrSect_u32))
        {
            /* calculate the address offset between logical start address of the sector and given start address */
            addrOffsetSect_u32 = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].startMarkerContent_u32 - startAddrSect_u32;
            /* check the alignment */
            if((addrOffsetSect_u32 % (uint32)rba_FeeFs1x_Sector_const_st.logPageLen_u8) == 0u)
            {
                /* calculate the logical page number of the given start address */
                logStartPage_u32 = rba_FeeFs1x_Sector_getLogStartPage(logSectIdx_u8);
                *page_pu32 = logStartPage_u32 + (addrOffsetSect_u32 / rba_FeeFs1x_Sector_const_st.logPageLen_u8);
                retVal = E_OK;
            }
        }
    }
    return retVal;
}

/**
 *********************************************************************
 * initialises erasing a sector
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: logical sector to be erased
 * \param   isReerasing_b: TRUE: this sector gets reerased, FALSE: initial erase of the sector
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_erase(uint8 logSectIdx_u8, boolean isReerasing_b)
{
    uint32 eraseStartAddr_u32;
    uint32 eraseEndAddr_u32;
    uint8 physSectorIdx_u8;

    /* calculate start and end address of the sector */
    physSectorIdx_u8 = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].physSectIdx_u8;
    eraseEndAddr_u32 = Fee_Prv_ConfigGetPhysEndAddress(physSectorIdx_u8, rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast);
    eraseStartAddr_u32 = Fee_Prv_ConfigGetPhysStartAddress(physSectorIdx_u8, rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast);

    /* check parameter and address range */
    rba_FeeFs1x_Sector_erase_st.eraseStartAddr_u32 = eraseStartAddr_u32;
    rba_FeeFs1x_Sector_erase_st.eraseLength_u32 = (eraseEndAddr_u32 - eraseStartAddr_u32) + 1u;

    rba_FeeFs1x_Sector_erase_st.logSectIdx_u8 = logSectIdx_u8;
    rba_FeeFs1x_Sector_erase_st.isReerasing_b = isReerasing_b;

    /* set initial state to erase */
    rba_FeeFs1x_Sector_erase_st.state_en = rba_FeeFs1x_Sector_eraseStm_writeEraseStartFlag_e;
    rba_FeeFs1x_Sector_erase_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * performs erasing a sector
 * In case the external erase function returns an error, infinite erase retries are performed
 * Hint: if sector erase fails, it will be retried infinite times. No external error will be returned
 *
 * \param   void
 * \return  state of sector erase
 * \retval  rba_FeeFs1x_JobOK_e: job finished succesfully
 * \retval  rba_FeeFs1x_Pending_e: operation still pending or external write function returned an error
 * \retval  rba_FeeFs1x_ErrorInternal_e: an internal error occured
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_eraseDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_Sector_erase_st.state_en)
    {
        case rba_FeeFs1x_Sector_eraseStm_writeEraseStartFlag_e:
        {
            retVal_en = rba_FeeFs1x_Sector_eraseDo_writeEraseStartFlag(&rba_FeeFs1x_Sector_erase_st);
            break;
        }

        case rba_FeeFs1x_Sector_eraseStm_erase_e:
        {
            retVal_en = rba_FeeFs1x_Sector_eraseDo_erase(&rba_FeeFs1x_Sector_erase_st);
            break;
        }

        case rba_FeeFs1x_Sector_eraseStm_wrEraseMarker_e:
        {
            retVal_en = rba_FeeFs1x_Sector_eraseDo_wrEraseMarker(&rba_FeeFs1x_Sector_erase_st);
            break;
        }

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    // on job exit, reset all statemachine data
    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_Sector_erase_st.state_en = rba_FeeFs1x_Sector_eraseStm_idle_e;
    }
    return  retVal_en;
}

/**
 *********************************************************************
 * state function for writing Erase Start Flag before erasing a sector
 *
 * \param   pointer to statemachine data
 * \return  state of sector erase
 * \retval  rba_FeeFs1x_Pending_e       : operation still pending
 * \retval  rba_FeeFs1x_ErrorInternal_e : corruption of the state variable
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_eraseDo_writeEraseStartFlag(rba_FeeFs1x_Sector_erase_tst *fsm_pst)
{
    MemIf_JobResultType retValBlankCheck_en;
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint32 adrStartFlag_u32;

    if(FEE_PRV_LIBENTRY)
    {
        adrStartFlag_u32 = fsm_pst->eraseStartAddr_u32 +
                (((uint8)rba_FeeFs1x_Sector_Marker_ERASE_START_FLAG_e - 1u) * rba_FeeFs1x_Sector_const_st.markerMemLen_u8);

        /* program the page with inverse erase pattern */
        rba_FeeFs1x_Sector_setMemComplementErasePattern(fsm_pst->WriteBfr_au8,
                    0u,
                    rba_FeeFs1x_Sector_const_st.markerMemLen_u8);

        /* compare after write is not required,
           but actually Fee_Prv_MediumWrite does not provide a suitable sequence without compare after write
           Eventually detected data mismatch will be ignored. */
        Fee_Prv_MediumWrite(Fee_Rb_DeviceName,
                            adrStartFlag_u32,
                            0u,
                            (void*) fsm_pst->WriteBfr_au8,
                            rba_FeeFs1x_Sector_const_st.markerMemLen_u8,
                            FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COPY_COMP_E);
    }

    retValBlankCheck_en = Fee_Prv_MediumWriteDo(Fee_Rb_DeviceName);

    switch(retValBlankCheck_en)
    {
        case MEMIF_JOB_PENDING:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_OK:
        case MEMIF_JOB_FAILED:
        {
            /* MEMIF_JOB_OK: page was blank, but consecutive writing of the page was successful
               MEMIF_JOB_FAILED: page is programmed, or write was successful but data comparison failed
               In both cases, the main goal, that the page is not blank anymore is reached
               Therefore in both cases switch to next state */
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_eraseStm_erase_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        default:
        {
            /* unexpected return value */
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * state function for erasing a sector
 *
 * \param   pointer to statemachine data
 * \return  state of sector erase
 * \retval  rba_FeeFs1x_JobOK_e: job finished succesfully
 * \retval  rba_FeeFs1x_Pending_e: operation still pending
 * \retval  rba_FeeFs1x_ErrorInternal_e: internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_eraseDo_erase(rba_FeeFs1x_Sector_erase_tst *fsm_pst)
{
    MemIf_JobResultType mediumEraseDoRetVal_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        Fee_Prv_MediumErase(Fee_Rb_DeviceName, fsm_pst->eraseStartAddr_u32, fsm_pst->eraseLength_u32);
    }
    mediumEraseDoRetVal_en = Fee_Prv_MediumEraseDo(Fee_Rb_DeviceName);

    switch(mediumEraseDoRetVal_en)
    {
        case MEMIF_JOB_OK:
        {
            /* sector erase successful finished, switch to write the erase marker */

            // update the sector data
            rba_FeeFs1x_Sector_sectorInfo_ast[fsm_pst->logSectIdx_u8].sectorMarkerWriteIdx_u8 = 0u;
            rba_FeeFs1x_Sector_sectorInfo_ast[fsm_pst->logSectIdx_u8].startMarkerContent_u32 = RBA_FEEFS1X_MAXUINT32;
            rba_FeeFs1x_Sector_sectorInfo_ast[fsm_pst->logSectIdx_u8].sectorState_en = rba_FeeFs1x_Sector_SectorState_UNDEF_e;
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_eraseStm_wrEraseMarker_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_PENDING:
        {
            /* keep state and return pending */
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            /* an infinite erase retry shall be performed -> reenter the erase state and stay pending */
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_eraseStm_erase_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        default:
        {
            /* return an internal error */
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }
    return(retVal_en);
}

/**
 *********************************************************************
 * state function for writing the erase marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   pointer to statemachine data
 * \return  state of writing the erase marker
 * \retval  rba_FeeFs1x_JobOK_e: job finished succesfully
 * \retval  rba_FeeFs1x_Pending_e: operation still pending
 * \retval  rba_FeeFs1x_ErrorInternal_e: internal processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_eraseDo_wrEraseMarker(rba_FeeFs1x_Sector_erase_tst *fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValMark_en;

    if(FEE_PRV_LIBENTRY)
    {
        // trigger the marker write
        rba_FeeFs1x_Sector_markErased(fsm_pst->logSectIdx_u8, fsm_pst->isReerasing_b);
    }

    // Cyclic call the Do function
    retValMark_en = rba_FeeFs1x_Sector_markSectorDo();

    //evaluate the result
    switch(retValMark_en)
    {
        case rba_FeeFs1x_JobOK_e:
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValMark_en;
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // if writing of the erased marker fails, restart complete sector erase
            fsm_pst->isReerasing_b = TRUE;
            FEE_PRV_LIBSC(rba_FeeFs1x_Sector_eraseStm_writeEraseStartFlag_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        } break;
        // case rba_FeeFs1x_JobFailed_e:
        // When writing the erase marker fails, ErrorExternal, not JobFailed is returned
        //-> handle JobFailed as an unexpected return value
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }
    return retVal_en;
}

/**
 *********************************************************************
 *
 * check and prepare data to write a defined marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   SectorState_en: sector state to be written
 * \param   MarkerState_en: marker state to be written
 * \param   logSectIdx_u8: logical sector from which the sector state shall be returned
 * \param   isReerasing_b: TRUE: this sector gets reerased, FALSE: initial erase of the sector
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_Sector_markSector(  rba_FeeFs1x_Sector_SectorState_ten SectorState_en,
                                            rba_FeeFs1x_Sector_markerStates_ten MarkerState_en,
                                            uint8 logSectIdx_u8,
                                            boolean isReerasing_b)
{
    uint32 secCngCnt_u32;
    rba_FeeFs1x_Sector_marker_tst marker_st;

    rba_FeeFs1x_Sector_markSector_st.writeRetries_u8 = 0;
    rba_FeeFs1x_Sector_markSector_st.sectorState_en = SectorState_en;
    /* initialize the state machine */
    rba_FeeFs1x_Sector_markSector_st.state_en = rba_FeeFs1x_Sector_markSectorDoStm_write_e;
    rba_FeeFs1x_Sector_markSector_st.entry_b = TRUE;

    rba_FeeFs1x_Sector_markSector_st.logSectIdx_u8 = logSectIdx_u8;
    /* determine the physical sector where the marker shall be written */
    rba_FeeFs1x_Sector_markSector_st.physSectIdx_u8 = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].physSectIdx_u8;

    secCngCnt_u32 = rba_FeeFs1x_Sector_calcSectCngCnt(logSectIdx_u8, isReerasing_b);

    /* prepare the sector header marker */
    marker_st.xPattern_u16 = rba_FeeFs1x_Sector_const_st.markerPattern_u16;
    marker_st.sectorMarkerState_en = MarkerState_en;
    marker_st.xContent_u32 = secCngCnt_u32;

    /* calculate the new marker CRC and save it to the marker structure */
    marker_st.xChecksum_u16 = rba_FeeFs1x_Sector_calcCRC(rba_FeeFs1x_Sector_const_st.markerPattern_u16,
            MarkerState_en, marker_st.xContent_u32);
    /* create the sector header marker which shall be written to flash */
    rba_FeeFs1x_Sector_createMarkerSectorHeader(rba_FeeFs1x_Sector_markSector_st.writeBfr_au8, &marker_st);
}

/**
 *********************************************************************
 *
 * check and prepare data to write the erase marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: logical sector for which the sector marker has to be written
 * \param   isReerasing_b: TRUE: this sector gets reerased, FALSE: initial erase of the sector
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE void rba_FeeFs1x_Sector_markErased(uint8 logSectIdx_u8, boolean isReerasing_b)
{
    rba_FeeFs1x_Sector_markSector(rba_FeeFs1x_Sector_SectorState_ERASED_e,rba_FeeFs1x_Sector_Marker_ERASED_e,logSectIdx_u8,isReerasing_b);
}



/**
 *********************************************************************
 *
 * check and prepare data to write a used marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: logical sector for which the sector marker has to be written
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_markUsed(uint8 logSectIdx_u8)
{
    rba_FeeFs1x_Sector_markSector(rba_FeeFs1x_Sector_SectorState_USED_e,rba_FeeFs1x_Sector_Marker_USED_e,logSectIdx_u8,FALSE);
}

/**
 *********************************************************************
 *
 * check and prepare data to write a full marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: logical sector for which the sector marker has to be written
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_markFull(uint8 logSectIdx_u8)
{
    rba_FeeFs1x_Sector_markSector(rba_FeeFs1x_Sector_SectorState_FULL_e,rba_FeeFs1x_Sector_Marker_FULL_e,logSectIdx_u8,FALSE);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_markStart
 * check and prepare data to write the start marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8     : logical sector for which the sector marker has to be written
 * \param   addrOfFirstBlk_u32: address to be written in start marker (only the least 3 bytes are written)
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_markStart(uint8 logSectIdx_u8, uint32 addrOfFirstBlk_u32)
{
    rba_FeeFs1x_Sector_marker_tst marker_st;

    /* initialize the state machine */
    rba_FeeFs1x_Sector_markSector_st.state_en = rba_FeeFs1x_Sector_markSectorDoStm_write_e;
    rba_FeeFs1x_Sector_markSector_st.entry_b = TRUE;

    rba_FeeFs1x_Sector_markSector_st.writeRetries_u8 = 0;

    /* do not change the sector state, so write again the actual state */
    rba_FeeFs1x_Sector_markSector_st.sectorState_en = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].sectorState_en;

    rba_FeeFs1x_Sector_markSector_st.logSectIdx_u8 = logSectIdx_u8;
    /* determine the physical sector where the marker shall be written */
    rba_FeeFs1x_Sector_markSector_st.physSectIdx_u8 = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].physSectIdx_u8;

    /* prepare the sector header marker */
    marker_st.xPattern_u16 = rba_FeeFs1x_Sector_const_st.markerPattern_u16;
    marker_st.sectorMarkerState_en = rba_FeeFs1x_Sector_Marker_START_e;
    /* content is the physical start address */
    marker_st.xContent_u32 = addrOfFirstBlk_u32;
    /* save start address offset to RAM */
    rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].startMarkerContent_u32 = addrOfFirstBlk_u32;

    /* calculate the new marker CRC and save it to the marker structure */
    marker_st.xChecksum_u16 = rba_FeeFs1x_Sector_calcCRC(rba_FeeFs1x_Sector_const_st.markerPattern_u16,
            rba_FeeFs1x_Sector_Marker_START_e, marker_st.xContent_u32);
    /* create the sector header marker which shall be written to flash */
    rba_FeeFs1x_Sector_createMarkerSectorHeader(rba_FeeFs1x_Sector_markSector_st.writeBfr_au8, &marker_st);
}

/**
 *********************************************************************
 *
 * check and prepare data to write the R2E marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: logical sector for which the sector marker has to be written
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_markReady4Erase(uint8 logSectIdx_u8)
{
    /* When a sector is marked for ready4erase, it means that contents of this sector are not longer needed.
     * This also includes start marker content. so set the start marker content to default value. */
    rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].startMarkerContent_u32 = RBA_FEEFS1X_MAXUINT32;

    rba_FeeFs1x_Sector_markSector(rba_FeeFs1x_Sector_SectorState_R2E_e,rba_FeeFs1x_Sector_Marker_ERASE_REQUEST_e,logSectIdx_u8,FALSE);
}

/**
 *********************************************************************
 *
 * check and prepare data to write the clone marker
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8: logical sector for which the sector marker has to be written
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_markClone(uint8 logSectIdx_u8)
{
    rba_FeeFs1x_Sector_markSector(rba_FeeFs1x_Sector_SectorState_CLONE_e,rba_FeeFs1x_Sector_Marker_CLONE_e,logSectIdx_u8,FALSE);
}

/**
 *********************************************************************
 *
 * execution of the previously prepared marker write
 *
 * \param   void
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e:       marker write is still ongoing
 * \retval  rba_FeeFs1x_JobOK_e:         marker write was succesfull
 * \retval  rba_FeeFs1x_ErrorInternal_e: critical internal error occured: e.g. header sector marker area is full
 * \retval  rba_FeeFs1x_ErrorExternal_e: requested marker could not be written
 * \retval  rba_FeeFs1x_JobFailed_e:     R2E or FULL marker could not be written to flash, but state in RAM is set
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_markSectorDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;


    if(rba_FeeFs1x_Sector_markSectorDoStm_write_e == rba_FeeFs1x_Sector_markSector_st.state_en)
    {
        retVal_en = rba_FeeFs1x_Sector_markSectorDo_write(&rba_FeeFs1x_Sector_markSector_st);
    }
    else
    {
        /* When calling the Do function and the State machine is not triggered return JobOK */
        retVal_en = rba_FeeFs1x_JobOK_e;
    }

    // on job exit, reset all statemachine data
    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_Sector_markSector_st.state_en   = rba_FeeFs1x_Sector_markSectorDoStm_idle_e;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 *
 * state function of markSectorDo: executes writing the marker
 *
 * \param   *fsm_pst: pointer to struct of the statemachine
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e:       marker write is still ongoing
 * \retval  rba_FeeFs1x_JobOK_e:         marker write was succesfull
 * \retval  rba_FeeFs1x_ErrorInternal_e: critical internal error occured: e.g. header sector marker area is full
 * \retval  rba_FeeFs1x_ErrorExternal_e: requested marker could not be written
 * \retval  rba_FeeFs1x_JobFailed_e:     R2E or FULL marker could not be written to flash, but state in RAM is set
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Sector_markSectorDo_write(rba_FeeFs1x_Sector_markSector_tst *fsm_pst)
{
    uint32                 sectorMarkerWriteAddr_u32;
    uint8                  sectorMarkerWriteIdx_u8;
    MemIf_JobResultType    mediumWriteRetVal_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    retVal_en = rba_FeeFs1x_Pending_e;
    /* entry action */
    if(FEE_PRV_LIBENTRY)
    {
        sectorMarkerWriteIdx_u8 = rba_FeeFs1x_Sector_sectorInfo_ast[fsm_pst->logSectIdx_u8].sectorMarkerWriteIdx_u8;
        /* check write position */
        if(sectorMarkerWriteIdx_u8 < rba_FeeFs1x_Sector_const_st.xSectorMarkers_u8)
        {
            /* calculate the write address */
            sectorMarkerWriteAddr_u32 = Fee_Prv_ConfigGetPhysStartAddress(fsm_pst->physSectIdx_u8, rba_FeeFs1x_Prv_Cfg_FlashPropTable_cast);
            sectorMarkerWriteAddr_u32 += sectorMarkerWriteIdx_u8 * rba_FeeFs1x_Sector_const_st.markerMemLen_u8;

            /* increment the header marker write position */
            sectorMarkerWriteIdx_u8++;

            /* save the next header marker write index -> it does not matter if write was successful or not. on error,
             * retry should be done on the next page. */
            rba_FeeFs1x_Sector_sectorInfo_ast[fsm_pst->logSectIdx_u8].sectorMarkerWriteIdx_u8 = sectorMarkerWriteIdx_u8;

            /* fill up the unused buffer space with the inverse erase pattern */
            rba_FeeFs1x_Sector_setMemComplementErasePattern(fsm_pst->writeBfr_au8,
                    rba_FeeFs1x_Sector_const_st.markerSize_u8,
                    rba_FeeFs1x_Sector_const_st.markerMemLen_u8);
            Fee_Prv_MediumWrite(
                    Fee_Rb_DeviceName,
                    sectorMarkerWriteAddr_u32,
                    0u,
                    (void*)fsm_pst->writeBfr_au8,
                    rba_FeeFs1x_Sector_const_st.markerMemLen_u8,
                    FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COPY_COMP_E);

            /* write the sector header marker */
            mediumWriteRetVal_en = Fee_Prv_MediumWriteDo(Fee_Rb_DeviceName);
        }
        else
        {
            /* the header marker section is full, so treat this as failure during writing the marker */
            mediumWriteRetVal_en = MEMIF_JOB_FAILED;
        }
    }
    else
    {
        /* write the sector header marker */
        mediumWriteRetVal_en = Fee_Prv_MediumWriteDo(Fee_Rb_DeviceName);
    }

    switch (mediumWriteRetVal_en)
    {
        case MEMIF_JOB_OK:
        {
            /* save the state to RAM */
            rba_FeeFs1x_Sector_sectorInfo_ast[fsm_pst->logSectIdx_u8].sectorState_en = fsm_pst->sectorState_en;
            /* reset the state, return OK */
            retVal_en = rba_FeeFs1x_JobOK_e;
            break;
        }
        case MEMIF_JOB_PENDING:
        {
            /* stay in actual state, return pending */
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            /* a write error occurred, retry write of the marker on the next marker space */
            /* decrease the number of write retries */
            fsm_pst->writeRetries_u8 ++;
            if(fsm_pst->writeRetries_u8 < rba_FeeFs1x_Sector_const_st.readWriteRetries_u8)
            {
                /* number of write retries is not exceeded, so try again */
                FEE_PRV_LIBSC(rba_FeeFs1x_Sector_markSectorDoStm_write_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                /* number of write retries is exceeded
                       in case the FULL or R2E marker could not be written, set at least the RAM state and indicate this by returning job failed */
                if((rba_FeeFs1x_Sector_SectorState_FULL_e == fsm_pst->sectorState_en) ||
                   (rba_FeeFs1x_Sector_SectorState_R2E_e == fsm_pst->sectorState_en))
                {
                    /* save the state to RAM */
                    rba_FeeFs1x_Sector_sectorInfo_ast[fsm_pst->logSectIdx_u8].sectorState_en = fsm_pst->sectorState_en;
                    retVal_en = rba_FeeFs1x_JobFailed_e;
                }
                else
                {
                    retVal_en = rba_FeeFs1x_ErrorExternal_e;
                }
            }
            break;
        }
        default:
        {
            /* something unexpected occured, return error */
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 *
 * calculate and return the sector change counter
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   logSectIdx_u8       : logical sector index
 * \param   preserveSectCngCnt_b: TRUE: keep the old sector change counter  FALSE: calculate a new sector change counter
 * \return  actual sector change counter
 * \seealso
 * \usedresources
 *********************************************************************
 */
static uint32 rba_FeeFs1x_Sector_calcSectCngCnt(uint8 logSectIdx_u8, boolean preserveSectCngCnt_b)
{
    uint32 secCngCnt_u32;

    if((rba_FeeFs1x_Sector_SectorState_UNDEF_e == rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].sectorState_en) &&
       (!preserveSectCngCnt_b))
    {
        /* sector is in state UNDEF -> there is no valid sector change counter available */
        rba_FeeFs1x_Sector_data_st.maxSectCngCnt_u32 ++;
        secCngCnt_u32 = rba_FeeFs1x_Sector_data_st.maxSectCngCnt_u32;
        rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].sectorCngCnt_u32 = secCngCnt_u32;
    }
    else
    {
        /* sector change counter is valid */
        secCngCnt_u32 = rba_FeeFs1x_Sector_sectorInfo_ast[logSectIdx_u8].sectorCngCnt_u32;
    }

    return (secCngCnt_u32);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_getSectChngCnt
 * Function returns the sector change counter of the latest sector
 *
 * \return  actual sector change counter
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_Sector_getSectChngCnt(void)
{
    uint32 xSecChngCnt_u32 = rba_FeeFs1x_Sector_data_st.maxSectCngCnt_u32;

    if(xSecChngCnt_u32 >= RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE)
    {
        /* Calculate the amount of sector reorganizations */
        xSecChngCnt_u32 = (xSecChngCnt_u32 - RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE);
    }
    else
    {
        /* No sector reorganization was necessary so far */
        xSecChngCnt_u32 = 0;
    }

    return(xSecChngCnt_u32);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_getNrOfLogPagesPerSector
 * Function returns the total number of logical pages per sector
 *
 * \return  number of logical pages
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_Sector_getNrOfLogPagesPerSector(void)
{
    return (rba_FeeFs1x_Sector_const_st.logSectorPages_u32);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_getTotalNrOfSectors
 * Function returns the total number of sectors in the complete DFLASH
 *
 * \return  number of logical pages
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint8 rba_FeeFs1x_Sector_getTotalNrOfSectors(void)
{
    return (rba_FeeFs1x_Sector_const_st.xSectors_u8);
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_reorderLogSect0
 * This function is called after reorganisation to reorder the state order.
 *
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_reorderLogSect0(void)
{
    uint8 idx_u8;
    rba_FeeFs1x_Sector_SectorListElement_tst sect0_st;

    // store the oldest sector values (which is marked as Request to Erase)
    sect0_st = rba_FeeFs1x_Sector_sectorInfo_ast[0];

    for(idx_u8 = 0; idx_u8 < (RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u); idx_u8++)
    {
        rba_FeeFs1x_Sector_sectorInfo_ast[idx_u8] = rba_FeeFs1x_Sector_sectorInfo_ast[idx_u8 + 1u];
    }

    // restore the contents of the latest sector
    rba_FeeFs1x_Sector_sectorInfo_ast[(RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE - 1u)] = sect0_st;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Sector_switchLogSectors
 * switch logical sectors
 * \attention: Function should be called only with valid parameter value!
 *
 * \param   idxLogSect1_u8: first sector to be switched
 * \param   idxLogSect2_u8: second sector to be switched
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Sector_switchLogSectors(uint8 idxLogSect1_u8 , uint8 idxLogSect2_u8)
{
    rba_FeeFs1x_Sector_SectorListElement_tst tmpSectorStorage_st;

    tmpSectorStorage_st = rba_FeeFs1x_Sector_sectorInfo_ast[idxLogSect1_u8];

    rba_FeeFs1x_Sector_sectorInfo_ast[idxLogSect1_u8] = rba_FeeFs1x_Sector_sectorInfo_ast[idxLogSect2_u8];
    rba_FeeFs1x_Sector_sectorInfo_ast[idxLogSect2_u8] = tmpSectorStorage_st;
}

/**
 *********************************************************************
 *
 * \param   markerPattern_u16:     marker pattern (0xCAFE)
 * \param   markerState_en:        state: USED, FULL, ...
 * \param   content_u32:           marker content: e.g sector change counter
 * \return  uint16
 * \retval  calculated checksum of filled struct of type rba_FeeFs1x_Sector_crcContent_tst
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE uint16 rba_FeeFs1x_Sector_calcCRC(uint16 markerPattern_u16, rba_FeeFs1x_Sector_markerStates_ten markerState_en, uint32 content_u32)
{
    uint16 crcChecksum_u16;
    rba_FeeFs1x_Sector_crcContent_tst crcContent_st;

    /* in Fee1 the CRC calculation was done over a struct with uint16 elements.
     * As a consequence the byte order of pattern_u16 changes between little- and big endian.
     * Due to backwards compatibility this must be preserved
     * address offset        0x0        0x1
     * little endian:        0xFE       0xCA
     * big endian:           0xCA       0xFE    */

    crcContent_st.pattern_u16 = markerPattern_u16;
    crcContent_st.id_u8 = (uint8)markerState_en;
    crcContent_st.contentHiByte_u8 = (uint8)(content_u32 >> 16);
    crcContent_st.contentMidByte_u8 = (uint8)(content_u32 >> 8);
    crcContent_st.contentLoByte_u8 = (uint8)content_u32;

    crcChecksum_u16 = Crc_CalculateCRC16((const uint8*)&crcContent_st,
            rba_FeeFs1x_Sector_const_st.markerDataSize_u8,
            rba_FeeFs1x_Sector_const_st.markerPattern_u16,
            FALSE);
    return (crcChecksum_u16);
}

/**
 *********************************************************************
 *
 * decompose raw sector header marker to *marker_pst
 *
 * \param   *readBfr_pu8: pointer to buffer with raw sector header marker
 * \param   *marker_pst: pointer to struct containing the decomposed sector header marker
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_init_createMarkerInfo(const uint8 *readBfr_pu8, rba_FeeFs1x_Sector_marker_tst *marker_pst)
{
    /* extract the sector header marker pattern */
    marker_pst->xPattern_u16 = ((uint16)(((uint16)(readBfr_pu8[RBA_FEEFS1X_SECTOR_M_PATTERN_HIBYTE_POS])) << 8u) |
            (uint16) readBfr_pu8[RBA_FEEFS1X_SECTOR_M_PATTERN_LOBYTE_POS]);
    /* extract the sector header marker Id (Used, Full,...) */
    marker_pst->sectorMarkerState_en = rba_FeeFs1x_Sector_init_mapMarkerID2Enum(readBfr_pu8[RBA_FEEFS1X_SECTOR_M_ID_POS]);
    /* extract the sector header marker content (sector change counter, ...) */
    marker_pst->xContent_u32 = ((uint32)readBfr_pu8[RBA_FEEFS1X_SECTOR_M_CONTENT_HIBYTE_POS] << 16u) |
            ((uint32)readBfr_pu8[RBA_FEEFS1X_SECTOR_M_CONTENT_MIDBYTE_POS] << 8u) |
            readBfr_pu8[RBA_FEEFS1X_SECTOR_M_CONTENT_LOBYTE_POS];
    /* extract the sector header marker checksum */
    marker_pst->xChecksum_u16 = ((uint16)(((uint16)(readBfr_pu8[RBA_FEEFS1X_SECTOR_M_CHECKSUM_HIBYTE_POS])) << 8u) |
            (uint16) readBfr_pu8[RBA_FEEFS1X_SECTOR_M_CHECKSUM_LOBYTE_POS]);
}


LOCAL_INLINE rba_FeeFs1x_Sector_markerStates_ten rba_FeeFs1x_Sector_init_mapMarkerID2Enum(uint8 id_u8)
{
    rba_FeeFs1x_Sector_markerStates_ten retVal_en;
    switch(id_u8)
    {
        case 1:
        {
            retVal_en = rba_FeeFs1x_Sector_Marker_ERASED_e;
        }break;
        case 2:
        {
            retVal_en = rba_FeeFs1x_Sector_Marker_USED_e;
        }break;
        case 3:
        {
            retVal_en = rba_FeeFs1x_Sector_Marker_FULL_e;
        }break;
        case 4:
        {
            retVal_en = rba_FeeFs1x_Sector_Marker_ERASE_REQUEST_e;
        }break;
        case 5:
        {
            retVal_en = rba_FeeFs1x_Sector_Marker_START_e;
        }break;
        case 6:
        {
            retVal_en = rba_FeeFs1x_Sector_Marker_CLONE_e;

        }break;
        default:
        {
            retVal_en = RBA_FEEFS1X_SECTOR_NUM_MARKER_E ;             /* Amount of required markers in the sector header + 1 */
        }break;
    }
    return retVal_en;
}
/**
 *********************************************************************
 *
 * create buffer to be written to flash
 *
 * \param   *writeBfr_pu8: pointer to buffer with raw sector header marker
 * \param   *marker_pcst: pointer to strcut containing the sector header marker information
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_createMarkerSectorHeader(uint8 *writeBfr_pu8, const rba_FeeFs1x_Sector_marker_tst *marker_pcst)
{
    /* write the sector header marker pattern to the write buffer */
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_PATTERN_HIBYTE_POS] = (uint8)(marker_pcst->xPattern_u16 >> 8u);
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_PATTERN_LOBYTE_POS] = (uint8)marker_pcst->xPattern_u16;

    /* write the sector header marker Id (Used, Full,...) to the write buffer */
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_ID_POS] = (uint8)marker_pcst->sectorMarkerState_en;

    /* write the sector header marker content (e.g. sequence change counter,...) to the write buffer */
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_CONTENT_HIBYTE_POS] = (uint8)(marker_pcst->xContent_u32 >> 16u);
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_CONTENT_MIDBYTE_POS] = (uint8)(marker_pcst->xContent_u32 >> 8u);
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_CONTENT_LOBYTE_POS] = (uint8)marker_pcst->xContent_u32;

    /* write the sector header marker checksum to the write buffer */
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_CHECKSUM_HIBYTE_POS] = (uint8)(marker_pcst->xChecksum_u16 >> 8u);
    writeBfr_pu8[RBA_FEEFS1X_SECTOR_M_CHECKSUM_LOBYTE_POS] = (uint8)marker_pcst->xChecksum_u16;
}

/**
 *********************************************************************
 *
 * fill up the buffer from startIdx_u8 to endIdx_u8 with the complementary erase pattern
 *
 * \param   *bfr_pu8: pointer to buffer to be filled with pattern
 * \param   startIdx_u8: start index for filling the buffer
 * \param   endIdx_u8: end index for filling the buffer
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_Sector_setMemComplementErasePattern(uint8 *bfr_pu8, uint8 startIdx_u8, uint8 endIdx_u8)
{
    uint32 i;

    for(i = startIdx_u8; i < endIdx_u8; i++)
    {
        bfr_pu8[i] = rba_FeeFs1x_Sector_const_st.complementErasePattern_u8;
    }
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
boolean rba_FeeFs1x_Sector_isSectorEraseOngoing(void)
{
    return (rba_FeeFs1x_Sector_eraseStm_idle_e != rba_FeeFs1x_Sector_erase_st.state_en);
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

