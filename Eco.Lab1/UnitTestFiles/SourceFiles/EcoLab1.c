/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>EcoLab1 UnitTest</summary>
 * <description>Console assertions for the ACOM gradient-descent component.</description>
 * <reference>openspec/changes/implement-gradient-descent-lab76</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoLab1.h"
#include "IEcoArtificialIntelligence.h"
#include "IEcoOptimizationTrace.h"
#include "ErrEcoLab1.h"
#include "CEcoLab1.h"

#define TEST_CHECK(condition, code) \
    do { if (!(condition)) { result = (code); goto Cleanup; } } while (0)

typedef struct LossContext {
    uint32_t count;
    uint32_t calls;
    uint32_t failAt;
    int16_t failCode;
    double_t target[4];
    double_t specialLoss;
    bool_t returnSpecial;
} LossContext;

typedef struct TraceContext {
    uint32_t calls;
    uint32_t failAt;
    int16_t failCode;
    double_t snapshots[4];
} TraceContext;

static int16_t ECOCALLMETHOD TraceWeights(const double_t* weights,
    uint32_t count, uint32_t completedUpdates, voidptr_t context) {
    TraceContext* pTrace = (TraceContext*)context;
    if (weights == 0 || pTrace == 0 || count != 1 ||
        completedUpdates != pTrace->calls || pTrace->calls >= 4) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    pTrace->snapshots[pTrace->calls] = weights[0];
    ++pTrace->calls;
    if (pTrace->failAt != 0 && pTrace->calls == pTrace->failAt) {
        return pTrace->failCode;
    }
    return ERR_ECO_SUCCESS;
}

static void InitTrace(TraceContext* pTrace) {
    uint32_t i;
    pTrace->calls = 0;
    pTrace->failAt = 0;
    pTrace->failCode = 1801;
    for (i = 0; i < 4; ++i) {
        pTrace->snapshots[i] = 0.0;
    }
}

static double_t TestAbs(double_t value) {
    return value < 0.0 ? -value : value;
}

static bool_t TestNear(double_t value, double_t expected, double_t tolerance) {
    return (bool_t)(TestAbs(value - expected) <= tolerance);
}

