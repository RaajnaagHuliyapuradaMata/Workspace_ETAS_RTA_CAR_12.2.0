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
#include "rba_FeeFs1x_Prv_PAMapTypes.h"
#include "rba_FeeFs1x_Prv_PAMap.h"

#include "rba_FeeFs1x_Prv_Cfg.h"
#include "rba_FeeFs1x_Prv.h"

#include "Fee_Prv_Lib.h"
#include "Fee_Prv_Medium.h"

#include "rba_FeeFs1x_Prv_PASrv.h"
#include "rba_FeeFs1x_Prv_Sector.h"
/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

# if ((RBA_FEEFS1X_PAMAP_DETECTWRPAGE_CHUNKSIZE) > ((RBA_FEEFS1X_PRV_CFG_PHYS_SEC_END0 - RBA_FEEFS1X_PRV_CFG_PHYS_SEC_START0) - (RBA_FEEFS1X_PRV_SECTOR_HEADER_SIZE)))
#error "Chunksize is greater than logical sector size"
# endif

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

static rba_FeeFs1x_PAMap_fillSectorEnd_tst rba_FeeFs1x_PAMap_fillSectorEnd_st;
static rba_FeeFs1x_PAMap_prepareSectorStates_data_tst rba_FeeFs1x_PAMap_prepareSectorStates_data_st;
static rba_FeeFs1x_PAMap_data_tst rba_FeeFs1x_PAMap_data_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"




#define FEE_START_SEC_CONST_8
#include "Fee_MemMap.h"

