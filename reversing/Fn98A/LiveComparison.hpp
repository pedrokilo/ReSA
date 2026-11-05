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
#include "Entity/Placeable.h"
#include "PathFind.h"
#include "Models/VehicleModelInfo.h"
#include "ScriptsForBrains.h"
#include "Scripts/TheScripts.h"
#include "RenderWare/rw/skeleton.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn98A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Seis funciones (hook S), cada una con 16384 pares EXE/DLL:
//   0  FloatMax                                       0x420800  cdecl (a, b), devuelve el mayor en ST0 (a si a > b ordenado, si no b); sin colaboradores.
//   1  CCarPathLink::GetNodeCoors (a CVector)         0x420A60  thiscall, enlace en ECX, destino en pila, `ret 4`; escribe (x * 0.125, y * 0.125, 0) y devuelve el destino en EAX; sin colaboradores.
//   2  CVehicleModelInfo::IncrementTimesUsed          0x421120  thiscall, info de modelo en ECX; byte +0x50 = min(int8 + 1, 0x78); sin colaboradores.
//   3  CPlaceable::GetForwardVector                   0x41CCB0  thiscall, placeable en ECX, destino en pila, `ret 4`; devuelve el destino en EAX; sin colaboradores (fsin/fcos del x87 corren de verdad en ambos lados).
//   4  CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName 0x46CED0 thiscall (nombre, entidad, tipo), `ret 0Ch`; void. Colaboradores (con hook RH): 0x46AA30 (thiscall, `ret 8`, devuelve AX) y 0x46CD80 (thiscall, `ret 10h`).
//   5  CTheScripts::IsPointWithinSearchLight          0x493280  cdecl (&posicion, indice), bool en AL. Colaborador (con hook RH): CVector::NormaliseAndMag 0x59C970 (thiscall, in situ, devuelve la magnitud en ST0).
//   6  WinInput::diPadEnumerate                       0x7477C0  cdecl (), int en EAX. psGlobal (0xC17054) apunta a una arena; la interfaz DirectInput es otra arena con vtabla propia (ranura 4 = EnumDevices, stdcall, ret 14h, falsa);
//      colaborador SIN hook RH en esta configuracion (SDL3): diPadSetRanges 0x746D80 (cdecl, (mando, numero)), falso en la direccion del EXE (la DLL lo llama por esa direccion). La funcion de retorno de EnumDevices (0x747020) se pasa como dato (el EXE y la DLL pasan 0x747020).
//   7  CCarCtrl::WeaveForPed                          0x426970  cdecl (entidad, vehiculo, &limite1, &limite2); void. Colaboradores (con hook RH): CWorld::FindPlayerPed 0x56E210 y CGeneral::GetATanOfXY 0x53CC70 (cdecl, (x, y), devuelve el angulo en ST0).
//      El vehiculo es una arena de 0x430 bytes (mision +0x3BA, objetivo +0x41C, modelo +0x22) y el indice de modelo apunta a una informacion de modelo falsa (arena) con una caja de colision falsa (arena).
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores tienen hook RH, apagado durante el banco). Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.
// NO se sustituyen (corren de verdad en ambos lados): toda la aritmetica de las funciones (x87 del EXE frente al C++ de la DLL).
// Memoria real del juego (se rellena y se restaura tras cada lado): el registro de foco de los scripts (0xA94D68 + indice * 0x7C).

namespace F98ALayout {
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CPlaceable, m_placement) == 0x4 && offsetof(CSimpleTransform, m_fHeading) == 0xC, "CPlaceable");
VALIDATE_OFFSET(CVehicleModelInfo, m_nTimesUsed, 0x50);
static_assert(offsetof(tScriptSearchlight, m_TargetSpot) == 0x58 && offsetof(tScriptSearchlight, vf64) == 0x64 && offsetof(tScriptSearchlight, vf70) == 0x70 && sizeof(tScriptSearchlight) == 0x7C, "tScriptSearchlight");
}

namespace F98ALive {
using namespace F98AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x420800, 0x420A60, 0x421120, 0x41CCB0, 0x46CED0, 0x493280, 0x7477C0, 0x426970};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F98AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kMiWin> objA;           // 0: enlace de camino, info de modelo, placeable o instancia de CScriptsForBrains (segun la funcion)
inline Arena<kOutWin> outA;          // 1: CVector de destino
inline Arena<kMatWin> matA;          // 2: matriz del placeable
inline Arena<kStrWin> strA;          // 3: nombre (funcion 4)
inline Arena<kEntWin> entA;          // 4: entidad (funcion 4)
inline Arena<kPosWin> posA;          // 5: posicion (funcion 5)
inline Arena<kVehWin> vehA;          // 6: vehiculo (funcion 7)
inline Arena<kMatWin> vmatA;         // 7: matriz del vehiculo (funcion 7)
inline Arena<kSmallWin2> miA;        // 8: informacion de modelo falsa (funcion 7)
inline Arena<kSmallWin2> colA;       // 9: caja de colision falsa (funcion 7)
inline constexpr u32 kRawPtr = 0x00C0FFEEu;

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned normCalls{}; };
inline std::array<uintptr_t, 16> ifaceVt{};
inline State g;
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}

