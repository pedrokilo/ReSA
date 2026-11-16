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

// Banco par a par Fn204A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 28 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) o V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en
// cuatro arenas propias (A, B, C y D con guardas de 0x40) y la memoria global que cada funcion usa (umbral 0xC18D48, pool de eventos 0xB744AC, reloj 0xB7CB84, byte de disturbios 0x96913F). Todas las funciones del lote
// tienen el hook APAGADO durante el banco salvo la del par (la candidata lo enciende). Falsos de los colaboradores con hook: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (F / Pair); los colaboradores
// REALES (aritmetica de vectores, constructores y destructores de eventos, constructor de CTaskSimpleGoToPoint, CEvent::operator delete) corren con su hook apagado. Se observan: los bytes cambiados de las cuatro arenas
// (con la vtable de un objeto recien construido canonizada), los cambios de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto y el estado x87 (siempre vacio).

namespace F204ALive {
using namespace F204AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F204AFix::Arena(WIN);
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


// Parametros del caso que leen los falsos
inline std::array<u32, 64> gA{};
inline u32 gPed{}, gTypeCalls{}, gHitCalls{}, gLoadedCalls{}, gBlendCalls{}, gLimitCalls{}, gRandCalls{}, gIsLooking{}, gAddRet{};
inline DWORD gBenchThread{};
inline std::array<void*, 4> gFakeFn{};

// ================================================================================================ falsos de los colaboradores
inline u32 Rd32(const void* p, u32 off = 0) { u32 v; std::memcpy(&v, static_cast<const u8*>(p) + off, 4); return v; }
// Cuerpo de destructor / constructor base (thiscall sin argumentos de pila): evento [direccion, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja a[41] en [ECX]; devuelve a[40]. Solo para objetos que viven en una arena.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    std::memcpy(self, &gA[41], 4);
    return gA[40];
}
// Cuerpo de destructor de un evento del pool (el objeto puede estar fuera de las arenas: no se toca): evento [direccion, ECX (etiqueta)]
template<u32 ADDR> inline u32 __fastcall FkEvBody(void* self, void*) { Ev& e = PushEv(ADDR, 1); e.v[0] = Tag(self); return 0; }
// CEntity::RegisterReference (0x571B70) y CleanUpOldReference (0x571A00) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), referencia (etiqueta)].
template<u32 ADDR> inline u32 __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ref);
    return 0;
}
// Ayuda para escribir eventos de longitud variable
struct EvW {
    Ev& e;
    explicit EvW(u32 kind) : e(PushEv(kind, 0)) {}
    void operator()(u32 v) { if (e.len < e.v.size()) e.v[e.len++] = v; }
};
inline u32 Resolve32(u32 v);

