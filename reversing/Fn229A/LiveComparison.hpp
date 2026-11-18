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

// Banco par a par Fn229A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedGlobalInstall). El banco invoca la direccion del EXE con objetos sinteticos (listas de materiales, arboles de sectores,
// mundos, tuberias, nodos de geometrias dinamicas) escritos en cuatro arenas propias (A, B, C y D con guardas de 0x40) con "pokes" sobre un relleno pseudoaleatorio, y con la memoria global del motor sembrada por caso
// (se guarda y se restaura). Colaboradores: TODOS los de RenderWare que llaman estas funciones estan falseados (el MISMO falso en la direccion del EXE y en el cuerpo de la DLL, porque todos tienen hook RH en la DLL,
// apagado durante el banco) salvo _rpMaterialListDeinitialize en _rpMaterialListStreamRead (el EXE lo lleva en linea: la DLL corre su cuerpo real) y las funciones de memoria de RwEngineInstance (+0x134 malloc, +0x138
// free, +0x13C realloc, +0x144 y +0x148 de las listas libres), que son punteros a funcion dentro del motor: se sustituyen por falsos en el propio motor (se guardan y restauran). Los falsos devuelven resultados de colas
// del caso (a[...]) y anotan cada llamada.
// Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena), los bytes cambiados de las regiones globales y el estado
// x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn229a-crash.txt (junto al EXE).

namespace F229ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn229a-crash.txt";
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


namespace F229ALive {
using namespace F229AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntryAddr[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F229AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            if (m[i] == r[i]) continue;
            out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        }
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }

// ================================================================================================ estado del caso para los falsos
inline std::array<u32, kSlots> gA{};          // a[] del caso (colas y parametros)
inline std::array<u32, kQueues> gQn{};        // contadores de consumo de cada cola
inline DWORD gBenchThread{};
inline u32 QNext(QueueId q) { return gA[kQ + 16 * q + ((gQn[q]++) & 15u)]; }
// Un token de puntero de las colas (0x10000000 + (arena << 20) + desplazamiento respecto a la memoria de la arena) -> direccion real
inline u32 Res(u32 tok) {
    if ((tok & 0xF0000000u) == 0x10000000u) {
        const u32 ar = (tok >> 20) & 0xFu, off = tok & 0xFFFFFu;
        if (ar < 4 && off < Arena<kWin>::N) return u32(uintptr_t(ArenaAt(ar).m.data()) + off);
    }
    return tok;
}
inline u32 gEngine{};

// Identificadores de eventos de las funciones de memoria del motor (no tienen direccion propia en el EXE)
inline constexpr u32 kEvAlloc = 0xE144, kEvFreeL = 0xE148, kEvMalloc = 0xE134, kEvRwFree = 0xE138, kEvRealloc = 0xE13C;

