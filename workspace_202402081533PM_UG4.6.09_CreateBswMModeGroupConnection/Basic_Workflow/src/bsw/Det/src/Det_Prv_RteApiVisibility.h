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

#ifndef DET_PRV_RTEAPIVISIBILITY_H
#define DET_PRV_RTEAPIVISIBILITY_H

#include "Std_Types.h"
#include "Det_Cfg.h"

/* If RTE is used then set the parameter EcuCRbRteInUse to True */
#if (DET_CFG_CHECK_API_CONSISTENCY == TRUE)
#define DET_HIDE_RTE_API
#endif

#endif /* DET_PRV_RTEAPIVISIBILITY_H */

