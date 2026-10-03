/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColAccel
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ColAccel.h"

const char* CColAccel::mp_cCacheName = "MODELS\\CINFO.BIN"; // 0x8D0F84

void CColAccel::InjectHooks() {
    RH_ScopedClass(CColAccel);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(isCacheLoading, 0x5B2AC0);
    RH_ScopedInstall(endCache, 0x5B2AD0);
    RH_ScopedInstall(addCacheCol, 0x5B2C20);
    RH_ScopedInstall(cacheLoadCol, 0x5B2CC0);
    RH_ScopedInstall(addColDef, 0x5B2DD0);
    RH_ScopedInstall(getColDef, 0x5B2E60);
    RH_ScopedInstall(setIplDef, 0x5B2ED0);
    RH_ScopedNamedGlobalInstall(getIplDefRaw, "getIplDef", 0x5B2EF0);
    RH_ScopedInstall(cacheIPLSection, 0x5B2F10);
    RH_ScopedInstall(addIPLEntity, 0x5B3040);
    RH_ScopedInstall(startCache, 0x5B31A0);
}

// 0x5B2AC0
bool CColAccel::isCacheLoading() {
    return m_iCacheState == eColAccelState::COLACCEL_LOADING;
}

// 0x5B31A0
void CColAccel::startCache() {
    ZoneScoped;

    m_iCachingColSize = GetColModelPool()->GetSize();
    m_iSectionSize    = (int32*)::operator new[](64 * sizeof(int32));

    // Como el original: operator new[] sin cabecera y solo se construye el rectángulo de cada entrada;
    // el resto de campos queda sin inicializar.
    auto* const iplDefs = (IplDef*)::operator new[](sizeof(IplDef) * TOTAL_IPL_MODEL_IDS);
    if (iplDefs) {
        for (auto i = 0u; i < TOTAL_IPL_MODEL_IDS; i++) {
            new (&iplDefs[i].bb) CRect();
        }
    }
    m_iplDefs = iplDefs;

    auto* const colBounds = (CColAccelColBound*)::operator new[](sizeof(CColAccelColBound) * TOTAL_IPL_MODEL_IDS);
    if (colBounds) {
        for (auto i = 0u; i < TOTAL_IPL_MODEL_IDS; i++) {
            new (&colBounds[i].m_Area) CRect();
        }
    }
    m_colBounds = colBounds;
}

// 0x5B2AD0
void CColAccel::endCache() {
    ZoneScoped;

    if (m_iCacheState == eColAccelState::COLACCEL_STARTED) {
        auto* file = CFileMgr::OpenFileForWriting(mp_cCacheName);
        CFileMgr::Write(file, &m_iNumColItems,  sizeof(m_iNumColItems));
        CFileMgr::Write(file, mp_caccColItems,  sizeof(CColAccelColEntry) * m_iNumColItems);
        CFileMgr::Write(file, &m_iNumSections,  sizeof(m_iNumSections));
        CFileMgr::Write(file, m_iSectionSize,   sizeof(m_iSectionSize) * m_iNumSections);
        CFileMgr::Write(file, m_iplDefs,        sizeof(IplDef) * TOTAL_IPL_MODEL_IDS);
        CFileMgr::Write(file, &m_iNumColBounds, sizeof(m_iNumColBounds));
        CFileMgr::Write(file, m_colBounds,      sizeof(CColAccelColBound) * m_iNumColBounds);
        CFileMgr::Write(file, &m_iNumIPLItems,  sizeof(m_iNumIPLItems));
        CFileMgr::Write(file, mp_caccIPLItems,  sizeof(CColAccelIPLEntry) * m_iNumIPLItems);
        CFileMgr::CloseFile(file);
    }

    // Como el original: las dos listas de elementos solo se liberan si existen; el resto se libera siempre.
    if (mp_caccIPLItems) {
        ::operator delete[](mp_caccIPLItems);
        mp_caccIPLItems = nullptr;
    }
    if (mp_caccColItems) {
        ::operator delete[](mp_caccColItems);
        mp_caccColItems = nullptr;
    }
    ::operator delete[](m_iSectionSize);
    m_iSectionSize = nullptr;
    ::operator delete[](m_iplDefs);
    m_iplDefs = nullptr;
    ::operator delete[](m_colBounds);
    m_colBounds = nullptr;

    m_iCacheState = eColAccelState::COLACCEL_ENDED;
}