// ================================================================================================ falsos de los colaboradores (cdecl). El evento es [direccion, argumentos...]
// Funciones de memoria del motor
inline u32 __cdecl FkAlloc(u32 list, u32 hint) { Ev& e = PushEv(kEvAlloc, 2); e.v[0] = Tag(uintptr_t(list)); e.v[1] = hint; return Res(QNext(qAlloc)); }
inline void __cdecl FkFreeL(u32 list, void* p) { Ev& e = PushEv(kEvFreeL, 2); e.v[0] = Tag(uintptr_t(list)); e.v[1] = Tag(p); }
inline u32 __cdecl FkMalloc(u32 size, u32 hint) { Ev& e = PushEv(kEvMalloc, 2); e.v[0] = size; e.v[1] = hint; return Res(QNext(qMalloc)); }
inline u32 __cdecl FkRealloc(void* p, u32 size, u32 hint) { Ev& e = PushEv(kEvRealloc, 3); e.v[0] = Tag(p); e.v[1] = size; e.v[2] = hint; return Res(QNext(qRealloc)); }
inline void __cdecl FkRwFree(void* p) { Ev& e = PushEv(kEvRwFree, 1); e.v[0] = Tag(p); }
// Flujo
inline u32 __cdecl FkFindChunk(void* stream, u32 type, u32* plen, u32* pver) {
    Ev& e = PushEv(0x7ED2D0, 4); e.v[0] = Tag(stream); e.v[1] = type; e.v[2] = plen ? 1u : 0u; e.v[3] = pver ? 1u : 0u;
    const u32 ok = QNext(qFindOk), len = QNext(qFindLen), ver = QNext(qFindVer);
    if (plen) *plen = len;
    if (pver) *pver = ver;
    return ok;
}
// RwStreamReadInt32(stream, buf, bytes): escribe min(bytes / 4, 16) palabras de la cola de valores y devuelve el resultado de la cola
inline u32 __cdecl FkReadInt32(void* stream, u32* buf, u32 n) {
    Ev& e = PushEv(0x7ED540, 3); e.v[0] = Tag(stream); e.v[1] = n; e.v[2] = Tag(buf);
    const u32 words = n / 4 < 16u ? n / 4 : 16u;
    for (u32 k = 0; k < words; ++k) buf[k] = QNext(qRi32Val);
    return QNext(qRi32Ok);
}
// Errores
inline u32 __cdecl FkRwError(u32 code, u32 arg) { Ev& e = PushEv(0x8088D0, 2); e.v[0] = code; e.v[1] = code == 0x80000013u ? arg : 0u; return code + gA[kErrDelta]; }
inline u32 __cdecl FkRwErrorSet(const u32* err) { Ev& e = PushEv(0x808820, 3); e.v[0] = Tag(err); e.v[1] = err[0]; e.v[2] = err[1]; return u32(uintptr_t(err)); }
// Materiales y registros
inline u32 __cdecl FkTexSize(void* tex) { Ev& e = PushEv(0x8045A0, 1); e.v[0] = Tag(tex); return QNext(qTexSize); }
inline u32 __cdecl FkRegSize(u32 reg, void* obj) { Ev& e = PushEv(0x808B00, 2); e.v[0] = reg; e.v[1] = Tag(obj); return QNext(qRegSize); }
inline u32 __cdecl FkMatDestroy(void* mat) { Ev& e = PushEv(0x74DA20, 1); e.v[0] = Tag(mat); return 1; }
inline u32 __cdecl FkMatSize(void* mat) { Ev& e = PushEv(0x74E010, 1); e.v[0] = Tag(mat); return QNext(qMatSize); }
inline u32 __cdecl FkMatRead(void* stream) { Ev& e = PushEv(0x74DD30, 1); e.v[0] = Tag(stream); return Res(QNext(qMatRead)); }
// _rpMaterialListAppendMaterial: con hueco y vector, guarda, refCount++ y numMaterials++ (devuelve el indice; si no, -1)
inline u32 __cdecl FkAppend(u32* list, u8* mat) {
    Ev& e = PushEv(0x74E350, 2); e.v[0] = Tag(list); e.v[1] = Tag(mat);
    const i32 n = i32(list[1]), space = i32(list[2]);
    u32* vec = reinterpret_cast<u32*>(uintptr_t(list[0]));
    if (space > n && vec) {
        vec[n] = u32(uintptr_t(mat));
        ++*reinterpret_cast<u16*>(mat + 0x18);
        list[1] = u32(n + 1);
        return u32(n);
    }
    return 0xFFFFFFFFu;
}
inline u32 __cdecl FkMatDeinit(void* list) { Ev& e = PushEv(0x74E150, 1); e.v[0] = Tag(list); return u32(uintptr_t(list)); }
inline u32 __cdecl FkMatInit(u32* list) { Ev& e = PushEv(0x74E1B0, 1); e.v[0] = Tag(list); list[2] = 0; list[0] = 0; list[1] = 0; return QNext(qInit) ? u32(uintptr_t(list)) : 0u; }
// Listas libres y tuberias
inline u32 __cdecl FkFlCreate(u32 a1, u32 a2, u32 a3, u32 a4) { Ev& e = PushEv(0x801980, 4); e.v[0] = a1; e.v[1] = a2; e.v[2] = a3; e.v[3] = a4; return QNext(qFlCreate); }
inline u32 __cdecl FkFlDestroy(void* list) { Ev& e = PushEv(0x801B80, 1); e.v[0] = Tag(list); return 1; }
inline u32 __cdecl FkPipeCreate() { PushEv(0x8057B0, 0); return Res(QNext(qPipeCreate)); }
inline u32 __cdecl FkLock(void* pipe) { Ev& e = PushEv(0x806990, 1); e.v[0] = Tag(pipe); return Res(QNext(qLock)); }
inline u32 __cdecl FkNodeDefSector() { PushEv(0x75E9F0, 0); return Res(QNext(qNodeDef)); }
inline u32 __cdecl FkNodeDefAtomic() { PushEv(0x7582E0, 0); return Res(QNext(qNodeDef)); }
inline u32 __cdecl FkAddFrag(void* locked, u32 b, void* d, u32 e4) { Ev& e = PushEv(0x806BE0, 4); e.v[0] = Tag(locked); e.v[1] = b; e.v[2] = Tag(d); e.v[3] = e4; return Res(QNext(qAddFrag)); }
inline u32 __cdecl FkPipeUnlock(void* p) { Ev& e = PushEv(0x805F40, 1); e.v[0] = Tag(p); return QNext(qPipeUnlock); }
inline void __cdecl FkSetDefSector(void* p) { Ev& e = PushEv(0x75E270, 1); e.v[0] = Tag(p); }
inline void __cdecl FkSetDefAtomic(void* p) { Ev& e = PushEv(0x75E2A0, 1); e.v[0] = Tag(p); }
inline void __cdecl FkPipeDestroy(void* p) { Ev& e = PushEv(0x805820, 1); e.v[0] = Tag(p); }
inline u32 __cdecl FkLightsOpen() { PushEv(0x755D80, 0); return QNext(qBool0); }
inline void __cdecl FkShaderOpen() { PushEv(0x760CF0, 0); }
inline void __cdecl FkShaderPurge() { PushEv(0x75EE60, 0); }
inline void __cdecl FkLightsClose() { PushEv(0x755FE0, 0); }
inline u32 __cdecl FkLightAttach() { PushEv(0x755B90, 0); return QNext(qBool0); }
inline u32 __cdecl FkGeoAttach() { PushEv(0x7587D0, 0); return QNext(qBool1); }
inline u32 __cdecl FkGetCb() { PushEv(0x7FAE30, 0); return QNext(qCbVal); }
inline void __cdecl FkSetCb(u32 cb) { Ev& e = PushEv(0x7FAE20, 1); e.v[0] = cb; }
// Sectores, mallas, registros de plugins
inline void __cdecl FkFreeRes(void* entry) { Ev& e = PushEv(0x807DE0, 1); e.v[0] = Tag(entry); }
inline u32 __cdecl FkTie(void* link) { Ev& e = PushEv(0x74FFF0, 1); e.v[0] = Tag(link); return 1; }
inline u32 __cdecl FkLightTie(void* link) { Ev& e = PushEv(0x74FFA0, 1); e.v[0] = Tag(link); return 1; }
inline u32 __cdecl FkDeInitObject(u32 reg, void* obj) { Ev& e = PushEv(0x808740, 2); e.v[0] = reg; e.v[1] = Tag(obj); return u32(uintptr_t(obj)); }
inline u32 __cdecl FkInitObject(u32 reg, void* obj) { Ev& e = PushEv(0x8086E0, 2); e.v[0] = reg; e.v[1] = Tag(obj); return u32(uintptr_t(obj)); }
inline u32 __cdecl FkMeshDestroy(void* mesh) { Ev& e = PushEv(0x758BC0, 1); e.v[0] = Tag(mesh); return 1; }
inline u32 __cdecl FkExec(void* pipe, void* sector, u32 flag) { Ev& e = PushEv(0x805710, 3); e.v[0] = Tag(pipe); e.v[1] = Tag(sector); e.v[2] = flag; return QNext(qExec); }
inline u32 __cdecl FkMeshCreate(u32 n) { Ev& e = PushEv(0x758A90, 1); e.v[0] = n; return Res(QNext(qMesh)); }
inline u32 __cdecl FkAddTri(void* mesh, void* mat, u32 v0, u32 v1, u32 v2, u32 mi, u32 ti, u32 ri, u32 pi) {
    Ev& e = PushEv(0x758C00, 9); e.v[0] = Tag(mesh); e.v[1] = Tag(mat); e.v[2] = v0; e.v[3] = v1; e.v[4] = v2; e.v[5] = mi; e.v[6] = ti; e.v[7] = ri; e.v[8] = pi;
    return u32(uintptr_t(mesh));
}
inline u32 __cdecl FkOptimise(void* mesh, u32 flag) { Ev& e = PushEv(0x75D970, 2); e.v[0] = Tag(mesh); e.v[1] = flag; return Res(QNext(qOpt)); }
inline u32 __cdecl FkMeshBDestroy(void* mesh) { Ev& e = PushEv(0x758B80, 1); e.v[0] = Tag(mesh); return 1; }
// Funciones del propio lote usadas como colaboradores por otras
inline u32 __cdecl FkWorldUnlock(void* world) { Ev& e = PushEv(0x74F210, 1); e.v[0] = Tag(world); return QNext(qWUnlock); }
inline void __cdecl FkDeinstall(void* sector) { Ev& e = PushEv(0x74ECA0, 1); e.v[0] = Tag(sector); }
inline void __cdecl FkDestroyRec(void* sector) { Ev& e = PushEv(0x74ED50, 1); e.v[0] = Tag(sector); }
// Callbacks de los iteradores (los pasa el banco como argumento)
inline u32 __cdecl FkMatCb(void* mat, void* data) { Ev& e = PushEv(0xCB01, 2); e.v[0] = Tag(mat); e.v[1] = Tag(data); return QNext(qCb); }
inline u32 __cdecl FkSectCb(void* sector, void* data) { Ev& e = PushEv(0xCB02, 2); e.v[0] = Tag(sector); e.v[1] = Tag(data); return QNext(qCb); }

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
    std::array<u32, 12> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

