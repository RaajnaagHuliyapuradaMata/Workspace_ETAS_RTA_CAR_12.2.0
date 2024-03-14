

#ifndef E2E_P06_H
#define E2E_P06_H

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

/* TRACE[SWS_E2E_00440] , TRACE[SWS_E2E_00445]  */
typedef enum
{
    E2E_P06STATUS_OK            = 0x00U,
    E2E_P06STATUS_NONEWDATA     = 0x01U,
    E2E_P06STATUS_ERROR         = 0x07U,
    E2E_P06STATUS_REPEATED      = 0x08U,
    E2E_P06STATUS_OKSOMELOST    = 0x20U,
    E2E_P06STATUS_WRONGSEQUENCE = 0x40U
} E2E_P06CheckStatusType;

/* TRACE[SWS_E2E_00437] , TRACE[SWS_E2E_00441] */
typedef struct
{
    uint16 Offset;                  /* offset in bits of the header */
    uint16 MinDataLength;           /* minimum length of data in bits */
    uint16 MaxDataLength;           /* maximum length of data in bits */
    uint16 DataID;                  /* system unique identifier */
    uint8  MaxDeltaCounter;         /* max allowed gap between 2 consecutive messages */
} E2E_P06ConfigType;

/* TRACE[SWS_E2E_00438] , TRACE[SWS_E2E_00443] */
typedef struct
{
    uint8 Counter;
} E2E_P06ProtectStateType;

/* TRACE[SWS_E2E_00439] , TRACE[SWS_E2E_00444] */
typedef struct
{
    E2E_P06CheckStatusType  Status;
    uint8                   Counter;
} E2E_P06CheckStateType;

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

extern Std_ReturnType E2E_P06ProtectInit(E2E_P06ProtectStateType *StatePtr);
extern Std_ReturnType E2E_P06Protect(const E2E_P06ConfigType *ConfigPtr, E2E_P06ProtectStateType *StatePtr,
                                     uint8 *DataPtr, uint16 Length);
extern Std_ReturnType E2E_P06CheckInit(E2E_P06CheckStateType *StatePtr);
extern Std_ReturnType E2E_P06Check(const E2E_P06ConfigType *ConfigPtr, E2E_P06CheckStateType *StatePtr,
                                   const uint8 *DataPtr, uint16 Length);
extern E2E_PCheckStatusType E2E_P06MapStatusToSM(Std_ReturnType CheckReturn, E2E_P06CheckStatusType Status);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_P06_H */
#endif