static int16_t ECOCALLMETHOD QuadraticLoss(const double_t* weights,
    uint32_t count, voidptr_t context, double_t* outLoss) {
    LossContext* pContext = (LossContext*)context;
    uint32_t i;
    double_t delta;
    double_t sum;
    ++pContext->calls;
    if (pContext->failAt != 0 && pContext->calls == pContext->failAt) {
        return pContext->failCode;
    }
    if (pContext->returnSpecial) {
        *outLoss = pContext->specialLoss;
        return ERR_ECO_SUCCESS;
    }
    if (weights == 0 || outLoss == 0 || count != pContext->count) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    sum = 0.0;
    for (i = 0; i < count; ++i) {
        delta = weights[i] - pContext->target[i];
        sum += delta * delta;
    }
    *outLoss = sum;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD MSELoss(const double_t* weights,
    uint32_t count, voidptr_t context, double_t* outLoss) {
    LossContext* pContext = (LossContext*)context;
    double_t error;
    double_t sum;
    uint32_t i;
    if (count != 2 || weights == 0 || outLoss == 0) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    ++pContext->calls;
    sum = 0.0;
    for (i = 0; i < 4; ++i) {
        error = weights[0] + weights[1] * (double_t)i -
            (1.0 + 2.0 * (double_t)i);
        sum += error * error;
    }
    *outLoss = sum / 4.0;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD MixedLoss(const double_t* weights,
    uint32_t count, voidptr_t context, double_t* outLoss) {
    LossContext* pContext = (LossContext*)context;
    double_t x;
    double_t y;
    if (count != 2 || weights == 0 || outLoss == 0) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    ++pContext->calls;
    x = weights[0] - 1.0;
    y = weights[1] + 2.0;
    *outLoss = x * x + y * y + 0.5 * x * y;
    return ERR_ECO_SUCCESS;
}

static void InitLoss(LossContext* pContext, uint32_t count) {
    uint32_t i;
    pContext->count = count;
    pContext->calls = 0;
    pContext->failAt = 0;
    pContext->failCode = 1701;
    pContext->specialLoss = 0.0;
    pContext->returnSpecial = 0;
    for (i = 0; i < 4; ++i) {
        pContext->target[i] = 0.0;
    }
}

static int16_t TestVector(IEcoArtificialIntelligence* pIAI,
    double_t infinity, double_t notNumber) {
    IEcoVector* pIVector = 0;
    IEcoVector* pIQueried = 0;
    IEcoUnknown* pIUnknown = 0;
    double_t source[2];
    double_t value;
    voidptr_t invalidInterface;
    uint32_t count;
    uint32_t refCount;
    int16_t result;
    source[0] = 1.0;
    source[1] = -2.0;
    result = pIAI->pVTbl->CreateVector(pIAI, 2, source, &pIVector);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIVector != 0, 101);
    source[0] = 10.0;
    TEST_CHECK(pIVector->pVTbl->GetSize(pIVector, &count) == ERR_ECO_SUCCESS &&
        count == 2, 102);
    TEST_CHECK(pIVector->pVTbl->GetValue(pIVector, 0, &value) == ERR_ECO_SUCCESS &&
        value == 1.0, 103);
    TEST_CHECK(pIVector->pVTbl->SetValue(pIVector, 1, 4.0) == ERR_ECO_SUCCESS, 104);
    TEST_CHECK(pIVector->pVTbl->GetValue(pIVector, 1, &value) == ERR_ECO_SUCCESS &&
        value == 4.0, 105);
    TEST_CHECK(pIVector->pVTbl->SetValue(pIVector, 2, 3.0) ==
        ERR_ECOLAB1_INDEX_OUT_OF_RANGE, 106);
    TEST_CHECK(pIVector->pVTbl->GetValue(pIVector, 2, &value) ==
        ERR_ECOLAB1_INDEX_OUT_OF_RANGE, 107);
    TEST_CHECK(pIVector->pVTbl->SetValue(pIVector, 1, infinity) ==
        ERR_ECOLAB1_NONFINITE, 108);
    TEST_CHECK(pIVector->pVTbl->SetValue(pIVector, 1, notNumber) ==
        ERR_ECOLAB1_NONFINITE, 109);
    TEST_CHECK(pIVector->pVTbl->GetValue(pIVector, 1, &value) == ERR_ECO_SUCCESS &&
        value == 4.0, 110);
    TEST_CHECK(pIVector->pVTbl->QueryInterface(pIVector, &IID_IEcoUnknown,
        (voidptr_t*)&pIUnknown) == ERR_ECO_SUCCESS && pIUnknown != 0, 111);
    TEST_CHECK(pIVector->pVTbl->QueryInterface(pIVector, &IID_IEcoVector,
        (voidptr_t*)&pIQueried) == ERR_ECO_SUCCESS && pIQueried == pIVector, 112);
    refCount = pIVector->pVTbl->AddRef(pIVector);
    TEST_CHECK(pIVector->pVTbl->Release(pIVector) + 1 == refCount, 113);
    TEST_CHECK(pIQueried->pVTbl->Release(pIQueried) >= 1, 114);
    pIQueried = 0;
    TEST_CHECK(pIUnknown->pVTbl->Release(pIUnknown) == 1, 115);
    pIUnknown = 0;
    TEST_CHECK(pIAI->pVTbl->CreateVector(pIAI, 0, source, &pIQueried) ==
        ERR_ECOLAB1_INVALID_ARGUMENT && pIQueried == 0, 116);
    TEST_CHECK(pIAI->pVTbl->CreateVector(pIAI,
        ((uint32_t)~(uint32_t)0) / (uint32_t)sizeof(double_t) + 1,
        source, &pIQueried) == ERR_ECOLAB1_INVALID_ARGUMENT, 117);
    TEST_CHECK(pIAI->pVTbl->CreateVector(pIAI, 1, 0, &pIQueried) ==
        ERR_ECO_POINTER, 118);
    source[0] = infinity;
    TEST_CHECK(pIAI->pVTbl->CreateVector(pIAI, 1, source, &pIQueried) ==
        ERR_ECOLAB1_NONFINITE, 119);
    source[0] = notNumber;
    TEST_CHECK(pIAI->pVTbl->CreateVector(pIAI, 1, source, &pIQueried) ==
        ERR_ECOLAB1_NONFINITE, 120);
    TEST_CHECK(pIVector->pVTbl->GetSize(pIVector, 0) == ERR_ECO_POINTER, 121);
    invalidInterface = (voidptr_t)pIVector;
    TEST_CHECK(pIVector->pVTbl->QueryInterface(pIVector, &CID_EcoLab1,
        &invalidInterface) == ERR_ECO_NOINTERFACE &&
        invalidInterface == 0, 122);
    invalidInterface = (voidptr_t)pIVector;
    TEST_CHECK(pIVector->pVTbl->QueryInterface(pIVector, 0,
        &invalidInterface) == ERR_ECO_POINTER && invalidInterface == 0, 123);
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIQueried != 0) {
        pIQueried->pVTbl->Release(pIQueried);
    }
    if (pIUnknown != 0) {
        pIUnknown->pVTbl->Release(pIUnknown);
    }
    if (pIVector != 0) {
        pIVector->pVTbl->Release(pIVector);
    }
    return result;
}

