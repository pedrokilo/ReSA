/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CStuntJumpManager
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "StuntJumpManager.h"
#include "Hud.h"

static constexpr uint16 STUNT_JUMP_COUNT = 256;

void CStuntJumpManager::InjectHooks() {
    RH_ScopedClass(CStuntJumpManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49CA50);
    RH_ScopedInstall(Shutdown, 0x49CBC0);
    RH_ScopedInstall(ShutdownForRestart, 0x49CB10);
    RH_ScopedInstall(Save, 0x5D5570);
    RH_ScopedInstall(Load, 0x5D5920);
    RH_ScopedInstall(AddOne, 0x49CB40);
    RH_ScopedInstall(Update, 0x49C490);
}

namespace {
// Como el original: el EXE manipula el pool de saltos con su propia instancia de CPool<CStuntJump>
// (ctor 0x49C8F0, Flush inline, Clear 0x49C9A0, New 0x49C9E0, GetAt 0x49C3B0), sin rellenos de depuración.
struct StuntJumpPoolRaw {
    CStuntJump* storage;
    uint8*      states;
    int32       size;
    int32       lastFree;
    bool        owns;
    bool        dealWithNoMemory;
};
VALIDATE_SIZE(StuntJumpPoolRaw, sizeof(CStuntJumpsPool));

StuntJumpPoolRaw* RawPool() {
    return reinterpret_cast<StuntJumpPoolRaw*>(CStuntJumpManager::mp_poolStuntJumps);
}

// 0x49C9E0
CStuntJump* StuntJumpPoolNew(StuntJumpPoolRaw* pool) {
    bool wrapped = false;
    for (;;) {
        pool->lastFree++;
        if (pool->lastFree == pool->size) {
            pool->lastFree = 0;
            if (wrapped) {
                return nullptr;
            }
            wrapped = true;
        }
        if (pool->states[pool->lastFree] & 0x80) {
            break;
        }
    }
    auto& state = pool->states[pool->lastFree];
    state &= 0x7F;
    state = (uint8)((state & 0x80) | ((state + 1) & 0x7F));
    return &pool->storage[pool->lastFree];
}

// Como el original: tiempo de paso en ms = ftol(ms_fTimeStep * 0.02f * 1000.0f)
int32 StuntJumpTimeStepInMS() {
    const float ms = CTimer::ms_fTimeStep * 0.02f * 1000.0f;
    return (int32)ms;
}

void SetJumpState(eJumpState state) {
    // Como el original: m_jumpState se escribe como dword
    *reinterpret_cast<uint32*>(&CStuntJumpManager::m_jumpState) = (uint32)state;
}
} // namespace

// 0x49CA50
void CStuntJumpManager::Init() {
    ZoneScoped;

    // Como el original: operator new(0x14) + ctor del pool 0x49C8F0 (sin rellenar el almacenamiento ni tocar el byte +0x11)
    auto* pool = static_cast<StuntJumpPoolRaw*>(::operator new(sizeof(StuntJumpPoolRaw)));
    if (pool) {
        pool->storage  = static_cast<CStuntJump*>(::operator new[](STUNT_JUMP_COUNT * sizeof(CStuntJump)));
        pool->states   = static_cast<uint8*>(::operator new[](STUNT_JUMP_COUNT));
        pool->owns     = true;
        pool->size     = STUNT_JUMP_COUNT;
        pool->lastFree = -1;
        for (auto i = 0; i < STUNT_JUMP_COUNT; i++) {
            pool->states[i] |= 0x80;
            pool->states[i] &= 0x80;
        }
    }
    mp_poolStuntJumps = reinterpret_cast<CStuntJumpsPool*>(pool);
    m_bActive         = true;
}

// 0x49CBC0
void CStuntJumpManager::Shutdown() {
    // Como el original: Flush inline del pool y operator delete del propio pool
    if (auto* pool = RawPool()) {
        if (pool->size > 0) {
            if (pool->owns) {
                ::operator delete[](pool->storage);
                ::operator delete[](pool->states);
            }
            pool->storage  = nullptr;
            pool->states   = nullptr;
            pool->size     = 0;
            pool->lastFree = 0;
        }
        ::operator delete(pool);
    }
    mp_poolStuntJumps = nullptr;
}

