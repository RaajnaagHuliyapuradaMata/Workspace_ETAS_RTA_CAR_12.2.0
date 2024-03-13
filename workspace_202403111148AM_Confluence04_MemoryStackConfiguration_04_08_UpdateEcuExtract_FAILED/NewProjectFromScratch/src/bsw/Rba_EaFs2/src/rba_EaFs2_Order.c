/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Ea_Prv_HideRteApi.h"

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#include "Ea_Prv.h"
#include "Ea_Cfg_SchM.h"

/* Protection selectable via Ea_Cfg_SchM.h to save code */
#ifndef EA_RB_USE_PROTECTION
#define EA_RB_USE_PROTECTION   (TRUE)
#endif

#define EA_START_SEC_VAR_CLEARED_BOOLEAN
#include "Ea_MemMap.h"

#if(EA_AR_RELEASE_MINOR_VERSION >= 5)
static boolean rba_EaFs2_Prv_SetModeReqflg_b;          /* Flag holds the Set Mode request Info. */
#endif /* EA_AR_RELEASE_MINOR_VERSION*/

static volatile boolean rba_EaFs2_Prv_flgUsedSema_b;   /* Flag to hold the main function access status */

/* Flag to hold the debug information, Reentrant MainFunction */
static volatile boolean rba_EaFs2_Prv_dbgReentrantMainFunction_b;

#define EA_STOP_SEC_VAR_CLEARED_BOOLEAN
#include "Ea_MemMap.h"

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#if(EA_AR_RELEASE_MINOR_VERSION >= 5)
static MemIf_ModeType rba_EaFs2_Prv_Eep_Mode;
#endif /* EA_AR_RELEASE_MINOR_VERSION*/

Ea_Prv_Order_tst Ea_Prv_Order_st;

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"


#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

static void rba_EaFs2_Prv_UsrJobDone(MemIf_JobResultType result_en);
static void rba_EaFs2_Prv_CheckOrderRequestType(void);
static void rba_EaFs2_Prv_ResetIfInitDone(MemIf_JobResultType result_en);

#if(EA_AR_RELEASE_MINOR_VERSION >= 5)
/**
 *********************************************************************
 * rba_EaFs2_Prv_SetModeRequest(): Set Mode requested by User
 *
 * \param    none
 * \seealso
 * \usedresources
 *********************************************************************
 */
 void rba_EaFs2_Prv_SetModeRequest(MemIf_ModeType Mode)
 {
     rba_EaFs2_Prv_Eep_Mode = Mode;
     rba_EaFs2_Prv_SetModeReqflg_b = (boolean)TRUE;
 }
#endif /* EA_AR_RELEASE_MINOR_VERSION*/

/**
 *********************************************************************
 * rba_EaFs2_Prv_TryToGetLock(): Detection and protection of re-entrant
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
LOCAL_INLINE boolean rba_EaFs2_Prv_TryToGetLock(void)
{
    boolean isLockAvailable_b = (boolean)FALSE;

#if(EA_RB_USE_PROTECTION != FALSE)
    /* Disable interrupts */
    SchM_Enter_Ea_Order();
#endif

    /* Is the Main function not invoked yet, then reserve it and allow the execution */
    if(!rba_EaFs2_Prv_flgUsedSema_b)
    {
        rba_EaFs2_Prv_flgUsedSema_b = (boolean)TRUE;
        isLockAvailable_b = (boolean)TRUE;
    }

#if(EA_RB_USE_PROTECTION != FALSE)
    /* Enable interrupts */
    SchM_Exit_Ea_Order();
#endif

    if(!isLockAvailable_b)
    {
        /* Set the debug flag for the detection of reentrant main function  */
        rba_EaFs2_Prv_dbgReentrantMainFunction_b = (boolean)TRUE;

        /* Multiple invocation of Ea_MainFunction, report error if det enabled */
        RBA_EAFS2_DET_REPORTERROR(EA_SID_MAINFUNCTION, EA_E_BUSY);
    }

