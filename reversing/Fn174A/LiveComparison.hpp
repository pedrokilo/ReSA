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

// Banco par a par Fn174A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 20 funciones del EXE (cuatro CPad::IsMouse*Pressed, CMenuManager::NoDiskInDriveMessage, el cuerpo del destructor, el destructor con borrado y SetupDamageAfterLoad de CHeli, el cuerpo del destructor de CMonsterTruck y de CTrain,
// CFontChar::Set, StripTildeTags y MapExtendedChar de CFont, TransformPointsSimple, TransformVectors (CMatrix) y DoesInfiniteLineCrossFiniteLine de common, el constructor de tHandlingData, CTrainNode::GetPosn, SetModelIndex y PreRender
// de CTrain; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias
// (A, B, C con guardas de 0x40) y, para las funciones que leen memoria global del juego (estado del raton, RsGlobal.quit y las palabras de Esc), con regiones guardadas y restauradas tras cada lado. Falsos de los colaboradores: el mismo
// falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair).
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los bytes cambiados de la memoria global, los eventos de los falsos (con sus argumentos), el valor
// devuelto (this o la salida como etiqueta de arena; EAX completo; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote174ABody_CTrainBaseDestructor();  // 0x6E2B40 (con hook): llamada no virtual a ~CVehicle desde el cuerpo del destructor de CTrain