// Eventos: 50 0x46AA30 [this, nombre, tipo]; 51 0x46CD80 [this, indice (dword completo), entidad, tipo, bandera]; 60 CVector::NormaliseAndMag 0x59C970 [&vector, x, y, z].
inline u32 __fastcall FkGetIndex(void* self, void*, void* name, u32 type) {                            // 0x46AA30 (`ret 8`; devuelve AX)
    Ev& e = PushEv(50, 3); e.v[0] = Tag(self); e.v[1] = Tag(name); e.v[2] = type;
    const u32 idx = g.c->a[1] & 0xFFFFu;
    return (idx & 0x8000u) ? ((g.c->a[4] << 16) | idx) : idx;
}
inline void __fastcall FkStartReq(void* self, void*, u32 idx, void* entity, u32 type, u32 flag) {      // 0x46CD80 (`ret 10h`)
    Ev& e = PushEv(51, 5); e.v[0] = Tag(self); e.v[1] = idx; e.v[2] = Tag(entity); e.v[3] = type; e.v[4] = flag;
}
inline float __fastcall FkNormMag(void* self, void*) {                                                  // 0x59C970 (in situ; devuelve la magnitud en ST0)
    Ev& e = PushEv(60, 4); e.v[0] = TagS(uintptr_t(self));
    const unsigned k = g.normCalls++;
    for (unsigned j = 0; j < 3; ++j) { memcpy(&e.v[1 + j], static_cast<uint8*>(self) + 4 * j, 4); memcpy(static_cast<uint8*>(self) + 4 * j, &g.c->a[6 + 4 * k + j], 4); }
    return F(g.c->a[9 + 4 * k]);
}

