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

#ifndef RBA_FEEFS1X_PRV_CACHEKWNTYPES_H
#define RBA_FEEFS1X_PRV_CACHEKWNTYPES_H
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

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/*Unsigned int used according to coding guideline CCode_004*/
/*CCode_005: 32 Bit int required (implementation defined behavior)*/
typedef struct
{
    unsigned int pageNo_uo    :20;
    unsigned int reserved_uo  :9;
    unsigned int copyRedundantConfig_u1:1;
    unsigned int dataValid_u1  :1;
    unsigned int cacheValid_u1 :1;
}rba_FeeFs1x_CacheKwn_CacheElement_tst;

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

