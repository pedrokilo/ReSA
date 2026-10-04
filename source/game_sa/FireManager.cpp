/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CFireManager
 * Reconstrucción: COMPLETA — 24/24 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/24 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FireManager.h"
#include "TheScripts.h"
#include "Shadows.h"

void CFireManager::InjectHooks() {
    RH_ScopedClass(CFireManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x539DA0);
    RH_ScopedInstall(Destructor, 0x538BB0);
    RH_ScopedInstall(Init, 0x538BC0);
    RH_ScopedInstall(Shutdown, 0x539DD0);
    RH_ScopedInstall(GetNumOfNonScriptFires, 0x538F10);
    RH_ScopedInstall(FindNearestFire, 0x538F40);
    RH_ScopedInstall(PlentyFiresAvailable, 0x539340);
    RH_ScopedInstall(ExtinguishPoint, 0x539450);
    RH_ScopedInstall(ExtinguishPointWithWater, 0x5394C0);
    RH_ScopedInstall(IsScriptFireExtinguished, 0x5396E0);
    RH_ScopedInstall(RemoveScriptFire, 0x539700);
    RH_ScopedInstall(RemoveAllScriptFires, 0x539720);
    RH_ScopedInstall(ClearAllScriptFireFlags, 0x5397A0);
    RH_ScopedInstall(SetScriptFireAudio, 0x5397B0);
    RH_ScopedInstall(GetScriptFireCoords, 0x5397E0);
    RH_ScopedInstall(GetNumFiresInRange, 0x5397F0);
    RH_ScopedInstall(GetNumFiresInArea, 0x539860);
    RH_ScopedInstall(DestroyAllFxSystems, 0x539D10);
    RH_ScopedInstall(CreateAllFxSystems, 0x539D50);
    RH_ScopedInstall(GetNextFreeFire, 0x539E50);
    RH_ScopedOverloadedInstall(StartFire, "NoTarget", 0x539F00, CFire*(CFireManager::*)(CVector, float, uint8, CEntity*, uint32, int8, uint8));
    RH_ScopedOverloadedInstall(StartFire, "", 0x53A050, CFire *(CFireManager::*)(CEntity*, CEntity*, float, uint8, uint32, int8));
    RH_ScopedInstall(StartScriptFire, 0x53A270);
    RH_ScopedInstall(Update, 0x53AF00);
}

// 0x539DA0
CFireManager::CFireManager() {
    Init();
}

CFireManager* CFireManager::Constructor() {
    this->CFireManager::CFireManager();
    return this;
}

CFireManager* CFireManager::Destructor() {
    this->CFireManager::~CFireManager();
    return this;
}

// 0x538BC0
void CFireManager::Init() {
    for (CFire& fire : m_aFires) {
        fire.Initialise();
    }
    m_nMaxFireGenerationsAllowed = 1'000'000 - 1; // aka 999'999
}

// 0x539DD0
void CFireManager::Shutdown() {
    for (CFire& fire : m_aFires) {
        fire.SetIsScript(false);
        if (fire.IsActive()) {
            fire.Extinguish();
        }
    }
}

// 0x538F10
uint32 CFireManager::GetNumOfNonScriptFires() {
    uint32 c = 0;
    for (CFire& fire : m_aFires)
        if (fire.IsActive() && !fire.IsScript())
            c++;
    return c;
}

// NOTSA
uint32 CFireManager::GetNumOfFires() {
    uint32 c = 0;
    for (CFire& fire : m_aFires) {
        if (fire.IsActive())
            c++;
    }
    return c;
}

// 0x538F40
CFire* CFireManager::FindNearestFire(const CVector& point, bool bCheckIsBeingExtinguished, bool bCheckWasCreatedByScript) {
    float fNearestDist2DSq = std::numeric_limits<float>::max(); // 0x863A3C
    CFire* nearestFire{};
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive())
            continue;

        if (bCheckWasCreatedByScript && fire.IsScript())
            continue;

        if (bCheckIsBeingExtinguished && fire.IsBeingExtinguished())
            continue;

        if (fire.GetEntityOnFire() && fire.GetEntityOnFire()->GetIsTypePed())
            continue;

        const float dx = fire.m_Position.x - point.x, dy = fire.m_Position.y - point.y;
        const float fDist2DSq = dx * dx + dy * dy;
        if (fDist2DSq < fNearestDist2DSq) {
            fNearestDist2DSq = fDist2DSq;
            nearestFire = &fire;
        }
    }
    return nearestFire;
}

