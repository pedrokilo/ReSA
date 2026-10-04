/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntryExitManager
 * Reconstrucción: COMPLETA — 18/18 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 17/18 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EntryExitManager.h"

using EntryExitTreeNode = CQuadTreeNode<CEntryExit*>;
auto& mp_QuadTree       = StaticRef<EntryExitTreeNode*>(0x96A7D0);

auto& mp_poolEntryExits = StaticRef<CEntryExitsPool*>(0x96A7D8);

void CEntryExitManager::InjectHooks() {
    RH_ScopedClass(CEntryExitManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x43F880);
    RH_ScopedInstall(Shutdown, 0x440B90);
    RH_ScopedInstall(AddEntryExitToStack, 0x43E410);
    RH_ScopedInstall(Update, 0x440D10);
    RH_ScopedInstall(ShutdownForRestart, 0x440C40);
    RH_ScopedInstall(DeleteOne, 0x43FD50);
    RH_ScopedInstall(AddOne, 0x43FA00);
    RH_ScopedInstall(FindNearestDoor, 0x43F630);
    RH_ScopedInstall(FindNearestEntryExit, 0x43F4B0);
    RH_ScopedInstall(EnableBurglaryHouses, 0x43F180);
    RH_ScopedInstall(GetPositionRelativeToOutsideWorld, 0x43F150);
    RH_ScopedInstall(PostEntryExitsCreation, 0x43F0A0);
    RH_ScopedInstall(LinkEntryExit, 0x43F050);
    RH_ScopedInstall(GetEntryExitIndex, 0x43EFD0);
    RH_ScopedInstall(ResetAreaCodeForVisibleObjects, 0x43ED80);
    RH_ScopedInstall(SetAreaCodeForVisibleObjects, 0x43ECF0);
    RH_ScopedInstall(Load, 0x5D55C0);
    RH_ScopedInstall(Save, 0x5D5970);
}

// 0x43F880
void CEntryExitManager::Init() {
    ZoneScoped;

    mp_QuadTree = new EntryExitTreeNode(WORLD_BOUNDS, 4);

    ms_exitEnterState = 0;
    ms_bDisabled = false;
    ms_entryExitStackPosn = 0;
    ms_bBurglaryHousesEnabled = false;

    // NOTSA: the intro's streaming can exceed the original 400 entries (419 in
    // the recorded trace). Reserve headroom up front: growing a live pool would
    // invalidate links, the quadtree and entry/exit stack pointers.
    mp_poolEntryExits = new CEntryExitsPool(512u, "Entry exits");
    mp_poolEntryExits->SetDealWithNoMemory(true);
}

// Como el original: lee el byte del área sin signo (movzx).
static int32 RawArea(const eAreaCodesS8& area) {
    return (uint8)area.get_underlying();
}

static int32 CurrArea() {
    return CGame::currArea.get_underlying();
}

// 0x440B90
void CEntryExitManager::Shutdown() {
    // Como el original: recorre el pool del último slot al primero.
    for (auto i = (int32)mp_poolEntryExits->GetSize(); i != 0;) {
        --i;
        if (!mp_poolEntryExits->IsFreeSlotAtIndex(i)) {
            mp_QuadTree->DeleteItem(mp_poolEntryExits->GetAt(i));
        }
    }

    delete mp_poolEntryExits; // Flush() called in destructor
    mp_poolEntryExits = nullptr;

    delete mp_QuadTree;
    mp_QuadTree = nullptr;

    ms_entryExitStackPosn = 0;
}

// 0x440C40
void CEntryExitManager::ShutdownForRestart() {
    // Como el original: recorre el pool del último slot al primero.
    for (auto i = (int32)mp_poolEntryExits->GetSize(); i != 0;) {
        --i;
        if (mp_poolEntryExits->IsFreeSlotAtIndex(i)) {
            continue;
        }
        auto& enex = *mp_poolEntryExits->GetAt(i);
        enex.bEnableAccess = true;
        if (enex.bDeleteEnex) {
            DeleteOne(i);
        }
    }

    EnableBurglaryHouses(false);
    if (mp_Active) {
        ResetAreaCodeForVisibleObjects();
        ms_exitEnterState = 0;
        mp_Active = nullptr;
    }
    ms_entryExitStackPosn = 0;
    ms_bDisabled = false;
}

