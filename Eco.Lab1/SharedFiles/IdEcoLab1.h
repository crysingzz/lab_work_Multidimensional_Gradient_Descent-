/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IdEcoLab1
 * </summary>
 *
 * <description>
 *   This header describes the interface IdEcoLab1
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

#ifndef __ID_ECOLAB1_H__
#define __ID_ECOLAB1_H__

#include "IEcoBase1.h"
#include "IEcoLab1.h"

/* EcoLab1 CID = {72A5C4F1-9CE7-4A6B-8F1E-EE6C9907DB59} */
#ifndef __CID_EcoLab1
static const UGUID CID_EcoLab1 = {0x01, 0x10, {0x72, 0xA5, 0xC4, 0xF1, 0x9C, 0xE7, 0x4A, 0x6B, 0x8F, 0x1E, 0xEE, 0x6C, 0x99, 0x07, 0xDB, 0x59}};
#endif /* __CID_EcoLab1 */

/* Component factory for dynamic and static layout */
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_72A5C4F19CE74A6B8F1EEE6C9907DB59;
#endif

#endif /* __ID_ECOLAB1_H__ */