// 0x49CB10
void CStuntJumpManager::ShutdownForRestart() {
    // Como el original: Clear 0x49C9A0 marca todos los slots libres y pone a cero su contador de referencia
    auto* pool = RawPool();
    for (auto i = 0; i < pool->size; i++) {
        pool->states[i] |= 0x80;
        pool->states[i] &= 0x80;
    }
    mp_Active       = nullptr;
    m_bHitReward    = false;
    m_iTimer        = 0;
    SetJumpState(eJumpState::START_POINT_INTERSECTED);
    m_iNumJumps     = 0;
    m_iNumCompleted = 0;
}

// 0x5D5570
bool CStuntJumpManager::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(&m_iNumJumps, sizeof(int32));
    for (auto i = 0; i < STUNT_JUMP_COUNT; i++) {
        auto* pool = RawPool();
        if ((pool->states[i] & 0x80) == 0) {
            CGenericGameStorage::SaveDataToWorkBuffer(&pool->storage[i], sizeof(CStuntJump));
        }
    }
    return true;
}

// 0x5D5920
bool CStuntJumpManager::Load() {
    int32 numJumps;
    CGenericGameStorage::LoadDataFromWorkBuffer(&numJumps, sizeof(int32));
    for (auto i = 0; i < numJumps; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(StuntJumpPoolNew(RawPool()), sizeof(CStuntJump));
    }
    m_iNumJumps = numJumps;
    return true;
}

// 0x49CB40
int32 CStuntJumpManager::AddOne(const CBoundingBox& start, const CBoundingBox& end, const CVector& camPos, int32 reward) {
    if (!RawPool()) {
        return -1;
    }
    auto* const jump = StuntJumpPoolNew(RawPool());
    if (!jump) {
        return -1;
    }
    std::memcpy(&jump->start, &start, sizeof(CBoundingBox)); // Como el original: rep movs
    std::memcpy(&jump->end, &end, sizeof(CBoundingBox));
    jump->camera = camPos;
    jump->reward = reward;
    jump->done   = false;
    jump->found  = false;
    ++m_iNumJumps;
    return (int32)(jump - RawPool()->storage);
}