static int16_t TestQuadratic(IEcoArtificialIntelligence* pIAI,
    uint32_t dimension) {
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    LossContext context;
    double_t values[4];
    double_t value;
    double_t loss;
    uint32_t i;
    uint32_t iterations;
    bool_t converged;
    int16_t result;
    InitLoss(&context, dimension);
    for (i = 0; i < dimension; ++i) {
        context.target[i] = (double_t)i - 1.0;
        values[i] = context.target[i] + 2.0;
    }
    result = pIAI->pVTbl->CreateVector(pIAI, dimension, values, &pIInitial);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 201);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.25, 80, 0.00001, 0.000001, &pIResult, &iterations,
        &converged, &loss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIResult != 0 && converged, 202);
    TEST_CHECK(iterations > 0 && iterations < 80, 203);
    TEST_CHECK(context.calls == 2 * dimension * (iterations + 1) + 1, 204);
    TEST_CHECK(TestNear(loss, 0.0, 0.00000000001), 205);
    for (i = 0; i < dimension; ++i) {
        TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, i, &value) ==
            ERR_ECO_SUCCESS, 206);
        TEST_CHECK(TestNear(value, context.target[i], 0.000001), 207);
        TEST_CHECK(pIInitial->pVTbl->GetValue(pIInitial, i, &value) ==
            ERR_ECO_SUCCESS && value == values[i], 208);
    }
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIResult != 0) {
        pIResult->pVTbl->Release(pIResult);
    }
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

static int16_t TestMSEAndReplacement(IEcoArtificialIntelligence* pIAI) {
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    LossContext context;
    double_t values[2];
    double_t weight;
    double_t loss;
    uint32_t iterations;
    bool_t converged;
    int16_t result;
    values[0] = 0.0;
    values[1] = 0.0;
    InitLoss(&context, 2);
    result = pIAI->pVTbl->CreateVector(pIAI, 2, values, &pIInitial);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 301);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, MSELoss, &context,
        0.1, 1000, 0.00001, 0.000001, &pIResult, &iterations,
        &converged, &loss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && converged, 302);
    TEST_CHECK(context.calls == 4 * (iterations + 1) + 1, 303);
    TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, 0, &weight) ==
        ERR_ECO_SUCCESS && TestNear(weight, 1.0, 0.00001), 304);
    TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, 1, &weight) ==
        ERR_ECO_SUCCESS && TestNear(weight, 2.0, 0.00001), 305);
    TEST_CHECK(TestNear(loss, 0.0, 0.000000001), 306);
    pIResult->pVTbl->Release(pIResult);
    pIResult = 0;
    InitLoss(&context, 2);
    context.target[0] = -3.0;
    context.target[1] = 4.0;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.25, 100, 0.00001, 0.000001, &pIResult, &iterations,
        &converged, &loss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && converged, 307);
    TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, 0, &weight) ==
        ERR_ECO_SUCCESS && TestNear(weight, -3.0, 0.00001), 308);
    TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, 1, &weight) ==
        ERR_ECO_SUCCESS && TestNear(weight, 4.0, 0.00001), 309);
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIResult != 0) {
        pIResult->pVTbl->Release(pIResult);
    }
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

static int16_t TestStopping(IEcoArtificialIntelligence* pIAI) {
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    LossContext context;
    double_t initial;
    double_t value;
    double_t loss;
    uint32_t iterations;
    bool_t converged;
    int16_t result;
    initial = 2.0;
    InitLoss(&context, 1);
    context.target[0] = 2.0;
    result = pIAI->pVTbl->CreateVector(pIAI, 1, &initial, &pIInitial);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 401);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 10, 0.00001, 0.000001, &pIResult, &iterations,
        &converged, &loss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && converged && iterations == 0, 402);
    TEST_CHECK(context.calls == 3 && loss == 0.0, 403);
    pIResult->pVTbl->Release(pIResult);
    pIResult = 0;
    context.target[0] = 0.0;
    context.calls = 0;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 1, 0.00001, 0.0, &pIResult, &iterations,
        &converged, &loss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && !converged && iterations == 1, 404);
    TEST_CHECK(context.calls == 3, 405);
    TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, 0, &value) ==
        ERR_ECO_SUCCESS && TestNear(value, 1.6, 0.000001), 406);
    TEST_CHECK(TestNear(loss, 2.56, 0.00001), 407);
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIResult != 0) {
        pIResult->pVTbl->Release(pIResult);
    }
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

static int16_t TestSimultaneous(IEcoArtificialIntelligence* pIAI) {
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    LossContext context;
    double_t values[2];
    double_t value;
    double_t loss;
    uint32_t iterations;
    bool_t converged;
    int16_t result;
    values[0] = 0.0;
    values[1] = 0.0;
    InitLoss(&context, 2);
    result = pIAI->pVTbl->CreateVector(pIAI, 2, values, &pIInitial);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 501);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, MixedLoss, &context,
        0.1, 1, 0.00001, 0.0, &pIResult, &iterations,
        &converged, &loss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && iterations == 1 && !converged, 502);
    TEST_CHECK(context.calls == 5, 503);
    TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, 0, &value) ==
        ERR_ECO_SUCCESS && TestNear(value, 0.1, 0.000001), 504);
    TEST_CHECK(pIResult->pVTbl->GetValue(pIResult, 1, &value) ==
        ERR_ECO_SUCCESS && TestNear(value, -0.35, 0.000001), 505);
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIResult != 0) {
        pIResult->pVTbl->Release(pIResult);
    }
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

static int16_t ECOCALLMETHOD TraceWithoutContext(const double_t* weights,
    uint32_t count, uint32_t completedUpdates, voidptr_t context) {
    (void)context;
    if (weights == 0 || count != 1 || completedUpdates > 1) {
        return ERR_ECOLAB1_INVALID_ARGUMENT;
    }
    return ERR_ECO_SUCCESS;
}

