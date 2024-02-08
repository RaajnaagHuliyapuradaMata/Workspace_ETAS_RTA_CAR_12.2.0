
#include "Os.h"
#include "Main_Cfg.h"

#include "Com.h"
#include "CanIf.h"
#include "ComM.h"
#include "EcuM.h"

TASK(InitTask) {
	EcuM_StartupTwo();
	Rte_Start();
    TerminateTask();
}


