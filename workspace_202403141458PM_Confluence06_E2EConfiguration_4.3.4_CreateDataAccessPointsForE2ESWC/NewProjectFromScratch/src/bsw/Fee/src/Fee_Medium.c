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
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
#if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "Fee_Prv_Medium.h"
#include "Crc.h"
#include "rba_MemLib.h"
#include "Fee_Prv_Lib.h"
#include "Fee_Prv_Pers.h"
#include "Fee_Prv_Dbg.h"
#include "MemIf_Types.h"
#include "Fee_Rb_Idx.h"
#include "Fee_PrvTypes.h"
#include "Fee_Prv_Config.h"


/*
 **********************************************************************************************************************
 * Static declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

static void Fee_Prv_MediumCalcShiftedAdr(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint32 * adrShift_pu32, uint16 * adrOffs_pu16);
static MemIf_JobResultType Fee_Prv_MediumFls(Fee_Prv_Medium_tst * medium_pst, boolean isFirstReq_b);
static MemIf_JobResultType Fee_Prv_MediumFlsDo(Fee_Prv_Medium_tst * medium_pst);
static void Fee_Prv_MediumUpdBufsVldy(Fee_Prv_MediumData_tst* mediumData_pst, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32, Fee_Prv_Medium_BufBhvr_ten bufBhvr_en);
static boolean Fee_Prv_MediumIsBfrHit(Fee_Prv_MediumBufData_tst const * mediumBufData_pst, uint32 adr_u32, uint32 length_u32);
static MemIf_JobResultType Fee_Prv_MediumLoad(Fee_Prv_ConfigDeviceTable_tst const *  deviceConfigTable_pcst, uint8 bufIdx_u8, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32, uint32 maxPrefetchLen_u32, Fee_Prv_MediumLoadMode_ten loadMode_en);
static void Fee_Prv_MediumProgram(Fee_Prv_MediumData_tst * mediumData_pst, uint32 adr_u32, uint8 nrPageShift_u8, void const * bfr_pcv, uint32 length_u32, boolean isTraErrSuppReqd_b);
static MemIf_JobResultType Fee_Prv_MediumProgramDo(Fee_Prv_MediumData_tst * mediumData_pst);


/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/

LOCAL_INLINE void Fee_Prv_MediumSetBufDirty(Fee_Prv_MediumBufData_tst* mediumBufData_pst);
LOCAL_INLINE void Fee_Prv_MediumSetBufDirtyAndCopy(Fee_Prv_MediumBufData_tst* mediumBufData_pst, uint8 const *src_pcu8, uint8 *dst_pu8, uint32 length_u32);
LOCAL_INLINE void Fee_Prv_MediumSetBufDirtyAndRead(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_MediumBufData_tst* mediumBufData_pst, uint32 adr_u32, uint8 nrPageShift_u8, void * bfr_pv, uint32 length_u32);

LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_BlankCheck(Fee_Prv_MediumWrite_tst * fsm_pst);
# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_BackupWl  (Fee_Prv_MediumWrite_tst * fsm_pst);
# endif
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_CopyData  (Fee_Prv_MediumWrite_tst * fsm_pst);
# if(FALSE != FEE_RB_WRITE_PADDING_REQUIRED)
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_Padding   (Fee_Prv_MediumWrite_tst * fsm_pst);
# endif
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_Program   (Fee_Prv_MediumWrite_tst * fsm_pst);
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_Compare   (Fee_Prv_MediumWrite_tst * fsm_pst);
# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_DetnWlErr (Fee_Prv_MediumWrite_tst * fsm_pst);
# endif

LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCompareFls2Fls_Read   (Fee_Prv_MediumCompareFls2Fls_tst * fsm_pst);
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCompareFls2Fls_Compare(Fee_Prv_MediumCompareFls2Fls_tst * fsm_pst);

LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCopy_Read (Fee_Prv_MediumCopy_tst * fsm_pst);
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCopy_Write(Fee_Prv_MediumCopy_tst * fsm_pst);


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/**
 * \brief   Calculate the shifted address and its offset (e.g. for FeeFs3 wear-levelling)
 *
 * \param   deviceName_en    Device (or instance) for which the calculation has to be performed
 * \param   adr_u32          Requested address before shift
 * \param   nrPageShift_u8   Data shift to consider (in no. of pages)
 * \param   adrShift_pu32    Pointer to shifted address
 * \param   adrOffs_pu16     Pointer to shifted address offset
*/
static void Fee_Prv_MediumCalcShiftedAdr(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint32 * adrShift_pu32, uint16 * adrOffs_pu16)
{
    if(0u < nrPageShift_u8)
    {
        uint32 adrRowLoglStart_u32 = Fee_Prv_FsIfGetLogicalRowStartAdr(deviceName_en, adr_u32);
        uint32 nrBytInRow_u32      = Fee_Prv_FsIfGetLogicalRowSize(deviceName_en);

        *adrOffs_pu16 = (uint16)((adr_u32-adrRowLoglStart_u32) + ((uint32)Fee_Prv_FsIfGetLogicalPageSize(deviceName_en)*nrPageShift_u8));

        if(*adrOffs_pu16 >= nrBytInRow_u32)
        {
            *adrOffs_pu16 -= (uint16)nrBytInRow_u32;
        }

        *adrShift_pu32 = adrRowLoglStart_u32;
    }
    else
    {
        *adrShift_pu32 = adr_u32;
        *adrOffs_pu16  = 0u;
    }

    return;
}

/**
 * \brief   (Re-)Start a flash job in case flash data might be shifted by an offset (e.g. for FeeFs3 wear-levelling)
 *
 * \param   medium_pst          Pointer to the medium variable for which the job has to be started
 * \param   isFirstReq_b        First (TRUE) or second (FALSE) request for this job
 *
 * \retval  MEMIF_JOB_PENDING   Fls job (re-)started and pending
 * \retval  MEMIF_JOB_FAILED    Fls job could could not be (re-)started
 * \retval  MEMIF_JOB_OK        Fls job done and no need to restart it
*/
static MemIf_JobResultType Fee_Prv_MediumFls(Fee_Prv_Medium_tst * medium_pst, boolean isFirstReq_b)
{
    /* Return variable */
    MemIf_JobResultType    result_en = MEMIF_JOB_PENDING;

    /* Local variables */
    uint32 nrBytInRow_u32 = Fee_Prv_FsIfGetLogicalRowSize(medium_pst->deviceName_en);
    uint32 nrBytCurr_u32  = rba_MemLib_Min(medium_pst->nrBytToProc_u32, nrBytInRow_u32 - medium_pst->adrOffs_u16);
    uint32 relFlsAdr_u32;

    /* First request? */
    if(isFirstReq_b)
    {
        /* Process data from offset until end of row; remaining part must be processed in a second request */
        medium_pst->flsAdr_u32 = medium_pst->flsAdr_u32 + medium_pst->adrOffs_u16;
    }
    /* Second request, and some data is left for processing? */
    else if(nrBytCurr_u32 < medium_pst->nrBytToProc_u32)
    {
        /* Pointer to buffer must be advanced according to the amount of data processed in the first request. For
         * Fls_Erase and Fls_BlankCheck, this can be a NULL_PTR, but adding nrBytCurr_u32 to 0 is no problem as long
         * as the pointer is not dereferenced. Anyway, there is neither Fls_Erase nor Fls_BlankCheck in FeeFs3, and
         * FeeFs1x/FeeFs2 don't go through this branch at all. Also note that medium_pst->dataBuf_un.dataChg_pu8 equals
         * medium_pst->dataBuf_un.dataCon_pcu8, i.e. it doesn't matter which pointer we advance here. */
        medium_pst->dataBuf_un.dataChg_pu8 = medium_pst->dataBuf_un.dataChg_pu8 + nrBytCurr_u32;

        /* Process remaining data from beginning of row */
        medium_pst->flsAdr_u32 = (medium_pst->flsAdr_u32 + nrBytCurr_u32) - nrBytInRow_u32;
        nrBytCurr_u32 = medium_pst->nrBytToProc_u32 - nrBytCurr_u32;

        /* Avoid a third request */
        medium_pst->nrBytToProc_u32 = 0u;
    }
    /* Second request, but there is no data left for processing? */
    else
    {
        /* We are done! */
        medium_pst->idFlsJobType_en = FEE_PRV_MEDIUM_FLS_NONE_E;
        medium_pst->nrBytToProc_u32 = 0u;
    }


    relFlsAdr_u32 = medium_pst->flsAdr_u32 - Fee_Prv_FsIfGetBaseAdr(medium_pst->deviceName_en);

    /* Start requested Fls job */
    switch(medium_pst->idFlsJobType_en)
    {
        case FEE_PRV_MEDIUM_FLS_READ_E:
        {
            medium_pst->lastFlsResult_en = Fee_Prv_Lib_Fls_Read(
                    medium_pst->flsDevId_en,
                    relFlsAdr_u32 ,
                    medium_pst->dataBuf_un.dataChg_pu8,
                    nrBytCurr_u32 );
        }
        break;

        case FEE_PRV_MEDIUM_FLS_WRITE_E:
        {
            medium_pst->lastFlsResult_en = Fee_Prv_Lib_Fls_Write(
                    medium_pst->flsDevId_en,
                    relFlsAdr_u32 ,
                    medium_pst->dataBuf_un.dataCon_pcu8,
                    nrBytCurr_u32,
                    medium_pst->isTraErrSuppReqd_b );
        }
        break;

        case FEE_PRV_MEDIUM_FLS_ERASE_E:
        {
            /* Erase isn't row- but sector wise, and always with medium_pst->adrOffs_u16 == 0u. Therefore,
             * we must pass the whole length medium_pst->nrBytToProc_u32 for the erase (NOT nrBytCurr_u32). */
            medium_pst->lastFlsResult_en = Fee_Prv_Lib_Fls_Erase(
                    medium_pst->flsDevId_en,
                    relFlsAdr_u32 ,
                    medium_pst->nrBytToProc_u32);

            /* Avoid second call of Fee_Prv_Lib_Fls_Erase */
            medium_pst->nrBytToProc_u32 = 0u;
        }
        break;

        case FEE_PRV_MEDIUM_FLS_COMPARE_E:
        {
            medium_pst->lastFlsResult_en = Fee_Prv_Lib_Fls_Compare(
                    medium_pst->flsDevId_en,
                    relFlsAdr_u32 ,
                    medium_pst->dataBuf_un.dataCon_pcu8,
                    nrBytCurr_u32 );
        }
        break;

        case FEE_PRV_MEDIUM_FLS_BLANKCHECK_E:
        {
            medium_pst->lastFlsResult_en = Fee_Prv_Lib_Fls_BlankCheck(
                    medium_pst->flsDevId_en,
                    relFlsAdr_u32,
                    nrBytCurr_u32);
        }
        break;

        case FEE_PRV_MEDIUM_FLS_NONE_E:
        {
            result_en = MEMIF_JOB_OK;
        }
        break;

        default:
        {
            medium_pst->lastFlsResult_en = E_NOT_OK;
            result_en = MEMIF_JOB_FAILED;
        }
        break;
    }

    return(result_en);
}

