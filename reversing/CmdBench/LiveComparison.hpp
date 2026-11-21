#pragma once
// Banco por comando de script (CmdBench): para cada manejador C++ reescrito (REGISTER_COMMAND_HANDLER) se ejecuta, con el mismo script sintetico
// y parametros deterministas, el manejador C++ y el manejador ORIGINAL del EXE (despachador de su centena, tabla 0x8A6168) sobre el mismo estado
// de partida (instantanea de .data del EXE + objetos de fixture + objeto de script, restaurada entre los dos lados), y se compara:
// valor de retorno (OpcodeResult), avance del IP, bandera de comparacion / AND-OR / NotFlag, variables del script (locales y globales de
// trabajo), bytes cambiados en .data del EXE (ScriptParams, estadisticas, camara, texto, mundo...) y en los objetos de fixture, y si cada lado
// provoco una excepcion o un assert. Update() se llama desde CGame::Process (probe_build.py). Ver README.md.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <excpt.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include <Scripts/CommandParser/Signature.hpp>
#include <Streaming.h>
#include <Population.h>
#include <CarCtrl.h>
#include <CutsceneMgr.h>
#include <Entity/Object/Object.h>

namespace CmdBenchLive {
using namespace notsa::script;
namespace S = notsa::script::sig;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

inline bool done{};
inline std::ofstream log;
inline std::filesystem::path dir;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }

// ----------------------------------------------------------------------------------------------------------------- aleatorio determinista
struct Rng {
    u64 s;
    explicit Rng(u64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567ull) { Next(); Next(); }
    u64 Next() { u64 z = (s += 0x9E3779B97F4A7C15ull); z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull; return z ^ (z >> 31); }
    u32 U(u32 n) { return n ? (u32)(Next() % n) : 0; }
    float F(float lo, float hi) { return lo + (hi - lo) * (float)((Next() >> 40) / 16777216.0); }
};

// ----------------------------------------------------------------------------------------------------------------- memoria
struct Region { uintptr_t addr; size_t size; std::vector<u8> snap; std::string name; bool fixture{}; std::vector<u8> pristine; };
inline std::vector<Region> regions;           // .data del EXE y objetos de fixture
inline std::vector<std::pair<uintptr_t, size_t>> noRestore;   // rangos que se comparan pero no se restauran (p. ej. contabilidad de memoria)

// Rangos que se restauran pero no se comparan: ScriptParams (el EXE deja ahi parametros y resultados; la DLL lee/escribe tipado, y ningun comando lee lo que dejo otro).
inline std::vector<std::pair<uintptr_t, size_t>> noCompare{ { 0xA43C78, 128 } };
inline bool InNoCompare(uintptr_t a) { for (auto& r : noCompare) if (a >= r.first && a < r.first + r.second) return true; return false; }
inline void SnapAll() { for (auto& r : regions) std::memcpy(r.snap.data(), (void*)r.addr, r.size); }

struct Diff { std::map<uintptr_t, u8> bytes; };
inline Diff DiffAgainstSnap(const std::vector<std::vector<u8>>& base, const std::set<uintptr_t>* mask = nullptr) {
    Diff d;
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto* live = (const u8*)regions[i].addr; const auto* b = base[i].data();
        for (size_t k = 0; k < regions[i].size; ++k) if (live[k] != b[k]) { if (!InNoCompare(regions[i].addr + k) && (!mask || !mask->count(regions[i].addr + k))) d.bytes[regions[i].addr + k] = live[k]; }
    }
    return d;
}
inline bool InNoRestore(uintptr_t a) { for (auto& r : noRestore) if (a >= r.first && a < r.first + r.second) return true; return false; }
inline void RestoreChanged(const std::vector<std::vector<u8>>& base) {
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* live = (u8*)regions[i].addr; const auto* b = base[i].data();
        for (size_t k = 0; k < regions[i].size; ++k) if (live[k] != b[k] && !InNoRestore(regions[i].addr + k)) live[k] = b[k];
    }
}

