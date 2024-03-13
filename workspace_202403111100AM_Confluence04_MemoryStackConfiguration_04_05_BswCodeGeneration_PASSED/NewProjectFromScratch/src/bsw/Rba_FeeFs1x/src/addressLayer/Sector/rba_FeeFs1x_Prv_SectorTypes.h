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

#ifndef RBA_FEEFS1X_PRV_SECTORTYPES_H
#define RBA_FEEFS1X_PRV_SECTORTYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

# if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

#include "MemIf_Types.h"
#include "rba_FeeFs1x_Prv.h"
#include "rba_FeeFs1x_Prv_Cfg.h"


/* flash data storage layout of sector header marker */

/*****************************************************************************************************************************************
adress offset:      0x0             0x1       |    0x2     |     0x3            0x4             0x5        |     0x6             0x7
content:        HiBytePattern  LoBytePattern  | marker Id  | HiByteContent  MidByteContent  LowByteContent | HiByteChecksum LoByteChecksum
 ******************************************************************************************************************************************/
/* defines for marker accesses */

#define RBA_FEEFS1X_SECTOR_M_SIZE                 8u          /* Each marker has a size of 8 Bytes (fix value is used in Fee_Process.pm) */
#define RBA_FEEFS1X_SECTOR_M_PATTERN              0xCAFEu     /* pattern to identify a marker */

#define RBA_FEEFS1X_SECTOR_M_PATTERN_HIBYTE_POS   0u
#define RBA_FEEFS1X_SECTOR_M_PATTERN_LOBYTE_POS   1u

#define RBA_FEEFS1X_SECTOR_M_ID_POS               2u

#define RBA_FEEFS1X_SECTOR_M_CONTENT_HIBYTE_POS   3u
#define RBA_FEEFS1X_SECTOR_M_CONTENT_MIDBYTE_POS  4u
#define RBA_FEEFS1X_SECTOR_M_CONTENT_LOBYTE_POS   5u

#define RBA_FEEFS1X_SECTOR_M_CHECKSUM_HIBYTE_POS  6u
#define RBA_FEEFS1X_SECTOR_M_CHECKSUM_LOBYTE_POS  7u
#define RBA_FEEFS1X_SECTOR_M_CHECKSUM_SIZE        2u          /* size of checksum [bytes] */

/* mimimum required space for a sector header marker is the maximum of the marker size and the virtual page size */
#  if((RBA_FEEFS1X_SECTOR_M_SIZE) < (RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE))
#define RBA_FEEFS1X_SECTOR_M_MIN_MEM_SIZE RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE
#  else
#define RBA_FEEFS1X_SECTOR_M_MIN_MEM_SIZE RBA_FEEFS1X_SECTOR_M_SIZE
#  endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

typedef enum
{
    rba_FeeFs1x_Sector_Marker_ERASED_e = 0x01u,     /* Sector was erased successfully */
    rba_FeeFs1x_Sector_Marker_USED_e,               /* Sector was used to store data */
    rba_FeeFs1x_Sector_Marker_FULL_e,               /* Sector is full */
    rba_FeeFs1x_Sector_Marker_ERASE_REQUEST_e,      /* Sector should be erased */
    rba_FeeFs1x_Sector_Marker_START_e,              /* Double storage of USED marker, contains address of first blockheader */
    rba_FeeFs1x_Sector_Marker_CLONE_e,              /* Marker to indicate that cloning of a defect sector has startet in this sector --> Ifx robust programming */
    rba_FeeFs1x_Sector_Marker_ID1_e,                /* Reserved range marker space in the sector header, currently not used! */
    rba_FeeFs1x_Sector_Marker_ID2_e,                /* Reserved range marker space in the sector header, currently not used! */
    rba_FeeFs1x_Sector_Marker_ID3_e,                /* Reserved range marker space in the sector header, currently not used! */
    rba_FeeFs1x_Sector_Marker_ID4_e,                /* Reserved range marker space in the sector header, currently not used! */
    rba_FeeFs1x_Sector_Marker_ID5_e,                /* Reserved range marker space in the sector header, currently not used! */
    rba_FeeFs1x_Sector_Marker_ERASE_START_FLAG_e,   /* Erase start flag */
    RBA_FEEFS1X_SECTOR_NUM_MARKER_E                 /* Amount of required markers in the sector header + 1 */
} rba_FeeFs1x_Sector_markerStates_ten;

/* info about state of the sector change counter */
typedef enum
{
    rba_FeeFs1x_Sector_CngCnt_Valid_e,
    rba_FeeFs1x_Sector_CngCnt_Invalid_e,
    rba_FeeFs1x_Sector_CngCnt_Invalidated_e
} rba_FeeFs1x_Sector_CngCntStates_ten;

typedef enum
{
    rba_FeeFs1x_Sector_markSectorDoStm_idle_e,
    rba_FeeFs1x_Sector_markSectorDoStm_write_e
} rba_FeeFs1x_Sector_markSectorStm_ten;

typedef enum
{
    rba_FeeFs1x_Sector_init_stm_idle_e,
    rba_FeeFs1x_Sector_init_stm_readMarker_e,
    rba_FeeFs1x_Sector_init_stm_blankCheckMarker_e,
    rba_FeeFs1x_Sector_init_stm_evaluateMarker_e,
    rba_FeeFs1x_Sector_init_stm_blankCheckLastPage_e,
    rba_FeeFs1x_Sector_init_stm_detInfoSect_e
} rba_FeeFs1x_Sector_init_stm_ten;

