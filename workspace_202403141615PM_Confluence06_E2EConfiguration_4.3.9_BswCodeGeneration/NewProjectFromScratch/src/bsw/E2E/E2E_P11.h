

#ifndef E2E_P11_H
#define E2E_P11_H

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

/* TRACE [SWS_E2E_00507] */
typedef enum
{
     E2E_P11_DATAID_BOTH   = 0U,
     E2E_P11_DATAID_NIBBLE = 1U
} E2E_P11DataIDMode;

/* TRACE [SWS_E2E_00564] */
typedef enum
{
    E2E_P11STATUS_OK            = 0x00U,
    E2E_P11STATUS_NONEWDATA     = 0x01U,
    E2E_P11STATUS_REPEATED      = 0x08U,
    E2E_P11STATUS_OKSOMELOST    = 0x20U,
    E2E_P11STATUS_WRONGSEQUENCE = 0x40U,
    E2E_P11STATUS_ERROR         = 0x07U

} E2E_P11CheckStatusType;

/* TRACE [SWS_E2E_00565] */
typedef struct
{
    uint16            CounterOffset;          /* bit Offset in MSB order */
    uint16            CRCOffset;              /* bit offset in MSB order */
    uint16            DataID;                 /* sender Identifier */
    uint16            DataIDNibbleOffset;     /* bit Offset in MSB order for Profile 11C */
    E2E_P11DataIDMode DataIDMode;             /* two byte, low byte, alternating */
    uint16            DataLength;             /* length of data in bits */
    uint8             MaxDeltaCounter;        /* maximum allowed gap, init value */
} E2E_P11ConfigType;

/* TRACE [SWS_E2E_00567]*/
typedef struct
{
    uint8 Counter;
} E2E_P11ProtectStateType;

/* TRACE [SWS_E2E_00563] */
typedef struct
{
    E2E_P11CheckStatusType  Status;
    uint8                   Counter;
} E2E_P11CheckStateType;

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

/* TRACE[SWS_E2E_00576] */
extern Std_ReturnType E2E_P11ProtectInit(E2E_P11ProtectStateType *StatePtr);
/* TRACE[SWS_E2E_00575]*/
extern Std_ReturnType E2E_P11Protect(const E2E_P11ConfigType *ConfigPtr, E2E_P11ProtectStateType *StatePtr,
                                     uint8 *DataPtr, uint16 Length);
/* TRACE[SWS_E2E_00573] */
extern Std_ReturnType E2E_P11CheckInit(E2E_P11CheckStateType *StatePtr);
/* TRACE[SWS_E2E_00572] */
extern Std_ReturnType E2E_P11Check(const E2E_P11ConfigType *ConfigPtr, E2E_P11CheckStateType *StatePtr,
                                   const uint8 *DataPtr, uint16 Length);
/* TRACE[SWS_E2E_00574] */
extern E2E_PCheckStatusType E2E_P11MapStatusToSM(Std_ReturnType CheckReturn, E2E_P11CheckStatusType Status);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_P11_H */
#endif
