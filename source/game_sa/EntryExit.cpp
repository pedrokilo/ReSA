/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntryExit
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 1.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   GetPositionRelativeToOutsideWorld | 0x0043EA00 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EntryExit.h"
#include "EntryExitManager.h"
#include "TaskComplexGotoDoorAndOpen.h"
#include "TaskSimpleUninterruptable.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskComplexFacial.h"
#include "Garages.h"
#include "PlayerPed.h"
#include "Object.h"
#include "Interior/InteriorManager_c.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventLeaderEntryExit.h"
#include "Rubbish.h"

static void __fastcall EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) {
    enex->GetPositionRelativeToOutsideWorld(*outPos);
}

// Como el original: 0x43EA00 no toca edx y CEntryExitManager::GetPositionRelativeToOutsideWorld (0x43F150)
// usa edx como contador del bucle a través de la llamada; envoltorio thiscall (ret 4) que conserva edx.
static __declspec(naked) void EntryExitGetPositionRelativeToOutsideWorldKeepEdx() {
    __asm {
        push edx
        push dword ptr [esp+8]
        call EntryExitGetPositionRelativeToOutsideWorld
        pop  edx
        ret  4
    }
}

void CEntryExit::InjectHooks() {
    RH_ScopedClass(CEntryExit);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GenerateAmbientPeds, 0x43E8B0);
    RH_ScopedInstall(GetEntryExitToDisplayNameOf, 0x43E650);
    RH_ScopedInstall(FindValidTeleportPoint, 0x43EAF0);
    RH_ScopedInstall(IsInArea, 0x43E460);
    RH_ScopedNamedGlobalInstall(EntryExitGetPositionRelativeToOutsideWorldKeepEdx, "GetPositionRelativeToOutsideWorld", 0x43EA00, {.locked = true});
    RH_ScopedInstall(TransitionStarted, 0x43FFD0);
    RH_ScopedInstall(TransitionFinished, 0x4404A0);
    RH_ScopedInstall(RequestObjectsInFrustum, 0x43E690);
    RH_ScopedInstall(RequestAmbientPeds, 0x43E6D0);
    RH_ScopedInstall(WarpGangWithPlayer, 0x43F1F0);
    RH_ScopedInstall(ProcessStealableObjects, 0x43E990);
}

// Code based on 0x43FA00
CEntryExit::CEntryExit(
    CVector center,
    CVector entranceRange,
    float entranceAngleDeg,
    CVector exit,
    float exitAngle,
    eAreaCodes area,
    CEntryExit::eFlags flags,
    int32 skyColor,
    int32 timeOn, int32 timeOff,
    int32 numberOfPeds,
    const char* name
) :
    m_nFlags{(uint16)flags}, // Set here, this way we can directly use the flag fields instead of using the enum
    m_recEntrance{
        center.x - entranceRange.x / 2.f,
        center.y - entranceRange.y / 2.f,
        center.x + entranceRange.x / 2.f,
        center.y + entranceRange.y / 2.f,
    },
    m_vecExitPos{ exit + CVector{ 0.f, 0.f, 1.f } },
    m_nArea{ area },
    m_nNumberOfPeds{ (uint8)numberOfPeds },
    m_fExitAngle{ exitAngle },
    m_nSkyColor{ (uint8)skyColor },
    m_fEntranceZ{ center.z + 1.f },
    m_fEntranceAngleRad{ DegreesToRadians(entranceAngleDeg) }
{
    std::tie(m_nTimeOn, m_nTimeOff) = [&]() -> std::pair<uint8, uint8> {
        if (bUnknownBurglary && CGeneral::RandomBool(50.f)) {
            return { 0, 0 };
        } else if (bBurglaryAccess) {
            return { 0, 24 };
        } else {
            return { timeOn, timeOff };
        }
    }();

    if (CEntryExitManager::ms_bBurglaryHousesEnabled || !bBurglaryAccess) {
        bEnableAccess = true;
    }

    if (name){
        strcpy_s(m_szName, name);
    } else {
        m_szName[0] = 0;
    }
}

// 0x43EA90
bool IsTeleportPointValid(const CVector& origin, const CVector& target) {
    return !CWorld::TestSphereAgainstWorld(target, 0.35f, nullptr, true, true, true, true, true, false)
        && CWorld::GetIsLineOfSightClear(origin, target, true, true, false, true, true, false, false);
}