#if(RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE > RBA_FEEFS1X_PAMAP_FILLPATTERN_SIZE)
#error "unsupported virtual page size"
#endif
static const uint8 rba_FeeFs1x_PAMap_fillSectorEnd_FillPattern[RBA_FEEFS1X_PAMAP_FILLPATTERN_SIZE] =
{
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT,
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT,
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT,
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT,
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT,
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT,
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT,
        (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT, (uint8)~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT
};

#define FEE_STOP_SEC_CONST_8
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */


#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE Std_ReturnType rba_FeeFs1x_PAMap_detectCurrentWrPage(void);
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_prepare(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst* fsm_pst);
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_determineLowerLimit(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst* fsm_pst);
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckAreas(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst* fsm_pst);
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckPagewise(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_fillSectorEndDo_write(rba_FeeFs1x_PAMap_fillSectorEnd_tst* fsm_pst);

static boolean          rba_FeeFs1x_PAMap_detectCurrentWrPage_execBlankcheckArea(uint32 startPage_u32 , uint32 chunkSize_u32);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrFull(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)   ;
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_erase(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)    ;
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrUsed(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)   ;
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrStart(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)  ;
/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 *rba_FeeFs1x_PAMap_detectCurrentWrPage
 * \return  Returns whether the detection was successful or faced a critical situation
 * \retval  E_OK : successfully detected
 * \retval  E_OK : detection failed, initialization not possible

 *********************************************************************
 */
LOCAL_INLINE Std_ReturnType rba_FeeFs1x_PAMap_detectCurrentWrPage(void)
{
    rba_FeeFs1x_PAMap_detectCurrentWrPage_tst jobData_st;
    Std_ReturnType retVal_en;

    jobData_st.state_en = rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_prepare_e;
    jobData_st.entry_b = TRUE;

    do
    {
        switch(jobData_st.state_en)
        {
            case rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_prepare_e:
            {
                rba_FeeFs1x_PAMap_detectCurrentWrPage_prepare(&jobData_st);
                break;
            }
            case rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_determLowerLimit_e:
            {
                rba_FeeFs1x_PAMap_detectCurrentWrPage_determineLowerLimit(&jobData_st);
                break;
            }
            case rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_areaBlChk_e:
            {
                rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckAreas(&jobData_st);
                break;
            }
            case rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_pageBlkChk_e:
            {
                rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckPagewise(&jobData_st);
                break;
            }
            default:
            {
                jobData_st.state_en = rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_error_e;
                break;
            }
        }

    }while
        (
                (jobData_st.state_en != rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e) &&
                (jobData_st.state_en !=  rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_error_e)
        );
    /* Evaluation of while loop: The function is called only during init phase where operations are in synchronous mode.
     * The while loop is required to go to different states. It has been checked that all paths will lead to exiting of
     * the state machine and is not stuck infinitely */

    if(jobData_st.state_en == rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e)
    {
        retVal_en = E_OK;
    }
    else
    {
        retVal_en = E_NOT_OK;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_detectCurrentWrPage_prepare
 * Statefunction of rba_FeeFs1x_PAMap_detectCurrentWrPage
 * prepares the contents of the statemachine struct required for the detection based on the sector states
 * \param   fsm_pst : statemachine variable
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_prepare(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst* fsm_pst)
{

    uint8 activeSectorIdx_u8;
    uint32 sectorStartPage_u32;
    rba_FeeFs1x_Sector_SectorState_ten stateOfActSect_en;

    /* Detect the state of the active sector*/
    activeSectorIdx_u8 = rba_FeeFs1x_Sector_getActiveSector();

    /*If the active sector is detected to be the amount of sectors(OOB to the sector array),
     *  all sectors are full (robust sector overflow)*/

    if(activeSectorIdx_u8 != RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE)
    {
        stateOfActSect_en = rba_FeeFs1x_Sector_getState(activeSectorIdx_u8);

        switch (stateOfActSect_en)
        {
            case rba_FeeFs1x_Sector_SectorState_ERASED_e:
            case rba_FeeFs1x_Sector_SectorState_USED_e:
            case rba_FeeFs1x_Sector_SectorState_FULL_e:
            {
                /*Execute the detection of the current write address*/

                fsm_pst->chunkSize_u32 = RBA_FEEFS1X_PAMAP_DETECTWRPAGE_CHUNKSIZE;

                // jobData_pst->currentChunkPage_u32 doesn't need initialization; gets inited in areaBlkChk

                fsm_pst->upperLimit_u32 = rba_FeeFs1x_Sector_getLogEndPage(activeSectorIdx_u8);
                fsm_pst->activeSectorIdx_u8 = activeSectorIdx_u8;

                FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_determLowerLimit_e);

                break;
            }
            case rba_FeeFs1x_Sector_SectorState_UNDEF_e:
            case rba_FeeFs1x_Sector_SectorState_R2E_e:
            default:
            {
                /**/

                sectorStartPage_u32 = rba_FeeFs1x_Sector_getLogStartPage(activeSectorIdx_u8);

                rba_FeeFs1x_PAMap_data_st.currWrPage_u32 = sectorStartPage_u32;
                FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e);
                break;
            }
        }
    }
    else
    {
        // robust sector overflow, set the current write page to the page behind the latest defined sector and exit
        rba_FeeFs1x_PAMap_data_st.currWrPage_u32 = RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE * RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR;
        FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e);
    }

}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_detectCurrentWrPage_determineLowerLimit
 * Statefunction of rba_FeeFs1x_PAMap_detectCurrentWrPage
 * Based on the flash device capabilities, the start marker is supposed to be considered or not.
 * If no start marker is available, the sector start is taken as lower limit
 * \param   fsm_pst : statemachine variable
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_determineLowerLimit(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst* fsm_pst)
{
    uint32 lowerLimit_u32;

    if(rba_FeeFs1x_Prv_isBlankChkRequired())
    {
        // On differential flash devices, blank areas can lead to ghost blocks. Thus, the start marker needs to be ignored
        lowerLimit_u32 = rba_FeeFs1x_Sector_getLogStartPage(fsm_pst->activeSectorIdx_u8);
        fsm_pst->lowerLimit_u32 = lowerLimit_u32;
        FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_areaBlChk_e);
    }
    else
    {
        /* On flashes without speaking blankcheck ( comparison against erase pattern),
         * the usage of start marker stops the rba_FeeFs1x to write into an overlapping block
         * with data matching the erase pattern and thus should be used as lower limit*/

        //try to access the start marker
        if(rba_FeeFs1x_Sector_getStartMarkerContentAsPageNo(fsm_pst->activeSectorIdx_u8, &lowerLimit_u32) == E_OK)
        {
            // start marker is available --> take the content as lower limit
            fsm_pst->lowerLimit_u32 = lowerLimit_u32;
            FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_areaBlChk_e);
        }
        else
        {
            // If the start marker is not present / accessible use the logical start page as lower limit
            lowerLimit_u32 = rba_FeeFs1x_Sector_getLogStartPage(fsm_pst->activeSectorIdx_u8);
            fsm_pst->lowerLimit_u32 = lowerLimit_u32;
            FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_areaBlChk_e);
        }
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckAreas
 * Statefunction of rba_FeeFs1x_PAMap_detectCurrentWrPage
 * Executes blankchecks for determining the area with the first programmed data within the detected limits
 * Switches to pagewise blankcheck state as soon as one area in backwards scan is not completely blank
 * \param   fsm_pst : statemachine variable
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckAreas(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst* fsm_pst)
{
    /*The incrementation by 1 is required to also capture the last page*/
    fsm_pst->currentChunkPage_u32 = fsm_pst->upperLimit_u32 + 1u;

    /* Post-Test loop preventing an underflow of the unsigned parameters:
     * - first iteration covers chunksize pages including the upper limit
     * - following iterations step chunkwise backwards until the last chunk is smaller than the chunksize
     * - This lowest chunk then separately gets processed
     * As soon as one area contains a non-blank page, the function exits
     * */
    do
    {
        fsm_pst->currentChunkPage_u32 -= fsm_pst->chunkSize_u32;

        fsm_pst->areaBlank_b = rba_FeeFs1x_PAMap_detectCurrentWrPage_execBlankcheckArea(fsm_pst->currentChunkPage_u32 , fsm_pst->chunkSize_u32);

    }while((fsm_pst->currentChunkPage_u32 >= (fsm_pst->lowerLimit_u32 + fsm_pst->chunkSize_u32)) && (fsm_pst->areaBlank_b) );
    /* Evaluation of while loop: The while loop is required to scan the sector is small steps (step size = RBA_FEEFS1X_PAMAP_DETECTWRPAGE_CHUNKSIZE)
     * The while loop exits when either the complete sector is scanned or when a non blank area is found. */

    /*Remaining area after latest chunksize
     * - only if all areas were blank until this one and
     * - the area wasn't exactly splittable in chunksizes (due to startpage usage as lower limit)*/
    if((fsm_pst->currentChunkPage_u32 != fsm_pst->lowerLimit_u32) && (fsm_pst->areaBlank_b))
    {
        fsm_pst->chunkSize_u32 = fsm_pst->currentChunkPage_u32 - fsm_pst->lowerLimit_u32;
        fsm_pst->currentChunkPage_u32 = fsm_pst->lowerLimit_u32;

        fsm_pst->areaBlank_b = rba_FeeFs1x_PAMap_detectCurrentWrPage_execBlankcheckArea(fsm_pst->currentChunkPage_u32 , fsm_pst->chunkSize_u32);
    }

    /*Either all pages within the boundaries were blank or an unblank page was detected*/
    if(fsm_pst->areaBlank_b)
    {
        /*lower boundary reached and all was blank*/
        rba_FeeFs1x_PAMap_data_st.currWrPage_u32 = fsm_pst->lowerLimit_u32;
        FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e);
    }
    else
    {
        /*If the loop exited with any unblank area or the latest chunk contained an unblank page, execute a pagewise blankcheck*/
        /*unblank area was found --> switch to pagewise blankcheck*/
        FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_pageBlkChk_e);
    }
}

