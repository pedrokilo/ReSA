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

// Banco par a par Fn232A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 25 funciones del EXE (ver Fixtures.hpp: RpWorld, Fx y la libjpeg 6b embebida), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall) y V (FxEmitter_c::Init, Reset y Update).
// El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40) segun el caso declarativo (cabecera + escrituras + retornos programados).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL; los que no, solo en la direccion del EXE. Las funciones
// que se llaman por puntero (error_exit, alloc_small, process_data...: los punteros viven en las estructuras del caso) y las listas libres de RwEngineInstance (+0x148, dos tablas falsas E1 y E2) son funciones
// falsas del banco. Se observan: los bytes cambiados de las cuatro arenas, las regiones globales vigiladas (gchanges), los eventos de los falsos (con sus argumentos; los punteros a las arenas se anotan como
// (arena, desplazamiento)), el valor devuelto (EAX, AL o nada segun la funcion) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn232a-crash.txt (junto al EXE).
namespace F232ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn232a-crash.txt";
    char text[2048]{};
    const auto* c = p->ContextRecord;
    int n = wsprintfA(text, "av eip=%08X addr=%08X rw=%u eax=%08X ecx=%08X edx=%08X esi=%08X edi=%08X ebp=%08X esp=%08X\r\nstack:", unsigned(c->Eip), unsigned(p->ExceptionRecord->ExceptionInformation[1]), unsigned(p->ExceptionRecord->ExceptionInformation[0]),
        unsigned(c->Eax), unsigned(c->Ecx), unsigned(c->Edx), unsigned(c->Esi), unsigned(c->Edi), unsigned(c->Ebp), unsigned(c->Esp));
    const unsigned* sp = reinterpret_cast<const unsigned*>(c->Esp);
    for (int i = 0; i < 48 && !IsBadReadPtr(sp + i, 4); ++i) n += wsprintfA(text + n, " %08X", sp[i]);
    HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(c->Eip), &m);
    n += wsprintfA(text + n, "\r\nmodule=%08X\r\n", unsigned(reinterpret_cast<uintptr_t>(m)));
    HANDLE f = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) { DWORD w{}; WriteFile(f, text, DWORD(n), &w, nullptr); CloseHandle(f); }
    return EXCEPTION_CONTINUE_SEARCH;
}
inline const bool installed = [] { AddVectoredExceptionHandler(1, Handler); return true; }();
}