// 0x43E8B0
void CEntryExit::GenerateAmbientPeds(const CVector& posn) {
    CPopulation::bInPoliceStation = false;
    if (CGame::currArea == eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        CPopulation::NumberOfPedsInUseInterior = 0;
        float exitAngle = DegreesToRadians(ms_spawnPoint->m_fExitAngle);
        CPopulation::CreateWaitingCoppers(posn, exitAngle);
    } else {
        constexpr const char* PoliceStationEnExNames[]{ "POLICE1", "POLICE2", "POLICE3", "POLICE4" };
        CPopulation::bInPoliceStation = rng::any_of(PoliceStationEnExNames, [this](auto&& n) { return _stricmp(m_szName, n) == 0; });
        // Como el original: movsx, el número de peds se extiende con signo
        const int32 numPedsToSpawn = (int8)(m_pLink ? m_pLink->m_nNumberOfPeds : m_nNumberOfPeds);
        CPopulation::NumberOfPedsInUseInterior = numPedsToSpawn;
        CPopulation::PopulateInterior(numPedsToSpawn, posn);
    }
}

// 0x43E650
CEntryExit* CEntryExit::GetEntryExitToDisplayNameOf() {
    if (ms_spawnPoint->m_nArea == eAreaCodes::AREA_CODE_NORMAL_WORLD || !HasNameSet()) {
        return nullptr;
    }
    if (m_nArea == eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        return this; // Como el original: devuelve `this` (eax = this), no nullptr
    }
    // TODO: Probably inlined from `CEntryExitManager`
    // Como el original: comparación con signo (jle) y devuelve la entrada anterior a la cima (posn - 2)
    const auto posn = (int32)CEntryExitManager::ms_entryExitStackPosn;
    if (posn > 1 && CEntryExitManager::ms_entryExitStack[posn - 1] == ms_spawnPoint) {
        return CEntryExitManager::ms_entryExitStack[posn - 2];
    }
    return this;
}

// 0x43EA00
// El hook del EXE pasa por EntryExitGetPositionRelativeToOutsideWorldKeepEdx (conserva edx).
void CEntryExit::GetPositionRelativeToOutsideWorld(CVector& outPos) {
    const auto pos = GetPosition(); // Como el original: centro de la entrada de `this`, antes de sustituir por el enlace
    const auto enex = GetLinkedOrThis(); // this = GetLinkedOrThis();
    if (enex->m_nArea != eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        // Como el original: primero suma la posición y después resta la salida, (out + pos) - exit
        outPos += pos;
        outPos -= enex->m_vecExitPos;
    }
}

// Return center of entrance rect
CVector CEntryExit::GetPosition() const {
    return CVector{ m_recEntrance.GetCenter(), m_fEntranceZ };
}

CVector2D CEntryExit::GetPosition2D() const {
    return CVector2D{ m_recEntrance.GetCenter() };
}

// Returns the matrix with which `rectEntrance`'s points were calculated.
CMatrix CEntryExit::GetRectEntranceMatrix() const {
    CMatrix mat;
    mat.SetRotateZ(m_fEntranceAngleRad);
    mat.GetPosition() = GetPosition();
    return mat;
}

bool CEntryExit::IsVisibleByTime() const {
    return CClock::GetIsTimeInRange(m_nTimeOn, m_nTimeOff);
}

// Transforms a point into the entrance rect
CVector CEntryExit::TransformEntrancePoint(const CVector& point) const {
    return GetRectEntranceMatrix().TransformPoint(point - GetPosition());
}

