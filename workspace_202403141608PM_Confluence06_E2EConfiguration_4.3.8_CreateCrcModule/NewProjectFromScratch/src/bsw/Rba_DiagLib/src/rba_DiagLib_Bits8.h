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

#ifndef RBA_DIAGLIB_BITS8_H
#define RBA_DIAGLIB_BITS8_H

#include "Std_Types.h"
#include "Compiler.h"

#define RBA_DIAGLIB_ISBITSET8       rba_DiagLib_Bit8IsBitSet
#define RBA_DIAGLIB_ISBITNOTSET8    rba_DiagLib_Bit8IsBitNotSet
#define RBA_DIAGLIB_GETSINGLEBIT8   rba_DiagLib_Bit8GetSingleBit
#define RBA_DIAGLIB_GETBITS8        rba_DiagLib_Bit8GetBits
#define RBA_DIAGLIB_OVERWRITEBIT8   rba_DiagLib_Bit8OverwriteBit
#define RBA_DIAGLIB_OVERWRITEBITS8  rba_DiagLib_Bit8OverwriteBits

LOCAL_INLINE uint8 rba_DiagLib_Bit8Mask(uint8 pos, uint8 len)
{
    uint8 bit2shift = 1;
    return (((bit2shift << len) - 1u) << pos);
}

LOCAL_INLINE void rba_DiagLib_Bit8SetBitMask(uint8 *buffer, uint8 bitMask)
{
    *buffer |= bitMask;
}

LOCAL_INLINE void rba_DiagLib_Bit8ClearBitMask(uint8 *buffer, uint8 bitMask)
{
    *buffer &= ((uint8) (~bitMask));
}

LOCAL_INLINE void rba_DiagLib_Bit8MergeBitmask(uint8 *buffer, uint8 value)
{
    *buffer &= value;
}

LOCAL_INLINE void rba_DiagLib_Bit8SetBit(uint8 *buffer, uint8 bit_position)
{
    uint8 bit2shift = 1;
    *buffer |= ((uint8) (bit2shift << bit_position));
}

LOCAL_INLINE void rba_DiagLib_Bit8ClearBit(uint8 *buffer, uint8 bit_position)
{
    uint8 bit2shift = 1;
    *buffer &= ((uint8) (~((uint8) (bit2shift << bit_position))));
}

LOCAL_INLINE void rba_DiagLib_Bit8OverwriteBit(uint8 *buffer, uint8 bit_position, boolean will_bit_be_set)
{
    if (will_bit_be_set)
    {
        rba_DiagLib_Bit8SetBit(buffer, bit_position);
    }
    else
    {
        rba_DiagLib_Bit8ClearBit(buffer, bit_position);
    }
}

LOCAL_INLINE uint8 rba_DiagLib_Bit8GetSingleBit(uint8 value, uint8 bit_position)
{
    return (uint8) ((value >> (bit_position)) & 1u);
}
LOCAL_INLINE boolean rba_DiagLib_Bit8IsBitSet(uint8 value, uint8 bit_position)
{
    return (boolean) (rba_DiagLib_Bit8GetSingleBit(value, bit_position) != 0u);
}
LOCAL_INLINE boolean rba_DiagLib_Bit8IsBitNotSet(uint8 value, uint8 bit_position)
{
    return (boolean) (rba_DiagLib_Bit8GetSingleBit(value, bit_position) == 0u);
}

LOCAL_INLINE uint8 rba_DiagLib_Bit8GetBits(uint8 value, uint8 bit_position, uint8 number_of_bits)
{
    uint8 bit2shift = 1;
    value = value >> bit_position;
    value = value % ((uint8) (bit2shift << number_of_bits));
    return value;
}

LOCAL_INLINE void rba_DiagLib_Bit8ClearBits(uint8 *value, uint8 bit_position, uint8 number_of_bits)
{
    uint8 bit2shift = 1;
    *value &= ((uint8) (~((uint8) ((((uint8) (bit2shift << number_of_bits)) - 1u) << bit_position))));
}

LOCAL_INLINE void rba_DiagLib_Bit8OverwriteBits(uint8 *value, uint8 bit_position, uint8 number_of_bits, uint8 newValue)
{
    uint8 bit2shift = 1;
    rba_DiagLib_Bit8ClearBits(value, bit_position, number_of_bits);
    *value |= ((uint8) ((newValue % ((uint8) (bit2shift << number_of_bits))) << bit_position));
}

LOCAL_INLINE void rba_DiagLib_Bit8ClearAll(uint8 *buffer)
{
    *buffer = 0u;
}

#endif

