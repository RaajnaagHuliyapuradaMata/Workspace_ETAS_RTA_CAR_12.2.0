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

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"


/* Disable the Fee common part when not needed */
#if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

/* Disable this unit when not needed */
# if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS))

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Crc.h"
#include "Fee_Cfg_SchM.h"
#include "Fee_Prv_Chunk.h"
#include "Fee_Prv_ChunkTypes.h"
#include "Fee_Prv_Config.h"
#include "Fee_Prv_ConfigTypes.h"
#include "Fee_Prv_FsIf.h"
#include "Fee_Prv_Job.h"
#include "Fee_Prv_JobTypes.h"
#include "Fee_Prv_Lib.h"
#include "Fee_Prv_Order.h"
#include "Fee_Rb_Idx.h"
#include "rba_MemLib.h"
#include "MemIf_Types.h"

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
#include "rba_FeeFs1_Prv_Cfg.h"
#include "rba_FeeFs1_Prv.h"


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

/* Collection of all variables of this unit */
static Fee_Prv_ChunkGetJobResult_tpfct    Fee_Prv_ChunkGetJobResult_pfct;
static Fee_Prv_JobDesc_tst                Fee_Prv_ChunkJob_st;
static Fee_Prv_ChunkInfo_tst              Fee_Prv_ChunkInfo_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

typedef union
{
    Fee_BlockPropertiesType_tst    const * xBlkPptyTbl_pcst;   /* Pointer to block property table in Fee1            */
    Fee_Rb_BlockPropertiesType_tst const * xRbBlkPptyTbl_pcst; /* Pointer to block property table in Fee1x/Fee2/Fee3 */
} Fee_Prv_Chunk_BlkPptyTbl_tun;


/* # if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED)) */
# else

typedef union
{
    Fee_Rb_BlockPropertiesType_tst const * xRbBlkPptyTbl_pcst; /* Pointer to block property table in Fee1x/Fee2/Fee3 */
} Fee_Prv_Chunk_BlkPptyTbl_tun;

/* # if(!defined(RBA_FEEFS1_PRV_CFG_ENABLED) || (FALSE ==  RBA_FEEFS1_PRV_CFG_ENABLED)) */
# endif


/*
 **********************************************************************************************************************
 * Static declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

static uint16    Fee_Prv_ChunkGetBlockLengthByBlockNr (
        Fee_Rb_DeviceName_ten                     deviceName_en,
        uint16                                    nrBlk_u16,
        Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst);

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
static Fee_Prv_ChunkGetJobResult_tpfct    Fee_Prv_ChunkGetJobResultFct (uint16 nrBlk_u16);
# endif

static Std_ReturnType    Fee_Prv_ChunkDetChkGnrl (
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint8                    idApi_u8,
        uint8 const *            dataBuf_pcu8,
        uint32                   nrBytMax_u32,
        uint32                   nrBytMin_u32);

static Std_ReturnType    Fee_Prv_ChunkDetChkFsSpc (
        Fee_Rb_DeviceName_ten                    deviceName_en,
        uint8                                    idApi_u8,
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint32                                   nrBytMax_u32,
        uint32                                   nrTotLen_u32);

static Std_ReturnType Fee_Prv_ChunkDetChkBlkLen (
        Fee_Rb_DeviceName_ten                     deviceName_en,
        uint8                                     idApi_u8,
        uint16                                    nrBlk_u16,
        uint16                                    nrLen_u16,
        Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst);

static Std_ReturnType    Fee_Prv_ChunkPut (
        Fee_Rb_DeviceName_ten          deviceName_en,
        uint8                          idApi_u8,
        Fee_Prv_JobDesc_tst const *    xJob_pcst);


/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/

LOCAL_INLINE Fee_Rb_BlockPropertiesType_tst const *
                       Fee_Prv_ChunkGetBlockPropertiesTable (Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16    Fee_Prv_ChunkGetNrOfBlocks           (Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16    Fee_Prv_ChunkGetDoubleStorageBitmask (Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16    Fee_Prv_ChunkGetSurvivalBitmask      (Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16    Fee_Prv_ChunkGetNoFallbackBitmask    (Fee_Rb_DeviceName_ten deviceName_en);

LOCAL_INLINE Fee_Prv_JobDesc_tst const *    Fee_Prv_ChunkGetActvJobDesc (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst);

LOCAL_INLINE void      Fee_Prv_ChunkUpdateChunkStatus (
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        Fee_Rb_JobResultType_ten                 result_en,
        uint32                                   nrBytProc_u32);

LOCAL_INLINE uint16    Fee_Prv_ChunkGetBlockStatusBitsByHdr (
        Fee_Rb_DeviceName_ten           deviceName_en,
        Fee_Prv_ChunkHdr_tst const *    xHdr_pcst);

LOCAL_INLINE boolean    Fee_Prv_ChunkIsHdrVld (Fee_Prv_ChunkHdr_tst const * xHdr_pcst);

LOCAL_INLINE Std_ReturnType     Fee_Prv_ChunkDetChkUkwnBlkWrPrms (
        Fee_Rb_DeviceName_ten           deviceName_en,
        uint8                           idApi_u8,
        Fee_Prv_ChunkHdr_tst const *    xHdr_pcst);

LOCAL_INLINE boolean    Fee_Prv_ChunkCreatHdr (
        Fee_Rb_DeviceName_ten          deviceName_en,
        Fee_Prv_JobDesc_tst const *    xJob_pcst,
        Fee_Rb_JobResultType_ten       stRes_en,
        Fee_Prv_ChunkHdr_tst *         xHdr_pst);

LOCAL_INLINE Fee_Rb_JobMode_ten    Fee_Prv_ChunkGetJobMode (
        Fee_Rb_DeviceName_ten                    deviceName_en,
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        Fee_Rb_JobType_ten                       idJobType_en);


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/**
 * \brief   Provide a pointer to the block properties table for a given device instance (temporary helper function
 *          extending Fee_Prv_FsIfGetBlockPropertiesTable to FeeFs1 until the latter is removed in near future)
 *
 * \param   deviceName_en   Device instance, for which the block properties table is requested
 *
 * \return  Pointer to block properties table for given device instance
*/
LOCAL_INLINE Fee_Rb_BlockPropertiesType_tst const * Fee_Prv_ChunkGetBlockPropertiesTable(
        Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    Fee_Prv_Chunk_BlkPptyTbl_tun    xBlkPptyTbl_un;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* Fee_Prv_FsIfGetBlockPropertiesTable isn't supported for FeeFs1, so take this table from rba_FeeFs1_Prv.h */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        xBlkPptyTbl_un.xBlkPptyTbl_pcst = &Fee_BlockProperties_st[0];
    }
    else
# endif
    {
        xBlkPptyTbl_un.xRbBlkPptyTbl_pcst = Fee_Prv_FsIfGetBlockPropertiesTable(deviceName_en);
    }

    return(xBlkPptyTbl_un.xRbBlkPptyTbl_pcst);
}


/**
 * \brief   Provide the number of configured blocks for a given device instance (temporary helper function extending
 *          Fee_Prv_FsIfGetNrOfBlocks to FeeFs1 until the latter is removed in near future)
 *
 * \param   deviceName_en   Device instance, for which the number of configured blocks is requested
 *
 * \return  Number of configured blocks for given device instance
*/
LOCAL_INLINE uint16 Fee_Prv_ChunkGetNrOfBlocks(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    uint16    nrOfBlocks_u16;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* Fee_Prv_FsIfGetNrOfBlocks isn't supported for FeeFs1, so take this number from rba_FeeFs1_Prv_Cfg.h */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        nrOfBlocks_u16 = FEE_NUM_BLOCKS;
    }
    else
# endif
    {
        nrOfBlocks_u16 = Fee_Prv_FsIfGetNrOfBlocks(deviceName_en);
    }

    return(nrOfBlocks_u16);
}


/**
 * \brief   Provide the double storage bitmask for a given device instance (temporary helper function extending
 *          Fee_Prv_FsIfGetDoubleStorageBitmask to FeeFs1 until the latter is removed in near future)
 *
 * \param   deviceName_en   Device instance, for which the double storage bitmask is requested
 *
 * \return  Double storage bitmask for given device instance
*/
LOCAL_INLINE uint16 Fee_Prv_ChunkGetDoubleStorageBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    uint16    flgDoubleStorage_u16;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* Fee_Prv_FsIfGetDoubleStorageBitmask isn't supported for FeeFs1, so take this mask from rba_FeeFs1_Prv.h. */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        flgDoubleStorage_u16 = FEE_FLAG_SEC_LEVEL_MSK;
    }
    else
# endif
    {
        flgDoubleStorage_u16 = Fee_Prv_FsIfGetDoubleStorageBitmask(deviceName_en);
    }

    return(flgDoubleStorage_u16);
}


/**
 * \brief   Provide the survival bitmask for a given device instance (temporary helper function extending
 *          Fee_Prv_FsIfGetNoFallbackBitmask to FeeFs1 until the latter is removed in near future)
 *
 * \param   deviceName_en   Device instance, for which the survival bitmask is requested
 *
 * \return  Survival bitmask for given device instance
*/
LOCAL_INLINE uint16 Fee_Prv_ChunkGetSurvivalBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    uint16    flgSurvival_u16;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* Fee_Prv_FsIfGetSurvivalBitmask isn't supported for FeeFs1, so take this mask from rba_FeeFs1_Prv.h. */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        flgSurvival_u16 = FEE_FLAG_SURV_ATTR_MSK;
    }
    else
# endif
    {
        flgSurvival_u16 = Fee_Prv_FsIfGetSurvivalBitmask(deviceName_en);
    }

    return(flgSurvival_u16);
}