// 0x539340
bool CFireManager::PlentyFiresAvailable() {
    size_t inactive = 0;
    for (auto& fire : m_aFires) {
        if (fire.IsActive()) {
            continue;
        }
        if (++inactive >= 6) {
            return true;
        }
    }
    return false;
}

// Como el original: distancia al cuadrado (y² + x²) + z², con punto - fuego.
static float FireDistSq3D(const CVector& point, const CVector& firePos) {
    const float dz = point.z - firePos.z, dy = point.y - firePos.y, dx = point.x - firePos.x;
    return dy * dy + dx * dx + dz * dz;
}

// 0x539450
void CFireManager::ExtinguishPoint(CVector point, float fRadius) {
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive())
            continue;

        // Como el original: solo si r² > d² estrictamente (la igualdad y NaN no apagan)
        if (!(fRadius * fRadius > FireDistSq3D(point, fire.m_Position)))
            continue;

        fire.SetIsScript(false);
        fire.Extinguish();
    }
}

// 0x5394C0
bool CFireManager::ExtinguishPointWithWater(CVector point, float fRadius, float fWaterStrength) {
    bool bSuccess = false;
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive())
            continue;

        if (!(fRadius * fRadius > FireDistSq3D(point, fire.m_Position)))
            continue;

        fire.ExtinguishWithWater(fWaterStrength);
        bSuccess = true;
    }
    return bSuccess;
}

// 0x5396E0
bool CFireManager::IsScriptFireExtinguished(int16 id) {
    // Como el original: acceso directo sin comprobar (movsx del índice)
    return !m_aFires.data()[id].IsActive();
}

// 0x539700
void CFireManager::RemoveScriptFire(int16 fireID) {
    CFire& fire = m_aFires.data()[fireID];
    if (fire.IsScript()) {
        fire.SetIsScript(false);
        fire.Extinguish();
    }
}

// 0x539720
void CFireManager::RemoveAllScriptFires() {
    for (CFire& fire : m_aFires) {
        if (fire.IsScript()) {
            fire.SetIsScript(false);
            fire.Extinguish();
        }
    }
}

// 0x5397A0
void CFireManager::ClearAllScriptFireFlags() {
    for (CFire& fire : m_aFires) {
        fire.SetIsScript(false);
    }
}

// 0x5397B0
void CFireManager::SetScriptFireAudio(int16 fireID, bool makesNoise) {
    m_aFires.data()[fireID].SetMakesNoise(makesNoise);
}

// 0x5397E0
const CVector& CFireManager::GetScriptFireCoords(int16 fireID) {
    return m_aFires.data()[fireID].m_Position;
}

// 0x5397F0
uint32 CFireManager::GetNumFiresInRange(const CVector& point, float fRadius) {
    uint32 c = 0;
    for (CFire& fire : m_aFires) {
        if (fire.IsActive() && !fire.IsScript()) {
            const float dx = fire.m_Position.x - point.x, dy = fire.m_Position.y - point.y;
            if (std::sqrt(dy * dy + dx * dx) <= fRadius) {
                c++;
            }
        }
    }
    return c;
}

// 0x539860
uint32 CFireManager::GetNumFiresInArea(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
    uint32 c = 0;
    for (CFire& fire : m_aFires) {
        if (fire.IsActive() && !fire.IsScript()) {
            // Como el original: x, y, z contra min y max por ese orden; NaN queda fuera
            const CVector& p = fire.m_Position;
            if (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY && p.z >= minZ && p.z <= maxZ) {
                c++;
            }
        }
    }
    return c;
}

// 0x539D10
void CFireManager::DestroyAllFxSystems() {
    for (CFire& fire : m_aFires) {
        // Como el original: FxManager_c::DestroyFxSystem (no FxSystem_c::Kill)
        if (fire.IsActive() && fire.m_FxSystem) {
            g_fxMan.DestroyFxSystem(fire.m_FxSystem);
            fire.m_FxSystem = nullptr;
        }
    }
}

// 0x539D50
void CFireManager::CreateAllFxSystems() {
    for (CFire& fire : m_aFires) {
        // Como el original: solo fuegos activos sin efecto
        if (fire.IsActive() && !fire.m_FxSystem) {
            fire.CreateFxSysForStrength(fire.m_Position, nullptr);
        }
    }
}

