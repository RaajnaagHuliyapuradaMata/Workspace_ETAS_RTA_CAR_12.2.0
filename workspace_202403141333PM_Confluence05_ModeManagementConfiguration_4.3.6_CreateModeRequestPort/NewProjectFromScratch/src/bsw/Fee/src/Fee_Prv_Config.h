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

#ifndef FEE_PRV_CONFIG_H
#define FEE_PRV_CONFIG_H


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "Fee_Prv_ConfigTypes.h"
#include "Fee_Cfg.h"

// To avoid build abort due to cyclic dependency of header file inclusions, the Config table type is created in this file instead of Fee_Prv_ConfigTypes.h
#include "Fee_Prv_JobTypes.h"
#include "Fee_Prv_DbgTypes.h"
#include "Fee_Prv_LibTypes.h"
#include "Fee_Prv_MediumTypes.h"
#include "Fee_Prv_PersTypes.h"
#include "Fee_Prv_ProcTypes.h"


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* RAM variable for Fee compoment. Here all the data objects of different units are collected. */
typedef struct
{
    Fee_Prv_DbgData_tst       dbgData_st;
    Fee_Prv_PersData_tst      persData_st;
    Fee_Prv_JobData_tst       jobData_st;
    Fee_Prv_LibData_tst       libData_st;
    Fee_Prv_MediumData_tst    mediumData_st;
    Fee_Prv_ProcData_tst      procData_st;
}Fee_Prv_Data_tst;

/* Constants required for medium buffer handling */
typedef struct
{
    uint8 *    mediumBuf_pu8;            // Pointer to the medium buffer
    uint32     mediumBufSize_u32;        // Size of the medium buffer, should support >= 64k
    uint8 *    persDataBuf_pu8;          // Pointer to buffer of persistent data unit
}Fee_Prv_ConfigDeviceBufTable_tst;

/* Data model strcture for each instance of Fee component */
typedef struct
{
    Fee_Prv_Data_tst *                       feeData_pst;                       // RAM variables for all the units in Fee
    const Fee_Prv_ConfigDeviceBufTable_tst * feeBufTable_pcast;                 // pointer to Fee buffer table
    uint8 * const                            mediumRescueBfr_pcu8;              // Buffer for FEE_PRV_MEDIUM_WL_ERROR_HNDLG_ENABLED feature
    Fee_Prv_PersSavedZone_tst *              persSavedZone_pst;                 // Reset-save data of persistent data unit
    void                                     (* syncLoopFlsMainHook_pfn)(void); // Fls hook function for initialisation
    Fee_Rb_DeviceName_ten                    deviceName_en;                     // Device name of the instance
    Fee_Prv_Fs_ten                           fs_en;                             // Type of FeeFsx used by this instance
    uint16                                   persDataBlkNr_u16;                 // Block number of the Fee-internal persistent data block
    uint8                                    persDataSizeHl_u8;                 // Size of Fee high level data inside Fee-internal block
    uint8                                    deviceIdx_u8;                      // Configured FeeRbGeneralDeviceIndex for the instance
}Fee_Prv_ConfigDeviceTable_tst;


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/
#define FEE_START_SEC_CONST_32
#include "Fee_MemMap.h"

extern uint32 const Fee_Prv_ConfigMaxEffort_cu32;

#define FEE_STOP_SEC_CONST_32
#include "Fee_MemMap.h"

#define FEE_START_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"

extern const Fee_Prv_ConfigDeviceTable_tst Fee_Prv_ConfigDeviceTable_cast[Fee_Rb_Device_Max];