// 70 IDirectInput8::EnumDevices [this, clase, funcion de retorno, dato, banderas]; 71 0x746D80 [mando, numero];
// 30 CWorld::FindPlayerPed 0x56E210 [id]; 80 CGeneral::GetATanOfXY 0x53CC70 [x, y]; 90+k: ranura k no esperada de la vtabla de la interfaz.
template <unsigned K> inline u32 __stdcall FkIfaceUnexpected(void* self) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline long __stdcall FkEnumDevices(void* self, u32 devClass, void* cb, void* ref, u32 flags) {          // ranura 4 de la vtabla de la interfaz (ret 14h)
    Ev& e = PushEv(70, 5); e.v[0] = Tag(self); e.v[1] = devClass;
    e.v[2] = u32(uintptr_t(cb));   // 0x747020: la funcion de retorno es siempre la del EXE (la DLL la pasa por su direccion)
    e.v[3] = Tag(ref); e.v[4] = flags;
    return long(g.c->a[0]);
}
inline void InitIfaceVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((ifaceVt[I] = uintptr_t(&FkIfaceUnexpected<I>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    ifaceVt[4] = uintptr_t(&FkEnumDevices);
}
inline long __cdecl FkSetRanges(void* dev, u32 pad) {                                                    // 0x746D80
    Ev& e = PushEv(71, 2); e.v[0] = Tag(dev); e.v[1] = pad;
    return 0;
}
inline u32 __cdecl FkFindPlayer(u32 id) {                                                                // 0x56E210
    Ev& e = PushEv(30, 1); e.v[0] = id;
    return g.c->a[15] == 0 ? u32(objA.Obj()) : (g.c->a[15] == 1 ? 0u : 0x00ABCD00u);
}
inline float __cdecl FkAtan(u32 x, u32 y) {                                                              // 0x53CC70 (devuelve el angulo en ST0)
    Ev& e = PushEv(80, 2); e.v[0] = x; e.v[1] = y;
    return F(g.c->a[17]);
}

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

inline uintptr_t PtrKind(u32 kind, uintptr_t arena) { return kind == 0 ? arena : (kind == 1 ? 0u : kRawPtr); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Fill(sd++); outA.Fill(sd++); matA.Fill(sd++); strA.Fill(sd++); entA.Fill(sd++); posA.Fill(sd++); vehA.Fill(sd++); vmatA.Fill(sd++); miA.Fill(sd++); colA.Fill(sd++);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());       // 0
        TagAdd(outA.m.data(), outA.m.size());       // 1
        TagAdd(matA.m.data(), matA.m.size());       // 2
        TagAdd(strA.m.data(), strA.m.size());       // 3
        TagAdd(entA.m.data(), entA.m.size());       // 4
        TagAdd(posA.m.data(), posA.m.size());       // 5
        TagAdd(vehA.m.data(), vehA.m.size());       // 6
        TagAdd(vmatA.m.data(), vmatA.m.size());     // 7
        TagAdd(miA.m.data(), miA.m.size());         // 8
        TagAdd(colA.m.data(), colA.m.size());       // 9
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        switch (fn) {
        case 0:   // FloatMax(a, b), cdecl
            args = { c.a[0], c.a[1] };
            popBytes = 8;
            break;
        case 1:   // CCarPathLink::GetNodeCoors(&out)
            objA.Put16(0, c.a[0]); objA.Put16(2, c.a[1]);
            outA.Put32(0, c.a[2]); outA.Put32(4, c.a[3]); outA.Put32(8, c.a[4]);
            ecx = u32(objA.Obj());
            args = { u32(outA.Obj()) };
            break;
        case 2:   // CVehicleModelInfo::IncrementTimesUsed()
            objA.Put8(0x50, c.a[0]);
            ecx = u32(objA.Obj());
            break;
        case 3:   // CPlaceable::GetForwardVector(&out)
            objA.Put32(0x10, c.a[1]);
            objA.Put32(0x14, c.a[0] ? u32(matA.Obj()) : 0u);
            matA.Put32(0x10, c.a[2]); matA.Put32(0x14, c.a[3]); matA.Put32(0x18, c.a[4]);
            outA.Put32(0, c.a[5]); outA.Put32(4, c.a[6]); outA.Put32(8, c.a[7]);
            ecx = u32(objA.Obj());
            args = { u32(outA.Obj()) };
            break;
        case 4:   // CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName(name, entity, type)
            ecx = u32(objA.Obj());
            args = { u32(PtrKind(c.a[2], strA.Obj())), u32(PtrKind(c.a[3], entA.Obj())), c.a[0] };
            break;
        case 6: {   // WinInput::diPadEnumerate(): psGlobal -> objA (+0x18 interfaz = strA con vtabla propia, +0x20 y +0x24 mandos)
            InitIfaceVt();
            strA.Put32(0, u32(Addr(ifaceVt.data())));
            objA.Put32(0x18, u32(strA.Obj()));
            objA.Put32(0x20, c.a[1] == 0 ? 0u : (c.a[1] == 1 ? u32(entA.Obj()) : 0x00C0FFEEu));
            objA.Put32(0x24, c.a[2] == 0 ? 0u : (c.a[2] == 1 ? u32(posA.Obj()) : 0x00C0FFEFu));
            G32(kPsGlobalCell) = u32(objA.Obj());
            break;
        }
        case 7: {   // CCarCtrl::WeaveForPed(entity, vehicle, &limit1, &limit2): peaton = objA (+0x14 matriz, +4 posicion), vehiculo = vehA
            const u32 mat = u32(matA.Obj()), vmat = u32(vmatA.Obj());
            objA.Put32(0x14, c.a[1] ? mat : 0u);
            if (c.a[1]) { matA.Put32(0x30, c.a[2]); matA.Put32(0x34, c.a[3]); matA.Put32(0x38, c.a[4]); objA.Put32(4, c.a[5]); objA.Put32(8, c.a[6]); objA.Put32(0xC, c.a[7]); }
            else { objA.Put32(4, c.a[2]); objA.Put32(8, c.a[3]); objA.Put32(0xC, c.a[4]); matA.Put32(0x30, c.a[5]); matA.Put32(0x34, c.a[6]); matA.Put32(0x38, c.a[7]); }
            vehA.Put32(0x14, c.a[8] ? vmat : 0u);
            if (c.a[8]) { vmatA.Put32(0x30, c.a[9]); vmatA.Put32(0x34, c.a[10]); vmatA.Put32(0x38, c.a[11]); vehA.Put32(4, c.a[12]); vehA.Put32(8, c.a[13]); vehA.Put32(0xC, c.a[14]); }
            else { vehA.Put32(4, c.a[9]); vehA.Put32(8, c.a[10]); vehA.Put32(0xC, c.a[11]); vmatA.Put32(0x30, c.a[12]); vmatA.Put32(0x34, c.a[13]); vmatA.Put32(0x38, c.a[14]); }
            vehA.Put16(0x22, c.a[21]);
            vehA.Put8(0x3BA, c.a[0]);
            vehA.Put32(0x41C, c.a[16] == 0 ? u32(objA.Obj()) : (c.a[16] == 1 ? 0u : 0x00ABCD01u));
            miA.Put32(0x14, u32(colA.Obj()));
            colA.Put32(0xC, c.a[18]);
            for (unsigned k = 0; k < 8; ++k) G32(kModelSlotsAddr + 4 * k) = u32(miA.Obj());   // los ocho indices de modelo 1500..1507 apuntan a la informacion falsa
            outA.Put32(0, c.a[19]); outA.Put32(4, c.a[20]);
            args = { u32(objA.Obj()), u32(vehA.Obj()), u32(outA.Obj(0)), u32(outA.Obj(4)) };
            popBytes = 16;
            break;
        }
        default: { // CTheScripts::IsPointWithinSearchLight(&pos, index)
            const uintptr_t slot = kSearchLightAddr + uintptr_t(c.a[20]) * 0x7C;
            for (unsigned k = 0; k < 3; ++k) {
                memcpy(reinterpret_cast<void*>(slot + 0x58 + 4 * k), &c.a[0 + k], 4);
                memcpy(reinterpret_cast<void*>(slot + 0x64 + 4 * k), &c.a[14 + k], 4);
                memcpy(reinterpret_cast<void*>(slot + 0x70 + 4 * k), &c.a[17 + k], 4);
            }
            posA.Put32(0, c.a[3]); posA.Put32(4, c.a[4]); posA.Put32(8, c.a[5]);
            args = { u32(posA.Obj()), c.a[20] };
            popBytes = 8;
            break;
        }
        }
        objA.Snap(); outA.Snap(); matA.Snap(); strA.Snap(); entA.Snap(); posA.Snap(); vehA.Snap(); vmatA.Snap(); miA.Snap(); colA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, outA.Changes()); add(2, matA.Changes()); add(3, strA.Changes()); add(4, entA.Changes()); add(5, posA.Changes()); add(6, vehA.Changes()); add(7, vmatA.Changes()); add(8, miA.Changes()); add(9, colA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        // valor devuelto: puntero al destino (funciones 1 y 3) o AL (funcion 5); las demas no devuelven nada que lea un llamador
        if (fn == 1 || fn == 3) obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
        else if (fn == 5) obs["ret"] = sides[s].reg.eax & 0xFFu;
        else if (fn == 6) obs["ret"] = sides[s].reg.eax;
        else obs["ret"] = nullptr;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& s : env.stubs[f]) {
            if (s.pair.hooked) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
            else ready &= s.pair.dll == 0 || ModuleOf(s.pair.dll) == env.asi;
        }
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
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
    if (!std::filesystem::exists(dir / "fn98a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn98a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn98a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {};
    env.stubs[1] = {};
    env.stubs[2] = {};
    env.stubs[3] = {};
    env.stubs[4] = {F(0x46AA30, &FkGetIndex), F(0x46CD80, &FkStartReq)};
    env.stubs[5] = {F(0x59C970, &FkNormMag)};
    env.stubs[6] = {F(0x746D80, &FkSetRanges)};
    env.stubs[7] = {F(0x56E210, &FkFindPlayer), F(0x53CC70, &FkAtan)};
    ready &= uintptr_t(&RsGlobal.ps) == kPsGlobalCell;
    const char* dllReal[kFunctions] = {
        "toda la funcion (comparacion ordenada y seleccion del segundo operando con NaN); el valor sale en ST0",
        "toda la funcion (productos exactos por 0.125 de los dos enteros de 16 bits, z = 0, destino devuelto en EAX)",
        "toda la funcion (byte con signo, incremento y tope 0x78)",
        "toda la funcion: copia en crudo de la fila adelante de la matriz o fsin/fcos del x87 del rumbo (sin matriz), guardado en float",
        "las dos llamadas son falsos identicos; la logica es la condicion del indice (int16 con signo) y el paso de los argumentos",
        "toda la aritmetica x87 (diferencias, productos, sumas, divisiones y la comparacion con 1.0); el falso de 0x59C970 deja los vectores y las magnitudes del caso",
        "toda la funcion: la llamada virtual de la interfaz es un falso (vtabla propia) y diPadSetRanges un falso identico; la logica es el resultado, la lectura repetida de psGlobal y las dos comprobaciones de mando",
        "toda la aritmetica x87 (distancia, ancho del modelo, margen, normalizacion de los dos limites y recorte a [-pi, pi]); el peaton y el vehiculo son arenas, la informacion de modelo es falsa; los falsos son 0x56E210 y 0x53CC70"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
