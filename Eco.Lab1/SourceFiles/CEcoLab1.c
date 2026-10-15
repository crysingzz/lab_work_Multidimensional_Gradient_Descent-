/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>CEcoLab1_9907DB59</summary>
 * <description>ACOM gradient descent with a preserved legacy interface.</description>
 * <reference>IEcoArtificialIntelligence.idl; IEcoLab1.h</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#include "CEcoLab1.h"
#include "ErrEcoLab1.h"
#include <stddef.h>

/* Interface pointers address their vtable fields inside the component. */
static CEcoLab1_9907DB59* CEcoLab1_FromAI(IEcoArtificialIntelligence* me) {
    return (CEcoLab1_9907DB59*)((unsigned char*)me -
        offsetof(CEcoLab1_9907DB59, m_pVTblIEcoArtificialIntelligence));
}

static CEcoLab1_9907DB59* CEcoLab1_FromTrace(IEcoOptimizationTrace* me) {
    return (CEcoLab1_9907DB59*)((unsigned char*)me -
        offsetof(CEcoLab1_9907DB59, m_pVTblIEcoOptimizationTrace));
}

static int16_t CEcoLab1_Query(CEcoLab1_9907DB59* pCMe,
    const UGUID* riid, voidptr_t* ppv) {
    if (ppv != 0) {
        *ppv = 0;
    }
    if (pCMe == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    if (IsEqualUGUID(riid, &IID_IEcoUnknown) ||
        IsEqualUGUID(riid, &IID_IEcoLab1)) {
        *ppv = (IEcoLab1*)pCMe;
    }
    else if (IsEqualUGUID(riid, &IID_IEcoArtificialIntelligence)) {
        *ppv = (IEcoArtificialIntelligence*)
            &pCMe->m_pVTblIEcoArtificialIntelligence;
    }
    else if (IsEqualUGUID(riid, &IID_IEcoOptimizationTrace)) {
        *ppv = (IEcoOptimizationTrace*)
            &pCMe->m_pVTblIEcoOptimizationTrace;
    }
    else {
        return ERR_ECO_NOINTERFACE;
    }
    ((IEcoLab1*)pCMe)->pVTbl->AddRef((IEcoLab1*)pCMe);
    return ERR_ECO_SUCCESS;
}

static uint32_t CEcoLab1_AddRef(CEcoLab1_9907DB59* pCMe) {
    if (pCMe == 0) {
        return 0;
    }
    ++pCMe->m_cRef;
    return pCMe->m_cRef;
}

static uint32_t CEcoLab1_Release(CEcoLab1_9907DB59* pCMe) {
    uint32_t remaining;
    if (pCMe == 0) {
        return 0;
    }
    remaining = --pCMe->m_cRef;
    if (remaining == 0) {
        pCMe->Delete(pCMe);
    }
    return remaining;
}

static int16_t ECOCALLMETHOD CEcoLab1_LegacyQuery(IEcoLab1* me,
    const UGUID* riid, voidptr_t* ppv) {
    return CEcoLab1_Query((CEcoLab1_9907DB59*)me, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab1_LegacyAddRef(IEcoLab1* me) {
    return CEcoLab1_AddRef((CEcoLab1_9907DB59*)me);
}

static uint32_t ECOCALLMETHOD CEcoLab1_LegacyRelease(IEcoLab1* me) {
    return CEcoLab1_Release((CEcoLab1_9907DB59*)me);
}

static int16_t ECOCALLMETHOD CEcoLab1_AIQuery(IEcoArtificialIntelligence* me,
    const UGUID* riid, voidptr_t* ppv) {
    if (me == 0) {
        if (ppv != 0) {
            *ppv = 0;
        }
        return ERR_ECO_POINTER;
    }
    return CEcoLab1_Query(CEcoLab1_FromAI(me), riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab1_AIAddRef(IEcoArtificialIntelligence* me) {
    if (me == 0) {
        return 0;
    }
    return CEcoLab1_AddRef(CEcoLab1_FromAI(me));
}

static uint32_t ECOCALLMETHOD CEcoLab1_AIRelease(IEcoArtificialIntelligence* me) {
    if (me == 0) {
        return 0;
    }
    return CEcoLab1_Release(CEcoLab1_FromAI(me));
}

static int16_t ECOCALLMETHOD CEcoLab1_TraceQuery(IEcoOptimizationTrace* me,
    const UGUID* riid, voidptr_t* ppv) {
    if (me == 0) {
        if (ppv != 0) {
            *ppv = 0;
        }
        return ERR_ECO_POINTER;
    }
    return CEcoLab1_Query(CEcoLab1_FromTrace(me), riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab1_TraceAddRef(IEcoOptimizationTrace* me) {
    if (me == 0) {
        return 0;
    }
    return CEcoLab1_AddRef(CEcoLab1_FromTrace(me));
}

static uint32_t ECOCALLMETHOD CEcoLab1_TraceRelease(IEcoOptimizationTrace* me) {
    if (me == 0) {
        return 0;
    }
    return CEcoLab1_Release(CEcoLab1_FromTrace(me));
}

/* Legacy ABI stays at the same VTbl slot; copied text remains object-owned. */
static int16_t ECOCALLMETHOD CEcoLab1_MyFunction(IEcoLab1* me,
    char_t* Name, char_t** copyName) {
    CEcoLab1_9907DB59* pCMe = (CEcoLab1_9907DB59*)me;
    char_t* copy;
    uint32_t length;
    uint32_t i;
    if (me == 0 || Name == 0 || copyName == 0) {
        return ERR_ECO_POINTER;
    }
    *copyName = 0;
    length = 0;
    while (Name[length] != 0) {
        if (length == (uint32_t)~(uint32_t)0 - 1) {
            return ERR_ECOLAB1_INVALID_ARGUMENT;
        }
        ++length;
    }
    copy = (char_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, length + 1);
    if (copy == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    for (i = 0; i <= length; ++i) {
        copy[i] = Name[i];
    }
    if (pCMe->m_Name != 0) {
        pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCMe->m_Name);
    }
    pCMe->m_Name = copy;
    *copyName = copy;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD CEcoLab1_CreateVector(
    IEcoArtificialIntelligence* me, uint32_t count, const double_t* values,
    IEcoVector** outVector) {
    CEcoLab1_9907DB59* pCMe;
    if (me == 0) {
        if (outVector != 0) {
            *outVector = 0;
        }
        return ERR_ECO_POINTER;
    }
    pCMe = CEcoLab1_FromAI(me);
    return EcoLab1CreateVector(pCMe->m_pIMem, count, values, outVector);
}

static int16_t CEcoLab1_Evaluate(EcoLossFunction loss, const double_t* base,
    double_t* trial, uint32_t count, uint32_t probeIndex, double_t probeValue,
    bool_t useProbe, voidptr_t context, double_t* outLoss) {
    uint32_t i;
    int16_t result;
    for (i = 0; i < count; ++i) {
        trial[i] = base[i];
    }
    if (useProbe) {
        trial[probeIndex] = probeValue;
    }
    result = loss(trial, count, context, outLoss);
    if (result != ERR_ECO_SUCCESS) {
        return result;
    }
    if (!EcoLab1IsFinite(*outLoss)) {
        return ERR_ECOLAB1_NONFINITE;
    }
    return ERR_ECO_SUCCESS;
}

/* All gradients refer to base; the working weights change only afterwards. */
static int16_t CEcoLab1_Gradient(EcoLossFunction loss,
    const double_t* base, double_t* trial, double_t* gradient,
    uint32_t count, double_t differenceStep, voidptr_t context,
    double_t* outMaximum) {
    uint32_t i;
    double_t scale;
    double_t h;
    double_t plusPoint;
    double_t minusPoint;
    double_t plusLoss;
    double_t minusLoss;
    double_t numerator;
    double_t denominator;
    double_t magnitude;
    int16_t result;
    *outMaximum = 0.0;
    for (i = 0; i < count; ++i) {
        scale = base[i] < 0.0 ? -base[i] : base[i];
        if (scale < 1.0) {
            scale = 1.0;
        }
        h = differenceStep * scale;
        if (!EcoLab1IsFinite(h)) {
            return ERR_ECOLAB1_NONFINITE;
        }
        if (h == 0.0) {
            return ERR_ECOLAB1_NUMERIC_ERROR;
        }
        plusPoint = base[i] + h;
        minusPoint = base[i] - h;
        if (!EcoLab1IsFinite(plusPoint) || !EcoLab1IsFinite(minusPoint)) {
            return ERR_ECOLAB1_NONFINITE;
        }
        if (plusPoint == base[i] || minusPoint == base[i]) {
            return ERR_ECOLAB1_NUMERIC_ERROR;
        }
        result = CEcoLab1_Evaluate(loss, base, trial, count, i, plusPoint,
            1, context, &plusLoss);
        if (result != ERR_ECO_SUCCESS) {
            return result;
        }
        result = CEcoLab1_Evaluate(loss, base, trial, count, i, minusPoint,
            1, context, &minusLoss);
        if (result != ERR_ECO_SUCCESS) {
            return result;
        }
        numerator = plusLoss - minusLoss;
        denominator = 2.0 * h;
        if (!EcoLab1IsFinite(numerator) || !EcoLab1IsFinite(denominator)) {
            return ERR_ECOLAB1_NONFINITE;
        }
        if (denominator == 0.0) {
            return ERR_ECOLAB1_NUMERIC_ERROR;
        }
        gradient[i] = numerator / denominator;
        if (!EcoLab1IsFinite(gradient[i])) {
            return ERR_ECOLAB1_NONFINITE;
        }
        magnitude = gradient[i] < 0.0 ? -gradient[i] : gradient[i];
        if (magnitude > *outMaximum) {
            *outMaximum = magnitude;
        }
    }
    return ERR_ECO_SUCCESS;
}

/* The output vector is published only after every callback and allocation succeeds. */
static int16_t CEcoLab1_OptimizeCore(
    CEcoLab1_9907DB59* pCMe, IEcoVector* initial,
    EcoLossFunction loss, voidptr_t context, double_t learningRate,
    uint32_t maxIterations, double_t differenceStep,
    double_t gradientTolerance, EcoIterationObserver observer,
    voidptr_t observerContext, IEcoVector** outWeights,
    uint32_t* outIterations, bool_t* outConverged, double_t* outLoss) {
    IEcoMemoryAllocator1* pIMem;
    double_t* weights;
    double_t* trial;
    double_t* gradient;
    IEcoVector* resultVector;
    uint32_t count;
    uint32_t i;
    uint32_t iteration;
    uint32_t bytes;
    double_t maximum;
    double_t delta;
    double_t value;
    double_t finalLoss;
    bool_t converged;
    int16_t result;
    if (outWeights != 0) {
        *outWeights = 0;
    }
    if (outIterations != 0) {
        *outIterations = 0;
    }
    if (outConverged != 0) {
        *outConverged = 0;
    }
    if (outLoss != 0) {
        *outLoss = 0.0;
    }
    if (pCMe == 0 || initial == 0 || loss == 0 || outWeights == 0 ||
        outIterations == 0 || outConverged == 0 || outLoss == 0) {
        return ERR_ECO_POINTER;
    }
    if (!EcoLab1IsFinite(learningRate) || !EcoLab1IsFinite(differenceStep) ||
        !EcoLab1IsFinite(gradientTolerance)) {
        return ERR_ECOLAB1_NONFINITE;
    }
    if (learningRate <= 0.0 || differenceStep <= 0.0 || maxIterations == 0 ||
        gradientTolerance < 0.0) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    result = initial->pVTbl->GetSize(initial, &count);
    if (result != ERR_ECO_SUCCESS) {
        return result;
    }
    if (count == 0 || count > ((uint32_t)~(uint32_t)0) / sizeof(double_t)) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    pIMem = pCMe->m_pIMem;
    bytes = count * (uint32_t)sizeof(double_t);
    weights = (double_t*)pIMem->pVTbl->Alloc(pIMem, bytes);
    trial = 0;
    gradient = 0;
    resultVector = 0;
    if (weights == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    result = ERR_ECO_SUCCESS;
    for (i = 0; i < count; ++i) {
        result = initial->pVTbl->GetValue(initial, i, &value);
        if (result != ERR_ECO_SUCCESS) {
            goto Cleanup;
        }
        if (!EcoLab1IsFinite(value)) {
            result = ERR_ECOLAB1_NONFINITE;
            goto Cleanup;
        }
        weights[i] = value;
    }
    trial = (double_t*)pIMem->pVTbl->Alloc(pIMem, bytes);
    if (trial == 0) {
        result = ERR_ECO_OUTOFMEMORY;
        goto Cleanup;
    }
    gradient = (double_t*)pIMem->pVTbl->Alloc(pIMem, bytes);
    if (gradient == 0) {
        result = ERR_ECO_OUTOFMEMORY;
        goto Cleanup;
    }
    if (observer != 0) {
        result = observer(weights, count, 0, observerContext);
        if (result != ERR_ECO_SUCCESS) {
            goto Cleanup;
        }
    }
    converged = 0;
    iteration = 0;
    while (iteration < maxIterations) {
        result = CEcoLab1_Gradient(loss, weights, trial, gradient, count,
            differenceStep, context, &maximum);
        if (result != ERR_ECO_SUCCESS) {
            goto Cleanup;
        }
        if (maximum <= gradientTolerance) {
            converged = 1;
            break;
        }
        for (i = 0; i < count; ++i) {
            delta = learningRate * gradient[i];
            value = weights[i] - delta;
            if (!EcoLab1IsFinite(delta) || !EcoLab1IsFinite(value)) {
                result = ERR_ECOLAB1_NONFINITE;
                goto Cleanup;
            }
            trial[i] = value;
        }
        for (i = 0; i < count; ++i) {
            weights[i] = trial[i];
        }
        ++iteration;
        if (observer != 0) {
            result = observer(weights, count, iteration, observerContext);
            if (result != ERR_ECO_SUCCESS) {
                goto Cleanup;
            }
        }
    }
    result = CEcoLab1_Evaluate(loss, weights, trial, count, 0, 0.0,
        0, context, &finalLoss);
    if (result != ERR_ECO_SUCCESS) {
        goto Cleanup;
    }
    result = EcoLab1CreateVector(pIMem, count, weights, &resultVector);
    if (result != ERR_ECO_SUCCESS) {
        goto Cleanup;
    }
    *outWeights = resultVector;
    *outIterations = iteration;
    *outConverged = converged;
    *outLoss = finalLoss;

Cleanup:
    if (gradient != 0) {
        pIMem->pVTbl->Free(pIMem, gradient);
    }
    if (trial != 0) {
        pIMem->pVTbl->Free(pIMem, trial);
    }
    pIMem->pVTbl->Free(pIMem, weights);
    return result;
}

static int16_t ECOCALLMETHOD CEcoLab1_Optimize(
    IEcoArtificialIntelligence* me, IEcoVector* initial,
    EcoLossFunction loss, voidptr_t context, double_t learningRate,
    uint32_t maxIterations, double_t differenceStep,
    double_t gradientTolerance, IEcoVector** outWeights,
    uint32_t* outIterations, bool_t* outConverged, double_t* outLoss) {
    CEcoLab1_9907DB59* pCMe = 0;
    if (me != 0) {
        pCMe = CEcoLab1_FromAI(me);
    }
    return CEcoLab1_OptimizeCore(pCMe, initial, loss, context,
        learningRate, maxIterations, differenceStep, gradientTolerance,
        0, 0, outWeights, outIterations, outConverged, outLoss);
}

static int16_t ECOCALLMETHOD CEcoLab1_OptimizeObserved(
    IEcoOptimizationTrace* me, IEcoVector* initial,
    EcoLossFunction loss, voidptr_t context, double_t learningRate,
    uint32_t maxIterations, double_t differenceStep,
    double_t gradientTolerance, EcoIterationObserver observer,
    voidptr_t observerContext, IEcoVector** outWeights,
    uint32_t* outIterations, bool_t* outConverged, double_t* outLoss) {
    CEcoLab1_9907DB59* pCMe = 0;
    if (me != 0) {
        pCMe = CEcoLab1_FromTrace(me);
    }
    if (observer == 0) {
        if (outWeights != 0) {
            *outWeights = 0;
        }
        if (outIterations != 0) {
            *outIterations = 0;
        }
        if (outConverged != 0) {
            *outConverged = 0;
        }
        if (outLoss != 0) {
            *outLoss = 0.0;
        }
        return ERR_ECO_POINTER;
    }
    return CEcoLab1_OptimizeCore(pCMe, initial, loss, context,
        learningRate, maxIterations, differenceStep, gradientTolerance,
        observer, observerContext, outWeights, outIterations,
        outConverged, outLoss);
}

static int16_t ECOCALLMETHOD initCEcoLab1_9907DB59(
    CEcoLab1_9907DB59Ptr_t me, IEcoUnknownPtr_t pIUnkSystem) {
    int16_t result;
    if (me == 0 || pIUnkSystem == 0) {
        return ERR_ECO_POINTER;
    }
    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem,
        &GID_IEcoSystem, (voidptr_t*)&me->m_pISys);
    if (result != ERR_ECO_SUCCESS || me->m_pISys == 0) {
        return result != ERR_ECO_SUCCESS ? result : ERR_ECO_NOSYSTEM;
    }
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD createCEcoLab1_9907DB59(
    CEcoLab1_9907DB59Ptr_t me, IEcoUnknownPtr_t pIUnkSystem,
    IEcoUnknownPtr_t pIUnkOuter) {
    if (me == 0 || pIUnkSystem == 0) {
        return ERR_ECO_POINTER;
    }
    if (pIUnkOuter != 0) {
        return ERR_ECO_NOAGGREGATION;
    }
    return ERR_ECO_SUCCESS;
}

static void ECOCALLMETHOD deleteCEcoLab1_9907DB59(
    CEcoLab1_9907DB59Ptr_t me) {
    IEcoMemoryAllocator1* pIMem;
    if (me == 0) {
        return;
    }
    pIMem = me->m_pIMem;
    if (me->m_Name != 0) {
        pIMem->pVTbl->Free(pIMem, me->m_Name);
    }
    if (me->m_pISys != 0) {
        me->m_pISys->pVTbl->Release(me->m_pISys);
    }
    pIMem->pVTbl->Free(pIMem, me);
    pIMem->pVTbl->Release(pIMem);
}

static IEcoLab1VTbl g_LegacyVTbl = {
    CEcoLab1_LegacyQuery,
    CEcoLab1_LegacyAddRef,
    CEcoLab1_LegacyRelease,
    CEcoLab1_MyFunction
};

static IEcoArtificialIntelligenceVTbl g_AIVTbl = {
    CEcoLab1_AIQuery,
    CEcoLab1_AIAddRef,
    CEcoLab1_AIRelease,
    CEcoLab1_CreateVector,
    CEcoLab1_Optimize
};

static IEcoOptimizationTraceVTbl g_TraceVTbl = {
    CEcoLab1_TraceQuery,
    CEcoLab1_TraceAddRef,
    CEcoLab1_TraceRelease,
    CEcoLab1_OptimizeObserved
};

CEcoLab1_9907DB59 g_xCEcoLab1_9907DB59 = {
    &g_LegacyVTbl,
    &g_AIVTbl,
    &g_TraceVTbl,
    initCEcoLab1_9907DB59,
    createCEcoLab1_9907DB59,
    deleteCEcoLab1_9907DB59,
    1,
    0,
    0,
    0
};
