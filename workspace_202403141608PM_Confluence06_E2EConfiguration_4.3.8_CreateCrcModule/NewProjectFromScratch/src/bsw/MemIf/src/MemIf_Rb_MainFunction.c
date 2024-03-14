/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/


/* Check for availability of Rte */
#include "rba_MemLib_UseRte.h"
#include "MemIf_Cfg.h"

/* If Rte is available then hide internal declarations */
#if (defined(RBA_MEMLIB_USE_RTE) && (MEMIF_RB_RTE_MAINFUNC_USED == STD_ON))
#define MEMIF_RB_HIDE_RTEAPI
#endif

/**************************************************/

#include "MemIf.h"
#include "MemIf_Cfg_SchM.h"

/* Special Rte includes if Rte is available (as they are hidden in header) */
#if defined(MEMIF_RB_HIDE_RTEAPI)
#include "SchM_MemIf.h"
#endif

/*
 *********************************************************************
     Macros
 *********************************************************************
 */


/* Protection selectable via MemIf_Cfg_SchM.h to save code */
#ifndef MEMIF_RB_USE_PROTECTION
#define MEMIF_RB_USE_PROTECTION   (TRUE)
#endif


/*
 *********************************************************************
     Variables
 *********************************************************************
 */


#define MEMIF_START_SEC_VAR_INIT_BOOLEAN
#include "MemIf_MemMap.h"

/* Flag to hold the main function access status */
static volatile boolean MemIf_Prv_flgUsedSema_b = FALSE;

/* Flag to hold the debug information, Reentrant MainFunction */
static volatile boolean MemIf_Prv_dbgReentrantMainFunction_b = FALSE;

#define MEMIF_STOP_SEC_VAR_INIT_BOOLEAN
#include "MemIf_MemMap.h"


/*
 *********************************************************************
     Code Area
 *********************************************************************
 */


#define MEMIF_START_SEC_CODE
#include "MemIf_MemMap.h"

/**
 *********************************************************************
 * MemIf_Prv_TryToGetLock(): Detection and protection of re-entrant
 * execution of MainFunction
 *
 * The function check the global flag for re-entrant invokation of
 * Mainfunction and reports if Det enabled
 *
 * \param    none
 * \retval   TRUE : Lock acquired or protection disabled
 * \retval   FALSE: Re-entrant invocation of MainFunction
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE boolean MemIf_Prv_TryToGetLock(void)
{
    boolean isLockAvailable_b = FALSE;

#if(MEMIF_RB_USE_PROTECTION != FALSE)
    /* Disable interrupts */
    SchM_Enter_MemIf_Main();
#endif

    /* Is the Main function not invoked yet, then reserve it and allow the execution */
    if(!MemIf_Prv_flgUsedSema_b)
    {
        MemIf_Prv_flgUsedSema_b = TRUE;
        isLockAvailable_b = TRUE;
    }

#if(MEMIF_RB_USE_PROTECTION != FALSE)
    /* Enable interrupts */
    SchM_Exit_MemIf_Main();
#endif

    if(!isLockAvailable_b)
    {
        /* Set the debug flag for the detection of reentrant main function  */
        MemIf_Prv_dbgReentrantMainFunction_b = TRUE;

        MemIf_Prv_DetReportError(MEMIF_RB_SID_MAIN_FUNCTION, MEMIF_RB_E_BUSY);
    }

#if(MEMIF_RB_USE_PROTECTION == FALSE)
    /* Protection disabled, only detection by default */
    isLockAvailable_b = TRUE;
#endif

    return isLockAvailable_b;
}

/**
 *********************************************************************
 * MemIf_Prv_ReleaseLock(): Reset the flag having reentrant information
 *
 * \param    none
 * \retval   none
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void MemIf_Prv_ReleaseLock(void)
{
    /* Reset the access flag */
    MemIf_Prv_flgUsedSema_b = FALSE;
    return;
}