/**
 * \brief   Provide the no-fallback bitmask for a given device instance (temporary helper function extending
 *          Fee_Prv_FsIfGetSurvivalBitmask to FeeFs1 until the latter is removed in near future)
 *
 * \param   deviceName_en   Device instance, for which the no-fallback bitmask is requested
 *
 * \return  No-fallback bitmask for given device instance
*/
LOCAL_INLINE uint16 Fee_Prv_ChunkGetNoFallbackBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    uint16    flgNoFallback_u16;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* Fee_Prv_FsIfGetNoFallbackBitmask isn't supported for FeeFs1, so take this mask from rba_FeeFs1_Prv.h. */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        flgNoFallback_u16 = FEE_FLAG_NOFALLBACK_MSK;
    }
    else
# endif
    {
        flgNoFallback_u16 = Fee_Prv_FsIfGetNoFallbackBitmask(deviceName_en);
    }

    return(flgNoFallback_u16);
}


/**
 * \brief   For a given (valid!) chunk header, provide the block's status bits the with positions of the given device
 *          instance's block properties table
 *
 * \param   deviceName_en   Device instance, for which the status bits are requested
 * \param   xHdr_pcst       Pointer to (valid!) chunk header
 *
 * \return  Status bits with positions of block properties table
*/
LOCAL_INLINE uint16 Fee_Prv_ChunkGetBlockStatusBitsByHdr(
        Fee_Rb_DeviceName_ten           deviceName_en,
        Fee_Prv_ChunkHdr_tst const *    xHdr_pcst)
{
    /* Return variable */
    uint16    flgBlkPpty_u16 = 0u;

    /* Convert block status flags from chunk header into corresponding bits of block properties table */
    if(0u != (xHdr_pcst->stFlg_u16 & FEE_PRV_CHUNK_DOUBLESTORAGE_BIT))
    {
        flgBlkPpty_u16 |= Fee_Prv_ChunkGetDoubleStorageBitmask(deviceName_en);
    }

    if(0u != (xHdr_pcst->stFlg_u16 & FEE_PRV_CHUNK_SURVIVAL_BIT))
    {
        flgBlkPpty_u16 |= Fee_Prv_ChunkGetSurvivalBitmask(deviceName_en);
    }

    if(0u != (xHdr_pcst->stFlg_u16 & FEE_PRV_CHUNK_NOFALLBACK_BIT))
    {
        flgBlkPpty_u16 |= Fee_Prv_ChunkGetNoFallbackBitmask(deviceName_en);
    }

    return(flgBlkPpty_u16);
}


/**
 * \brief   Provide the length of a given block for a given device instance (temporary helper function extending
 *          Fee_Prv_ConfigGetBlockLengthByBlockNr to FeeFs1 until the latter is removed in near future)
 *
 * \param   deviceName_en      Device instance, for which the block number and properties table are given
 * \param   nrBlk_u16          Block number, for which the length is requested
 * \param   xBlkPptyTbl_pcst   Pointer to block properties table
 *
 * \return  Length of given block for given device instance
*/
static uint16 Fee_Prv_ChunkGetBlockLengthByBlockNr(
        Fee_Rb_DeviceName_ten                     deviceName_en,
        uint16                                    nrBlk_u16,
        Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst)
{
    /* Return variable */
    uint16    nrLen_u16 = Fee_Prv_ConfigGetBlockLengthByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1_PRV_CFG_ENABLED))
    /* FeeFs1 includes data CRC32 into robust block length, but NvM/rba_MemBkup/... don't => length must be patched! */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        if(Fee_Prv_ConfigIsBlockRobustnessActiveByBlockNr(nrBlk_u16, FEE_FLAG_ROBUST_ATTR_MSK, xBlkPptyTbl_pcst))
        {
            nrLen_u16 -= (uint16)sizeof(uint32);
        }
        else
        {
            /* Keep configured length */
        }
    }
    else
    {
        /* Keep configured length */
    }
# else
    (void)deviceName_en;
# endif

    return(nrLen_u16);
}


/**
 * \brief   Provide the read only pointer to the active job (temporary helper function extending
 *          Fee_Prv_JobGetActvDesc to FeeFs1 until the latter is removed in near future)
 *
 * \param   xCfgDevTbl_pcst  Pointer to the config table, for which the job has to be checked
 *
 * \return  Details of the currently ongoing chunk job
*/
LOCAL_INLINE Fee_Prv_JobDesc_tst const * Fee_Prv_ChunkGetActvJobDesc(
        Fee_Prv_ConfigDeviceTable_tst const * xCfgDevTbl_pcst)
{
    /* Return variable */
    Fee_Prv_JobDesc_tst const * xJob_pcst;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* For rba_FeeFs1, details of the chunk job is present in the static variable of this unit and not in rba_FeeFs1. */
    if(Fee_Rb_DeviceName == xCfgDevTbl_pcst->deviceName_en)
    {
        xJob_pcst = &Fee_Prv_ChunkJob_st;
    }
    else
# endif
    {
        xJob_pcst = Fee_Prv_JobGetActvDesc(xCfgDevTbl_pcst);
    }

    return(xJob_pcst);
}


/**
 * \brief   Function called after the current chunk is finished to update the status and number of bytes done
 *          (temporary helper function extending Fee_Prv_JobUpdateChunkStatus to FeeFs1 until the latter is removed
 *          in near future)
 *
 *
 * \param   xCfgDevTbl_pcst   Pointer to the config table for which the job has to be finished
 * \param   result_en         Result of the job
 * \param   nrBytProc_u32     Number of bytes processed in last chunk (including chunk header size)
*/
LOCAL_INLINE void Fee_Prv_ChunkUpdateChunkStatus(
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        Fee_Rb_JobResultType_ten                 result_en,
        uint32                                   nrBytProc_u32)
{
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* For rba_FeeFs1, the status of the job is maintained in the static variable of this unit */
    if(Fee_Rb_DeviceName == xCfgDevTbl_pcst->deviceName_en)
    {
        SchM_Enter_Fee_Order();

        Fee_Prv_ChunkInfo_st.result_en = result_en;
        Fee_Prv_ChunkInfo_st.nrBytProc_u32 = nrBytProc_u32;
        /* Memorize that chunk is done */
        Fee_Prv_ChunkGetJobResult_pfct = NULL_PTR;

        SchM_Exit_Fee_Order();
    }
    else
# endif
    {
        Fee_Prv_JobUpdateChunkStatus(xCfgDevTbl_pcst, result_en, nrBytProc_u32);
    }

    return;
}


/**
 * \brief   Provide a pointer to the GetJobResult function for a given block (temporary helper for FeeFs1 until the
 *          latter is removed in near future)
 *
 * \param   nrBlk_u16   Block number, for which the GetJobResult function is requested
 *
 * \return  Pointer to Fee_GetJobResult or Fee_Rb_GetAdapterJobResult depending on the given block's user
*/
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
static Fee_Prv_ChunkGetJobResult_tpfct Fee_Prv_ChunkGetJobResultFct(uint16 nrBlk_u16)
{
    /* Return variable */
    Fee_Prv_ChunkGetJobResult_tpfct xGetJobResult_pfct;

    /* Local variables */
    Fee_Prv_Chunk_BlkPptyTbl_tun    xBlkPptyTbl_un;
    Fee_Rb_User_ten                 user_en = FEE_RB_USER_MAX_E;

    /* Block known? */
    if(FEE_PRV_MAX_UINT16 != nrBlk_u16)
    {
        /* Get block's user */
        xBlkPptyTbl_un.xBlkPptyTbl_pcst = &Fee_BlockProperties_st[0];
        user_en = Fee_Prv_ConfigGetBlockUserByBlockNr(nrBlk_u16, xBlkPptyTbl_un.xRbBlkPptyTbl_pcst);
    }
    else
    {
        /* Unknown block, i.e. the request is unknown as well */
        user_en = FEE_RB_USER_MAX_E;
    }

    /* Determine the job result function for the block's user
     * For a better readability, the coding rule CCode_BlockStyle_001 is not followed here on purpose. */
    switch(user_en)
    {
        case FEE_RB_USER_NVM_E     : {xGetJobResult_pfct = &Fee_GetJobResult          ;} break;
        case FEE_RB_USER_ADAPTER_E : {xGetJobResult_pfct = &Fee_Rb_GetAdapterJobResult;} break;
        default                    : {xGetJobResult_pfct = NULL_PTR                   ;} break;
    }

    return(xGetJobResult_pfct);
}
# endif


