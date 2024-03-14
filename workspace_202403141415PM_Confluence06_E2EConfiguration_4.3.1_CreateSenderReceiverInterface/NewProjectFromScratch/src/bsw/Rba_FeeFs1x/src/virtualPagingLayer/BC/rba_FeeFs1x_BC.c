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
#include "rba_FeeFs1x_Prv_Cfg.h"

#include "rba_FeeFs1x_Prv_BC.h"
#include "rba_FeeFs1x_Prv_BCTypes.h"


#include "rba_FeeFs1x_Prv_PASrv.h"
#include "rba_FeeFs1x_Prv_PARead.h"
#include "rba_FeeFs1x_Prv_PAWrite.h"
#include "rba_MemLib.h"
#include "Fee_Prv_Lib.h"
#include "Fee_Rb_Types.h"


#include "Crc.h"
#if (!defined(CRC_AR_RELEASE_MAJOR_VERSION) || (CRC_AR_RELEASE_MAJOR_VERSION != FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
#define RBA_FEEFS1X_BC_PREAMBEL_BLKHDR      (0xA53C96uL)                                  /* Pattern of preambel */
#define RBA_FEEFS1X_BC_PREAMBEL_BYTE_2      ((uint8)(RBA_FEEFS1X_BC_PREAMBEL_BLKHDR & 0xFFu)         )  /* 0x96 */
#define RBA_FEEFS1X_BC_PREAMBEL_BYTE_1      ((uint8)((RBA_FEEFS1X_BC_PREAMBEL_BLKHDR >> 8u) & 0xFFu) )  /* 0x3C */
#define RBA_FEEFS1X_BC_PREAMBEL_BYTE_0      ((uint8)((RBA_FEEFS1X_BC_PREAMBEL_BLKHDR >> 16u) & 0xFFu))  /* 0xA5 */


#define RBA_FEEFS1X_BC_HDRCRC_BYTES2BECONSIDERED        (8u)

#define RBA_FEEFS1X_BC_NR_ROBCRCBYTES                   (4u)

#define RBA_FEEFS1X_BC_HDRCRC_STARTVALUE                (0xCAFEu)
#define RBA_FEEFS1X_BC_HDRCRC_NRBYTES                   (8u)

#define RBA_FEEFS1X_BC_DATACRC_STARTVALUE(hdrCrc_u16)   ((uint32)hdrCrc_u16 ^ RBA_FEEFS1X_MAXUINT32)
/*
 *
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define FEE_START_SEC_VAR_SECURED_CLEARED_8
#include "Fee_MemMap.h"

static uint8 rba_FeeFs1x_BC_buf_au8[RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE*RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR];
static uint8 rba_FeeFs1x_BC_robDataCrcBuf_au8[RBA_FEEFS1X_BC_NR_ROBCRCBYTES];

#define FEE_STOP_SEC_VAR_SECURED_CLEARED_8
#include "Fee_MemMap.h"


#define FEE_START_SEC_VAR_SECURED_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

// any user data has to be located to a secured memory area, rba_FeeFs1x_BC_blockCopies_ast contains user data
// -> locate struct or at least the 2 data bytes to a secured memory area
static rba_FeeFs1x_BC_Cpy_tst rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_NrOfCopies_e];

#define FEE_STOP_SEC_VAR_SECURED_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"


#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

static rba_FeeFs1x_BC_invalidate_tst rba_FeeFs1x_BC_invalidate_st;
static rba_FeeFs1x_BC_readVldtFls2Ram_tst rba_FeeFs1x_BC_readVldtFls2Ram_st;
static rba_FeeFs1x_BC_extractNValHdr_tst rba_FeeFs1x_BC_extractNValHdr_st;
static rba_FeeFs1x_BC_initCopy_stm_tst rba_FeeFs1x_BC_initCopy_stm_st;
static rba_FeeFs1x_BC_initCopyWr_tst rba_FeeFs1x_BC_initCopyWr_st;
static rba_FeeFs1x_BC_initCopySectReorgWr_tst rba_FeeFs1x_BC_initCopySectReorgWr_st;
static rba_FeeFs1x_BC_wrData2Fls_tst rba_FeeFs1x_BC_wrData2Fls_st; // variable for the FLS write operation

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"




#define FEE_START_SEC_CONST_8
#include "Fee_MemMap.h"

static const uint8 rba_FeeFs1x_NoFallbackMarkerPattern_au8[RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE] =
{
        0xDE, 0xAD, 0xB1, 0x0C, 0xB1, 0x0C, 0xDE, 0xAD,
        0xDE, 0xAD, 0xB1, 0x0C, 0xB1, 0x0C, 0xDE, 0xAD
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

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyDo_extractHdr  (rba_FeeFs1x_BC_initCopy_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyDo_validateHdr (rba_FeeFs1x_BC_initCopy_stm_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyDo_end         (rba_FeeFs1x_BC_initCopy_stm_tst const * fsm_pst);

// sub functions to handle the internal header buffer reads and updates
LOCAL_INLINE void rba_FeeFs1x_BC_fill1stHdrPgInBC(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en);
LOCAL_INLINE void rba_FeeFs1x_BC_fill2ndHdrPgInBC(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en);
static void rba_FeeFs1x_BC_fillToHdrBuffer(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en);
LOCAL_INLINE void rba_FeeFs1x_BC_updateDataCrcInHdrBuffer(uint32 dataCrc_u32);

LOCAL_INLINE boolean rba_FeeFs1x_BC_getCpyIgnoreBit(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en);
LOCAL_INLINE boolean rba_FeeFs1x_BC_getCpyLegacyRobCRCBit(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en);

// state functions for invalidate request
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_invalidateDo_prepareHdr(rba_FeeFs1x_BC_invalidate_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_invalidateDo_wrHdr(rba_FeeFs1x_BC_invalidate_tst * const fsm_pst);

// state functions for valdiate header
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_firstChecks(rba_FeeFs1x_BC_extractNValHdr_tst  * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_calcCRC(rba_FeeFs1x_BC_extractNValHdr_tst  * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_checkFullyProg(rba_FeeFs1x_BC_extractNValHdr_tst * const  fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_checkNoFBPattern(rba_FeeFs1x_BC_extractNValHdr_tst  * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_checkNoFBDestr(rba_FeeFs1x_BC_extractNValHdr_tst  * const fsm_pst);

// state functions for readVldtFls2RamDo operation
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_procHdrBlkData(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_readBlkDataToUsrBuf(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_vldtBlkDataFromUsrBuf(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_vldtRmngBlkData(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_compareDataCrc(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_compareLegacyRobDataCrc(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst);

LOCAL_INLINE uint8 rba_FeeFs1x_BC_getNrHdrPages(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en);
static uint16 rba_FeeFs1x_BC_getNrDataPagesByBlkLen(uint16 len_u16);

static uint32 rba_FeeFs1x_BC_calcDataCrcForDataBytesInHdr(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en);

// state functions for wrData2FlsDo operation
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromFls(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromRam(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromFls(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromRam(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_recomputeDataCrc(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrHdr(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrDummyHdr(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst);
LOCAL_INLINE void rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromFls_updateStatusByte(rba_FeeFs1x_BC_wrData2Fls_tst const * const fsm_pst);

LOCAL_INLINE void rba_FeeFs1x_BC_extractHdr_CopyContentAndUpdatePageNr(uint8 const * adrPageInBuffer_pcu8 , uint32 nrPage_u32);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_BC_init
 * initializes the internal variables of the BC
 *********************************************************************
 */