/**
 *********************************************************************
 * MemIf_Rb_MainFunction(): Calls requiered underlaying
 *                          Mainfunctions.
 *
 * The function calls the required underlaying main functions,
 * depending on which modules are configured.
 *
 * \param    none
 * \retval   none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void MemIf_Rb_MainFunction(void)
{
    /* Local flag to store the lock accuring status */
    boolean flgUsed_b = FALSE;

    /* Check for re-entrant invocation of MainFunction*/
    flgUsed_b = MemIf_Prv_TryToGetLock();

    if(flgUsed_b)
    {

#if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
        Fee_MainFunction();
        Fls_MainFunction();
#endif
#if (defined(MEMIF_NUM_OF_EA_DEVICES) && (MEMIF_NUM_OF_EA_DEVICES > 0u))
        Ea_MainFunction();
        Eep_MainFunction();
        Spi_MainFunction_Handling();
#endif

        /* Release the lock */
        MemIf_Prv_ReleaseLock();
    }

    return;
}

/**
 *********************************************************************
 * MemIf_Rb_MainFunctionAndDependency(): Calls underlaying
 *                          Mainfunctions and dependencies of all instances.
 *
 * The function calls the required underlaying main functions and dependencies.
 *
 * \param    none
 * \retval   none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void MemIf_Rb_MainFunctionAndDependency (void)
{
#if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
# if (defined(MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))

    uint8 deviceIndex_u8;

    for (deviceIndex_u8 = 0u; deviceIndex_u8 < MEMIF_MAX_NUM_DEVICES; deviceIndex_u8++)
    {
        if (Fee_Rb_IsThisDeviceIndexFee(deviceIndex_u8))
        {
            Fee_Rb_Idx_MainFunctionAndDependency(Fee_Rb_GetDeviceNameFromDeviceIndex(deviceIndex_u8));
        }
    }
# else
    Fee_MainFunction();

#  if defined(FLS_MAIN_FUNCTION_API_AVAIL)
    /* only useable in Fls standard configuration */
    Fls_Rb_MainFunctionAndDependency();
#  else
    /* for limited functionality call at least Fls */
    Fls_MainFunction();
#  endif

# endif /* MEMIF_RB_NUM_OF_FEE_DEVICES */

#endif /* MEMIF_FEE_USED */

#if (defined(MEMIF_NUM_OF_EA_DEVICES) && (MEMIF_NUM_OF_EA_DEVICES > 0u))
    Ea_Rb_MainFunctionAndDependency();
#endif /* MEMIF_NUM_OF_EA_DEVICES */

    return;
}


#if (defined(MEMIF_FEE_AND_EA_USED) && (MEMIF_FEE_AND_EA_USED == STD_ON))
/**
 *********************************************************************
 * MemIf_Rb_BlockMaintenance(): Calls corresponding block maintenance functions.
 *
 * The function calls the required underlying block maintenance functions,
 * depending on which modules are configured.
 *
 * \param    Block number
 * \retval   E_OK:      Order was accepted
 * \retval   E_NOT_OK:  Order was not accepted.
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType MemIf_Rb_BlockMaintenance(uint8 DeviceIndex, uint16 BlockNumber)
{
    Std_ReturnType xRetVal = E_NOT_OK;       /* Default return value */

    (void)DeviceIndex;
    (void)BlockNumber;

    MemIf_CheckDeviceIndex(DeviceIndex);

# if (defined (MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))
    if(Fee_Rb_IsThisDeviceIndexFee(DeviceIndex))
    {
#  if( (!(defined(FEE_RB_IDX_MAINTAIN))) || (defined(FEE_RB_IDX_MAINTAIN) && (FEE_RB_IDX_MAINTAIN != FALSE)) )
        xRetVal = Fee_Rb_Idx_BlockMaintenance(Fee_Rb_GetDeviceNameFromDeviceIndex(DeviceIndex),BlockNumber);
#  else
        /* Intentionally empty - will return E_NOT_OK */
#  endif
    }
# else
    if (DeviceIndex == 0u)
    {
#  if( !defined(FEE_RB_IDX_MAINTAIN) || (FEE_RB_IDX_MAINTAIN != FALSE))
        xRetVal = Fee_Rb_BlockMaintenance(BlockNumber);
#  else
        /* Intentionally empty - will return E_NOT_OK */
#  endif
    }
# endif
    else
    {
        xRetVal = Ea_Rb_BlockMaintenance(BlockNumber);
    }
    return(xRetVal);
}
#endif

