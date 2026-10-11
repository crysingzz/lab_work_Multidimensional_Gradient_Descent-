/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>CEcoVector</summary>
 * <description>Private storage for the IEcoVector interface.</description>
 * <reference>IEcoVector.idl</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#ifndef __C_ECO_VECTOR_H__
#define __C_ECO_VECTOR_H__

#include "IEcoVector.h"
#include "IEcoMemoryAllocator1.h"

int16_t EcoLab1IsFinite(double_t value);
int16_t EcoLab1CreateVector(IEcoMemoryAllocator1* pIMem, uint32_t count,
    const double_t* values, IEcoVector** outVector);

#endif /* __C_ECO_VECTOR_H__ */
