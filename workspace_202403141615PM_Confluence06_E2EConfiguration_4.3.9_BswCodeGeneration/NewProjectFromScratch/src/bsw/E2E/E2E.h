
#ifndef E2E_H
#define E2E_H

/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/
#include "Std_Types.h"                  /* AUTOSAR standard type definitions */
#include "Crc.h"                        /* AR module Crc, used by E2E profile 1, 2, 5, 4 and 6 */
#include "E2E_SM.h"                    /* E2E state machine, public interfaces */
#include "E2E_P01.h"                    /* E2E Profile 1, public interfaces */
#include "E2E_P02.h"                    /* E2E Profile 2, public interfaces */
#include "E2E_P04.h"                    /* E2E Profile 4, public interfaces */
#include "E2E_P05.h"                    /* E2E Profile 5, public interfaces */
#include "E2E_P06.h"                    /* E2E Profile 6, public interfaces */
#include "E2E_P07.h"                    /* E2E Profile 7, public interfaces */
#include "E2E_P11.h"                    /* E2E Profile 11, public interfaces */
#include "E2E_P22.h"                    /* E2E Profile 22, public interfaces */
#include "E2E_P44.h"                    /* E2E Profile 44, public interfaces */

/*
**********************************************************************************************************************
* Defines/Macros
**********************************************************************************************************************
*/

/* Version information parameters */
#define E2E_VENDOR_ID                    6U
#define E2E_MODULE_ID                    207U
#define E2E_SW_MAJOR_VERSION             5U
#define E2E_SW_MINOR_VERSION             0U
#define E2E_SW_PATCH_VERSION             0U
#define E2E_AR_RELEASE_MAJOR_VERSION     4U
#define E2E_AR_RELEASE_MINOR_VERSION     5U

/* Revision of E2E */
#define E2E_AR_RELEASE_REVISION_VERSION  2U

/* Error flags, used for the Std_ReturnType of the E2E routines */
#define E2E_E_INPUTERR_NULL              0x13U
#define E2E_E_INPUTERR_WRONG             0x17U
#define E2E_E_INTERR                     0x19U
#define E2E_E_OK                         0x00U
#define E2E_E_WRONGSTATE                 0x1AU

/* Warning "unused parameter",
 * systematically produced as the abstract interfaces (if) have more parameters than the specific ones */
#define E2E_PARAM_UNUSED(param)         (void)(param)

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

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 Auxiliary functions
 **********************************************************************************************************************
 */
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"

/* TRACE[SWS_E2E_00032] */
extern void E2E_GetVersionInfo(Std_VersionInfoType * VersionInfo);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_H */
#endif