#if (defined(MEMIF_FEE_AND_EA_USED) && (MEMIF_FEE_AND_EA_USED == STD_ON))
/**
 *********************************************************************
 * MemIf_VarLenRead(): MemIf_VarLenRead interface
 *
 * This function places a read job to the FEE when runtime RAM block
 * configuration feature is enabled and the block length is variable.
 *
 * \param    DeviceIndex:   Index of requeste device
 * \param    Blocknumber:   Nvm index (based on this index the
 *                          corresponding Fee index will be searched
 *                          and placed into the order queue)
 * \param    BlockOffset:   Offset of block
 * \param    DataBufferPtr: Pointer to data buffer
 * \param    Length:        Length of block
 * \return   Function success
 * \retval   E_OK:              Order was successfully placed
 * \retval   E_NOT_OK:          Order was not placed because the
 *                              respective order queue entry is
 *                              not empty or the module has not been
 *                              initialized yet
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType MemIf_Rb_VarLenRead(uint8 DeviceIndex, uint16 BlockNumber, uint16 BlockOffset, uint8 * DataBufferPtr, uint16 Length)
{
    Std_ReturnType xRetVal = E_NOT_OK;       /* Default return value */

    (void)DeviceIndex;
    (void)BlockNumber;
    (void)BlockOffset;
    (void)DataBufferPtr;
    (void)Length;

    MemIf_CheckDeviceIndex(DeviceIndex);

# if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))

#  if (defined (MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))
    if(Fee_Rb_IsThisDeviceIndexFee(DeviceIndex))
    {
#   if(( !defined(FEE_RB_IDX_VAR_LEN_READ_WRITE_API) || (FEE_RB_IDX_VAR_LEN_READ_WRITE_API != FALSE)) || ( !defined(FEE_RB_IDX_FIRST_READ_DATA_MIGRATION_API) || (FEE_RB_IDX_FIRST_READ_DATA_MIGRATION_API != FALSE)))
        xRetVal = Fee_Rb_Idx_VarLenRead(Fee_Rb_GetDeviceNameFromDeviceIndex(DeviceIndex),BlockNumber,BlockOffset,
                                                                                DataBufferPtr,Length);
#   else
/* Intentionally empty - will return E_NOT_OK */
#   endif
    }
#  else
    if (DeviceIndex == 0u)
    {
#   if(( !defined(FEE_RB_VAR_LEN_READ_WRITE_API) || (FEE_RB_VAR_LEN_READ_WRITE_API != FALSE)) || ( !defined(FEE_RB_FIRST_READ_DATA_MIGRATION_API) || (FEE_RB_FIRST_READ_DATA_MIGRATION_API != FALSE)))
        /* Block is configured for Fee (Flash) */
        xRetVal = Fee_Rb_VarLenRead(BlockNumber,BlockOffset,DataBufferPtr,Length);
#   else
        /* Intentionally empty - will return E_NOT_OK */
#   endif
    }
#  endif
# endif

    return(xRetVal);
}
#endif

#if (defined(MEMIF_FEE_AND_EA_USED) && (MEMIF_FEE_AND_EA_USED == STD_ON))
/**
 *********************************************************************
 * MemIf_Rb_VarLenWrite(): MemIf_Rb_VarLenWrite interface
 *
 * This function places a write job to the FEE when runtime RAM block
 * configuration feature is enabled and the block length is variable.
 *
 * \param    DeviceIndex:   Index of requested device
 * \param    Blocknumber:   Nvm index (based on this index the
 *                          corresponding Fee index will be searched
 *                          and placed into the order queue)
 * \param    DataBufferPtr: Pointer Pointer to data buffer
 * \param    Length:        NV block length
 * \return   Function success
 * \retval   E_OK:              Order was successfully placed
 * \retval   E_NOT_OK:          Order was not placed because the
 *                              respective order queue entry is
 *                              not empty or the module has not been
 *                              initialized yet
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType MemIf_Rb_VarLenWrite(uint8 DeviceIndex, uint16 BlockNumber, uint8 * DataBufferPtr, uint16 Length)
{
    Std_ReturnType xRetVal = E_NOT_OK;   /* Default return value */

    (void)DeviceIndex;
    (void)BlockNumber;
    (void)DataBufferPtr;
    (void)Length;

    MemIf_CheckDeviceIndex(DeviceIndex);

