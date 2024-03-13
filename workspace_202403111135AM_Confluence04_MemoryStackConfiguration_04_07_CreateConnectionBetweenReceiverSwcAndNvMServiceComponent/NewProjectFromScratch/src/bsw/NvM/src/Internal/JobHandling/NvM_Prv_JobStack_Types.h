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
#ifndef NVM_PRV_JOBSTACK_TYPES_H
#define NVM_PRV_JOBSTACK_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Cfg.h"
#include "NvM_Prv_JobStateMachine_Types.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * Definition of the structure for an entry on the job stack used for execution of a job state machine.
 */
typedef struct
{
    NvM_Prv_idJobStateMachine_ten   idJobStateMachine_en;
    NvM_Prv_stJob_ten               stJob_en;
    boolean                         isStateMachineReady_b;

} NvM_Prv_JobStack_Entry_tst;

/**
 * Definition of the structure for the job stack used for NvM internal job execution.
 */
typedef struct
{
    NvM_Prv_JobStack_Entry_tst Entries_ast[NVM_CFG_JOB_STACK_SIZE];
    uint8 idxJobStack_uo;

} NvM_Prv_JobStack_tst;

/* NVM_PRV_JOBSTACK_TYPES_H */
#endif

