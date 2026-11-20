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

// Banco par a par FnS7A1A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp (copia del de Fn226A).
// 21 funciones del EXE (ver Fixtures.hpp: lotes S7A y S1A del plan de sustitucion), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedGlobalInstall / Named). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Colaboradores: todos tienen hook RH en la DLL (apagado durante el banco) y llevan el MISMO falso en la direccion del EXE y en el cuerpo de
// la DLL, salvo el puente QuickTime 0x81A3C0 (sin hook: solo se parchea la direccion del EXE, que es la que llama la DLL).
// Se observan: los bytes cambiados de las cuatro arenas, las regiones globales vigiladas (gchanges y, la grande, su CRC), los eventos de los falsos (con sus argumentos; un puntero a la pila se anota como 0x7F000000),
// el valor devuelto y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace FS7Live {
using namespace FS7Fix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = FS7Fix::Arena(WIN);
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
inline u32 gReadRet{}, gReadWrite{}, gReadVal{}, gWriteRet{};
inline u32 gQtEax{}, gQtWrite{}, gQtOut{}, gQtA{}, gQtB{}, gQtC{};
inline u32 gAtomic{}, gHier{}, gSkin{}, gNumBones{}, gMats{}, gXor[3]{};
inline u32 gPokeId{}, gPokeIdx{}, gPokeKind{}, gPokeA{}, gPokeB{}, gPokeVal{}, gPokeCount{};
inline DWORD gBenchThread{};

inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }
// Efecto lateral del caso: tras anotar el evento `id` (la llamada gPokeIdx de ese id) escribe 4 bytes en el destino (region global mas desplazamiento o arena mas desplazamiento)
inline void PokeIf(u32 id) {
    if (!gPokeId || id != gPokeId) return;
    if (gPokeCount++ != gPokeIdx) return;
    const uintptr_t at = gPokeKind == 0 ? uintptr_t(kGlobal[gPokeA]) + gPokeB : ArenaAt(gPokeA).Obj(gPokeB);
    memcpy(reinterpret_cast<void*>(at), &gPokeVal, 4);
}

// --- funciones falsas (cdecl)
inline u32 __cdecl FkStreamRead(u32 stream, u32 ptr, u32 size) {
    Ev& e = PushEv(0x7ED540, 3); DumpTag(e, 0, stream); DumpTag(e, 1, ptr); e.v[2] = size;
    if (gReadWrite) memcpy(reinterpret_cast<void*>(uintptr_t(ptr)), &gReadVal, 4);
    PokeIf(0x7ED540);
    return gReadRet;
}
inline u32 __cdecl FkStreamWrite(u32 stream, u32 ptr, u32 size) {
    u32 v; memcpy(&v, reinterpret_cast<const void*>(uintptr_t(ptr)), 4);
    Ev& e = PushEv(0x7ED460, 4); DumpTag(e, 0, stream); DumpTag(e, 1, ptr); e.v[2] = size; e.v[3] = v;
    PokeIf(0x7ED460);
    return gWriteRet;
}
// Puente QuickTime 0x81A3C0 (12 argumentos): escribe A, B y C (y la salida) y devuelve el OSErr en la palabra baja de EAX
inline u32 __cdecl FkQt(u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10, u32 a11, u32 a12) {
    Ev& e = PushEv(0x81A3C0, 12);
    e.v[0] = a1; e.v[1] = a2; e.v[2] = a3; DumpTag(e, 3, a4); e.v[4] = a5; DumpTag(e, 5, a6); DumpTag(e, 6, a7); e.v[7] = a8; e.v[8] = a9; e.v[9] = a10; DumpTag(e, 10, a11); e.v[11] = a12;
    memcpy(reinterpret_cast<void*>(uintptr_t(a7)), &gQtA, 4);
    memcpy(reinterpret_cast<void*>(uintptr_t(a11)), &gQtB, 4);
    memcpy(reinterpret_cast<void*>(uintptr_t(a6)), &gQtC, 4);
    if (gQtWrite) memcpy(reinterpret_cast<void*>(uintptr_t(a4)), &gQtOut, 4);
    PokeIf(0x81A3C0);
    return gQtEax;
}
// RpClumpForAllAtomics (0x749B70): los dos callbacks de RwHelper solo guardan el atomo o la jerarquia en *data
inline u32 __cdecl FkForAllAtomics(u32 clump, u32 cb, u32 data) {
    Ev& e = PushEv(0x749B70, 3); DumpTag(e, 0, clump); e.v[1] = cb; DumpTag(e, 2, data);
    if (cb == 0x734810) memcpy(reinterpret_cast<void*>(uintptr_t(data)), &gAtomic, 4);
    else if (cb == 0x734A20) memcpy(reinterpret_cast<void*>(uintptr_t(data)), &gHier, 4);
    PokeIf(0x749B70);
    return clump;
}
inline u32 __cdecl FkGetSkin(u32 geometry) { Ev& e = PushEv(0x7C7550, 1); e.v[0] = geometry; PokeIf(0x7C7550); return gSkin; }
inline u32 __cdecl FkNumBones(u32 skin) { Ev& e = PushEv(0x7C77E0, 1); e.v[0] = skin; PokeIf(0x7C77E0); return gNumBones; }
inline u32 __cdecl FkSkinMats(u32 skin) { Ev& e = PushEv(0x7C7810, 1); e.v[0] = skin; PokeIf(0x7C7810); return gMats; }
inline u32 __cdecl FkMatArray(u32 hier) { Ev& e = PushEv(0x7C5120, 1); DumpTag(e, 0, hier); PokeIf(0x7C5120); return gMats; }
inline u32 __cdecl FkMatInvert(u32 dst, u32 src) {
    u32 w[16]; memcpy(w, reinterpret_cast<const void*>(uintptr_t(src)), 64);
    Ev& e = PushEv(0x7F2070, 3); DumpTag(e, 0, dst); DumpTag(e, 1, src); e.v[2] = Crc32(w, 64);
    u32 o[16];
    for (u32 k = 0; k < 16; ++k) o[k] = w[k] ^ (k * 0x01010101u + gXor[0]);
    memcpy(reinterpret_cast<void*>(uintptr_t(dst)), o, 64);
    PokeIf(0x7F2070);
    return dst;
}
inline u32 __cdecl FkTransform(u32 dst, u32 src, u32 count, u32 mat) {
    u32 v[3]; memcpy(v, reinterpret_cast<const void*>(uintptr_t(src)), 12);
    const u32 mcrc = Crc32(reinterpret_cast<const void*>(uintptr_t(mat)), 64);
    Ev& e = PushEv(0x7EDD90, 6); DumpTag(e, 0, dst); DumpTag(e, 1, src); e.v[2] = count; DumpTag(e, 3, mat); e.v[4] = Crc32(v, 12); e.v[5] = mcrc;
    for (int i = 0; i < 3; ++i) { const u32 w = v[i] ^ gXor[i] ^ mcrc; memcpy(reinterpret_cast<void*>(uintptr_t(dst) + 4 * i), &w, 4); }
    PokeIf(0x7EDD90);
    return dst;
}

