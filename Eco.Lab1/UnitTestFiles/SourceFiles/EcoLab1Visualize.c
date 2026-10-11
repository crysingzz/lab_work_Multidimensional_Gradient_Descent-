/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>EcoLab1Visualize</summary>
 * <description>Console demonstration of observed gradient descent.</description>
 * <reference>openspec/changes/add-lab76-console-visualization</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include "IEcoArtificialIntelligence.h"
#include "IEcoOptimizationTrace.h"

#define DEMO_STEPS 6
#define DEMO_ROWS (DEMO_STEPS + 1)
#define DEMO_MAX_WEIGHTS 10
#define DEMO_TEXT_CAPACITY 8192

typedef struct DemoLossContext {
    uint32_t kind;
    uint32_t calls;
} DemoLossContext;

typedef struct DemoTrace {
    uint32_t count;
    uint32_t rows;
    double_t weights[DEMO_ROWS][DEMO_MAX_WEIGHTS];
} DemoTrace;

typedef struct DemoText {
    char_t data[DEMO_TEXT_CAPACITY];
    uint32_t length;
    bool_t failed;
} DemoText;

static double_t AbsValue(double_t value) {
    return value < 0.0 ? -value : value;
}

/* Both the optimizer callback and display use this exact loss formula. */
static int16_t CalculateLoss(uint32_t kind, const double_t* weights,
    uint32_t count, double_t* outLoss) {
    uint32_t index;
    double_t residual;
    double_t sum;
    if (weights == 0 || outLoss == 0 || count != kind) {
        return 41;
    }
    if (kind == 1) {
        *outLoss = weights[0] * weights[0];
        return ERR_ECO_SUCCESS;
    }
    sum = 0.0;
    if (kind == 2) {
        for (index = 0; index < 4; ++index) {
            residual = weights[0] + weights[1] * (double_t)index -
                (1.0 + 2.0 * (double_t)index);
            sum += residual * residual;
        }
        *outLoss = sum / 4.0;
        return ERR_ECO_SUCCESS;
    }
    if (kind != 5 && kind != 10) {
        return 42;
    }
    for (index = 0; index < count; ++index) {
        residual = weights[index] - (double_t)index;
        sum += residual * residual;
    }
    *outLoss = sum;
    return ERR_ECO_SUCCESS;
}

static int16_t ECOCALLMETHOD DemoLoss(const double_t* weights,
    uint32_t count, voidptr_t context, double_t* outLoss) {
    DemoLossContext* pContext = (DemoLossContext*)context;
    if (pContext == 0) {
        return 43;
    }
    ++pContext->calls;
    return CalculateLoss(pContext->kind, weights, count, outLoss);
}

static int16_t ECOCALLMETHOD SaveSnapshot(const double_t* weights,
    uint32_t count, uint32_t completedUpdates, voidptr_t context) {
    DemoTrace* pTrace = (DemoTrace*)context;
    uint32_t index;
    if (pTrace == 0 || weights == 0 || count != pTrace->count ||
        count > DEMO_MAX_WEIGHTS ||
        completedUpdates != pTrace->rows || pTrace->rows >= DEMO_ROWS) {
        return 44;
    }
    for (index = 0; index < count; ++index) {
        pTrace->weights[pTrace->rows][index] = weights[index];
    }
    ++pTrace->rows;
    return ERR_ECO_SUCCESS;
}

static void AppendChar(DemoText* pText, char_t value) {
    if (pText->length + 1 >= DEMO_TEXT_CAPACITY) {
        pText->failed = 1;
        return;
    }
    pText->data[pText->length] = value;
    ++pText->length;
}

static void AppendText(DemoText* pText, const char_t* value) {
    uint32_t index;
    for (index = 0; value[index] != 0; ++index) {
        AppendChar(pText, value[index]);
    }
}

static void AppendUnsigned(DemoText* pText, uint32_t value) {
    char_t reversed[10];
    uint32_t count;
    count = 0;
    do {
        reversed[count] = (char_t)('0' + value % 10);
        value /= 10;
        ++count;
    } while (value != 0);
    while (count != 0) {
        --count;
        AppendChar(pText, reversed[count]);
    }
}