namespace F232ALive {
using namespace F232AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F232AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put(size_t off, u32 width, u32 v) { memcpy(m.data() + kGuard + off, &v, width); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }

// ================================================================================================ falsos de los colaboradores
inline u32 gPokeId{}, gPokeIdx{}, gPokeArena{}, gPokeOff{}, gPokeVal{};
inline DWORD gBenchThread{};
// Retornos programados del caso (id, numero de llamada, valor) y contadores de llamadas por id (sin montón: los falsos se ejecutan con el operator new del ASI parcheado)
inline std::array<u32, 3 * 96> gRets{};
inline unsigned gNRets{};
inline constexpr std::array<u32, 5> kFixedIds{0x5C6BB0, 0x5C6BD0, 0x5CCCC0, 0x823674, 0x823E86};
inline std::array<u32, 5 + 14> gCnt{};
inline unsigned IdIdx(u32 id) {
    if (id >= 0xC100 && id <= 0xC10D) return 5 + (id - 0xC100);
    for (unsigned i = 0; i < kFixedIds.size(); ++i) if (kFixedIds[i] == id) return i;
    std::abort();
}
// Especificacion de valor del caso -> valor real (arenas, tablas E1/E2 y funciones falsas por puntero)
inline void* FakeFn(u32 k);
inline u32 Res(u32 v) {
    if (IsPS(v)) return u32(ArenaAt((v >> 28) - 0xAu).Obj(v & 0x0FFFFFFFu));
    if (IsFS(v)) return u32(uintptr_t(FakeFn(v & 0xFFu)));
    return v;
}
inline u32 NextCall(u32 id) { return gCnt[IdIdx(id)]++; }
inline u32 RetFor(u32 id, u32 n) {
    for (unsigned i = 0; i < gNRets; ++i) if (gRets[3 * i] == id && gRets[3 * i + 1] == n) return Res(gRets[3 * i + 2]);
    return 0;
}
// Efecto lateral del caso: tras anotar el evento `id` (la llamada gPokeIdx de ese id) escribe 4 bytes en la arena gPokeArena, desplazamiento gPokeOff
inline void PokeIf(u32 id, u32 n) {
    if (!gPokeId || id != gPokeId || n != gPokeIdx) return;
    memcpy(reinterpret_cast<void*>(ArenaAt(gPokeArena).Obj(gPokeOff)), &gPokeVal, 4);
}
inline u32 Done(u32 id, u32 n) { PokeIf(id, n); return RetFor(id, n); }
#define FK_BEGIN(ID, LEN) Ev& e = PushEv(ID, LEN); const u32 n = NextCall(ID); (void)e;

// --- funciones falsas fijas (cdecl salvo las thiscall: __fastcall con ECX y EDX, que limpia los argumentos de pila como el `ret n` del original)
inline u32 __cdecl FkFwrite(u32 buf, u32 size, u32 count, u32 file) { FK_BEGIN(0x823674, 4) e.v[0] = Tag(uintptr_t(buf)); e.v[1] = size; e.v[2] = count; e.v[3] = Tag(uintptr_t(file)); return Done(0x823674, n); }   // fwrite del CRT del EXE
inline u32 __cdecl FkFflush(u32 file) { FK_BEGIN(0x823E86, 1) e.v[0] = Tag(uintptr_t(file)); return Done(0x823E86, n); }   // fflush del CRT del EXE
template<u32 ID> inline u32 __cdecl FkCi1(u32 ci) { FK_BEGIN(ID, 1) e.v[0] = Tag(uintptr_t(ci)); return Done(ID, n); }
template<u32 ID> inline u32 __cdecl FkCi2b(u32 ci, u32 b) { FK_BEGIN(ID, 2) e.v[0] = Tag(uintptr_t(ci)); e.v[1] = b & 0xFFu; return Done(ID, n); }
// --- funciones falsas llamadas por puntero (libjpeg): id 0xC100 + k
inline u32 __cdecl FkEmit(u32 ci, u32 level) { FK_BEGIN(0xC101, 2) e.v[0] = Tag(uintptr_t(ci)); e.v[1] = level; return Done(0xC101, n); }
inline u32 __cdecl FkAllocSmall(u32 ci, u32 pool, u32 size) { FK_BEGIN(0xC103, 3) e.v[0] = Tag(uintptr_t(ci)); e.v[1] = pool; e.v[2] = size; return Done(0xC103, n); }
inline u32 __cdecl FkAllocLarge(u32 ci, u32 pool, u32 size) { FK_BEGIN(0xC104, 3) e.v[0] = Tag(uintptr_t(ci)); e.v[1] = pool; e.v[2] = size; return Done(0xC104, n); }
inline u32 __cdecl FkAllocSarray(u32 ci, u32 pool, u32 w, u32 h) { FK_BEGIN(0xC105, 4) e.v[0] = Tag(uintptr_t(ci)); e.v[1] = pool; e.v[2] = w; e.v[3] = h; return Done(0xC105, n); }
inline u32 __cdecl FkReqBarray(u32 ci, u32 pool, u32 pre, u32 w, u32 h, u32 v) { FK_BEGIN(0xC106, 6) e.v[0] = Tag(uintptr_t(ci)); e.v[1] = pool; e.v[2] = pre; e.v[3] = w; e.v[4] = h; e.v[5] = v; return Done(0xC106, n); }
inline u32 __cdecl FkProcess(u32 ci, u32 scan, u32 rowctr, u32 nl) {
    FK_BEGIN(0xC10C, 4) e.v[0] = Tag(uintptr_t(ci)); e.v[1] = Tag(uintptr_t(scan)); e.v[2] = Tag(uintptr_t(rowctr)); e.v[3] = nl;
    PokeIf(0xC10C, n);
    const u32 v = RetFor(0xC10C, n);
    memcpy(reinterpret_cast<void*>(uintptr_t(rowctr)), &v, 4);
    return 0;
}
inline void* FakeFn(u32 k) {
    switch (k) {
    case 0: return reinterpret_cast<void*>(&FkCi1<0xC100>);
    case 1: return reinterpret_cast<void*>(&FkEmit);
    case 2: return reinterpret_cast<void*>(&FkCi1<0xC102>);
    case 3: return reinterpret_cast<void*>(&FkAllocSmall);
    case 4: return reinterpret_cast<void*>(&FkAllocLarge);
    case 5: return reinterpret_cast<void*>(&FkAllocSarray);
    case 6: return reinterpret_cast<void*>(&FkReqBarray);
    case 7: return reinterpret_cast<void*>(&FkCi1<0xC107>);
    case 8: return reinterpret_cast<void*>(&FkCi1<0xC108>);
    case 9: return reinterpret_cast<void*>(&FkCi1<0xC109>);
    case 10: return reinterpret_cast<void*>(&FkCi1<0xC10A>);
    case 11: return reinterpret_cast<void*>(&FkCi1<0xC10B>);
    case 12: return reinterpret_cast<void*>(&FkProcess);
    default: return reinterpret_cast<void*>(&FkCi1<0xC10D>);
    }
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
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{}, eax{};
    bool floatRet{};
};

// Prepara las arenas y las regiones globales del caso; devuelve la llamada. Las escrituras son las del oraculo (fixture_oracle.py).
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx; call.eax = c.eax;
    const auto& a = c.a;
    arenaA.Pattern(a[8], a[9]); arenaB.Pattern(a[8], a[10]); arenaC.Pattern(a[8], a[11]); arenaD.Pattern(a[8], a[12]);
    gCnt.fill(0);
    gNRets = unsigned(c.r.size() / 3);
    if (gNRets > 96) std::abort();
    for (size_t i = 0; i < c.r.size(); ++i) gRets[i] = c.r[i];
    for (size_t i = 0; i + 2 < c.p.size(); i += 3) {
        const u32 dest = c.p[i] >> 8, width = c.p[i] & 0xFFu, off = c.p[i + 1], v = c.p[i + 2];
        if (dest == 4) { const u32 w = Res(v); memcpy(reinterpret_cast<void*>(uintptr_t(off)), &w, 4); }
        else ArenaAt(dest).Put(off, width, width == 4 ? Res(v) : v);
    }
    gPokeId = a[14]; gPokeIdx = a[15]; gPokeArena = a[16]; gPokeOff = a[17]; gPokeVal = a[18] ? Res(a[19]) : a[19];
    call.argCount = a[0];
    for (u32 i = 0; i < call.argCount; ++i) call.args[i] = Res(a[1 + i]);
    if (a[7]) call.ecx = Res(a[7]);
    if (a[21]) call.eax = Res(a[21]);   // build_ycc_rgb_table recibe cinfo en EAX
    // todas cdecl (el llamador limpia la pila); build_ycc_rgb_table no tiene argumentos de pila
    call.popBytes = 4 * call.argCount;
    return call;
}