// 0x43EAF0
void CEntryExit::FindValidTeleportPoint(CVector& outTeleportPoint) {
    outTeleportPoint = ms_spawnPoint->m_vecExitPos;

    if (!CWorld::TestSphereAgainstWorld(outTeleportPoint, 0.35f, nullptr, true, true, true, true, true, false)) {
        return;
    }
    
    // Test 2 * 8 spheres around the spawn point, and return whichever
    // doesn't collide with the world and the line of sight between it and `outPoint` is clear
    // Como el original: ángulo = i * 6.2831855 (flt_858CBC) * 0.125 (flt_858C48); fcos/fsin a precisión completa,
    // multiplicados (radio 1.25, flt_8595F0) o duplicados (radio 2, fadd st,st) antes de redondear; luego + out.
    constexpr auto NumTestPoints{ 8 };
    for (auto ring = 0; ring < 2; ring++) { // Test with 2 ranges
        for (int32 i = 0; i < NumTestPoints; i++) {
            const float rot = (float)i * std::bit_cast<float>(0x40C90FDBu) * 0.125f;
            float offX, offY;
            if (ring == 0) {
                float ringRadius = 1.25f;
                __asm {
                    fld   rot
                    fld   st(0)
                    fcos
                    fmul  ringRadius
                    fstp  offX
                    fsin
                    fmul  ringRadius
                    fstp  offY
                }
            } else {
                __asm {
                    fld   rot
                    fld   st(0)
                    fcos
                    fadd  st, st(0)
                    fstp  offX
                    fsin
                    fadd  st, st(0)
                    fstp  offY
                }
            }
            const CVector point{ offX + outTeleportPoint.x, offY + outTeleportPoint.y, outTeleportPoint.z };
            if (IsTeleportPointValid(outTeleportPoint, point)) {
                outTeleportPoint = point;
                return;
            }
        }
    }
}

bool CEntryExit::HasNameSet() const {
    return m_szName[0] != 0;
}

const CRect& CEntryExit::GetEntranceRect() const {
    return m_recEntrance;
}

uint8 CEntryExit::GetMyOrLinkedArea() const {
    return m_pLink ? m_pLink->m_nArea : m_nArea;
}

// 0x43E460
bool CEntryExit::IsInArea(const CVector& position) {
    float x, y;
    // It's quite common `m_fEntranceAngle` is 0, so I guess this is some kind of an optimization?
    if (m_fEntranceAngleRad == 0.f) { // Como el original: NaN va por la rama rotada (test ah,44h)
        x = position.x;
        y = position.y;
    } else {
        // Como el original: centro = (max - min) * 0.5 + min; d = (p.x - cx, p.y - cy, p.z) sin restar la z;
        // CMatrix::SetRotateZ (0x59B0E0, fcos/fsin de precisión completa) y MultiplyMatrixWithVector
        // (0x59C890) con su orden de sumas; después se suma el centro 2D.
        const float cy = (m_recEntrance.top - m_recEntrance.bottom) * 0.5f + m_recEntrance.bottom;
        const float cx = (m_recEntrance.right - m_recEntrance.left) * 0.5f + m_recEntrance.left;
        const float dz = position.z;
        const float dy = position.y - cy;
        const float dx = position.x - cx;
        const float rot = m_fEntranceAngleRad;
        float rotCos, rotSin;
        __asm {
            fld   rot
            fcos
            fstp  rotCos
            fld   rot
            fsin
            fstp  rotSin
        }
        const float zero = 0.0f;
        x = ((zero * dz + -rotSin * dy) + rotCos * dx) + zero;
        y = ((zero * dz + rotSin * dx) + rotCos * dy) + zero;
        x = x + cx;
        y = y + cy;
    }
    // Como el original: cada comparación falla sólo si es ordenada (NaN en x/y pasa), y |dz| < 1 estricto
    if (x < m_recEntrance.left || x > m_recEntrance.right) {
        return false;
    }
    if (y < m_recEntrance.bottom || y > m_recEntrance.top) {
        return false;
    }
    return std::abs(position.z - m_fEntranceZ) < 1.0f;
}

