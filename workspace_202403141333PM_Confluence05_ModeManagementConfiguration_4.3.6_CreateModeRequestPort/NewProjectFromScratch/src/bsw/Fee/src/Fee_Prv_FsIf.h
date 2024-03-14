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

#ifndef FEE_PRV_FSIF_H
#define FEE_PRV_FSIF_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* The FsIf (Filesystem-Interace) unit is the connection from the main function to the actual file system.
 * Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Fee_Rb_Idx.h"
#include "MemIf_Types.h"
#include "Fee_Rb_Types.h"
#include "Fee_Cfg.h"
#include "Fee_Prv_ConfigTypes.h"
#include "Fee_Prv_JobTypes.h"
#include "Fee_Prv_Job.h"
#include "Fee_PrvTypes.h"
#include "Fee_Prv_Config.h"
#  if (defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))
#include "rba_FeeFs1x.h"
#  endif
#  if (defined(RBA_FEEFS2_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS2_PRV_CFG_ENABLED))
#include "rba_FeeFs2.h"
#  endif
#  if (defined(RBA_FEEFS3_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS3_PRV_CFG_ENABLED))
#include "rba_FeeFs3.h"
#  endif

/*
 **********************************************************************************************************************
 * Macros
 **********************************************************************************************************************
*/
// Below are list of local macros that are used to simplify the implementation in the code below
#  if (defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))
#define FEE_PRV_FSIF_FEE1X_ENABLED (TRUE)
#  else
#define FEE_PRV_FSIF_FEE1X_ENABLED (FALSE)
#  endif

#  if (defined(RBA_FEEFS2_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS2_PRV_CFG_ENABLED))
#define FEE_PRV_FSIF_FEE2_ENABLED (TRUE)
#  else
#define FEE_PRV_FSIF_FEE2_ENABLED (FALSE)
#  endif

#  if (defined(RBA_FEEFS3_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS3_PRV_CFG_ENABLED))
#define FEE_PRV_FSIF_FEE3_ENABLED (TRUE)
#  else
#define FEE_PRV_FSIF_FEE3_ENABLED (FALSE)
#  endif


/*
 **********************************************************************************************************************
 * Inline Declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

// Following functions are called when data object is not yet available, so the information is needed based on the device name
LOCAL_INLINE uint16                                 Fee_Prv_FsIfGetMigrationBitmask(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16                                 Fee_Prv_FsIfGetVarLenBitmask(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16                                 Fee_Prv_FsIfGetDoubleStorageBitmask(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16                                 Fee_Prv_FsIfGetSurvivalBitmask(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16                                 Fee_Prv_FsIfGetNoFallbackBitmask(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE Fee_Rb_BlockPropertiesType_tst const * Fee_Prv_FsIfGetBlockPropertiesTable(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16                                 Fee_Prv_FsIfGetNrOfBlocks(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16                                 Fee_Prv_FsIfRbGetNrOfSectorsOrRows(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE boolean                                Fee_Prv_FsIfIsDirectMemoryReadAllowed(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE boolean                                Fee_Prv_FsIfIsUnknownBlockWriteAllowed(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE boolean                                Fee_Prv_FsIfIsUnknownSurvivalBlockWriteAllowed(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint8                                  Fee_Prv_FsIfGetEraseValue(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfGetEmulationStartAdr(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfGetBaseAdr(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfGetEmulationEndAdr(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint8                                  Fee_Prv_FsIfGetLogicalPageSize(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfGetLogicalRowSize(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfGetLogicalRowStartAdr(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrPhy_u32);
LOCAL_INLINE uint8                                  Fee_Prv_FsIfGetMinPayloadInFirstChunk(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_JobType_ten jobType_en);
LOCAL_INLINE MemIf_Rb_MigrationResult_ten           Fee_Prv_FsIfRbGetMigrationResultForBlockNr(Fee_Rb_DeviceName_ten deviceName_en, uint16 blkNr_u16);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfRbGetSectorChgCntr(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfRbGetSectorEraseCntr(Fee_Rb_DeviceName_ten deviceName_en, uint8 idxFeeLogl_u8);
LOCAL_INLINE uint32                                 Fee_Prv_FsIfRbGetFreeSpace(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE void                                   Fee_Prv_FsIfRbGetNrFreeBytesAndFatEntries(Fee_Rb_DeviceName_ten deviceName_en, uint32 * nrFreeBytes_pu32, uint32 * nrFreeFatEntries_pu32, boolean isEraseAccepted_b);
LOCAL_INLINE uint8 *                                Fee_Prv_FsIfGetPersistentBufAdr(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE boolean                                Fee_Prv_FsIfIsSystemJobSupported(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_JobType_ten systemJobType_en);
LOCAL_INLINE boolean                                Fee_Prv_FsIfIsJobModeSupported(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_JobType_ten jobType_en, Fee_Rb_JobMode_ten jobMode_en);
LOCAL_INLINE Std_ReturnType                         Fee_Prv_FsIfInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                                   Fee_Prv_FsIfWrite(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                                   Fee_Prv_FsIfRead(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                                   Fee_Prv_FsIfInvalidateBlock(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                                   Fee_Prv_FsIfRbMaintainBlock(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                                   Fee_Prv_FsIfEraseImmediateBlock(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                                   Fee_Prv_FsIfRbCancelBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void                                   Fee_Prv_FsIfRbSystemJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobType_ten systemJobType_en);
LOCAL_INLINE void                                   Fee_Prv_FsIfCancelJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean                                Fee_Prv_FsIfIsCancelJobSuprtdForUser(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_User_ten user_en);
LOCAL_INLINE boolean                                Fee_Prv_FsIfIsHighSpeedModeSuprtd(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobResultType_ten               Fee_Prv_FsIfWriteDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobResultType_ten               Fee_Prv_FsIfReadDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobResultType_ten               Fee_Prv_FsIfInvalidateBlockDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobResultType_ten               Fee_Prv_FsIfRbMaintainBlockDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobResultType_ten               Fee_Prv_FsIfEraseImmediateBlockDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobResultType_ten               Fee_Prv_FsIfBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobResultType_ten               Fee_Prv_FsIfRbSystemJobDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobType_ten systemJobType_en);
LOCAL_INLINE boolean                                Fee_Prv_FsIfRbIsSoftReorgOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean                                Fee_Prv_FsIfRbIsHardReorgOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean                                Fee_Prv_FsIfRbIsSectorEraseOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobMode_ten                     Fee_Prv_FsIfGetJobModeForFastDriverMode(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobType_ten jobType_en);
LOCAL_INLINE MemIf_Rb_DetailedBlkInfo_ten           Fee_Prv_FsIfRbGetDetailedBlkInfo(Fee_Rb_DeviceName_ten deviceName_en, uint16 blkNr_u16);
LOCAL_INLINE Fee_Rb_SectState_ten                   Fee_Prv_FsIfRbGetSectState(Fee_Rb_DeviceName_ten deviceName_en, uint16 idxFeePhy_u16);

/*
 **********************************************************************************************************************
 * Inline Implementations
 **********************************************************************************************************************
*/