/**
 * \brief   Check whether all general conditions are met to start or continue a chunk-wise job
 *
 * \param   deviceName_en   Device instance to be checked
 * \param   idApi_u8        API requesting this check (needed for Fee_Prv_LibDetReport in case of an error)
 * \param   dataBuf_pcu8    Buffer location to be checked
 * \param   nrBytMax_u32    Maximum number of bytes in buffer
 * \param   nrBytMin_u32    Minimum number of bytes expected
 *
 * \return  Result of check
 * \retval  E_OK      if all general conditions are met
 * \retval  E_NOT_OK  if any general condition is not met
*/
static Std_ReturnType Fee_Prv_ChunkDetChkGnrl(
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint8                    idApi_u8,
        uint8 const *            dataBuf_pcu8,
        uint32                   nrBytMax_u32,
        uint32                   nrBytMin_u32)
{
    /* Return variable */
    Std_ReturnType          stRet_u8;
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    uint8                   secondInstanceDeviceIdx_u8;
    Fee_Rb_DeviceName_ten   secondInstanceDeviceName_en;
# endif

    /* Device name okay? */
    stRet_u8 = Fee_Prv_OrderDetCheckDeviceName(deviceName_en, idApi_u8);
    if(E_OK == stRet_u8)
    {
        /* Fee not yet initialized or in stop mode? */
        if(E_OK != Fee_Prv_OrderDetCheckModuleInitAndStopMode(deviceName_en, idApi_u8))
        {
            stRet_u8 = E_NOT_OK;
        }
        else
        {
            /* When Fee1.0 is used, the static variables of Chunk unit should also be initialized before placing the
             * request. The variable is initialized during Fee_Init of the second instance. So before accepting any
             * request for Fee1.0, check if the second instance is initialized.
             * Without this change, all chunk jobs in Fee1.0 are rejected, if Fee_Init() is called more then once
             * during a driving cycle (CT tests of rba_MemBckUp fails without this change). */
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
            if(Fee_Rb_DeviceName == deviceName_en)
            {
                /* Order is for Fee1.0, check if the second instance is also initialized before accepting the request.
                 * To get the device index and device name for second instance the information is directly taken from
                 * the second instance of the device config table. This is ok because of the following reasons:
                 * 1. So far maximum of two instances are supported and
                 * 2. When multi instance is enabled with Fee1.0, the first instance is always Fee1.0 and second
                 * instance is always != Fee1.0 */
                secondInstanceDeviceIdx_u8 = Fee_Prv_ConfigDeviceTable_cast[1u].deviceIdx_u8;
                secondInstanceDeviceName_en = Fee_Rb_GetDeviceNameFromDeviceIndex(secondInstanceDeviceIdx_u8);

                /* Deviation: Not calling Fee_Prv_OrderDetCheckModuleInitAndStopMode() here. */
                /* Reason: It could be possible to continue to work with chunk operations for Fee1.0 even when other
                 * instance is put the stop mode. Important is that second instance has to be initialized correctly. */
                if(MEMIF_UNINIT == Fee_Rb_Idx_GetStatus(secondInstanceDeviceName_en))
                {
                    Fee_Prv_LibDetReport(secondInstanceDeviceName_en, idApi_u8, FEE_E_UNINIT);
                    stRet_u8 = E_NOT_OK;
                }
            }
# endif
        }

        /* Provided buffer invalid? */
        if(E_OK != Fee_Prv_OrderDetCheckAdrPtr(deviceName_en, idApi_u8, (void const *)dataBuf_pcu8))
        {
            stRet_u8 = E_NOT_OK;
        }
        else
        {
            /* Keep stRet_u8 as is */
        }

        /* Data doesn't fit into chunk/provided buffer? */
        if(nrBytMin_u32 > nrBytMax_u32)
        {
            Fee_Prv_LibDetReport(deviceName_en, idApi_u8, FEE_E_INVALID_BUF_LEN);
            stRet_u8 = E_NOT_OK;
        }
        else
        {
            /* Keep stRet_u8 as is */
        }
    }
    else
    {
        /* Keep stRet_u8 = E_NOT_OK */
    }

    return(stRet_u8);
}


/**
 * \brief   Check whether all file system specific conditions are met to start a chunk-wise job
 *
 * \param   deviceName_en     Device instance, for which conditions are to be checked
 * \param   idApi_u8          API requesting this check (needed for Fee_Prv_LibDetReport in case of an error)
 * \param   xCfgDevTbl_pcst   Pointer to config table (not used for FeeFs1)
 * \param   nrBytMax_u32      Maximum number of bytes in buffer
 * \param   nrTotLen_u32      Total data length (counted over all chunks)
 *
 * \return  Result of check
 * \retval  E_OK      if all file system specific conditions are met
 * \retval  E_NOT_OK  if any file system specific condition is not met
*/
static Std_ReturnType Fee_Prv_ChunkDetChkFsSpc(
        Fee_Rb_DeviceName_ten                    deviceName_en,
        uint8                                    idApi_u8,
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        uint32                                   nrBytMax_u32,
        uint32                                   nrTotLen_u32)
{
    /* Return variable */
    Std_ReturnType    stRet_u8 = E_OK;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1_PRV_CFG_ENABLED))
    /* FeeFs1 supports neither read nor write jobs that are separated into multiple chunks,
     * i.e. the complete payload + chunk header must fit into the buffer! */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        if(nrTotLen_u32 > nrBytMax_u32)
        {
            Fee_Prv_LibDetReport(deviceName_en, idApi_u8, FEE_E_INVALID_BUF_LEN);
            stRet_u8 = E_NOT_OK;
        }
        else
        {
            /* Keep stRet_u8 as is */
        }
    }
    else
# else
    (void)nrBytMax_u32;
    (void)nrTotLen_u32;
# endif
    {
        /* Waiting to read/write next chunk? */
        if(Fee_Prv_JobIsWaitingForNextChunk(xCfgDevTbl_pcst))
        {
            /* Deny request in case we are waiting for next chunk */
            Fee_Prv_LibDetReport(deviceName_en, idApi_u8, FEE_E_INVALID_SEQUENCE);
            stRet_u8 = E_NOT_OK;
        }
        else
        {
            /* Keep stRet_u8 = E_OK */
        }
    }

    return(stRet_u8);
}


/**
 * \brief   Check whether requested and configured length are matching
 *
 * \param   deviceName_en      Device instance, for which the length is to be checked
 * \param   idApi_u8           API requesting this check (needed for Fee_Prv_LibDetReport in case of an error)
 * \param   nrBlk_u16          Block number, for which the length is to be checked
 * \param   nrLen_u16          Requested block length
 * \param   xBlkPptyTbl_pcst   Pointer to block properties table
 *
 * \return  Result of check
 * \retval  E_OK      if requested and configured block length are matching
 * \retval  E_NOT_OK  if requested and configured block length aren't matching
*/
static Std_ReturnType Fee_Prv_ChunkDetChkBlkLen(
        Fee_Rb_DeviceName_ten                     deviceName_en,
        uint8                                     idApi_u8,
        uint16                                    nrBlk_u16,
        uint16                                    nrLen_u16,
        Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst)
{
    /* Return variable */
    Std_ReturnType    stRet_u8 = E_OK;

    /* Deny request in case of a length mismatch */
    if(Fee_Prv_ChunkGetBlockLengthByBlockNr(deviceName_en, nrBlk_u16, xBlkPptyTbl_pcst) != nrLen_u16)
    {
        Fee_Prv_LibDetReport(deviceName_en, idApi_u8, FEE_E_INVALID_BLOCK_LEN);
        stRet_u8 = E_NOT_OK;
    }
    else
    {
        /* Keep stRet_u8 = E_OK */
    }

    return(stRet_u8);
}


/**
 * \brief   Check whether a given chunk header is valid
 *
 * \param   xHdr_pcst   Pointer to header to be checked
 *
 * \return  Result of check
 * \retval  TRUE   if header is valid
 * \retval  FALSE  if header is invalid
*/
LOCAL_INLINE boolean Fee_Prv_ChunkIsHdrVld(Fee_Prv_ChunkHdr_tst const * xHdr_pcst)
{
    /* Calculate CRC over chunk header */
    uint16    xCrc_u16 = Crc_CalculateCRC16(
                            (uint8 const *)xHdr_pcst,
                            sizeof(Fee_Prv_ChunkHdr_tst) - sizeof(xHdr_pcst->xCrc_u16),
                            CRC_INITIAL_VALUE16,
                            TRUE );

    /* The chunk header is valid, if its CRC is correct */
    return(xHdr_pcst->xCrc_u16 == xCrc_u16);
}


/**
 * \brief   For a given (valid!) chunk header of an unknown block, check the permission to write it
 *
 * \param   deviceName_en   Device instance, for which the permission is to be checked
 * \param   idApi_u8        API requesting this check (needed for Fee_Prv_LibDetReport in case of an error)
 * \param   xHdr_pcst       Pointer to (valid!) header of unknown block to be checked
 *
 * \return  Result of check
 * \retval  E_OK      if permission is granted
 * \retval  E_NOT_OK  if permission is not granted
 *
*/
LOCAL_INLINE Std_ReturnType Fee_Prv_ChunkDetChkUkwnBlkWrPrms(
        Fee_Rb_DeviceName_ten           deviceName_en,
        uint8                           idApi_u8,
        Fee_Prv_ChunkHdr_tst const *    xHdr_pcst)
{
    /* Return variable */
    Std_ReturnType    stRet_u8 = E_OK;

    /* Local variables */
    boolean    isUkwnBlkWrAllwd_b         = Fee_Prv_FsIfIsUnknownBlockWriteAllowed(deviceName_en);
    boolean    isUkwnSurvivalBlkWrAllwd_b = Fee_Prv_FsIfIsUnknownSurvivalBlockWriteAllowed(deviceName_en);
    boolean    isSurvivalBlk_b            = (0u != (xHdr_pcst->stFlg_u16 & FEE_PRV_CHUNK_SURVIVAL_BIT));

    /* Deny request in case the unkown block write is deactivated or not supported */
    if( !(isUkwnBlkWrAllwd_b || (isUkwnSurvivalBlkWrAllwd_b && isSurvivalBlk_b)) )
    {
        Fee_Prv_LibDetReport(deviceName_en, idApi_u8, FEE_E_INVALID_BLOCK_NO);
        stRet_u8 = E_NOT_OK;
    }
    else
    {
        /* Keep stRet_u8 = E_OK */
    }

    return(stRet_u8);
}


