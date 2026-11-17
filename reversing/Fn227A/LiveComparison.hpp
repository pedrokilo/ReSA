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

// Banco par a par Fn227A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 15 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedGlobalInstall). El banco invoca la direccion del EXE con objetos sinteticos (clumps, atomicos, luces, camaras,
// geometrias, marcos, listas y flujos) escritos en cuatro arenas propias (A, B, C y D con guardas de 0x40) con "pokes" sobre un relleno pseudoaleatorio, y con la memoria global del motor sembrada por caso
// (se guarda y se restaura). Colaboradores: TODOS los de RenderWare que llaman estas funciones estan falseados (el MISMO falso en la direccion del EXE y en el cuerpo de la DLL, porque todos tienen hook RH en la DLL,
// apagado durante el banco) salvo los que corren reales (hook apagado: el EXE corre sus bytes y la DLL su cuerpo) y las funciones de memoria de RwEngineInstance (+0x134 malloc, +0x138 free, +0x144 y +0x148
// de las listas libres), que son punteros a funcion dentro del motor: se sustituyen por falsos en el propio motor (se guardan y restauran). Los falsos devuelven resultados de colas del caso (a[...]) y anotan cada llamada.
// Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena), los bytes cambiados de las regiones globales y el estado
// x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn227a-crash.txt (junto al EXE).

namespace F227ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn227a-crash.txt";
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