#if(EA_RB_USE_PROTECTION == FALSE)
    /* Protection disabled, only detection by default */
    isLockAvailable_b = (boolean)TRUE;
#endif

    return isLockAvailable_b;
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_ReleaseLock(): Reset the flag having reentrant information
 *
 * \param    none
 * \retval   none
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE void rba_EaFs2_Prv_ReleaseLock(void)
{
    /* Reset the access flag */
    rba_EaFs2_Prv_flgUsedSema_b = (boolean)FALSE;
    return;
}

/**
 *********************************************************************
 * User Requested Job Completed.Update status,Job result and reset
 * the variables to accept newRequest
 * \param    result_en  Ea_JobResult passed as Argument
 * \retval   none
 * \seealso
 * \usedresources
 *********************************************************************
 */

static void rba_EaFs2_Prv_UsrJobDone(MemIf_JobResultType result_en)
{
    /* Disable Interrupt */
    SchM_Enter_Ea_Order();

    Ea_GlobModuleState_st = MEMIF_IDLE;
    Ea_JobResult = result_en;
    rba_EaFs2_Prv_RestOrder();

    #if(EA_RB_ENTER_STOP_MODE != FALSE)
    if(RBA_EAFS2_PRV_STOPMODE_INPROCESS == rba_EaFs2_Prv_StopMode_en)
    {
        Ea_Prv_Order_st.OrderType_en = EA_PRV_STOP_MODE_E;
    }
    #endif

    /* Enable Interrupt */
    SchM_Exit_Ea_Order();
}

/**
 *********************************************************************
 * Ea_Cancel(): Cancels an ongoing user operation
 *
 *********************************************************************
 */
void Ea_Cancel(void)
{
    return;
}

/**
 *********************************************************************
 * Ea_EraseImmediateBlock(): Clears the content of a logical block
 *
 * \param    BlockNumber:       Logical block Number
 *
 * \return   Order acceptance status
 * \retval   E_OK:      Order was accepted
 * \retval   E_NOT_OK:  Order was not accepted
 *********************************************************************
 */
Std_ReturnType Ea_EraseImmediateBlock(uint16 BlockNumber)
{
    (void)BlockNumber;
    return(E_NOT_OK);
}

/**
 *********************************************************************
 * Ea_Write(): Write a logical block data to EEPROM
 *
 * This function writes one or more complete EEPROM pages depending on the user parameters. The job processing is done
 * within the module's main state and is called cyclically.The result of the function indicates whether the order
 * is accepted or not.
 *
 * \param    BlockNumber:       Logical block Number
 * \param    DataBufferPtr:     Pointer to buffer which contains the data to be written
 *
 * \return   Order acceptance status
 * \retval   E_OK:      Order was accepted
 * \retval   E_NOT_OK:  Order was not accepted
 *********************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: DataBufferPtr is not const because of backward compatability issues*/
Std_ReturnType Ea_Write(uint16 BlockNumber, uint8* DataBufferPtr)
{
    Std_ReturnType result_en = E_NOT_OK;

    if(E_OK == Ea_Rb_CheckInit(EA_SID_WRITE))
    {
        if(E_OK == Ea_Rb_CheckBusy(EA_SID_WRITE))
        {
            if(E_OK == Ea_Rb_CheckBlkNr(EA_SID_WRITE,BlockNumber))
            {
                if(E_OK == Ea_Rb_CheckDataPtr(EA_SID_WRITE, DataBufferPtr))
                {
#if (EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
                    if(EA_RB_MIGRATION_ERROR != Ea_Rb_MigrationStaus_u8)
#endif
                    {
                        result_en = E_OK;
                    }
                }
            }
        }
    }

    if(E_OK == result_en)
    {
        SchM_Enter_Ea_Order();

        if(EA_RB_NO_ORDER_E == Ea_Prv_Order_st.OrderType_en)
        {
            Ea_JobResult = MEMIF_JOB_PENDING;
            Ea_GlobModuleState_st = MEMIF_BUSY;

            Ea_Prv_Order_st.OrderType_en     = EA_PRV_WRITE_ORDER_E;
            Ea_Prv_Order_st.bfr_pu8          = NULL_PTR;
            Ea_Prv_Order_st.bfr_pcu8         = DataBufferPtr;
            Ea_Prv_Order_st.blockNumber_u16  = BlockNumber;
            Ea_Prv_Order_st.length_u16       = Ea_BlockProperties_st[BlockNumber].Ea_BlkLength_u16;
            Ea_Prv_Order_st.offset_u16       = 0;
        }
        else
        {
            result_en = E_NOT_OK;
        }

        SchM_Exit_Ea_Order();
    }

    return result_en;
}