/* The fixed examples stay in a small range; six decimals are for display only. */
static void AppendFixed6(DemoText* pText, double_t value) {
    uint32_t scaled;
    uint32_t fraction;
    uint32_t divisor;
    if (value < 0.0) {
        AppendChar(pText, '-');
        value = -value;
    }
    if (value > 4000.0) {
        pText->failed = 1;
        return;
    }
    scaled = (uint32_t)(value * 1000000.0 + 0.5);
    AppendUnsigned(pText, scaled / 1000000);
    AppendChar(pText, '.');
    fraction = scaled % 1000000;
    divisor = 100000;
    while (divisor != 0) {
        AppendChar(pText, (char_t)('0' + fraction / divisor));
        fraction %= divisor;
        divisor /= 10;
    }
}

/* Keep every nonnegative loss right-aligned in a ten-character column. */
static void AppendAlignedLoss(DemoText* pText, double_t loss) {
    if (loss < 10.0) {
        AppendText(pText, "  ");
    }
    else if (loss < 100.0) {
        AppendChar(pText, ' ');
    }
    AppendFixed6(pText, loss);
}

static int16_t AppendExample(DemoText* pText, const DemoTrace* pTrace,
    uint32_t kind) {
    double_t initialLoss;
    double_t loss;
    double_t previousLoss;
    uint32_t row;
    uint32_t barLength;
    uint32_t index;
    uint32_t firstWeight;
    uint32_t lastWeight;
    int16_t result;
    if (kind == 1) {
        AppendText(pText, "Quadratic L(w)=w^2 | n=1 | eta=0.1\n");
    }
    else if (kind == 2) {
        AppendText(pText, "MSE y=w0+w1*x    | n=2 | eta=0.03\n");
    }
    else {
        AppendText(pText, "Separable quadratic sum((w_i-i)^2) | n=");
        AppendUnsigned(pText, kind);
        AppendText(pText, kind == 5 ? " | eta=0.08\n" : " | eta=0.05\n");
    }
    AppendText(pText, "step |       loss | relative loss\n");
    AppendText(pText, "-----+------------+------------------------------\n");
    result = CalculateLoss(kind, pTrace->weights[0], pTrace->count,
        &initialLoss);
    if (result != ERR_ECO_SUCCESS || initialLoss <= 0.0) {
        return 45;
    }
    previousLoss = initialLoss;
    for (row = 0; row < pTrace->rows; ++row) {
        result = CalculateLoss(kind, pTrace->weights[row], pTrace->count,
            &loss);
        if (result != ERR_ECO_SUCCESS || loss < 0.0 ||
            loss > previousLoss + 0.000000001) {
            return 46;
        }
        AppendText(pText, "   ");
        AppendUnsigned(pText, row);
        AppendText(pText, " | ");
        AppendAlignedLoss(pText, loss);
        AppendText(pText, " | ");
        barLength = (uint32_t)(30.0 * loss / initialLoss + 0.5);
        if (barLength > 30) {
            barLength = 30;
        }
        for (index = 0; index < barLength; ++index) {
            AppendChar(pText, '#');
        }
        AppendChar(pText, '\n');
        for (firstWeight = 0; firstWeight < pTrace->count;
            firstWeight += 5) {
            lastWeight = firstWeight + 5;
            if (lastWeight > pTrace->count) {
                lastWeight = pTrace->count;
            }
            AppendText(pText, "     | w[");
            AppendUnsigned(pText, firstWeight);
            if (lastWeight - firstWeight > 1) {
                AppendText(pText, "..");
                AppendUnsigned(pText, lastWeight - 1);
            }
            AppendText(pText, "] = ");
            for (index = firstWeight; index < lastWeight; ++index) {
                if (index != firstWeight) {
                    AppendText(pText, "  ");
                }
                AppendFixed6(pText, pTrace->weights[row][index]);
            }
            AppendChar(pText, '\n');
        }
        previousLoss = loss;
    }
    AppendText(pText, "Bar scale: initial loss = 30 #; values are rounded to 6 decimals.\n\n");
    return pText->failed ? 47 : ERR_ECO_SUCCESS;
}

