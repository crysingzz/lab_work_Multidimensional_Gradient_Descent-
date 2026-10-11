/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>IEcoOptimizationTrace</summary>
 * <description>Optional ACOM gradient-descent trajectory observer.</description>
 * <reference>IEcoOptimizationTrace.idl</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#ifndef __I_ECO_OPTIMIZATION_TRACE_H__
#define __I_ECO_OPTIMIZATION_TRACE_H__

#include "IEcoArtificialIntelligence.h"

/* IEcoOptimizationTrace IID = {1C56BC65-1759-4278-80AF-4EDD861B2720} */
#ifndef __IID_IEcoOptimizationTrace
static const UGUID IID_IEcoOptimizationTrace = {0x01, 0x10,
    {0x1C, 0x56, 0xBC, 0x65, 0x17, 0x59, 0x42, 0x78,
     0x80, 0xAF, 0x4E, 0xDD, 0x86, 0x1B, 0x27, 0x20}};
#endif

typedef struct IEcoOptimizationTrace* IEcoOptimizationTracePtr_t;

/* Borrowed weights and context are valid only during the synchronous call. */
typedef int16_t (ECOCALLMETHOD *EcoIterationObserver)(
    /* in */ const double_t* weights, /* in */ uint32_t count,
    /* in */ uint32_t completedUpdates, /* in */ voidptr_t context);

typedef struct IEcoOptimizationTraceVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(
        /* in */ IEcoOptimizationTracePtr_t me, /* in */ const UGUID* riid,
        /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoOptimizationTracePtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoOptimizationTracePtr_t me);

    /* Outputs are published only after all callbacks and allocations succeed. */
    int16_t (ECOCALLMETHOD *OptimizeObserved)(
        /* in */ IEcoOptimizationTracePtr_t me,
        /* in */ IEcoVector* initial, /* in */ EcoLossFunction loss,
        /* in */ voidptr_t context, /* in */ double_t learningRate,
        /* in */ uint32_t maxIterations, /* in */ double_t differenceStep,
        /* in */ double_t gradientTolerance,
        /* in */ EcoIterationObserver observer,
        /* in */ voidptr_t observerContext,
        /* out */ IEcoVector** outWeights,
        /* out */ uint32_t* outIterations,
        /* out */ bool_t* outConverged, /* out */ double_t* outLoss);
} IEcoOptimizationTraceVTbl, *IEcoOptimizationTraceVTblPtr_t;

interface IEcoOptimizationTrace {
    struct IEcoOptimizationTraceVTbl* pVTbl;
} IEcoOptimizationTrace;

#endif /* __I_ECO_OPTIMIZATION_TRACE_H__ */
