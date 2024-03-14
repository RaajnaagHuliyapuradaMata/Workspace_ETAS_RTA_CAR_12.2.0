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

#ifndef FEE_PRV_LIB_H
#define FEE_PRV_LIB_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "MemIf_Types.h"
#include "Fee_Rb_Types.h"
#include "Fee_Prv_Config.h"
#include "Fee_Prv_LibTypes.h"
#include "Fee_PrvTypes.h"
#include "Fls.h"
#include "rba_MemLib.h"

#  if (defined(FLS_RB_VENDOR_ID) && (0x06 == FLS_RB_VENDOR_ID))
#include "Fls_Rb_Idx_Access.h"
#  endif

#  if ((STD_ON == FEE_PRV_CFG_DEV_ERROR_DETECT) || (TRUE == FEE_PRV_CFG_ENABLE_RUNTIME_ERROR_REPORTING))
#include "Det.h"
#  endif


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/*
 * Set the state of a state machine struct to another state.
 * Then set the entry_b variable to indicate the state is newly entered.
 * Since C neither supports templates nor inheritance this is the only way to do it type safe.
 */
#define FEE_PRV_LIBTRANS(FSM_STRUCT, FSM_STATE) \
{                                               \
    (FSM_STRUCT).state_en   = (FSM_STATE);      \
    (FSM_STRUCT).entry_b    = (TRUE);           \
}

/* Change to the new state FSM_STATE of the state machine *fsm_pst. */
#define FEE_PRV_LIBSC(FSM_STATE) FEE_PRV_LIBTRANS((*fsm_pst), (FSM_STATE))

/* Re-enter the current state of the state machine *fsm_pst from the beginning. */
#define FEE_PRV_LIBSR()  (fsm_pst->entry_b = (TRUE))

/* Check whether a state of the state machine *fsm_pst is entered for the first or a later time. */
#define FEE_PRV_LIBENTRY (Fee_Prv_LibEnter(&fsm_pst->entry_b))


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CONST_8
#include "Fee_MemMap.h"

extern uint8                        const   Fee_Prv_LibEffortWeights_au8[FEE_PRV_LIMIT_MAX_E];

#define FEE_STOP_SEC_CONST_8
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern void Fee_Prv_LibCalcCrcRam(Fee_Rb_DeviceName_ten deviceName_en, uint8 * bfr_pu8, uint32 length_u32, uint32 * crc_pu32, boolean firstCall_b);
extern MemIf_JobResultType Fee_Prv_LibCalcCrcRamDo(Fee_Rb_DeviceName_ten deviceName_en);


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/

/*
 * \brief Cancel function for the Lib unit resetting all variables
 * For faster init (delay due to calling of function with parameter), the function is made inline.
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE void Fee_Prv_LibCancel(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_LibEffortMeasure_tst * libEffortMeasure_pst = &deviceConfigTable_pcst->feeData_pst->libData_st.libEffortMeasure_st;
    Fee_Prv_LibCalcCrcRam_tst*     libCalcCrcRam_pst    = &deviceConfigTable_pcst->feeData_pst->libData_st.libCalcCrcRam_st;

    libEffortMeasure_pst->effortCtr_u32 = 0u;
    libEffortMeasure_pst->enabled_b     = FALSE;

    libCalcCrcRam_pst->bfr_pu8          = NULL_PTR;
    libCalcCrcRam_pst->crc_pu32         = NULL_PTR;
    libCalcCrcRam_pst->firstCall_b      = TRUE;
    libCalcCrcRam_pst->length_u32       = 0u;
    libCalcCrcRam_pst->nrBytesDone_u32  = 0u;

    return;
}

/*
 * \brief init function for the Lib unit resetting all variables
 * For faster init (delay due to calling of function with parameter), the function is made inline.
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE void Fee_Prv_LibInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_LibCancel(deviceConfigTable_pcst);

    return;
}

/**
 * \brief   Forward an error to the Det module.
 *          AR4.2: If development error detection is not enabled, the errors are not forwarded to the Det module.
 *          AR4.4 and newer: If development error detection is not enabled, only runtime errors are forwarded to the Det module.
 *
 * \param   apiId_u8         The ID of the API
 * \param   deviceName_en    Device instance which had the error
 * \param   errorId_u8       The ID of the error
 */