// 0x49C490
void CStuntJumpManager::Update() {
    if (!mp_poolStuntJumps || CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    // Como el original: sin comprobar nulos; la info es la del jugador en foco y el vehículo es m_pVehicle
    const auto plyr     = FindPlayerPed(-1);
    auto&      plyrInfo = CWorld::Players[CWorld::PlayerInFocus];
    const auto plyrVeh  = plyr->m_pVehicle;

    switch (m_jumpState) {
    case eJumpState::START_POINT_INTERSECTED: { // 0x49C77A
        if (!m_bActive) {
            return;
        }
        if (plyrInfo.m_nPlayerState != PLAYERSTATE_PLAYING || !plyr->bInVehicle || !plyrVeh) {
            return;
        }
        if (plyrVeh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT || plyrVeh->m_pDriver != plyr) {
            return;
        }
        if (plyrVeh->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE || plyrVeh->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI) {
            return;
        }
        // 0x49C7DB:
        // Game sometimes miss successful jumps: it's somewhat rare but happens quite a lot
        // while speed-running. This check here is suspicious, small bumps may cause fail the jump.
        //
        // lordmau5 added a grace period allowing the vehicle to be airborne if any wheel
        // touched ground on his MTA script to fix.
        if (plyrVeh->m_nNumEntitiesCollided != 0) {
            return;
        }
        if (plyrVeh->m_vecMoveSpeed.Magnitude() * 50.f < 20.f) {
            return;
        }

        // Como el original: recorre los 256 slots sin salir del bucle; si hay varias coincidencias gana la última
        for (auto i = 0; i < STUNT_JUMP_COUNT; i++) {
            if (RawPool()->states[i] & 0x80) {
                continue;
            }
            auto* const j = &RawPool()->storage[i];
            if (!j->start.IsPointWithin(plyrVeh->GetPosition())) {
                continue;
            }

            SetJumpState(eJumpState::IN_FLIGHT);
            mp_Active    = j;
            m_iTimer     = 0;
            m_bHitReward = false;

            if (!j->found) {
                j->found = true;
                CStats::IncrementStat(STAT_UNIQUE_JUMPS_FOUND, 1.0f);
            }

            CTimer::SetTimeScale(0.3f);
            TheCamera.SetCamPositionForFixedMode(mp_Active->camera, CVector{ 0.f, 0.f, 0.f });
            TheCamera.TakeControl(plyrVeh, MODE_FIXED, eSwitchType::JUMPCUT, 1);
        }
        break;
    }
    case eJumpState::IN_FLIGHT: { // 0x49C4E6
        if (!mp_Active) {
            SetJumpState(eJumpState::START_POINT_INTERSECTED);
            return;
        }

        bool ended = false;
        if (plyrVeh->m_nNumEntitiesCollided != 0 && (int32)m_iTimer >= 100) {
            ended = true; // Como el original: salta directamente sin comprobar la caja final
        } else {
            if (plyrInfo.m_nPlayerState != PLAYERSTATE_PLAYING) {
                ended = true;
            }
            if (!plyr->bInVehicle) {
                ended = true;
            }
            if (plyrVeh->GetStatus() == STATUS_WRECKED || plyrVeh->vehicleFlags.bIsDrowning || plyrVeh->physicalFlags.bSubmergedInWater) {
                ended = true;
            }
            if (mp_Active->end.IsPointWithin(plyrVeh->GetPosition())) {
                m_bHitReward = true;
            }
        }

        int32 time;
        if (ended) {
            SetJumpState(eJumpState::END_POINT_INTERSECTED);
            time = 0;
        } else {
            time = (int32)m_iTimer;
        }
        const auto newTime = time + StuntJumpTimeStepInMS();
        m_iTimer = (uint32)newTime;
        // Como el original: solo al cruzar el segundo de vuelo (comparaciones con signo)
        if (newTime > 1'000 && time <= 1'000) {
            if (FindPlayerVehicle(-1, false)) {
                if (const auto psgr = FindPlayerVehicle(-1, false)->PickRandomPassenger()) {
                    psgr->Say(CTX_GLOBAL_CAR_JUMP, 0, 1.0f, 0, 0, 0);
                }
            }
        }
        break;
    }
    case eJumpState::END_POINT_INTERSECTED: { // 0x49C4ED
        m_iTimer += (uint32)StuntJumpTimeStepInMS();
        if ((int32)m_iTimer < 300) {
            return;
        }

        CTimer::SetTimeScale(1.f);
        TheCamera.RestoreWithJumpCut();

        if (m_bHitReward && !mp_Active->done) {
            mp_Active->done = true;

            ++m_iNumCompleted;

            CStats::IncrementStat(STAT_UNIQUE_JUMPS_DONE, 1.f);

            const auto reward = m_iNumCompleted == (uint32)m_iNumJumps
                ? 10'000
                : mp_Active->reward;
            plyrInfo.m_nMoney += reward;
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE, 0.0f, 1.0f);

            // Como el original: USJ, luego USJ_ALL (ayuda) y por último REWARD
            if (const auto txt = TheText.Get("USJ")) { // UNIQUE STUNT BONUS!
                CMessages::AddBigMessageQ(txt, 5'000, STYLE_MIDDLE_SMALLER_HIGHER);
            }
            if (m_iNumCompleted == (uint32)m_iNumJumps) {
                if (const auto txt = TheText.Get("USJ_ALL")) { // ALL UNIQUE STUNTS COMPLETED!
                    CHud::SetHelpMessage(txt, false, false, false);
                }
            }
            if (const auto txt = TheText.Get("REWARD")) { // REWARD
                CMessages::AddBigMessageWithNumber(txt, 6'000, STYLE_WHITE_MIDDLE_SMALLER, reward);
            }
        }

        SetJumpState(eJumpState::START_POINT_INTERSECTED);
        mp_Active = nullptr;

        break;
    }
    }
}

// unused
// 0x49C370
void CStuntJumpManager::SetActive(bool active) {
    m_bActive = active;
}

// 0x0
void CStuntJumpManager::Render() {
    // NOP
}

// NOTSA
void ResetAllJumps() {
    for (auto& j : CStuntJumpManager::mp_poolStuntJumps->GetAllValid()) {
        j.done  = false;
        j.found = false;
    }
}

// NOTSA
void StuntJumpTestCode() {
    CPad* pad = CPad::GetPad(0);
    if (pad->IsStandardKeyJustDown('1')) {
        NOTSA_LOG_DEBUG("ResetAllJumps");
        ResetAllJumps();
    }
    if (pad->IsStandardKeyJustDown('2')) {
        auto player = FindPlayerPed();
        if (player) {
            CVector posn{ -2053.93848f, 236.598221f, 35.5952835f };
            player->SetPosn(posn);
            CCheat::VehicleCheat(MODEL_NRG500);
        }
    }
}
