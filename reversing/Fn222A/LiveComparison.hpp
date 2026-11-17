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

// Banco par a par Fn222A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 16 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedNamedGlobalInstall / RH_ScopedNamedInstall / RH_ScopedInstall) y uno V (CEntity::Add(), ranura 2 de la vtable
// 0x863928). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F): CEntity::CEntity 0x532A90, CAESound::~CAESound 0x4EF660,
// CDecision::SetDefault 0x600530, operator new/delete de nodos 0x552380/0x552390/0x5523C0/0x5523D0 y CEntity::~CEntity 0x535E90; operator delete del CRT 0x8214BD solo en la direccion del EXE (ByExeAddress).
// CEntity::Add() llama por la vtable del OBJETO: el banco construye vtables falsas en las arenas (huecos 1 y 9 con falsos que anotan sus argumentos; los demas huecos anotan que se llamaron por error).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (punteros como etiqueta de arena; AL en IsInvisibleOrRenderDamaged) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).


// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn222a-crash.txt (junto al EXE).
namespace F222ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn222a-crash.txt";
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

namespace F222ALive {
using namespace F222AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F222AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
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
inline u32 gNew{};                   // lo que devuelven los falsos de operator new de nodos
inline u32 gRet{};                   // EAX que devuelven los falsos thiscall
inline u32 gW0{}, gW1{}, gW2{};      // lo que escriben los falsos de CEntity::CEntity y de los destructores
inline u32 gRect[4]{};               // rectangulo que escribe el falso de GetBoundRect
inline u32 gAddMode{}, gVt2Addr{};   // modo del falso de GetBoundRect y la vtable a la que cambia el objeto
inline DWORD gBenchThread{};

inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }

// operator new de nodos (0x552380 / 0x5523C0, cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// operator delete (0x8214BD, CRT) y operator delete de nodos (0x552390 / 0x5523D0): cdecl, un puntero CRUDO (no se desreferencia); evento [direccion, puntero (etiqueta)].
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(p));
}
// CEntity::CEntity (0x532A90, thiscall sin argumentos): evento [direccion, ECX (etiqueta)]; escribe gW0 en +0, gW1 en +0x1C y el byte gW2 en +0x36 (para comprobar el orden de las escrituras del constructor).
inline u32 __fastcall FkEntityCtor(void* self, void*) {
    Ev& e = PushEv(0x532A90, 1);
    DumpTag(e, 0, uintptr_t(self));
    u8* p = static_cast<u8*>(self);
    memcpy(p, &gW0, 4); memcpy(p + 0x1C, &gW1, 4); p[0x36] = u8(gW2);
    return gRet;
}
// CAESound::~CAESound (0x4EF660) y CEntity::~CEntity (0x535E90): thiscall sin argumentos; evento [direccion, ECX (etiqueta)]; escribe gW0 en [ECX].
template<u32 ADDR> inline u32 __fastcall FkDtorW(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(self));
    memcpy(self, &gW0, 4);
    return gRet;
}
// CDecision::SetDefault (0x600530, thiscall sin argumentos): evento [direccion, ECX (etiqueta)].
inline u32 __fastcall FkDecision(void* self, void*) {
    Ev& e = PushEv(0x600530, 1);
    DumpTag(e, 0, uintptr_t(self));
    return gRet;
}
// Ranuras de las vtables falsas de CEntity::Add(): GetBoundRect (hueco 9, thiscall con la salida CRect en la pila): evento [0xB000 + vtable, ECX, salida (etiqueta de pila)]; escribe gRect en la salida; la vtable 1 con
// gAddMode = 1 cambia la vtable del objeto a la 2; devuelve la salida. Add(rect) (hueco 1): evento [0xC000 + vtable, ECX, puntero, 4 palabras del rectangulo]. Las demas ranuras anotan [0xD000 + 0x100 * vtable + hueco, ECX].
template<int VT> inline u32 __fastcall FkBound(void* self, void*, u32* out) {
    Ev& e = PushEv(0xB000 + VT, 2);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(out));
    for (int k = 0; k < 4; ++k) out[k] = gRect[k];
    if (VT == 1 && gAddMode == 1) memcpy(self, &gVt2Addr, 4);
    return u32(uintptr_t(out));
}
template<int VT> inline u32 __fastcall FkAddRect(void* self, void*, const u32* r) {
    Ev& e = PushEv(0xC000 + VT, 6);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(r));
    for (int k = 0; k < 4; ++k) e.v[2 + k] = r[k];
    return 0;
}
template<int VT, int S> inline u32 __fastcall FkWrong(void* self, void*) {
    Ev& e = PushEv(u32(0xD000 + VT * 0x100 + S), 1);
    DumpTag(e, 0, uintptr_t(self));
    return 0;
}
template<int VT, size_t... S> inline void FillVtable(u32* out, std::index_sequence<S...>) {
    ((out[S] = u32(uintptr_t(S == 9 ? reinterpret_cast<void*>(&FkBound<VT>) : (S == 1 ? reinterpret_cast<void*>(&FkAddRect<VT>) : reinterpret_cast<void*>(&FkWrong<VT, int(S)>))))), ...);
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
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

// Prepara arenas del caso; devuelve la llamada
inline u8* At(uintptr_t addr) { return reinterpret_cast<u8*>(addr); }
inline void PutStr(uintptr_t addr, u32 len, u32 seed) {
    u8* p = At(addr);
    for (u32 i = 0; i < len; ++i) p[i] = u8(StrChar(seed, i));
    p[len] = 0;
}
inline void Put32At(uintptr_t addr, u32 v) { memcpy(At(addr), &v, 4); }
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gNew = 0; gRet = 0; gW0 = gW1 = gW2 = 0; gAddMode = 0; gVt2Addr = 0;
    for (auto& r : gRect) r = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kStdEmpty:
        call.args[0] = a[5] == 0 ? a[4] : u32(arenaB.Obj(a[4])) + (a[5] - 1); call.argCount = 1; call.popBytes = 0;
        break;
    case kEnc:
    case kDec: {
        const u32 key = a[4] <= 1 ? u32(arenaB.Obj(a[5])) : u32(arenaA.Obj(a[5]));
        call.args[0] = thisA; call.args[1] = key; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kBldCtor:
        gW0 = a[4]; gW1 = a[5]; gW2 = a[6];
        break;
    case kAudDtor:
        gW0 = a[4];
        break;
    case kMission:
        arenaA.Put8(o + 0x2A, a[4]);
        call.args[0] = a[5]; call.argCount = 1; call.popBytes = 0;
        break;
    case kDecision:
        break;
    case kInvis:
        arenaA.Put32(o + 0x1C, a[4]);
        break;
    case kTextPair: {
        const u32 L1 = a[4], L2 = a[5], m1 = a[6], off1 = a[7], m2 = a[8], off2 = a[9];
        uintptr_t src1, src2;
        if (m1 == 0) src1 = arenaB.Obj(off1);
        else if (m1 == 1) src1 = arenaC.Obj(off1);
        else src1 = uintptr_t(thisA) + off1;
        PutStr(src1, L1, a[10]);
        if (m2 == 0) {
            const bool inC = m1 == 0 ? true : (m1 == 1 ? false : a[11] != 0);
            src2 = inC ? arenaC.Obj(off2) : arenaB.Obj(off2);
            PutStr(src2, L2, a[12]);
        } else if (m2 == 2) {
            src2 = uintptr_t(thisA) + 0x18 + off2;
            PutStr(src2, L2, a[12]);
        } else {
            src2 = uintptr_t(thisA) + off2;
        }
        call.args[0] = u32(src1); call.args[1] = u32(src2); call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kEntAdd: {
        const u32 vt1 = u32(arenaB.Obj(a[5])), vt2 = u32(arenaC.Obj(a[6]));
        u32 tab[24];
        FillVtable<1>(tab, std::make_index_sequence<24>{});
        memcpy(At(vt1), tab, sizeof(tab));
        FillVtable<2>(tab, std::make_index_sequence<24>{});
        memcpy(At(vt2), tab, sizeof(tab));
        arenaA.Put32(o, vt1);
        gAddMode = a[4]; gVt2Addr = vt2;
        for (int k = 0; k < 4; ++k) gRect[k] = a[7 + k];
        break;
    }
    case kSAdd:
    case kDAdd: {
        const u32 heads[5] = {0u, u32(arenaC.Obj(a[5])), thisA, thisA + 4, thisA - 4};
        const u32 head = heads[a[4]];
        arenaA.Put32(o, head);
        const u32 mode = a[6];
        const u32 nodes[7] = {u32(arenaD.Obj(a[7])), u32(arenaD.Obj(a[7])), thisA, thisA - 4, thisA - 8, thisA + 4, head ? head : u32(arenaD.Obj(a[7]))};
        gNew = nodes[mode];
        call.args[0] = a[9] == 0 ? a[8] : (a[9] == 1 ? thisA : u32(arenaC.Obj(a[5]))); call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kSRem:
    case kDRem: {
        const u32 L = a[4], base = a[5], sep = a[6], idx = a[7], imode = a[8], iseed = a[9], sm = a[10], incoh = a[11];
        const bool dbl = kKind[c.fn] == kDRem;
        u32 nodes[8]{}, items[8]{};
        for (u32 i = 0; i < L; ++i) { nodes[i] = u32(arenaB.Obj(base + sep * i)); items[i] = ItemValue(iseed, imode, i); }
        for (u32 i = 0; i < L; ++i) {
            Put32At(nodes[i], items[i]);
            Put32At(nodes[i] + 4, i + 1 < L ? nodes[i + 1] : 0u);
            if (dbl) Put32At(nodes[i] + 8, i > 0 ? nodes[i - 1] : 0u);
        }
        if (dbl && idx < L && incoh == 1) Put32At(nodes[idx] + 8, 0u);
        else if (dbl && idx < L && incoh == 2) Put32At(nodes[idx] + 8, u32(arenaC.Obj(a[12])));
        arenaA.Put32(o, L ? nodes[0] : 0u);
        const u32 search = sm == 0 ? items[idx] : (sm == 1 ? (0xE0000000u | (iseed & 0x0FFFFFFFu)) : 0u);
        call.args[0] = search; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kScalarDel:
        gW0 = a[5];
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    return call;
}

enum RetKind { rNone, rTag, rAL };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kBldCtor: case kTextPair: case kSAdd: case kDAdd: case kScalarDel: return rTag;   // this o el nodo, como etiqueta de arena
    case kInvis: return rAL;                                                                 // booleano en AL
    default: return rNone;
    }
}

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
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        switch (gThrew ? rNone : RetOf(kKind[fn])) {
        case rTag: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case rAL: obs["ret"] = eax & 0xFFu; break;           // booleano (AL)
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN222A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN222A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn222a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn222a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn222a"); }
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
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador con hook RH que la DLL llama POR LA DIRECCION DEL EXE (plugin::Call): el falso va solo en la direccion del EXE (el hook queda apagado)
    auto FE = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Constructor de C++ (RH_ScopedOverloadedInstall(Constructor, ...)): el hook apunta a una envoltura `Constructor(...)` que llama al constructor real; `new T(...)` del codigo de la DLL llama a este ultimo, asi que el falso va
    // en el constructor real, cuya direccion sale de la envoltura (la primera llamada de la envoltura: el marco no tiene llamadas de comprobacion antes). El EXE lo llama por la direccion del EXE (falso en la entrada con el hook apagado).
    auto FC = [&](uintptr_t exe, auto* fake) {
        Pair p = Collab(exe);
        uintptr_t ctor = 0;
        for (uintptr_t a = p.dll; p.dll && a < p.dll + 0x200 && !ctor; ++a) {
            const uint8* q = reinterpret_cast<const uint8*>(a);
            if (q[0] == 0xE8) ctor = Body(a + 5 + *reinterpret_cast<const int32*>(a + 1));
        }
        hooksFound.push_back({{"ctor_wrapper", p.dll}, {"ctor", ctor}, {"wrapper_bytes", p.dll ? HexBytes(reinterpret_cast<void*>(p.dll), 96) : std::string()}});
        if (!p.hooked || !ctor || ModuleOf(ctor) != env.asi) ready = false;
        p.dll = ctor;
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto DelEv = [&] { return ByExeAddress(0x8214BD, &FkDelete<0x8214BD>); };
    env.stubs[3] = {F(0x532A90, &FkEntityCtor)};                                   // CEntity::CEntity
    env.stubs[4] = {F(0x4EF660, &FkDtorW<0x4EF660>)};                              // CAESound::~CAESound
    env.stubs[5] = {F(0x4EF660, &FkDtorW<0x4EF660>)};
    env.stubs[7] = {F(0x600530, &FkDecision)};                                     // CDecision::SetDefault
    env.stubs[11] = {F(0x552380, &FkNew<0x552380>)};                               // operator new de nodos simples
    env.stubs[12] = {F(0x552390, &FkDelete<0x552390>)};                            // operator delete de nodos simples
    env.stubs[13] = {F(0x5523C0, &FkNew<0x5523C0>)};                               // operator new de nodos dobles
    env.stubs[14] = {F(0x5523D0, &FkDelete<0x5523D0>)};                            // operator delete de nodos dobles
    env.stubs[15] = {F(0x535E90, &FkDtorW<0x535E90>), DelEv()};                    // CEntity::~CEntity y operator delete del CRT
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u, 2u, 6u, 8u, 9u, 10u}) dllReal[f] = "toda la funcion (sin colaboradores falseados)";
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
    // Ninguna memoria global del juego se toca (los colaboradores que lo harian estan falseados)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}

