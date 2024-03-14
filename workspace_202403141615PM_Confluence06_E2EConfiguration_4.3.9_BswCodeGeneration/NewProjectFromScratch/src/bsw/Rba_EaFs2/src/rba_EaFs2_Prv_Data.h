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

#ifndef RBA_EAFS2_PRV_DATA_H
#define RBA_EAFS2_PRV_DATA_H

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
#define RBA_EAFS2_PRV_VALIDPATTERN   0xCAFEu
#define RBA_EAFS2_PRV_INVALIDPATTERN 0xBADDu

/*
 * Last four bits of the following macros are used to mention the state of each copy
 * +----+------+---------------+---------------+
 * |    | .... |  isInvalidCopy|  isConsistent |
 * +----+------+-------+-------+-------+-------+
 * |    | .... | Copy 2| Copy 1| Copy 2|Copy 1 |
 * +----+------+-------+-------+-------+-------+
 * |  0 |  CC  |   0   |   0   |   0   |   0   |
 * |  1 |  VC  |   0   |   0   |   0   |   1   |
 * |  2 |  CV  |   0   |   0   |   1   |   0   |
 * |  3 |  VV  |   0   |   0   |   1   |   1   |
 * |  5 |  IC  |   0   |   1   |   0   |   1   |
 * |  7 |  IV  |   0   |   1   |   1   |   1   |
 * | 10 |  CI  |   1   |   0   |   1   |   0   |
 * | 11 |  VI  |   1   |   0   |   1   |   1   |
 * | 15 |  II  |   1   |   1   |   1   |   1   |
 * +----+------+-------+-------+-------+-------+
 *
 **/
#define RBA_EAFS2_PRV_DATACOPYSTATE_CC 0u
#define RBA_EAFS2_PRV_DATACOPYSTATE_VC 1u
#define RBA_EAFS2_PRV_DATACOPYSTATE_CV 2u
#define RBA_EAFS2_PRV_DATACOPYSTATE_VV 3u
#define RBA_EAFS2_PRV_DATACOPYSTATE_IC 5u
#define RBA_EAFS2_PRV_DATACOPYSTATE_IV 7u
#define RBA_EAFS2_PRV_DATACOPYSTATE_CI 10u
#define RBA_EAFS2_PRV_DATACOPYSTATE_VI 11u
#define RBA_EAFS2_PRV_DATACOPYSTATE_II 15u


#define RBA_EAFS2_PRV_ECC_BYTES 4u

#define RBA_EAFS2_PRV_DATLENGTH(LEN)  (( ( (LEN)+(RBA_EAFS2_PRV_ECC_BYTES) - 1u )/ (RBA_EAFS2_PRV_ECC_BYTES) )*(RBA_EAFS2_PRV_ECC_BYTES))

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/* States for the Data access from eeprom */
typedef enum
{
    RBA_EAFS2_PRV_DATA_IDLE_E = 0,     /* Nothing to do                              */
    RBA_EAFS2_PRV_DATA_INIT_E,
    RBA_EAFS2_PRV_DATA_SPLITSTART_E,   /* find physical address for split access*/
    RBA_EAFS2_PRV_DATA_CALL_E,
    RBA_EAFS2_PRV_DATA_CHECK_E,
    RBA_EAFS2_PRV_DATA_FINAL_E
}rba_EaFs2_Prv_DataState_ten;

typedef enum
{
    RBA_EAFS2_PRV_DAT_FUNC_READ_E = 0,
    RBA_EAFS2_PRV_DAT_FUNC_WRITE_E,
    RBA_EAFS2_PRV_DAT_FUNC_COMPARE_E,
    RBA_EAFS2_PRV_DAT_FUNC_BLANKCHECK_E,
    RBA_EAFS2_PRV_DAT_FUNC_CRC_E
}rba_EaFs2_Prv_DatFunc_ten;

typedef enum
{
    RBA_EAFS2_PRV_DATAREPAIR_IDLE_E = 0,
    RBA_EAFS2_PRV_DATAREPAIR_INIT_E,
    RBA_EAFS2_PRV_DATAREPAIR_READMETA_E,
    RBA_EAFS2_PRV_DATAREPAIR_VERIFYCRC_E,
    RBA_EAFS2_PRV_DATAREPAIR_READDATA_E,
    RBA_EAFS2_PRV_DATAREPAIR_PROCESS_E,
    RBA_EAFS2_PRV_DATAREPAIR_WRITEDATA_E,
    RBA_EAFS2_PRV_DATAREPAIR_OPENMETA_E,
    RBA_EAFS2_PRV_DATAREPAIR_CLOSEMETA_E,
    RBA_EAFS2_PRV_DATAREPAIR_INVMETA_E,
#if (EA_WRITE_SPREAD != FALSE)
    RBA_EAFS2_PRV_DATAREPAIR_GETLOCATION_E,
    RBA_EAFS2_PRV_DATAREPAIR_UPDATECOUNTER_E,
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    RBA_EAFS2_PRV_DATAREPAIR_FINAL_E
}rba_EaFs2_Prv_DataRepair_en;

