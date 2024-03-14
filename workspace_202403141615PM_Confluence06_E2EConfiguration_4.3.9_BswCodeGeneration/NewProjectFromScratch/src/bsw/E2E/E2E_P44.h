

#ifndef E2E_P44_H
#define E2E_P44_H

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
/* TRACE[SWS_E2E_00337] */
typedef enum
{
    E2E_P44STATUS_OK            = 0x00U,
    E2E_P44STATUS_NONEWDATA     = 0x01U,
    E2E_P44STATUS_ERROR         = 0x07U,
    E2E_P44STATUS_REPEATED      = 0x08U,
    E2E_P44STATUS_OKSOMELOST    = 0x20U,
    E2E_P44STATUS_WRONGSEQUENCE = 0x40U
} E2E_P44CheckStatusType;

/* TRACE[SWS_E2E_00334] */
typedef struct
{
    uint32 DataID;                  /* system unique identifier */
    uint32 Offset;                  /* offset in bits of the header */
    uint32 MinDataLength;           /* minimum length of data in bits */
    uint32 MaxDataLength;           /* maximum length of data in bits */
    uint16 MaxDeltaCounter;         /* max allowed gap between 2 consecutive messages */
} E2E_P44ConfigType;

/* TRACE[SWS_E2E_00335] */
typedef struct
{
    uint16 Counter;
} E2E_P44ProtectStateType;

/* TRACE[SWS_E2E_00336] */
typedef struct
{
    E2E_P44CheckStatusType  Status;
    uint16                  Counter;
} E2E_P44CheckStateType;

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

/* TRACE[SWS_E2E_00373] */
extern Std_ReturnType E2E_P44ProtectInit(E2E_P44ProtectStateType *StatePtr);
/* TRACE[SWS_E2E_00338] */
extern Std_ReturnType E2E_P44Protect(const E2E_P44ConfigType *ConfigPtr, E2E_P44ProtectStateType *StatePtr,
                                     uint8 *DataPtr, uint16 Length);
/* TRACE[SWS_E2E_00350] */
extern Std_ReturnType E2E_P44CheckInit(E2E_P44CheckStateType *StatePtr);
/* TRACE[SWS_E2E_00339] */
extern Std_ReturnType E2E_P44Check(const E2E_P44ConfigType *ConfigPtr, E2E_P44CheckStateType *StatePtr,
                                   const uint8 *DataPtr, uint16 Length);
extern E2E_PCheckStatusType E2E_P44MapStatusToSM(Std_ReturnType CheckReturn, E2E_P44CheckStatusType Status);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_P44_H */
#endif