namespace F227ALive {
using namespace F227AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntryAddr[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F227AFix::Arena(WIN);
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
inline std::array<u32, kQueues> gQn{};
inline DWORD gBenchThread{};        // contadores de consumo de cada cola
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
inline std::array<u32, 2> gCbDll{};           // cuerpos de la DLL de ClumpAtomicDestroyCallBack (0) y ClumpLightDestroyCallBack (1), para normalizar los callbacks que reciben RpClumpForAll*

// Identificadores de eventos de las funciones de memoria del motor (no tienen direccion propia en el EXE)
inline constexpr u32 kEvAlloc = 0xE144, kEvFreeL = 0xE148, kEvMalloc = 0xE134, kEvRwFree = 0xE138;

// ================================================================================================ falsos de los colaboradores (cdecl). El evento es [direccion, argumentos...]
inline u32 CbNorm(u32 cb) { const u32 b = u32(Body(cb)); return b == gCbDll[0] ? 0x74BCC0u : (b == gCbDll[1] ? 0x74A1C0u : cb); }
// Funciones de memoria del motor
inline u32 __cdecl FkAlloc(u32 list, u32 hint) { Ev& e = PushEv(kEvAlloc, 2); e.v[0] = Tag(uintptr_t(list)); e.v[1] = hint; return Res(QNext(qAlloc)); }
inline void __cdecl FkFreeL(u32 list, void* p) { Ev& e = PushEv(kEvFreeL, 2); e.v[0] = Tag(uintptr_t(list)); e.v[1] = Tag(p); }
inline u32 __cdecl FkMalloc(u32 size, u32 hint) { Ev& e = PushEv(kEvMalloc, 2); e.v[0] = size; e.v[1] = hint; return Res(QNext(qMalloc)); }
inline void __cdecl FkRwFree(void* p) { Ev& e = PushEv(kEvRwFree, 1); e.v[0] = Tag(p); }
// Flujo
inline u32 __cdecl FkFindChunk(void* stream, u32 type, u32* plen, u32* pver) {
    Ev& e = PushEv(0x7ED2D0, 4); e.v[0] = Tag(stream); e.v[1] = type; e.v[2] = plen ? 1u : 0u; e.v[3] = pver ? 1u : 0u;
    const u32 ok = QNext(qFindOk), len = QNext(qFindLen), ver = QNext(qFindVer);
    if (plen) *plen = len;
    if (pver) *pver = ver;
    return ok;
}
inline u32 __cdecl FkStreamRead(void* stream, void* buf, u32 n) {
    Ev& e = PushEv(0x7EC9D0, 2); e.v[0] = Tag(stream); e.v[1] = n;
    const u32 count = QNext(qReadCnt);
    const u32 nn = n < 16 ? n : 16;
    for (u32 k = 0; k * 4 < nn; ++k) {
        const u32 w = QNext(qReadData);
        std::memcpy(static_cast<u8*>(buf) + 4 * k, &w, nn - 4 * k < 4 ? nn - 4 * k : 4);
    }
    return count;
}
inline u32 __cdecl FkReadInt32(void* stream, u32* buf, u32 n) {
    Ev& e = PushEv(0x7ED540, 2); e.v[0] = Tag(stream); e.v[1] = n;
    const u32 val = QNext(qRi32Val), ok = QNext(qRi32Ok);
    *buf = val;
    return ok;
}
// Errores
inline u32 __cdecl FkRwError(u32 code, u32 arg) { Ev& e = PushEv(0x8088D0, 2); e.v[0] = code; e.v[1] = code == 0x80000013u ? arg : 0u; return code + gA[kErrDelta]; }
inline u32 __cdecl FkRwErrorSet(const u32* err) { Ev& e = PushEv(0x808820, 3); e.v[0] = Tag(err); e.v[1] = err[0]; e.v[2] = err[1]; return u32(uintptr_t(err)); }
// Registros de plugins
inline u32 __cdecl FkInitObject(u32 reg, void* obj) { Ev& e = PushEv(0x8086E0, 2); e.v[0] = reg; e.v[1] = Tag(obj); return u32(uintptr_t(obj)); }
inline u32 __cdecl FkDeInitObject(u32 reg, void* obj) { Ev& e = PushEv(0x808740, 2); e.v[0] = reg; e.v[1] = Tag(obj); return u32(uintptr_t(obj)); }
inline u32 __cdecl FkCopyObject(u32 reg, void* dst, void* src) { Ev& e = PushEv(0x808770, 3); e.v[0] = reg; e.v[1] = Tag(dst); e.v[2] = Tag(src); return u32(uintptr_t(dst)); }
inline u32 __cdecl FkReadDataChunks(u32 reg, void* stream, void* obj) {
    Ev& e = PushEv(0x808980, 3); e.v[0] = reg; e.v[1] = Tag(stream); e.v[2] = Tag(obj);
    *reinterpret_cast<u32*>(uintptr_t(kRightsIdAt)) = gA[kRightsId];            // deja los derechos que la lectura de plugins registraria
    *reinterpret_cast<u32*>(uintptr_t(kRightsExtraAt)) = gA[kRightsExtra];
    return gA[kRdChunksRet];
}
inline u32 __cdecl FkInvokeRights(u32 reg, u32 id, void* obj, u32 extra) { Ev& e = PushEv(0x808AB0, 4); e.v[0] = reg; e.v[1] = id; e.v[2] = Tag(obj); e.v[3] = extra; return 1; }
inline u32 __cdecl FkAddPlugin(u32 reg, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    Ev& e = PushEv(0x8084A0, 6); e.v[0] = reg; e.v[1] = Tag(uintptr_t(a1)); e.v[2] = Tag(uintptr_t(a2)); e.v[3] = Tag(uintptr_t(a3)); e.v[4] = Tag(uintptr_t(a4)); e.v[5] = Tag(uintptr_t(a5));
    return gA[kRegRet];
}
inline u32 __cdecl FkAddPluginStream(u32 reg, u32 a1, u32 a2, u32 a3, u32 a4) {
    Ev& e = PushEv(0x8088E0, 5); e.v[0] = reg; e.v[1] = Tag(uintptr_t(a1)); e.v[2] = Tag(uintptr_t(a2)); e.v[3] = Tag(uintptr_t(a3)); e.v[4] = Tag(uintptr_t(a4));
    return gA[kRegRet];
}
// Atomicos
inline u32 __cdecl FkAtomicDestroy(void* a) { Ev& e = PushEv(0x749DC0, 1); e.v[0] = Tag(a); return 1; }
inline u32 __cdecl FkAtomicClone(void* a) { Ev& e = PushEv(0x749E60, 1); e.v[0] = Tag(a); return Res(QNext(qAtom)); }
inline u32 __cdecl FkAtomicCreate() { PushEv(0x749C50, 0); return Res(QNext(qAtom)); }
inline u32 __cdecl FkAtomicSetGeometry(void* a, void* g, u32 flags) { Ev& e = PushEv(0x749D40, 3); e.v[0] = Tag(a); e.v[1] = Tag(g); e.v[2] = flags; return u32(uintptr_t(a)); }
inline u32 __cdecl FkForAllAtomics(void* clump, u32 cb, void* data) { Ev& e = PushEv(0x749B70, 3); e.v[0] = Tag(clump); e.v[1] = CbNorm(cb); e.v[2] = Tag(data); return u32(uintptr_t(clump)); }
inline u32 __cdecl FkForAllLights(void* clump, u32 cb, void* data) { Ev& e = PushEv(0x749C00, 3); e.v[0] = Tag(clump); e.v[1] = CbNorm(cb); e.v[2] = Tag(data); return u32(uintptr_t(clump)); }
inline u32 __cdecl FkForAllCameras(void* clump, u32 cb, void* data) { Ev& e = PushEv(0x749BB0, 3); e.v[0] = Tag(clump); e.v[1] = CbNorm(cb); e.v[2] = Tag(data); return u32(uintptr_t(clump)); }
// Marcos
inline u32 __cdecl FkCloneAndLink(void* frame) { Ev& e = PushEv(0x7EFFB0, 1); e.v[0] = Tag(frame); return Res(gA[kCloneFrame]); }
inline void __cdecl FkPurgeClone(void* frame) { Ev& e = PushEv(0x7F01A0, 1); e.v[0] = Tag(frame); }
inline void __cdecl FkDestroyHierarchy(void* frame) { Ev& e = PushEv(0x7F08A0, 1); e.v[0] = Tag(frame); }
inline void __cdecl FkUpdateObjects(void* frame) { Ev& e = PushEv(0x7F0910, 1); e.v[0] = Tag(frame); }
inline u32 __cdecl FkSetFrame(void* obj, void* frame) {
    Ev& e = PushEv(0x804EF0, 2); e.v[0] = Tag(obj); e.v[1] = Tag(frame);
    *reinterpret_cast<u32*>(static_cast<u8*>(obj) + 4) = u32(uintptr_t(frame));      // como el real: deja el marco en el campo parent del objeto
    return u32(uintptr_t(obj));
}
inline void __cdecl FkReleaseFrame(void* obj) { Ev& e = PushEv(0x804F40, 1); e.v[0] = Tag(obj); }
inline void __cdecl FkFreeResEntry(void* entry) { Ev& e = PushEv(0x807DE0, 1); e.v[0] = Tag(entry); }
inline u32 __cdecl FkFrameListRead(void* stream, u32* list) {
    Ev& e = PushEv(0x807970, 2); e.v[0] = Tag(stream); e.v[1] = Tag(list);
    const u32 ok = QNext(qFrameOk);
    if (ok) { list[0] = Res(gA[kFrameListPtr]); list[1] = gA[kFrameListNum]; }
    return ok;
}
inline u32 __cdecl FkFrameListDeinit(const u32* list) { Ev& e = PushEv(0x807720, 3); e.v[0] = Tag(list); e.v[1] = Tag(uintptr_t(list[0])); e.v[2] = list[1]; return u32(uintptr_t(list)); }
// Geometrias, luces, camaras y mundo
inline u32 __cdecl FkGeometryDestroy(void* g) { Ev& e = PushEv(0x74CCC0, 1); e.v[0] = Tag(g); return 1; }
inline u32 __cdecl FkGeometryStreamRead(void* stream) { Ev& e = PushEv(0x74D190, 1); e.v[0] = Tag(stream); return Res(QNext(qGeo)); }
inline u32 __cdecl FkGetWorld(void* atomic) { Ev& e = PushEv(0x751050, 1); e.v[0] = Tag(atomic); return QNext(qWorld); }
inline u32 __cdecl FkLightDestroy(void* l) { Ev& e = PushEv(0x7520D0, 1); e.v[0] = Tag(l); return 1; }
inline u32 __cdecl FkCameraDestroy(void* c) { Ev& e = PushEv(0x7EE4B0, 1); e.v[0] = Tag(c); return 1; }
inline u32 __cdecl FkLightStreamRead(void* stream) { Ev& e = PushEv(0x751F00, 1); e.v[0] = Tag(stream); return Res(QNext(qLight)); }
inline u32 __cdecl FkCameraStreamRead(void* stream) { Ev& e = PushEv(0x808DE0, 1); e.v[0] = Tag(stream); return Res(QNext(qCam)); }
// Funciones del propio lote usadas como colaboradores por otras
inline u32 __cdecl FkClumpCreate() { PushEv(0x74A290, 0); return Res(QNext(qClump)); }
inline u32 __cdecl FkClumpDestroy(void* clump) { Ev& e = PushEv(0x74A310, 1); e.v[0] = Tag(clump); return 1; }
inline u32 __cdecl FkAtomicStreamRead(void* stream, const u32* fl, const u32* gl) {
    Ev& e = PushEv(0x74B060, 7); e.v[0] = Tag(stream); e.v[1] = Tag(fl); e.v[2] = Tag(gl); e.v[3] = Tag(uintptr_t(fl[0])); e.v[4] = fl[1]; e.v[5] = Tag(uintptr_t(gl[0])); e.v[6] = gl[1];
    return Res(QNext(qAtom));
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
inline constexpr u32 kEngineFields = 0x130, kEngineFieldBytes = 0x20;
inline constexpr std::array<unsigned, kFunctions> kArgCount{1, 2, 0, 1, 2, 2, 2, 1, 3, 1, 2, 5, 5, 4, 4};
// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gA = c.a; gQn = {};
    for (u32 r = 0; r < 3; ++r) FillGlobal(kRegAt[r], kRegBytes[r], a[2], RegionSeed(c, r));
    *reinterpret_cast<u8*>(kBenchMarker) = u8(kMarkerValue);   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    // campos de funciones de memoria del motor: falsos en el propio motor
    FillGlobal(gEngine + kEngineFields, kEngineFieldBytes, a[2], RegionSeed(c, 7));
    PutG32(gEngine + 0x134, u32(uintptr_t(&FkMalloc))); PutG32(gEngine + 0x138, u32(uintptr_t(&FkRwFree)));
    PutG32(gEngine + 0x144, u32(uintptr_t(&FkAlloc))); PutG32(gEngine + 0x148, u32(uintptr_t(&FkFreeL)));
    for (const auto& p : c.pokes) {
        const u32 space = p[0], off = p[1], size = p[2], vk = p[3], v1 = p[4], v2 = p[5];
        u32 value = v1;
        if (vk != 0) value = u32(uintptr_t(ArenaAt(v1).m.data()) + kGuard + v2) - (vk == 2 ? gEngine : 0u);
        u8* dst = space < 4 ? ArenaAt(space).m.data() + kGuard + off : reinterpret_cast<u8*>(uintptr_t(off));
        memcpy(dst, &value, size);
    }
    call.argCount = kArgCount[c.fn]; call.popBytes = 4 * call.argCount;
    for (u32 i = 0; i < call.argCount; ++i) {
        const u32 k = a[kArgs + 3 * i], v1 = a[kArgs + 3 * i + 1], v2 = a[kArgs + 3 * i + 2];
        call.args[i] = k == 0 ? v1 : u32(uintptr_t(ArenaAt(v1).m.data()) + kGuard + v2);
    }
    return call;
}

enum RetKind { rNone, rTag, rRaw };
inline RetKind RetOf(unsigned fn) { return (fn == kDestroy || fn >= kRegAtomic) ? rRaw : rTag; }
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
// Casos por funcion: kCases salvo que la variable de entorno FN227A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN227A_CASES", buffer, sizeof(buffer)) > 0) {
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
struct StubDef { u32 exe; void* fake; };
inline const std::map<u32, void*>& FakeTable() {
    static const std::map<u32, void*> t = {
        {0x7ED2D0, reinterpret_cast<void*>(&FkFindChunk)}, {0x7EC9D0, reinterpret_cast<void*>(&FkStreamRead)}, {0x7ED540, reinterpret_cast<void*>(&FkReadInt32)},
        {0x8088D0, reinterpret_cast<void*>(&FkRwError)}, {0x808820, reinterpret_cast<void*>(&FkRwErrorSet)}, {0x8086E0, reinterpret_cast<void*>(&FkInitObject)},
        {0x808740, reinterpret_cast<void*>(&FkDeInitObject)}, {0x808770, reinterpret_cast<void*>(&FkCopyObject)}, {0x808980, reinterpret_cast<void*>(&FkReadDataChunks)},
        {0x808AB0, reinterpret_cast<void*>(&FkInvokeRights)}, {0x8084A0, reinterpret_cast<void*>(&FkAddPlugin)}, {0x8088E0, reinterpret_cast<void*>(&FkAddPluginStream)},
        {0x749DC0, reinterpret_cast<void*>(&FkAtomicDestroy)}, {0x749E60, reinterpret_cast<void*>(&FkAtomicClone)}, {0x749C50, reinterpret_cast<void*>(&FkAtomicCreate)},
        {0x749D40, reinterpret_cast<void*>(&FkAtomicSetGeometry)}, {0x749B70, reinterpret_cast<void*>(&FkForAllAtomics)}, {0x749C00, reinterpret_cast<void*>(&FkForAllLights)},
        {0x749BB0, reinterpret_cast<void*>(&FkForAllCameras)}, {0x7EFFB0, reinterpret_cast<void*>(&FkCloneAndLink)}, {0x7F01A0, reinterpret_cast<void*>(&FkPurgeClone)},
        {0x7F08A0, reinterpret_cast<void*>(&FkDestroyHierarchy)}, {0x7F0910, reinterpret_cast<void*>(&FkUpdateObjects)}, {0x804EF0, reinterpret_cast<void*>(&FkSetFrame)},
        {0x804F40, reinterpret_cast<void*>(&FkReleaseFrame)}, {0x807DE0, reinterpret_cast<void*>(&FkFreeResEntry)}, {0x74CCC0, reinterpret_cast<void*>(&FkGeometryDestroy)},
        {0x74D190, reinterpret_cast<void*>(&FkGeometryStreamRead)}, {0x751050, reinterpret_cast<void*>(&FkGetWorld)}, {0x7520D0, reinterpret_cast<void*>(&FkLightDestroy)},
        {0x7EE4B0, reinterpret_cast<void*>(&FkCameraDestroy)}, {0x807970, reinterpret_cast<void*>(&FkFrameListRead)}, {0x807720, reinterpret_cast<void*>(&FkFrameListDeinit)},
        {0x751F00, reinterpret_cast<void*>(&FkLightStreamRead)}, {0x808DE0, reinterpret_cast<void*>(&FkCameraStreamRead)}, {0x74A290, reinterpret_cast<void*>(&FkClumpCreate)},
        {0x74A310, reinterpret_cast<void*>(&FkClumpDestroy)}, {0x74B060, reinterpret_cast<void*>(&FkAtomicStreamRead)}};
    return t;
}
inline const std::array<std::vector<u32>, kFunctions>& StubLists() {
    static const std::array<std::vector<u32>, kFunctions> t = [] {
        std::array<std::vector<u32>, kFunctions> s;
        s[kClone] = {0x74A290, 0x7EFFB0, 0x749E60, 0x804EF0, 0x7F01A0, 0x749DC0, 0x808740, 0x808770, 0x807DE0, 0x749D40, 0x804F40, 0x8088D0, 0x808820, 0x749BB0, 0x7F08A0, 0x7520D0};
        s[kLightCb] = {0x7520D0};
        s[kCreate] = {0x8086E0};
        s[kDestroy] = {0x808740, 0x807DE0, 0x74CCC0, 0x751050, 0x7F0910, 0x804F40, 0x7520D0, 0x7EE4B0, 0x7F08A0};
        s[kGeoDeinit] = {0x74CCC0};
        s[kAtomicRead] = {0x7ED2D0, 0x7EC9D0, 0x749C50, 0x804EF0, 0x74CCC0, 0x751050, 0x7F0910, 0x74D190, 0x808740, 0x807DE0, 0x804F40, 0x808980, 0x808AB0, 0x8088D0, 0x808820};
        s[kStreamRead] = {0x7ED2D0, 0x7EC9D0, 0x7ED540, 0x74A290, 0x807970, 0x807720, 0x74B060, 0x74D190, 0x751F00, 0x808DE0, 0x804EF0, 0x808740, 0x808980, 0x749B70, 0x749C00, 0x749BB0, 0x7F08A0, 0x749DC0, 0x74A310, 0x74CCC0, 0x8088D0, 0x808820};
        s[kAtomicCb] = {0x808740, 0x807DE0, 0x74CCC0, 0x751050, 0x7F0910, 0x804F40};
        s[kRegAtomic] = {0x8084A0}; s[kRegClump] = {0x8084A0}; s[kRegAtomicStream] = {0x8088E0}; s[kRegClumpStream] = {0x8088E0};
        return s;
    }();
    return t;
}
// Colaboradores REALES (hook apagado): (direccion, 1 = la DLL llama a su cuerpo; 0 = solo EXE: la DLL lo hace en linea)
inline const std::array<std::vector<std::pair<u32, bool>>, kFunctions>& RealLists() {
    static const std::array<std::vector<std::pair<u32, bool>>, kFunctions> t = [] {
        std::array<std::vector<std::pair<u32, bool>>, kFunctions> s;
        s[kClone] = {{0x74A490, true}, {0x74A520, true}, {0x74A1C0, true}};
        s[kAtomicRead] = {{0x74CCB0, false}};
        s[kStreamRead] = {{0x74A490, true}, {0x74AE50, true}};
        return s;
    }();
    return t;
}

// Casilla de la IAT de un modulo para una funcion importada por nombre (no usada en este lote; se conserva por si algun colaborador futuro lo necesita)
inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn227a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn227a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn227a"); }
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
    gCbDll[0] = Body(env.route[kAtomicCb]->Body());
    gCbDll[1] = Body(env.route[kLightCb]->Body());
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
    dllReal[kClone] = "toda la funcion; RpClumpAddAtomic 0x74A490, RpClumpRemoveLight 0x74A520 y ClumpLightDestroyCallBack 0x74A1C0 corren reales (el EXE sus bytes, la DLL su cuerpo); los demas colaboradores son falsos identicos";
    dllReal[kAtomicRead] = "toda la funcion; RpGeometryAddRef 0x74CCB0 corre real en el EXE y en linea (++refCount) en la DLL; los demas colaboradores son falsos identicos";
    dllReal[kStreamRead] = "toda la funcion; RpClumpAddAtomic 0x74A490 y _rpGeometryListDeinitialize 0x74AE50 corren reales (el EXE sus bytes, la DLL su cuerpo); los demas colaboradores son falsos identicos";
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
    // Memoria global del juego tocada (regiones de Fixtures.hpp y los campos de funciones de memoria del motor): se guardan al empezar y se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (size_t r = 0; r < kRegAt.size(); ++r) regions.emplace_back(kRegAt[r], kRegBytes[r]);
    regions.emplace_back(gEngine + kEngineFields, kEngineFieldBytes);
    for (unsigned f = 0; f < kFunctions; ++f) for (size_t r = 0; r < regions.size(); ++r) regionsOf[f].push_back(r);
    Run(env, ready, registered, bodies, collaborators);
}
}