# if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
#  if (defined (MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))
    if(Fee_Rb_IsThisDeviceIndexFee(DeviceIndex))
    {
#   if( !defined(FEE_RB_IDX_VAR_LEN_READ_WRITE_API) || (FEE_RB_IDX_VAR_LEN_READ_WRITE_API != FALSE))
        xRetVal = Fee_Rb_Idx_VarLenWrite(Fee_Rb_GetDeviceNameFromDeviceIndex(DeviceIndex),BlockNumber,
                                                                                DataBufferPtr,Length);
#   else
/* Intentionally empty - will return E_NOT_OK */
#   endif
    }
#  else
    if (DeviceIndex == 0u)
    {
        /* Block is configured for Fee (Flash) */
#   if( !defined(FEE_RB_VAR_LEN_READ_WRITE_API) || (FEE_RB_VAR_LEN_READ_WRITE_API != FALSE))
        xRetVal = Fee_Rb_VarLenWrite(BlockNumber,DataBufferPtr,Length);
#   else
        /* Intentionally empty - will return E_NOT_OK */
#   endif
    }
#  endif
# endif

    return(xRetVal);
}
#endif

#if (defined(MEMIF_FEE_AND_EA_USED) && (MEMIF_FEE_AND_EA_USED == STD_ON))
/**
 *********************************************************************
 * MemIf_Rb_GetMigrationResult(): MemIf_Rb_GetMigrationResult interface
 *
 * Service returns the migration result of the last data reading
 * during initialisation (calling the API after subsequent read jobs
 * in the same driving cycle does not change the initially returned
 * value anymore).
 *
 * \param    DeviceIndex:   Index of requested device
 * \param    BlockNumber:   Corresponding Fee or Ea block number
 *
 * \retval   MEMIF_RB_MIGRATION_RESULT_INIT_E
 * \retval   MEMIF_RB_MIGRATION_RESULT_NOT_NECESSARY_E
 * \retval   MEMIF_RB_MIGRATION_RESULT_TO_SMALLER_SIZE_E
 * \retval   MEMIF_RB_MIGRATION_RESULT_TO_BIGGER_SIZE_E
 * \retval   MEMIF_RB_MIGRATION_RESULT_NOT_DONE_E
 * \retval   MEMIF_RB_MIGRATION_RESULT_DEACTIVATED_E
 *
 * \seealso
 * \usedresources
 *********************************************************************
 */
MemIf_Rb_MigrationResult_ten MemIf_Rb_GetMigrationResult(uint8 DeviceIndex, uint16 BlockNumber)
{
    MemIf_Rb_MigrationResult_ten xRetVal = MEMIF_RB_MIGRATION_RESULT_INIT_E;  /* Default return value */

    (void)DeviceIndex;
    (void)BlockNumber;

    MemIf_CheckDeviceIndex(DeviceIndex);

# if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
#  if (defined (MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))
    if(Fee_Rb_IsThisDeviceIndexFee(DeviceIndex))
    {
#   if( !defined(FEE_RB_IDX_FIRST_READ_DATA_MIGRATION_API) || (FEE_RB_IDX_FIRST_READ_DATA_MIGRATION_API != FALSE))
        xRetVal = Fee_Rb_Idx_GetMigrationResult(Fee_Rb_GetDeviceNameFromDeviceIndex(DeviceIndex),BlockNumber);
#   else
/* Intentionally empty - will return MEMIF_RB_MIGRATION_RESULT_INIT_E */
#   endif
   }
#  else
    if (DeviceIndex == 0u)
    {
        /* Block is configured for Fee (Flash) */
#   if( !defined(FEE_RB_FIRST_READ_DATA_MIGRATION_API) || (FEE_RB_FIRST_READ_DATA_MIGRATION_API != FALSE))
        xRetVal = Fee_Rb_GetMigrationResult(BlockNumber);
#   else
/* Intentionally empty - will return MEMIF_RB_MIGRATION_RESULT_INIT_E */
#   endif
    }
#  endif
# endif

    return(xRetVal);
}
#endif