/**
 *********************************************************************
 * Ea_Read(): Read one or more complete EEPROM pages
 *
 * This function reads one or more complete EEPROM pages depending on the user parameters. The job processing is done
 * within the module's main state and is called cyclically.The result of the function indicates whether the order
 * is accepted or not.
 *
 * \param    BlockNumber:       Number of logical block
 * \param    BlockOffset:       Read address offset inside the block
 * \param    DataBufferPtr:     Pointer to data buffer
 * \param    Length:            Number of bytes to read
 * \return   Order acceptance status
 * \retval   E_OK:      Order was accepted
 * \retval   E_NOT_OK:  Order was not accepted
 *********************************************************************
 */
Std_ReturnType Ea_Read(uint16 BlockNumber,
        uint16 BlockOffset,
        uint8* DataBufferPtr,
        uint16 Length)
{

    Std_ReturnType result_en = E_NOT_OK;
    uint16 ilen_u16;

    if(E_OK == Ea_Rb_CheckInit(EA_SID_READ))
    {
        if(E_OK == Ea_Rb_CheckBusy(EA_SID_READ))
        {
            if(E_OK == Ea_Rb_CheckDataPtr(EA_SID_READ,DataBufferPtr))
            {
                if(E_OK == Ea_Rb_CheckBlkNr(EA_SID_READ,BlockNumber))
                {
#if (EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
                    if(EA_RB_MIGRATION_ERROR != Ea_Rb_MigrationStaus_u8)
#endif
                    {
                        result_en = E_OK;
                    }
                }
            }
        }
    }

    /* Wrong Block length DET is disabled as per requirement*/
    /*    if(E_OK != rba_EaFs2_Prv_CheckBlkLength(EA_SID_READ, BlockNumber, BlockOffset, Length ) )  [SWS_Ea_00169]
    {
        result_en = E_NOT_OK;
    }*/

    if(E_OK == result_en)
    {
        SchM_Enter_Ea_Order();

        if(
                (EA_RB_NO_ORDER_E == Ea_Prv_Order_st.OrderType_en)&&
                (Length>0)
        )
        {
            if(0u == rba_EaFs2_Prv_RamTable_st[BlockNumber].address_u32)
            {
                /* Return inconsistent in case the block has not been allocated yet*/
                Ea_JobResult = MEMIF_BLOCK_INCONSISTENT;
                rba_EaFs2_Prv_ReadJobResultDetailed_aen[BlockNumber] = MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E;
            }
            else
            {
                Ea_JobResult = MEMIF_JOB_PENDING;
                Ea_GlobModuleState_st = MEMIF_BUSY;

                Ea_Prv_Order_st.OrderType_en     = EA_PRV_READ_ORDER_E;
                Ea_Prv_Order_st.bfr_pu8          = DataBufferPtr;
                Ea_Prv_Order_st.bfr_pcu8         = NULL_PTR;
                Ea_Prv_Order_st.blockNumber_u16  = BlockNumber;
                Ea_Prv_Order_st.offset_u16       = BlockOffset;

                rba_EaFs2_Prv_ReadJobResultDetailed_aen[BlockNumber] = MEMIF_RB_DETAILED_BLK_INFO_INIT_E;

                if(Length > Ea_BlockProperties_st[BlockNumber].Ea_BlkLength_u16)
                {
                   Ea_Prv_Order_st.length_u16 = Ea_BlockProperties_st[BlockNumber].Ea_BlkLength_u16;

                   for(ilen_u16=Length; ilen_u16 >Ea_BlockProperties_st[BlockNumber].Ea_BlkLength_u16; ilen_u16--)
                   {
                       DataBufferPtr[ilen_u16-1] = EA_RB_EEP_ERASE_VALUE;
                   }
                }

                else
                {
                   Ea_Prv_Order_st.length_u16 = Length;
                }
            }
        }
        else
        {
            result_en = E_NOT_OK;
        }

        SchM_Exit_Ea_Order();
    }

    return result_en;
}