/**
 * \brief   get the migration bitmask for the block properties table of the given instance.
 *
 * \param   deviceName_en   Device instance for which the migration bit is requested
 *
 * \return  Migration bitmask
*/
LOCAL_INLINE uint16 Fee_Prv_FsIfGetMigrationBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint16    retVal_u16 = 0u;

#  if FEE_PRV_FSIF_FEE2_ENABLED
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);
    if(FEE_FS2 == configFS_en)
    {
        retVal_u16 = rba_FeeFs2_GetMigrationBitmask();
    }
#  else
    (void)deviceName_en;    // feature not supported for Fee1x and Fee3
#  endif

    return retVal_u16;
}

/**
 * \brief   get the variable length bitmask for the block properties table of the given instance.
 *
 * \param   deviceName_en   Device instance for which variable length bitmask is requested
 *
 * \return  Variable length bitmask
*/
LOCAL_INLINE uint16 Fee_Prv_FsIfGetVarLenBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint16    retVal_u16 = 0u;

#  if FEE_PRV_FSIF_FEE2_ENABLED
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);
    if(FEE_FS2 == configFS_en)
    {
        retVal_u16 = rba_FeeFs2_GetVarLenBitmask();
    }
#  else
    (void)deviceName_en;    // feature not supported for Fee1x and Fee3
#  endif

    return retVal_u16;
}

/**
 * \brief   get the double storage bitmask for the block properties table of the given instance.
 *
 * \param   deviceName_en   Device instance for which double storage bitmask is requested
 *
 * \return  Double storage bitmask
*/
LOCAL_INLINE uint16 Fee_Prv_FsIfGetDoubleStorageBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint16          retVal_u16  = 0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u16 = rba_FeeFs1x_getDoubleStorageBitmask();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u16 = rba_FeeFs2_GetDoubleStorageBitmask();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u16 = rba_FeeFs3_GetDoubleStorageBitmask();
    }
#  endif

    return retVal_u16;
}

/**
 * \brief   get the survival bitmask for the block properties table of the given instance.
 *
 * \param   deviceName_en   Device instance for which survival bitmask is requested
 *
 * \return  Survival bitmask
*/
LOCAL_INLINE uint16 Fee_Prv_FsIfGetSurvivalBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint16          retVal_u16  = 0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u16 = rba_FeeFs1x_getSurvivalBitmask();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u16 = rba_FeeFs2_GetSurvivalBitmask();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u16 = rba_FeeFs3_GetSurvivalBitmask();
    }
#  endif

    return retVal_u16;
}

/**
 * \brief   get the no-fallback bitmask for the block properties table of the given instance.
 *
 * \param   deviceName_en   Device instance for which no-fallback bitmask is requested
 *
 * \return  No-fallback bitmask
*/
LOCAL_INLINE uint16 Fee_Prv_FsIfGetNoFallbackBitmask(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint16          retVal_u16  = 0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u16 = rba_FeeFs1x_getNoFallbackBitmask();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u16 = rba_FeeFs2_GetNoFallbackBitmask();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void)configFS_en;    // feature not supported for Fee3
#  endif

    return retVal_u16;
}