// 0x440D10
void CEntryExitManager::Update() {
    ZoneScoped;

    const bool bDontShowMarkers =
         CCutsceneMgr::ms_cutsceneProcessing
      || CPad::GetPad()->DisablePlayerControls
      || CGameLogic::IsCoopGameGoingOn()
      || CReplay::Mode == eReplayMode::MODE_PLAYBACK
      || CEntryExitManager::ms_bDisabled;

    // Como el original: la lista se recorre aunque no se muestren marcadores; `bDontShowMarkers` se mira
    // después de la prueba de visibilidad.
    if (ms_exitEnterState == 0) {
        EntryExitTreeNode::ListType matches{};
        mp_QuadTree->GetAllMatching(
            CRect{ TheCamera.GetPosition() + TheCamera.m_mCameraMatrix.GetForward() * 30.f, 30.f },
            matches
        );

        for (auto* const enex : matches) {
            if (!enex->bEnableAccess) {
                continue;
            }
            // Has link and link is in current area, or has no link and is not in current area
            const bool inCurrArea = RawArea(enex->m_nArea) == CurrArea();
            if (enex->m_pLink ? !inCurrArea : inCurrArea) {
                continue;
            }

            const auto enexCenter = enex->GetPosition();
            if (!TheCamera.IsSphereVisible(enexCenter, 1.f)) {
                continue;
            }
            if (bDontShowMarkers) {
                continue;
            }
            if (!CClock::GetIsTimeInRange(enex->m_nTimeOn, enex->m_nTimeOff)) {
                continue;
            }

            // Como el original: distancia 2D a la cámara (dx² + dy² < 40²)
            const auto& camPos = TheCamera.GetPosition();
            const float dy = enexCenter.y - camPos.y;
            const float dx = enexCenter.x - camPos.x;
            if (!(dx * dx + dy * dy < 40.f * 40.f)) {
                continue;
            }

            CVector markerPos{ enexCenter.x, enexCenter.y, enexCenter.z + 1.f };
            C3dMarkers::PlaceMarkerCone((uint32)enex, markerPos, 2.0, 255, 255, 0, 255, 1024, 1.0, 5, 0);
        }
    }

    const auto player = FindPlayerPed();
    if (mp_Active) {
        if (mp_Active->TransitionFinished(player)) {
            CEntryExitManager::mp_Active = nullptr;
        }
        return;
    }

    // Como el original: posición del vehículo si bInVehicle (sin comprobar el puntero), si no la del peatón; copia local
    const CVector playerPos = player->bInVehicle ? player->m_pVehicle->GetPosition() : player->GetPosition();

    EntryExitTreeNode::ListType matches{};
    mp_QuadTree->GetAllMatching(CVector2D{ playerPos.x, playerPos.y }, matches);

    bool wasAnyMarkerInArea{};
    for (auto* const enex : matches) {
        // Como el original: primero IsInArea y después el flag de acceso
        if (enex->IsInArea(playerPos) && enex->bEnableAccess) {
            wasAnyMarkerInArea = true;
            if (!bDontShowMarkers && enex->TransitionStarted(player)) {
                mp_Active = enex;
                return;
            }
        }
    }

    if (!wasAnyMarkerInArea) {
        ms_exitEnterState = ms_exitEnterState != 3 ? 0 : 4; // TODO: Missing enum
    }

    // matches.Flush(); - Called in it's destructor
}

// 0x43E410
void CEntryExitManager::AddEntryExitToStack(CEntryExit* enex) {
    const auto linked = enex->GetLinkedOrThis();

    // Como el original: compara con signo (jle) y con la entrada enlazada, pero apila la entrada recibida
    if ((int32)ms_entryExitStackPosn > 0 && ms_entryExitStack[ms_entryExitStackPosn - 1] == linked) {
        ms_entryExitStackPosn--;
    } else if (RawArea(linked->m_nArea) == AREA_CODE_NORMAL_WORLD) {
        ms_entryExitStackPosn = 0;
    } else {
        ms_entryExitStack[ms_entryExitStackPosn++] = enex;
    }
}