// ----------------------------------------------------------------------------------------------------------------- script sintetico
inline CRunningScript* gScript{};
inline u8* gCode{};                               // bytecode (alineado)
constexpr size_t kCodeSize = 1024;
constexpr uint16_t kGlobalBase = 0xFE00;          // variables globales de trabajo (desplazamiento en ScriptSpace)
inline std::vector<u8> gCodeSnap;

struct Param { u8 kind{}; u8 sub{}; u8 enc{}; u32 raw{}; char str[8]{}; uint16_t slot{}; bool isOut{}; };
struct Case {
    std::vector<Param> in; std::vector<Param> out; bool notFlag{};
    std::vector<u8> code; std::vector<std::pair<int, u32>> locals; std::vector<std::pair<uint16_t, u32>> globals;   // precarga
    std::string desc;
};

inline int gPed0{}, gPed1{}, gVeh0{}, gObj0{};   // manejadores de fixtures
inline CVector gPlayerPos{};
inline std::vector<int> gModels{};

// codificacion
struct Enc {
    std::vector<u8>& c;
    void B(u8 v) { c.push_back(v); }
    void W(uint16_t v) { B((u8)v); B((u8)(v >> 8)); }
    void D(u32 v) { W((uint16_t)v); W((uint16_t)(v >> 16)); }
};

inline u32 PickInt(Rng& r, u8 kind) {
    switch (kind) {
    case S::K_BOOL: return r.U(2);
    case S::K_I8: { int d = (int)r.U(100); return (u32)(d < 70 ? (int)r.U(9) : d < 85 ? (int)r.U(100) : d < 92 ? -1 : (int)r.U(256) - 128); }
    case S::K_U8: { int d = (int)r.U(100); return d < 70 ? r.U(9) : d < 90 ? r.U(100) : r.U(256); }
    case S::K_I16: { int d = (int)r.U(100); return (u32)(d < 60 ? (int)r.U(9) : d < 85 ? (int)r.U(300) : d < 92 ? -1 : (int)r.U(65536) - 32768); }
    case S::K_U16: { int d = (int)r.U(100); return d < 60 ? r.U(9) : d < 90 ? r.U(300) : r.U(65536); }
    default: { int d = (int)r.U(100); return (u32)(d < 55 ? (int)r.U(9) : d < 75 ? (int)r.U(64) : d < 85 ? (int)r.U(1000) : d < 90 ? -1 : d < 94 ? (int)r.U(100000) : (int)r.Next()); }
    }
}
inline float PickFloat(Rng& r) {
    const u32 d = r.U(100);
    if (d < 12) return 0.0f;
    if (d < 20) return r.U(2) ? 1.0f : -1.0f;
    if (d < 45) return r.F(-5.0f, 5.0f);
    if (d < 60) return gPlayerPos.x + r.F(-30.f, 30.f);
    if (d < 70) return gPlayerPos.y + r.F(-30.f, 30.f);
    if (d < 78) return gPlayerPos.z + r.F(-5.f, 15.f);
    if (d < 92) return r.F(-300.f, 300.f);
    return r.F(-3000.f, 3000.f);
}

// ----------------------------------------------------------------------------------------------------------------- generacion de casos
inline bool Supported(const S::CmdSig& s) {
    if (!s.registered || !s.known) return false;
    for (u8 i = 0; i < s.nIn; ++i) {
        switch (s.in[i]) {
        case S::K_SCRIPTTHING: case S::K_UNKNOWN: case S::K_POOL_OTHER: return false;
        default: break;
        }
        if (s.in[i] == S::K_VEH && s.inSub[i] && ((s.inSub[i] & 0x7F) != 1)) return false;   // solo automoviles de fixture
    }
    return true;
}