/**
 * \brief   return the pointer to the block properties table for the given instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE Fee_Rb_BlockPropertiesType_tst const * Fee_Prv_FsIfGetBlockPropertiesTable(Fee_Rb_DeviceName_ten deviceName_en)
{
    Fee_Rb_BlockPropertiesType_tst const* retVal_past = NULL_PTR;
    Fee_Prv_Fs_ten                        configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_past = rba_FeeFs1x_getBlockPropertiesTable();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_past = rba_FeeFs2_GetBlockPropertiesTable();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_past = rba_FeeFs3_GetBlockPropertiesTable();
    }
#  endif

    return(retVal_past);
}

/**
 * \brief   return the number of blocks present in the given instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE uint16 Fee_Prv_FsIfGetNrOfBlocks(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint16          retVal_u16  = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u16 = rba_FeeFs1x_getNrOfBlocks();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u16 = rba_FeeFs2_GetNrOfBlocks();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u16 = rba_FeeFs3_GetNrOfBlocks();
    }
#  endif

    return retVal_u16;
}

/**
 * \brief   return the available number of sectors for the given instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE uint16 Fee_Prv_FsIfRbGetNrOfSectorsOrRows(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint16         retVal_u16   = 0x0u;
    Fee_Prv_Fs_ten configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u16 = rba_FeeFs1x_getNrOfSectors();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u16 = rba_FeeFs2_GetNrOfSectors();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u16 = rba_FeeFs3_GetNrOfRows();
    }
#  endif

    return retVal_u16;
}

/**
 * \brief   return if direct memory access is allowed or not.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE boolean Fee_Prv_FsIfIsDirectMemoryReadAllowed(Fee_Rb_DeviceName_ten deviceName_en)
{
    boolean         retVal_b;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_isDirectMemoryReadAllowed();
    }
    else
    {
        retVal_b = FALSE;    // feature not supported for Fee2 and Fee3
    }
#  else
    (void)configFS_en;
    retVal_b = FALSE;        // feature not supported for Fee2 and Fee3
#  endif

    return retVal_b;
}

/**
 * \brief   The function returns if writing of unknown block feature is activated or not.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE boolean Fee_Prv_FsIfIsUnknownBlockWriteAllowed(Fee_Rb_DeviceName_ten deviceName_en)
{
    boolean         retVal_b = FALSE; /* Don't change, it's required for Fee1.0 in Fee_Prv_ChunkDetChkUkwnBlkWrPrms */
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_isUnknownBlockWriteAllowed();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = rba_FeeFs2_IsUnknownBlockWriteAllowed();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_b = rba_FeeFs3_IsUnknownBlockWriteAllowed();
    }
#  endif

    return retVal_b;
}

/**
 * \brief   The function returns if writing of unknown Survival block feature is activated or not.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE boolean Fee_Prv_FsIfIsUnknownSurvivalBlockWriteAllowed(Fee_Rb_DeviceName_ten deviceName_en)
{
    boolean         retVal_b = FALSE; /* Don't change, it's required for Fee1.0 in Fee_Prv_ChunkDetChkUkwnBlkWrPrms */
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_isUnknownSurvivalBlockWriteAllowed();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = rba_FeeFs2_IsUnknownSurvivalBlockWriteAllowed();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_b = rba_FeeFs3_IsUnknownSurvivalBlockWriteAllowed();
    }
#  endif

    return retVal_b;
}

/**
 * \brief   return the erase value for the instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE uint8 Fee_Prv_FsIfGetEraseValue(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint8           retVal_u8   = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u8 = rba_FeeFs1x_getEraseValue();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u8 = rba_FeeFs2_GetEraseValue();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u8 = rba_FeeFs3_GetEraseValue();
    }
#  endif

    return retVal_u8;
}

/**
 * \brief   return the start address of the data flash for given instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfGetEmulationStartAdr(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint32          retVal_u32  = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u32 = rba_FeeFs1x_getEmulationStartAdr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u32 = rba_FeeFs2_GetEmulationStartAdr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u32 = rba_FeeFs3_GetEmulationStartAdr();
    }
#  endif

    return retVal_u32;
}

/**
 * \brief   return the Base address of the data flash for given instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfGetBaseAdr(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint32          retVal_u32  = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
            retVal_u32 = rba_FeeFs1x_getBaseAdr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
            retVal_u32 = rba_FeeFs2_GetBaseAdr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u32 = rba_FeeFs3_GetInverseBaseAdr();
    }
#  endif

    return retVal_u32;
}

/**
 * \brief   return the end address of the data flash for given instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfGetEmulationEndAdr(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint32          retVal_u32  = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u32 = rba_FeeFs1x_getEmulationEndAdr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u32 = rba_FeeFs2_GetEmulationEndAdr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u32 = rba_FeeFs3_GetEmulationEndAdr();
    }
#  endif

    return retVal_u32;
}

/**
 * \brief   Get the logical page size of the given instance
 *
 * \param   deviceName_en   Device instance for which page size is requested
*/
LOCAL_INLINE uint8 Fee_Prv_FsIfGetLogicalPageSize(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint8           retVal_u8   = 0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u8 = rba_FeeFs1x_getLogicalPageSize();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u8 = rba_FeeFs2_GetLogicalPageSize();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u8 = rba_FeeFs3_GetLogicalPageSize();
    }
#  endif

    return(retVal_u8);
}