/*!
* @addr 0x43FA00
* @brief Add a new entry exit
*
* @param timeOn If flag `BURGLARY_ACCESS` is not set it's going to define the value of enex->timeOn
* @param timeOff Unused
* @param name Name of the entry exit, may be nullptr. If flag `CREATE_LINKED_PAIR is set it is also used to identify the linked pair. (In this case it can't be nullptr)
*
* @returns The pool index of the created entry exit, 0 if failed.
*/
int32 CEntryExitManager::AddOne(
    float centerX, float centerY, float centerZ,
    float entranceAngle,
    float entranceRangeX, float entranceRangeY, float entranceRangeZ /*unused*/,
    float exitX, float exitY, float exitZ,
    float exitAngle,
    eAreaCodes area,
    CEntryExit::eFlags flags,
    int32 skyColor,
    int32 timeOn, int32 timeOff,
    int32 numberOfPeds,
    const char* name
) {
    const auto enex = mp_poolEntryExits->New();
    if (!enex) {
#ifdef FIX_BUGS
        return -1;
#else
        return 0; // Returns an actually valid index, not good
#endif
    }

    // Como el original: los campos se escriben sobre el slot sin construir el objeto (no se tocan el byte 0x37
    // ni, sin nombre, los bytes 1..7 del nombre); el ángulo de salida y la salida X/Y se copian tal cual.
    uint32 enexFlags = (uint32)flags;
    uint8  enexTimeOn;
    int32  enexTimeOff = timeOff;
    if (enexFlags & CEntryExit::BURGLARY_ACCESS) {
        enexTimeOn  = 0;
        enexTimeOff = 24;
    } else {
        enexTimeOn = (uint8)timeOn;
    }
    if ((enexFlags & CEntryExit::UNKNOWN_BURGLARY) && rand() < 0x3FFF) { // Como el original: rand directo (0x821B1E)
        enexTimeOn  = 0;
        enexTimeOff = 0;
    }
    if (ms_bBurglaryHousesEnabled || !(enexFlags & CEntryExit::BURGLARY_ACCESS)) {
        enexFlags |= CEntryExit::ENABLE_ACCESS;
    }

    const float halfX = entranceRangeX * 0.5f;
    const float halfY = entranceRangeY * 0.5f;
    enex->m_vecExitPos.x       = exitX;
    enex->m_nArea              = area;
    enex->m_recEntrance.left   = centerX - halfX;
    enex->m_nNumberOfPeds      = (uint8)numberOfPeds;
    enex->m_vecExitPos.y       = exitY;
    enex->m_fExitAngle         = exitAngle;
    enex->m_nSkyColor          = (uint8)skyColor;
    enex->m_recEntrance.bottom = centerY - halfY;
    enex->m_nTimeOn            = enexTimeOn;
    enex->m_nTimeOff           = (uint8)enexTimeOff;
    enex->m_nFlags             = (uint16)enexFlags;
    enex->m_pLink              = nullptr;
    enex->m_recEntrance.right  = halfX + centerX;
    enex->m_recEntrance.top    = halfY + centerY;
    enex->m_fEntranceZ         = centerZ + 1.f;
    enex->m_fEntranceAngleRad  = entranceAngle * std::bit_cast<float>(0x3C8EFA35u); // 0x8595EC
    enex->m_vecExitPos.z       = exitZ + 1.f;
    if (name) {
        strncpy(enex->m_szName, name, std::size(enex->m_szName));
    } else {
        enex->m_szName[0] = 0;
    }

    // Como el original: rectángulo del árbol con CMatrix::SetRotateZ (0x59B0E0) del ángulo EN GRADOS (sin
    // convertir) y MultiplyMatrixWithVector (0x59C890, con su orden de sumas) de (-ancho/2, -alto/2, 0) y
    // (ancho/2, alto/2, 0); después se suma el centro y se ordenan los extremos (NaN cae en la rama `else`).
    float rotCos, rotSin;
    __asm {
        fld   entranceAngle
        fcos
        fstp  rotCos
        fld   entranceAngle
        fsin
        fstp  rotSin
    }
    const float zero     = 0.0f;
    const float corner0X = entranceRangeX * -0.5f;
    const float corner0Y = entranceRangeY * -0.5f;
    float p0x = ((zero * zero + -rotSin * corner0Y) + rotCos * corner0X) + zero;
    float p0y = ((zero * zero + rotSin * corner0X) + rotCos * corner0Y) + zero;
    float p1x = ((zero * zero + -rotSin * halfY) + rotCos * halfX) + zero;
    float p1y = ((zero * zero + rotSin * halfX) + rotCos * halfY) + zero;
    p0x = centerX + p0x;
    p0y = centerY + p0y;
    p1x = centerX + p1x;
    p1y = centerY + p1y;

    CRect rect;
    if (p0x < p1x) {
        rect.left  = p0x;
        rect.right = p1x;
    } else {
        rect.right = p0x;
        rect.left  = p1x;
    }
    if (p0y < p1y) {
        rect.bottom = p0y;
        rect.top    = p1y;
    } else {
        rect.top    = p0y;
        rect.bottom = p1y;
    }
    mp_QuadTree->AddItem(enex, rect);

    // Como el original: el flag CREATE_LINKED_PAIR se comprueba aquí (LinkEntryExit en línea)
    if (enex->bCreateLinkedPair) {
        LinkEntryExit(enex);
    }

    return mp_poolEntryExits->GetIndex(enex);
}

