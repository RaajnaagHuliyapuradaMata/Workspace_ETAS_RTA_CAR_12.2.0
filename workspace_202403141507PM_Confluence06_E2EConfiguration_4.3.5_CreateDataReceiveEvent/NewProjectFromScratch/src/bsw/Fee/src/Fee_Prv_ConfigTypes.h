/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef FEE_PRV_CONFIGTYPES_H
#define FEE_PRV_CONFIGTYPES_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))
#include "Fee_Cfg.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

typedef struct
{
    uint16  BlockPersistentId_u16;
    uint16  Flags_u16;
    uint16  Length_u16;
} Fee_Rb_BlockPropertiesType_tst;

typedef struct
{
    uint32  Fee_PhysStartAddress_u32;    /* Physical sector: start address */
    uint32  Fee_PhysEndAddress_u32;      /* Physical sector: end address   */
} Fee_Rb_FlashProp_tst;

typedef enum
{
    FEE_FS10 = 0,
    FEE_FS1X,
    FEE_FS2,
    FEE_FS3
}Fee_Prv_Fs_ten;

# endif
/* FEE_PRV_CONFIGTYPES_H */
#endif