#define FEE_STOP_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern uint16                           Fee_Prv_ConfigGetBlockLengthByBlockNr               (uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern uint16                           Fee_Prv_ConfigGetBlockLengthByPersistentId          (uint16 persistentId_u16, uint16 nrOfBlks_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern uint16                           Fee_Prv_ConfigGetBlockNrByPersistentId              (uint16 persistentId_u16, uint16 nrOfBlks_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern uint16                           Fee_Prv_ConfigGetBlockPersistentIdByBlockNr         (uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern uint16                           Fee_Prv_ConfigGetBlockStatusBitsByBlockNr           (uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern Fee_Rb_User_ten                  Fee_Prv_ConfigGetBlockUserByBlockNr                 (uint16 blockNr_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern uint32                           Fee_Prv_ConfigGetPhysStartAddress                   (uint8 physSectNr_u8, const Fee_Rb_FlashProp_tst* flashPropTable_pcst);
extern uint32                           Fee_Prv_ConfigGetPhysEndAddress                     (uint8 physSectNr_u8, const Fee_Rb_FlashProp_tst* flashPropTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockMigrationActiveByBlockNr       (uint16 blkNr_u16, uint16 migrationBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockMigrationActiveByPersistentId  (uint16 persistentId_u16, uint16 nrOfBlks_u16, uint16 migrationBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockLengthVariableByBlockNr        (uint16 blockNr_u16, uint16 varLenBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockRobustnessActiveByBlockNr      (uint16 blockNr_u16, uint16 robBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr   (uint16 blockNr_u16, uint16 redBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockSurvivalActiveByBlockNr        (uint16 blockNr_u16, uint16 survivalBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr      (uint16 blockNr_u16, uint16 noFBBitmask_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern boolean                          Fee_Prv_ConfigIsBlockKnownByPersistentId            (uint16 persistentId_u16, uint16 nrOfBlks_u16, const Fee_Rb_BlockPropertiesType_tst* blockPropertiesTable_pcst);
extern Fee_Rb_DeviceName_ten            Fee_Prv_ConfigGetDeviceNameFromDeviceIndex          (uint8 feeDeviceIdx_u8);

#  if(defined(FEE_PRV_CFG_MULTIINSTANCE_ENABLED) && (FEE_PRV_CFG_MULTIINSTANCE_ENABLED != FALSE))

// multi instance is being used, so call the corresponding function in Fee_Order unit to return the configured file system.
extern Fee_Prv_Fs_ten                       Fee_Prv_ConfigGetFSFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en);
extern Fee_Prv_ConfigDeviceTable_tst const* Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en);
extern uint16                               Fee_Prv_ConfigGetBlockNumberOfPersistentData(Fee_Rb_DeviceName_ten deviceName_en);

#  else


/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/

// single instance is being used, to optimize code size, directly map the return value based on the configured file system.
LOCAL_INLINE Fee_Prv_Fs_ten                       Fee_Prv_ConfigGetFSFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE Fee_Prv_ConfigDeviceTable_tst const* Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en);
LOCAL_INLINE uint16                               Fee_Prv_ConfigGetBlockNumberOfPersistentData(Fee_Rb_DeviceName_ten deviceName_en);


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

LOCAL_INLINE Fee_Prv_ConfigDeviceTable_tst const * Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en)
{
    (void)deviceName_en;
    return(&Fee_Prv_ConfigDeviceTable_cast[0u]);
}

LOCAL_INLINE Fee_Prv_Fs_ten Fee_Prv_ConfigGetFSFromDeviceName(Fee_Rb_DeviceName_ten deviceName_en)
{
    (void)deviceName_en;
#   if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))
    return(FEE_FS1X);
#   elif(defined(RBA_FEEFS2_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS2_PRV_CFG_ENABLED))
    return(FEE_FS2);
#   elif(defined(RBA_FEEFS3_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS3_PRV_CFG_ENABLED))
    return(FEE_FS3);
#   else
    return(FEE_FS10);
#   endif
}

LOCAL_INLINE uint16 Fee_Prv_ConfigGetBlockNumberOfPersistentData(Fee_Rb_DeviceName_ten deviceName_en)
{
    (void)deviceName_en;
    return(Fee_Prv_ConfigDeviceTable_cast[0u].persDataBlkNr_u16);
}

#  endif


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* FEE_PRV_CFG_COMMON_ENABLED */
# endif

/* FEE_PRV_CONFIG_H */
#endif
