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

// Banco par a par Fn135A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Funciones del EXE de formas variadas (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedGlobalInstall: enlace de la entrada del EXE hacia la DLL). El banco invoca la direccion
// del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, donde hace falta, memoria global real del juego (se guarda al empezar y se restaura tras cada lado).
// Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair); las funciones de kernel32 (getDvdGamePath) se sustituyen en la casilla de la IAT del
// EXE y en la de la DLL; operator new[] / delete[] de la CRT del EXE (0x821195, 0x8213AE) llevan el falso solo en la direccion del EXE (la DLL los llama por esa direccion).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto y el estado x87.

namespace F135ALive {
using namespace F135AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x53F2D0, 0x747300, 0x50D590};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F135AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC;

// Regiones de memoria global del juego que el banco toca (indices fijos; ver regionsOf)
enum : unsigned { kRegPsPtr, kRegMenu, kRegModels, kRegActiveCam, kRegCams, kRegNearPtr };
inline constexpr uintptr_t kPsPtrAt = 0xC17054, kMenuAt = 0xBA67A4, kModelsAt = 0xA9B0C0, kModelsBase = 0xA9B0C8, kActiveCamAt = 0xB6F081, kCamsAt = 0xB6F19C, kNearPtrAt = 0xC1703C;
inline constexpr size_t kModelsBytes = 0x1010, kCamsBytes = 3 * 0x238;

// ================================================================================================ falsos de los colaboradores
inline const C* gC{};
inline u32 gRet{};                   // EAX que devuelven los falsos
inline DWORD gBenchThread{};
alignas(16) inline std::array<u8, 0x40> gPs{};      // psGlobal falso (fuera de las arenas: lleva el puntero del dispositivo en +0x1C)
alignas(16) inline std::array<u8, 0x100> gRwCamera{};   // RwCamera falsa: el plano cercano en +0x80
inline u32 gMouseAfter{}, gButtons{}, gAxes{}, gHr{};
inline unsigned gTypeIdx{}, gVolIdx{}, gNewIdx{};

// Lee un dword solo si el puntero cae dentro de una arena registrada
inline u32 SafeBits(const void* p) {
    for (const auto& t : tagTable) if (uintptr_t(p) >= t.base && uintptr_t(p) - t.base + 4 <= t.size) return BitsOf(p);
    return 0xDEAD0000u;
}
inline u32 StrLen(const char* s) { return s ? u32(strlen(s)) : 0u; }

// ---- GetMouseSetUp
// IsVideoModeExclusive (0x745CA0, cdecl sin argumentos, AL): evento [direccion]; devuelve EAX del caso
inline u32 __cdecl FkVideoExclusive() { PushEv(0x745CA0, 0); return gRet; }
// WinInput::diMouseInit (0x7469A0, cdecl, un dword; el EXE empuja el dword con basura en los bytes altos: solo se mira el byte bajo): evento [direccion, byte bajo, dispositivo actual (etiqueta)];
// deja en psGlobal+0x1C el dispositivo del caso
inline void __cdecl FkDiMouseInit(u32 arg) {
    Ev& e = PushEv(0x7469A0, 2);
    e.v[0] = arg & 0xFFu; e.v[1] = Tag(uintptr_t(BitsOf(gPs.data() + 0x1C)));
    memcpy(gPs.data() + 0x1C, &gMouseAfter, 4);
}
// IDirectInputDevice8A::GetCapabilities (ranura 3 de la vtable del dispositivo falso; stdcall (this, DIDEVCAPS*), `ret 8`): evento [0xD1C0, this (etiqueta), dwSize, hash de los 40 bytes restantes];
// escribe dwAxes (+0xC) y dwButtons (+0x10) y devuelve el HRESULT del caso
inline u32 __stdcall FkGetCaps(void* self, u8* caps) {
    Ev& e = PushEv(0xD1C0, 3);
    e.v[0] = Tag(self); e.v[1] = BitsOf(caps); e.v[2] = Fnv(caps + 4, 40);
    memcpy(caps + 0xC, &gAxes, 4); memcpy(caps + 0x10, &gButtons, 4);
    return gHr;
}

