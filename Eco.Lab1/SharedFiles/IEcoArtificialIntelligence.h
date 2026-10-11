/*
 * <character encoding>UTF-8 with signature</character encoding>
 * <summary>IEcoArtificialIntelligence</summary>
 * <description>Public ACOM gradient-descent optimizer.</description>
 * <reference>IEcoArtificialIntelligence.idl</reference>
 * <author>Eco.Lab1 contributors</author>
 */

#ifndef __I_ECO_ARTIFICIAL_INTELLIGENCE_H__
#define __I_ECO_ARTIFICIAL_INTELLIGENCE_H__

#include "IEcoVector.h"

/* IEcoArtificialIntelligence IID = {D4366E09-1671-4880-B8BC-14E842996252} */
#ifndef __IID_IEcoArtificialIntelligence
static const UGUID IID_IEcoArtificialIntelligence = {0x01, 0x10,
    {0xD4, 0x36, 0x6E, 0x09, 0x16, 0x71, 0x48, 0x80,
     0xB8, 0xBC, 0x14, 0xE8, 0x42, 0x99, 0x62, 0x52}};
#endif

typedef struct IEcoArtificialIntelligence* IEcoArtificialIntelligencePtr_t;

/* Synchronous loss callback. Context and weights are borrowed for this call only. */
typedef int16_t (ECOCALLMETHOD *EcoLossFunction)(/* in */ const double_t* weights,
    /* in */ uint32_t count, /* in */ voidptr_t context, /* out */ double_t* outLoss);

typedef struct IEcoArtificialIntelligenceVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(
        /* in */ IEcoArtificialIntelligencePtr_t me, /* in */ const UGUID* riid,
        /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoArtificialIntelligencePtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoArtificialIntelligencePtr_t me);

    /* Creates a copy of finite values; caller owns the returned reference. */
    int16_t (ECOCALLMETHOD *CreateVector)(/* in */ IEcoArtificialIntelligencePtr_t me,
        /* in */ uint32_t count, /* in */ const double_t* values,
        /* out */ IEcoVector** outVector);

    /* Returns a new vector only on success. The initial vector is never modified. */
    int16_t (ECOCALLMETHOD *Optimize)(/* in */ IEcoArtificialIntelligencePtr_t me,
        /* in */ IEcoVector* initial, /* in */ EcoLossFunction loss,
        /* in */ voidptr_t context, /* in */ double_t learningRate,
        /* in */ uint32_t maxIterations, /* in */ double_t differenceStep,
        /* in */ double_t gradientTolerance, /* out */ IEcoVector** outWeights,
        /* out */ uint32_t* outIterations, /* out */ bool_t* outConverged,
        /* out */ double_t* outLoss);
} IEcoArtificialIntelligenceVTbl, *IEcoArtificialIntelligenceVTblPtr_t;

interface IEcoArtificialIntelligence {
    struct IEcoArtificialIntelligenceVTbl* pVTbl;
} IEcoArtificialIntelligence;

#endif /* __I_ECO_ARTIFICIAL_INTELLIGENCE_H__ */
