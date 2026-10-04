/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: InteriorManager_c
 * Reconstrucción: COMPLETA — 21/21 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 21/21 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "InteriorManager_c.h"
#include "FurnitureManager_c.h"

#include <TaskTypes/TaskSimpleCarDrive.h>

/**
 * Registra las operaciones de detección, pooling y actualización de interiores.
 */
void InteriorManager_c::InjectHooks() {
    RH_ScopedClass(InteriorManager_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(AddSameGroupEffectInfos, 0x598430);
    RH_ScopedInstall(AreAnimsLoaded, 0x5980F0);
    RH_ScopedInstall(PruneVisibleEffects, 0x598A60);
    RH_ScopedInstall(Exit, 0x598010);
    RH_ScopedInstall(GetVisibleEffects, 0x598D80);
    RH_ScopedInstall(IsInteriorEffectVisible, 0x598690);
    RH_ScopedInstall(GetPedsInterior, 0x598620);
    RH_ScopedInstall(ReturnInteriorToPool, 0x5984B0);
    RH_ScopedInstall(GetInteriorFromPool, 0x5984A0);
    RH_ScopedInstall(GetVectorsInterior, 0x5983D0);
    RH_ScopedInstall(SetStealableObjectStolen, 0x598390);
    RH_ScopedOverloadedInstall(FindStealableObjectId, "1", 0x598360,  int32(InteriorManager_c::*)(CEntity *) const);
    RH_ScopedOverloadedInstall(FindStealableObjectId, "2", 0x5982F0,  int32(InteriorManager_c::*)(int32, int32, CVector) const);
    RH_ScopedInstall(HasInteriorHadStealDataSetup, 0x5982B0);
    RH_ScopedInstall(IsGroupActive, 0x598280);
    RH_ScopedInstall(GetPedsInteriorGroup, 0x598240);
    RH_ScopedInstall(SetEntryExitPtr, 0x598180);
    RH_ScopedInstall(GetBoundingBox, 0x598090);
    RH_ScopedInstall(ActivatePeds, 0x598080);
    RH_ScopedInstall(Update, 0x598F50);
    RH_ScopedInstall(Init, 0x5C0500);
}

namespace {
// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
CVector MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// Como el original: CModelInfo::ms_modelInfoPtrs[movsx m_nModelIndex] sin comprobaciones
CBaseModelInfo* ModelInfoOf(const CEntity* entity) {
    return CModelInfo::ms_modelInfoPtrs[(int16)entity->m_nModelIndex];
}

// Como el original: el área se lee con movzx (byte sin signo)
int32 AreaOf(const CEntity* entity) {
    return (int32)(uint8)entity->GetAreaCode();
}
} // namespace

// 0x5C0500
/**
 * Inicializa los pools fijos de Interior_c/InteriorGroup_c y el catálogo de muebles.
 */
bool InteriorManager_c::Init() {
    m_IsFrozen       = false;
    m_IsActive       = true;
    m_ArePedsEnabled = true;
    // Como el original: los interiores se devuelven al pool del gestor global (ecx = &g_interiorMan)
    for (auto& i : m_Interiors) {
        g_interiorMan.ReturnInteriorToPool(&i);
    }
    for (int32 i = 0; i < 8; i++) {
        m_InteriorGroups[i].m_id = (uint8)i;
        m_InteriorGroupPool.AddItem(&m_InteriorGroups[i]);
    }
    g_furnitureMan.Init(); // Como el original: en línea en el EXE
    m_EnEx          = nullptr;
    m_InteriorCount = 0;
    m_ObjectCount   = 0;
    rng::fill(m_InteriorPedsAliveState, true);
    m_TimeLastPickupsGenerated = -180000; // Como el original: 0xFFFD40E0
    return true;
}

// 0x598F50
/**
 * Sincroniza los grupos de interior activos con los efectos 2DFX actualmente visibles.
 */
bool InteriorManager_c::Update() {
    ZoneScoped;

    if (m_IsFrozen) {
        return false;
    }

    bool hasAddedAnyInteriors = false;
    const auto plyr = FindPlayerPed(-1);

    InteriorEffectInfo_t visibleIntFxBuf[32];
    int32 numVisibleIntFx = 0;
    // Como el original: sin tarea activa no se buscan efectos (GetActiveTask se llama dos veces)
    if (AreaOf(plyr) != AREA_CODE_NORMAL_WORLD && m_IsActive) {
        if (plyr->GetTaskManager().GetActiveTask() && plyr->GetTaskManager().GetActiveTask()->GetTaskType() != TASK_SIMPLE_CAR_DRIVE) {
            numVisibleIntFx = (int32)GetVisibleEffects(visibleIntFxBuf, 32);
        }
    }
    PruneVisibleEffects(visibleIntFxBuf, (size_t)numVisibleIntFx, 8, 20.f);

    // Como el original: el grupo se asocia por entidad y m_groupId (+0xD), no por m_id
    const auto IsFxAssociatedWithGroup = [](const InteriorEffectInfo_t& fx, const InteriorGroup_c* g) {
        return g->m_pEntity == fx.Entity && g->m_groupId == (uint8)fx.Effects.data()[0]->m_groupId;
    };

    // Devuelve al pool los grupos cuyo efecto dejó de ser visible.
    for (auto* g = m_InteriorGroupList.GetHead(); g;) {
        auto* const next = g->m_pNext;
        bool keep = false;
        for (int32 j = 0; j < numVisibleIntFx; j++) {
            if (IsFxAssociatedWithGroup(visibleIntFxBuf[j], g) && !visibleIntFxBuf[j].IsCulled) {
                keep = true;
                break;
            }
        }
        if (!keep) {
            g->Exit();
            m_InteriorGroupList.RemoveItem(g);
            m_InteriorGroupPool.AddItem(g);
        }
        g = next;
    }

    for (int32 i = 0; i < numVisibleIntFx; i++) {
        auto& ifx = visibleIntFxBuf[i];
        if (ifx.IsCulled) {
            continue;
        }

        // Como el original: recorre toda la lista sin salir al encontrarlo
        bool exists = false;
        for (auto* g = m_InteriorGroupList.GetHead(); g; g = g->m_pNext) {
            if (IsFxAssociatedWithGroup(ifx, g)) {
                exists = true;
            }
        }
        if (exists) {
            continue;
        }

        const auto grp = m_InteriorGroupPool.RemoveHead();
        grp->Init(ifx.Entity, ifx.Effects.data()[0]->m_groupId);
        grp->m_EnEx = m_EnEx;
        m_InteriorGroupList.AddItem(grp);

        for (int32 k = 0; k < (int32)ifx.NumFx; k++) {
            const auto interior = m_InteriorPool.RemoveHead();
            if (!interior) { // Como el original: sigue el bucle sin crear el interior
                continue;
            }
            const auto& fxPos = ifx.Entity->GetPosition();

            interior->m_box        = ifx.Effects.data()[k];
            // Como el original: ftol((z * y) * x) truncado a 64 bits
            interior->m_interiorId = (int32)(int64)(fxPos.z * fxPos.y * fxPos.x) + (int32)ifx.FxIds.data()[k];
            interior->m_areaCode   = AreaOf(ifx.Entity);
            interior->m_pGroup     = grp;

            interior->Init(ifx.Effects.data()[k]->m_Pos);
            grp->AddInterior(interior);

            hasAddedAnyInteriors = true;
        }

        grp->Setup();

        m_TimeLastPickupsGenerated = CTimer::GetTimeInMS();
    }

    // Actualiza los grupos que permanecen activos tras altas y bajas.
    for (auto* g = m_InteriorGroupList.GetHead(); g; g = g->m_pNext) {
        g->Update();
    }

    return hasAddedAnyInteriors;
}

// 0x598A60
/**
 * Limita los efectos activos por cantidad y distancia a la cámara.
 */
void InteriorManager_c::PruneVisibleEffects(InteriorEffectInfo_t* infos, size_t numInfos, size_t maxVisibleFxCount, float maxDist) {
    const float maxDistSq = maxDist * maxDist;
    const auto  n         = (int32)numInfos;
    const auto  maxCount  = (int32)maxVisibleFxCount;

    // Como el original: suma con signo de todos los efectos
    int32 total = 0;
    for (int32 i = 0; i < n; i++) {
        total += (int32)infos[i].NumFx;
    }
    if (total <= maxCount) {
        return;
    }

    //> 0x598AA3 - Distancia 2D mínima a la cámara (en orden ascendente de efectos)
    for (int32 i = 0; i < n; i++) {
        auto& info = infos[i];
        info.DistSq = 100'000'000.f; // 0x4CBEBC20
        for (int32 k = 0; k < (int32)info.NumFx; k++) {
            const auto  pt  = MatrixTimesPoint59C890(info.Entity->GetMatrix(), info.Effects.data()[k]->m_Pos);
            const auto& cam = TheCamera.GetPosition();
            const float dx  = cam.x - pt.x;
            const float dy  = cam.y - pt.y;
            const float d   = dy * dy + dx * dx;
            if (d < info.DistSq) {
                info.DistSq = d;
            }
        }
    }

    //> 0x598B6E - Como el original: ordenación por intercambio (no estable) copiando la estructura entera
    for (int32 i = 0; i < n - 1; i++) {
        for (int32 j = i + 1; j < n; j++) {
            if (infos[j].DistSq < infos[i].DistSq) {
                InteriorEffectInfo_t tmp;
                std::memcpy(&tmp, &infos[i], sizeof(tmp));
                std::memcpy(&infos[i], &infos[j], sizeof(tmp));
                std::memcpy(&infos[j], &tmp, sizeof(tmp));
            }
        }
    }

    //> 0x598D4E - Como el original: NaN también se descarta
    int32 count = 0;
    for (int32 i = 0; i < n; i++) {
        auto& info = infos[i];
        const int32 c = (int32)info.NumFx + count;
        if (c > maxCount || !(info.DistSq <= maxDistSq)) {
            info.IsCulled = true;
        } else {
            count = c;
        }
    }
}

// 0x598430
// Como el original: el segundo argumento no se usa; el límite es 8 y el bucle no sale antes.
void InteriorManager_c::AddSameGroupEffectInfos(InteriorEffectInfo_t* ifxi, int32) {
    const auto mi = ModelInfoOf(ifxi->Entity);
    for (int32 i = 0; i < (int32)mi->m_n2dfxCount; i++) {
        if (i == (int32)ifxi->FxIds.data()[0]) {
            continue;
        }

        const auto fx = mi->Get2dEffect(i);
        if (fx->m_Type != EFFECT_INTERIOR) {
            continue;
        }
        const auto ifx = reinterpret_cast<C2dEffectInterior*>(fx);
        if (ifxi->Effects.data()[0]->m_groupId != ifx->m_groupId) {
            continue;
        }
        if ((int32)ifxi->NumFx >= 8) {
            continue;
        }
        ifxi->Effects.data()[ifxi->NumFx] = ifx;
        ifxi->FxIds.data()[ifxi->NumFx]   = (uint32)i;
        ifxi->NumFx++;
    }
}

// 0x5980F0
bool InteriorManager_c::AreAnimsLoaded(int32 animBlock) const {
    switch (animBlock) {
    case 0: animBlock = CAnimManager::GetAnimationBlockIndex("int_house"); break;
    case 1: animBlock = CAnimManager::GetAnimationBlockIndex("int_shop"); break;
    case 2: animBlock = CAnimManager::GetAnimationBlockIndex("int_office"); break;
    default: break;
    }
    // Como el original: CAnimManager::ms_aAnimBlocks (0xB5D4A0) sin límites, IsLoaded (+0x10) comparado con 1
    return *reinterpret_cast<const uint8*>(0xB5D4B0 + animBlock * 0x20) == 1;
}

// 0x598010
void InteriorManager_c::Exit() {
    for (auto* g = m_InteriorGroupList.GetHead(); g; g = g->m_pNext) {
        g->Exit();
    }
    m_InteriorGroupList.RemoveAll();
    m_InteriorPool.RemoveAll();
    m_InteriorGroupPool.RemoveAll();
    g_furnitureMan.Exit();
}

// 0x598D80
/**
 * Reúne efectos C2dEffectInterior visibles de entidades cercanas a la cámara.
 */
size_t InteriorManager_c::GetVisibleEffects(InteriorEffectInfo_t* intFxInfos, uint32 maxNumIntFxInfo) {
    int16 objCount = -1;
    CEntity* entitiesInRange[2048];
    CWorld::FindObjectsInRange(TheCamera.GetPosition(), 100.f, false, &objCount, 2048, entitiesInRange, true, false, false, false, false);

    int32 numIntFxInfo = 0;
    for (int32 e = 0; e < objCount; e++) {
        const auto entity = entitiesInRange[e];
        const auto mi     = ModelInfoOf(entity);
        if (!entity->GetRwObject()) {
            continue;
        }
        if (AreaOf(entity) != (int32)CGame::currArea && AreaOf(entity) != AREA_CODE_13) {
            continue;
        }

        for (int32 i = 0; i < (int32)mi->m_n2dfxCount; i++) {
            const auto fx = mi->Get2dEffect(i);
            if (fx->m_Type != EFFECT_INTERIOR) {
                continue;
            }
            const auto ifx = reinterpret_cast<C2dEffectInterior*>(fx);
            if (!IsInteriorEffectVisible(ifx, entity)) {
                continue;
            }

            // Como el original: se guarda sólo si no hay ya una entrada de la misma entidad y grupo
            bool found = false;
            for (int32 j = 0; j < numIntFxInfo; j++) {
                if (intFxInfos[j].Entity == entity && intFxInfos[j].Effects.data()[0]->m_groupId == ifx->m_groupId) {
                    found = true;
                }
            }
            if (found || numIntFxInfo >= (int32)maxNumIntFxInfo) {
                continue; // Como el original: el bucle continúa aunque el buffer esté lleno
            }

            // Como el original: sólo se escriben estos campos
            auto& info             = intFxInfos[numIntFxInfo++];
            info.Entity            = entity;
            info.Effects.data()[0] = ifx;
            info.FxIds.data()[0]   = (uint32)i;
            info.NumFx             = 1;
            info.IsCulled          = false;
        }
    }

    for (int32 i = 0; i < numIntFxInfo; i++) {
        AddSameGroupEffectInfos(&intFxInfos[i], (int32)maxNumIntFxInfo);
    }

    return (size_t)numIntFxInfo;
}

// 0x598690
// Como el original: caja alineada en coordenadas del mundo con las 8 esquinas transformadas del bounding box
// del modelo; se comprueban cámara, jugador 0 y jugador 1 (todas, sin cortocircuito) con límites inclusivos.
bool InteriorManager_c::IsInteriorEffectVisible(C2dEffectInterior*, CEntity* entity) {
    const auto& bb = ModelInfoOf(entity)->m_pColModel->m_boundBox;
    const CVector mn = bb.m_vecMin, mx = bb.m_vecMax;
    CVector corners[8];
    corners[0] = CVector{ mn.x, mn.y, mn.z };
    corners[1] = CVector{ mn.x, mx.y, mn.z };
    corners[2] = CVector{ mx.x, mx.y, mn.z };
    corners[3] = CVector{ mx.x, mn.y, mn.z };
    corners[4] = CVector{ mn.x, mn.y, mx.z };
    corners[5] = CVector{ mn.x, mx.y, mx.z };
    corners[6] = CVector{ mx.x, mx.y, mx.z };
    corners[7] = CVector{ mx.x, mn.y, mx.z };

    CVector bbMin{ 999999.f, 999999.f, 999999.f }, bbMax{ -999999.f, -999999.f, -999999.f }; // 0x497423F0 / 0xC97423F0
    for (auto& c : corners) {
        c = MatrixTimesPoint59C890(entity->GetMatrix(), c);
        if (c.x < bbMin.x) bbMin.x = c.x;
        if (c.y < bbMin.y) bbMin.y = c.y;
        if (c.z < bbMin.z) bbMin.z = c.z;
        if (c.x > bbMax.x) bbMax.x = c.x;
        if (c.y > bbMax.y) bbMax.y = c.y;
        if (c.z > bbMax.z) bbMax.z = c.z;
    }

    const auto IsInside = [&](const CVector& p) {
        return p.x >= bbMin.x && p.x <= bbMax.x
            && p.y >= bbMin.y && p.y <= bbMax.y
            && p.z >= bbMin.z && p.z <= bbMax.z;
    };
    const bool camInside = IsInside(TheCamera.GetPosition());
    const bool p0Inside  = IsInside(FindPlayerPed(0)->GetPosition());
    bool       p1Inside  = false;
    if (FindPlayerPed(1)) {
        p1Inside = IsInside(FindPlayerPed(1)->GetPosition());
    }
    return p1Inside || p0Inside || camInside;
}

// 0x598620
// Como el original: sólo mira los interiores del grupo del peatón (GetPedsInteriorGroup), los 8 huecos.
Interior_c* InteriorManager_c::GetPedsInterior(const CPed* ped) {
    const auto grp = GetPedsInteriorGroup(ped);
    if (!grp) {
        return nullptr;
    }
    for (const auto interior : grp->m_interiors) {
        if (interior && interior->IsPtInside(ped->GetPosition())) {
            return interior;
        }
    }
    return nullptr;
}

// 0x5984B0
void InteriorManager_c::ReturnInteriorToPool(Interior_c* interior) {
    m_InteriorPool.AddItem(interior);
}

// 0x5984A0
Interior_c* InteriorManager_c::GetInteriorFromPool() {
    return m_InteriorPool.RemoveHead();
}

// 0x5983D0
// Como el original: recorre los 8 huecos de cada grupo (no sólo m_numInteriors).
Interior_c* InteriorManager_c::GetVectorsInterior(const CVector& pt) {
    for (auto* g = m_InteriorGroupList.GetHead(); g; g = g->m_pNext) {
        for (const auto interior : g->m_interiors) {
            if (interior && interior->IsPtInside(pt)) {
                return interior;
            }
        }
    }
    return nullptr;
}

// 0x598390
void InteriorManager_c::SetStealableObjectStolen(CEntity* entity, uint8 isStolen) {
    if (const auto idx = FindStealableObjectId(entity); idx != -1) {
        // Como el original: guarda el byte tal cual (mov byte ptr); asignarlo al bool lo normalizaba a 1
        reinterpret_cast<uint8&>(m_Objects[idx].wasStolen) = isStolen;
    }
}

// 0x598360
int32 InteriorManager_c::FindStealableObjectId(CEntity* entity) const {
    for (int32 i = 0; i < m_ObjectCount; i++) { // Como el original: contador con signo
        if (m_Objects[i].entity == entity) {
            return i;
        }
    }
    return -1;
}

// 0x5982F0
int32 InteriorManager_c::FindStealableObjectId(int32 interiorId, int32 modelId, CVector point) const {
    for (int32 i = 0; i < m_ObjectCount; i++) { // Como el original: contador con signo
        const auto& v = m_Objects[i];
        if (v.modelId == modelId && v.interiorId == interiorId && v.pos.x == point.x && v.pos.y == point.y && v.pos.z == point.z) {
            return i;
        }
    }
    return -1;
}

// 0x5982B0
bool InteriorManager_c::HasInteriorHadStealDataSetup(Interior_c* interior) const {
    for (int32 i = 0; i < (int32)m_InteriorCount; i++) { // Como el original: contador con signo
        if (m_InteriorIds[i] == interior->m_interiorId) {
            return true;
        }
    }
    return false;
}

// 0x598280
int8 InteriorManager_c::IsGroupActive(int32 groupType) const {
    for (auto* g = m_InteriorGroupList.GetHead(); g; g = g->m_pNext) {
        if ((int32)(int8)g->m_groupType == groupType) { // Como el original: movsx
            return true;
        }
    }
    return false;
}

// 0x598240
InteriorGroup_c* InteriorManager_c::GetPedsInteriorGroup(const CPed* ped) {
    for (auto* g = m_InteriorGroupList.GetHead(); g; g = g->m_pNext) {
        for (int32 i = 0; i < (int32)g->m_numPeds; i++) { // Como el original: contador int8 con signo
            if (g->m_peds[i] == ped) {
                return g;
            }
        }
    }
    return nullptr;
}

// 0x598180
/**
 * Cambia el EntryExit de referencia; si cambia el rectángulo de entrada reinicia los registros
 * de objetos robables/interiores.
 */
void InteriorManager_c::SetEntryExitPtr(CEntryExit* enex) {
    if (enex->GetLinkedOrThis()->m_nArea == eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        return;
    }
    const auto& r = enex->m_recEntrance;
    if (r.top != m_EnExRect.top || r.bottom != m_EnExRect.bottom || r.left != m_EnExRect.left || r.right != m_EnExRect.right) {
        m_ObjectCount   = 0;
        m_InteriorCount = 0;
        rng::fill(m_InteriorPedsAliveState, true);
    }
    // Como el original: el puntero y el rectángulo se guardan siempre
    m_EnEx     = enex;
    m_EnExRect = enex->m_recEntrance;
}

// 0x598090
// Como el original: devuelve la caja del primer interior que contiene el FurnitureEntity_c de la entidad.
bool InteriorManager_c::GetBoundingBox(CEntity* entity, CVector* pos) {
    for (auto* g = m_InteriorGroupList.GetHead(); g; g = g->m_pNext) {
        for (const auto interior : g->m_interiors) {
            if (!interior) {
                continue;
            }
            if (const auto fe = interior->GetFurnitureEntity(entity)) {
                return interior->GetBoundingBox(fe, pos);
            }
        }
    }
    return false;
}

// 0x598080
void InteriorManager_c::ActivatePeds(bool enable) {
    m_ArePedsEnabled = enable;
}