// --- CTaskComplexAvoidEntity (SetUpIK, QuitIK, ControlSubTask, Clone, CreateFirstSubTask)
inline u32 __cdecl FkGetPad(int arg) { EvW e(0x53FB70); e(u32(arg)); return Resolve32(gA[11]); }
inline u32 __cdecl FkFindPlayer(int arg) { EvW e(0x56E210); e(u32(arg)); return gA[4] == 0 ? gPed : (gA[4] == 1 ? Resolve32(gA[12]) : 0u); }
inline u32 __fastcall FkOnScreen(void* self, void*) { EvW e(0x534540); e(Tag(self)); return gA[5]; }
inline u32 __fastcall FkGetLookAt(void* self, void*, void* ped) { EvW e(0x6181D0); e(Tag(self)); e(Tag(ped)); return gA[6]; }
inline u32 __fastcall FkTaskSecondary(void* self, void*, int arg) { EvW e(0x681810); e(Tag(self)); e(u32(arg)); return gA[7]; }
// ranura 4 de la vtable falsa (GetTaskType del padre): la primera llamada devuelve a[8] y la segunda a[9]
inline u32 __fastcall FkTypeSlot(void* self, void*) { EvW e(0xD0000004); e(Tag(self)); return gTypeCalls++ == 0 ? gA[8] : gA[9]; }
// ranura 0 de la vtable falsa (destructor con borrado del evento interno de CEventGroupEvent): `ret 4`
inline void* __fastcall FkInnerDelete(void* self, void*, u32 flag) { EvW e(0xD0000000); e(Tag(self)); e(flag); return self; }
// IKChainManager_c::LookAt (0x618970; thiscall, 11 argumentos de pila, `ret 2Ch`): [direccion, ECX, hash del nombre, peaton, entidad, duracion, hueso, vector (3 dwords por el puntero), bool, 0.25f, 500, 3, bool]
inline u32 __fastcall FkLookAt(void* self, void*, const char* name, void* ped, void* entity, u32 dur, u32 bone, const u32* vec, u32 b0, u32 f025, u32 a8, u32 a9, u32 b1) {
    EvW e(0x618970);
    e(Tag(self)); e(FnvStr(name)); e(Tag(ped)); e(Tag(entity)); e(dur); e(bone); e(vec[0]); e(vec[1]); e(vec[2]); e(b0 & 0xFFu); e(f025); e(a8); e(a9); e(b1 & 0xFFu);
    return gA[13];
}
inline u32 __fastcall FkIsLooking(void* self, void*, void* ped) { EvW e(0x6181A0); e(Tag(self)); e(Tag(ped)); return gIsLooking; }
inline void __fastcall FkAbortLookAt(void* self, void*, void* ped, int ms) { EvW e(0x618280); e(Tag(self)); e(Tag(ped)); e(u32(ms)); }
inline u32 __cdecl FkTaskNew(u32 size) { EvW e(0x61A5A0); e(size); return Resolve32(gA[14]); }
inline void __cdecl FkTaskDelete(void* p) { EvW e(0x61A5B0); e(Tag(p)); }
inline u32 __fastcall FkAvCtor(void* self, void*, u32 a1, void* a2, void* a3) { EvW e(0x66AA20); e(Tag(self)); e(a1); e(Tag(a2)); e(Tag(a3)); return gA[16]; }
inline u32 __fastcall FkRouteRound(void* self, void*, void* ped) { EvW e(0x66FD30); e(Tag(self)); e(Tag(ped)); return gA[15]; }
inline u32 __fastcall FkClear(void* self, void*) { EvW e(0x5FF9D0); e(Tag(self)); return 0; }

// --- escaneres y eventos (0x606580 y 0x606BA0)
inline u32 __fastcall FkIsAlive(void* self, void*) { EvW e(0x5E0170); e(Tag(self)); return Rd32(self, 0x484); }
inline u32 __fastcall FkGetSimplest(void* self, void*) { EvW e(0x6819D0); e(Tag(self)); return Rd32(self, 0xFC); }
inline u32 __cdecl FkIsGoTo(void* task) { EvW e(0x61A360); e(Tag(task)); return Rd32(task, 0x1C); }
inline u32 __fastcall FkGetMoveState(void* self, void*) { EvW e(0x601D70); e(Tag(self)); return Rd32(self, 0x104); }
inline void* __fastcall FkSphereSet(void* self, void*, u32 radius, const u32* center, u32 surface, u32 piece, u32 lighting) {
    EvW e(0x40FD10);
    e(Tag(self)); e(radius); e(center[0]); e(center[1]); e(center[2]); e(surface); e(piece); e(lighting & 0xFFu);
    return self;
}
inline u32 __fastcall FkIntersect(void* self, void*, const u32* start, const u32* end, void* o1, void* o2) {
    EvW e(0x4100E0);
    e(Tag(self)); e(start[0]); e(start[1]); e(start[2]); e(end[0]); e(end[1]); e(end[2]); e(Tag(o1)); e(Tag(o2));
    const u32 k = gHitCalls++;
    return ((gA[20] >> (k % 32)) & 1) ? 0xAB01u : 0xAB00u;
}
// Campos de un evento ya construido (los que escriben tanto el constructor del EXE como el de la DLL): tipo (ranura 1 de la vtable), +4, +8, +0xC, +0xE, +0x10 y el resto segun el tipo
inline void LogEventFields(EvW& e, const u8* ev) {
    const u32 vt = Rd32(ev, 0);
    const int type = reinterpret_cast<int(__fastcall*)(const void*, void*)>(Rd32(reinterpret_cast<const void*>(uintptr_t(vt)), 4))(ev, nullptr);
    e(u32(type)); e(Rd32(ev, 4)); e(ev[8]); e(ev[0xC]); e(u32(*reinterpret_cast<const u16*>(ev + 0xE))); e(u32(*reinterpret_cast<const u16*>(ev + 0x10)));
    if (type == 13) { e(Rd32(ev, 0x14)); e(Rd32(ev, 0x18)); e(Rd32(ev, 0x1C)); e(Tag(uintptr_t(Rd32(ev, 0x20)))); e(Rd32(ev, 0x24)); }
    else { e(Tag(uintptr_t(Rd32(ev, 0x14)))); if (type == 63) { e(Rd32(ev, 0x18)); e(Rd32(ev, 0x1C)); e(Rd32(ev, 0x20)); e(Rd32(ev, 0x24)); } }
}
inline u32 __fastcall FkAdd(void* self, void*, const u8* ev, u32 flag) { EvW e(0x4AB420); e(Tag(self)); LogEventFields(e, ev); e(flag & 0xFFu); return gAddRet; }
inline u32 __fastcall FkAddEvent(void* self, void*, const u8* ev) { EvW e(0x5F7470); e(Tag(self)); LogEventFields(e, ev); return gA[24]; }
inline float __fastcall FkCanSee(void* self, void*, void* entity, u32 arg) { EvW e(0x605550); e(Tag(self)); e(Tag(entity)); e(arg); return std::bit_cast<float>(gA[20]); }
inline u32 __cdecl FkRiots() { EvW e(0x441C10); return gA[21]; }
inline u32 __cdecl FkCanJoin(void* ped, void* other) { EvW e(0x603AF0); e(Tag(ped)); e(Tag(other)); return gA[22]; }
inline u32 __cdecl FkGetPedsGroup(void* ped) { EvW e(0x5F7E80); e(Tag(ped)); return Resolve32(gA[23]); }

