/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Platform_Types.h"
#include "Compiler.h"
#include "rba_BswSrv_Fifo_Prv.h"
#include "rba_BswSrv_Fifo.h"


/*
 ***************************************************************************************************
 * Helper functions
 ***************************************************************************************************
 */






LOCAL_INLINE
boolean isValidFifoId(rba_BswSrv_FifoId_t fifoId)
{
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    boolean valid = (boolean)(fifoId < RBA_BSWSRV_FIFO_ID_NUMBEROF);
	return valid;
#else
	(void) fifoId;
	return FALSE;
#endif
}

LOCAL_INLINE
const struct rba_BswSrv_Fifo * selectFifo(rba_BswSrv_FifoId_t fifoId)
{
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    return isValidFifoId(fifoId) ? &rba_BswSrv_FifoList[fifoId] : NULL_PTR;
#else
    (void) fifoId;
    return NULL_PTR;
#endif
}

/* MR12 DIR 4.1, RULE 1.3, 11.4 VIOLATION: The warning is not critical and non-removable since casting between integers and pointers is required to interpret the address.
   Function below is needed because C99 _Alignof() is not available in our enviroment
   There is no real NULL-pointer dereference. */
LOCAL_INLINE
uint32 gapToNextMsg(uint32 len)
{
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    struct rba_BswSrv_FifoInfo_temp{ uint8 b; struct rba_BswSrv_FifoMsgInfo msg; };
    struct rba_BswSrv_FifoInfo_temp *zero_p,temp_st;
    zero_p=&temp_st;
    uint32 requiredMsgAlign = (uint32)&zero_p->msg-(uint32)&zero_p->b;
    uint32 misalignedTailBytes = len % requiredMsgAlign;

    return (misalignedTailBytes > 0u) ? (requiredMsgAlign - misalignedTailBytes) : 0u;
#else
    (void) len;
    return 0u;
#endif
}

LOCAL_INLINE
struct rba_BswSrv_FifoMsg * getMsg(const union rba_BswSrv_FifoState *state_p,
                                   const uint8 *circStart_p)
{
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    union rba_BswSrv_FifoState st ;
    uint32 val;
    uint32 invalid;
    struct rba_BswSrv_FifoMsg *msg;

    st.atomic =  state_p->atomic;
    val = st.pos.val;
    invalid = (rba_BswSrv_FifoMsgLen_t)~0u - val - st.pos.guard;

    // FIFO position and buffer are guaranteed to be correctly aligned for messages!
    if (invalid != 0u) {
        msg = NULL_PTR;
    } else {
        /* MR12 RULE 11.4 VIOLATION: The Warning is uncritical and non-removable since casting between integers and pointers is required to interpret the address */
        uint32 circStart = (uint32) circStart_p; // get rid of const
        /* MR12 RULE 11.5, 11.6 VIOLATION: The Warning is uncritical and non-removable since conversion from a pointer to object into a pointer to void is required to access buffer */
        msg = (struct rba_BswSrv_FifoMsg *)(void*)(circStart + val);
    }

    return msg;
#else
    (void ) *state_p;
    (void ) *circStart_p;
    return NULL_PTR;
#endif
}

LOCAL_INLINE
void setMsgPos(union rba_BswSrv_FifoState *state_p, uint32 pos)
{
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    union rba_BswSrv_FifoState st;
    st.pos.val= (rba_BswSrv_FifoMsgLen_t)pos;
    st.pos.guard =  (rba_BswSrv_FifoMsgLen_t)~pos;
    state_p->atomic = st.atomic;
#else
    (void ) *state_p;
    (void) pos;
#endif
}

LOCAL_INLINE
uint32 nextMsgPos(const struct rba_BswSrv_FifoMsg *msgRef_p,
                             const uint8 *circStart_p, const uint8 *circWrap_p)
{
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    const uint8 *circRef_p = (const uint8*)msgRef_p;
    /* MR12 Rule 18.3 VIOLATION:  The Warning is uncritical and non-removable since comparisons between pointers is required to get the address of the next message */
    return (msgRef_p->data > circWrap_p) ? 0u : (uint32)(circRef_p - circStart_p);
#else
    (void ) *msgRef_p;
    (void ) *circStart_p;
    (void ) *circWrap_p;
    return 0u;
#endif
}