// 0x539E50
CFire* CFireManager::GetNextFreeFire(bool bMayExtinguish) {
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive() && !fire.IsScript()) {
            return &fire;
        }
    }

    if (!bMayExtinguish)
        return nullptr;

    // Como el original: se recicla el primero que no sea a la vez de script y de primera generación
    for (CFire& fire : m_aFires) {
        if (!fire.IsScript() || !fire.IsFirstGen()) {
            fire.SetIsScript(false);
            fire.Extinguish();
            return &fire;
        }
    }
    return nullptr;
}

// Como el original: mínimo con signo entre las generaciones (movsx) y el máximo del gestor global (0xB728E0)
static uint8 ClampFireGenerations(int8 nGenerations) {
    const auto maxGens = (int32)gFireManager.m_nMaxFireGenerationsAllowed;
    return (uint8)((int32)nGenerations < maxGens ? (int32)nGenerations : maxGens);
}

// 0x539F00
CFire* CFireManager::StartFire(CVector pos, float size, uint8 unused, CEntity* creator, uint32 nTimeToBurn, int8 nGenerations, uint8 unused_) {
    if (CWaterLevel::IsPointUnderwaterNoWaves(pos))
        return nullptr;

    if (CFire* fire = GetNextFreeFire(false)) {
        fire->Start(creator, pos, nTimeToBurn, ClampFireGenerations(nGenerations));
        return fire;
    }
    return nullptr;
}

// 0x53A050
CFire* CFireManager::StartFire(CEntity* target, CEntity* creator, float size, uint8 unused, uint32 lifetime, int8 numGenerations) {
    switch (target->GetType()) {
    case ENTITY_TYPE_PED: {
        auto pedTarget = target->AsPed();
        if (pedTarget->m_pFire) // Como el original: m_pFire antes que IsPedInControl
            return nullptr;
        if (!pedTarget->IsPedInControl())
            return nullptr;
        if (pedTarget->physicalFlags.bFireProof)
            return nullptr;
        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        auto vehTarget = target->AsVehicle();
        if (vehTarget->m_pFire)
            return nullptr;

        if (vehTarget->IsAutomobile()) {
            if (target->AsAutomobile()->m_damageManager.GetEngineStatus() >= 225)
                return nullptr;
        }

        if (vehTarget->physicalFlags.bFireProof)
            return nullptr;
        break;
    }
    case ENTITY_TYPE_OBJECT: {
        if (target->AsObject()->m_pFire)
            return nullptr;
        break;
    }
    }

    CFire* fire = GetNextFreeFire(false);
    if (fire) {
        fire->Start(creator, target, lifetime, ClampFireGenerations(numGenerations));
    }
    return fire; // Como el original: devuelve el fuego (la DLL devolvía siempre nullptr)
}

// 0x53A270
int32 CFireManager::StartScriptFire(const CVector& pos, CEntity* target, float _fUnused, uint8 _nUnused, int8 nGenerations, int32 nStrength) {
    if (target) {
        switch (target->GetType()) {
        case ENTITY_TYPE_PED: {
            auto pedTarget = target->AsPed();
            if (pedTarget->m_pFire) {
                pedTarget->m_pFire->SetIsScript(false);
                pedTarget->m_pFire->Extinguish();
            }
            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            auto vehTarget = target->AsVehicle();
            if (vehTarget->m_pFire) {
                vehTarget->m_pFire->SetIsScript(false);
                vehTarget->m_pFire->Extinguish();
            }

            if (vehTarget->IsAutomobile()) {
                auto& dmgMgr = vehTarget->AsAutomobile()->m_damageManager;
                if (dmgMgr.GetEngineStatus() >= 225)
                    dmgMgr.SetEngineStatus(215);
            }
            break;
        }
        }
    }

    if (auto fire = GetNextFreeFire(true)) {
        fire->Start(pos, (float)nStrength, target, ClampFireGenerations(nGenerations));
        return CTheScripts::GetNewUniqueScriptThingIndex((int32)GetIndexOf(fire), SCRIPT_THING_FIRE);
    }
    return -1;
}

// Como el original: fsin/fcos x87 a precisión completa y producto redondeado
static float FireSinMul(float angle, float mult) {
    float res;
    __asm {
        fld dword ptr [angle]
        fsin
        fmul dword ptr [mult]
        fstp dword ptr [res]
    }
    return res;
}

static float FireCosMul(float angle, float mult) {
    float res;
    __asm {
        fld dword ptr [angle]
        fcos
        fmul dword ptr [mult]
        fstp dword ptr [res]
    }
    return res;
}

