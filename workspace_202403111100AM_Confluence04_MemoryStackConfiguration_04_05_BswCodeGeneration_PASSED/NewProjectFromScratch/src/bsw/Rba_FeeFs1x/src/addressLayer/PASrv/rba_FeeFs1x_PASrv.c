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
#include "rba_FeeFs1x_Prv_PASrv.h"
#include "rba_FeeFs1x_Prv_PASrvTypes.h"

#include "rba_FeeFs1x_Prv.h"
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "Fee_Prv_Lib.h"
#include "rba_MemLib.h"

#include "rba_FeeFs1x_Prv_PAWrite.h"
#include "rba_FeeFs1x_Prv_PAMap.h"
#include "rba_FeeFs1x_Prv_Sector.h"
#include "Fee_Prv_Medium.h"

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

static rba_FeeFs1x_PASrv_calcCRC32_data_tst rba_FeeFs1x_PASrv_calcCRC32_data_st;

static rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoData_tst rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoData_st;

static rba_FeeFs1x_PASrv_blankcheckPageDoData_tst rba_FeeFs1x_PASrv_blankcheckPageDoData_st;

static rba_FeeFs1x_PASrv_compare_data_tst rba_FeeFs1x_PASrv_compare_data_st;

static rba_FeeFs1x_PASrv_compareFls2Fls_data_tst rba_FeeFs1x_PASrv_compareFls2Fls_data_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_calcCRC32Do_limit(rba_FeeFs1x_PASrv_calcCRC32_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_calcCRC32Do_calc (rba_FeeFs1x_PASrv_calcCRC32_data_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareDo_limit  (rba_FeeFs1x_PASrv_compare_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareDo_compare(rba_FeeFs1x_PASrv_compare_data_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareFls2FlsDo_compare(rba_FeeFs1x_PASrv_compareFls2Fls_data_tst * fsm_pst);
/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */


/**
 *********************************************************************
 *rba_FeeFs1x_PASrv_getCurrWritePage
 * \return returns the current Write page(upper limit of programmed cells in the linear page layout)
 * The write page doesn't need to be directly usable; it also could point behint the highest sector
 * (robust sector overflow)
 *********************************************************************
 */
uint32 rba_FeeFs1x_PASrv_getCurrWritePage(void)
{
    return  rba_FeeFs1x_PAMap_getCurrWrPage();
}
/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_checkPageFullyProgrammed
 * Triggers a blankcheck on the last physical page of a virtual page -> fully programmed
 * \param   page_u32 : Page to be blankchecked
 * \return  void     : if the job is not accepted the Do function will return the error
 * \seealso rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PASrv_checkPageFullyProgrammed(uint32 page_u32)
{
    uint32 addressPageBlankCheck_u32;
    uint32 adrLastPhysPageOfLogPage_u32;

    const uint8 ofsLastPhysPageInLog_cu8 = ((RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE / RBA_FEEFS1X_PRV_CFG_PHY_PAGE_LEN) - 1u) * RBA_FEEFS1X_PRV_CFG_PHY_PAGE_LEN;

    if(rba_FeeFs1x_Prv_isBlankChkRequired())
    {
        addressPageBlankCheck_u32 = rba_FeeFs1x_PAMap_getAddressByPage(page_u32);

        // calculate the address to be blankchecked
        adrLastPhysPageOfLogPage_u32 = addressPageBlankCheck_u32 + ofsLastPhysPageInLog_cu8;

        // set the state to signal that the blankcheck is active
        rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoData_st.state_en =
                rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo_blankcheck_e;

        // trigger the blankcheck of the last physical
        Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName, adrLastPhysPageOfLogPage_u32, 0u, RBA_FEEFS1X_PRV_CFG_PHY_PAGE_LEN);
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo
 * \return  returns whether the virtual page is fully programmed (last physical page blankcheck)
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_fullyProgrammed_en:  physical page is programmed,
 *                                                                  virtual page is fully programmed
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_blank_e : physical page is blank,
 *                                                       virtual page is not fully programmed
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_pending_en: job ongoing, call again during the next cycle
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_error_en: internal error, job aborted
 * \seealso rba_FeeFs1x_PASrv_checkPageFullyProgrammed
 *********************************************************************
 */
rba_FeeFs1x_PASrv_BlankCheckResult_ten rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo(void)
{
    rba_FeeFs1x_PASrv_BlankCheckResult_ten retVal_en;

    if(rba_FeeFs1x_Prv_isBlankChkRequired())
    {
        switch(rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoData_st.state_en)
        {
            case rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo_blankcheck_e:
            {
                MemIf_JobResultType blankcheckRetVal_en;

                blankcheckRetVal_en = Fee_Prv_MediumBlankCheckDo(Fee_Rb_DeviceName);

                // EXIT check
                switch (blankcheckRetVal_en)
                {
                    case MEMIF_JOB_OK:
                    {
                        // physical page is blank --> only partially programmed page
                        retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_blank_e;

                        break;
                    }
                    case MEMIF_JOB_FAILED:
                    {
                        // Page is not blank --> fully programmed page
                        retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_fullyProgrammed_e;

                        break;
                    }
                    case MEMIF_JOB_PENDING:
                    {
                        // call the function again during the next mainfunction cycle
                        retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_pending_e;
                        break;
                    }

                    default:
                    {
                        // not expected return values --> error
                        retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_error_e;
                        break;
                    }
                }
                break;
            }
            default:
            {
                /*On RAM corruption or while the statemachine is in idle, exit with error*/
                retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_error_e;
                break;
            }
        }

        /*Reset the statemachine on exit (robustness)*/
        if(retVal_en != rba_FeeFs1x_PASrv_BlankCheckResult_pending_e)
        {
            rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoData_st.state_en = rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo_idle_e;

        }
    }
    else
    {
        retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_error_e;
    }

    return  retVal_en;
}



/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_blankcheckPage
 * Triggers a blankcheck on the whole virtual page -> page already started to be programmed
 * \param   page_u32 : Page to be blankchecked
 * \return  void     : if the job is rejected the Do function will retun an error
 * \seealso rba_FeeFs1x_PASrv_blankcheckPageDo
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PASrv_blankcheckPage(uint32 page_u32)
{
    uint32 adrPageBlankCheck_u32;

    adrPageBlankCheck_u32 = rba_FeeFs1x_PAMap_getAddressByPage(page_u32);
    rba_FeeFs1x_PASrv_blankcheckPageDoData_st.state_en = rba_FeeFs1x_PASrv_blankcheckPageDo_blankcheck_e;
    Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName, adrPageBlankCheck_u32, 0u, RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_blankcheckPageDo
 * \return  returns the blankness state of the whole virtual page
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e: virtual page contains unblank data
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_blank_e : virtual page is blank
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_pending_en: job ongoing, call again during the next cycle
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_error_en: internal error, job aborted
 * \seealso rba_FeeFs1x_PASrv_checkPageFullyProgrammed
 *********************************************************************
 */
rba_FeeFs1x_PASrv_BlankCheckResult_ten rba_FeeFs1x_PASrv_blankcheckPageDo(void)
{
    rba_FeeFs1x_PASrv_BlankCheckResult_ten retVal_en;
    switch(rba_FeeFs1x_PASrv_blankcheckPageDoData_st.state_en)
    {
        case rba_FeeFs1x_PASrv_blankcheckPageDo_blankcheck_e:
        {
            MemIf_JobResultType blankcheckRetVal_en;

            blankcheckRetVal_en = Fee_Prv_MediumBlankCheckDo(Fee_Rb_DeviceName);

            // EXIT check
            switch (blankcheckRetVal_en)
            {
                case MEMIF_JOB_OK:
                {
                    // page is blank
                    retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_blank_e;

                    break;
                }
                case MEMIF_JOB_FAILED:
                {
                    // Page is not (completely) blank
                    // switch the state to a blankcheck of the last physical page of the logical page

                    retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e;
                    break;
                }
                case MEMIF_JOB_PENDING:
                {
                    // call the function again during the next mainfunction cycle
                    retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_pending_e;
                    break;
                }

                default:
                {
                    // not expected return values --> error
                    retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_error_e;
                    break;
                }
            }
        }break;

        default:
        {
            retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_error_e;
        }break;
    }

    if(rba_FeeFs1x_PASrv_BlankCheckResult_pending_e != retVal_en)
    {
        rba_FeeFs1x_PASrv_blankcheckPageDoData_st.state_en = rba_FeeFs1x_PASrv_blankcheckPageDo_idle_e;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_blankcheckArea
 * it is insured by the call process that the StartPage and EndPage are within the same sector
 * \param   startPage_u32 : firt page of the are to be blankchecked
 * \param   noOfPages_u32 : amount of pages to be blanchecked (last page = startP + noOfPages - 1u)
 *                          The last page is supposed to be in the same sector as the start page
 * \return  void
 * \seealso rba_FeeFs1x_PASrv_blankcheckAreaDo
 *          rba_FeeFs1x_PAMap_detectCurrentWrPage_blankcheckAreas
 *********************************************************************
 */
void rba_FeeFs1x_PASrv_blankcheckArea(uint32 startPage_u32, uint32 noOfPages_u32)
{
    uint32 startAddressBlankCheck_u32 , blankcheckLength_u32;

    /*translate the start page into an address*/
    startAddressBlankCheck_u32 = rba_FeeFs1x_PAMap_getAddressByPage(startPage_u32);

    /*Only execute the medium blankcheck if none of the upper getters returned an error
     * and the start and end page are within the same sector*/
    blankcheckLength_u32 = noOfPages_u32 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;

    /*Trigger the Medium BlankCheck with the address of the page and the amount of requested bytes for this area*/
    Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName, startAddressBlankCheck_u32, 0u, blankcheckLength_u32);
}

/**
 *********************************************************************
 *rba_FeeFs1x_PASrv_blankcheckAreaDo
 * \return  Returns whether the area is blank or contains unblank pages
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_blank_en : all pages are unprogrammed
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_en : some pages are programmed
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_pending_en : job needs further calls
 * \retval  rba_FeeFs1x_PASrv_BlankCheckResult_error_en : internal error, job aborted
 * \seealso rba_FeeFs1x_PASrv_blankcheckArea
 *********************************************************************
 */
rba_FeeFs1x_PASrv_BlankCheckResult_ten rba_FeeFs1x_PASrv_blankcheckAreaDo(void)
{
    MemIf_JobResultType retValBlkChk_en;
    rba_FeeFs1x_PASrv_BlankCheckResult_ten retVal_en;


    retValBlkChk_en = Fee_Prv_MediumBlankCheckDo(Fee_Rb_DeviceName);

    switch(retValBlkChk_en)
    {
        case MEMIF_JOB_OK:
        {
            // Area is blank
            retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_blank_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            // Area is not blank
            retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e;
            break;
        }
        case MEMIF_JOB_PENDING:
        {
            // operation ongoing
            retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_pending_e;
            break;
        }
        default:
        {
            // internal error
            retVal_en = rba_FeeFs1x_PASrv_BlankCheckResult_error_e;
            break;
        }
    }

    return  retVal_en;
}

/**
 * This function calculates the data crc over the dflash copy. the function only
 * accepts requests that are aligned at the page boundary
 */

/**
 *********************************************************************
 *
 * \param   startPage_u32   : first page containing block data
 * \param   len_u16         : amount of bytes the CRC has to be calculated on
 * \param   crc_pu32        : pointer to the variable the CRC shall be stored in.
 *                            The initial value has to be stored here before the call!
 * \param   startOfs_u8     : offset[bytes] to the start page from where first user data is stored.
 * \return  void            : in case the job is rejected the error will be returned from the Do function
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PASrv_calcCRC32(uint32 startPage_u32, uint16 len_u16, uint32* crc_pu32, uint8 startOfs_u8)
{
    uint32 endPage_u32;

    // For a length == VIRTUALPAGESIZE (and ofs == 0), all bytes are within the same page --> substract 1 from the length
    endPage_u32 = startPage_u32 + ( ((uint32)len_u16 - 1u) / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);

    if(endPage_u32 < rba_FeeFs1x_PAMap_getCurrWrPage())
    {
        // request is below the current write page --> valid request
        rba_FeeFs1x_PASrv_calcCRC32_data_st.startPage_u32 = startPage_u32;
        rba_FeeFs1x_PASrv_calcCRC32_data_st.length_u16    = len_u16;

        rba_FeeFs1x_PASrv_calcCRC32_data_st.crc_pu32      = crc_pu32;

        rba_FeeFs1x_PASrv_calcCRC32_data_st.nextChunkPage_u32 = startPage_u32;

        rba_FeeFs1x_PASrv_calcCRC32_data_st.remainingNBytes_u32 = len_u16;

        rba_FeeFs1x_PASrv_calcCRC32_data_st.startOfs_u8 = startOfs_u8;

        rba_FeeFs1x_PASrv_calcCRC32_data_st.state_en = rba_FeeFs1x_PASrv_calcCRC32_stm_limit_e;
        rba_FeeFs1x_PASrv_calcCRC32_data_st.entry_b  = TRUE;
    }
    else
    {
        /**
         * request is surpassing the current write page --> reject the request
         * Error will returned by the Do-function and Correcly mapped to the wanted behavior
         */
    }
}
/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_calcCRC32Do
 * jobDo function for calculating a CRC32 in DFLASH
 * \return  job result of the calculation
 * \retval rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval rba_FeeFs1x_Pending_e       : job ongoing; call again during next cycle
 * \retval rba_FeeFs1x_ErrorInternal_e         : internal error during processing (e.g. OOB access)
 * \retval rba_FeeFs1x_ErrorExternal_e : processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_calcCRC32Do(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch (rba_FeeFs1x_PASrv_calcCRC32_data_st.state_en)
    {
        case rba_FeeFs1x_PASrv_calcCRC32_stm_limit_e:
        {
            retVal_en = rba_FeeFs1x_PASrv_calcCRC32Do_limit(&rba_FeeFs1x_PASrv_calcCRC32_data_st);
        }break;

        case rba_FeeFs1x_PASrv_calcCRC32_stm_calc_e:
        {
            retVal_en = rba_FeeFs1x_PASrv_calcCRC32Do_calc(&rba_FeeFs1x_PASrv_calcCRC32_data_st);
        }break;

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    // on job exit, reset all statemachine data
    if( retVal_en != rba_FeeFs1x_Pending_e)
    {

        rba_FeeFs1x_PASrv_calcCRC32_data_st.state_en    = rba_FeeFs1x_PASrv_calcCRC32_stm_idle_e;
    }

    return  retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_calcCRC32Do_limit
 * statefunction of calcCRC32Do
 * limits the request to sector boundaries
 * \param   fsm_pst : statefunction pointer
 * \return  return value to be returned by the statemachine
 * \retval rba_FeeFs1x_Pending_e       : job ongoing; call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_calcCRC32Do_limit(rba_FeeFs1x_PASrv_calcCRC32_data_tst * fsm_pst)
{
    uint32 adrCurrPage_u32;
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint32 allowedBytes_u32;
    // If the block is not overlapping and has an offset, a single call shall be enough


    // limit the current chunk length to the sector boundaries
    allowedBytes_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries( fsm_pst->nextChunkPage_u32,
            fsm_pst->startOfs_u8,
            fsm_pst->remainingNBytes_u32);

    fsm_pst->currentChunkLen_u16 = (uint16) allowedBytes_u32;

    // translate the pageno into its address
    adrCurrPage_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->nextChunkPage_u32);

    // trigger the Medium-CRC calculation
    // Fs1 didn't use the firstCall CRC boolean.
    //Thus, to be compliant to the legacy code, the bool always is set to false
    Fee_Prv_MediumCalcCrc(Fee_Rb_DeviceName,
            adrCurrPage_u32 + fsm_pst->startOfs_u8,
            0u,
            fsm_pst->currentChunkLen_u16,
            fsm_pst->crc_pu32,
            FALSE);

    // Proceed with calculating the CRC
    FEE_PRV_LIBSC(rba_FeeFs1x_PASrv_calcCRC32_stm_calc_e);
    retVal_en = rba_FeeFs1x_Pending_e;

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_calcCRC32Do_calc
 * statefunction of calcCRC32Do
 * calls the Medium CRC calculation and handles the exit criteria
 * \param   fsm_pst : statefunction pointer
 * \return  return value to be returned by the statemachine
 * \retval rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval rba_FeeFs1x_Pending_e       : job ongoing; call again during next cycle
 * \retval rba_FeeFs1x_ErrorInternal_e         : internal error during processing (e.g. OOB access)
 * \retval rba_FeeFs1x_ErrorExternal_e : processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_calcCRC32Do_calc(rba_FeeFs1x_PASrv_calcCRC32_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    MemIf_JobResultType retValMedium_en;

    retValMedium_en = Fee_Prv_MediumCalcCrcDo(Fee_Rb_DeviceName);


    if(retValMedium_en != MEMIF_JOB_PENDING)
    {
        if(retValMedium_en == MEMIF_JOB_OK)
        {
            // current chunk calculation finished,
            // decide whether more chunks are required to be calculated

            //current chunk length is limited to max. remaining bytes --> no underflow
            fsm_pst->remainingNBytes_u32 -= fsm_pst->currentChunkLen_u16;
            if(fsm_pst->remainingNBytes_u32 > 0u)
            {
                // another chunk is to be calculated
                fsm_pst->nextChunkPage_u32 = rba_FeeFs1x_PAMap_getLastPageNrOfChunk(
                        fsm_pst->nextChunkPage_u32,
                        fsm_pst->startOfs_u8,
                        fsm_pst->currentChunkLen_u16) + 1u;

                // reset the start offset; only to be considered within the first read
                fsm_pst->startOfs_u8 = 0u;

                FEE_PRV_LIBSC(rba_FeeFs1x_PASrv_calcCRC32_stm_limit_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                // CRC calculation finished, return OK
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
        }
        else
        {
            // CRC calculation caused an error --> abort the job
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
        }
    }
    else
    {
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_compare
 * This function compares the user buffer contents against DFLASH contents (called
 * during write job to check if the new data is different from data in DFLASH).
 * \param   startPage_u32 : first page containing data to be compared
 * \param   startOffset_u8 : offset of the first data within the startPage
 * \param   len_u16 : amount of bytes the comparison has to be executed on
 * \param   userbuffer_pcu8 : pointer to the RAM area to compare against
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PASrv_compare(uint32 startPage_u32, uint8 startOffset_u8, uint16 nrBytes_u16, uint8 const * userbuffer_pcu8)
{
    // lastPageOfBlock should be lower than CurrWrPage
    // job accepted

    // store the parameters
    rba_FeeFs1x_PASrv_compare_data_st.startPage_u32 = startPage_u32;
    rba_FeeFs1x_PASrv_compare_data_st.startOffset_u8 = startOffset_u8;
    rba_FeeFs1x_PASrv_compare_data_st.userbuffer_pcu8 = userbuffer_pcu8;

    rba_FeeFs1x_PASrv_compare_data_st.length_u16 = nrBytes_u16;


    rba_FeeFs1x_PASrv_compare_data_st.nrBytesRemaining_u32 = nrBytes_u16;
    rba_FeeFs1x_PASrv_compare_data_st.nrBytesDone_u32 = 0u;
    rba_FeeFs1x_PASrv_compare_data_st.nextChunkPage_u32 = startPage_u32;
    rba_FeeFs1x_PASrv_compare_data_st.nextChunkOffset_u8 = startOffset_u8;


    // set the statemachine
    rba_FeeFs1x_PASrv_compare_data_st.state_en = rba_FeeFs1x_PASrv_compare_stm_limit_e;
    rba_FeeFs1x_PASrv_compare_data_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_compareDo
 * statemachine of compare job. The function splits the compare request at sector boundaries
 * and exectes the bytewise comparison between RAM and DFLASH

 * \return  job accepted?
 * \retval  rba_FeeFs1x_Job_OK_e   : job finished successfully, no differences between RAM and flash detected
 * \retval  rba_FeeFs1x_JobFailed_e: job detected a difference between RAM and flash
 * \retval  rba_FeeFs1x_ErrorInternal_e : computation error
 * \retval  rba_FeeFs1x_Pending_e  : call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_PASrv_compare_data_st.state_en)
    {
        case rba_FeeFs1x_PASrv_compare_stm_limit_e:
        {
            retVal_en = rba_FeeFs1x_PASrv_compareDo_limit(&rba_FeeFs1x_PASrv_compare_data_st);
        }break;
        case rba_FeeFs1x_PASrv_compare_stm_compare_e:
        {
            retVal_en = rba_FeeFs1x_PASrv_compareDo_compare(&rba_FeeFs1x_PASrv_compare_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_PASrv_compare_data_st.state_en = rba_FeeFs1x_PASrv_compare_stm_idle_e;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_compareDo_limit
 * statefunction of compare job. Limits the request to sector boundaries
 * and exectes the bytewise comparison between RAM and DFLASH
 * \param   fsm_pst : pointer to statemachine data
 * \return  job accepted?
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareDo_limit(rba_FeeFs1x_PASrv_compare_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint32 allowedBytes_u32;
    uint32 currChunkPageAdr_u32;

    allowedBytes_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(fsm_pst->nextChunkPage_u32,
                                                                        fsm_pst->nextChunkOffset_u8,
                                                                        fsm_pst->nrBytesRemaining_u32);

    // allowed bytes for this chunk calculated, store the address of the first page of this chunk
    currChunkPageAdr_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->nextChunkPage_u32);

    // store the job data for this chunk
    fsm_pst->currChunkOffset_u8   = fsm_pst->nextChunkOffset_u8;
    fsm_pst->currChunkAdr_u32    = currChunkPageAdr_u32 + fsm_pst->currChunkOffset_u8;
    fsm_pst->currChunkNrBytes_u32 = allowedBytes_u32;

    // prepare the data for the next chunk

    // calculate the next chunk's start page: page after the last page of the chunk
    fsm_pst->nextChunkPage_u32 =
            rba_FeeFs1x_PAMap_getLastPageNrOfChunk
            (
                    fsm_pst->nextChunkPage_u32,
                    fsm_pst->nextChunkOffset_u8,
                    (uint16)allowedBytes_u32
            ) + 1u;


    // reset the offset for the next chunk; only the first chunk can start unaligned
    fsm_pst->nextChunkOffset_u8 = 0u;

    FEE_PRV_LIBSC(rba_FeeFs1x_PASrv_compare_stm_compare_e);
    retVal_en = rba_FeeFs1x_Pending_e;

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_compareDo_compare
 * statefunction of compare job. executes the comparison on the current chunk
 * and exectes the bytewise comparison between RAM and DFLASH
 * \param   fsm_pst : pointer to statemachine data
 * \return  job accepted?
 * \retval  rba_FeeFs1x_Job_OK_e   : job finished successfully, no differences between RAM and flash detected
 * \retval  rba_FeeFs1x_JobFailed_e: job detected a difference between RAM and flash
 * \retval  rba_FeeFs1x_ErrorInternal_e : computation error
 * \retval  rba_FeeFs1x_Pending_e  : call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareDo_compare(rba_FeeFs1x_PASrv_compare_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    MemIf_JobResultType retValCompare_en;

    if(FEE_PRV_LIBENTRY)
    {
        // on first call, trigger a mediumCompare for the current chunk
        Fee_Prv_MediumCompareFls2Buf(
                Fee_Rb_DeviceName,
                fsm_pst->currChunkAdr_u32,
                0u,
                (void const *)&(fsm_pst->userbuffer_pcu8[fsm_pst->nrBytesDone_u32]),
                fsm_pst->currChunkNrBytes_u32);
    }
    // execute the mediumDo until done
    retValCompare_en = Fee_Prv_MediumCompareFls2BufDo(Fee_Rb_DeviceName);

    switch(retValCompare_en)
    {
        case MEMIF_JOB_OK:
        {
            // current chunk matches on bytelevel

            // update the byte counters
            fsm_pst->nrBytesDone_u32 += fsm_pst->currChunkNrBytes_u32;
            fsm_pst->nrBytesRemaining_u32 -= fsm_pst->currChunkNrBytes_u32;
            // if another chunk is missing, switch back to limiting
            if(fsm_pst->nrBytesRemaining_u32 > 0u)
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_PASrv_compare_stm_limit_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                // comparison complete, bytewise match
                // exit the statemachine with jobOK
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
        }break;
        case MEMIF_BLOCK_INCONSISTENT:
        {
            // return that there is a byte mismatch between RAM and flash
            retVal_en = rba_FeeFs1x_JobFailed_e;
        }break;
        case MEMIF_JOB_PENDING:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
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
 * rba_FeeFs1x_PASrv_compareFls2Fls
 * The function compares 2 dflash copies. it converts the virtual page number (+
 * start offset) into absolute addresses, limits them to the sector boundaries and
 * sends the command to Fee medium to perform the compare operation. Fee medium
 * would internally read one copy into local buffer and invoke the fls_compare
 * operation.
 * \param   copy1_PageNr_u32 : start page of copy 1
 * \param   copy2_PageNr_u32 : start page of copy 2
 * \param   startOfs_u8      : byte offset within the copies ( equal offset for both copies)
 * \param   nrBytes_u16      : amount of bytes to be compared
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PASrv_compareFls2Fls(uint32 copy1_PageNr_u32,
        uint32 copy2_PageNr_u32, uint8 startOfs_u8 , uint16 nrBytes_u16)
{
    /**
     * Last pages should be lower than CurrWrPage
     */
    rba_FeeFs1x_PASrv_compareFls2Fls_data_st.state_en = rba_FeeFs1x_PASrv_compareFls2Fls_stm_compare_e;
    rba_FeeFs1x_PASrv_compareFls2Fls_data_st.entry_b = TRUE;

    rba_FeeFs1x_PASrv_compareFls2Fls_data_st.nrBytesRemaining_u16 = nrBytes_u16;

    rba_FeeFs1x_PASrv_compareFls2Fls_data_st.cpy1_nextChunkPage_u32 = copy1_PageNr_u32;
    rba_FeeFs1x_PASrv_compareFls2Fls_data_st.cpy2_nextChunkPage_u32 = copy2_PageNr_u32;

    rba_FeeFs1x_PASrv_compareFls2Fls_data_st.nextByteOfsInPage_u8   = startOfs_u8;
}


/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_compareFls2FlsDo
 * statemachine for processing the compareFls2Fls job.
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e          : job finished successfully, no differences between the areas detected
 * \retval  rba_FeeFs1x_JobFailed_e      : job detected a difference between the areas
 * \retval  rba_FeeFs1x_ErrorInternal_e  : internal error during execution
 * \retval  rba_FeeFs1x_ErrorExternal_e  : external unit returned error
 * \retval  rba_FeeFs1x_Pending_e        : call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareFls2FlsDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch(rba_FeeFs1x_PASrv_compareFls2Fls_data_st.state_en)
    {
        case rba_FeeFs1x_PASrv_compareFls2Fls_stm_compare_e:
        {
            retVal_en = rba_FeeFs1x_PASrv_compareFls2FlsDo_compare(&rba_FeeFs1x_PASrv_compareFls2Fls_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    // on job exit, reset the statemachine data
    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_PASrv_compareFls2Fls_data_st.state_en = rba_FeeFs1x_PASrv_compareFls2Fls_stm_idle_e;
    }
    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_compareFls2FlsDo_compare
 * statefunction of compareFls2Fls job.
 * \param   fsm_pst : pointer to statemachine data
 * \return  job result
 * \retval  rba_FeeFs1x_Job_OK_e         : job finished successfully, no differences between the areas detected
 * \retval  rba_FeeFs1x_JobFailed_e      : job detected a difference between the areas
 * \retval  rba_FeeFs1x_ErrorInternal_e  : entry state checks failed
 * \retval  rba_FeeFs1x_ErrorExternal_e  : external Fls2Fls Compare failed
 * \retval  rba_FeeFs1x_Pending_e        : call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PASrv_compareFls2FlsDo_compare(rba_FeeFs1x_PASrv_compareFls2Fls_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    MemIf_JobResultType retValMedium_en;

    if(FEE_PRV_LIBENTRY)
    {
        uint32 cpy1NrBytesAllowed_u32 , cpy2NrBytesAllowed_u32;
        uint32 adrCpy1StartPage_u32 , adrCpy2StartPage_u32;
        uint16 minLength_u16;

        // limit to sector boundaries
        cpy1NrBytesAllowed_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(
                fsm_pst->cpy1_nextChunkPage_u32,
                fsm_pst->nextByteOfsInPage_u8,
                fsm_pst->nrBytesRemaining_u16
        );
        cpy2NrBytesAllowed_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(
                fsm_pst->cpy2_nextChunkPage_u32,
                fsm_pst->nextByteOfsInPage_u8,
                fsm_pst->nrBytesRemaining_u16
        );


        // translate the current chunk pages into addresses
        adrCpy1StartPage_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->cpy1_nextChunkPage_u32);
        adrCpy2StartPage_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->cpy2_nextChunkPage_u32);

        // limit to the minimum amount of bytes possible
        // (worst case 3 chunks are required if both copies have a sector overlap)
        minLength_u16 = (uint16) rba_MemLib_Min(cpy1NrBytesAllowed_u32, cpy2NrBytesAllowed_u32);
        // was accepted --> start the job
        Fee_Prv_MediumCompareFls2Fls(Fee_Rb_DeviceName,
                adrCpy1StartPage_u32,
                0u,
                adrCpy2StartPage_u32,
                0u,
                minLength_u16);

        // store the number of bytes of the current chunk
        fsm_pst->nrBytesCurrChunk_u16 = minLength_u16;

        // prepare the next chunk pages

        // first copy page after the current chunk
        fsm_pst->cpy1_nextChunkPage_u32 = 1u +
                rba_FeeFs1x_PAMap_getLastPageNrOfChunk
                (
                        fsm_pst->cpy1_nextChunkPage_u32,
                        fsm_pst->nextByteOfsInPage_u8,
                        fsm_pst->nrBytesCurrChunk_u16
                );
        // second copy page after the current chunk
        fsm_pst->cpy2_nextChunkPage_u32 = 1u +
                rba_FeeFs1x_PAMap_getLastPageNrOfChunk
                (
                        fsm_pst->cpy2_nextChunkPage_u32,
                        fsm_pst->nextByteOfsInPage_u8,
                        fsm_pst->nrBytesCurrChunk_u16
                );


        // reset the offset for the next cycle
        fsm_pst->nextByteOfsInPage_u8 = 0u;
    }

    // Cyclic call the Do function
    retValMedium_en = Fee_Prv_MediumCompareFls2FlsDo(Fee_Rb_DeviceName);


    // evaluate the comparison result
    switch (retValMedium_en)
    {
        case MEMIF_JOB_PENDING:
        {
            // call again during next cycle
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case MEMIF_JOB_OK:
        {
            // current chunk didn't contain bytewise differences
            // check whether another chunk needs to be processed
            fsm_pst->nrBytesRemaining_u16 -= fsm_pst->nrBytesCurrChunk_u16;
            if(fsm_pst->nrBytesRemaining_u16 > 0u)
            {
                // another chunk is left to be compared

                // reenter the state with the new remaining byte value
                FEE_PRV_LIBSC(rba_FeeFs1x_PASrv_compareFls2Fls_stm_compare_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                // all requested bytes are compared, exit the job returning "equal"
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
        }break;
        case MEMIF_BLOCK_INCONSISTENT:
        {
            // compare finished with mismatch between contents in flash and data buffer
            retVal_en = rba_FeeFs1x_JobFailed_e;
        }break;
        case MEMIF_JOB_FAILED:
        {
            // compare finished with an error
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
        }break;
        // state entry checks failed
        case MEMIF_BLOCK_INVALID:
        default:
        {
            // unexpected return value or state entry checks failed
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }


    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_cancel
 * resets internal variables of PASrv
 *********************************************************************
 */

void rba_FeeFs1x_PASrv_cancel(void)
{
    rba_FeeFs1x_PASrv_calcCRC32_data_st.state_en    = rba_FeeFs1x_PASrv_calcCRC32_stm_idle_e;

    rba_FeeFs1x_PASrv_checkPageFullyProgrammedDoData_st.state_en = rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo_idle_e;
    rba_FeeFs1x_PASrv_blankcheckPageDoData_st.state_en = rba_FeeFs1x_PASrv_blankcheckPageDo_idle_e;

    rba_FeeFs1x_PASrv_compare_data_st.state_en = rba_FeeFs1x_PASrv_compare_stm_idle_e;

    rba_FeeFs1x_PASrv_compareFls2Fls_data_st.state_en = rba_FeeFs1x_PASrv_compareFls2Fls_stm_idle_e;
}

# if (RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR > 1u)
/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd
 * checks whether a header extraction onto the given page oversteps a sector boundary
 * (e.g. first page still in older, but second header page in the next sector)
 * (Medium buffer cannot handle those requests)
 * \return        Is the next header extraction request surpassing a sector boundary?
 * \retval  TRUE  Yes, the next request is surpassing a sector end, prevent this access
 * \retval  FALSE No,  the next request stays in a sector
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(uint32 nrPageCurrExtract_u32)
{
    boolean isSurpassing_b;
    uint32 idxLastHdrPage_u32;

    // calculate the pagenumber of the last header page
    idxLastHdrPage_u32 = nrPageCurrExtract_u32 + RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR - 1u;

    // check whether the last header page is overlapping into the next sector
    isSurpassing_b = (idxLastHdrPage_u32 % RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR) < (RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR - 1u);

    return isSurpassing_b;
}
# endif

# if (FALSE != RBA_FEEFS1X_PRV_IS_MULTI_SECTOR_EMULATION)
/**
 *********************************************************************
 * rba_FeeFs1x_PASrv_isBlkOverlappingToNextSector
 * checks whether given block oversteps a logical sector boundary
 *
 * \param   firstPageOfBlk_u32: first page of the block
 * \param   lastPageOfBlk_u32 : last page of the block
 *
 * \return
 * \retval  TRUE  : The block overlaps into next logical sector
 * \retval  FALSE : The block is within the same logical sector
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_PASrv_isBlkOverlappingToNextSector(uint32 firstPageOfBlk_u32, uint32 lastPageOfBlk_u32)
{
    boolean isSurpassing_b = FALSE;
    uint8   logIdx_u8;
    uint32  lastPgOfSector_u32;

    logIdx_u8 = rba_FeeFs1x_PAMap_getLogIdxByPagePrechecked(firstPageOfBlk_u32);

    lastPgOfSector_u32 = rba_FeeFs1x_Sector_getLogEndPage(logIdx_u8);

    if(lastPageOfBlk_u32 > lastPgOfSector_u32)
    {
        isSurpassing_b = TRUE;
    }

    return isSurpassing_b;
}
# endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