// Memoria global del banco: se rellena la region entera de forma determinista y despues se escriben los campos del caso
inline void FillGlobal(u32 at, u32 bytes, u32 kind, u32 seed) {
    std::vector<u8> tmp(bytes);
    FillKind(tmp.data(), bytes, kind, seed);
    memcpy(reinterpret_cast<void*>(uintptr_t(at)), tmp.data(), bytes);
}
inline void PutG32(u32 at, u32 v) { memcpy(reinterpret_cast<void*>(uintptr_t(at)), &v, 4); }
inline u32 RegionSeed(const C& c, u32 region) { return c.a[90] + region * 17u; }
inline constexpr u32 kEngineFields = 0x130, kEngineFieldBytes = 0x20, kEngineWorldAt = 4;
inline constexpr std::array<unsigned, kFunctions> kArgCount{1, 1, 2, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 3, 3};
// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gA = c.a; gQn = {};
    for (u32 r = 0; r < 5; ++r) FillGlobal(kRegAt[r], kRegBytes[r], a[2], RegionSeed(c, r));
    *reinterpret_cast<u8*>(kBenchMarker) = u8(kMarkerValue);   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    FillGlobal(gEngine + kEngineWorldAt, 4, a[2], RegionSeed(c, 6));
    // campos de funciones de memoria del motor: falsos en el propio motor
    FillGlobal(gEngine + kEngineFields, kEngineFieldBytes, a[2], RegionSeed(c, 7));
    PutG32(gEngine + 0x134, u32(uintptr_t(&FkMalloc))); PutG32(gEngine + 0x138, u32(uintptr_t(&FkRwFree))); PutG32(gEngine + 0x13C, u32(uintptr_t(&FkRealloc)));
    PutG32(gEngine + 0x144, u32(uintptr_t(&FkAlloc))); PutG32(gEngine + 0x148, u32(uintptr_t(&FkFreeL)));
    for (const auto& p : c.pokes) {
        const u32 space = p[0], off = p[1], size = p[2], vk = p[3], v1 = p[4], v2 = p[5];
        u32 value = v1;
        if (vk != 0) value = u32(uintptr_t(ArenaAt(v1).m.data()) + kGuard + v2) - (vk == 2 ? gEngine : 0u);
        u8* dst = space < 4 ? ArenaAt(space).m.data() + kGuard + off : reinterpret_cast<u8*>(uintptr_t(space == 4 ? off : gEngine + off));
        memcpy(dst, &value, size);
    }
    call.argCount = kArgCount[c.fn]; call.popBytes = 4 * call.argCount;
    for (u32 i = 0; i < call.argCount; ++i) {
        const u32 k = a[kArgs + 3 * i], v1 = a[kArgs + 3 * i + 1], v2 = a[kArgs + 3 * i + 2];
        call.args[i] = k == 0 ? v1 : (k == 1 ? u32(uintptr_t(ArenaAt(v1).m.data()) + kGuard + v2) : u32(v1 == 0 ? uintptr_t(&FkMatCb) : uintptr_t(&FkSectCb)));
    }
    return call;
}

