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

#ifndef RBA_FEEFS1X_PRV_CACHEUNKWNTYPES_H
#define RBA_FEEFS1X_PRV_CACHEUNKWNTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

#define RBA_FEEFS1X_CACHEUNKWN_IDXNATCPY    0u
#define RBA_FEEFS1X_CACHEUNKWN_IDXREDCPY    1u
#define RBA_FEEFS1X_CACHEUNKWN_MAXCPY       2u

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/* CCode_004: use Unsigned int for bitfields*/
/* CCode_005: 32 Bit int required (implementation defined behavior)*/
typedef struct
{
    unsigned int pageNr_uo    :20;
    unsigned int reserved_uo  :9;
    unsigned int copyRedConfig_u1:1;
    unsigned int dataValid_u1  :1;
    unsigned int cacheValid_u1 :1;
}rba_FeeFs1x_CacheUnkwn_unkwnCacheElement_tst;

typedef struct
{
    rba_FeeFs1x_CacheUnkwn_unkwnCacheElement_tst cpy_st[RBA_FEEFS1X_CACHEUNKWN_MAXCPY]; // 1st element = NatCpy, 2nd element = RedCpy
    uint16 persID_u16;
}rba_FeeFs1x_CacheUnkwn_unkwnBlkCache_tst;

typedef struct
{
    uint16 idxNextFreeCachePos_u16;    // index pointing to the current cache entry
}rba_FeeFs1x_CacheUnkwn_tst;

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

