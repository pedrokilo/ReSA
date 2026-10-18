#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "World.h"
#include "CutsceneMgr.h"
#include "Entity/Ped/PlayerPed.h"
#include "AEAudioHardware.h"
#include "AEAudioChannel.h"
#include "AEStaticChannel.h"
#include "AEMP3BankLoader.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par de CAEAudioHardware::PlaySound 0x4D86B0 (hook RH apagado = EXE, encendido = DLL), dentro del juego.
//   thiscall, siete palabras de pila, `ret 0x1C`, sin valor de retorno. 16 familias x 128 casos = 2048 pares (Fixtures.hpp).
//   Todo corre sobre un CAEAudioHardware FALSO (arena propio de 0x24000 bytes relleno al azar por caso) y 65 canales falsos con tabla de metodos virtuales falsa: nunca se toca AEAudioHardware,
//   DirectSound, EAX, los cargadores ni los hilos de audio reales, y no se abre ni se reproduce ningun sonido.
// Colaboradores sustituidos en AMBOS lados por el mismo falso trazado (EntryPatch en la direccion del EXE Y en el cuerpo de la DLL, resuelto con Resolve() ANTES de crear HookRoute / CollabHooks /
// parches): CAEMP3BankLoader::GetSoundBuffer 0x4E0280 y GetLoopOffset 0x4E0380, CAEStaticChannel::SetAudioBuffer 0x4F0C40, CAEAudioChannel::SetNotInRoom 0x4D7B50; GetLength y Play se llaman por
// la vtable del canal falso (mismo objeto para los dos lados). Sus hooks RH (otros bancos) se apagan durante la corrida y se restauran (CollabHooks).
// NO se sustituyen: floor del CRT (0x8219F0) ni _ftol2 (0x821B40): sus efectos se comparan por el estado x87 completo y la posicion que llega a Play.
namespace Dependencies26ALive {
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t a) { MEMORY_BASIC_INFORMATION i{}; return VirtualQuery(reinterpret_cast<void*>(a), &i, sizeof(i)) == sizeof(i) ? uintptr_t(i.AllocationBase) : 0; }
inline uintptr_t Resolve(uintptr_t a) { for (unsigned i = 0; i < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++i) a += 5 + *reinterpret_cast<int32*>(a + 1); return a; }
// Cuerpo de la DLL de una funcion: sigue los JMP (thunk de enlace incremental) mientras el destino sea de la DLL y se detiene ante el JMP al EXE que RH deja en la entrada cuando el hook esta
// APAGADO (run_case arranca con --unhook=Audio,...: todos los hooks de Audio/Hardware y Audio/Loaders empiezan apagados). Resolve() seguiria ese JMP hasta el EXE. Como AudioHw54.
inline uintptr_t Body(uintptr_t p) {
    for (int k = 0; k < 4 && *reinterpret_cast<const uint8*>(p) == 0xE9; ++k) { const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1); if (t >= 0x400000 && t < 0x1000000) break; p = t; }
    return p;
}
inline void Write(const json& row) { RestoreFloatingState(); ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); RestoreFloatingState(); }

static_assert(offsetof(CAEAudioHardware, m_pMP3BankLoader) == kBankLoaderAt);
static_assert(offsetof(CAEAudioHardware, m_n3dEffectsQueryResult) == kN3dAt);
static_assert(offsetof(CAEAudioHardware, m_aChannels) == kChannelTableAt);
static_assert(offsetof(CAEAudioHardware, m_anNumChannelsInSlot) == kNumChannelsInSlotAt);
static_assert(offsetof(CAEAudioHardware, m_awChannelFlags) == kFlagsAt);
static_assert(offsetof(CAEAudioChannel, m_nFlags) == kChannelFlagsAt);

struct Watch { uintptr_t address; std::vector<uint8> saved; Watch(uintptr_t a, size_t n) : address(a), saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a), n)) {} bool Intact() const { return saved == ReversingProbe::Bytes(reinterpret_cast<void*>(address), saved.size()); } };
// Los parches se deshacen en orden INVERSO
struct PatchSet { std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; void Add(uintptr_t at, void* f) { v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at, f)); } ~PatchSet() { while (!v.empty()) v.pop_back(); } };

// Colaboradores: direccion del EXE, cuerpo de la DLL (ya resuelto) y falso
struct Col { const char* name; uintptr_t exe, dll; void* fake; };
inline constexpr std::array<uintptr_t, 4> kCollabExe{0x4E0280, 0x4E0380, 0x4F0C40, 0x4D7B50};