inline Case MakeCase(int cmd, const S::CmdSig& sg, u32 n) {
    Rng r((u64)cmd * 1000003ull + n * 7919ull + 17);
    Case c; Enc e{c.code};
    c.notFlag = (r.U(8) == 0);
    int nextLocal = 0; uint16_t nextGlobal = kGlobalBase;
    auto NewLocalSlot = [&]() { return nextLocal++; };
    auto NewGlobalSlot = [&]() { auto v = nextGlobal; nextGlobal += 4; return v; };
    e.W((uint16_t)cmd);   // el bit alto (NotFlag) lo fija ProcessOneCommand; aqui el opcode va limpio y m_NotFlag se asigna aparte
    char buf[64];
    for (u8 i = 0; i < sg.nIn; ++i) {
        const u8 k = sg.in[i];
        Param p; p.kind = k; p.sub = sg.inSub[i];
        auto emitInt = [&](u32 v, bool allowVar) {
            u32 mode = r.U(100);
            if (allowVar && mode < 12) { auto s = NewLocalSlot(); e.B(3); e.W((uint16_t)s); c.locals.push_back({ s, v }); p.enc = 3; }
            else if (allowVar && mode < 20) { auto s = NewGlobalSlot(); e.B(2); e.W(s); c.globals.push_back({ s, v }); p.enc = 2; }
            else if (mode < 50 && (int)v >= -128 && (int)v <= 127) { e.B(4); e.B((u8)v); p.enc = 4; }
            else if (mode < 70 && (int)v >= -32768 && (int)v <= 32767) { e.B(5); e.W((uint16_t)v); p.enc = 5; }
            else { e.B(1); e.D(v); p.enc = 1; }
            p.raw = v;
        };
        auto emitFloat = [&](float f) {
            u32 raw; std::memcpy(&raw, &f, 4); const u32 mode = r.U(100);
            if (mode < 10) { auto s = NewLocalSlot(); e.B(3); e.W((uint16_t)s); c.locals.push_back({ s, raw }); p.enc = 3; }
            else if (mode < 16) { auto s = NewGlobalSlot(); e.B(2); e.W(s); c.globals.push_back({ s, raw }); p.enc = 2; }
            else { e.B(6); e.D(raw); p.enc = 6; }
            p.raw = raw;
        };
        switch (k) {
        case S::K_CMD: case S::K_SCRIPT: continue;       // no se leen del script
        case S::K_F32: emitFloat(PickFloat(r)); break;
        case S::K_STR: {
            static const char* L[] = { "", "MAIN", "CJ", "SWEET", "T1", "EMPTY", "FIRST", "CARLS01" };
            const char* t = L[r.U(8)]; std::memset(p.str, 0, 8); std::strncpy(p.str, t, 7); e.B(9); for (int q = 0; q < 8; ++q) e.B((u8)p.str[q]); p.enc = 9; break; }
        case S::K_PED: {
            u32 d = r.U(100); int h = d < 45 ? gPed1 : d < 85 ? gPed0 : -1; if ((p.sub & 0x80) == 0 && h == -1) h = gPed1;
            emitInt((u32)h, true); break; }
        case S::K_VEH: { u32 d = r.U(100); int h = d < 85 ? gVeh0 : -1; if ((p.sub & 0x80) == 0 && h == -1) h = gVeh0; emitInt((u32)h, true); break; }
        case S::K_OBJ: { u32 d = r.U(100); int h = d < 85 ? gObj0 : -1; if ((p.sub & 0x80) == 0 && h == -1) h = gObj0; emitInt((u32)h, true); break; }
        case S::K_PLAYERPED: case S::K_PLAYERINFO: emitInt(0, true); break;
        case S::K_MODEL: emitInt((u32)gModels[r.U((u32)gModels.size())], true); break;
        case S::K_VAR_I32: case S::K_VAR_F32: case S::K_VAR_OTHER: {
            const bool loc = r.U(2); u32 v = k == S::K_VAR_F32 ? 0 : PickInt(r, S::K_I32);
            if (k == S::K_VAR_F32) { float f = PickFloat(r); std::memcpy(&v, &f, 4); }
            if (loc) { auto s = NewLocalSlot(); e.B(3); e.W((uint16_t)s); c.locals.push_back({ s, v }); p.enc = 3; }
            else { auto s = NewGlobalSlot(); e.B(2); e.W(s); c.globals.push_back({ s, v }); p.enc = 2; }
            p.raw = v; break; }
        default: emitInt(PickInt(r, k), true); break;
        }
        c.in.push_back(p);
    }
    for (u8 i = 0; i < sg.nOut; ++i) {
        Param p; p.kind = sg.out[i]; p.isOut = true;
        if (r.U(3)) { auto s = NewLocalSlot(); e.B(3); e.W((uint16_t)s); c.locals.push_back({ s, 0xCAFE0000u + i }); p.enc = 3; p.slot = (uint16_t)s; }
        else { auto s = NewGlobalSlot(); e.B(2); e.W(s); c.globals.push_back({ s, 0xCAFE0000u + i }); p.enc = 2; p.slot = s; }
        c.out.push_back(p);
    }
    // relleno: ceros (si un lado lee de mas, ambos leeran lo mismo)
    std::snprintf(buf, sizeof buf, "n=%u", n); c.desc = buf;
    return c;
}