// signature changed
// 0x5B2C20
void CColAccel::addCacheCol(PackedModelStartEnd startEnd, const CColModel& colModel) {
    if (m_iCacheState != eColAccelState::COLACCEL_STARTED)
        return;

    auto& colEntry = mp_caccColItems[m_iNumColItems];
    colEntry.m_wModelStart = startEnd.wModelStart;
    colEntry.m_wModelEnd = startEnd.wModelEnd;
    colEntry.m_boundBox = colModel.m_boundBox;
    colEntry.m_boundSphere = colModel.m_boundSphere;
    colEntry.m_nColSlot = colModel.m_nColSlot;
    colEntry.m_bColModelNotEmpty = colModel.m_bHasCollisionVolumes;

    ++m_iNumColItems;
}

// 0x5B2CC0
void CColAccel::cacheLoadCol() {
    if (!isCacheLoading())
        return;

    for (auto i = 0; i < m_iNumColItems; i++) {
        auto& colEntry = mp_caccColItems[i];
        // Como el original: el modelo es la palabra doble empaquetada (inicio | fin << 16).
        const auto modelId = reinterpret_cast<const PackedModelStartEnd&>(colEntry.m_wModelStart).modelId;
        auto* mi = CModelInfo::ms_modelInfoPtrs[modelId];
        auto* cm = new CColModel();

        cm->m_boundBox = colEntry.m_boundBox;
        cm->m_boundSphere = colEntry.m_boundSphere;
        cm->m_nColSlot = colEntry.m_nColSlot;
        cm->m_bHasCollisionVolumes = colEntry.m_bColModelNotEmpty;

        mi->SetColModel(cm, true);
        CColStore::IncludeModelIndex(colEntry.m_nColSlot, modelId);
    }
}

// 0x5B2DD0
void CColAccel::addColDef(ColDef colDef) {
    auto& colBound = m_colBounds[m_iNumColBounds];
    colBound.m_Area = colDef.m_Area;
    colBound.m_wModelStart = colDef.m_nModelIdStart;
    colBound.m_wModelEnd = colDef.m_nModelIdEnd;
    colBound.m_bInterior = colDef.m_bInterior;
    colBound.m_bProcedural = colDef.m_bProcedural;

    ++m_iNumColBounds;
}

// 0x5B2E60
void CColAccel::getColDef(ColDef& colDef) {
    auto& colBound = m_colBounds[m_iNumColBounds];
    colDef.m_Area = colBound.m_Area;
    colDef.m_nModelIdStart = colBound.m_wModelStart;
    colDef.m_nModelIdEnd = colBound.m_wModelEnd;
    colDef.m_bInterior = colBound.m_bInterior;
    colDef.m_bProcedural = colBound.m_bProcedural;

    ++m_iNumColBounds;
}

// 0x5B2ED0
void CColAccel::setIplDef(int32 iplIndex, IplDef iplDef) {
    // Como el original: copia los 0x34 bytes (relleno incluido).
    memcpy(&m_iplDefs[iplIndex], &iplDef, sizeof(IplDef));
}

// 0x5B2EF0
IplDef* CColAccel::getIplDefRaw(IplDef* out, int32 iplIndex) {
    // Como el original: copia los 0x34 bytes (relleno incluido) en el resultado y lo devuelve en eax.
    memcpy(out, &m_iplDefs[iplIndex], sizeof(IplDef));
    return out;
}