// 0x43FD50
void CEntryExitManager::DeleteOne(int32 index) {
    if (const auto enex = mp_poolEntryExits->GetAt(index)) {
        mp_QuadTree->DeleteItem(enex);
#ifdef FIX_BUGS // Call destructor
        std::destroy_at(enex);
#endif
        mp_poolEntryExits->Delete(enex);
    }
}

// 0x43F630
CObject* CEntryExitManager::FindNearestDoor(CEntryExit const& exit, float radius) {
    const auto entranceCenter = exit.GetPosition();

    CEntity* objsInRange[100];
    int16 numObjsInRange;
    CWorld::FindObjectsInRange(entranceCenter, radius, false, &numObjsInRange, (int16)std::size(objsInRange), objsInRange, false, false, false, true, false);

    // Como el original: la distancia inicial es el radio, se recorre del último al primero y se compara la
    // distancia real sqrt((dz² + dx²) + dy²) de centro - objeto, estricta.
    float closestDist{ radius };
    CObject* closest{};
    for (int16 i = numObjsInRange; i != 0;) {
        --i;
        const auto obj = objsInRange[i]->AsObject();
        if (!obj->physicalFlags.bDisableMoveForce) {
            continue;
        }

        const auto& objPos = obj->GetPosition();
        const float dx   = entranceCenter.x - objPos.x;
        const float dy   = entranceCenter.y - objPos.y;
        const float dz   = entranceCenter.z - objPos.z;
        const float dist = std::sqrt((dz * dz + dx * dx) + dy * dy);
        if (dist < closestDist) {
            closest     = obj;
            closestDist = dist;
        }
    }
    return closest;
}

// 0x43F4B0
int32 CEntryExitManager::FindNearestEntryExit(const CVector2D& position, float range, int32 ignoreArea) {
    // Como el original: rectángulo calculado en línea (sin el assert de CRect con rango negativo)
    CRect rect;
    rect.left   = position.x - range;
    rect.bottom = position.y - range;
    rect.right  = position.x + range;
    rect.top    = position.y + range;

    EntryExitTreeNode::ListType matches;
    mp_QuadTree->GetAllMatching(rect, matches);

    float closestDist2D{ range + range };
    CEntryExit* closest{};
    for (auto* const enex : matches) {
        if (RawArea(enex->GetLinkedOrThis()->m_nArea) == ignoreArea) {
            continue;
        }

        const auto  center = enex->GetPosition2D();
        const float dy     = position.y - center.y;
        const float dx     = position.x - center.x;
        const float dist   = std::sqrt(dx * dx + dy * dy);
        if (dist < closestDist2D) {
            closest = enex;
            closestDist2D = dist;
        }
    }

    return closest
        ? mp_poolEntryExits->GetIndex(closest)
        : -1;
}

