/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/


#ifndef EA_PRV_H
#define EA_PRV_H


/* This header is used to include the private headers based on file system selection */
#if(defined(EA_RB_CFG_SELECTED_FS) && (1 == EA_RB_CFG_SELECTED_FS))
#include "rba_EaFs1_Prv.h"
#include "Ea_Rb_EaFs1_EepDepndcyAdaptr.h"
#endif

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))
#include "rba_EaFs2_Prv.h"
#include "Ea_Rb_EepDepndcyAdaptr.h"
#endif

#endif /* EA_PRV_H */



