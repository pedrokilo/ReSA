#include "StdInc.h"
#include "AutoSteal.h"

#include "World.h"
#include "CarCtrl.h"
#include "Population.h"
#include "Streaming.h"
#include "MenuManager.h"
#include "Game.h"
#include "Timer.h"
#include "ColStore.h"
#include "ModelInfo.h"
#include "reversiblehooks/RootHookCategory.h"
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
bool   s_scaffold{};   // 5.o campo de --robar: reactivar hooks de menu/carga (experimento; no resolvio el control)

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
uint32 s_lastSkipLog{}; int s_skips{}; float s_lastGz{}; CVector s_lastAt{};
constexpr int32 MODELS[]{ 496, 436, 401, 410, 517, 405, 549, 507 };

CVehicle* Veh() { return s_vehRef >= 0 ? GetVehiclePool()->GetAtRef(s_vehRef) : nullptr; }
bool BadVec(const CVector& v) { return !std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z); }

// Mantiene el coche quieto (conductor IA sin conducir) hasta que CJ lo robe.
void Freeze(CVehicle* v) {
    v->m_vecMoveSpeed = CVector{};
    v->m_vecTurnSpeed = CVector{};
    v->vehicleFlags.bIsHandbrakeOn = true;
    v->m_autoPilot.SetCarMission(MISSION_NONE);
    v->m_autoPilot.SetTempAction(TEMPACT_WAIT, 30000);
}

struct Rec { uint32 t; int cycle, state, task, simple, inVeh, drvIn; float p[3], v[3], d[3], vs[3]; bool mNaN; };
Rec  s_ring[256]; uint32 s_ringN{}; bool s_dumped{};
int  TaskT(CTask* t) { return t ? (int)t->GetTaskType() : -1; }

void Record(CPlayerPed* pl, uint32 now) {
    Rec r{}; r.t = now; r.cycle = s_cycle + 1; r.state = (int)s_state;
    r.task = TaskT(pl->GetTaskManager().GetActiveTask()); r.simple = TaskT(pl->GetTaskManager().GetSimplestActiveTask());
    r.inVeh = pl->bInVehicle;
    const auto p = pl->GetPosition(); r.p[0] = p.x; r.p[1] = p.y; r.p[2] = p.z;
    // matriz del jugador: cualquier NaN en los 12 elementos
    if (const auto m = pl->m_matrix) { const float* f = (const float*)m; (void)f; r.mNaN = BadVec(m->GetPosition()) || BadVec(m->GetRight()) || BadVec(m->GetForward()) || BadVec(m->GetUp()); }
    if (const auto v = Veh()) {
        const auto q = v->GetPosition(); r.v[0] = q.x; r.v[1] = q.y; r.v[2] = q.z;
        r.vs[0] = v->m_vecMoveSpeed.x; r.vs[1] = v->m_vecMoveSpeed.y; r.vs[2] = v->m_vecMoveSpeed.z;
        r.drvIn = v->m_pDriver ? (v->m_pDriver == pl ? 2 : 1) : 0;
        if (const auto d = v->m_pDriver) { const auto dp = d->GetPosition(); r.d[0] = dp.x; r.d[1] = dp.y; r.d[2] = dp.z; }
    }
    s_ring[s_ringN++ % std::size(s_ring)] = r;
}

void DumpRing(const char* motivo) {
    if (s_dumped) return;
    s_dumped = true;
    NOTSA_LOG_WARN("[ROBAR] VOLCADO tras {}: ultimos fotogramas (t_ms ciclo estado tarea simple enVeh conductor(0 no,1 otro,2 CJ) pos_CJ pos_coche pos_cond vel_coche matrizNaN)", motivo);
    const uint32 n = std::min<uint32>(s_ringN, 90);
    for (uint32 i = s_ringN - n; i < s_ringN; ++i) {
        const auto& r = s_ring[i % std::size(s_ring)];
        NOTSA_LOG_WARN("[ROBAR]  {} c{} e{} T{} S{} v{} d{} CJ({:.2f},{:.2f},{:.2f}) car({:.2f},{:.2f},{:.2f}) drv({:.2f},{:.2f},{:.2f}) vel({:.3f},{:.3f},{:.3f}) mNaN{}",
            r.t, r.cycle, r.state, r.task, r.simple, r.inVeh, r.drvIn, r.p[0], r.p[1], r.p[2], r.v[0], r.v[1], r.v[2], r.d[0], r.d[1], r.d[2], r.vs[0], r.vs[1], r.vs[2], (int)r.mNaN);
    }
}

