/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2020, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef RBA_EAFS2_PRV_DBG_H
#define RBA_EAFS2_PRV_DBG_H

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))
#if(FALSE != EA_PRV_DEBUGGING)
#include "rba_MemLib.h"

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */


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
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

typedef struct
{
    uint32 iTime_u32;
    uint32 fTime_u32;
    uint32 maxTime_u32;
    uint32 cycle_u32;
    uint32 maxCycle_u32;
}rba_EaFs2_Prv_time_tst;


#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define EA_START_SEC_CODE
#include "Ea_MemMap.h"


extern void rba_EaFs2_Prv_DebugTimeStart(rba_EaFs2_Prv_time_tst *debugTime_pst);
extern void rba_EaFs2_Prv_DebugTimeStop(rba_EaFs2_Prv_time_tst *debugTime_pst);

extern void  rba_EaFs2_Prv_Initialize_Adapter(void);
extern void  rba_EaFs2_Prv_Initialize_Defrag(void);
extern void  rba_EaFs2_Prv_Initialize_Eeprom(void);
extern void  rba_EaFs2_Prv_Initialize_Fat(void);
extern void  rba_EaFs2_Prv_Initialize_Init(void);
extern void  rba_EaFs2_Prv_Initialize_Order(void);
extern void  rba_EaFs2_Prv_Test_Initialize_EaFs2(void);
extern void rba_EaFs2_Prv_Initialize_GlobalVarible(void);




#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (FALSE != EA_PRV_DEBUGGING) */
#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
#endif /* RBA_EAFS2_PRV_DBG_H */