static int16_t TestTrace(IEcoArtificialIntelligence* pIAI,
    IEcoOptimizationTrace* pITrace) {
    IEcoVector* pIInitial = 0;
    IEcoVector* pIZero = 0;
    IEcoVector* pIBase = 0;
    IEcoVector* pIObserved = 0;
    LossContext lossContext;
    TraceContext trace;
    double_t initial;
    double_t zero;
    double_t value;
    double_t baseLoss;
    double_t observedLoss;
    uint32_t baseIterations;
    uint32_t observedIterations;
    bool_t baseConverged;
    bool_t observedConverged;
    int16_t result;
    initial = 2.0;
    zero = 0.0;
    InitLoss(&lossContext, 1);
    InitTrace(&trace);
    result = pIAI->pVTbl->CreateVector(pIAI, 1, &initial, &pIInitial);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIInitial != 0, 901);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial,
        QuadraticLoss, &lossContext, 0.1, 3, 0.00001, 0.0,
        &pIBase, &baseIterations, &baseConverged, &baseLoss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIBase != 0 &&
        baseIterations == 3 && !baseConverged && lossContext.calls == 7, 902);
    lossContext.calls = 0;
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIInitial,
        QuadraticLoss, &lossContext, 0.1, 3, 0.00001, 0.0,
        TraceWeights, &trace, &pIObserved, &observedIterations,
        &observedConverged, &observedLoss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIObserved != 0 &&
        observedIterations == baseIterations &&
        observedConverged == baseConverged &&
        TestNear(observedLoss, baseLoss, 0.000000001) &&
        lossContext.calls == 7 && trace.calls == 4, 903);
    TEST_CHECK(TestNear(trace.snapshots[0], 2.0, 0.000001) &&
        TestNear(trace.snapshots[1], 1.6, 0.000001) &&
        TestNear(trace.snapshots[2], 1.28, 0.000001) &&
        TestNear(trace.snapshots[3], 1.024, 0.000001), 904);
    TEST_CHECK(pIObserved->pVTbl->GetValue(pIObserved, 0, &value) ==
        ERR_ECO_SUCCESS && TestNear(value, trace.snapshots[3],
        0.000000001), 905);
    TEST_CHECK(pIBase->pVTbl->GetValue(pIBase, 0, &value) ==
        ERR_ECO_SUCCESS && TestNear(value, trace.snapshots[3],
        0.000000001), 906);
    pIObserved->pVTbl->Release(pIObserved);
    pIObserved = 0;
    result = pIAI->pVTbl->CreateVector(pIAI, 1, &zero, &pIZero);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIZero != 0, 907);
    InitTrace(&trace);
    lossContext.calls = 0;
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIZero,
        QuadraticLoss, &lossContext, 0.1, 3, 0.00001, 0.000001,
        TraceWeights, &trace, &pIObserved, &observedIterations,
        &observedConverged, &observedLoss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIObserved != 0 &&
        observedIterations == 0 && observedConverged &&
        observedLoss == 0.0 && trace.calls == 1 &&
        trace.snapshots[0] == 0.0 && lossContext.calls == 3, 908);
    pIObserved->pVTbl->Release(pIObserved);
    pIObserved = 0;
    lossContext.calls = 0;
    pIObserved = pIInitial;
    observedIterations = 77;
    observedConverged = 1;
    observedLoss = 77.0;
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIInitial,
        QuadraticLoss, &lossContext, 0.1, 3, 0.00001, 0.0,
        0, &trace, &pIObserved, &observedIterations,
        &observedConverged, &observedLoss);
    TEST_CHECK(result == ERR_ECO_POINTER && pIObserved == 0 &&
        observedIterations == 0 && !observedConverged &&
        observedLoss == 0.0 && lossContext.calls == 0, 909);
    InitTrace(&trace);
    trace.failAt = 1;
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIInitial,
        QuadraticLoss, &lossContext, 0.1, 3, 0.00001, 0.0,
        TraceWeights, &trace, &pIObserved, &observedIterations,
        &observedConverged, &observedLoss);
    TEST_CHECK(result == trace.failCode && pIObserved == 0 &&
        trace.calls == 1 && lossContext.calls == 0, 910);
    InitTrace(&trace);
    trace.failAt = 2;
    lossContext.calls = 0;
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIInitial,
        QuadraticLoss, &lossContext, 0.1, 3, 0.00001, 0.0,
        TraceWeights, &trace, &pIObserved, &observedIterations,
        &observedConverged, &observedLoss);
    TEST_CHECK(result == trace.failCode && pIObserved == 0 &&
        trace.calls == 2 && lossContext.calls == 2 &&
        observedIterations == 0 && !observedConverged &&
        observedLoss == 0.0, 911);
    TEST_CHECK(pIInitial->pVTbl->GetValue(pIInitial, 0, &value) ==
        ERR_ECO_SUCCESS && value == initial, 912);
    lossContext.calls = 0;
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIInitial,
        QuadraticLoss, &lossContext, 0.1, 1, 0.00001, 0.0,
        TraceWithoutContext, 0, &pIObserved, &observedIterations,
        &observedConverged, &observedLoss);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIObserved != 0 &&
        observedIterations == 1 && lossContext.calls == 3, 913);
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIObserved != 0 && pIObserved != pIInitial) {
        pIObserved->pVTbl->Release(pIObserved);
    }
    if (pIBase != 0) {
        pIBase->pVTbl->Release(pIBase);
    }
    if (pIZero != 0) {
        pIZero->pVTbl->Release(pIZero);
    }
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