// ---- getDvdGamePath
inline u32 __stdcall FkSetErrorMode(u32 mode) { Ev& e = PushEv(0x85813C, 1); e.v[0] = mode; return gC->a[5]; }
inline u32 __stdcall FkGetLogicalDriveStrings(u32 n, char* buf) {
    Ev& e = PushEv(0x858138, 2); e.v[0] = n; e.v[1] = Tag(buf);
    if (!buf) return gC->a[0];
    memcpy(buf, gC->s.data(), gC->s.size());
    return gC->a[3];
}
inline u32 __stdcall FkGetDriveType(const char* path) {
    Ev& e = PushEv(0x858134, 2); e.v[0] = FnvStr(path); e.v[1] = StrLen(path);
    const unsigned k = gTypeIdx++;
    return u32(gC->t[k % 16]) | ((k & 1) ? gC->a[8] : 0u);
}
inline u32 __stdcall FkGetVolumeInformation(const char* root, char* vol, u32 size, u32 serial, u32 maxComp, u32 flags, u32 fsName, u32 fsSize) {
    Ev& e = PushEv(0x858130, 9);
    e.v[0] = FnvStr(root); e.v[1] = StrLen(root); e.v[2] = vol ? 1u : 0u; e.v[3] = size; e.v[4] = serial; e.v[5] = maxComp; e.v[6] = flags; e.v[7] = fsName; e.v[8] = fsSize;
    const unsigned k = gVolIdx++;
    if (!gC->t[16 + k % 16]) return 0;
    static constexpr const char* names[8] = {"GTA_SAN_ANDREAS", "GTA_SAN_ANDREAS\0JUNKJUNK", "GTA_SAN_ANDREA", "GTA_SAN_ANDREASX", "gta_san_andreas", "", "GTA_SAN_ANDREAS ", "GTA"};
    static constexpr size_t lens[8] = {16, 24, 15, 17, 16, 1, 17, 4};
    const unsigned sel = gC->t[32 + k % 16] % 8;
    memcpy(vol, names[sel], lens[sel]);
    return gC->a[9];
}
// operator new[] del EXE (0x821195, cdecl, tamano): evento [direccion, tamano]; devuelve el bloque de unidades (arena B) la primera vez y el del resultado (arena C) despues
inline void* __cdecl FkNewArray(u32 size) {
    Ev& e = PushEv(0x821195, 1); e.v[0] = size;
    return reinterpret_cast<void*>(gNewIdx++ == 0 ? arenaB.Obj(gC->a[6]) : arenaC.Obj(gC->a[7]));
}
// operator delete[] / free del EXE (0x8213AE, cdecl, puntero): evento [direccion, puntero (etiqueta), dword de [puntero] si cae en una arena]
inline void __cdecl FkDeleteArray(void* p) { Ev& e = PushEv(0x8213AE, 2); e.v[0] = Tag(p); e.v[1] = SafeBits(p); }

// ---- GetNearestDistanceOfPedSphereToCameraNearClip
// CPedModelInfo::AnimatePedColModelSkinnedWorld (0x4C7170, thiscall, clump en la pila, `ret 4`): evento [direccion, ECX (etiqueta), clump (crudo)]; escribe las 12 esferas (centro y radio) y el plano
// cercano posteriores del caso (la funcion los lee DESPUES de esta llamada) y devuelve EAX del caso. Las esferas se localizan con los desplazamientos del EXE: modelo+0x34, colision+0x2C, datos+8
inline u32 __fastcall FkAnimate(void* self, void*, void* clump) {
    Ev& e = PushEv(0x4C7170, 2); e.v[0] = Tag(self); e.v[1] = u32(uintptr_t(clump));
    const u32 colModel = BitsOf(reinterpret_cast<u8*>(self) + 0x34);
    const u32 colData = BitsOf(reinterpret_cast<u8*>(uintptr_t(colModel)) + 0x2C);
    const u32 spheres = BitsOf(reinterpret_cast<u8*>(uintptr_t(colData)) + 8);
    for (u32 i = 0; i < 12; ++i) memcpy(reinterpret_cast<u8*>(uintptr_t(spheres)) + i * 0x14, gC->u.data() + i * 16, 16);
    memcpy(gRwCamera.data() + 0x80, &gC->a[10], 4);
    return gRet;
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
    std::array<u32, 4> args{};
    u32 argCount{}, popBytes{}, ecx{}, zeroStack{};
};

inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline u32 DeviceOffset(const C& c) { return 0x40 + 4 * (Rand(c.a[14], 1) % 0x100); }
inline u32 VtableOffset(const C& c) { return 0x600 + 4 * (Rand(c.a[14], 2) % 0x100); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    gC = &c; gTypeIdx = gVolIdx = gNewIdx = 0;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    switch (c.fn) {
    case 0: {   // GetMouseSetUp: el resultado por valor va a la arena A (destino por la pila); el EXE lee basura de pila: la pila se siembra con ceros
        const u32 device = u32(arenaC.Obj(DeviceOffset(c)));
        arenaC.Put32(DeviceOffset(c), u32(arenaC.Obj(VtableOffset(c))));
        arenaC.Put32(VtableOffset(c) + 12, u32(reinterpret_cast<uintptr_t>(&FkGetCaps)));
        FillKind(gPs.data(), gPs.size(), 0, a[15]);
        const u32 initial = a[3] ? device : 0u;
        memcpy(gPs.data() + 0x1C, &initial, 4);
        PutGlobal32(kPsPtrAt, u32(uintptr_t(gPs.data())));
        reinterpret_cast<u8*>(kMenuAt)[0] = u8(a[4]);
        gRet = a[5]; gMouseAfter = a[6] ? device : 0u; gButtons = a[7]; gAxes = a[8]; gHr = a[9];
        call.args[0] = u32(arenaA.Obj(a[0])); call.argCount = 1; call.popBytes = 0; call.zeroStack = 1;
        break;
    }
    case 1: {   // getDvdGamePath: sin argumentos; todo lo demas lo hacen los falsos
        call.argCount = 0; call.popBytes = 0;
        break;
    }
    default: {  // GetNearestDistanceOfPedSphereToCameraNearClip
        const u32 idxWord = a[3] & 0xFFFFu;
        const int64 idx = int16_t(idxWord);
        arenaA.Put32(a[0] + 0x18, 0x00ABC000u + (a[12] & 0xFFF0u));           // clump crudo (no es un objeto: el EXE no lo desreferencia)
        arenaA.Put16(a[0] + 0x22, u16(idxWord));
        const u32 mi = u32(arenaB.Obj(a[4])), colModel = u32(arenaC.Obj(a[5])), colData = u32(arenaC.Obj(a[6])), spheres = u32(arenaC.Obj(a[7]));
        arenaB.Put32(a[4] + 0x34, colModel);
        arenaC.Put32(a[5] + 0x2C, colData);
        arenaC.Put32(a[6] + 8, spheres);
        PutGlobal32(uintptr_t(int64(kModelsBase) + idx * 4), mi);
        for (u32 i = 0; i < 12; ++i) memcpy(arenaC.m.data() + kGuard + a[7] + i * 0x14, c.s.data() + i * 16, 16);
        FillKind(reinterpret_cast<u8*>(kCamsAt), kCamsBytes, 0, a[15]);
        reinterpret_cast<u8*>(kActiveCamAt)[0] = u8(a[8]);
        memcpy(reinterpret_cast<u8*>(kCamsAt + a[8] * 0x238 + 0x190), c.t.data(), 24);
        FillKind(gRwCamera.data(), gRwCamera.size(), 0, a[15] + 1);
        memcpy(gRwCamera.data() + 0x80, &a[9], 4);
        PutGlobal32(kNearPtrAt, u32(uintptr_t(gRwCamera.data())));
        gRet = 0x00C0FFEEu;
        call.args[0] = u32(arenaA.Obj(a[0])); call.argCount = 1; call.popBytes = 4;   // cdecl: limpia el llamador
        break;
    }
    }
    return call;
}

