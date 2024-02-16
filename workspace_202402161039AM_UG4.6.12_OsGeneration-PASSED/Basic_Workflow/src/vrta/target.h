#ifndef __TARGET_H__
#define __TARGET_H__

#include "Rte_Type.h"
#include "vrtaTypes.h"

typedef vrtaUInt TargetClockType;

#ifdef __cplusplus
extern "C" {
#endif

void TargetInit(void);
void TargetEnableInterrupts(void);
TargetClockType TargetReadClock( void );

#ifdef __cplusplus
}
#endif

#endif /*__TARGET_H__*/