LOCAL_INLINE void Fee_Prv_LibDetReport(Fee_Rb_DeviceName_ten deviceName_en, uint8 apiId_u8, uint8 errorId_u8)
{
    (void)deviceName_en;
    (void)apiId_u8;
    (void)errorId_u8;

#  if (FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION == 4) && (FEE_PRV_CFG_AR_RELEASE_MINOR_VERSION >=4)
    /* From Autosar 4.4 and newer, runtime error could also be reported to DET module.
     * However, it is possible to deactivate this reporting (by default the error is always reported) */
    if((FEE_E_BUSY == errorId_u8) || (FEE_E_INVALID_CANCEL == errorId_u8))
    {
#  if (TRUE == FEE_PRV_CFG_ENABLE_RUNTIME_ERROR_REPORTING)
        (void)Det_ReportRuntimeError (FEE_PRV_CFG_MODULE_ID, (uint8)deviceName_en, apiId_u8, errorId_u8);
#  endif
    }
#   if (STD_ON == FEE_PRV_CFG_DEV_ERROR_DETECT)
    else
    {
        (void)Det_ReportError(FEE_PRV_CFG_MODULE_ID, (uint8)deviceName_en, apiId_u8, errorId_u8);
    }
#   endif
#  elif (STD_ON == FEE_PRV_CFG_DEV_ERROR_DETECT)
    (void)Det_ReportError(FEE_PRV_CFG_MODULE_ID, (uint8)deviceName_en, apiId_u8, errorId_u8);
#  endif

    return;
}

/**
 * \brief   Check whether a state machine's state is entered for the first or a later time.
 *
 * \param   entry_pb    Pointer to the entry_b variable of the state machine
 *
 * \retval  TRUE    State is entered for the first time
 * \retval  FALSE   State is not entered for the first, but a later time
*/
LOCAL_INLINE boolean Fee_Prv_LibEnter(boolean * entry_pb)
{
    boolean result_b = FALSE;

    /* If not already entered then set the entry_b to FALSE and return TRUE (= state is entered) */
    if(*entry_pb)
    {
        result_b    = TRUE;
        *entry_pb   = FALSE;
    }

    return(result_b);
}

/**
 * \brief   Round value up to the next page border.
 *
 * \param   value_u32     Value to round up
 * \param   pageSize_u8   Page size to which value_u32 should be rounded up
 *
 * \return  Value rounded up to the next page border
 *
 * \attention   Whenever a flash must be supported which does NOT have 2^n page size this function must be changed!
*/
LOCAL_INLINE uint32 Fee_Prv_LibCeil(uint32 value_u32, uint8 pageSize_u8)
{
    /*
     * Example with value 0x12345 and page size 8.
     *
     *      0001 0010 0011 0100 0101
     *  &   0000 0000 0000 0000 0111 (VIRTUAL_PAGE_SIZE - 1)
     *  =   0000 0000 0000 0000 0101
     *  ~   1111 1111 1111 1111 1010
     * +1   1111 1111 1111 1111 1011
     *  &   0000 0000 0000 0000 0111 (VIRTUAL_PAGE_SIZE - 1)
     *  =   0000 0000 0000 0000 0011 (=3)
     *
     * Result: 0x12345 + 0x3 = 0x12348
     *
     * NOTE: VIRTUAL_PAGE_SIZE - 1 is a const expression and can be evaluated at compile time.
     * NOTE: This strategy only works with page sizes which are % 2 = 0!
     */
    uint32 summand_u32 = (((~(value_u32 & (uint32)(pageSize_u8 - 1u)))) + 1u) & (uint32)(pageSize_u8 - 1u);
    return(value_u32 + summand_u32);
}

