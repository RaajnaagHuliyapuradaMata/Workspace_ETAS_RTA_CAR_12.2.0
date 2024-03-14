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

#ifndef MEMIF_H
#define MEMIF_H

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
#define FEE_DEVICE_INDEX   1u
#define EA_DEVICE_INDEX    2u

/* AUTOSAR error codes for API used for DET module                                                                  */
#define MEMIF_E_PARAM_DEVICE                0x01u   /* Error code for wrong device index                            */
/* RB specific error codes - counting up from 0x10                                                                  */
#define MEMIF_RB_E_BUSY                     0x10u   /* Error code for multiple mainfunction execution               */
#define MEMIF_RB_E_SERVICE_NOT_AVAIL        0x11u   /* Error code for a call to a service which is not available    */

/* AUTSOAR service IDs - starting value not defined by AUTOSAR                                                      */
#define MEMIF_SID_CHECK_INDEX               0x81u   /* Service ID for MemIf check device index function             */
/* RB specific service IDs - counting down from 0xFF                                                                */
#define MEMIF_RB_SID_MAIN_FUNCTION          0xFFu   /* Service ID for MemIf_Rb_MainFunction                         */
#define MEMIF_RB_SID_ENTER_HIGH_SPEED_MODE  0xFE    /* Service ID for MemIf_Rb_EnterHighSpeedMode                   */
#define MEMIF_RB_SID_EXIT_HIGH_SPEED_MODE   0xFD    /* Service ID for MemIf_Rb_ExitHighSpeedMode                    */

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "MemIf_Cfg.h"
#include "MemIf_Inl.h"

#if (defined(MEMIF_FEE_USED) && (MEMIF_FEE_USED == STD_ON))
#include "Fee.h"
#include "Fls.h"
#endif
#if (defined(MEMIF_NUM_OF_EA_DEVICES) && (MEMIF_NUM_OF_EA_DEVICES > 0u))
#include "Ea.h"
#endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define MEMIF_START_SEC_CODE
#include "MemIf_MemMap.h"

#if (defined(MEMIF_VERSION_INFO_API) && (MEMIF_VERSION_INFO_API == STD_ON))
extern void MemIf_GetVersionInfo( Std_VersionInfoType* VersionInfoPtr );
#endif /* MEMIF_VERSION_INFO_API */

extern MemIf_StatusType MemIf_Rb_GetStatus(void);

extern void MemIf_CheckDeviceIndex(uint8 DeviceIndex);

/* Remove Rte generated APIs if included in MemIf context */
#ifndef MEMIF_RB_HIDE_RTEAPI
/* Functions: scheduled */
extern void MemIf_Rb_MainFunction(void);
#endif

extern void MemIf_Rb_MainFunctionAndDependency (void);

#define MEMIF_STOP_SEC_CODE
#include "MemIf_MemMap.h"

/* MEMIF_H */
#endif
