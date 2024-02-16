#include "vrtaLoggerDevice.h"
#undef CONST

#include "target.h"
#include "Main_Cfg.h"
#include "VRTA_SystemClock.h"

extern Logger SysLogger;

/**
 * Initialize vrtaDevice.
 * This function is called by VRTA-OS to initialize vrtaDevices.
 */
void InitializeDevices( void )
{
    VECU_SystemClock_Init();
}

/**
 * Initialize the VECU
 * This function is called by OS_MAIN (see main.c) before the AUTOSAR-OS
 * is started.
 */
extern "C" void TargetInit(void)
{
   /* Prepare the target. */
   VECU_SystemClock_Start();
   SysLogger.setfile((vrtaTextPtr)"6runAll_Vecu_Logger.txt");
}

/**
 * Enable Interrupts.
 * This function is called by the OS Startup Hook (see OS_Hooks.c). The
 * interrupts are enabled by sending a vrtaAction the ICU-Manager.
 */
extern "C" void TargetEnableInterrupts( void )
{
   vrtaAction action;

   action.devID         = ICU_DEVICE_ID;
   action.devAction     = ICU_ACTION_ID_Unmask;
   action.devActionLen  = sizeof(action.devEmbeddedData.uVal);
   action.devActionData = NULL;
   for ( unsigned int i = 1; i < 32; ++i )
   {
      action.devEmbeddedData.uVal = i;
      vrtaSendAction( ICU_DEVICE_ID, &action );
   }
}

/**
 * Get value of VECU System Clock counter
 *
 * @return Number of ticks
 */
extern "C" TargetClockType TargetReadClock( void )
{
   return VECU_SystemClock_GetValue();
}