// 0x43F180
void CEntryExitManager::EnableBurglaryHouses(bool enable) {
    ms_bBurglaryHousesEnabled = enable;
    // Como el original: solo las entradas con BURGLARY_ACCESS, y cambia su ENABLE_ACCESS (del último slot al primero)
    for (auto i = (int32)mp_poolEntryExits->GetSize(); i != 0;) {
        --i;
        if (mp_poolEntryExits->IsFreeSlotAtIndex(i)) {
            continue;
        }
        auto& enex = *mp_poolEntryExits->GetAt(i);
        if (enex.bBurglaryAccess) {
            enex.bEnableAccess = enable;
        }
    }
}

// 0x43F150
void CEntryExitManager::GetPositionRelativeToOutsideWorld(CVector& pos) {
    for (auto i = ms_entryExitStackPosn; i; i--) {
        ms_entryExitStack[i - 1]->GetPositionRelativeToOutsideWorld(pos);
    }
}

// 0x43F0A0
void CEntryExitManager::PostEntryExitsCreation() {
    // Como el original: del último slot al primero
    for (auto i = (int32)mp_poolEntryExits->GetSize(); i != 0;) {
        --i;
        if (mp_poolEntryExits->IsFreeSlotAtIndex(i)) {
            continue;
        }
        const auto enex = mp_poolEntryExits->GetAt(i);
        if (enex->bCreateLinkedPair && !enex->m_pLink) {
            LinkEntryExit(enex);
        }
    }
}

// 0x43F050
void CEntryExitManager::LinkEntryExit(CEntryExit* enex) {
    // Como el original: no comprueba CREATE_LINKED_PAIR (lo hacen AddOne y PostEntryExitsCreation)
    const auto linkedEnExIdx{ GetEntryExitIndex(enex->m_szName, 0, CEntryExit::eFlags::CREATE_LINKED_PAIR) };
    if (linkedEnExIdx == -1) {
        return;
    }

    const auto linkedEnEx{ mp_poolEntryExits->GetAt(linkedEnExIdx) };
    enex->m_pLink = linkedEnEx;
    // Como el original: la otra entrada solo se enlaza de vuelta si no tenía enlace
    if (!linkedEnEx->m_pLink) {
        linkedEnEx->m_pLink = enex;
    }
    linkedEnEx->m_nTimeOn  = 0;
    linkedEnEx->m_nTimeOff = 24;
}

// 0x43EFD0
int32 CEntryExitManager::GetEntryExitIndex(const char* name, uint16 enabledFlags, uint16 disabledFlags) {
    // Como el original: del último slot al primero; solo cuentan los 8 bits bajos de ambas máscaras
    for (auto i = (int32)mp_poolEntryExits->GetSize(); i != 0;) {
        --i;
        if (mp_poolEntryExits->IsFreeSlotAtIndex(i)) {
            continue;
        }
        const auto& enex    = *mp_poolEntryExits->GetAt(i);
        const auto  enabled = (uint16)(uint8)enabledFlags;
        if ((uint16)(enex.m_nFlags & enabled) == enabled
            && ((uint8)enex.m_nFlags & (uint8)disabledFlags) == 0) {
            if (!_strnicmp(enex.m_szName, name, std::size(enex.m_szName))) {
                return i;
            }
        }
    }
    return -1;
}

// 0x43ED80
void CEntryExitManager::ResetAreaCodeForVisibleObjects() {
    // Como el original: post-decremento, el contador termina en -1
    while (ms_numVisibleEntities--) {
        ms_visibleEntityList[ms_numVisibleEntities]->SetAreaCode((eAreaCodes)ms_oldAreaCode);
    }
}

