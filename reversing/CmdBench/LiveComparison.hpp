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
constexpr size_t kBlock = 256;
inline Diff DiffAgainstSnap(const std::vector<std::vector<u8>>& base, const std::set<uintptr_t>* mask = nullptr) {
    Diff d;
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto* live = (const u8*)regions[i].addr; const auto* b = base[i].data();
        for (size_t o = 0; o < regions[i].size; o += kBlock) {
            const size_t n = std::min(kBlock, regions[i].size - o);
            if (std::memcmp(live + o, b + o, n) == 0) continue;
            for (size_t k = o; k < o + n; ++k) if (live[k] != b[k]) { if (!InNoCompare(regions[i].addr + k) && (!mask || !mask->count(regions[i].addr + k))) d.bytes[regions[i].addr + k] = live[k]; }
        }
    }
    return d;
}
inline bool InNoRestore(uintptr_t a) { for (auto& r : noRestore) if (a >= r.first && a < r.first + r.second) return true; return false; }
inline void RestoreChanged(const std::vector<std::vector<u8>>& base) {
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* live = (u8*)regions[i].addr; const auto* b = base[i].data();
        for (size_t o = 0; o < regions[i].size; o += kBlock) {
            const size_t n = std::min(kBlock, regions[i].size - o);
            if (std::memcmp(live + o, b + o, n) == 0) continue;
            for (size_t k = o; k < o + n; ++k) if (live[k] != b[k] && !InNoRestore(regions[i].addr + k)) live[k] = b[k];
        }
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
// Comandos con precondicion "el peaton va en un coche" (el original no la comprueba o el C++ lanza si no se cumple): el parametro de peaton es siempre ped1 y ped1 conduce veh0.
inline const std::set<int> kNeedsInCar{ 217, 960, 2065 };
inline std::set<int> gNeedsInCar = kNeedsInCar;
// Dominios validos de parametros enteros que el original usa como indice sin comprobar (asiento, ranura, etc.): {comando, indice de parametro de script} -> valores permitidos
inline const std::map<std::pair<int, int>, std::vector<int>> kParamDomain{
    { { 2, 1 }, { -16, -24, -32, -64, -100 } }, { { 76, 1 }, { -16, -24, -32, -64, -100 } }, { { 77, 1 }, { -16, -24, -32, -64, -100 } }, { { 80, 1 }, { -16, -24, -32, -64, -100 } },   // GOTO/GOSUB: destino negativo = relativo a m_BaseIP (dentro del bytecode propio)
    { { 214, 1 }, { 0, 1, 2, 3, 21, 22, 23 } },   // ANDOR: 0 (ninguno), 1..8 (AND), 21..28 (OR)
    { { 1074, 1 }, { 0, 1, 2, 3 } },   // GET_CHAR_IN_CAR_PASSENGER_SEAT: asiento 0..3
    { { 1208, 1 }, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } },   // GET_CHAR_WEAPON_IN_SLOT: ranuras de arma 0..12
};
inline CVector gPlayerPos{};
inline std::vector<int> gModels{};

// codificacion
struct Enc {
    std::vector<u8>& c;
    void B(u8 v) { c.push_back(v); }
    void W(uint16_t v) { B((u8)v); B((u8)(v >> 8)); }
    void D(u32 v) { W((uint16_t)v); W((uint16_t)(v >> 16)); }
};

