/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*
 * The medium unit is the lowest layer in the Fee architecture.
 * It abstracts direct flash access and Fls_Read flash access.
 * The Fee assumes that it is the only flash user.
 * To reduce the number of error paths it is not visible to higher layers,
 * whether starting a flash job returned with E_NOT_OK or
 * whether the flash job finished with MEMIF_JOB_FAILED.
 * Additionally it contains a big buffer which is used for various operations,
 * mainly the buffer is used for writing internal management data and during the data sector change.
 * The medium unit does also provide blocking flash access,
 * this means polling until Fls_GetJobResult != MEMIF_JOB_PENDING
 * or asynchronous flash access.
*/

#ifndef FEE_PRV_MEDIUM_H
#define FEE_PRV_MEDIUM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"
#include "Fee_Prv_Lib.h"

/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "MemIf_Types.h"
#include "Fee_Prv_MediumTypes.h"
#include "Fee_PrvTypes.h"
#include "Fee_Cfg.h"
#include "Fee_Prv_FsIf.h"
#include "rba_MemLib.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern void Fee_Prv_MediumSetSync(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, boolean newSync_b);
extern void Fee_Prv_MediumRead(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, void * bfr_pv, uint32 length_u32);
extern void Fee_Prv_MediumWrite(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, void const * bfr_pcv, uint32 length_u32, Fee_Prv_MediumSeqWrite_ten sequence_en);
extern void Fee_Prv_MediumErase(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint32 length_u32);
extern void Fee_Prv_MediumCompareFls2Buf(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrFls_u32, uint8 nrPageShift_u8, void const * bfr_pcv, uint32 length_u32);
extern void Fee_Prv_MediumCompareFls2Fls(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrFls1_u32, uint8 nrPageShift1_u8, uint32 adrFls2_u32, uint8 nrPageShift2_u8, uint32 length_u32);
extern void Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32);
extern void Fee_Prv_MediumShredder(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint8 value_u8, uint32 length_u32, boolean isTraErrSuppReqd_b);
extern void Fee_Prv_MediumPatShredder(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8,uint32 patOffs_u32, uint32 patSize_u32, void const * patBfr_pcv, uint32 length_u32, boolean isTraErrSuppReqd_b);
extern void Fee_Prv_MediumCalcCrc(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32, uint32 * crc_pu32, boolean firstCall_b);
extern void Fee_Prv_MediumCopy(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrSrc_u32, uint8 nrPageShiftSrc_u8, uint32 adrDest_u32, uint8 nrPageShiftDest_u8, uint32 length_u32, Fee_Prv_MediumSeqCopy_ten sequence_en);

