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

#ifndef RBA_FEEFS1X_PRV_PAWRITETYPES_H
#define RBA_FEEFS1X_PRV_PAWRITETYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv.h"
#include "Fee_Prv_Lib.h"

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
    rba_FeeFs1x_PAWrite_prepareSpace_stm_idle_e,
    rba_FeeFs1x_PAWrite_prepareSpace_stm_prepSectSt_e,
    rba_FeeFs1x_PAWrite_prepareSpace_stm_checkHdrFit_e,
    rba_FeeFs1x_PAWrite_prepareSpace_stm_fillSectEnd_e,
    rba_FeeFs1x_PAWrite_prepareSpace_stm_switchActive_e,
    rba_FeeFs1x_PAWrite_prepareSpace_stm_checkMem_e,
    rba_FeeFs1x_PAWrite_prepareSpace_stm_fillSectorForHSR_e,
    rba_FeeFs1x_PAWrite_prepareSpace_stm_execHSR_e
}rba_FeeFs1x_PAWrite_prepareSpace_stm_ten;



typedef struct
{
    uint32 noDataPages_u32;
    uint32 * startPageForRequest_pu32;
    rba_FeeFs1x_PAWrite_prepareSpace_stm_ten state_en;
    uint8 noHdrPages_u8;
    boolean isHSRCheckRequired_b;
    boolean entry_b;
}rba_FeeFs1x_PAWrite_prepareSpace_data_tst;

typedef enum
{
    rba_FeeFs1x_PAWrite_write_stm_idle_e,
    rba_FeeFs1x_PAWrite_write_stm_limitChunkToSect_e,
    rba_FeeFs1x_PAWrite_write_stm_writeChunk_e,
    rba_FeeFs1x_PAWrite_write_stm_switchActiveSect_e
}rba_FeeFs1x_PAWrite_write_stm_ten;

typedef struct
{
    uint32 startPage_u32;
    uint32 nrBytesRemaining_u32;
    uint32 nextChunkPage_u32;
    uint32 currChunkPage_u32;
    uint32 currChunkNrBytes_u32;
    uint32 nrBytesDone_u32;

    uint8 const * sourcebuffer_pcu8;

    rba_FeeFs1x_PAWrite_write_stm_ten state_en;
    Fee_Prv_MediumSeqWrite_ten writeSequence_en;

    boolean updateCurrWrPage_b;
    boolean entry_b;
}rba_FeeFs1x_PAWrite_write_data_tst;


typedef enum
{
    rba_FeeFs1x_PAWrite_copyFls2Fls_stm_idle_e,
    rba_FeeFs1x_PAWrite_copyFls2Fls_stm_limit_e,
    rba_FeeFs1x_PAWrite_copyFls2Fls_stm_copy_e,
    rba_FeeFs1x_PAWrite_copyFls2Fls_stm_switch_e
}rba_FeeFs1x_PAWrite_copyFls2Fls_stm_ten;

typedef struct
{
    uint32 noOfPagesRemaining_u32;
    uint32 noOfPagesDone_u32;
    uint32 nextChunkSourcePage_u32;
    uint32 nextChunkDestPage_u32;
    uint32 currChunkSourcePage_u32;
    uint32 currChunkDestPage_u32;
    uint32 currChunkNoOfPages_u32;

    rba_FeeFs1x_PAWrite_copyFls2Fls_stm_ten state_en;
    boolean entry_b;
    boolean currChunkIsLimitedByDest_b;
    boolean updateCurrWrPage_b;
}rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst;

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

