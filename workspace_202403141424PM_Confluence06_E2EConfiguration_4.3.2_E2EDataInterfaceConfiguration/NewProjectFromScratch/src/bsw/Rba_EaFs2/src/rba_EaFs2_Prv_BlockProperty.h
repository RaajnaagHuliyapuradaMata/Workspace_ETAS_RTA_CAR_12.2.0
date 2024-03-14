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

#ifndef RBA_EAFS2_PRV_BLOCKPROPERTY_H
#define RBA_EAFS2_PRV_BLOCKPROPERTY_H

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#include "Std_Types.h"
#include "rba_EaFs2_Prv.h"

typedef enum
{
    RBA_EAFS2_PRV_BLOCKPROPERTY_INIT_E=0,
    RBA_EAFS2_PRV_BLOCKPROPERTY_WRITE_FAT_E,
    RBA_EAFS2_PRV_BLOCKPROPERTY_FINAL_E
}rba_EaFs2_Prv_BlockProperty_ten;


typedef enum
{
    RBA_EAFS2_PRV_PROPERTYCHANGE_INIT_E = 0,
    RBA_EAFS2_PRV_PROPERTYCHANGE_FINDNEXTBLOCK_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_GETFREESPACE_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_METAREAD_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_DATAREAD_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_DATAWRITE_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_DATAVERIFY_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_WRITEFAT_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_CLEARFAT_E,
    RBA_EAFS2_PRV_PROPERTYCHANGE_FINAL_E
}rba_EaFs2_Prv_PropertyChange_ten;

typedef struct
{
    rba_EaFs2_Prv_BlockProperty_ten   state_en;                         /* State of the Defrag state machine                    */
    rba_EaFs2_Prv_Block_tst           blockToUpdate_st;                 /* FAT Block used to erase/overwrite/clear FAT entries  */
    uint16                            srcIdx_u16;                       /* hold source index when moving block */
    boolean                           entry_b;
}rba_EaFs2_Prv_BlockPropertyInfor_tst;

typedef struct
{
    rba_EaFs2_Prv_PropertyChange_ten state_en;                  /* Stores state of temp marker state machine*/
    rba_EaFs2_Prv_Block_tst         block_st;
    rba_EaFs2_Prv_Block_tst         blockToUpdate_st;
    rba_EaFs2_Prv_metaBlock_tst     metaBlock_st;        /* structure to hold metadata entry                     */
    rba_EaFs2_Prv_FatEntry_tst      fatUnderRead_st;
    uint32                          readAddress_u32;            /* address to read from                                 */
    uint32                          writeAddress_u32;           /* address to write to                                  */
    uint32                          dataBlockAddress_u32;
    uint16                          lengthToMove_u16;           /* length to move                                       */
    uint16                          dataBlockLength_u16;
    uint16                          srcLength_u16;              /* block length in medium of the current moving block   */
    uint16                          workingFatIndex_u16;
    uint16                          targetFatIndex_u16;
    uint16                          firstChangeIdx_u16;
    uint8                           curCopy_u8;
    boolean                         entry_b;
}rba_EaFs2_Prv_PropertyChange_tst;


extern rba_EaFs2_Prv_BlockPropertyInfor_tst rba_EaFs2_Prv_BlockPropertyInfo_st;
extern rba_EaFs2_Prv_PropertyChange_tst rba_EaFs2_Prv_PropertyChange_st;

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

extern MemIf_JobResultType rba_EaFs2_Prv_BlockPropertyUpdateDo(rba_EaFs2_Prv_BlockPropertyInfor_tst * fsm_pst);
extern MemIf_JobResultType rba_EaFs2_Prv_PropertyChangeDo ( void );

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"
#endif
#endif /* (EA_RB_CFG_SELECTED_FS == 2) */