/**
 * \brief   Check the status of flash jobs.
 *          In synchronous mode call the Fls_MainFunction until the job is not pending any more.
 *          To reduce the number of software paths, MEMIF_JOB_FAILED and E_NOT_OK are both mapped to MEMIF_JOB_FAILED.
 *          This way the higher layers do not know whether an error occurred during job placing or job execution.
 *          In the end the reaction is typically always the same: Retry or abort.
 *          In order to differentiate between hardware failures (with return value MEMIF_JOB_FAILED) and comparison
 *          mismatches of Fls compare jobs, the return value MEMIF_BLOCK_INCONSISTENT is kept for the latter. For
 *          these two situations, different reaction strategies can be required.
 *
 * \param   medium_pst                 Pointer to the medium variable for which the job has to be performed
 *
 * \retval  MEMIF_JOB_PENDING          Job still pending
 * \retval  MEMIF_JOB_OK               Job finished successfully
 * \retval  MEMIF_JOB_FAILED           Job finished with errors
 * \retval  MEMIF_BLOCK_INCONSISTENT   Compare job finished with mismatch between contents in flash and data buffer
 *                                     Blank Check job has been executed and the result is not blank
*/
static MemIf_JobResultType Fee_Prv_MediumFlsDo(Fee_Prv_Medium_tst * medium_pst)
{
    MemIf_JobResultType result_en = MEMIF_JOB_FAILED;
    if(E_OK == medium_pst->lastFlsResult_en)
    {
        if(medium_pst->sync_b)
        {
            // get the pointer to the config table to get the hook function to be called
            Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(medium_pst->deviceName_en);

            do
            {
                /*
                 * The hook function call enables the user to serve tasks, which might be needed to keep the system
                 * stable during the runtime of the loop.
                 */
                if(deviceConfigTable_pcst->syncLoopFlsMainHook_pfn != NULL_PTR)
                {
                    deviceConfigTable_pcst->syncLoopFlsMainHook_pfn();
                }
                Fee_Prv_Lib_Fls_MainFunction(medium_pst->flsDevId_en);
                result_en = Fee_Prv_Lib_Fls_GetJobResult(medium_pst->flsDevId_en, medium_pst->flsAdr_u32);
            } while(MEMIF_JOB_PENDING == result_en);
            /* Evaluation of while loop: This while loop is executed only during init phase, which operates in synchronous
             * mode. In worst case the while loop will be stuck infinitely when the given Fls job is never completed.
             * This can be handled by using the timeout feature of Fls.
             * Also, for longer execution time, projects could use the FeeRbSyncFlsMainLoopHook for safe handling. */
        }
        else
        {
            result_en = Fee_Prv_Lib_Fls_GetJobResult(medium_pst->flsDevId_en, medium_pst->flsAdr_u32);
        }

        /* In case the flash data is stored with an offset, we have to restart the Fls job to process the second request */
        if(MEMIF_JOB_OK == result_en)
        {
            result_en = Fee_Prv_MediumFls(medium_pst, FALSE);
        }

        Fee_Prv_DbgFailDump(medium_pst->deviceName_en, result_en);
    }
    else
    {
        Fee_Prv_DbgDump(medium_pst->deviceName_en);
    }

    return(result_en);
}

/**
 * \brief   Clear all data of the indexed buffer
 *
 * \param   mediumBufData_pst  Pointer to the medium data for access to buffer working data
*/
LOCAL_INLINE void Fee_Prv_MediumSetBufDirty(Fee_Prv_MediumBufData_tst* mediumBufData_pst)
{
    mediumBufData_pst->limitLo_u32 = 0;
    mediumBufData_pst->limitHi_u32 = 0;

    return;
}

/**
 * \brief   Clear indexed buffer and perform requested copy operation
 *
 * \param   mediumBufData_pst  Pointer to the medium data for access to buffer working data
 * \param   src_pcu8           Source pointer
 * \param   dst_pu8            Destination pointer
 * \param   length_u32         Length of data to be copied
*/
LOCAL_INLINE void Fee_Prv_MediumSetBufDirtyAndCopy(Fee_Prv_MediumBufData_tst* mediumBufData_pst, uint8 const *src_pcu8, uint8 *dst_pu8, uint32 length_u32)
{
    /* delete buffer content, in case the buffer is used for intermediate storage */
    Fee_Prv_MediumSetBufDirty(mediumBufData_pst);
    rba_MemLib_MemCopy(src_pcu8, dst_pu8, length_u32);

    return;
}

/**
 * \brief   Clear indexed buffer and perform requested copy operation
 *
 * \param   deviceName_en       Device (or Instance) for which the job has to be performed
 * \param   mediumBufData_pst   Pointer to the medium data for access to buffer working data
 * \param   adr_u32             Address to read from
 * \param   nrPageShift_u8      Data shift to consider (in no. of pages)
 * \param   bfr_pv              Buffer to read to
 * \param   length_u32          Amount of bytes to read
*/
LOCAL_INLINE void Fee_Prv_MediumSetBufDirtyAndRead(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_MediumBufData_tst* mediumBufData_pst, uint32 adr_u32, uint8 nrPageShift_u8, void * bfr_pv, uint32 length_u32)
{
    Fee_Prv_MediumRead(deviceName_en, adr_u32, nrPageShift_u8, bfr_pv, length_u32);
    /* delete data of the indexed buffer before reloading it from flash */
    Fee_Prv_MediumSetBufDirty(mediumBufData_pst);

    return;
}

/**
 * \brief Update validity of the buffer content on an address collision
 *
 * bufBhvr_en == FEE_PRV_MEDIUM_BUF_CLR_E:
 * If passed address range is at least partially matching the buffered data,
 *          all data of all buffers is cleared
 *
 * bufBhvr_en == FEE_PRV_MEDIUM_BUF_CUT_E:
 * If passed address range is at least partially matching the buffered data,
 *          data of all buffers above the passed start address is cleared
 *
 * \param   mediumData_pst   Pointer to the medium data for access to buffer working data
 * \param   adr_u32          Start address of the modified address range
 * \param   nrPageShift_u8   Data shift to consider (in no. of pages)
 * \param   length_u32       Length of the modified address range
 * \param   bufBhvr_en       FEE_PRV_MEDIUM_BUF_CLR_E: clear buffered address range, FEE_PRV_MEDIUM_BUF_CUT_E: limit buffered address range
*/
static void Fee_Prv_MediumUpdBufsVldy(Fee_Prv_MediumData_tst* mediumData_pst, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32, Fee_Prv_Medium_BufBhvr_ten bufBhvr_en)
{
    uint8  i_u8;
    uint32 endAdr_u32;
    endAdr_u32 = adr_u32 + length_u32;
    for(i_u8 = 0u; i_u8 < FEE_PRV_CFG_RB_NR_PAGE_BUFFER; i_u8++)
    {
        if(adr_u32 <= mediumData_pst->mediumBufData_ast[i_u8].limitLo_u32)
        {
            if(endAdr_u32 > mediumData_pst->mediumBufData_ast[i_u8].limitLo_u32)
            {
                /* in case the requested range overlaps the beginning of the buffered data, delete all */
                Fee_Prv_MediumSetBufDirty(&mediumData_pst->mediumBufData_ast[i_u8]);
            }
        }
        else
        {
            /* address > limitLo */
            if(adr_u32 < mediumData_pst->mediumBufData_ast[i_u8].limitHi_u32)
            {
                if(    (FEE_PRV_MEDIUM_BUF_CLR_E == bufBhvr_en)
                    || (nrPageShift_u8 != mediumData_pst->mediumBufData_ast[i_u8].nrPageShift_u8) )
                {
                    /* Delete the buffered data, if passed address range is within the buffered data */
                    Fee_Prv_MediumSetBufDirty(&mediumData_pst->mediumBufData_ast[i_u8]);
                }
                else
                {
                    /* in case the requested range is somewhere in the middle of the buffered data,
                     * adapt the higher limit to the start of the address range */
                    mediumData_pst->mediumBufData_ast[i_u8].limitHi_u32 = adr_u32;
                }
            }
        }
    }

    return;
}

/**
 * \brief   Check if a block located at flash address <adr_u32> and length <length_u32> is fully loaded into Medium buffer.
 *
 * \param   mediumBufData_pst  Pointer to the buffer working data to check the buffer hit
 * \param   adr_u32            Start address of the block in the flash
 * \param   length_u32         Length of the block
*/
static boolean Fee_Prv_MediumIsBfrHit(Fee_Prv_MediumBufData_tst const * mediumBufData_pst, uint32 adr_u32, uint32 length_u32)
{
    boolean result_b;

    if((mediumBufData_pst->limitLo_u32 <= (adr_u32 + 0u         )) &&
       (mediumBufData_pst->limitHi_u32 >= (adr_u32 + length_u32 )))
    {
        result_b = TRUE;
    }
    else
    {
        result_b = FALSE;
    }

    return(result_b);
}

/**
 * \brief   Load some data from the flash to the RAM in the medium buffer.
 *          This is useful for buffered access to the flash.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table entry of the instance for which the job has to be performed
 * \param   bufIdx_u8               Index of the medium buffer to be loaded
 * \param   adr_u32                 Address in the flash to load from
 * \param   nrPageShift_u8          Data shift to consider (in no. of pages)
 * \param   length_u32              Amount of bytes to load
 * \param   maxPrefetchLen_u32      Maximum amount of data to load including prefetch
 * \param   loadMode_en             Load direction
 *
 * \retval  MEMIF_JOB_PENDING       Loading still going on
 * \retval  MEMIF_JOB_OK            Loading finished successfully
 * \retval  MEMIF_JOB_FAILED        Loading finished with an error (Ex. address was not in limit)
*/
static MemIf_JobResultType Fee_Prv_MediumLoad(Fee_Prv_ConfigDeviceTable_tst const *  deviceConfigTable_pcst, uint8 bufIdx_u8, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32, uint32 maxPrefetchLen_u32, Fee_Prv_MediumLoadMode_ten loadMode_en)
{
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumLoad_tst*                  mediumLoad_pst;
    Fee_Prv_MediumBufData_tst *              mediumBufData_pst;
    Fee_Prv_MediumData_tst*                  mediumData_pst;
    MemIf_JobResultType                      result_en;
    Fee_Rb_DeviceName_ten                    deviceName_en;
    uint32                                   nrBytes2Load_u32;
    uint32                                   reloadAdr_u32;

    mediumData_pst    = &deviceConfigTable_pcst->feeData_pst->mediumData_st;
    mediumBufData_pst = &mediumData_pst->mediumBufData_ast[bufIdx_u8];

    if(Fee_Prv_MediumIsBfrHit(mediumBufData_pst, adr_u32, length_u32))
    {
        result_en = MEMIF_JOB_OK;
    }
    else
    {
        result_en = MEMIF_JOB_PENDING;
        deviceName_en = deviceConfigTable_pcst->deviceName_en;
        mediumLoad_pst = &mediumData_pst->mediumLoad_st;

        if(FEE_PRV_LOAD_BUSY_E != mediumLoad_pst->state_en)
        {
            feeBuf_pst = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];
            nrBytes2Load_u32 = rba_MemLib_Min(feeBuf_pst->mediumBufSize_u32, maxPrefetchLen_u32);
            mediumBufData_pst->nrBytes2Load_u32 = nrBytes2Load_u32;

            if(length_u32 <= nrBytes2Load_u32)
            {
                if(FEE_PRV_POSTLOAD_E == loadMode_en)
                {
                    reloadAdr_u32 = rba_MemLib_Min(
                            adr_u32,
                            (Fee_Prv_FsIfGetEmulationEndAdr(deviceName_en) + 1u - nrBytes2Load_u32) );
                }
                else
                {
                    reloadAdr_u32 = rba_MemLib_Max(
                            adr_u32 - (nrBytes2Load_u32 - length_u32),
                            Fee_Prv_FsIfGetEmulationStartAdr(deviceName_en) );
                }
                mediumBufData_pst->reloadAdr_u32 = reloadAdr_u32;

                /* In case of maxPrefetchLen_u32 and length_u32 is 0, avoid data read and return MEMIF_JOB_OK */
                if(0u < nrBytes2Load_u32)
                {
                    /* Start buffer load */
                    Fee_Prv_MediumSetBufDirtyAndRead(
                            deviceName_en,
                            mediumBufData_pst,
                            reloadAdr_u32,
                            nrPageShift_u8,
                            ((void *)feeBuf_pst->mediumBuf_pu8),
                            nrBytes2Load_u32);

                    mediumLoad_pst->state_en = FEE_PRV_LOAD_BUSY_E;
                }
                else
                {
                    result_en = MEMIF_JOB_OK;
                }
            }
            else
            {
                result_en = MEMIF_JOB_FAILED;
            }
        }

        if(MEMIF_JOB_PENDING == result_en)
        {
            result_en = Fee_Prv_MediumReadDo(deviceName_en);
        }

        if(MEMIF_JOB_PENDING != result_en)
        {
            mediumLoad_pst->state_en = FEE_PRV_LOAD_IDLE_E;

            if(MEMIF_JOB_OK == result_en)
            {
                mediumBufData_pst->limitLo_u32 = mediumBufData_pst->reloadAdr_u32;
                mediumBufData_pst->limitHi_u32 = mediumBufData_pst->reloadAdr_u32 + mediumBufData_pst->nrBytes2Load_u32;
            }
        }
    }

    return(result_en);
}

