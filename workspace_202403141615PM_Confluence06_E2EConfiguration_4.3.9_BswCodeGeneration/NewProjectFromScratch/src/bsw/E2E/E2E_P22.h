

#ifndef E2E_P22_H
#define E2E_P22_H

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
    E2E_P22STATUS_OK            = 0x00U,
    E2E_P22STATUS_NONEWDATA     = 0x01U,
    E2E_P22STATUS_ERROR         = 0x07U,
    E2E_P22STATUS_REPEATED      = 0x08U,
    E2E_P22STATUS_OKSOMELOST    = 0x20U,
    E2E_P22STATUS_WRONGSEQUENCE = 0x40U
} E2E_P22CheckStatusType;

/* TRACE[SWS_E2E_00571] */
typedef struct
{
    uint16 DataLength;              /* length of data in bits */
    uint8  DataIDList[16];          /* list of sender Identifier */
    uint8  MaxDeltaCounter;         /* maximum allowed gap, init value */
    uint16 Offset ;                 /* offset of the E2E header in the Data[] array in bits. */
                                    /* Currently not used it should be always set to 0. */
} E2E_P22ConfigType;

/* TRACE[SWS_E2E_00570] */
typedef struct
{
    uint8 Counter;
} E2E_P22ProtectStateType;

/* TRACE[SWS_E2E_00568] */
typedef struct
{
    uint8                   Counter;
    E2E_P22CheckStatusType  Status;
} E2E_P22CheckStateType;

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

/* TRACE[SWS_E2E_00581] */
extern Std_ReturnType E2E_P22ProtectInit(E2E_P22ProtectStateType *StatePtr);
/* TRACE[SWS_E2E_00580] */
extern Std_ReturnType E2E_P22Protect(const E2E_P22ConfigType *ConfigPtr, E2E_P22ProtectStateType *StatePtr,
                                     uint8 *DataPtr, uint16 Length);
/* TRACE[SWS_E2E_00578] */
extern Std_ReturnType E2E_P22CheckInit(E2E_P22CheckStateType *StatePtr);
/* TRACE[SWS_E2E_00577] */
extern Std_ReturnType E2E_P22Check(const E2E_P22ConfigType *ConfigPtr, E2E_P22CheckStateType *StatePtr,
                                   const uint8 *DataPtr, uint16 Length);
/* TRACE[SWS_E2E_00579] */
extern E2E_PCheckStatusType E2E_P22MapStatusToSM(Std_ReturnType CheckReturn, E2E_P22CheckStatusType Status);

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"

/* E2E_P22_H */
#endif