// ----------------------------------------------------------------------------------------------------------------- ejecucion de un lado
struct Side {
    bool exc{}; u32 excCode{}; uintptr_t excEip{}; std::string assertMsg; int ret{-99};
    int ipDelta{}; bool cond{}; u16 andor{}; bool notFlag{}; std::vector<u8> scriptBytes; std::vector<u8> codeBytes;
    Diff diff; bool wroteOut{}; std::vector<u32> outVals;
};
inline thread_local char gAssertMsg[256];
inline thread_local bool gAssertHit{};
inline void __cdecl BenchAssert(const wchar_t* expr, const wchar_t* file, unsigned line) {
    gAssertHit = true;
    std::snprintf(gAssertMsg, sizeof gAssertMsg, "%ls:%u %.60ls", file, line, expr);
    RaiseException(0xE0CB0001u, 0, 0, nullptr);
}
inline u32 gExcCode{}; inline uintptr_t gExcEip{};
inline int Filter(EXCEPTION_POINTERS* p) { gExcCode = p->ExceptionRecord->ExceptionCode; gExcEip = (uintptr_t)p->ExceptionRecord->ExceptionAddress; return EXCEPTION_EXECUTE_HANDLER; }

inline int CallCpp(CommandHandlerFunction h, CRunningScript* s) {
    int r = -77;
    __try { r = (int)h(s); } __except (Filter(GetExceptionInformation())) { r = -88; }
    return r;
}
inline int CallExe(CRunningScript* s, int cmd) {
    int r = -77;
    __try { r = (int)(s->*CRunningScript::s_OriginalCommandHandlerTable[(size_t)cmd / 100])((int32)cmd); } __except (Filter(GetExceptionInformation())) { r = -88; }
    return r;
}

inline void PrepareScript(const Case& c) {
    auto* s = gScript;
    std::memset((void*)s, 0, sizeof(CRunningScript));
    s->Init();
    std::memset(gCode, 0, kCodeSize);
    std::memcpy(gCode, c.code.data(), std::min(c.code.size(), kCodeSize));
    s->m_BaseIP = gCode; s->m_IP = gCode;
    s->m_IsActive = true;
    for (auto& [i, v] : c.locals) s->m_LocalVars[(size_t)i].iParam = (int32)v;
    for (auto& [o, v] : c.globals) *(u32*)&CTheScripts::ScriptSpace[o] = v;
    s->m_NotFlag = c.notFlag;
}

inline bool gHaveOriginalSlot[27]{};

