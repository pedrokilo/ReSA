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

// Banco par a par Fn205A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 32 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor de diez eventos
// (0x4AF0A0, 0x4AF1C0, 0x4AF2D0, 0x4AF400, 0x4AF700, 0x4AF890, 0x4AFBF0, 0x4AFE70, 0x4B07B0, 0x4B0A50), 0x66F670, 0x66A1D0, ~CTaskComplex 0x61A3D0, CTask::operator delete 0x61A5B0, CColSphere::Set 0x40FD10, ComputeRouteRoundSphere
// 0x5F1890, AbortPedTasks 0x5EAF60 e IsRegisteredWithPed 0x5EB4C0 (todos con hook); sin hook en esta base (la DLL los llama por su direccion del EXE): 0x66AAE0, 0x5EAC50 y 0x5EACC0. El borrado en linea de los pools
// (0xB744AC y 0xB744C4) corre real sobre pools sinteticos. Memoria global del juego tocada (restaurada tras cada lado): los punteros a los pools 0xB744AC y 0xB744C4 y la tabla de model infos 0xA9B0C8 (desde el indice -16 hasta el 32767).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F205ALive {
using namespace F205AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F205AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    void Put16(size_t off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    // Version compacta (FurnitureManager_c::Constructor escribe unos 4200 bytes): numero de bytes cambiados y FNV-1a de la lista (desplazamiento como int32 little endian y byte nuevo)
    json ChangesCompact() const {
        u32 h = 2166136261u, n = 0;
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) {
                const i32 off = i32(int64(i) - int64(kGuard));
                const u8* b = reinterpret_cast<const u8*>(&off);
                for (int k = 0; k < 4; ++k) { h ^= b[k]; h *= 16777619u; }
                h ^= m[i]; h *= 16777619u; ++n;
            }
        return {{"n", n}, {"h", h}};
    }
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
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de ComputeRouteRoundSphere
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline std::array<u32, 4> gSph{};    // lo que escribe el falso de ComputeSphere (centro x, y, z y radio)
inline u32 gMask{}, gJunk{}, gCalls{};   // resultados de IsRegisteredWithPed (bit por llamada), bits altos de EAX y numero de llamadas
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Igual pero SIN escribir nada (AbortPedTasks 0x5EAF60): evento [direccion, ECX (etiqueta), dword que tenia [ECX] al entrar].
template<u32 ADDR> inline u32 __fastcall FkRec(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    return 0;
}
// CColSphere::Set (0x40FD10; thiscall, radio, puntero al centro y tres bytes de pila, `ret 14h`): evento [direccion, ECX (etiqueta), radio (bits), x, y, z del centro, material, pieza, iluminacion (solo el BYTE bajo)].
inline void __fastcall FkSet(void* self, void*, u32 radius, const u32* center, u32 material, u32 piece, u32 lighting) {
    Ev& e = PushEv(0x40FD10, 8);
    e.v[0] = Tag(self); e.v[1] = radius; e.v[2] = center[0]; e.v[3] = center[1]; e.v[4] = center[2];
    e.v[5] = material & 0xFFu; e.v[6] = piece & 0xFFu; e.v[7] = lighting & 0xFFu;
}
// ComputeSphere (0x66F6C0; thiscall con ECX sin leer, salida y array de 16 punteros, `ret 8`): evento [direccion, ECX (etiqueta), salida (etiqueta), array (etiqueta), los 16 punteros (etiquetas)];
// escribe gSph en la salida (centro y radio).
inline void __fastcall FkSphere(void* self, void*, u32* out, u32* peds) {
    Ev& e = PushEv(0x66F6C0, 19);
    e.v[0] = Tag(self); e.v[1] = Tag(out); e.v[2] = Tag(peds);
    for (u32 i = 0; i < 16; ++i) e.v[3 + i] = Tag(uintptr_t(peds[i]));
    out[0] = gSph[0]; out[1] = gSph[1]; out[2] = gSph[2]; out[3] = gSph[3];
}
// CPedGeometryAnalyser::ComputeRouteRoundSphere (0x5F1890; cdecl, peaton, esfera, cuatro punteros): evento [direccion, peaton (etiqueta), centro x, y, z y radio de la esfera, los cuatro punteros (etiquetas)]; devuelve gRet.
inline u32 __cdecl FkRouteRound(void* ped, const u32* sphere, void* a, void* b, void* c, void* d) {
    Ev& e = PushEv(0x5F1890, 9);
    e.v[0] = Tag(ped); e.v[1] = sphere[0]; e.v[2] = sphere[1]; e.v[3] = sphere[2]; e.v[4] = sphere[3];
    e.v[5] = Tag(a); e.v[6] = Tag(b); e.v[7] = Tag(c); e.v[8] = Tag(d);
    return gRet;
}
// copy_backward de 20 y de 12 bytes (0x5EAC50 / 0x5EACC0; cdecl, cuatro argumentos que se leen): evento [direccion, out, first, last, destEnd (etiquetas)]. No escribe nada.
template<u32 ADDR> inline u32 __cdecl FkCopyW(void* out, void* first, void* last, void* destEnd) {
    Ev& e = PushEv(ADDR, 4);
    e.v[0] = Tag(out); e.v[1] = Tag(first); e.v[2] = Tag(last); e.v[3] = Tag(destEnd);
    return 0;
}
// CPedAttractor::IsRegisteredWithPed (0x5EB4C0; thiscall, el peaton, `ret 4`): evento [direccion, ECX (etiqueta), peaton (etiqueta)]; devuelve el bit gCalls de gMask en AL y gJunk en los bytes altos.
inline u32 __fastcall FkReg(void* self, void*, void* ped) {
    Ev& e = PushEv(0x5EB4C0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
    const u32 bit = (gMask >> (gCalls & 31)) & 1u;
    ++gCalls;
    return bit | (gJunk & ~0xFFu);
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
    std::array<u32, 16> args{};
    u32 argCount{}, popBytes{}, ecx{}, eax{};
};

inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }
inline size_t Off(u32 base, i32 delta) { return size_t(int64(base) + delta); }
inline Arena<kWin>& ArenaOf(u32 i) { switch (i & 3) { case 0: return arenaA; case 1: return arenaB; case 2: return arenaC; default: return arenaD; } }
inline u32 AddrIn(u32 arena, u32 off) { return u32(ArenaOf(arena).Obj(off)); }
inline std::vector<u32> WordsOf(const std::vector<u8>& s) { std::vector<u32> w(s.size() / 4); if (!s.empty()) memcpy(w.data(), s.data(), w.size() * 4); return w; }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx; call.eax = c.eax;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gSph = {}; gMask = 0; gJunk = 0; gCalls = 0;
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    call.ecx = self;
    arenaA.Put32(o, a[3]);
    switch (kKind[c.fn]) {
    case kEvtSdd:
    case kAttrDel: {
        const bool evt = kKind[c.fn] == kEvtSdd;
        const i32 stride = evt ? 0x44 : 0xC4;
        const i32 delta = i32(a[5]);
        const i32 idx = delta / stride;   // truncado hacia cero, como el EXE
        arenaB.Put32(a[4] + 0, self - u32(delta)); arenaB.Put32(a[4] + 4, u32(arenaD.Obj(a[6]))); arenaB.Put32(a[4] + 8, a[12]); arenaB.Put32(a[4] + 0xC, a[10]);
        arenaD.Put8(Off(a[6], idx), u8(a[11]));
        PutRaw32(evt ? kEventPoolGlobal : kAttrPoolGlobal, u32(arenaB.Obj(a[4])));
        if (evt) { gRet = a[8]; gDtorWrite = a[9]; call.args[0] = a[7]; call.argCount = 1; }
        else { call.args[0] = self; call.argCount = 1; call.popBytes = 4; }
        break;
    }
    case kTaskSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kBody:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kBool1:
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kSphere: {
        const auto s = WordsOf(c.s);
        for (u32 i = 0; i < kSelWords; ++i) arenaA.Put32(o + 4 * i, s[i] ? u32(arenaC.Obj(kPedStride * (s[i] - 1))) : 0u);
        const u32 poolAt = kSelWords + 16 * kSlotWords;
        for (u32 j = 0; j < 16; ++j) {
            const u32 b = kSelWords + j * kSlotWords, po = kPedStride * j, mo = kMatStride * j;
            arenaC.Put32(po + 4, s[b + 0]); arenaC.Put32(po + 8, s[b + 1]); arenaC.Put32(po + 0xC, s[b + 2]);
            arenaC.Put32(po + 0x14, s[b + 3] ? u32(arenaD.Obj(mo)) : 0u);
            if (s[b + 3]) { arenaD.Put32(mo + 0x30, s[b + 4]); arenaD.Put32(mo + 0x34, s[b + 5]); arenaD.Put32(mo + 0x38, s[b + 6]); }
            arenaC.Put16(po + 0x22, u16(s[poolAt + s[b + 7]]));
        }
        for (u32 q = 0; q < 8; ++q) {
            arenaD.Put32(kMiBase + q * kMiStride + 0x14, u32(arenaD.Obj(kCmBase + q * kCmStride)));
            arenaD.Put32(kCmBase + q * kCmStride + 0x24, s[poolAt + 8 + q]);
            PutRaw32(kModelTable + 4u * u32(i32(i16(s[poolAt + q]))), u32(arenaD.Obj(kMiBase + q * kMiStride)));
        }
        call.args[0] = u32(arenaB.Obj(a[4])); call.args[1] = self; call.argCount = 2;
        break;
    }
    case kRoute: {
        arenaA.Put32(o + 0xC, a[4]);
        const u32 ped = u32(arenaB.Obj(a[5]));
        arenaB.Put32(a[5] + 4, a[6]); arenaB.Put32(a[5] + 8, a[7]); arenaB.Put32(a[5] + 0xC, a[8]);
        const u32 matrix = a[9] ? u32(arenaC.Obj(a[10])) : 0u;
        arenaB.Put32(a[5] + 0x14, matrix);
        if (matrix) { arenaC.Put32(a[10] + 0x30, a[11]); arenaC.Put32(a[10] + 0x34, a[12]); arenaC.Put32(a[10] + 0x38, a[13]); }
        gSph = {a[14], a[15], a[16], a[17]};
        gRet = a[18];
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kCopyBack: {
        const u32 cnt = a[6], sa = a[7] % 4, sfirst = a[8] + a[9], slast = sfirst + 4 * cnt;
        u32 da, dend;
        if (a[10] == 0) { da = (sa + 1 + a[11] % 3) % 4; dend = a[11] + a[12] + 4 * cnt; }
        else if (a[10] == 1) { da = sa; dend = u32(i32(slast) + 4 * i32(a[13])); }
        else { da = sa; dend = slast + 0x300; }
        const u32 dstart = dend - 4 * cnt;
        u32 oa = 1, ooff = a[5];
        if (a[4] == 1 && cnt) { oa = da; ooff = dstart + 4 * a[14]; }
        else if (a[4] == 2 && cnt) { oa = sa; ooff = sfirst + 4 * a[14]; }
        call.args = {AddrIn(oa, ooff), AddrIn(sa, sfirst), AddrIn(sa, slast), AddrIn(da, dend)}; call.argCount = 4; call.popBytes = 16;
        break;
    }
    case kWrap:
        for (u32 i = 0; i < 4; ++i) call.args[i] = a[8 + i] ? AddrIn((a[8 + i] >> 16) - 1, a[8 + i] & 0xFFFFu) : a[4 + i];
        call.argCount = 4; call.popBytes = 16;
        break;
    case kFill12: {
        const u32 first = AddrIn(1, a[4] + a[8]);
        const u32 val = a[6] == 0 ? AddrIn(2, a[7] + a[9]) : (a[6] == 1 ? first + 4u * a[7] : a[10]);
        call.args = {first, first + 12u * a[5], val}; call.argCount = 3; call.popBytes = 12;
        break;
    }
    case kFillN: {
        const u32 dest = a[4] ? AddrIn(1, a[5] + a[9]) : 0u;
        const u32 val = a[7] == 0 ? AddrIn(2, a[8] + a[10]) : (a[7] == 1 ? dest + 4u * a[8] : a[11]);
        call.args = {dest, a[6], val, a[12], a[13]}; call.argCount = 5; call.popBytes = 20;
        break;
    }
    case kRemove:
    case kIsReg:
    case kIsRegEff: {
        const u32 m = a[9], fx = a[6], ent = a[7], ped = a[8];
        const u32 firstArr = u32(arenaD.Obj(a[5]));
        for (u32 i = 0; i < m; ++i) arenaD.Put32(a[5] + 4 * i, u32(arenaB.Obj(0x40 * a[10 + i])));
        for (u32 slot = 0; slot < 8; ++slot) {
            arenaB.Put32(0x40 * slot + 4, a[18 + slot] == 0 ? fx : fx + 1 + slot);
            arenaB.Put32(0x40 * slot + 8, a[26 + slot] == 0 ? ent : ent + 1 + slot);
        }
        u32 first = firstArr, last = firstArr + 4 * m;
        switch (a[35]) {
        case 1: first = 0; last = 0x12345678u; break;
        case 2: last = firstArr - 4 * (1 + a[9] % 8); break;
        case 3: last = firstArr; break;
        case 4: last = firstArr + 0x80000000u + 4 * a[9]; break;
        default: break;
        }
        arenaC.Put32(a[4] + 4, first); arenaC.Put32(a[4] + 8, last);
        gMask = a[34]; gJunk = c.edx; gCalls = 0;
        const u32 vec = u32(arenaC.Obj(a[4]));
        if (kKind[c.fn] == kRemove) { call.args = {fx, vec}; call.argCount = 2; }
        else if (kKind[c.fn] == kIsReg) { call.args = {ped, vec}; call.argCount = 2; }
        else { call.args = {ped, fx, ent, vec}; call.argCount = 4; }
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
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kEvtSdd: case kTaskSdd: case kCopyBack: case kWrap: obs["ret"] = Tag(uintptr_t(eax)); break;                                  // this / out como etiqueta de arena
        case kBool1: case kRoute: case kRemove: case kIsReg: case kIsRegEff: obs["ret"] = eax & 0xFFu; break;                              // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN205A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN205A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && (env.chain[f] >= 0 || ModuleOf(body) == env.asi);
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
    if (!std::filesystem::exists(dir / "fn205a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn205a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn205a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        constexpr int kChainOf[kFunctions] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,18,-1,-1,24,-1,24,-1,-1,-1};   // copias identicas B y C de copy_backward de 4 bytes y de fill_n de 4 bytes
        if (found && kChainOf[f] >= 0 && env.route[kChainOf[f]]) {
            // copia identica: el destino de su hook es la entrada de la primera copia (codigo del EXE si su hook esta apagado); el candidato se prueba con los dos hooks encendidos (Execute)
            env.chain[f] = kChainOf[f];
            env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        } else if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0 && (env.chain[f] >= 0 || ModuleOf(env.route[f]->Body()) == env.asi);
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && (env.chain[f] >= 0 || ModuleOf(env.route[f]->Body()) == env.asi);
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}, {"body", env.route[f]->Body()}, {"chain", env.chain[f]}, {"kchain", kChainOf[f]}, {"chain_body", kChainOf[f] >= 0 && env.route[kChainOf[f]] ? env.route[kChainOf[f]]->Body() : 0}, {"body_module", ModuleOf(env.route[f]->Body())}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { collaborators = ownHooks; Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    // Colaborador SIN hook en esta base: la DLL lo llama por su direccion del EXE (se comprueba que de verdad no tenga hook)
    auto U = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x4AF0A0, &FkBody<0x4AF0A0>)};
    env.stubs[1] = {F(0x4AF1C0, &FkBody<0x4AF1C0>)};
    env.stubs[2] = {F(0x4AF2D0, &FkBody<0x4AF2D0>)};
    env.stubs[3] = {F(0x4AF400, &FkBody<0x4AF400>)};
    env.stubs[4] = {F(0x4AF700, &FkBody<0x4AF700>)};
    env.stubs[5] = {F(0x4AF890, &FkBody<0x4AF890>)};
    env.stubs[6] = {F(0x4AFBF0, &FkBody<0x4AFBF0>)};
    env.stubs[7] = {F(0x4AFE70, &FkBody<0x4AFE70>)};
    env.stubs[8] = {F(0x4B07B0, &FkBody<0x4B07B0>)};
    env.stubs[9] = {F(0x4B0A50, &FkBody<0x4B0A50>)};
    env.stubs[10] = {F(0x66F670, &FkBody<0x66F670>), Del()};
    env.stubs[11] = {F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[12] = {F(0x66A1D0, &FkBody<0x66A1D0>), Del()};
    env.stubs[13] = {U(0x66AAE0, &FkBody<0x66AAE0>), Del()};
    env.stubs[14] = {F(0x40FD10, &FkSet)};
    env.stubs[15] = {Rt(14, &FkSphere), F(0x5F1890, &FkRouteRound)};
    env.stubs[19] = {U(0x5EAC50, &FkCopyW<0x5EAC50>)};
    env.stubs[22] = {U(0x5EACC0, &FkCopyW<0x5EACC0>)};
    env.stubs[29] = {F(0x5EAF60, &FkRec<0x5EAF60>)};
    env.stubs[30] = {F(0x5EB4C0, &FkReg)};
    env.stubs[31] = {F(0x5EB4C0, &FkReg)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {11u, 16u, 17u, 18u, 20u, 21u, 23u, 24u, 25u, 26u, 27u, 28u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 0; f < 10; ++f) dllReal[f] = "toda la funcion salvo el cuerpo del destructor (falseado); el borrado de la ranura del pool de eventos corre real sobre un pool sintetico";
    dllReal[17] = "toda la funcion; el borrado de la ranura del pool de atractores corre real sobre un pool sintetico";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada; se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kEventPoolGlobal, 4);
    for (unsigned f = 0; f < 10; ++f) regionsOf[f] = {regions.size() - 1};
    regions.emplace_back(kAttrPoolGlobal, 4); regionsOf[17] = {regions.size() - 1};
    regions.emplace_back(kModelRegionStart, kModelRegionBytes); regionsOf[14] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
