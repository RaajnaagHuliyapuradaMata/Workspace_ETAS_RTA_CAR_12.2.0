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

#ifndef RBA_FEEFS1X_PRV_PAREADTYPES_H
#define RBA_FEEFS1X_PRV_PAREADTYPES_H
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
typedef enum
{
    rba_FeeFs1x_PARead_unbufferedReadDoStm_checkAndPrepare_e,
    rba_FeeFs1x_PARead_unbufferedReadDoStm_read_e,
    rba_FeeFs1x_PARead_unbufferedReadDoStm_readfinal_e,
    rba_FeeFs1x_PARead_unbufferedReadDoStm_idle_e
}rba_FeeFs1x_PARead_unbufferedReadStm_ten;

typedef struct
{
    uint8 const * * adrPageInBuffer_ppcu8;
    boolean isBackwardBuffering_b;
    uint32 cntPages_u32;
    uint32 nrBytesPrefetch_u32;
}rba_FeeFs1x_PARead_getPages_tst;

typedef struct
{
    uint32 startPage_u32;
    uint32 nrOfBytes_u32;
    uint8* writeBuff_pu8;
    uint32 writeBuffOfset_u32;
    rba_FeeFs1x_PARead_unbufferedReadStm_ten state_en;
    uint8 startOfs_u8;
    boolean entry_b;
}rba_FeeFs1x_PARead_unbufRead_tst;

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