// Ejecuta un lado y deja el estado resultante en `out`; no restaura (lo hace el llamador).
inline void RunSide(int side, int cmd, const Case& c, const std::vector<std::vector<u8>>& base, Side& out) {
    PrepareScript(c);
    auto* s = gScript;
    // ProcessOneCommand: lee el opcode (avanza 2) y asigna m_NotFlag
    s->m_IP += 2;
    s->m_NotFlag = c.notFlag;
    CRunningScript::ScriptArgCharNextFreeBuffer = 0;
    gAssertHit = false;
    const auto h = CRunningScript::CustomCommandHandlerOf((eScriptCommands)cmd);
    if (side == 0) out.ret = CallCpp(h, s); else out.ret = CallExe(s, cmd);
    out.exc = (out.ret == -88);
    if (out.exc) { out.excCode = gExcCode; out.excEip = gExcEip; }
    if (gAssertHit) out.assertMsg = gAssertMsg;
    out.ipDelta = (int)(s->m_IP - gCode);
    out.cond = s->m_CondResult; out.andor = s->m_AndOrState; out.notFlag = s->m_NotFlag;
    for (size_t i = 0; i < c.out.size(); ++i) {
        const auto& p = c.out[i];
        const u32 v = p.enc == 3 ? (u32)s->m_LocalVars[p.slot].iParam : *(u32*)&CTheScripts::ScriptSpace[p.slot];
        out.outVals.push_back(v);
        if (v != 0xCAFE0000u + (u32)i) out.wroteOut = true;
    }
    out.diff = DiffAgainstSnap(base);
}

// ----------------------------------------------------------------------------------------------------------------- comparacion
inline std::string Hex(u32 v) { char b[16]; std::snprintf(b, sizeof b, "%08X", v); return b; }

struct CmdStats { u32 cases{}, equal{}, diffs{}, excCpp{}, excExe{}, excBoth{}, excDiff{}, wroteOut{}, flagChanged{}, memChanged{}, ipMoved{}, assertCpp{}; };

inline json DescribeIn(const Case& c) {
    json a = json::array();
    for (auto& p : c.in) a.push_back({ { "k", p.kind }, { "enc", p.enc }, { "v", p.kind == S::K_STR ? json(std::string(p.str, strnlen(p.str, 8))) : json(p.raw) } });
    return a;
}
inline json DiffSample(const Diff& a, const Diff& b, size_t max = 12) {
    json out = json::array();
    std::set<uintptr_t> keys; for (auto& [k, v] : a.bytes) keys.insert(k); for (auto& [k, v] : b.bytes) keys.insert(k);
    for (auto k : keys) {
        auto ia = a.bytes.find(k), ib = b.bytes.find(k);
        const int va = ia == a.bytes.end() ? -1 : ia->second, vb = ib == b.bytes.end() ? -1 : ib->second;
        if (va != vb) { out.push_back({ { "addr", Hex((u32)k) }, { "cpp", va }, { "exe", vb } }); if (out.size() >= max) break; }
    }
    return out;
}

inline bool SideEqual(const Side& a, const Side& b) {
    if (a.exc != b.exc) return false;
    if (a.exc) return true;   // las dos fallaron: no se compara el resto (estado indefinido), se anota aparte
    if (a.ret != b.ret || a.ipDelta != b.ipDelta || a.cond != b.cond || a.andor != b.andor || a.notFlag != b.notFlag) return false;
    return a.outVals == b.outVals && a.diff.bytes == b.diff.bytes;
}