LOCAL_INLINE
void setMsgInfo(struct rba_BswSrv_FifoMsgInfo *dest, const struct rba_BswSrv_FifoMsgInfo *src)
{
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    dest->len = src->len;
    dest->tag = src->tag;
#else
    (void )*dest;
    (void )*src;

#endif
}


/**
 ***************************************************************************************************
 * Peek into a FIFO for the next (oldest) unread message.
 *
 * The purpose of this function is to check if there are unread messages in a FIFO and (if present)
 * what properties the oldest message has.
 * The state of the FIFO and its messages is not changed by this operation.
 *
 * \attention This function may only be used by the data-sink associated with a FIFO.
 *
 * \param[in]   fifoId     Identifier to select a specific FIFO for peek operation.
 * \param[out]  msgInfo_p  Destination to store the properties of the next unread FIFO message.
 *
 * \return  A status code that describes the result of the function:
 * \retval  RBA_BSWSRV_FIFO_ERROR_NONE    There is currently at least one unread message in the
 *                                        selected FIFO. Properties of the next (oldest) message are
 *                                        stored in msgInfo_p.
 * \retval  RBA_BSWSRV_FIFO_ERROR_NODATA  There are currently no unread messages in the FIFO.
 * \retval  RBA_BSWSRV_FIFO_ERROR_BADF    Invalid FIFO identifier.
 * \retval  RBA_BSWSRV_FIFO_ERROR_NOTRDY  FIFO is corrupted/ not completely initialized.
 ***************************************************************************************************
 */
#define RBA_BSWSRV_START_SEC_CODE
#include "rba_BswSrv_MemMap.h"
rba_BswSrv_FifoError_t rba_BswSrv_FifoPeek(rba_BswSrv_FifoId_t fifoId,
                                           struct rba_BswSrv_FifoMsgInfo *msgInfo_p)
{
	rba_BswSrv_FifoError_t rc;
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    const struct rba_BswSrv_Fifo *fifo_p = selectFifo(fifoId);

	if (fifo_p == NULL_PTR) {
		rc = RBA_BSWSRV_FIFO_ERROR_BADF;
    } else {
        const struct rba_BswSrv_FifoMsg *push_p, *pop_p;

        { // workaround for 'const poisoning'
            uint8 *circStart_p = fifo_p->cfg.buf_p;
            push_p = getMsg(fifo_p->statePush_p, circStart_p);
            pop_p = getMsg(fifo_p->statePop_p, circStart_p);
        }

        if ((push_p == NULL_PTR) || (pop_p == NULL_PTR)) {
            rc = RBA_BSWSRV_FIFO_ERROR_NOTRDY;
        } else if (push_p == pop_p) {
            rc = RBA_BSWSRV_FIFO_ERROR_NODATA;
        } else {
            if (msgInfo_p != NULL_PTR) {
                setMsgInfo(msgInfo_p, &pop_p->info);
            }

            rc = RBA_BSWSRV_FIFO_ERROR_NONE;
        }
    }
#else
	(void) fifoId;
	(void ) *msgInfo_p;
    rc = RBA_BSWSRV_FIFO_ERROR_NOTCFG;
#endif
	return rc;

}


/**
 ***************************************************************************************************
 * Pop (read) the next (oldest) message from a FIFO.
 *
 * The purpose of this function is to pop the next (oldest) message (if there is any) from a FIFO.
 * 'Pop' means 'reading' this message with the side effect of removing it from a FIFO.
 *
 * \attention  This function may only be used by the data-sink associated with a FIFO.
 *
 * \param[in]   fifoId          Identifier to select a specific FIFO for pop operation.
 * \param[out]  msgInfo_p       Destination to store the properties of the next unread FIFO message.
 * \param[out]  msgDataBuf_pu8  Destination (buffer) to store the message read from FIFO.
 * \param[in]   sizeBuf         Size of the destination buffer.
 *
 * \return  A status code that describes the result of the function:
 * \retval  RBA_BSWSRV_FIFO_ERROR_NONE    Message was successfully read (and removed) from FIFO.
 * \retval  RBA_BSWSRV_FIFO_ERROR_NODATA  There are currently no unread messages in the FIFO.
 * \retval  RBA_BSWSRV_FIFO_ERROR_BADF    Invalid FIFO identifier.
 * \retval  RBA_BSWSRV_FIFO_ERROR_INVAL   Invalid function arguments.
 * \retval  RBA_BSWSRV_FIFO_ERROR_NOTRDY  FIFO is corrupted/ not completely initialized.
 * \retval  RBA_BSWSRV_FIFO_ERROR_NOBUFS  The message doesn't fit into the destination buffer.
 *                                        Evaluate msgInfo_p to get the size of the message!
 *
 * \remark The implementation heavily depends on a properly aligned and sized FIFO buffer.
 ***************************************************************************************************
 */
