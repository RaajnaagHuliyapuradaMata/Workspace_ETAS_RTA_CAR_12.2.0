// %= file_banner($0) %>
#ifndef CAN_HWPROVIDERTYPES_H_
#define CAN_HWPROVIDERTYPES_H_

/*******************************************************************************
 *                             Include Files.
 *******************************************************************************/
#include <limits.h> /*lint !e7 Unable to open include file 'limits.h' */

/*******************************************************************************
 *                         Macros
 ******************************************************************************/
/* Extended bit MSB, FD bit MSB-1 */
/* [MISRA 2012 Dir 4.9] */ /*lint -save -estring(9026, *) */
#define CAN_EXTENDED_ID_BIT                (( 1U << ((CHAR_BIT * (uint8)sizeof (Can_IdType))-1U)))
#define CAN_CANONICAL_ID_IS_STANDARD(_id)  ((((Can_IdType) (_id)) & CAN_EXTENDED_ID_BIT) == 0)
#define CAN_CANONICAL_ID_IS_EXTENDED(_id)  ((((Can_IdType) (_id)) & CAN_EXTENDED_ID_BIT) != 0)
/*lint -restore */
#define CAN_MAX_FRAME_LENGTH               (64U)
#define CAN_ID_FD_BIT                      (( 1U << ((CHAR_BIT * (uint8)sizeof (Can_IdType))-2U)))
#define CAN_ID_FD_BIT_SET(_id)             ((((Can_IdType) (_id)) & CAN_ID_FD_BIT) != 0) /*lint !e9026 */
#define CAN_PDUL_MASK                      (~(CAN_EXTENDED_ID_BIT | CAN_ID_FD_BIT))

#define CAN_OBJECT_PL_12 (12U)
#define CAN_OBJECT_PL_16 (16U)
#define CAN_OBJECT_PL_20 (20U)
#define CAN_OBJECT_PL_24 (24U)
#define CAN_OBJECT_PL_32 (32U)
#define CAN_OBJECT_PL_48 (48U)
#define CAN_OBJECT_PL_64 (64U)
#define CAN_OBJECT_PL_8  (8U)

/*******************************************************************************
 *                         Types
 ******************************************************************************/

typedef enum {
  CAN_HWP_RXTYPE_BASIC,
  CAN_HWP_RXTYPE_FULL
} Can_Hwp_CanRxType;

typedef enum {
  CAN_HWP_IDTYPE_STANDARD,
  CAN_HWP_IDTYPE_EXTENDED,
  CAN_HWP_IDTYPE_MIXED
} Can_Hwp_CanIdType;

typedef enum {
  CAN_HWP_EVENT_RUNNING,
  CAN_HWP_EVENT_STOPPED,
  CAN_HWP_EVENT_SLEEPING,
  CAN_HWP_EVENT_TX_CANCELLED,
  CAN_HWP_EVENT_TX_COMPLETED,
  CAN_HWP_EVENT_RX,
  CAN_HWP_EVENT_DATALOST,
  CAN_HWP_EVENT_WAKEUP,
  CAN_HWP_EVENT_BUSOFF,
  CAN_HWP_ERROR_NO_ERROR,
  CAN_HWP_ERROR_BIT_MONITORING1,
  CAN_HWP_ERROR_BIT_MONITORING0,
  CAN_HWP_ERROR_BIT,
  CAN_HWP_ERROR_CHECK_ACK_FAILED,
  CAN_HWP_ERROR_ACK_DELIMITER,
  CAN_HWP_ERROR_ARBITRATION_LOST,
  CAN_HWP_ERROR_OVERLOAD,
  CAN_HWP_ERROR_CHECK_FORM_FAILED,
  CAN_HWP_ERROR_CHECK_STUFFING_FAILED,
  CAN_HWP_ERROR_CHECK_CRC_FAILED,
  CAN_HWP_ERROR_BUS_LOCK
} Can_Hwp_Event;

// ---------------------------------------------------------------------------
typedef struct {
  uint32 code; // [$CAN 1201] [$CAN21 9469]
  uint32 mask; // [$CAN 1202] [$CAN21 9470]
} Can_Hwp_HwFilter; // [$CAN 1200] [$CAN21 9468]

