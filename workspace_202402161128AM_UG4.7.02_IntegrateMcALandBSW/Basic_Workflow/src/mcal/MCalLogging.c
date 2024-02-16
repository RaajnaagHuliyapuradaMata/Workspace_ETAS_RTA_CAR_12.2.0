
#include "MCalLogging.h"
#include <stdio.h>
#include <stdarg.h>

extern int vrtaPrintf(const char *format, ...);

void mcal_warn(const char * id, const char * message) {
  mcal_printf(id, message);
}

void mcal_error(const char * id, const char * format, ...)  {
  va_list argptr;
  char buf[1024];
  va_start(argptr, format);
  vrtaPrintf("%s Error: ", id);
  vsprintf(buf, format, argptr);
  vrtaPrintf("\n");
  va_end(argptr);
  vrtaPrintf(buf);
}

void mcal_printf(const char * id, const char * format, ...) {
  va_list argptr;
  char buf[1024];
  va_start(argptr, format);
  vrtaPrintf("%s: ", id);
  vsprintf(buf, format, argptr);
  va_end(argptr);
  vrtaPrintf(buf);
}


