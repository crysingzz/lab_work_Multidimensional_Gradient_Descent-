/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoLab1_9907DB59Factory
 * </summary>
 *
 * <description>
 *   This header describes the implementation of the factory for the component
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"

#include "CEcoLab1.h"
#include "CEcoLab1Factory.h"

extern CEcoLab1_9907DB59 g_xCEcoLab1_9907DB59;

/*
 *
 * <summary>
 *   QueryInterface function
 * </summary>
 *
 * <description>
 *   The function returns a pointer to the interface
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_9907DB59Factory_QueryInterface(IEcoComponentFactory* me, const UGUID* riid, void** ppv) {
    if (ppv != 0) {
        *ppv = 0;
    }
    if (me == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    if ( IsEqualUGUID(riid, &IID_IEcoUnknown) || IsEqualUGUID(riid, &IID_IEcoComponentFactory) ) {
        *ppv = me;
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    ((IEcoUnknown*)(*ppv))->pVTbl->AddRef((IEcoUnknown*)*ppv);

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   AddRef function
 * </summary>
 *
 * <description>
 *   The function increments the reference count for the interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_9907DB59Factory_AddRef(/* in */ IEcoComponentFactory* me) {
    CEcoLab1_9907DB59Factory* pCMe = (CEcoLab1_9907DB59Factory*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   Release function
 * </summary>
 *
 * <description>
 *   The function decrements the reference count for the interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_9907DB59Factory_Release(/* in */ IEcoComponentFactory* me) {
    CEcoLab1_9907DB59Factory* pCMe = (CEcoLab1_9907DB59Factory*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Decrementing the component reference counter */
    --pCMe->m_cRef;

    /* If the counter is zeroed, free the instance data */
    if ( pCMe->m_cRef == 0 ) {
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   Init function
 * </summary>
 *
 * <description>
 *   The function initializes the component with parameters
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_9907DB59Factory_Init(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pIUnkSystem, /* in */ void* pv) {
    if (me == 0 || pIUnkSystem == 0) {
        return ERR_ECO_POINTER;
    }
    (void)pv;
    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   Alloc function
 * </summary>
 *
 * <description>
 *   The function creates a component
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_9907DB59Factory_Alloc(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pISystem, /* in */ struct IEcoUnknown *pIUnknownOuter, /* in */ const UGUID* riid, /* out */ void** ppv) {
    int16_t result = ERR_ECO_POINTER;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoLab1_9907DB59* pCObj = 0;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    if (ppv != 0) {
        *ppv = 0;
    }
    if (me == 0 || pISystem == 0 || riid == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    if (pIUnknownOuter != 0) {
        return ERR_ECO_NOAGGREGATION;
    }

    /* Getting the application system interface */
    result = pISystem->pVTbl->QueryInterface(pISystem, &GID_IEcoSystem, (void **)&pISys);
    /* Check */
    if (result != ERR_ECO_SUCCESS || pISys == 0) {
        return result != ERR_ECO_SUCCESS ? result : ERR_ECO_NOSYSTEM;
    }

    /* Getting the interface for working with the interface bus */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Check */
    if (result != ERR_ECO_SUCCESS || pIBus == 0) {
        result = result != ERR_ECO_SUCCESS ? result : ERR_ECO_NOBUS;
        goto Cleanup;
    }

    /* Getting the component ID for memory operations */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == ERR_ECO_SUCCESS && pIMemExt != 0) {
        UGUID* manager = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        if (manager != 0) {
            rcid = manager;
        }
    }

    /* Getting the memory allocator interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0,
        &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    if (result != ERR_ECO_SUCCESS || pIMem == 0) {
        result = result != ERR_ECO_SUCCESS ? result : ERR_ECO_GET_MEMORY_ALLOCATOR;
        goto Cleanup;
    }

    /* Allocating memory for instance data */
    pCObj = (CEcoLab1_9907DB59*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoLab1_9907DB59));
    if (pCObj == 0) {
        result = ERR_ECO_OUTOFMEMORY;
        goto Cleanup;
    }

    /* Forming instance data */
    if (pIMem->pVTbl->Copy(pIMem, pCObj, &g_xCEcoLab1_9907DB59,
        (uint32_t)sizeof(CEcoLab1_9907DB59)) == 0) {
        pIMem->pVTbl->Free(pIMem, pCObj);
        pCObj = 0;
        result = ERR_ECO_OUTOFMEMORY;
        goto Cleanup;
    }
    pCObj->m_pIMem = pIMem;
    pIMem = 0;

    /* Component creation */
    result = pCObj->Create(pCObj, pISystem, pIUnknownOuter);
    if (result != ERR_ECO_SUCCESS) {
        goto Cleanup;
    }

    /* Component initialization */
    result = pCObj->Init(pCObj, pISystem);
    if (result != ERR_ECO_SUCCESS) {
        goto Cleanup;
    }

    /* Getting a pointer to the interface */
    result = ((IEcoUnknown*)pCObj)->pVTbl->QueryInterface(
        (IEcoUnknown*)pCObj, riid, ppv);

Cleanup:
    if (pCObj != 0) {
        ((IEcoUnknown*)pCObj)->pVTbl->Release((IEcoUnknown*)pCObj);
    }
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }
    if (pIMemExt != 0) {
        pIMemExt->pVTbl->Release(pIMemExt);
    }
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }
    return result;
}
/*
 *
 * <summary>
 *   get_Name function
 * </summary>
 *
 * <description>
 *   The function returns the name of the component
 * </description>
 *
 */
static char_t* ECOCALLMETHOD CEcoLab1_9907DB59Factory_get_Name(/* in */ struct IEcoComponentFactory* me) {
    CEcoLab1_9907DB59Factory* pCMe = (CEcoLab1_9907DB59Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Name;
}

/*
 *
 * <summary>
 *   get_Version function
 * </summary>
 *
 * <description>
 *   The function returns the version of the component
 * </description>
 *
 */
static char_t* ECOCALLMETHOD CEcoLab1_9907DB59Factory_get_Version(/* in */ struct IEcoComponentFactory* me) {
    CEcoLab1_9907DB59Factory* pCMe = (CEcoLab1_9907DB59Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Version;
}

/*
 *
 * <summary>
 *   get_Manufacturer function
 * </summary>
 *
 * <description>
 *   The function returns the name of the component manufacturer
 * </description>
 *
 */
static char_t* ECOCALLMETHOD CEcoLab1_9907DB59Factory_get_Manufacturer(/* in */ struct IEcoComponentFactory* me) {
    CEcoLab1_9907DB59Factory* pCMe = (CEcoLab1_9907DB59Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Manufacturer;
}

/* Create Virtual Table */
IEcoComponentFactoryVTbl g_x72A5C4F19CE74A6B8F1EEE6C9907DB59FactoryVTbl = {
    CEcoLab1_9907DB59Factory_QueryInterface,
    CEcoLab1_9907DB59Factory_AddRef,
    CEcoLab1_9907DB59Factory_Release,
    CEcoLab1_9907DB59Factory_Alloc,
    CEcoLab1_9907DB59Factory_Init,
    CEcoLab1_9907DB59Factory_get_Name,
    CEcoLab1_9907DB59Factory_get_Version,
    CEcoLab1_9907DB59Factory_get_Manufacturer
};

/*
 *
 * <summary>
 *   Create function
 * </summary>
 *
 * <description>
 *   The function 
 * </description>
 *
 */
CEcoLab1_9907DB59Factory g_x72A5C4F19CE74A6B8F1EEE6C9907DB59Factory = {
    &g_x72A5C4F19CE74A6B8F1EEE6C9907DB59FactoryVTbl,
    0,
    "EcoLab1\0",
    "1.0.0.0\0",
    "\0"
};

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr() {
    return (IEcoComponentFactory*)&g_x72A5C4F19CE74A6B8F1EEE6C9907DB59Factory;
};
#elif ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_72A5C4F19CE74A6B8F1EEE6C9907DB59 = (IEcoComponentFactory*)&g_x72A5C4F19CE74A6B8F1EEE6C9907DB59Factory;
#endif
