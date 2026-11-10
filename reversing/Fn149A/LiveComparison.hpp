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

// Banco par a par Fn149A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (destructores con borrado y cuerpos de tareas, constructores, constantes de GetTaskType y de eventos, Securom, TakesPriorityOver, AffectsPed, destructor con borrado de edificio y de canal de audio,
// IsBufferPlaying y SetEventResponseTask; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall / RH_ScopedOverloadedInstall / RH_ScopedGlobalInstall) y V (RH_ScopedVMTInstall).
// El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair):
// cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la funcion de la DLL; sin hook, `U`: funcion propia de la DLL; con hook y tambien funcion probada, `Rt`: se reutiliza su ruta), constructor de
// CTask 0x61A3B0, ~CTask 0x61A3A0, ~CEntity 0x535E90, CPed::IsAlive 0x5E0170, CPedGroupIntelligence::SetTask 0x5F7540, CTask::operator delete 0x61A5B0 (con hook) y operator delete del CRT 0x8214BD (sin hook, la DLL lo llama
// por esa direccion). Los objetos COM del canal de audio y las vtables de TakesPriorityOver son falsos en las arenas (la misma funcion falsa en ambos lados, sin parches). Memoria global del juego que se toca: seis palabras de
// Securom, el contador de canales 0xB5F898 y el puntero del pool de edificios 0xB74498 (se guardan al empezar y se restauran tras cada lado; sus cambios se observan).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (this como etiqueta de arena en destructores con borrado y constructores; EAX completo
// en constantes de 32 bits; solo AL en los booleanos) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote149ABody_CTaskSimpleCower();                       // 0x48E900 (con hook)
uintptr_t Lote149ABody_CTaskComplexEnterAnyCarAsDriver();        // 0x63D100 (con hook)
uintptr_t Lote149ABody_CTaskComplexEnterCar();                   // 0x63DFA0 (con hook)
uintptr_t Lote149ABody_CTaskSimpleWaitForBus();                  // 0x632A90 (con hook)
uintptr_t Lote149ABody_CTaskSimpleWaitForPizza();                // 0x632B70 (sin hook)
uintptr_t Lote149ABuildingEntityDestructor();                    // 0x535E90 llamado desde 0x404180 (con hook)