typedef enum
{
    rba_FeeFs1x_Sector_eraseStm_idle_e,
    rba_FeeFs1x_Sector_eraseStm_writeEraseStartFlag_e,
    rba_FeeFs1x_Sector_eraseStm_erase_e,
    rba_FeeFs1x_Sector_eraseStm_wrEraseMarker_e
} rba_FeeFs1x_Sector_erase_ten;

/* Sector marker structure */
typedef struct
{
    uint16 xPattern_u16;                                      /* Static marker pattern: 0xCAFE */
    rba_FeeFs1x_Sector_markerStates_ten sectorMarkerState_en; /* Ident to identify the marker */
    uint32 xContent_u32;                                      /* e.g. sector change counter */
    uint16 xChecksum_u16;                                     /* Crc16 checksum, calculation over Pattern, Ident and content */
} rba_FeeFs1x_Sector_marker_tst;

typedef struct
{
    uint32 logSectorPages_u32;             /* number of logical pages per sector */
    uint16 markerPattern_u16;              /* marker pattern */
    uint8  logSectorStartOffset_u8;        /* offset to start address of the logical sector area*/
    uint8  xSectorMarkers_u8;              /* number of available sector header markers */
    uint8  xSectors_u8;                    /* number of available sectors */
    uint8  markerSize_u8;                  /* size of sector header marker[bytes] */
    uint8  markerDataSize_u8;              /* size of sector header marker data without CRC value[bytes] */
    uint8  addrOffsetToLastPageInMarker_u8;/* address offset[byte] to the last physical page in the sector header marker */
    uint8  physPageLen_u8;                 /* length of a physical page[bytes] */
    uint8  logPageLen_u8;                  /* length of a logical page[bytes] */
    uint8  markerMemLen_u8;                /* minimum required memory space for the sector header marker[bytes] */
    uint8  readWriteRetries_u8;            /* number of read retries */
    uint8  complementErasePattern_u8;      /* complement erase pattern */
} rba_FeeFs1x_Sector_const_tst;

typedef struct
{
    uint32 maxSectCngCnt_u32;
} rba_FeeFs1x_Sector_data_tst;

typedef struct
{
    uint32                              sectorCngCnt_u32;
    uint32                              startMarkerContent_u32;

    rba_FeeFs1x_Sector_SectorState_ten  sectorState_en;

    uint8                               sectorMarkerWriteIdx_u8;
    uint8                               physSectIdx_u8;
} rba_FeeFs1x_Sector_SectorListElement_tst;

typedef struct
{
    uint32                              sectorCngCnt_u32;
    uint32                              startMarkerContent_u32;

    rba_FeeFs1x_Sector_SectorState_ten  sectorState_en;
    rba_FeeFs1x_Sector_CngCntStates_ten stateSectorCngCnt_en;

    uint8                               idxPhySect_u8;
    uint8                               xMarkerSuspicious_u8;
    uint8                               sectorMarkerWriteIdx_u8;
} rba_FeeFs1x_Sector_unsortedSectorInfo_tst;

typedef struct
{
    uint8                                   writeBfr_au8[RBA_FEEFS1X_SECTOR_M_MIN_MEM_SIZE];

    rba_FeeFs1x_Sector_markSectorStm_ten    state_en;
    rba_FeeFs1x_Sector_SectorState_ten      sectorState_en;

    uint8                                   logSectIdx_u8;
    uint8                                   physSectIdx_u8;
    uint8                                   writeRetries_u8;
    boolean                                 entry_b;
} rba_FeeFs1x_Sector_markSector_tst;

typedef struct
{
    uint32                          eraseStartAddr_u32;
    uint32                          eraseLength_u32;

    uint8                           WriteBfr_au8[RBA_FEEFS1X_SECTOR_M_MIN_MEM_SIZE];

    rba_FeeFs1x_Sector_erase_ten    state_en;

    uint8                           logSectIdx_u8;
    boolean                         entry_b;
    boolean                         isReerasing_b;
} rba_FeeFs1x_Sector_erase_tst;

/* do not change order or sizes of structure elements */
typedef struct
{
    uint16 pattern_u16;                     /* marker preamble: 0xCAFE[2byte] */
    uint8 id_u8;                            /* marker Id: Used, Full, ... */
    uint8 contentHiByte_u8;                 /* bits[16..23] of the marker content */
    uint8 contentMidByte_u8;                /* bits[8..15] of the marker content */
    uint8 contentLoByte_u8;                 /* bits[0..7] of the marker content */
} rba_FeeFs1x_Sector_crcContent_tst;

typedef struct
{
    uint32                                      sectorMarkerAddr_u32;
    rba_FeeFs1x_Sector_unsortedSectorInfo_tst*  sectorInfo_ast;

    rba_FeeFs1x_Sector_marker_tst               marker_st;
    rba_FeeFs1x_Sector_init_stm_ten             state_en;

    uint8                                       readRetries_u8;
    uint8                                       markerPos_u8;
    boolean                                     isMarkerBlank_b;
    boolean                                     entry_b;
} rba_FeeFs1x_Sector_init_detInfoSingleMarker_data_tst;

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

# endif
#endif

