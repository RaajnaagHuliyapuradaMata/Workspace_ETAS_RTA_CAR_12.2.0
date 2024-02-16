#ifndef DEM_H
#define DEM_H

#include "Std_Types.h"

#define DEM_AR_RELEASE_MAJOR_VERSION 4
#define DEM_AR_RELEASE_MINOR_VERSION 2

typedef uint8 Dem_InitMonitorReasonType;
typedef uint8 Dem_EventStatusType;

typedef uint16 Dem_EventIdType;

void Dem_ReportErrorStatus (Dem_EventIdType EventId, uint8 EventStatus);
Std_ReturnType Dem_SetEventStatus (Dem_EventIdType EventId, Dem_EventStatusType EventStatus);

#define DEM_EVENT_STATUS_PASSED			0x00
#define DEM_EVENT_STATUS_FAILED			0x01
#define DEM_EVENT_STATUS_PREPASSED      0x02
#define DEM_EVENT_STATUS_PREFAILED		0x03

#define FLS_E_ERASE_FAILED				0x01
#define FLS_E_WRITE_FAILED				0x02
#define FLS_E_READ_FAILED				0x03
#define FLS_E_COMPARE_FAILED			0x04

#endif