namespace F149ALive {
using namespace F149AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F149AFix::Arena(WIN);
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
inline DWORD gBenchThread{};

// Cuerpo del destructor o constructor base (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEventGroup::Flush (0x4AB370; thiscall, un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), dword de [ECX] al entrar, argumento]; deja gDtorWrite en [ECX].
inline void __fastcall FkFlush(void* self, void*, u32 arg) {
    Ev& e = PushEv(0x4AB370, 3);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    e.v[2] = arg;
    memcpy(self, &gDtorWrite, 4);
}
// operator delete (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CPed::IsAlive (0x5E0170; thiscall, sin argumentos de pila, `ret`): evento [direccion, ECX (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkIsAlive(void* self, void*) { Ev& e = PushEv(0x5E0170, 1); e.v[0] = Tag(self); return gRet; }
// CPedGroupIntelligence::SetTask (0x5F7540; thiscall, cinco dwords de pila, `ret 14h`): evento [direccion, ECX (etiqueta), ped, tarea, pares (etiqueta), slot, force]. No hace nada mas.
inline void __fastcall FkSetTask(void* self, void*, u32 ped, u32 task, u32 pairs, u32 slot, u32 force) {
    Ev& e = PushEv(0x5F7540, 6);
    e.v[0] = Tag(self); e.v[1] = ped; e.v[2] = task; e.v[3] = Tag(uintptr_t(pairs)); e.v[4] = slot; e.v[5] = force;
}
// GetEventType falso del argumento y del propio objeto de TakesPriorityOver (ranura 1 de las vtables falsas de las arenas): evento [id, ECX (etiqueta)]; devuelven el tipo del caso.
inline u32 gTypeRef{}, gTypeOwn{}, gSelf{};
inline u32 __fastcall FkTypeRef(void* self, void*) { Ev& e = PushEv(kEvTypeRef, 1); e.v[0] = Tag(self); return gTypeRef; }
inline u32 __fastcall FkTypeOwn(void* self, void*) { Ev& e = PushEv(kEvTypeOwn, 1); e.v[0] = Tag(self); return gTypeOwn; }
// IUnknown::Release falso (stdcall, un argumento, `ret 4`; ranura 2 de las vtables falsas de los objetos COM): evento [id, objeto (etiqueta), vtable de la clase (+0 de `this`), +8 (etiqueta), +0xC (etiqueta), contador 0xB5F898];
// lee `this` (gSelf) en el momento de la llamada para observar el orden de las escrituras del destructor.
inline u32 __stdcall FkRelease(void* p) {
    Ev& e = PushEv(kEvRelease, 5);
    const u8* t = reinterpret_cast<const u8*>(uintptr_t(gSelf));
    e.v[0] = Tag(p); e.v[1] = BitsOf(t); e.v[2] = Tag(uintptr_t(BitsOf(t + 8))); e.v[3] = Tag(uintptr_t(BitsOf(t + 0xC)));
    e.v[4] = BitsOf(reinterpret_cast<const void*>(uintptr_t(kChannelCounter)));
    return gRet;
}
inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline void PutGlobal16(uintptr_t at, u16 v) { memcpy(reinterpret_cast<void*>(at), &v, 2); }
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
    gRet = 0; gDtorWrite = 0;
    const u32 self = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kBody: case kCtor:   // thiscall sin argumentos de pila: [this] = dword previo; los falsos devuelven gRet y dejan gDtorWrite
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kScalarDeleting: case kScalarDeletingBuilding:   // banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        if (kKind[c.fn] == kScalarDeletingBuilding) {   // pool falso de la arena B: +0 almacenamiento = this - d, +4 estados (estados[indice] cae en la arena C), +8 relleno, +0xC cursor
            const i32 index = i32(a[7]) / 56;
            arenaB.Put32(a[10] + 0x0, self - a[7]);
            arenaB.Put32(a[10] + 0x4, u32(arenaC.Obj(a[9])) - u32(index));
            arenaB.Put32(a[10] + 0x8, a[11]);
            arenaB.Put32(a[10] + 0xC, a[8]);
            PutGlobal32(kBuildingPoolPtr, u32(arenaB.Obj(a[10])));
        }
        break;
    case kCtorNoCall: case kConstant: case kConstantAL:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        break;
    case kStatus:
        arenaA.Put32(a[0] + 0x5C, a[3]);
        call.ecx = self;
        break;
    case kSecurom:   // seis palabras de 16 bits de la memoria global (el resto de los bytes se deja como esta)
        PutGlobal16(0xB733F6, u16(a[3])); PutGlobal16(0xB73186, u16(a[4])); PutGlobal16(0xB733F8, u16(a[5]));
        PutGlobal16(0xB73188, u16(a[6])); PutGlobal16(0xB733E6, u16(a[7])); PutGlobal16(0xB73176, u16(a[8]));
        break;
    case kPriority: { // [this] = vtable falsa propia (ranura 1 = tipo propio); argumento = otro objeto (o el mismo) con su vtable falsa
        const u32 ownVt = u32(arenaC.Obj(a[7])), refVt = u32(arenaC.Obj(a[8]));
        arenaA.Put32(a[0], ownVt);
        arenaC.Put32(a[7] + 4, u32(uintptr_t(&FkTypeOwn)));
        u32 ref = self;
        if (!(a[5] & 1)) {
            ref = u32(arenaB.Obj(a[6]));
            arenaB.Put32(a[6], refVt);
            arenaC.Put32(a[8] + 4, u32(uintptr_t(&FkTypeRef)));
        }
        gTypeRef = a[3]; gTypeOwn = a[4];
        call.ecx = self;
        call.args[0] = ref; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kAffects: {
        arenaA.Put32(a[0] + 0x14, a[4]);
        gRet = a[3];
        call.ecx = self;
        call.args[0] = (a[5] & 1) ? u32(arenaB.Obj(a[6])) : a[7]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kChannel: {
        const u32 obj[3] = {0u, u32(arenaB.Obj(a[8])), u32(arenaB.Obj(a[9]))};   // 0 nulo, 1 objeto COM 1, 2 objeto COM 3
        const u32 p1 = obj[a[4] % 3];
        const u32 p3 = a[5] == 1 ? obj[2] : (a[5] == 2 ? obj[1] : 0u);
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 8, p1); arenaA.Put32(a[0] + 0xC, p3);
        arenaB.Put32(a[8], u32(arenaC.Obj(a[10]))); arenaC.Put32(a[10] + 8, u32(uintptr_t(&FkRelease)));
        arenaB.Put32(a[9], u32(arenaC.Obj(a[11]))); arenaC.Put32(a[11] + 8, u32(uintptr_t(&FkRelease)));
        PutGlobal32(kChannelCounter, a[7]);
        gRet = a[16]; gSelf = self;
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kSetResp:   // seis dwords de pila, `ret 18h`
        call.ecx = self;
        for (unsigned i = 0; i < 6; ++i) call.args[i] = a[3 + i];
        call.argCount = 6; call.popBytes = 0;
        break;
    }
    return call;
}

