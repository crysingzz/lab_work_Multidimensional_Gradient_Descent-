/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>CEcoVector</summary>
 * <description>Owned ACOM vector for optimizer weights.</description>
 * <reference>IEcoVector.idl</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#include "CEcoVector.h"
#include "ErrEcoLab1.h"

typedef struct CEcoVector {
    IEcoVectorVTbl* m_pVTbl;
    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    uint32_t m_count;
    double_t* m_values;
} CEcoVector;

/* double_t is the Eco platform double; a comparison rejects both infinities. */
int16_t EcoLab1IsFinite(double_t value) {
    return (int16_t)(value == value && value <= 1.7976931348623157e308 &&
        value >= -1.7976931348623157e308);
}

static int16_t ECOCALLMETHOD CEcoVector_QueryInterface(IEcoVector* me,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv != 0) {
        *ppv = 0;
    }
    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    if (!IsEqualUGUID(riid, &IID_IEcoVector) &&
        !IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        return ERR_ECO_NOINTERFACE;
    }
    *ppv = me;
    me->pVTbl->AddRef(me);
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoVector_AddRef(IEcoVector* me) {
    CEcoVector* pCMe = (CEcoVector*)me;
    if (me == 0) {
        return 0;
    }
    ++pCMe->m_cRef;
    return pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoVector_Release(IEcoVector* me) {
    CEcoVector* pCMe = (CEcoVector*)me;
    IEcoMemoryAllocator1* pIMem;
    uint32_t remaining;
    if (me == 0) {
        return 0;
    }
    remaining = --pCMe->m_cRef;
    if (remaining == 0) {
        pIMem = pCMe->m_pIMem;
        pIMem->pVTbl->Free(pIMem, pCMe->m_values);
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
    return remaining;
}

static int16_t ECOCALLMETHOD CEcoVector_GetSize(IEcoVector* me,
    uint32_t* outCount) {
    if (me == 0 || outCount == 0) {
        return ERR_ECO_POINTER;
    }
    *outCount = ((CEcoVector*)me)->m_count;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CEcoVector_GetValue(IEcoVector* me,
    uint32_t index, double_t* outValue) {
    CEcoVector* pCMe = (CEcoVector*)me;
    if (me == 0 || outValue == 0) {
        return ERR_ECO_POINTER;
    }
    if (index >= pCMe->m_count) {
        return ERR_ECOLAB1_INDEX_OUT_OF_RANGE;
    }
    *outValue = pCMe->m_values[index];
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CEcoVector_SetValue(IEcoVector* me,
    uint32_t index, double_t value) {
    CEcoVector* pCMe = (CEcoVector*)me;
    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    if (index >= pCMe->m_count) {
        return ERR_ECOLAB1_INDEX_OUT_OF_RANGE;
    }
    if (!EcoLab1IsFinite(value)) {
        return ERR_ECOLAB1_NONFINITE;
    }
    pCMe->m_values[index] = value;
    return ERR_ECO_SUCCESS;
}

static IEcoVectorVTbl g_EcoVectorVTbl = {
    CEcoVector_QueryInterface,
    CEcoVector_AddRef,
    CEcoVector_Release,
    CEcoVector_GetSize,
    CEcoVector_GetValue,
    CEcoVector_SetValue
};

/* The allocator receives a retained reference on successful construction. */
int16_t EcoLab1CreateVector(IEcoMemoryAllocator1* pIMem, uint32_t count,
    const double_t* values, IEcoVector** outVector) {
    CEcoVector* pCMe;
    uint32_t i;
    uint32_t bytes;
    if (outVector == 0) {
        return ERR_ECO_POINTER;
    }
    *outVector = 0;
    if (pIMem == 0 || values == 0) {
        return ERR_ECO_POINTER;
    }
    if (count == 0 || count > ((uint32_t)~(uint32_t)0) / sizeof(double_t)) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    for (i = 0; i < count; ++i) {
        if (!EcoLab1IsFinite(values[i])) {
            return ERR_ECOLAB1_NONFINITE;
        }
    }
    bytes = count * (uint32_t)sizeof(double_t);
    pCMe = (CEcoVector*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)sizeof(CEcoVector));
    if (pCMe == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    pCMe->m_values = (double_t*)pIMem->pVTbl->Alloc(pIMem, bytes);
    if (pCMe->m_values == 0) {
        pIMem->pVTbl->Free(pIMem, pCMe);
        return ERR_ECO_OUTOFMEMORY;
    }
    pCMe->m_pVTbl = &g_EcoVectorVTbl;
    pCMe->m_cRef = 1;
    pCMe->m_pIMem = pIMem;
    pCMe->m_count = count;
    for (i = 0; i < count; ++i) {
        pCMe->m_values[i] = values[i];
    }
    pIMem->pVTbl->AddRef(pIMem);
    *outVector = (IEcoVector*)pCMe;
    return ERR_ECO_SUCCESS;
}
