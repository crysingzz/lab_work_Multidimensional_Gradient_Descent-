/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoLab1
 * </summary>
 *
 * <description>
 *   This header describes the interface IEcoLab1
 * </description>
 *
 * <reference>
 *
 * </reference>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __I_ECOLAB1_H__
#define __I_ECOLAB1_H__

#include "IEcoBase1.h"

/* IEcoLab1 IID = {FF78E014-072D-4E61-974E-EC79454EFA8C} */
#ifndef __IID_IEcoLab1
static const UGUID IID_IEcoLab1 = {0x01, 0x10, {0xFF, 0x78, 0xE0, 0x14, 0x07, 0x2D, 0x4E, 0x61, 0x97, 0x4E, 0xEC, 0x79, 0x45, 0x4E, 0xFA, 0x8C}};
#endif /* __IID_IEcoLab1 */

typedef struct IEcoLab1* IEcoLab1Ptr_t;

typedef struct IEcoLab1VTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoLab1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoLab1Ptr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoLab1Ptr_t me);

    /* IEcoLab1 */
    int16_t (ECOCALLMETHOD *MyFunction)(/* in */ IEcoLab1Ptr_t me, /* in */ char_t* Name, /* out */ char_t** CopyName);

} IEcoLab1VTbl, *IEcoLab1VTblPtr_t;

interface IEcoLab1 {
    struct IEcoLab1VTbl *pVTbl;
} IEcoLab1;


#endif /* __I_ECOLAB1_H__ */