// 0x43ECF0
void CEntryExitManager::SetAreaCodeForVisibleObjects() {
    auto objsInFrustum    = (CEntity**)&PC_Scratch[256];
    auto numObjsInFrustum = CRenderer::GetObjectsInFrustum(objsInFrustum, 6.f, nullptr);

    const auto playerPed{ FindPlayerPed() };

    ms_numVisibleEntities = 0;
    // Como el original: el límite de 32 se mira antes de cada entidad y el área se lee con movzx
    for (; numObjsInFrustum != 0; numObjsInFrustum--, objsInFrustum++) {
        if (ms_numVisibleEntities >= 32) {
            break;
        }
        const auto entity = *objsInFrustum;
        if ((uint8)entity->GetAreaCode() == CurrArea() && entity != playerPed) {
            ms_visibleEntityList[ms_numVisibleEntities++] = entity;
            entity->SetAreaCode(AREA_CODE_13);
        }
    }

    ms_oldAreaCode = CGame::currArea;
}

bool CEntryExitManager::WeAreInInteriorTransition() {
    return ms_exitEnterState != 0;
}

// 0x5D55C0
bool CEntryExitManager::Load() {
    // Load entry exit stack
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_entryExitStackPosn);
    for (auto i = 0; i < (int32)ms_entryExitStackPosn; i++) { // Como el original: comparación con signo
        int16 enexIdx; // Como el original: índice con signo (movsx)
        CGenericGameStorage::LoadDataFromWorkBuffer(enexIdx);
        ms_entryExitStack[i] = mp_poolEntryExits->GetAt(enexIdx);
    }

    // Load entry exits
    auto enexIdx = CGenericGameStorage::LoadDataFromWorkBuffer<int16>();
    while (enexIdx != -1) {
        const auto enex      = mp_poolEntryExits->GetAt(enexIdx);
        const auto flags     = CGenericGameStorage::LoadDataFromWorkBuffer<uint16>();
        const auto linkedIdx = CGenericGameStorage::LoadDataFromWorkBuffer<int16>();

        // Como el original: un slot libre se ignora en silencio
        if (enex) {
            enex->m_nFlags = flags;
            enex->m_pLink  = linkedIdx == -1 ? nullptr : mp_poolEntryExits->GetAt(linkedIdx);
        }

        CGenericGameStorage::LoadDataFromWorkBuffer(enexIdx);
    }

    return true;
}

// 0x5D5970
bool CEntryExitManager::Save() {
    // Save entry exit stack
    CGenericGameStorage::SaveDataToWorkBuffer(ms_entryExitStackPosn);
    for (auto i = 0; i < (int32)ms_entryExitStackPosn; i++) { // Como el original: comparación con signo
        CGenericGameStorage::SaveDataToWorkBuffer<uint16>((uint16)mp_poolEntryExits->GetIndex(ms_entryExitStack[i]));
    }

    // Save entry exits
    // NOTSA: el original recorre 400 slots fijos; aquí todo el pool (Init reserva 512)
    for (auto&& [i, enex] : mp_poolEntryExits->GetAllValidWithIndex()) {
        // Como el original: el enlace solo se guarda si apunta a un slot ocupado del pool
        int16 data = -1;
        if (enex.m_pLink && mp_poolEntryExits->IsObjectValid(enex.m_pLink)) {
            data = (int16)mp_poolEntryExits->GetIndex(enex.m_pLink);
        }
        CGenericGameStorage::SaveDataToWorkBuffer<int16>(i); // Enex idx in pool
        CGenericGameStorage::SaveDataToWorkBuffer(enex.m_nFlags);
        CGenericGameStorage::SaveDataToWorkBuffer(data); // Linked enex idx in pool
    }

    // Mark the end of ENEX table
    CGenericGameStorage::SaveDataToWorkBuffer((int16)-1);
    return true;
}

CEntryExit* CEntryExitManager::GetInSlot(int32 slot) {
    return mp_poolEntryExits->GetAt(slot);
}

CEntryExitsPool* CEntryExitManager::GetPool() {
    return mp_poolEntryExits;
}
