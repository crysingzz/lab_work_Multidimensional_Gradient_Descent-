/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>EcoLab1Benchmark</summary>
 * <description>Matched ACOM and independent C baseline workloads.</description>
 * <reference>openspec/changes/implement-gradient-descent-lab76</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoArtificialIntelligence.h"
#include "IdEcoLab1.h"

#define BENCH_DIMENSION 4
#define BENCH_ITERATIONS 100
#define BENCH_RUNS 100000
#define BENCH_EXPECTED_CALLS \
    (BENCH_RUNS * (2 * BENCH_DIMENSION * BENCH_ITERATIONS + 1))

static volatile uint32_t g_lossCalls = 0;
static volatile double_t g_checksum = 0.0;
static const double_t g_target[BENCH_DIMENSION] = {1.0, -1.0, 2.0, 0.5};
static const double_t g_initial[BENCH_DIMENSION] = {4.0, 3.0, -2.0, 1.0};

static double_t AnalyticWeight(uint32_t index) {
    double_t factor;
    uint32_t iteration;
    factor = 1.0;
    for (iteration = 0; iteration < BENCH_ITERATIONS; ++iteration) {
        factor *= 0.98;
    }
    return g_target[index] + (g_initial[index] - g_target[index]) * factor;
}

static bool_t AccurateWeight(double_t value, uint32_t index) {
    double_t error;
    error = value - AnalyticWeight(index);
    if (error < 0.0) {
        error = -error;
    }
    return (bool_t)(error <= 0.000001);
}

static int16_t ECOCALLMETHOD BenchmarkLoss(const double_t* weights,
    uint32_t count, voidptr_t context, double_t* outLoss) {
    uint32_t i;
    double_t residual;
    double_t sum;
    (void)context;
    if (count != BENCH_DIMENSION || weights == 0 || outLoss == 0) {
        return 1;
    }
    ++g_lossCalls;
    sum = 0.0;
    for (i = 0; i < count; ++i) {
        residual = weights[i] - g_target[i];
        sum += residual * residual;
    }
    *outLoss = sum;
    return ERR_ECO_SUCCESS;
}

/* Independent client-side implementation for this fixed benchmark only. */
#ifdef LAB76_BASELINE
static int16_t BaselineRun(void) {
    static bool_t checked = 0;
    double_t weights[BENCH_DIMENSION];
    double_t trial[BENCH_DIMENSION];
    double_t gradient[BENCH_DIMENSION];
    double_t scale;
    double_t h;
    double_t plusLoss;
    double_t minusLoss;
    double_t loss;
    uint32_t iteration;
    uint32_t i;
    uint32_t j;
    for (i = 0; i < BENCH_DIMENSION; ++i) {
        weights[i] = g_initial[i];
    }
    for (iteration = 0; iteration < BENCH_ITERATIONS; ++iteration) {
        for (i = 0; i < BENCH_DIMENSION; ++i) {
            scale = weights[i] < 0.0 ? -weights[i] : weights[i];
            if (scale < 1.0) {
                scale = 1.0;
            }
            h = 0.00001 * scale;
            for (j = 0; j < BENCH_DIMENSION; ++j) {
                trial[j] = weights[j];
            }
            trial[i] = weights[i] + h;
            if (BenchmarkLoss(trial, BENCH_DIMENSION, 0, &plusLoss) !=
                ERR_ECO_SUCCESS) {
                return 2;
            }
            trial[i] = weights[i] - h;
            if (BenchmarkLoss(trial, BENCH_DIMENSION, 0, &minusLoss) !=
                ERR_ECO_SUCCESS) {
                return 3;
            }
            gradient[i] = (plusLoss - minusLoss) / (2.0 * h);
        }
        for (i = 0; i < BENCH_DIMENSION; ++i) {
            weights[i] -= 0.01 * gradient[i];
        }
    }
    if (BenchmarkLoss(weights, BENCH_DIMENSION, 0, &loss) != ERR_ECO_SUCCESS) {
        return 4;
    }
    if (!checked) {
        for (i = 0; i < BENCH_DIMENSION; ++i) {
            if (!AccurateWeight(weights[i], i)) {
                return 13;
            }
        }
        checked = 1;
    }
    g_checksum += loss;
    return ERR_ECO_SUCCESS;
}
#endif

#ifndef LAB76_BASELINE
static int16_t ComponentRuns(IEcoUnknown* pIUnk) {
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoArtificialIntelligence* pIAI = 0;
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    uint32_t run;
    uint32_t iterations;
    bool_t converged;
    double_t loss;
    double_t weight;
    uint32_t i;
    int16_t result;
    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem,
        (voidptr_t*)&pISys);
    if (result != ERR_ECO_SUCCESS || pISys == 0) {
        return 5;
    }
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&pIBus);
    if (result != ERR_ECO_SUCCESS || pIBus == 0) {
        result = 6;
        goto Cleanup;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_72A5C4F19CE74A6B8F1EEE6C9907DB59);
    if (result != ERR_ECO_SUCCESS) {
        result = 7;
        goto Cleanup;
    }
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0,
        &IID_IEcoArtificialIntelligence, (voidptr_t*)&pIAI);
    if (result != ERR_ECO_SUCCESS || pIAI == 0) {
        result = 8;
        goto Cleanup;
    }
    result = pIAI->pVTbl->CreateVector(pIAI, BENCH_DIMENSION,
        g_initial, &pIInitial);
    if (result != ERR_ECO_SUCCESS) {
        result = 9;
        goto Cleanup;
    }
    for (run = 0; run < BENCH_RUNS; ++run) {
        result = pIAI->pVTbl->Optimize(pIAI, pIInitial, BenchmarkLoss, 0,
            0.01, BENCH_ITERATIONS, 0.00001, 0.0, &pIResult,
            &iterations, &converged, &loss);
        if (result != ERR_ECO_SUCCESS || pIResult == 0 ||
            iterations != BENCH_ITERATIONS || converged) {
            result = 10;
            goto Cleanup;
        }
        if (run == 0) {
            for (i = 0; i < BENCH_DIMENSION; ++i) {
                result = pIResult->pVTbl->GetValue(pIResult, i, &weight);
                if (result != ERR_ECO_SUCCESS || !AccurateWeight(weight, i)) {
                    result = 14;
                    goto Cleanup;
                }
            }
        }
        g_checksum += loss;
        pIResult->pVTbl->Release(pIResult);
        pIResult = 0;
    }
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIResult != 0) {
        pIResult->pVTbl->Release(pIResult);
    }
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    if (pIAI != 0) {
        pIAI->pVTbl->Release(pIAI);
    }
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }
    pISys->pVTbl->Release(pISys);
    return result;
}
#endif

int16_t EcoMain(IEcoUnknown* pIUnk) {
    uint32_t run;
    int16_t result;
    if (pIUnk == 0) {
        return 11;
    }
#ifdef LAB76_BASELINE
    for (run = 0; run < BENCH_RUNS; ++run) {
        result = BaselineRun();
        if (result != ERR_ECO_SUCCESS) {
            return result;
        }
    }
#else
    run = 0;
    result = ComponentRuns(pIUnk);
    if (result != ERR_ECO_SUCCESS) {
        return result;
    }
#endif
    if (g_lossCalls != BENCH_EXPECTED_CALLS || g_checksum <= 0.0) {
        return 12;
    }
    (void)run;
    return ERR_ECO_SUCCESS;
}