// ---------------------------------------------------------------------------
/* Description of a receive hardware object. */
typedef struct {
  Can_HwHandleType  handle; /* Handle of the receive hardware object. */
  Can_Hwp_CanRxType rxType; /* Receive type - FULL (i.e. exact ID match) or BASIC (filtered ID match). */
  Can_Hwp_CanIdType idType; /* ID type to receive - standard, extended or mixed. */
  uint32 numHwFilters;
  const Can_Hwp_HwFilter * hwFilters;
  uint8 objectPayloadLength; /* $CAN21 9495 */
} Can_Hwp_RxHwObject;

// ---------------------------------------------------------------------------
/* Description of a transmit hardware object. */
typedef struct {
  Can_HwHandleType  handle;        /* Handle of the transmit hardware object. */
  uint8 fdPaddingValue; /* [$CAN 1203] [$CAN21 9485] Specifies the value which is used to pad unspecified data in CAN FD frames > 8 bytes for transmission. */
  uint8 objectPayloadLength; /* $CAN21 9495 */
} Can_Hwp_TxHwObject;

// ---------------------------------------------------------------------------
typedef struct {
  boolean                   txCancelLowerPriorityEnabled;  /* true if cancellation of lower priority transmit */
                                                           /* frames is enabled. */
  boolean                   txCancelEqualPriorityEnabled;  /* true if cancellation of equal priority transmit */
                                                           /* frames is enabled. */
  const Can_Hwp_RxHwObject* rxHwObjects;
  uint32                    numRxHwObjects;
  const Can_Hwp_TxHwObject* txHwObjects;
  uint32                    numTxHwObjects;
} Can_Hwp_ChannelConfig;

// ---------------------------------------------------------------------------
  /* Information used to configure the Fd baudrate of a channel. */
typedef struct {
  uint16 baudrate; /* FD Baudrate in Kbps. */
  uint8  propagationDelay; /* Propagation delay in time quanta. */
  uint8  phaseSegment1;     /* Phase segment 1 in time quanta. */
  uint8  phaseSegment2;     /* Phase segment 2 in time quanta. */
  uint8  syncJumpWidth; /* Synchronization jump width for the controller in time quanta. */
  uint16 trcvDelayCompensationOffset; /* Specifies the Transceiver Delay Compensation Offset in ns */
    uint8 sspOffset; /* Specifies the Transmitter Delay Compensation Offset in minimum time quanta */
    boolean txBitRateSwitch; /* Specifies if the bit rate switching shall be used for transmissions. */
} Can_HwpFdbaudrateConfig;

// ---------------------------------------------------------------------------
/* Information used to configure the baudrate of a channel. */
typedef struct {
  uint16 baudrate;           /* Baudrate in Kbps. */
  uint8  propagationDelay;   /* Propagation delay in time quanta. */
  uint8  phaseSegment1;      /* Phase segment 1 in time quanta. */
  uint8  phaseSegment2;      /* Phase segment 2 in time quanta. */
  uint8  syncJumpWidth;      /* Synchronization jump width for the controller in time quanta. */
  Can_HwpFdbaudrateConfig* fdBaudrateConfig;
} Can_Hwp_BaudrateConfig;

// ---------------------------------------------------------------------------
/* Description of a CAN frame. */
typedef struct {
  boolean    injected;                    /* true if the frame been injected by an action. */
  boolean    txConfirm;                   /* true if the CAN driver call CanIf_TxConfirmation or */
                                          /* CanIf_CancelTxConfirmation for this frame? */
  PduIdType  swPduHandle;                 /* CanIf's transmit identifier. */
  Can_IdType id;                          /* The frame identifier in canonical form. */
  uint8      length;                      /* The number of bytes of data. */
  uint8      data[CAN_MAX_FRAME_LENGTH];  /* The data. */
} Can_Hwp_Frame;

/* Upcall function used a channel object to raise events in the CAN driver */
typedef void (*Can_Hwp_UpCall)(uint32 context, Can_Hwp_Event event, Can_HwHandleType handle, Can_Hwp_Frame* frame);


#endif /* CAN_HWPROVIDERTYPES_H_ */

