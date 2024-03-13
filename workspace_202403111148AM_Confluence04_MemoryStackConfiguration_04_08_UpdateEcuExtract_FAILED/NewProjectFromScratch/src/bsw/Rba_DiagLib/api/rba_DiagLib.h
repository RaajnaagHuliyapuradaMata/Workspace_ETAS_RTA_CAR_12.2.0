/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2012, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef RBA_DIAGLIB_H
#define RBA_DIAGLIB_H

/**
 * \file
 * \brief rba_DiagLib-Interface. Only supported for Pac Diagnosis components.
 */

#include "rba_DiagLib_Bits8.h"
#include "rba_DiagLib_Bits16.h"
#include "rba_DiagLib_Bits32.h"
#include "rba_DiagLib_ByteOrderUtils.h"
#include "rba_DiagLib_MemUtils.h"

#define RBA_DIAGLIB_RANGECHECK(B)      "not defined yet. use component specific macro."
#define RBA_DIAGLIB_UNUSED_PARAM(P)   ((void)(P))
#define RBA_DIAGLIB_ASSERT_ISLOCKED()  "not defined yet. use component specific macro."

#endif