typedef struct FakeVector {
    IEcoVectorVTbl* pVTbl;
    uint32_t count;
    int16_t valueError;
    double_t value;
} FakeVector;

static int16_t ECOCALLMETHOD FakeVector_GetSize(IEcoVector* me,
    uint32_t* outCount) {
    *outCount = ((FakeVector*)me)->count;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD FakeVector_GetValue(IEcoVector* me,
    uint32_t index, double_t* outValue) {
    FakeVector* pFake = (FakeVector*)me;
    (void)index;
    if (pFake->valueError != ERR_ECO_SUCCESS) {
        return pFake->valueError;
    }
    *outValue = pFake->value;
    return ERR_ECO_SUCCESS;
}

static IEcoVectorVTbl g_FakeVectorVTbl = {
    0, 0, 0, FakeVector_GetSize, FakeVector_GetValue, 0
};

static int16_t TestFailures(IEcoArtificialIntelligence* pIAI,
    double_t infinity, double_t notNumber) {
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    FakeVector fake;
    LossContext context;
    double_t initial;
    double_t loss;
    double_t value;
    uint32_t iterations;
    bool_t converged;
    int16_t result;
    initial = 1.0;
    InitLoss(&context, 1);
    result = pIAI->pVTbl->CreateVector(pIAI, 1, &initial, &pIInitial);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 601);
    pIResult = (IEcoVector*)pIInitial;
    iterations = 77;
    converged = 1;
    loss = 77.0;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, 0, &context,
        0.1, 10, 0.00001, 0.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECO_POINTER && pIResult == 0 &&
        iterations == 0 && !converged && loss == 0.0 && context.calls == 0, 602);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.0, 10, 0.00001, 0.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_INVALID_ARGUMENT && context.calls == 0, 603);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 0, 0.00001, 0.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_INVALID_ARGUMENT && context.calls == 0, 604);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 10, -0.00001, 0.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_INVALID_ARGUMENT && context.calls == 0, 605);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 10, 0.00001, -1.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_INVALID_ARGUMENT && context.calls == 0, 606);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        infinity, 10, 0.00001, 0.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NONFINITE && context.calls == 0, 607);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 10, notNumber, 0.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NONFINITE && context.calls == 0, 608);
    result = pIAI->pVTbl->Optimize(pIAI, 0, QuadraticLoss, &context,
        0.1, 10, 0.00001, 0.0, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECO_POINTER && context.calls == 0, 620);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 10, 0.00001, 0.0, 0, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECO_POINTER && context.calls == 0, 621);
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss, &context,
        0.1, 10, 0.00001, infinity, &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NONFINITE && context.calls == 0, 622);
    fake.pVTbl = &g_FakeVectorVTbl;
    fake.count = 0;
    fake.valueError = ERR_ECO_SUCCESS;
    fake.value = 1.0;
    result = pIAI->pVTbl->Optimize(pIAI, (IEcoVector*)&fake,
        QuadraticLoss, &context, 0.1, 10, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_INVALID_ARGUMENT && context.calls == 0, 609);
    fake.count = (uint32_t)~(uint32_t)0;
    result = pIAI->pVTbl->Optimize(pIAI, (IEcoVector*)&fake,
        QuadraticLoss, &context, 0.1, 10, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_INVALID_ARGUMENT && context.calls == 0, 610);
    fake.count = 1;
    fake.valueError = 1702;
    result = pIAI->pVTbl->Optimize(pIAI, (IEcoVector*)&fake,
        QuadraticLoss, &context, 0.1, 10, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == 1702 && context.calls == 0, 611);
    fake.valueError = ERR_ECO_SUCCESS;
    fake.value = notNumber;
    result = pIAI->pVTbl->Optimize(pIAI, (IEcoVector*)&fake,
        QuadraticLoss, &context, 0.1, 10, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NONFINITE && context.calls == 0, 612);
    context.failAt = 3;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss,
        &context, 0.1, 1, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == context.failCode && context.calls == 3, 613);
    TEST_CHECK(pIResult == 0 && iterations == 0 && !converged && loss == 0.0, 614);
    context.failAt = 0;
    context.calls = 0;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss,
        &context, 1.0e308, 1, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NONFINITE && pIResult == 0 &&
        iterations == 0 && !converged && loss == 0.0, 623);
    context.failAt = 0;
    context.calls = 0;
    context.returnSpecial = 1;
    context.specialLoss = infinity;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss,
        &context, 0.1, 10, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NONFINITE && context.calls == 1, 615);
    context.specialLoss = notNumber;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss,
        &context, 0.1, 10, 0.00001, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NONFINITE, 616);
    context.returnSpecial = 0;
    context.calls = 0;
    result = pIAI->pVTbl->Optimize(pIAI, pIInitial, QuadraticLoss,
        &context, 0.1, 10, 1.0e-30, 0.0,
        &pIResult, &iterations, &converged, &loss);
    TEST_CHECK(result == ERR_ECOLAB1_NUMERIC_ERROR && context.calls == 0, 617);
    TEST_CHECK(pIInitial->pVTbl->GetValue(pIInitial, 0, &value) ==
        ERR_ECO_SUCCESS && value == initial, 618);
    TEST_CHECK(pIResult == 0 && iterations == 0 && !converged && loss == 0.0, 619);
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

