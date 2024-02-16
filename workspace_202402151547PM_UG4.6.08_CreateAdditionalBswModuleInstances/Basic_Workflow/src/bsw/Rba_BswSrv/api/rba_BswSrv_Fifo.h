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

#ifndef RBA_BSWSRV_FIFO_H
#define RBA_BSWSRV_FIFO_H


/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "rba_BswSrv_Fifo_Cfg.h"

/*
 ***************************************************************************************************
 * Macros
 ***************************************************************************************************
 */

// Return codes
#define RBA_BSWSRV_FIFO_ERROR_NONE         0  // Operation successfully finished
#define RBA_BSWSRV_FIFO_ERROR_BADF         1  // Bad FIFO identifier
#define RBA_BSWSRV_FIFO_ERROR_INVAL        2  // Invalid arguments
#define RBA_BSWSRV_FIFO_ERROR_NOTRDY       3  // FIFO not ready (corrupted or not initialized)
#define RBA_BSWSRV_FIFO_ERROR_NOBUFS       4  // No buffer space available
#define RBA_BSWSRV_FIFO_ERROR_NODATA       5  // Nothing to read
#define RBA_BSWSRV_FIFO_ERROR_NOTCFG       6  // Fifo not configured

/*
 ***************************************************************************************************
 * Types
 ***************************************************************************************************
 */

typedef uint16 rba_BswSrv_FifoMsgLen_t; // type for the length of a FIFO message
typedef uint16 rba_BswSrv_FifoMsgTag_t; // type for the FIFO message tag
typedef uint32 rba_BswSrv_FifoId_t;     // type for the FIFO identifier
typedef uint32 rba_BswSrv_FifoError_t;  // FIFO function return type

// Type to describe a FIFO message
struct rba_BswSrv_FifoMsgInfo
{
    rba_BswSrv_FifoMsgLen_t len;
    rba_BswSrv_FifoMsgTag_t tag;
};


/*
 ***************************************************************************************************
 * Declarations
 ***************************************************************************************************
 */
#define RBA_BSWSRV_START_SEC_CODE
#include "rba_BswSrv_MemMap.h"
extern rba_BswSrv_FifoError_t rba_BswSrv_FifoInitPush(rba_BswSrv_FifoId_t fifoId);
extern rba_BswSrv_FifoError_t rba_BswSrv_FifoInitPop(rba_BswSrv_FifoId_t fifoId);
extern rba_BswSrv_FifoError_t rba_BswSrv_FifoPeek(rba_BswSrv_FifoId_t fifoId,
                                                  struct rba_BswSrv_FifoMsgInfo *msgInfo_p);
extern rba_BswSrv_FifoError_t rba_BswSrv_FifoPop(rba_BswSrv_FifoId_t fifoId,
                                                 struct rba_BswSrv_FifoMsgInfo *msgInfo_p,
                                                 void *msgDataBuf_p, uint32 sizeBuf);
extern rba_BswSrv_FifoError_t rba_BswSrv_FifoPush(rba_BswSrv_FifoId_t fifoId,
                                                  const struct rba_BswSrv_FifoMsgInfo *msgInfo_p,
                                                  const void *msgData_p);

#define RBA_BSWSRV_STOP_SEC_CODE
#include "rba_BswSrv_MemMap.h"
#endif // #ifndef RBA_BSWSRV_FIFO_H