/**
 *********************************************************************
 *rba_FeeFs1x_PAMap_detectCurrentWrPage_execblankcheckArea
 * \param   startPage_u32: startpage of the area to be blankchecked
 * \param   chunkSize_u32 : size of the area
 * \return  rba_FeeFs1x_PAMap_BlkChkRetVal_ten
 * \retval  rba_FeeFs1x_PAMap_BlkChkRetVal_unblank_en : At least one unblank page was found in the area
 * \retval  rba_FeeFs1x_PAMap_BlkChkRetVal_blank_en   : All pages of the area are unprogrammed
 * \retval  rba_FeeFs1x_PAMap_BlkChkRetVal_error_en   : An internal error occured
 * Preconditions:
 *  - Medium is required to be in POLLING MODE --> ONLY ALLOWED DURING FEE_INIT
 *  - startPage_u32 and (startPage_u32 + chunkSize_u32) have to be in the same sector
 * The statefunction synchronously executes an area-blankcheck
 *********************************************************************
 */
static boolean rba_FeeFs1x_PAMap_detectCurrentWrPage_execBlankcheckArea(uint32 startPage_u32 , uint32 chunkSize_u32)
{
    rba_FeeFs1x_PASrv_BlankCheckResult_ten blankcheckresult_en;
    boolean retVal_en;

    rba_FeeFs1x_PASrv_blankcheckArea(startPage_u32, chunkSize_u32);
    blankcheckresult_en = rba_FeeFs1x_PASrv_blankcheckAreaDo();

    switch(blankcheckresult_en)
    {
        case rba_FeeFs1x_PASrv_BlankCheckResult_fullyProgrammed_e:
        case rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e:
        {
            /*As soon as an area contains data, the statemachine shall switch to a pagewise blankcheck*/
            retVal_en = FALSE;
            break;
        }
        case rba_FeeFs1x_PASrv_BlankCheckResult_blank_e:
        {
            retVal_en = TRUE;
            break;
        }

        /* The function rba_FeeFs1x_PASrv_blankcheckAreaDo() should not return _pending, as this is called only during
         * init phase, where the Fee_Medium operates in synchronous mode and calls Fls_MainFunction till the requested
         * operation is completed. */
        case rba_FeeFs1x_PASrv_BlankCheckResult_pending_e:
        default:
        {
            /*Leaving the loop while still pending also is an error*/
            retVal_en = FALSE;
            break;
        }
    }


    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckPagewise
 * Statefunction of rba_FeeFs1x_PAMap_detectCurrentWrPage
 * The first backwards area with unblank pages gets scanned backwards pagewise
 * to detect the actual latest programmed page
 * \param   fsm_pst : statemachine variable
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckPagewise(rba_FeeFs1x_PAMap_detectCurrentWrPage_tst * fsm_pst)
{
    uint32 currentPage_u32;
    rba_FeeFs1x_PASrv_BlankCheckResult_ten retValBChkDo_en;
    boolean exitLoop_b = FALSE;

    for
    (
            currentPage_u32 = fsm_pst->currentChunkPage_u32 + fsm_pst->chunkSize_u32 - 1u;
            (currentPage_u32 >= fsm_pst->currentChunkPage_u32) && (!exitLoop_b);
            currentPage_u32--
    )
    {
        rba_FeeFs1x_PASrv_blankcheckPage(currentPage_u32);
        retValBChkDo_en = rba_FeeFs1x_PASrv_blankcheckPageDo();

        switch(retValBChkDo_en)
        {
            case rba_FeeFs1x_PASrv_BlankCheckResult_fullyProgrammed_e:
            case rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e:
            {
                /*If the checked page is not blank, take the page after that as current write page*/
                rba_FeeFs1x_PAMap_data_st.currWrPage_u32 = currentPage_u32 + 1u;
                FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e);
                exitLoop_b = TRUE;
                break;
            }
            case rba_FeeFs1x_PASrv_BlankCheckResult_blank_e:
            {
                if(currentPage_u32 == fsm_pst->currentChunkPage_u32)
                {
                    // last page was also rechecked for blank and was found to be blank. to avoid underflow exit the loop
                    exitLoop_b = TRUE;
                    /* Although the area blankcheck detected an unblank page, the pagewise check only found blank pages -->
                     * This means that the blank check for the page had toggled. better to set the write address to one page later. */
                    rba_FeeFs1x_PAMap_data_st.currWrPage_u32 = currentPage_u32 + 1u;
                    FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_final_e);
                }
                else
                {
                    /* else stay in the loop and blank check the next page */
                }

                break;
            }

            /* The function rba_FeeFs1x_PASrv_blankcheckAreaDo() should not return _pending, as this is called only during
             * init phase, where the Fee_Medium operates in synchronous mode and calls Fls_MainFunction till the requested
             * operation is completed. */
            case rba_FeeFs1x_PASrv_BlankCheckResult_pending_e:
            default:
            {
                /*Unknown return type --> error*/
                FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_detectCurrrentWrPageStm_error_e);
                exitLoop_b = TRUE;
                break;
            }
        }
    }
}