IplDef CColAccel::getIplDef(int32 iplIndex) {
    return m_iplDefs[iplIndex];
}

// 0x5B2F10
void CColAccel::cacheIPLSection(CEntity** ppEntities, int32 entitiesCount) {
    if (isCacheLoading()) {
        for (auto i = m_iNumIPLItems; i < m_iSectionSize[m_iNumSections]; ++i) {
            auto& iplEntry = mp_caccIPLItems[i];
            if (iplEntry.m_bIsUnderwater)
                ppEntities[iplEntry.m_nLodIndex]->m_bUnderwater = true; // Como el original: la palabra de +4

            if (iplEntry.m_bOwnsColModel)
                CModelInfo::GetModelInfo(iplEntry.m_nModelId)->SetOwnsColModel(true);

            if (iplEntry.m_bIsFarDrawDist)
                CModelInfo::GetModelInfo(iplEntry.m_nModelId)->m_fDrawDistance = 400.0F;

            if (iplEntry.m_bHasColModel) {
                auto* lodModelInfo = CModelInfo::GetModelInfo(iplEntry.m_nLodModelId);
                auto* entModelInfo = CModelInfo::GetModelInfo(iplEntry.m_nModelId);
                lodModelInfo->DeleteCollisionModel();
                lodModelInfo->SetColModel(entModelInfo->GetColModel(), false);
            }

            ++m_iNumIPLItems;
        }

        ++m_iNumSections;
    } else if (m_iCacheState == eColAccelState::COLACCEL_STARTED) {
        m_iSectionSize[m_iNumSections] = m_iNumIPLItems;
        ++m_iNumSections;
    }
}

// 0x5B3040
void CColAccel::addIPLEntity(CEntity** ppEntities, int32 entitiesCount, int32 entityIndex) {
    if (m_iCacheState != eColAccelState::COLACCEL_STARTED)
        return;

    auto* const entity = ppEntities[entityIndex];
    auto* const lod    = entity->GetLod();

    CColAccelIPLEntry iplEntry;
    iplEntry.m_nFlags       = 0;
    iplEntry.m_nEntityIndex = entityIndex;
    iplEntry.m_nModelId     = entity->GetModelIndex();
    iplEntry.m_nLodModelId  = lod->GetModelIndex();
    iplEntry.m_nLodIndex    = -1;

    // Como el original: los bits de distancia lejana y de colisión propia van cruzados respecto a
    // cacheIPLSection (lejana -> bit 8, colisión propia -> bit 4).
    if (entity->GetNumLodChildren() || TheCamera.m_fLODDistMultiplier * CModelInfo::ms_modelInfoPtrs[iplEntry.m_nModelId]->m_fDrawDistance > 300.0F)
        iplEntry.m_bOwnsColModel = true;

    for (auto i = 0; i < entitiesCount; ++i) {
        if (ppEntities[i] == lod) {
            iplEntry.m_nLodIndex = i;
            break;
        }
    }

    auto* const entModelInfo = CModelInfo::ms_modelInfoPtrs[iplEntry.m_nModelId];
    auto* const lodModelInfo = CModelInfo::ms_modelInfoPtrs[iplEntry.m_nLodModelId];
    if (lod->GetNumLodChildren() == 1) {
        if (entity->m_bUnderwater)
            iplEntry.m_bIsUnderwater = true;

        // Como el original: solo si la colisión difiere de la del LOD y existe.
        if (lodModelInfo->GetColModel() != entModelInfo->GetColModel() && entModelInfo->GetColModel())
            iplEntry.m_bHasColModel = true;
    } else {
        if (entModelInfo->bDoWeOwnTheColModel)
            iplEntry.m_bIsFarDrawDist = true;
    }

    if (iplEntry.m_nFlags) {
        mp_caccIPLItems[m_iNumIPLItems] = iplEntry;
        ++m_iNumIPLItems;
    }
}