/* HIS METRIC LEVEL VIOLATION IN rba_BswSrv_FifoPop: FIFO (circular buffer) corner cases
   must be all checked in a way that performance does not suffer */
rba_BswSrv_FifoError_t rba_BswSrv_FifoPop(rba_BswSrv_FifoId_t fifoId,
                                          struct rba_BswSrv_FifoMsgInfo *msgInfo_p,
                                          void *msgDataBuf_p, uint32 sizeBuf)
{
    rba_BswSrv_FifoError_t rc;
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    const struct rba_BswSrv_Fifo *fifo_p = selectFifo(fifoId);

    if (fifo_p == NULL_PTR) {
        rc = RBA_BSWSRV_FIFO_ERROR_BADF;
    } else if ((msgInfo_p == NULL_PTR) || (msgDataBuf_p == NULL_PTR) || (sizeBuf == 0u)) {
		rc = RBA_BSWSRV_FIFO_ERROR_INVAL;
	} else {
        const uint8 *circStart_p;
        const struct rba_BswSrv_FifoMsg *push_p, *pop_p;

        { // workaround for 'const poisoning'
            uint8 *refStart_p = fifo_p->cfg.buf_p;
            push_p = getMsg(fifo_p->statePush_p, refStart_p);
            pop_p = getMsg(fifo_p->statePop_p, refStart_p);
            circStart_p = refStart_p;
        }

        if ((push_p == NULL_PTR) || (pop_p == NULL_PTR)) {
            rc = RBA_BSWSRV_FIFO_ERROR_NOTRDY;
        } else if (push_p == pop_p) {
            rc = RBA_BSWSRV_FIFO_ERROR_NODATA;
        } else {
            uint32 msgLen = pop_p->info.len;

            setMsgInfo(msgInfo_p, &pop_p->info);
            if (msgLen > sizeBuf) {
                rc = RBA_BSWSRV_FIFO_ERROR_NOBUFS;
            } else {
                /* MR12 Rule 11.5 VIOLATION: The Warning is uncritical and non-removable since conversion from a pointer to object into a pointer to void is required to interpret the address. */
                uint8 *msgDataBuf_pu8 = (uint8*)msgDataBuf_p;
                const uint8 *circWrap_p = circStart_p + fifo_p->cfg.size;
                const uint8 *circPos_p = pop_p->data;

                for (; msgLen > 0u; --msgLen) { // MISRA
                    /* MR12 Rule 18.3 VIOLATION: The Warning is uncritical and non-removable since comparisons between pointers is required to set the next address to be pointed*/
                    if (circPos_p >= circWrap_p) {
                        circPos_p = circStart_p;
                    }
                    *msgDataBuf_pu8 = *circPos_p;
                    ++msgDataBuf_pu8;
                    ++circPos_p;
                }
                /* MR12 RULE 11.4 VIOLATION: The Warning is uncritical and non-removable since casting between integers and pointers is required to interpret the address. */
                circPos_p += gapToNextMsg((uint32)circPos_p); // Align for msg


                /* MR12 RULE 11.5 VIOLATION: The Warning is uncritical and non-removable since conversion from a pointer to object into a pointer to void is required to interpret the address. */
                setMsgPos(fifo_p->statePop_p,
                          nextMsgPos((const struct rba_BswSrv_FifoMsg*)(const void*)circPos_p,
                                     circStart_p, circWrap_p));

                rc = RBA_BSWSRV_FIFO_ERROR_NONE;
            }
        }
    }

#else
    (void) fifoId;
    (void ) *msgInfo_p;
    (void ) *msgDataBuf_p;
    (void) sizeBuf;
    rc = RBA_BSWSRV_FIFO_ERROR_NOTCFG;
#endif
    return rc;
}

