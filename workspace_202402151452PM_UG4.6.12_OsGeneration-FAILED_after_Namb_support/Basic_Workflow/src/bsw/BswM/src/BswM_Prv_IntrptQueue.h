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
#ifndef BSWM_PRV_INTRPTQUEUE_H
#define BSWM_PRV_INTRPTQUEUE_H

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

typedef struct
{
    BswM_Cfg_MRPType_ten dataMRPType_en;
    uint16 idChannel_u16;
    uint16 idxMRPChnl_u16;
    uint16 dataReqMode_u16;
}BswM_Prv_IntrptQueueType_tst;

typedef struct
{
    uint8 idxHead_u8;
    uint8 idxTail_u8;
    uint8 dataSize_u8;
}BswM_Prv_IntrptQueueInfoType_tst;
/*
 **********************************************************************************************************************
 * Function definitions
 **********************************************************************************************************************
*/
#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

void BswM_Prv_IntrptQueueIn(BswM_Cfg_MRPType_ten dataMRPType_en, uint16 idChannel_u16, uint16 idxMRPChnl_u16, uint16 dataReqMode_u16);
Std_ReturnType BswM_Prv_IntrptQueueOut(BswM_Cfg_MRPType_ten * dataMRPType_pen, uint16 * idChannel_pu16, uint16 * idxMRPChnl_pu16, uint16 * dataReqMode_pu16);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define BSWM_START_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

extern BswM_Prv_IntrptQueueInfoType_tst BswM_Prv_IntrptQueueInfo_st;

#define BSWM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "BswM_MemMap.h"

#endif /* BSWM_PRV_INTRPTQUEUE_H */
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