// Especificacion de valor del caso -> valor real
inline u32 Res(u32 v) { return IsPS(v) ? u32(ArenaAt((v >> 28) - 0xAu).Obj(v & 0x0FFFFFFFu)) : v; }

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

inline u8* At(uintptr_t addr) { return reinterpret_cast<u8*>(addr); }
// Banderas de los nodos de la jerarquia (hueso 1..n-1): mismas reglas que el oraculo (nunca POP sin PUSH previo; profundidad maxima 24)
inline void PutNodeFlags(Arena<kWin>& C, u32 base, u32 seed, i32 n) {
    i32 depth = 0;
    for (i32 i = 1; i < n; ++i) {
        u32 fl = Rand(seed, u32(i)) & 3u;
        if ((fl & 2u) && depth >= 24) fl &= ~2u;
        if (fl & 2u) ++depth;
        if ((fl & 1u) && depth == 0) fl &= ~1u; else if (fl & 1u) --depth;
        C.Put32(base + 16u * u32(i) + 8u, fl);
    }
}
// Prepara las arenas y las regiones globales del caso; devuelve la llamada. Las escrituras son las del oraculo (fixture_oracle.py).
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gReadRet = gReadWrite = gReadVal = gWriteRet = 0;
    gQtEax = gQtWrite = gQtOut = gQtA = gQtB = gQtC = 0;
    gAtomic = gHier = gSkin = gNumBones = gMats = 0;
    for (auto& v : gXor) v = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    for (const u32 gi : GlobalsOf(c.fn)) {
        u8* p = reinterpret_cast<u8*>(kGlobal[gi]);
        if (gi == 3) { FillWords(p, 0x400, a[62]); p[0x400] = uint8(a[51]); }
        else { const u32 v = a[48 + gi]; memcpy(p, &v, 4); }
    }
    auto& A = arenaA; auto& B = arenaB; auto& Cc = arenaC; auto& D = arenaD;
    constexpr u32 kJunk0 = 0xAAAA5555u, kJunk1 = 0x00003333u;
    switch (kKind[c.fn]) {
    case kEngine:
    case kTexCtor:
    case kTexDtor: {
        const u32 obj = kKind[c.fn] == kTexCtor ? thisA : a[4];
        call.args = {obj, a[5], a[6]}; call.argCount = 3; call.popBytes = 12;
        break;
    }
    case kTexCopy: {
        const u32 src = a[10] ? thisA : u32(B.Obj(0)) + a[9];
        call.args = {thisA, src, kJunk0, kJunk1}; call.argCount = 4; call.popBytes = 16;
        break;
    }
    case kStRead:
        gReadRet = a[8]; gReadWrite = a[9]; gReadVal = a[10];
        call.args = {a[4], a[5], a[6] ? thisA : 0u, kJunk0, kJunk1}; call.argCount = 5; call.popBytes = 20;
        break;
    case kStWrite:
        A.Put32(o, a[7]);
        gWriteRet = a[9];
        call.args = {a[4], a[5], a[6] ? thisA : 0u, kJunk0, kJunk1}; call.argCount = 5; call.popBytes = 20;
        break;
    case kStSize:
        A.Put32(o, a[7]);
        if (a[9] >= 1 && a[9] <= 5) { static constexpr u32 vals[6]{0, 1, 0, 2, 0xFF, 0}; A.Put8(a[8], a[9] == 5 ? (a[10] & 0xFF) : vals[a[9]]); }
        call.args = {a[6] ? thisA : 0u, kJunk0, kJunk1}; call.argCount = 3; call.popBytes = 12;
        break;
    case kFindFree: {
        for (u32 i = 0; i < 75; ++i) {
            const u32 r = Rand(a[5], i);
            u32 t = (r % 255u) + 1u;
            if (i == a[4]) t = 0;
            else if (i > a[4] && (r >> 16) % 3u == 0) t = 0;
            A.Put8(o + 8u * i, t);
        }
        call.argCount = 0; call.popBytes = 0;
        break;
    }
    case kQuat: {
        const u32 srcAddr = a[4] == 0 ? u32(B.Obj(a[5])) : (a[4] == 1 ? u32(Cc.Obj(a[5])) : (a[4] == 2 ? u32(A.Obj(a[5])) : thisA));
        if (a[4] != 3) memcpy(reinterpret_cast<void*>(uintptr_t(srcAddr)), &a[6], 16);
        call.args[0] = srcAddr; call.argCount = 1; call.popBytes = 0;   // `ret 4`: limpia el llamado
        break;
    }
    case kNop: {
        const u32 p1 = a[6] == 0 ? u32(B.Obj(0x20 + (a[4] & 7))) : (a[6] == 1 ? thisA : u32(A.Obj(0x60 + (a[4] & 3))));
        const u32 p2 = a[7] == 0 ? u32(B.Obj(0x40 + (a[5] & 7))) : (a[7] == 1 ? u32(Cc.Obj(0x10 + (a[5] & 3))) : u32(A.Obj(0x200 + (a[5] & 7))));
        call.args = {p1, p2}; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kFill: {
        A.Put32(o + 0x4C, a[5]); A.Put32(o + 0x50, a[6]); A.Put32(o + 0x44, a[7]); A.Put32(o + 0x48, a[8]); A.Put32(o + 0x54, a[9]);
        A.Put8(o + 0x58, a[10]); A.Put8(o + 0x59, a[11]);
        gQtEax = a[12]; gQtOut = a[13]; gQtWrite = a[14]; gQtA = a[15]; gQtB = a[16]; gQtC = a[17];
        call.args = {u32(B.Obj(a[4])), thisA}; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kFight: {
        const u32 anim = u32(B.Obj(a[4]));
        B.Put16(a[4] + 0x2C, a[5] & 0xFFFF);
        u32 w1c = a[7], w20 = a[8];
        if (a[6] == 0) w1c = anim; else if (a[6] == 1) w20 = anim; else if (a[6] == 3) { w1c = anim; w20 = anim; }
        A.Put32(o + 0x1C, w1c); A.Put32(o + 0x20, w20); A.Put8(o + 0x27, a[9]); A.Put8(o + 8, a[10]);
        call.args = {anim, thisA}; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kKick:
    case kHit: {
        const u32 off = kKind[c.fn] == kKick ? 0xCu : 0x14u;
        A.Put32(o + off, a[4]); A.Put8(o + 8, a[5]);
        call.args = {u32(B.Obj(a[6])), thisA}; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kGang: {
        const u32 anim = a[7] ? 0u : u32(B.Obj(a[6]));
        A.Put32(o + 0x2C, a[5] ? anim : a[4]);
        call.args = {anim, thisA}; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kSit: {
        const u32 anim = u32(B.Obj(a[4]));
        B.Put16(a[4] + 0x2C, a[5] & 0xFFFF); B.Put32(a[4] + 0x1C, a[6]);
        A.Put8(o + 0x2A, a[7]); A.Put8(o + 0x29, a[8]); A.Put32(o + 0x18, a[9]); A.Put32(o + 0x14, a[10]);
        call.args = {anim, thisA}; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kAlpha:
        call.args = {thisA, a[4]}; call.argCount = 2; call.popBytes = 8;
        break;
    case k2dfx: {
        A.Put32(o + 0x18, u32(B.Obj(a[4])));
        B.Put32(a[5], a[6] ? u32(Cc.Obj(a[6])) : 0u);
        if (a[6]) Cc.Put32(a[6], a[7]);
        call.args = {thisA, u32(D.Obj(a[8]))}; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kSkinGet:
    case kSkinSet: {
        const i32 n = i32(a[4]);
        gNumBones = a[4]; gSkin = a[5]; gAtomic = u32(B.Obj(a[7])); gHier = u32(B.Obj(a[8])); gMats = u32(D.Obj(a[10]));
        gXor[0] = a[11]; gXor[1] = a[12]; gXor[2] = a[13];
        B.Put32(a[7] + 0x18, a[6]);
        B.Put32(a[8] + 0x10, u32(Cc.Obj(a[9])));
        PutNodeFlags(Cc, a[9], a[14], n);
        if (kKind[c.fn] == kSkinSet) {
            u8* p = reinterpret_cast<u8*>(kGlobal[3]);
            for (i32 i = 1; i < n; ++i) { const u32 parent = Rand(a[14], 1000u + u32(i)) % u32(n); memcpy(p + 16 * i, &parent, 4); }
        }
        call.args[0] = thisA; call.argCount = 1; call.popBytes = 4;
        break;
    }
    }
    gPokeId = a[40]; gPokeIdx = a[41]; gPokeKind = a[42]; gPokeA = a[43]; gPokeB = a[44]; gPokeVal = a[46]; gPokeCount = 0;
    return call;
}

enum RetKind { rNone, rEax, rAl };
inline RetKind RetOf(unsigned fn) {
    if (fn <= 8 || fn == 17 || fn == 18) return rEax;
    return fn == 11 ? rAl : rNone;
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
        obs["big"] = RegionBigCrc();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        if (!gThrew && RetOf(fn) == rEax) obs["ret"] = eax;
        else if (!gThrew && RetOf(fn) == rAl) obs["ret"] = eax & 0xFFu;
        else obs["ret"] = nullptr;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FNS7A1A_CASES (solo para el humo y las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FNS7A1A_CASES", buffer, sizeof(buffer)) > 0) {
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
    // El banco ya escribio todos los resultados: el juego sale aqui (el siguiente fotograma tras una corrida larga puede perder el dispositivo D3D, ajeno al lote).
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
    if (!std::filesystem::exists(dir / "fns7a1a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fns7a1a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fns7a1a"); }
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
    auto Collab = [&](uintptr_t exe) -> Pair {
        auto it = collabCache.find(exe);
        if (it != collabCache.end()) return it->second;
        Pair p = MakeCollab(env, exe, hooksFound);
        return collabCache.emplace(exe, p).first->second;
    };
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en la direccion del EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook (el puente QuickTime 0x81A3C0): falso solo en la direccion del EXE, que es la que llama la DLL
    auto U = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (p.hooked) ready = false;              // se esperaba codigo original del EXE
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[5] = {F(0x7ED540, &FkStreamRead)};
    env.stubs[6] = {F(0x7ED460, &FkStreamWrite)};
    env.stubs[11] = {U(0x81A3C0, &FkQt)};
    env.stubs[19] = {F(0x749B70, &FkForAllAtomics), F(0x7C7550, &FkGetSkin), F(0x7C77E0, &FkNumBones), F(0x7C7810, &FkSkinMats), F(0x7F2070, &FkMatInvert), F(0x7EDD90, &FkTransform)};
    env.stubs[20] = {F(0x749B70, &FkForAllAtomics), F(0x7C7550, &FkGetSkin), F(0x7C5120, &FkMatArray), F(0x7C77E0, &FkNumBones), F(0x7EDD90, &FkTransform)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en el cuerpo de la DLL)";
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
    for (unsigned gi = 0; gi < kRegions; ++gi) regions.emplace_back(kGlobal[gi], kGSize[gi]);
    for (unsigned f = 0; f < kFunctions; ++f) for (const u32 gi : GlobalsOf(f)) regionsOf[f].push_back(gi);
    Run(env, ready, registered, bodies, collaborators);
}
}
