/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef BSWM_TYPES_H
#define BSWM_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "BswM_Cfg_General.h"

#if (BSWM_RTE_IN_USE == TRUE)
#include "Rte_BswM_Type.h"
#else

/* This type identifies the modes that can be requested by BswM Users. */
typedef uint16 BswM_ModeType;

/* This type identifies a BswM User that makes mode requests to the BswM. */
typedef uint16 BswM_UserType;

#endif
/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/


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


#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