// 0x43FFD0
bool CEntryExit::TransitionStarted(CPed* ped) {
    if (   !bEnableAccess
        || CEntryExitManager::ms_exitEnterState != 0
        || !IsVisibleByTime()
    ) {
        return false;
    }

    // If ped in vehicle check if enex accepts it
    if (ped->bInVehicle) {
        if (ped->m_pVehicle->m_pHandlingData->m_bIsBig) { // Move before the IsAuto or Bike check
            return false; // No big vehicles allowed :(
        }

        switch (ped->m_pVehicle->m_nVehicleType) {
        case eVehicleType::VEHICLE_TYPE_AUTOMOBILE: {
            if (!bCarsAndAircraft) {
                return false;
            }
            break;
        }
        case eVehicleType::VEHICLE_TYPE_BIKE: {
            if (!bBikesAndMotorcycles) {
                return false;
            }
            break;
        }
        default: // Nothing else is disallowed - TODO: Uncomment this, I want my Rhino in CJ's house
            return false;
        }
    } else if (bDisableExit) {
        return false;
    }

    if (m_pLink) {
        ms_spawnPoint = m_pLink;
        m_pLink->m_pLink = this;
    } else {
        ms_spawnPoint = this;
    }

    const auto spawnPointExitToUs{ ms_spawnPoint->m_vecExitPos - GetPosition() };

    if (!ped->AsPlayer()->CanPlayerStartMission()) {
        return false;
    }

    bEnteredWithoutExit = true;
    ms_pDoor            = nullptr;

    ped->bCanExitCar    = false;

    const auto AddPedScriptCommand = [ped](auto* task) {
        ped->GetEventGroup().Add(CEventScriptCommand{
            TASK_PRIMARY_PRIMARY,
            task,
            false
        });
    };

    // Como el original: CVector::Magnitude (0x4082C0, fsqrt) comparado con 10.0 (flt_85862C), no la magnitud al cuadrado
    if (bUnknownPairing || bFoodDateFlag || ped->bInVehicle) {
        if (spawnPointExitToUs.Magnitude() > 10.f) {
            ms_bWarping = true;
        }
    } else {
        ms_bWarping = spawnPointExitToUs.Magnitude() > 10.f;

        const auto spawnPointExitToUsDir = Normalized(spawnPointExitToUs);

        const auto SetupFixedCamera = [this](CVector lookAtDir) {
            // 0x44031A
            auto fixedModePos = GetPosition() - lookAtDir * 3.f;
            fixedModePos.z += 1.f;
            TheCamera.SetCamPositionForFixedMode(fixedModePos, {});
            TheCamera.TakeControlNoEntity(GetPosition() + lookAtDir, eSwitchType::JUMPCUT, 1);
        };


        if (const auto door = CEntryExitManager::FindNearestDoor(*this, 10.f)) {
            // 0x4401AB

            AddPedScriptCommand(new CTaskComplexGotoDoorAndOpen(door));
            ms_pDoor = door;

            SetupFixedCamera(Normalized(CVector{ CVector2D{door->GetPosition()} - GetPosition2D(), 0.f }));
        } else {
            // 0x440246
            if (ms_bWarping) {
                bUnknownPairing = true;
                return true;
            }
            AddPedScriptCommand(new CTaskComplexGotoDoorAndOpen(GetPosition(), GetPosition() + spawnPointExitToUsDir * 4.f));

            // Como el original: sin puerta también se fija la cámara (0x440309), mirando hacia la salida
            SetupFixedCamera(spawnPointExitToUsDir);
        }
    }

    CPad::GetPad()->bPlayerOnInteriorTransition = true;
    if (!ped->bInVehicle) {
        AddPedScriptCommand(new CTaskSimpleUninterruptable);
    }

    return true;
}