inline bool gSafeDomain = false;   // CMDBENCH_DOMAIN=safe: enteros en [0,6] (sin indices fuera de rango); wide: dominio amplio
inline u32 PickInt(Rng& r, u8 kind) {
    if (gSafeDomain && kind != S::K_BOOL) return 1 + r.U(6);   // 1..6: los indices de estilo/bandera de los comandos de texto son 1-based (valor - 1)
    switch (kind) {
    case S::K_BOOL: return r.U(2);
    case S::K_ENUM: return r.U(100) < 85 ? r.U(5) : r.U(10);   // enumerados: indices pequenos (el EXE los usa como indice de tablas sin comprobar)
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

// Comandos de BasicCommands (SET/CSET/ADD/SUB/MULT/DIV/IS_*): el tipo de cada operando lo fija el nombre (VAR = global, LVAR = local, NUMBER/CONSTANT/VAL = constante); el EXE lee las
// variables con GetPointerToScriptVariable, que no admite una constante. Devuelve 'g', 'l' o 'c' por operando en el orden del bytecode (ADD_x_TO_y y SUB_x_FROM_y llevan el destino primero).
inline std::string BasicOperandKinds(const std::string& name) {
    static const char* prefixes[] = { "SET_", "CSET_", "ADD_", "SUB_", "MULT_", "DIV_", "IS_", "ABS_" };
    bool ok = false; for (auto* p : prefixes) if (name.rfind(p, 0) == 0) ok = true;
    if (!ok) return {};
    std::string kinds; size_t pos = 0;
    while (pos <= name.size()) {
        size_t e = name.find('_', pos); if (e == std::string::npos) e = name.size();
        const std::string w = name.substr(pos, e - pos);
        if (w == "VAR") kinds += 'g'; else if (w == "LVAR") kinds += 'l'; else if (w == "NUMBER" || w == "CONSTANT" || w == "VAL") kinds += 'c'; else if (w == "TEXT" || w == "LABEL" || w == "LABEL16") return {};
        pos = e + 1;
    }
    if (kinds.size() == 2 && (name.rfind("ADD_", 0) == 0 || name.rfind("SUB_", 0) == 0)) std::swap(kinds[0], kinds[1]);
    return kinds;
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
    const std::string forced = BasicOperandKinds(std::string(::notsa::script::GetScriptCommandName((eScriptCommands)cmd)));
    size_t operand = 0;
    for (u8 i = 0; i < sg.nIn; ++i) {
        const u8 k = sg.in[i];
        Param p; p.kind = k; p.sub = sg.inSub[i];
        char fk = 0;
        if (!forced.empty() && k != S::K_CMD && k != S::K_SCRIPT && operand < forced.size()) fk = forced[operand++];
        auto emitInt = [&](u32 v, bool allowVar) {
            u32 mode = r.U(100);
            if (fk) mode = fk == 'l' ? 0 : fk == 'g' ? 15 : 99;
            if (allowVar && mode < 12) { auto s = NewLocalSlot(); e.B(3); e.W((uint16_t)s); c.locals.push_back({ s, v }); p.enc = 3; }
            else if (allowVar && mode < 20) { auto s = NewGlobalSlot(); e.B(2); e.W(s); c.globals.push_back({ s, v }); p.enc = 2; }
            else if (mode < 50 && (int)v >= -128 && (int)v <= 127) { e.B(4); e.B((u8)v); p.enc = 4; }
            else if (mode < 70 && (int)v >= -32768 && (int)v <= 32767) { e.B(5); e.W((uint16_t)v); p.enc = 5; }
            else { e.B(1); e.D(v); p.enc = 1; }
            p.raw = v;
        };
        auto emitFloat = [&](float f) {
            u32 raw; std::memcpy(&raw, &f, 4); u32 mode = r.U(100);
            if (fk) mode = fk == 'l' ? 0 : fk == 'g' ? 12 : 99;
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
            if (gNeedsInCar.count(cmd)) h = gPed1;
            emitInt((u32)h, true); break; }
        case S::K_VEH: { u32 d = r.U(100); int h = d < 85 ? gVeh0 : -1; if ((p.sub & 0x80) == 0 && h == -1) h = gVeh0; emitInt((u32)h, true); break; }
        case S::K_OBJ: { u32 d = r.U(100); int h = d < 85 ? gObj0 : -1; if ((p.sub & 0x80) == 0 && h == -1) h = gObj0; emitInt((u32)h, true); break; }
        case S::K_PLAYERPED: case S::K_PLAYERINFO: emitInt(0, true); break;
        case S::K_MODEL: emitInt((u32)gModels[r.U((u32)gModels.size())], true); break;
        case S::K_VAR_I32: case S::K_VAR_F32: case S::K_VAR_OTHER: {
            bool loc = r.U(2); if (fk) loc = (fk == 'l'); u32 v = k == S::K_VAR_F32 ? 0 : PickInt(r, S::K_I32);
            if (k == S::K_VAR_F32) { float f = PickFloat(r); std::memcpy(&v, &f, 4); }
            if (loc) { auto s = NewLocalSlot(); e.B(3); e.W((uint16_t)s); c.locals.push_back({ s, v }); p.enc = 3; }
            else { auto s = NewGlobalSlot(); e.B(2); e.W(s); c.globals.push_back({ s, v }); p.enc = 2; }
            p.raw = v; break; }
        default: { auto dom = kParamDomain.find({ cmd, (int)i }); emitInt(dom != kParamDomain.end() ? (u32)dom->second[r.U((u32)dom->second.size())] : PickInt(r, k), true); break; }
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
    Diff diff; bool wroteOut{}; std::vector<u32> outVals; u32 seed{}; u32 effect{};
};
inline thread_local char gAssertMsg[256];
inline thread_local bool gAssertHit{};
inline void __cdecl BenchAssert(const wchar_t* expr, const wchar_t* file, unsigned line) {
    // Como en una compilacion Release: se anota y se sigue (no se lanza excepcion)
    if (!gAssertHit) std::snprintf(gAssertMsg, sizeof gAssertMsg, "%ls:%u %.60ls", file, line, expr);
    gAssertHit = true;
}
inline u32 gExcCode{}; inline uintptr_t gExcEip{};
inline int Filter(EXCEPTION_POINTERS* p) { gExcCode = p->ExceptionRecord->ExceptionCode; gExcEip = (uintptr_t)p->ExceptionRecord->ExceptionAddress; return EXCEPTION_EXECUTE_HANDLER; }

inline int CallCpp(CommandHandlerFunction h, CRunningScript* s) {
    int r = -77;
    __try { r = (int)h(s); } __except (Filter(GetExceptionInformation())) { r = -88; }
    return r;
}
// F4 (codigo vivo): con CMDBENCH_VIA_DISPATCHER=1 el lado C++ de los comandos 1500..1599 y 2000..2099 llama al despachador C++ (ProcessCommands1500To1599 / ProcessCommands2000To2099)
// en lugar de al manejador propio del comando: compara el despachador reescrito contra el original del EXE (con los enganches de esos dos despachadores apagados mas abajo).
inline bool ViaDispatcher(int cmd) {
    static const bool enabled = [] { char* env = nullptr; size_t len = 0; bool on = false; if (_dupenv_s(&env, &len, "CMDBENCH_VIA_DISPATCHER") == 0 && env) { on = env[0] == '1'; free(env); } return on; }();
    return enabled && ((cmd >= 1500 && cmd <= 1599) || (cmd >= 2000 && cmd <= 2099));
}
inline int CallCppDispatcher(CRunningScript* s, int cmd) {
    int r = -77;
    __try { r = (int)(cmd < 2000 ? s->ProcessCommands1500To1599((int32)cmd) : s->ProcessCommands2000To2099((int32)cmd)); } __except (Filter(GetExceptionInformation())) { r = -88; }
    return r;
}
inline int CallExe(CRunningScript* s, int cmd) {
    int r = -77;
    __try { r = (int)(s->*CRunningScript::s_OriginalCommandHandlerTable[(size_t)cmd / 100])((int32)cmd); } __except (Filter(GetExceptionInformation())) { r = -88; }
    return r;
}

inline int gCurCmd{};
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
    if (gCurCmd == 81) { s->m_StackDepth = 1; s->m_IPStack[0] = gCode + 64; }   // RETURN necesita una direccion de retorno en la pila
}

// La semilla de rand() del EXE (CRT estatico, 0x821B1E) vive en los datos del hilo (_getptd()+0x14), no en .data: se fija antes de cada lado y se compara al final
// (el numero de llamadas a rand() es parte del comportamiento).
inline u32& GameRandSeed() { return *reinterpret_cast<u32*>(reinterpret_cast<char*>(reinterpret_cast<void*(__cdecl*)()>(0x827B3D)()) + 0x14); }
inline u32 gSeed0{};

// Ejecuta un lado y deja el estado resultante en `out`; no restaura (lo hace el llamador).
inline void RunSide(int side, int cmd, const Case& c, const std::vector<std::vector<u8>>& base, Side& out) {
    gCurCmd = cmd;
    PrepareScript(c);
    auto* s = gScript;
    // ProcessOneCommand: lee el opcode (avanza 2) y asigna m_NotFlag
    s->m_IP += 2;
    s->m_NotFlag = c.notFlag;
    CRunningScript::ScriptArgCharNextFreeBuffer = 0;
    gAssertHit = false;
    GameRandSeed() = gSeed0;
    const auto h = CRunningScript::CustomCommandHandlerOf((eScriptCommands)cmd);
    if (side == 0) out.ret = ViaDispatcher(cmd) ? CallCppDispatcher(s, cmd) : CallCpp(h, s); else out.ret = CallExe(s, cmd);
    out.exc = (out.ret == -88);
    if (out.exc) { out.excCode = gExcCode; out.excEip = gExcEip; }
    if (gAssertHit) out.assertMsg = gAssertMsg;
    out.seed = GameRandSeed();
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

struct CmdStats { u32 unhandled{}, effect{}, ptrJitter{}, noiseBytes{}, nondet{}, cases{}, equal{}, diffs{}, excCpp{}, excExe{}, excBoth{}, excDiff{}, wroteOut{}, flagChanged{}, memChanged{}, ipMoved{}, assertCpp{}; };

inline json DescribeIn(const Case& c) {
    json a = json::array();
    for (auto& p : c.in) a.push_back({ { "k", p.kind }, { "enc", p.enc }, { "v", p.kind == S::K_STR ? json(std::string(p.str, strnlen(p.str, 8))) : json(p.raw) } });
    return a;
}
inline const std::vector<std::vector<u8>>* gBaseForLog{};
inline int BaseByte(uintptr_t a) {
    if (!gBaseForLog) return -1;
    for (size_t i = 0; i < regions.size(); ++i) if (a >= regions[i].addr && a < regions[i].addr + regions[i].size) return (*gBaseForLog)[i][a - regions[i].addr];
    return -1;
}
inline json DiffSample(const Diff& a, const Diff& b, size_t max = 12) {
    json out = json::array();
    std::set<uintptr_t> keys; for (auto& [k, v] : a.bytes) keys.insert(k); for (auto& [k, v] : b.bytes) keys.insert(k);
    std::set<uintptr_t> shown;
    for (auto k : keys) {
        auto ia = a.bytes.find(k), ib = b.bytes.find(k);
        const int va = ia == a.bytes.end() ? -1 : ia->second, vb = ib == b.bytes.end() ? -1 : ib->second;
        if (va != vb) {
            const uintptr_t w = k & ~(uintptr_t)3;
            if (shown.count(w)) continue;
            shown.insert(w);
            auto D = [&](const Diff& d) { u32 v = 0; for (int q = 3; q >= 0; --q) { auto it = d.bytes.find(w + (uintptr_t)q); int x = it != d.bytes.end() ? it->second : BaseByte(w + (uintptr_t)q); v = (v << 8) | (u32)(x < 0 ? 0 : x); } return Hex(v); };
            u32 bv = 0; for (int q = 3; q >= 0; --q) { int x = BaseByte(w + (uintptr_t)q); bv = (bv << 8) | (u32)(x < 0 ? 0 : x); }
            out.push_back({ { "addr", Hex((u32)k) }, { "cpp", va }, { "exe", vb }, { "dword", Hex((u32)w) }, { "base4", Hex(bv) }, { "cpp4", D(a) }, { "exe4", D(b) } });
            if (out.size() >= max) break;
        }
    }
    return out;
}

inline bool PointsToFixture(u32 v) {
    for (auto& r : regions) if (r.fixture || r.name == "script" || r.name == "code") if (v >= (u32)r.addr && v < (u32)r.addr + (u32)r.size) return true;
    return false;
}
inline u32 IgnoreHeapPointerJitter(Diff& a, Diff& b, const std::vector<std::vector<u8>>& base) {
    std::set<uintptr_t> words;
    for (auto& [k, v] : a.bytes) words.insert(k & ~(uintptr_t)3);
    for (auto& [k, v] : b.bytes) words.insert(k & ~(uintptr_t)3);
    u32 n = 0;
    auto Val = [&](const Diff& d, uintptr_t w) { u32 v = 0; for (int q = 3; q >= 0; --q) { auto it = d.bytes.find(w + (uintptr_t)q); int x = it != d.bytes.end() ? it->second : BaseByte(w + (uintptr_t)q); v = (v << 8) | (u32)(x < 0 ? 0 : x); } return v; };
    gBaseForLog = &base;
    for (auto w : words) {
        const u32 va = Val(a, w), vb = Val(b, w);
        if (va == vb) continue;
        auto heap = [](u32 v) { return v >= 0x01000000u && v < 0x7FFF0000u; };
        if (heap(va) && heap(vb) && !PointsToFixture(va) && !PointsToFixture(vb)) {
            for (int q = 0; q < 4; ++q) { a.bytes.erase(w + (uintptr_t)q); b.bytes.erase(w + (uintptr_t)q); }
            ++n;
        }
    }
    return n;
}
// bytes que el comando cambio realmente: fuera del objeto de script, el bytecode y las variables globales de trabajo
inline u32 EffectBytes(const Diff& d) {
    const uintptr_t locals0 = (uintptr_t)&gScript->m_LocalVars[0], locals1 = locals0 + sizeof(gScript->m_LocalVars);
    u32 n = 0;
    for (auto& [a, v] : d.bytes) {
        const bool inScript = a >= (uintptr_t)gScript && a < (uintptr_t)gScript + sizeof(CRunningScript);
        if (inScript && !(a >= locals0 && a < locals1)) continue;   // cabecera del script (IP, banderas): se compara aparte
        if (a >= (uintptr_t)gCode && a < (uintptr_t)gCode + kCodeSize) continue;
        ++n;
    }
    return n;
}
inline bool SideEqual(const Side& a, const Side& b) {
    if (a.exc != b.exc) return false;
    if (a.exc) return true;   // las dos fallaron: no se compara el resto (estado indefinido), se anota aparte
    if (a.ret != b.ret || a.ipDelta != b.ipDelta || a.cond != b.cond || a.andor != b.andor || a.notFlag != b.notFlag) return false;
    return a.outVals == b.outVals && a.seed == b.seed && a.diff.bytes == b.diff.bytes;
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
inline int gMode{};
inline void ApplyMode(u64 seed, int cmd) {
    Rng rg(seed ^ 0x77ull);
    gMode = ((rg.U(100) < 30 || gNeedsInCar.count(cmd)) && gP1 && gV0) ? 1 : 0;
    if (gMode == 1) {   // ped1 al volante de veh0 (estado de reposo + vinculo)
        gP1->m_pVehicle = gV0; gP1->bInVehicle = true; gP1->m_nPedState = PEDSTATE_DRIVING; gV0->m_pDriver = gP1;
    }    // salud/armadura con parte fraccionaria (los lectores GET_*_HEALTH truncan; sin esto casi todos los casos tendrian valores enteros)
    Rng r2(seed ^ 0x5151ull);
    if (r2.U(100) < 60) {
        if (gP0) { gP0->m_fHealth = r2.F(1.f, 200.f); gP0->m_fArmour = r2.F(0.f, 100.f); }
        if (gP1) { gP1->m_fHealth = r2.F(1.f, 200.f); gP1->m_fArmour = r2.F(0.f, 100.f); }
        if (gV0) gV0->m_fHealth = r2.F(1.f, 1000.f);
    }
}
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
    { char* d = nullptr; size_t l = 0; if (_dupenv_s(&d, &l, "CMDBENCH_DOMAIN") == 0 && d) { gSafeDomain = std::strcmp(d, "safe") == 0; free(d); } }
    auto wanted = ReadIdList("CMDBENCH_CMDS");
    auto skip = ReadIdList("CMDBENCH_SKIP");
    // 1. despachadores reescritos del EXE: se apagan para que la ranura apunte al codigo original
    std::vector<std::unique_ptr<OriginalDependencies499::HookRoute>> routes;
    json hookInfo = json::array();
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        if (category.Name() == "CRunningScript")
            for (auto& hook : category.Items()) {
                const std::string nm = hook->Name();
                if (nm == "ProcessCommands800To899" || nm == "ProcessCommands1200To1299" || nm == "ProcessCommands1900To1999" || nm == "ProcessCommands1500To1599" || nm == "ProcessCommands2000To2099" || nm == "ProcessCommands0To99" || nm == "ProcessCommands100To199" || nm == "ProcessCommands200To299" || nm == "ProcessCommands500To599" || nm == "ProcessCommands600To699" || nm == "ProcessCommands700To799" || nm == "ProcessCommands900To999") {
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
    Write({ { "event", "begin" }, { "domain", gSafeDomain ? "safe" : "wide" }, { "cases_per_command", cases }, { "commands", (int)cmds.size() }, { "hooks", hookInfo },
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
        const std::string nameStr(::notsa::script::GetScriptCommandName((eScriptCommands)cmd));   // string_view: puede estar vacio (comandos sin nombre en la tabla)
        const char* name = nameStr.c_str();
        if (!Supported(sg)) {
            Write({ { "event", "command_skipped" }, { "command", cmd }, { "name", name }, { "known", sg.known }, { "reason", "firma no soportada por el generador (ver Supported)" } });
            continue;
        }
        Write({ { "event", "command_start" }, { "command", cmd }, { "name", name } });   // marca para saber en que comando cayo el proceso
        for (int n = 0; n < cases; ++n) {
            Case c = MakeCase(cmd, sg, (u32)n);
            PerturbFixtures((u64)cmd * 1000003ull + (u64)n);
            ApplyMode((u64)cmd * 1000003ull + (u64)n, cmd);
            gCurCmd = cmd;
            PrepareScript(c);   // la instantanea incluye el andamiaje: el diff solo recoge lo que cambia el comando
            Snap();
            gSeed0 = 0x5EED0000u + (u32)n * 7919u + (u32)cmd;
            Side a, b, a2, b2;
            auto Reset = [&]() { RestoreChanged(base); for (size_t i = 1; i < regions.size(); ++i) std::memcpy((void*)regions[i].addr, base[i].data(), regions[i].size); };
            RunSide(0, cmd, c, base, a);  Reset();
            RunSide(1, cmd, c, base, b);  Reset();
            RunSide(0, cmd, c, base, a2); Reset();
            RunSide(1, cmd, c, base, b2); Reset();
            // ruido: bytes que cambian de forma distinta entre dos ejecuciones del MISMO lado desde el mismo estado (hilos de audio/streaming, contadores): se descartan en la comparacion
            gBaseForLog = &base;
            std::set<uintptr_t> noise;
            auto Noise = [&](const Diff& x, const Diff& y) {
                for (auto& [k, v] : x.bytes) { auto it = y.bytes.find(k); if (it == y.bytes.end() || it->second != v) noise.insert(k); }
                for (auto& [k, v] : y.bytes) if (!x.bytes.count(k)) noise.insert(k);
            };
            Noise(a.diff, a2.diff); Noise(b.diff, b2.diff);
            for (auto k : noise) { a.diff.bytes.erase(k); b.diff.bytes.erase(k); }
            // Punteros a memoria dinamica (RenderWare, tareas...): su valor depende del orden de las reservas, no del comando. Se ignoran los dwords que difieren cuando ambos
            // lados escriben un valor de montones (0x01000000..0x7FFFFFFF) que no apunta a una fixture ni al script.
            st.ptrJitter += IgnoreHeapPointerJitter(a.diff, b.diff, base);
            b.effect = EffectBytes(b.diff);
            st.noiseBytes += (u32)noise.size();
            if (a2.exc != a.exc || b2.exc != b.exc) ++st.nondet;
            for (auto& r : regions) if (r.fixture) std::memcpy((void*)r.addr, r.pristine.data(), r.size);
            const bool eq = SideEqual(a, b);
            ++st.cases; ++totalCases;
            st.equal += eq; st.diffs += !eq;
            st.excCpp += a.exc; st.excExe += b.exc; st.excBoth += (a.exc && b.exc); st.excDiff += (a.exc != b.exc);
            st.assertCpp += !a.assertMsg.empty();
            st.flagChanged += (b.cond != false);
            st.memChanged += !b.diff.bytes.empty();
            st.wroteOut += b.wroteOut;
            st.effect += (b.effect > 0);
            st.unhandled += (b.ret == -1 && b.ipDelta == 2 && !b.exc);   // la rama por defecto del despachador del EXE (OR_INTERRUPT sin leer parametros): el original no implementa el comando
            if (!eq) {
                ++totalDiffs;
                Write({ { "event", "comparison" }, { "command", cmd }, { "name", name }, { "case", n }, { "equal", false }, { "mode", gMode }, { "input", DescribeIn(c) }, { "not_flag", c.notFlag },
                        { "cpp", { { "exc", a.exc }, { "code", a.excCode }, { "eip", Hex((u32)a.excEip) }, { "assert", a.assertMsg }, { "ret", a.ret }, { "ip", a.ipDelta }, { "cond", a.cond }, { "andor", a.andor }, { "diff_bytes", a.diff.bytes.size() }, { "out", a.outVals }, { "seed", a.seed } } },
                        { "exe", { { "exc", b.exc }, { "code", b.excCode }, { "eip", Hex((u32)b.excEip) }, { "ret", b.ret }, { "ip", b.ipDelta }, { "cond", b.cond }, { "andor", b.andor }, { "diff_bytes", b.diff.bytes.size() }, { "out", b.outVals }, { "seed", b.seed } } },
                        { "diff_sample", DiffSample(a.diff, b.diff) } });
            }
        }
        Write({ { "event", "command_summary" }, { "command", cmd }, { "name", name }, { "cases", st.cases }, { "equal", st.equal }, { "diffs", st.diffs }, { "exc_cpp", st.excCpp }, { "exc_exe", st.excExe }, { "exc_both", st.excBoth },
                { "exc_differ", st.excDiff }, { "assert_cpp", st.assertCpp }, { "exe_mem_changed", st.memChanged }, { "exe_wrote_out", st.wroteOut }, { "exe_effect", st.effect }, { "exe_unhandled", st.unhandled }, { "noise_bytes", st.noiseBytes }, { "ptr_jitter", st.ptrJitter }, { "nondet_exc", st.nondet }, { "exe_flag_set", st.flagChanged }, { "ret", (int)sg.ret }, { "n_in", sg.nIn }, { "n_out", sg.nOut } });
    }
    // restauracion final
    for (size_t i = 0; i < regions.size(); ++i) std::memcpy((void*)regions[i].addr, base[i].data(), regions[i].size);
    Write({ { "event", "complete" }, { "cases", totalCases }, { "diff_cases", totalDiffs } });
}

inline LONG WINAPI CrashFilter(EXCEPTION_POINTERS* p) {
    json st = json::array();
    const auto* sp = reinterpret_cast<const uint32_t*>(p->ContextRecord->Esp);
    for (int k = 0; k < 16; ++k) { MEMORY_BASIC_INFORMATION mi{}; if (!VirtualQuery(sp + k, &mi, sizeof(mi)) || mi.State != MEM_COMMIT) break; st.push_back(Hex(sp[k])); }
    Write({ { "event", "error" }, { "reason", "unhandled_exception" }, { "code", Hex(p->ExceptionRecord->ExceptionCode) }, { "eip", Hex(p->ContextRecord->Eip) }, { "stack", st },
            { "access", p->ExceptionRecord->NumberParameters > 1 ? json(Hex((u32)p->ExceptionRecord->ExceptionInformation[1])) : json(nullptr) } });
    ExitProcess(95);
    return EXCEPTION_EXECUTE_HANDLER;
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
        SetUnhandledExceptionFilter(CrashFilter);
    }
    static int frames = 0;
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) { frames = 0; return; }
    if (++frames < EnvInt("CMDBENCH_WARMUP", 180)) return;
    done = true;
    Run();
}
}
