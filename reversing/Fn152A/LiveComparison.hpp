#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn152A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 24 funciones del EXE (tUserList::AddPedType; GetTaskType y cuerpo del destructor de CTaskSimpleInAir; TestModelIndices; CPad::Is{LeftShoulder1,RightShoulder1,DPadUp,Triangle,Circle}Pressed; GetEventType,
// GetLifeTime, GetEventPriority y TakesPriorityOver de CEventPotentialWalkIntoObject; operator= y constructor de copia de CEventKnockOffBike; destructores con borrado y cuerpos de CTaskSimpleCarDrive,
// CBmx, CTaskComplexLeaveCarAndDie y CTaskComplexLeaveCarAndWander; StartRollingDoor; CBmx::BlowUpCar; IsExtraSunnyWeather; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S
// (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los
// colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor y ~CTask / ~CBike / ~CTaskComplexLeaveCar (con hook propio: `F` si la DLL llama al
// cuerpo del hook, `H` si llama a una funcion propia distinta), CTask::operator delete 0x61A5B0 (con hook), CVehicle::operator delete 0x6E2D90 (sin hook, funcion propia de la DLL: `U`),
// CAnimBlendAssociation::SetDeleteCallback 0x4CEBC0 y SetFinishCallback 0x4CEBE0, CEntity::CleanUpOldReference 0x571A00, CEventKnockOffBike::From 0x4AFDD0, CVehicleAnimGroup::GetGroup 0x6E3B00,
// CAnimManager::AddAnimation 0x4D3AA0 y CAEVehicleAudioEntity::Terminate 0x4FB8C0. Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas), los eventos de los
// falsos (con sus argumentos), el valor devuelto (this o etiqueta de arena; EAX completo en constantes y CPad; solo AL en TakesPriorityOver e IsExtraSunnyWeather) y el estado x87 (siempre vacio).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote152ACallback_CarDriveRollDoor();                 // 0x63C840 (con hook): callback que StartRollingDoor entrega a SetFinishCallback; el banco lo anota como 0x63C840
uintptr_t Lote152ABody_CBmxBaseDestructor();                   // 0x6B57A0 (con hook): llamada no virtual a ~CBike desde el cuerpo del destructor de CBmx
uintptr_t Lote152ABody_LeaveCarBaseDestructor();               // 0x63B970 (con hook): llamada no virtual a ~CTaskComplexLeaveCar desde el cuerpo del destructor de CTaskComplexLeaveCarAndDie

