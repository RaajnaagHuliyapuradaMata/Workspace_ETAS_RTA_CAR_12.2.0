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

#ifndef MEMIF_INL_H
#define MEMIF_INL_H

#include "Std_Types.h"
#if((MEMIF_FEE_USED==STD_ON)&&(MEMIF_RB_NUM_OF_FEE_DEVICES==1))
#include "Fee.h"
#endif
#if((MEMIF_FEE_USED==STD_OFF)&&(MEMIF_NUM_OF_EA_DEVICES == 1))
#include "Ea.h"
#endif
#include "MemIf_Cfg.h"

#if (defined(MEMIF_DEV_ERROR_DETECT) && (MEMIF_DEV_ERROR_DETECT == STD_ON))
#include "Det.h"
#endif

/**
 *********************************************************************
 * MemIf_Prv_DetReportError(): Call Det_ReportError if enabled
 *
 * Raises a Det error if development error detection is enabled
 * for MemIf
 *
 * \param   sid_u8:   Service ID
 * \param   error_u8: Error ID
 * \return  None
 *********************************************************************
 */
LOCAL_INLINE void MemIf_Prv_DetReportError(uint8 sid_u8, uint8 error_u8)
{
#if (defined(MEMIF_DEV_ERROR_DETECT) && (MEMIF_DEV_ERROR_DETECT == STD_ON))
    (void)Det_ReportError(MEMIF_MODULE_ID, MEMIF_INSTANCE_ID, sid_u8, error_u8);
#else
    /* Avoid warnings for unused parameters */
    (void)sid_u8;
    (void)error_u8;
#endif
}

#if((MEMIF_FEE_AND_EA_USED==STD_OFF)&&((MEMIF_FEE_USED==STD_ON)&&(MEMIF_RB_NUM_OF_FEE_DEVICES==1))||((MEMIF_FEE_USED==STD_OFF)&&(MEMIF_NUM_OF_EA_DEVICES==1)))