/**
 * \brief   Get the logical row size of the given instance
 *
 * \param   deviceName_en   Device instance for which row size is requested
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfGetLogicalRowSize(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint32    retVal_u32 = FEE_PRV_MAX_UINT32;
#  if FEE_PRV_FSIF_FEE3_ENABLED
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);
    if(FEE_FS3 == configFS_en)
    {
        retVal_u32 = rba_FeeFs3_GetLogicalRowSize();
    }
#  else
    (void)deviceName_en;    // feature not supported for Fee1x and Fee2
#  endif

    return(retVal_u32);
}

/**
 * \brief   For a given instance and physical address in the data-NVM, this function returns the start address of the
 *          corresponding logical row
 *
 * \param   deviceName_en   Device instance for which logical row's start address is requested
 * \param   adrPhy_u32      Physical address for which logical row's start address is requested
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfGetLogicalRowStartAdr(Fee_Rb_DeviceName_ten deviceName_en, uint32 adrPhy_u32)
{
    uint32    retVal_u32 = adrPhy_u32;
#  if FEE_PRV_FSIF_FEE3_ENABLED
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);
    if(FEE_FS3 == configFS_en)
    {
        retVal_u32 = rba_FeeFs3_GetLogicalRowStartAdr(adrPhy_u32);
    }
#  else
    (void)deviceName_en;    // feature not supported for Fee1x and Fee2
#  endif

    return(retVal_u32);
}

/**
 * \brief   For a given instance, get the minimum payload that must be provided in the very first of successive chunks
 *
 * \param   deviceName_en   Device instance, for which minimum payload has to be returned
 * \param   jobType_en      Job type, for which minimum payload has to be returned
 *
 * \attention  Any filesystem (other than Fee1.0) shall return a value >= 1u; otherwise Fee_Prv_JobIsFirstPart doesn't
 *             work as intended!
*/
LOCAL_INLINE uint8 Fee_Prv_FsIfGetMinPayloadInFirstChunk(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_JobType_ten jobType_en)
{
    uint8           retVal_u8   = 0u; /* Don't change, it's required for Fee1.0 in Fee_Rb_Idx_ChunkWriteFirst */
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u8 = rba_FeeFs1x_getMinPayloadInFirstChunk(jobType_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u8 = rba_FeeFs2_GetMinPayloadInFirstChunk(jobType_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u8 = rba_FeeFs3_GetMinPayloadInFirstChunk(jobType_en);
    }
#  endif

    return(retVal_u8);
}

/**
 * \brief   Get the migration result of the given block number
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 * \param   blkNr_u16       Block number for which the result has to be extracted.
 *
 * \return  Migration result
*/
LOCAL_INLINE MemIf_Rb_MigrationResult_ten Fee_Prv_FsIfRbGetMigrationResultForBlockNr(Fee_Rb_DeviceName_ten deviceName_en, uint16 blkNr_u16)
{
    MemIf_Rb_MigrationResult_ten retVal_en   = MEMIF_RB_MIGRATION_RESULT_INIT_E;
    Fee_Prv_Fs_ten               configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    (void)configFS_en;    // feature not supported for Fee1x
    (void)blkNr_u16;
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_en = rba_FeeFs2_GetMigrationResultForBlkNr(blkNr_u16);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void)configFS_en;    // feature not supported for Fee3
    (void)blkNr_u16;
#  endif

    return(retVal_en);
}

/**
 * \brief   Function to return sector change counter
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 *
 * \retval  sector change counter
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfRbGetSectorChgCntr(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint32          retVal_u32  = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u32 = rba_FeeFs1x_GetSectorChgCntr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u32 = rba_FeeFs2_GetSectorChgCntr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void)configFS_en;    // feature not supported for Fee3
#  endif

    return retVal_u32;
}

/**
 * \brief   Get the number of sector erases for the given physical sector
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 * \param   physSectIdx_u8  Physical sector index
 *
 * \return  sector erase counter of requested sector
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfRbGetSectorEraseCntr(Fee_Rb_DeviceName_ten deviceName_en, uint8 idxFeeLogl_u8)
{
    uint32          retVal_u32  = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

    (void)configFS_en;    // feature not supported for Fee1x and Fee3
    (void)idxFeeLogl_u8;

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_u32 = rba_FeeFs2_GetSectorEraseCntr(idxFeeLogl_u8);
    }
#  endif

    return retVal_u32;
}

/**
 * \brief   Get the amount of free space in DFLASH
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 *
 * \return  Amount of free space in bytes
*/
LOCAL_INLINE uint32 Fee_Prv_FsIfRbGetFreeSpace(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint32          retVal_u32  = 0x0u;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_u32 = rba_FeeFs1x_GetFreeSpace();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    (void)configFS_en;    // feature not supported for Fee2
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_u32 = rba_FeeFs3_GetFreeSpace();
    }
#  endif

    return retVal_u32;
}

/**
 * \brief   Get the amount of free user data space and number of free control data space (FAT entries) in DFLASH before a hard reorganization will be started.
 *          If erase operations are not accepted by the user, free non erased sectors will not be considered for user data space evaluation.
 *
 * \param   deviceName_en          Device instance for which the job has to be performed
 * \param   nrFreeBytes_pu32       The pointer to the address, where the number of free data bytes information shall be written.
 * \param   nrFreeFatEntries_pu32  The pointer to the address, where the number of free FAT entries information shall be written.
 * \param   isEraseAccepted_b      Information how to handle free non erased sectors.
 *                                 TRUE:  Free non erased sectors shall be considered, erase operations may occur when writing the given number of bytes
 *                                 FALSE: Free non erased sectors shall not be considered, no erase operations when writing the given number of bytes
 */
LOCAL_INLINE void Fee_Prv_FsIfRbGetNrFreeBytesAndFatEntries(Fee_Rb_DeviceName_ten deviceName_en, uint32 * nrFreeBytes_pu32, uint32 * nrFreeFatEntries_pu32, boolean isEraseAccepted_b)
{
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        *nrFreeBytes_pu32      = 0u;
        *nrFreeFatEntries_pu32 = 0u;
        (void)isEraseAccepted_b;
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_GetNrFreeBytesAndFatEntries(nrFreeBytes_pu32, nrFreeFatEntries_pu32, isEraseAccepted_b);
    }
    else
    {
        *nrFreeBytes_pu32      = 0u;
        *nrFreeFatEntries_pu32 = 0u;
        (void)isEraseAccepted_b;
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        *nrFreeBytes_pu32      = 0u;
        *nrFreeFatEntries_pu32 = 0u;
        (void)isEraseAccepted_b;
    }