void rba_FeeFs1x_BC_init(void)
{
    uint8_least idx_qu8;
    // init extract Hdr struct
    rba_FeeFs1x_BC_extractNValHdr_st.state_en = rba_FeeFs1x_BC_extractNValHdrStates_idle_e;

    // init initCopy struct
    rba_FeeFs1x_BC_initCopy_stm_st.state_en = rba_FeeFs1x_BC_initCopy_stm_idle_e;

    // init invalidate struct
    rba_FeeFs1x_BC_invalidate_st.state_en = rba_FeeFs1x_BC_invalidateDoStates_idle_e;

    // init wrFls2Fls struct
    rba_FeeFs1x_BC_wrData2Fls_st.state_en = rba_FeeFs1x_BC_wrData2FlsDoStates_idle_e;

    // init readVldtFls2Ram struct
    rba_FeeFs1x_BC_readVldtFls2Ram_st.state_en = rba_FeeFs1x_BC_readVldtFls2RamStates_idle_e;

    // init header buffer
    for(idx_qu8 = 0u; idx_qu8 < (RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE * RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR) ; idx_qu8++)
    {
        rba_FeeFs1x_BC_buf_au8[idx_qu8] = 0u;
    }

    // init block copies
    for(idx_qu8 = (uint8_least)rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e;
        idx_qu8 < (uint8_least)rba_FeeFs1x_BC_Cpy_NrOfCopies_e;
        idx_qu8++)
    {
        /* MR12 RULE 10.5 VIOLATION: casting is safe here because number of block copies are limited to 9 */
        rba_FeeFs1x_BC_finalizeCopy((rba_FeeFs1x_BC_Cpy_blockCopyObject_ten)idx_qu8);
    }

}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopy
 * initializes one copy object for a copy already present in DFLASH
 * -> extracts the relevant data from DFLASH
 *
 * \param   copy_en : copy object to be processed
 * \param   persID_u16 : persistent ID of the copy expected in this pageNo
 * \param   pageNr_u32 : pagenumber the copy should be located in
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_initCopy(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en, uint16 persID_u16, uint32 pageNr_u32)
{
    // valid copy index --> store the given parameters
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16 = persID_u16;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32 = pageNr_u32;

    rba_FeeFs1x_BC_initCopy_stm_st.copy_en = copy_en;

    rba_FeeFs1x_BC_initCopy_stm_st.state_en = rba_FeeFs1x_BC_initCopy_stm_extract_e;
    rba_FeeFs1x_BC_initCopy_stm_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopyDo
 * \return  extraction passed, header is valid and the searched one
 * \retval rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval rba_FeeFs1x_Pending_e       : job ongoing; call again during next cycle
 * \retval rba_FeeFs1x_JobFailed_e     : page doesn't contain the searched block
 *                                      (no header at all, not valid or other persID)
 * \retval rba_FeeFs1x_ErrorInternal_e : internal error e.g OOB, RAM corruption
 * \retval rba_FeeFs1x_ErrorExternal_e : processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    switch(rba_FeeFs1x_BC_initCopy_stm_st.state_en)
    {
        case rba_FeeFs1x_BC_initCopy_stm_extract_e:
        {
            retVal_en = rba_FeeFs1x_BC_initCopyDo_extractHdr(&rba_FeeFs1x_BC_initCopy_stm_st);
        }break;

        case rba_FeeFs1x_BC_initCopy_stm_validate_e:
        {
            retVal_en = rba_FeeFs1x_BC_initCopyDo_validateHdr(&rba_FeeFs1x_BC_initCopy_stm_st);
        }break;

        case rba_FeeFs1x_BC_initCopy_stm_end_e:
        {
            // Extraction and validation passed --> copy the content from the internal processing element
            //                                      to the designated copy
            retVal_en = rba_FeeFs1x_BC_initCopyDo_end(&rba_FeeFs1x_BC_initCopy_stm_st);
        }break;

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(rba_FeeFs1x_Pending_e != retVal_en)
    {
        // reset the state machine and finalise the internal copy used during the extract and validate header
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_Internal_e);

        rba_FeeFs1x_BC_initCopy_stm_st.state_en = rba_FeeFs1x_BC_initCopy_stm_idle_e;
    }
    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopyDo_extractHdr
 * statefunction
 * \param   fsm_pst : statefunction pointer
 * \return  extraction passed?
 * \retval rba_FeeFs1x_Pending_e       : job ongoing; call again during next cycle
 * \retval rba_FeeFs1x_ErrorInternal_e : page doesn't contain the searched block
 *                                      (no header at all, not valid or other persID)
 * \retval rba_FeeFs1x_ErrorExternal_e : processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyDo_extractHdr(rba_FeeFs1x_BC_initCopy_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValExtractHdr_en;
    if(FEE_PRV_LIBENTRY)
    {
        // on initial calls, init the extract Hdr job
        rba_FeeFs1x_BC_extractHdr(rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].pageNr_u32);
    }

    // Cyclic call the Do-function of extract hdr job
    retValExtractHdr_en = rba_FeeFs1x_BC_extractHdrDo();

    // As soon as the extraction exited switch the state according to the return value
    if(rba_FeeFs1x_JobOK_e == retValExtractHdr_en)
    {
        // extraction of header with known position successful, continue with second part of header validation
        FEE_PRV_LIBSC(rba_FeeFs1x_BC_initCopy_stm_validate_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // on internal-, external error, pending or job failed do no state switch, just return
        retVal_en = retValExtractHdr_en;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopyDo_validateHdr
 * statefunction
 * \param   fsm_pst : statefunction pointer
 * \return  validation passed?
 * \retval rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval rba_FeeFs1x_Pending_e       : job ongoing; call again during next cycle
 * \retval rba_FeeFs1x_JobFailed_e     : page doesn't contain the searched block(no header at all or not valid)
 * \retval rba_FeeFs1x_ErrorInternal_e : internal error e.g. OOB
 * \retval rba_FeeFs1x_ErrorExternal_e : processing error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyDo_validateHdr(rba_FeeFs1x_BC_initCopy_stm_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValValHdrDo_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_validateHdr(FALSE);
    }
    retValValHdrDo_en = rba_FeeFs1x_BC_validateHdrDo();

    switch(retValValHdrDo_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // valid header; duplicate the internal copy to the requested one
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_initCopy_stm_end_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_ErrorInternal_e:
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        {
            // return the inner return value
            retVal_en = retValValHdrDo_en;
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopyDo_end
 * statefunction
 * \param   fsm_pst : statefunction pointer
 * \return  correct persID?
 * \retval rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval rba_FeeFs1x_ErrorInternal_e : persistent Id mismatch
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyDo_end(rba_FeeFs1x_BC_initCopy_stm_tst const * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    // If the extracted persistent ID matches the requested one, copy the internal BC to the designated one
    if(rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].persID_u16 ==
            rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].persID_u16)
    {

        rba_FeeFs1x_BC_copyObjectInternal(fsm_pst->copy_en);

        retVal_en = rba_FeeFs1x_JobOK_e;
    }
    else
    {
        // persistent Id mismatching: internal error or RAM corruption
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopyWr
 * This function initialises the block copy to be created in DFLASH.
 * The same function would be called for all write operations i.e., write job, invalidate job and maintain job.
 * Hence, all the required information must be passed during the function call. This function when called triggers
 * prepare space function of Page abstractor.
 *
 * \param   copy_en         : Block copy that is required to be stored to DFLASH.
 * \param   len_u16         : length of the block copy to be stored.
 * \param   persID_u16      : persistent id of the block copy to be stored.
 * \param   statusByte_u16  : status flag of the block copy to be stored.
 *
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_initCopyWr(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en,
        uint16 len_u16, uint16 persID_u16, uint16 statusByte_u16, Fee_Rb_JobMode_ten jobMode_en)
{
    uint8 nrHdrPgs_u8;
    uint16 nrDataPages_u16;

    // Store the block copy being initialized
    rba_FeeFs1x_BC_initCopyWr_st.copy_en = copy_en;
    rba_FeeFs1x_BC_initCopyWr_st.validBCWasMovedDuringReorg_b = FALSE;

    // Store the information into the block copy
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16 = persID_u16;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16 = len_u16;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 = statusByte_u16;

    // compute the number of header pages and data pages
    nrHdrPgs_u8 = rba_FeeFs1x_BC_getNrHdrPages(copy_en);
    nrDataPages_u16 = rba_FeeFs1x_BC_getNrDataPagesByBlkLen(len_u16);

    // call the prepare space function to prepare the required space. the page number is returned in .pageNr_u32
    rba_FeeFs1x_PAWrite_jobReqPrepareSpace( nrDataPages_u16,
                                            nrHdrPgs_u8,
                                            &rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32,
                                            jobMode_en );
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopyWrDo
 * This is the do function for preparing space before the storage of block copy to DFLASH.
 *
 * \return  Returns whether the preapre space opeartion was completed
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : prepare space was successfully completed
 * \retval  rba_FeeFs1x_JobFailed_e     : block copy was moved during sector reorg, return failure to restart block jobs
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during preperation of space
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, prepare space wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopyWrDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten prepareSpaceDoRetVal_en;

    prepareSpaceDoRetVal_en = rba_FeeFs1x_PAWrite_jobReqPrepareSpaceDo();

    switch (prepareSpaceDoRetVal_en)
    {
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = prepareSpaceDoRetVal_en;
        }break;
        case rba_FeeFs1x_JobOK_e:
        {
            if(!rba_FeeFs1x_BC_initCopyWr_st.validBCWasMovedDuringReorg_b)
            {
                // Leave one page blank for no fall back block
                if (rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_initCopyWr_st.copy_en))
                {
                    rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_initCopyWr_st.copy_en].pageNr_u32++;
                }
                else
                {
                    // do nothing
                }

                // Prepare space is completed successfully. Set flag to indicate that block copy is sucessfully created
                rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_initCopyWr_st.copy_en].isInitailized_b = TRUE;

                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // possible underflow during sector reorg, return job failed so that block job is restarted with search
                retVal_en = rba_FeeFs1x_JobFailed_e;
            }

            rba_FeeFs1x_BC_initCopyWr_st.validBCWasMovedDuringReorg_b = FALSE;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // Prepare space is ongoing, wait till its completed.
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopySectReorgWr
 * The function is called to prepare the block copy to be created in DFLASH during sector reorganization.
 * Same function is called for both transfer of known and unknown blocks. Hence, all the required information must be
 * passed during the function call. This function when called triggers prepare space function of Page abstractor.
 *
 * \param   copy_en         : Block copy that is required to be stored to DFLASH.
 * \param   len_u16         : length of the block copy to be stored.
 * \param   persID_u16      : persistent id of the block copy to be stored.
 * \param   statusByte_u16  : status flag of the block copy to be stored.
 *
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_initCopySectReorgWr(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en,
        uint16 len_u16, uint16 persID_u16, uint16 statusByte_u16)
{
    uint8 nrHdrPgs_u8;
    uint16 nrDataPages_u16;

    // Store the block copy being initialized
    rba_FeeFs1x_BC_initCopySectReorgWr_st.copy_en = copy_en;

    // Store the information into the block copy
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16 = persID_u16;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16 = len_u16;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 = statusByte_u16;

    // compute the number of header pages and data pages
    nrHdrPgs_u8 = rba_FeeFs1x_BC_getNrHdrPages(copy_en);
    nrDataPages_u16 = rba_FeeFs1x_BC_getNrDataPagesByBlkLen(len_u16);

    // call the prepare space function to prepare the requried space. the page number is returned in .pageNr_u32
    rba_FeeFs1x_PAWrite_reorgPrepareSpace(  nrDataPages_u16,
                                            nrHdrPgs_u8,
                                            &rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32 );
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_initCopySectReorgWrDo
 * This is the do function for preparing space before the storage of block copy to DFLASH.
 *
 * \return  Returns whether the preapre space opeartion was completed
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e : prepare space was successfully completed
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during preperation of space
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, prepare space wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_initCopySectReorgWrDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten prepareSpaceDoRetVal_en;
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en;
    uint32 firstBlockJobIdx_u32;
    uint32 lastBlockJobIdx_u32;
    uint32 idx_u32;

    prepareSpaceDoRetVal_en = rba_FeeFs1x_PAWrite_reorgPrepareSpaceDo();

    switch (prepareSpaceDoRetVal_en)
    {
        case rba_FeeFs1x_Pending_e:
            // Prepare space is ongoing, wait till its completed.
        case rba_FeeFs1x_JobFailed_e:
            // sector overflow detected, stop the init
        case rba_FeeFs1x_ErrorExternal_e:
            // Error during prepare space
        {
            retVal_en = prepareSpaceDoRetVal_en;
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            copy_en = rba_FeeFs1x_BC_initCopySectReorgWr_st.copy_en;

            // Leave one page blank for no fall back block
            if(rba_FeeFs1x_BC_getCpyNoFBBitActive(copy_en))
            {
                rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32++;
            }

            // reorg is transferring a copy from sector 0 or writing another eventually lost copy
            // In the case you have a noFB block, no additional copy must be written by the pending block job, which
            // triggered the reorg. This can be prevented by switching the block job back to search for copies
            // -> switch back to search copies by setting validBCWasMovedDuringReorg_b (BC_initCopy returns jobFailed)
            if(!rba_FeeFs1x_BC_initCopyWr_st.validBCWasMovedDuringReorg_b)
            {
                firstBlockJobIdx_u32 = (uint32)rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e;
                lastBlockJobIdx_u32 = (uint32)rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e;
                for(idx_u32 = firstBlockJobIdx_u32; idx_u32 <= lastBlockJobIdx_u32; idx_u32++)
                {
                    if((rba_FeeFs1x_BC_blockCopies_ast[idx_u32].isInitailized_b) &&
                       (rba_FeeFs1x_BC_blockCopies_ast[idx_u32].persID_u16
                               == rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16))
                    {
                        rba_FeeFs1x_BC_initCopyWr_st.validBCWasMovedDuringReorg_b = TRUE;
                    }
                }
            }

            // Prepare space is completed successfully. Set flag to indicate that the block copy is sucessfully created
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].isInitailized_b = TRUE;

            retVal_en = rba_FeeFs1x_JobOK_e;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // Prepare space is ongoing, wait till its completed.
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_compareFls2Fls
 * The function when called triggers byte by byte compare operations for 2 copies
 * present in flash (both the header and data is compared). The function shall be triggered only for same blocks. This
 * function would be called during maintain to check if the 2 copies are identical or not.
 *
 * \param   copy1_en    : First copy in DFLASH
 * \param   copy2_en    : Second copy in DFLASH
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_compareFls2Fls(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy1_en, rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy2_en)
{
    uint16 nrBytes_u16;

    nrBytes_u16 = rba_FeeFs1x_BC_blockCopies_ast[copy1_en].blkLength_u16 +
                    (uint16)rba_FeeFs1x_BC_HdrPgValueMap_datCRC3_e;

    /* Compare the two copies */
    rba_FeeFs1x_PASrv_compareFls2Fls(
            rba_FeeFs1x_BC_blockCopies_ast[copy1_en].pageNr_u32,
            rba_FeeFs1x_BC_blockCopies_ast[copy2_en].pageNr_u32,
            0,
            nrBytes_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_compareFls2FlsDo
 * This function polls and return the status of the flash to flash compare operation.
 *
 * \return  compare finished?
 * \retval  rba_FeeFs1x_Pending_e   : comparision ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e     : comparision completed and data was same
 * \retval  rba_FeeFs1x_JobFailed_e : comparision completed but data was different
 * \retval  rba_FeeFs1x_ErrorInternal_e     : processing error, validation wasn't completed
 * \retval  rba_FeeFs1x_ErrorExternal_e : component external error, e.g. Fls rejected the job
 * \seealso rba_FeeFs1x_BC_compareBuffer2Copy
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_compareFls2FlsDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    retVal_en = rba_FeeFs1x_PASrv_compareFls2FlsDo();

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_compare2Config
 * This function compares the DFLASH status byte against the PFLASH status bytes.
 *
 * \param   copy_en              : Copy in DFLASH
 * \param   confStatusByte_u16   : Status bytes
 * \return  isMatchingConfig_pb  : status of the comparision is returned in this pointer.
 *                                 TRUE = status bytes are same.
 *                                 FALSE = status bytes are different.
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BC_compare2Config(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en , uint16 confStatusByte_u16)
{
    boolean isMatchingConfig_pb;
    uint8   dflashstatusByte_u8;
    uint8   confStatusByte_u8;

    // only the lower 8 bits of the config shall be compared
    dflashstatusByte_u8 = (uint8)rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16;

    // Mask the Invalidate and Ignore flags for comparision (as these flag are set only in DFLASH)
    dflashstatusByte_u8 &= ~RBA_FEEFS1X_PRV_CFG_IGNORE_BITS;
    dflashstatusByte_u8 &= ~RBA_FEEFS1X_PRV_CFG_INVALIDATE_BITS;

    confStatusByte_u8 = (uint8)confStatusByte_u16;

    // Check if the status flags are same
    if(dflashstatusByte_u8 == confStatusByte_u8)
    {
        // Status flags are same
        isMatchingConfig_pb = TRUE;
    }
    else
    {
        // Status flags are different
        isMatchingConfig_pb = FALSE;
    }

    return (isMatchingConfig_pb);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_compareBuffer2Copy
 * This function compares the user buffer with the DFLASH copy.
 *
 * \param   copy_en     : Copy in DFLASH
 * \param   buffer_pu8  : Pointer to user buffer
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_compareBuffer2Copy(uint8 const * buffer_pu8, rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copyInFlash_en)
{
    uint32 startPage_u32;
    uint16 nrBytes_u16;
    uint8  startOffset_u8;

    // Calculate the first requested page where the data starts
    startPage_u32 = rba_FeeFs1x_BC_blockCopies_ast[copyInFlash_en].pageNr_u32 +
            (RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR - 1u);

    // Calculate the start offset for the data bytes
    startOffset_u8 = ((uint8)rba_FeeFs1x_BC_HdrPgValueMap_dat0_e % RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);

    // Complete block length has to be compared
    nrBytes_u16 = rba_FeeFs1x_BC_blockCopies_ast[copyInFlash_en].blkLength_u16;

    // trigger the compare operation
    rba_FeeFs1x_PASrv_compare(startPage_u32, startOffset_u8, nrBytes_u16, buffer_pu8);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_compareBuffer2CopyDo
 * This function polls and return the status of the user buffer to flash compare operation.
 *
 * \return  compare finished?
 * \retval  rba_FeeFs1x_Pending_e       : comparison ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : comparison completed and data was same
 * \retval  rba_FeeFs1x_JobFailed_e     : difference between RAM and flash data
 * \retval  rba_FeeFs1x_ErrorInternal_e : processing error, validation wasn't completed
 * \seealso rba_FeeFs1x_BC_compareBuffer2Copy
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_compareBuffer2CopyDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    retVal_en = rba_FeeFs1x_PASrv_compareDo();

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_copyObjectInternal
 * Function for copying array elements of rba_FeeFs1x_BC_blockCopies_ast from rba_FeeFs1x_BC_Cpy_Internal_e index to
 * passed destination BC.
 * \param   destCopy_en : destination of the object duplication
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_copyObjectInternal(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten destCopy_en)
{
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten srcCopy_en = rba_FeeFs1x_BC_Cpy_Internal_e;

    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].persID_u16 =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].persID_u16;

    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].blkLength_u16 =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].blkLength_u16;

    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].pageNr_u32 =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].pageNr_u32;

    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].HdrCrc_u16 =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].HdrCrc_u16;


    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].dataCrc_u32 =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].dataCrc_u32;

    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].first2DatB_au8[0u] =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].first2DatB_au8[0u];

    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].first2DatB_au8[1u] =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].first2DatB_au8[1u];


    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].isInitailized_b =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].isInitailized_b;

    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].statusByte_u16 =
            rba_FeeFs1x_BC_blockCopies_ast[srcCopy_en].statusByte_u16;

}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_destroy
 * This function is to be used for no fall back blocks only and is used to mark
 * the old copies of the block as no longer valid copies. This is achieved by
 * writing the no fall back marker before the header page of the block copy.
 *
 * \param   Copy_en     : Copy for the invaldiate operation
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_destroy(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    if(rba_FeeFs1x_Prv_isNoFallbackActive())
    {
        // write the no fall back marker which is one page before the header page
        rba_FeeFs1x_PAWrite_write
        (
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32 - 1u ,
            RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE,
            rba_FeeFs1x_NoFallbackMarkerPattern_au8,
            FALSE,
            FEE_RB_ALLJOBS_ALLSTEPS_E
        );
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_destroyDo
 * This function checks and returns the status of the no fall back marker write
 * triggered in rba_FeeFs1x_BlockCopy_destroy. It also finalizes the corresponding block copy (mark it as no longer valid)
 *
 * \return  Returns status of the destroy operation
 * \retval  rba_FeeFs1x_Pending_e       : destory ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : block copy destroyed sucessfully
 * \retval  rba_FeeFs1x_JobFailed_e     : block copy was already destroyed
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during destorying of the block copy
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_destroyDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(rba_FeeFs1x_Prv_isNoFallbackActive())
    {
        retVal_en = rba_FeeFs1x_PAWrite_writeDo();
    }
    else
    {
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }

    return  (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_invalidate
 * This function writes an invaldiate copy of the block copy into DFLASH.
 *
 * \param   copy_en    : copy for the invaldiate operation
 * \param   jobMode_en : mode in which the current job has to be performed
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_invalidate(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en, Fee_Rb_JobMode_ten jobMode_en)
{
    // store parameters to working variable
    rba_FeeFs1x_BC_invalidate_st.copy_en = copy_en;
    rba_FeeFs1x_BC_invalidate_st.jobMode_en = jobMode_en;
    rba_FeeFs1x_BC_invalidate_st.state_en = rba_FeeFs1x_BC_invalidateDoStates_prepareHdr_e;
    rba_FeeFs1x_BC_invalidate_st.entry_b  = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_invalidateDo
 * This is the do function for invaldiate operation.
 * \return  Returns status of the invalidate storage operation
 * \retval  rba_FeeFs1x_Pending_e       : Storage ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : Data stored sucessfully
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during storage of the data
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_invalidateDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_BC_invalidate_st.state_en)
    {
        case rba_FeeFs1x_BC_invalidateDoStates_prepareHdr_e:
        {
            retVal_en = rba_FeeFs1x_BC_invalidateDo_prepareHdr(&rba_FeeFs1x_BC_invalidate_st);
            break;
        }

        case rba_FeeFs1x_BC_invalidateDoStates_wrHdr_e:
        {
            retVal_en = rba_FeeFs1x_BC_invalidateDo_wrHdr(&rba_FeeFs1x_BC_invalidate_st);
            break;
        }

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    if(rba_FeeFs1x_Pending_e != retVal_en)
    {
        rba_FeeFs1x_BC_invalidate_st.state_en = rba_FeeFs1x_BC_invalidateDoStates_idle_e;
        rba_FeeFs1x_BC_invalidate_st.jobMode_en = FEE_RB_ALLJOBS_ALLSTEPS_E;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_invaildateDo_prepareHdr
 * Statefunction of rba_FeeFs1x_BC_invalidateDo. It prepares the header page and stores it in an internal buffer.
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : Prepartion of header finished
 * \retVal  no other return value is possible in this function.
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_invalidateDo_prepareHdr(rba_FeeFs1x_BC_invalidate_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en; // local variable to speed up the access for the array
    uint8 idx_u8;

    copy_en = fsm_pst->copy_en;

    // set the invaldiate bit in the status flag
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 |= RBA_FEEFS1X_PRV_CFG_INVALIDATE_BITS;

    // compute and store the header CRC in the blockcopy.
    // DataCRC was already filled in the write request rba_FeeFs1x_BC_wrRam2Fls
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16 = rba_FeeFs1x_BC_calcBlkHdrCRC(
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16,
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16,
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16);

    // The data bytes in the header are filled with the inverse erase pattern
    for(idx_u8 = 0; idx_u8 < RBA_FEEFS1X_DATABYTES_IN_HDR; idx_u8++)
    {
        rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8[idx_u8]=(uint8)(~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT);
    }

    // Set the data CRC to the init value
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32 =
                                RBA_FEEFS1X_BC_DATACRC_STARTVALUE((rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16));

    // update the internal buffer with the complete header information
    rba_FeeFs1x_BC_fillToHdrBuffer(copy_en);

    // No data pages programming required, directly program the header page
    FEE_PRV_LIBSC(rba_FeeFs1x_BC_invalidateDoStates_wrHdr_e);

    // Set return value to OK as there is no other possible in this function
    retVal_en = rba_FeeFs1x_Pending_e;

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_invdalidateDo_wrHdr
 * Statefunction of rba_FeeFs1x_BC_invdalidateDo. This function writes the block header to dflash.
 * that is programmed to the memory.
 * \param   fsm_pst                         : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e           : Header page write ongoing
 * \retVal  rba_FeeFs1x_ErrorInternal_e     : Internal error during writing of header page
 * \retVal  rba_FeeFs1x_ErrorExternal_e     : Error returned by PA
 * \retVal  rba_FeeFs1x_JobOK_e             : Header page write was completed sucessfully
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_invalidateDo_wrHdr(rba_FeeFs1x_BC_invalidate_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                  retVal_en;        // return value of this state function

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // Trigger write of the header pages
        rba_FeeFs1x_PAWrite_write
                (
                        rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].pageNr_u32 ,
                        (RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE * RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR),
                        rba_FeeFs1x_BC_buf_au8,
                        TRUE,
                        fsm_pst->jobMode_en
                );
    }

    // Cyclic call the do-function
    retVal_en = rba_FeeFs1x_PAWrite_writeDo();

    //rba_FeeFs1x_PAWrite_writeDo will never return rba_FeeFs1x_JobFailed_e -> return of inner to outer state possible
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_finalizeCopy
 * This function is called to finalize a block copy i.e., to mark it as no longer
 * valid. It internally sets all the elements to default value and sets
 * isInitailized_b = FALSE. *
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16      = RBA_FEEFS1X_MAXUINT16;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16   = 0u         ;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32      = 0uL         ;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16      = 0u         ;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32     = 0uL         ;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].isInitailized_b     = FALSE       ;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16   = 0u        ;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8[0u] = 0u;
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8[1u] = 0u;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_extractHdrSync_CopyContent
 * copies the first header page to the internal block copy object
 * \param   adrPageInBuffer_pcu8 : read pointer
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_extractHdrSync_CopyContent(uint8 const * adrPageInBuffer_pcu8)
{
    rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8 = adrPageInBuffer_pcu8;

    // extract the block data from pagebuffer to the block copy
    rba_FeeFs1x_BC_fill1stHdrPgInBC(rba_FeeFs1x_BC_Cpy_Internal_e);

    // mark the copy as present in DFLASH though the full validation is not yet completed but
    // would be required for accessing the persistent index by the cache build up function
    rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].isInitailized_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_extractHdrSync_UpdatePage
 * This function initializes the check for the presence of a preamble in a page.
 * \param   nrPage_u32           : processed page number
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_extractHdrSync_UpdatePage(uint32 nrPage_u32)
{
    // set the copie's page number to the value given to the extraction job
    rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].pageNr_u32 = nrPage_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_extractHdr_CopyContentAndUpdatePageNr
 * Stores the page number of the block header and copies the first header page to the internal block copy object
 * \param   adrPageInBuffer_pcu8 : read pointer
 * \param   nrPage_u32           : processed page number
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_BC_extractHdr_CopyContentAndUpdatePageNr(uint8 const * adrPageInBuffer_pcu8, uint32 nrPage_u32)
{
    rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8 = adrPageInBuffer_pcu8;

    // extract the block data from pagebuffer to the block copy
    rba_FeeFs1x_BC_fill1stHdrPgInBC(rba_FeeFs1x_BC_Cpy_Internal_e);

    rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].pageNr_u32 =
            nrPage_u32;

    // mark the copy as present in DFLASH though the full validation is not yet completed but
    // would be required for accessing the persistent index by the cache build up function
    rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].isInitailized_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_extractHdr
 * This function initializes the check for the presence of a preamble in a page.
 * Hdr Validation is split into extract and validate
 * \param   page_u32 : page number to be checked
 * \return  void
 * \seealso rba_FeeFs1x_BC_extractHdrDo
 * \seealso rba_FeeFs1x_BC_validateHdr
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_extractHdr(uint32 page_u32)
{
    rba_FeeFs1x_PARead_getPages(page_u32,
                                RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR,
                                &(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8) );
    // Page read accepted, extit for extractDo execution

    // store the current state into the job struct
    rba_FeeFs1x_BC_extractNValHdr_st.page_u32 = page_u32;
    rba_FeeFs1x_BC_extractNValHdr_st.isExtractActive_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_extractHdrDo
 * This is the do function for extracting the header triggered by
 * rba_FeeFs1x_BlockCopy_extractHeader(void). Once a preamble is found, it then also
 * creates an object and fills in the information into this object.
 * \return  job result; page contained a hdr, didn't or read error
 * \retval  rba_FeeFs1x_JobOK_e : page contains a preamble; data is copied to BC_Cpy_Internal
 * \rba_FeeFs1x_JobFailed_e:  page doesn't contain a preamble
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e : Error during read or unexpected states
 * \seealso rba_FeeFs1x_BC_extractHdr
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_extractHdrDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValGetPageDo_en;

    uint32 PageContent_u32;


    if(rba_FeeFs1x_BC_extractNValHdr_st.isExtractActive_b)
    {

        retValGetPageDo_en = rba_FeeFs1x_PARead_getPagesDo();


        switch (retValGetPageDo_en)
        {
            case rba_FeeFs1x_JobOK_e:
            {
                // read finished successfully
                uint32 PreamblePattern_u32;

                PreamblePattern_u32 =
                        ((uint32)(RBA_FEEFS1X_BC_PREAMBEL_BYTE_0)        )|
                        ((uint32)(RBA_FEEFS1X_BC_PREAMBEL_BYTE_1) << 8uL )|
                        ((uint32)(RBA_FEEFS1X_BC_PREAMBEL_BYTE_2) << 16uL)|
                        ((uint32)(0uL)                            << 24uL);

                PageContent_u32 =
                        ((uint32) (rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_preamble0_e])        )|
                        ((uint32) (rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_preamble1_e]) << 8uL )|
                        ((uint32) (rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_preamble2_e]) << 16uL)|
                        ( 0uL                                                                                                       << 24uL);

                // check the buffer for presence of a preamble
                if(PageContent_u32 == PreamblePattern_u32)
                {

                    // copy the data from DFLASH into the BC internal block copy
                    rba_FeeFs1x_BC_extractHdr_CopyContentAndUpdatePageNr(
                            rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8 ,
                            rba_FeeFs1x_BC_extractNValHdr_st.page_u32);

                    // page contains a preamble
                    rba_FeeFs1x_BC_extractNValHdr_st.isExtractActive_b = FALSE;

                    retVal_en = rba_FeeFs1x_JobOK_e;
                }
                else
                {
                    // page doesn't contain a preamble
                    retVal_en = rba_FeeFs1x_JobFailed_e;
                }
                break;
            }
            case rba_FeeFs1x_ErrorExternal_e:
            {
                // read finished with an error, return this read error
                retVal_en = rba_FeeFs1x_ErrorExternal_e;

                // reset the job data
                rba_FeeFs1x_BC_extractNValHdr_st.page_u32 = 0uL;
                rba_FeeFs1x_BC_extractNValHdr_st.isExtractActive_b = FALSE;
                break;
            }
            case rba_FeeFs1x_Pending_e:
            {
                // read pending
                retVal_en = rba_FeeFs1x_Pending_e;
                break;
            }
            default:
            {
                // unexpected return value
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
                break;
            }
        }
    }
    else
    {
        // wrong function call sequence
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }

    return  retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_BC_fill1stHdrPgInBC
 * Copies the data of the first 8B chunk of a header from the internal buffer to the passed block copy object
 * For 16B pages, the extraction also should include the second 8B chunk
 *
 * \param   copy_en : copy object to be processed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_BC_fill1stHdrPgInBC(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 =
            rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_statusB_e];

    // Extract the persistent ID out of the read buffer
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16 =
            ( ( (uint16) (rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_persID0_e]) ) << 8u) |
            ((uint16)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_persID1_e])) ;

    // extract the block length out of the read buffer
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16 =
            (((uint16)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_len0_e])) << 8u) |
            ((uint16)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_len1_e])) ;

}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_fill2ndHdrPgInBC
 * Copies the data of the second 8B chunk of a header from the internal buffer to the passed block copy object
 *
 * \param   copy_en : copy object to be processed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_BC_fill2ndHdrPgInBC(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16 =
            ( (uint16)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_hdrCRC0_e]) << 8uL ) |
            ( (uint16)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_hdrCRC1_e]) );

    // Data CRC is not required during cache buildup but at later header validations
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32 =
            ( (uint32)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC0_e]) << 24uL ) |
            ( (uint32)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC1_e]) << 16uL ) |
            ( (uint32)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC2_e]) <<  8uL ) |
            ( (uint32)(rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC3_e])         ) ;

    // Also store the first data bytes in RAM
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8[0uL] =
            rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_dat0_e];
    rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8[1uL] =
            rba_FeeFs1x_BC_extractNValHdr_st.adrPageInBuffer_pcu8[rba_FeeFs1x_BC_HdrPgValueMap_dat1_e];
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_fillToHdrBuffer
 * Copies the data of the complete header from the passed block copy object to internal header buffer
 *
 * \param   copy_en : copy object to be processed
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_BC_fillToHdrBuffer(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    // Fill the preamable in the buffer
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_preamble0_e] = RBA_FEEFS1X_BC_PREAMBEL_BYTE_0;
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_preamble1_e] = RBA_FEEFS1X_BC_PREAMBEL_BYTE_1;
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_preamble2_e] = RBA_FEEFS1X_BC_PREAMBEL_BYTE_2;

    // Fill thes status bytes
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_statusB_e] =
                                                (uint8)(rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 & 0xFFu);

    // Fill the persistent ID
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_persID0_e] =
                                                (uint8)(rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16 >> 8u);
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_persID1_e] =
                                                (uint8)(rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16 & 0xFFu);

    // Fill the length
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_len0_e] =
                                                (uint8)(rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16 >> 8u);
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_len1_e] =
                                                (uint8)(rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16 & 0xFFu);

    // Store the header CRC to the header buffer
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_hdrCRC0_e] =
            (uint8)((rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16 >> 8u) & 0xFFu );
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_hdrCRC1_e] =
            (uint8) (rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16 & 0xFFu);

    // Store the Data CRC to the header buffer
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC0_e] =
            (uint8) ( (rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32 >> 24u) & 0xFFu );
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC1_e] =
            (uint8) ( (rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32 >> 16u) & 0xFFu );
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC2_e] =
            (uint8) ( (rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32 >> 8u) & 0xFFu );
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC3_e] =
            (uint8) (rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32 & 0xFFu);

    // Also store the first data bytes to the header buffer
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_dat0_e] =
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8[0u];
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_dat1_e] =
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8[1u];
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_fillToHdrBuffer
 * The function updates the data CRC into the header buffer
 *
 * \param   dataCrc_u32 : new data crc value
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_BC_updateDataCrcInHdrBuffer(uint32 dataCrc_u32)
{
    // Store the Data CRC to the header buffer
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC0_e] = (uint8) ( (dataCrc_u32 >> 24u) & 0xFFu );
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC1_e] = (uint8) ( (dataCrc_u32 >> 16u) & 0xFFu );
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC2_e] = (uint8) ( (dataCrc_u32 >> 8u)  & 0xFFu );
    rba_FeeFs1x_BC_buf_au8[rba_FeeFs1x_BC_HdrPgValueMap_datCRC3_e] = (uint8) ( (dataCrc_u32      )  & 0xFFu);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_validateHdr
 * This function when called validates that was read out in
 * rba_FeeFs1x_BlockCopy_extractHeader(void). It internally triggers read of 2nd
 * header page from the medium using iterator if required.
 * Always works on internal block copy!
 * \param   isCacheBuildup_b : shall fully programmed check be executed
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_validateHdr(boolean isCacheBuildup_b)
{
    rba_FeeFs1x_BC_extractNValHdr_st.state_en = rba_FeeFs1x_BC_extractNValHdrStates_firstChk_e;
    rba_FeeFs1x_BC_extractNValHdr_st.entry_b = TRUE;
    // Due to extensive runtime, in case of cache build up is ongoing during startup fully programmed check is skipped
    rba_FeeFs1x_BC_extractNValHdr_st.isCacheBuildup_b = isCacheBuildup_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_validateHdrDo
 * This is the do function for validating the header triggered by
 * rba_FeeFs1x_BlockCopy_validateHeader(void). Once a valid copy is found, it then
 * also creates an object and fills in the information into this object.
 * \return  Returns whether the extracted Hdr
 * \retval  rba_FeeFs1x_Pending_e : call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e : block header valid
 * \retval  rba_FeeFs1x_JobFailed_e : block header not valid
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal error e.g. OOB
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_BC_extractNValHdr_st.state_en)
    {
        case rba_FeeFs1x_BC_extractNValHdrStates_firstChk_e:
        {
            retVal_en = rba_FeeFs1x_BC_validateHdrDo_firstChecks(&rba_FeeFs1x_BC_extractNValHdr_st);
            break;
        }

        case rba_FeeFs1x_BC_extractNValHdrStates_crc_e:
        {
            retVal_en = rba_FeeFs1x_BC_validateHdrDo_calcCRC(&rba_FeeFs1x_BC_extractNValHdr_st);
            break;
        }

        case rba_FeeFs1x_BC_extractNValHdrStates_fullyProg_e:
        {
            retVal_en = rba_FeeFs1x_BC_validateHdrDo_checkFullyProg(&rba_FeeFs1x_BC_extractNValHdr_st);
            break;
        }
        case rba_FeeFs1x_BC_extractNValHdrStates_NoFBPatternChk_e:
        {
            retVal_en = rba_FeeFs1x_BC_validateHdrDo_checkNoFBPattern(&rba_FeeFs1x_BC_extractNValHdr_st);
            break;
        }
        case rba_FeeFs1x_BC_extractNValHdrStates_NoFBdestr_e:
        {
            retVal_en = rba_FeeFs1x_BC_validateHdrDo_checkNoFBDestr(&rba_FeeFs1x_BC_extractNValHdr_st);
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    if(rba_FeeFs1x_Pending_e != retVal_en)
    {
        rba_FeeFs1x_BC_extractNValHdr_st.state_en = rba_FeeFs1x_BC_extractNValHdrStates_idle_e;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_validateHdrDo_firstChecks
 * Statefunction of validateHdrDo. evaluates the IGNORE bit and executes a length comparison
 * \param   fsm_pst : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e :  call valHdrDo again in next cycle - Either this state needs more time
 *                                   or state transition
 * \retVal  rba_FeeFs1x_JobFailed_e: valHdrDo result detected as non-valid Hdr
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_firstChecks(rba_FeeFs1x_BC_extractNValHdr_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    boolean hasIgnoreBitActive_b;

    rba_FeeFs1x_BC_fill2ndHdrPgInBC(rba_FeeFs1x_BC_Cpy_Internal_e);

    hasIgnoreBitActive_b = rba_FeeFs1x_BC_getCpyIgnoreBit(rba_FeeFs1x_BC_Cpy_Internal_e);

    if(!hasIgnoreBitActive_b)
    {
        // the copy is not to be ignored

        // Continue with reading out the second header page for Hdr CRC comparison
        FEE_PRV_LIBSC(rba_FeeFs1x_BC_extractNValHdrStates_crc_e);

        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // block is to be ignored --> header is invalid
        retVal_en = rba_FeeFs1x_JobFailed_e;
    }
    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_BC_extract_isInternalCpyLengthMatching
 * validateHdrDo_firstChecks subfunction for comparing the length of known blocks against the config.
 * \param   nrBytesExpectedLength_u16 : expected length
 * \return  boolean
 * \retval  TRUE: expected length is matching
 * \retVal  FALSE: expected length is not matching
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BC_extract_isInternalCpyLengthMatching(uint16 nrBytesExpectedLength_u16)
{
    boolean retVal_b;

    uint16 copyLength_u16;
    boolean copyHasRobCRCActive_b;

    // Check whether a length comparison against config is required: not invalidated and known block
    if(!rba_FeeFs1x_BC_getCpyInvBitActive(rba_FeeFs1x_BC_Cpy_Internal_e))
    {

        copyLength_u16        = rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].blkLength_u16;
        copyHasRobCRCActive_b = rba_FeeFs1x_BC_getCpyLegacyRobCRCBit(rba_FeeFs1x_BC_Cpy_Internal_e);

        // for known blocks, determine the correctness of the length based on the robust CRC bit configuration
        // in config and the current block copy
        if(copyHasRobCRCActive_b)
        {
            // Both copy and config have the RobCRC Bit active
            //--> but legacy config bit on DFLASH will have 4 additional bytes for ROB crc
            retVal_b = ( (copyLength_u16 - RBA_FEEFS1X_BC_NR_ROBCRCBYTES) == nrBytesExpectedLength_u16);
        }
        else
        {
            // Configured, but disabled on the copy
            //--> this could mean that the copy in DFLASH is also robustness active -> length should match
            retVal_b = ( copyLength_u16 == nrBytesExpectedLength_u16);
        }
    }
    else
    {
        retVal_b = TRUE;
    }
    return retVal_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_validateHdrDo_calcCRC
 * Statefunction of validateHdrDo. calculates the HdrCRC and compares it against the stored one
 * \param   fsm_pst : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e : call valHdrDo again in next cycle - Either this state needs more time
 *                                  or state transition
 * \retVal  rba_FeeFs1x_JobFailed_e : valHdrDo result detected as non-valid Hdr
 * \retVal  rba_FeeFs1x_JobOK_e : valHdrDo result detected as valid Hdr
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_calcCRC(rba_FeeFs1x_BC_extractNValHdr_tst * const  fsm_pst)
{

    rba_FeeFs1x_RetVal_ten retVal_en;

    uint16 CalculatedHdrCRC_u16;
    uint16 StoredHdrCRC_u16;

    // call the CRC16 calculation function with equal parameters to the legacy FeeFs1
    CalculatedHdrCRC_u16 = Crc_CalculateCRC16(
            &(fsm_pst->adrPageInBuffer_pcu8[0uL])   ,
            RBA_FEEFS1X_BC_HDRCRC_NRBYTES    ,
            RBA_FEEFS1X_BC_HDRCRC_STARTVALUE ,
            FALSE);

    StoredHdrCRC_u16 = rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].HdrCrc_u16;

    if(CalculatedHdrCRC_u16 == StoredHdrCRC_u16)
    {
        // CRC is valid; continue with checking the last hdr page for fully programmed state

        // 2 cases, where fully programmed check is not done
        //    1: in the initialization during cache build to reduce initialization time
        //    2: on devices with non meaningful blank check (blank check done by comparing the content of the header
        //       page to the erase pattern). In that case, if content of the header page == erase pattern, a valid
        //       header will be detected as blank -> fallback to old data or loss of data occurs
        if((!fsm_pst->isCacheBuildup_b) && rba_FeeFs1x_Prv_isBlankChkRequired())
        {
            // switch to blank check, if blank check is feasible
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_extractNValHdrStates_fullyProg_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            if(rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_Internal_e))
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_BC_extractNValHdrStates_NoFBPatternChk_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
        }
    }
    else
    {
        // The stored and calculated CRCs mismatch --> invalid header
        retVal_en = rba_FeeFs1x_JobFailed_e;
    }
    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_BC_validateHdrDo_checkFullyProg
 * Statefunction of validateHdrDo. blankchecks the last header page to detect ghost copies
 * \param   fsm_pst : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e:       call valHdrDo again in next cycle - Either this state needs more time
 *                                       or state transition
 * \retVal  rba_FeeFs1x_JobFailed_e:     valHdrDo result detected as non-valid Hdr
 * \retVal  rba_FeeFs1x_ErrorInternal_e: internal error e.g OOB
 * \retVal  rba_FeeFs1x_JobOK_e:         valHdrDo result detected as valid Hdr
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_checkFullyProg(rba_FeeFs1x_BC_extractNValHdr_tst  * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_PASrv_BlankCheckResult_ten retValChkFullyProg_en;

    if(rba_FeeFs1x_Prv_isBlankChkRequired())
    {
        if(FEE_PRV_LIBENTRY)
        {
            // first entry of the state

            // execute a fullyProgrammed check onto the last header page
            uint32 lastHdrPage_u32 =
                    rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].pageNr_u32 +
                    (RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR - 1uL);

            rba_FeeFs1x_PASrv_checkPageFullyProgrammed(lastHdrPage_u32);
        }

        // Cyclic call the Do function
        retValChkFullyProg_en = rba_FeeFs1x_PASrv_checkPageFullyProgrammedDo();


        // Evaluate the blankcheck result
        switch(retValChkFullyProg_en)
        {
            case rba_FeeFs1x_PASrv_BlankCheckResult_fullyProgrammed_e:
            {
                // Block header is fully written --> valid header

                // If the copy now is a noFallback block, check the NoFallback header page
                // For all non-NoFallback blocks, the copy is valid

                if(rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_Internal_e))
                {
                    FEE_PRV_LIBSC(rba_FeeFs1x_BC_extractNValHdrStates_NoFBPatternChk_e);
                    retVal_en = rba_FeeFs1x_Pending_e;
                }
                else
                {
                    retVal_en = rba_FeeFs1x_JobOK_e;
                }
                break;
            }
            case rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e:
            case rba_FeeFs1x_PASrv_BlankCheckResult_blank_e:
            {
                // second header page is not written completely --> copy is ghost
                retVal_en = rba_FeeFs1x_JobFailed_e;
                break;
            }
            case rba_FeeFs1x_PASrv_BlankCheckResult_pending_e:
            {
                // on pending, call the do-function in the next cycle again
                retVal_en = rba_FeeFs1x_Pending_e;
                break;
            }
            case rba_FeeFs1x_PASrv_BlankCheckResult_error_e:
            default:
            {
                // error during blankcheck or unexpected state --> exit the job with error
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
                break;
            }
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
 * rba_FeeFs1x_BC_validateHdrDo_checkNoFBPattern
 * Statefunction of validateHdrDo. pattern check for the noFB header page to detect if the copy is destroyed or not
 * \param   fsm_pst : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e       : call valHdrDo again in next cycle - Either this state needs more time
 *                                        or state transition
 * \retVal  rba_FeeFs1x_JobFailed_e     : valHdrDo result detected as non-valid Hdr
 * \retVal  rba_FeeFs1x_ErrorInternal_e : internal error e.g OOB
 * \retVal  rba_FeeFs1x_JobOK_e         : valHdrDo result detected as valid Hdr
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_checkNoFBPattern(rba_FeeFs1x_BC_extractNValHdr_tst  * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    boolean                retValCompare_b;
    rba_FeeFs1x_RetVal_ten retValGetPageDo_en;

    if(FEE_PRV_LIBENTRY)
    {
        // first entry of the state

        // execute a noFB pattern check onto the page before the first header page.
        // There is no need to check if this page is at sector boundary, as this is already ensured by write operation
        // that the first page before the block is left blank.
        uint32 noFbHdrPage_u32 =
                rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].pageNr_u32 - 1uL;

        rba_FeeFs1x_PARead_getPages(noFbHdrPage_u32, 1uL, &(fsm_pst->adrPageInBuffer_pcu8));
    }

    // Cyclic call the Do-function
    retValGetPageDo_en = rba_FeeFs1x_PARead_getPagesDo();

    // Evaluate the read result
    switch(retValGetPageDo_en)
    {
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = retValGetPageDo_en;
        }
        break;

        // read was sucessful
        case rba_FeeFs1x_JobOK_e:
        {
            // check if the noFB header page has pattern equal to noFB marker or not
            retValCompare_b = rba_MemLib_MemCompareBfr( fsm_pst->adrPageInBuffer_pcu8,
                                                        rba_FeeFs1x_NoFallbackMarkerPattern_au8,
                                                        RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE );

            if(retValCompare_b)
            {
                // data is same, perform blank check to verify if the page is really programmed or is GHOST
                FEE_PRV_LIBSC(rba_FeeFs1x_BC_extractNValHdrStates_NoFBdestr_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                // data is different, no need to perform blank check, return that the copy is not invalidated
                // i.e., copy is valid
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            break;
        }

        default:
        {
            // error during read or unexpected state --> exit the job with error
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }


    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_validateHdrDo_checkNoFBDestr
 * Statefunction of validateHdrDo. blankchecks the noFB header page to detect destroyed copies as invalid
 * \param   fsm_pst : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e:       call valHdrDo again in next cycle - Either this state needs more time
 *                                       or state transition
 * \retVal  rba_FeeFs1x_JobFailed_e:     valHdrDo result detected as non-valid Hdr
 * \retVal  rba_FeeFs1x_ErrorInternal_e: internal error e.g OOB
 * \retVal  rba_FeeFs1x_JobOK_e :        valHdrDo result detected as valid Hdr
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_validateHdrDo_checkNoFBDestr(rba_FeeFs1x_BC_extractNValHdr_tst  * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                 retVal_en;
    rba_FeeFs1x_PASrv_BlankCheckResult_ten retValChkFullyProg_en;

    if(FEE_PRV_LIBENTRY)
    {
        // first entry of the state

        // execute a fullyProgrammed check onto the page before the first header page
        uint32 noFbHdrPage_u32 =
                rba_FeeFs1x_BC_blockCopies_ast[rba_FeeFs1x_BC_Cpy_Internal_e].pageNr_u32 - 1uL;

        rba_FeeFs1x_PASrv_blankcheckPage(noFbHdrPage_u32);
    }

    // Cyclic call the Do function
    retValChkFullyProg_en = rba_FeeFs1x_PASrv_blankcheckPageDo();

    // Evaluate the blankcheck result
    switch(retValChkFullyProg_en)
    {
        case rba_FeeFs1x_PASrv_BlankCheckResult_notFullyProgrammed_e:
        case rba_FeeFs1x_PASrv_BlankCheckResult_fullyProgrammed_e:
        {
            // NoFB header is written --> destroyed header --> return hdr invalid
            retVal_en = rba_FeeFs1x_JobFailed_e;

            break;
        }

        case rba_FeeFs1x_PASrv_BlankCheckResult_blank_e:
        {
            // NoFB header is not written --> valid copy
            retVal_en = rba_FeeFs1x_JobOK_e;
            break;
        }
        case rba_FeeFs1x_PASrv_BlankCheckResult_pending_e:
        {
            // on pending, call the do-function in the next cycle again
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_PASrv_BlankCheckResult_error_e:
        default:
        {
            // error during blankcheck or unexpected state --> exit the job with error
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getDataCRC
 * The function returns the data CRC value of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \return  dataCrc value
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_BC_getDataCRC(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return(rba_FeeFs1x_BC_blockCopies_ast[copy_en].dataCrc_u32);
}


/**
 *********************************************************************
 * rba_FeeFs1x_BC_getPageNo
 * The function returns the page number of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \param   pageNo_pu32 : callByref return of extracted value
 * \return  Page number
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_BC_getPageNo(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return(rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getPersID
 * The function returns the persistent ID of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \return  persID_pu16 of the requested block copy object
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint16 rba_FeeFs1x_BC_getPersID(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return(rba_FeeFs1x_BC_blockCopies_ast[copy_en].persID_u16);

}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getCpyRedBit
 * The function returns the redundant bit of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \param   hasRedBitActive_pb : callByref return of extracted value
 * \return  is redundnacy bit Active: TRUE : redundnacy bit Active
 *                                    FALSE: redundnacy bit not active
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BC_getCpyRedBit(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    boolean hasRedBitActive_b;
    // Mask the statusbyte with the one for the redundant bit and return (call by reference) whether the bit is set
    hasRedBitActive_b = (rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 &
            RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS) > 0uL;
    return (hasRedBitActive_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getCpyIgnoreBit
 * The function returns the ignore bit of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \return  value of the ignore bit
 * \retval  TRUE : ignore bit is set
 * \retval  FALSE: ignore bit is not set
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_BC_getCpyIgnoreBit(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return (rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 &
            RBA_FEEFS1X_PRV_CFG_IGNORE_BITS) > 0uL;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getCpyLegacyRobCRCBit
 * The function returns the robCRC bit of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \return  value of the robCRC bit
 * \retval  TRUE : robCRC bit is set
 * \retval  FALSE: robCRC bit is not set
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean rba_FeeFs1x_BC_getCpyLegacyRobCRCBit(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return (rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 &
            RBA_FEEFS1X_PRV_CFG_LEGACY_ROB_BITS) > 0uL;
}
/**
 *********************************************************************
 * rba_FeeFs1x_BC_getCpyNewRobCRCBit
 * The function returns the new robCRC bit of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \return  value of the robCRC bit
 * \retval  TRUE : robCRC bit is set
 * \retval  FALSE: robCRC bit is not set
 * \seealso
 * \usedresources
 *********************************************************************
 */
static boolean rba_FeeFs1x_BC_getCpyNewRobCRCBit(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return (rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 &
            RBA_FEEFS1X_PRV_CFG_ROBUSTNESS_BITS) > 0uL;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getCpyInvBitActive
 * \param   copy_en : copy object the value shall be returned for
 * \param   hasInvBitActive_pb : callByref return of extracted value
 * \return  is invalidation bit active
 * \retval  TRUE  : job is executed; copy_en in in range and the copy is valid to be accessed
 * \retval  FALSE : job rejected; OOB request or invalid copy object
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BC_getCpyInvBitActive(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return (rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 &
            RBA_FEEFS1X_PRV_CFG_INVALIDATE_BITS) > 0uL;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getBlkLength
 * External function definition for getting the block length
 * \param   copy_en : copy object the value shall be returned for
 * \return  length_pu16 : Block length of the requested block
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint16  rba_FeeFs1x_BC_getBlkLength(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return(rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getBlkDFLASHStatusByte
 * External function definition for getting the block length
 * \param   copy_en                 : copy object the value shall be returned for
 * \param   DFLASHStatusByte_pu8    : callByref return of extracted value
 * \return  DFLASHStatusByte_pu8    : Status byte of the requested block
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint8 rba_FeeFs1x_BC_getBlkDFLASHStatusByte (rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return( (uint8) rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getCpyNoFBBitActive
 * is selected block copy a noFallback block
 * \param   copy_en : copy object the value shall be returned for
 * \param   hasNoFBBitActive_pb : callByref return of block copy type
 * \return  Is no Fallback bit active
 * \retval  TRUE : copy has no fall back bit set
 * \retval  FALSE: copy has no fall back bit not set
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    return (rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 &
            RBA_FEEFS1X_PRV_CFG_NOFALLBACK_BITS) > 0uL;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getCpySurvivalBit
 * The function returns the survival bit of the requested block copy object.
 * \param   copy_en : copy object the value shall be returned for
 * \return  is the survival bit set for this copy?
 * \retval  TRUE : copy has survival bit set
 * \retval  FALSE: copy has survival bit not set
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BC_getCpySurvivalBit(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    boolean isSurvival_pb;
    isSurvival_pb = (rba_FeeFs1x_BC_blockCopies_ast[copy_en].statusByte_u16 &
                                RBA_FEEFS1X_PRV_CFG_SURVIVAL_BITS) > 0uL;
    return isSurvival_pb;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getPageNrOfLastPage
 * Calculates the pageNo of the last page of the copy
 * \param   copy_en : copy object the value shall be returned for
 * \return  uint32: Last Blk Page
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint32 rba_FeeFs1x_BC_getPageNrOfLastPage(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    uint32 nrDataPages_u32;
    uint32 pageNrLastBlkPage_u32;

    // calculate the amount of data pages
    nrDataPages_u32 = rba_FeeFs1x_BC_getNrDataPagesByBlkLen(rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16);

    // return the pageNr of the last page of the block

    // --> pageNr of the HrdPg containing the preamble +
    // (pages used by Hdr - the one containing the preamble) +
    // Amount of dedicated data pages
    pageNrLastBlkPage_u32 =
            rba_FeeFs1x_BC_blockCopies_ast[copy_en].pageNr_u32 +
            (RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR - 1uL) +
            nrDataPages_u32;

    return(pageNrLastBlkPage_u32);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_isBlkOverlappingToNextSector
 * Function which checks if the passed block copy is overlapping to next logical sector or not.
 *
 * \param   copy_en : copy object which has to be checked
 *
 * \return  TRUE    : Block overlaps to next logical sector
 *          FALSE   : Block does not overlap to next logical sector
 * \seealso
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BC_isBlkOverlappingToNextSector(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    boolean retVal_b;
    uint32  firstPgOfBlk_u32, lastPgOfBlk_u32;

    firstPgOfBlk_u32 = rba_FeeFs1x_BC_getPageNo(copy_en);

    lastPgOfBlk_u32 = rba_FeeFs1x_BC_getPageNrOfLastPage(copy_en);

    retVal_b = rba_FeeFs1x_PASrv_isBlkOverlappingToNextSector(firstPgOfBlk_u32, lastPgOfBlk_u32);

    return(retVal_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_reduceOwnPages
 * The function when called reduces all stored page numbers in currently active block copies by the number passed in
 * function parameter. As a safety check, any underflows are checked before reducing the pages and if found the
 * corresponding copy is finalized and the a flag is set to inform the block copy about the failure and retriger the
 * block job starting from search.
 *
 * \param   amountOfPages_u32 : amount of pages to be reduced
 * \return  job performed successfully?
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_reduceOwnPages(uint32 amountOfPages_u32)
{
    uint8_least idx_qu8;

    // it is not required to check for internal copy
    for(idx_qu8 = (uint8_least)rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e;
        idx_qu8 < (uint8_least)rba_FeeFs1x_BC_Cpy_Internal_e;
        idx_qu8++)
    {
        // Check if the block copy is currently active (i.e., is initialised)
        if (rba_FeeFs1x_BC_blockCopies_ast[idx_qu8].isInitailized_b)
        {
            // Block copy is active. Check for underflow before reducing the pages
            if(rba_FeeFs1x_BC_blockCopies_ast[idx_qu8].pageNr_u32 >= amountOfPages_u32)
            {
                // Page is within limit, decrement the page number.
                rba_FeeFs1x_BC_blockCopies_ast[idx_qu8].pageNr_u32 -= amountOfPages_u32;
            }
            else
            {
                // Underflow. finalize copy, as it can't be used anymore, set flag to inform about failure to block job.
                /* MR12 RULE 10.5 VIOLATION: casting is safe here because number of block copies are limited to 9 */
                rba_FeeFs1x_BC_finalizeCopy((rba_FeeFs1x_BC_Cpy_blockCopyObject_ten)idx_qu8);
            }
        }
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrFls2Fls
 * This function transfers the block from one memory location to another. Could be
 * called during sector reorganization or maintain (or during redundant copy write).
 *
 * \param   srcCopy_en       : Source copy for the write operation
 * \param   destCopy_en      : Copy that has to be stored to DFLASH
 * \param   updateStatus_b   : shall the status be updated
 * \param   newStatusByte_u16: new status for the block copy
 * \param   jobMode_en       : mode in which the current job has to be performed
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_wrFls2Fls
                    (rba_FeeFs1x_BC_Cpy_blockCopyObject_ten srcCopy_en,
                    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten destCopy_en,
                    boolean updateStatus_b,
                    uint16 newStatusByte_u16,
                    Fee_Rb_JobMode_ten jobMode_en)
{
        // store parameters to working variable
        rba_FeeFs1x_BC_wrData2Fls_st.srcCopy_en = srcCopy_en;
        rba_FeeFs1x_BC_wrData2Fls_st.destCopy_en = destCopy_en;
        rba_FeeFs1x_BC_wrData2Fls_st.newStatusByte_u16 = newStatusByte_u16;
        rba_FeeFs1x_BC_wrData2Fls_st.updateStatus_b = updateStatus_b;
        rba_FeeFs1x_BC_wrData2Fls_st.state_en = rba_FeeFs1x_BC_wrData2FlsDoStates_prepareHdrFromFls_e;
        rba_FeeFs1x_BC_wrData2Fls_st.jobMode_en = jobMode_en;
        rba_FeeFs1x_BC_wrData2Fls_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrRam2Fls
 * This function writes the RAM copy to DFLASH. The function will write only one copy to DFLASH.
 * For redundant block the Block Job state machine will trigger the write of the second copy.
 * \param copy_en         : Copy to write to DFLASH
 * \param dataCrc_u32     : Data CRC for the copy to be written (was calculated during the data comparision before write
 *                          in Block Job state machine)
 * \param userBuf_pcu8    : Pointer to user buffer from where the data to be written is taken.
 * \param nrDataBytCur_u16: Length of current chunk (for single chunk operation, this will be = complete block length)
 * \param jobMode_en      : mode in which the current job has to be performed
 * \param isChunkJob_b    : TRUE -> job placed by chunk unit, or FALSE -> job placed by order unit
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_wrRam2Fls(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten destCopy_en, uint32 dataCrc_u32,
        uint8 const* userBuf_pcu8, uint16 nrDataBytCur_u16, Fee_Rb_JobMode_ten jobMode_en, boolean isChunkJob_b)
{

    /* If chunk job is ongoing, initialization has already been performed -> go directly to the write state */
    if(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDataPagesFromRam_e != rba_FeeFs1x_BC_wrData2Fls_st.state_en)
    {
        // Either it is first or single chunk write, in both cases start with preparation of header
        rba_FeeFs1x_BC_wrData2Fls_st.state_en = rba_FeeFs1x_BC_wrData2FlsDoStates_prepareHdrFromRam_e;

        // store parameters to working variable
        rba_FeeFs1x_BC_wrData2Fls_st.destCopy_en = destCopy_en;

        // Data CRC is stored in the block copy (all other information were updated during rba_FeeFs1x_BC_initCopyWr)
        rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].dataCrc_u32 = dataCrc_u32;

        rba_FeeFs1x_BC_wrData2Fls_st.jobMode_en = jobMode_en;
        rba_FeeFs1x_BC_wrData2Fls_st.updateStatus_b = FALSE;
        rba_FeeFs1x_BC_wrData2Fls_st.isChunkJob_b = isChunkJob_b;

        rba_FeeFs1x_BC_wrData2Fls_st.nrDataBytDone_u16 = 0u;
    }

    rba_FeeFs1x_BC_wrData2Fls_st.nrDataBytCur_u16 = nrDataBytCur_u16;
    rba_FeeFs1x_BC_wrData2Fls_st.buf_pcu8 = userBuf_pcu8;
    rba_FeeFs1x_BC_wrData2Fls_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo
 * Generic function for execution of Fls2Fls and Ram2Fls transfer.
 * \return  Returns status of the storage operation
 * \retval  rba_FeeFs1x_Pending_e   : Storage ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e     : Data stored sucessfully
 * \retval  rba_FeeFs1x_ErrorInternal_e     : Error during storage of the data
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_BC_wrData2Fls_st.state_en)
    {
        case rba_FeeFs1x_BC_wrData2FlsDoStates_prepareHdrFromFls_e:
        {
            retVal_en = rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromFls(&rba_FeeFs1x_BC_wrData2Fls_st);
            break;
        }

        case rba_FeeFs1x_BC_wrData2FlsDoStates_wrDataPagesFromFls_e:
        {
            retVal_en = rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromFls(&rba_FeeFs1x_BC_wrData2Fls_st);
            break;
        }

        case rba_FeeFs1x_BC_wrData2FlsDoStates_prepareHdrFromRam_e:
        {
            retVal_en = rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromRam(&rba_FeeFs1x_BC_wrData2Fls_st);
            break;
        }

        case rba_FeeFs1x_BC_wrData2FlsDoStates_wrDataPagesFromRam_e:
        {
            retVal_en = rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromRam(&rba_FeeFs1x_BC_wrData2Fls_st);
            break;
        }

        case rba_FeeFs1x_BC_wrData2FlsDoStates_recomputeDataCrc_e:
        {
            retVal_en = rba_FeeFs1x_BC_wrData2FlsDo_recomputeDataCrc(&rba_FeeFs1x_BC_wrData2Fls_st);
            break;
        }

        case rba_FeeFs1x_BC_wrData2FlsDoStates_wrHdr_e:
        {
            retVal_en = rba_FeeFs1x_BC_wrData2FlsDo_wrHdr(&rba_FeeFs1x_BC_wrData2Fls_st);
            break;
        }

        case rba_FeeFs1x_BC_wrData2FlsDoStates_wrDummyHdr_e:
        {
            retVal_en = rba_FeeFs1x_BC_wrData2FlsDo_wrDummyHdr(&rba_FeeFs1x_BC_wrData2Fls_st);
            break;
        }

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    // Reset the state machine when job failed or when all the chunks are completed
    if((rba_FeeFs1x_Pending_e != retVal_en) &&
       !((rba_FeeFs1x_JobOK_e == retVal_en) &&
               (rba_FeeFs1x_BC_wrData2FlsDoStates_wrDataPagesFromRam_e == rba_FeeFs1x_BC_wrData2Fls_st.state_en)))
    {
        rba_FeeFs1x_BC_wrData2Fls_st.state_en = rba_FeeFs1x_BC_wrData2FlsDoStates_idle_e;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo_PrepareHdrFromFls
 * Statefunction of rba_FeeFs1x_BC_wrData2FlsDo. The function prepares the header page and stores it in an internal buffer.
 *
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : Prepartion of header finished
 * \retVal  no other return value is possible in this function.
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromFls(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst)
{
    uint32 destPageNr_u32;              // back up of the destination page number

    // Update the information to the destination block copy. Restore the page number into a temporary variable.
    destPageNr_u32 = rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].pageNr_u32;
    rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en] = rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->srcCopy_en];
    rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].pageNr_u32 = destPageNr_u32;

    if(fsm_pst->updateStatus_b)
    {
        rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromFls_updateStatusByte(fsm_pst);

        // compute the initial new data crc value (required for later calculation for remaining data bytes)
        fsm_pst->newDataCrc_u32 = rba_FeeFs1x_BC_calcDataCrcForDataBytesInHdr(fsm_pst->destCopy_en);
    }

    // update the internal buffer with the complete header information
    rba_FeeFs1x_BC_fillToHdrBuffer(fsm_pst->destCopy_en);

    // Switch to next state depending on if data pages are to be programmed
    if (rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].blkLength_u16 > RBA_FEEFS1X_DATABYTES_IN_HDR)
    {
        // data pages are to be programmed
        FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDataPagesFromFls_e);

        // clear the flag to indicate that the header write should not update write page
        // (as it is already updated during data page write)
        rba_FeeFs1x_BC_wrData2Fls_st.updateWrPgDuringHdrWr_b = FALSE;
    }
    else
    {
        if(fsm_pst->updateStatus_b)
        {
            // no data pages to be transferred, so update the new dataCrc value directly into the block copy
            rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].dataCrc_u32 = fsm_pst->newDataCrc_u32;
            rba_FeeFs1x_BC_updateDataCrcInHdrBuffer(fsm_pst->newDataCrc_u32);
        }

        // No data pages programming required, directly program the header page
        FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrHdr_e);

        // set the flag to indicate that the header page should update the write page (as no data page write happens)
        rba_FeeFs1x_BC_wrData2Fls_st.updateWrPgDuringHdrWr_b = TRUE;
    }

    // return pending as there is no other possible in this function
    return  rba_FeeFs1x_Pending_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromRam
 * Statefunction of rba_FeeFs1x_BC_wrData2FlsDo. It prepares the header page and stores it in an internal buffer.
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : Prepartion of header finished
 * \retVal  no other return value is possible in this function.
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromRam(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                 retVal_en = rba_FeeFs1x_Pending_e; // return value of this state function
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten destCopy_en; // local variable to speed up the access for the array
    uint8_least                            idx_qu8;

    destCopy_en = fsm_pst->destCopy_en;

    // compute and store the header CRC in the blockcopy.
    // DataCRC was already filled in the write request rba_FeeFs1x_BC_wrRam2Fls
    rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].HdrCrc_u16 = rba_FeeFs1x_BC_calcBlkHdrCRC(
            rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].persID_u16,
            rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].blkLength_u16,
            rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].statusByte_u16);

    // Copy the data bytes in the header page to block copy
    for(idx_qu8 = 0; idx_qu8 < RBA_FEEFS1X_DATABYTES_IN_HDR; idx_qu8++)
    {
        if(idx_qu8 < rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].blkLength_u16)
        {
            rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].first2DatB_au8[idx_qu8] = fsm_pst->buf_pcu8[idx_qu8];
        }
        else
        {
            // fill unused data with the inverse erase pattern
            rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].first2DatB_au8[idx_qu8] = (uint8)(~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT);
        }
    }

    // calculate and store the initial data crc (required for later calculation for remaining data bytes)
    fsm_pst->newDataCrc_u32 = rba_FeeFs1x_BC_calcDataCrcForDataBytesInHdr(destCopy_en);

    // update the internal buffer with the complete header information
    rba_FeeFs1x_BC_fillToHdrBuffer(destCopy_en);

    // Switch to next state depending on if data pages are to be programmed
    if (rba_FeeFs1x_BC_blockCopies_ast[destCopy_en].blkLength_u16 > RBA_FEEFS1X_DATABYTES_IN_HDR)
    {
        // data pages are to be programmed
        FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDataPagesFromRam_e);

        fsm_pst->curDataPageNr_u32 = (rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].pageNr_u32 +
                                    RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR);

        // adapt the length and buffer pointer for the data bytes that will be written along with the header
        fsm_pst->nrDataBytCur_u16 -= RBA_FEEFS1X_DATABYTES_IN_HDR;
        fsm_pst->nrDataBytDone_u16 += RBA_FEEFS1X_DATABYTES_IN_HDR;
        fsm_pst->buf_pcu8 += RBA_FEEFS1X_DATABYTES_IN_HDR;

        // Check if there are still more data bytes available
        if(0u == fsm_pst->nrDataBytCur_u16)
        {
            // No more data bytes. Current chunk is completed.
            retVal_en = rba_FeeFs1x_JobOK_e;
        }

        // clear the flag to indicate that the header write should not update write page
        // (as it is already updated during data page write)
        fsm_pst->updateWrPgDuringHdrWr_b = FALSE;
    }
    else
    {
        // No data pages programming required, directly program the header page
        if(
            (fsm_pst->newDataCrc_u32 == rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].dataCrc_u32) ||
            (rba_FeeFs1x_BC_getCpyNewRobCRCBit(fsm_pst->destCopy_en)) ||
            (fsm_pst->isChunkJob_b)
          )
        {
            // no data change or data change is allowed -> update the block copy and switch to write header
            rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].dataCrc_u32 = fsm_pst->newDataCrc_u32;
            rba_FeeFs1x_BC_updateDataCrcInHdrBuffer(fsm_pst->newDataCrc_u32);
            // set flag to indicate that the header page should update the write page (as no data page write happens)
            fsm_pst->updateWrPgDuringHdrWr_b = TRUE;

            FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrHdr_e);
        }
        else
        {
            // unallowed data change -> write dummy header
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDummyHdr_e);
        }
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromFls
 * Statefunction of rba_FeeFs1x_BC_wrData2FlsDo. The function triggers the write of data pages to the PA layer.
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : Data pages writing ongoing or completed (state machine is changed to next state)
 * \retVal  rba_FeeFs1x_ErrorInternal_e     : Error during writing of data pages
 * \retVal  rba_FeeFs1x_ErrorExternal_e: Error returned by PA
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromFls(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten  retVal_en;          // return value of this state function
    rba_FeeFs1x_RetVal_ten  retValWriteDo_en;   // return value for the PA_WriteDo function
    uint16                  nrDataPages_u16;    // number of data pages

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // Trigger write of the data pages
        nrDataPages_u16 = rba_FeeFs1x_BC_getNrDataPagesByBlkLen(rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->srcCopy_en].blkLength_u16);

        rba_FeeFs1x_PAWrite_copyFls2Fls
                (
                        (rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->srcCopy_en].pageNr_u32 + RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR) ,
                        nrDataPages_u16,
                        (rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].pageNr_u32 + RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR) ,
                        TRUE
                );
    }

    // Cyclic call the do-function
    retValWriteDo_en = rba_FeeFs1x_PAWrite_copyFls2FlsDo();

    switch(retValWriteDo_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // Write of data pages to DFLASH was successful. Check if status has been changed.
            // If yes, recomputation of data crc is required, else go to program header.
            if( (fsm_pst->updateStatus_b) &&
                (fsm_pst->newStatusByte_u16 != rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->srcCopy_en].statusByte_u16))
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_recomputeDataCrc_e);
            }
            else
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrHdr_e);
            }
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            // Error during storage of data pages, fill up the header with a dummy value
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDummyHdr_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_Pending_e:
        {
            // Continue state in next cycle
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromRam
 * Statefunction of rba_FeeFs1x_BC_wrData2FlsDo. The function triggers the write of data pages to the PA layer.
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : Data pages writing ongoing or completed (state machine is changed to next state)
 * \retVal  rba_FeeFs1x_ErrorInternal_e     : Error during writing of data pages
 * \retVal  rba_FeeFs1x_ErrorExternal_e: Error returned by PA
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrDataPagesFromRam(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten  retVal_en;          // return value of this state function
    rba_FeeFs1x_RetVal_ten  retValWriteDo_en;   // return value for the PA_WriteDo function
    uint16                  nrDataPages_u16;      // number of data pages

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // Trigger write of the data pages
        rba_FeeFs1x_PAWrite_write
                (
                        fsm_pst->curDataPageNr_u32,
                        fsm_pst->nrDataBytCur_u16,
                        fsm_pst->buf_pcu8,
                        TRUE,
                        fsm_pst->jobMode_en
                );
    }

    // Cyclic call the do-function
    retValWriteDo_en = rba_FeeFs1x_PAWrite_writeDo();

    switch(retValWriteDo_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            fsm_pst->nrDataBytDone_u16 += fsm_pst->nrDataBytCur_u16;

            // adapt the write page number for the next chunk
            nrDataPages_u16 = (uint16)Fee_Prv_LibCeil(fsm_pst->nrDataBytCur_u16, RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE)
                                        /RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
            fsm_pst->curDataPageNr_u32 += nrDataPages_u16;

            // check if there are pending bytes for the chunk job
            if(fsm_pst->nrDataBytDone_u16 < rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].blkLength_u16)
            {
                // Stay in same state and wait for next chunk data but return that current chunk write was successful
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // All chunks were programmed successfully, go to next step
                if((FEE_RB_ALLJOBS_ALLSTEPS_E == fsm_pst->jobMode_en) || (fsm_pst->isChunkJob_b))
                {
                    // Switch to next state to check for data change or prepare datacrc for chunk wise write
                    FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_recomputeDataCrc_e);
                }
                else
                {
                    // in all other mode of write check for data changes is skipped -> directly write the header
                    FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrHdr_e);
                }
                retVal_en = rba_FeeFs1x_Pending_e;
            }
            break;
        }

        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            // Error during storage of data pages, fill up the header with a dummy value
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDummyHdr_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_Pending_e:
        {
            // Continue state in next cycle
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo_recomputeDataCrc
 * Statefunction of rba_FeeFs1x_BC_wrData2FlsDo. The function triggers the computation of data crc over the
 * DFLASH content that is programmed to the memory and updates the new checksum in the block header.
 *
 * \param   fsm_pst                     : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e       : Data change check ongoing or completed (state machine changes to next state)
 * \retVal  rba_FeeFs1x_ErrorInternal_e : internal error during check of data change
 * \retVal  rba_FeeFs1x_ErrorExternal_e : Error returned by PA
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_recomputeDataCrc(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                  retVal_en;              // return value of this state function
    rba_FeeFs1x_RetVal_ten                  retValCalcCrcDo_en;     // return value for the PA CalcCrcDo function

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // Trigger calculation of the data CRC
        rba_FeeFs1x_PASrv_calcCRC32
                (
                        (rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].pageNr_u32 + RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR),
                        (rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].blkLength_u16 - (uint16)RBA_FEEFS1X_DATABYTES_IN_HDR),
                        &fsm_pst->newDataCrc_u32,
                        0u
                );
    }

    // Cyclic call the do-function
    retValCalcCrcDo_en = rba_FeeFs1x_PASrv_calcCRC32Do();

    switch(retValCalcCrcDo_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // Check if there was change in data Crc
            if(fsm_pst->newDataCrc_u32 == rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].dataCrc_u32)
            {
                // go to next state to write header
                FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrHdr_e);
            }
            else
            {
                // if an update of the status was requested or data change is allowed
                if( (fsm_pst->updateStatus_b) ||
                    rba_FeeFs1x_BC_getCpyNewRobCRCBit(fsm_pst->destCopy_en) ||
                    (fsm_pst->isChunkJob_b) )
                {
                    // New data crc prepared, update the same in block copy and internal header
                    rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].dataCrc_u32 = fsm_pst->newDataCrc_u32;
                    rba_FeeFs1x_BC_updateDataCrcInHdrBuffer(fsm_pst->newDataCrc_u32);

                    // go to next state to write header
                    FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrHdr_e);
                }
                else
                {
                    // data change not allowed, write dummy header
                    FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDummyHdr_e);
                }
            }

            retVal_en = rba_FeeFs1x_Pending_e;

            break;
        }
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            // Error during calculation of data Crc, write dummy header
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_wrData2FlsDoStates_wrDummyHdr_e);
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_Pending_e:
        {
            // Continue state in next cycle
            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo_wrHdr
 * Statefunction of rba_FeeFs1x_BC_wrData2FlsDo. This function writes the block header to dflash.
 * that is programmed to the memory.
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : Header page write ongoing
 * \retVal  rba_FeeFs1x_ErrorInternal_e     : Internal error during writing of header page
 * \retVal  rba_FeeFs1x_ErrorExternal_e: Error returned by PA
 * \retVal  rba_FeeFs1x_JobOK_e     : Header page write was completed sucessfully
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrHdr(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                  retVal_en;          // return value of this state function

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // Trigger write of the header pages
        rba_FeeFs1x_PAWrite_write
        (
                rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].pageNr_u32,
                (RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE * RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR),
                rba_FeeFs1x_BC_buf_au8,
                fsm_pst->updateWrPgDuringHdrWr_b,
                fsm_pst->jobMode_en
        );
    }

    // Cyclic call the do-function
    retVal_en = rba_FeeFs1x_PAWrite_writeDo();

    //rba_FeeFs1x_PAWrite_writeDo will never return rba_FeeFs1x_JobFailed_e -> return of inner to outer state possible
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_wrData2FlsDo_wrDummyHdr
 * Statefunction of rba_FeeFs1x_BC_wrData2FlsDo. This function triggers the write of the dummy header.
 * Dummy header is written to avoid blank area and avoid GHOST areas in the memory.
 * \param   fsm_pst                    : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e      : Header page write ongoing
 * \retVal  rba_FeeFs1x_ErrorInternal_e: Internal error during writing of header page,
 *                                       or dummy header was successfully programmed
 * \retVal  rba_FeeFs1x_ErrorExternal_e: Error returned by PA
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_wrData2FlsDo_wrDummyHdr(rba_FeeFs1x_BC_wrData2Fls_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                  retVal_en;          // return value of this state function
    rba_FeeFs1x_RetVal_ten                  retValWriteDo_en;   // return value for the PA_WriteDo function
    uint8 idx_u8;

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // Fill up the header with inverse erase values
        for(idx_u8 = 0u; idx_u8 < (RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE * RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR) ; idx_u8++)
        {
            rba_FeeFs1x_BC_buf_au8[idx_u8] = (uint8)(~RBA_FEEFS1X_PRV_CFG_ERASE_PATTERN_8BIT);
        }

        // Trigger write of the header pages
        rba_FeeFs1x_PAWrite_write
        (
            rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].pageNr_u32 ,
            (RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE * RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR),
            rba_FeeFs1x_BC_buf_au8,
            FALSE,
            FEE_RB_ALLJOBS_ALLSTEPS_E
        );
    }

    // Cyclic call the do-function
    retValWriteDo_en = rba_FeeFs1x_PAWrite_writeDo();

    switch(retValWriteDo_en)
    {
        // Same reaction if the writing was successful or failed, i.e., inform upper layer about the failure
        case rba_FeeFs1x_JobOK_e:
        {
            // Error during storage of header page
            retVal_en = rba_FeeFs1x_ErrorExternal_e;
            break;
        }

        case rba_FeeFs1x_ErrorInternal_e:
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = retValWriteDo_en;
            break;
        }

        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * sub function of rba_FeeFs1x_BC_wrData2FlsDo_PrepareHdrFromFls.
 * The function updates the status byte.
 *
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_FeeFs1x_BC_wrData2FlsDo_prepareHdrFromFls_updateStatusByte(rba_FeeFs1x_BC_wrData2Fls_tst const * const fsm_pst)
{
    // remember the previous settings of the inv flag in DFLASH
    // ensure that the invalidate bit is not changed (as this bit is not set in status bytes in config which was passed)
    boolean dflashInvBit_b = rba_FeeFs1x_BC_getCpyInvBitActive(fsm_pst->srcCopy_en);

    rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].statusByte_u16 = fsm_pst->newStatusByte_u16;

    if(dflashInvBit_b)
    {
        // Invalidate bit was set in the DFLASH copy, set it again
        rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].statusByte_u16 |= RBA_FEEFS1X_PRV_CFG_INVALIDATE_BITS;
    }

    // compute and store the header new CRC in the blockcopy
    rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].HdrCrc_u16 = rba_FeeFs1x_BC_calcBlkHdrCRC(
            rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].persID_u16,
            rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].blkLength_u16,
            rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->destCopy_en].statusByte_u16);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getNrHdrPages
 * This function computes the number of logical pages required for header. This
 * depends on the logical page size and the no fall back feature.
 *
 * \param   copy_en     : Block copy for which the header pages calculation is required
 *
 * \retval  uint8       : number of logical header pages
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE uint8 rba_FeeFs1x_BC_getNrHdrPages(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    uint8 nrHdrPgs_u8;

    nrHdrPgs_u8 = RBA_FEEFS1X_PRV_NR_OF_LOG_PAGES_USED_BY_BLK_HDR;

    if(rba_FeeFs1x_BC_getCpyNoFBBitActive(copy_en))
    {
        // Increment one more page for no fall back marker
        nrHdrPgs_u8++;
    }

    return  nrHdrPgs_u8;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_getNrDataPagesByBlkLen
 * This function calculates the number of data pages required for the block, considering the first 2 data bytes in the
 * header and alignment to page boundary.
 *
 * \param   len_u16     : Length of the block in bytes
 *
 * \retval  uint8       : number of logical data pages
 * \seealso
 * \usedresources
 *********************************************************************
 */
static uint16 rba_FeeFs1x_BC_getNrDataPagesByBlkLen(uint16 len_u16)
{
    uint16 nrDataPages_u16;
    uint16 nrAlgnBytes_u16;

    if(len_u16 <= RBA_FEEFS1X_DATABYTES_IN_HDR)
    {
        // no additional data pages requried
        nrDataPages_u16 = 0;
    }
    else
    {
        nrAlgnBytes_u16 = (uint16)Fee_Prv_LibCeil(  (len_u16 - (uint16)RBA_FEEFS1X_DATABYTES_IN_HDR),
                                                    RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE );
        nrDataPages_u16 = nrAlgnBytes_u16/RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
    }
    return  nrDataPages_u16;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_calcBlkHdrCRC
 * This function calculates the header crc for the block.
 *
 * \param   persID_u16   : persistent index of the block
 * \param   blkLength_u16: block length of the block
 * \param   statusByte_u16: status byte of the block
 * \retval  HdrCrc_u16   : Header CRC of the block copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
uint16 rba_FeeFs1x_BC_calcBlkHdrCRC(uint16 persID_u16 , uint16 blkLength_u16 , uint16 statusByte_u16)
{
    uint8 headerBuffer_au8[RBA_FEEFS1X_BC_HDRCRC_BYTES2BECONSIDERED];
    uint16 hdrCRC_u16;

    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_preamble0_e] = RBA_FEEFS1X_BC_PREAMBEL_BYTE_0 ;
    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_preamble1_e] = RBA_FEEFS1X_BC_PREAMBEL_BYTE_1 ;
    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_preamble2_e] = RBA_FEEFS1X_BC_PREAMBEL_BYTE_2 ;
    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_statusB_e  ] = (uint8) statusByte_u16 ;
    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_persID0_e  ] = (uint8) (persID_u16 >> 8u);
    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_persID1_e  ] = (uint8) (persID_u16 & 0xFFu);
    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_len0_e     ] = (uint8) (blkLength_u16 >> 8u);
    headerBuffer_au8[rba_FeeFs1x_BC_HdrPgValueMap_len1_e     ] = (uint8) (blkLength_u16 & 0xFFu);

    hdrCRC_u16 = Crc_CalculateCRC16(headerBuffer_au8,
            RBA_FEEFS1X_BC_HDRCRC_BYTES2BECONSIDERED,
            (uint16)RBA_FEEFS1X_SECTORMARKER_PREAMBLE,
            FALSE);

    return hdrCRC_u16;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_calcDataCrcForDataBytesInHdr
 * This function calculates the data crc for the first data bytes that are part of the header page.
 *
 * \param   copy_en      : Block copy for which data crc has to be calculated
 * \retval  dataCrc_u32  : Data CRC over the data bytes in the header page
 * \seealso
 * \usedresources
 *********************************************************************
 */