// Observa las regiones globales de la funcion: [[region, desplazamiento, byte nuevo]] respecto a la imagen tras preparar el caso
inline json GlobalChanges(const std::vector<std::vector<u8>>& before) {
    json out = json::array();
    const auto& idx = regionsOf[curFn];
    for (size_t k = 0; k < idx.size(); ++k) {
        const auto& r = regions[idx[k]];
        for (size_t i = 0; i < r.bytes; ++i)
            if (r.Ptr()[i] != before[k][i]) out.push_back(json::array({u32(idx[k]), u32(i), r.Ptr()[i]}));
    }
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        gRet = 0;
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        std::vector<std::vector<u8>> before;
        for (const auto i : regionsOf[fn]) before.push_back(ReversingProbe::Bytes(regions[i].Ptr(), regions[i].bytes));
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.zeroStack);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = GlobalChanges(before);
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (fn) {
        case 0: obs["ret"] = Tag(uintptr_t(eax)); obs["extra"] = json::array({Tag(uintptr_t(BitsOf(gPs.data() + 0x1C)))}); break;
        case 1: obs["ret"] = Tag(uintptr_t(eax)); break;
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN135A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN135A_CASES", buffer, sizeof(buffer)) > 0) {
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

// Casilla de la IAT de un modulo para una funcion importada por nombre (0 si no esta)
inline uintptr_t* FindImportSlot(uintptr_t module, const char* func) {
    const auto* base = reinterpret_cast<const uint8*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    const auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress) return nullptr;
    for (const auto* d = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + dir.VirtualAddress); d->Name; ++d) {
        const auto* names = reinterpret_cast<const IMAGE_THUNK_DATA32*>(base + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        auto* slots = reinterpret_cast<IMAGE_THUNK_DATA32*>(const_cast<uint8*>(base) + d->FirstThunk);
        for (; names->u1.AddressOfData; ++names, ++slots) {
            if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG32) continue;
            const auto* by = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(base + names->u1.AddressOfData);
            if (!std::strcmp(reinterpret_cast<const char*>(by->Name), func)) return reinterpret_cast<uintptr_t*>(&slots->u1.Function);
        }
    }
    return nullptr;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn135a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn135a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn135a"); }
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
    // Colaborador con hook RH en la DLL (apagado durante el banco; tambien los bloqueados, que HookRoute enciende y apaga): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador de la CRT del EXE: sin hook y sin funcion propia de la DLL (dll = 0: la DLL llama a la direccion del EXE); el falso va solo en la direccion del EXE
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x745CA0, &FkVideoExclusive), F(0x7469A0, &FkDiMouseInit)};
    env.stubs[1] = {X(0x821195, &FkNewArray), X(0x8213AE, &FkDeleteArray)};
    env.stubs[2] = {F(0x4C7170, &FkAnimate)};
    // kernel32 de getDvdGamePath: la casilla de la IAT del EXE (0x85813C SetErrorMode, 0x858138 GetLogicalDriveStringsA, 0x858134 GetDriveTypeA, 0x858130 GetVolumeInformationA) y la de la DLL (por nombre)
    struct Imp {const char* name; uintptr_t exeSlot; void* fake;};
    const Imp imps[4] = {{"SetErrorMode", 0x85813C, reinterpret_cast<void*>(&FkSetErrorMode)}, {"GetLogicalDriveStringsA", 0x858138, reinterpret_cast<void*>(&FkGetLogicalDriveStrings)},
                         {"GetDriveTypeA", 0x858134, reinterpret_cast<void*>(&FkGetDriveType)}, {"GetVolumeInformationA", 0x858130, reinterpret_cast<void*>(&FkGetVolumeInformation)}};
    json iatInfo = json::array();
    for (const auto& imp : imps) {
        uintptr_t* const exeSlot = reinterpret_cast<uintptr_t*>(imp.exeSlot);
        uintptr_t* const dllSlot = FindImportSlot(env.asi, imp.name);
        const bool same = dllSlot && *dllSlot == *exeSlot && *exeSlot;
        iatInfo.push_back({{"name", imp.name}, {"exe_slot", imp.exeSlot}, {"dll_slot_found", dllSlot != nullptr}, {"same_function", same}});
        if (!same) { ready = false; continue; }
        env.iat[1].push_back(IatSpec{exeSlot, imp.fake});
        env.iat[1].push_back(IatSpec{dllSlot, imp.fake});
        env.watches.emplace_back(imp.exeSlot, 4);
    }
    const char* dllReal[kFunctions] = {
        "toda la funcion salvo IsVideoModeExclusive (0x745CA0, con hook, falso identico) y diMouseInit (0x7469A0, hook bloqueado, falso identico en la direccion del EXE y en el cuerpo de la DLL); GetCapabilities es una ranura de la vtable del dispositivo (falso en la arena C)",
        "toda la funcion salvo SetErrorMode, GetLogicalDriveStringsA, GetDriveTypeA y GetVolumeInformationA (falsos identicos en las casillas de la IAT del EXE y de la DLL) y operator new[] / delete[] (0x821195 / 0x8213AE de la CRT del EXE, falsos en la direccion del EXE: la DLL los llama por esa direccion); lstrlenA y lstrcpyA se ejecutan reales en ambos lados",
        "toda la funcion salvo CPedModelInfo::AnimatePedColModelSkinnedWorld (0x4C7170, con hook, falso identico); la aritmetica x87 es PedSphereNearestCpp (X87Math.cpp)"};
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", f == 1 ? iatInfo : json::array()}};
    }
    // Memoria global del juego que toca el banco (se guarda ahora y se restaura tras cada lado)
    regions.clear();
    regions.emplace_back(kPsPtrAt, 4);
    regions.emplace_back(kMenuAt, 1);
    regions.emplace_back(kModelsAt, kModelsBytes);
    regions.emplace_back(kActiveCamAt, 1);
    regions.emplace_back(kCamsAt, kCamsBytes);
    regions.emplace_back(kNearPtrAt, 4);
    regionsOf[0] = {kRegPsPtr, kRegMenu};
    regionsOf[2] = {kRegModels, kRegActiveCam, kRegCams, kRegNearPtr};
    Run(env, ready, registered, bodies, collaborators);
}
}
