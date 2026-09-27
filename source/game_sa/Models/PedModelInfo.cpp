/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedModelInfo
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedModelInfo.h"

/**
 * Registra las operaciones específicas de modelos de peatón.
 */
void CPedModelInfo::InjectHooks() {
    RH_ScopedVirtualClass(CPedModelInfo, 0x85BDC0, 17);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(GetModelType, 0x4C57C0);
    RH_ScopedVMTInstall(DeleteRwObject, 0x4C6C50);
    RH_ScopedVMTInstall(SetClump, 0x4C7340);
    RH_ScopedInstall(AddXtraAtomics, 0x4C6D40);
    RH_ScopedInstall(SetFaceTexture, 0x4C6D50);
    RH_ScopedInstall(CreateHitColModelSkinned, 0x4C6D90);
    RH_ScopedInstall(AnimatePedColModelSkinned, 0x4C6F70);
    RH_ScopedInstall(AnimatePedColModelSkinnedWorld, 0x4C7170);
    RH_ScopedInstall(IncrementVoice, 0x4C7300);
}

// 0x4C57C0
ModelInfoType CPedModelInfo::GetModelType() {
    return ModelInfoType::MODEL_INFO_PED;
}

// 0x4C6C50
// Libera primero el clump heredado y después el modelo de colisión de impactos propio.
void CPedModelInfo::DeleteRwObject() {
    CClumpModelInfo::DeleteRwObject();
    delete m_pHitColModel;
    m_pHitColModel = nullptr;
}

// 0x4C7340
/**
 * Instala el clump del ped, asigna IDs de bones/frames y prepara la colisión de impactos.
 *
 * Todos los atomics reciben además el callback de render específico de peatones.
 */
void CPedModelInfo::SetClump(RpClump* clump) {
    CClumpModelInfo::SetClump(clump);
    CClumpModelInfo::SetFrameIds(CPedModelInfo::m_pPedIds);

    if (!m_pHitColModel)
        CPedModelInfo::CreateHitColModelSkinned(clump);

    RpClumpForAllAtomics(GetRpClump(), CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPedCB);
    GetAnimHierarchyFromClump(GetRpClump()); // Unused? Or indirectly passes through EAX somehow?
}

// 0x4C6D40
void CPedModelInfo::AddXtraAtomics(RpClump* clump) {
    // NOP
}

// 0x4C6D50
void CPedModelInfo::SetFaceTexture(RwTexture* texture) {
    // NOP
}

// 0x4C6D90
/**
 * Construye un CColModel compuesto por esferas ligadas a bones relevantes del esqueleto.
 *
 * Cada centro local se transforma mediante la matriz actual del bone, de modo que
 * m_pHitColModel pueda reutilizarse como volumen de impactos animado.
 */
void CPedModelInfo::CreateHitColModelSkinned(RpClump* clump) {
    auto hierarchy = GetAnimHierarchyFromSkinClump(clump);
    auto cm = new CColModel();
    cm->AllocateData(NUM_PED_COL_NODE_INFOS, 0, 0, 0, 0, false);
    RwMatrixInvert(CGame::m_pWorkingMatrix1, RwFrameGetMatrix(RpClumpGetFrame(clump)));
    for (auto i = 0; i < NUM_PED_COL_NODE_INFOS; ++i) {
        auto& sphere = cm->m_pColData->m_pSpheres[i];
        auto& nodeInfo = m_pColNodeInfos[i];
        memcpy(CGame::m_pWorkingMatrix2, CGame::m_pWorkingMatrix1, sizeof(RwMatrix));

        CVector vecCenter = nodeInfo.m_vecCenter;
        auto animId = RpHAnimIDGetIndex(hierarchy, nodeInfo.m_nBoneID);
        auto pAnimMat = &RpHAnimHierarchyGetMatrixArray(hierarchy)[animId];
        RwMatrixTransform(CGame::m_pWorkingMatrix2, pAnimMat, RwOpCombineType::rwCOMBINEPRECONCAT);
        RwV3dTransformPoints(&vecCenter, &vecCenter, 1, CGame::m_pWorkingMatrix2);

        sphere.m_vecCenter = vecCenter;
        sphere.m_fRadius = nodeInfo.m_fRadius;
        sphere.m_Surface.m_nMaterial = eSurfaceType::SURFACE_PED;
        sphere.m_Surface.m_nPiece = nodeInfo.m_nFlags;
    }

    cm->m_boundSphere.Set(1.5F, CVector(0.0F, 0.0F, 0.0F));
    cm->m_boundBox.Set(CVector(-0.5F, -0.5F, -1.2F), CVector(0.5F, 0.5F, 1.2F));
    cm->m_nColSlot = 0;

    m_pHitColModel = cm;
}

// 0x4C6F70
/**
 * Reposiciona las esferas del hit-col en espacio local del clump usando la pose actual.
 * También centra los bounds alrededor de BONE_SPINE1 para acompañar al cuerpo.
 */