#  endif
}

/**
 * \brief   Return the pointer to the ram buffer for the given instance.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 *
 * \retval  pointer to ram buffer
*/
LOCAL_INLINE uint8 * Fee_Prv_FsIfGetPersistentBufAdr(Fee_Rb_DeviceName_ten deviceName_en)
{
    uint8*          retVal_pu8  = NULL_PTR;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    (void)configFS_en;    // feature not supported for Fee1x
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_pu8 = rba_FeeFs2_GetPersistentBufAdr();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void)configFS_en;    // feature not supported for Fee3
#  endif

    return retVal_pu8;
}

/**
 * \brief   Return if passed system job type is supported or not.
 *
 * \param   deviceName_en      Device instance for which the job has to be performed
 * \param   systemJobType_en   System job type to be checked
 *
 * \retval  TRUE: if passed job type is supported, else FALSE is returned
*/
LOCAL_INLINE boolean Fee_Prv_FsIfIsSystemJobSupported(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_JobType_ten systemJobType_en)
{
    boolean         retVal_b = FALSE;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_isSystemJobSupported(systemJobType_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = rba_FeeFs2_IsSystemJobSupported(systemJobType_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void) configFS_en;
    (void) systemJobType_en;
#  endif

    return retVal_b;
}

/**
 * \brief   return if passed job mode for the job type is supported or not.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 * \param   jobType_en      Job type for which mode has to be checked
 * \param   jobMode_en      Mode that is requested to be checked for the passed job type
 *
 * \retval  TRUE: if passed job mode is supported for the job, else FALSE is returned
*/
LOCAL_INLINE boolean Fee_Prv_FsIfIsJobModeSupported(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_JobType_ten jobType_en, Fee_Rb_JobMode_ten jobMode_en)
{
    boolean         retVal_b = FALSE;
    Fee_Prv_Fs_ten  configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_isJobModeSupported(jobType_en, jobMode_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = rba_FeeFs2_IsJobModeSupported(jobType_en, jobMode_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_b = rba_FeeFs3_IsJobModeSupported(jobType_en, jobMode_en);
    }
#  endif

    return retVal_b;

}

/**
 * \brief   Initialize a file system.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the job has to be performed
*/
LOCAL_INLINE Std_ReturnType Fee_Prv_FsIfInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Std_ReturnType  stRetVal_u8 = E_NOT_OK;
    Fee_Prv_Fs_ten  configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        stRetVal_u8 = rba_FeeFs1x_Init(deviceConfigTable_pcst->deviceName_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        stRetVal_u8 = rba_FeeFs2_Init(deviceConfigTable_pcst->deviceName_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        stRetVal_u8 = rba_FeeFs3_Ini(deviceConfigTable_pcst->deviceName_en);
    }
#  endif

    return(stRetVal_u8);
}

/**
 * \brief   Let the file system start a write job.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the job has to be performed
*/
LOCAL_INLINE void Fee_Prv_FsIfWrite(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Fs_ten              configFS_en = deviceConfigTable_pcst->fs_en;
    Fee_Prv_JobDesc_tst const * order_pcst  = Fee_Prv_JobGetActvDesc(deviceConfigTable_pcst);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        rba_FeeFs1x_Write(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_Write(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        rba_FeeFs3_Write(order_pcst);
    }
#  endif

    (void)order_pcst;

    return;
}

/**
 * \brief   Let the file system start a read job.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the job has to be performed
*/
LOCAL_INLINE void Fee_Prv_FsIfRead(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Fs_ten              configFS_en = deviceConfigTable_pcst->fs_en;
    Fee_Prv_JobDesc_tst const * order_pcst  = Fee_Prv_JobGetActvDesc(deviceConfigTable_pcst);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        rba_FeeFs1x_Read(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_Read(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        rba_FeeFs3_Read(order_pcst);
    }
#  endif

    (void)order_pcst;

    return;
}

/**
 * \brief   Let the file system start an invalidate job.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the job has to be performed
*/
LOCAL_INLINE void Fee_Prv_FsIfInvalidateBlock(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Fs_ten              configFS_en = deviceConfigTable_pcst->fs_en;
    Fee_Prv_JobDesc_tst const * order_pcst  = Fee_Prv_JobGetActvDesc(deviceConfigTable_pcst);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        rba_FeeFs1x_Invalidate(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_Invalidate(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        rba_FeeFs3_Invalidate(order_pcst);
    }
#  endif

    (void)order_pcst;

    return;
}

/**
 * \brief Let the file system start a maintain job.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the job has to be performed
*/
LOCAL_INLINE void Fee_Prv_FsIfRbMaintainBlock(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Fs_ten              configFS_en = deviceConfigTable_pcst->fs_en;
    Fee_Prv_JobDesc_tst const * order_pcst  = Fee_Prv_JobGetActvDesc(deviceConfigTable_pcst);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        rba_FeeFs1x_Rb_Maintain(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_Maintain(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        rba_FeeFs3_Maintain(order_pcst);
    }
#  endif

    (void)order_pcst;

    return;
}

/**
 * \brief   Let the file system start an erase immediate job.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the job has to be performed
 *
 * \attention   Currently no file system supports erase immediate.
*/
LOCAL_INLINE void Fee_Prv_FsIfEraseImmediateBlock(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Fs_ten              configFS_en = deviceConfigTable_pcst->fs_en;
    Fee_Prv_JobDesc_tst const * order_pcst  = Fee_Prv_JobGetActvDesc(deviceConfigTable_pcst);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    (void)configFS_en;    // feature not supported for Fee1x
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_EraseImmediate(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        rba_FeeFs3_Erase(order_pcst);
    }
#  endif

    (void)order_pcst;

    return;
}

/**
 * \brief   Let the file system cancel a background operation.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the background operation has to be canceled
 *
*/
LOCAL_INLINE void Fee_Prv_FsIfRbCancelBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Fs_ten configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    (void)configFS_en;    // feature not supported for Fee1x
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_CancelBackground();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        rba_FeeFs3_CancelBackground();
    }
#  endif

    return;
}

/**
 * \brief   Let the file system start a system job.
 *
 * \param   deviceConfigTable_pcst:  Pointer to the config table for which the job has to be performed
 * \param   systemJobType_en:        The kind of system job, which is requested
*/
LOCAL_INLINE void Fee_Prv_FsIfRbSystemJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobType_ten systemJobType_en)
{
    /* This function is called in default case of Fee_Prv_ProcStartExtJob,
     * so don't do anything in case systemJobType_en is invalid */
    Fee_Prv_Fs_ten configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        if(FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORG_E == systemJobType_en)
        {
            rba_FeeFs1x_TriggerReorg();
        }
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        if(FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGFAT_E == systemJobType_en)
        {
            rba_FeeFs2_TriggerReorgFat();
        }

        if(FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGDAT_E == systemJobType_en)
        {
            rba_FeeFs2_TriggerReorgDat();
        }
    }
#endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void) configFS_en;
    (void) systemJobType_en;
#  endif

    return;
}

/**
 * \brief   Cancel any ongoing job and reset the state machines to start with the new job.
 *
 * \param   deviceConfigTable_pcst:  Pointer to the config table for which the job has to be canceled
 *
 * \attention Cancel feature for all users is currently supported only in Fee1x. For Fee2 and Fee3, cancel for
 *            chunk user is supported, so this function calls the corresponding termianate API. Once cancel feature
 *            is supported in these file systems, the below function should also be updated to replace
 *            rba_FeeFs2_TerminateChunkJob by rba_FeeFs2_CancelJob (same for Fee3)
*/
LOCAL_INLINE void Fee_Prv_FsIfCancelJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Fs_ten              configFS_en = deviceConfigTable_pcst->fs_en;
    Fee_Prv_JobDesc_tst const * order_pcst  = Fee_Prv_JobGetActvDesc(deviceConfigTable_pcst);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    (void)order_pcst;
    if(FEE_FS1X == configFS_en)
    {
        rba_FeeFs1x_CancelJob(deviceConfigTable_pcst->deviceName_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        rba_FeeFs2_TerminateChunkJob(order_pcst);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        rba_FeeFs3_TerminateChunkJob(order_pcst);
    }
#  endif
}

/**
 * \brief   The function checks if FeeFsx supports the cancel feature for the passer user.
 * Note: Ideally this check should be routed to respective FeeFsx component. But this is a temporary function and
 * will be removed when cancel feature is fully supported for all file system, so the implementation is kept here.
 *
 * \param   deviceConfigTable_pcst: Pointer to the config table for which the info is requested
 * \param   user_en:                User for which the cancel feature is requested
 *
 * \retval  TRUE  : FeeFsx supports cancel feature for passed user
 * \retval  FALSE : FeeFsx does not support cancel feature for passed user
*/
LOCAL_INLINE boolean Fee_Prv_FsIfIsCancelJobSuprtdForUser(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_User_ten user_en)
{
    boolean         retVal_b = FALSE;
    Fee_Prv_Fs_ten  configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    (void)user_en;
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = TRUE;
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = (FEE_RB_USER_CHUNK_E == user_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_b = (FEE_RB_USER_CHUNK_E == user_en);
    }
#  endif

    return(retVal_b);
}

/**
 * \brief   The function checks if FeeFsx supports the high speed mode feature.
 * Note: Ideally this check should be routed to respective FeeFsx component. But this is a temporary function and
 * will be removed when high speed mode feature is fully supported for all file system, so the implementation is kept here.
 *
 * \param   deviceConfigTable_pcst: Pointer to the config table for which the info is requested
 *
 * \retval  TRUE  : FeeFsx supports high speed mode feature
 * \retval  FALSE : FeeFsx does not support high speed mode feature
*/
LOCAL_INLINE boolean Fee_Prv_FsIfIsHighSpeedModeSuprtd(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    boolean         retVal_b = FALSE;
    Fee_Prv_Fs_ten  configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = TRUE;
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = FALSE;
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_b = FALSE;
    }
#  endif

    return(retVal_b);
}

/**
 * \brief   Do the work of a write job.
 *
 * \param   deviceConfigTable_pcst:  pointer to the config table for which the job has to be performed
 *
 * \retval  FEE_RB_JOB_PENDING       Write is not yet finished
 * \retval  FEE_RB_JOB_OK            Write finished successfully
 * \retval  FEE_RB_JOB_FAILED        Write failed
*/
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_FsIfWriteDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten stFeeRbJobRes_en;
    MemIf_JobResultType      stMemIfJobRes_en = MEMIF_JOB_FAILED;
    Fee_Prv_Fs_ten           configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs1x_BlockJobDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs2_WriteDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs3_WriteDo();
    }