typedef enum
{
    RBA_EAFS2_PRV_WIP_COMPLETE_E =0,
    RBA_EAFS2_PRV_WIP_ONGOING_E
}rba_EaFs2_Prv_wipState_en;

/*
 * Enum to mention the copy status of block
 *
 * VALID   - 0xCAFE
 * INVALID - other than 0xCAFE
 *
 * Donot modify the enum values, impact in function rba_EaFs2_Prv_DataRepair_ReadMeta
 */
typedef enum
{
    RBA_EAFS2_PRV_BLOCK_VALID_E =0,
    RBA_EAFS2_PRV_BLOCK_INVALIDATED_E
}rba_EaFs2_Prv_blkStatus_en;

/*
 * #----------------------------------!!ATTENTION!!------------------------------------#
 * #---------------Do not modify this structure, written in Layout---------------------#
 * #-----------------------------------------------------------------------------------#
 */
typedef struct
{
    uint16 crc_u16;             /* 2 Byte Partial CRC of the Metadata       */
    uint8  validity_u8;         /* Holds 1 byte validity info for the block */
    uint8  wipState_u8;         /* Holds 1 byte WIP state for the block     */
}rba_EaFs2_Prv_metaDataPart_tst;

/*
 * #----------------------------------!!ATTENTION!!------------------------------------#
 * #---------------Do not modify this structure, written in Layout---------------------#
 * #-----------------------------------------------------------------------------------#
 */
typedef struct
{
    rba_EaFs2_Prv_metaDataPart_tst part1_st;        /* First 4 bytes of the MetaData */
    rba_EaFs2_Prv_metaDataPart_tst part2_st;        /* Last 4 bytes of the MetaData  */
}rba_EaFs2_Prv_metaDataEntry_tst;

typedef struct
{
    rba_EaFs2_Prv_wipState_en       wip_en;             /* Decode WIP state of the metadata            */
    rba_EaFs2_Prv_blkStatus_en      status_en;          /* Decoded Block status Valid or invalid       */
    rba_EaFs2_Prv_metaDataEntry_tst metaDataEntry_st;   /* MetaData Aligned to write in EEPROM         */
    uint32                          crc_u32;            /* Combined CRC used for computation           */
    uint16                          wipPattern_u16;     /* Combined WIP pattern used for manipulation  */
    boolean                         isConsistent_b;     /* Flag to indicate if the block is consistent */
}rba_EaFs2_Prv_metaBlock_tst;

/* Data for the SPLIT functionality  */
typedef struct
{
    rba_EaFs2_Prv_DataState_ten     state_en;              /* State of the state machine                              */
    rba_EaFs2_Prv_DatFunc_ten       func_en;               /* Function to executed spanning over several data sectors */
    uint32                          adrNow_u32;            /* Current address                                         */
    uint32                          addrDelta_u32;         /* Store difference between requested and Ram address      */
    uint32                        * crc_pu32;              /* Pointer to store the calculated crc                     */
    uint16                          length_u16;            /* Length to complete in bytes                             */
    uint16                          nrBytesDone_u16;       /* Already completed data in bytes                         */
    uint16                          nrBytesToConsider_u16; /* Amount of bytes that are currently handled              */
    uint16                          blocknumber_u16;       /* Curent block number                                     */
    uint8                           splitIter_u8;          /* Iterator to scan through each regions                   */
    uint8                   const * bfr_pcu8;              /* Pointer to const buffer - for writes                    */
    uint8                         * bfr_pu8;               /* Pointer to variable buffer - for reads                  */
    boolean                         entry_b;               /* State will be entered or is already entered             */
} rba_EaFs2_Prv_Data_tst;