/**
 *********************************************************************
 * fill the rest of the sector with predefined pattern
 * \param   void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAMap_fillSectorEnd(void)
{
    uint8  logSectIdx_u8;
    uint32 logEndWritePage_u32;
    uint32 currWrPage_u32;

    /* calculate the logical end page of the actual sector */
    logSectIdx_u8 = 0;
    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();
    (void) rba_FeeFs1x_PAMap_getLogIdxByPage(currWrPage_u32, &logSectIdx_u8);
    logEndWritePage_u32 = rba_FeeFs1x_Sector_getLogEndPage(logSectIdx_u8);

    rba_FeeFs1x_PAMap_fillSectorEnd_st.logStartWritePage_u32 = currWrPage_u32;
    rba_FeeFs1x_PAMap_fillSectorEnd_st.logEndWritePage_u32 = logEndWritePage_u32;
    rba_FeeFs1x_PAMap_fillSectorEnd_st.state_en = rba_FeeFs1x_PAMap_fillSectorEnd_stm_write_e;
    rba_FeeFs1x_PAMap_fillSectorEnd_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * do function for filling the rest of a sector with a predefined pattern
 * \param   void
 * \return  result of the execution
 * \retval  rba_FeeFs1x_JobOK_e: job finished succesfully
 * \retval  rba_FeeFs1x_Pending_e: job is still pending
 * \retval  rba_FeeFs1x_ErrorInternal_e: an internal error occured
 * \retval  rba_FeeFs1x_ErrorExternal_e: external write functionality failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_fillSectorEndDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_PAMap_fillSectorEnd_st.state_en)
    {
        case rba_FeeFs1x_PAMap_fillSectorEnd_stm_write_e:
        {
            retVal_en = rba_FeeFs1x_PAMap_fillSectorEndDo_write(&rba_FeeFs1x_PAMap_fillSectorEnd_st);
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    // on exiting the job, reset the statemachine data
    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_PAMap_fillSectorEnd_st.state_en = rba_FeeFs1x_PAMap_fillSectorEnd_stm_idle_e;
    }
    return  retVal_en;
}

