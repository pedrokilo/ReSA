/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedModelInfo
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedModelInfo.h"

namespace CColModelBody {
void __fastcall DestructorExe(CColModel* self, void* edx); // Collision/ColModel.cpp: 0x40F700
}

namespace CBaseModelInfoBody {
CBaseModelInfo* __fastcall Constructor(CBaseModelInfo* self, void* edx); // BaseModelInfo.cpp: 0x4C4A60
}

// 0x4C57A0 (lote 223A, sin validar; el listado decia CWeaponModelInfo; la clase sale de la vtable 0x85BDC0 y del campo +0x34): constructor de CPedModelInfo del EXE (thiscall, sin argumentos de pila, `ret`, 25 bytes: `push esi;
// mov esi,ecx; call 0x4C4A60; mov [esi],0x85BDC0; mov [esi+0x34],0; mov eax,esi; pop esi; ret`). Como el original: llama al cuerpo del constructor de CBaseModelInfo (0x4C4A60, con hook; no al de CClumpModelInfo), deja la
// vtable de CPedModelInfo del EXE (0x85BDC0), pone a cero el modelo de colision de impactos (+0x34) y devuelve `this`. Lo pasa el inicializador estatico del almacen de peatones (ModelInfo.cpp, 0x4C67D0) al iterador de
// constructores de vectores. Funcion propia sin expansion en linea.
__declspec(noinline) static CPedModelInfo* __fastcall PedModelInfoConstructorBody(CPedModelInfo* self, void* /*edx*/) {
    static_assert(offsetof(CPedModelInfo, m_pHitColModel) == 0x34);
    CBaseModelInfoBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x85BDC0;
    self->m_pHitColModel = nullptr;
    return self;
}

// 0x4C62F0 (lote 194A, sin validar; el listado decia CModelInfo; thiscall, sin argumentos de pila, `ret`, 90 bytes con marco SEH que solo desenrolla): cuerpo del destructor de CPedModelInfo del EXE (lo pasa a los
// iteradores de vectores del almacen de peatones `push 0x4C62F0` en 0x4C67D1 y 0x4C6391, y lo llama el destructor con borrado 0x4C6350). Como el original: vtable de CPedModelInfo (0x85BDC0); si el modelo de
// colision de impactos (+0x34) no es nulo, destructor de CColModel (0x40F700, con hook) y CColModel::operator delete (0x40FC40, con hook); vtable de CBaseModelInfo (0x85BB9C). Funcion propia sin expansion en linea.
__declspec(noinline) static void __fastcall PedModelInfoDestructorBody(CPedModelInfo* self, void* /*edx*/) {
    static_assert(offsetof(CPedModelInfo, m_pHitColModel) == 0x34);
    *reinterpret_cast<uint32*>(self) = 0x85BDC0;
    if (CColModel* const hitColModel = self->m_pHitColModel) {
        CColModelBody::DestructorExe(hitColModel, nullptr);
        CColModel::operator delete(hitColModel);
    }
    *reinterpret_cast<uint32*>(self) = 0x85BB9C;
}

// 0x4C6350 (lote 194A, sin validar): destructor con borrado de CPedModelInfo del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 27 bytes: `push esi; mov esi,ecx; call 0x4C62F0; test byte [esp+8],1; je ..; push esi;
// call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x4C62F0, con hook), libera `this` con operator delete (0x8214BD, CRT) solo si el bit 0 del BYTE
// de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
static CPedModelInfo* __fastcall PedModelInfoScalarDeletingDestructor(CPedModelInfo* self, void* /*edx*/, uint8 flags) {
    PedModelInfoDestructorBody(self, nullptr);
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}

/**
 * Registra las operaciones específicas de modelos de peatón.
 */
void CPedModelInfo::InjectHooks() {
    RH_ScopedVirtualClass(CPedModelInfo, 0x85BDC0, 17);
    RH_ScopedCategory("Models");
    RH_ScopedNamedGlobalInstall(PedModelInfoConstructorBody, "ConstructorBody", 0x4C57A0); // lote 223A (sin validar)
    RH_ScopedNamedGlobalInstall(PedModelInfoDestructorBody, "DestructorBody", 0x4C62F0); // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(PedModelInfoScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4C6350); // lote 194A (sin validar)

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
 *
 * @addr 0x4C7340
 */
void CPedModelInfo::SetClump(RpClump* clump) {
    CClumpModelInfo::SetClump(clump);
    CClumpModelInfo::SetFrameIds(CPedModelInfo::m_pPedIds);

    if (!m_pHitColModel)
        CPedModelInfo::CreateHitColModelSkinned(clump);

    RpClumpForAllAtomics(GetRpClump(), CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPedCB);
    GetAnimHierarchyFromClump(clump); // Como el original: recibe el clump del parámetro (resultado sin usar).
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
 *
 * @addr 0x4C6D90
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
 *
 * @addr 0x4C6F70
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
