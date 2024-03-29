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

#ifndef RBA_DIAGLIB_BITS32_H
#define RBA_DIAGLIB_BITS32_H

#include "Std_Types.h"
#include "Compiler.h"

#define RBA_DIAGLIB_ISBITSET32       rba_DiagLib_Bit32IsBitSet
#define RBA_DIAGLIB_ISBITNOTSET32    rba_DiagLib_Bit32IsBitNotSet
#define RBA_DIAGLIB_GETSINGLEBIT32   rba_DiagLib_Bit32GetSingleBit
#define RBA_DIAGLIB_GETBITS32        rba_DiagLib_Bit32GetBits
#define RBA_DIAGLIB_OVERWRITEBIT32   rba_DiagLib_Bit32OverwriteBit
#define RBA_DIAGLIB_OVERWRITEBITS32  rba_DiagLib_Bit32OverwriteBits

LOCAL_INLINE uint32 rba_DiagLib_Bit32Mask(uint8 pos, uint8 len)
{
    uint32 bit2shift = 1;
    return (((bit2shift << len) - 1u) << pos);
}

LOCAL_INLINE void rba_DiagLib_Bit32SetBitMask(uint32 *buffer, uint32 bitMask)
{
    *buffer |= bitMask;
}

LOCAL_INLINE void rba_DiagLib_Bit32ClearBitMask(uint32 *buffer, uint32 bitMask)
{
    *buffer &= ((uint32) (~bitMask));
}

LOCAL_INLINE void rba_DiagLib_Bit32MergeBitmask(uint32 *buffer, uint32 value)
{
    *buffer &= value;
}

LOCAL_INLINE void rba_DiagLib_Bit32SetBit(uint32 *buffer, uint8 bit_position)
{
    uint32 bit2shift = 1;
    *buffer |= ((uint32) (bit2shift << bit_position));
}

LOCAL_INLINE void rba_DiagLib_Bit32ClearBit(uint32 *buffer, uint8 bit_position)
{
    uint32 bit2shift = 1;
    *buffer &= ((uint32) (~((uint32) (bit2shift << bit_position))));
}

LOCAL_INLINE void rba_DiagLib_Bit32OverwriteBit(uint32 *buffer, uint8 bit_position, boolean will_bit_be_set)
{
    if (will_bit_be_set)
    {
        rba_DiagLib_Bit32SetBit(buffer, bit_position);
    }
    else
    {
        rba_DiagLib_Bit32ClearBit(buffer, bit_position);
    }
}

LOCAL_INLINE uint32 rba_DiagLib_Bit32GetSingleBit(uint32 value, uint8 bit_position)
{
    return (uint32) ((value >> bit_position) & 1u);
}

LOCAL_INLINE boolean rba_DiagLib_Bit32IsBitSet(uint32 value, uint8 bit_position)
{
    return (boolean) (rba_DiagLib_Bit32GetSingleBit(value, bit_position) != 0u);
}

LOCAL_INLINE boolean rba_DiagLib_Bit32IsBitNotSet(uint32 value, uint8 bit_position)
{
    return (boolean) (rba_DiagLib_Bit32GetSingleBit(value, bit_position) == 0u);
}

LOCAL_INLINE uint32 rba_DiagLib_Bit32GetBits(uint32 value, uint8 bit_position, uint8 number_of_bits)
{
    uint32 bit2shift = 1;
    value = value >> bit_position;
    value = value % ((uint32) (bit2shift << number_of_bits));
    return value;
}

LOCAL_INLINE void rba_DiagLib_Bit32ClearBits(uint32 *value, uint8 bit_position, uint8 number_of_bits)
{
    uint32 bit2shift = 1;
    *value &= ((uint32) (~((uint32) ((((uint32) (bit2shift << number_of_bits)) - 1u) << bit_position))));
}

LOCAL_INLINE void rba_DiagLib_Bit32OverwriteBits(uint32 *value, uint8 bit_position, uint8 number_of_bits, uint32 newValue)
{
    uint32 bit2shift = 1;
    rba_DiagLib_Bit32ClearBits(value, bit_position, number_of_bits);
    *value |= ((uint32) ((newValue % ((uint32) (bit2shift << number_of_bits))) << bit_position));
}

LOCAL_INLINE void rba_DiagLib_Bit32ClearAll(uint32 *buffer)
{
    *buffer = 0u;
}

#endif

