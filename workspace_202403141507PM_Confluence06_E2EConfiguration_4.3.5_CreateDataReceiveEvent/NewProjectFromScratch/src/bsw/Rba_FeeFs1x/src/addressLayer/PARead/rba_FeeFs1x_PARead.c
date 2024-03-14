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
#include "rba_FeeFs1x_Prv_PARead.h"
#include "rba_FeeFs1x_Prv_PAReadTypes.h"

#include "rba_FeeFs1x_Prv.h"

#include "rba_FeeFs1x_Prv_PAMap.h"
#include "rba_FeeFs1x_Prv_PASrv.h"

#include "Fee_Prv_Lib.h"
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

static rba_FeeFs1x_PARead_getPages_tst   rba_FeeFs1x_PARead_getPage_st;
static rba_FeeFs1x_PARead_unbufRead_tst rba_FeeFs1x_PARead_unbufRead_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_unbufferedReadDo_checkAndPrepare(rba_FeeFs1x_PARead_unbufRead_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_unbufferedReadDo_read(rba_FeeFs1x_PARead_unbufRead_tst *fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_unbufferedReadDo_readfinal(rba_FeeFs1x_PARead_unbufRead_tst *fsm_pst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 *rba_FeeFs1x_PARead_setIteratorDirection
 * \param   backward_b : sets the iteratorDirection acc. to that parameter. FALSE = forward, TRUE = backward
 * \param   nrBytesPrefetch_u32 : sets the amount of bytes the buffering should prefetch.
 *                   For larger scans, the buffer size can be used, for data validations ideally the block copie's size
 *********************************************************************
 */
void rba_FeeFs1x_PARead_setIteratorPreferences(boolean isBackwardBuffering_b, uint32 nrBytesPrefetch_u32)
{
    rba_FeeFs1x_PARead_getPage_st.isBackwardBuffering_b = isBackwardBuffering_b;
    rba_FeeFs1x_PARead_getPage_st.nrBytesPrefetch_u32 = nrBytesPrefetch_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PARead_getPages
 * Job acceptor function for getting a page from DFLASH
 * \param   nrPage_u32: requested page
 * \param   cntPages_u32: amount of pages to be read
 * \param   adrPageInBuffer_ppcu8: callByRef param to return a pointer to the location the data is stored at
 * \return  void
 *********************************************************************
 */
void rba_FeeFs1x_PARead_getPages(uint32 nrPage_u32 , uint32 cntPages_u32 , uint8 const * * adrPageInBuffer_ppcu8)
{
    uint32 nrCurrWrPage_u32;
    /*If the read is surpassing the write page, reject the job*/
    nrCurrWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();

    if(((nrPage_u32 + cntPages_u32 - 1u) < nrCurrWrPage_u32)    ||
        (rba_FeeFs1x_PARead_getPage_st.isBackwardBuffering_b && (nrPage_u32 < nrCurrWrPage_u32)))
    {
        // If the job is not affected by the writePage forward scan check

        // check whether the requested header would overlap a sector boundary (data junk and OOB possibility)
        if( ! rba_FeeFs1x_PASrv_isNextHdrCheckSurpassingSectorEnd(nrPage_u32))
        {
            // A header on the requested page is NOT overlapping a sector boundary
            uint32 adrPage_u32;
            uint32 nrBytesPrefetch_u32;

            nrBytesPrefetch_u32 = rba_FeeFs1x_PARead_getPage_st.nrBytesPrefetch_u32;

            // Try to get the address of the requested page to prepare the medium buffered read
            adrPage_u32 = rba_FeeFs1x_PAMap_getAddressByPage(nrPage_u32);

            /*store the pagebuffer pointer*/
            rba_FeeFs1x_PARead_getPage_st.adrPageInBuffer_ppcu8 = adrPageInBuffer_ppcu8;
            rba_FeeFs1x_PARead_getPage_st.cntPages_u32 = cntPages_u32;
            /*Set the medium iterator to the address of the requested page*/
            Fee_Prv_MediumSetIterator(Fee_Rb_DeviceName, adrPage_u32, nrBytesPrefetch_u32);
        }
        else
        {
            // A header on the requested page IS overlapping -> data junk and OOB access possible
            // --> forbid this page read
        }
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_PARead_getPageDo
 * Processing function for getting a page read from DFLASH
 * \return  rba_FeeFs1x_RetVal_ten: Read job result
 * \retval  rba_FeeFs1x_JobOK_e: job finished, the buffer pointer was used to transfer the page content to the user
 * \retval  rba_FeeFs1x_ErrorExternal_e: job aborted, user page buffer is not touched
 * \retval  rba_FeeFs1x_Pending_e: ongoing; function should be called again during the next mainfunction cycle
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_getPagesDo(void)
{
    MemIf_JobResultType retValMedium_en;
    rba_FeeFs1x_RetVal_ten retVal_en;
    const uint8 * adrMediumBufferContent_pcu8;
    uint32 cntReqestedBytes_u32;

    if(NULL_PTR != rba_FeeFs1x_PARead_getPage_st.adrPageInBuffer_ppcu8)
    {
        cntReqestedBytes_u32 = rba_FeeFs1x_PARead_getPage_st.cntPages_u32 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;

        if(rba_FeeFs1x_PARead_getPage_st.isBackwardBuffering_b)
        {
            /* MR12 RULE 11.3 VIOLATION: uint8 * can always be converted safely to void * */
            retValMedium_en = Fee_Prv_MediumPre(Fee_Rb_DeviceName, cntReqestedBytes_u32, (void const **)(&adrMediumBufferContent_pcu8));
        }
        else
        {
            /* MR12 RULE 11.3 VIOLATION: uint8 * can always be converted safely to void * */
            retValMedium_en = Fee_Prv_MediumNext(Fee_Rb_DeviceName, cntReqestedBytes_u32, (void const **)(&adrMediumBufferContent_pcu8));
        }

        switch(retValMedium_en)
        {
            case MEMIF_JOB_OK:
            {
                /*Medium job finished successfully, the page content can be used now from the buffer*/
                *(rba_FeeFs1x_PARead_getPage_st.adrPageInBuffer_ppcu8) = adrMediumBufferContent_pcu8;

                retVal_en = rba_FeeFs1x_JobOK_e;

            }break;

            case MEMIF_JOB_PENDING:
            {
                retVal_en = rba_FeeFs1x_Pending_e;
            }break;
            case MEMIF_JOB_FAILED:
            case MEMIF_JOB_CANCELED:
            case MEMIF_BLOCK_INCONSISTENT:
            case MEMIF_BLOCK_INVALID:
            default:
            {
                retVal_en = rba_FeeFs1x_ErrorExternal_e;
            }break;
        }
    }
    else
    {
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 *
 * \param   abstrPage_u32 : start page where first user data is stored.
 * \param   nrOfBytes_u16 : length of block data[bytes] to be read
 * \param   *buffer_pu8 : pointer to buffer, where the data has to be stored
 * \param   startOfs_u8 : offset[bytes] to the start page from where first user data is stored.
 *          abstrPage_u32 + startOfs_u8 is the first address after the block header.
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PARead_unbufferedRead(uint32 abstrPage_u32, uint16 nrOfBytes_u16, uint8 *buffer_pu8, uint8 startOfs_u8)
{
    uint32 nrLogPagesOffset_u32;
    uint32 nrLogPages_u32;

    /* calculate the page offset to the start read page */
    nrLogPagesOffset_u32 = (nrOfBytes_u16 + startOfs_u8 - 1u) / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
    nrLogPages_u32 = abstrPage_u32 + nrLogPagesOffset_u32;

    /* check, if the address of the read request is behind the actual write address */
    /* buffer_pu8 is not checked against NULL_PTR. already done in high level user interfasce */
    if(rba_FeeFs1x_PAMap_isPageSmallerWrAdr(nrLogPages_u32))
    {
        /* initialize the state machine */
        rba_FeeFs1x_PARead_unbufRead_st.state_en = rba_FeeFs1x_PARead_unbufferedReadDoStm_checkAndPrepare_e;
        rba_FeeFs1x_PARead_unbufRead_st.entry_b = TRUE;
        /* initialize data */
        rba_FeeFs1x_PARead_unbufRead_st.startPage_u32 = abstrPage_u32;
        rba_FeeFs1x_PARead_unbufRead_st.nrOfBytes_u32 = nrOfBytes_u16;
        rba_FeeFs1x_PARead_unbufRead_st.writeBuff_pu8 = buffer_pu8;
        rba_FeeFs1x_PARead_unbufRead_st.writeBuffOfset_u32 = 0u;
        rba_FeeFs1x_PARead_unbufRead_st.startOfs_u8 = startOfs_u8;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_PARead_unbufferedReadDo
 * execution of the requested read
 * \param   void
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e: read is finished succesfully
 * \retval  rba_FeeFs1x_Pending_e: read is still ongoing
 * \retval  rba_FeeFs1x_ErrorInternal_e: something went wrong
 * \retval  rba_FeeFs1x_ErrorExternal_e: dflash access failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_unbufferedReadDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_PARead_unbufRead_st.state_en)
    {
        case rba_FeeFs1x_PARead_unbufferedReadDoStm_checkAndPrepare_e:
        {
            retVal_en = rba_FeeFs1x_PARead_unbufferedReadDo_checkAndPrepare(&rba_FeeFs1x_PARead_unbufRead_st);
            break;
        }
        case rba_FeeFs1x_PARead_unbufferedReadDoStm_read_e:
        {
            retVal_en = rba_FeeFs1x_PARead_unbufferedReadDo_read(&rba_FeeFs1x_PARead_unbufRead_st);
            break;
        }
        case rba_FeeFs1x_PARead_unbufferedReadDoStm_readfinal_e:
        {
            retVal_en = rba_FeeFs1x_PARead_unbufferedReadDo_readfinal(&rba_FeeFs1x_PARead_unbufRead_st);
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }
    // on job exit, reset the statemachine data
    if(rba_FeeFs1x_Pending_e != retVal_en)
    {
        rba_FeeFs1x_PARead_unbufRead_st.state_en = rba_FeeFs1x_PARead_unbufferedReadDoStm_idle_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PARead_unbufferedReadDo_checkAndPrepare
 * state function for checking if a read has to be performed and calculation of the required read parameters
 * \param   *fsm_pst : pointer to struct containing the state variables
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e: still ongoing
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_unbufferedReadDo_checkAndPrepare(rba_FeeFs1x_PARead_unbufRead_tst *fsm_pst)
{
    uint32 nrBytesInSector_u32;
    uint32 readAddr_u32;
    rba_FeeFs1x_RetVal_ten retVal_en;

    /* check, how many logical pages may be read until the sector boundary is reached */
    nrBytesInSector_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries
                            (
                                fsm_pst->startPage_u32,
                                fsm_pst->startOfs_u8,
                                fsm_pst->nrOfBytes_u32
                            );

    // read the amount of bytes using the current page and the offset

    // translate the current page into an address
    readAddr_u32 = rba_FeeFs1x_PAMap_getAddressByPage(rba_FeeFs1x_PARead_unbufRead_st.startPage_u32);

    /* read available number of pages in the sector */
    Fee_Prv_MediumRead(
            Fee_Rb_DeviceName,
            readAddr_u32 + fsm_pst->startOfs_u8,
            0u,
            (void*)(&(fsm_pst->writeBuff_pu8[fsm_pst->writeBuffOfset_u32])),
            nrBytesInSector_u32);

    // prepare the next cycle for reading the next sector
    fsm_pst->nrOfBytes_u32 -= nrBytesInSector_u32;
    if(0u < fsm_pst->nrOfBytes_u32)
    {
        // increment the start page for the next cycle
        fsm_pst->startPage_u32 = rba_FeeFs1x_PAMap_getLastPageNrOfChunk(
                fsm_pst->startPage_u32,
                fsm_pst->startOfs_u8,
                (uint16)nrBytesInSector_u32) + 1u;

        // reset the start ofset; only to be considered within the first read
        fsm_pst->startOfs_u8 = 0u;

        // increment the pointer by the amount of currently read bytes
        fsm_pst->writeBuffOfset_u32 += nrBytesInSector_u32;
        FEE_PRV_LIBSC(rba_FeeFs1x_PARead_unbufferedReadDoStm_read_e);
    }
    else
    {
        // final Read, execute the finalRead state to finish this cycle
        FEE_PRV_LIBSC(rba_FeeFs1x_PARead_unbufferedReadDoStm_readfinal_e);
    }
    retVal_en = rba_FeeFs1x_Pending_e;

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PARead_unbufferedReadDo_read
 * state function performing the read
 * \param   *fsm_pst : pointer to struct containing the state variables
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e: read is still ongoing
 * \retval  rba_FeeFs1x_ErrorInternal_e: something went wrong
 * \retval  rba_FeeFs1x_ErrorExternal_e: dflash access failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_unbufferedReadDo_read(rba_FeeFs1x_PARead_unbufRead_tst *fsm_pst)
{
    MemIf_JobResultType retValMediumRead_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    retValMediumRead_en = Fee_Prv_MediumReadDo(Fee_Rb_DeviceName);

    switch(retValMediumRead_en)
    {
        case MEMIF_JOB_OK:
        {
            /* read finished, so check again if there is still data to be read and stay PENDING */

            FEE_PRV_LIBSC(rba_FeeFs1x_PARead_unbufferedReadDoStm_checkAndPrepare_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_PENDING:
        {
            /* reading still ongoing, stay in state and stay PENDING */
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            /* dflash access finished with an error */
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
            break;
        }
        default:
        {
            /* an error occured */
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PARead_unbufferedReadDo_readfinal
 * state function performing the final read
 * \param   *fsm_pst : pointer to struct containing the state variables
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e: read is finished succesfully
 * \retval  rba_FeeFs1x_Pending_e: read is still ongoing
 * \retval  rba_FeeFs1x_ErrorInternal_e: an error occured
 * \retval  rba_FeeFs1x_ErrorExternal_e: dflash access failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PARead_unbufferedReadDo_readfinal(rba_FeeFs1x_PARead_unbufRead_tst *fsm_pst)
{
    MemIf_JobResultType retValMediumRead_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    *fsm_pst = *fsm_pst;      /* Done to remove compiler and MISRA warnings */

    retValMediumRead_en = Fee_Prv_MediumReadDo(Fee_Rb_DeviceName);

    switch(retValMediumRead_en)
    {
        case MEMIF_JOB_OK:
        {
            /* last read finished, so leave the read job */
            retVal_en = rba_FeeFs1x_JobOK_e;
            break;
        }
        case MEMIF_JOB_PENDING:
        {
            /* reading still ongoing, stay in state and stay PENDING */
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case MEMIF_JOB_FAILED:
        {
            /* dflash access finished with an error */
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
            break;
        }
        default:
        {
            /* an error occured */
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }
    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_PARead_cancel
 * inits the internal variables of PARead
 *********************************************************************
 */
void rba_FeeFs1x_PARead_cancel(void)
{
    rba_FeeFs1x_PARead_getPage_st.isBackwardBuffering_b = TRUE;
    rba_FeeFs1x_PARead_getPage_st.nrBytesPrefetch_u32   = 0u;
    rba_FeeFs1x_PARead_unbufRead_st.state_en = rba_FeeFs1x_PARead_unbufferedReadDoStm_idle_e;
}




#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

