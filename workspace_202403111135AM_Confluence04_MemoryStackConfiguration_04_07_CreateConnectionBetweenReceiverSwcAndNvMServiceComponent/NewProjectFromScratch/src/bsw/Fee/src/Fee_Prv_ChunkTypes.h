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

#ifndef FEE_PRV_CHUNKTYPES_H
#define FEE_PRV_CHUNKTYPES_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"


/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Fee_Rb_Types.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
/* Block status/property bits stored in the common header. Don't change position, this is stored in the medium (via
 * rba_MemBckUp). Note: This bit position is different from the PFLASH bit position in the Fee_PrvTypes.h. They are
 * not required to be in sync. */
#define FEE_PRV_CHUNK_DOUBLESTORAGE_BIT           (0x0001u)
#define FEE_PRV_CHUNK_INVALIDATE_BIT              (0x0008u)
#define FEE_PRV_CHUNK_SURVIVAL_BIT                (0x0010u)
#define FEE_PRV_CHUNK_NOFALLBACK_BIT              (0x0040u)
#define FEE_PRV_CHUNK_BLOCK_NOT_PRESENT_BIT       (0x0080u)


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/* Header layout for chunk-wise jobs
 * Attention: Any change in its layout isn't downward compatible, because it is stored by chunk-unit to medium!!! */
typedef struct
{
    uint16    idPers_u16;                                      /* Block's persistent ID (unique block identifier)    */
    uint16    nrLen_u16;                                       /* Block's payload length (in bytes, excluding CRC32) */
    uint16    stFlg_u16;                                       /* Block's status/property flags                      */
    uint8     xResv_au8[2];                                    /* Fill up reserve with 0x00                          */
    uint16    xCrc_u16;                                        /* CCITT-CRC16 over block header                      */
} Fee_Prv_ChunkHdr_tst;

/* Pointer to FeeFs1 GetJobResult function */
typedef MemIf_JobResultType (*Fee_Prv_ChunkGetJobResult_tpfct)(void);

/* Chunk-wise job status for FeeFs1*/
typedef struct
{
    uint32                    nrBytProc_u32;  /* Bytes processed in last/current chunk(-wise job), including header */
    Fee_Rb_JobResultType_ten  result_en;      /* Result of last/current chunk(-wise job)                            */
} Fee_Prv_ChunkInfo_tst;

/* # if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED)) */
# endif

/* #ifndef FEE_PRV_CHUNKTYPES_H */
#endif
