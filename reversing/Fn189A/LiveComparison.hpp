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

// Banco par a par Fn189A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 29 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall / RH_ScopedOverloadedInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) para los que tienen hook
// (CStreaming::SetMissionDoesntRequireModel 0x409C90, CStreaming::RemoveModel 0x4089A0, CClock::GetIsTimeInRange 0x52CEE0, CStreaming::RequestModel 0x4087E0), y solo en la direccion del EXE para los que la DLL llama por ella
// (operator delete 0x8214BD y operator new[] 0x821195). ~CDirectory (0x5322D0) corre REAL (EXE con el hook apagado, DLL su ~CDirectory). Las virtuales CEntity::CreateRwObject y CBaseModelInfo::GetTimeInfo de
// ProcessEntitiesInSectorList se resuelven por vtables falsas en la arena D. Memoria global del juego tocada (restaurada tras cada lado): ver Update (Regiones).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; sin valor en las funciones void) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F189ALive {
using namespace F189AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F189AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline DWORD gBenchThread{};
inline const u8* gPool{};                       // constructor de CPool<CVehicleStructure>: objeto que se construye (el falso de new[] lee su +0 y +4 al entrar)
inline u32 gNewRet[2]{};                        // lo que devuelven las dos llamadas a operator new[]
inline unsigned gNewCall{};
inline u32 gClockBits{}, gClockNoise{};         // ProcessEntities: "en rango" por llamada (bit n de gClockBits) y ruido de los 24 bits altos de EAX
inline unsigned gClockCall{};
inline u32 gCreateMode{}, gCreateWord{};        // ProcessEntities: el falso de CreateRwObject reescribe el WORD +0x22 de la entidad