namespace F152ALive {
using namespace F152AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F152AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline u32 gGroup{}, gAnim{};        // valores que devuelven los falsos de GetGroup y AddAnimation
inline DWORD gBenchThread{};
inline u32 gRollCallbackDll{};       // direccion del callback de la DLL que StartRollingDoor entrega (se anota como 0x63C840)

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CAnimBlendAssociation::SetDeleteCallback (0x4CEBC0; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (etiqueta), callback, dato]. No escribe nada.
inline void __fastcall FkSetDelete(void* self, void*, u32 callback, u32 data) {
    Ev& e = PushEv(0x4CEBC0, 3);
    e.v[0] = Tag(self);
    e.v[1] = callback;
    e.v[2] = data;
}
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta), dword del argumento]. No escribe nada.
inline void __fastcall FkCleanUp(void* self, void*, void* ref) {
    Ev& e = PushEv(0x571A00, 3);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
    e.v[2] = Tag(ref) != u32(uintptr_t(ref)) ? BitsOf(ref) : 0xBADBAD01u;
}
// CEventKnockOffBike::From (0x4AFDD0; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX, argumento (etiqueta), dword de [ECX], dword de [ECX+4], byte de [ECX+8]]; deja gDtorWrite en [ECX+4]
// y un byte de gRet en [ECX+8]; devuelve gRet (el EXE no lo usa: devuelve this).
inline u32 __fastcall FkFrom(void* self, void*, void* other) {
    Ev& e = PushEv(0x4AFDD0, 5);
    e.v[0] = Tag(self);
    e.v[1] = Tag(other);
    e.v[2] = BitsOf(self);
    e.v[3] = BitsOf(static_cast<u8*>(self) + 4);
    e.v[4] = *(static_cast<u8*>(self) + 8);
    memcpy(static_cast<u8*>(self) + 4, &gDtorWrite, 4);
    *(static_cast<u8*>(self) + 8) = u8(gRet >> 8);
    return gRet;
}
// CVehicleAnimGroup::GetGroup (0x6E3B00; thiscall, un dword de pila, `ret 4`): evento [direccion, ECX (crudo), animacion]; devuelve gGroup.
inline u32 __fastcall FkGetGroup(void* self, void*, u32 animId) {
    Ev& e = PushEv(0x6E3B00, 2);
    e.v[0] = u32(uintptr_t(self));
    e.v[1] = animId;
    return gGroup;
}
// CAnimManager::AddAnimation (0x4D3AA0; cdecl, tres dwords): evento [direccion, clump, grupo, animacion]; devuelve gAnim.
inline u32 __cdecl FkAddAnim(u32 clump, u32 group, u32 animId) {
    Ev& e = PushEv(0x4D3AA0, 3);
    e.v[0] = clump;
    e.v[1] = group;
    e.v[2] = animId;
    return gAnim;
}
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos dwords de pila, `ret 8`) de StartRollingDoor: evento [direccion, ECX (crudo), callback (el de la DLL se anota como 0x63C840),
// dato (etiqueta), dword de [dato+0Ch] (la tarea ya tiene la animacion)]. No escribe nada.
inline void __fastcall FkSetFinishRoll(void* self, void*, u32 callback, void* data) {
    Ev& e = PushEv(0x4CEBE0, 4);
    e.v[0] = u32(uintptr_t(self));
    e.v[1] = (gRollCallbackDll && callback == gRollCallbackDll) ? 0x63C840u : callback;
    e.v[2] = Tag(data);
    e.v[3] = Tag(data) != u32(uintptr_t(data)) ? BitsOf(static_cast<u8*>(data) + 0xC) : 0xBADBAD02u;
}
// CAEVehicleAudioEntity::Terminate (0x4FB8C0; thiscall, sin pila): evento [direccion, ECX (etiqueta), dword de [ECX-0x138] (el [this] del vehiculo en ese momento)]. No escribe nada.
inline void __fastcall FkTerminate(void* self, void*) {
    Ev& e = PushEv(0x4FB8C0, 2);
    e.v[0] = Tag(self);
    const void* owner = static_cast<u8*>(self) - 0x138;
    e.v[1] = Tag(owner) != u32(uintptr_t(owner)) ? BitsOf(owner) : 0xBADBAD03u;
}
template<size_t... I> inline std::array<void*, kFunctions> MakeBodyFakes(std::index_sequence<I...>) { return {reinterpret_cast<void*>(&FkBody<kBodyExe[I]>)...}; }
inline const std::array<void*, kFunctions> kBodyFakes = MakeBodyFakes(std::make_index_sequence<kFunctions>{});

// ================================================================================================ cierre de un lado
inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gGroup = 0; gAnim = 0;
    const size_t o = a[0];
    switch (kKind[c.fn]) {
    case kSdd:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kConst: case kNopCdecl:
        arenaA.Put32(o, a[3]);
        call.ecx = u32(arenaA.Obj(o));
        break;
    case kBoolArg:   // thiscall con un puntero de pila (`ret 4`)
        arenaA.Put32(o, a[3]);
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kNopThis:   // thiscall con dos dwords de pila (`ret 8`)
        arenaA.Put32(o, a[3]);
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2; call.popBytes = 0;
        break;
    case kPad: {
        const size_t off = kPadOff[c.fn], al = off & ~size_t(3);
        const u32 jn = a[5] & 0xFFFFu, jo = a[6] & 0xFFFFu, nw = a[3] & 0xFFFFu, ow = a[4] & 0xFFFFu;
        arenaA.Put32(o + al, (off & 2) ? ((nw << 16) | jn) : ((jn << 16) | nw));
        arenaA.Put32(o + 0x30 + al, (off & 2) ? ((ow << 16) | jo) : ((jo << 16) | ow));
        call.ecx = u32(arenaA.Obj(o));
        break;
    }
    case kUserList:
        for (unsigned i = 0; i < 4; ++i) { arenaA.Put32(o + 4 * i, a[3 + i]); arenaA.Put32(o + 0x10 + 4 * i, a[7 + i]); }
        arenaA.Put32(o + 0x20, a[11]);
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = a[16]; call.argCount = 1; call.popBytes = 0;
        break;
    case kInAirBody:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 0x1C, a[6]); arenaA.Put32(o + 0x38, a[7]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(o));
        break;
    case kAssign: case kCopyCtor:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 4, a[7]); arenaA.Put32(o + 8, a[8]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = u32(arenaB.Obj(a[6])); call.argCount = 1; call.popBytes = 0;
        break;
    case kBmxBody: case kLeaveBody:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(o));
        break;
    case kRollDoor:
        arenaA.Put32(o + 0xC, a[3]);
        arenaB.Put32(a[6] + 0x18, a[7]);
        arenaB.Put32(a[6] + 0x58C, u32(arenaC.Obj(a[8])));
        arenaC.Put32(a[8] + 0x384, u32(arenaB.Obj(a[9])));
        arenaB.Put32(a[9] + 0xDC, (a[11] & 0xFF00FFFFu) | ((a[10] & 0xFFu) << 16));
        gGroup = a[16]; gAnim = a[17];
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = u32(arenaB.Obj(a[6])); call.argCount = 1; call.popBytes = 0;
        break;
    case kWeather:   // cdecl con un dword de pila: lo limpia el llamador
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSdd: case kAssign: case kCopyCtor: obs["ret"] = Tag(uintptr_t(eax)); break;   // this como etiqueta de arena
        case kConst: case kPad: obs["ret"] = eax; break;                                    // valor completo de EAX
        case kBoolArg: case kWeather: obs["ret"] = eax & 0xFFu; break;                      // bool: solo AL
        default: obs["ret"] = nullptr; break;                                               // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN152A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN152A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreAllRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE) y Virtual (direccion de la funcion del EXE que ocupa la ranura): de un Virtual devuelve ademas
