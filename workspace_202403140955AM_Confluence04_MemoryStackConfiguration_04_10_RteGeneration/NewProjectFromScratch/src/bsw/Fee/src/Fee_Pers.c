/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"


/* Disable the Fee common part when not needed */
#if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

/* Disable this unit when not needed */
# if(defined(FEE_PRV_CFG_RB_PERSIST_DATA) && (TRUE == FEE_PRV_CFG_RB_PERSIST_DATA))

/*
 **********************************************************************************************************************
 * Notes
 **********************************************************************************************************************
 * This unit duplicates some design patterns, which are already existing on NvM level:
 * a) Detection of a soft reset by means of an alive marker
 * b) Reading blocks in init (NvM_ReadAll)
 * c) Writing blocks in shutdown / on request (NvM_WriteAll / NvM_WriteBlock)
 *
 * There might be reasonable doubts if it is really the best architectural decision to realize all this on Fee level
 * again - instead of re-using the existing logic in NvM. However, the following constraints have been given when
 * introducing this unit:
 * 1) Re-use the already existing framework of the Fee-internal persistent data block (for the FeeFs2 sector erase
 *    counters). Since this involves persistent data block writes triggered by the file system's background, everything
 *    must be managed on Fee level.
 * 2) Permit stress measurement and driving cycle prediction also in HSM context (see comment 7 in CR185178). Since
 *    HSMs usually don't integrate the NvM component, everything must be implemented without using NvM features.
 * 3) The measurement and driving cycle prediction shall consider all users, not only NvM.
 * 4) The Fee-internal persistent data block shall be the last one to write in shutdown. When using NvM, the order
 *    can't (easily) be controlled.
*/


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Fee_Prv_Config.h"
#include "Fee_Prv_FsIf.h"
#include "Fee_Prv_Job.h"
#include "Fee_Prv_JobTypes.h"
#include "Fee_Prv_Lib.h"
#include "Fee_Prv_MediumTypes.h"   /* For FEE_RB_WRITE_PADDING_REQUIRED check only */
#include "Fee_Prv_Pers.h"
#include "Fee_Prv_PersTypes.h"
#include "Fee_Prv_Proc.h"
#include "Fee_Prv_ProcTypes.h"
#include "Fee_PrvTypes.h"
#include "Fee_Rb_Idx.h"
#include "Fee_Rb_Types.h"
#include "MemIf_Types.h"
#include "rba_MemLib.h"


/*
 **********************************************************************************************************************
 * Preprocessor checks and compile-time asserts
 **********************************************************************************************************************
*/

#  if(!defined FEE_RB_WRITE_PADDING_REQUIRED)
#error "Please remove call of Fee_Prv_LibCeil in Fee_Prv_PersStressMeasmtUpd when Fee_Medium is always padding"
#  endif

#  if(defined(FEE_PRV_CFG_RB_STRESS_MEASMT) && (TRUE == FEE_PRV_CFG_RB_STRESS_MEASMT))

#   if(!defined(FLS_RB_VENDOR_ID) || (0x06 != FLS_RB_VENDOR_ID))
#error "FEE_PRV_CFG_RB_STRESS_MEASMT can't be enabled for a 3rd party Fls/MemAcc!"
#   endif

#  endif


/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

#  if(!defined(FEE_PRV_CFG_RB_STRESS_MEASMT) || (FALSE == FEE_PRV_CFG_RB_STRESS_MEASMT))

LOCAL_INLINE void    Fee_Prv_PersDataUpdProgdBytWoOvf (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint32                                   nrBytProgd_u32);

#  endif


LOCAL_INLINE void    Fee_Prv_PersDataInctDrvCycCntrWoOvf    (Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst);
LOCAL_INLINE void    Fee_Prv_PersDataInctBlkWritesCntrWoOvf (Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst);
LOCAL_INLINE void    Fee_Prv_PersDataGetHlBufAndLen         (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint8 **                                 dataBufHl_ppu8,
        uint32 *                                 nrLenHl_pu32);


/*
 **********************************************************************************************************************
 * Static declarations
 **********************************************************************************************************************
*/

static void    Fee_Prv_PersDataGetFsSpcBufAndLen (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint8 **                                 dataBufFsSpc_ppu8,
        uint32 *                                 nrLenFsSpc_pu32);

#  if(defined(FEE_PRV_CFG_RB_STRESS_MEASMT) && (TRUE == FEE_PRV_CFG_RB_STRESS_MEASMT))