extern MemIf_JobResultType Fee_Prv_MediumReadDo           (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumWriteDo          (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumEraseDo          (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumCompareFls2BufDo (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumCompareFls2FlsDo (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumBlankCheckDo     (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumShredderDo       (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumCalcCrcDo        (Fee_Rb_DeviceName_ten deviceName_en);
extern MemIf_JobResultType Fee_Prv_MediumCopyDo           (Fee_Rb_DeviceName_ten deviceName_en);

extern void                Fee_Prv_MediumSelBufSetIterator(Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8, uint32 adr_u32, uint8 nrPageShift_u8, uint32 maxPrefetchLen_u32);
extern uint32              Fee_Prv_MediumSelBufGetIterator(Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8);
extern MemIf_JobResultType Fee_Prv_MediumSelBufNext       (Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8, uint32 length_u32, void const ** ptr_ppcv);
extern MemIf_JobResultType Fee_Prv_MediumSelBufPre        (Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8, uint32 length_u32, void const ** ptr_ppcv);

#  if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)

extern boolean Fee_Prv_MediumHasWlError(Fee_Rb_DeviceName_ten deviceName_en);
extern void Fee_Prv_MediumGetWlData(Fee_Rb_DeviceName_ten deviceName_en, uint32 * wlAdr_pu32, uint32 * wlLen_pu32, void const ** wlBfr_ppcv);

#  else

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/

LOCAL_INLINE boolean Fee_Prv_MediumHasWlError(Fee_Rb_DeviceName_ten deviceName_en)
{
    (void)deviceName_en;

    return(FALSE);
}

LOCAL_INLINE void Fee_Prv_MediumGetWlData(Fee_Rb_DeviceName_ten deviceName_en, uint32 * wlAdr_pu32, uint32 * wlLen_pu32, void const ** wlBfr_ppcv)
{
    (void)deviceName_en;

    if(NULL_PTR != wlAdr_pu32)
    {
        /* Avoid violation of MISRA rule 8.13 (QAC_3673) */
        *wlAdr_pu32 = 0u;
    }

    if(NULL_PTR != wlLen_pu32)
    {
        /* Avoid violation of MISRA rule 8.13 (QAC_3673) */
        *wlLen_pu32 = 0u;
    }

    if(NULL_PTR != wlBfr_ppcv)
    {
        /* Avoid violation of MISRA rule 8.13 (QAC_3673) */
        *wlBfr_ppcv = NULL_PTR;
    }

    return;
}

/*
 * Use inlines instead of defines since defines lead to compiler warnings, example: Comparison >= 0
 * Compiler should still be able to perfomr maximum optimizations with inlines.
 */
#  endif

LOCAL_INLINE void                Fee_Prv_MediumCancel(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                Fee_Prv_MediumInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumPreSync(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint32 length_u32, uint32 maxPrefetchLen_u32, void const ** ptr_ppcv);
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumPatShredderDo(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE void                Fee_Prv_MediumSetIterator(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint32 maxPrefetchLen_u32);
LOCAL_INLINE uint32              Fee_Prv_MediumGetIterator(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumNext(Fee_Rb_DeviceName_ten deviceName_en, uint32 length_u32, void const ** ptr_ppcv);
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumPre(Fee_Rb_DeviceName_ten deviceName_en, uint32 length_u32, void const ** ptr_ppcv);
LOCAL_INLINE boolean             Fee_Prv_MediumGetWlErrHndlgEnad(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint32              Fee_Prv_MediumGetWlSize(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE Std_ReturnType      Fee_Prv_MediumGetSectorInfo(Fee_Rb_DeviceName_ten idFeeDev_en, Fls_Rb_FlsDevice_ten idFlsDev_en, Fls_Rb_AddressType adrTarget_u32, Fls_Rb_SectorInfo_tst * xSectorInfo_pst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/*
 * \brief Cancel function for the Medium unit resetting all variables
 * For faster init (delay due to calling of function with parameter), the function is made inline.
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE void Fee_Prv_MediumCancel(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    uint32 i_u32;

    Fee_Prv_MediumData_tst*             mediumData_pst = &deviceConfigTable_pcst->feeData_pst->mediumData_st;
    Fee_Rb_DeviceName_ten               deviceName_en  = deviceConfigTable_pcst->deviceName_en;
    Fee_Prv_Medium_tst*                 medium_pst;
    Fee_Prv_MediumRead_tst*             mediumRead_pst;
    Fee_Prv_MediumCalcCrc_tst*          mediumCalcCrc_pst;
    Fee_Prv_MediumLoad_tst*             mediumLoad_pst;
    Fee_Prv_MediumWrite_tst*            mediumWrite_pst;
    Fee_Prv_MediumCopy_tst*             mediumCopy_pst;
    Fee_Prv_MediumCompareFls2Fls_tst*   mediumCompareFls2Fls_pst;
    Fee_Prv_MediumBufData_tst*          mediumBufData_pst;

    medium_pst                          = &mediumData_pst->medium_st;
    medium_pst->dataBuf_un.dataChg_pu8  = NULL_PTR;
    medium_pst->flsAdr_u32              = 0u;
    medium_pst->nrBytToProc_u32         = 0u;
    medium_pst->adrOffs_u16             = 0u;
    medium_pst->idFlsJobType_en         = FEE_PRV_MEDIUM_FLS_NONE_E;
    medium_pst->deviceName_en           = deviceName_en;
    medium_pst->flsDevId_en             = Fls_Rb_GetDeviceIdFromDrvIndex(deviceConfigTable_pcst->deviceIdx_u8);
    medium_pst->lastFlsResult_en        = E_NOT_OK;
    medium_pst->isTraErrSuppReqd_b      = FALSE;
    medium_pst->sync_b                  = FALSE;

    mediumRead_pst                      = &mediumData_pst->mediumRead_st;
    mediumRead_pst->adr_pcu8            = NULL_PTR;
    mediumRead_pst->bfr_pu8             = NULL_PTR;
    mediumRead_pst->length_u32          = 0u;
    mediumRead_pst->nrBytesDone_u32     = 0u;
    mediumRead_pst->isBufferHit_b       = FALSE;

    mediumCalcCrc_pst                   = &mediumData_pst->mediumCalcCrc_st;
    mediumCalcCrc_pst->crc_pu32         = NULL_PTR;
    mediumCalcCrc_pst->firstCall_b      = TRUE;
    mediumCalcCrc_pst->length_u32       = 0u;
    mediumCalcCrc_pst->nrBytesDone_u32  = 0u;

    mediumLoad_pst                      = &mediumData_pst->mediumLoad_st;
    mediumLoad_pst->state_en            = FEE_PRV_LOAD_IDLE_E;


    for(i_u32=0u; i_u32<FEE_PRV_CFG_RB_NR_PAGE_BUFFER; i_u32++)
    {
        mediumBufData_pst                     = &mediumData_pst->mediumBufData_ast[i_u32];
        mediumBufData_pst->maxPrefetchLen_u32 = 0u;
        mediumBufData_pst->iterator_u32       = 0u;
        mediumBufData_pst->reloadAdr_u32      = 0u;
        mediumBufData_pst->nrBytes2Load_u32   = 0u;
        // setting the limits implicitly sets the buffer dirty --> reload
        mediumBufData_pst->limitHi_u32        = 0u;
        mediumBufData_pst->limitLo_u32        = 0u;
        mediumBufData_pst->nrPageShift_u8     = 0u;
    }

    mediumWrite_pst                     = &mediumData_pst->mediumWrite_st;
    mediumWrite_pst->bfr_pcv            = NULL_PTR;
    mediumWrite_pst->data2Program_pcv   = NULL_PTR;
    mediumWrite_pst->adrDest_u32        = 0u;
    mediumWrite_pst->length_u32         = 0u;
    mediumWrite_pst->done_u32           = 0u;
    mediumWrite_pst->bytes2Program_u32  = 0u;
    mediumWrite_pst->state_en           = FEE_PRV_MEDIUM_WRITE_IDLE_E;
    mediumWrite_pst->deviceName_en      = deviceName_en;
    mediumWrite_pst->nrPageShift_u8     = 0u;
    mediumWrite_pst->isMedBfrByp_b      = FALSE;
    mediumWrite_pst->isTraErrSuppReqd_b = FALSE;
    mediumWrite_pst->isCmpSkip_b        = FALSE;
    mediumWrite_pst->isWlBckpReqd_b     = FALSE;
    mediumWrite_pst->hasWlError_b       = FALSE;
    mediumWrite_pst->entry_b            = TRUE;

    mediumCopy_pst                      = &mediumData_pst->mediumCopy_st;
    mediumCopy_pst->adrSrc_u32          = 0u;
    mediumCopy_pst->adrDest_u32         = 0u;
    mediumCopy_pst->length_u32          = 0u;
    mediumCopy_pst->done_u32            = 0u;
    mediumCopy_pst->bytes2Copy_u32      = 0u;
    mediumCopy_pst->state_en            = FEE_PRV_MEDIUM_COPY_IDLE_E;
    mediumCopy_pst->writeSeq_en         = FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COMP_E;
    mediumCopy_pst->deviceName_en       = deviceName_en;
    mediumCopy_pst->nrPageShiftSrc_u8   = 0u;
    mediumCopy_pst->nrPageShiftDest_u8  = 0u;
    mediumCopy_pst->entry_b             = TRUE;

    mediumCompareFls2Fls_pst                          = &mediumData_pst->mediumCompareFls2Fls_st;
    mediumCompareFls2Fls_pst->adrFls1_u32             = 0u;
    mediumCompareFls2Fls_pst->adrFls2_u32             = 0u;
    mediumCompareFls2Fls_pst->length_u32              = 0u;
    mediumCompareFls2Fls_pst->nrBytesDone_u32         = 0u;
    mediumCompareFls2Fls_pst->nrBytesCurrentChunk_u32 = 0u;
    mediumCompareFls2Fls_pst->state_en                = FEE_PRV_MEDIUM_COMPAREFLS2FLS_IDLE_E;
    mediumCompareFls2Fls_pst->deviceName_en           = deviceName_en;
    mediumCompareFls2Fls_pst->nrPageShift1_u8         = 0u;
    mediumCompareFls2Fls_pst->nrPageShift2_u8         = 0u;
    mediumCompareFls2Fls_pst->entry_b                 = TRUE;

#  if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
    rba_MemLib_MemSet(deviceConfigTable_pcst->mediumRescueBfr_pcu8, 0u , FEE_PRV_CFG_WL_ERROR_HNDLG_WLSIZE);
#  endif

    return;
}

/*
 * \brief Init function for the Medium unit resetting all variables
 * For faster init (delay due to calling of function with parameter), the function is made inline.
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE void Fee_Prv_MediumInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_MediumCancel(deviceConfigTable_pcst);

    return;
}

/**
 * \brief   Without direct DFLASH access, set the current iterator to the given address,
 *          set a pointer to the current iterator then decrease the iterator for specified length.
 *          With direct DFLASH access, return the address into the DFLASH. Without, point into the buffered area.
 *          This function only is allowed during Fee_init (synchronous Fls job execution).
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   adrRead_u32         Address in DFLASH
 * \param   length_u32          Amount of bytes to step after pointer is set to current iterator
 * \param   nrBytesPrefetch_u32 Amount of bytes to be buffered if a rebuffering is required
 * \param   ptr_ppcv            Pointer to the pointer which shall be set to the address of the iterator
 *
 * \retval  MEMIF_JOB_OK        Job finished successfully
 * \retval  MEMIF_JOB_FAILED    Job finished with errors. With direct DFLASH access, this return value cannot occur.
 * \retval  MEMIF_JOB_PENDING   This return value is not possible when the function is called during Fee_init. Pay attention to not call this function outside Fee_Init execution
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumPreSync(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32 , uint32 length_u32 , uint32 maxPrefetchLen_u32, void const ** ptr_ppcv)
{
    MemIf_JobResultType result_en;

#  if (!defined(FEE_PRV_MEDIUM_DIRECT_READ_ALLOWED) || (TRUE == FEE_PRV_MEDIUM_DIRECT_READ_ALLOWED))
    if (Fee_Prv_FsIfIsDirectMemoryReadAllowed(deviceName_en))
    {
        /* MR12 RULE 11.6 VIOLATION: Addresses of type uint32 can safely be casted to pointer in this situation as it
         * guaranteed that the code is activated if the pointer size is <=4bytes */
        *ptr_ppcv = (void const *)(adr_u32);

        result_en = MEMIF_JOB_OK;
    }
    else
#  endif
    {
        // set the iterator to the requested address
        Fee_Prv_MediumSetIterator(deviceName_en, adr_u32 , maxPrefetchLen_u32);

        // get the requested address with backward buffering
        result_en = Fee_Prv_MediumPre(deviceName_en, length_u32 , ptr_ppcv);
    }
    return(result_en);
}

/**
 * \brief   Map Fee_Prv_MediumPatShredderDo to Fee_Prv_MediumShredderDo
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumPatShredderDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    return(Fee_Prv_MediumShredderDo(deviceName_en));
}

/**
 * \brief   Set the iterator to a location in the flash for the first buffer.
 *
 * \param   deviceName_en       Device (or Instance) for which the job has to be performed
 * \param   adr_u32             Address in flash where the iterator shall point to
 * \param   maxPrefetchLen_u32  Maximum amount of data to load including prefetch
*/
LOCAL_INLINE void Fee_Prv_MediumSetIterator(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint32 maxPrefetchLen_u32)
{
    /* always use the first buffer */
    Fee_Prv_MediumSelBufSetIterator(deviceName_en, 0u, adr_u32, 0u, maxPrefetchLen_u32);

    return;
}

/**
 * \brief   Get the value of the flash iterator of the first buffer.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 *
 * \seealso Fee_Prv_MediumSetIterator
*/
LOCAL_INLINE uint32 Fee_Prv_MediumGetIterator(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* use the first buffer */
    return(Fee_Prv_MediumSelBufGetIterator(deviceName_en, 0u));
}

/**
 * \brief   Set a pointer to the current iterator for the first buffer then increase the iterator for specified length.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   length_u32       Amount of bytes to step after pointer is set to current iterator
 * \param   ptr_ppcv         Pointer to the pointer which shall be set to the address of the iterator
 *
 * \retval  MEMIF_JOB_PENDING   Job still pending
 * \retval  MEMIF_JOB_OK        Job finished successfully
 * \retval  MEMIF_JOB_FAILED    Job finished with errors
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumNext(Fee_Rb_DeviceName_ten deviceName_en, uint32 length_u32, void const ** ptr_ppcv)
{
    /* use buffer one */
    return(Fee_Prv_MediumSelBufNext(deviceName_en, 0u, length_u32, ptr_ppcv));
}

/**
 * \brief   Set a pointer to the current iterator then decrease the iterator for specified length.
 *          First buffer is used.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   length_u32       Amount of bytes to step after pointer is set to current iterator
 * \param   ptr_ppcv         Pointer to the pointer which shall be set to the address of the iterator
 *
 * \retval  MEMIF_JOB_PENDING   Job still pending
 * \retval  MEMIF_JOB_OK        Job finished successfully
 * \retval  MEMIF_JOB_FAILED    Job finished with errors
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumPre(Fee_Rb_DeviceName_ten deviceName_en, uint32 length_u32, void const ** ptr_ppcv)
{
    /* use the first buffer */
    return(Fee_Prv_MediumSelBufPre(deviceName_en, 0u, length_u32, ptr_ppcv));
}

/**
 * \brief   Get if wordline errors handling is enabled for given instance
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \return  Wordline error handling configured
*/
LOCAL_INLINE boolean Fee_Prv_MediumGetWlErrHndlgEnad(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* unused parameters */
    (void)deviceName_en;

    // Currently only Fs2 uses wordline handling - reuse value directly from config without multi-instance handling */
    return(FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED);
}

/**
 * \brief   Get the size of a single wordline.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \return  Size of a wordline in the flash in bytes
*/
LOCAL_INLINE uint32 Fee_Prv_MediumGetWlSize(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* unused parameters */
    (void)deviceName_en;
#  if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
    // Currently only Fs2 uses wordline handling - reuse value directly from config without multi-instance handling */
    return(FEE_PRV_CFG_WL_ERROR_HNDLG_WLSIZE);
#  else
    return(0u);
#  endif
}

/**
 * \brief   Get the SectorInfo.
 *
 * \param  idFeeDev_en     Fee device (or instance), for which the sector info is requested
 * \param  idFlsDev_en     Fls device (or instance), for which the sector info is requested
 * \param  adrTarget_u32   Target address of the sector, for which the info is requested
 * \param  SectorInfo_pst  Pointer to location, where the sector info shall be copied into
 *
 *
 * \return  Function result
 * \retval  E_OK if sector info has been provided sucessfully in the given location
 * \retval  E_NOT_OK  if sector info could not be provided due to an error (e.g. missing Fls interface)
*/
LOCAL_INLINE Std_ReturnType Fee_Prv_MediumGetSectorInfo(Fee_Rb_DeviceName_ten idFeeDev_en, Fls_Rb_FlsDevice_ten idFlsDev_en, Fls_Rb_AddressType adrTarget_u32, Fls_Rb_SectorInfo_tst * xSectorInfo_pst)
{
    return(Fee_Prv_Lib_Fls_Rb_GetSectorInfo(
                idFlsDev_en,
                adrTarget_u32 - Fee_Prv_FsIfGetBaseAdr(idFeeDev_en),
                xSectorInfo_pst) );
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* FEE_PRV_CFG_COMMON_ENABLED */
# endif

/* FEE_PRV_MEDIUM_H */
#endif
