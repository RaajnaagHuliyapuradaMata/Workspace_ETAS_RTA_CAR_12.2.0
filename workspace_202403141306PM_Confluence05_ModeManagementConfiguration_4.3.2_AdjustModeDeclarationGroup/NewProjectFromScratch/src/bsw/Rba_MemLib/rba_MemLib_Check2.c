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

#include "rba_MemLib.h"
#include "rba_MemLib_Prv.h"
#include "rba_MemLib_CheckProto.h"

/* Preparation for integrity check (MemLibIntegrityCheck) */
#define RBA_MEMLIB_PRV_MAGIC_NUMBER   (20230311u)

/* Integration integrity check */
#if (!defined(RBA_MEMLIB_CHECKPROTO_MAGIC_NUMBER) || (RBA_MEMLIB_CHECKPROTO_MAGIC_NUMBER != (RBA_MEMLIB_PRV_MAGIC_NUMBER-2u)))
#error Integrity error in rba_MemLib_CheckProto.h integration - use the provided header only
#endif