// 0x4404A0
bool CEntryExit::TransitionFinished(CPed* ped) {
    auto spawnPos = ms_spawnPoint->m_vecExitPos;

    if (const auto entity = ped->GetEntityThatThisPedIsHolding()) {
        entity->SetAreaCode((eAreaCodes)ms_spawnPoint->m_nArea);
    }

    const auto DisplayEnExName = [this]{
        if (const auto enex = GetEntryExitToDisplayNameOf()) {
            CHud::SetZoneName(TheText.Get(enex->m_szName), true);
        }
    };

    if ((bFoodDateFlag || bUnknownPairing) || ped->bInVehicle) {
        switch (CEntryExitManager::ms_exitEnterState) {
        case EXIT_ENTER_STATE_0: { // 0x440ABE
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(0.5f, eFadeFlag::FADE_IN);
            CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_2;

            if (const auto pedsGroup = CPedGroups::GetPedsGroup(ped)) {
                CEventGroupEvent groupEvent{ ped, new CEventLeaderEntryExit(ped) };
                pedsGroup->GetIntelligence().AddEvent(&groupEvent);
            }
            DisplayEnExName();
            return false;
        }
        default: { // 0x44068F
            CGame::currArea = (eAreaCodes)(uint8)CEntryExit::ms_spawnPoint->m_nArea.get_underlying(); // Como el original: movzx
            CEntryExitManager::ms_numVisibleEntities = 0;
            break;
        }
        case EXIT_ENTER_STATE_2: { // 0x440A92
            if (!TheCamera.GetFading()) {
                CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_3;
            }
            return false;
        }
        }
    } else { // 0x44051B
        CColStore::AddCollisionNeededAtPosn(spawnPos);
        CIplStore::AddIplsNeededAtPosn(spawnPos);

        if (ms_bWarping) { // 0x440534
            CRenderer::m_loadingPriority = false;
            CStreaming::AddModelsToRequestList(spawnPos, STREAMING_LOADING_SCENE);
            ms_spawnPoint->RequestObjectsInFrustum();
            ThePaths.SetPathsNeededAtPosition(spawnPos);
        }

        switch (CEntryExitManager::ms_exitEnterState) {
        case 0: { // 0x4405E8
            CEntryExitManager::SetAreaCodeForVisibleObjects();
            CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_1;
            CGame::currArea = (eAreaCodes)(uint8)ms_spawnPoint->m_nArea.get_underlying(); // Como el original: movzx

            if (const auto pedsGroup = CPedGroups::GetPedsGroup(ped)) {
                CEventGroupEvent groupEvent{ ped, new CEventLeaderEntryExit(ped) };
                pedsGroup->GetIntelligence().AddEvent(&groupEvent);
            }

            return false;
        }
        case 1: { // 0x4405A1
            const auto primaryTask = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY);
            if (!primaryTask || primaryTask->GetTaskType() != TASK_COMPLEX_GOTO_DOOR_AND_OPEN) {
                TheCamera.SetFadeColour(0, 0, 0);
                TheCamera.Fade(1.f, eFadeFlag::FADE_IN);
                CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_2;
            }
            DisplayEnExName();
            return false;
        }
        case 2: { // 0x44057F
            if (!TheCamera.GetFading()) {
                CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_3;
                break;
            }
            return false;
        }
        }
    }

    // ms_exitEnterState == 3
    ped->SetAreaCode((eAreaCodes)CGame::currArea);
    if (ped->m_pVehicle && ped->bInVehicle) {
        ped->m_pVehicle->SetAreaCode((eAreaCodes)CGame::currArea);
    }
    ped->AsPlayer()->m_pEnex = CGame::currArea == AREA_CODE_NORMAL_WORLD ? nullptr : this;

    CEntryExitManager::AddEntryExitToStack(this);

    ped->bCanExitCar = true;
    CPad::GetPad()->bPlayerOnInteriorTransition = false;

    CClothes::RebuildPlayerIfNeeded(ped->AsPlayer());

    if (bFoodDateFlag) {
        ms_spawnPoint->bEnteredWithoutExit = false;
        return true;
    }

    TheCamera.SetPosn(spawnPos);
    TheCamera.RestoreWithJumpCut();

    CAudioZones::Update(true, spawnPos);

    CWaterLevel::FindNearestWaterAndItsFlow();
    CGarages::CloseHideOutGaragesBeforeSave();
    CEntryExitManager::ResetAreaCodeForVisibleObjects();
    g_interiorMan.SetEntryExitPtr(this);
    CPopulation::RemoveAllRandomPeds();
    RequestAmbientPeds();
    CStreaming::LoadAllRequestedModels(0);
    CTimer::Suspend();

    if (CGame::currArea == AREA_CODE_NORMAL_WORLD) {
        RwCameraSetFarClipPlane(TheCamera.m_pRwCamera, CTimeCycle::FindFarClipForCoors(spawnPos));
    }

    ms_spawnPoint->RequestObjectsInFrustum();
    CStreaming::LoadScene(spawnPos);
    CStreaming::LoadAllRequestedModels(0);
    GenerateAmbientPeds(spawnPos);

    if (g_interiorMan.Update()) {
        CStreaming::SetLoadVehiclesInLoadScene(false);
        CStreaming::LoadScene(spawnPos);
        CStreaming::SetLoadVehiclesInLoadScene(true);
    }

    CTimer::Resume();

    CStreaming::ClearFlagForAll(STREAMING_LOADING_SCENE);

    if (ms_spawnPoint->m_nSkyColor) {
        CTimeCycle::StartExtraColour(ms_spawnPoint->m_nSkyColor - 1, 0);
    } else {
        CTimeCycle::StopExtraColour(0);
    }

    CRubbish::SetVisibility(ms_spawnPoint->bUnknownInterior);

    if (ped->bInVehicle) {
        CVector teleportPos = spawnPos;
        teleportPos.z = ped->m_pVehicle->GetHeightAboveRoad() + (teleportPos.z - 1.f);
        ped->m_pVehicle->Teleport(teleportPos, false);
        ped->m_pVehicle->SetHeading(DegreesToRadians(ms_spawnPoint->m_fExitAngle));
    } else {
        ms_spawnPoint->FindValidTeleportPoint(spawnPos);
        ped->Teleport(spawnPos, false);

        ped->m_fCurrentRotation = DegreesToRadians(ms_spawnPoint->m_fExitAngle);
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        ped->SetHeading(ped->m_fCurrentRotation);
    }

    TheCamera.Fade(1.f, eFadeFlag::FADE_OUT);

    CTheScripts::ClearSpaceForMissionEntity(ped->GetPosition(), ped);

    if (ms_spawnPoint->bUsedRewardEntrance) {
        CShopping::RemoveLoadedShop();
    } else if (ms_spawnPoint->bRewardInterior) {
        CShopping::LoadShop(m_szName);
        if (m_pLink) {
            bUsedRewardEntrance = true;
        }
    }

    CPopulation::ManageAllPopulation();
    CTheScripts::Process();

    if (ms_spawnPoint->bAcceptNpcGroup) {
        WarpGangWithPlayer(ped->AsPlayer());
    }

    ProcessStealableObjects(ped);

    ms_spawnPoint->bEnteredWithoutExit = false;
    if (ms_spawnPoint->bDeleteEnex) {
        CEntryExitManager::DeleteOne(CEntryExitManager::GetPool()->GetIndex(ms_spawnPoint));
    }

    auto task = static_cast<CTaskComplexFacial*>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
    task->StopAll();

    CGame::TidyUpMemory(true, true);

    return true;
}

