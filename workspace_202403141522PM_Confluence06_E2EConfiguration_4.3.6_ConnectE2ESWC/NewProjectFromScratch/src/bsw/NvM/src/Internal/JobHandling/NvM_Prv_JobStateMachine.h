/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_JOBSTATEMACHINE_H
#define NVM_PRV_JOBSTATEMACHINE_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_JobStateMachine_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRecalcUserDataCrc_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobCalcNvCrc_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRead_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobReadRecovery_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobWrite_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRestore_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRestoreWrite_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobInvalidate_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobInvalidateAll_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobValidate_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRecalcRamBlkCrc_GetData(void);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_JOBSTATEMACHINE_H */
#endif

