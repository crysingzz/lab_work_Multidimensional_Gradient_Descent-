/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>IEcoVector</summary>
 * <description>Public ACOM vector of finite double_t weights.</description>
 * <reference>IEcoVector.idl</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#ifndef __I_ECO_VECTOR_H__
#define __I_ECO_VECTOR_H__

#include "IEcoBase1.h"

/* IEcoVector IID = {66142D06-DCC4-4B5D-84EB-22F401BBC9DE} */
#ifndef __IID_IEcoVector
static const UGUID IID_IEcoVector = {0x01, 0x10, {0x66, 0x14, 0x2D, 0x06,
    0xDC, 0xC4, 0x4B, 0x5D, 0x84, 0xEB, 0x22, 0xF4, 0x01, 0xBB, 0xC9, 0xDE}};
#endif

typedef struct IEcoVector* IEcoVectorPtr_t;

typedef struct IEcoVectorVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoVectorPtr_t me,
        /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoVectorPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoVectorPtr_t me);

    /* The dimension is immutable; a successful outCount receives count > 0. */
    int16_t (ECOCALLMETHOD *GetSize)(/* in */ IEcoVectorPtr_t me,
        /* out */ uint32_t* outCount);
    /* Index is zero-based; only finite values may be stored. */
    int16_t (ECOCALLMETHOD *GetValue)(/* in */ IEcoVectorPtr_t me,
        /* in */ uint32_t index, /* out */ double_t* outValue);
    int16_t (ECOCALLMETHOD *SetValue)(/* in */ IEcoVectorPtr_t me,
        /* in */ uint32_t index, /* in */ double_t value);
} IEcoVectorVTbl, *IEcoVectorVTblPtr_t;

interface IEcoVector {
    struct IEcoVectorVTbl* pVTbl;
} IEcoVector;

#endif /* __I_ECO_VECTOR_H__ */