/**
 * \brief   Create chunk header for a given job result and device instance
 *
 * \param   deviceName_en   Device instance, for which chunk header is requested
 * \param   xJob_pcst       Pointer to job description
 * \param   stRes_en        Job result
 * \param   xHdr_pst        Pointer to location, where chunk header shall be created
 *
 * \return  Payload status of chunk-wise read
 * \retval  TRUE   if header has been created for a chunk-wise read containing payload
 * \retval  FALSE  if header has been created for a chunk-wise read containing no payload
*/
LOCAL_INLINE boolean Fee_Prv_ChunkCreatHdr(
        Fee_Rb_DeviceName_ten          deviceName_en,
        Fee_Prv_JobDesc_tst const *    xJob_pcst,
        Fee_Rb_JobResultType_ten       stRes_en,
        Fee_Prv_ChunkHdr_tst *         xHdr_pst)
{
    /* Return variable */
    boolean    hasData_b = TRUE;
    /* Local variables */
    Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst = Fee_Prv_ChunkGetBlockPropertiesTable(deviceName_en);

    /* Fill chunk header with block properties */
    xHdr_pst->idPers_u16  = Fee_Prv_ConfigGetBlockPersistentIdByBlockNr(
                                xJob_pcst->blockNumber_u16,
                                xBlkPptyTbl_pcst );
    xHdr_pst->nrLen_u16   = Fee_Prv_ChunkGetBlockLengthByBlockNr(
                                deviceName_en,
                                xJob_pcst->blockNumber_u16,
                                xBlkPptyTbl_pcst );
    xHdr_pst->stFlg_u16    = 0u;
    xHdr_pst->xResv_au8[0] = 0u;
    xHdr_pst->xResv_au8[1] = 0u;

    /* Set double storage bit in chunk header's status byte depending on block property */
    if(Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr(
            xJob_pcst->blockNumber_u16,
            Fee_Prv_ChunkGetDoubleStorageBitmask(deviceName_en),
            xBlkPptyTbl_pcst) )
    {
        xHdr_pst->stFlg_u16 |= FEE_PRV_CHUNK_DOUBLESTORAGE_BIT;
    }
    else
    {
        /* Keep FEE_PRV_CHUNK_DOUBLESTORAGE_BIT cleared */
    }

    /* Set survival bit in chunk header's status byte depending on block property */
    if(Fee_Prv_ConfigIsBlockSurvivalActiveByBlockNr(
            xJob_pcst->blockNumber_u16,
            Fee_Prv_ChunkGetSurvivalBitmask(deviceName_en),
            xBlkPptyTbl_pcst) )
    {
        xHdr_pst->stFlg_u16 |= FEE_PRV_CHUNK_SURVIVAL_BIT;
    }
    else
    {
        /* Keep FEE_PRV_CHUNK_SURVIVAL_BIT cleared */
    }

    /* Set no-fallback bit in chunk header's status byte depending on block property */
    if(Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr(
            xJob_pcst->blockNumber_u16,
            Fee_Prv_ChunkGetNoFallbackBitmask(deviceName_en),
            xBlkPptyTbl_pcst) )
    {
        xHdr_pst->stFlg_u16 |= FEE_PRV_CHUNK_NOFALLBACK_BIT;
    }
    else
    {
        /* Keep FEE_PRV_CHUNK_NOFALLBACK_BIT cleared */
    }

    /* Set invalidate or missing bit in chunk header's status byte depending on block status */
    switch(stRes_en)
    {
        case FEE_RB_BLOCK_INVALID:
        {
            xHdr_pst->stFlg_u16 |= FEE_PRV_CHUNK_INVALIDATE_BIT;
            hasData_b = FALSE;
        }
        break;

        case FEE_RB_BLOCK_INCONSISTENT:
        {
            xHdr_pst->stFlg_u16 |= FEE_PRV_CHUNK_BLOCK_NOT_PRESENT_BIT;
            hasData_b = FALSE;
        } break;

        default:
        {
            /* Keep both status bits cleared, and hasData_b = TRUE */
        } break;
    }

    /* Append CRC behind chunk header */
    xHdr_pst->xCrc_u16 = Crc_CalculateCRC16(
                            (uint8 const *)xHdr_pst,
                            sizeof(Fee_Prv_ChunkHdr_tst) - sizeof(xHdr_pst->xCrc_u16),
                            CRC_INITIAL_VALUE16,
                            TRUE );

    return(hasData_b);
}


/**
 * \brief   Provide the job mode for a given job type and device instance (temporary helper function extending
 *          Fee_Prv_JobGetJobMode to FeeFs1 until the latter is removed in near future)
 *
 * \param   deviceName_en     Device instance, for which config table is provided
 * \param   xCfgDevTbl_pcst   Pointer to config table
 * \param   idJobType_en      Job type
 *
 * \return  Job mode for given job type and device instance
*/
LOCAL_INLINE Fee_Rb_JobMode_ten Fee_Prv_ChunkGetJobMode(
        Fee_Rb_DeviceName_ten                    deviceName_en,
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        Fee_Rb_JobType_ten                       idJobType_en)
{
    /* Return variable */
    Fee_Rb_JobMode_ten    idJobMode_en;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
    /* Fee_Prv_JobGetJobMode isn't supported for FeeFs1, value actually doesn't matter */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        idJobMode_en = FEE_RB_ALLJOBS_ALLSTEPS_E;
    }
    else
# else
    (void)deviceName_en;
# endif
    {
        idJobMode_en = Fee_Prv_JobGetJobMode(xCfgDevTbl_pcst, idJobType_en);
    }

    return(idJobMode_en);
}


/**
 * \brief   Request to process a job (temporary helper function extending Fee_Prv_JobPut to FeeFs1 until the latter
 *          is removed in near future)
 *
 * \param   deviceName_en   Device instance, for which job is requested
 * \param   idApi_u8        API requesting the job (needed for Fee_Prv_LibDetReport in case of an error)
 * \param   xJob_pcst       Pointer to job description
 *
 * \return  Request acceptance
 * \retval  E_OK      The request has been accepted, and the job processing is started
 * \retval  E_NOT_OK  The request has been erroneous and therefore been rejected
*/
static Std_ReturnType Fee_Prv_ChunkPut(
        Fee_Rb_DeviceName_ten          deviceName_en,
        uint8                          idApi_u8,
        Fee_Prv_JobDesc_tst const *    xJob_pcst)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))

    /* Local variables */
    Fee_Prv_ChunkGetJobResult_tpfct    xGetJobResult_pfct;
    Fee_Prv_LibBufferU8_tun            dataBuf_un;
    MemIf_JobResultType                stMemIfJobRes_en;
    Fee_Rb_JobResultType_ten           stFeeRbJobRes_en;

    /* In FeeFs1, request the read/write/invalidate directly via its public interfaces */
    if(Fee_Rb_DeviceName == deviceName_en)
    {
        /* Slot available for job? */
        xGetJobResult_pfct = Fee_Prv_ChunkGetJobResultFct(xJob_pcst->blockNumber_u16);
        stMemIfJobRes_en = xGetJobResult_pfct();
        stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en);

        if(    (FEE_RB_JOB_PENDING != stFeeRbJobRes_en)
            && (FEE_RB_JOB_PENDING != Fee_Prv_ChunkInfo_st.result_en) )
        {
            switch(xJob_pcst->type_en)
            {
                case FEE_RB_JOBTYPE_READ_E:
                {
                    stRet_u8 = Fee_Read(
                            xJob_pcst->blockNumber_u16,
                            xJob_pcst->offset_u16,
                            xJob_pcst->bfr_pu8,
                            xJob_pcst->length_u16);
                }
                break;

                case FEE_RB_JOBTYPE_WRITE_E:
                {
                    /* Avoid compiler warning "Passing argument 3 of 'Fee_Rb_Idx_Write'
                     * discards * 'const' qualifier from pointer target type. */
                    dataBuf_un.dataCon_pcu8 = xJob_pcst->bfr_pcu8;

                    stRet_u8 = Fee_Write(
                            xJob_pcst->blockNumber_u16,
                            dataBuf_un.dataChg_pu8);
                }
                break;

                case FEE_RB_JOBTYPE_INVALIDATE_E:
                {
                    stRet_u8 = Fee_InvalidateBlock(xJob_pcst->blockNumber_u16);
                }
                break;

                default:
                {
                    stRet_u8 = E_NOT_OK;
                }
                break;
            }
        }
        else
        {
            /* Deny request; job slot is occupied */
            Fee_Prv_LibDetReport(deviceName_en, idApi_u8, FEE_E_BUSY_INTERNAL);
            stRet_u8 = E_NOT_OK;
        }

        /* Request has been accepted? */
        if(E_OK == stRet_u8)
        {
            SchM_Enter_Fee_Order();

            /* Memorize GetJobResult function (this also indicates that a chunk is under processing) */
            Fee_Prv_ChunkGetJobResult_pfct = xGetJobResult_pfct;

            /* Reset chunk-wise job status */
            Fee_Prv_ChunkInfo_st.nrBytProc_u32 = 0u;
            Fee_Prv_ChunkInfo_st.result_en     = FEE_RB_JOB_PENDING;

            SchM_Exit_Fee_Order();

            /* Memorize job details */
            Fee_Prv_ChunkJob_st = *xJob_pcst;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
# endif
    {
        /* Start chunk-wise job */
        stRet_u8 = Fee_Prv_JobPut(deviceName_en, idApi_u8, xJob_pcst);
    }

    return(stRet_u8);
}