#  endif

    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en );
    return(stFeeRbJobRes_en);
}

/**
 * \brief   Do the work of a read job.
 *
 * \param   deviceConfigTable_pcst:      pointer to the config table for which the job has to be performed
 *
 * \retval  FEE_RB_JOB_PENDING           Read is not yet finished
 * \retval  FEE_RB_JOB_OK                Read finished successfully
 * \retval  FEE_RB_BLOCK_INCONSISTENT    Block was not found or the data was not consistent
 * \retval  FEE_RB_BLOCK_INVALID         Block was previously invalidated with an invalidate order
 * \retval  FEE_RB_JOB_FAILED            Read failed
*/
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_FsIfReadDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten stFeeRbJobRes_en;
    MemIf_JobResultType      stMemIfJobRes_en = MEMIF_JOB_FAILED;
    Fee_Prv_Fs_ten           configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs1x_BlockJobDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs2_ReadDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs3_ReadDo();
    }
#  endif

    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en );
    return(stFeeRbJobRes_en);
}

/**
 * \brief   Do the work of an invalidate job.
 *
 * \param   deviceConfigTable_pcst   pointer to the config table for which the job has to be performed
 *
 * \retval  FEE_RB_JOB_PENDING       Invalidate is not yet finished
 * \retval  FEE_RB_JOB_OK            Invalidate finished successfully
 * \retval  FEE_RB_JOB_FAILED        Invalidate failed
*/
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_FsIfInvalidateBlockDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten stFeeRbJobRes_en;
    MemIf_JobResultType      stMemIfJobRes_en = MEMIF_JOB_FAILED;
    Fee_Prv_Fs_ten           configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs1x_BlockJobDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs2_InvalidateDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs3_InvalidateDo();
    }