static void    Fee_Prv_PersDataUpdProgdBytWoOvf (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint32                                   nrBytProgd_u32);


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/**
 * \brief   Update programmed byte counter, but avoid its uint64 overflow
 *
 * \param   xCfgDevTbl_pcst   Pointer to config table
 * \param   nrBytProgd_u32    Number of programmed bytes to add to counter
*/
static void Fee_Prv_PersDataUpdProgdBytWoOvf(
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint32                                   nrBytProgd_u32)
{
    if((FEE_PRV_MAX_UINT64-nrBytProgd_u32) >= xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrBytProgdTot_u64)
    {
        xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrBytProgdTot_u64 += nrBytProgd_u32;
    }
    else
    {
        xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrBytProgdTot_u64 = FEE_PRV_MAX_UINT64;
    }

    return;
}


/**
 * \brief   Update amount of stress exposed to medium
 *
 * \param   deviceName_en    Device instance, for which the update is requested
 * \param   nrBytProgd_u32   Stress exposed to medium (in number of programmed bytes)
*/
void Fee_Prv_PersStressMeasmtUpd(Fee_Rb_DeviceName_ten deviceName_en, uint32 nrBytProgd_u32)
{
    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst;
    uint32                                   nrBytRnddToNxtPage_u32;

    /* Get config table */
    xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

    /* Given instance has persistent data (i.e. a saved zone)? */
    if(NULL_PTR != xCfgDevTbl_pcst->persSavedZone_pst)
    {
        /* Consider padding bytes in case less than a page has been programmed
         * Note: This can be removed when Fee_Medium is always padding (after replacing Fls by MemAcc) */
        nrBytRnddToNxtPage_u32 = Fee_Prv_LibCeil(nrBytProgd_u32, Fee_Prv_FsIfGetLogicalPageSize(deviceName_en));

        /* Programming occurs during initialization of FeeFs, and the saved zone hasn't yet been setup? Note: We don't
         * check the Fee driver state here, since it changes from UNINIT to INIT before Fee_Prv_PersDataRead. When
         * coming out of a hard reset, we should not risk an externally triggered access to the saved zone during the
         * short time window where it hasn't yet been setup! */
        if(FEE_PRV_PERS_SAVED_ZONE_ALIVE != xCfgDevTbl_pcst->persSavedZone_pst->idSavedZoneMarker_u32)
        {
            /* Memorize amount of data programmed during FeeFs-specific initialization. It is safe to assume that all
             * programming operations during the FeeFs-specific initialization won't result in a uint32 overflow. */
            xCfgDevTbl_pcst->feeData_pst->persData_st.cntrBytProgdInIni_u32 += nrBytRnddToNxtPage_u32;
        }
        else
        {
            /* Update amount of programmed data (but avoid its overflow) */
            Fee_Prv_PersDataUpdProgdBytWoOvf(xCfgDevTbl_pcst, nrBytRnddToNxtPage_u32);
        }
    }
    else
    {
        /* Given instance doesn't have persistent data/a saved zone; do nothing */
    }

    return;
}


/**
 * \brief   Restart the stress measurement and driving cycle prediction
 *
 * \param   deviceName_en   Device instance, for which restart is requested
 *
 * \return  Request acceptance
 * \retval  E_OK      The request has been accepted; stress measurement and driving cycle prediction are restarted
 * \retval  E_NOT_OK  The request has been erroneous and therefore been rejected
*/
Std_ReturnType Fee_Prv_PersStressMeasmtRstrt(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

    /* Local variable */
    Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst;

    /* Get config table */
    xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

    /* Given instance has persistent data (i.e. a saved zone)? */
    if(NULL_PTR != xCfgDevTbl_pcst->persSavedZone_pst)
    {
        /* Saved zone has been setup? Note: We don't check the Fee driver state here, since it changes from UNINIT
         * to INIT before Fee_Prv_PersDataRead. When coming out of a hard reset, we should not risk an externally
         * triggered access to the saved zone during the short time window where it hasn't yet been setup! */
        if(FEE_PRV_PERS_SAVED_ZONE_ALIVE == xCfgDevTbl_pcst->persSavedZone_pst->idSavedZoneMarker_u32)
        {
            /* Memorize amount of programmed data and driving cycles at measurement (re-)start */
            xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrBytProgdPredStrt_u64 =
                    xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrBytProgdTot_u64;
            xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrDrvCycPredStrt_u32 =
                    xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrDrvCycTot_u32;

            /* Reset driving cycle prediction result */
            xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.nrDrvCycMaxRea_u32 = 0u;

            stRet_u8 = E_OK;
        }
        else
        {
            /* Deny request if the saved zone hasn't yet been setup */
            Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_RESTART_STRESS_MEASMT, FEE_E_UNINIT);
            stRet_u8 = E_NOT_OK;
        }
    }
    else
    {
        /* Deny request if given instance doesn't have persistent data/a saved zone */
        Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_RESTART_STRESS_MEASMT, FEE_E_INVALID_DEVICE_NAME);
        stRet_u8 = E_NOT_OK;
    }

    return(stRet_u8);
}