// ----------------------------------------------------------------------------------------------------------------- fixtures
inline CPed* gP0{}; inline CPed* gP1{}; inline CVehicle* gV0{}; inline CObject* gO0{};
inline void CreateFixtures() {
    gPlayerPos = FindPlayerCoors(0);
    for (int m : { 7, 445, 1220 }) { CStreaming::RequestModel(m, STREAMING_GAME_REQUIRED); }
    CStreaming::LoadAllRequestedModels(false);
    gP0 = FindPlayerPed(0);
    gP1 = CPopulation::AddPed(PED_TYPE_CIVMALE, MODEL_MALE01, gPlayerPos + CVector{ 4.f, 0.f, 0.f }, false);
    gV0 = CCarCtrl::CreateCarForScript(MODEL_ADMIRAL, gPlayerPos + CVector{ 10.f, 4.f, 1.f }, false);
    auto* obj = CObject::Create(1220, false);
    if (obj) { obj->SetPosn(gPlayerPos + CVector{ -4.f, 3.f, 1.f }); CWorld::Add(obj); }
    gO0 = obj;
    gPed0 = gP0 ? (int)notsa::PoolOf<CPed>().GetRef(gP0) : -1;
    gPed1 = gP1 ? (int)notsa::PoolOf<CPed>().GetRef(gP1) : -1;
    gVeh0 = gV0 ? (int)notsa::PoolOf<CVehicle>().GetRef(gV0) : -1;
    gObj0 = gO0 ? (int)notsa::PoolOf<CObject>().GetRef(gO0) : -1;
    gModels = { 7, 445, 1220, 0 };
}

inline void AddRegion(uintptr_t a, size_t n, const char* name, bool fixture = false) {
    Region r; r.addr = a; r.size = n; r.snap.resize(n); r.name = name; r.fixture = fixture;
    if (fixture) { r.pristine.resize(n); std::memcpy(r.pristine.data(), (void*)a, n); }
    regions.push_back(std::move(r));
}
// Perturbacion de las fixtures por caso: los enteros/banderas/flotantes (no los punteros ni el indice de modelo) de ped, vehiculo y objeto se alteran
// para que los lectores (GET_*, IS_*) vean valores distintos de los del estado de reposo; el estado vuelve al original tras cada caso.
inline void PerturbFixtures(u64 seed) {
    for (auto& r : regions) {
        if (!r.fixture) continue;
        std::memcpy((void*)r.addr, r.pristine.data(), r.size);
    }
    Rng rg(seed ^ 0xABCDEF1234ull);
    if (rg.U(100) < 20) return;   // una parte de los casos usa el estado de reposo
    for (auto& r : regions) {
        if (!r.fixture) continue;
        auto* d = (u8*)r.addr;
        for (size_t o = 4; o + 4 <= r.size; o += 4) {
            if (o == 0x20) continue;                      // CEntity: indice de modelo
            u32 v; std::memcpy(&v, d + o, 4);
            if (v >= 0x400000u && v < 0xC0000000u) continue;   // parece un puntero
            if (rg.U(12) != 0) continue;
            switch (rg.U(4)) {
            case 0: v ^= 1u << rg.U(32); break;
            case 1: v = rg.U(256); break;
            case 2: { float f = rg.F(-10.f, 10.f); std::memcpy(&v, &f, 4); break; }
            default: v = (u32)rg.Next(); if (v >= 0x400000u && v < 0xC0000000u) v &= 0xFFFF; break;
            }
            std::memcpy(d + o, &v, 4);
        }
    }
}

inline std::set<int> ReadIdList(const char* envName) {
    std::set<int> out; char* env = nullptr; size_t len = 0;
    if (_dupenv_s(&env, &len, envName) == 0 && env) {
        if (std::ifstream in{ env }) for (std::string line; std::getline(in, line);) { int a = 0, b = 0; if (std::sscanf(line.c_str(), "%d-%d", &a, &b) == 2) {} else if (std::sscanf(line.c_str(), "%d", &a) == 1) b = a; else continue; for (int i = a; i <= b; ++i) out.insert(i); }
        free(env);
    }
    return out;
}
inline int EnvInt(const char* name, int def) { char* env = nullptr; size_t len = 0; int v = def; if (_dupenv_s(&env, &len, name) == 0 && env) { v = std::atoi(env); free(env); } return v; }

