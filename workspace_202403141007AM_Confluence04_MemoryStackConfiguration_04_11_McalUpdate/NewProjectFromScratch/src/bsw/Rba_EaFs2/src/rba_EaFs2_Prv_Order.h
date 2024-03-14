/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef RBA_EAFS2_PRV_ORDER_H
#define RBA_EAFS2_PRV_ORDER_H

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

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

typedef enum
{
    EA_RB_NO_ORDER_E =0,
    EA_PRV_STOP_MODE_E,
    EA_PRV_READ_ORDER_E,
    EA_PRV_WRITE_ORDER_E,
    EA_PRV_INVALIDATE_ORDER_E,
    EA_PRV_BLOCKMAINTAIN_ORDER_E
}Ea_Prv_OrderType_ten;

typedef struct
{
    Ea_Prv_OrderType_ten    OrderType_en;       /* Holds the current request under processing */
    uint8                  *bfr_pu8;
    uint8            const *bfr_pcu8;
    uint16                  blockNumber_u16;    /* Block Number under processing */
    uint16                  length_u16;         /* Configured length of the Block under processing */
    uint16                  offset_u16;         /* Offset from the start address of the block */
}Ea_Prv_Order_tst;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

extern Ea_Prv_Order_tst Ea_Prv_Order_st;

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

extern MemIf_JobResultType rba_EaFs2_Prv_BgoDo(void);
extern MemIf_JobResultType rba_EaFs2_Prv_ReadDo(void);
extern MemIf_JobResultType rba_EaFs2_Prv_WriteDo(void);
#if(EA_AR_RELEASE_MINOR_VERSION >= 5)
extern void rba_EaFs2_Prv_SetModeRequest(MemIf_ModeType Mode);
#endif /* EA_AR_RELEASE_MINOR_VERSION */

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */

#endif /* RBA_EAFS2_PRV_ORDER_H */