/**
 * \brief   Get result of stress measurement and driving cycle prediction
 *
 * \param   deviceName_en         Device instance, for which result is requested
 * \param   isSncePredStrtReq_b   Request result since measurement (re-)start (TRUE) or over ECU lifetime (FALSE)
 * \param   nrDrvCycMaxRea_pu32   Pointer to location, where maximum number of reachable driving cycles shall be stored
 *
 * \return  Result of function
 * \retval  E_OK      if requested information has been provided in the given pointer
 * \retval  E_NOT_OK  if requested information couldn't be provided because of some error
 *
 * \attention  1) This function predicts the maximum number of reachable driving cycles under the assumption, that the
 *                stress is evenly distributed over the complete data-NvM.
 *             2) If this function is called without calling Fee_Rb_Idx_RestartStressMeasmt before, isSncePredStrtReq_b
 *                will be ignored, and the *nrDrvCycMaxRea_pu32 will be set to the ECU lifetime result.
 *             3) If this function is called without having any programmed bytes, or without having at least one new
 *                driving cycle since measurement (re-)start, *nrDrvCycMaxRea_pu32 will be set to 0 (the function
 *                returns E_OK in that case).
 *             4) If this function is called without having the Fee-internal persistent data block stored, it returns
 *                E_NOT_OK. Fee_Rb_(Idx_)WritePersistentData must be called (during each shutdown!) in order to obtain
 *                a meaningful stress measurement and driving cycle prediction.
*/
Std_ReturnType Fee_Prv_PersStressMeasmtGetRes(
        Fee_Rb_DeviceName_ten    deviceName_en,
        boolean                  isSncePredStrtReq_b,
        uint32 *                 nrDrvCycMaxRea_pu32)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst;
    uint64                                   cntrBytProgd_u64, cntrBytProgdPerCyc_u64, cntrBytProgPtd_u64;
    uint32                                   cntrDrvCyc_u32;

    /* Get config table */
    xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

    /* Given instance has persistent data (i.e. a saved zone)? */
    if(NULL_PTR != xCfgDevTbl_pcst->persSavedZone_pst)
    {
        /* (Consistent) Fee-internal persistent data block stored in data-NvM? */
        if(xCfgDevTbl_pcst->persSavedZone_pst->isBlkStrd_b)
        {
            /* Determine number of programmed bytes and driving cycles over ECU lifetime */
            cntrBytProgd_u64 = xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrBytProgdTot_u64;
            cntrDrvCyc_u32   = xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrDrvCycTot_u32;

            /* Result since measurement (re-)start requested? */
            if(isSncePredStrtReq_b)
            {
                /* Determine number of programmed bytes and driving cycles since measurement (re-)start */
                cntrBytProgd_u64 -= xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrBytProgdPredStrt_u64;
                cntrDrvCyc_u32   -= xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrDrvCycPredStrt_u32;
            }
            else
            {
                /* Keep ECU lifetime values */
            }

            /* Some programmed bytes and at least one new driving cycle since measurement (re-)start? */
            if( (0u < cntrBytProgd_u64) && (0u < cntrDrvCyc_u32) )
            {
                /* Determine avg. number of bytes, which have been programmed per cycle since measurement (re-)start */
                cntrBytProgdPerCyc_u64 = cntrBytProgd_u64 / cntrDrvCyc_u32;

                /* Determine max. permitted number of bytes that can be programmed with an evenly distributed stress */
                cntrBytProgPtd_u64 = (uint64)xCfgDevTbl_pcst->persSavedZone_pst->nrEraseProgCycPtd_u32 *
                                                xCfgDevTbl_pcst->persSavedZone_pst->nrDeviceSize_u32;

                /* Determine number of reachable driving cycle */
                xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.nrDrvCycMaxRea_u32 =
                        (uint32)(cntrBytProgPtd_u64 / cntrBytProgdPerCyc_u64);
                /*      (uint32)( (cntrBytProgPtd_u64 * cntrDrvCycSnceStrt_u32) / cntrBytProgdSnceStrt_u64 )
                 *      would be more precise, but might result in an uint64 overflow that we should avoid here. */
            }
            else
            {
                /* Avoid division by zero, but inform the user by a result causing a failed driving cycle prediction
                 * test. A DET entry isn't appropriate here, since this is no development error but a user operation
                 * issue. */
                xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.nrDrvCycMaxRea_u32 = 0u;
            }

            /* Provide result in given parameter */
            *nrDrvCycMaxRea_pu32 = xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.nrDrvCycMaxRea_u32;
            stRet_u8 = E_OK;
        }
        else
        {
            /* Deny request if Fee-internal persistent data block hasn't yet been read, is missing or inconsistent */
            Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_GET_STRESS_MEASMT_RESULT, FEE_E_INVALID_SEQUENCE);
            stRet_u8 = E_NOT_OK;
        }
    }
    else
    {
        /* Deny request if given instance doesn't have persistent data/a saved zone */
        Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_GET_STRESS_MEASMT_RESULT, FEE_E_INVALID_DEVICE_NAME);
        stRet_u8 = E_NOT_OK;
    }

    return(stRet_u8);
}