#if (defined(MEMIF_FEE_AND_EA_USED) && (MEMIF_FEE_AND_EA_USED == STD_ON))
/**
 *********************************************************************
 * MemIf_Rb_GetDetailedBlkInfo(): MemIf_Rb_GetDetailedBlkInfo interface
 *
 * Service returns the detailed block info regarding valid block header,
 * data CRC and block length.
 * Calls the underlaying module API's depending on configured device
 *
 * \param    DeviceIndex:   Index of requested device
 * \param    BlockNumber:   Corresponding Fee or Ea block number
 *
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E          Job not supported by Fee/Ea
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INIT_E                   Block was never read inconsitent - result not available
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E   MEMIF_BLOCK_INCONSISTENT: Valid block (header) was not found on the medium
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_DATA_E      MEMIF_BLOCK_INCONSISTENT: Data CRC of block is wrong
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_LENGTH_E    MEMIF_BLOCK_INCONSISTENT: Block length is not consistent with config
 *
 * \seealso
 * \usedresources
 *********************************************************************
 */
MemIf_Rb_DetailedBlkInfo_ten MemIf_Rb_GetDetailedBlkInfo(uint8 DeviceIndex, uint16 BlockNumber)
{
    MemIf_Rb_DetailedBlkInfo_ten xRetVal = MEMIF_RB_DETAILED_BLK_INFO_INIT_E;  /* Default return value */

    (void)DeviceIndex;
    (void)BlockNumber;

    MemIf_CheckDeviceIndex(DeviceIndex);

# if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
#  if (defined (MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))
    if(Fee_Rb_IsThisDeviceIndexFee(DeviceIndex))
    {
#   if ( !defined(FEE_RB_IDX_DETAILED_BLK_INFO_API) || (FEE_RB_IDX_DETAILED_BLK_INFO_API != FALSE))
        xRetVal = Fee_Rb_Idx_GetDetailedBlkInfo(Fee_Rb_GetDeviceNameFromDeviceIndex(DeviceIndex),BlockNumber);
#   else
        /* Intentionally empty - will return MEMIF_RB_DETAILED_BLK_INFO_INIT_E */
#   endif  // FEE_RB_IDX_DETAILED_BLK_INFO_API
    }
#  else  // MEMIF_RB_NUM_OF_FEE_DEVICES
    if (DeviceIndex == 0u)
    {
        /* Block is configured for Fee (Flash) */
#   if( !defined(FEE_RB_DETAILED_BLK_INFO_API) || (FEE_RB_DETAILED_BLK_INFO_API != FALSE))
        xRetVal = Fee_Rb_GetDetailedBlkInfo(BlockNumber);
#   else
        /* Intentionally empty - will return MEMIF_RB_DETAILED_BLK_INFO_INIT_E */
#   endif // FEE_RB_DETAILED_BLK_INFO_API
    }
#  endif  // MEMIF_RB_NUM_OF_FEE_DEVICES
    else
# endif  // MEMIF_FEE_USED
    {
        xRetVal = Ea_Rb_GetDetailedBlkInfo(BlockNumber);
    }
    return(xRetVal);
}
#endif // MEMIF_FEE_AND_EA_USED

#if (defined(MEMIF_FEE_AND_EA_USED) && (MEMIF_FEE_AND_EA_USED == STD_ON))
/**
 *********************************************************************
 * MemIf_Rb_EnterHighSpeedMode(): MemIf_Rb_EnterHighSpeedMode interface
 *
 * This function calls the corresponding low level enter high speed mode function if available.
 *
 * \param    DeviceIndex:       Index of requested device
 * \return   Function success
 * \retval   E_OK:                      Order was successfully placed
 * \retval   E_NOT_OK:                  Order was reject
 * \retval   E_MEMIF_SERVICE_NOT_AVAIL: Feature not supported for the passed device
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType MemIf_Rb_EnterHighSpeedMode(uint8 DeviceIndex)
{
    Std_ReturnType xRetVal = E_MEMIF_SERVICE_NOT_AVAIL;   /* Default return value */

    (void)DeviceIndex;

    MemIf_CheckDeviceIndex(DeviceIndex);

