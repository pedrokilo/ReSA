#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Bench.hpp"

// Banco par a par (dentro del juego) de 20 funciones de biblioteca de RenderWare: EXE con el hook RH apagado frente a la reconstruccion C++ con el hook encendido.
// Veinte funciones x 1024 casos = 20480 pares; ver README.md y Bench.hpp (fixtures, colaboradores falsos, gates).
namespace Dependencies25BLive {
using namespace Dependencies25BFix;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline void Write(const json& row) { RestoreFloatingState(); ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); RestoreFloatingState(); }
inline bool JumpsToDllTrampoline(std::uintptr_t a) { return ReversingProbe::JumpsToDll(a); }

// Si otro grupo reconstruye un colaborador (p. ej. _rwPluginRegistryGetPluginOffset, 0x808470) su hook RH deja en la direccion del EXE un JMP a la DLL: durante la corrida se apaga
// (la direccion del EXE vuelve a ser codigo original, que el falso sustituye igual que a los demas) y se restaura al final con su estado Hooked/Locked inicial.
struct CollabHooks {
    struct Item { ReversibleHooks::ReversibleHook::Base* hook; bool enabled, locked; std::uintptr_t address; std::string name; };
    std::vector<Item> items;
    explicit CollabHooks(const std::array<std::uintptr_t, kCollabKinds>& exe) {
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
            for (auto& hook : category.Items()) {
                if (hook->Symbol()[0] != 'S') continue;
                const auto at = std::uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(hook.get())->GetHookGTAAddress());
                if (std::find(exe.begin() + 1, exe.end(), at) != exe.end()) items.push_back({hook.get(), hook->Hooked(), hook->Locked(), at, std::string(category.Name()) + "/" + hook->Name()});
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

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependencies25b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "dependencies25b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("dependencies25b"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;

    Env env{};
    env.asi = ModuleOf(std::uintptr_t(&ModuleMarker)); env.exe = std::uintptr_t(GetModuleHandleA(nullptr)); env.jumpsToDll = &JumpsToDllTrampoline;

    // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado. Orden = enum Fid.
    using Route = OriginalDependencies499::HookRoute;
std::array<std::unique_ptr<Route>, kFunctions> routes{};
    routes[IndexBuf]       = std::make_unique<Route>("RwCore", "RwD3D9IndexBufferCreate", 0x4C9970, &RwD3D9IndexBufferCreate);
    routes[MaxSupported]   = std::make_unique<Route>("RpAnisot", "RpAnisotGetMaxSupportedMaxAnisotropy", 0x748F20, &RpAnisotGetMaxSupportedMaxAnisotropy);
    routes[AnisoSet]       = std::make_unique<Route>("RpAnisot", "RpAnisotTextureSetMaxAnisotropy", 0x748F30, &RpAnisotTextureSetMaxAnisotropy);
    routes[AnisoGet]       = std::make_unique<Route>("RpAnisot", "RpAnisotTextureGetMaxAnisotropy", 0x748F50, &RpAnisotTextureGetMaxAnisotropy);
    routes[ClumpAtoms]     = std::make_unique<Route>("RpWorld", "RpClumpGetNumAtomics", 0x7498E0, &RpClumpGetNumAtomics);
    routes[AtomAlways]     = std::make_unique<Route>("RpWorld", "RpAtomicSetStreamAlwaysCallBack", 0x74BE30, &RpAtomicSetStreamAlwaysCallBack);
    routes[AtomRights]     = std::make_unique<Route>("RpWorld", "RpAtomicSetStreamRightsCallBack", 0x74BE50, &RpAtomicSetStreamRightsCallBack);
    routes[AtomPlugOff]    = std::make_unique<Route>("RpWorld", "RpAtomicGetPluginOffset", 0x74BEC0, &RpAtomicGetPluginOffset);
    routes[TriMat]         = std::make_unique<Route>("RpWorld", "RpGeometryTriangleGetMaterial", 0x74C760, &RpGeometryTriangleGetMaterial);
    routes[MatInit]        = std::make_unique<Route>("RpWorld", "_rpMaterialListInitialize", 0x74E1B0, &_rpMaterialListInitialize);
    routes[MatGet]         = std::make_unique<Route>("RpWorld", "_rpMaterialListGetMaterial", 0x74E2B0, &_rpMaterialListGetMaterial);
    routes[MatFind]        = std::make_unique<Route>("RpWorld", "_rpMaterialListFindMaterialIndex", 0x74E420, &_rpMaterialListFindMaterialIndex);
    routes[WorldReg]       = std::make_unique<Route>("RpWorld", "RpWorldRegisterPlugin", 0x74FCD0, &RpWorldRegisterPlugin);
    routes[WorldRegStream] = std::make_unique<Route>("RpWorld", "RpWorldRegisterPluginStream", 0x74FD00, &RpWorldRegisterPluginStream);
    routes[AtomWorld]      = std::make_unique<Route>("RpWorld", "RpAtomicGetWorld", 0x751050, &RpAtomicGetWorld);
    routes[LightReg]       = std::make_unique<Route>("RpWorld", "RpLightRegisterPlugin", 0x751D60, &RpLightRegisterPlugin);
    routes[VdeclSize]      = std::make_unique<Route>("RpWorld", "_rpD3D9VertexDeclarationGetSize", 0x7522E0, &_rpD3D9VertexDeclarationGetSize);
    routes[GeoUsage]       = std::make_unique<Route>("RpWorld", "RpD3D9GeometryGetUsageFlags", 0x7588D0, &RpD3D9GeometryGetUsageFlags);
    routes[SecSet]         = std::make_unique<Route>("RpWorld", "RpD3D9WorldSectorSetUsageFlags", 0x7588E0, &RpD3D9WorldSectorSetUsageFlags);
    routes[SecGet]         = std::make_unique<Route>("RpWorld", "RpD3D9WorldSectorGetUsageFlags", 0x758900, &RpD3D9WorldSectorGetUsageFlags);

    // Colaboradores: envoltorios de la DLL que llaman al EXE (se parchean junto con la direccion del EXE).
    env.collabDll[1] = Resolve(std::bit_cast<std::uintptr_t>(&RwD3D9GetCaps));
    env.collabDll[2] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPlgnStrmlwysCB));
    env.collabDll[3] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPlgnStrmRightsCB));
    env.collabDll[4] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryGetPluginOffset));
    env.collabDll[5] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPlugin));
    env.collabDll[6] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPluginStream));
    env.collabDll[7] = Resolve(std::bit_cast<std::uintptr_t>(&D3DResourceSystem::CreateIndexBuffer));

    // Las entradas de la DLL se resuelven con los hooks tal como estan (arriba): con un hook apagado RH deja en la entrada de la DLL un JMP al EXE y Resolve() daria la direccion del EXE.
    // Solo despues se apaga cualquier hook RH que alguien haya puesto en la direccion del EXE de un colaborador (hoy _rwPluginRegistryGetPluginOffset, 0x808470, de g25e).
    CollabHooks collabHooks(kCollabExe);
    // Hoy deben ser: el codigo del EXE sin hook y los envoltorios de la DLL.
    bool collaboratorsOK = true;
    json collabReasons = json::array();
    for (unsigned k = 1; k < kCollabKinds; ++k) {
        const bool dllInAsi = ModuleOf(env.collabDll[k]) == env.asi, exeFree = !ReversingProbe::JumpsToDll(kCollabExe[k]) && ModuleOf(Resolve(kCollabExe[k])) == env.exe;
        if (!dllInAsi || !exeFree) collabReasons.push_back({{"kind", k}, {"dll_in_asi", dllInAsi}, {"exe_unhooked", exeFree}, {"dll", env.collabDll[k]}, {"exe", kCollabExe[k]}});
        collaboratorsOK = collaboratorsOK && dllInAsi && exeFree;
    }
    bool ready = collaboratorsOK;
    json bodies = json::object(), registered = json::object();
    for (unsigned f = 0; f < kFunctions; ++f) {
        ready = ready && routes[f]->Registered() && ModuleOf(routes[f]->Body()) == env.asi;
        bodies[kFns[f].name] = routes[f]->Body(); registered[kFns[f].name] = routes[f]->Registered();
    }
    std::vector<Watch> watches;
    for (unsigned f = 0; f < kFunctions; ++f) watches.emplace_back(kFns[f].entry, 16);
    for (unsigned k = 1; k < kCollabKinds; ++k) { watches.emplace_back(kCollabExe[k], 16); watches.emplace_back(env.collabDll[k], 16); }
    auto watchesIntact = [&] { return std::ranges::all_of(watches, [](const auto& w) { return w.Intact(); }); };
    // Globales que el banco impone por caso: se guardan al empezar y deben coincidir al final.
    const std::array<std::uintptr_t, 4> globalAddrs{0xC92478, 0xC925A8, 0xC926E8, 0xC926EC};
    std::array<u32, 4> globalsInitial{}; for (unsigned i = 0; i < 4; ++i) globalsInitial[i] = *reinterpret_cast<u32*>(globalAddrs[i]);

    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json watched = json::array(); for (const auto& w : watches) watched.push_back({{"address", w.address}, {"bytes", CodeHex(w.address, 16)}});
    json collabs = json::object(); for (unsigned k = 1; k < kCollabKinds; ++k) collabs[std::to_string(k)] = {{"exe", kCollabExe[k]}, {"dll", env.collabDll[k]}};
    json functions = json::array(); for (unsigned f = 0; f < kFunctions; ++f) functions.push_back({{"name", kFns[f].name}, {"entry", kFns[f].entry}, {"collab", kFns[f].collab}, {"global", kFns[f].glob}});
    constexpr unsigned total = kFunctions * kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"functions", kFunctions}, {"pairs", total}, {"cases", kCases}, {"compare_dead_x87_registers", kCompareDeadX87Registers},
           {"collaborators_stubbed", true}, {"collaborators", collabs}, {"collaborator_hooks_disabled", collabHooks.Names()}, {"not_ready_collaborators", collabReasons}, {"function_table", functions}, {"watched_entries", watched}, {"bodies", bodies}, {"registered", registered},
           {"asi_module", env.asi}, {"original_module", env.exe}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR},
           {"initial_globals", std::vector<u32>(globalsInitial.begin(), globalsInitial.end())}});

    int pairs{}; bool passed = ready;
    for (unsigned f = 0; f < kFunctions && ready; ++f) {
        int failures{};
        for (unsigned c = 0; c < kCases; ++c) {
            const json input = CaseInput(static_cast<Fid>(f), c);
            Side a = RunSide(*routes[f], env, static_cast<Fid>(f), c, false);
            Side b = RunSide(*routes[f], env, static_cast<Fid>(f), c, true);
            // los gates de FNSAVE/MXCSR se comparan arriba (post_floating_state dentro de original/candidate); globals_restored ya viene dentro de cada lado
            bool ok = a.out == b.out;
            for (const char* k : {"fp_ip_route_ok", "stack_restored", "nonvolatile_preserved", "route_ok", "globals_restored"}) ok &= a.out[k] == true && b.out[k] == true;
            a.out["floating_state_restored"] = (RestoreFloatingState(), FloatingStateRestored());
            b.out["floating_state_restored"] = (RestoreFloatingState(), FloatingStateRestored());
            ok &= a.out["floating_state_restored"] == true && b.out["floating_state_restored"] == true;
            passed &= ok; failures += !ok;
            Write({{"event", "comparison"}, {"function", kFns[f].name}, {"case", c}, {"pair", pairs++}, {"input", input}, {"original", a.out}, {"candidate", b.out},
                   {"fp_difference", FpDifference(a.run.fp, b.run.fp)},
                   {"original_residue", {{"eax", a.run.reg.eax}, {"ecx", a.run.reg.ecx}, {"edx", a.run.reg.edx}}}, {"candidate_residue", {{"eax", b.run.reg.eax}, {"ecx", b.run.reg.ecx}, {"edx", b.run.reg.edx}}},
                   {"original_fp_ip", a.run.ip}, {"candidate_fp_ip", b.run.ip}, {"equal", ok}, {"accepted", ok}});
        }
        Write({{"event", "function_summary"}, {"function", kFns[f].name}, {"cases", int(kCases)}, {"failures", failures}, {"state_restored", watchesIntact()}});
    }

    // Las vigilancias se tomaron con los hooks apagados (el constructor de HookRoute los apaga): se comprueban ANTES de Restore(), que vuelve a encender los hooks.
    const bool entriesIntactBefore = watchesIntact();
    bool stateRestored = entriesIntactBefore;
    for (auto& r : routes) stateRestored &= r->Restore();
    stateRestored &= collabHooks.Restore();
    for (unsigned i = 0; i < 4; ++i) stateRestored &= *reinterpret_cast<u32*>(globalAddrs[i]) == globalsInitial[i];
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntactBefore},
           {"globals", std::vector<u32>{*reinterpret_cast<u32*>(globalAddrs[0]), *reinterpret_cast<u32*>(globalAddrs[1]), *reinterpret_cast<u32*>(globalAddrs[2]), *reinterpret_cast<u32*>(globalAddrs[3])}}});
    Write({{"event", passed && stateRestored ? "complete" : "error"}, {"pairs", pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntactBefore}});
    RestoreFloatingState(); savedFPActive = false;
}
}  // namespace Dependencies25BLive