/**
 * \brief   Increment driving cycle counter, but avoid its uint32 overflow
 *
 * \param   xCfgDevTbl_pcst   Pointer to config table
*/
LOCAL_INLINE void Fee_Prv_PersDataInctDrvCycCntrWoOvf(Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst)
{
    /* Increment driving cycle counter (but avoid its uint32 overflow) */
    uint32    cntrDrvCycTot_u32 = xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrDrvCycTot_u32;
    xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrDrvCycTot_u32 = rba_MemLib_Max(
                                                                        cntrDrvCycTot_u32,
                                                                        cntrDrvCycTot_u32+1u );
    return;
}


/**
 * \brief   Increment persistent data block write counter, but avoid its uint32 overflow
 *
 * \param   xCfgDevTbl_pcst   Pointer to config table
*/
LOCAL_INLINE void Fee_Prv_PersDataInctBlkWritesCntrWoOvf(Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst)
{
    /* Increment persistent data block write counter (but avoid its uint32 overflow) */
    uint32    cntrPersBlkWrites_u32 = xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrPersBlkWrites_u32;
    xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st.cntrPersBlkWrites_u32 = rba_MemLib_Max(
                                                                            cntrPersBlkWrites_u32,
                                                                            cntrPersBlkWrites_u32+1u );
    return;
}

/**
 * \brief   Get pointer to the Fee high level persistent data buffer and its length
 *
 * \param   xCfgDevTbl_pcst   Pointer to config table
 * \param   dataBufHl_ppu8    Pointer location, where Fee high level persistent data buffer shall be stored
 * \param   nrLenHl_pu32      Pointer to location, where length of data shall be stored
*/
LOCAL_INLINE void    Fee_Prv_PersDataGetHlBufAndLen (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint8 **                                 dataBufHl_ppu8,
        uint32 *                                 nrLenHl_pu32)
{
    /* Fee high level persistent data not existing */
    *dataBufHl_ppu8 = (uint8 *)&xCfgDevTbl_pcst->persSavedZone_pst->dataHl_st;
    *nrLenHl_pu32   = xCfgDevTbl_pcst->persDataSizeHl_u8;
    return;
}


#  else

/**
 * \brief   Dummy function to keep unit compilable when stress measurement and driving cycle prediction are disabled
*/
LOCAL_INLINE void Fee_Prv_PersDataUpdProgdBytWoOvf (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint32                                   nrBytProgd_u32)
{
    (void)xCfgDevTbl_pcst;
    (void)nrBytProgd_u32;
    return;
}


/**
 * \brief   Dummy function to keep unit compilable when stress measurement and driving cycle prediction are disabled
*/
LOCAL_INLINE void Fee_Prv_PersDataInctDrvCycCntrWoOvf(Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst)
{
    (void)xCfgDevTbl_pcst;
    return;
}


/**
 * \brief   Dummy function to keep unit compilable when stress measurement and driving cycle prediction are disabled
*/
LOCAL_INLINE void Fee_Prv_PersDataInctBlkWritesCntrWoOvf(Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst)
{
    (void)xCfgDevTbl_pcst;
    return;
}


/**
 * \brief   Dummy function to keep unit compilable when stress measurement and driving cycle prediction are disabled
*/
LOCAL_INLINE void Fee_Prv_PersDataGetHlBufAndLen (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint8 **                                 dataBufHl_ppu8,
        uint32 *                                 nrLenHl_pu32)
{
    (void)xCfgDevTbl_pcst;

    /* Fee high level persistent data not existing */
    *dataBufHl_ppu8 = NULL_PTR;
    *nrLenHl_pu32   = 0u;
    return;
}
#  endif