/**
 * \brief   Callback function to be called upon completion or termination of every processed chunk. It determines
 *          and provides the chunk status (i.e. result as well as number of processed bytes) in the given pointer.
 *          For the first chunk-wise read, it additionally provides the chunk header in the given job's buffer.
 *
 * \param   xCfgDevTbl_pcst  Pointer to the config table, for which the job has to be checked
 * \param   stRes_en         Job result
*/
void Fee_Prv_ChunkDoneCbk(
        Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst,
        Fee_Rb_JobResultType_ten                 stRes_en)
{
    /* Local variables */
    boolean                     isFirstChunk_b;
    boolean                     hasData_b = TRUE;
    Fee_Rb_JobResultType_ten    result_en;           /* Result of last/current chunk(-wise job) */
    Fee_Prv_ChunkHdr_tst        xHdr_st;
    uint8 *                     xHdr_pu8;
    uint32                      nrBytProc_u32 = 0u;  /* Assume chunk without payload */
    Fee_Prv_JobDesc_tst const * xJob_pcst = Fee_Prv_ChunkGetActvJobDesc(xCfgDevTbl_pcst);

    /* Fill chunk header and update chunk status */
    if(FEE_RB_JOB_FAILED == stRes_en)
    {
        /* Update chunk result for failed chunk(-wise job) */
        result_en = FEE_RB_JOB_FAILED;
    }
    else if(FEE_RB_JOB_CANCELED == stRes_en)
    {
        /* Update chunk result for terminated chunk(-wise job) */
        result_en = FEE_RB_JOB_CANCELED;
    }
    else
    {
        /* First chunk, or inconsistent chunk-wise read? */
        isFirstChunk_b = Fee_Prv_JobIsFirstPart(xJob_pcst);
        if( isFirstChunk_b || (FEE_RB_BLOCK_INCONSISTENT == stRes_en) )
        {
            /* Chunk-wise read? */
            if(FEE_RB_JOBTYPE_READ_E == xJob_pcst->type_en)
            {
                /* Create chunk header in buffer, which the job is working on */
                hasData_b = Fee_Prv_ChunkCreatHdr(xCfgDevTbl_pcst->deviceName_en, xJob_pcst, stRes_en, &xHdr_st);

                /* The user buffer always points to payload. For the first chunk, we have already reserved space for
                 * the header at the beginning of the buffer, i.e. we need to adjust the position where to copy it. */
                xHdr_pu8  = isFirstChunk_b  ?  (xJob_pcst->bfr_pu8 - sizeof(Fee_Prv_ChunkHdr_tst))
                                            :   xJob_pcst->bfr_pu8;

                /* Copy chunk header from stack to user buffer (the latter might not be aligned on a 16-bit boundary) */
                rba_MemLib_MemCopy((uint8 const *)&xHdr_st, xHdr_pu8, sizeof(Fee_Prv_ChunkHdr_tst));
            }
            else if(FEE_RB_JOBTYPE_INVALIDATE_E == xJob_pcst->type_en)
            {
                /* Block invalidation => chunk without payload */
                hasData_b = FALSE;
            }
            else
            {
                /* First chunk of chunk-wise write; keep hasData_b = TRUE */
            }

            /* Consider chunk header in number of processed bytes for first chunk */
            nrBytProc_u32 = sizeof(Fee_Prv_ChunkHdr_tst);
        }
        else
        {
            /* For subsequent chunks, keep stChunk_pst->nrBytProc_u32 = 0u and hasData_b = TRUE */
        }

        /* Chunk with payload? */
        if(hasData_b)
        {
            /* Consider payload in number of processed bytes */
            nrBytProc_u32 += xJob_pcst->length_u16;

            /* All payload processed? */
            if(Fee_Prv_JobIsLastPart(xJob_pcst))
            {
                /* We are completely done! */
                result_en = FEE_RB_JOB_OK;
            }
            else
            {
                /* We need to wait for the next chunk */
                result_en = FEE_RB_BLOCK_PART_OK;
            }
        }
        else
        {
            /* First chunk? */
            if(isFirstChunk_b)
            {
                /* Chunk without payload; we are completely done! */
                result_en = FEE_RB_JOB_OK;
            }
            else
            {
                /* Chunk-wise read has detected inconsistent data for any but the first chunk */
                result_en = FEE_RB_BLOCK_INCONSISTENT;
            }
        }
    }

    Fee_Prv_ChunkUpdateChunkStatus(xCfgDevTbl_pcst, result_en, nrBytProc_u32);

    return;
}


/**
 * \brief   Function called during Fee_Init(). It initializes all variables of this unit.
 *  The below implementation is needed only when Fee1 is used and the static variables are needed.
 *  When Fee1.0 is not used, there are no static variables in Chunk unit and so the function could be dummy
 *  (dummy function implemented in header file Fee_Prv_Chunk.h).
 *
 * \param   none
 *
 * \return  none
*/
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
void Fee_Prv_ChunkInit(void)
{
    Fee_Prv_ChunkGetJobResult_pfct     = NULL_PTR;

    Fee_Prv_ChunkJob_st.type_en        = FEE_RB_JOBTYPE_MAX_E;

    Fee_Prv_ChunkInfo_st.nrBytProc_u32 = 0u;
    Fee_Prv_ChunkInfo_st.result_en     = FEE_RB_JOB_OK;

    return;
}
# endif

/**
 * \brief   Request to start a chunk-wise read job with the first chunk
 *
 * \param   deviceName_en  Device instance, for which job is requested
 * \param   idPers_u16     Block's persistent ID (unique block identifier)
 * \param   nrBlkLen_u16   Block's data length (in bytes, excluding chunk header)
 * \param   dataBuf_pu8    Buffer location, where chunk data is copied into
 * \param   nrBytMax_u32   Maximum number of bytes, which can be copied into buffer
 *
 * \return  Request acceptance
 * \retval  E_OK      The request has been accepted, and the job processing is started
 * \retval  E_NOT_OK  The request has been erroneous and therefore been rejected
 *
 * \attention  This request is not accepted duing FEE_RB_JOB_PENDING or FEE_RB_BLOCK_PART_OK.
 *             The first chunk will start with the chunk header, followed by the block data. The maximum number of
 *             bytes, which can be copied into buffer, shall be big enough to hold the chunk header plus at least
 *             one byte of block data.
 *             After processing this request successfully, it is mandatory to query the number of processed bytes with
 *             Fee_Rb_Idx_ChunkGetNrBytProc! Background: If the block is invalidated or missing, the chunk-wise read
 *             will only provide the chunk header but no block data.
*/
Std_ReturnType Fee_Rb_Idx_ChunkReadFirst(
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint16                   idPers_u16,
        uint16                   nrBlkLen_u16,
        uint8 *                  dataBuf_pu8,
        uint32                   nrBytMax_u32)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

    /* Local variables */
    uint32                                    nrBytMin_u32;
    Fee_Prv_ConfigDeviceTable_tst const *     xCfgDevTbl_pcst;
    Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst;
    uint16                                    nrOfBlks_u16, nrBlk_u16;
    Fee_Prv_JobDesc_tst                       xJob_st;

    /* Determine the minimum size of the provided buffer
     * Note: For Fee1.0, Fee_Prv_FsIfGetNrOfMinBytesInFirstChunk returns 0, i.e. the request is only denied here if
     * the chunk header doesn't fit into the provided buffer. But a read request, which is separated into multiple
     * chunks, will be rejected by Fee_Prv_ChunkDetChkFsSpc. */
    nrBytMin_u32 = Fee_Prv_FsIfGetMinPayloadInFirstChunk(deviceName_en, FEE_RB_JOBTYPE_READ_E);
    nrBytMin_u32 = rba_MemLib_Min(nrBlkLen_u16, nrBytMin_u32) + sizeof(Fee_Prv_ChunkHdr_tst);

    /* Check request in general */
    stRet_u8 = Fee_Prv_ChunkDetChkGnrl(
                deviceName_en,
                FEE_SID_RB_CHUNK_READ_FIRST,
                dataBuf_pu8,
                nrBytMax_u32,
                nrBytMin_u32 );
    if(E_OK == stRet_u8)
    {
        /* Get config table */
        xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

        /* Convert persistent ID into block number */
        nrOfBlks_u16     = Fee_Prv_ChunkGetNrOfBlocks(deviceName_en);
        xBlkPptyTbl_pcst = Fee_Prv_ChunkGetBlockPropertiesTable(deviceName_en);
        nrBlk_u16        = Fee_Prv_ConfigGetBlockNrByPersistentId(idPers_u16, nrOfBlks_u16, xBlkPptyTbl_pcst);

        /* Block unknown? */
        if(FEE_PRV_MAX_UINT16 == nrBlk_u16)
        {
            /* Deny request; the chunk-wise read doesn't support unknown blocks (in none of the file systems) */
            Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_READ_FIRST, FEE_E_INVALID_BLOCK_NO);
            stRet_u8 = E_NOT_OK;
        }
        else
        {
            /* Check requested against configured block length */
            stRet_u8 = Fee_Prv_ChunkDetChkBlkLen(
                        deviceName_en,
                        FEE_SID_RB_CHUNK_READ_FIRST,
                        nrBlk_u16,
                        nrBlkLen_u16,
                        xBlkPptyTbl_pcst );
        }

        /* Check request for file system specific conditions */
        if(E_OK != Fee_Prv_ChunkDetChkFsSpc(
                    deviceName_en,
                    FEE_SID_RB_CHUNK_READ_FIRST,
                    xCfgDevTbl_pcst,
                    nrBytMax_u32,
                    sizeof(Fee_Prv_ChunkHdr_tst)+(uint32)nrBlkLen_u16) )
        {
            /* Deny request, DET entry has been done by Fee_Prv_ChunkDetChkFsSpc */
            stRet_u8 = E_NOT_OK;
        }
        else
        {
            /* Keep stRet_u8 as is */
        }
    }
    else
    {
        /* Deny request and keep stRet_u8 = E_NOT_OK, DET entry done by Fee_Prv_ChunkDetChkGnrl */
    }

    /* Request valid? */
    if(E_OK == stRet_u8)
    {
        /* Setup descriptor for chunk-wise job */
        xJob_st.type_en         = FEE_RB_JOBTYPE_READ_E;
        xJob_st.jobMode_en      = Fee_Prv_ChunkGetJobMode(deviceName_en, xCfgDevTbl_pcst, xJob_st.type_en);
        xJob_st.bfr_pu8         = dataBuf_pu8 + sizeof(Fee_Prv_ChunkHdr_tst);
        xJob_st.bfr_pcu8        = NULL_PTR;
        xJob_st.blockNumber_u16 = nrBlk_u16;
        xJob_st.length_u16      = (uint16)rba_MemLib_Min(nrBlkLen_u16, nrBytMax_u32 - sizeof(Fee_Prv_ChunkHdr_tst));
        xJob_st.offset_u16      = 0u;
        xJob_st.nrBytTot_u16    = nrBlkLen_u16;
        xJob_st.bytDone_un.cntrPayloadBytDone_u32 = 0u;
        xJob_st.idPers_u16      = idPers_u16;
        xJob_st.statusFlag_u16  = Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);
        xJob_st.isChunkJob_b    = TRUE;
        xJob_st.isUnknownBlk_b  = FALSE; /* Currently only known block read operation is supported */

        /* Start chunk-wise job */
        stRet_u8 = Fee_Prv_ChunkPut(deviceName_en, FEE_SID_RB_CHUNK_READ_FIRST, &xJob_st);
    }
    else
    {
        /* Keep stRet_u8 = E_NOT_OK */
    }

    return(stRet_u8);
}