// --- CTaskInteriorSitInChair::ProcessPed
inline void __fastcall FkSetMoveState(void* self, void*, u32 st) { EvW e(0x5DEC00); e(Tag(self)); e(st); }
inline u32 __cdecl FkGetAssoc(void* clump, int id) { EvW e(0x4D68B0); e(Tag(clump)); e(u32(id)); return gA[5]; }
inline u32 __fastcall FkAnimsLoaded(void* self, void*, u32 arg) {
    EvW e(0x5980F0); e(Tag(self)); e(arg);
    const u32 k = gLoadedCalls++;
    return (k < 32 && ((gA[4] >> k) & 1)) ? 0xAB01u : 0xAB00u;
}
inline void __fastcall FkSetDeleteCb(void* self, void*, void* cb, void* data) {
    EvW e(0x4CEBC0);
    const bool canon = cb == reinterpret_cast<void*>(0x4D1390) || cb == reinterpret_cast<void*>(&CDefaultAnimCallback::DefaultAnimCB);
    e(Tag(self)); e(canon ? 0x4D1390u : u32(uintptr_t(cb))); e(Tag(data));
}
inline void __fastcall FkSetFinishCb(void* self, void*, void* cb, void* data) { EvW e(0x4CEBE0); e(Tag(self)); e(u32(uintptr_t(cb))); e(Tag(data)); }
inline u32 __cdecl FkBlend(void* clump, u32 group, u32 id, u32 delta) {
    EvW e(0x4D4610); e(Tag(clump)); e(group); e(id); e(delta);
    const u32 k = gBlendCalls++;
    return Resolve32(0xE0200000u | (0x100u * gA[6 + (k < 3 ? k : 2)]));
}
inline u32* __cdecl FkMatVec(u32* out, void* mat, void* vec) {
    EvW e(0x59C890); e(Tag(out)); e(Tag(mat)); e(Tag(vec));
    out[0] = gA[12]; out[1] = gA[13]; out[2] = gA[14];
    return out;
}
inline float __cdecl FkLimit(u32 bits) { EvW e(0x53CB50); e(bits); return std::bit_cast<float>(gLimitCalls++ == 0 ? gA[9] : gA[11]); }
inline void __fastcall FkSetRotZ(void* self, void*, u32 angle) { EvW e(0x59B020); e(Tag(self)); e(angle); }
inline void __fastcall FkSetPosn(void* self, void*, const u32* vec) { EvW e(0x4241C0); e(Tag(self)); e(vec[0]); e(vec[1]); e(vec[2]); }
inline u32 __fastcall FkIsOutOfTime(void* self, void*) { EvW e(0x420E30); e(Tag(self)); return gA[15]; }
inline float __cdecl FkRadianBetween(u32 a, u32 b, u32 c, u32 d) { EvW e(0x53CBE0); e(a); e(b); e(c); e(d); return std::bit_cast<float>(gA[10]); }
// rand() del CRT (0x821B1E en el EXE; CGeneral::GetRandomNumber en la DLL): evento [0x821B1E]; devuelve a[16] la primera vez y a[17] la segunda. El EXE corre su 0x407180 REAL y la DLL convierte en linea.
inline u32 __cdecl FkRand() { EvW e(0x821B1E); return gRandCalls++ == 0 ? gA[16] : gA[17]; }
inline u32 __fastcall FkPedsGroup(void* self, void*, void* ped) { EvW e(0x598240); e(Tag(self)); e(Tag(ped)); return Resolve32(gA[18]); }
inline void __fastcall FkFindClosest(void* self, void*, u32 a0, u32 vx, u32 vy, u32 vz, u32 rad, u32* info, u32* interior, u32* dist) {
    EvW e(0x594A50);
    e(Tag(self)); e(a0); e(vx); e(vy); e(vz); e(rad); e(Tag(info)); e(Tag(interior)); e(Tag(dist));
    *info = Resolve32(gA[19]); *interior = gA[20]; *dist = gA[21];
}