/**
 * \brief   Get pointer to the FeeFs-specific persistent data buffer and its length
 *
 * \param   xCfgDevTbl_pcst     Pointer to config table
 * \param   dataBufFsSpc_ppu8   Pointer to location, where FeeFs-specific persistent data buffer shall be stored
 * \param   nrLenFsSpc_pu32     Pointer to location, where length of data shall be stored
 *
 * \attention  When there is no FeeFs-specific persistent data, the buffer will be set to NULL_PTR and the length to 0.
*/
static void Fee_Prv_PersDataGetFsSpcBufAndLen(
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint8 **                                 dataBufFsSpc_ppu8,
        uint32 *                                 nrLenFsSpc_pu32)
{
    /* Local variable */
    uint16     nrBlkLen_u16;

    /* FeeFs-specific persistent data existing? */
    *dataBufFsSpc_ppu8 = Fee_Prv_FsIfGetPersistentBufAdr(xCfgDevTbl_pcst->deviceName_en);
    if(NULL_PTR != *dataBufFsSpc_ppu8)
    {
        /* Determine block length */
        nrBlkLen_u16 = Fee_Prv_ConfigGetBlockLengthByBlockNr(
                            xCfgDevTbl_pcst->persDataBlkNr_u16,
                            Fee_Prv_FsIfGetBlockPropertiesTable(xCfgDevTbl_pcst->deviceName_en) );

        /* Provide length of FeeFs-specific persistent data (buffer) in given parameter */
        *nrLenFsSpc_pu32 = nrBlkLen_u16 - xCfgDevTbl_pcst->persDataSizeHl_u8;
    }
    else
    {
        /* FeeFs-specific persistent data not existing */
        *nrLenFsSpc_pu32 = 0u;
    }

    return;
}


/**
 * \brief   Function called during Fee_Init() BEFORE Fee_Prv_FsIfInit(). It zero-initializes all environment variables
 *          of this unit. Variables insided the saved zone are only initialized upon its loss.
 *
 * \param   xCfgDevTbl_pcst   Pointer to the config table, which has to be initialized
*/
void Fee_Prv_PersInit(Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst)
{
    /* Local variables */
    Fls_Rb_SectorInfo_tst    xSectorInfo_st;
    uint32                   adrStrt_u32, nrLenFsSpc_u32;
    uint8 *                  dataFsSpc_pu8;

    /* Zero-initialize this unit's environment variables */
    rba_MemLib_MemSet((uint8 *)&xCfgDevTbl_pcst->feeData_pst->persData_st, 0u, sizeof(Fee_Prv_PersData_tst));

    /* Given instance has persistent data (i.e. a saved zone)? */
    if(NULL_PTR != xCfgDevTbl_pcst->persSavedZone_pst)
    {
        /* Saved zone is lost? Note: We can't check the Fee driver state here, since this is (and must be) set to UNINIT
         * after each hard or soft reset (Fee_Prv_ProcInit). */
        if(FEE_PRV_PERS_SAVED_ZONE_ALIVE != xCfgDevTbl_pcst->persSavedZone_pst->idSavedZoneMarker_u32)
        {
            /* Zero-initialize Fee high level persistent data in saved zone */
            rba_MemLib_MemSet((uint8 *)xCfgDevTbl_pcst->persSavedZone_pst, 0u, sizeof(Fee_Prv_PersSavedZone_tst));

            /* Determine start address of data-NvM */
            adrStrt_u32 = Fee_Prv_FsIfGetEmulationStartAdr(xCfgDevTbl_pcst->deviceName_en);

            /* Sector info available? */
            if(E_OK == Fee_Prv_Lib_Fls_Rb_GetSectorInfo(
                    Fls_Rb_GetDeviceIdFromDrvIndex(xCfgDevTbl_pcst->deviceIdx_u8),
                    adrStrt_u32 - Fee_Prv_FsIfGetBaseAdr(xCfgDevTbl_pcst->deviceName_en),
                    &xSectorInfo_st) )
            {
                /* Memorize device size and permitted erase/program cycles in order to have them accessible
                 * in the same struct like the stress measurement and driving cycle prediction variables. */
                xCfgDevTbl_pcst->persSavedZone_pst->nrDeviceSize_u32 = 1u +
                                        (Fee_Prv_FsIfGetEmulationEndAdr(xCfgDevTbl_pcst->deviceName_en) - adrStrt_u32);
                xCfgDevTbl_pcst->persSavedZone_pst->nrEraseProgCycPtd_u32 = xSectorInfo_st.nrSpecifiedEraseCycles_u32;
            }
            else
            {
                /* In case of a 3rd party Fls/Memacc, there is no sector info available,
                 but this would cause a build abort for FEE_PRV_CFG_RB_STRESS_MEASMT above! */
            }

            /* FeeFs-specific persistent data existing? */
            Fee_Prv_PersDataGetFsSpcBufAndLen(xCfgDevTbl_pcst, &dataFsSpc_pu8, &nrLenFsSpc_u32);
            if(NULL_PTR != dataFsSpc_pu8)
            {
                /* Zero-initialize FeeFs-specific persistent data in saved zone */
                rba_MemLib_MemSet(dataFsSpc_pu8, 0u, nrLenFsSpc_u32);
            }
            else
            {
                /* No FeeFs-specific persistent data to initialize */
            }
        }
        else
        {
            /* Saved zone is still alive, keep its contents as is */
        }
    }
    else
    {
        /* Given instance doesn't have persistent data/a saved zone; do nothing */
    }

    return;
}


