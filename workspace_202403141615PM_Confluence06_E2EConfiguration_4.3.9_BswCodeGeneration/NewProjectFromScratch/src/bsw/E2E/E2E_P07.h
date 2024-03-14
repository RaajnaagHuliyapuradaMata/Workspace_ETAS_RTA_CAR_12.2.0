

#ifndef E2E_P07_H
#define E2E_P07_H

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

/* TRACE[SWS_E2E_00543] */
typedef enum
{
    E2E_P07STATUS_OK            = 0x00U,
    E2E_P07STATUS_NONEWDATA     = 0x01U,
    E2E_P07STATUS_ERROR         = 0x02U,
    E2E_P07STATUS_REPEATED      = 0x08U,
    E2E_P07STATUS_OKSOMELOST    = 0x20U,
    E2E_P07STATUS_WRONGSEQUENCE = 0x40U
} E2E_P07CheckStatusType;

/* TRACE[SWS_E2E_00544] */
typedef struct
{
    uint32 DataID;                  /* system unique identifier */
    uint32 Offset;                  /* offset in bits of the header */
    uint32 MinDataLength;           /* minimum length of data in bits */
    uint32 MaxDataLength;           /* maximum length of data in bits */
    uint32 MaxDeltaCounter;         /* max allowed gap between 2 consecutive messages */
} E2E_P07ConfigType;

/* TRACE[SWS_E2E_00545]  */
typedef struct
{
    uint32 Counter;
} E2E_P07ProtectStateType;

/* TRACE[SWS_E2E_00542]  */
typedef struct
{
    E2E_P07CheckStatusType  Status;
    uint32                  Counter;
} E2E_P07CheckStateType;

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

/* TRACE[SWS_E2E_00547]*/
extern Std_ReturnType E2E_P07ProtectInit(E2E_P07ProtectStateType *StatePtr);
/* TRACE[SWS_E2E_00546] */
extern Std_ReturnType E2E_P07Protect(const E2E_P07ConfigType *ConfigPtr, E2E_P07ProtectStateType *StatePtr,
                                     uint8 *DataPtr, uint32 Length);
/* TRACE[SWS_E2E_00549] */
extern Std_ReturnType E2E_P07CheckInit(E2E_P07CheckStateType *StatePtr);
/* TRACE[SWS_E2E_00548] */
extern Std_ReturnType E2E_P07Check(const E2E_P07ConfigType *ConfigPtr, E2E_P07CheckStateType *StatePtr,
                                   const uint8 *DataPtr, uint32 Length);
/* TRACE[SWS_E2E_00550] */
extern E2E_PCheckStatusType E2E_P07MapStatusToSM(Std_ReturnType CheckReturn, E2E_P07CheckStatusType Status);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_P07_H */
#endif