/**
 * \brief   Start a programming operation on the flash.
 *          Contains the sole call of Fls_Write in the whole Fee!
 *
 * \param   mediumData_pst     Pointer to the medium data for which the data has to be programmed
 * \param   adr_u32            Address to write to
 * \param   nrPageShift_u8     Data shift to consider (in no. of pages)
 * \param   bfr_pcv            Buffer to write from
 * \param   length_u32         Amount of bytes to write
 * \param   isTraErrSuppReqd_b Suppress (TRUE) / Allow (FALSE) transient error reporting on Fls level (overprogramming error / robust prog check)
*/
static void Fee_Prv_MediumProgram(Fee_Prv_MediumData_tst * mediumData_pst, uint32 adr_u32, uint8 nrPageShift_u8, void const * bfr_pcv, uint32 length_u32, boolean isTraErrSuppReqd_b)
{
    Fee_Prv_Medium_tst * medium_pst;

    medium_pst = &mediumData_pst->medium_st;

    Fee_Prv_MediumCalcShiftedAdr(medium_pst->deviceName_en, adr_u32, nrPageShift_u8, &medium_pst->flsAdr_u32, &medium_pst->adrOffs_u16);

    /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
    medium_pst->dataBuf_un.dataCon_pcu8 = bfr_pcv;
    medium_pst->nrBytToProc_u32         = length_u32;
    medium_pst->isTraErrSuppReqd_b      = isTraErrSuppReqd_b;
    medium_pst->idFlsJobType_en         = FEE_PRV_MEDIUM_FLS_WRITE_E;

    (void)Fee_Prv_MediumFls(medium_pst, TRUE);

    if(E_OK == medium_pst->lastFlsResult_en)
    {
        /* if data is written to flash, it has to be reloaded to the buffer, so delete the buffered data in case of address collision */
        Fee_Prv_MediumUpdBufsVldy(mediumData_pst, adr_u32, nrPageShift_u8, length_u32, FEE_PRV_MEDIUM_BUF_CUT_E);
    }

    /* Inform persistent data unit about additional stress on medium */
    Fee_Prv_PersStressMeasmtUpd(medium_pst->deviceName_en, mediumData_pst->medium_st.nrBytToProc_u32);

    return;
}

/**
 * \brief   Do the work for a program job.
 *
 * \param   mediumData_pst     Pointer to the medium data for which the data has to be programmed
*/
static MemIf_JobResultType Fee_Prv_MediumProgramDo(Fee_Prv_MediumData_tst * mediumData_pst)
{
    MemIf_JobResultType retVal_en;

    retVal_en = Fee_Prv_MediumFlsDo(&mediumData_pst->medium_st);
    if(MEMIF_JOB_FAILED == retVal_en)
    {
        /* in any error case, delete all buffered data */
        Fee_Prv_MediumUpdBufsVldy(mediumData_pst, 0x0u, 0u, FEE_PRV_MAX_UINT32, FEE_PRV_MEDIUM_BUF_CLR_E);
    }

    return(retVal_en);
}

/**
 * \brief   Set synchronous (polling) or asynchronous flash access.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table entry of the instance for which the sync status has to be set
 * \param   newSync_b               TRUE = synchronous, FALSE = asynchronous
 *
 * \attention   If the flash access is set to synchronous the Fee_MainFunction can take very long!
 *              Calling this function must consider the scheduling environment.
 *              Typically synchronous flash access is only acceptable in init.
*/
void Fee_Prv_MediumSetSync(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, boolean newSync_b)
{
    deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st.sync_b = newSync_b;

    return;
}


/**
 * \brief   Check if data to be read are already cached inside the Medium buffer otherwise start a read operation from the flash.
 *          Contains the sole call of Fls_Read in the whole Fee!
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   adr_u32          Address to read from
 * \param   nrPageShift_u8   Data shift to consider (in no. of pages)
 * \param   bfr_pv           Buffer to read to
 * \param   length_u32       Amount of bytes to read
 *
 * \attention   The general usage of a data shift (nrPageShift_u8 > 0) is only permitted in case length_u32 never
 *              exceeds Fee_Prv_FsIfGetLogicalRowSize). In other words, this feature shall not be used unless a
 *              complete row fits into the medium buffer.
*/
void Fee_Prv_MediumRead(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, void * bfr_pv, uint32 length_u32)
{
    Fee_Prv_ConfigDeviceTable_tst    const * deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_Medium_tst                     * medium_pst;
    Fee_Prv_MediumRead_tst                 * mediumRead_pst;
    Fee_Prv_MediumBufData_tst              * mediumBufData_pst;
    uint32                                   index_u32;
    const uint8                              bufIdx_u8 = 0; // always use the first buffer

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    medium_pst             = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;
    mediumRead_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumRead_st;
    mediumBufData_pst      = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumBufData_ast[bufIdx_u8];
    feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];

    if(Fee_Prv_MediumIsBfrHit(mediumBufData_pst, adr_u32, length_u32))
    {
        /* Data are cached */
        index_u32 = adr_u32 - mediumBufData_pst->limitLo_u32;

        mediumRead_pst->adr_pcu8        = &feeBuf_pst->mediumBuf_pu8[index_u32];
        /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
        mediumRead_pst->bfr_pu8         = bfr_pv;
        mediumRead_pst->length_u32      = length_u32;
        mediumRead_pst->nrBytesDone_u32 = 0u;
        mediumRead_pst->isBufferHit_b   = TRUE;
    }
    else
    {
        Fee_Prv_MediumCalcShiftedAdr(medium_pst->deviceName_en, adr_u32, nrPageShift_u8, &medium_pst->flsAdr_u32, &medium_pst->adrOffs_u16);

        /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
        medium_pst->dataBuf_un.dataChg_pu8 = bfr_pv;
        medium_pst->nrBytToProc_u32        = length_u32;
        medium_pst->idFlsJobType_en        = FEE_PRV_MEDIUM_FLS_READ_E;

        (void)Fee_Prv_MediumFls(medium_pst, TRUE);

        mediumRead_pst->isBufferHit_b      = FALSE;
    }

    return;
}

/**
 * \brief   Start a write operation to the flash with the given sequence.
 *          Contains the sole call of Fls_Write in the whole Fee!
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   adr_u32          Address to write to
 * \param   nrPageShift_u8   Data shift to consider (in no. of pages)
 * \param   bfr_pcv          Buffer to write from
 * \param   length_u32       Amount of bytes to write
 * \param   sequence_en      Selected write sequence
 *
 * \attention   The general usage of a data shift (nrPageShift_u8 > 0) is only permitted in case length_u32 never
 *              exceeds Fee_Prv_FsIfGetLogicalRowSize). In other words, this feature shall not be used unless a
 *              complete row fits into the medium buffer.
*/
void Fee_Prv_MediumWrite(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, void const * bfr_pcv, uint32 length_u32, Fee_Prv_MediumSeqWrite_ten sequence_en)
{
    Fee_Prv_ConfigDeviceTable_tst    const * deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumWrite_tst *                mediumWrite_pst;
    Fee_Prv_MediumWrite_ten                  stMediumWrite_en;
    const uint8                              bufIdx_u8 = 0;  // always use the first buffer

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumWrite_pst        = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumWrite_st;

    /* Check selected write sequence */
    switch(sequence_en)
    {
        /* Note: The first two cases, which rely on the blank check in Fls, could be enclosed by the compiler switch
         * (STD_OFF != FLS_BLANK_CHECK_API) to make the job fail in the first call of Fee_Prv_MediumWriteDo if the
         * functionality is not supported.
         * However, we want to reduce the use of compiler switches as much as possible. Here, this is possible, since
         * the write job will anyway fail in the first call of Fee_Prv_MediumBlankCheckDo (= Fee_Prv_MediumFlsDo) after
         * Fee_Prv_MediumBlankCheck in case the blank check functionality is not supported by Fls. */

        /* Selected sequence is blank check -> copy -> program -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COPY_COMP_E:
        {
            /* Start state machine in blank check state without medium buffer bypass */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_BLANKCHECK_E;
            mediumWrite_pst->isMedBfrByp_b = FALSE;
            mediumWrite_pst->isTraErrSuppReqd_b = FALSE;
            mediumWrite_pst->isCmpSkip_b = FALSE;
            mediumWrite_pst->isWlBckpReqd_b = FALSE;
        }
        break;

        /* Selected sequence is blank check -> wl backup -> copy -> program -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_WLBCKP_COPY_COMP_E:
        {
            /* Start state machine in blank check state without medium buffer bypass */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_BLANKCHECK_E;
            mediumWrite_pst->isMedBfrByp_b = FALSE;
            mediumWrite_pst->isTraErrSuppReqd_b = FALSE;
            mediumWrite_pst->isCmpSkip_b = FALSE;
            mediumWrite_pst->isWlBckpReqd_b = Fee_Prv_MediumGetWlErrHndlgEnad(deviceName_en);
        }
        break;

        /* Selected sequence is blank check -> program -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COMP_E:
        {
            /* Start state machine in blank check state with medium buffer bypass */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_BLANKCHECK_E;
            mediumWrite_pst->isMedBfrByp_b = TRUE;
            mediumWrite_pst->isTraErrSuppReqd_b = FALSE;
            mediumWrite_pst->isCmpSkip_b = FALSE;
            mediumWrite_pst->isWlBckpReqd_b = FALSE;
        }
        break;

        /* Selected sequence is blank check -> wl backup -> program -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_WLBCKP_COMP_E:
        {
            /* Start state machine in blank check state with medium buffer bypass */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_BLANKCHECK_E;
            mediumWrite_pst->isMedBfrByp_b = TRUE;
            mediumWrite_pst->isTraErrSuppReqd_b = FALSE;
            mediumWrite_pst->isCmpSkip_b = FALSE;
            mediumWrite_pst->isWlBckpReqd_b = Fee_Prv_MediumGetWlErrHndlgEnad(deviceName_en);
        }
        break;

        /* Selected sequence is copy -> program -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_WRITE_COPY_COMP_E:
        {
            /* Start state machine in copy state without medium buffer bypass */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_COPYDATA_E;
            mediumWrite_pst->isMedBfrByp_b = FALSE;
            mediumWrite_pst->isTraErrSuppReqd_b = TRUE;
            mediumWrite_pst->isCmpSkip_b = FALSE;
            mediumWrite_pst->isWlBckpReqd_b = FALSE;
        }
        break;

        /* Selected sequence is program -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_WRITE_COMP_E:
        {
            /* Start state machine in copy state with medium buffer bypass */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_COPYDATA_E;
            mediumWrite_pst->isMedBfrByp_b = TRUE;
            mediumWrite_pst->isTraErrSuppReqd_b = TRUE;
            mediumWrite_pst->isCmpSkip_b = FALSE;
            mediumWrite_pst->isWlBckpReqd_b = FALSE;
        }
        break;

        /* Selected sequence is program */
        case FEE_PRV_MEDIUM_SEQ_WRITE_ONLY_E:
        {
            /* Start state machine in write state with medium buffer bypass */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_COPYDATA_E;
            mediumWrite_pst->isMedBfrByp_b = TRUE;
            mediumWrite_pst->isTraErrSuppReqd_b = TRUE;
            mediumWrite_pst->isCmpSkip_b = TRUE;
            mediumWrite_pst->isWlBckpReqd_b = FALSE;
        }
        break;

        default:
        {
            /* Sequence not (yet) supported; keep state machine idle to cause MEMIF_JOB_FAILED */
            stMediumWrite_en = FEE_PRV_MEDIUM_WRITE_IDLE_E;
        }
        break;
    }

    /* Selected sequence is supported? */
    if(FEE_PRV_MEDIUM_WRITE_IDLE_E != stMediumWrite_en)
    {
        /* Memorize details of request */
        mediumWrite_pst->deviceName_en  = deviceName_en;
        mediumWrite_pst->adrDest_u32    = adr_u32;
        mediumWrite_pst->nrPageShift_u8 = nrPageShift_u8;
        mediumWrite_pst->bfr_pcv        = bfr_pcv;
        mediumWrite_pst->length_u32     = length_u32;
        mediumWrite_pst->done_u32       = 0u;
        mediumWrite_pst->hasWlError_b   = FALSE;

        /* Medium buffer shall be by used (i.e. bypass inactive)? */
        if(!mediumWrite_pst->isMedBfrByp_b)
        {
            feeBuf_pst = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];
            mediumWrite_pst->data2Program_pcv = feeBuf_pst->mediumBuf_pu8;
        }
        else
        {
            /* mediumWrite_pst->data2Program_pcv is set in state Fee_Prv_MediumWrite_CopyData later */
        }

# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
        /* Is wordline backup requested? */
        /* If yes wordline errors can noit occur at beginning of a wordline */
        /* Skip backup if write adress is same as wordline start adress */
        if( mediumWrite_pst->isWlBckpReqd_b &&
            (0u == (adr_u32 - rba_MemLib_FloorPwr2(adr_u32, Fee_Prv_MediumGetWlSize(deviceName_en)))))
        {
            mediumWrite_pst->isWlBckpReqd_b = FALSE;
        }
# endif
    }

    /* Reset/restart the state machine */
    FEE_PRV_LIBTRANS(*mediumWrite_pst, stMediumWrite_en);

    return;
}

/**
 * \brief   Erase a part of the flash.
 *          Contains the sole call of Fls_Erase in the whole Fee!
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   adr_u32          Address where the erase shall begin
 * \param   length_u32       Amount of bytes to erase
*/
void Fee_Prv_MediumErase(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint32 length_u32)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumData_tst *              mediumData_pst;
    Fee_Prv_Medium_tst *                  medium_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;
    medium_pst             = &mediumData_pst->medium_st;

    medium_pst->flsAdr_u32      = adr_u32;
    medium_pst->adrOffs_u16     = 0u;     /* We can only delete whole sectors, i.e. an address offset is meaningless */
    medium_pst->nrBytToProc_u32 = length_u32;
    medium_pst->idFlsJobType_en = FEE_PRV_MEDIUM_FLS_ERASE_E;

    (void)Fee_Prv_MediumFls(medium_pst, TRUE);

    /* clear buffers in case they contain data of the sector to be erased */
    Fee_Prv_MediumUpdBufsVldy(mediumData_pst, adr_u32, 0u, length_u32, FEE_PRV_MEDIUM_BUF_CLR_E);

    return;
}

/**
 * \brief   Compare the content of a piece of flash with the content of a piece of RAM
 *          Contains the sole call of Fls_Compare in the whole Fee!
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   flsAdr_u32       Address in flash where the compare shall start
 * \param   nrPageShift_u8   Data shift to consider (in no. of pages)
 * \param   bfr_pcv          Address in the RAM where the compare shall start
 * \param   length_u32       Amount of bytes to compare
 *
 * \attention   The general usage of a data shift (nrPageShift_u8 > 0) is only permitted in case length_u32 never
 *              exceeds Fee_Prv_FsIfGetLogicalRowSize). In other words, this feature shall not be used unless a
 *              complete row fits into the medium buffer.
*/
void Fee_Prv_MediumCompareFls2Buf(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrFls_u32, uint8 nrPageShift_u8, void const * bfr_pcv, uint32 length_u32)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_Medium_tst *                  medium_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    medium_pst             = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;

    Fee_Prv_MediumCalcShiftedAdr(medium_pst->deviceName_en, adrFls_u32, nrPageShift_u8, &medium_pst->flsAdr_u32, &medium_pst->adrOffs_u16);

    /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
    medium_pst->dataBuf_un.dataCon_pcu8 = bfr_pcv;
    medium_pst->nrBytToProc_u32         = length_u32;
    medium_pst->idFlsJobType_en         = FEE_PRV_MEDIUM_FLS_COMPARE_E;

    (void)Fee_Prv_MediumFls(medium_pst, TRUE);

    return;
}

/**
 * \brief   Start a compare operation of 2 flash locations.
 *
 * \param   deviceName_en     Device (or Instance) for which the job has to be performed
 * \param   flsAdr1_u32       Address of 1st flash location
 * \param   nrPageShift1_u8   Data shift to consider at 1st flash location (in no. or pages)
 * \param   flsAdr2_u32       Address of 2nd flash location
 * \param   nrPageShift2_u8   Data shift to consider at 2nd flash location (in no. of pages)
 * \param   length_u32        Bytes to compare in total
 *
 * \attention   The general usage of a data shift (nrPageShift1_u8 > 0 or nrPageShift1_u8 > 0) is only permitted in
 *              case length_u32 never exceeds Fee_Prv_FsIfGetLogicalRowSize). In other words, this feature shall not be
 *              used unless a complete row fits into the medium buffer.
*/
void Fee_Prv_MediumCompareFls2Fls(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrFls1_u32, uint8 nrPageShift1_u8, uint32 adrFls2_u32, uint8 nrPageShift2_u8, uint32 length_u32)
{
    Fee_Prv_ConfigDeviceTable_tst const *  deviceConfigTable_pcst;
    Fee_Prv_MediumCompareFls2Fls_tst *     mediumCompareFls2Fls_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst    = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumCompareFls2Fls_pst  = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumCompareFls2Fls_st;

    mediumCompareFls2Fls_pst->deviceName_en   = deviceName_en;
    mediumCompareFls2Fls_pst->adrFls1_u32     = adrFls1_u32;
    mediumCompareFls2Fls_pst->nrPageShift1_u8 = nrPageShift1_u8;
    mediumCompareFls2Fls_pst->adrFls2_u32     = adrFls2_u32;
    mediumCompareFls2Fls_pst->nrPageShift2_u8 = nrPageShift2_u8;
    mediumCompareFls2Fls_pst->length_u32      = length_u32;
    mediumCompareFls2Fls_pst->nrBytesDone_u32 = 0u;

    FEE_PRV_LIBTRANS(*mediumCompareFls2Fls_pst, FEE_PRV_MEDIUM_COMPAREFLS2FLS_READ_E);

    return;
}

/**
 * \brief   Check whether a piece of flash is erased or programmed.
 *          Contains the sole call of Fls_BlankCheck in the whole Fee!
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   adr_u32          Address in flash where the blank check shall start
 * \param   nrPageShift_u8   Data shift to consider (in no. of pages)
 * \param   length_u32       Amount of bytes which shall be checked
 *
 * \attention   The general usage of a data shift (nrPageShift_u8 > 0) is only permitted in case length_u32 never
 *              exceeds Fee_Prv_FsIfGetLogicalRowSize). In other words, this feature shall not be used unless a
 *              complete row fits into the medium buffer.
*/
void Fee_Prv_MediumBlankCheck(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_Medium_tst *                  medium_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    medium_pst             = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;

    Fee_Prv_MediumCalcShiftedAdr(medium_pst->deviceName_en, adr_u32, nrPageShift_u8, &medium_pst->flsAdr_u32, &medium_pst->adrOffs_u16);

    medium_pst->nrBytToProc_u32 = length_u32;
    medium_pst->idFlsJobType_en = FEE_PRV_MEDIUM_FLS_BLANKCHECK_E;

    (void)Fee_Prv_MediumFls(medium_pst, TRUE);

    return;
}

/**
 * \brief   Destroy already programmed data by overprogramming with the given value.
 *
 * \param   deviceName_en      Device (or Instance) for which the job has to be performed
 * \param   adr_u32            Address of the data which shall be shredded
 * \param   nrPageShift_u8     Data shift to consider (in no. of pages)
 * \param   value_u8           Shredder value (= value used for overprogramming, typically the inverse erase pattern)
 * \param   length_u32         Amount of bytes to shredder (0 is permitted, must fit into the medium buffer, and must be page aligned)
 * \param   isTraErrSuppReqd_b Suppress (TRUE) / Allow (FALSE) transient error reporting on Fls level (overprogramming error / robust prog check)
*/
void Fee_Prv_MediumShredder(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint8 value_u8, uint32 length_u32, boolean isTraErrSuppReqd_b)
{
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumData_tst *                 mediumData_pst;
    const uint8                              bufIdx_u8 = 0;   // always use the first buffer

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];
    mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;

    if(length_u32 <= feeBuf_pst->mediumBufSize_u32)
    {
        /* delete buffer content, in case the buffer is used for intermediate storage */
        Fee_Prv_MediumSetBufDirty(&mediumData_pst->mediumBufData_ast[bufIdx_u8]);
        rba_MemLib_MemSet(feeBuf_pst->mediumBuf_pu8, value_u8, length_u32);

        Fee_Prv_MediumProgram(mediumData_pst, adr_u32, nrPageShift_u8, feeBuf_pst->mediumBuf_pu8, length_u32, isTraErrSuppReqd_b);
    }
    else
    {
        mediumData_pst->medium_st.lastFlsResult_en = E_NOT_OK;
    }

    return;
}

