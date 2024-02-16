#include "Os.h"
#include "Main_Cfg.h"
#include "Rte_Const.h"

/* This interrupt occurs every 1ms */
#define VECU_SYSTEMCLOCK_INTERVAL_US (1000)

/* Calculate rate to call Rte counter */
#define ms_per_rte_tick (RTE_PERIODIC_COUNTER_TICK_INTERVAL_US / VECU_SYSTEMCLOCK_INTERVAL_US)
static int rte_tick_interval_count = 0;

/* This ISR is driven by the VECU_SystemClock defined in VECU_SystemClock.cpp. */
ISR(VECU_SystemClock_ISR) {
#ifdef RTE_PERIODIC_COUNTER_TICK_INTERVAL_US
    if (ms_per_rte_tick == ++rte_tick_interval_count) {
        rte_tick_interval_count = 0;
        Os_IncrementCounter_Rte_TickCounter();
    }
#endif
}
