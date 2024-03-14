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

#ifndef RBA_EAFS2_PRV_EEPROM_H
#define RBA_EAFS2_PRV_EEPROM_H

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
#if(defined(EEP_NAME_USED) && (EEP_NAME_USED == 1))
/* Mapping of read/write to EEP because of name conflict if EEP used */
#define EA_EEP_READ(EepromAddress, DataBufferPtr, Length)      Eep_Rb_Read(EepromAddress, DataBufferPtr, Length)
#define EA_EEP_WRITE(EepromAddress, DataBufferPtr, Length)     Eep_Rb_Write(EepromAddress, DataBufferPtr, Length)

#else

#define EA_EEP_READ(EepromAddress, DataBufferPtr, Length)      Eep_Read(EepromAddress, DataBufferPtr, Length)
#define EA_EEP_WRITE(EepromAddress, DataBufferPtr, Length)     Eep_Write(EepromAddress, DataBufferPtr, Length)

#endif
/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/* States for loading data from EEPROM to RAM */
typedef enum
{
    RBA_EAFS2_PRV_LOAD_IDLE_E = 0,    /* Nothing to do        */
    RBA_EAFS2_PRV_LOAD_BUSY_E         /* Currently loading    */
} rba_EaFs2_Prv_EepromLoad_ten;


/* Prefetch modes */
typedef enum
{
    RBA_EAFS2_PRV_PRELOAD_E,
    RBA_EAFS2_PRV_NXTLOAD_E
} rba_EaFs2_Prv_EepromLoadMode_ten;

typedef union
{
    uint32 u32[EA_DATA_BUFFER_SIZE/4u];
    uint8  u8[EA_DATA_BUFFER_SIZE];
}rba_EaFs2_Prv_Buffer_tst;

typedef struct
{
    rba_EaFs2_Prv_EepromLoad_ten state_en;
    rba_EaFs2_Prv_Buffer_tst buffer_u;
    uint32 iterator_u32;
    uint32 upLimit_u32;
    uint32 lowLimit_u32;
    uint32 indexer_u32;
    uint32 loadAddr_u32;
    boolean sync_b;
    Std_ReturnType lastJobResult_en;
}rba_EaFs2_Prv_Eeprom_tst;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

extern rba_EaFs2_Prv_Eeprom_tst rba_EaFs2_Prv_Eeprom_st;

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

extern void rba_EaFs2_Prv_SetIterator   (uint32 addr_u32);
extern void rba_EaFs2_Prv_EepromErase   (uint32 addr_u32, uint32 length_u32);
extern void rba_EaFs2_Prv_EepromRead    (uint32 addr_u32, uint8       * buffer_pu8,  uint32 length_u32);
extern void rba_EaFs2_Prv_EepromWrite   (uint32 addr_u32, uint8 const * buffer_pcu8, uint32 length_u32);
extern void rba_EaFs2_Prv_EepromCompare (uint32 addr_u32, uint8 const * buffer_pcu8, uint32 length_u32);

extern MemIf_JobResultType rba_EaFs2_Prv_EepromLoad(uint32 addr_u32,uint16 len_u16,rba_EaFs2_Prv_EepromLoadMode_ten Mode_en);
extern void rba_EaFs2_Prv_PrepareEeprom(void);
extern MemIf_JobResultType rba_EaFs2_Prv_EepromStatus(void);

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */

#endif /* RBA_EAFS2_PRV_EEPROM_H */