// operator delete del CRT del EXE (0x8214BD; cdecl, un puntero CRUDO): evento [direccion, argumento (etiqueta o crudo)]
inline void __cdecl FkDelete(void* p) { Ev& e = PushEv(0x8214BD, 1); e.v[0] = Tag(p); }
// operator new[] del CRT del EXE (0x821195; cdecl, tamano): evento [direccion, tamano, dword [pool+0], dword [pool+4]] (orden de las escrituras del constructor); devuelve el bloque que toca
inline u32 __cdecl FkNewArray(u32 size) {
    Ev& e = PushEv(0x821195, 3);
    e.v[0] = size; e.v[1] = gPool ? BitsOf(gPool) : 0u; e.v[2] = gPool ? BitsOf(gPool + 4) : 0u;
    return gNewRet[gNewCall < 2 ? gNewCall++ : 1];
}
// CStreaming::SetMissionDoesntRequireModel (0x409C90) y CStreaming::RemoveModel (0x4089A0): cdecl, un argumento; evento [direccion, argumento]
inline void __cdecl FkMission(u32 id) { Ev& e = PushEv(0x409C90, 1); e.v[0] = id; }
inline void __cdecl FkRemove(u32 id) { Ev& e = PushEv(0x4089A0, 1); e.v[0] = id; }
// CClock::GetIsTimeInRange (0x52CEE0; cdecl, dos bytes): evento [direccion, encendido (byte), apagado (byte)]; devuelve un bit por llamada (AL) con ruido en los bits altos
inline u32 __cdecl FkClock(u32 on, u32 off) {
    Ev& e = PushEv(0x52CEE0, 2);
    e.v[0] = on & 0xFFu; e.v[1] = off & 0xFFu;
    const u32 bit = (gClockBits >> (gClockCall & 31u)) & 1u;
    ++gClockCall;
    return bit | (gClockNoise & ~0xFFu);
}
// CStreaming::RequestModel (0x4087E0; cdecl, indice y banderas): evento [direccion, indice, banderas]
inline void __cdecl FkRequest(u32 model, u32 flags) { Ev& e = PushEv(0x4087E0, 2); e.v[0] = model; e.v[1] = flags; }
// CEntity::CreateRwObject (virtual, vtable +0x1C; thiscall): evento [0x40C4F0, ECX (etiqueta)]; opcionalmente reescribe el modelo de la entidad
inline void __fastcall FkCreateRw(void* self, void*) {
    Ev& e = PushEv(0x40C4F0, 1);
    e.v[0] = Tag(self);
    if (gCreateMode) memcpy(static_cast<u8*>(self) + 0x22, &gCreateWord, 2);
}
// CBaseModelInfo::GetTimeInfo (virtual, vtable +0x14; thiscall): evento [0x40C4F1, ECX (etiqueta)]; devuelve el dword de [this + 0x20] (puntero a las horas o nulo)
inline u32 __fastcall FkGetTimeInfo(void* self, void*) {
    Ev& e = PushEv(0x40C4F1, 1);
    e.v[0] = Tag(self);
    u32 v{};
    memcpy(&v, static_cast<u8*>(self) + 0x20, 4);
    return v;
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

// Enlaces de una lista CLinkList<CEntity*> (enlaces de 0xC bytes: dato 0..3, anterior +4, siguiente +8): cabeza usada +0, cola usada +0xC, cabeza libre +0x18, cola libre +0x24
inline void LinkChain(u32 headAddr, const std::vector<u32>& mids, u32 tailAddr) {
    std::vector<u32> chain{headAddr};
    chain.insert(chain.end(), mids.begin(), mids.end());
    chain.push_back(tailAddr);
    for (size_t i = 0; i < chain.size(); ++i) {
        if (i > 0) memcpy(reinterpret_cast<void*>(chain[i] + 4), &chain[i - 1], 4);
        if (i + 1 < chain.size()) memcpy(reinterpret_cast<void*>(chain[i] + 8), &chain[i + 1], 4);
    }
}
inline u32 AreaByte(const C& c, u32 e) {   // byte de area (+0x2F) de la entidad e
    const u32 mode = c.a[28 + e];
    return mode == 0 ? (c.a[44] & 0xFFu) : (mode == 1 ? 13u : ((c.a[44] + 1u + e * 7u) & 0xFFu));
}
inline void Put32G(u32 addr, u32 v) { memcpy(reinterpret_cast<void*>(addr), &v, 4); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind kind = kKind[c.fn];
    const bool stl = kind >= kFill4;
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(stl ? 0u : a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gPool = nullptr; gNewCall = 0; gNewRet[0] = gNewRet[1] = 0; gClockBits = gClockNoise = gClockCall = 0; gCreateMode = gCreateWord = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kind) {
    case kDirDtor: {
        const u32 entries = a[8] == 0 ? 0u : (a[8] == 1 ? u32(arenaB.Obj(a[9])) : a[4]);
        arenaA.Put32(o + 0, entries); arenaA.Put32(o + 4, a[5]); arenaA.Put32(o + 8, a[6]); arenaA.Put8(o + 0xC, a[7]);
        call.args[0] = a[10]; call.argCount = 1; call.popBytes = 0;   // `ret 4`
        break;
    }
    case kInCd: {
        u32 idx;
        if (a[4] == 0) {
            const u32 target = u32(arenaB.Obj(a[7]));
            arenaB.Put32(a[7], a[6]);
            idx = ((((target - 0x8E4CCCu) >> 2) * 0xCCCCCCCDu) & 0x3FFFFFFFu) | (a[8] << 30);   // indice con indice*20 + 0x8E4CCC = el dword de la arena (modulo 2^32)
        } else {
            idx = u32(kCdSlots[a[5]]);
            Put32G(kInfoBase + idx * 20u + 0xC, a[6]);
        }
        call.args[0] = idx; call.argCount = 1; call.popBytes = 4;
        break;
    }
    case kForce:
        Put32G(kForceByte, a[5]); Put32G(kForceByte + 4, a[6]);
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kSetVeh:
        Put32G(kLoadVeh, a[5]);
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kNop:
        *reinterpret_cast<u8*>(kMarker) = 0x5A;   // marca que la prueba negativa (mutate.py) lee para que las funciones vacias tengan algo observable; el juego real nunca la pone
        Put32G(kMemUsedBytes, a[24]);   // contador de CStreaming (aleatorio); la pila de ids y los contadores de CMemoryMgr (los usa el monton del propio banco) solo se vigilan, sin escribirlos
        break;
    case kLevel: {
        const u32 table = c.fn == 7 ? kAmbTable : kFireTable;
        for (u32 i = 0; i < 24; ++i) Put32G(kTables + 4 * i, a[8 + i]);
        u32 level;
        if (a[4] == 0) level = a[5];
        else {
            const u32 target = u32(arenaB.Obj(a[6]));
            arenaB.Put32(a[6], a[32]);
            level = (((target - table) >> 2) & 0x3FFFFFFFu) | (a[7] << 30);
        }
        Put32G(kCurrLevel, level);
        break;
    }
    case kLinkIns: {
        const u32 nUsed = a[4], nFree = a[5];
        std::vector<u32> used, freed;
        for (u32 i = 0; i < nUsed + nFree; ++i) {
            const u32 slot = a[8 + i];
            (i < nUsed ? used : freed).push_back(u32(arenaB.Obj(a[6] + 0x20 * slot)));
        }
        LinkChain(thisA + 0, used, thisA + 0xC);
        LinkChain(thisA + 0x18, freed, thisA + 0x24);
        arenaC.Put32(a[27], a[24]);
        call.args[0] = u32(arenaC.Obj(a[27])); call.argCount = 1; call.popBytes = 0;   // `ret 4`
        break;
    }
    case kPedGetAt: {
        arenaA.Put32(o + 0, a[4] == 1 ? u32(arenaC.Obj(a[5])) : a[5]);
        arenaA.Put32(o + 4, u32(arenaD.Obj(a[6])));
        arenaA.Put32(o + 8, a[9]);
        arenaD.Put8(a[6] + a[7], a[8]);
        call.args[0] = a[7]; call.argCount = 1; call.popBytes = 0;   // `ret 4`
        break;
    }
    case kStartRender:
        Put32G(kRenderLink, a[4]); Put32G(kRenderLink + 4, a[5]);
        break;
    case kMission:
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kProcess: {
        Put32G(kScanCode, a[15]); Put32G(kCurrArea, a[44]);
        const u32 miVt = u32(arenaD.Obj(a[46])), entVt = u32(arenaD.Obj(a[45]));
        for (u32 j = 0; j < 8; ++j) {
            const u32 idx = u32(kModelIdx[j]);
            Put32G(kInfoBase + idx * 20u + 0x10, a[36 + j]);
            const u32 obj = a[47] + 0x40 * j;
            Put32G(kInfoTable + 4 * idx, u32(arenaD.Obj(obj)));
            arenaD.Put32(obj + 0, miVt);
            arenaD.Put32(obj + 0x1C, a[49 + j]);
            arenaD.Put32(obj + 0x20, a[57 + j] ? u32(arenaD.Obj(a[48] + 4 * j)) : 0u);
            if (a[57 + j]) arenaD.Put32(a[48] + 4 * j, a[65 + j]);
        }
        arenaD.Put32(a[45] + 0x1C, u32(reinterpret_cast<uintptr_t>(&FkCreateRw)));
        arenaD.Put32(a[46] + 0x14, u32(reinterpret_cast<uintptr_t>(&FkGetTimeInfo)));
        for (u32 e = 0; e < 4; ++e) {
            const u32 base = a[7] + 0x40 * e;
            arenaC.Put32(base + 0, entVt);
            arenaC.Put32(base + 0x18, a[32 + e]);
            arenaC.Put32(base + 0x1C, a[24 + e]);
            arenaC.Put16(base + 0x22, u32(kModelIdx[a[20 + e]]) & 0xFFFFu);
            arenaC.Put16(base + 0x2C, a[16 + e] & 0xFFFFu);
            arenaC.Put8(base + 0x2F, AreaByte(c, e));
        }
        for (u32 s = 0; s < a[4]; ++s) {
            arenaB.Put32(a[6] + 8 * s, u32(arenaC.Obj(a[7] + 0x40 * a[8 + s])));
            arenaB.Put32(a[6] + 8 * s + 4, s + 1 < a[4] ? u32(arenaB.Obj(a[6] + 8 * (s + 1))) : 0u);
        }
        // Pool falso de nodos CPtrNodeSingleLink (8 bytes) para las precondiciones (assert) del iterador de la DLL: almacenamiento = los nodos en B, 8 ranuras, todas en uso (estado 0)
        arenaD.Put32(kFakePool + 0, u32(arenaB.Obj(a[6]))); arenaD.Put32(kFakePool + 4, u32(arenaD.Obj(kFakeStates))); arenaD.Put32(kFakePool + 8, 8); arenaD.Put32(kFakePool + 0xC, 0xFFFFFFFFu); arenaD.Put8(kFakePool + 0x10, 1);
        for (u32 i = 0; i < 8; ++i) arenaD.Put8(kFakeStates + i, 0);
        Put32G(kPoolSingle, u32(arenaD.Obj(kFakePool)));
        arenaA.Put32(o, a[4] > 0 ? u32(arenaB.Obj(a[6])) : 0u);
        gClockBits = a[73]; gClockNoise = a[74]; gClockCall = 0; gCreateMode = a[75]; gCreateWord = a[76];
        call.args[0] = thisA; call.args[1] = a[14]; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kPoolCtor: {
        const i32 cap = i32(a[4]);
        gPool = reinterpret_cast<const u8*>(arenaA.Obj(o));
        gNewRet[0] = cap > 0 ? u32(arenaB.Obj(a[5])) : a[8];
        gNewRet[1] = cap > 0 ? u32(arenaC.Obj(a[6])) : a[9];
        call.args[0] = a[4]; call.args[1] = a[7]; call.argCount = 2; call.popBytes = 0;   // `ret 8`
        break;
    }
    default: {   // STL
        const u32 stride = ElemSize(kind), n = a[4];
        const u32 first = u32(arenaB.Obj(a[7])), last = first + n * stride;
        u32 dst = a[6] == 1 ? first + u32(i32(a[8])) * stride : u32(arenaC.Obj(a[9])) + a[10];
        if (a[14] == 1) dst = 0;
        const u32 out = thisA;
        switch (kind) {
        case kFill4: case kFill20: {
            const u32 value = (a[11] == 1 && n > 0) ? first + a[13] * stride : u32(arenaD.Obj(a[12]));
            call.args[0] = first; call.args[1] = last; call.args[2] = value; call.argCount = 3; call.popBytes = 12;
            break;
        }
        case kCopy20:
            call.args[0] = out; call.args[1] = first; call.args[2] = last; call.args[3] = dst; call.argCount = 4; call.popBytes = 16;
            break;
        case kCopyBack20: case kCopyBack12:
            call.args[0] = out; call.args[1] = first; call.args[2] = last; call.args[3] = dst + n * stride; call.argCount = 4; call.popBytes = 16;
            break;
        default:   // uninitialized_copy
            call.args[0] = first; call.args[1] = last; call.args[2] = dst; call.argCount = 3; call.popBytes = 12;
            break;
        }
        break;
    }
    }
    return call;
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
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kDirDtor: case kLinkIns: case kPedGetAt: case kPoolCtor: case kCopy20: case kCopyBack20: case kCopyBack12: case kUnin4: case kUnin20: case kUnin12:
            obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kInCd: obs["ret"] = eax & 0xFFu; break;   // bool: solo AL
        case kLevel: obs["ret"] = eax; break;          // entero (eModelID)
        default: obs["ret"] = nullptr; break;          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN189A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN189A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn189a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn189a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn189a"); }
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
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0 && ModuleOf(env.route[f]->Body()) == env.asi;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
        collaborators[kFn[f]] = {{"found", found}, {"body", found ? env.route[f]->Body() : 0}, {"body_module", found ? ModuleOf(env.route[f]->Body()) : 0}, {"asi", env.asi}, {"cat", cat}, {"name", name}};
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete 0x8214BD y operator new[] 0x821195, MemoryMgr.cpp): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {ByExeAddress(0x8214BD, &FkDelete)};
    {   // ~CDirectory (0x5322D0): colaborador REAL con el hook apagado (el EXE corre sus bytes y la DLL su ~CDirectory)
        auto it = collabCache.find(0x5322D0);
        if (it == collabCache.end()) it = collabCache.emplace(0x5322D0, MakeCollab(env, 0x5322D0, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        env.reals[0] = {it->second};
    }
    env.stubs[13] = {F(0x409C90, &FkMission)};
    env.stubs[14] = {F(0x4089A0, &FkRemove)};
    env.stubs[15] = {F(0x52CEE0, &FkClock), F(0x4087E0, &FkRequest)};
    env.stubs[16] = {ByExeAddress(0x821195, &FkNewArray)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[0] = "toda la funcion salvo operator delete (0x8214BD, falso solo en la direccion del EXE: la DLL lo llama por ella); ~CDirectory (0x5322D0) corre real (EXE con el hook apagado; la DLL su ~CDirectory)";
    dllReal[13] = "toda la funcion salvo CStreaming::SetMissionDoesntRequireModel (0x409C90, con hook: falso identico en el EXE y en el cuerpo de la DLL)";
    dllReal[14] = "toda la funcion salvo CStreaming::RemoveModel (0x4089A0, con hook: falso identico en el EXE y en el cuerpo de la DLL)";
    dllReal[15] = "toda la funcion salvo CClock::GetIsTimeInRange (0x52CEE0) y CStreaming::RequestModel (0x4087E0), con hook (falsos identicos), y las dos virtuales CreateRwObject (vtable +0x1C) y GetTimeInfo (vtable +0x14) de vtables falsas en la arena D";
    dllReal[16] = "toda la funcion salvo operator new[] (0x821195, falso solo en la direccion del EXE: la DLL lo llama por ella)";
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
    // Memoria global del juego tocada (se guarda al empezar y se restaura tras cada lado)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    auto R = [&](uintptr_t at, size_t n) { regions.emplace_back(at, n); return regions.size() - 1; };
    for (const i32 slot : kCdSlots) regionsOf[1].push_back(R(kInfoBase + u32(slot) * 20u + 0xC, 4));
    regionsOf[2] = {R(kForceByte, 8)};
    regionsOf[3] = {R(kLoadVeh, 4)};
    {
        const std::vector<size_t> nop{R(kMarker, 1), R(kIdStack, 0x48), R(kMemId, 4), R(kMemUsedMgr, 4), R(kMemUsedBytes, 4)};
        regionsOf[4] = regionsOf[5] = regionsOf[6] = regionsOf[9] = nop;
    }
    {
        const std::vector<size_t> level{R(kCurrLevel, 4), R(kTables, 0x60)};
        regionsOf[7] = regionsOf[8] = level;
    }
    regionsOf[12] = {R(kRenderLink, 8)};
    {
        std::vector<size_t> proc{R(kScanCode, 4), R(kCurrArea, 4), R(kPoolSingle, 4)};
        for (const i32 idx : kModelIdx) { proc.push_back(R(kInfoBase + u32(idx) * 20u + 0x10, 4)); proc.push_back(R(kInfoTable + 4u * u32(idx), 4)); }
        regionsOf[15] = proc;
    }
    Run(env, ready, registered, bodies, collaborators);
}
}