/**
 ***************************************************************************************************
 * Push (write) a message to the FIFO.
 *
 * The purpose of this function is to push (write) a message to the top of a FIFO.
 *
 * \attention  This function may only be used by the data-source associated with a FIFO.
 *
 * \param[in]   fifoId        Identifier to select a specific FIFO for push operation.
 * \param[in]   msgInfo_p     The properties of the message to be pushed.
 * \param[in]   msgData_pcu8  The message (data of the message) as specified by msgInfo_p.
 *
 * \return  A status code that describes the result of the function:
 * \retval  RBA_BSWSRV_FIFO_ERROR_NONE    Message was successfully written (pushed) to FIFO.
 * \retval  RBA_BSWSRV_FIFO_ERROR_BADF    Invalid FIFO identifier.
 * \retval  RBA_BSWSRV_FIFO_ERROR_INVAL   Invalid function arguments.
 * \retval  RBA_BSWSRV_FIFO_ERROR_NOTRDY  FIFO is corrupted/ not completely initialized.
 * \retval  RBA_BSWSRV_FIFO_ERROR_NOBUFS  The message currently doesn't fit into the FIFO.
 *
 * \remark The implementation heavily depends on a properly aligned and sized FIFO buffer.
 ***************************************************************************************************
 */

rba_BswSrv_FifoError_t rba_BswSrv_FifoPush(rba_BswSrv_FifoId_t fifoId,
                                           const struct rba_BswSrv_FifoMsgInfo *msgInfo_p,
                                           const void *msgData_p)
{
    rba_BswSrv_FifoError_t rc;
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    const struct rba_BswSrv_Fifo *fifo_p = selectFifo(fifoId);

    if (fifo_p == NULL_PTR) {
        rc = RBA_BSWSRV_FIFO_ERROR_BADF;
    } else if ((msgInfo_p == NULL_PTR) || (msgInfo_p->len == 0) || (msgData_p == NULL_PTR)) {
		rc = RBA_BSWSRV_FIFO_ERROR_INVAL;
    } else {
        uint8 *circStart_p = fifo_p->cfg.buf_p;
        struct rba_BswSrv_FifoMsg *push_p = getMsg(fifo_p->statePush_p, circStart_p);
        const struct rba_BswSrv_FifoMsg *pop_p = getMsg(fifo_p->statePop_p, circStart_p);

        if ((push_p == NULL_PTR) || (pop_p == NULL_PTR)) {
            rc = RBA_BSWSRV_FIFO_ERROR_NOTRDY;
        } else {
            uint32 msgLen = msgInfo_p->len;
            uint32 circSize = fifo_p->cfg.size;
            const uint8 *circWrap_p = circStart_p + circSize;
            uint32 msgTailPad = gapToNextMsg(msgLen);
            uint32 circRqrd = sizeof(*push_p) + msgLen + msgTailPad;
            uint32 circFree;


            if (pop_p > push_p) {
                circFree = (uint32)((const uint8*)pop_p - (uint8*)push_p);
            } else {
                uint32 circFreeContig = (uint32)(circWrap_p - (uint8*)push_p);

                circFree = (pop_p == push_p) ? circSize :
                    (circSize - (uint32)((uint8*)push_p - (const uint8*)pop_p));

                if (circRqrd < circFreeContig) {
                    uint32 circFreeAftMsg = circFreeContig - circRqrd;
                    if (circFreeAftMsg < sizeof(*push_p)) {
                        circFree -= circFreeAftMsg;
                    }
                }
            }
            if (circFree < (circRqrd + RBA_BSWSRV_FIFO_PUSH_TO_POP_MIN_DST)) {
                rc =  RBA_BSWSRV_FIFO_ERROR_NOBUFS;
            } else {
                /* MR12 Rule 11.5 VIOLATION: The Warning is uncritical and non-removable since conversion from a pointer to object into a pointer to void is required to interpret the address. */
                const uint8 *msgData_pcu8 = (const uint8*)msgData_p;
                uint8 *circPos_p = push_p->data;

                for (; msgLen > 0u; --msgLen) {
                    /* MR12 Rule 18.3 The Warning is uncritical and non-removable since comparisons between pointers is required to set the next address to be pointed */
                    if (circPos_p >= circWrap_p) {
                        circPos_p = circStart_p;
                    }
                    *circPos_p = *msgData_pcu8;
                    ++circPos_p;
                    ++msgData_pcu8;
                }
                circPos_p += msgTailPad; // Align for msg


                setMsgInfo(&push_p->info, msgInfo_p);



                /* MR12 RULE 11.5 VIOLATION: The Warning is uncritical and non-removable since conversion from a pointer to object into a pointer to void is required to interpret the address. */
                setMsgPos(fifo_p->statePush_p,
                          nextMsgPos((struct rba_BswSrv_FifoMsg*)(void*)circPos_p,
                                     circStart_p, circWrap_p));

                rc = RBA_BSWSRV_FIFO_ERROR_NONE;
            }
        }
    }

#else
    (void) fifoId;
    (void ) *msgInfo_p;
    (void ) *msgData_p;
    rc = RBA_BSWSRV_FIFO_ERROR_NOTCFG;
#endif
    return rc;
}