namespace F174ALive {
using namespace F174AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F174AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    uint8* At(size_t off) { return m.data() + kGuard + off; }
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
inline i32 gGuns{};                  // lo que devuelve el GetPlaneNumGuns falso
inline DWORD gBenchThread{};
inline constexpr uintptr_t kAddrWorkingMatrix = 0xB72920;   // CGame::m_pWorkingMatrix1 (puntero global, solo se lee)
// Puntero a la matriz de trabajo: el valor de la global se anota con una etiqueta fija; otro valor, en crudo
inline u32 WmTag(const void* p) { return u32(uintptr_t(p)) == *reinterpret_cast<const u32*>(kAddrWorkingMatrix) ? 0x5A5A0001u : u32(uintptr_t(p)); }

// Cuerpo del destructor / de la base (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
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
// Funcion sin argumentos (cdecl, `ret`): evento [direccion]; devuelve gRet.
template<u32 ADDR> inline u32 __cdecl FkNoArg() { PushEv(ADDR, 0); return gRet; }
// DoRWStuffStartOfFrame (0x53D690; cdecl, siete dwords en la pila): evento [direccion, los siete argumentos]
inline void __cdecl FkStart(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6) {
    Ev& e = PushEv(0x53D690, 7);
    e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; e.v[5] = a5; e.v[6] = a6;
}
// CMenuManager::MessageScreen (0x579330; thiscall, tres argumentos de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), FNV-1a de la clave, segundo y tercer argumentos (dwords completos)]
inline void __fastcall FkMessageScreen(void* self, void*, const char* key, u32 black, u32 camera) {
    Ev& e = PushEv(0x579330, 4);
    e.v[0] = Tag(self); e.v[1] = FnvStr(key); e.v[2] = black; e.v[3] = camera;
}
// CPad::GetPad (0x53FB70; cdecl, un argumento): evento [direccion, argumento]; devuelve gRet
inline u32 __cdecl FkGetPad(u32 index) { Ev& e = PushEv(0x53FB70, 1); e.v[0] = index; return gRet; }
// RsEventHandler (0x619B60; cdecl, dos argumentos): evento [direccion, evento, parametro]; devuelve gRet
inline u32 __cdecl FkRsEvent(u32 event, u32 param) { Ev& e = PushEv(0x619B60, 2); e.v[0] = event; e.v[1] = param; return gRet; }
// CVehicle::GetPlaneNumGuns (0x6D3F30; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]; devuelve gGuns
inline i32 __fastcall FkGuns(void* self, void*) { Ev& e = PushEv(0x6D3F30, 1); e.v[0] = Tag(self); return gGuns; }
// FxSystem_c::Kill (0x4AA3F0; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]
inline void __fastcall FkKill(void* self, void*) { Ev& e = PushEv(0x4AA3F0, 1); e.v[0] = Tag(self); }
// FxManager_c::DestroyFxSystem (0x4A9810; thiscall, un argumento de pila, `ret 4`): evento [direccion, ECX (crudo: &g_fxMan), argumento (etiqueta)]
inline void __fastcall FkDestroyFx(void* self, void*, void* fx) { Ev& e = PushEv(0x4A9810, 2); e.v[0] = u32(uintptr_t(self)); e.v[1] = Tag(fx); }
// CAEVehicleAudioEntity::Terminate (0x4FB8C0; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]
inline void __fastcall FkTerminate(void* self, void*) { Ev& e = PushEv(0x4FB8C0, 1); e.v[0] = Tag(self); }
// CMatrix::CopyToRwMatrix (0x59B8B0; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (matriz de trabajo)]
inline void __fastcall FkCopyRw(void* self, void*, void* rw) { Ev& e = PushEv(0x59B8B0, 2); e.v[0] = Tag(self); e.v[1] = WmTag(rw); }
// RwMatrixUpdate (0x7F18A0; cdecl, un puntero): evento [direccion, argumento (matriz de trabajo)]; devuelve 1
inline u32 __cdecl FkRwUpdate(void* matrix) { Ev& e = PushEv(0x7F18A0, 1); e.v[0] = WmTag(matrix); return 1; }
// RwV3dTransformVector (0x7EDDC0; cdecl, tres punteros): evento [direccion, salida (etiqueta), entrada (etiqueta), matriz (matriz de trabajo)]; escribe en la salida los tres dwords de la entrada con xor
// 0x11111111, 0x22222222 y 0x33333333 (leidos antes de escribir) y devuelve la salida
inline void* __cdecl FkRwTransformVector(u32* out, const u32* in, const void* matrix) {
    Ev& e = PushEv(0x7EDDC0, 3);
    e.v[0] = Tag(out); e.v[1] = Tag(in); e.v[2] = WmTag(matrix);
    const u32 x = in[0], y = in[1], z = in[2];
    out[0] = x ^ 0x11111111u; out[1] = y ^ 0x22222222u; out[2] = z ^ 0x33333333u;
    return out;
}
// CVehicle::SetModelIndex (0x6D6A40, ranura virtual; thiscall, un argumento de pila, `ret 4`): evento [direccion, ECX (etiqueta), indice]
inline void __fastcall FkSetModelIndex(void* self, void*, u32 index) { Ev& e = PushEv(0x6D6A40, 2); e.v[0] = Tag(self); e.v[1] = index; }
// CClumpModelInfo::FillFrameArray (0x4C5440; cdecl, clump y tabla): evento [direccion, clump (etiqueta), tabla (etiqueta)]; deja gDtorWrite en tabla[0]
inline void __cdecl FkFillFrame(void* clump, u32* frames) { Ev& e = PushEv(0x4C5440, 2); e.v[0] = Tag(clump); e.v[1] = Tag(frames); memcpy(frames, &gDtorWrite, 4); }
// CVehicle::PreRender (0x6D6480, ranura virtual; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]
inline void __fastcall FkPreRender(void* self, void*) { Ev& e = PushEv(0x6D6480, 1); e.v[0] = Tag(self); }
// CVehicle::DoVehicleLights (0x6E1A60; thiscall, matriz y banderas en la pila, `ret 8`): evento [direccion, ECX (etiqueta), matriz (etiqueta), banderas]
inline void __fastcall FkDoLights(void* self, void*, void* matrix, u32 flags) { Ev& e = PushEv(0x6E1A60, 3); e.v[0] = Tag(self); e.v[1] = Tag(matrix); e.v[2] = flags; }

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

inline void PutRaw32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline void PutRaw16(uintptr_t at, u16 v) { memcpy(reinterpret_cast<void*>(at), &v, 2); }

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

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gGuns = 0;
    const u32 self = u32(arenaA.Obj(a[0]));
    const size_t o = a[0];
    switch (kKind[c.fn]) {
    case kMouse: {   // memoria global: la region de 0x28 bytes se rellena con la semilla y despues se fijan el byte nuevo y el anterior de la funcion
        u8 junk[kMouseRegionBytes];
        FillWords(junk, sizeof(junk), a[15]);
        memcpy(reinterpret_cast<void*>(uintptr_t(kMouseRegion)), junk, sizeof(junk));
        *reinterpret_cast<u8*>(uintptr_t(kMouseNew + 3 + c.fn)) = u8(a[3]);
        *reinterpret_cast<u8*>(uintptr_t(kMouseOld + 3 + c.fn)) = u8(a[4]);
        break;
    }
    case kNoDisk:
        PutRaw32(kAddrQuit, a[3]);
        PutRaw16(kAddrEscNew, u16(a[4]));
        PutRaw16(kAddrEscOld, u16(a[5]));
        arenaA.Put32(o + 0x58, a[6]);
        gRet = a[7];
        call.ecx = self;
        break;
    case kHeliBody: {
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5]; gGuns = i32(a[8]);
        if (a[6]) {
            arenaA.Put32(o + 0xA0C, u32(arenaB.Obj(a[7])));
            for (unsigned i = 0; i < 16; ++i) {
                const u32 mode = a[32 + i];
                arenaB.Put32(a[7] + 4 * i, mode == 0 ? 0u : (mode == 1 ? u32(arenaC.Obj(a[16 + i])) : a[16 + i]));
            }
        } else {
            arenaA.Put32(o + 0xA0C, 0);
        }
        call.ecx = self;
        break;
    }
    case kHeliDamage:
        arenaA.Put32(o + 0x424, a[3]); arenaA.Put32(o + 0x428, a[4]); arenaA.Put32(o + 0x42C, a[5]);
        call.ecx = self;
        break;
    case kSdd:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kVtBody:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kFontSet: {   // thiscall con un puntero de pila (`ret 4`); el contenido del origen y del destino se vuelve a rellenar con palabras aleatorias (los rellenos constantes no distinguirian la copia)
        FillWords(arenaA.At(o), 0x30, a[12] ^ 0xAAAAu);
        u32 src = self;
        if (!a[4]) {
            src = u32(arenaB.Obj(a[3]));
            FillWords(arenaB.At(a[3]), 0x30, a[13] ^ 0x5555u);
        }
        call.ecx = self;
        call.args[0] = src; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kStrip: {   // cdecl con un puntero (`ret`: lo limpia el llamador)
        if (!c.s.empty()) memcpy(arenaB.At(a[3]), c.s.data(), c.s.size());
        arenaB.Put8(a[3] + c.s.size(), 0);
        call.args[0] = u32(arenaB.Obj(a[3])); call.argCount = 1; call.popBytes = 4;
        break;
    }
    case kMapChar:
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    case kPoints: {
        arenaA.Put32(o, a[5]); arenaA.Put32(o + 4, a[6]); arenaA.Put32(o + 8, a[7]); arenaA.Put32(o + 0xC, a[8]);
        for (unsigned i = 0; i < 18; ++i) arenaB.Put32(a[9] + 4 * i, a[16 + i]);
        const u32 in = u32(arenaB.Obj(a[9])), out = a[4] ? in : u32(arenaC.Obj(a[10]));
        call.args = {out, a[3], self, in, 0, 0, 0, 0}; call.argCount = 4; call.popBytes = 16;
        break;
    }
    case kVecs: {
        for (unsigned i = 0; i < 18; ++i) arenaB.Put32(a[9] + 4 * i, a[16 + i]);
        const u32 in = u32(arenaB.Obj(a[9])), out = a[4] ? in : u32(arenaC.Obj(a[10]));
        call.args = {out, a[3], self, in, 0, 0, 0, 0}; call.argCount = 4; call.popBytes = 16;
        break;
    }
    case kCross:
        for (unsigned i = 0; i < 8; ++i) call.args[i] = a[3 + i];
        call.argCount = 8; call.popBytes = 32;
        break;
    case kHandCtor:
        arenaA.Put32(o + 0x2C, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kNode:   // thiscall con un puntero de salida oculto (`ret 4`)
        arenaA.Put16(o, u16(a[3])); arenaA.Put16(o + 2, u16(a[4])); arenaA.Put16(o + 4, u16(a[5]));
        call.ecx = self;
        call.args[0] = u32(arenaB.Obj(a[6])); call.argCount = 1; call.popBytes = 0;
        break;
    case kTrainSet: {   // thiscall con un argumento de pila (`ret 4`)
        FillWords(arenaA.At(o + kTrainNodes), kTrainNodeBytes, a[13] ^ 0x3333u);
        u32 clump = 0;
        if (a[4]) { clump = u32(arenaB.Obj(a[5])); arenaB.Put8(a[5], 2); }   // rpCLUMP = 2: el aserto de GetRpClump lee el tipo
        arenaA.Put32(o + kTrainClump, clump);
        gDtorWrite = a[7];
        call.ecx = self;
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kTrainPre:
        arenaA.Put16(o + kTrainFlags, u16(a[3])); arenaA.Put16(o + kTrainModel, u16(a[4]));
        arenaA.Put32(o + kTrainMatrix, a[5] ? u32(arenaB.Obj(a[7])) : a[6]);
        call.ecx = self;
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
        RegionsSnap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSdd: case kFontSet: case kHandCtor: case kNode: obs["ret"] = Tag(uintptr_t(eax)); break;   // this / la salida como etiqueta de arena
        case kMouse: case kMapChar: case kCross: obs["ret"] = eax & 0xFFu; break;                          // bool o byte: solo AL
        default: obs["ret"] = nullptr; break;                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN174A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN174A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador con hook S o V: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL (de un hook V, la entrada de su vtabla, que la DLL llama de forma no virtual); si no (dll = 0), la DLL llama a la
// direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
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
    if (!std::filesystem::exists(dir / "fn174a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn174a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn174a"); }
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
    // Colaborador con hook RH (S o V) en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
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
    // Colaborador SIN hook RH que la DLL llama por una funcion propia (direccion dada): el falso va en la direccion del EXE y en esa funcion (y se exige que de verdad no tenga hook).
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, void* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, fake);
    };
    // Colaborador enganchado FUERA del registro RH (HookInstall en InjectHooksMain): la entrada del EXE salta siempre a la funcion de la DLL; el falso se parchea sobre ese salto y en la funcion de la DLL.
    auto Ur = [&](uintptr_t exe, uintptr_t dllAddress, void* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false, true}, fake);
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (funciones de RenderWare y del CRT): el falso va solo en la direccion del EXE (y se exige que de verdad no tenga hook).
    auto X = [&](uintptr_t exe, void* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, fake);
    };
    const auto& K = kBodyFakes;
    using VoidDelete = void (*)(void*);
    const uintptr_t vehicleDelete = reinterpret_cast<uintptr_t>(static_cast<VoidDelete>(&CVehicle::operator delete));
    const uintptr_t padUpdate = reinterpret_cast<uintptr_t>(&CPad::UpdatePads);
    env.stubs[4] = {F(0x53D690, &FkStart), F(0x746870, &FkNoArg<0x746870>), Ur(0x541DD0, padUpdate, reinterpret_cast<void*>(&FkNoArg<0x541DD0>)), F(0x579330, &FkMessageScreen),
                    F(0x71A210, &FkNoArg<0x71A210>), F(0x53D840, &FkNoArg<0x53D840>), F(0x53FB70, &FkGetPad), F(0x619B60, &FkRsEvent)};
    env.stubs[5] = {F(0x6D3F30, &FkGuns), F(0x4AA3F0, &FkKill), F(0x4A9810, &FkDestroyFx), X(0x8213AE, reinterpret_cast<void*>(&FkDelete<0x8213AE>)), F(0x4FB8C0, &FkTerminate), F(0x6A61E0, K[5])};
    env.stubs[7] = {Rt(5, K[7]), U(0x6E2D90, vehicleDelete, reinterpret_cast<void*>(&FkDelete<0x6E2D90>))};
    env.stubs[8] = {F(0x6A61E0, K[8])};
    env.stubs[13] = {F(0x59B8B0, &FkCopyRw), X(0x7F18A0, reinterpret_cast<void*>(&FkRwUpdate)), X(0x7EDDC0, reinterpret_cast<void*>(&FkRwTransformVector))};
    env.stubs[15] = {F(0x6D0450, K[15])};
    env.stubs[17] = {F(0x4FB8C0, &FkTerminate), H(0x6E2B40, Lote174ABody_CTrainBaseDestructor(), K[17])};
    env.stubs[18] = {F(0x6D6A40, &FkSetModelIndex), F(0x4C5440, &FkFillFrame)};
    env.stubs[19] = {F(0x6D6480, &FkPreRender), F(0x6E1A60, &FkDoLights)};
    // DrawFonts (0x71A210) es en el EXE un `jmp` a RenderFontBuffer (0x719840, con hook en la DLL): se le da su ruta (hook apagado, codigo real del EXE) para que la entrada del falso resuelva a codigo del EXE
    {
        const Pair p = MakeCollab(env, 0x719840, hooksFound);
        if (!p.hooked || !p.dll) ready = false;
        env.reals[4] = {p};
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 1u, 2u, 3u, 6u, 9u, 10u, 11u, 12u, 14u, 16u}) dllReal[f] = "toda la funcion (sin colaboradores)";
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
    // Memoria global del juego que el banco toca: el estado del raton (nuevo y anterior), RsGlobal.quit y las palabras de Esc (nueva y anterior)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kMouseRegion, kMouseRegionBytes);   // 0
    regions.emplace_back(kAddrQuit, 4);                       // 1
    regions.emplace_back(kAddrEscNew, 2);                     // 2
    regions.emplace_back(kAddrEscOld, 2);                     // 3
    for (const unsigned f : {0u, 1u, 2u, 3u}) regionsOf[f] = {0};
    regionsOf[4] = {1, 2, 3};
    Run(env, ready, registered, bodies, collaborators);
}
}