// Si otro banco reconstruye un colaborador su hook RH deja en la direccion del EXE un JMP a la DLL: durante la corrida se apaga (la direccion del EXE vuelve a ser codigo original, que el falso
// sustituye igual) y se restaura al final con su estado Hooked/Locked inicial.
struct CollabHooks {
    struct Item { ReversibleHooks::ReversibleHook::Base* hook; bool enabled, locked; uintptr_t address; std::string name; };
    std::vector<Item> items;
    explicit CollabHooks(const std::array<uintptr_t, 4>& exe) {
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
            for (auto& hook : category.Items()) {
                if (hook->Symbol()[0] != 'S') continue;
                const auto at = uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(hook.get())->GetHookGTAAddress());
                if (std::find(exe.begin(), exe.end(), at) != exe.end()) items.push_back({hook.get(), hook->Hooked(), hook->Locked(), at, std::string(category.Name()) + "/" + hook->Name()});
            }
        });
        for (auto& i : items) { i.hook->LockState(false); i.hook->State(false); }
    }
    json Names() const { json out = json::array(); for (const auto& i : items) out.push_back({{"address", i.address}, {"hook", i.name}}); return out; }
    bool Restore() {
        bool ok = true;
        for (auto& i : items) { i.hook->State(i.enabled); i.hook->LockState(i.locked); ok &= i.hook->Hooked() == i.enabled && i.hook->Locked() == i.locked; }
        items.clear();
        return ok;
    }
    ~CollabHooks() { Restore(); }
};