CColModel* CPedModelInfo::AnimatePedColModelSkinned(RpClump* clump) {
    if (!m_pHitColModel) {
        CreateHitColModelSkinned(clump);
        return m_pHitColModel;
    }

    auto hierarchy = GetAnimHierarchyFromSkinClump(clump);
    RwMatrixInvert(CGame::m_pWorkingMatrix1, RwFrameGetMatrix(RpClumpGetFrame(clump)));
    for (int32 i = 0; i < NUM_PED_COL_NODE_INFOS; ++i) {
        auto& sphere = m_pHitColModel->m_pColData->m_pSpheres[i];
        auto& nodeInfo = m_pColNodeInfos[i];
        memcpy(CGame::m_pWorkingMatrix2, CGame::m_pWorkingMatrix1, sizeof(RwMatrix));

        CVector vecCenter = nodeInfo.m_vecCenter;
        auto animId = RpHAnimIDGetIndex(hierarchy, nodeInfo.m_nBoneID);
        auto pAnimMat = &RpHAnimHierarchyGetMatrixArray(hierarchy)[animId];
        RwMatrixTransform(CGame::m_pWorkingMatrix2, pAnimMat, RwOpCombineType::rwCOMBINEPRECONCAT);
        RwV3dTransformPoints(&vecCenter, &vecCenter, 1, CGame::m_pWorkingMatrix2);

        sphere.m_vecCenter = vecCenter;
    }

    memcpy(CGame::m_pWorkingMatrix2, CGame::m_pWorkingMatrix1, sizeof(RwMatrix));
    auto animId = RpHAnimIDGetIndex(hierarchy, eBoneTag::BONE_SPINE1);
    auto pAnimMat = &RpHAnimHierarchyGetMatrixArray(hierarchy)[animId];
    RwMatrixTransform(CGame::m_pWorkingMatrix2, pAnimMat, RwOpCombineType::rwCOMBINEPRECONCAT);
    auto vecSpine = CVector(0.0F, 0.0F, 0.0F);
    RwV3dTransformPoints(&vecSpine, &vecSpine, 1, CGame::m_pWorkingMatrix2);
    m_pHitColModel->m_boundSphere.Set(1.5F, vecSpine);
    m_pHitColModel->m_boundBox.Set(
        { vecSpine.x - 1.2F, vecSpine.y - 1.2F, vecSpine.z - 1.2F },
        { vecSpine.x + 1.2F, vecSpine.y + 1.2F, vecSpine.z + 1.2F }
    );

    return m_pHitColModel;
}

// 0x4C7170
// Variante que escribe directamente posiciones world obtenidas de las matrices del esqueleto.
CColModel* CPedModelInfo::AnimatePedColModelSkinnedWorld(RpClump* clump) {
    if (!m_pHitColModel)
        CreateHitColModelSkinned(clump);

    auto hierarchy = GetAnimHierarchyFromSkinClump(clump);
    for (int32 i = 0; i < NUM_PED_COL_NODE_INFOS; ++i) {
        auto& sphere = m_pHitColModel->m_pColData->m_pSpheres[i];
        auto& nodeInfo = m_pColNodeInfos[i];

        CVector vecCenter = nodeInfo.m_vecCenter;
        auto animId = RpHAnimIDGetIndex(hierarchy, nodeInfo.m_nBoneID);
        auto pAnimMat = &RpHAnimHierarchyGetMatrixArray(hierarchy)[animId];
        RwV3dTransformPoints(&vecCenter, &vecCenter, 1, pAnimMat);

        sphere.m_vecCenter = vecCenter;
    }

    auto animId = RpHAnimIDGetIndex(hierarchy, eBoneTag::BONE_SPINE1);
    auto pAnimMat = &RpHAnimHierarchyGetMatrixArray(hierarchy)[animId];
    auto vecSpine = CVector(0.0F, 0.0F, 0.0F);
    RwV3dTransformPoints(&vecSpine, &vecSpine, 1, pAnimMat);
    m_pHitColModel->m_boundSphere.Set(1.5F, vecSpine);
    m_pHitColModel->m_boundBox.Set(
        { vecSpine.x - 1.2F, vecSpine.y - 1.2F, vecSpine.z - 1.2F },
        { vecSpine.x + 1.2F, vecSpine.y + 1.2F, vecSpine.z + 1.2F }
    );

    return m_pHitColModel;
}

// 0x4C7300
// Avanza cíclicamente dentro del rango de voces configurado; -1 invalida la selección.
void CPedModelInfo::IncrementVoice() {
    if (m_nVoiceMin < 0 || m_nVoiceMax < 0) {
        m_nVoiceId = -1;
    } else {
        ++m_nVoiceId;
        if (m_nVoiceId > m_nVoiceMax || m_nVoiceId < m_nVoiceMin) {
            m_nVoiceId = m_nVoiceMin;
        }
    }
}
