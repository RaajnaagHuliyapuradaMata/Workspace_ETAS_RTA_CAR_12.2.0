/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2012, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef BFX_TYPES_H
#define BFX_TYPES_H

/* 64 bit integer data types */
typedef unsigned long long Bfx_uint64;

/*
 **********************************************************************************************************************
 * Hash defines, symbols
 **********************************************************************************************************************
 */
 
#define BFX_MAXUINT8        (0xff)
#define BFX_MAXUINT8_U      (0xffu)
#define BFX_MINUINT8        (0x0)
#define BFX_MAXSINT8        (0x7f)
#define BFX_MINSINT8        (-(BFX_MAXSINT8) - 1)
#define BFX_MAXUINT16       (0xffff)
#define BFX_MAXUINT16_U     (0xffffu)
#define BFX_MINUINT16       (0x0)
#define BFX_MAXSINT16       (0x7fff)
#define BFX_MINSINT16       (-(BFX_MAXSINT16) - 1)
#define BFX_MAXUINT32       (0xffffffffL)
#define BFX_MAXUINT32_U     (0xffffffffuL)
#define BFX_MINUINT32       (0x0uL)
#define BFX_MAXSINT32       (0x7fffffffL)
#define BFX_MAXSINT32_U     (0x7fffffffuL)
#define BFX_MINSINT32       (-(BFX_MAXSINT32)-1L)
#define BFX_MAXUINT64       (0xFFFFFFFFFFFFFFFFLL)
#define BFX_MAXUINT64_U     (0xFFFFFFFFFFFFFFFFuLL)
#define BFX_MINUINT64       (0x0uLL)




/* BFX_TYPES_H */
#endif
