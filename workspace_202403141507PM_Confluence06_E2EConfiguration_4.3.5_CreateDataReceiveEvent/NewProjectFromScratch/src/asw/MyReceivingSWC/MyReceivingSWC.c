#include "Std_Types.h"

#include "Rte_MyReceivingSWC.h"

extern FUNC(void, BswM_CODE) BswM_RequestMode(VAR(BswM_UserType, AUTOMATIC) Requesting_user, VAR(BswM_ModeType, AUTOMATIC) Requested_mode); //TBD: Rte service port

#define MyReceivingSWC_START_SEC_CODE
#include "MyReceivingSWC_MemMap.h"
FUNC(void, MyReceivingSWC_CODE) RE_Rx_func(CONST(Rte_SelfType_MyReceivingSWC, AUTOMATIC) self){
   self->Pim_PIM_01 = 0;
   self->Pim_PIM_02 = 0x55;
   self->R_NvMService_0.Call_SetRamBlockStatus(self, TRUE);
// EcuM_SelectShutdownTrigger();
   BswM_RequestMode(0, 1);
}
#define MyReceivingSWC_STOP_SEC_CODE
#include "MyReceivingSWC_MemMap.h"