static int16_t RunExample(IEcoArtificialIntelligence* pIAI,
    IEcoOptimizationTrace* pITrace, DemoText* pText, uint32_t kind) {
    IEcoVector* pIInitial = 0;
    IEcoVector* pIResult = 0;
    DemoLossContext lossContext;
    DemoTrace trace;
    double_t initial[DEMO_MAX_WEIGHTS];
    double_t resultWeight;
    double_t finalLoss;
    double_t displayLoss;
    double_t expected;
    double_t factor;
    double_t power;
    double_t learningRate;
    uint32_t count;
    uint32_t index;
    uint32_t row;
    uint32_t iterations;
    bool_t converged;
    int16_t result;
    count = kind;
    if (kind != 1 && kind != 2 && kind != 5 && kind != 10) {
        return 65;
    }
    for (index = 0; index < count; ++index) {
        initial[index] = kind == 1 ? 2.0 :
            (kind == 2 ? 0.0 : 2.0 * (double_t)index + 1.0);
    }
    learningRate = kind == 1 ? 0.1 :
        (kind == 2 ? 0.03 : (kind == 5 ? 0.08 : 0.05));
    lossContext.kind = kind;
    lossContext.calls = 0;
    trace.count = count;
    trace.rows = 0;
    for (row = 0; row < DEMO_ROWS; ++row) {
        for (index = 0; index < DEMO_MAX_WEIGHTS; ++index) {
            trace.weights[row][index] = 0.0;
        }
    }
    result = pIAI->pVTbl->CreateVector(pIAI, count, initial, &pIInitial);
    if (result != ERR_ECO_SUCCESS || pIInitial == 0) {
        result = 48;
        goto Cleanup;
    }
    result = pITrace->pVTbl->OptimizeObserved(pITrace, pIInitial,
        DemoLoss, &lossContext, learningRate,
        DEMO_STEPS, 0.00001, 0.0,
        SaveSnapshot, &trace, &pIResult, &iterations, &converged, &finalLoss);
    if (result != ERR_ECO_SUCCESS || pIResult == 0 || converged ||
        iterations != DEMO_STEPS || trace.rows != DEMO_ROWS ||
        lossContext.calls != 2 * count * DEMO_STEPS + 1) {
        result = 49;
        goto Cleanup;
    }
    for (index = 0; index < count; ++index) {
        result = pIResult->pVTbl->GetValue(pIResult, index, &resultWeight);
        if (result != ERR_ECO_SUCCESS ||
            AbsValue(resultWeight - trace.weights[DEMO_STEPS][index]) >
                0.000000001) {
            result = 50;
            goto Cleanup;
        }
        result = pIInitial->pVTbl->GetValue(pIInitial, index, &resultWeight);
        if (result != ERR_ECO_SUCCESS || resultWeight != initial[index]) {
            result = 51;
            goto Cleanup;
        }
    }
    result = CalculateLoss(kind, trace.weights[DEMO_STEPS], count,
        &displayLoss);
    if (result != ERR_ECO_SUCCESS || AbsValue(displayLoss - finalLoss) >
        0.000000001) {
        result = 52;
        goto Cleanup;
    }
    if (kind != 2) {
        factor = 1.0 - 2.0 * learningRate;
        power = 1.0;
        for (row = 0; row < DEMO_ROWS; ++row) {
            for (index = 0; index < count; ++index) {
                expected = kind == 1 ? 2.0 * power :
                    (double_t)index + ((double_t)index + 1.0) * power;
                if (AbsValue(trace.weights[row][index] - expected) >
                    0.000001) {
                    result = 53;
                    goto Cleanup;
                }
            }
            power *= factor;
        }
    }
    result = AppendExample(pText, &trace, kind);
Cleanup:
    if (pIResult != 0) {
        pIResult->pVTbl->Release(pIResult);
    }
    if (pIInitial != 0) {
        pIInitial->pVTbl->Release(pIInitial);
    }
    return result;
}

