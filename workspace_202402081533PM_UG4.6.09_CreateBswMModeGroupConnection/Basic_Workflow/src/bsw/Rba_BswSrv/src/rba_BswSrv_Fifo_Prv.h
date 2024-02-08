/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef RBA_BSWSRV_FIFO_PRV_H
#define RBA_BSWSRV_FIFO_PRV_H


/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "Platform_Types.h"
#include "rba_BswSrv_Fifo.h"


/*
 ***************************************************************************************************
 * Macros
 ***************************************************************************************************
 */

#define RBA_BSWSRV_FIFO_PUSH_TO_POP_MIN_DST 1u


/*
 ***************************************************************************************************
 * Types
 ***************************************************************************************************
 */

// FIFO handles variable sized portions of data as passed by users (push/pop)
// Inside the internal FIFO buffer those data portions will be prefixed with
// a "header". Header + data portion is called a 'FIFO message'
/* MR12 RULE 18.7 VIOLATION: The warning is not critical and non-removable since Structure is used to access variable sized objects with "overlay pointers"
   This is allowed with C99 (which we use).
 */
struct rba_BswSrv_FifoMsg
{
    struct rba_BswSrv_FifoMsgInfo info;
    uint8 data[];
};


typedef uint32 rba_BswSrv_FifoStateAtomic_t;

// Dynamic state of a FIFO - must be placed in RAM
union rba_BswSrv_FifoState
{
    rba_BswSrv_FifoStateAtomic_t atomic;
    struct {
        rba_BswSrv_FifoMsgLen_t val;
        rba_BswSrv_FifoMsgLen_t guard;
    } pos;
};

// Static configuration of a FIFO - must be placed in ROM
struct rba_BswSrv_FifoCfg
{
    const uint32 size;  // size of the data buffer for FIFO messages
    uint8 * const buf_p; // Pointer the data buffer for FIFO messages
};

// A complete description of a FIFO - must be placed in ROM
struct rba_BswSrv_Fifo
{
    union rba_BswSrv_FifoState * const statePush_p;
    union rba_BswSrv_FifoState * const statePop_p;
    const struct rba_BswSrv_FifoCfg cfg;
};


/*
 ***************************************************************************************************
 * Declarations
 ***************************************************************************************************
 */
#define RBA_BSWSRV_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "rba_BswSrv_MemMap.h"
extern const struct rba_BswSrv_Fifo rba_BswSrv_FifoList[];
#define RBA_BSWSRV_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "rba_BswSrv_MemMap.h"

#endif // RBA_BSWSRV_FIFO_PRV_H