// Cambios de la memoria global de la funcion respecto a la imagen tras preparar el caso: [direccion, byte nuevo]
inline std::vector<std::vector<uint8>> regionBefore;
inline void RegionsSnap() {
    regionBefore.clear();
    for (const auto i : regionsOf[curFn]) regionBefore.push_back(ReversingProbe::Bytes(regions[i].Ptr(), regions[i].bytes));
}
inline json RegionChanges() {
    json out = json::array();
    size_t k = 0;
    for (const auto i : regionsOf[curFn]) {
        const auto& r = regions[i];
        for (size_t b = 0; b < r.bytes; ++b) if (r.Ptr()[b] != regionBefore[k][b]) out.push_back(json::array({uint64(r.at + b), r.Ptr()[b]}));
        ++k;
    }
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        RegionsSnap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        if (kKind[fn] == kChannel) {   // los dos punteros COM (+8 y +0xC) valen direcciones de las arenas: se observan sus valores finales aparte y no como bytes cambiados
            const size_t at = kGuard + c.a[0] + 8;
            obs["fields"] = json::array({BitsOf(arenaA.m.data() + at), BitsOf(arenaA.m.data() + at + 4)});
            memcpy(arenaA.r.data() + at, arenaA.m.data() + at, 8);
        }
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kScalarDeleting: case kScalarDeletingBuilding: case kCtor: case kCtorNoCall: case kChannel: obs["ret"] = Tag(uintptr_t(eax)); break;   // this como etiqueta de arena
        case kConstant: obs["ret"] = eax; break;                                                                                                       // valor completo de EAX
        case kConstantAL: case kStatus: case kSecurom: case kPriority: case kAffects: obs["ret"] = eax & 0xFFu; break;                                // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                          // void (cuerpos y SetEventResponseTask)
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN149A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN149A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn149a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn149a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn149a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
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
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    const auto& K = kBodyFakes;
    env.stubs[1] = {F(0x61A3A0, K[1])};                                                                       // ~CTask (cuerpo de CTask::Destructor)
    env.stubs[2] = {Rt(1, K[2]), Del()};
    env.stubs[3] = {H(0x48E900, Lote149ABody_CTaskSimpleCower(), K[3]), Del()};
    env.stubs[6] = {F(0x61A3B0, K[6])};                                                                       // constructor de CTaskComplex
    env.stubs[7] = {H(0x63D100, Lote149ABody_CTaskComplexEnterAnyCarAsDriver(), K[7]), Del()};
    env.stubs[8] = {H(0x63DFA0, Lote149ABody_CTaskComplexEnterCar(), K[8]), Del()};
    env.stubs[9] = {H(0x632A90, Lote149ABody_CTaskSimpleWaitForBus(), K[9]), Del()};
    env.stubs[10] = {U(0x632B70, Lote149ABody_CTaskSimpleWaitForPizza(), K[10]), Del()};
    env.stubs[16] = {F(0x5E0170, &FkIsAlive)};
    env.stubs[17] = {H(0x535E90, Lote149ABuildingEntityDestructor(), K[17])};
    env.stubs[18] = {Rt(17, K[18])};
    env.stubs[21] = {E(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[22] = {F(0x5F7540, &FkSetTask)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 4u, 5u, 11u, 12u, 13u, 14u, 15u, 19u, 20u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[15] = "toda la funcion (las dos llamadas virtuales van a los falsos de las vtables de las arenas, iguales en ambos lados)";
    dllReal[21] = "toda la funcion salvo operator delete del CRT (8214BD, sin hook); los dos Release de COM van a los falsos de las vtables de las arenas, iguales en ambos lados";
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
    // memoria global: seis palabras de Securom (una region de 2 bytes cada una), el contador de canales de audio y el puntero del pool de edificios
    for (const uintptr_t w : {0xB733F6u, 0xB73186u, 0xB733F8u, 0xB73188u, 0xB733E6u, 0xB73176u}) regions.emplace_back(w, 2);
    regions.emplace_back(kChannelCounter, 4);       // 6
    regions.emplace_back(kBuildingPoolPtr, 4);      // 7
    for (const unsigned f : {4u, 5u}) regionsOf[f] = {0, 1, 2, 3, 4, 5};
    regionsOf[21] = {6};
    regionsOf[18] = {7};
    Run(env, ready, registered, bodies, collaborators);
}
}