/**
 *********************************************************************
 * Ea_InvalidateBlock(): Invalidate a logical block in the EEPROM
 *
 * This function invalidates a logical block as indicated by the block number. The job processing is done
 * within the module's main state and is called cyclically.The result of the function indicates whether the order
 * is accepted or not.
 *
 * \param    BlockNumber:       Logical block Number
 *
 * \return   Order acceptance status
 * \retval   E_OK:      Order was accepted
 * \retval   E_NOT_OK:  Order was not accepted
 *********************************************************************
 */
Std_ReturnType Ea_InvalidateBlock(uint16 BlockNumber)
{
    Std_ReturnType result_en = E_NOT_OK;

    if(E_OK == Ea_Rb_CheckInit(EA_SID_INVALIDATEBLOCK))
    {
        if(E_OK == Ea_Rb_CheckBusy(EA_SID_INVALIDATEBLOCK))
        {
            if(E_OK == Ea_Rb_CheckBlkNr(EA_SID_INVALIDATEBLOCK, BlockNumber))
            {
#if (EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
                if(EA_RB_MIGRATION_ERROR != Ea_Rb_MigrationStaus_u8)
#endif
                {
                    result_en = E_OK;
                }
            }
        }
    }

    if(E_OK == result_en)
    {
        SchM_Enter_Ea_Order();

        if(EA_RB_NO_ORDER_E == Ea_Prv_Order_st.OrderType_en)
        {
            Ea_JobResult = MEMIF_JOB_PENDING;
            Ea_GlobModuleState_st = MEMIF_BUSY;

            Ea_Prv_Order_st.OrderType_en     = EA_PRV_INVALIDATE_ORDER_E;
            Ea_Prv_Order_st.bfr_pu8          = NULL_PTR;
            Ea_Prv_Order_st.bfr_pcu8         = NULL_PTR;
            Ea_Prv_Order_st.blockNumber_u16  = BlockNumber;
            Ea_Prv_Order_st.length_u16       = Ea_BlockProperties_st[BlockNumber].Ea_BlkLength_u16;
            Ea_Prv_Order_st.offset_u16       = 0u;
        }
        else
        {
            result_en = E_NOT_OK;
        }

        SchM_Exit_Ea_Order();
    }

    return result_en;
}

/**
 *********************************************************************
 * Ea_Rb_MainFunctionIntern(): Internal routine for Mainfunction of EA
 *
 * This function performs the processing of the EEPROM read, write
 * erase jobs.
 * When a job has been initiated, the environment of EA module
 * shall call this function cyclically until the job is finished.
 *
 *********************************************************************
 */