/* Eco System supplies the entry point and propagates this status. */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoArtificialIntelligence* pIAI = 0;
    IEcoOptimizationTrace* pITrace = 0;
    IEcoFileSystemManagement1* pIFS = 0;
    IEcoFileManager1* pIManager = 0;
    IEcoFile1* pIFile = 0;
    DemoText output;
    uint32_t size;
    int16_t result;
    output.length = 0;
    output.failed = 0;
#ifdef LAB76_VISUALIZATION_INJECT_FAILURE
    return 77;
#endif
    if (pIUnk == 0) {
        return 54;
    }
    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem,
        (voidptr_t*)&pISys);
    if (result != ERR_ECO_SUCCESS || pISys == 0) {
        result = 55;
        goto Cleanup;
    }
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1,
        (voidptr_t*)&pIBus);
    if (result != ERR_ECO_SUCCESS || pIBus == 0) {
        result = 56;
        goto Cleanup;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_72A5C4F19CE74A6B8F1EEE6C9907DB59);
    if (result != ERR_ECO_SUCCESS) {
        result = 57;
        goto Cleanup;
    }
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0,
        &IID_IEcoArtificialIntelligence, (voidptr_t*)&pIAI);
    if (result != ERR_ECO_SUCCESS || pIAI == 0) {
        result = 58;
        goto Cleanup;
    }
    result = pIAI->pVTbl->QueryInterface(pIAI, &IID_IEcoOptimizationTrace,
        (voidptr_t*)&pITrace);
    if (result != ERR_ECO_SUCCESS || pITrace == 0) {
        result = 59;
        goto Cleanup;
    }
    AppendText(&output, "LAB 76 | Observed gradient descent\n\n");
    result = RunExample(pIAI, pITrace, &output, 1);
    if (result != ERR_ECO_SUCCESS) {
        goto Cleanup;
    }
    result = RunExample(pIAI, pITrace, &output, 2);
    if (result != ERR_ECO_SUCCESS) {
        goto Cleanup;
    }
    result = RunExample(pIAI, pITrace, &output, 5);
    if (result != ERR_ECO_SUCCESS) {
        goto Cleanup;
    }
    result = RunExample(pIAI, pITrace, &output, 10);
    if (result != ERR_ECO_SUCCESS || output.failed) {
        result = 60;
        goto Cleanup;
    }
    result = pIBus->pVTbl->QueryComponent(pIBus,
        &CID_EcoFileSystemManagement1, 0,
        &IID_IEcoFileSystemManagement1, (voidptr_t*)&pIFS);
    if (result != ERR_ECO_SUCCESS || pIFS == 0) {
        result = 61;
        goto Cleanup;
    }
    pIManager = pIFS->pVTbl->get_FileManager(pIFS);
    if (pIManager == 0) {
        result = 62;
        goto Cleanup;
    }
    pIFile = pIManager->pVTbl->Create(pIManager, "Lab76-visualization.txt");
    if (pIFile == 0) {
        result = 63;
        goto Cleanup;
    }
    size = output.length;
    result = pIFile->pVTbl->Write(pIFile, output.data, &size);
    if (result != ERR_ECO_SUCCESS || size != output.length) {
        result = 64;
        goto Cleanup;
    }
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (pIFile != 0) {
        pIFile->pVTbl->Release(pIFile);
    }
    if (pIManager != 0) {
        pIManager->pVTbl->Release(pIManager);
    }
    if (pIFS != 0) {
        pIFS->pVTbl->Release(pIFS);
    }
    if (pITrace != 0) {
        pITrace->pVTbl->Release(pITrace);
    }
    if (pIAI != 0) {
        pIAI->pVTbl->Release(pIAI);
    }
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }
    return result;
}
