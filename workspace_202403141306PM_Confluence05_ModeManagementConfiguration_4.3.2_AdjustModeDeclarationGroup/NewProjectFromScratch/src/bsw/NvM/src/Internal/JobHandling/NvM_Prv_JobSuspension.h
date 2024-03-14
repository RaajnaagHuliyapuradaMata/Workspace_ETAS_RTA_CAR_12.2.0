/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_JOBSUSPENSION_H
#define NVM_PRV_JOBSUSPENSION_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_JobSuspension_Handle(void);
extern void NvM_Prv_JobSuspension_PrepareHighSpeedMode(boolean isHighSpeedModeRequired_b);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"


/* NVM_PRV_JOBSUSPENSION_H */
#endif