// Puntero simbolico (P(arena, desplazamiento) o FK(i)) -> direccion real; otro valor queda igual
inline u32 Resolve32(u32 v) {
    if (!IsSym(v)) return v;
    if ((v >> 20) == 0xE04u) return u32(uintptr_t(gFakeFn[v & 3]));
    return u32(gArenas[(v >> 20) & 3]->Obj(v & 0xFFFFF));
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
};

// Aplica el guion de la escena (arenas y memoria global) y devuelve la llamada
inline Call Setup(const C& c) {
    Call call;
    arenaA.Pattern(c.a[2], c.a[36]); arenaB.Pattern(c.a[2], c.a[37]); arenaC.Pattern(c.a[2], c.a[38]); arenaD.Pattern(c.a[2], c.a[39]);
    gA = c.a;
    gTypeCalls = gHitCalls = gLoadedCalls = gBlendCalls = gLimitCalls = gRandCalls = 0;
    gIsLooking = c.fn == kSitProcess ? c.a[22] : c.a[10];
    gAddRet = c.fn == kPedColl ? c.a[21] : c.a[25];
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
    gPed = c.nargs ? call.args[0] : 0u;
    return call;
}

// Forma del valor devuelto: 0 void, 1 puntero (etiqueta de arena), 2 bool (solo AL), 3 EAX completo, 4 (EAX, ECX, EDX)
inline constexpr std::array<u32, kFunctions> kRetShape{0, 0, 2, 3, 1, 1, 1, 1, 2, 1, 0, 0, 1, 0, 0, 2, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

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
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, false);
        });
        // vtable del objeto recien construido: se canoniza (EXE: la vtable esperada del EXE; DLL: una direccion del modulo ASI)
        if (c.vtArena < 4) {
            auto& ar = *gArenas[c.vtArena];
            const u32 v = Rd32(ar.m.data() + kGuard + c.vtOff);
            const bool changed = v != Rd32(ar.r.data() + kGuard + c.vtOff);   // solo si la funcion lo escribio
            const bool ok = changed && (candidate ? (v != 0 && ModuleOf(v) == env.asi) : (v == c.vtExpect));
            if (ok) { const u32 marker = 0xA5A5A5A5u; memcpy(ar.m.data() + kGuard + c.vtOff, &marker, 4); }
        }
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kRetShape[fn]) {
        case 1: obs["ret"] = Tag(uintptr_t(eax)); break;       // puntero como etiqueta de arena
        case 2: obs["ret"] = eax & 0xFFu; break;               // bool
        case 3: obs["ret"] = eax; break;
        case 4: obs["ret"] = json::array({eax, sides[s].reg.ecx, sides[s].reg.edx}); break;
        default: obs["ret"] = nullptr; break;                  // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN204A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN204A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn204a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn204a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn204a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    gFakeFn = {reinterpret_cast<void*>(&FkInnerDelete), reinterpret_cast<void*>(&FkTypeSlot), nullptr, nullptr};
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado (las 28 funciones del lote quedan apagadas durante todo el banco salvo la candidata de cada par).
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
    auto Cached = [&](uintptr_t exe) -> Pair& {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        Pair& p = Cached(exe);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL (hook apagado: el EXE corre sus bytes y la DLL su cuerpo o su codigo en linea)
    auto R = [&](uintptr_t exe) -> Pair { Pair p = Cached(exe); return p; };
    // Colaborador SIN hook RH cuya contraparte en la DLL es una funcion propia (rand): el falso va en la direccion del EXE y en esa funcion
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Funcion del propio lote (hook apagado durante el banco): se usa su ruta y su cuerpo en la DLL
    auto Lot = [&](unsigned idx) { return Pair{kEntries[idx], Body(env.route[idx]->Body()), true}; };

    const std::array<StubSpec, 8> av = {F(0x53FB70, &FkGetPad), F(0x56E210, &FkFindPlayer), F(0x534540, &FkOnScreen), F(0x6181D0, &FkGetLookAt), F(0x681810, &FkTaskSecondary), F(0x618970, &FkLookAt),
                                        F(0x6181A0, &FkIsLooking), F(0x618280, &FkAbortLookAt)};
    env.stubs[kSetUpIK].assign(av.begin(), av.end());
    env.stubs[kControl].assign(av.begin(), av.end());
    for (unsigned f : {unsigned(kQuitIK), unsigned(kMakeAbortable), unsigned(kCreateNext)}) env.stubs[f] = {F(0x6181A0, &FkIsLooking), F(0x618280, &FkAbortLookAt)};
    env.stubs[kClone] = {F(0x61A5A0, &FkTaskNew), F(0x66AA20, &FkAvCtor)};
    env.stubs[kCreateFirst] = {F(0x66FD30, &FkRouteRound), F(0x61A5A0, &FkTaskNew), F(0x6181A0, &FkIsLooking), F(0x618280, &FkAbortLookAt)};
    env.stubs[kSitSdd] = {F(0x675C90, &FkBody<0x675C90>), F(0x61A5B0, &FkTaskDelete)};
    env.stubs[kLieSdd] = {F(0x675E90, &FkBody<0x675E90>), F(0x61A5B0, &FkTaskDelete)};
    for (unsigned f : {unsigned(kVehScDtor), unsigned(kPedScDtor), unsigned(kObjScDtor)}) env.stubs[f] = {F(0x5FF9D0, &FkClear)};
    env.stubs[kSitProcess] = {F(0x5DEC00, &FkSetMoveState), F(0x4D68B0, &FkGetAssoc), F(0x5980F0, &FkAnimsLoaded), F(0x4CEBC0, &FkSetDeleteCb), F(0x4D4610, &FkBlend), F(0x4CEBE0, &FkSetFinishCb),
                              F(0x59C890, &FkMatVec), F(0x53CB50, &FkLimit), F(0x59B020, &FkSetRotZ), F(0x4241C0, &FkSetPosn), F(0x420E30, &FkIsOutOfTime), F(0x53CBE0, &FkRadianBetween),
                              F(0x6181A0, &FkIsLooking), F(0x618280, &FkAbortLookAt), D(0x821B1E, std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), &FkRand), F(0x598240, &FkPedsGroup), F(0x594A50, &FkFindClosest), F(0x618970, &FkLookAt)};
    env.stubs[kPedColl] = {F(0x5E0170, &FkIsAlive), F(0x6819D0, &FkGetSimplest), F(0x61A360, &FkIsGoTo), F(0x601D70, &FkGetMoveState), F(0x40FD10, &FkSphereSet), F(0x4100E0, &FkIntersect),
                           F(0x4AB420, &FkAdd), F(0x571B70, &FkRef<0x571B70>), F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[kAcqEvent] = {F(0x4AB420, &FkAdd), F(0x5F7470, &FkAddEvent), F(0x605550, &FkCanSee), F(0x441C10, &FkRiots), F(0x603AF0, &FkCanJoin), F(0x5F7E80, &FkGetPedsGroup),
                            F(0x571B70, &FkRef<0x571B70>), F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[kRunOver] = {F(0x5E0170, &FkIsAlive)};
    env.stubs[kEvSdd0 + 0] = {F(0x4ADF70, &FkEvBody<0x4ADF70>)};
    env.stubs[kEvSdd0 + 1] = {F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[kEvSdd0 + 2] = {F(0x4AE270, &FkEvBody<0x4AE270>)};
    env.stubs[kEvSdd0 + 3] = {F(0x4AE3C0, &FkEvBody<0x4AE3C0>)};
    env.stubs[kEvSdd0 + 4] = {F(0x4AE670, &FkEvBody<0x4AE670>)};
    env.stubs[kEvSdd0 + 5] = {F(0x4AE7A0, &FkEvBody<0x4AE7A0>)};
    env.stubs[kEvSdd0 + 6] = {F(0x4AEA50, &FkEvBody<0x4AEA50>)};
    env.stubs[kEvSdd0 + 7] = {F(0x4AEB90, &FkEvBody<0x4AEB90>)};
    env.stubs[kEvSdd0 + 8] = {F(0x4AED60, &FkEvBody<0x4AED60>)};
    env.stubs[kEvSdd0 + 9] = {F(0x4AEEA0, &FkEvBody<0x4AEEA0>)};
    // colaboradores REALES (hook apagado)
    env.reals[kSetUpIK] = {R(0x40FE60), R(0x406DA0), R(0x59C910)};
    env.reals[kControl] = {Lot(kSetUpIK), R(0x40FE60), R(0x406DA0), R(0x59C910)};
    env.reals[kCreateFirst] = {R(0x667CD0), R(0x61A390)};
    env.reals[kSitProcess] = {R(0x407180)};
    env.reals[kPedColl] = {R(0x40FE00), R(0x406DA0), R(0x40FDB0), R(0x40FEC0), R(0x4EEA80), R(0x4AE6E0), R(0x4AE7A0)};
    env.reals[kAcqEvent] = {R(0x4AF820), R(0x4AF890), R(0x420E70)};
    for (unsigned f = kEvSdd0; f < kFunctions; ++f) env.reals[f] = {R(0x4B5630)};
    env.reals[kEvSdd0 + 1].push_back(R(0x4AE070));
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores sustituidos (con hook: falsos identicos en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kSetUpIK] = "toda la funcion y su aritmetica (VectorSubtract 0x40FE60, SquaredMagnitude 0x406DA0 y Normalise 0x59C910 reales); GetTaskType del padre por una vtable falsa de la arena C";
    dllReal[kControl] = "toda la funcion; SetUpIK (0x66AB40, del lote, hook apagado) real y su aritmetica real";
    dllReal[kCreateFirst] = "toda la funcion; el constructor de CTaskSimpleGoToPoint (0x667CD0 y 0x61A390) es real (la vtable del objeto recien construido se canoniza)";
    dllReal[kPedColl] = "toda la funcion y su aritmetica (0x40FE00, 0x406DA0, 0x40FDB0, 0x40FEC0 y 0x4EEA80 reales); constructor y destructor de CEventPotentialWalkIntoPed (0x4AE6E0 y 0x4AE7A0) reales";
    dllReal[kAcqEvent] = "toda la funcion; constructores y destructor de los eventos de conocido (0x4AF820, 0x420E70 y 0x4AF890) reales; el tipo de cada evento sale de su vtable (ranura 1)";
    for (unsigned f = kEvSdd0; f < kFunctions; ++f) dllReal[f] = "toda la funcion; el EXE libera la ranura en linea y la DLL con CEvent::operator delete (0x4B5630, real, hook apagado); cuerpo del destructor falso";
    dllReal[kEvSdd0 + 1] = "toda la funcion; cuerpo del destructor (0x4AE070) real en el EXE y el destructor C++ en la DLL; CleanUpOldReference falso; el evento interno se borra por una vtable falsa";
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
    regions.emplace_back(kThr, 4);
    regionsOf[kSetUpIK] = regionsOf[kControl] = {regions.size() - 1};
    regions.emplace_back(kTimerMs, 4);
    regionsOf[kSitProcess] = {regions.size() - 1};
    regionsOf[kAcqEvent].push_back(regions.size() - 1);
    regions.emplace_back(kRiotByte & ~3u, 4);
    regionsOf[kAcqEvent].push_back(regions.size() - 1);
    regions.emplace_back(kEvPoolPtr, 4);
    for (unsigned f = kEvSdd0; f < kFunctions; ++f) regionsOf[f] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