void Observe(CPlayerPed* pl) {
    const auto p = pl->GetPosition();
    if (BadVec(p)) {
        if (!s_cycleNaN) { NOTSA_LOG_WARN("[ROBAR] NaN en la posicion del jugador (ciclo {})", s_cycle + 1); DumpRing("NaN del jugador"); }
        s_cycleNaN = true;
    } else {
        s_minZ = std::min(s_minZ, p.z);
        if (p.z < -50.f) s_cycleFall = true;
    }
    if (const auto v = Veh(); v && !s_cycleFall) {
        const auto d = v->m_pDriver;
        const bool dFall = d && !BadVec(d->GetPosition()) && d->GetPosition().z < -50.f;
        const bool vFall = !BadVec(v->GetPosition()) && v->GetPosition().z < -50.f;
        const bool pFall = !BadVec(pl->GetPosition()) && pl->GetPosition().z < -50.f;
        if (dFall || vFall || pFall) {
            s_cycleFall = true;
            NOTSA_LOG_WARN("[ROBAR] CAIDA ciclo {}: cae conductor={} coche={} CJ={}; conductor z={:.1f} coche z={:.1f} CJ z={:.1f}; suelo_z de creacion={:.1f}; coche en vehiculo CJ={}",
                s_cycle + 1, (int)dFall, (int)vFall, (int)pFall, d ? d->GetPosition().z : 0.f, v->GetPosition().z, pl->GetPosition().z, s_lastGz, (int)pl->bInVehicle);
            DumpRing("caida a traves del mapa");
        }
    }
    if (const auto v = Veh()) {
        if (BadVec(v->GetPosition())) {
            if (!s_cycleNaN) { NOTSA_LOG_WARN("[ROBAR] NaN en la posicion del vehiculo (ciclo {})", s_cycle + 1); DumpRing("NaN del vehiculo"); }
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
        NOTSA_LOG_INFO("[ROBAR] FIN ciclos={} robados={} sin_robar={} ciclos_con_NaN={} ciclos_con_caida={} fallos_de_creacion={} esperas_por_colision={}",
            s_cycle, s_stolen, s_failed, s_nan, s_falls, s_spawnFail, s_skips);
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
        case 4: s_scaffold = v != 0; break;
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
    Record(pl, now);
    if (s_ring[(s_ringN - 1) % std::size(s_ring)].mNaN && !s_cycleNaN) { s_cycleNaN = true; NOTSA_LOG_WARN("[ROBAR] NaN en la matriz del jugador (ciclo {})", s_cycle + 1); DumpRing("NaN en la matriz del jugador"); }
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
        // Criterio: solo se crea el coche si hay colision cargada bajo el punto y hay suelo cerca de la altura de CJ
        // (evita el artefacto de crear un coche sobre un sitio sin colision, que haria caer al coche y al conductor).
        bool sueloOk = false;
        const float gz = CWorld::FindGroundZFor3DCoord(CVector{ at.x, at.y, pl->GetPosition().z + 1.f }, &sueloOk, nullptr);
        const bool colOk = CColStore::HasCollisionLoaded(at, CGame::currArea);
        if (!colOk || !sueloOk || std::fabs(gz - pl->GetPosition().z) > 3.f) {
            if (now - s_lastSkipLog > 5000) { s_lastSkipLog = now; NOTSA_LOG_INFO("[ROBAR] ciclo {}: sin colision/suelo bajo ({:.1f},{:.1f}) col={} suelo={} gz={:.1f}; se espera", s_cycle + 1, at.x, at.y, (int)colOk, (int)sueloOk, gz); }
            ++s_skips; s_nextStart = now + 1000; break;
        }
        s_lastGz = gz; s_lastAt = at;
        const auto v = CCarCtrl::CreateCarForScript(model, at, false);
        if (!v) { ++s_spawnFail; s_nextStart = now + 3000; break; }
        s_vehRef = GetVehiclePool()->GetRef(v);
        const auto d = CPopulation::AddPedInCar(v, true, -1, 0, false, false);
        s_drvRef = d ? GetPedPool()->GetRef(d) : -1;
        v->vehicleFlags.bIsLocked = false;
        v->m_nDoorLock = CARLOCK_UNLOCKED;
        Freeze(v);
        {   // CJ junto a la puerta del conductor (izquierda), a ~1.8 m
            const auto right = v->GetRight();
            const auto door = v->GetPosition() - CVector{ right.x, right.y, 0.f } * 1.8f;
            pl->Teleport(CVector{ door.x, door.y, pl->GetPosition().z }, false);
            pl->m_fCurrentRotation = pl->m_fAimingRotation = std::atan2(right.x, -right.y); // mirando hacia el coche
        }
        s_stateT0 = now;
        s_state = State::Steal;
        {
            const auto mi = CModelInfo::GetModelInfo(model);
            NOTSA_LOG_INFO("[ROBAR] ciclo {}: modelo coche cargado={} colmodel_coche={} modelo_conductor={} cargado={}", s_cycle + 1,
                (int)CStreaming::IsModelLoaded(model), (mi && mi->GetColModel()) ? 1 : 0, d ? (int)d->m_nModelIndex : -1, d ? (int)CStreaming::IsModelLoaded(d->m_nModelIndex) : -1);
        }
        NOTSA_LOG_INFO("[ROBAR] ciclo {}: coche modelo {} creado con conductor={} en ({:.1f},{:.1f},{:.1f}) suelo_z={:.1f} CJ=({:.1f},{:.1f},{:.1f}) coche_z={:.1f}", s_cycle + 1, model, d ? "si" : "no", s_lastAt.x, s_lastAt.y, s_lastAt.z, s_lastGz, pl->GetPosition().x, pl->GetPosition().y, pl->GetPosition().z, v->GetPosition().z);
        break;
    }
    case State::Steal: {
        const auto v = Veh();
        if (!v) { ++s_spawnFail; EndCycle(pl, false, now); break; }
        NOTSA_LOG_INFO("[ROBAR] ciclo {}: se asigna CTaskComplexEnterCarAsDriver a CJ (t={})", s_cycle + 1, now);
        pl->GetTaskManager().SetTask(new CTaskComplexEnterCarAsDriver{ v }, TASK_PRIMARY_PRIMARY);
        s_stateT0 = now;
        s_state = State::Enter;
        break;
    }
    case State::Enter: {
        const auto v = Veh();
        if (!v) { EndCycle(pl, false, now); break; }
        if (pl->bInVehicle && pl->m_pVehicle == v) { NOTSA_LOG_INFO("[ROBAR] ciclo {}: CJ ya conduce (t={})", s_cycle + 1, now); s_stateT0 = now; s_state = State::Drive; break; }
        if (!pl->bInVehicle) Freeze(v);   // el coche no se va mientras CJ llega y saca al conductor
        if (now - s_stateT0 > 20000) {
            const auto t = pl->GetTaskManager().GetActiveTask();
            const auto simple = pl->GetTaskManager().GetSimplestActiveTask();
            NOTSA_LOG_INFO("[ROBAR] ciclo {}: sin completar el robo en 20 s: tarea activa={} simple={} bInVehicle={} dist={:.1f} conductor_dentro={}",
                s_cycle + 1, t ? (int)t->GetTaskType() : -1, simple ? (int)simple->GetTaskType() : -1,
                (int)pl->bInVehicle, (v->GetPosition() - pl->GetPosition()).Magnitude(), v->m_pDriver ? 1 : 0);
            EndCycle(pl, false, now);
        }
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

// El bucle original del EXE se PAUSA cuando la ventana no esta en primer plano (ForegroundApp = 0 en 0x8D621C): con --unhook-all
// el juego se quedaba cargando sin avanzar mientras el usuario usaba otra ventana. Con --robar se mantiene el dword a 1 (lo que
// escribiria WM_ACTIVATEAPP) para que la prueba automatica no dependa de a que ventana mira el usuario. Misma conducta en A y en B.
DWORD WINAPI KeepForegroundThread(LPVOID) {
    for (;;) {
        *(volatile uint32*)0x8D621C = 1;
        Sleep(100);
    }
}

void InstallIdleDetourIfNeeded() {
    if (!s_enabled || s_detoured) return;
    // Con --unhook-all (o apagado el grupo) InitUserDirectories (0x744FB0) vuelve a ser el del EXE y devuelve la carpeta de
    // partidas por defecto del juego en vez de Misc.SaveDirectoryPath del INI de pruebas: FastLoader no encuentra la ranura y el
    // juego se queda en el menu (el control nunca empezaba). Con --robar se deja ese unico hook ENCENDIDO en A y en B (solo
    // resuelve la ruta de las partidas; no interviene en entidades, coches ni fisica).
    // Tambien se dejan encendidas las clases del menu y de la carga de partidas (andamio de la prueba, no sospechosas del fallo de
    // los coches): con el menu original el FastLoader no llegaba a iniciar la partida en el control (se quedaba en FRONTEND_IDLE).
    if (s_scaffold) ReversibleHooks::GetRootCategory().ForEachCategory([](auto& cat) {
        const auto& n = cat.Name();
        const bool scaffold = n == "CMenuManager" || n == "CGenericGameStorage" || n == "C_PcSave" || n == "CFileMgr" || n == "CLoadingScreen" || n == "Game";
        size_t k = 0;
        for (auto& item : cat.Items()) {
            if ((scaffold || item->Name() == "InitUserDirectories") && item->Name() != "Idle" && !item->Hooked() && item->State(true)) ++k;
        }
        if (k) NOTSA_LOG_INFO("[ROBAR] andamio de carga: {} hooks reactivados en '{}'", k, n);
    });
    static bool s_threadStarted = false;
    if (!s_threadStarted) {
        s_threadStarted = true;
        if (const auto h = CreateThread(nullptr, 0, KeepForegroundThread, nullptr, 0, nullptr)) CloseHandle(h);
        NOTSA_LOG_INFO("[ROBAR] hilo que mantiene ForegroundApp=1 (el juego no se pausa en segundo plano)");
    }
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