// el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato. Un Virtual lleva dentro un Simple sobre la misma direccion (Virtual::Switch).
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name, uintptr_t* virtualBody = nullptr) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            } else if (virtualBody && h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
                *virtualBody = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
        }
    });
    return found;
}
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn152a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn152a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn152a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador CON hook RH cuya llamada, en la funcion probada, va a una funcion propia de la DLL distinta de la envoltura del hook (direccion dada): el hook se apaga (ruta) y el falso va en la
    // direccion del EXE y en esa funcion.
    auto H = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        Pair p = it->second;
        p.dll = Body(dllAddress); p.hooked = true;
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete del CRT): un solo parche, en el EXE.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    const auto& K = kBodyFakes;
    using VoidDelete = void (*)(void*);
    const uintptr_t vehicleDelete = reinterpret_cast<uintptr_t>(static_cast<VoidDelete>(&CVehicle::operator delete));
    gRollCallbackDll = u32(Lote152ACallback_CarDriveRollDoor());
    env.stubs[2] = {F(0x4CEBC0, &FkSetDelete), F(0x571A00, &FkCleanUp), F(0x61A3A0, K[2])};
    env.stubs[13] = {F(0x4AFDD0, &FkFrom)};
    env.stubs[14] = {F(0x4AFDD0, &FkFrom)};
    env.stubs[15] = {F(0x63C460, K[15]), Del()};
    env.stubs[16] = {F(0x6E3B00, &FkGetGroup), F(0x4D3AA0, &FkAddAnim), F(0x4CEBE0, &FkSetFinishRoll)};
    env.stubs[18] = {Rt(19, K[18]), U(0x6E2D90, vehicleDelete, &FkDelete<0x6E2D90>)};
    env.stubs[19] = {F(0x4FB8C0, &FkTerminate), H(0x6B57A0, Lote152ABody_CBmxBaseDestructor(), K[19])};
    env.stubs[20] = {H(0x63B970, Lote152ABody_LeaveCarBaseDestructor(), K[20])};
    env.stubs[21] = {Rt(20, K[21]), Del()};
    env.stubs[22] = {F(0x63BE50, K[22]), Del()};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 1u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 17u, 23u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Sin memoria global del juego: todo ocurre en las tres arenas propias
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