/**
 * \brief   Round down an value to the next page border.
 *
 * \param   value_u32     Value to round down
 * \param   pageSize_u8   Page size to which value_u32 should be rounded up
 *
 * \return  Value rounded down to the next page border
 *
 * \attention   Whenever a flash must be supported which does NOT have 2^n page size this function must be changed!
*/
LOCAL_INLINE uint32 Fee_Prv_LibFloor(uint32 value_u32, uint8 pageSize_u8)
{
    /*
     * Example with value 0x12345 and page size 8.
     *
     *
     *      0000 0000 0000 0000 0111 (VIRTUAL_PAGE_SIZE - 1)
     *  ~   1111 1111 1111 1111 1000
     *  &   0001 0010 0011 0100 0101 (value)
     *  =   0001 0010 0011 0100 0000 (result)
     *
     * Result: 0x12345 => 0x12340
     *
     * NOTE: VIRTUAL_PAGE_SIZE - 1 is a const expression and can be evaluated at compile time.
     * NOTE: This strategy only works with page sizes which are % 2 = 0!
     */
    uint32 result_u32 = value_u32 & (~((uint32)(pageSize_u8 - 1u)));
    return(result_u32);
}

/**
 * \brief   Check whether the given data is equal to the erase pattern.
 *
 * \param   bfr_pcv         Buffer to the data
 * \param   length_u32      Amount of bytes to check for the erase pattern.
 * \param   eraseVal_u32    Erase value of the data flash
 *
 * \return  Function result
 * \retval  TRUE  if given data is equal to the erase pattern
 * \retval  FALSE if given data is different from the erase pattern
*/
LOCAL_INLINE boolean Fee_Prv_LibIsErased(void const * bfr_pcv, uint32 length_u32, uint8 eraseVal_u32)
{
    uint32          a_u32;
    /* MR12 RULE 11.5 VIOLATION: uint8 * alignment is not stricter than void * alignment */
    uint8   const * bfr_pcu8 = bfr_pcv;

    for(a_u32 = 0; a_u32 < length_u32; a_u32++)
    {
        if(eraseVal_u32 != bfr_pcu8[a_u32])
        {
            return(FALSE);
        }
    }
    return(TRUE);
}

/**
 * \brief   Set the effort counter to its initial value.
 *          That means the main function is allowed to do some work.
 *          Typically this is only called at the beginning of the main function.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the effort has to be reset.
*/
LOCAL_INLINE void Fee_Prv_LibEffortReset(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_LibEffortMeasure_tst * libEffortMeasure_pst = &deviceConfigTable_pcst->feeData_pst->libData_st.libEffortMeasure_st;
    libEffortMeasure_pst->effortCtr_u32 = Fee_Prv_ConfigMaxEffort_cu32;

    return;
}

/**
 * \brief   Check whether the main function is allowed to continue its work or whether it shall exit to not break timing limitations.
 *          This function is always called if the main function want to execute a time consuming action.
 *          It is possible to specify for each time consuming action what the "costs" of this action are.
 *          This function multiplies the number of time consuming actions the main function wants to do with a weight factor.
 *          Then it checks whether the main function has the right to execute this action (there was not yet so much work done).
 *          Or whether the main function already did too much and it is getting dangerous to continue regarding timing limitations.
 *          Example:
 *              The main function already calculated 2k of CRC in the RAM.
 *              Now it wants to calculate again 2k of CRC in the flash.
 *              Regarding only one of both actions it would be still ok and no timing constraints are violated.
 *              Regading both actions together it might get dangerous to violate timing constraints regarding main function execution time.
 *              With the help of this function the main function will notice that and calculate the 2k CRC in the flash the next time it is called.
 *
 * \param   id_en           Which operation shall be executed
 *                          Currently can be:
 *                          - CRC in RAM
 *                          - CRC in flash
 *                          - Handle one FAT entry
 * \param   requested_u32   How often shall the operation be executed
 *                          Can mean:
 *                          - Amount of bytes for CRC calculation
 *                          - Amount of FAT entries to handle
 *
 * \return  The main function may continue or it shall stop and continue with the next call
 *
 * \retval  MEMIF_JOB_OK        The main function can continue
 * \retval  MEMIF_JOB_PENDING   The main function shall stop and continue with its next call
*/
LOCAL_INLINE MemIf_JobResultType Fee_Prv_LibEffortLimit(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_LibEffortLimit_ten id_en, uint32 requested_u32)
{
    MemIf_JobResultType result_en = MEMIF_JOB_OK;
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_LibEffortMeasure_tst *       libEffortMeasure_pst   = &deviceConfigTable_pcst->feeData_pst->libData_st.libEffortMeasure_st;

    if(libEffortMeasure_pst->enabled_b)
    {
        /*
         * Weighting an action is only allowed logarithmic.
         * That means an action costs 1,2,4 or 8..
         * This is to ensure fast calculation since the action*weight can be calculated with bit shifting.
         */
        uint32 cost_u32 = requested_u32 << Fee_Prv_LibEffortWeights_au8[(uint32)id_en];
        if(cost_u32 <= libEffortMeasure_pst->effortCtr_u32)
        {
            libEffortMeasure_pst->effortCtr_u32 -= cost_u32;
        }
        else
        {
            libEffortMeasure_pst->effortCtr_u32 = 0u;
            result_en = MEMIF_JOB_PENDING;
        }
    }

    return(result_en);
}