static void FireRegisterCorona(uint32 id, uint8 red, uint8 green, uint8 blue, const CVector& pos, float coronaSize, eCoronaFlareType flare) {
    CCoronas::RegisterCorona(id, nullptr, red, green, blue, 0xFF, pos, coronaSize, 70.0f, eCoronaType::CORONATYPE_HEADLIGHT, flare,
        eCoronaReflType::CORREFL_NONE, eCoronaLOSCheck::LOSCHECK_OFF, eCoronaTrail::TRAIL_OFF, 0.0f, false, 1.5f, false, 15.0f, false, false);
}

// 0x53AF00
void CFireManager::Update() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    for (CFire& fire : m_aFires) {
        if (fire.IsActive())
            fire.ProcessFire();
    }

    if (CGameLogic::LaRiotsActiveHere()
     && CTimer::GetTimeInMS() / 500u != CTimer::GetPreviousTimeInMS() / 500u
    ) {
        // Como el original: rand * (1/32767) * 2pi y rand * (1/32767) * 25 + 35 (floats del EXE)
        const float fRandomAngleRad = (float)(int32)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL * 6.2831855f;
        const float fRandomDist = (float)(int32)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL * 25.0f + 35.0f;
        const float offX = FireSinMul(fRandomAngleRad, fRandomDist);
        const float offY = FireCosMul(fRandomAngleRad, fRandomDist);

        CVector point;
        {
            const CVector& camPos = TheCamera.GetPosition();
            point.x = offX + camPos.x;
            point.y = offY + camPos.y;
            point.z = camPos.z + 10.0f;
        }

        CEntity* hitEntity;
        bool bHit;
        point.z = CWorld::FindGroundZFor3DCoord(point, &bHit, &hitEntity);

        const CVector& camPos = TheCamera.GetPosition();
        // Como el original: dirección punto - cámara (la DLL usaba cámara - punto)
        CVector camToPointDir{ point.x - camPos.x, point.y - camPos.y, point.z - camPos.z };
        camToPointDir.Normalise();

        // Como el original: sin matriz se usa el heading del placement (-sin, cos, 0)
        CVector camFwd;
        if (TheCamera.m_matrix) {
            camFwd = TheCamera.m_matrix->GetForward();
        } else {
            const float heading = TheCamera.m_placement.m_fHeading;
            float sinH, cosH;
            __asm {
                fld dword ptr [heading]
                fsin
                fchs
                fstp dword ptr [sinH]
                fld dword ptr [heading]
                fcos
                fstp dword ptr [cosH]
            }
            camFwd = CVector{ sinH, cosH, 0.0f };
        }

        // Como el original: solo con suelo encontrado sobre una entidad con el bit 0 de +0x1E y z > 0.5
        if (hitEntity && (reinterpret_cast<const uint8*>(hitEntity)[0x1E] & 1) && bHit && point.z > 0.5f) {
            const float dot = camFwd.z * camToPointDir.z + camFwd.y * camToPointDir.y + camFwd.x * camToPointDir.x;
            if ((double)dot > 0.2 /* double del EXE (0x862D08) */ || CGeneral::GetRandomNumber() < 0x3FFF) {
                if (auto fx = g_fxMan.CreateFxSystem("riot_smoke", point, nullptr, true))
                    fx->PlayAndKill();
            } else {
                StartFire(point, CGeneral::GetRandomNumberInRange(0.5f, 2.5f), 1, nullptr, 30000u, 5, 1);
            }
        }
    }

    // Como el original: color base leído del EXE (0x8CCFF0..2 = 64, 50, 32)
    const auto* baseColor = reinterpret_cast<const uint8*>(0x8CCFF0);

    auto nFires = (int32)GetNumOfFires();
    bool firesVisited[MAX_NUM_FIRES] = { false };
    while (nFires > 0) {
        // Como el original: el más fuerte parte de -1.0 (si ninguno lo supera, el índice queda en -1)
        float fBest = -1.0f;
        int32 strongestIdx = -1;
        for (int32 i = 0; i < MAX_NUM_FIRES; i++) {
            CFire& fire = m_aFires[i];
            if (firesVisited[i] || !fire.IsActive())
                continue;
            if (fBest < fire.m_Strength) {
                fBest = fire.m_Strength;
                strongestIdx = i;
            }
        }
        CFire* strongest = m_aFires.data() + strongestIdx;

        float fCombinedStrength = 0.0f;
        int32 nCombinedCeilStrength = 0;
        for (int32 i = 0; i < MAX_NUM_FIRES; i++) {
            CFire& fire = m_aFires[i];
            if (firesVisited[i] || !fire.IsActive())
                continue;

            const float dx = fire.m_Position.x - strongest->m_Position.x, dy = fire.m_Position.y - strongest->m_Position.y;
            if (std::sqrt(dx * dx + dy * dy) < 6.0f) {
                fCombinedStrength = fCombinedStrength + fire.m_Strength;
                nCombinedCeilStrength += (int32)(int64)std::ceil((double)fire.m_Strength); // CRT ceil + _ftol2
                nFires--;
                firesVisited[i] = true;
            }
        }

        if (fCombinedStrength > 4.0f && nCombinedCeilStrength) {
            const float t = fCombinedStrength - 6.0f + 3.0f;
            const float fDir = (7.0f < t) ? 7.0f : t;

            // Como el original: 0.39999998 (0x3ECCCCCC) y 0.6
            float fColorMult = (float)(int32)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL * std::bit_cast<float>(0x3ECCCCCCu) + 0.6f;

            CVector shdwPos = strongest->m_Position;
            shdwPos.z = shdwPos.z + 5.0f;
            {
                const auto blue  = (uint8)(int64)((float)baseColor[2] * fColorMult);
                const auto green = (uint8)(int64)((float)baseColor[1] * fColorMult);
                const auto red   = (uint8)(int64)((float)baseColor[0] * fColorMult);
                // Como el original: frontX = dir * 1.2 y sideY = dir * -1.2
                CShadows::StoreStaticShadow(
                    reinterpret_cast<uint32>(strongest),
                    SHADOW_ADDITIVE,
                    gpShadowExplosionTex,
                    shdwPos,
                    fDir * 1.2f,
                    0.0f,
                    0.0f,
                    fDir * -1.2f,
                    0,
                    red,
                    green,
                    blue,
                    10.0f,
                    1.0f,
                    40.0f,
                    false,
                    0.0f
                );
            }

            if (fCombinedStrength > 6.0f) {
                fColorMult = fColorMult * 0.8f;
                const auto blue  = (uint8)(int64)((float)baseColor[2] * fColorMult);
                const auto green = (uint8)(int64)((float)baseColor[1] * fColorMult);
                const auto red   = (uint8)(int64)((float)baseColor[0] * fColorMult);

                CVector point = strongest->m_Position;
                point.z = point.z + 2.6f;
                const CVector& camPos = TheCamera.GetPosition();
                CVector toCam{ camPos.x - point.x, camPos.y - point.y, camPos.z - point.z };
                toCam.Normalise();
                point = CVector{ toCam.x * 3.5f + point.x, toCam.y * 3.5f + point.y, toCam.z * 3.5f + point.z };

                const float coronaSize = fDir * 0.5f;
                const auto strongestId = reinterpret_cast<uint32>(strongest);

                // Como el original: ids consecutivos (id, id+1, id+2, id+3)
                FireRegisterCorona(strongestId, red, green, blue, point, coronaSize, eCoronaFlareType::FLARETYPE_HEADLIGHTS);

                point.z = point.z + 2.0f;
                FireRegisterCorona(strongestId + 1, red, green, blue, point, coronaSize, eCoronaFlareType::FLARETYPE_NONE);

                point.z = point.z - 2.0f;
                // Como el original: derecha de m_mCameraMatrix (0xB6F99C) con z = 0
                CVector camRight{ TheCamera.m_mCameraMatrix.GetRight().x, TheCamera.m_mCameraMatrix.GetRight().y, 0.0f };
                camRight.Normalise();
                point = CVector{ camRight.x * 2.0f + point.x, camRight.y * 2.0f + point.y, camRight.z * 2.0f + point.z };
                FireRegisterCorona(strongestId + 2, red, green, blue, point, coronaSize, eCoronaFlareType::FLARETYPE_NONE);

                point = CVector{ point.x - camRight.x * 4.0f, point.y - camRight.y * 4.0f, point.z - camRight.z * 4.0f };
                FireRegisterCorona(strongestId + 3, red, green, blue, point, coronaSize, eCoronaFlareType::FLARETYPE_NONE);
            }
        }
    }
}

// NOTSA
CFire& CFireManager::GetRandomFire() {
    return CGeneral::RandomChoice(m_aFires);
}
