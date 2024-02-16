#include "vrtaCore.h"
#include "os.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
void VECU_SystemClock_Init();
void VECU_SystemClock_Start();

vrtaUInt VECU_SystemClock_GetValue();


#ifdef __cplusplus
}
#endif