/**
 * \brief   Request an amount of work for a specific action.
 *          If it not possible to do all the work in one main function then return as much as possible.
 *
 * \param   id_en           ID of the action
 * \param   requested_u32   Requested amount of work to do
 *
 * \return  Allowed amount of work to do in this main function call
 *
 * \see also Fee_Prv_LibEffortLimit
*/
LOCAL_INLINE uint32 Fee_Prv_LibEffortGet(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_LibEffortLimit_ten id_en, uint32 requested_u32)
{
    uint32 result_u32 = requested_u32;
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_LibEffortMeasure_tst *       libEffortMeasure_pst   = &deviceConfigTable_pcst->feeData_pst->libData_st.libEffortMeasure_st;

    if(libEffortMeasure_pst->enabled_b)
    {
        uint32 cost_u32                        = requested_u32 << Fee_Prv_LibEffortWeights_au8[id_en];
        result_u32                             = rba_MemLib_Min(cost_u32, libEffortMeasure_pst->effortCtr_u32);
        libEffortMeasure_pst->effortCtr_u32   -= result_u32;
        result_u32                           >>= Fee_Prv_LibEffortWeights_au8[id_en];
    }

    return(result_u32);
}

/**
 * \brief   Enable or disable the usage of the effort limitation.
 *          During init this is disabled since the only goal is to initalize as fast as possible.
 *          During cyclic operation this is enabled in order to not violate task execution time constraints.
*/
LOCAL_INLINE void Fee_Prv_LibUseEffortLimit(Fee_Rb_DeviceName_ten deviceName_en, boolean use_b)
{
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_LibEffortMeasure_tst *       libEffortMeasure_pst   = &deviceConfigTable_pcst->feeData_pst->libData_st.libEffortMeasure_st;
    libEffortMeasure_pst->enabled_b = use_b;

    return;
}

/**
 * \brief   function checks if there was any effort spent or not.
 *          if no effort spent, it means that the debug main function is allowed to do some work.
 *          Currently this is only called in debug main function before writing the debug block.
 *
 * \param   deviceName_en   Device instance for which information is requested
 *
 * \return  TRUE            No effort has been spent by other job
 *          FALSE           Some effort has been spent by other job
*/
LOCAL_INLINE boolean Fee_Prv_LibNoEffortSpent(Fee_Rb_DeviceName_ten deviceName_en)
{
    boolean retVal_b;

    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_LibEffortMeasure_tst *       libEffortMeasure_pst   = &deviceConfigTable_pcst->feeData_pst->libData_st.libEffortMeasure_st;

    if(libEffortMeasure_pst->effortCtr_u32 == Fee_Prv_ConfigMaxEffort_cu32)
    {
        retVal_b = TRUE;
    }
    else
    {
        retVal_b = FALSE;
    }

    return(retVal_b);
}