// 0x43E6D0
void CEntryExit::RequestAmbientPeds() {
    // Como el original: lee CGame::currArea directamente; en el mundo exterior (área 0) quita los peds dudosos
    if (CGame::currArea == AREA_CODE_NORMAL_WORLD) {
        CStreaming::RemoveDodgyPedsFromRandomSlots();
        return;
    }

    constexpr struct { const char* name; int32 models[TOTAL_LOADED_PEDS]; } mapping[]{
        { 
            "bar1", 
            {MODEL_BFYRI, MODEL_OFYST, MODEL_WFYST, MODEL_WMYST, MODEL_BMYRI, MODEL_BMYST, MODEL_OMOST, MODEL_OMYST} 
        },
        { 
            "strip2", 
            {MODEL_SBFYSTR, MODEL_SWFYSTR, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID} 
        },
        { 
            "LAstrip", 
            {MODEL_VWFYST1, MODEL_VBFYST2, MODEL_VHFYST3, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID} 
        },
        { 
            "MAFCAS", 
            {MODEL_BFORI, MODEL_HFYRI, MODEL_OMYRI, MODEL_WMYRI, MODEL_OFYST, MODEL_VHMYELV, MODEL_WMOST, MODEL_BMORI} 
        },
        { 
            "TRICAS", 
            {MODEL_BFYRI, MODEL_BMYRI, MODEL_HFORI, MODEL_HMORI, MODEL_WMORI, MODEL_WFYRI, MODEL_OMOST, MODEL_VBMYELV} 
        },
    };

    // Como el original: compara todos los nombres en orden (_stricmp(nombre, m_szName) == 0), sin salir antes
    for (auto&& [name, models] : mapping) {
        if (_stricmp(name, m_szName) == 0) {
            CStreaming::StreamPedsIntoRandomSlots(models);
        }
    }
}
// 0x43E690
void CEntryExit::RequestObjectsInFrustum() const {
    CRenderer::RequestObjectsInDirection(ms_spawnPoint->m_vecExitPos, m_fExitAngle, STREAMING_LOADING_SCENE); // Como el original: push 20h
}

