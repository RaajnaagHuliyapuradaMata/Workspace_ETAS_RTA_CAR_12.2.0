/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*---------------------------------------------------------------------------------------*/
/*- Include files                                                                       -*/
/*---------------------------------------------------------------------------------------*/

#include "BswM.h"
#include "BswM_Prv.h"

/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                      -*/
/*---------------------------------------------------------------------------------------*/
#define BSWM_START_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

BswM_Prv_IntrptQueueInfoType_tst BswM_Prv_IntrptQueueInfo_st = {0, 0, 0};

#define BSWM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

#define BSWM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "BswM_MemMap.h"

static BswM_Prv_IntrptQueueType_tst BswM_Prv_IntrptQueue_ast[BSWM_RB_INTRPTQUEUE_MAXSIZE];

#define BSWM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "BswM_MemMap.h"

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"


/************************************************************************************************
*Function name  : BswM_Prv_IntrptQueueIn (BswM_Cfg_MRPType_ten dataMRPType_en, uint16 idChannel_u16,
                            uint16 idxMRPChnl_u16, uint16 dataReqMode_u16)
*Description  : This function inserts mode request information into the interrupt queue.
*Parameters(in) :
*   dataMRPType_en- The type or source of the mode request (CamSMIndication, ComMIndication, etc)
*   idChannel_u16 - The ChannelId of the respective mode requester.
*   idxMRPChnl_u16- The index (in the PBstruct and RAM buffer) occupied by this channel.
*   dataReqMode_u16 - The mode currently requested.
*Parameters(out): None
*Return value   : Std_ReturnType
*                 E_OK : The request has been successfully queued.
*                 E_NOT_OK : The queue is already full and new requests cannot be queued.
************************************************************************************************/

void BswM_Prv_IntrptQueueIn
(
    BswM_Cfg_MRPType_ten dataMRPType_en,
    uint16 idChannel_u16,
    uint16 idxMRPChnl_u16,
    uint16 dataReqMode_u16
)
{
    /* Variable declaration*/
    uint8 tailIndex_u8= 0;

    if(BSWM_RB_INTRPTQUEUE_MAXSIZE == BswM_Prv_IntrptQueueInfo_st.dataSize_u8)
    {

        /* Do nothing */
    }
    else
    {
        tailIndex_u8 = BswM_Prv_IntrptQueueInfo_st.idxTail_u8;

        /* Queue in the passed information*/
        BswM_Prv_IntrptQueue_ast[tailIndex_u8].dataMRPType_en = dataMRPType_en;
        BswM_Prv_IntrptQueue_ast[tailIndex_u8].idChannel_u16 = idChannel_u16;
        BswM_Prv_IntrptQueue_ast[tailIndex_u8].idxMRPChnl_u16 = idxMRPChnl_u16;
        BswM_Prv_IntrptQueue_ast[tailIndex_u8].dataReqMode_u16 = dataReqMode_u16;

        /* Increment the tail index and the size of the queue*/
        BswM_Prv_IntrptQueueInfo_st.idxTail_u8++;
        BswM_Prv_IntrptQueueInfo_st.dataSize_u8++;
        /*indicate that request was delayed*/
        BswM_Prv_isReqstDelayed_b = TRUE;
        /* Has the final index of the queue been reached?*/
        if (BSWM_RB_INTRPTQUEUE_MAXSIZE == BswM_Prv_IntrptQueueInfo_st.idxTail_u8)
        {
            /*Reset the tail index to indicate the first index of the queue*/
            BswM_Prv_IntrptQueueInfo_st.idxTail_u8 = 0;
        }
    }
    return;
}

/************************************************************************************************
*Function name  : BswM_Prv_IntrptQueueOut (BswM_Cfg_MRPType_ten* dataMRPType_pen, uint16 * idChannel_pu16,
*                    uint16 * idxMRPChnl_pu16, uint16* dataReqMode_pu16)
*Description    : This function retrieves information about the queued mode request from the interrupt queue.
*Parameters(in) : None
*Parameters(out):
*   dataMRPType_pen - Pointer to the type or source of the mode request (CamSMIndication, ComMIndication, etc)
*   idChannel_pu16 -  Pointer to the ChannelId of the respective mode requester.
*   idxMRPChnl_pu16 - Pointer to the index (in the PBstruct and RAM buffer) occupied by this channel.
*   dataReqMode_pu16 - Pointer to the mode currently requested
*Return value   : Std_ReturnType
*                 E_OK : The queue is not empty and a value is returned.
*                 E_NOT_OK : The queue is empty, no values are returned.
************************************************************************************************/

Std_ReturnType BswM_Prv_IntrptQueueOut
(
    BswM_Cfg_MRPType_ten * dataMRPType_pen,
    uint16 * idChannel_pu16,
    uint16 * idxMRPChnl_pu16,
    uint16 * dataReqMode_pu16
)
{
    /* Variable declaration*/
    Std_ReturnType retVal = E_OK;
    uint8 headIndex_u8= 0;

    if(0 == BswM_Prv_IntrptQueueInfo_st.dataSize_u8)
    {
        retVal = E_NOT_OK;
    }
    else
    {
        headIndex_u8 = BswM_Prv_IntrptQueueInfo_st.idxHead_u8;

        /* Dequeue the information stored at head index*/
        *dataMRPType_pen = BswM_Prv_IntrptQueue_ast[headIndex_u8].dataMRPType_en;
        *idChannel_pu16 = BswM_Prv_IntrptQueue_ast[headIndex_u8].idChannel_u16;
        *idxMRPChnl_pu16 = BswM_Prv_IntrptQueue_ast[headIndex_u8].idxMRPChnl_u16;
        *dataReqMode_pu16 = BswM_Prv_IntrptQueue_ast[headIndex_u8].dataReqMode_u16;

        /*Clear the values at head index*/
        BswM_Prv_IntrptQueue_ast[headIndex_u8].dataMRPType_en = BSWM_MRP_DEFAULT;
        BswM_Prv_IntrptQueue_ast[headIndex_u8].idChannel_u16 = 0xFF;
        BswM_Prv_IntrptQueue_ast[headIndex_u8].idxMRPChnl_u16 = 0xFF;
        BswM_Prv_IntrptQueue_ast[headIndex_u8].dataReqMode_u16 = 0xFF;

        /* Increment the head index and decrement the size of the queue*/
        BswM_Prv_IntrptQueueInfo_st.idxHead_u8++;
        BswM_Prv_IntrptQueueInfo_st.dataSize_u8--;

        /*Is the queue empty?*/
        if(0 == BswM_Prv_IntrptQueueInfo_st.dataSize_u8)
        {
            /* Reset head and tail indexes of the queue*/
            BswM_Prv_IntrptQueueInfo_st.idxHead_u8 = 0;
            BswM_Prv_IntrptQueueInfo_st.idxTail_u8 = 0;
        }
        else
        {
            /* Has the final index of the queue been reached?*/
            if (BSWM_RB_INTRPTQUEUE_MAXSIZE == BswM_Prv_IntrptQueueInfo_st.idxHead_u8)
            {
                /*Reset the head index to indicate the first index of the queue*/
                BswM_Prv_IntrptQueueInfo_st.idxHead_u8 = 0;
            }

        }
    }
    return retVal;
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/