/**
 * \brief   Function called during Fee_Init() AFTER Fee_Prv_FsIfInit(). Upon loss of the saved zone, the contained
 *          variables are initialzed with data read from the Fee-internal persistent data block (if stored).
 *
 * \param   xCfgDevTbl_pcst   Pointer to the config table, which has to be initialized
*/
void Fee_Prv_PersDataRead(Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst)
{
    /* Local variables */
    uint16                                    nrBlk_u16;
    Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst;
    Fee_Prv_JobDesc_tst                       xJob_st;
    MemIf_JobResultType                       stRes_en;
    uint8 *                                   dataBuf_pu8;
    uint32                                    nrLenHl_u32, nrLenFsSpc_u32;

    /* Given instance has persistent data (i.e. a saved zone)? */
    if(NULL_PTR != xCfgDevTbl_pcst->persSavedZone_pst)
    {
        /* Saved zone is lost? Note: We can't check the Fee driver state here, since this is (and must be) set to INIT
         * whenever this function is called - no matter if coming out of a hard or soft reset (Fee_Rb_Idx_Init). */
       if(FEE_PRV_PERS_SAVED_ZONE_ALIVE != xCfgDevTbl_pcst->persSavedZone_pst->idSavedZoneMarker_u32)
       {
           nrBlk_u16        = xCfgDevTbl_pcst->persDataBlkNr_u16;
           xBlkPptyTbl_pcst = Fee_Prv_FsIfGetBlockPropertiesTable(xCfgDevTbl_pcst->deviceName_en);

           /* Setup descriptor for read job */
           xJob_st.type_en         = FEE_RB_JOBTYPE_READ_E;
           xJob_st.jobMode_en      = Fee_Prv_JobGetJobMode(xCfgDevTbl_pcst, FEE_RB_JOBTYPE_READ_E);
           xJob_st.bfr_pu8         = xCfgDevTbl_pcst->feeBufTable_pcast->persDataBuf_pu8;
           xJob_st.bfr_pcu8        = NULL_PTR;
           xJob_st.blockNumber_u16 = nrBlk_u16;
           xJob_st.length_u16      = Fee_Prv_ConfigGetBlockLengthByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);
           xJob_st.offset_u16      = 0u;
           xJob_st.nrBytTot_u16    = xJob_st.length_u16;
           xJob_st.bytDone_un.cntrPayloadBytDone_u32 = 0u;
           xJob_st.idPers_u16      = Fee_Prv_ConfigGetBlockPersistentIdByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);
           xJob_st.statusFlag_u16  = Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);
           xJob_st.isChunkJob_b    = FALSE;
           xJob_st.isUnknownBlk_b  = FALSE;

           /* Start read job */
           if(E_OK == Fee_Prv_JobPut(xCfgDevTbl_pcst->deviceName_en, FEE_SID_READ, &xJob_st))
           {
               /* Perform the read */
               do
               {
                   /* Basically, we could call Fee_Prv_ProcMainFunctionIntern and Fee_Prv_JobGetResult instead of
                    * Fee_Rb_Idx_MainFunction and Fee_Rb_Idx_GetInternalJobResult here. This would reduce the runtime
                    * during initialization, but also involves some risks: The user might have placed a stop mode/
                    * cancel request and/or start scheduling the main function in parallel. This is all very unlikely
                    * since we are still in initialization phase, but Fee_Rb_Idx_MainFunction and
                    * Fee_Rb_Idx_GetInternalJobResult help to mitigate these risks. */
                   Fee_Rb_Idx_MainFunction(xCfgDevTbl_pcst->deviceName_en);
                   stRes_en = Fee_Rb_Idx_GetInternalJobResult(xCfgDevTbl_pcst->deviceName_en);
               } while(MEMIF_JOB_PENDING == stRes_en);
           }
           else
           {
               stRes_en = MEMIF_JOB_FAILED;
           }

           /* Read has succeeded? */
           if(MEMIF_JOB_OK == stRes_en)
           {
               /* Fee high level persistent data existing? */
               Fee_Prv_PersDataGetHlBufAndLen(xCfgDevTbl_pcst, &dataBuf_pu8, &nrLenHl_u32);
               if(NULL_PTR != dataBuf_pu8)
               {
                   /* Initialize Fee high level persistent data with block contents */
                   rba_MemLib_MemCopy(
                           xCfgDevTbl_pcst->feeBufTable_pcast->persDataBuf_pu8,
                           dataBuf_pu8,
                           nrLenHl_u32 );
               }
               else
               {
                   /* No Fee high level persistent data to initialize */
               }

               /* FeeFs-specific persistent data existing? */
               Fee_Prv_PersDataGetFsSpcBufAndLen(xCfgDevTbl_pcst, &dataBuf_pu8, &nrLenFsSpc_u32);
               if(NULL_PTR != dataBuf_pu8)
               {
                   /* Initialize FeeFs-specific persistent data with block contents */
                   rba_MemLib_MemCopy(
                           nrLenHl_u32 + xCfgDevTbl_pcst->feeBufTable_pcast->persDataBuf_pu8,
                           dataBuf_pu8,
                           nrLenFsSpc_u32 );
               }
               else
               {
                   /* No FeeFs-specific persistent data to initialize */
               }

               /* Memorize that a consistent Fee-internal persistent data block is stored in the data-NvM */
               xCfgDevTbl_pcst->persSavedZone_pst->isBlkStrd_b = TRUE;
           }
           else
           {
               /* Do nothing; persistent data has already been zero-initialized by Fee_Prv_PersInit */
           }
       }
       else
       {
           /* Saved zone is still alive, keep its contents as is */
           stRes_en = MEMIF_JOB_OK;
       }

       /* Persistent data either still alive in saved zone, or loaded from persistent data block? */
       if(MEMIF_JOB_OK == stRes_en)
       {
           /* Increment driving cycle counter (but avoid its uint32 overflow) */
           Fee_Prv_PersDataInctDrvCycCntrWoOvf(xCfgDevTbl_pcst);
       }
       else
       {
           /* Keep driving cycle counter as is (zero-initialized by Fee_Prv_PersInit) */
       }

       /* Saved zone is ready now */
       xCfgDevTbl_pcst->persSavedZone_pst->idSavedZoneMarker_u32 = FEE_PRV_PERS_SAVED_ZONE_ALIVE;

       /* Consider amount of data programmed during initialization */
       Fee_Prv_PersDataUpdProgdBytWoOvf(
               xCfgDevTbl_pcst,
               xCfgDevTbl_pcst->feeData_pst->persData_st.cntrBytProgdInIni_u32 );
    }
    else
    {
        /* Given instance doesn't have persistent data/a saved zone; do nothing */
    }

    return;
}