// 0x43F1F0
void CEntryExit::WarpGangWithPlayer(CPlayerPed* player) {
    // Como el original: índice del grupo leído de m_pPlayerData->m_nPlayerGroup, activo = ms_activeGroups[id]
    const auto groupId = player->GetPlayerData()->m_nPlayerGroup;
    if (!CPedGroups::ms_activeGroups.data()[groupId]) {
        return;
    }

    auto& ms = CPedGroups::ms_groups.data()[groupId].GetMembership();
    if (!ms.IsLeader(player)) {
        return;
    }

    const auto& offsets = CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets;
    constexpr int32 NumOffsets = TOTAL_PED_GROUP_MEMBERS; // Como el original: cmp esi, 40h (8 desplazamientos de 8 bytes)

    // Como el original: sólo los 7 seguidores (GetMember 0..6). Cada uno prueba como mucho 3 desplazamientos
    // consecutivos alrededor del JUGADOR; si los 3 fallan (o se acaban los desplazamientos) termina la función.
    int32 offsetIdx = 0;
    for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        const auto mem = ms.GetMember(i);
        if (!mem || mem == player || offsetIdx >= NumOffsets) {
            continue;
        }

        const auto& plyrPos = player->GetPosition();
        CVector memTeleportTo{ offsets[offsetIdx].x + plyrPos.x, offsets[offsetIdx].y + plyrPos.y, plyrPos.z };
        offsetIdx++;
        if (!IsTeleportPointValid(plyrPos, memTeleportTo)) {
            if (offsetIdx >= NumOffsets) {
                return;
            }
            memTeleportTo = CVector{ offsets[offsetIdx].x + plyrPos.x, offsets[offsetIdx].y + plyrPos.y, plyrPos.z };
            offsetIdx++;
            if (!IsTeleportPointValid(plyrPos, memTeleportTo)) {
                if (offsetIdx >= NumOffsets) {
                    return;
                }
                // Como el original: el tercer intento usa operator+ (0x40FE30) con z = 0
                memTeleportTo = plyrPos + CVector{ offsets[offsetIdx].x, offsets[offsetIdx].y, 0.f };
                offsetIdx++;
                if (!IsTeleportPointValid(plyrPos, memTeleportTo)) {
                    return;
                }
            }
        }

        // Como el original: dirección 2D normalizada (0x59C910) y GetRadianAngleBetweenPoints(x, y, 0, 0) + LimitRadianAngle
        const auto& memPos = mem->GetPosition();
        CVector dir{ plyrPos.x - memPos.x, plyrPos.y - memPos.y, 0.f };
        dir.Normalise();
        const auto memHeading = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f));

        // Teleport them
        mem->Teleport(memTeleportTo, false);
        mem->GetIntelligence()->FlushImmediately(false);

        // Make the member be heading towards the player
        mem->m_fCurrentRotation = mem->m_fAimingRotation = memHeading;
        mem->SetHeading(memHeading);
        mem->SetAreaCode(player->GetAreaCode());
        mem->m_pEnex = player->m_pEnex;
    }
}

// 0x43E990
void CEntryExit::ProcessStealableObjects(CPed* ped) {
    const auto helde = ped->GetEntityThatThisPedIsHolding();
    if (!helde) {
        return;
    }

    if (!helde->GetIsTypeObject()) {
        return; 
    }

    if (!helde->AsObject()->objectFlags.bIsLiftable) {
        return;
    }

    const auto heldobj = helde->AsObject();
    switch (heldobj->m_nObjectType) {
    case OBJECT_MISSION:
    case OBJECT_MISSION2:
        break;
    default: {
        if (ms_spawnPoint->GetArea() != AREA_CODE_NORMAL_WORLD) {
            if (g_interiorMan.FindStealableObjectId(heldobj) != -1) {
                g_interiorMan.SetStealableObjectStolen(heldobj, false);
            }
        } else {
            g_interiorMan.SetStealableObjectStolen(heldobj, true);
        }
        break;
    }
    }
    heldobj->SetAreaCode(ms_spawnPoint->GetArea());
}