/**
 * \brief   Destroy already programmed data by overprogramming with the given repetitive pattern.
 *
 * \param   deviceName_en      Device (or Instance) for which the job has to be performed
 * \param   adr_u32            Address of the data which shall be shredded
 * \param   nrPageShift_u8     Data shift to consider (in no. of pages)
 * \param   patOffs_u32        Offset from where to start in first shredder pattern
 * \param   patSize_u32        Shredder pattern size (in number of bytes, must be greater than patOffs_u32)
 * \param   patBfr_pcv         Pointer to shredder pattern
 * \param   length_u32         Amount of bytes to shredder in total (0 is permitted, must fit into the medium buffer, and must be page aligned)
 * \param   isTraErrSuppReqd_b Suppress (TRUE) / Allow (FALSE) transient error reporting on Fls level (overprogramming error / robust prog check)
 *
 * \attention   Refrain from calling this function with a pattern size of one (even though this will work)!
 *              Use Fee_Prv_MediumShredder instead, since the latter is much more efficient for such use-case.
*/
void Fee_Prv_MediumPatShredder(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint32 patOffs_u32, uint32 patSize_u32, void const * patBfr_pcv, uint32 length_u32, boolean isTraErrSuppReqd_b)
{
    uint32                                   nrBytesDone_u32;
    uint32                                   nrBytesCurrentChunk_u32;
    uint32                                   nrBytesCurrentOffs_u32;
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumData_tst *                 mediumData_pst;
    Fee_Prv_Medium_tst *                     medium_pst;
    const uint8                              bufIdx_u8 = 0;    // always use the first buffer

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];
    mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;
    medium_pst             = &mediumData_pst->medium_st;

    if(length_u32 <= feeBuf_pst->mediumBufSize_u32)
    {
        nrBytesDone_u32         = 0u;
        nrBytesCurrentOffs_u32  = patOffs_u32;
        nrBytesCurrentChunk_u32 = rba_MemLib_Min(patSize_u32 - patOffs_u32, length_u32);

        while(nrBytesCurrentChunk_u32 > 0u)
        {
            /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
            Fee_Prv_MediumSetBufDirtyAndCopy(
                    &mediumData_pst->mediumBufData_ast[bufIdx_u8],
                    (uint8 const *)patBfr_pcv + nrBytesCurrentOffs_u32,
                    feeBuf_pst->mediumBuf_pu8 + nrBytesDone_u32,
                    nrBytesCurrentChunk_u32
            );

            nrBytesDone_u32        += nrBytesCurrentChunk_u32;
            nrBytesCurrentOffs_u32  = 0u;
            nrBytesCurrentChunk_u32 = rba_MemLib_Min(patSize_u32, length_u32 - nrBytesDone_u32);
        };

        Fee_Prv_MediumProgram(mediumData_pst, adr_u32, nrPageShift_u8, feeBuf_pst->mediumBuf_pu8, length_u32, isTraErrSuppReqd_b);
    }
    else
    {
        medium_pst->lastFlsResult_en = E_NOT_OK;
    }

    return;
}

/**
 * \brief   Calculate the CRC directly from the flash.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   adr_u32          Address where the calculation shall start
 *  param   nrPageShift_u8   Data shift to consider (in no. of pages)
 * \param   length_u32       Amount of bytes to consider
 * \param   crc_pu32         Point to an uint32 where the result is placed
 * \param   firstCall_b      Is this the first call for a chain of CRC calculations or not
 *
 * \attention   The general usage of a data shift (nrPageShift_u8 > 0) is only permitted in case length_u32 never
 *              exceeds Fee_Prv_FsIfGetLogicalRowSize). In other words, this feature shall not be used unless a
 *              complete row fits into the medium buffer.
*/
void Fee_Prv_MediumCalcCrc(Fee_Rb_DeviceName_ten deviceName_en, uint32 adr_u32, uint8 nrPageShift_u8, uint32 length_u32, uint32 * crc_pu32, boolean firstCall_b)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumCalcCrc_tst *           mediumCalcCrc_pst;
    const uint8                           bufIdx_u8 = 0;    // always use the first buffer

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumCalcCrc_pst      = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumCalcCrc_st;

    mediumCalcCrc_pst->nrBytesDone_u32    = 0u;
    mediumCalcCrc_pst->length_u32         = length_u32;
    mediumCalcCrc_pst->crc_pu32           = crc_pu32;
    mediumCalcCrc_pst->firstCall_b        = firstCall_b;

    Fee_Prv_MediumSelBufSetIterator(deviceName_en, bufIdx_u8, adr_u32, nrPageShift_u8, length_u32);

    return;
}

/**
 * \brief   Start a copy operation from one flash location to another (with the given sequence, using internal buffer).
 *
 * \param   deviceName_en        Device (or Instance) for which the job has to be performed
 * \param   adrSrc_u32           Source address in flash
 * \param   nrPageShiftSrc_u8    Source data shift to consider (in no. of pages)
 * \param   adrDest_u32          Destination address in flash
 * \param   nrPageShiftDest_u8   Destination data shift to consider (in no. of pages)
 * \param   length_u32           Bytes to copy in total
 * \param   sequence_en          Selected copy sequence
 *
 * \attention   The general usage of a data shift (nrPageShiftSrc_u8 > 0 or nrPageShiftDest_u8 > 0) is only permitted
 *              in case length_u32 never exceeds Fee_Prv_FsIfGetLogicalRowSize). In other words, this feature shall not
 *              be used unless a complete row fits into the medium buffer.
*/
void Fee_Prv_MediumCopy(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrSrc_u32, uint8 nrPageShiftSrc_u8, uint32 adrDest_u32, uint8 nrPageShiftDest_u8, uint32 length_u32, Fee_Prv_MediumSeqCopy_ten sequence_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumCopy_tst *              mediumCopy_pst;
    Fee_Prv_MediumCopy_ten                stMediumCopy_en;
    Fee_Prv_MediumSeqWrite_ten            seqMediumWrite_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumCopy_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumCopy_st;

    /* Check selected write sequence */
    switch(sequence_en)
    {
        /* Note: The first case, which relies on the blank check in Fls, could be enclosed by the compiler switch
         * (STD_OFF != FLS_BLANK_CHECK_API) to make the job fail in the first call of Fee_Prv_MediumCopyDo if the
         * functionality is not supported.
         * However, we want to reduce the use of compiler switches as much as possible. Here, this is possible, since
         * the copy job will anyway fail in the first call of Fee_Prv_MediumBlankCheckDo (= Fee_Prv_MediumFlsDo) after
         * Fee_Prv_MediumBlankCheck in case the blank check functionality is not supported by Fls. */

        /* Selected sequence is blank check -> copy -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_COPY_BLNKCHK_COMP_E:
        {
            /* Start state machine in read state; the following writing shall be preceded by a blank check */
            stMediumCopy_en   = FEE_PRV_MEDIUM_COPY_READ_E;
            seqMediumWrite_en = FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COMP_E;
        }
        break;

        /* Selected sequence is blank check -> wl backup -> copy -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_COPY_BLNKCHK_WLBCKP_COMP_E:
        {
            /* Start state machine in read state; the following writing shall be preceded by a blank check */
            stMediumCopy_en   = FEE_PRV_MEDIUM_COPY_READ_E;
            seqMediumWrite_en = FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_WLBCKP_COMP_E;
        }
        break;

        /* Selected sequence is copy -> compare ? */
        case FEE_PRV_MEDIUM_SEQ_COPY_COMP_E:
        {
            /* Start state machine in read state; the following writing shall not be preceded by a blank check */
            stMediumCopy_en   = FEE_PRV_MEDIUM_COPY_READ_E;
            seqMediumWrite_en = FEE_PRV_MEDIUM_SEQ_WRITE_COMP_E;
        }
        break;

        default:
        {
            /* Sequence not (yet) supported; keep state machine idle to cause MEMIF_JOB_FAILED */
            stMediumCopy_en   = FEE_PRV_MEDIUM_COPY_IDLE_E;
            seqMediumWrite_en = FEE_PRV_MEDIUM_SEQ_WRITE_ONLY_E;
        }
        break;
    }

    /* Selected sequence is supported? */
    if(FEE_PRV_MEDIUM_COPY_IDLE_E != stMediumCopy_en)
    {
        mediumCopy_pst->deviceName_en      = deviceName_en;
        mediumCopy_pst->adrSrc_u32         = adrSrc_u32;
        mediumCopy_pst->nrPageShiftSrc_u8  = nrPageShiftSrc_u8;
        mediumCopy_pst->adrDest_u32        = adrDest_u32;
        mediumCopy_pst->nrPageShiftDest_u8 = nrPageShiftDest_u8;
        mediumCopy_pst->length_u32         = length_u32;
        mediumCopy_pst->done_u32           = 0u;
        mediumCopy_pst->writeSeq_en        = seqMediumWrite_en;
    }

    /* Reset/restart the state machine */
    FEE_PRV_LIBTRANS(*mediumCopy_pst, stMediumCopy_en);

    return;
}

/**
 * \brief   Do the work for a read job.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
*/
MemIf_JobResultType Fee_Prv_MediumReadDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_Medium_tst *                  medium_pst;
    Fee_Prv_MediumRead_tst *              mediumRead_pst;
    MemIf_JobResultType                   result_en;
    uint32                                nrBytesAllowedToProcess_u32;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

    mediumRead_pst = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumRead_st;

    if(mediumRead_pst->isBufferHit_b)
    {
        /* Adapt data size to copy to the available Fee time budget.                                               */
        /* In case of a large data block (max. 64kB) which fits into the Medium buffer, the copy process has to be */
        /* distributed over multiple Fee main calls.                                                               */
        nrBytesAllowedToProcess_u32 = Fee_Prv_LibEffortGet(deviceName_en, FEE_PRV_LIMIT_CRCINRAM_CPYRAM_E,
                                                           mediumRead_pst->length_u32 - mediumRead_pst->nrBytesDone_u32);

        rba_MemLib_MemCopy(mediumRead_pst->adr_pcu8 + mediumRead_pst->nrBytesDone_u32,
                           mediumRead_pst->bfr_pu8  + mediumRead_pst->nrBytesDone_u32,
                           nrBytesAllowedToProcess_u32);

        mediumRead_pst->nrBytesDone_u32 += nrBytesAllowedToProcess_u32;

        if(mediumRead_pst->length_u32 <= mediumRead_pst->nrBytesDone_u32)
        {
            result_en = MEMIF_JOB_OK;
        }
        else
        {
            result_en = MEMIF_JOB_PENDING;
        }
    }
    else
    {
        medium_pst = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;
        result_en = Fee_Prv_MediumFlsDo(medium_pst);
    }

    return(result_en);
}

/**
 * \brief   Before programming assure that the space which shall be programmed is blank
 *
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_BlankCheck(Fee_Prv_MediumWrite_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* There is no need to round up the blank check length to be physical page aligned, as Fls driver will
         * internally check for the required number of physical page itself. This is a standard AUTOSAR feature of Fls */
        Fee_Prv_MediumBlankCheck(fsm_pst->deviceName_en, fsm_pst->adrDest_u32, fsm_pst->nrPageShift_u8, fsm_pst->length_u32);
    }

    result_en = Fee_Prv_MediumBlankCheckDo(fsm_pst->deviceName_en);

    if(MEMIF_JOB_OK == result_en)
    {
# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
        if(fsm_pst->isWlBckpReqd_b)
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_BACKUPWL_E);
        }
        else
# endif
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_COPYDATA_E);
        }
    }

    return(result_en);
}

# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
/**
 * \brief   Before programming backup the wordline of current adress which shall be programmed
 *
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_BackupWl(Fee_Prv_MediumWrite_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst const *   deviceConfigTable_pcst;
    uint32                                  wlSize_u32        = Fee_Prv_MediumGetWlSize(fsm_pst->deviceName_en);
    MemIf_JobResultType                     result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);

    if(FEE_PRV_LIBENTRY)
    {
        // Page shift is required for operations inside logical rows. But here we are working on physical wordlines - therefore we will use fixed value 0
        Fee_Prv_MediumRead(
                fsm_pst->deviceName_en,
                rba_MemLib_FloorPwr2(fsm_pst->adrDest_u32, wlSize_u32),
                0u,
                ((void *)deviceConfigTable_pcst->mediumRescueBfr_pcu8),
                fsm_pst->adrDest_u32 - rba_MemLib_FloorPwr2(fsm_pst->adrDest_u32, wlSize_u32));
    }

    result_en = Fee_Prv_MediumReadDo(fsm_pst->deviceName_en);

    if(MEMIF_JOB_OK == result_en)
    {
        FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_COPYDATA_E);
    }

    return(result_en);
}
# endif

/**
 * \brief   Before programming copy the data to an internal buffer in case the local buffer is not bypassed.
 *          Afterwards start programming using this buffer or switch to padding.
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_CopyData(Fee_Prv_MediumWrite_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumData_tst*                  mediumData_pst;
    Fee_Prv_MediumBufData_tst*               mediumBufData_pst;
    uint8 const *                            startAdr_pcu8;
    uint32                                   bytes2Program_u32;

    const uint8 bufIdx_u8 = 0;    // always use the first buffer

    /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
    startAdr_pcu8 = (uint8 const *)fsm_pst->bfr_pcv;
    startAdr_pcu8 += fsm_pst->done_u32;
    bytes2Program_u32 = fsm_pst->length_u32 - fsm_pst->done_u32;

    if(fsm_pst->isMedBfrByp_b)
    {   /* Internal buffer is bypassed */

        /* Iterate through the original source and program directly from there */
        fsm_pst->data2Program_pcv = (void const *)startAdr_pcu8;
    }
    else
    {   /* Internal buffer is used */

        /* Get the pointer to config data model */
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);
        feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];
        mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;
        mediumBufData_pst      = &mediumData_pst->mediumBufData_ast[bufIdx_u8];

        /* If data size > buffer size, perform chunkwise writes */
        bytes2Program_u32 = rba_MemLib_Min(feeBuf_pst->mediumBufSize_u32, bytes2Program_u32);

        /* Copy original source to the internal buffer */
        Fee_Prv_MediumSetBufDirtyAndCopy(
                mediumBufData_pst,
                startAdr_pcu8,
                feeBuf_pst->mediumBuf_pu8,
                bytes2Program_u32);

        /* Always program from the internal buffer start address
         * This is done once in the job acceptor function, and has not to be done with every iteration
         * fsm_pst->data2Program_pcv = mediumBuf_pu8; */
    }

    /* Store the calculated number of bytes to program */
    fsm_pst->bytes2Program_u32 = bytes2Program_u32;

# if(FALSE == FEE_RB_WRITE_PADDING_REQUIRED)
    FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_PROGRAM_E);
#else
    FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_PADDING_E);
#endif

    return(MEMIF_JOB_OK);
}

/**
 * \brief   Before programming the last logical page perform padding.
 *          Afterwards start programming using the internal buffer.
 *
*/
# if(FALSE != FEE_RB_WRITE_PADDING_REQUIRED)
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_Padding(Fee_Prv_MediumWrite_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    uint8 *                               mediumBuf_pu8;
    Fee_Prv_MediumData_tst*               mediumData_pst;
    Fee_Prv_MediumBufData_tst*            mediumBufData_pst;
    uint8 const *                         startAdr_pcu8;
    uint32                                bytes2Program_u32;
    uint8                                 logPageSize_u8, startPadding_u8, paddingBytes_u8;

    const uint8 bufIdx_u8 = 0;    // Always use the first buffer

    /* Calculate number of padding bytes */
    bytes2Program_u32 = fsm_pst->bytes2Program_u32;
    logPageSize_u8    = Fee_Prv_FsIfGetLogicalPageSize(fsm_pst->deviceName_en);
    startPadding_u8   = (uint8)rba_MemLib_Modulo(bytes2Program_u32, logPageSize_u8);
    paddingBytes_u8   = logPageSize_u8 - startPadding_u8;

    if(startPadding_u8 != 0)
    {   /* Prepare data which is logical page aligned */

        /* Get the pointer to config data model */
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);
        mediumBuf_pu8          = deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8].mediumBuf_pu8;

        if(fsm_pst->isMedBfrByp_b)
        {   /* Internal buffer is bypassed */
            if(bytes2Program_u32 > logPageSize_u8)
            {
                /* Only program until the last complete logical page */
                bytes2Program_u32 -= startPadding_u8;
            }
            else
            {   /* Last page is reached and not aligned to a logical page */
                /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
                startAdr_pcu8 = (uint8 const *)fsm_pst->bfr_pcv;
                startAdr_pcu8 += fsm_pst->done_u32;

                mediumData_pst = &deviceConfigTable_pcst->feeData_pst->mediumData_st;
                mediumBufData_pst = &mediumData_pst->mediumBufData_ast[bufIdx_u8];

                /* Build up the last page in the internal buffer */
                Fee_Prv_MediumSetBufDirtyAndCopy(
                        mediumBufData_pst,
                        startAdr_pcu8,
                        mediumBuf_pu8,
                        bytes2Program_u32);

                /* To achieve page alignment fill up the internal buffer with the inverse erase pattern */
                rba_MemLib_MemSet(&mediumBuf_pu8[startPadding_u8], ~Fee_Prv_FsIfGetEraseValue(fsm_pst->deviceName_en), paddingBytes_u8);

                /* Write complete page */
                bytes2Program_u32 = logPageSize_u8;
                /* Set the source address to the internal buffer */
                fsm_pst->data2Program_pcv = mediumBuf_pu8;
            }
        }
        else
        {   /* Internal buffer is used */

            /* To achieve page alignment fill up the internal buffer with the inverse erase pattern */
            rba_MemLib_MemSet(&mediumBuf_pu8[bytes2Program_u32], ~Fee_Prv_FsIfGetEraseValue(fsm_pst->deviceName_en), paddingBytes_u8);

            /* Round data to complete logical page */
            bytes2Program_u32 += paddingBytes_u8;
        }

        fsm_pst->bytes2Program_u32 = bytes2Program_u32;
    }

    FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_PROGRAM_E);

    return(MEMIF_JOB_OK);
}
# endif

/**
 * \brief   program.
 *
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_Program(Fee_Prv_MediumWrite_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumData_tst *              mediumData_pst;
    MemIf_JobResultType                   result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);
    mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;

    if(FEE_PRV_LIBENTRY)
    {
        Fee_Prv_MediumProgram(mediumData_pst, fsm_pst->adrDest_u32 + fsm_pst->done_u32, fsm_pst->nrPageShift_u8, fsm_pst->data2Program_pcv, fsm_pst->bytes2Program_u32, fsm_pst->isTraErrSuppReqd_b);
    }

    result_en = Fee_Prv_MediumProgramDo(mediumData_pst);

    if(MEMIF_JOB_PENDING != result_en)
    {
        if(MEMIF_JOB_OK == result_en)
        {
            if(fsm_pst->isCmpSkip_b)
            {
                fsm_pst->done_u32 += fsm_pst->bytes2Program_u32;
                if(fsm_pst->done_u32 < fsm_pst->length_u32)
                {
                    /* If Fls driver is used: No padding is required to align the data
                     * 3rd-party Fls driver will be supported as well,
                     * by going back to the copy state and writing the last page with padding */
                    FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_COPYDATA_E);
                }
                else
                {
                    FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_FINAL_E);
                }
            }
            else
            {
                FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_COMPARE_E);
            }
        }
# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
        else if(MEMIF_JOB_FAILED == result_en)
        {
            if(fsm_pst->isWlBckpReqd_b)
            {
                /* Program action failed - continue to detect a possible wordline error */
                FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_DETNWLERR_E);
                result_en = MEMIF_JOB_OK;
            }
        }
        else
        {
            /* intentionally empty */
        }
# endif
    }

    return(result_en);
}


/**
 * \brief   Compare what should have been programmed with what was actually programmed
 *
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_Compare(Fee_Prv_MediumWrite_tst * fsm_pst)
{
    MemIf_JobResultType result_en;

    if(FEE_PRV_LIBENTRY)
    {
        Fee_Prv_MediumCompareFls2Buf(fsm_pst->deviceName_en,
                                     fsm_pst->adrDest_u32 + fsm_pst->done_u32,
                                     fsm_pst->nrPageShift_u8,
                                     fsm_pst->data2Program_pcv,
                                     fsm_pst->bytes2Program_u32);
    }

    result_en = Fee_Prv_MediumCompareFls2BufDo(fsm_pst->deviceName_en);

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->done_u32 += fsm_pst->bytes2Program_u32;
        if(fsm_pst->done_u32 < fsm_pst->length_u32)
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_COPYDATA_E);
        }
        else
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_FINAL_E);
        }
    }
    else if(MEMIF_BLOCK_INCONSISTENT == result_en)
    {
# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
        if(fsm_pst->isWlBckpReqd_b)
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_DETNWLERR_E);
            result_en = MEMIF_JOB_OK;
        }
        else
# endif
        {
            result_en = MEMIF_JOB_FAILED;
        }

    }
    else
    {
        /* intentionally empty */
    }

    return(result_en);
}

# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
/**
 * \brief   Detect if programming caused a wordline error
 *
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumWrite_DetnWlErr(Fee_Prv_MediumWrite_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    uint32                                wlSize_u32        = Fee_Prv_MediumGetWlSize(fsm_pst->deviceName_en);
    MemIf_JobResultType                   result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);

    if(FEE_PRV_LIBENTRY)
    {
        // Page shift is required for operations inside logical rows. But here we are working on physical wordlines - therefore we will use fixed value 0
        Fee_Prv_MediumCompareFls2Buf(fsm_pst->deviceName_en,
                                     rba_MemLib_FloorPwr2(fsm_pst->adrDest_u32, wlSize_u32),
                                     0u,
                                     deviceConfigTable_pcst->mediumRescueBfr_pcu8,
                                     fsm_pst->adrDest_u32 - rba_MemLib_FloorPwr2(fsm_pst->adrDest_u32, wlSize_u32));
    }

    result_en = Fee_Prv_MediumCompareFls2BufDo(fsm_pst->deviceName_en);

    if(MEMIF_JOB_PENDING != result_en)
    {
        if(MEMIF_JOB_OK == result_en)
        {
            /* No wordline error - data is still present */
            /* Consider as HW failure during write */
            fsm_pst->hasWlError_b = FALSE;
        }
        else
        {
            /* For MEMIF_BLOCK_INCONSISTENT: Wordline error - data is not present */
            /* For MEMIF_BLOCK_FAILED: HW failure - we were not able to read within the wordline - consider this as wordline error */
            fsm_pst->hasWlError_b = TRUE;
        }

        result_en = MEMIF_JOB_FAILED;
    }

    return(result_en);
}
# endif

/**
 * \brief   Do the work for a write job.
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
*/
MemIf_JobResultType Fee_Prv_MediumWriteDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumWrite_tst *             fsm_pst;
    MemIf_JobResultType                   result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    fsm_pst                = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumWrite_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case FEE_PRV_MEDIUM_WRITE_BLANKCHECK_E:             result_en = Fee_Prv_MediumWrite_BlankCheck(fsm_pst);            break;
# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
            case FEE_PRV_MEDIUM_WRITE_BACKUPWL_E:               result_en = Fee_Prv_MediumWrite_BackupWl(fsm_pst);              break;
# endif
            case FEE_PRV_MEDIUM_WRITE_COPYDATA_E:               result_en = Fee_Prv_MediumWrite_CopyData(fsm_pst);              break;
# if(FALSE != FEE_RB_WRITE_PADDING_REQUIRED)
            case FEE_PRV_MEDIUM_WRITE_PADDING_E:                result_en = Fee_Prv_MediumWrite_Padding(fsm_pst);               break;