typedef struct
{
    rba_EaFs2_Prv_Block_tst         block_st;               /* FAT block structure                                  */
    rba_EaFs2_Prv_DataRepair_en     state_en;               /* State of the data Repair state machine               */
    rba_EaFs2_Prv_metaBlock_tst     metaBlock1_st;          /* Place holder for the metadata of copy 1              */
    rba_EaFs2_Prv_metaBlock_tst     metaBlock2_st;          /* Place holder for the metadata of copy 2              */
    rba_EaFs2_Prv_metaBlock_tst   * metaBlock_pst;          /* Pointer to select the active metadata structure      */
    Ea_Prv_Order_tst        const * order_pcst;             /* Order structure as updated from user request         */
    uint32                          readAddr_u32;           /* Address From where the data need to be read          */
    uint32                          writeAddr_u32;          /* Address to where the data need to be written         */
    uint32                          calculatedCRC_u32;      /* CRC calculated after reading the data to verify      */
#if (EA_WRITE_SPREAD != FALSE)
    uint32                          blockWriteCounter_u32[2]; /* Number of times write spread block is written. used only when write spread is enabled */
    uint32                          blockWriteCounterT1_u32;/*Number of times write spread block is written at previous location. used only when write spread is enabled*/
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    uint16                          remainingLength_u16;    /* Length remaining to copy inorder to reapir the block */
    uint16                          blockLength_u16;        /* Length of a copy which is ECC aligned with metadata  */
    uint16                          lengthToConsider_u16;   /* Length of the active data under copy                 */
    uint8                           currCopy_u8;            /* Points to the avtive copy (1 or 2)                   */
    uint8                           copyState_u8;           /* Mentions the Direction of copy (1 -> 2) or (2 -> 1)  */
#if (EA_WRITE_SPREAD != FALSE)
    uint8                           CurrentLocation_u8;    /* Indicate the current Location to clear all counters for write spread blocks*/
    uint8                           currCopyCounter_u8;    /* Inidcates the current copy of counter of the write spread block*/
    boolean                         CurrentLocationFound_b;
#endif /*(EA_WRITE_SPREAD != FALSE)*/
    boolean                         doInvalidate_b;         /* Indicates if the copy needs to be invalidated        */
    boolean                         entry_b;                /* State will be entered or is already entered          */
}rba_EaFs2_Prv_DataRepair_tst;

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
#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

extern void rba_EaFs2_Prv_DataRead(uint32 addr_u32,
        uint8 * bfr_pu8,
        uint16 length_u16,
        uint16 blocknumber_u16
);
extern void rba_EaFs2_Prv_DataWrite(uint32 addr_u32,
        uint8 const * bfr_pcu8,
        uint16 length_u16,
        uint16 blocknumber_u16
);
extern void rba_EaFs2_Prv_DataCrcVerify(uint32 addr_u32,
        uint16 length_u16,
        uint16 blocknumber_u16,
        uint32 * crc_u32
);
extern void rba_EaFs2_Prv_DataCompare(uint32 addr_u32,
        uint8 const * bfr_pcu8,
        uint16 length_u16,
        uint16 blocknumber_u16
);

extern void rba_EaFs2_Prv_DataRepair (void);

void rba_EaFs2_Prv_DataOpenMetaData       (rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16);
void rba_EaFs2_Prv_DataReadMetaData       (rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16);
void rba_EaFs2_Prv_DataInvalidateMetaData (rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16);
void rba_EaFs2_Prv_DataEmptyMetaData      (rba_EaFs2_Prv_metaBlock_tst * metaBlock_pst,uint32 addr_u32,uint16 blocknumber_u16);
void rba_EaFs2_Prv_DataCloseMetaData      (rba_EaFs2_Prv_metaBlock_tst const * metaBlock_pcst,uint32 addr_u32,uint16 blocknumber_u16);

extern void rba_EaFs2_Prv_DecodeMetaData  (rba_EaFs2_Prv_metaBlock_tst * metaData_pst);

#if (EA_WRITE_SPREAD != FALSE)
extern void rba_EaFs2_Prv_DataBlockCounterRead    (uint32 * WriteCounterBlock_u32,uint32 addr_u32,uint16 blocknumber_u16);
extern void rba_EaFs2_Prv_BlockCounterWrite           (uint32 addr_u32,uint8 const * bfr_pcu8,uint16 length_u16,uint16 blocknumber_u16);
#endif /*(EA_WRITE_SPREAD != FALSE)*/

extern MemIf_JobResultType rba_EaFs2_Prv_DataDo       (void);
extern MemIf_JobResultType rba_EaFs2_Prv_DataRepairDo (void);

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif /* (EA_RB_CFG_SELECTED_FS == 2) */

#endif /* RBA_EAFS2_PRV_DATA_H */