/**
 * \brief   Fill (and return pointer to) the Fee high level buffer, which will be used for writing the persistent data
 *
 * \param   deviceName_en       Device instance, for which the write operation will be requested
 * \param   dataBufFsSpc_pcu8   Pointer to FeeFs-specific persistent data (or NULL_PTR in case no such data existing)
 * \param   nrLenFsSpc_u32      Length of FeeFs-specific persistent data
 *
 * \return  Pointer to the (filled) Fee high level buffer, which is used for writing the persistent data
*/
uint8 const * Fee_Prv_PersDataFillWriteBuf(
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint8 const *            dataFsSpc_pcu8,
        uint32                   nrLenFsSpc_u32)
{
    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst;
    uint8 *                                  dataHl_pu8;
    uint32                                   nrLenHl_u32;

    /* Get config table */
    xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

    /* Increment persistent data block write counter (but avoid its uint32 overflow) */
    Fee_Prv_PersDataInctBlkWritesCntrWoOvf(xCfgDevTbl_pcst);

    /* Fee high level persistent data existing? */
    Fee_Prv_PersDataGetHlBufAndLen(xCfgDevTbl_pcst, &dataHl_pu8, &nrLenHl_u32);
    if(NULL_PTR != dataHl_pu8)
    {
        /* Copy Fee high level persistent data to write buffer (to avoid changes during write operation) */
        rba_MemLib_MemCopy(
                dataHl_pu8,
                xCfgDevTbl_pcst->feeBufTable_pcast[0].persDataBuf_pu8,
                nrLenHl_u32 );
    }
    else
    {
        /* No Fee high level persistent data existing */
    }

    /* FeeFs-specific persistent data existing? */
    if(NULL_PTR != dataFsSpc_pcu8)
    {
        /* Append FeeFs-specific persistent data to write buffer (to avoid changes during write operation) */
        rba_MemLib_MemCopy(
                dataFsSpc_pcu8,
                nrLenHl_u32 + xCfgDevTbl_pcst->feeBufTable_pcast[0].persDataBuf_pu8,
                nrLenFsSpc_u32 );
    }
    else
    {
        /* No FeeFs-specific persistent data existing */
    }

    /* Return pointer to Fee high level write buffer */
    return(xCfgDevTbl_pcst->feeBufTable_pcast[0].persDataBuf_pu8);
}