# endif
            case FEE_PRV_MEDIUM_WRITE_PROGRAM_E:                result_en = Fee_Prv_MediumWrite_Program(fsm_pst);               break;
            case FEE_PRV_MEDIUM_WRITE_COMPARE_E:                result_en = Fee_Prv_MediumWrite_Compare(fsm_pst);               break;
# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
            case FEE_PRV_MEDIUM_WRITE_DETNWLERR_E:              result_en = Fee_Prv_MediumWrite_DetnWlErr(fsm_pst);             break;
# endif
            default:                                            result_en = MEMIF_JOB_FAILED;                                   break;
        }
    } while((MEMIF_JOB_OK == result_en) && (FEE_PRV_MEDIUM_WRITE_FINAL_E != fsm_pst->state_en));
    /* Evaluation of while loop: The while loop is required to speed up the state tranisition by one main function call.
     * Since the transition just starts the next step but does not wait till completion, the while loop will not be stuck
     * infinitely. */

    if(MEMIF_JOB_PENDING != result_en)
    {
        FEE_PRV_LIBSC(FEE_PRV_MEDIUM_WRITE_IDLE_E);
    }

    return(result_en);
}

/**
 * \brief   Do the work for an erase job.
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
*/
MemIf_JobResultType Fee_Prv_MediumEraseDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_Medium_tst * medium_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    medium_pst             = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;

    return(Fee_Prv_MediumFlsDo(medium_pst));
}

/**
 * \brief   Do the work for a compare job.
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
*/
MemIf_JobResultType Fee_Prv_MediumCompareFls2BufDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_Medium_tst * medium_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    medium_pst             = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;

    return(Fee_Prv_MediumFlsDo(medium_pst));
}

/**
 * \brief   Read data from the 1st flash location to the internal buffer.
 * \param   fsm_pst: reference to the statemachine data of the current job
 * \return  return value of the triggered read operation
 *
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCompareFls2Fls_Read(Fee_Prv_MediumCompareFls2Fls_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumData_tst *                 mediumData_pst;
    MemIf_JobResultType                      result_en;

    const uint8 bufIdx_u8 = 0; // always use the first buffer

    if(FEE_PRV_LIBENTRY)
    {
        /* Get the pointer to config data model */
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);
        feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];
        mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;

        fsm_pst->nrBytesCurrentChunk_u32 = rba_MemLib_Min(fsm_pst->length_u32 - fsm_pst->nrBytesDone_u32,
                feeBuf_pst->mediumBufSize_u32);

        Fee_Prv_MediumSetBufDirtyAndRead(
                fsm_pst->deviceName_en,
                &mediumData_pst->mediumBufData_ast[bufIdx_u8],
                fsm_pst->adrFls1_u32 + fsm_pst->nrBytesDone_u32,
                fsm_pst->nrPageShift1_u8,
                ((void *)feeBuf_pst->mediumBuf_pu8),
                fsm_pst->nrBytesCurrentChunk_u32);
    }

    result_en = Fee_Prv_MediumReadDo(fsm_pst->deviceName_en);

    if(MEMIF_JOB_OK == result_en)
    {
        FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COMPAREFLS2FLS_COMPARE_E);
    }

    return(result_en);
}

/**
 * \brief   Compare data from the internal buffer to the 2nd flash location
 * \param   fsm_pst: reference to the statemachine data of the current job
 * \return  return value of the triggered compare operation
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCompareFls2Fls_Compare(Fee_Prv_MediumCompareFls2Fls_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst    const * deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    MemIf_JobResultType                      result_en;

    const uint8 bufIdx_u8 = 0; // always use the first buffer

    if(FEE_PRV_LIBENTRY)
    {
        /* Get the pointer to config data model */
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);
        feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];

        Fee_Prv_MediumCompareFls2Buf(fsm_pst->deviceName_en,
                                     fsm_pst->adrFls2_u32 + fsm_pst->nrBytesDone_u32,
                                     fsm_pst->nrPageShift2_u8,
                                     feeBuf_pst->mediumBuf_pu8,
                                     fsm_pst->nrBytesCurrentChunk_u32);
    }

    result_en = Fee_Prv_MediumCompareFls2BufDo(fsm_pst->deviceName_en);

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->nrBytesDone_u32 += fsm_pst->nrBytesCurrentChunk_u32;
        if(fsm_pst->nrBytesDone_u32 < fsm_pst->length_u32)
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COMPAREFLS2FLS_READ_E);
        }
        else
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COMPAREFLS2FLS_FINAL_E);
        }
    }

    return(result_en);
}

/**
 * \brief   Do the work for the Fls2Fls compare operation.
 * \return job result of the comparison
*/
MemIf_JobResultType Fee_Prv_MediumCompareFls2FlsDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumCompareFls2Fls_tst *    fsm_pst;
    MemIf_JobResultType                   result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    fsm_pst                = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumCompareFls2Fls_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case FEE_PRV_MEDIUM_COMPAREFLS2FLS_READ_E:      result_en = Fee_Prv_MediumCompareFls2Fls_Read(fsm_pst);     break;
            case FEE_PRV_MEDIUM_COMPAREFLS2FLS_COMPARE_E:   result_en = Fee_Prv_MediumCompareFls2Fls_Compare(fsm_pst);  break;
            default:                                        result_en = MEMIF_JOB_FAILED;                               break;
        }
    } while((MEMIF_JOB_OK == result_en) && (FEE_PRV_MEDIUM_COMPAREFLS2FLS_FINAL_E != fsm_pst->state_en));
    /* Evaluation of while loop: The while loop is required to speed up the state tranisition by one main function call.
     * Since the transition just starts the next step but does not wait till completion, the while loop will not be stuck
     * infinitely. */

    if(MEMIF_JOB_PENDING != result_en)
    {
        FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COMPAREFLS2FLS_IDLE_E);
    }

    return(result_en);
}

/**
 * \brief   Do the work for a blank check job.
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
*/
MemIf_JobResultType Fee_Prv_MediumBlankCheckDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_Medium_tst *                  medium_pst;
    MemIf_JobResultType                   result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    medium_pst             = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;

    result_en = Fee_Prv_MediumFlsDo(medium_pst);

    if(MEMIF_BLOCK_INCONSISTENT == result_en)
    {
        /*
         * Map MEMIF_BLOCK_INCONSISTENT (Blank Check job has been executed and the result is not blank) to
         * MEMIF_BLOCK_FAILED (Job cannot be executed) -> The file system handles both cases in the same way.
         */
        result_en = MEMIF_JOB_FAILED;
    }

    return(result_en);
}

/**
 * \brief   Do the work for a shredder job.
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
*/
MemIf_JobResultType Fee_Prv_MediumShredderDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumData_tst *              mediumData_pst;
    MemIf_JobResultType                   result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;

    result_en = Fee_Prv_MediumProgramDo(mediumData_pst);

    return(result_en);
}

/**
 * \brief   The worker function for the CRC calculation.
 *
 * \param   deviceName_en       Device (or Instance) for which the job has to be performed
 * \return                      Job status
 * \retval  MEMIF_JOB_PENDING:  Job pending
 * \retval  MEMIF_JOB_OK:       CRC calculation successful
 * \retval  MEMIF_JOB_FAILED:   CRC calculation not successful (currently not possible)
*/
MemIf_JobResultType Fee_Prv_MediumCalcCrcDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_MediumCalcCrc_tst *              mediumCalcCrc_pst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    void const *                             ptr_pcv;
    MemIf_JobResultType                      result_en;
    uint32                                   nrBytesToConsider_u32;
    uint32                                   nrBytesAllowedToProcess_u32;

    const uint8  bufIdx_u8 = 0;  // always use the first buffer

    result_en = MEMIF_JOB_PENDING;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumCalcCrc_pst      = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumCalcCrc_st;
    feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];

    nrBytesToConsider_u32 = rba_MemLib_Min(feeBuf_pst->mediumBufSize_u32,
                                           (mediumCalcCrc_pst->length_u32 - mediumCalcCrc_pst->nrBytesDone_u32));

    /* Adapt data size for CRC calculation to the available Fee time budget.                                      */
    /* In case of a large data block (max. 64kB) which fits into the Medium buffer, the CRC calculation has to be */
    /* distributed over multiple Fee main calls.                                                                  */
    nrBytesAllowedToProcess_u32 = Fee_Prv_LibEffortGet(deviceName_en, FEE_PRV_LIMIT_CRCINRAM_CPYRAM_E, nrBytesToConsider_u32);

    if(nrBytesAllowedToProcess_u32 != nrBytesToConsider_u32)
    {
        /* Align to the next lower page border */
        nrBytesAllowedToProcess_u32 = Fee_Prv_LibFloor(nrBytesAllowedToProcess_u32, Fee_Prv_FsIfGetLogicalPageSize(deviceName_en));
    }

    if(0u < nrBytesAllowedToProcess_u32)
    {
        /* Load all data but process only number of data the time budget allows */
        result_en = Fee_Prv_MediumSelBufNext(deviceName_en, bufIdx_u8, nrBytesAllowedToProcess_u32, &ptr_pcv);

        if(MEMIF_JOB_OK == result_en)
        {
            /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
            *mediumCalcCrc_pst->crc_pu32 = Crc_CalculateCRC32(ptr_pcv,
                                                              nrBytesAllowedToProcess_u32,
                                                              *mediumCalcCrc_pst->crc_pu32,
                                                              mediumCalcCrc_pst->firstCall_b);

            mediumCalcCrc_pst->nrBytesDone_u32 += nrBytesAllowedToProcess_u32;
            mediumCalcCrc_pst->firstCall_b      = FALSE;

            result_en = MEMIF_JOB_PENDING;
        }
    }

    /* Length should never be smaller, but to be safe consider it */
    if(mediumCalcCrc_pst->length_u32 <= mediumCalcCrc_pst->nrBytesDone_u32)
    {
        result_en = MEMIF_JOB_OK;
    }

    return(result_en);
}

/**
 * \brief   Read data from the flash to the internal buffer.
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCopy_Read(Fee_Prv_MediumCopy_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst const *    deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumData_tst *                 mediumData_pst;
    Fee_Rb_DeviceName_ten                    deviceName_en;
    MemIf_JobResultType                      result_en;

    const uint8 bufIdx_u8 = 0;    // always use the first buffer

    deviceName_en = fsm_pst->deviceName_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* Get the pointer to config data model */
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
        mediumData_pst         = &deviceConfigTable_pcst->feeData_pst->mediumData_st;
        feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];

        fsm_pst->bytes2Copy_u32 = rba_MemLib_Min(fsm_pst->length_u32 - fsm_pst->done_u32, feeBuf_pst->mediumBufSize_u32);

        Fee_Prv_MediumSetBufDirtyAndRead(
                deviceName_en,
                &mediumData_pst->mediumBufData_ast[bufIdx_u8],
                fsm_pst->adrSrc_u32 + fsm_pst->done_u32,
                fsm_pst->nrPageShiftSrc_u8,
                ((void *)feeBuf_pst->mediumBuf_pu8),
                fsm_pst->bytes2Copy_u32);
    }

    result_en = Fee_Prv_MediumReadDo(deviceName_en);

    if(MEMIF_JOB_OK == result_en)
    {
        FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COPY_WRITE_E);
    }

    return(result_en);
}

