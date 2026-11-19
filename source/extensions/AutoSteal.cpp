#include "StdInc.h"
#include "AutoSteal.h"

#include "World.h"
#include "CarCtrl.h"
#include "Population.h"
#include "Streaming.h"
#include "MenuManager.h"
#include "Game.h"
#include "Timer.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleCarSetPedOut.h"
#include <Utility.h>

namespace AutoSteal {
namespace {
bool   s_enabled{};
int    s_cycles{ 20 };
uint32 s_periodMs{ 20000 };
uint32 s_startDelayMs{ 20000 };
uint32 s_driveMs{ 6000 };

enum class State { Wait, Steal, Enter, Drive, Done };
State  s_state{ State::Wait };
int    s_cycle{};            // ciclos terminados
uint32 s_exteriorSince{};    // desde cuándo el jugador está fuera de la casa (0 = no)
uint32 s_nextStart{};
uint32 s_stateT0{};
int    s_vehRef{ -1 }, s_drvRef{ -1 };
int    s_stolen{}, s_failed{}, s_nan{}, s_falls{}, s_spawnFail{};
bool   s_cycleNaN{}, s_cycleFall{};
float  s_minZ{ 1e9f };
constexpr int32 MODELS[]{ 496, 436, 401, 410, 517, 405, 549, 507 };

CVehicle* Veh() { return s_vehRef >= 0 ? GetVehiclePool()->GetAtRef(s_vehRef) : nullptr; }
bool BadVec(const CVector& v) { return !std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z); }

void Observe(CPlayerPed* pl) {
    const auto p = pl->GetPosition();
    if (BadVec(p)) {
        if (!s_cycleNaN) NOTSA_LOG_WARN("[ROBAR] NaN en la posicion del jugador (ciclo {})", s_cycle + 1);
        s_cycleNaN = true;
    } else {
        s_minZ = std::min(s_minZ, p.z);
        if (p.z < -50.f) s_cycleFall = true;
    }
    if (const auto v = Veh()) {
        if (BadVec(v->GetPosition())) {
            if (!s_cycleNaN) NOTSA_LOG_WARN("[ROBAR] NaN en la posicion del vehiculo (ciclo {})", s_cycle + 1);
            s_cycleNaN = true;
        } else if (v->GetPosition().z < -50.f) {
            s_cycleFall = true;
        }
    }
}

void Cleanup(CPlayerPed* pl) {
    if (const auto v = Veh()) {
        if (pl->bInVehicle && pl->m_pVehicle == v) {
            CTaskSimpleCarSetPedOut{ v, TARGET_DOOR_DRIVER, true, true }.ProcessPed(pl);
        }
        CWorld::RemoveVehicleAndItsOccupants(v);
    }
    s_vehRef = s_drvRef = -1;
}

void EndCycle(CPlayerPed* pl, bool stolen, uint32 now) {
    const auto p = pl->GetPosition();
    if (stolen) ++s_stolen; else ++s_failed;
    if (s_cycleNaN) ++s_nan;
    if (s_cycleFall) ++s_falls;
    ++s_cycle;
    NOTSA_LOG_INFO("[ROBAR] ciclo {}/{}: robado={} nan={} z_bajo={} pos=({:.1f},{:.1f},{:.1f}) zmin={:.1f}",
        s_cycle, s_cycles, stolen ? "si" : "no", s_cycleNaN ? "SI" : "no", s_cycleFall ? "SI" : "no", p.x, p.y, p.z, s_minZ);
    Cleanup(pl);
    s_cycleNaN = s_cycleFall = false;
    s_nextStart = now + s_periodMs;
    s_state = s_cycle >= s_cycles ? State::Done : State::Wait;
    if (s_state == State::Done) {
        NOTSA_LOG_INFO("[ROBAR] FIN ciclos={} robados={} sin_robar={} ciclos_con_NaN={} ciclos_con_caida={} fallos_de_creacion={}",
            s_cycle, s_stolen, s_failed, s_nan, s_falls, s_spawnFail);
    }
}
} // namespace

void Configure(std::string_view spec) {
    s_enabled = true;
    int i = 0;
    for (auto tok : SplitStringView(spec, ",")) {
        const int v = std::atoi(std::string{ tok }.c_str());
        switch (i++) {
        case 0: if (v > 0) s_cycles = v; break;
        case 1: if (v > 0) s_periodMs = v * 1000u; break;
        case 2: if (v >= 0) s_startDelayMs = v * 1000u; break;
        case 3: if (v >= 0) s_driveMs = v * 1000u; break;
        }
    }
    NOTSA_LOG_INFO("[ROBAR] activado: {} ciclos, cada {} s, espera inicial {} s, conducir {} s", s_cycles, s_periodMs / 1000, s_startDelayMs / 1000, s_driveMs / 1000);
}

bool Enabled() { return s_enabled; }

