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

// Banco par a par Fn203A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C
// y D con guardas de 0x40) y la memoria global que cada funcion usa (pool de COctTree 0xBC12DC.., puntero del pool de CVehicleStructure 0xB4E680, tabla de modelos 0xA9B0B8..). Todas las funciones del lote tienen el hook
// APAGADO durante el banco salvo la del par (la candidata lo enciende). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) cuando la DLL tiene hook; solo en la
// direccion del EXE cuando la DLL los llama por ella (operator delete 0x8214BD, RwTexDictionaryRemoveTexture 0x7F39C0, RwTextureDestroy 0x7F3820, RpAtomicDestroy 0x749DC0, RwFrameDestroy 0x7F05A0).
// Se observan: los bytes cambiados de las cuatro arenas, los cambios de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto y el estado x87 (siempre vacio).

namespace F203ALive {
using namespace F203AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F203AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>* const gArenas[4] = {&arenaA, &arenaB, &arenaC, &arenaD};
// Puntero simbolico (P(arena, desplazamiento)) -> direccion real; otro valor queda igual
inline u32 Resolve32(u32 v) { return IsSym(v) ? u32(gArenas[(v >> 20) & 3]->Obj(v & 0xFFFFF)) : v; }

// Parametros del caso que leen los falsos
inline std::array<u32, 48> gA{};
inline u32 gDestroyCalls{}, gGet2dCalls{}, gXformCalls{}, gHasCalls{};
inline int16* gCountPtr{};
inline uintptr_t gThis{};
inline DWORD gBenchThread{};

// ================================================================================================ falsos de los colaboradores
// operator delete del CRT (0x8214BD; cdecl, un puntero CRUDO): evento [direccion, argumento (etiqueta o crudo)]. No libera nada.
inline void __cdecl FkDelete(void* p) { Ev& e = PushEv(0x8214BD, 1); e.v[0] = Tag(p); }
// Cuerpo / constructor base sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja a[41] en [ECX]; devuelve a[40].
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gA[41], 4);
    return gA[40];
}
// CEntity::RegisterReference (0x571B70) y CleanUpOldReference (0x571A00) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta cruda), referencia (etiqueta)].
template<u32 ADDR> inline u32 __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ref);
    return 0;
}
inline u32 __cdecl FkTexRemove(u32 tex) { Ev& e = PushEv(0x7F39C0, 1); e.v[0] = Tag(uintptr_t(tex)); return tex; }
inline u32 __cdecl FkTexDestroy(u32 tex) { Ev& e = PushEv(0x7F3820, 1); e.v[0] = Tag(uintptr_t(tex)); return 1; }
inline u32 __cdecl FkNodeName(u32 frame) { Ev& e = PushEv(0x72FB30, 1); e.v[0] = Tag(uintptr_t(frame)); return Resolve32(gA[9]); }
// RpAtomicDestroy (0x749DC0): evento [direccion, atomico (etiqueta)]; si a[17] != 0 escribe ese dword en [atomico + 4]; tras la llamada a[18] cambia el contador de extras de la estructura (BYTE +0x30C de `this`) a a[19].
inline u32 __cdecl FkAtomDestroy(u32 atomic) {
    Ev& e = PushEv(0x749DC0, 1);
    e.v[0] = Tag(uintptr_t(atomic));
    ++gDestroyCalls;
    if (gA[17] != 0) memcpy(reinterpret_cast<void*>(uintptr_t(atomic) + 4), &gA[17], 4);
    if (gA[18] != 0 && gA[18] == gDestroyCalls) *reinterpret_cast<u8*>(gThis + 0x30C) = u8(gA[19]);
    return 1;
}
inline u32 __cdecl FkFrameDestroy(u32 frame) { Ev& e = PushEv(0x7F05A0, 1); e.v[0] = Tag(uintptr_t(frame)); return 1; }
// CVehicleStructure::~CVehicleStructure (0x4C7410; thiscall): evento [direccion, ECX (etiqueta cruda)].
inline u32 __fastcall FkVsDtor(void* self, void*) { Ev& e = PushEv(0x4C7410, 1); e.v[0] = Tag(self); return 0; }
// CWorld::FindObjectsInRange (0x564A20; cdecl, 11 argumentos): evento [punto (etiqueta), radio (bits), b2D, cuenta inicial (u16), maximo, edificios, vehiculos, peatones, objetos, ficticios]; escribe la cuenta a[4] (int16)
// y las ocho entidades a[5..12].
inline void __cdecl FkFindObjects(const void* point, u32 radius, u32 b2d, int16* count, u32 maxCount, void** list, u32 b1, u32 b2, u32 b3, u32 b4, u32 b5) {
    Ev& e = PushEv(0x564A20, 10);
    e.v[0] = Tag(point); e.v[1] = radius; e.v[2] = b2d; e.v[3] = u32(uint16(*count)); e.v[4] = maxCount; e.v[5] = b1; e.v[6] = b2; e.v[7] = b3; e.v[8] = b4; e.v[9] = b5;
    *count = int16(gA[4]);
    for (u32 i = 0; i < 8; ++i) list[i] = reinterpret_cast<void*>(uintptr_t(arenaA.Obj(0x100 * gA[5 + i])));
    gCountPtr = count;
}
// CBaseModelInfo::Get2dEffect (0x4C4C70; thiscall, un entero de pila, `ret 4`): evento [direccion, ECX (etiqueta), indice]; devuelve el puntero guardado en [modelo + 0x20 + 4 * indice]. En la llamada a[13] cambia el contador
// de efectos del modelo a a[14]; en la llamada a[15] cambia la cuenta de entidades a a[16].
inline u32 __fastcall FkGet2d(void* self, void*, int index) {
    Ev& e = PushEv(0x4C4C70, 2);
    e.v[0] = Tag(self); e.v[1] = u32(index);
    ++gGet2dCalls;
    u32 effect{};
    memcpy(&effect, reinterpret_cast<uint8*>(self) + 0x20 + 4 * index, 4);
    if (gA[13] != 0 && gA[13] == gGet2dCalls) *(reinterpret_cast<uint8*>(self) + 0xD) = u8(gA[14]);
    if (gA[15] != 0 && gA[15] == gGet2dCalls && gCountPtr) *gCountPtr = int16(gA[16]);
    return effect;
}
// CEntity::TransformFromObjectSpace (0x5334F0; thiscall, devuelve por puntero oculto, dos punteros de pila, `ret 8`): evento [direccion, ECX (etiqueta), salida (etiqueta), posicion del efecto (etiqueta)]; cada eje i sale del
// vector de la entidad (+0x30) si el bit i de a[18] esta puesto, y de la posicion del efecto si no.
inline void* __fastcall FkTransform(void* self, void*, void* out, const void* offset) {
    Ev& e = PushEv(0x5334F0, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(out); e.v[2] = Tag(offset);
    ++gXformCalls;
    for (u32 i = 0; i < 3; ++i) {
        u32 v{};
        memcpy(&v, ((gA[18] >> i) & 1) ? reinterpret_cast<const uint8*>(self) + 0x30 + 4 * i : reinterpret_cast<const uint8*>(offset) + 4 * i, 4);
        memcpy(reinterpret_cast<uint8*>(out) + 4 * i, &v, 4);
    }
    return out;
}
// GetPedAttractorManager (0x5EE190; cdecl, sin argumentos): evento [direccion]; devuelve el testigo a[19].
inline u32 __cdecl FkGetMgr() { PushEv(0x5EE190, 0); return Resolve32(gA[19]); }
// CPedAttractorManager::HasEmptySlot (0x5EBB00; thiscall, dos punteros de pila, `ret 8`): evento [direccion, ECX (etiqueta), efecto (etiqueta), entidad (etiqueta)]; el byte bajo del valor sale del bit (k % 32) de a[17].
inline u32 __fastcall FkHasEmpty(void* self, void*, void* fx, void* entity) {
    Ev& e = PushEv(0x5EBB00, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(fx); e.v[2] = Tag(entity);
    const u32 k = gHasCalls++;
    const u32 low = ((gA[17] >> (k % 32)) & 1) ? (k % 3 == 0 ? 0x80u : 1u) : 0u;
    return (gA[20] & 0xFFFFFF00u) | low;
}

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
    std::array<u32, 9> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

// Aplica el guion de la escena (arenas y memoria global) y devuelve la llamada
inline Call Setup(const C& c) {
    Call call;
    arenaA.Pattern(c.a[2], c.a[36]); arenaB.Pattern(c.a[2], c.a[37]); arenaC.Pattern(c.a[2], c.a[38]); arenaD.Pattern(c.a[2], c.a[39]);
    gA = c.a;
    gDestroyCalls = gGet2dCalls = gXformCalls = gHasCalls = 0; gCountPtr = nullptr;
    size_t i = 0;
    const auto& s = c.s;
    while (i < s.size()) {
        const u8 op = s[i];
        u32 addr; memcpy(&addr, &s[i + 1], 4);
        u16 len; memcpy(&len, &s[i + 5], 2);
        const u8* data = &s[i + 7];
        i += 7 + len;
        u8 buf[0x400];
        if (len > sizeof(buf)) std::abort();
        memcpy(buf, data, len);
        if (op & 2) { u32 v; memcpy(&v, buf, 4); v = Resolve32(v); memcpy(buf, &v, 4); }
        if (op & 1) memcpy(reinterpret_cast<void*>(uintptr_t(addr)), buf, len);
        else {
            auto& ar = *gArenas[addr >> 24];
            memcpy(ar.m.data() + kGuard + (addr & 0xFFFFFF), buf, len);
        }
    }
    for (u32 k = 0; k < c.nargs; ++k) call.args[k] = Resolve32(c.args[k]);
    call.argCount = c.nargs;
    call.popBytes = kPop[c.fn];
    call.ecx = Resolve32(c.ecx);
    gThis = call.ecx;
    return call;
}

// Forma del valor devuelto: 0 void, 1 puntero (etiqueta de arena), 2 bool (solo AL), 3 EAX completo
inline constexpr std::array<u32, kFunctions> kRetShape{1, 0, 0, 0, 1, 0, 1, 0, 2, 1, 1, 1, 2, 1, 1, 0, 1, 1, 0, 1, 1, 3, 0};

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kRetShape[fn]) {
        case 1: obs["ret"] = Tag(uintptr_t(eax)); break;       // puntero como etiqueta de arena
        case 2: obs["ret"] = eax & 0xFFu; break;               // bool
        case 3: obs["ret"] = eax; break;
        default: obs["ret"] = nullptr; break;                  // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN203A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN203A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador con hook V (RH_ScopedVMTInstall): la ruta usa el cuerpo de la DLL de la ranura (la funcion que la DLL llama de forma no virtual) y el hook se apaga.
inline Pair MakeCollabV(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody)) {
        if (vbody) env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        else env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", vbody != 0}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn203a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn203a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn203a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado (las 23 funciones del lote quedan apagadas durante todo el banco salvo la candidata de cada par).
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Funcion del propio lote (hook apagado durante el banco): se usa su ruta (sin otra HookRoute) y su cuerpo en la DLL
    auto Lot = [&](unsigned idx) { return Pair{kEntries[idx], Body(env.route[idx]->Body()), true}; };
    env.stubs[kFlushA] = {ByExeAddress(0x8214BD, &FkDelete)};
    env.stubs[kFlushB] = {ByExeAddress(0x8214BD, &FkDelete)};
    env.stubs[kTexCb] = {ByExeAddress(0x7F39C0, &FkTexRemove), ByExeAddress(0x7F3820, &FkTexDestroy)};
    env.stubs[kAtomCb] = {F(0x72FB30, &FkNodeName)};
    env.stubs[kFindAttr] = {F(0x564A20, &FkFindObjects), F(0x4C4C70, &FkGet2d), F(0x5334F0, &FkTransform), F(0x5EE190, &FkGetMgr), F(0x5EBB00, &FkHasEmpty)};
    env.stubs[kVmiCtor] = {F(0x4C4A60, &FkBody<0x4C4A60>)};
    env.stubs[kVmiSdd] = {F(0x4C5920, &FkBody<0x4C5920>), ByExeAddress(0x8214BD, &FkDelete)};
    env.stubs[kVsDtor] = {ByExeAddress(0x749DC0, &FkAtomDestroy), ByExeAddress(0x7F05A0, &FkFrameDestroy)};
    env.stubs[kVsSdd] = {S(Lot(kVsDtor), reinterpret_cast<void*>(&FkVsDtor))};
    env.stubs[kAvCtor] = {F(0x61A3B0, &FkBody<0x61A3B0>), F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[kAvDtor] = {F(0x571A00, &FkRef<0x571A00>), F(0x61A3D0, &FkBody<0x61A3D0>)};
    // colaboradores REALES con hook (del lote) apagado: el EXE corre sus bytes y la DLL su cuerpo
    env.reals[kEmpty] = {Lot(kPoolDelOct)};
    env.reals[kOctSdd] = {Lot(kEmpty)};
    env.reals[kRemoveChildren] = {Lot(kPoolDelOct)};
    env.reals[kInsertTree] = {Lot(kPoolNewOct), Lot(kOctCtor), Lot(kOctSdd), Lot(kRemoveChildren)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f : {unsigned(kFlushA), unsigned(kFlushB), unsigned(kVmiSdd)}) dllReal[f] = "toda la funcion salvo operator delete (0x8214BD: la DLL lo llama por la direccion del EXE, falso)";
    dllReal[kTexCb] = "toda la funcion salvo RwTexDictionaryRemoveTexture (0x7F39C0) y RwTextureDestroy (0x7F3820): la DLL los llama por la direccion del EXE (falsos)";
    dllReal[kAtomCb] = "toda la funcion salvo GetFrameNodeName (0x72FB30, con hook: falso identico en la direccion del EXE y en el cuerpo de la DLL); _stricmp real (el CRT de la DLL)";
    dllReal[kFindAttr] = "toda la funcion salvo CWorld::FindObjectsInRange, CBaseModelInfo::Get2dEffect, CEntity::TransformFromObjectSpace, GetPedAttractorManager y CPedAttractorManager::HasEmptySlot (con hook: falsos identicos en el EXE y en la DLL); _stricmp real";
    dllReal[kVsDtor] = "toda la funcion salvo RpAtomicDestroy (0x749DC0) y RwFrameDestroy (0x7F05A0): la DLL los llama por la direccion del EXE (falsos); el EXE ademas llama al iterador de destructores de vectores 0x821E02 (real, cuerpo 0x4C7400 vacio)";
    dllReal[kVsSdd] = "toda la funcion salvo el destructor 0x4C7410 (del lote, con hook: falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kVmiCtor] = "toda la funcion salvo el cuerpo de CBaseModelInfo 0x4C4A60 (con hook: falso identico)";
    dllReal[kAvCtor] = "toda la funcion salvo CTaskComplex 0x61A3B0 y CEntity::RegisterReference 0x571B70 (con hook: falsos identicos)";
    dllReal[kAvDtor] = "toda la funcion salvo CEntity::CleanUpOldReference 0x571A00 y CTaskComplex 0x61A3D0 (con hook: falsos identicos)";
    dllReal[kEmpty] = "toda la funcion; la DLL libera las ranuras con su propio codigo y el EXE con 0x5A73C0 (del lote, hook apagado)";
    dllReal[kOctSdd] = "toda la funcion; el EXE llama a 0x5A6FC0 (del lote, hook apagado) y la DLL a su propio vaciado";
    dllReal[kInsertTree] = "toda la funcion; la llamada virtual al hijo cae en la propia entrada (candidata encendida, original apagada); el EXE llama a 0x5A7350, 0x5A6DB0, 0x5A7490 y 0x5A74F0 (del lote, hooks apagados) y la DLL a su propio codigo";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada (se restaura tras cada lado)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kTreeBase, kTreeBytes);
    regionsOf[kEmpty] = regionsOf[kOctSdd] = regionsOf[kRemoveChildren] = regionsOf[kInsertTree] = {regions.size() - 1};
    regions.emplace_back(kVsPoolPtr, 4);
    regionsOf[kVsOpDelete] = regionsOf[kVsSdd] = {regions.size() - 1};
    regions.emplace_back(kModelRegionLo, kModelRegionBytes);
    regionsOf[kFindAttr] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