# if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
#  if (defined (MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))
    if(Fee_Rb_IsThisDeviceIndexFee(DeviceIndex))
    {
#   if( !defined(FEE_RB_IDX_ENTER_AND_EXIT_HIGH_SPEED_MODE_API) || (FEE_RB_IDX_ENTER_AND_EXIT_HIGH_SPEED_MODE_API != FALSE))
        xRetVal = Fee_Rb_Idx_EnterHighSpeedMode(Fee_Rb_GetDeviceNameFromDeviceIndex(DeviceIndex), FEE_RB_USER_NVM_E);
#   else
        /* will return E_MEMIF_SERVICE_NOT_AVAIL */
        MemIf_Prv_DetReportError(MEMIF_RB_SID_ENTER_HIGH_SPEED_MODE, MEMIF_RB_E_SERVICE_NOT_AVAIL);
#   endif
    }
#  else
    if (DeviceIndex == 0u)
    {
        /* Block is configured for Fee (Flash) */
#   if( !defined(FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API) || (FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API != FALSE))
        xRetVal = Fee_Rb_EnterHighSpeedMode();
#   else
        /* will return E_MEMIF_SERVICE_NOT_AVAIL */
        MemIf_Prv_DetReportError(MEMIF_RB_SID_ENTER_HIGH_SPEED_MODE, MEMIF_RB_E_SERVICE_NOT_AVAIL);
#   endif
    }
#  endif
# endif

    return(xRetVal);
}
#endif

#if (defined(MEMIF_FEE_AND_EA_USED) && (MEMIF_FEE_AND_EA_USED == STD_ON))
/**
 *********************************************************************
 * MemIf_Rb_ExitHighSpeedMode(): MemIf_Rb_ExitHighSpeedMode interface
 *
 * This function calls the corresponding low level enter high speed mode function if available.
 *
 * \param    DeviceIndex:       Index of requested device
 * \return   Function success
 * \retval   E_OK:                      Order was successfully placed
 * \retval   E_NOT_OK:                  Order was reject
 * \retval   E_MEMIF_SERVICE_NOT_AVAIL: Feature not supported for the passed device
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType MemIf_Rb_ExitHighSpeedMode(uint8 DeviceIndex)
{
    Std_ReturnType xRetVal = E_MEMIF_SERVICE_NOT_AVAIL;   /* Default return value */

    (void)DeviceIndex;

    MemIf_CheckDeviceIndex(DeviceIndex);

# if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
#  if (defined (MEMIF_RB_NUM_OF_FEE_DEVICES) && (MEMIF_RB_NUM_OF_FEE_DEVICES > 1u))
    if(Fee_Rb_IsThisDeviceIndexFee(DeviceIndex))
    {
#   if( !defined(FEE_RB_IDX_ENTER_AND_EXIT_HIGH_SPEED_MODE_API) || (FEE_RB_IDX_ENTER_AND_EXIT_HIGH_SPEED_MODE_API != FALSE))
        xRetVal = Fee_Rb_Idx_ExitHighSpeedMode(Fee_Rb_GetDeviceNameFromDeviceIndex(DeviceIndex), FEE_RB_USER_NVM_E);
#   else
        /* will return E_MEMIF_SERVICE_NOT_AVAIL */
        MemIf_Prv_DetReportError(MEMIF_RB_SID_EXIT_HIGH_SPEED_MODE, MEMIF_RB_E_SERVICE_NOT_AVAIL);
#   endif
    }
#  else
    if (DeviceIndex == 0u)
    {
        /* Block is configured for Fee (Flash) */
#   if( !defined(FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API) || (FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API != FALSE))
        xRetVal = Fee_Rb_ExitHighSpeedMode();
#   else
        /* will return E_MEMIF_SERVICE_NOT_AVAIL */
        MemIf_Prv_DetReportError(MEMIF_RB_SID_EXIT_HIGH_SPEED_MODE, MEMIF_RB_E_SERVICE_NOT_AVAIL);
#   endif
    }
#  endif
# endif

    return(xRetVal);
}
#endif

#define MEMIF_STOP_SEC_CODE
#include "MemIf_MemMap.h"