inline void Run() {
    const int cases = EnvInt("CMDBENCH_CASES", 64);
    auto wanted = ReadIdList("CMDBENCH_CMDS");
    auto skip = ReadIdList("CMDBENCH_SKIP");
    // 1. despachadores reescritos del EXE: se apagan para que la ranura apunte al codigo original
    std::vector<std::unique_ptr<OriginalDependencies499::HookRoute>> routes;
    json hookInfo = json::array();
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        if (category.Name() == "CRunningScript")
            for (auto& hook : category.Items()) {
                const std::string nm = hook->Name();
                if (nm == "ProcessCommands800To899" || nm == "ProcessCommands1200To1299" || nm == "ProcessCommands1900To1999") {
                    hookInfo.push_back({ { "name", nm }, { "hooked_before", hook->Hooked() } });
                    hook->LockState(false); hook->State(false);
                }
            }
    });
    // 2. fixtures y regiones
    CreateFixtures();
    {
        const auto* dos = (const IMAGE_DOS_HEADER*)GetModuleHandleA(nullptr);
        const auto* nt = (const IMAGE_NT_HEADERS*)((const u8*)dos + dos->e_lfanew);
        const auto* sec = IMAGE_FIRST_SECTION(nt);
        for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i)
            if ((sec[i].Characteristics & IMAGE_SCN_MEM_WRITE) && std::memcmp(sec[i].Name, ".data", 5) == 0)
                AddRegion((uintptr_t)dos + sec[i].VirtualAddress, sec[i].Misc.VirtualSize, ".data");
    }
    if (gP0) AddRegion((uintptr_t)gP0, 0x7A4, "ped0", true);
    if (gP1) AddRegion((uintptr_t)gP1, 0x7A4, "ped1", true);
    if (gV0) AddRegion((uintptr_t)gV0, 0x5A0, "veh0", true);
    if (gO0) AddRegion((uintptr_t)gO0, 0x180, "obj0", true);
    gScript = (CRunningScript*)_aligned_malloc(sizeof(CRunningScript) + 64, 16);
    gCode = (u8*)_aligned_malloc(kCodeSize, 16);
    AddRegion((uintptr_t)gScript, sizeof(CRunningScript), "script");
    AddRegion((uintptr_t)gCode, kCodeSize, "code");
    // 3. orden de comandos
    std::vector<int> cmds;
    auto& tab = S::Table();
    for (int id = 0; id <= (int)COMMAND_HIGHEST_VANILLA_ID; ++id) {
        if (!tab[(size_t)id].registered) continue;
        if (!wanted.empty() && !wanted.count(id)) continue;
        if (skip.count(id)) continue;
        cmds.push_back(id);
    }
    Write({ { "event", "begin" }, { "cases_per_command", cases }, { "commands", (int)cmds.size() }, { "hooks", hookInfo },
            { "no_compare", "ScriptParams 0xA43C78..+128" }, { "entry_hooked_after", { { "0x481300", ReversingProbe::JumpsToDll(0x481300) }, { "0x48B590", ReversingProbe::JumpsToDll(0x48B590) }, { "0x46B460", ReversingProbe::JumpsToDll(0x46B460) } } },
            { "fixtures", { { "ped0", gPed0 }, { "ped1", gPed1 }, { "veh0", gVeh0 }, { "obj0", gObj0 } } },
            { "regions", [&] { json a = json::array(); for (auto& r : regions) a.push_back({ { "name", r.name }, { "addr", Hex((u32)r.addr) }, { "size", r.size } }); return a; }() } });
    std::vector<std::vector<u8>> base(regions.size());
    for (size_t i = 0; i < regions.size(); ++i) base[i].resize(regions[i].size);
    auto Snap = [&]() { for (size_t i = 0; i < regions.size(); ++i) std::memcpy(base[i].data(), (void*)regions[i].addr, regions[i].size); };
    u32 totalDiffs = 0, totalCases = 0;
    for (int cmd : cmds) {
        const auto& sg = tab[(size_t)cmd];
        CmdStats st;
        const char* name = ::notsa::script::GetScriptCommandName((eScriptCommands)cmd).data();
        if (!Supported(sg)) {
            Write({ { "event", "command_skipped" }, { "command", cmd }, { "name", name }, { "known", sg.known }, { "reason", "firma no soportada por el generador (ver Supported)" } });
            continue;
        }
        Write({ { "event", "command_start" }, { "command", cmd }, { "name", name } });   // marca para saber en que comando cayo el proceso
        for (int n = 0; n < cases; ++n) {
            Case c = MakeCase(cmd, sg, (u32)n);
            PerturbFixtures((u64)cmd * 1000003ull + (u64)n);
            Snap();
            Side a, b;
            RunSide(0, cmd, c, base, a);
            RestoreChanged(base);
            // el lado C++ puede haber dejado los datos de la fixture distintos si fallo: se vuelve a la instantanea completa de las regiones pequenas
            for (size_t i = 1; i < regions.size(); ++i) std::memcpy((void*)regions[i].addr, base[i].data(), regions[i].size);
            RunSide(1, cmd, c, base, b);
            RestoreChanged(base);
            for (size_t i = 1; i < regions.size(); ++i) std::memcpy((void*)regions[i].addr, base[i].data(), regions[i].size);
            for (auto& r : regions) if (r.fixture) std::memcpy((void*)r.addr, r.pristine.data(), r.size);
            const bool eq = SideEqual(a, b);
            ++st.cases; ++totalCases;
            st.equal += eq; st.diffs += !eq;
            st.excCpp += a.exc; st.excExe += b.exc; st.excBoth += (a.exc && b.exc); st.excDiff += (a.exc != b.exc);
            st.assertCpp += !a.assertMsg.empty();
            st.flagChanged += (b.cond != false);
            st.memChanged += !b.diff.bytes.empty();
            st.wroteOut += b.wroteOut;
            if (!eq) {
                ++totalDiffs;
                Write({ { "event", "comparison" }, { "command", cmd }, { "name", name }, { "case", n }, { "equal", false }, { "input", DescribeIn(c) }, { "not_flag", c.notFlag },
                        { "cpp", { { "exc", a.exc }, { "code", a.excCode }, { "eip", Hex((u32)a.excEip) }, { "assert", a.assertMsg }, { "ret", a.ret }, { "ip", a.ipDelta }, { "cond", a.cond }, { "andor", a.andor }, { "diff_bytes", a.diff.bytes.size() } } },
                        { "exe", { { "exc", b.exc }, { "code", b.excCode }, { "eip", Hex((u32)b.excEip) }, { "ret", b.ret }, { "ip", b.ipDelta }, { "cond", b.cond }, { "andor", b.andor }, { "diff_bytes", b.diff.bytes.size() } } },
                        { "diff_sample", DiffSample(a.diff, b.diff) } });
            }
        }
        Write({ { "event", "command_summary" }, { "command", cmd }, { "name", name }, { "cases", st.cases }, { "equal", st.equal }, { "diffs", st.diffs }, { "exc_cpp", st.excCpp }, { "exc_exe", st.excExe }, { "exc_both", st.excBoth },
                { "exc_differ", st.excDiff }, { "assert_cpp", st.assertCpp }, { "exe_mem_changed", st.memChanged }, { "exe_wrote_out", st.wroteOut }, { "exe_flag_set", st.flagChanged }, { "ret", (int)sg.ret }, { "n_in", sg.nIn }, { "n_out", sg.nOut } });
    }
    // restauracion final
    for (size_t i = 0; i < regions.size(); ++i) std::memcpy((void*)regions[i].addr, base[i].data(), regions[i].size);
    Write({ { "event", "complete" }, { "cases", totalCases }, { "diff_cases", totalDiffs } });
}

inline void ModuleMarker() {}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "cmdbench-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(dir / "cmdbench-runtime.jsonl", std::ios::app); Write({ { "event", "waiting_for_player" } });
        ReversingProbe::InstallFailureCapture("cmdbench");
        static ReversingProbe::EntryPatch patch(reinterpret_cast<uintptr_t>(&_wassert), BenchAssert);
    }
    static int frames = 0;
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) { frames = 0; return; }
    if (++frames < EnvInt("CMDBENCH_WARMUP", 180)) return;
    done = true;
    Run();
}
}
