/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoLab1_9907DB59
 * </summary>
 *
 * <description>
 *   This header describes the implementation of the CEcoLab1_9907DB59 component
 * </description>
 *
 * <author>
 *   Copyright (c) 2018 . All rights reserved.
 * </author>
 *
 */

#ifndef __C_ECOLAB1_H__
#define __C_ECOLAB1_H__

#include "IEcoLab1.h"
#include "IEcoArtificialIntelligence.h"
#include "IEcoOptimizationTrace.h"
#include "CEcoVector.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoLab1_9907DB59* CEcoLab1_9907DB59Ptr_t;

typedef struct CEcoLab1AIView {
    IEcoArtificialIntelligenceVTbl* pVTbl;
    CEcoLab1_9907DB59Ptr_t m_owner;
} CEcoLab1AIView;

typedef struct CEcoLab1TraceView {
    IEcoOptimizationTraceVTbl* pVTbl;
    CEcoLab1_9907DB59Ptr_t m_owner;
} CEcoLab1TraceView;

typedef struct CEcoLab1_9907DB59 {

    /* IEcoLab1 interface function table */
    IEcoLab1VTbl* m_pVTblIEcoLab1;
    CEcoLab1AIView m_ai;
    CEcoLab1TraceView m_trace;

    /* Instance initialization */
    int16_t (ECOCALLMETHOD *Init)(/*in*/ CEcoLab1_9907DB59Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);
    /* Instance creation */
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CEcoLab1_9907DB59Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);
    /* Deletion */
    void (ECOCALLMETHOD *Delete)(/*in*/ CEcoLab1_9907DB59Ptr_t pCMe);


    /* Reference counter */
    uint32_t m_cRef;

    /* Interface for memory operations */
    IEcoMemoryAllocator1* m_pIMem;

    /* System interface */
    IEcoSystem1* m_pISys;

    /* Instance data */
    char_t* m_Name;

} CEcoLab1_9907DB59;

#endif /* __C_ECOLAB1_H__ */