/**
 * \brief   Request to start a chunk-wise write job with the first chunk
 *
 * \param   deviceName_en   Device instance, for which job is requested
 * \param   nrTotLen_u32    Total length incl. chunk header and payload (counted over all chunks)
 * \param   dataBuf_pcu8    Buffer location, from where chunk data is copied
 * \param   nrBytMax_u32    Maximum number of bytes, which can be copied from buffer
 *
 * \return  Request acceptance
 * \retval  E_OK      The request has been accepted, and the job processing is started
 * \retval  E_NOT_OK  The request has been erroneous and therefore been rejected
 *
 * \attention  This request is not accepted duing FEE_RB_JOB_PENDING or FEE_RB_BLOCK_PART_OK.
 *             The first chunk must start with the chunk header, followed by the block data.
 *             For an invalidated or missing block, there shall be no payload, i.e. the total length must be equal to
 *             the chunk header size (the request is rejected otherwise). For a present known block, the total length
 *             must be equal to the configured block's data length plus the chunk header size (the request is rejected
 *             otherwise).
 *             Depending on the file system, some minimum amount of block data must be provided behind the chunk header
 *             unless the block is missing, is invalidated or completely fits into the first chunk. I.e. the maximum
 *             number of bytes, which can be copied from buffer for the first of multiple chunks, should be big enough
 *             (e.g. at least one page) to avoid that the request gets rejected.
 *             After processing this request successfully, it is mandatory to query the number of processed bytes with
 *             Fee_Rb_Idx_ChunkGetNrBytProc before continuing with the next chunk! Background: Depending on page
 *             alignment, the chunk-wise write will not necessarily process the given buffer contents completely. The
 *             user is responsible to provide the unprocessed data (from the chunk's end) at the beginning of the next
 *             chunk again!
*/
Std_ReturnType Fee_Rb_Idx_ChunkWriteFirst(
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint32                   nrTotLen_u32,
        uint8 const *            dataBuf_pcu8,
        uint32                   nrBytMax_u32)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *     xCfgDevTbl_pcst;
    Fee_Prv_ChunkHdr_tst                      xHdr_st = {0, 0, 0, {0, 0}, 0};
    Fee_Rb_BlockPropertiesType_tst const *    xBlkPptyTbl_pcst;
    uint16                                    nrOfBlks_u16, nrBlk_u16, flgBlkPpty_u16;
    uint32                                    nrLen_u32, nrBytPayload_u32, nrBytMin_u32;
    boolean                                   isUnknownBlk_b;
    uint8                                     nrPageSize_u8;
    Fee_Prv_JobDesc_tst                       xJob_st;

    /* Check request in general
     * Note: The minimum number of bytes we expect here is the chunk header size (for invalidated/missing blocks).
     * For other blocks, additional checks can only be done after the chunk header validity has been confirmed. */
    stRet_u8 = Fee_Prv_ChunkDetChkGnrl(
                deviceName_en,
                FEE_SID_RB_CHUNK_WRITE_FIRST,
                dataBuf_pcu8,
                rba_MemLib_Min(nrBytMax_u32, nrTotLen_u32),
                sizeof(Fee_Prv_ChunkHdr_tst) );
    if(E_OK == stRet_u8)
    {
        /* Determine available payload bytes of incoming first chunk */
        nrBytPayload_u32 = nrBytMax_u32 - sizeof(Fee_Prv_ChunkHdr_tst);

        /* Chunk header valid? Since the chunk header inside the user buffer might
         * not be aligned on a 16-bit boundary, we have to copy it onto stack. */
        rba_MemLib_MemCopy(dataBuf_pcu8, (uint8 *)&xHdr_st, sizeof(Fee_Prv_ChunkHdr_tst));
        if(Fee_Prv_ChunkIsHdrVld(&xHdr_st))
        {
            /* Get config table */
            xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

            /* Determine block length from request parameters */
            nrLen_u32 = nrTotLen_u32 - sizeof(Fee_Prv_ChunkHdr_tst);

            /* Convert persistent ID into block number */
            nrOfBlks_u16     = Fee_Prv_ChunkGetNrOfBlocks(deviceName_en);
            xBlkPptyTbl_pcst = Fee_Prv_ChunkGetBlockPropertiesTable(deviceName_en);
            nrBlk_u16        = Fee_Prv_ConfigGetBlockNrByPersistentId(
                                xHdr_st.idPers_u16,
                                nrOfBlks_u16,
                                xBlkPptyTbl_pcst );

            /* Unknown block? */
            if(FEE_PRV_MAX_UINT16 == nrBlk_u16)
            {
                isUnknownBlk_b = TRUE;

                /* For unknown blocks, take status bits/property flags from chunk header */
                flgBlkPpty_u16 = Fee_Prv_ChunkGetBlockStatusBitsByHdr(deviceName_en, &xHdr_st);

                /* For unknown blocks, check if writing is enabled/supported */
                stRet_u8 = Fee_Prv_ChunkDetChkUkwnBlkWrPrms(deviceName_en, FEE_SID_RB_CHUNK_WRITE_FIRST, &xHdr_st);
            }
            else
            {
                isUnknownBlk_b = FALSE;

                /* For known blocks, take status bits/property flags from configuration */
                flgBlkPpty_u16 = Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(nrBlk_u16, xBlkPptyTbl_pcst);

                /* For known blocks, check requested against configured block length */
                stRet_u8 = Fee_Prv_ChunkDetChkBlkLen(
                            deviceName_en,
                            FEE_SID_RB_CHUNK_WRITE_FIRST,
                            nrBlk_u16,
                            xHdr_st.nrLen_u16,
                            xBlkPptyTbl_pcst );
            }

            /* Block present and not invalidated? */
            if( 0u == (xHdr_st.stFlg_u16 & (FEE_PRV_CHUNK_INVALIDATE_BIT|FEE_PRV_CHUNK_BLOCK_NOT_PRESENT_BIT)) )
            {
                /* Check requested length against block length inside chunk header */
                if((uint32)xHdr_st.nrLen_u16 != nrLen_u32)
                {
                    /* Deny request in case of a length mismatch */
                    Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_FIRST, FEE_E_INVALID_MNGT_DATA);
                    stRet_u8 = E_NOT_OK;
                }
                else
                {
                    /* Keep stRet_u8 as is */
                }

                /* Check if the first of subsequent chunks provides the minimum payload expected by the file system
                 * Note: For Fee1.0, Fee_Prv_FsIfGetNrOfMinBytesInFirstChunk returns 0, i.e. the request is never
                 * denied here. But a write request, which is separated into multiple chunks, will be rejected by
                 * Fee_Prv_ChunkDetChkFsSpc instead. */
                nrBytMin_u32 = Fee_Prv_FsIfGetMinPayloadInFirstChunk(deviceName_en, FEE_RB_JOBTYPE_WRITE_E);
                if(nrBytPayload_u32 < rba_MemLib_LimHigh(nrLen_u32, nrBytMin_u32, nrBytMin_u32))
                {
                    /* Deny request */
                    Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_FIRST, FEE_E_INVALID_BUF_LEN);
                    stRet_u8 = E_NOT_OK;
                }
                else
                {
                    /* Keep stRet_u8 as is */
                }
            }
            else
            {
                /* Check requested length for an invalidated or missing block */
                if(0u != nrLen_u32)
                {
                    /* Deny request in case of a length mismatch */
                    Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_FIRST, FEE_E_INVALID_MNGT_DATA);
                    stRet_u8 = E_NOT_OK;
                }
                else
                {
                    /* Keep stRet_u8 as is */
                }
            }

            /* Check request for file system specific conditions */
            if(E_OK != Fee_Prv_ChunkDetChkFsSpc(
                        deviceName_en,
                        FEE_SID_RB_CHUNK_WRITE_FIRST,
                        xCfgDevTbl_pcst,
                        nrBytMax_u32,
                        nrTotLen_u32) )
            {
                /* Deny request, DET entry has been done by Fee_Prv_ChunkDetChkFsSpc */
                stRet_u8 = E_NOT_OK;
            }
            else
            {
                /* Keep stRet_u8 as is */
            }
        }
        else
        {
            /* Deny request in case of an invalid header */
            Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_FIRST, FEE_E_INVALID_MNGT_DATA);
            stRet_u8 = E_NOT_OK;
        }
    }
    else
    {
        /* Deny request and keep stRet_u8 = E_NOT_OK, DET entry done by Fee_Prv_ChunkDetChkGnrl */
    }

    /* Request valid? */
    if(E_OK == stRet_u8)
    {
        /* There is some block data to write? */
        if(0u < nrLen_u32)
        {
            /* Write  block */
            xJob_st.type_en = FEE_RB_JOBTYPE_WRITE_E;

            /* Subsequent chunk(s) will follow? */
            if(nrBytMax_u32 < nrTotLen_u32)
            {
                /* Cut off excess data that's not filling up the last page */
                nrPageSize_u8 = Fee_Prv_FsIfGetLogicalPageSize(deviceName_en);
                nrLen_u32 = Fee_Prv_LibFloor(nrBytPayload_u32, nrPageSize_u8);
                if((nrLen_u32 + nrBytMin_u32) <= nrBytPayload_u32)
                {
                    nrLen_u32 += nrBytMin_u32;
                }
                else
                {
                    nrLen_u32 -= (nrPageSize_u8 - nrBytMin_u32);
                }
            }
            else
            {
                /* Keep nrLen_u32 = nrTotLen_u32 - sizeof(Fee_Prv_ChunkHdr_tst) to
                 * write the complete block data if there are no subsequent chunks. */
            }
        }
        else
        {
            /* Invalidate block */
            xJob_st.type_en = FEE_RB_JOBTYPE_INVALIDATE_E;
        }

        /* Setup descriptor for chunk-wise job */
        xJob_st.jobMode_en      = Fee_Prv_ChunkGetJobMode(deviceName_en, xCfgDevTbl_pcst, xJob_st.type_en);
        xJob_st.bfr_pu8         = NULL_PTR;
        xJob_st.bfr_pcu8        = dataBuf_pcu8 + sizeof(Fee_Prv_ChunkHdr_tst);
        xJob_st.blockNumber_u16 = nrBlk_u16;
        xJob_st.length_u16      = (uint16)nrLen_u32;
        xJob_st.offset_u16      = 0u;
        xJob_st.nrBytTot_u16    = xHdr_st.nrLen_u16;
        xJob_st.bytDone_un.cntrPayloadBytDone_u32 = 0u;
        xJob_st.idPers_u16      = xHdr_st.idPers_u16;
        xJob_st.statusFlag_u16  = flgBlkPpty_u16;
        xJob_st.isChunkJob_b    = TRUE;
        xJob_st.isUnknownBlk_b  = isUnknownBlk_b;

        /* Start chunk-wise job */
        stRet_u8 = Fee_Prv_ChunkPut(deviceName_en, FEE_SID_RB_CHUNK_WRITE_FIRST, &xJob_st);
    }
    else
    {
        /* Keep stRet_u8 = E_NOT_OK */
    }

    return(stRet_u8);
}