void OnFrame() {
    if (!s_enabled || s_state == State::Done) return;
    const auto pl = FindPlayerPed();
    const auto now = CTimer::GetTimeInMS();
    if (!pl || FrontEndMenuManager.m_bMenuActive || CGame::currArea != AREA_CODE_NORMAL_WORLD || !pl->IsAlive()) {
        s_exteriorSince = 0;
        return;
    }
    if (!s_exteriorSince) {
        s_exteriorSince = now;
        s_nextStart = now + s_startDelayMs;
        NOTSA_LOG_INFO("[ROBAR] jugador en el exterior; el primer ciclo empieza en {} s", s_startDelayMs / 1000);
    }
    if (s_state != State::Wait) Observe(pl);

    switch (s_state) {
    case State::Wait: {
        if (now < s_nextStart || BadVec(pl->GetPosition())) break;
        const int32 model = MODELS[s_cycle % std::size(MODELS)];
        if (!CStreaming::IsModelLoaded(model)) {
            CStreaming::RequestModel(model, STREAMING_GAME_REQUIRED);
            CStreaming::LoadAllRequestedModels(false);
        }
        if (!CStreaming::IsModelLoaded(model)) { ++s_spawnFail; s_nextStart = now + 3000; break; }
        const auto fwd = pl->GetForward();
        const auto at = pl->GetPosition() + CVector{ fwd.x, fwd.y, 0.f } * 5.f;
        const auto v = CCarCtrl::CreateCarForScript(model, at, false);
        if (!v) { ++s_spawnFail; s_nextStart = now + 3000; break; }
        s_vehRef = GetVehiclePool()->GetRef(v);
        const auto d = CPopulation::AddPedInCar(v, true, -1, 0, false, false);
        s_drvRef = d ? GetPedPool()->GetRef(d) : -1;
        v->vehicleFlags.bIsLocked = false;
        v->m_nDoorLock = CARLOCK_UNLOCKED;
        s_stateT0 = now;
        s_state = State::Steal;
        NOTSA_LOG_INFO("[ROBAR] ciclo {}: coche modelo {} creado con conductor={}", s_cycle + 1, model, d ? "si" : "no");
        break;
    }
    case State::Steal: {
        const auto v = Veh();
        if (!v) { ++s_spawnFail; EndCycle(pl, false, now); break; }
        pl->GetTaskManager().SetTask(new CTaskComplexEnterCarAsDriver{ v }, TASK_PRIMARY_PRIMARY);
        s_stateT0 = now;
        s_state = State::Enter;
        break;
    }
    case State::Enter: {
        const auto v = Veh();
        if (!v) { EndCycle(pl, false, now); break; }
        if (pl->bInVehicle && pl->m_pVehicle == v) { s_stateT0 = now; s_state = State::Drive; break; }
        if (now - s_stateT0 > 30000) EndCycle(pl, false, now);
        break;
    }
    case State::Drive: {
        const auto v = Veh();
        if (!v || !pl->bInVehicle) { EndCycle(pl, true, now); break; }
        // acelerar hacia delante para forzar la carga de mundo, como al conducir de verdad
        const auto fwd = v->GetForward();
        if (now - s_stateT0 < s_driveMs) v->m_vecMoveSpeed = CVector{ fwd.x, fwd.y, 0.f } * 0.4f;
        else EndCycle(pl, true, now);
        break;
    }
    default: break;
    }
}

namespace {
constexpr uintptr_t IDLE_ADDR = 0x53E920;
uint8 s_savedBytes[5]{};
bool  s_detoured{};

void __cdecl IdleDetour(void* param);

void WriteJump(bool on) {
    DWORD old;
    VirtualProtect((void*)IDLE_ADDR, 5, PAGE_EXECUTE_READWRITE, &old);
    if (on) {
        auto* p = (uint8*)IDLE_ADDR;
        p[0] = 0xE9;
        const auto rel = (int32)((uintptr_t)&IdleDetour - (IDLE_ADDR + 5));
        std::memcpy(p + 1, &rel, 4);
    } else {
        std::memcpy((void*)IDLE_ADDR, s_savedBytes, 5);
    }
    VirtualProtect((void*)IDLE_ADDR, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void*)IDLE_ADDR, 5);
}

void __cdecl IdleDetour(void* param) {
    WriteJump(false);
    ((void(__cdecl*)(void*))IDLE_ADDR)(param); // Idle original del EXE
    WriteJump(true);
    OnFrame();
}
} // namespace

void InstallIdleDetourIfNeeded() {
    if (!s_enabled || s_detoured) return;
    if (*(uint8*)IDLE_ADDR == 0xE9) {
        NOTSA_LOG_INFO("[ROBAR] Idle de la DLL activo: OnFrame se llama desde el.");
        return;
    }
    std::memcpy(s_savedBytes, (void*)IDLE_ADDR, 5);
    s_detoured = true;
    WriteJump(true);
    NOTSA_LOG_INFO("[ROBAR] Idle original del EXE: se engancha un desvio propio para llamar a OnFrame.");
}
} // namespace AutoSteal