struct Run { Gate gate{}; bool routeOK{}; };
template<class F> Run Execute(OriginalDependencies499::HookRoute& route, bool candidate, uintptr_t entry, uintptr_t asi, F gate) {
    Run run{};
    route.Invoke(candidate, [&] {
        run.routeOK = candidate ? ReversingProbe::JumpsToDll(entry) && ModuleOf(Resolve(entry)) == asi : !ReversingProbe::JumpsToDll(entry);
        run.gate = gate(); return 0;
    });
    return run;
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependencies26a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "dependencies26a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("dependencies26a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    gMainThread = GetCurrentThreadId(); InitFakes();
    const uintptr_t asi = ModuleOf(uintptr_t(&ModuleMarker)), gameExe = uintptr_t(GetModuleHandleA(nullptr));
    constexpr uintptr_t kEntry = 0x4D86B0;

    // 1) Cuerpos de la DLL de los colaboradores: ANTES de crear HookRoute / CollabHooks / parches; con Body() (no Resolve()) porque los hooks de Audio arrancan apagados (--unhook).
    const std::array<Col, 4> cols{{
        {"CAEMP3BankLoader::GetSoundBuffer", kCollabExe[0], Body(std::bit_cast<uintptr_t>(&CAEMP3BankLoader::GetSoundBuffer)), reinterpret_cast<void*>(&FGetSoundBuffer)},
        {"CAEMP3BankLoader::GetLoopOffset", kCollabExe[1], Body(std::bit_cast<uintptr_t>(&CAEMP3BankLoader::GetLoopOffset)), reinterpret_cast<void*>(&FGetLoopOffset)},
        {"CAEStaticChannel::SetAudioBuffer", kCollabExe[2], Body(std::bit_cast<uintptr_t>(&CAEStaticChannel::SetAudioBuffer)), reinterpret_cast<void*>(&FSetAudioBuffer)},
        {"CAEAudioChannel::SetNotInRoom", kCollabExe[3], Body(std::bit_cast<uintptr_t>(&CAEAudioChannel::SetNotInRoom)), reinterpret_cast<void*>(&FSetNotInRoom)},
    }};
    // 2) Ruta RH de PlaySound: cuerpo = destino registrado del hook (GetHookOurAddress), como AudioHw54 (el constructor enciende el hook para leerlo y lo deja apagado) y hooks de colaboradores apagados.
    OriginalDependencies499::HookRoute route("CAEAudioHardware", "PlaySound", kEntry, OriginalDependencies499::HookRoute::RegisteredTarget{});
    CollabHooks collabHooks(kCollabExe);
    bool collaboratorsOK = true; json collabReasons = json::array();
    for (const auto& c : cols) {
        const bool dllInAsi = ModuleOf(c.dll) == asi, exeFree = !ReversingProbe::JumpsToDll(c.exe) && ModuleOf(Resolve(c.exe)) == gameExe;
        if (!dllInAsi || !exeFree) collabReasons.push_back({{"name", c.name}, {"dll_in_asi", dllInAsi}, {"exe_unhooked", exeFree}, {"dll", c.dll}, {"exe", c.exe}});
        collaboratorsOK = collaboratorsOK && dllInAsi && exeFree;
    }
    const bool ready = route.Registered() && ModuleOf(route.Body()) == asi && collaboratorsOK;
    // El floor del CRT del EXE (0x8219F0) toma la ruta SSE2 si [0xC9C400] != 0 (es la del juego); la ruta x87 (0x828A60) no se ejercita ni se valida.
    const uint32 crtSse2 = *reinterpret_cast<uint32*>(0xC9C400);

    std::vector<Watch> watches;
    watches.emplace_back(kEntry, 16);
    for (const auto& c : cols) { watches.emplace_back(c.exe, 16); if (c.dll != c.exe) watches.emplace_back(c.dll, 16); }
    watches.emplace_back(0x821B40, 16); watches.emplace_back(0x8219F0, 16);
    auto watchesIntact = [&] { return std::ranges::all_of(watches, [](const auto& w) { return w.Intact(); }); };
    auto brokenWatches = [&] { json o = json::array(); for (const auto& w : watches) if (!w.Intact()) o.push_back({{"address", w.address}, {"expected", Hex(w.saved.data(), w.saved.size())}}); return o; };
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json bodies = json::array(); for (const auto& c : cols) bodies.push_back({{"name", c.name}, {"exe", c.exe}, {"dll", c.dll}});
    Write({{"event", "begin"}, {"registered_hook", ready}, {"functions", 1}, {"pairs", kCases}, {"cases", kCases}, {"families", kFamilies}, {"cases_per_family", kCasesPerFamily},
           {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"collaborators_stubbed", true}, {"bodies", {{"CAEAudioHardware::PlaySound", route.Body()}}}, {"collaborator_bodies", bodies},
           {"collaborator_hooks_disabled", collabHooks.Names()}, {"not_ready_collaborators", collabReasons}, {"crt_floor_sse2_flag", crtSse2}, {"asi_module", asi}, {"original_module", gameExe},
           {"initial_fp", Hex(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    int pairs{}; bool passed = ready && crtSse2 != 0;
    const std::string name = "CAEAudioHardware::PlaySound";
    int failures{};
    for (unsigned n = 0; n < kCases && passed; ++n) {
        const Case c = MakeCase(n);
        Run run[2]; json rows[2];
        for (int side = 0; side < 2; ++side) {
            const bool candidate = side == 1;
            RestoreFloatingState(); Apply(c); MakePrime(c.fsw); nev = 0;
            run[side] = Execute(route, candidate, kEntry, asi, [&] {
                PatchSet p; for (const auto& k : cols) { p.Add(k.exe, k.fake); if (k.dll != k.exe) p.Add(k.dll, k.fake); }
                return GateThis7(kEntry, hw.data(), c.args, 0xA5000000u | n, 0x2DE50000u | n);
            });
            RestoreFloatingState();   // el estado x87/SSE del juego, intacto ANTES de escribir (Write bombea mensajes)
            json out = SideJson(c, run[side].gate);
            const uintptr_t where = ModuleOf(FipOf(run[side].gate.img));
            out["fp_ip_route_ok"] = FipOf(run[side].gate.img) == 0 || where == (candidate ? asi : gameExe) || where == asi;
            out["route_ok"] = run[side].routeOK;
            out["globals_restored"] = watchesIntact() && *reinterpret_cast<uint32*>(0xC9C400) == crtSse2; out["floating_state_restored"] = FloatingStateRestored();
            rows[side] = out;
        }
        Fp fa = run[0].gate.img, fb = run[1].gate.img; memset(fa.data() + 12, 0, 4); memset(fb.data() + 12, 0, 4);
        bool ok = rows[0] == rows[1];
        for (const char* k : {"fp_ip_route_ok", "stack_restored", "nonvolatile_preserved", "route_ok", "globals_restored", "floating_state_restored"}) ok &= rows[0][k] == true && rows[1][k] == true;
        const json diff = FpDifference(fa, fb); ok &= diff.empty(); passed &= ok; failures += !ok;
        // EAX/ECX/EDX al volver NO se exigen: el unico llamador (0x4F07F8) los sobrescribe antes de leerlos (ver g26a-audit.txt); se registran.
        Write({{"event", "comparison"}, {"function", name}, {"case", n}, {"pair", pairs++}, {"original", rows[0]}, {"candidate", rows[1]}, {"fp_difference", diff}, {"dead_registers_equal", DeadEqual(fa, fb)},
               {"original_residue", {{"eax", run[0].gate.eax}, {"ecx", run[0].gate.ecx}, {"edx", run[0].gate.edx}}}, {"candidate_residue", {{"eax", run[1].gate.eax}, {"ecx", run[1].gate.ecx}, {"edx", run[1].gate.edx}}},
               {"equal", ok}, {"accepted", ok}});
    }
    Write({{"event", "function_summary"}, {"function", name}, {"cases", int(kCases)}, {"failures", failures}, {"state_restored", watchesIntact()}, {"broken_watches", brokenWatches()}});
    // Las entradas se vigilan ANTES de Restore(); despues se restauran el hook, los hooks de colaboradores y el estado x87
    const bool entriesIntactBefore = watchesIntact(); const json brokenBefore = brokenWatches();
    bool stateRestored = entriesIntactBefore; stateRestored &= route.Restore(); stateRestored &= collabHooks.Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"state_restored", stateRestored}, {"watched_intact", entriesIntactBefore}, {"broken_watches_before_restore", brokenBefore}});
    Write({{"event", passed && stateRestored ? "complete" : "error"}, {"pairs", pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntactBefore}});
    RestoreFloatingState(); savedFPActive = false;
}
}