/**
 * \brief   Request to continue an ongoing chunk-wise read job with the next chunk
 *
 * \param   deviceName_en  Device instance, for which job is requested
 * \param   dataBuf_pu8    Buffer location, where chunk data is copied into
 * \param   nrBytMax_u32   Maximum number of bytes, which can be copied into buffer
 *
 * \return  Request acceptance
 * \retval  E_OK      The request has been accepted, and the job processing is continued
 * \retval  E_NOT_OK  The request has been erroneous and therefore been rejected
 *
 * \attention  This request is only accepted during FEE_RB_BLOCK_PART_OK.
*/
Std_ReturnType Fee_Rb_Idx_ChunkReadNext(
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint8 *                  dataBuf_pu8,
        uint32                   nrBytMax_u32)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst;
    Fee_Prv_JobDesc_tst                      xJob_st;
    boolean                                  isCurrentJobWaitingForNextChunk_b;
    boolean                                  isReadJobOngoing_b;

    /* Check request in general (in subsequent chunks, we must be able to read at least one byte) */
    stRet_u8 = Fee_Prv_ChunkDetChkGnrl(deviceName_en, FEE_SID_RB_CHUNK_READ_NEXT, dataBuf_pu8, nrBytMax_u32, 1u);
    if(E_OK == stRet_u8)
    {
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1_PRV_CFG_ENABLED))
        /* Reading subsequent chunks is not supported for FeeFs1 */
        if(Fee_Rb_DeviceName == deviceName_en)
        {
            Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_READ_NEXT, FEE_E_INVALID_SEQUENCE);
            stRet_u8 = E_NOT_OK;
        }
        else
# endif
        {
            /* Copy active job */
            xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);

            /* Check if current job is a chunk wise read job and waiting for next chunk */
            isCurrentJobWaitingForNextChunk_b = Fee_Prv_JobIsWaitingForNextChunk(xCfgDevTbl_pcst);
            isReadJobOngoing_b = Fee_Prv_JobIsReadOngoing(xCfgDevTbl_pcst);
            if( !isCurrentJobWaitingForNextChunk_b || !isReadJobOngoing_b )
            {
                /* Deny request
                 * Note: It can happen that the chunk request was canceled by another user (e.g. during crash, NvM job
                 * cancels the ongoing chunk job). In such case, we should reject the request but not report DET error,
                 * as the job was not canceled by chunk user. */
                if(!Fee_Prv_JobIsChunkCancldByOtherUser(xCfgDevTbl_pcst))
                {
                    Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_READ_NEXT, FEE_E_INVALID_SEQUENCE);
                }
                stRet_u8 = E_NOT_OK;
            }
            else
            {
                /* Keep stRet_u8 = E_OK */
            }
        }
    }
    else
    {
        /* Deny request and keep stRet_u8 = E_NOT_OK, DET entry done by Fee_Prv_ChunkDetChkGnrl */
    }

    /* Request valid? */
    if(E_OK == stRet_u8)
    {
        /* Setup descriptor for chunk-wise job */
        xJob_st            = *Fee_Prv_JobGetActvDesc(xCfgDevTbl_pcst);
        xJob_st.type_en    = FEE_RB_JOBTYPE_READ_E;
        xJob_st.bfr_pu8    = dataBuf_pu8;
        /* The value in the element cntrPayloadBytDone_u32 can never be greater then uint16 (max block size supported
         * in Fee is 65535 bytes), so below operation of nrBytTot_u16-cntrPayloadBytDone_u32, won't cause underflow. */
        xJob_st.length_u16 = (uint16)rba_MemLib_Min(
                                             ((uint32)xJob_st.nrBytTot_u16 - xJob_st.bytDone_un.cntrPayloadBytDone_u32),
                                             nrBytMax_u32);
        xJob_st.offset_u16 = (uint16)xJob_st.bytDone_un.cntrPayloadBytDone_u32;

        /* Keep other job information of previous chunk(s) */

        /* Continue chunk-wise job */
        stRet_u8 = Fee_Prv_JobPut(deviceName_en, FEE_SID_RB_CHUNK_READ_NEXT, &xJob_st);
    }
    else
    {
        /* Keep stRet_u8 = E_NOT_OK */
    }

    return(stRet_u8);
}


/**
 * \brief   Request to continue an ongoing chunk-wise write job with the next chunk
 *
 * \param   deviceName_en  Device instance, for which job is requested
 * \param   dataBuf_pcu8   Buffer location, from where chunk data is copied
 * \param   nrBytMax_u32   Maximum number of bytes, which can be copied from buffer
 *
 * \return  Request acceptance
 * \retval  E_OK      The request has been accepted, and the job processing is continued
 * \retval  E_NOT_OK  The request has been erroneous and therefore been rejected
 *
 * \attention  This request is only accepted during FEE_RB_BLOCK_PART_OK.
 *             As a minimum, one page of block data must be provided in the buffer unless this request is for the last
 *             chunk. For the last chunk, the exact number of bytes, which completes the block data, must be provided.
 *             Otherwise, the request is rejected.
 *             After processing this request successfully, it is mandatory to query the number of processed bytes with
 *             Fee_Rb_Idx_ChunkGetNrBytProc before continuing with the next chunk! Background: Depending on page
 *             alignment, the chunk-wise write will not necessarily process the given buffer contents completely. The
 *             user is responsible to provide the unprocessed data (from the chunk's end) at the beginning of the next
 *             chunk again!
*/
Std_ReturnType Fee_Rb_Idx_ChunkWriteNext(
        Fee_Rb_DeviceName_ten    deviceName_en,
        uint8 const *            dataBuf_pcu8,
        uint32                   nrBytMax_u32)
{
    /* Return variable */
    Std_ReturnType    stRet_u8;

    /* Local variables */
    Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst;
    Fee_Prv_JobDesc_tst                      xJob_st;
    uint8                                    nrPageSize_u8;
    uint32                                   nrLen_u32;
    boolean                                  isCurrentJobWaitingForNextChunk_b;
    boolean                                  isWriteJobOngoing_b;

    /* Check request in general
     * Note: The chunk's length check can only be done after knowing the active job, and should only be done
     * if we are actually waiting for the next chunk (otherwise the active job might contain nonsense data). */
    stRet_u8 = Fee_Prv_ChunkDetChkGnrl(
                deviceName_en,
                FEE_SID_RB_CHUNK_WRITE_NEXT,
                dataBuf_pcu8,
                nrBytMax_u32,
                1u );
    if(E_OK == stRet_u8)
    {
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1_PRV_CFG_ENABLED))
        /* Writing subsequent chunks is not supported for FeeFs1 */
        if(Fee_Rb_DeviceName == deviceName_en)
        {
            Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_NEXT, FEE_E_INVALID_SEQUENCE);
            stRet_u8 = E_NOT_OK;
        }
        else