typedef struct FakeAllocator {
    IEcoMemoryAllocator1VTbl* pVTbl;
    IEcoMemoryAllocator1* m_parent;
    uint32_t m_refCount;
    uint32_t m_allocations;
    uint32_t m_failAt;
    uint32_t m_outstanding;
} FakeAllocator;

static int16_t ECOCALLMETHOD FakeAllocator_Query(IEcoMemoryAllocator1* me,
    const UGUID* riid, voidptr_t* ppv) {
    (void)me;
    (void)riid;
    if (ppv != 0) {
        *ppv = 0;
    }
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD FakeAllocator_AddRef(IEcoMemoryAllocator1* me) {
    return ++((FakeAllocator*)me)->m_refCount;
}

static uint32_t ECOCALLMETHOD FakeAllocator_Release(IEcoMemoryAllocator1* me) {
    return --((FakeAllocator*)me)->m_refCount;
}

static voidptr_t ECOCALLMETHOD FakeAllocator_Alloc(IEcoMemoryAllocator1* me,
    uint32_t size) {
    FakeAllocator* pFake = (FakeAllocator*)me;
    voidptr_t memory;
    ++pFake->m_allocations;
    if (pFake->m_allocations == pFake->m_failAt) {
        return 0;
    }
    memory = pFake->m_parent->pVTbl->Alloc(pFake->m_parent, size);
    if (memory != 0) {
        ++pFake->m_outstanding;
    }
    return memory;
}

static void ECOCALLMETHOD FakeAllocator_Free(IEcoMemoryAllocator1* me,
    voidptr_t memory) {
    FakeAllocator* pFake = (FakeAllocator*)me;
    if (memory != 0) {
        --pFake->m_outstanding;
    }
    pFake->m_parent->pVTbl->Free(pFake->m_parent, memory);
}

static IEcoMemoryAllocator1VTbl g_FakeAllocatorVTbl = {
    FakeAllocator_Query,
    FakeAllocator_AddRef,
    FakeAllocator_Release,
    FakeAllocator_Alloc,
    FakeAllocator_Free,
    0, 0, 0, 0, 0
};

static int16_t TestAllocatorFailures(IEcoArtificialIntelligence* pIAI,
    IEcoOptimizationTrace* pITrace, IEcoLab1* pILegacy,
    IEcoMemoryAllocator1* pIMem) {
    CEcoLab1_9907DB59* pCMe = (CEcoLab1_9907DB59*)pILegacy;
    FakeAllocator fake;
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    LossContext context;
    TraceContext trace;
    double_t initial;
    double_t loss;
    double_t value;
    uint32_t iterations;
    uint32_t failAt;
    bool_t converged;
    int16_t result;
    initial = 1.0;
    InitLoss(&context, 1);
    fake.pVTbl = &g_FakeAllocatorVTbl;
    fake.m_parent = pIMem;
    fake.m_refCount = 1;
    fake.m_allocations = 0;
    fake.m_failAt = 1;
    fake.m_outstanding = 0;
    result = EcoLab1CreateVector((IEcoMemoryAllocator1*)&fake, 1,
        &initial, &pIResult);
    TEST_CHECK(result == ERR_ECO_OUTOFMEMORY && pIResult == 0 &&
        fake.m_outstanding == 0, 701);
    fake.m_allocations = 0;
    fake.m_failAt = 2;
    result = EcoLab1CreateVector((IEcoMemoryAllocator1*)&fake, 1,
        &initial, &pIResult);
    TEST_CHECK(result == ERR_ECO_OUTOFMEMORY && pIResult == 0 &&
        fake.m_outstanding == 0, 702);
    result = pIAI->pVTbl->CreateVector(pIAI, 1, &initial, &pIInitial);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 703);
    for (failAt = 1; failAt <= 5; ++failAt) {
        fake.m_allocations = 0;
        fake.m_failAt = failAt;
        fake.m_outstanding = 0;
        iterations = 77;
        converged = 1;
        loss = 77.0;
        pIResult = (IEcoVector*)pIInitial;
        context.calls = 0;
        pCMe->m_pIMem = (IEcoMemoryAllocator1*)&fake;
        result = pIAI->pVTbl->Optimize(pIAI, pIInitial,
            QuadraticLoss, &context, 0.1, 1, 0.00001, 0.0,
            &pIResult, &iterations, &converged, &loss);
        pCMe->m_pIMem = pIMem;
        TEST_CHECK(result == ERR_ECO_OUTOFMEMORY && pIResult == 0, 704);
        TEST_CHECK(iterations == 0 && !converged && loss == 0.0, 705);
        TEST_CHECK(fake.m_outstanding == 0 && fake.m_refCount == 1, 706);
        TEST_CHECK(pIInitial->pVTbl->GetValue(pIInitial, 0, &value) ==
            ERR_ECO_SUCCESS && value == initial, 707);
    }
    InitTrace(&trace);
    trace.failAt = 2;
    context.calls = 0;
    fake.m_allocations = 0;
    fake.m_failAt = 0;
    fake.m_outstanding = 0;
    pCMe->m_pIMem = (IEcoMemoryAllocator1*)&fake;
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIInitial,
        QuadraticLoss, &context, 0.1, 1, 0.00001, 0.0,
        TraceWeights, &trace, &pIResult, &iterations,
        &converged, &loss);
    pCMe->m_pIMem = pIMem;
    TEST_CHECK(result == trace.failCode && pIResult == 0 &&
        trace.calls == 2 && context.calls == 2 &&
        fake.m_outstanding == 0 && fake.m_refCount == 1, 708);
    result = ERR_ECO_SUCCESS;