static void Ea_Rb_MainFunctionIntern(void)
{
    MemIf_JobResultType result_en = MEMIF_JOB_PENDING;

    if(MEMIF_JOB_PENDING !=Eep_GetJobResult())
    {
#if(FALSE != EA_PRV_DEBUGGING)
        rba_EaFs2_Prv_DebugTimeStart(&rba_EaFs2_Debug_st.mainTime_st);
#endif
        /*
         * Check the current order, if a new request comes and a BGO is running set the cancel bit.
         * Keep the state in EA_PRV_BACKGROUND_E till its iterruptabile. Then process the incoming request
         * Idle decides if control should go to BGO,
         */
        if(
                (EA_RB_NO_ORDER_E != Ea_Prv_Order_st.OrderType_en) &&
                (EA_PRV_DRIVERSTATE_BACKGROUND_E == rba_EaFs2_Prv_DriverState_en)
        )
        {
            rba_EaFs2_Prv_FsInfo_st.suspendBgo_b = (boolean)TRUE;
        }

        if(EA_PRV_DRIVERSTATE_IDLE_E == rba_EaFs2_Prv_DriverState_en)
        {
            /* Check order request type, set DriverState and initialize for order request type */
            rba_EaFs2_Prv_CheckOrderRequestType();
        }

        switch(rba_EaFs2_Prv_DriverState_en)
        {
            case EA_PRV_DRIVERSTATE_IDLE_E:              break; /* Do nothing in case of Idle state */
            case EA_PRV_DRIVERSTATE_BACKGROUND_E:        result_en = rba_EaFs2_Prv_BgoDo       (); break;
            case EA_PRV_DRIVERSTATE_READ_E:              result_en = rba_EaFs2_Prv_ReadDo      (); break;
            case EA_PRV_DRIVERSTATE_WRITE_E:             result_en = rba_EaFs2_Prv_WriteDo     (); break;
            case EA_PRV_DRIVERSTATE_INVALIDATEBLOCK_E:   result_en = rba_EaFs2_Prv_WriteDo     (); break;
            case EA_PRV_DRIVERSTATE_BLOCKMAINTENANCE_E:  result_en = rba_EaFs2_Prv_DataRepairDo(); break;
            default:                                     result_en = MEMIF_JOB_FAILED;         break;
        }

        /* Reset the variables if only Init done properly */
        rba_EaFs2_Prv_ResetIfInitDone(result_en);

#if(EA_AR_RELEASE_MINOR_VERSION >= 5)
        /* Check for Set Mode Request, Ea setMode request is set when defragmenation Fat repair is completed  */
        if(rba_EaFs2_Prv_SetModeReqflg_b != (boolean)FALSE)
        {
            if(MEMIF_IDLE == Ea_GlobModuleState_st)
            {
                if(MEMIF_IDLE == Eep_GetStatus())
                {
                    Eep_SetMode(rba_EaFs2_Prv_Eep_Mode);
                    Ea_Mode= rba_EaFs2_Prv_Eep_Mode;
                    rba_EaFs2_Prv_SetModeReqflg_b = (boolean)FALSE;  /* Set Mode operation complete */
                }
            }
        }
#endif /* EA_AR_RELEASE_MINOR_VERSION */

#if(FALSE != EA_PRV_DEBUGGING)
        rba_MemLib_UpdateTimerUs();
        rba_EaFs2_Prv_DebugTimeStop(&rba_EaFs2_Debug_st.mainTime_st);
#endif

    }
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_ResetIfInitDone():
 *
 * \param   result_en : result return from previous job
 *
 * Check and Reset the variables if only Init done properly
 *
 *********************************************************************
 */
static void rba_EaFs2_Prv_ResetIfInitDone(MemIf_JobResultType result_en){

    if ((MEMIF_JOB_PENDING != result_en) &&
        (rba_EaFs2_Prv_DriverState_en != EA_PRV_DRIVERSTATE_INIT_E))
    {
        /* In case of user request Reset Order and notify callbacks?*/
        if(EA_PRV_DRIVERSTATE_BACKGROUND_E != rba_EaFs2_Prv_DriverState_en)
        {
            rba_EaFs2_Prv_UsrJobDone(result_en);
        }

        rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_IDLE_E;
    }
}

/**
 *********************************************************************
 * rba_EaFs2_Prv_CheckOrderRequestType():
 *
 * Check order request type, set DriverState and initialize for order request type
 *
 *********************************************************************
 */
static void rba_EaFs2_Prv_CheckOrderRequestType(void){

    switch(Ea_Prv_Order_st.OrderType_en)
    {
        case EA_PRV_READ_ORDER_E:
            rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_READ_E;
            rba_EaFs2_Read();
            break;
        case EA_PRV_WRITE_ORDER_E:
            rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_WRITE_E;
            rba_EaFs2_Write((boolean)FALSE);
            break;
        case EA_PRV_INVALIDATE_ORDER_E:
            rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_INVALIDATEBLOCK_E;
            rba_EaFs2_Write((boolean)TRUE);
            break;
        case EA_PRV_BLOCKMAINTAIN_ORDER_E:
            rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_BLOCKMAINTENANCE_E;
            rba_EaFs2_Prv_DataRepair();
            break;
        case EA_PRV_STOP_MODE_E:
#if(EA_RB_ENTER_STOP_MODE != FALSE)
            rba_EaFs2_Prv_StopModeActivate();
#endif
            break;
        case EA_RB_NO_ORDER_E:
            rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_BACKGROUND_E;
            break;
        default: rba_EaFs2_Prv_DriverState_en = EA_PRV_DRIVERSTATE_IDLE_E;
        break;
    }
}


/**
 *********************************************************************
 * Ea_MainFunction(): Mainfunction of EA
 *
 * This function prevents the multiple execution of MainFunction if
 * protection switch is enabled and invokes the internal processing
 * of EEPROM jobs
 *
 *********************************************************************
 */
void Ea_MainFunction(void)
{
    /* Local flag to store the lock accuring status */
    boolean flgUsed_b = (boolean)FALSE;

    /* Check for re-entrant invocation of MainFunction*/
    flgUsed_b = rba_EaFs2_Prv_TryToGetLock();

    if(flgUsed_b)
    {
        Ea_Rb_MainFunctionIntern();

        /* Release the lock */
        rba_EaFs2_Prv_ReleaseLock();
    }

    return;
}


/**
 *********************************************************************
 * EaFs2_Rb_GetDetailedBlkInfo(): Check the detailed error information of particular block
 *
 * This function will distinguish between EMPTY and CORRUPTED for the block
 *
 * \param   BlockNumber
 * \return  MemIf_Rb_DetailedBlkInfo_ten
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INIT_E                   Block was never read inconsitent - result not available
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E   MEMIF_BLOCK_INCONSISTENT: Valid block (header) was not found on the medium
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_DATA_E      MEMIF_BLOCK_INCONSISTENT: Data CRC of block is wrong
 * \retval  MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_LENGTH_E    is not supported in Ea.
 *
 *********************************************************************
 */
MemIf_Rb_DetailedBlkInfo_ten rba_EaFs2_Prv_GetDetailedBlkInfo(uint16 blockNumber_u16)
{

    MemIf_Rb_DetailedBlkInfo_ten result_en = MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E;
    Std_ReturnType result_loc = E_NOT_OK;

    if(E_OK == Ea_Rb_CheckInit(EA_SID_RB_GETDETAILEDBLKINFO))
    {
        if(E_OK == Ea_Rb_CheckBlkNr(EA_SID_RB_GETDETAILEDBLKINFO, blockNumber_u16))
        {
#if (EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
            if(EA_RB_MIGRATION_ERROR != Ea_Rb_MigrationStaus_u8)
#endif
            {
                result_loc = E_OK;
            }
        }
    }

    if(E_OK == result_loc)
    {
          result_en = rba_EaFs2_Prv_ReadJobResultDetailed_aen[blockNumber_u16];
    }else
    {
         result_en = MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E;
    }
    return result_en;
}


/**
 *********************************************************************
 * Ea_Rb_BlockMaintenance(): Repairs a logical redundant block in the EEPROM
 *
 * This function repairs a logical redundant block as indicated by the block number. The job processing is done
 * within the module's main state and is called cyclically.The result of the function indicates whether the order
 * is accepted or not.
 *
 * \param    BlockNumber:       Logical block Number
 *
 * \return   Order acceptance status
 * \retval   E_OK:      Order was accepted
 * \retval   E_NOT_OK:  Order was not accepted
 *********************************************************************
 */
Std_ReturnType Ea_Rb_BlockMaintenance(uint16 BlockNumber)
{
    Std_ReturnType result_en = E_NOT_OK;

    if(E_OK == Ea_Rb_CheckInit(EA_SID_RB_MAINTENANCE))
    {
        if(E_OK == Ea_Rb_CheckBlkNr(EA_SID_RB_MAINTENANCE, BlockNumber))
        {
#if (EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
            if(EA_RB_MIGRATION_ERROR != Ea_Rb_MigrationStaus_u8)
#endif
            {
                result_en = E_OK;
            }
        }
    }

    if(E_OK == result_en)
    {
        SchM_Enter_Ea_Order();

        if(EA_RB_NO_ORDER_E == Ea_Prv_Order_st.OrderType_en)
        {
            /* Maintenance is applicable only for double storage blocks*/
            if((boolean)FALSE != rba_EaFs2_Prv_IsBlockDblStorage(BlockNumber))
            {
                if(0u == rba_EaFs2_Prv_RamTable_st[BlockNumber].address_u32)
                {
                    /* Return inconsistent in case the block has not been allocated yet*/
                    Ea_JobResult = MEMIF_BLOCK_INCONSISTENT;
                }
                else
                {
                    Ea_JobResult = MEMIF_JOB_PENDING;
                    Ea_GlobModuleState_st = MEMIF_BUSY;

                    Ea_Prv_Order_st.OrderType_en     = EA_PRV_BLOCKMAINTAIN_ORDER_E;
                    Ea_Prv_Order_st.bfr_pu8          = NULL_PTR;
                    Ea_Prv_Order_st.bfr_pcu8         = NULL_PTR;
                    Ea_Prv_Order_st.blockNumber_u16  = BlockNumber;
                    Ea_Prv_Order_st.length_u16       = Ea_BlockProperties_st[BlockNumber].Ea_BlkLength_u16;
                    Ea_Prv_Order_st.offset_u16       = 0;
                }
            }
            else
            {
                result_en = E_NOT_OK;
            }
        }
        else
        {
            result_en = E_NOT_OK;
        }

        SchM_Exit_Ea_Order();
    }

    return result_en;
}


#if(FALSE != EA_PRV_DEBUGGING)
/**
 *********************************************************************
 * rba_EaFs2_Prv_Initialize_Order():
 *
 * This function used as initialzation for global variable
 *
 * \param    None
 *
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_EaFs2_Prv_Initialize_Order(void)
{
    Ea_Prv_Order_st.OrderType_en=EA_RB_NO_ORDER_E;
    Ea_Prv_Order_st.bfr_pu8=NULL_PTR;
    Ea_Prv_Order_st.bfr_pcu8=NULL_PTR;
    Ea_Prv_Order_st.blockNumber_u16=0u;
    Ea_Prv_Order_st.length_u16=0u;
    Ea_Prv_Order_st.offset_u16=0u;
#if(EA_AR_RELEASE_MINOR_VERSION >= 5)
    rba_EaFs2_Prv_Eep_Mode = MEMIF_MODE_SLOW;
    rba_EaFs2_Prv_SetModeReqflg_b = FALSE;
#endif /* EA_AR_RELEASE_MINOR_VERSION */
}
#endif

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