#  if (defined(FEE_PRV_CFG_MULTIINSTANCE_ENABLED) && (TRUE == FEE_PRV_CFG_MULTIINSTANCE_ENABLED))
/* Multi instance is enabled. Route the accesses to Fls via the Fls_Rb_Idx_ functions */
LOCAL_INLINE void Fee_Prv_Lib_Fls_Cancel(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
#   if (defined(FLS_CANCEL_API) && (STD_OFF != FLS_CANCEL_API))
    Fls_Rb_Idx_CancelAdr(idDevice, TargetAddress);
#   endif

    return;
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_EnterHighSpeedMode(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
#   if (defined(FLS_RB_HIGH_SPEED_MODE_API) && (STD_OFF != FLS_RB_HIGH_SPEED_MODE_API))
    return(Fls_Rb_Idx_EnterHighSpeedModeAdr(idDevice, TargetAddress));
#   else
    (void)idDevice;
    (void)TargetAddress;
    return (E_NOT_OK);
#   endif
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_ExitHighSpeedMode(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
#   if (defined(FLS_RB_HIGH_SPEED_MODE_API) && (STD_OFF != FLS_RB_HIGH_SPEED_MODE_API))
    return(Fls_Rb_Idx_ExitHighSpeedModeAdr(idDevice, TargetAddress));
#   else
    (void)idDevice;
    (void)TargetAddress;
    return (E_NOT_OK);
#   endif
}

LOCAL_INLINE void Fee_Prv_Lib_Fls_MainFunction(Fls_Rb_FlsDevice_ten idDevice)
{
    Fls_Rb_Idx_MainFunction(idDevice, TRUE);

    return;
}

LOCAL_INLINE MemIf_JobResultType Fee_Prv_Lib_Fls_GetJobResult(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
    return (Fls_Rb_Idx_GetJobResultAdr(idDevice, TargetAddress));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Write(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress,
                                                     const uint8* SourceAddressPtr, Fls_Rb_LengthType Length, boolean isTraErrSuppReqd_b)
{
    uint8 jobDetail_u8;

    if(isTraErrSuppReqd_b)
    {
        jobDetail_u8 = (uint8)FLS_RB_ST_DETAIL_WRITE_WITHOUT_TRANS;
    }
    else
    {
        jobDetail_u8 = (uint8)FLS_RB_ST_DETAIL_STD;
    }

    return (Fls_Rb_Idx_Write(idDevice, TargetAddress, SourceAddressPtr, Length, jobDetail_u8));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Read(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType SourceAddress,
                                                    uint8* TargetAddressPtr, Fls_Rb_LengthType Length)
{
    return (Fls_Rb_Idx_Read(idDevice, SourceAddress, TargetAddressPtr, Length));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Erase(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress, Fls_Rb_LengthType Length)
{
    return (Fls_Rb_Idx_Erase(idDevice, TargetAddress, Length,FLS_RB_ST_DETAIL_STD));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Compare(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType SourceAddress,
                                                       const uint8* TargetAddressPtr, Fls_Rb_LengthType Length)
{
    return (Fls_Rb_Idx_Compare(idDevice, SourceAddress, TargetAddressPtr, Length));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_BlankCheck(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress,
                                                          Fls_Rb_LengthType Length)
{
#   if (defined(FLS_BLANK_CHECK_API) && (STD_OFF != FLS_BLANK_CHECK_API))
    return (Fls_Rb_Idx_BlankCheck(idDevice, TargetAddress, Length));
#   else
    (void)idDevice;
    (void)TargetAddress;
    (void)Length;
    return (E_NOT_OK);
#   endif
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Rb_GetSectorInfo(
        Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress, Fls_Rb_SectorInfo_tst * SectorInfo_pst)
{
    /* Note: In case the element SectorInfo_pst.adSectorStart_uo is used, then the calling function should also handle
     * the relative/absolute addressing. Ex. see the implementation + usuage of Fee_Prv_MediumGetWlErrorAdr(). */
#   if (defined(FLS_RB_GET_SECTOR_INFO) && (STD_OFF != FLS_RB_GET_SECTOR_INFO))
    return (Fls_Rb_Idx_GetSectorInfo(idDevice, TargetAddress, SectorInfo_pst));
#   else
    (void)idDevice;
    (void)TargetAddress;
    SectorInfo_pst->nrSpecifiedEraseCycles_u32 = 0u;
    return (E_NOT_OK);
#   endif
}

#  else
/****************************************************************************************************/
/* Single instance of Fee is used. Route the accesses to Fls via the standard AUTOSAR Fls functions */
/****************************************************************************************************/

LOCAL_INLINE void Fee_Prv_Lib_Fls_Cancel(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
    (void)idDevice;
    (void)TargetAddress;
#   if (defined(FLS_CANCEL_API_AVAIL) && (STD_OFF != FLS_CANCEL_API_AVAIL))
    Fls_Cancel();
#   endif

    return;
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_EnterHighSpeedMode(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
    (void)idDevice;
    (void)TargetAddress;
#   if (defined(FLS_RB_HIGH_SPEED_MODE_API_AVAIL) && (STD_OFF != FLS_RB_HIGH_SPEED_MODE_API_AVAIL))
    return(Fls_Rb_EnterHighSpeedMode());
#   else
    return (E_NOT_OK);
#   endif
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_ExitHighSpeedMode(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
    (void)idDevice;
    (void)TargetAddress;
#   if (defined(FLS_RB_HIGH_SPEED_MODE_API_AVAIL) && (STD_OFF != FLS_RB_HIGH_SPEED_MODE_API_AVAIL))
    return(Fls_Rb_ExitHighSpeedMode());
#   else
    return (E_NOT_OK);
#   endif
}

LOCAL_INLINE void Fee_Prv_Lib_Fls_MainFunction(Fls_Rb_FlsDevice_ten idDevice)
{
    (void)idDevice;
#   if (defined(FLS_RB_VENDOR_ID) && (0x06 == FLS_RB_VENDOR_ID))
    Fls_Rb_MainFunctionAndDependency();
#   else
    Fls_MainFunction();
#   endif

    return;
}

LOCAL_INLINE MemIf_JobResultType Fee_Prv_Lib_Fls_GetJobResult(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress)
{
    (void)idDevice;
    (void)TargetAddress;
    return (Fls_GetJobResult());
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Write(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress,
                                                     const uint8* SourceAddressPtr, Fls_Rb_LengthType Length, boolean isTraErrSuppReqd_b)
{
    (void)idDevice;

#  if defined(FLS_RB_WRITE_WITHOUT_TRANS_API_AVAIL) && (FLS_RB_WRITE_WITHOUT_TRANS_API_AVAIL != STD_OFF)
    if(isTraErrSuppReqd_b)
    {
        return (Fls_Rb_WriteWithoutTransientError(TargetAddress, SourceAddressPtr, Length));
    }
    else
    {
        return (Fls_Write(TargetAddress, SourceAddressPtr, Length));
    }
#  else
    (void)isTraErrSuppReqd_b;
    return (Fls_Write(TargetAddress, SourceAddressPtr, Length));
#  endif
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Read(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType SourceAddress,
                                                    uint8* TargetAddressPtr, Fls_Rb_LengthType Length)
{
    (void)idDevice;
    return (Fls_Read(SourceAddress, TargetAddressPtr, Length));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Erase(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress, Fls_Rb_LengthType Length)
{
    (void)idDevice;
    return (Fls_Erase(TargetAddress, Length));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Compare(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType SourceAddress,
                                                       const uint8* TargetAddressPtr, Fls_Rb_LengthType Length)
{
    (void)idDevice;
    return (Fls_Compare(SourceAddress, TargetAddressPtr, Length));
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_BlankCheck(Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress,
                                                          Fls_Rb_LengthType Length)
{
    (void)idDevice;
#   if (defined(FLS_BLANK_CHECK_API_AVAIL) && (STD_OFF != FLS_BLANK_CHECK_API_AVAIL))
    return (Fls_BlankCheck(TargetAddress, Length));
#   else
    (void)TargetAddress;
    (void)Length;
    return (E_NOT_OK);
#   endif
}

LOCAL_INLINE Std_ReturnType Fee_Prv_Lib_Fls_Rb_GetSectorInfo(
        Fls_Rb_FlsDevice_ten idDevice, Fls_Rb_AddressType TargetAddress, Fls_Rb_SectorInfo_tst * SectorInfo_pst)
{
    (void)idDevice;
#   if (defined(FLS_RB_GET_SECTOR_INFO_AVAIL) && (STD_OFF != FLS_RB_GET_SECTOR_INFO_AVAIL))
    /* Note: In case the element SectorInfo_pst.adSectorStart_uo is used, then the calling function should also handle
     * the relative/absolute addressing. Ex. see the implementation + usuage of Fee_Prv_MediumGetWlErrorAdr(). */
    return (Fls_Rb_GetSectorInfo(TargetAddress, SectorInfo_pst));
#   else
    (void)TargetAddress;
    SectorInfo_pst->nrSpecifiedEraseCycles_u32 = 0u;
    return (E_NOT_OK);
#   endif
}

// single instance
#  endif


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* FEE_PRV_CFG_COMMON_ENABLED */
# endif

/* FEE_PRV_LIB_H */
#endif
