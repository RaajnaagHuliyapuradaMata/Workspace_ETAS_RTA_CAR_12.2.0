
/*
 * Support code for MCALs that want to log information at run-time
 */
extern void mcal_warn(const char * id, const char * message);
extern void mcal_error(const char * id, const char * format, ...);
extern void mcal_printf(const char * id, const char * format, ...);


