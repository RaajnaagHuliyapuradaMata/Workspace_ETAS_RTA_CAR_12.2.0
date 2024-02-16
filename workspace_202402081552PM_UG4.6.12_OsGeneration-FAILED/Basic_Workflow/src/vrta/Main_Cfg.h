#ifndef __MAIN_CFG_H__
#define __MAIN_CFG_H__

/* ------------------------------------------------------------
 * Global settings that you may need to adapt according to your
 * AUTOSAR and OS configuration.
 * ------------------------------------------------------------ */


/* VECU System Clock Period: Duration of one Clock cycle in milli seconds. */
#define VECU_SYSTEMCLOCK_PERIOD 1

/* Number of the VRTA interrupt used by the VECU System Clock
 * (see VECU_SystemClock.cpp). This must match the interrupt number
 * specified in your RTA OS timer configuration (see there).
 */
#define VECU_SYSTEMCLOCK_IRQ 7


#endif /*__MAIN_CFG_H__*/
