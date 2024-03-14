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

#ifndef RBA_FEEFS1X_PRV_RB_MAINTAINJOBTYPES_H
#define RBA_FEEFS1X_PRV_RB_MAINTAINJOBTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"

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
// structure for Maintain job
typedef struct
{
    uint16  feeIndex_u16;       // Fee index of the block which has to be maintained
    boolean writeTwice_b;       // Flag to indicate if block has to be written twice
}rba_FeeFs1x_MtJob_JobData_tst;

// state machine for check maintain operation
typedef enum
{
    rba_FeeFs1x_MtJob_checkMt_stm_idle_e,                          // Idle state, default state of the state machine
    rba_FeeFs1x_MtJob_checkMt_stm_chkSearchResultCompToCfg_e,      // Check the search result and check if the status flag of the latest copy in DFLASH is different from config value
    rba_FeeFs1x_MtJob_checkMt_stm_cmpCopies_e                      // Compare the header and data of the 2 copies
}rba_FeeFs1x_MtJob_checkMt_stm_ten;

// structure for check maintain operation
typedef struct
{
    rba_FeeFs1x_MtJob_checkMt_stm_ten   state_en;         // state machine for check operation
    boolean                             entry_b;          // flag to indicate if this is the first entry in the state

    rba_FeeFs1x_Searcher_RetVal_ten     retValSearch_en;  // return value for the search operation
}rba_FeeFs1x_MtJob_checkMt_tst;

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