/** ***********************************************************************
 * MemIf_Read(): Calls corresponding read functions.
 *
 * The function calls the required underlying read functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
 * \param   BlockOffset
 * \param   DataBufferPtr
 * \param   Length
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType MemIf_Read(uint8 DeviceIndex, uint16 BlockNumber, uint16 BlockOffset, uint8* DataBufferPtr, uint16 Length)
{
#if(MEMIF_FEE_USED==STD_ON)
    {
        (void)DeviceIndex;
        return(Fee_Read(BlockNumber,BlockOffset,DataBufferPtr,Length));
    }
#else
    {
        (void)DeviceIndex;
        return(Ea_Read(BlockNumber,BlockOffset,DataBufferPtr,Length));
    }
#endif

}


/** ***********************************************************************
 * MemIf_Write(): Calls corresponding write functions.
 *
 * The function calls the required underlying write functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
 * \param   DataBufferPtr
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType MemIf_Write(uint8 DeviceIndex,uint16 BlockNumber,uint8* DataBufferPtr)
{
#if(MEMIF_FEE_USED==STD_ON)
    {
        (void)DeviceIndex;
        return(Fee_Write(BlockNumber,DataBufferPtr));
    }
#else
    {
        (void)DeviceIndex;
        return(Ea_Write(BlockNumber,DataBufferPtr));
    }
#endif

}

/** ***********************************************************************
 * MemIf_InvalidateBlock(): Calls corresponding invalidate block functions.
 *
 * The function calls the required underlying invalidate block functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType MemIf_InvalidateBlock(uint8 DeviceIndex,uint16 BlockNumber)
{
#if(MEMIF_FEE_USED==STD_ON)
    {
        (void)DeviceIndex;
        return(Fee_InvalidateBlock(BlockNumber));
    }
#else
    {
        (void)DeviceIndex;
        return(Ea_InvalidateBlock(BlockNumber));
    }
#endif

}

/** ***********************************************************************
 * MemIf_GetStatus(): Calls corresponding get status functions.
 *
 * The function calls the required underlying get status functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE MemIf_StatusType MemIf_GetStatus(uint8 DeviceIndex)
{
#if(MEMIF_FEE_USED==STD_ON)
    {
        (void)DeviceIndex;
        return(Fee_GetStatus());
    }
#else
    {
        (void)DeviceIndex;
        return(Ea_GetStatus());
    }
#endif

}

/** ***********************************************************************
 * MemIf_GetJobResult(): Calls corresponding job result functions.
 *
 * The function calls the required underlying get job result functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE MemIf_JobResultType  MemIf_GetJobResult(uint8 DeviceIndex)
{
#if(MEMIF_FEE_USED==STD_ON)
    {
        (void)DeviceIndex;
        return(Fee_GetJobResult());
    }
#else
    {
        (void)DeviceIndex;
        return(Ea_GetJobResult());
    }
#endif

}

/** ***********************************************************************
 * MemIf_Cancel(): Calls corresponding cancel functions.
 *
 * The function calls the required underlying cancel functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE void  MemIf_Cancel(uint8 DeviceIndex)
{
#if(MEMIF_FEE_USED==STD_ON)
    {
        (void)DeviceIndex;
        Fee_Cancel();
        return;
    }
#else
    {
        (void)DeviceIndex;
        Ea_Cancel();
        return;
    }
#endif

}

/** ***********************************************************************
 * MemIf_Rb_EnterHighSpeedMode(): Calls corresponding enter high speed mode functions.
 *
 * The function calls the required underlying enter high speed mode functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType  MemIf_Rb_EnterHighSpeedMode(uint8 DeviceIndex)
{
    (void)DeviceIndex;
#if(defined(FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API)&&(FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API != FALSE))
    return(Fee_Rb_EnterHighSpeedMode());
#else
    MemIf_Prv_DetReportError(MEMIF_RB_SID_ENTER_HIGH_SPEED_MODE, MEMIF_RB_E_SERVICE_NOT_AVAIL);
    return(E_MEMIF_SERVICE_NOT_AVAIL);
#endif
}

/** ***********************************************************************
 * MemIf_Rb_ExitHighSpeedMode(): Calls corresponding exit high speed mode functions.
 *
 * The function calls the required underlying exit high speed mode functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType  MemIf_Rb_ExitHighSpeedMode(uint8 DeviceIndex)
{
    (void)DeviceIndex;
#if(defined(FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API)&&(FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API != FALSE))
    return(Fee_Rb_ExitHighSpeedMode());
#else
    MemIf_Prv_DetReportError(MEMIF_RB_SID_EXIT_HIGH_SPEED_MODE, MEMIF_RB_E_SERVICE_NOT_AVAIL);
    return(E_MEMIF_SERVICE_NOT_AVAIL);
#endif
}

/** ***********************************************************************
 * MemIf_EraseImmediateBlock(): Calls corresponding erase immediate block functions.
 *
 * The function calls the required underlying erase immediate block functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType  MemIf_EraseImmediateBlock(uint8 DeviceIndex,uint16 BlockNumber)
{
#if(MEMIF_FEE_USED==STD_ON)
    {
        (void)DeviceIndex;
        return(Fee_EraseImmediateBlock(BlockNumber));
    }
#else
    {
        (void)DeviceIndex;
        return(Ea_EraseImmediateBlock(BlockNumber));
    }
#endif

}


/** ***********************************************************************
 * MemIf_VarLenRead(): Calls corresponding var length read functions.
 *
 * The function calls the required underlying var length read functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
 * \param   BlockOffset
 * \param   DataBufferPtr
 * \param   Length
*/
#if((defined(FEE_RB_VAR_LEN_READ_WRITE_API)&&(FEE_RB_VAR_LEN_READ_WRITE_API!=FALSE))||(defined(FEE_RB_FIRST_READ_DATA_MIGRATION_API)&&(FEE_RB_FIRST_READ_DATA_MIGRATION_API!=FALSE)))
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType  MemIf_Rb_VarLenRead(uint8 DeviceIndex,uint16 BlockNumber,uint16 BlockOffset,uint8* DataBufferPtr,uint16 Length)
{

    (void)DeviceIndex;
    return(Fee_Rb_VarLenRead(BlockNumber,BlockOffset,DataBufferPtr,Length));

}
#else
/* MR12 RULE 5.9,8.13 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation / const-pointer not used because of other config-variant- no issue */
LOCAL_INLINE Std_ReturnType  MemIf_Rb_VarLenRead(uint8 DeviceIndex,uint16 BlockNumber,uint16 BlockOffset,uint8* DataBufferPtr,uint16 Length)
{
    (void)DeviceIndex;
    (void)BlockNumber;
    (void)BlockOffset;
    (void)DataBufferPtr;
    (void)Length;
    return(E_NOT_OK);
}
#endif