static uint32 rba_FeeFs1x_BC_calcDataCrcForDataBytesInHdr(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en)
{
    uint32 dataCrc_u32;
    uint32 dataCrcLen_u32;

    // Limit the number of data bytes in the header page
    if(rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16 <= RBA_FEEFS1X_DATABYTES_IN_HDR)
    {
        dataCrcLen_u32 = (uint32)rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16;
    }
    else
    {
        dataCrcLen_u32 = (uint32)RBA_FEEFS1X_DATABYTES_IN_HDR;
    }


    dataCrc_u32 = Crc_CalculateCRC32(rba_FeeFs1x_BC_blockCopies_ast[copy_en].first2DatB_au8,
            dataCrcLen_u32,
            RBA_FEEFS1X_BC_DATACRC_STARTVALUE((rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16)),
            FALSE);

    return dataCrc_u32;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2Ram
 * This function is the first step to read out and validate the data from DFLASH:
 * - Read requested data to user buffer.
 * - Read data for validation to medium buffer.
 * - Validate block data from user/medium buffer
 * In this function the order is accepted and the processing will start in function rba_FeeFs1x_BC_readVldtFls2RamDo.
 * Note: In case of a chunk-wise read, FeeHl ensures that the buffer, which has been provided for the first chunk, has
 * sufficient space to store at least RBA_FEEFS1X_DATABYTES_IN_HDR+1u bytes of payload behind the chunk header. It
 * significantly simplifies the possible transitions of the state machine, because
 * - state procHdrBlkData will always be completed with the first chunk
 * - states readBlkDataToUsrBuf and vldtBlkDataFromUsrBuf have some payload to process in the first chunk
 * - subsequent chunks always restart from state readBlkDataToUsrBuf.
 *
 * \param   copy_en     : The copy which that has to be read out from FLS
 * \param   len_u16     : length of the data to be read out of the block(offset + length <= block length !)
 * \param   ofs_u16     : Offset within the block the read shall start at
 * \param   userBuf_pu8 : pointer to the location the read data should be stored at
 * \param   isChunkJob_b: TRUE -> job placed by chunk unit, or FALSE -> job placed by order unit
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BC_readVldtFls2Ram(rba_FeeFs1x_BC_Cpy_blockCopyObject_ten copy_en, uint16 len_u16, uint16 ofs_u16, uint8 * userBuf_pu8, boolean isChunkJob_b)
{
    // Valid copy,
    // present in DFLASH,
    // requested bytes are not succeeding the found copy

    /* Normal read, or first chunk of chunk-wise read?
     * For subsequent chunks of a chunk-wise read, the state machine initialization has already been performed.
     * -> re-enter from state readBlkDataToUsrBuf, and only update its variables where necessary! */
    if(rba_FeeFs1x_BC_readVldtFls2RamStates_readBlkDataToUsrBuf_e != rba_FeeFs1x_BC_readVldtFls2Ram_st.state_en)
    {
        // Store the order details into internal variable
        rba_FeeFs1x_BC_readVldtFls2Ram_st.copy_en = copy_en;

        rba_FeeFs1x_BC_readVldtFls2Ram_st.vldtBytes_u16 = 0u;
        rba_FeeFs1x_BC_readVldtFls2Ram_st.dataCRC_u32 =
                                RBA_FEEFS1X_BC_DATACRC_STARTVALUE((rba_FeeFs1x_BC_blockCopies_ast[copy_en].HdrCrc_u16));

        // set the data block length for the validation
        if (rba_FeeFs1x_BC_getCpyLegacyRobCRCBit(copy_en))
        {
            // for ROB block, the data length is 4 bytes smaller due to ROB crc at the end of the block
            rba_FeeFs1x_BC_readVldtFls2Ram_st.verifLen_u16 =
                                rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16 - RBA_FEEFS1X_BC_NR_ROBCRCBYTES;
        }
        else
        {
            rba_FeeFs1x_BC_readVldtFls2Ram_st.verifLen_u16 = rba_FeeFs1x_BC_blockCopies_ast[copy_en].blkLength_u16;
        }

        rba_FeeFs1x_BC_readVldtFls2Ram_st.state_en = rba_FeeFs1x_BC_readVldtFls2RamStates_procHdrBlkData_e;
        rba_FeeFs1x_BC_readVldtFls2Ram_st.isUsrBufWr_b = FALSE;
        rba_FeeFs1x_BC_readVldtFls2Ram_st.isChunkJob_b = isChunkJob_b;
    }

    if(userBuf_pu8 != NULL_PTR)
    {
        rba_FeeFs1x_BC_readVldtFls2Ram_st.nrOfBytesToCopyToUserBuf_u16 = len_u16;
        rba_FeeFs1x_BC_readVldtFls2Ram_st.jobOfs_u16 = ofs_u16;
    }
    else
    {
        rba_FeeFs1x_BC_readVldtFls2Ram_st.nrOfBytesToCopyToUserBuf_u16 = 0u;
        rba_FeeFs1x_BC_readVldtFls2Ram_st.jobOfs_u16 = 0u;
    }

    rba_FeeFs1x_BC_readVldtFls2Ram_st.userBuf_pu8 = userBuf_pu8;
    rba_FeeFs1x_BC_readVldtFls2Ram_st.readBytes_u16 = 0u;
    rba_FeeFs1x_BC_readVldtFls2Ram_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2RamDo
 * This is a state machine that reads out and validates the data from DFLASH:
 * - Read requested data to user buffer.
 * - Read data for validation to medium buffer.
 * - Validate block data from user/medium buffer
 *
 * \return  Finished?
 * \retval  rba_FeeFs1x_JobOK_e:            Job finished successfully
 * \retval  rba_FeeFs1x_JobFailed_e:        Error during processing; aborted
 * \retval  rba_FeeFs1x_Pending_e:          Job ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e:    An internal error occured
 * \retval  rba_FeeFs1x_ErrorExternal_e:    Read from DFlash failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo(void)
{
    rba_FeeFs1x_BC_readVldtFls2Ram_tst * fsm_pst = &rba_FeeFs1x_BC_readVldtFls2Ram_st;
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_BC_readVldtFls2Ram_st.state_en)
    {
        case rba_FeeFs1x_BC_readVldtFls2RamStates_procHdrBlkData_e:
        {
            retVal_en = rba_FeeFs1x_BC_readVldtFls2RamDo_procHdrBlkData(fsm_pst);
            break;
        }
        case rba_FeeFs1x_BC_readVldtFls2RamStates_readBlkDataToUsrBuf_e:
        {
            retVal_en = rba_FeeFs1x_BC_readVldtFls2RamDo_readBlkDataToUsrBuf(fsm_pst);
            break;
        }
        case rba_FeeFs1x_BC_readVldtFls2RamStates_vldtBlkDataFromUsrBuf_e:
        {
            retVal_en = rba_FeeFs1x_BC_readVldtFls2RamDo_vldtBlkDataFromUsrBuf(fsm_pst);
            break;
        }
        case rba_FeeFs1x_BC_readVldtFls2RamStates_vldtRmngBlkData_e:
        {
            retVal_en = rba_FeeFs1x_BC_readVldtFls2RamDo_vldtRmngBlkData(fsm_pst);
            break;
        }
        case rba_FeeFs1x_BC_readVldtFls2RamStates_compareDataCrc_e:
        {
            retVal_en = rba_FeeFs1x_BC_readVldtFls2RamDo_compareDataCrc(fsm_pst);
            break;
        }
        case rba_FeeFs1x_BC_readVldtFls2RamStates_compareLegacyRobDataCrc_e:
        {
            retVal_en = rba_FeeFs1x_BC_readVldtFls2RamDo_compareLegacyRobDataCrc(fsm_pst);
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        if((rba_FeeFs1x_JobOK_e != retVal_en) && fsm_pst->isUsrBufWr_b)
        {
            /* A read job was not finished successfully */
            /* We have used the given user buffer while reading to buffer data for block data crc calculation */
            /* In case it contains invalid block data */
            /* We need to wipe the user buffer to ensure the user is not using false block data */
            rba_MemLib_MemSet(fsm_pst->userBuf_pu8, 0u, fsm_pst->nrOfBytesToCopyToUserBuf_u16);
        }

        if( (rba_FeeFs1x_JobOK_e != retVal_en) ||
            (rba_FeeFs1x_BC_readVldtFls2RamStates_readBlkDataToUsrBuf_e != fsm_pst->state_en) )
        {
            // Reset the state machine when job failed, or when all the chunks are completed
            fsm_pst->state_en = rba_FeeFs1x_BC_readVldtFls2RamStates_idle_e;
        }
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2RamDo_procHdrBlkData
 * Statefunction of rba_FeeFs1x_BC_readVldtFls2RamDo. The function handles the data bytes present in the header page.
 * The data bytes present in the header page is read out from the block copy in RAM which was read out during
 * block copy initialization. This is done to avoid junk data being returned when the page content might change from
 * the point when the header was validated and when the data is read out. The data CRC calculation is done for these
 * bytes and if requested for read they are copied to user buffer.
 * Note: When processing the first chunk (no matter if for a single or multiple chunk read), the state machine starts
 * from here. A read, which is seperated into multiple chunks, will enter this state only once (as such reads are
 * rejected by FeeHl if starting with less than RBA_FEEFS1X_DATABYTES_IN_HDR+1u bytes of payload).
 *
 * \param   fsm_pst                 : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : data from the data pages have to be read out
 * \retVal  no other return value is possible in this function.
 * \seealso
 * \usedresources
 *********************************************************************
 */

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_procHdrBlkData(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst)
{
    rba_FeeFs1x_BC_readVldtFls2RamStates_ten    stNext_en;
    uint16                                      nrBytesToDo_u16;
    boolean                                     copyToUserBufStarted_b = FALSE;

    nrBytesToDo_u16 = (uint16)rba_MemLib_Min(RBA_FEEFS1X_DATABYTES_IN_HDR ,fsm_pst->verifLen_u16);

    fsm_pst->dataCRC_u32 = Crc_CalculateCRC32(  rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].first2DatB_au8,
                                                nrBytesToDo_u16,
                                                fsm_pst->dataCRC_u32,
                                                FALSE);

    // If requested, copy data to user buffer
    if((fsm_pst->userBuf_pu8 != NULL_PTR) && (fsm_pst->jobOfs_u16 < nrBytesToDo_u16))
    {
        for(fsm_pst->readBytes_u16 = 0u; ((fsm_pst->jobOfs_u16 + fsm_pst->readBytes_u16) < rba_MemLib_Min(RBA_FEEFS1X_DATABYTES_IN_HDR,(fsm_pst->jobOfs_u16 + fsm_pst->nrOfBytesToCopyToUserBuf_u16))); fsm_pst->readBytes_u16++)
        {
            *(fsm_pst->userBuf_pu8 + fsm_pst->readBytes_u16) = rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].first2DatB_au8[fsm_pst->jobOfs_u16 + fsm_pst->readBytes_u16];
        }

        fsm_pst->isUsrBufWr_b = TRUE;
        copyToUserBufStarted_b = TRUE;
    }

    if(RBA_FEEFS1X_DATABYTES_IN_HDR >= fsm_pst->verifLen_u16)
    {
        /* Read/validation done - no remaining block data available */
        stNext_en = rba_FeeFs1x_BC_readVldtFls2RamStates_compareDataCrc_e;
    }
    else
    {
        /* Check if the data has to be copied to user buffer first */
        if((RBA_FEEFS1X_DATABYTES_IN_HDR == fsm_pst->jobOfs_u16) ||
           ((copyToUserBufStarted_b) && (fsm_pst->readBytes_u16 < fsm_pst->nrOfBytesToCopyToUserBuf_u16)))
        {
            /* read and validate the data using user buffer */
            stNext_en = rba_FeeFs1x_BC_readVldtFls2RamStates_readBlkDataToUsrBuf_e;
        }
        else
        {
            /* read and validate the data using internal buffer */
            stNext_en = rba_FeeFs1x_BC_readVldtFls2RamStates_vldtRmngBlkData_e;
        }
    }

    fsm_pst->vldtBytes_u16 += (nrBytesToDo_u16 - fsm_pst->readBytes_u16);

    FEE_PRV_LIBSC(stNext_en);

    return (rba_FeeFs1x_Pending_e);

}


/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2RamDo_readBlkDataToUsrBuf
 * Statefunction of rba_FeeFs1x_BC_readVldtFls2RamDo.
 * The function copies the data bytes from DFLASH to user buffer in RAM.
 * Note: When reading subsequent chunks, the state machine re-enters from here.
 *
 * \param   fsm_pst:                        Statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e:          Read successfull, call again during next cycle to continue with next state
 * \retval  rba_FeeFs1x_ErrorInternal_e:    An internal error occured
 * \retval  rba_FeeFs1x_ErrorExternal_e:    Read from DFlash failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_readBlkDataToUsrBuf(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        // first entry of the state
        uint32 startPageOfRequest_u32;
        uint32 byteOfsFromBlockStart_u32;
        uint8 byteOfsInPage_u8;

        // Calculate the first requested page and the byte offset within this page
        byteOfsFromBlockStart_u32 =
                        ((uint32)rba_FeeFs1x_BC_HdrPgValueMap_dat0_e) + (fsm_pst->jobOfs_u16 + fsm_pst->readBytes_u16);

        startPageOfRequest_u32 = rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].pageNr_u32 +
                (byteOfsFromBlockStart_u32 / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);
        byteOfsInPage_u8 = (uint8)(byteOfsFromBlockStart_u32 % RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);

        rba_FeeFs1x_PARead_unbufferedRead(  startPageOfRequest_u32,
                                            (fsm_pst->nrOfBytesToCopyToUserBuf_u16 - fsm_pst->readBytes_u16),
                                            (fsm_pst->userBuf_pu8 + fsm_pst->readBytes_u16),
                                            byteOfsInPage_u8 );

        fsm_pst->isUsrBufWr_b = TRUE;
    }

    // Cyclic call for Do function
    retVal_en = rba_FeeFs1x_PARead_unbufferedReadDo();

    if(rba_FeeFs1x_JobOK_e == retVal_en)
    {
        FEE_PRV_LIBSC(rba_FeeFs1x_BC_readVldtFls2RamStates_vldtBlkDataFromUsrBuf_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2RamDo_vldtBlkDataFromUsrBuf
 * Statefunction of rba_FeeFs1x_BC_readVldtFls2RamDo.
 * The function validates the data bytes previously read from DFLASH directly from user buffer in RAM by
 * continuing the computation of data CRC.
 * Note: When reaching the end of a chunk and subsequent chunks are pending, this is the final state.
 *
 * \param   fsm_pst                 : Statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e     : End of current chunk reached, but there will be subsequent chunks!
 * \retval  rba_FeeFs1x_Pending_e   : Data CRC computation completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_vldtBlkDataFromUsrBuf(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst)
{
    MemIf_JobResultType     resultInner_en;
    rba_FeeFs1x_RetVal_ten  retVal_en = rba_FeeFs1x_Pending_e;

    if(FEE_PRV_LIBENTRY)
    {
        Fee_Prv_LibCalcCrcRam(  Fee_Rb_DeviceName,
                                (fsm_pst->userBuf_pu8 + fsm_pst->readBytes_u16),
                                (fsm_pst->nrOfBytesToCopyToUserBuf_u16 - fsm_pst->readBytes_u16),
                                &fsm_pst->dataCRC_u32,
                                FALSE);
    }

    // Cyclic call for Do function
    resultInner_en = Fee_Prv_LibCalcCrcRamDo(Fee_Rb_DeviceName);

    if(MEMIF_JOB_OK == resultInner_en)
    {
        if((fsm_pst->jobOfs_u16 + fsm_pst->nrOfBytesToCopyToUserBuf_u16) == fsm_pst->verifLen_u16)
        {
            // End of block reached - continue with CRC compare
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_readVldtFls2RamStates_compareDataCrc_e);
        }
        else if(fsm_pst->isChunkJob_b)
        {
            // End of chunk reached
            retVal_en = rba_FeeFs1x_JobOK_e;

            // Re-enter the state machine from state readBlkDataToUsrBuf if subsequent chunks are pending
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_readVldtFls2RamStates_readBlkDataToUsrBuf_e);
        }
        else
        {
            // Prepare validation of remaining data
            fsm_pst->readBytes_u16 = fsm_pst->nrOfBytesToCopyToUserBuf_u16;

            // End of block not reached - continue with validation of remaining data
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_readVldtFls2RamStates_vldtRmngBlkData_e);
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2RamDo_vldtRmngBlkData
 * Statefunction of rba_FeeFs1x_BC_readVldtFls2RamDo. The function continues the computation of the data CRC for the
 * remaining data pages, which are not part of the header and were not requested to read.
 * Note: This state will never be entered for read jobs, which were placed by chunk unit.
 *
 * \param   fsm_pst                 : Statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_Pending_e   : Computation of data CRC ongoing or completed successfully
 * \retval  rba_FeeFs1x_ErrorInternal_e : Internal error during processing
 * \retval  rba_FeeFs1x_ErrorExternal_e : External error during processing
 * \retVal  no other return value is possible in this function.
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_vldtRmngBlkData(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten  retValCRCDo_en;
    rba_FeeFs1x_RetVal_ten  retVal_en;

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // first entry of the state
        uint32 startPageOfRequest_u32;
        uint32 byteOfsFromBlockStart_u32;
        uint16 blkLength_u16;
        uint8 byteOfsInPage_u8;

        // Calculate the first requested page and the byte offset within this page
        byteOfsFromBlockStart_u32 =
                        ((uint32)rba_FeeFs1x_BC_HdrPgValueMap_dat0_e) + fsm_pst->readBytes_u16 + fsm_pst->vldtBytes_u16;
        startPageOfRequest_u32 = rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].pageNr_u32 +
                (byteOfsFromBlockStart_u32 / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);

        byteOfsInPage_u8 = (uint8)(byteOfsFromBlockStart_u32 % RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);

        if((fsm_pst->readBytes_u16 != 0u) || (fsm_pst->userBuf_pu8 == NULL_PTR))
        {
            blkLength_u16 = fsm_pst->verifLen_u16 - (fsm_pst->readBytes_u16 + fsm_pst->vldtBytes_u16);
        }
        else
        {
            blkLength_u16 = fsm_pst->jobOfs_u16 - fsm_pst->vldtBytes_u16;
        }

        fsm_pst->vldtBytes_u16 += blkLength_u16;

        rba_FeeFs1x_PASrv_calcCRC32(startPageOfRequest_u32,
                blkLength_u16,
                &(fsm_pst->dataCRC_u32),
                byteOfsInPage_u8);
    }

    // Cyclic call the do-function
    retValCRCDo_en = rba_FeeFs1x_PASrv_calcCRC32Do();

    switch(retValCRCDo_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            if((fsm_pst->readBytes_u16 != 0u) || (fsm_pst->userBuf_pu8 == NULL_PTR))
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_BC_readVldtFls2RamStates_compareDataCrc_e);
            }
            else
            {
                FEE_PRV_LIBSC(rba_FeeFs1x_BC_readVldtFls2RamStates_readBlkDataToUsrBuf_e);
            }

            retVal_en = rba_FeeFs1x_Pending_e;
            break;
        }
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = retValCRCDo_en;
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2RamDo_compareDataCrc
 * Statefunction of rba_FeeFs1x_BC_readVldtFls2RamDo. The function compares the data CRC. if CRC mismatches it checks if
 * legacy ROB bit is set and transfers the control to next state to read and compare against legacy ROB.
 *
 * \param   fsm_pst                : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e    : Valid on data level
 * \retval  rba_FeeFs1x_JobFailed_e: Data is invalid
 * \retval  rba_FeeFs1x_Pending_e  : Data crc didnt match and legacy ROB crc must be checked; control set to next state
 * \retVal  no other return value is possible in this function.
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_compareDataCrc(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // compare the calculated data CRC result to the one stored in the header
    if(fsm_pst->dataCRC_u32 == rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].dataCrc_u32)
    {
        // data is valid
        retVal_en = rba_FeeFs1x_JobOK_e;
    }
    else
    {
        // data is different
        // check if legacy ROB is activated
        if(rba_FeeFs1x_BC_getCpyLegacyRobCRCBit(fsm_pst->copy_en))
        {
            // switch to next state to read out and compare against legacy ROB Crc
            FEE_PRV_LIBSC(rba_FeeFs1x_BC_readVldtFls2RamStates_compareLegacyRobDataCrc_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            // data crc does not match, invalid copy
            retVal_en = rba_FeeFs1x_JobFailed_e;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BC_readVldtFls2RamDo_compareLegacyRobDataCrc
 * Statefunction of rba_FeeFs1x_BC_readVldtFls2RamDo. The function reads out legacy ROB checksum and compares against
 * computed data CRC. if CRC matches it returns JobOK else it returns JobFailed.
 *
 * \param   fsm_pst                     : statemachine pointer
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e         : valid on data level
 * \retval  rba_FeeFs1x_JobFailed_e     : data is invalid
 * \retval  rba_FeeFs1x_Pending_e       : reading out of ROB crc ongoing, call again
 * \retval rba_FeeFs1x_ErrorInternal_e : Internal error during processing
 * \retval rba_FeeFs1x_ErrorExternal_e : External error during processing
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BC_readVldtFls2RamDo_compareLegacyRobDataCrc(rba_FeeFs1x_BC_readVldtFls2Ram_tst * const fsm_pst)
{
    uint32                  legacyRobDataCRC_u32;
    rba_FeeFs1x_RetVal_ten  retValLegacyRobCRCRead_en;
    rba_FeeFs1x_RetVal_ten  retVal_en;

    // If it's the first call of the state
    if(FEE_PRV_LIBENTRY)
    {
        // Valid copy,
        // present in DFLASH,
        // requested bytes are not succeeding the found copy
        uint32 startPageOfRequest_u32;
        uint32 byteOfsFromBlockStart_u32;
        uint8 byteOfsInPage_u8;

        // Calculate the first requested page and the byte offset within this page
        byteOfsFromBlockStart_u32 = ((uint32)rba_FeeFs1x_BC_HdrPgValueMap_dat0_e) + fsm_pst->verifLen_u16;

        startPageOfRequest_u32 = rba_FeeFs1x_BC_blockCopies_ast[fsm_pst->copy_en].pageNr_u32 +
                (byteOfsFromBlockStart_u32 / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);
        byteOfsInPage_u8 = (uint8)(byteOfsFromBlockStart_u32 % RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE);

        // trigger the unbuffered read directly into the user buffer
        rba_FeeFs1x_PARead_unbufferedRead(
                startPageOfRequest_u32,
                RBA_FEEFS1X_BC_NR_ROBCRCBYTES,
                rba_FeeFs1x_BC_robDataCrcBuf_au8,
                byteOfsInPage_u8);
    }

    // Cyclic call the do-function
    retValLegacyRobCRCRead_en = rba_FeeFs1x_PARead_unbufferedReadDo();

    switch(retValLegacyRobCRCRead_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            // robust data CRC in DFlash is always stored in big endian format
            legacyRobDataCRC_u32 = rba_MemLib_Cnv4ByteBigEndToNative_u32(rba_FeeFs1x_BC_robDataCrcBuf_au8);

            if (legacyRobDataCRC_u32 == fsm_pst->dataCRC_u32)
            {
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // data crc does not match, invalid copy
                retVal_en = rba_FeeFs1x_JobFailed_e;
            }
            break;
        }
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorInternal_e:
        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = retValLegacyRobCRCRead_en;
            break;
        }
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
            break;
        }
    }


    return retVal_en;
}


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