/**
 * \brief   Request to start a write job for the Fee-internal persistent data
 *
 * \param   deviceName_en   Device instance, for which job is requested
 *
 * \return  Request acceptance
 * \retval  E_OK      The request has been accepted, and the job processing is started
 * \retval  E_NOT_OK  The request has been erroneous and therefore been rejected
 *
 * \attention  The caller of this function (i.e. the order unit) shall reject external write request in case
 *             of an invalid device name, or in case the Fee is either not yet initialized or in stop mode.
*/
Std_ReturnType Fee_Prv_PersDataWrite(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *     xCfgDevTbl_pcst;
    uint8 *                                   dataFsSpc_pu8;
    uint32                                    nrLenFsSpc_u32;
    Fee_Rb_User_ten                           idUser_en;
    Fee_Prv_ProcDriverState_ten               stDrvr_en;
    uint8 const *                             dataToWrite_pcu8;
    Fee_Prv_JobDesc_tst                       xJob_st;
    uint16                                    nrBlk_u16;
    Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst;

    /* Get config table */
    xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

    /* Given instance has persistent data (i.e. a saved zone)? */
    if(NULL_PTR != xCfgDevTbl_pcst->persSavedZone_pst)
    {
        /* Get FeeFs-specific persistent data and its length (if existing) */
        Fee_Prv_PersDataGetFsSpcBufAndLen(xCfgDevTbl_pcst, &dataFsSpc_pu8, &nrLenFsSpc_u32);

        /* The slot for Fee-internal jobs (debug user) might already be occupied:
         * - A debug write might have been triggered from Fee_Prv_DbgMainFunction (only if FEE_PRV_CFG_DBG_BLOCK is TRUE).
         * - The user has called Fee_Rb_(Idx_)WritePersistentData twice, while the first one still hasn't been complete.
         * But even if the slot for Fee-internal jobs is empty, an Fee-internal persistent data block update might be
         * ongoing. This situation occurs only in FeeFs2 (and only if FEE_PRV_CFG_RB_GET_NR_SECTOR_ERASES is TRUE). There,
         * a sector erase will trigger the block update for the sector erase counters (from background).
         * In any of the above situations, we should reject the request. */
        idUser_en = Fee_Prv_JobGetActvUser(xCfgDevTbl_pcst);
        stDrvr_en = Fee_Prv_ProcGetDriverState(deviceName_en);
        if( (FEE_RB_USER_DEBUG_E != idUser_en) && (FEE_PRV_PROC_DRIVERSTATE_BACKGROUND_E != stDrvr_en) )
        {
            dataToWrite_pcu8 = Fee_Prv_PersDataFillWriteBuf(deviceName_en, dataFsSpc_pu8, nrLenFsSpc_u32);
        }
        else
        {
            dataToWrite_pcu8 = NULL_PTR;
        }

        /* No persistent (or debug) data write ongoing? */
        if(NULL_PTR != dataToWrite_pcu8)
        {
            nrBlk_u16        = xCfgDevTbl_pcst->persDataBlkNr_u16;
            xBlkPptyTbl_pcst = Fee_Prv_FsIfGetBlockPropertiesTable(deviceName_en);

            /* Setup descriptor for write job */
            xJob_st.type_en         = FEE_RB_JOBTYPE_WRITE_E;
            xJob_st.jobMode_en      = Fee_Prv_JobGetJobMode(xCfgDevTbl_pcst, FEE_RB_JOBTYPE_WRITE_E);
            xJob_st.bfr_pu8         = NULL_PTR;
            xJob_st.bfr_pcu8        = dataToWrite_pcu8;
            xJob_st.blockNumber_u16 = nrBlk_u16;
            xJob_st.length_u16      = Fee_Prv_ConfigGetBlockLengthByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);
            xJob_st.offset_u16      = 0u;
            xJob_st.nrBytTot_u16    = xJob_st.length_u16;
            xJob_st.bytDone_un.cntrPayloadBytDone_u32 = 0u;
            xJob_st.idPers_u16      = Fee_Prv_ConfigGetBlockPersistentIdByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);
            xJob_st.statusFlag_u16  = Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);
            xJob_st.isChunkJob_b    = FALSE;
            xJob_st.isUnknownBlk_b  = FALSE;

            /* Start write job */
            stRet_u8 = Fee_Prv_JobPut(deviceName_en, FEE_SID_RB_WRITE_PERSISTENT_DATA, &xJob_st);
        }
        else
        {
            /* Deny request if a persistent (or debug) data write is already ongoing */
            Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_WRITE_PERSISTENT_DATA, FEE_E_BUSY_INTERNAL);
            stRet_u8 = E_NOT_OK;
        }
    }
    else
    {
        /* Deny request if given instance doesn't have persistent data/a saved zone */
        Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_WRITE_PERSISTENT_DATA, FEE_E_INVALID_DEVICE_NAME);
        stRet_u8 = E_NOT_OK;
    }

    return(stRet_u8);
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* #if(defined(FEE_PRV_CFG_RB_PERSIST_DATA) && (TRUE == FEE_PRV_CFG_RB_PERSIST_DATA)) */
# endif

/* #if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED)) */
#endif