enum RetKind { rNone, rEax, rAl };
inline RetKind RetOf(unsigned fn) { return fn == 7 ? rEax : (fn == 8 ? rAl : rNone); }

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
        nev = 0; gThrew = false;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        const RetKind rk = RetOf(fn);
        if (!gThrew && rk == rEax) obs["ret"] = eax; else if (!gThrew && rk == rAl) obs["ret"] = eax & 0xFFu; else obs["ret"] = nullptr;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN232A_CASES (solo para el humo y las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN232A_CASES", buffer, sizeof(buffer)) > 0) {
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
        char fromBuf[16]{};
        const unsigned firstFn = GetEnvironmentVariableA("FN232A_FROM", fromBuf, sizeof(fromBuf)) > 0 ? unsigned(std::atoi(fromBuf)) : 0u;   // solo para diagnosticar (la corrida de evidencia no la usa)
        for (unsigned f = firstFn; f < kFunctions; ++f) {
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
    // El banco ya escribio todos los resultados: el juego sale aqui. Con la partida cargada el siguiente fotograma tras una corrida larga pierde el dispositivo D3D (asercion de RenderWare ajena al lote)
    // antes de que el analizador termine; la salida inmediata deja el registro acabado en `complete` (o `error`).
    log.flush();
    ExitProcess(ready && totals.passed && stateRestored ? 0u : 1u);
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
    if (!std::filesystem::exists(dir / "fn232a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn232a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn232a"); }
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
    if (!ready) {
        for (unsigned f = 0; f < kFunctions; ++f) collaborators[kFn[f]] = {{"own_hook", ownHooks[kFn[f]]}, {"body", env.route[f] ? env.route[f]->Body() : 0}, {"body_module", env.route[f] ? ModuleOf(env.route[f]->Body()) : 0}};
        Run(env, false, registered, bodies, collaborators);
        return;
    }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Un colaborador con hook RH que es tambien una funcion probada reutiliza su ruta (el hook ya esta apagado); si no, se crea una ruta de colaborador
    auto Collab = [&](uintptr_t exe) -> Pair {
        auto it = collabCache.find(exe);
        if (it != collabCache.end()) return it->second;
        Pair p{exe, 0, false};
        for (unsigned f = 0; f < kFunctions; ++f)
            if (kEntries[f] == exe && env.route[f]) { p.dll = Body(env.route[f]->Body()); p.hooked = true; hooksFound.push_back({{"exe", exe}, {"tested_function", kFn[f]}}); }
        if (!p.hooked) p = MakeCollab(env, exe, hooksFound);
        return collabCache.emplace(exe, p).first->second;
    };
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en la direccion del EXE y en el cuerpo de la DLL. `expectHook` = false: CRT o salto puro del EXE sin hook (solo el EXE).
    auto F = [&](uintptr_t exe, auto* fake, bool expectHook = true) {
        const Pair p = Collab(exe);
        if (expectHook && (!p.hooked || !p.dll)) ready = false;
        if (!expectHook && p.hooked) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x5C6BB0, &FkCi1<0x5C6BB0>)};
    env.stubs[3] = {F(0x5C6BB0, &FkCi1<0x5C6BB0>), F(0x5C6BD0, &FkCi1<0x5C6BD0>)};
    env.stubs[8] = {F(0x5CCCC0, &FkCi1<0x5CCCC0>)};
    env.stubs[9] = {F(0x823674, &FkFwrite, false), F(0x823E86, &FkFflush, false)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en el cuerpo de la DLL); funciones de la libjpeg llamadas por puntero y RwEngineInstance falsas";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Regiones globales del juego que el banco fija y vigila: una por region; cada funcion declara las suyas
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (const u32 g : kGlobal) regions.emplace_back(g, 4);
    for (unsigned f = 0; f < kFunctions; ++f) for (const u32 gi : GlobalsOf(f)) regionsOf[f].push_back(gi);
    Run(env, ready, registered, bodies, collaborators);
}
}