/**
 *********************************************************************
 * write function for filling the rest of a sector with a predefined pattern
 * \param   void
 * \return  result of the execution
 * \retval  rba_FeeFs1x_JobOK_e: job finished succesfully
 * \retval  rba_FeeFs1x_Pending_e: job is still pending
 * \retval  rba_FeeFs1x_ErrorInternal_e: an error occured
 * \retval  rba_FeeFs1x_ErrorExternal_e: external write functionality failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_fillSectorEndDo_write(rba_FeeFs1x_PAMap_fillSectorEnd_tst* fsm_pst)
{
    uint32 startWriteAddress_u32;
    MemIf_JobResultType mediumWriteDoRetVal_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        startWriteAddress_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->logStartWritePage_u32);
        /* it does not matter if write was successful or failed, set the write page to next page. */
        rba_FeeFs1x_PAMap_setCurrWrPage(fsm_pst->logStartWritePage_u32 + 1u);

        Fee_Prv_MediumWrite(
                Fee_Rb_DeviceName,
                startWriteAddress_u32,
                0u,
                (void const * )rba_FeeFs1x_PAMap_fillSectorEnd_FillPattern,
                RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE,
                FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COPY_COMP_E);
    }

    mediumWriteDoRetVal_en = Fee_Prv_MediumWriteDo(Fee_Rb_DeviceName);

    switch(mediumWriteDoRetVal_en)
    {
        case MEMIF_JOB_OK:
        {
            /* write was successful, so increase the start write page and set the new current write page */
            fsm_pst->logStartWritePage_u32 ++;
            if(fsm_pst->logStartWritePage_u32 <= fsm_pst->logEndWritePage_u32)
            {
                /* write next logical page, so reenter the state and stay pending */
                FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_fillSectorEnd_stm_write_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                /* sector filling successfully finished, so leave and return Job OK */
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            break;
        }
        case MEMIF_JOB_PENDING:
        {
            /* keep state and return pending */
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        default:
        {
            /* switch state to idle */
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
            break;
        }
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_getLogIdxByPage
 * \param   page_u32 : page to be checked for it's logical sector index
 * \param   logSectIdx_pu8 : pointer for returning the sector index if the page was inbound
 * \return  parameters accepted?
 * \retval  E_OK : job accepted, accessing logSectIdx allowed
 * \retval  E_NOT_OK : job not accepted, logSectIdx not altered
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_PAMap_getLogIdxByPage(uint32 page_u32 , uint8 * logSectIdx_pu8)
{
    const uint32 pagesPerSect_u32 = RBA_FEEFS1X_SECTOR_LOG_SIZE / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
    Std_ReturnType retVal_en;

    if(page_u32 < (pagesPerSect_u32 * RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE))
    {
        // page is inbound of the defined sector range --> return the log sector index
        *logSectIdx_pu8 = rba_FeeFs1x_PAMap_getLogIdxByPagePrechecked(page_u32);

        retVal_en = E_OK;
    }
    else
    {
        // requested page is outside of the defined sector range
        retVal_en = E_NOT_OK;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_getAddressByPage
 * \attention: Function should be called only with valid parameter value!
 * \param   page_u32 : page to be translated into an address
 * \return  the address of the page
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_PAMap_getAddressByPage(uint32 page_u32)
{
    uint8 logSectIdx_u8;
    uint32 logStartAddress_u32;

    logSectIdx_u8 = rba_FeeFs1x_PAMap_getLogIdxByPagePrechecked(page_u32);

    logStartAddress_u32 =  rba_FeeFs1x_Sector_getLogStartAddressPrechecked(logSectIdx_u8);

    return ((page_u32 % RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR) * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE) + logStartAddress_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries
 * Checks how many bytes are allowed to be read in one chunk without leaving a sector
 * \param   startPage_u32 : page the first byte is located in
 * \param   ofs_u32 : byte offset of the first byte within the startpage
 * \param   requestedNBytes_u32 : amount of bytes the requester overall wants to process
 * \param   allowedBytes_pu32 : returned value how many bytes are allowed to be read in one chunk
 * \return  job accepted?
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(uint32 startPage_u32, uint32 ofs_u32 ,
        uint32 requestedNBytes_u32)
{
    uint32 requestedNPages_u32, requestedLastPage_u32;
    uint8  logSectIdxStartPage_u8 = 0xFFu;
    uint32 logSectEndPage_u32;
    uint32 totalNBytes_u32;
    uint32 allowedBytes_u32;

    (void) rba_FeeFs1x_PAMap_getLogIdxByPage(startPage_u32, &logSectIdxStartPage_u8);

    logSectEndPage_u32 = rba_FeeFs1x_Sector_getLogEndPage(logSectIdxStartPage_u8);

    totalNBytes_u32 = ofs_u32 + requestedNBytes_u32;
    if(RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE >= totalNBytes_u32)
    {
        // total number of bytes fits into the startpage
        requestedLastPage_u32 = startPage_u32;
    }
    else
    {
        // total number of bytes exceeds the start page
        requestedNPages_u32 = (totalNBytes_u32 - 1u) / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
        requestedLastPage_u32 = startPage_u32 + requestedNPages_u32;
    }

    if(requestedLastPage_u32 > logSectEndPage_u32)
    {
        // limit the request to the bytes within the start page
        // plus the amount of remaining pages within this sector
        // (logEnd - start) doesn't get incremented by 1, because the start page is handled separately
        allowedBytes_u32 = (RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE - ofs_u32) +
                ( (logSectEndPage_u32 - startPage_u32) * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE );
    }
    else
    {
        allowedBytes_u32 = requestedNBytes_u32;
    }

    return  allowedBytes_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_init
 * \return  init successful?
 * \retval  E_OK : successful
 * \retval  E_NOT_OK : not successful, lock down the Fee
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_PAMap_init(void)
{
    Std_ReturnType retVal_en;

    rba_FeeFs1x_PAMap_cancel();

    rba_FeeFs1x_PAMap_data_st.currWrPage_u32 = 0u;

    retVal_en = rba_FeeFs1x_PAMap_detectCurrentWrPage();

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_cancel
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAMap_cancel(void)
{
    rba_FeeFs1x_PAMap_fillSectorEnd_st.state_en = rba_FeeFs1x_PAMap_fillSectorEnd_stm_idle_e;

    rba_FeeFs1x_PAMap_prepareSectorStates_data_st.state_en = rba_FeeFs1x_PAMap_prepareSectorStates_stm_idle_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_isPageSmallerWrAdr
 * compares the page against the current wr address
 * \param   page_u32 : page to be compared
 * \return  comparison result
 * \retval  TRUE  : Yes, the page is smaller
 * \retval  FALSE : no, the page is not smaller (--> greater or equal)
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_PAMap_isPageSmallerWrAdr(uint32 page_u32)
{
    return  page_u32 < rba_FeeFs1x_PAMap_data_st.currWrPage_u32;
}

/**
 *********************************************************************
 * all variables, state variables containing page information are reduced by given number of pages
 * !!! No parallel execution with any unit internal function using page information !!!
 * \param   amountOfPages_u32 : number with which all page relevant variables shall be reduced
 * \return  check, if reduction of number of pages could be done
 * \retval  E_OK    : reduction of the number of pages was successful
 * \retval  E_NOT_OK: reduction of the number of pages would lead to a negative page number
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_PAMap_reduceOwnPages(uint32 amountOfPages_u32)
{
    uint32 currWrPage_u32;
    Std_ReturnType prepareSectorStatesRetVal_en;
    Std_ReturnType fillSectorEndRetVal_en;
    Std_ReturnType currWrPageRetVal_en;
    Std_ReturnType retVal_en;


    /* reduce the current write page
       and the currently requested page operation storages by the given amount of pages */

    /* update the current write page */
    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();
    if (currWrPage_u32 >= amountOfPages_u32)
    {
        // If the page number is greater or equal than the requested reduce-amount, exec the reduce
        currWrPage_u32 -= amountOfPages_u32;
        rba_FeeFs1x_PAMap_setCurrWrPage(currWrPage_u32);
        currWrPageRetVal_en = E_OK;
    }
    else
    {
        /* current write page points to reorganized sector */
        currWrPageRetVal_en = E_NOT_OK;
    }

    /* only check and update rba_FeeFs1x_PAMap_prepareSectorStates_data_st, if state machine is active */
    if(rba_FeeFs1x_PAMap_prepareSectorStates_stm_idle_e == rba_FeeFs1x_PAMap_prepareSectorStates_data_st.state_en)
    {
        // there is no switch active sector currently ongoing, no update of page requried.
        prepareSectorStatesRetVal_en = E_OK;
    }
    else
    {
        if(rba_FeeFs1x_PAMap_prepareSectorStates_data_st.nextBlockStartPage_u32 >= amountOfPages_u32)
        {
            rba_FeeFs1x_PAMap_prepareSectorStates_data_st.nextBlockStartPage_u32 -= amountOfPages_u32;
            prepareSectorStatesRetVal_en = E_OK;
        }
        else
        {
            // underflow possible
            prepareSectorStatesRetVal_en = E_NOT_OK;
        }
    }

    /* only check and update rba_FeeFs1x_PAMap_fillSectorEnd_st, if state machine is active */
    if(rba_FeeFs1x_PAMap_fillSectorEnd_stm_idle_e == rba_FeeFs1x_PAMap_fillSectorEnd_st.state_en)
    {
        // there is no fill sector end currently ongoing, no update of page requried.
        fillSectorEndRetVal_en = E_OK;
    }
    else
    {
        if (rba_FeeFs1x_PAMap_fillSectorEnd_st.logEndWritePage_u32 >= amountOfPages_u32)
        {
            rba_FeeFs1x_PAMap_fillSectorEnd_st.logEndWritePage_u32 -= amountOfPages_u32;
            fillSectorEndRetVal_en = E_OK;
        }
        else
        {
            // underflow possible
            fillSectorEndRetVal_en = E_NOT_OK;
        }
    }

    /* check successful update of all required variables */
    if( (E_OK == currWrPageRetVal_en)           &&
            (E_OK == prepareSectorStatesRetVal_en)   &&
            (E_OK == fillSectorEndRetVal_en)     )
    {
        retVal_en = E_OK;
    }
    else
    {
        retVal_en = E_NOT_OK;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_prepareSectorStates
 * job checks the sector states at an overlap condition:
 *   writes a missing full marker and updates the current sector's state up to USED
 *   internally triggers an erase for the current sector if required
 * \param   nextBlockStartPage_u32: start page of the next block
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAMap_prepareSectorStates(uint32 nextBlockStartPage_u32)
{
    uint8  logSectIdxNextBlkStartPage_u8;
    Std_ReturnType getLogSectIdxNextBlock_retVal_en;

    getLogSectIdxNextBlock_retVal_en =
            rba_FeeFs1x_PAMap_getLogIdxByPage(nextBlockStartPage_u32, &logSectIdxNextBlkStartPage_u8);

    // check, if logSectIdxNextBlkStartPage_u8 is not exceeding available sectors is done in rba_FeeFs1x_PAMap_getLogIdxByPage()
    if(E_OK == getLogSectIdxNextBlock_retVal_en)
    {
        // set the initial state, first the FULL marker has to be written to the active sector
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.state_en =
                rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrFull_e;

        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.nextBlockStartPage_u32 = nextBlockStartPage_u32;
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.logSectIdxNextBlock_u8 = logSectIdxNextBlkStartPage_u8;
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.isReerasing_b = FALSE;

        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.entry_b = TRUE;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_prepareSectorStatesOnCurrWrPage
 * job checks the sector states at an overlap condition:
 *   writes a missing full marker and updates the current sector's state up to USED
 *   internally triggers an erase for the current sector if required
 * \return  job accepted?
 * \retval  E_OK  : job accepted
 * \retval  E_NOT_OK : job rejected
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType rba_FeeFs1x_PAMap_prepareSectorStatesOnCurrWrPage(void)
{
    Std_ReturnType retVal_en;
    uint32 currWrPage_u32;
    uint8 logSectIdxCurrWrPg_u8;
    uint32 logStartPage_u32;

    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();
    if(E_OK == rba_FeeFs1x_PAMap_getLogIdxByPage(currWrPage_u32, &(logSectIdxCurrWrPg_u8)))
    {
        // trigger the marker write
        logStartPage_u32 = rba_FeeFs1x_Sector_getLogStartPage(logSectIdxCurrWrPg_u8);

        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.nextBlockStartPage_u32 = logStartPage_u32;
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.logSectIdxNextBlock_u8 = logSectIdxCurrWrPg_u8;
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.isReerasing_b = FALSE;

        //sector misses it, thus write the full marker
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.state_en =
                rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrFull_e;
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.entry_b = TRUE;

        retVal_en = E_OK;
    }
    else
    {
        //getting the logical index of the current write page failed
        retVal_en = E_NOT_OK;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_prepareSectorStatesDo
 * do function of prepareSectorStates job
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e  : job finished successfully, current sector is usable
 * \retval  rba_FeeFs1x_ErrorInternal_e  : error caused Fs1x internally
 * \retval  rba_FeeFs1x_ErrorExternal_e  : error using Fls
 * \retval  rba_FeeFs1x_Pending_e  : call again in next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch(rba_FeeFs1x_PAMap_prepareSectorStates_data_st.state_en)
    {
        case rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrFull_e:
        {
            retVal_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrFull(&rba_FeeFs1x_PAMap_prepareSectorStates_data_st);
        }break;
        case rba_FeeFs1x_PAMap_prepareSectorStates_stm_erase_e:
        {
            retVal_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo_erase(&rba_FeeFs1x_PAMap_prepareSectorStates_data_st);
        }break;
        case rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrUsed_e:
        {
            retVal_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrUsed(&rba_FeeFs1x_PAMap_prepareSectorStates_data_st);
        }break;
        case rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrStart_e:
        {
            retVal_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrStart(&rba_FeeFs1x_PAMap_prepareSectorStates_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    // on job exit, reset the statemachine data
    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_PAMap_prepareSectorStates_data_st.state_en = rba_FeeFs1x_PAMap_prepareSectorStates_stm_idle_e;
    }
    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrFull
 * statefunction of prepareSectorStatesDo
 * \param   fsm_pst : pointer to statemachine data
 *
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e  : job finished successfully, current sector is usable
 * \retval  rba_FeeFs1x_ErrorInternal_e  : error caused Fs1x internally
 * \retval  rba_FeeFs1x_Pending_e  : call again in next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrFull(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValMark_en;
    rba_FeeFs1x_Sector_SectorState_ten sectorStatePrevSect_en;

    if(FEE_PRV_LIBENTRY)
    {
        // check, if full marker has to be written
        if(fsm_pst->logSectIdxNextBlock_u8 > 0u)
        {
            // check the state of the previous sector
            sectorStatePrevSect_en = rba_FeeFs1x_Sector_getState(fsm_pst->logSectIdxNextBlock_u8 - 1u);

            if(rba_FeeFs1x_Sector_SectorState_USED_e == sectorStatePrevSect_en)
            {
                // trigger the marker write
                rba_FeeFs1x_Sector_markFull(fsm_pst->logSectIdxNextBlock_u8 - 1u);
            }
            else
            {
                retValMark_en = rba_FeeFs1x_JobOK_e;
            }
        }
        else
        {
            // lesser sector index not present, no writing of full marker required
            retValMark_en = rba_FeeFs1x_JobOK_e;
        }
    }

    // Cyclic call the Do function
    retValMark_en = rba_FeeFs1x_Sector_markSectorDo();

    //evaluate the result
    switch(retValMark_en)
    {
        // writing the FULL marker to flash failed, but state in RAM is valid
        // lost FULL marker in flash is handled during startup
        // operation may be continued
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_JobOK_e:
        {
            rba_FeeFs1x_Sector_SectorState_ten sectorState_en;
            // switch to next state

            sectorState_en = rba_FeeFs1x_Sector_getState(fsm_pst->logSectIdxNextBlock_u8);
            switch(sectorState_en)
            {
                case rba_FeeFs1x_Sector_SectorState_ERASED_e:
                {
                    // write the used marker
                    FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrUsed_e);
                    retVal_en = rba_FeeFs1x_Pending_e;
                }break;
                case rba_FeeFs1x_Sector_SectorState_UNDEF_e:
                case rba_FeeFs1x_Sector_SectorState_R2E_e:
                {
                    // erase the sector
                    FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_prepareSectorStates_stm_erase_e);
                    retVal_en = rba_FeeFs1x_Pending_e;
                }break;
                case rba_FeeFs1x_Sector_SectorState_CLONE_e:
                case rba_FeeFs1x_Sector_SectorState_USED_e:
                {
                    retVal_en = rba_FeeFs1x_JobOK_e;
                }break;
                case rba_FeeFs1x_Sector_SectorState_FULL_e:
                default:
                {
                    // switching into already full sectors is forbidden
                    retVal_en = rba_FeeFs1x_ErrorInternal_e;
                }break;
            }
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = retValMark_en;
        }break;
        // case rba_FeeFs1x_ErrorExternal_e:
        // When writing the full marker fails, JobFailed, not ErrorExternal is returned
        //-> handle ErrorExternal as an unexpected return value
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_prepareSectorStatesDo_erase
 * statefunction of prepareSectorStatesDo
 * \param   fsm_pst : pointer to statemachine data
 *
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e  : job finished successfully, current sector is usable
 * \retval  rba_FeeFs1x_ErrorInternal_e  : error caused Fs1x internally
 * \retval  rba_FeeFs1x_Pending_e  : call again in next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_erase(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValErase_en;

    if(FEE_PRV_LIBENTRY)
    {
        // trigger the marker write
        rba_FeeFs1x_Sector_erase(fsm_pst->logSectIdxNextBlock_u8, fsm_pst->isReerasing_b);
    }

    // Cyclic call the Do function
    retValErase_en = rba_FeeFs1x_Sector_eraseDo();

    //evaluate the result
    switch(retValErase_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // marker write finished successfully; switch state
            FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrUsed_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValErase_en;
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrUsed
 * statefunction of prepareSectorStatesDo
 * \param   fsm_pst : pointer to statemachine data
 *
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e  : job finished successfully, current sector is usable
 * \retval  rba_FeeFs1x_ErrorInternal_e  : error caused Fs1x internally
 * \retval  rba_FeeFs1x_Pending_e  : call again in next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrUsed(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValMark_en;

    if(FEE_PRV_LIBENTRY)
    {
        // trigger the marker write
        rba_FeeFs1x_Sector_markUsed(fsm_pst->logSectIdxNextBlock_u8);
    }

    // Cyclic call the Do function
    retValMark_en = rba_FeeFs1x_Sector_markSectorDo();

    //evaluate the result
    switch(retValMark_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // marker write finished successfully; switch state
            FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_prepareSectorStates_stm_wrStart_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        {
            fsm_pst->isReerasing_b = TRUE;
            // if writing of the used marker fails, restart complete sector erase
            FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_prepareSectorStates_stm_erase_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValMark_en;
        }break;
        // case rba_FeeFs1x_JobFailed_e:
        // When writing the used marker fails, ErrorExternal, not JobFailed is returned
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
 * rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrStart
 * statefunction of prepareSectorStatesDo
 * \param   fsm_pst : pointer to statemachine data
 *
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e  : job finished successfully, current sector is usable
 * \retval  rba_FeeFs1x_ErrorInternal_e  : error caused Fs1x internally
 * \retval  rba_FeeFs1x_Pending_e  : call again in next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAMap_prepareSectorStatesDo_wrStart(rba_FeeFs1x_PAMap_prepareSectorStates_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValMark_en;

    if(FEE_PRV_LIBENTRY)
    {
        uint32 startAdr_u32;
        startAdr_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->nextBlockStartPage_u32);
        rba_FeeFs1x_Sector_markStart(fsm_pst->logSectIdxNextBlock_u8 , startAdr_u32);
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
            // all expected return values are mapped directly
            // --> on job OK, the sector states are prepared
            retVal_en = retValMark_en;
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // if writing of the start marker fails, restart complete sector erase
            fsm_pst->isReerasing_b = TRUE;
            FEE_PRV_LIBSC(rba_FeeFs1x_PAMap_prepareSectorStates_stm_erase_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        } break;
        // case rba_FeeFs1x_JobFailed_e:
        // When writing the start marker fails, ErrorExternal, not JobFailed is returned
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
 * rba_FeeFs1x_PAMap_setCurrWrPage
 * sets the current write page to a new value
 * \param   page_u32 : new currWrPage value
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAMap_setCurrWrPage(uint32 page_u32)
{
    rba_FeeFs1x_PAMap_data_st.currWrPage_u32 = page_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_getCurrWrPage
 * \return  returns the current write page
 *********************************************************************
 */
uint32 rba_FeeFs1x_PAMap_getCurrWrPage(void)
{
    return  rba_FeeFs1x_PAMap_data_st.currWrPage_u32;
}


/**
 *********************************************************************
 * rba_FeeFs1x_PAMap_getLastPageNrOfChunk
 * calculates the pagenumber of the last page of a chunk
 * \param   chunkStartPage_u32      : first page of the chunk
 * \param   offsetInStartPage_u8    : byte offset the first byte of the chunk has within the start page.
 *                                    ENSURE that it is smaller the pagesize
 * \param   nrBytesInChunk_u16      : amount of bytes processed in one chunk;
 *                                      to be added to the offset in start page to get the last page of the cunk
 *                                      ENSURE nrBytes >= (pagesize - offset)
 *                                      ENSURE nrBytes >  0
 * \return  pagenumber of the last page within the chunk
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_PAMap_getLastPageNrOfChunk(uint32 chunkStartPage_u32 , uint8 offsetInStartPage_u8 ,
        uint16 nrBytesInChunk_u16)
{
    uint32 nrPagesAfterStartPage_u32;
    uint8  nrBytesInStartPage_u8;
    uint32 chunkEndPage_u32;

    nrBytesInStartPage_u8 = RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE - offsetInStartPage_u8;


    nrPagesAfterStartPage_u32 = Fee_Prv_LibCeil((nrBytesInChunk_u16 - nrBytesInStartPage_u8), RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE) /
            RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;

    chunkEndPage_u32 = chunkStartPage_u32 + nrPagesAfterStartPage_u32;

    return chunkEndPage_u32;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