/**
 * \brief   Write data from the internal buffer to the flash.
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_MediumCopy_Write(Fee_Prv_MediumCopy_tst * fsm_pst)
{
    Fee_Prv_ConfigDeviceTable_tst    const * deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Rb_DeviceName_ten                    deviceName_en;
    MemIf_JobResultType                      result_en;

    const uint8 bufIdx_u8 = 0; // always use the first buffer

    deviceName_en = fsm_pst->deviceName_en;

    if(FEE_PRV_LIBENTRY)
    {
        /* Get the pointer to config data model */
        deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(fsm_pst->deviceName_en);
        feeBuf_pst = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];

        Fee_Prv_MediumWrite(deviceName_en,
                            fsm_pst->adrDest_u32 + fsm_pst->done_u32,
                            fsm_pst->nrPageShiftDest_u8,
                            feeBuf_pst->mediumBuf_pu8,
                            fsm_pst->bytes2Copy_u32,
                            fsm_pst->writeSeq_en);
    }

    result_en = Fee_Prv_MediumWriteDo(deviceName_en);

    if(MEMIF_JOB_OK == result_en)
    {
        fsm_pst->done_u32 += fsm_pst->bytes2Copy_u32;
        if(fsm_pst->done_u32 < fsm_pst->length_u32)
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COPY_READ_E);
        }
        else
        {
            FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COPY_FINAL_E);
        }
    }

    return(result_en);
}

/**
 * \brief   Do the work for a copy operation.
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
*/
MemIf_JobResultType Fee_Prv_MediumCopyDo(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumCopy_tst *              fsm_pst;
    MemIf_JobResultType                   result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    fsm_pst                = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumCopy_st;

    do
    {
        switch(fsm_pst->state_en)
        {
            case FEE_PRV_MEDIUM_COPY_READ_E:    result_en = Fee_Prv_MediumCopy_Read(fsm_pst);    break;
            case FEE_PRV_MEDIUM_COPY_WRITE_E:   result_en = Fee_Prv_MediumCopy_Write(fsm_pst);   break;
            default:                            result_en = MEMIF_JOB_FAILED;                    break;
        }
    } while((MEMIF_JOB_OK == result_en) && (FEE_PRV_MEDIUM_COPY_FINAL_E != fsm_pst->state_en));
    /* Evaluation of while loop: The while loop is required to speed up the state tranisition by one main function call.
     * Since the transition just starts the next step but does not wait till completion, the while loop will not be stuck
     * infinitely. */

    if(MEMIF_JOB_PENDING != result_en)
    {
        FEE_PRV_LIBSC(FEE_PRV_MEDIUM_COPY_IDLE_E);
    }

    return(result_en);
}

/**
 * \brief   Set the iterator to a location in the flash.
 *          From there on the iterator can be used to step forwards or backwards through the flash.
 *          Additionally the maximum number of data to be prefetched will be set.
 *  Note:   The function does not validate if the passed address is within the limits of the sector. The function can be
 *          called only within one logical sector. otherwise, it might be rejected by Fls.
 *          So the caller of the function must ensure that the addresses + length passed is within limits.
 *  Note:   The function does not validate if the passed buffer index is within the limits.
 *          So the caller of the function must ensure that the passed buffer index is within limits,
 *          otherwise OOB accesses may occur.
 *
 * \param   deviceName_en        Device (or Instance) for which the job has to be performed
 * \param   bufIdx_u8            Buffer index
 * \param   adr_u32              Address in flash where the iterator shall point to
 * \param   nrPageShift_u8       Data shift to consider (in no. of pages)
 * \param   maxPrefetchLen_u32   Maximum amount of data to load including prefetch
 *
 * \attention   The general usage of a data shift (nrPageShift_u8 > 0) is only permitted in case adr_u32 +
 *              maxPrefetchLen_u32 never exceeds the end of a logical row. Fee_Prv_FsIfGetLogicalRowSize). Also,
 *              this feature shall not be used unless a complete row fits into the medium buffer.
*/
void Fee_Prv_MediumSelBufSetIterator(Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8, uint32 adr_u32, uint8 nrPageShift_u8, uint32 maxPrefetchLen_u32)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumBufData_tst *           mediumBufData_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst                = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumBufData_pst                     = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumBufData_ast[bufIdx_u8];
    mediumBufData_pst->iterator_u32       = adr_u32;
    mediumBufData_pst->nrPageShift_u8     = nrPageShift_u8;
    mediumBufData_pst->maxPrefetchLen_u32 = maxPrefetchLen_u32;

    return;
}

/**
 * \brief   Get the value of the flash iterator of the requested buffer.
 *          Note: If requested buffer is not available the first buffer is used
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   bufIdx_u8        Buffer index
 *
 * \attention:  1) The function does not validate if the passed buffer index is within the limits. So the caller of
 *                 this function must ensure that the passed buffer index is within limits, otherwise OOB accesses
 *                 may occur.
 *              2) Do not use this function together with a data shift (in Fee_Prv_MediumSelBufSetIterator,
 *                 nrPageShift_u8 > 0), because flash and buffer contents won't match.
 *
 * \seealso Fee_Prv_MediumSelBufSetIterator
*/
uint32 Fee_Prv_MediumSelBufGetIterator(Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    return deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumBufData_ast[bufIdx_u8].iterator_u32;
}

# if(FALSE != FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED)
/**
 * \brief   Find out whether the flash lost data on the same word line during last programming action.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \return  Word line error occurred YES/NO
*/
boolean Fee_Prv_MediumHasWlError(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumWrite_tst *             mediumWrite_pst;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumWrite_pst        = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumWrite_st;

    return(mediumWrite_pst->hasWlError_b);
}


/**
 * \brief  Get the data of a damaged wordline.
 *
 * \param   deviceName_en    Device (or Instance) for which the job has to be performed
 * \param   wlAdr_pu32       Pointer to variable to be filled with wordline error begin adress
 * \param   wlLen_pu32       Pointer to variable to be filled with wordline error length
 * \param   wlBfr_ppcv       Pointer to variable to be filled with pointer to wordline rescue buffer
*/
void Fee_Prv_MediumGetWlData(Fee_Rb_DeviceName_ten deviceName_en, uint32 * wlAdr_pu32, uint32 * wlLen_pu32, void const ** wlBfr_ppcv)
{
    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst;
    Fee_Prv_MediumWrite_tst *             mediumWrite_pst;
    uint32                                wlSize_u32        = Fee_Prv_MediumGetWlSize(deviceName_en);

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumWrite_pst        = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumWrite_st;

    if(NULL_PTR != wlAdr_pu32)
    {
        *wlAdr_pu32 = rba_MemLib_FloorPwr2(mediumWrite_pst->adrDest_u32, wlSize_u32);
    }

    if(NULL_PTR != wlLen_pu32)
    {
        *wlLen_pu32 = mediumWrite_pst->adrDest_u32 - rba_MemLib_FloorPwr2(mediumWrite_pst->adrDest_u32, wlSize_u32);
    }

    if(NULL_PTR != wlBfr_ppcv)
    {
        *wlBfr_ppcv = &(deviceConfigTable_pcst->mediumRescueBfr_pcu8[0]);
    }
}

# endif

/*
 * Those functions are activated if direct read from the Fls is not active.
 * Currently this is not fully supported in the whole Fee so that part must currently always stay OFF.
*/

/**
 * \brief   Set a pointer to the current iterator then increase the iterator for specified length.
 *  Note:   The function does not validate if the passed address is within the limits of the sector. The function can be
 *          called only within one logical sector. otherwise, it might be rejected by Fls.
 *          So the caller of the function must ensure that the addresses + length passed is within limits.
 *  Note:   The function does not validate if the passed buffer index is within the limits.
 *          So the caller of the function must ensure that the passed buffer index is within limits,
 *          otherwise OOB accesses may occur.
 *
 * \param   deviceName_en       Device (or Instance) for which the job has to be performed
 * \param   bufIdx_u8           Buffer index
 * \param   length_u32          Amount of bytes to step after pointer is set to current iterator
 * \param   ptr_ppcv            Pointer to the pointer which shall be set to the address of the iterator
 *
 * \retval  MEMIF_JOB_PENDING   Job still pending
 * \retval  MEMIF_JOB_OK        Job finished successfully
 * \retval  MEMIF_JOB_FAILED    Job finished with errors
*/
MemIf_JobResultType Fee_Prv_MediumSelBufNext(Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8, uint32 length_u32, void const ** ptr_ppcv)
{
    Fee_Prv_ConfigDeviceTable_tst    const * deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumBufData_tst *              mediumBufData_pst;
    MemIf_JobResultType                      result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    feeBuf_pst             = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];
    mediumBufData_pst      = &deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumBufData_ast[bufIdx_u8];

    result_en = Fee_Prv_MediumLoad(deviceConfigTable_pcst,
            bufIdx_u8,
            mediumBufData_pst->iterator_u32,
            mediumBufData_pst->nrPageShift_u8,
            length_u32,
            mediumBufData_pst->maxPrefetchLen_u32,
            FEE_PRV_POSTLOAD_E);

    if(MEMIF_JOB_OK == result_en)
    {
        uint32 index_u32        = mediumBufData_pst->iterator_u32 - mediumBufData_pst->limitLo_u32;
        *ptr_ppcv               = &feeBuf_pst->mediumBuf_pu8[index_u32];
        mediumBufData_pst->iterator_u32 += length_u32;
    }

    return(result_en);
}

/**
 * \brief   Set a pointer to the current iterator then decrease the iterator for specified length.
 *  Note:   The function does not validate if the passed address is within the limits of the sector. The function can be
 *          called only within one logical sector. otherwise, it might be rejected by Fls.
 *          So the caller of the function must ensure that the addresses + length passed is within limits.
 *  Note:   The function does not validate if the passed buffer index is within the limits.
 *          So the caller of the function must ensure that the passed buffer index is within limits,
 *          otherwise OOB accesses may occur.
 *
 * \param   deviceName_en       Device (or Instance) for which the job has to be performed
 * \param   bufIdx_u8           Buffer index
 * \param   length_u32          Amount of bytes to step after pointer is set to current iterator
 * \param   ptr_ppcv            Pointer to the pointer which shall be set to the address of the iterator
 *
 * \retval  MEMIF_JOB_PENDING   Job still pending
 * \retval  MEMIF_JOB_OK        Job finished successfully
 * \retval  MEMIF_JOB_FAILED    Job finished with errors
*/
MemIf_JobResultType Fee_Prv_MediumSelBufPre(Fee_Rb_DeviceName_ten deviceName_en, uint8 bufIdx_u8, uint32 length_u32, void const ** ptr_ppcv)
{
    Fee_Prv_ConfigDeviceTable_tst    const * deviceConfigTable_pcst;
    Fee_Prv_ConfigDeviceBufTable_tst const * feeBuf_pst;
    Fee_Prv_MediumBufData_tst *              mediumBufData_pst;
    MemIf_JobResultType                      result_en;

    /* Get the pointer to config data model */
    deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    mediumBufData_pst      = &(deviceConfigTable_pcst->feeData_pst->mediumData_st.mediumBufData_ast[bufIdx_u8]);

    result_en = Fee_Prv_MediumLoad(deviceConfigTable_pcst,
            bufIdx_u8,
            mediumBufData_pst->iterator_u32,
            mediumBufData_pst->nrPageShift_u8,
            length_u32,
            mediumBufData_pst->maxPrefetchLen_u32,
            FEE_PRV_PRELOAD_E);

    if(MEMIF_JOB_OK == result_en)
    {
        uint32 index_u32 = mediumBufData_pst->iterator_u32 - mediumBufData_pst->limitLo_u32;
        feeBuf_pst       = &deviceConfigTable_pcst->feeBufTable_pcast[bufIdx_u8];

        *ptr_ppcv                        = &feeBuf_pst->mediumBuf_pu8[index_u32];
        mediumBufData_pst->iterator_u32 -= length_u32;
    }

    return(result_en);
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"
#endif