/**
 ***************************************************************************************************
 * FIFO initialization for the data-source associated with a FIFO.
 *
 * The purpose of this function is to initialize a FIFO. A FIFO is ready for use only after it has
 * been initialized by both the data-source and the data-sink.
 *
 * \attention  This function may only be used by the data-source connected to the FIFO.
 *
 * \param[in]  fifoId  The identifier of the FIFO to be initialized.
 *
 * \return  A status code that describes the result of the function:
 * \retval  RBA_BSWSRV_FIFO_ERROR_NONE  FIFO's data-source related parts successfully initialized.
 * \retval  RBA_BSWSRV_FIFO_ERROR_BADF  Invalid FIFO identifier.
 ***************************************************************************************************
 */

rba_BswSrv_FifoError_t rba_BswSrv_FifoInitPush(rba_BswSrv_FifoId_t fifoId)
{
	rba_BswSrv_FifoError_t rc;
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    const struct rba_BswSrv_Fifo *fifo_p = selectFifo(fifoId);

    if (fifo_p == NULL_PTR) {
        rc = RBA_BSWSRV_FIFO_ERROR_BADF;
    } else {
        setMsgPos(fifo_p->statePush_p, 0);
        rc = RBA_BSWSRV_FIFO_ERROR_NONE;
	}

#else
    (void) fifoId;
    rc = RBA_BSWSRV_FIFO_ERROR_NOTCFG;
#endif
    return rc;
}


/**
 ***************************************************************************************************
 * FIFO initialization for the data-sink associated with a FIFO.
 *
 * The purpose of this function is to initialize a FIFO. A FIFO is ready for use only after it has
 * been initialized by both the data-source and the data-sink.
 *
 * \attention  This function may only be used by the data-sink connected to the FIFO.
 *
 * \param[in]  fifoId  The identifier of the FIFO to be initialized.
 *
 * \return  A status code that describes the result of the function:
 * \retval  RBA_BSWSRV_FIFO_ERROR_NONE  FIFO's data-sink related parts successfully initialized.
 * \retval  RBA_BSWSRV_FIFO_ERROR_BADF  Invalid FIFO identifier.
 ***************************************************************************************************
 */

rba_BswSrv_FifoError_t rba_BswSrv_FifoInitPop(rba_BswSrv_FifoId_t fifoId)
{
    rba_BswSrv_FifoError_t rc;
#if RBA_BSWSRV_FIFO_ID_NUMBEROF > 0
    const struct rba_BswSrv_Fifo *fifo_p = selectFifo(fifoId);

    if (fifo_p == NULL_PTR) {
        rc = RBA_BSWSRV_FIFO_ERROR_BADF;
    } else {
        setMsgPos(fifo_p->statePop_p, 0);
        rc = RBA_BSWSRV_FIFO_ERROR_NONE;
    }

#else
    (void) fifoId;
    rc = RBA_BSWSRV_FIFO_ERROR_NOTCFG;
#endif
    return rc;
}

#define RBA_BSWSRV_STOP_SEC_CODE
#include "rba_BswSrv_MemMap.h"