Cleanup:
    pCMe->m_pIMem = pIMem;
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

/* Eco System provides the executable entry point and propagates this status. */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoLab1* pILegacy = 0;
    IEcoLab1* pILegacyAgain = 0;
    IEcoArtificialIntelligence* pIAI = 0;
    IEcoArtificialIntelligence* pIAIDirect = 0;
    IEcoArtificialIntelligence* pIAIFromTrace = 0;
    IEcoOptimizationTrace* pITrace = 0;
    IEcoOptimizationTrace* pITraceDirect = 0;
    IEcoLab1* pILegacyFromTrace = 0;
    IEcoUnknown* pICanonical1 = 0;
    IEcoUnknown* pICanonical2 = 0;
    IEcoUnknown* pICanonicalTrace = 0;
    IEcoComponentFactory* pIFactory;
    voidptr_t invalidInterface;
    char_t legacyName[4];
    char_t* legacyCopy;
    double_t infinity;
    double_t notNumber;
    volatile double_t huge;
    uint32_t refCount;
    int16_t result;
#ifdef LAB76_INJECT_FAILURE
    return 77;
#endif
    if (pIUnk == 0) {
        return ERR_ECO_POINTER;
    }
    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem,
        (voidptr_t*)&pISys);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pISys != 0, 801);
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&pIBus);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIBus != 0, 802);
    pIFactory = GetIEcoComponentFactoryPtr_72A5C4F19CE74A6B8F1EEE6C9907DB59;
    TEST_CHECK(pIFactory != 0, 803);
    invalidInterface = (voidptr_t)pIFactory;
    TEST_CHECK(pIFactory->pVTbl->QueryInterface(pIFactory, 0,
        &invalidInterface) == ERR_ECO_POINTER && invalidInterface == 0, 822);
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1,
        (IEcoUnknown*)pIFactory);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 804);
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1,
        0, &IID_IEcoMemoryAllocator1, (voidptr_t*)&pIMem);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIMem != 0, 805);
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1,
        0, &IID_IEcoLab1, (voidptr_t*)&pILegacy);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pILegacy != 0, 806);
    legacyName[0] = 'L';
    legacyName[1] = '7';
    legacyName[2] = '6';
    legacyName[3] = 0;
    legacyCopy = 0;
    TEST_CHECK(pILegacy->pVTbl->MyFunction(pILegacy, legacyName,
        &legacyCopy) == ERR_ECO_SUCCESS && legacyCopy != 0 &&
        legacyCopy[0] == 'L' && legacyCopy[1] == '7' &&
        legacyCopy[2] == '6' && legacyCopy[3] == 0, 823);
    invalidInterface = (voidptr_t)pILegacy;
    TEST_CHECK(pILegacy->pVTbl->QueryInterface(pILegacy, &CID_EcoLab1,
        &invalidInterface) == ERR_ECO_NOINTERFACE &&
        invalidInterface == 0, 824);
    invalidInterface = (voidptr_t)pILegacy;
    TEST_CHECK(pILegacy->pVTbl->QueryInterface(pILegacy, 0,
        &invalidInterface) == ERR_ECO_POINTER && invalidInterface == 0, 825);
    invalidInterface = (voidptr_t)pILegacy;
    TEST_CHECK(pIFactory->pVTbl->Alloc(pIFactory, pIUnk, pIUnk,
        &IID_IEcoArtificialIntelligence, &invalidInterface) ==
        ERR_ECO_NOAGGREGATION && invalidInterface == 0, 826);
    invalidInterface = (voidptr_t)pILegacy;
    TEST_CHECK(pIFactory->pVTbl->Alloc(pIFactory, pIUnk, 0,
        &CID_EcoLab1, &invalidInterface) == ERR_ECO_NOINTERFACE &&
        invalidInterface == 0, 827);
    result = pILegacy->pVTbl->QueryInterface(pILegacy,
        &IID_IEcoArtificialIntelligence, (voidptr_t*)&pIAI);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIAI != 0, 807);
    result = pIAI->pVTbl->QueryInterface(pIAI, &IID_IEcoLab1,
        (voidptr_t*)&pILegacyAgain);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pILegacyAgain == pILegacy, 808);
    result = pILegacy->pVTbl->QueryInterface(pILegacy, &IID_IEcoUnknown,
        (voidptr_t*)&pICanonical1);
    TEST_CHECK(result == ERR_ECO_SUCCESS, 809);
    result = pIAI->pVTbl->QueryInterface(pIAI, &IID_IEcoUnknown,
        (voidptr_t*)&pICanonical2);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pICanonical2 == pICanonical1, 810);
    refCount = pIAI->pVTbl->AddRef(pIAI);
    TEST_CHECK(pIAI->pVTbl->Release(pIAI) + 1 == refCount, 811);
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1,
        0, &IID_IEcoArtificialIntelligence, (voidptr_t*)&pIAIDirect);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIAIDirect != 0, 812);
    result = pIAI->pVTbl->QueryInterface(pIAI,
        &IID_IEcoOptimizationTrace, (voidptr_t*)&pITrace);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pITrace != 0, 830);
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1,
        0, &IID_IEcoOptimizationTrace, (voidptr_t*)&pITraceDirect);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pITraceDirect != 0, 831);
    result = pITrace->pVTbl->QueryInterface(pITrace,
        &IID_IEcoArtificialIntelligence, (voidptr_t*)&pIAIFromTrace);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pIAIFromTrace == pIAI, 832);
    result = pITrace->pVTbl->QueryInterface(pITrace,
        &IID_IEcoLab1, (voidptr_t*)&pILegacyFromTrace);
    TEST_CHECK(result == ERR_ECO_SUCCESS && pILegacyFromTrace == pILegacy, 833);
    result = pITrace->pVTbl->QueryInterface(pITrace,
        &IID_IEcoUnknown, (voidptr_t*)&pICanonicalTrace);
    TEST_CHECK(result == ERR_ECO_SUCCESS &&
        pICanonicalTrace == pICanonical1, 834);
    refCount = pITrace->pVTbl->AddRef(pITrace);
    TEST_CHECK(pITrace->pVTbl->Release(pITrace) + 1 == refCount, 835);
    huge = 1.0e308;
    infinity = huge * huge;
    notNumber = infinity - infinity;
    TEST_CHECK(TestVector(pIAI, infinity, notNumber) == ERR_ECO_SUCCESS, 813);
    TEST_CHECK(TestQuadratic(pIAI, 1) == ERR_ECO_SUCCESS, 814);
    TEST_CHECK(TestQuadratic(pIAI, 2) == ERR_ECO_SUCCESS, 815);
    TEST_CHECK(TestQuadratic(pIAI, 4) == ERR_ECO_SUCCESS, 816);
    TEST_CHECK(TestMSEAndReplacement(pIAI) == ERR_ECO_SUCCESS, 817);
    TEST_CHECK(TestStopping(pIAI) == ERR_ECO_SUCCESS, 818);
    TEST_CHECK(TestSimultaneous(pIAI) == ERR_ECO_SUCCESS, 819);
    TEST_CHECK(TestFailures(pIAI, infinity, notNumber) == ERR_ECO_SUCCESS, 820);
    TEST_CHECK(TestAllocatorFailures(pIAI, pITrace, pILegacy, pIMem) ==
        ERR_ECO_SUCCESS, 821);
    TEST_CHECK(TestTrace(pIAI, pITrace) == ERR_ECO_SUCCESS, 836);
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pICanonicalTrace != 0) {
        pICanonicalTrace->pVTbl->Release(pICanonicalTrace);
    }
    if (pILegacyFromTrace != 0) {
        pILegacyFromTrace->pVTbl->Release(pILegacyFromTrace);
    }
    if (pIAIFromTrace != 0) {
        pIAIFromTrace->pVTbl->Release(pIAIFromTrace);
    }
    if (pITraceDirect != 0) {
        pITraceDirect->pVTbl->Release(pITraceDirect);
    }
    if (pITrace != 0) {
        pITrace->pVTbl->Release(pITrace);
    }
    if (pIAIDirect != 0) {
        pIAIDirect->pVTbl->Release(pIAIDirect);
    }
    if (pICanonical2 != 0) {
        pICanonical2->pVTbl->Release(pICanonical2);
    }
    if (pICanonical1 != 0) {
        pICanonical1->pVTbl->Release(pICanonical1);
    }
    if (pILegacyAgain != 0) {
        pILegacyAgain->pVTbl->Release(pILegacyAgain);
    }
    if (pIAI != 0) {
        pIAI->pVTbl->Release(pIAI);
    }
    if (pILegacy != 0) {
        pILegacy->pVTbl->Release(pILegacy);
    }
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }
    return result;
}
