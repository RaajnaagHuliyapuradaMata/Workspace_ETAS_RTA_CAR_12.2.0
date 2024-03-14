

#ifndef E2E_SM_H
#define E2E_SM_H

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

typedef enum
{
    E2E_P_OK            = 0x00U,
    E2E_P_REPEATED      = 0x01U,
    E2E_P_WRONGSEQUENCE = 0x02U,
    E2E_P_ERROR         = 0x03U,
    E2E_P_NOTAVAILABLE  = 0x04U,
    E2E_P_NONEWDATA     = 0x05U
} E2E_PCheckStatusType;

/* TRACE[SWS_E2E_00342] */
typedef struct
{
    uint8    WindowSize;           /* Size of the monitoring window for the state machine */
    uint8    MinOkStateInit;
    uint8    MaxErrorStateInit;
    uint8    MinOkStateValid;
    uint8    MaxErrorStateValid;
    uint8    MinOkStateInvalid;
    uint8    MaxErrorStateInvalid;
} E2E_SMConfigType;

/* TRACE[SWS_E2E_00344] */
typedef enum
{
    E2E_SM_VALID    = 0x00U,
    E2E_SM_DEINIT   = 0x01U,
    E2E_SM_NODATA   = 0x02U,
    E2E_SM_INIT     = 0x03U,
    E2E_SM_INVALID  = 0x04U
} E2E_SMStateType;

/* TRACE[SWS_E2E_00343] */
typedef struct
{
    uint8*          ProfileStatusWindow;
    uint8           WindowTopIndex;
    uint8           OkCount;
    uint8           ErrorCount;
    E2E_SMStateType SMState;
} E2E_SMCheckStateType;

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

/* TRACE[SWS_E2E_00340] */
extern Std_ReturnType E2E_SMCheck(E2E_PCheckStatusType ProfileStatus, const E2E_SMConfigType * const ConfigPtr,

/* TRACE[SWS_E2E_00353] */                                  E2E_SMCheckStateType *StatePtr);
extern Std_ReturnType E2E_SMCheckInit(E2E_SMCheckStateType *StatePtr, const E2E_SMConfigType * const ConfigPtr);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_SM_H */
#endif