enum RetKind { rNone, rTag, rRaw };
inline RetKind RetOf(unsigned fn) {
    switch (fn) {
    case kMatStrSize: case kMatListAppend: case kMatListStrSize: case kPluginAttach: case kMatPipeOpen: case kSectPipeOpen: case kAtomPipeOpen: case kWorldDestroy: return rRaw;
    case kDynAdd: case kDynRemove: case kMatPipeClose: case kSectPipeClose: case kAtomPipeClose: case kDeinstAll: case kDestroyRec: return rNone;
    default: return rTag;
    }
}
inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    input["engine"] = gEngine;
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
        obs["vptrs"] = json::array();
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        switch (gThrew ? rNone : RetOf(fn)) {
        case rTag: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case rRaw: obs["ret"] = eax; break;                  // valor crudo
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN229A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN229A_CASES", buffer, sizeof(buffer)) > 0) {
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

// Busca el hook registrado para una direccion del EXE (categoria y nombre).
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

// Lista de colaboradores falseados y reales de cada funcion (direccion del EXE; el orden es el de run_local.py)
inline const std::map<u32, void*>& FakeTable() {
    static const std::map<u32, void*> t = {
        {0x8045A0, reinterpret_cast<void*>(&FkTexSize)}, {0x808B00, reinterpret_cast<void*>(&FkRegSize)}, {0x74DA20, reinterpret_cast<void*>(&FkMatDestroy)}, {0x74E010, reinterpret_cast<void*>(&FkMatSize)},
        {0x74DD30, reinterpret_cast<void*>(&FkMatRead)}, {0x74E350, reinterpret_cast<void*>(&FkAppend)}, {0x74E150, reinterpret_cast<void*>(&FkMatDeinit)}, {0x74E1B0, reinterpret_cast<void*>(&FkMatInit)},
        {0x801980, reinterpret_cast<void*>(&FkFlCreate)}, {0x801B80, reinterpret_cast<void*>(&FkFlDestroy)}, {0x8057B0, reinterpret_cast<void*>(&FkPipeCreate)}, {0x806990, reinterpret_cast<void*>(&FkLock)},
        {0x75E9F0, reinterpret_cast<void*>(&FkNodeDefSector)}, {0x7582E0, reinterpret_cast<void*>(&FkNodeDefAtomic)}, {0x806BE0, reinterpret_cast<void*>(&FkAddFrag)}, {0x805F40, reinterpret_cast<void*>(&FkPipeUnlock)},
        {0x75E270, reinterpret_cast<void*>(&FkSetDefSector)}, {0x75E2A0, reinterpret_cast<void*>(&FkSetDefAtomic)}, {0x805820, reinterpret_cast<void*>(&FkPipeDestroy)}, {0x755D80, reinterpret_cast<void*>(&FkLightsOpen)},
        {0x760CF0, reinterpret_cast<void*>(&FkShaderOpen)}, {0x75EE60, reinterpret_cast<void*>(&FkShaderPurge)}, {0x755FE0, reinterpret_cast<void*>(&FkLightsClose)}, {0x755B90, reinterpret_cast<void*>(&FkLightAttach)},
        {0x7587D0, reinterpret_cast<void*>(&FkGeoAttach)}, {0x7FAE30, reinterpret_cast<void*>(&FkGetCb)}, {0x7FAE20, reinterpret_cast<void*>(&FkSetCb)}, {0x807DE0, reinterpret_cast<void*>(&FkFreeRes)},
        {0x74FFF0, reinterpret_cast<void*>(&FkTie)}, {0x74FFA0, reinterpret_cast<void*>(&FkLightTie)}, {0x808740, reinterpret_cast<void*>(&FkDeInitObject)}, {0x8086E0, reinterpret_cast<void*>(&FkInitObject)},
        {0x758BC0, reinterpret_cast<void*>(&FkMeshDestroy)}, {0x805710, reinterpret_cast<void*>(&FkExec)}, {0x758A90, reinterpret_cast<void*>(&FkMeshCreate)}, {0x758C00, reinterpret_cast<void*>(&FkAddTri)},
        {0x75D970, reinterpret_cast<void*>(&FkOptimise)}, {0x758B80, reinterpret_cast<void*>(&FkMeshBDestroy)}, {0x74F210, reinterpret_cast<void*>(&FkWorldUnlock)}, {0x74ECA0, reinterpret_cast<void*>(&FkDeinstall)},
        {0x74ED50, reinterpret_cast<void*>(&FkDestroyRec)}, {0x7ED2D0, reinterpret_cast<void*>(&FkFindChunk)}, {0x7ED540, reinterpret_cast<void*>(&FkReadInt32)}, {0x8088D0, reinterpret_cast<void*>(&FkRwError)},
        {0x808820, reinterpret_cast<void*>(&FkRwErrorSet)}};
    return t;
}
inline const std::array<std::vector<u32>, kFunctions>& StubLists() {
    static const std::array<std::vector<u32>, kFunctions> t = [] {
        std::array<std::vector<u32>, kFunctions> s;
        s[kMatStrSize] = {0x8045A0, 0x808B00};
        s[kMatListDeinit] = {0x74DA20};
        s[kMatListAppend] = {0x8088D0, 0x808820};
        s[kMatListStrSize] = {0x74E010};
        s[kMatListStrRead] = {0x7ED2D0, 0x7ED540, 0x8088D0, 0x808820, 0x74DD30, 0x74E350, 0x74DA20};
        s[kDynAdd] = {0x801980};
        s[kPluginAttach] = {0x755B90, 0x7587D0, 0x7FAE30, 0x7FAE20};
        s[kSectPipeOpen] = {0x8057B0, 0x806990, 0x75E9F0, 0x806BE0, 0x805F40, 0x75E270, 0x805820};
        s[kSectPipeClose] = {0x75E270, 0x805820};
        s[kAtomPipeOpen] = {0x8057B0, 0x806990, 0x7582E0, 0x806BE0, 0x805F40, 0x75E2A0, 0x755D80, 0x760CF0, 0x805820};
        s[kAtomPipeClose] = {0x801B80, 0x75EE60, 0x755FE0, 0x75E2A0, 0x805820};
        s[kDeinstAll] = {0x807DE0, 0x74FFF0, 0x74FFA0, 0x808740, 0x758BC0};
        s[kDestroyRec] = {0x807DE0, 0x74FFF0, 0x74FFA0, 0x808740, 0x758BC0};
        s[kDefRender] = {0x805710};
        s[kWorldUnlock] = {0x758A90, 0x758C00, 0x75D970, 0x758B80};
        s[kWorldDestroy] = {0x758BC0, 0x74E150, 0x74ECA0, 0x74ED50, 0x808740};
        s[kWorldCreate] = {0x74E1B0, 0x8088D0, 0x808820, 0x8086E0, 0x74F210, 0x758BC0, 0x74E150, 0x74ED50, 0x808740};
        return s;
    }();
    return t;
}
// Colaboradores REALES (hook apagado): (direccion, la DLL llama a su cuerpo)
inline const std::array<std::vector<std::pair<u32, bool>>, kFunctions>& RealLists() {
    static const std::array<std::vector<std::pair<u32, bool>>, kFunctions> t = [] {
        std::array<std::vector<std::pair<u32, bool>>, kFunctions> s;
        s[kMatListStrRead] = {{0x74E150, true}};     // el EXE lleva el cuerpo en linea; la DLL llama a _rpMaterialListDeinitialize (cuerpo real, con RpMaterialDestroy y RwFree falsos)
        return s;
    }();
    return t;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn229a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn229a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn229a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    gEngine = *reinterpret_cast<u32*>(uintptr_t(kEngineAt));
    bool ready = gEngine != 0; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
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
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto Collab = [&](uintptr_t exe) -> Pair {
        auto it = collabCache.find(exe);
        if (it != collabCache.end()) return it->second;
        Pair p{exe, 0, false};
        for (unsigned f = 0; f < kFunctions; ++f)
            if (kEntries[f] == exe && env.route[f]) { p.dll = Body(env.route[f]->Body()); p.hooked = true; hooksFound.push_back({{"exe", exe}, {"tested_function", kFn[f]}}); }
        if (!p.hooked) p = MakeCollab(env, exe, hooksFound);
        return collabCache.emplace(exe, p).first->second;
    };
    for (unsigned f = 0; f < kFunctions; ++f) {
        for (const u32 exe : StubLists()[f]) {
            const Pair p = Collab(exe);
            if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
            env.stubs[f].push_back(StubSpec{p, FakeTable().at(exe)});
        }
        for (const auto& [exe, viaDll] : RealLists()[f]) {
            const Pair p = Collab(exe);
            if (viaDll) { if (!p.hooked || !p.dll) ready = false; env.reals[f].push_back(p); }
            else env.reals[f].push_back(Pair{exe, 0, false});
        }
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    dllReal[kMatListStrRead] = "toda la funcion; _rpMaterialListDeinitialize 0x74E150 corre real en la DLL (cuerpo C++, con RpMaterialDestroy y RwFree falsos) y en linea en el EXE; los demas colaboradores son falsos identicos";
    json iatNone = nullptr;
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", iatNone}};
    }
    // Memoria global del juego tocada (regiones de Fixtures.hpp, el mundo actual del motor y los campos de funciones de memoria del motor): se guardan al empezar y se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (size_t r = 0; r < kRegAt.size(); ++r) regions.emplace_back(kRegAt[r], kRegBytes[r]);
    regions.emplace_back(gEngine + kEngineWorldAt, 4);
    regions.emplace_back(gEngine + kEngineFields, kEngineFieldBytes);
    for (unsigned f = 0; f < kFunctions; ++f) for (size_t r = 0; r < regions.size(); ++r) regionsOf[f].push_back(r);
    Run(env, ready, registered, bodies, collaborators);
}
}