# endif
        {
            /* Copy active job */
            xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
            xJob_st         = *Fee_Prv_JobGetActvDesc(xCfgDevTbl_pcst);

            /* Check if current job is a chunk wise write job and waiting for next chunk */
            isCurrentJobWaitingForNextChunk_b = Fee_Prv_JobIsWaitingForNextChunk(xCfgDevTbl_pcst);
            isWriteJobOngoing_b = Fee_Prv_JobIsWriteOngoing(xCfgDevTbl_pcst);
            if( !isCurrentJobWaitingForNextChunk_b || !isWriteJobOngoing_b )
            {
                /* Deny request
                 * Note: It can happen that the chunk request was canceled by another user (e.g. during crash, NvM job
                 * cancels the ongoing chunk job). In such case, we should reject the request but not report DET error,
                 * as the job was not canceled by chunk user. */
                if(!Fee_Prv_JobIsChunkCancldByOtherUser(xCfgDevTbl_pcst))
                {
                    Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_NEXT, FEE_E_INVALID_SEQUENCE);
                }
                stRet_u8 = E_NOT_OK;
            }
            else
            {
                /* Subsequent chunk(s) will follow? */
                if((nrBytMax_u32 + xJob_st.bytDone_un.cntrPayloadBytDone_u32) < xJob_st.nrBytTot_u16)
                {
                    /* Chunk contains at least one page of block data? */
                    nrPageSize_u8 = Fee_Prv_FsIfGetLogicalPageSize(deviceName_en);
                    if(nrBytMax_u32 >= nrPageSize_u8)
                    {
                        /* Cut off excess block data that's not filling up a complete page */
                        nrLen_u32 = Fee_Prv_LibFloor(nrBytMax_u32, nrPageSize_u8);
                    }
                    else
                    {
                        /* Deny request */
                        Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_NEXT, FEE_E_INVALID_BUF_LEN);
                        stRet_u8 = E_NOT_OK;
                    }
                }
                else
                {
                    /* Chunk contains the exact amount of payload to complete the block data? */
                    if((nrBytMax_u32 + xJob_st.bytDone_un.cntrPayloadBytDone_u32) == xJob_st.nrBytTot_u16)
                    {
                        /* Write remaining block data completely */
                        nrLen_u32 = nrBytMax_u32;
                    }
                    else
                    {
                        /* Deny request */
                        Fee_Prv_LibDetReport(deviceName_en, FEE_SID_RB_CHUNK_WRITE_NEXT, FEE_E_INVALID_BUF_LEN);
                        stRet_u8 = E_NOT_OK;
                    }
                }
            }
        }
    }
    else
    {
        /* Deny request and keep stRet_u8 = E_NOT_OK, DET entry done by Fee_Prv_ChunkDetChkGnrl */
    }

    /* Request valid? */
    if(E_OK == stRet_u8)
    {
        /* Setup descriptor for chunk-wise job */
        xJob_st.type_en    = FEE_RB_JOBTYPE_WRITE_E;
        xJob_st.bfr_pcu8   = dataBuf_pcu8;
        xJob_st.length_u16 = (uint16)nrLen_u32;
        xJob_st.offset_u16 = (uint16)xJob_st.bytDone_un.cntrPayloadBytDone_u32;

        /* Keep other job information of previous chunk(s) */

        /* Continue chunk-wise job */
        stRet_u8 = Fee_Prv_JobPut(deviceName_en, FEE_SID_RB_CHUNK_WRITE_NEXT, &xJob_st);
    }
    else
    {
        /* Keep stRet_u8 = E_NOT_OK */
    }

    return(stRet_u8);
}


/**
 * \brief   Request to return the result of the current/last chunk(-wise job)
 *
 * \param   deviceName_en  Device instance, for which the result is requested
 *
 * \return  Result of the current/last chunk(-wise job)
 * \retval  FEE_RB_JOB_OK              Processing of all chunks has succeeded, complete chunk-wise job is done
 * \retval  FEE_RB_BLOCK_PART_OK       Processing of last chunk has succeeded, ready for next chunk
 * \retval  FEE_RB_BLOCK_INCONSISTENT  Processing of chunk-wise read job has detected inconsistent data for ReadNext
 * \retval  FEE_RB_JOB_FAILED          Processing of chunk-wise job has failed
 * \retval  FEE_RB_JOB_PENDING         Processing of last/current chunk hasn't finished yet
 * \retval  FEE_RB_JOB_CANCELED        Processing of chunk-wise job has been terminated by the user
 *
 * \attention  If the processing of a chunk-wise read has resulted in FEE_RB_BLOCK_INCONSISTENT (which is possible
 *             for any but the first chunk), the buffer hasn't been loaded with payload but with the chunk header
 *             instead. One single retry of the complete chunk-wise read shall be done for that block!
*/
Fee_Rb_JobResultType_ten Fee_Rb_Idx_ChunkGetResult(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    Fee_Rb_JobResultType_ten    stFeeRbJobRes_en;

# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1_PRV_CFG_ENABLED))
    /* Local variable */
    MemIf_JobResultType         stMemIfJobRes_en;
# endif

    /* Device name okay? */
    if(E_OK == Fee_Prv_OrderDetCheckDeviceName(deviceName_en, FEE_SID_RB_CHUNK_GET_RESULT))
    {
        /* Fee initialized and not in stop mode? */
        if(E_OK == Fee_Prv_OrderDetCheckModuleInitAndStopMode(deviceName_en, FEE_SID_RB_CHUNK_GET_RESULT))
        {
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1_PRV_CFG_ENABLED))
            /* In FeeFs1, we have to poll and determine the result ourselves. Reason is that it is a
             * deprecated file systems, where only bug fixes but no more new features are allowed. */
            if(Fee_Rb_DeviceName == deviceName_en)
            {
                /* Chunk under processing? */
                if(NULL_PTR != Fee_Prv_ChunkGetJobResult_pfct)
                {
                    /* Chunk done? */
                    stMemIfJobRes_en = Fee_Prv_ChunkGetJobResult_pfct();
                    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en);
                    if(FEE_RB_JOB_PENDING != stFeeRbJobRes_en)
                    {
                        Fee_Prv_ChunkDoneCbk(
                                Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en),
                                stFeeRbJobRes_en );
                    }
                    else
                    {
                        /* Do nothing (return Fee_Prv_ChunkInfo_st.result_en = FEE_RB_JOB_PENDING) */
                    }
                }
                else
                {
                    /* Do nothing (return Fee_Prv_ChunkInfo_st.result_en of last chunk-wise job) */
                }

                stFeeRbJobRes_en = Fee_Prv_ChunkInfo_st.result_en;
            }
            else
# endif
            {
                /* In all other file systems, we obtain the result from the job unit */
                stFeeRbJobRes_en = Fee_Prv_JobGetResult(deviceName_en, FEE_RB_USER_CHUNK_E);
            }
        }
        else
        {
            /* Leave with error, DET entry done by Fee_Prv_OrderDetCheckModuleInitAndStopMode */
            stFeeRbJobRes_en = FEE_RB_JOB_FAILED;
        }
    }
    else
    {
        /* Leave with error, DET entry done by Fee_Prv_OrderDetCheckDeviceName */
        stFeeRbJobRes_en = FEE_RB_JOB_FAILED;
    }

    return(stFeeRbJobRes_en);
}


/**
 * \brief   Request to return the number of bytes that have been processed in the current/last chunk
 *
 * \param   deviceName_en  Device instance, for which number of bytes is requested
 *
 * \return  Number of bytes that have been processed in the current/last chunk
*/
uint32 Fee_Rb_Idx_ChunkGetNrBytProc(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Return variable */
    uint32    nrBytProc_u32 = 0u;

    /* Local variable */
    Fee_Prv_ConfigDeviceTable_tst const *    xCfgDevTbl_pcst;

    /* Device name okay? */
    if(E_OK == Fee_Prv_OrderDetCheckDeviceName(deviceName_en, FEE_SID_RB_CHUNK_GET_NR_BYT_PROC))
    {
        /* Fee initialized and not in stop mode? */
        if(E_OK == Fee_Prv_OrderDetCheckModuleInitAndStopMode(deviceName_en, FEE_SID_RB_CHUNK_GET_NR_BYT_PROC))
        {
# if(defined(RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1_PRV_CFG_ENABLED))
            /* In FeeFs1, we have to obtain the number of processed bytes from this unit's environment variables */
            if(Fee_Rb_DeviceName == deviceName_en)
            {
                nrBytProc_u32 = Fee_Prv_ChunkInfo_st.nrBytProc_u32;
            }
            else
# endif
            {
                xCfgDevTbl_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
                /* In all other file systems, we obtain the number of processed bytes from the job unit */
                nrBytProc_u32 = Fee_Prv_JobGetChunkNrBytProc(xCfgDevTbl_pcst);
            }
        }
        else
        {
            /*  Keep nrBytProc_u32 = 0u, DET entry done by Fee_Prv_OrderDetCheckModuleInitAndStopMode */
        }
    }
    else
    {
        /*  Keep nrBytProc_u32 = 0u, DET entry done by Fee_Prv_OrderDetCheckDeviceName */
    }

    return(nrBytProc_u32);
}


/**
 * \brief   Request to cancel an ongoing chunk-wise read or write job that is waiting for the next chunk (result
 *          FEE_RB_BLOCK_PART_OK), e.g. if the user has run into problems while processing a read chunk.
 *          The check for waiting for next chunk is implemented in Fee_Rb_Idx_Cancel().
 *          Because we have to do basic check first, like valid deviceName_en, Fee is initialized and so on.
 *
 * \param   deviceName_en  Device instance, for which job termination is requested
 *
*/
void Fee_Rb_Idx_ChunkCancel(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Cancel chunk job and continue to process all other jobs. The called Fee_Rb_Idx_Cancel requires that
     * FEE_PRV_CFG_RB_CANCEL is TRUE, which is ensured by the configuration processor/the code generator. */
    Fee_Rb_Idx_Cancel(deviceName_en, FEE_RB_USER_CHUNK_E);

    return;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* #if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS)) */
#endif

/* #if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED)) */
#endif