/** ***********************************************************************
 * MemIf_VarLenWrite(): Calls corresponding var lenth write functions.
 *
 * The function calls the required underlying var length write functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
 * \param   DataBufferPtr
 * \param   Length
*/
#if(defined(FEE_RB_VAR_LEN_READ_WRITE_API)&&(FEE_RB_VAR_LEN_READ_WRITE_API!=FALSE))
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE Std_ReturnType  MemIf_Rb_VarLenWrite(uint8 DeviceIndex,uint16 BlockNumber,uint8* DataBufferPtr,uint16 Length)
{
    (void)DeviceIndex;
    return(Fee_Rb_VarLenWrite(BlockNumber,DataBufferPtr,Length));
}
#else
/* MR12 RULE 5.9,8.13 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation / const-pointer not used because of other config-variant- no issue */
LOCAL_INLINE Std_ReturnType  MemIf_Rb_VarLenWrite(uint8 DeviceIndex,uint16 BlockNumber,uint8* DataBufferPtr,uint16 Length)
{
    (void)DeviceIndex;
    (void)BlockNumber;
    (void)DataBufferPtr;
    (void)Length;
    return(E_NOT_OK);
}
#endif


/** ***********************************************************************
 * MemIf_GetMigrationResult(): Calls corresponding get migration result functions.
 *
 * The function calls the required underlying get migration result functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
*/
#if(defined(FEE_RB_FIRST_READ_DATA_MIGRATION_API)&&(FEE_RB_FIRST_READ_DATA_MIGRATION_API!=FALSE))
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE MemIf_Rb_MigrationResult_ten  MemIf_Rb_GetMigrationResult(uint8 DeviceIndex,uint16 BlockNumber)
{
    (void)DeviceIndex;
    return(Fee_Rb_GetMigrationResult(BlockNumber));
}
#else
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE MemIf_Rb_MigrationResult_ten  MemIf_Rb_GetMigrationResult(uint8 DeviceIndex,uint16 BlockNumber)
{
    (void)DeviceIndex;
    (void)BlockNumber;
    return(MEMIF_RB_MIGRATION_RESULT_DEACTIVATED_E);
}
#endif


/** ***********************************************************************
 * MemIf_GetDetailedBlkInfo(): Calls corresponding get detailed block info functions.
 *
 * The function calls the required underlying get detailed block info functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
LOCAL_INLINE MemIf_Rb_DetailedBlkInfo_ten  MemIf_Rb_GetDetailedBlkInfo(uint8 DeviceIndex,uint16 BlockNumber)
{
#if(defined(FEE_RB_DETAILED_BLK_INFO_API)&&(FEE_RB_DETAILED_BLK_INFO_API!=FALSE))
    (void)DeviceIndex;
    return(Fee_Rb_GetDetailedBlkInfo(BlockNumber));
#elif(MEMIF_FEE_USED==STD_OFF)
    (void)DeviceIndex;
    return(Ea_Rb_GetDetailedBlkInfo(BlockNumber));
#else
    (void)DeviceIndex;
    (void)BlockNumber;
    return(MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E);
#endif
}

/** ***********************************************************************
 * MemIf_Rb_BlockMaintenance(): Calls corresponding block maintenance functions.
 *
 * The function calls the required underlying block maintenance functions,
 * depending on which modules are configured.
 * \param   DeviceIndex
 * \param   BlockNumber
*/
/* MR12 RULE 5.9 VIOLATION: declaration/definition of LOCAL_INLINE functions are split on different headers for hiding implementation - no issue */
/* The Fee block maintenance API is not available under all conditions */
LOCAL_INLINE Std_ReturnType  MemIf_Rb_BlockMaintenance(uint8 DeviceIndex,uint16 BlockNumber)
{
#if(defined(FEE_RB_MAINTAIN)&&(FEE_RB_MAINTAIN!=FALSE))
    (void)DeviceIndex;
    return(Fee_Rb_BlockMaintenance(BlockNumber));
#elif(MEMIF_FEE_USED==STD_OFF)
    (void)DeviceIndex;
    return(Ea_Rb_BlockMaintenance(BlockNumber));
#else
    (void)DeviceIndex;
    (void)BlockNumber;
    return(E_NOT_OK);
#endif
}

#endif

#endif /*MEMIF_INL_H*/