#  endif

    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en );
    return(stFeeRbJobRes_en);
}

/**
 * \brief   Do the work of an Maintain job.
 *
 * \param   deviceConfigTable_pcst   pointer to the config table for which the job has to be performed
 *
 * \retval  FEE_RB_JOB_PENDING       Maintain is not yet finished
 * \retval  FEE_RB_JOB_OK            Maintain finished successfully
 * \retval  FEE_RB_JOB_FAILED        Maintain failed
*/
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_FsIfRbMaintainBlockDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten stFeeRbJobRes_en;
    MemIf_JobResultType      stMemIfJobRes_en = MEMIF_JOB_FAILED;
    Fee_Prv_Fs_ten           configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs1x_BlockJobDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs2_MaintainDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs3_MaintainDo();
    }
#  endif

    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en );
    return(stFeeRbJobRes_en);
}

/**
 * \brief   Do the work of an erase immediate job.
 *
 * \param   deviceConfigTable_pcst   pointer to the config table for which the job has to be performed
 *
 * \retval  FEE_RB_JOB_PENDING       Erase immediate is not yet finished
 * \retval  FEE_RB_JOB_OK            Erase immediate finished successfully
 * \retval  FEE_RB_JOB_FAILED        Erase immediate failed
 *
 * \attention   Currently no file system supports erase immediate.
*/
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_FsIfEraseImmediateBlockDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten stFeeRbJobRes_en;
    MemIf_JobResultType      stMemIfJobRes_en = MEMIF_JOB_FAILED;
    Fee_Prv_Fs_ten           configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    (void)configFS_en;    // feature not supported for Fee1x
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs2_EraseImmediateDo();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs3_EraseDo();
    }
#  endif

    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en );
    return(stFeeRbJobRes_en);
}

/**
 * \brief   Let the file system perform some background actions.
 *          This can for example mean erasing a sector in the background.
 *
 * \param   deviceConfigTable_pcst   pointer to the config table for which the job has to be performed
 *
 * \retval  FEE_RB_JOB_PENDING       A background operation is currently running
 * \retval  FEE_RB_JOB_OK            No background operation is currently running
*/
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_FsIfBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten stFeeRbJobRes_en;
    MemIf_JobResultType      stMemIfJobRes_en = MEMIF_JOB_FAILED;
    Fee_Prv_Fs_ten           configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs1x_Background();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs2_Background();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        stMemIfJobRes_en = rba_FeeFs3_Background();
    }
#  endif

    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en );
    return(stFeeRbJobRes_en);
}

/**
 * \brief   Do the work of a system job.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the job has to be performed
 * \param   systemJobType_en        System job to be performed
 *
 * \retval  FEE_RB_JOB_PENDING       System job is not yet finished
 * \retval  FEE_RB_JOB_OK            System job is finished successfully
 * \retval  FEE_RB_JOB_FAILED        System job failed
*/
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_FsIfRbSystemJobDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobType_ten systemJobType_en)
{
    /* This function is called in default case of Fee_Prv_ProcProcessExtJob,
     * so don't do anything but return FEE_RB_JOB_FAILED in case systemJobType_en is invalid */
    Fee_Rb_JobResultType_ten stFeeRbJobRes_en;
    MemIf_JobResultType      stMemIfJobRes_en = MEMIF_JOB_FAILED;
    Fee_Prv_Fs_ten           configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        if(FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORG_E == systemJobType_en)
        {
            stMemIfJobRes_en = rba_FeeFs1x_TriggerReorgDo();
        }
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        if(FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGFAT_E == systemJobType_en)
        {
            stMemIfJobRes_en = rba_FeeFs2_TriggerReorgFatDo();
        }

        if(FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGDAT_E == systemJobType_en)
        {
            stMemIfJobRes_en = rba_FeeFs2_TriggerReorgDatDo();
        }
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void) systemJobType_en;
    (void) configFS_en;
    (void) stMemIfJobRes_en ;
