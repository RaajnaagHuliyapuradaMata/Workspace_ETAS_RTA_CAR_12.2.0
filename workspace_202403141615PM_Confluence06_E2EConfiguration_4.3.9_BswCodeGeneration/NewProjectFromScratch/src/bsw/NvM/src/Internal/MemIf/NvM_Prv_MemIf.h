/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2020, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_MEMIF_H
#define NVM_PRV_MEMIF_H
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

extern void NvM_Prv_MemIf_MainFunctionAndDependency (void);

extern Std_ReturnType NvM_Prv_MemIf_EnterHighSpeedMode(void);
extern Std_ReturnType NvM_Prv_MemIf_ExitHighSpeedMode(void);

extern boolean NvM_Prv_MemIf_Cancel(NvM_Prv_Job_tst* Job_pst);

extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Read_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Write_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Invalidate_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Maintain_GetData(void);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"


/* NVM_PRV_MEMIF_H */
#endif

