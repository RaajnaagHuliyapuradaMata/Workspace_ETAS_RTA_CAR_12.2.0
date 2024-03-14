

#ifndef E2E_P01_H
#define E2E_P01_H

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

/* TRACE [SWS_E2E_00200] */
typedef enum
{
     E2E_P01_DATAID_BOTH   = 0U,
     E2E_P01_DATAID_ALT    = 1U,
     E2E_P01_DATAID_LOW    = 2U,
     E2E_P01_DATAID_NIBBLE = 3U
} E2E_P01DataIDMode;

/* TRACE [SWS_E2E_00022]  */
typedef enum
{
    E2E_P01STATUS_OK            = 0x00U,
    E2E_P01STATUS_NONEWDATA     = 0x01U,
    E2E_P01STATUS_WRONGCRC      = 0x02U,
    E2E_P01STATUS_SYNC          = 0x03U,
    E2E_P01STATUS_INITIAL       = 0x04U,
    E2E_P01STATUS_REPEATED      = 0x05U,
    E2E_P01STATUS_OKSOMELOST    = 0x06U,
    E2E_P01STATUS_WRONGSEQUENCE = 0x07U
} E2E_P01CheckStatusType;

/* TRACE [SWS_E2E_00018] */
typedef struct
{
    uint16            CounterOffset;          /* bit Offset in MSB order */
    uint16            CRCOffset;              /* bit offset in MSB order */
    uint16            DataID;                 /* sender Identifier */
    uint16            DataIDNibbleOffset;     /* bit Offset in MSB order for Profile 1C */
    E2E_P01DataIDMode DataIDMode;             /* two byte, low byte, alternating */
    uint16            DataLength;             /* length of data in bits */
    uint8             MaxDeltaCounterInit;    /* maximum allowed gap, init value */
    uint8             MaxNoNewOrRepeatedData; /* maximum of missing or repeated data */
    uint8             SyncCounterInit;        /* number of data required for syncronization */
} E2E_P01ConfigType;

/* TRACE [SWS_E2E_00020]*/
typedef struct
{
    uint8 Counter;
} E2E_P01ProtectStateType;

/* TRACE [SWS_E2E_00021] */
typedef struct
{
    uint8                   LastValidCounter;
    uint8                   MaxDeltaCounter;
    boolean                 WaitForFirstData;
    boolean                 NewDataAvailable;
    uint8                   LostData;
    E2E_P01CheckStatusType  Status;
    uint8                   SyncCounter;
    uint8                   NoNewOrRepeatedDataCounter;
} E2E_P01CheckStateType;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * External declarations
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"

/* TRACE[SWS_E2E_00385] */
extern Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType *StatePtr);
/* TRACE[SWS_E2E_00166]*/
extern Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType *ConfigPtr, E2E_P01ProtectStateType *StatePtr,
                                     uint8 *DataPtr);
/* TRACE[SWS_E2E_00390] */
extern Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType *StatePtr);
/* TRACE[SWS_E2E_00158] */
extern Std_ReturnType E2E_P01Check(const E2E_P01ConfigType *ConfigPtr, E2E_P01CheckStateType *StatePtr,
                                   const uint8 *DataPtr);
/* TRACE[SWS_E2E_00382]*/
extern E2E_PCheckStatusType E2E_P01MapStatusToSM(Std_ReturnType CheckReturn, E2E_P01CheckStatusType Status,
                                                 boolean profileBehavior);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_P01_H */
#endif