#  endif

    stFeeRbJobRes_en = Fee_Rb_MapMemIfRetToFeeRbJobRet(stMemIfJobRes_en );
    return(stFeeRbJobRes_en);
}

/**
 * \brief   Get the status of soft reorg (only when in driverstate background!)
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the job has to be performed
 *
 * \return  Status of soft reorg
*/
LOCAL_INLINE boolean Fee_Prv_FsIfRbIsSoftReorgOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    boolean         retVal_b    = FALSE;
    Fee_Prv_Fs_ten  configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_IsSoftReorgOngoing();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = rba_FeeFs2_IsReorgOngoing();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_b = rba_FeeFs3_IsSoftDefragOngoing();
    }
#  endif

    return retVal_b;
}

/**
 * \brief   Get the status of hard reorg
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the job has to be performed
 *
 * \return  Status of hard reorg
*/
LOCAL_INLINE boolean Fee_Prv_FsIfRbIsHardReorgOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    boolean         retVal_b    = FALSE;
    Fee_Prv_Fs_ten  configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_IsHardReorgOngoing();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = rba_FeeFs2_IsReorgOngoing();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_b = rba_FeeFs3_IsHardDefragOngoing();
    }
#  endif

    return retVal_b;
}

/**
 * \brief   Get the status of sector erase
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the job has to be performed
 *
 * \return  Status of sector erase
*/
LOCAL_INLINE boolean Fee_Prv_FsIfRbIsSectorEraseOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    boolean         retVal_b    = FALSE;
    Fee_Prv_Fs_ten  configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_b = rba_FeeFs1x_IsSectorEraseOngoing();
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_b = rba_FeeFs2_IsSectorEraseOngoing();
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    (void)configFS_en;    // feature not supported for Fee3
#  endif

    return retVal_b;
}

/**
 * \brief   return the job mode to be used when driver is working in MEMIF_FAST_MODE mode.
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 * \param   jobType_en      Job type for which mode has to be checked
 *
 * \retval  mode to be used
*/
LOCAL_INLINE Fee_Rb_JobMode_ten Fee_Prv_FsIfGetJobModeForFastDriverMode(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobType_ten jobType_en)
{
    Fee_Rb_JobMode_ten retVal_en = FEE_RB_ALLJOBS_ALLSTEPS_E;
    Fee_Prv_Fs_ten     configFS_en = deviceConfigTable_pcst->fs_en;

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_en = rba_FeeFs1x_getJobModeForFastDriverMode(jobType_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_en = rba_FeeFs2_GetJobModeForFastDriverMode(jobType_en);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_en = rba_FeeFs3_GetJobModeForFastDriverMode(jobType_en);
    }
#  endif

    return (retVal_en);
}

/**
 * \brief   return the detailed block info in case of block inconsistency
 *
 * \param   deviceName_en   Device instance for which the job has to be performed
 * \param   blkNr_u16       Block number
 *
 * \retval  Detailed block info
*/
LOCAL_INLINE MemIf_Rb_DetailedBlkInfo_ten Fee_Prv_FsIfRbGetDetailedBlkInfo(Fee_Rb_DeviceName_ten deviceName_en, uint16 blkNr_u16)
{
    MemIf_Rb_DetailedBlkInfo_ten retVal_en = MEMIF_RB_DETAILED_BLK_INFO_INIT_E;
    Fee_Prv_Fs_ten               configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_en = rba_FeeFs1x_GetDetailedBlkInfo(blkNr_u16);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_en = rba_FeeFs2_GetDetailedBlkInfo(blkNr_u16);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_en = rba_FeeFs3_GetDetailedBlkInfo(blkNr_u16);
    }
#  endif

    return (retVal_en);
}

/**
 * \brief   return the state of the sector or row
 *
 * \param   deviceName_en       Device instance for which the job has to be performed
 * \param   idxFeePhy_u16      Sector or row index for which the state information is requested
 *
 * \retval  state of the sector or row
*/
LOCAL_INLINE Fee_Rb_SectState_ten Fee_Prv_FsIfRbGetSectState(Fee_Rb_DeviceName_ten deviceName_en, uint16 idxFeePhy_u16)
{
    Fee_Rb_SectState_ten retVal_en = FEE_RB_SECT_STATE_UNDEFD_E;
    Fee_Prv_Fs_ten       configFS_en = Fee_Prv_ConfigGetFSFromDeviceName(deviceName_en);

#  if FEE_PRV_FSIF_FEE1X_ENABLED
    if(FEE_FS1X == configFS_en)
    {
        retVal_en = rba_FeeFs1x_GetSectorState((uint8)idxFeePhy_u16);
    }
#  endif

#  if FEE_PRV_FSIF_FEE2_ENABLED
    if(FEE_FS2 == configFS_en)
    {
        retVal_en = rba_FeeFs2_GetSectorState((uint8)idxFeePhy_u16);
    }
#  endif

#  if FEE_PRV_FSIF_FEE3_ENABLED
    if(FEE_FS3 == configFS_en)
    {
        retVal_en = rba_FeeFs3_GetRowState(idxFeePhy_u16);
    }
#  endif

    return (retVal_en);
}


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* FEE_PRV_CFG_COMMON_ENABLED */
# endif

/* FEE_PRV_FSIF_H */
#endif
