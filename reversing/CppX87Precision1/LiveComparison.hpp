#pragma once
// Banco dentro del motor (SIN COMPILAR NI EJECUTAR TODAVIA): independencia de los ayudantes de X87Math.cpp frente a la palabra de control del x87.
// 14 funciones puras cuyo original del EXE se llama por su direccion y cuyo candidato es la DLL (11 con hook conmutado, 3 llamadas directas por simbolo/instruccion).
// Cada funcion se ejecuta con 1024 entradas deterministas por combinacion de precision (24, 53, 64 bits) y redondeo (al mas cercano, hacia -inf, hacia +inf, hacia 0):
// 12 combinaciones, 12288 pares por funcion. Las dos palabras de control (x87 y MXCSR) se fijan justo antes de cada llamada y se restauran justo despues, dentro
// del mismo bloque de ensamblador; se comprueba que la funcion las deja como las recibio, que la pila x87 queda vacia, ESP y EBX/ESI/EDI/EBP conservados, y al
// terminar que ambas palabras, errno del EXE, las globales de InitGearRatios y los hooks estan como al empezar.
// Las funciones que devuelven un float en ST0 se comparan con sus 80 bits. Cada funcion lleva una mascara de precisiones "exigidas" (el contrato): sus diferencias
// fallan el banco; las de las demas precisiones se registran como exploratorias (required=false) y se cuentan en el resumen, sin ocultarlas.
// Cortafuegos (ningun caso puede colgar el juego): (1) las dos funciones del banco con un bucle dependiente de los datos que tambien tiene el EXE
// (CPed::GetLocalDirection: "+2pi mientras < 0"; cHandlingDataMgr::ConvertDataToGameUnits: "-0.01 mientras > 0") predicen en C++ el numero de iteraciones
// ANTES de llamar y no llaman si supera kMaxLoopIterations o si la x87 dejaria de avanzar (se registra un error "firewall" y el banco falla, sin colgarse);
// el generador ya acota esas entradas para que no ocurra; (2) un vigilante en otro hilo aborta con un registro de error si una sola llamada tarda mas de 12 s.
#include <fstream>
#include <atomic>
#include <thread>
#include <float.h>
#include <xmmintrin.h>
#include <bit>
#include <cmath>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "General.h"
#include "cHandlingDataMgr.h"
#include "common.h"
#include "X87Math.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppX87Precision1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }

// --- cortafuegos ---
inline constexpr uint64 kMaxLoopIterations = 5'000'000, kUnbounded = ~0ull;
inline std::atomic<unsigned> firewallTrips{};
// CPed::GetLocalDirection (EXE 0x5DEF60..0x5DEFA3): valor = fpatan(-x, y) - rot + pi/4 (fpatan en [-pi, pi]); mientras valor < 0 (fcom ordenado: con NaN no entra) valor += 2pi.
// Con rot = +inf o muy grande el valor es -inf o tan grande que sumar 6,28 no lo cambia (a 24 bits ya desde ~2^27; con redondeo hacia abajo tambien): el EXE no termina.
// Se admite NaN (no entra), cualquier rot negativo (incluido -inf: el valor es +inf) y rot <= 2^24 (<= ~4,2 M iteraciones de paso >= 4 incluso con el redondeo en contra).
inline constexpr float kLocalDirectionMaxRotation = 16777216.f;
inline uint64 LocalDirectionLoopBound(float rot) {
    if (rot != rot) return 0;
    if (!(rot <= kLocalDirectionMaxRotation)) return kUnbounded;
    const double deficit = 2.36 + double(rot);   // 3pi/4 = pi - pi/4: el valor inicial es >= -deficit
    return deficit <= 0.0 ? 0 : uint64(deficit / 4.0) + 3;
}
// cHandlingDataMgr::ConvertDataToGameUnits (EXE 0x6F50F0..0x6F5158): B = [0xC2B9BC] * MaxVelocity; mientras B > 0 (NaN sale) B -= 0.01 (double 0x85A308) y sale antes si se cumple la condicion
// de arrastre: como mucho B/0,01 vueltas. Por encima de ~2^18 la x87 a 24 bits deja de avanzar al restar 0,01 (+inf nunca baja): no acotado.
inline uint64 HandlingLoopBound(float maxVelocity) {
    const double b = double(*reinterpret_cast<const float*>(0xC2B9BC)) * double(maxVelocity);
    if (b != b || b <= 0.0) return 0;
    if (!(b < 1.0e5)) return kUnbounded;
    return uint64(b / 0.0099) + 3;
}
// vigilante: una llamada del banco que no vuelve en kWatchdogMs deja un error (con funcion, caso, precision, redondeo y lado) y cierra el proceso en vez de colgar el juego
inline constexpr uint64 kWatchdogMs = 12000;
inline std::atomic<uint64> guardStart{0};
inline std::atomic<unsigned> guardFn{}, guardCase{}, guardSide{}, guardPc{}, guardRc{};
inline void StartWatchdog() {
    static bool started = false; if (started) return; started = true;
    std::thread([] {
        for (;;) {
            Sleep(250);
            const uint64 s = guardStart.load();
            if (s && GetTickCount64() - s > kWatchdogMs)
                ReversingProbe::Fail({{"event", "error"}, {"reason", "watchdog: una llamada del banco no vuelve (bucle sin fin en el EXE o en la DLL)"}, {"function_index", guardFn.load()},
                                      {"case", guardCase.load()}, {"candidate_side", guardSide.load() != 0}, {"pc", guardPc.load()}, {"rc", guardRc.load()}, {"elapsed_ms", GetTickCount64() - s}}, 95);
        }
    }).detach();
}
struct CallGuard {
    CallGuard(unsigned fi, unsigned i, bool candidate) { guardFn = fi; guardCase = i; guardSide = candidate ? 1u : 0u; guardStart = GetTickCount64(); }
    ~CallGuard() { guardStart = 0; }
};
inline std::string Hex(const void* p, size_t n) {
    static const char* d = "0123456789abcdef"; auto* b = static_cast<const uint8*>(p); std::string s; s.reserve(n * 2);
    for (size_t i = 0; i < n; ++i) { s.push_back(d[b[i] >> 4]); s.push_back(d[b[i] & 15]); }
    return s;
}

enum Kind { kATan, kRadian, kAngleDeg, kNodeHeading, kQuadratic, kRandomRange, kBikeWorld, kBikeGame, kHandlingGame, kLocalDirection, kTransformPoint,
            kFtol2Float, kFtol2Double, kFistp };
// mask: bit 0 = PC24, bit 1 = PC53, bit 2 = PC64 (precisiones en las que se EXIGE igualdad); el redondeo se exige siempre.
// Contrato inicial (a ampliar hasta 7 conforme se cierren los limites):
//  - 3: ayudantes que pasan un double por la pila entre dos operaciones del EXE (ratio -> fpatan, discriminante -> fsqrt, angulo -> _ftol2): PC64 no es expresable sin un tipo de 80 bits.
//  - 1: el codigo de la clase es C++ ordinario (SSE en Debug) alrededor del ayudante; solo coincide con la x87 con PC24.
//  - 7: ayudante de una sola expresion, o la propia instruccion x87.
struct Fn { const char* cls; const char* name; uintptr_t address; Kind kind; unsigned mask; int cases; };
inline const std::array<Fn, 14> fns{{
    {"CGeneral","GetATanOfXY",0x53CC70,kATan,3,1024},
    {"CGeneral","GetRadianAngleBetweenPoints-",0x53CBE0,kRadian,3,1024},
    {"CGeneral","GetAngleBetweenPoints",0x53CEA0,kAngleDeg,1,1024},
    {"CGeneral","GetNodeHeadingFromVector",0x53CDC0,kNodeHeading,1,1024},
    {"CGeneral","SolveQuadratic",0x53CE30,kQuadratic,3,1024},
    {"CGeneral","GetRandomNumberInRange<float>-",0x41BD90,kRandomRange,7,1024},
    {"cHandlingDataMgr","ConvertBikeDataToWorldUnits",0x6F5240,kBikeWorld,7,1024},
    {"cHandlingDataMgr","ConvertBikeDataToGameUnits",0x6F5290,kBikeGame,7,1024},
    {"cHandlingDataMgr","ConvertDataToGameUnits",0x6F5080,kHandlingGame,1,1024},
    {"CPed","GetLocalDirection",0x5DEF60,kLocalDirection,3,1024},
    {"Common","TransformPoint",0x54ECE0,kTransformPoint,3,1024},
    {"X87","Ftol2RegsCpp",0x821B40,kFtol2Float,7,1024},
    {"X87","Ftol2RegsDblCpp",0x821B40,kFtol2Double,7,1024},
    {"X87","RwPlCoreFastRealToUInt32Cpp",0,kFistp,7,1024},
}};
constexpr unsigned kPcCount = 3, kRcCount = 4;
constexpr const char* kRcNames[kRcCount]{"near","down","up","chop"};
constexpr unsigned kPcBits[kPcCount]{24, 53, 64};

// --- RNG falso del juego (CGeneral::GetRandomNumber y rand del CRT del EXE, 0x821B1E) ---
inline unsigned randomValue{}, randomCalls{};
inline int __cdecl FakeRandom() { ++randomCalls; return int(randomValue); }

// --- palabras de control ---
inline uint16 savedCw{}; inline uint32 savedCsr{};
struct Modes { uint16 cwIn; uint32 csrIn; };
inline uint16 ReadCw() { uint16 cw; __asm { fnstcw word ptr cw } return cw; }
inline void WriteCw(uint16 cw) { __asm { fldcw word ptr cw } }
inline uint32 ReadTop() { uint16 sw; __asm { fnstsw word ptr sw } return (sw >> 11) & 7u; }
// x87: PC en los bits 8-9 (00 = 24, 10 = 53, 11 = 64), RC en los 10-11 (00 cercano, 01 abajo, 10 arriba, 11 cero); el resto como estaba (excepciones enmascaradas).
// MXCSR: RC en los bits 13-14 con la misma codificacion; asi el codigo SSE de la DLL y el x87 del EXE ven el mismo modo.
inline Modes MakeModes(unsigned pc, unsigned rc) {
    const uint16 pcBits = pc == 0 ? 0x0000 : pc == 1 ? 0x0200 : 0x0300;
    return { uint16((savedCw & ~0x0F00u) | pcBits | (rc << 10)), (savedCsr & ~0x6000u) | (rc << 13) };
}
// Los bits 6-7 de la palabra x87 estan reservados (fnstcw los devuelve como el hardware quiera tras una ida y vuelta por el CRT de la DLL): se ignoran; PC, RC y las mascaras se comparan.
inline constexpr uint16 kCwReservedBits = 0x00C0;
inline bool CwSame(uint16 a, uint16 b) { return (a & ~kCwReservedBits) == (b & ~kCwReservedBits); }
inline bool ModeKept(const Modes& m, uint16 cwAfter, uint32 csrAfter) { return CwSame(cwAfter, m.cwIn) && (csrAfter & 0xFFC0u) == (m.csrIn & 0xFFC0u); }

struct CallOut { uint32 eax{}, ecx{}, edx{}; std::array<uint8, 10> st{}; uint32 top{}; bool stack{}, regs{}, mode{}; };
// Llamada cdecl/stdcall generica a una direccion del EXE (con o sin hook) con ECX de entrada; el modo se fija y se restaura dentro del bloque.
inline CallOut CallRaw(uintptr_t fn, const uint32* args, int n, bool calleePops, uint32 ecxIn, bool fpu, const Modes& m) {
    uint32 rEax{}, rEcx{}, rEdx{}, csrAfter{}; uint16 cwAfter{}, fsw{}; uint8 x87[10]{};
    uintptr_t espB{}, espA{}, ebxB{}, ebxA{}, esiB{}, esiA{}, ediB{}, ediA{}, ebpB{}, ebpA{};
    const int popBytes = calleePops ? 0 : n * 4; const int fpuFlag = fpu ? 1 : 0;
    const uint16 cwIn = m.cwIn, cwSaved = savedCw; const uint32 csrIn = m.csrIn, csrSaved = savedCsr;
    __asm {
        mov espB, esp
        mov ebxB, ebx
        mov esiB, esi
        mov ediB, edi
        mov ebpB, ebp
        fldcw word ptr cwIn
        ldmxcsr dword ptr csrIn
        mov ecx, n
        mov edx, args
    L_push:
        test ecx, ecx
        jz L_call
        dec ecx
        push dword ptr [edx+ecx*4]
        jmp L_push
    L_call:
        mov ecx, ecxIn
        call dword ptr fn
        mov rEax, eax
        mov rEcx, ecx
        mov rEdx, edx
        add esp, popBytes
        mov espA, esp
        mov ebxA, ebx
        mov esiA, esi
        mov ediA, edi
        mov ebpA, ebp
        fnstcw word ptr cwAfter
        stmxcsr dword ptr csrAfter
        cmp fpuFlag, 0
        je L_nofpu
        fstp tbyte ptr x87
    L_nofpu:
        fnstsw word ptr fsw
        fldcw word ptr cwSaved
        ldmxcsr dword ptr csrSaved
    }
    CallOut o; o.eax = rEax; o.ecx = rEcx; o.edx = rEdx; std::memcpy(o.st.data(), x87, 10); o.top = (fsw >> 11) & 7u;
    o.stack = espB == espA; o.regs = ebxB == ebxA && esiB == esiA && ediB == ediA && ebpB == ebpA; o.mode = ModeKept(m, cwAfter, csrAfter);
    return o;
}
// _ftol2 (0x821B40) recibe el valor en ST0 (fld dword o fld qword) y ECX; devuelve EAX, ECX, EDX.
inline CallOut CallFtol2(uintptr_t fn, uint64 valueBits, bool isDouble, uint32 ecxIn, const Modes& m) {
    uint32 rEax{}, rEcx{}, rEdx{}, csrAfter{}; uint16 cwAfter{}, fsw{};
    uintptr_t espB{}, espA{}, ebxB{}, ebxA{}, esiB{}, esiA{}, ediB{}, ediA{}, ebpB{}, ebpA{};
    uint32 floatBits = uint32(valueBits); uint64 doubleBits = valueBits; const int dbl = isDouble ? 1 : 0;
    const uint16 cwIn = m.cwIn, cwSaved = savedCw; const uint32 csrIn = m.csrIn, csrSaved = savedCsr;
    __asm {
        mov espB, esp
        mov ebxB, ebx
        mov esiB, esi
        mov ediB, edi
        mov ebpB, ebp
        fldcw word ptr cwIn
        ldmxcsr dword ptr csrIn
        cmp dbl, 0
        jne L_double
        fld dword ptr floatBits
        jmp L_loaded
    L_double:
        fld qword ptr doubleBits
    L_loaded:
        mov ecx, ecxIn
        call dword ptr fn
        mov rEax, eax
        mov rEcx, ecx
        mov rEdx, edx
        mov espA, esp
        mov ebxA, ebx
        mov esiA, esi
        mov ediA, edi
        mov ebpA, ebp
        fnstcw word ptr cwAfter
        stmxcsr dword ptr csrAfter
        fnstsw word ptr fsw
        fldcw word ptr cwSaved
        ldmxcsr dword ptr csrSaved
    }
    CallOut o; o.eax = rEax; o.ecx = rEcx; o.edx = rEdx; o.top = (fsw >> 11) & 7u;
    o.stack = espB == espA; o.regs = ebxB == ebxA && esiB == esiA && ediB == ediA && ebpB == ebpA; o.mode = ModeKept(m, cwAfter, csrAfter);
    return o;
}
// Candidato llamado directamente por simbolo desde C++: el modo se fija justo antes de la llamada y se restaura justo despues; devuelve tambien si la funcion lo dejo intacto y la pila x87 vacia.
template<class Callable> inline auto RunDirect(const Modes& m, bool& modeOk, Callable&& call) {
    WriteCw(m.cwIn); _mm_setcsr(m.csrIn);
    auto result = call();
    const uint16 cwAfter = ReadCw(); const uint32 csrAfter = _mm_getcsr(); const uint32 top = ReadTop();
    WriteCw(savedCw); _mm_setcsr(savedCsr);
    modeOk = ModeKept(m, cwAfter, csrAfter) && top == 0;
    return result;
}
// FLD dword [x]; FISTP dword [res] con el modo vigente: la propia instruccion que RwPlCoreFastRealToUInt32Cpp reproduce (rwplcore.h).
inline uint32 ReferenceFistp(float x, const Modes& m, bool& modeOk) {
    uint32 res{}; uint16 cwAfter{}, fsw{}; const uint16 cwIn = m.cwIn, cwSaved = savedCw;
    __asm {
        fldcw word ptr cwIn
        fld dword ptr x
        fistp dword ptr res
        fnstcw word ptr cwAfter
        fnstsw word ptr fsw
        fldcw word ptr cwSaved
    }
    modeOk = CwSame(cwAfter, cwIn) && ((fsw >> 11) & 7u) == 0;
    return res;
}

// --- entradas deterministas ---
inline constexpr uint32 kSpecialF[]{
    0x00000000,0x80000000,0x00000001,0x80000001,0x007FFFFF,0x807FFFFF,0x00800000,0x80800000,
    0x3F800000,0xBF800000,0x3F000000,0xBF000000,0x3EFFFFFF,0xBEFFFFFF,0x3F000001,0xBF000001,
    0x3FC00000,0xBFC00000,0x40200000,0xC0200000,0x3F7FFFFF,0xBF7FFFFF,0x3F800001,0xBF800001,
    0x4B000000,0xCB000000,0x4AFFFFFF,0xCAFFFFFF,0x4B000001,0x4B800000,
    0x4EFFFFFF,0xCEFFFFFF,0x4F000000,0xCF000000,0xCF000001,0x4F800000,
    0x5EFFFFFF,0xDEFFFFFF,0x5F000000,0xDF000000,0x5F000001,
    0x7F7FFFFF,0xFF7FFFFF,0x7F800000,0xFF800000,
    0x7FC00000,0xFFC00000,0x7F800001,0xFF800001,0x7FA00000,0xFFA00000,0x7FFFFFFF,0xFFFFFFFF,0x7FC12345,0xFFC0ABCD,
    0x40490FDB,0xC0490FDB,0x3FC90FDB,0xBFC90FDB,0x40C90FDB,0x3D4CCCCD,0x3EAAAAAB,0x34000000,0xB4000000};
inline constexpr uint64 kSpecialD[]{
    0x0000000000000001ull,0x000FFFFFFFFFFFFFull,0x0010000000000000ull,0x3FE0000000000000ull,0x3FDFFFFFFFFFFFFFull,0x3FE0000000000001ull,
    0x3FF0000000000000ull,0x3FEFFFFFFFFFFFFFull,0x3FF0000000000001ull,0x4330000000000000ull,0x432FFFFFFFFFFFFFull,0x43E0000000000000ull,
    0x43DFFFFFFFFFFFFFull,0xC3E0000000000000ull,0xC3E0000000000001ull,0x7FF0000000000000ull,0xFFF0000000000000ull,0x7FF8000000000000ull,
    0xFFF8000000000000ull,0x7FF0000000000001ull,0x7FFFFFFFFFFFFFFFull,0x47E0000000000000ull,0x7FEFFFFFFFFFFFFFull,0x3FF8000000000000ull,
    0x4004000000000000ull,0xBFF8000000000000ull,0x8000000000000000ull,0x0000000000000000ull};

struct Gen {
    uint32 s;
    explicit Gen(uint32 seed) : s(seed * 2654435761u + 0x9E3779B9u) { if (!s) s = 1; for (int i = 0; i < 4; ++i) next(); }
    uint32 next() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
    float uni(float a, float b) { return a + (b - a) * float(next() % 1000001u) / 1000000.f; }
    static bool NonFinite(float f) { return (std::bit_cast<uint32>(f) & 0x7F800000u) == 0x7F800000u; }
    // flotante de prueba: especiales (NaN, inf, +-0, subnormales, extremos), uniforme, pequeno, rejilla de cuartos, patron de bits finito, k o k+0.5
    float f(float scale, bool allowNonFinite) {
        float v;
        switch (next() % 10) {
        case 0: case 1: case 2: v = std::bit_cast<float>(kSpecialF[next() % (sizeof kSpecialF / sizeof kSpecialF[0])]); break;
        case 3: case 4: case 5: v = uni(-scale, scale); break;
        case 6: v = uni(-1e-3f, 1e-3f); break;
        case 7: v = float(int32(next() % 2001u) - 1000) * 0.25f; break;
        case 8: { uint32 b = next(); if ((b & 0x7F800000u) == 0x7F800000u) b &= 0xBFFFFFFFu; v = std::bit_cast<float>(b); break; }
        default: v = float(int32(next() % 65536u) - 32768) + (next() % 2 ? 0.5f : 0.f); break;
        }
        if (!allowNonFinite && NonFinite(v)) v = (next() % 2 ? 1.f : -1.f) * scale;
        return v;
    }
    // double de prueba: ensanchados de float, mantisas completas de 53 bits, especiales, k + 0.5 con 1 ulp de desvio, patron de bits aleatorio
    double d() {
        switch (next() % 10) {
        case 0: case 1: return double(std::bit_cast<float>(kSpecialF[next() % (sizeof kSpecialF / sizeof kSpecialF[0])]));
        case 2: case 3: return std::bit_cast<double>(kSpecialD[next() % (sizeof kSpecialD / sizeof kSpecialD[0])]);
        case 4: case 5: case 6: {
            const uint64 mant = (uint64(next()) << 20 ^ next()) & 0xFFFFFFFFFFFFFull; const uint64 exp = 0x3FFull - 60 + next() % 130;
            return std::bit_cast<double>((uint64(next() % 2) << 63) | (exp << 52) | mant); }
        case 7: { double k = double(int64(next() % 4000001u) - 2000000) + 0.5; uint64 b = std::bit_cast<uint64>(k); b += next() % 5; b -= 2; return std::bit_cast<double>(b); }
        case 8: { uint64 b = (uint64(next()) << 32) | next(); if (((b >> 52) & 0x7FF) == 0x7FF) b &= ~(1ull << 62); return std::bit_cast<double>(b); }
        default: return double(f(3000.f, true)) * (next() % 4 == 0 ? 1.0000000001 : 1.0);
        }
    }
};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls, const char* name) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == name) r = h.get(); });
    return r;
}
inline int* ExeErrno() { return reinterpret_cast<int*(__cdecl*)()>(0x82B181)(); }

// Ejecuta un caso (funcion fi, indice i) con el modo dado; candidate = true usa la DLL (hook activado o llamada directa).
inline json RunCase(const Fn& fn, unsigned fi, unsigned i, const Modes& m, bool candidate, ReversibleHooks::ReversibleHook::Base* hook) {
    Gen g(fi * 7919u + i * 2u + 1u);
    const CallGuard guard(fi, i, candidate);
    auto trip = [&](const char* why, uint64 bound) {
        if (firewallTrips++ == 0) Write({{"event", "error"}, {"reason", std::string("firewall: ") + why}, {"function", std::string(fn.cls) + "::" + fn.name}, {"case", i}, {"predicted_iterations", bound}});
    };
    const bool hooked = hook != nullptr;
    const bool routeOk = hooked ? ReversingProbe::JumpsToDll(fn.address) == candidate : (fn.address == 0 || !ReversingProbe::JumpsToDll(fn.address));
    CallOut o{}; json out = json::object(); bool modeOk = true, stackOk = true, regsOk = true, topOk = true;
    auto note = [&] { stackOk = o.stack; regsOk = o.regs; modeOk = o.mode; topOk = o.top == 0; };
    auto call = [&](const uint32* a, int n, bool pops, uint32 ecx, bool fpu) { o = CallRaw(fn.address, a, n, pops, ecx, fpu, m); note(); };
    *ExeErrno() = 0x5A5A5A5A;
    switch (fn.kind) {
    case kATan: {
        const float s = g.next() % 4 ? 3000.f : 1e25f; uint32 a[2]{F(g.f(s, true)), F(g.f(s, true))};
        if (g.next() % 8 == 0) a[1] = a[0] ^ (g.next() % 2 ? 0x80000000u : 0u);
        call(a, 2, false, 0, true); out = {{"st", Hex(o.st.data(), 10)}, {"top", o.top}}; break; }
    case kRadian: case kAngleDeg: {
        const bool nan = fn.kind == kRadian; const float s = g.next() % 4 ? 3000.f : 1e25f; uint32 a[4];
        for (int k = 0; k < 4; ++k) a[k] = F(g.f(s, nan));
        if (g.next() % 6 == 0) a[2] = a[0];
        if (g.next() % 6 == 0) a[3] = a[1];
        call(a, 4, false, 0, true); out = {{"st", Hex(o.st.data(), 10)}, {"top", o.top}}; break; }
    case kNodeHeading: {
        uint32 a[2]{F(g.f(3000.f, false)), F(g.f(3000.f, false))};
        call(a, 2, false, 0, false); out = {{"eax", o.eax}, {"top", o.top}}; break; }
    case kQuadratic: {
        auto av = [&] { const float v = g.uni(0.01f, 1000.f); return g.next() % 2 ? v : -v; };
        const float big = g.next() % 4 ? 1000.f : 1e25f; float x1 = std::bit_cast<float>(0x7F7F1234u), x2 = x1;
        uint32 a[5]{F(g.next() % 10 == 0 ? float(int32(g.next() % 9) - 4) + 0.5f : av()), F(g.f(big, true)), F(g.f(big, true)), uint32(uintptr_t(&x1)), uint32(uintptr_t(&x2))};
        call(a, 5, false, 0, false); out = {{"ret", o.eax & 0xFFu}, {"x1", F(x1)}, {"x2", F(x2)}, {"top", o.top}}; break; }
    case kRandomRange: {
        const float s = g.next() % 4 ? 1000.f : 1e30f; uint32 a[2]{F(g.f(s, false)), F(g.f(s, false))};
        const unsigned pick = g.next() % 8; randomValue = pick == 0 ? 0u : pick == 1 ? 32767u : g.next() & 32767u; randomCalls = 0;
        call(a, 2, false, 0, true); out = {{"st", Hex(o.st.data(), 10)}, {"top", o.top}, {"rng_calls", randomCalls}}; break; }
    case kBikeWorld: case kBikeGame: {
        alignas(16) std::array<uint8, 0x40> b{}; for (auto& x : b) x = uint8(g.next());
        const bool world = fn.kind == kBikeWorld;
        for (int k = 0; k < 4; ++k) { static const int offs[4]{0x14, 0x18, 0x2C, 0x30}; const float v = world ? (g.next() % 3 ? g.uni(-1.2f, 1.2f) : g.f(2.f, true)) : g.f(1000.f, true); std::memcpy(b.data() + offs[k], &v, 4); }
        uint32 a[1]{uint32(uintptr_t(b.data()))};
        call(a, 1, true, uint32(uintptr_t(&gHandlingDataMgr)), false);
        out = {{"buf", Hex(b.data(), b.size())}, {"errno", *ExeErrno()}, {"top", o.top}}; break; }
    case kHandlingGame: {
        alignas(16) std::array<uint8, sizeof(tHandlingData)> b{}; auto* h = reinterpret_cast<tHandlingData*>(b.data()); auto& t = h->GetTransmission();
        auto clampV = [&](float v) { return (v == v && std::fabs(v) <= 1000.f) || v != v ? v : 1000.f; };   // sin infinitos ni enormes: el bucle de reduccion de velocidad no termina con ellos (ni en el EXE)
        h->m_fMass = g.f(20000.f, true); h->m_nPercentSubmerged = uint8(g.next() % 130); h->m_fCollisionDamageMultiplier = g.f(5.f, true);
        h->m_fDragMult = g.next() % 4 ? g.uni(0.f, 0.05f) : g.f(1.f, true); h->m_fBrakeDeceleration = g.f(10.f, true);
        h->m_nVehicleId = g.next() % 5 == 0 ? int32(VT_RCBANDIT) : g.next() % 4 == 0 ? int32(VT_BIKE) : int32(g.next() % 200);
        h->m_bUseMaxspLimit = (g.next() % 3) == 0;
        t.m_EngineAcceleration = clampV(g.f(100.f, true)); t.m_MaxVelocity = clampV(g.f(400.f, true));
        for (int k = 0; k < 64 && HandlingLoopBound(t.m_MaxVelocity) > kMaxLoopIterations; ++k) t.m_MaxVelocity *= 0.5f;   // el bucle del EXE da B/0,01 vueltas, B = [0xC2B9BC] * MaxVelocity
        static const char drives[3]{'4', 'F', 'R'}; t.m_nDriveType = drives[g.next() % 3]; t.m_nNumberOfGears = uint8(1 + g.next() % 5);
        uint32 savedG0 = *reinterpret_cast<uint32*>(0xC1CB30), savedG1 = *reinterpret_cast<uint32*>(0xC1CB34);
        uint32 a[1]{uint32(uintptr_t(b.data()))};
        if (const uint64 bound = HandlingLoopBound(t.m_MaxVelocity); bound > kMaxLoopIterations) { trip("ConvertDataToGameUnits: bucle de velocidad sin cota", bound); out = {{"firewall", true}}; break; }
        call(a, 1, true, uint32(uintptr_t(&gHandlingDataMgr)), false);
        *reinterpret_cast<uint32*>(0xC1CB30) = savedG0; *reinterpret_cast<uint32*>(0xC1CB34) = savedG1;
        out = {{"buf", Hex(b.data(), b.size())}, {"top", o.top}}; break; }
    case kLocalDirection: {
        alignas(16) std::array<uint8, 0x600> ped{}; float rot = g.f(10.f, true);
        if (LocalDirectionLoopBound(rot) > kMaxLoopIterations) rot = -rot;   // +inf y grandes positivos: el EXE no termina; se prueban sus opuestos (-inf, -FLT_MAX...: valor +inf/enorme, sin bucle)
        std::memcpy(ped.data() + 0x558, &rot, 4);
        float point[2]{g.f(100.f, true), g.f(100.f, true)}; if (g.next() % 8 == 0) point[0] = -0.f;
        uint32 a[1]{uint32(uintptr_t(point))};
        { float sent; std::memcpy(&sent, ped.data() + 0x558, 4);
          if (const uint64 bound = LocalDirectionLoopBound(sent); bound > kMaxLoopIterations) { trip("GetLocalDirection: rotacion que no termina el bucle +2pi", bound); out = {{"firewall", true}}; break; } }
        call(a, 1, true, uint32(uintptr_t(ped.data())), false);
        out = {{"eax", o.eax}, {"ecx", o.ecx}, {"edx", o.edx}, {"top", o.top}}; break; }
    case kTransformPoint: {
        alignas(16) std::array<uint8, 64> blk{}; for (auto& x : blk) x = uint8(0xC0u ^ g.next());
        float tr[4]{g.f(1000.f, true), g.f(1000.f, true), g.f(1000.f, true), g.next() % 2 ? g.uni(-100.f, 100.f) : g.f(10.f, true)}; float in[3]{g.f(1000.f, true), g.f(1000.f, true), g.f(1000.f, true)};
        std::memcpy(blk.data() + 16, tr, 16); std::memcpy(blk.data() + 40, in, 12);
        uint32 a[3]{uint32(uintptr_t(blk.data())), uint32(uintptr_t(blk.data() + 16)), uint32(uintptr_t(blk.data() + 40))};
        call(a, 3, false, 0, false); out = {{"blk", Hex(blk.data(), blk.size())}, {"top", o.top}}; break; }
    case kFtol2Float: case kFtol2Double: {
        const bool dbl = fn.kind == kFtol2Double; const float vf = g.f(3000.f, true); const double vd = g.d(); const uint32 ecx = g.next();
        if (!candidate) {
            o = CallFtol2(fn.address, dbl ? std::bit_cast<uint64>(vd) : uint64(F(vf)), dbl, ecx, m); note();
            out = {{"eax", o.eax}, {"ecx", o.ecx}, {"edx", o.edx}};
        } else {
            const Ftol2Regs r = RunDirect(m, modeOk, [&] { return dbl ? Ftol2RegsDblCpp(vd, ecx) : Ftol2RegsCpp(vf, ecx); });
            out = {{"eax", r.eax}, {"ecx", r.ecx}, {"edx", r.edx}};
        }
        break; }
    case kFistp: {
        const float x = g.f(3.0e9f, true);
        if (!candidate) { const uint32 r = ReferenceFistp(x, m, modeOk); out = {{"r", r}}; }
        else { const uint32 r = RunDirect(m, modeOk, [&] { return RwPlCoreFastRealToUInt32Cpp(x); }); out = {{"r", r}}; }
        break; }
    }
    json abi{{"route_ok", routeOk}, {"stack_restored", stackOk}, {"nonvolatile_registers_preserved", regsOk}, {"mode_restored", modeOk}, {"x87_stack_empty", topOk}};
    return {{"out", out}, {"abi", abi}};
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "cppx87precision1-runtime.start")) return;
    if (!log.is_open()) {
        log.open(dir / "cppx87precision1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppx87precision1");
        StartWatchdog();
        Write({{"event", "waiting_for_player"}});
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    std::string only; { std::ifstream f(dir / "cppx87precision1-only.txt"); if (f) std::getline(f, only); }

    savedCw = ReadCw(); savedCsr = _mm_getcsr();
    const int savedErrno = *ExeErrno(); const uint32 savedG0 = *reinterpret_cast<uint32*>(0xC1CB30), savedG1 = *reinterpret_cast<uint32*>(0xC1CB34);
    std::array<ReversibleHooks::ReversibleHook::Base*, fns.size()> hooks{}; std::array<bool, fns.size()> enabled{}, locked{};
    for (size_t f = 0; f < fns.size(); ++f) {
        if (fns[f].address == 0 || fns[f].kind >= kFtol2Float) continue;
        if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({{"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fns[f].cls) + "::" + fns[f].name}}); return; }
        enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false);
    }
    const bool directOk = !ReversingProbe::JumpsToDll(0x821B40);   // _ftol2 del EXE sin hook
    Write({{"event", "begin"}, {"functions", fns.size()}, {"combinations", kPcCount * kRcCount}, {"cases_per_combination", 1024}, {"pairs", fns.size() * kPcCount * kRcCount * 1024},
           {"fp_control_saved", savedCw}, {"mxcsr_saved", savedCsr}, {"ftol2_unhooked", directOk}, {"collaborators_stubbed", json::array({"rand 0x821B1E", "CGeneral::GetRandomNumber"})}});
    if (!directOk) { Write({{"event", "error"}, {"reason", "_ftol2 hookeado"}}); return; }

    bool passed = true; unsigned pair = 0;
    auto setAll = [&](bool on) { for (auto* h : hooks) if (h) h->State(on); };
    {
        ReversingProbe::EntryPatch exeRng(0x821B1E, FakeRandom), dllRng(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), FakeRandom);
        for (size_t f = 0; f < fns.size(); ++f) {
            const auto& fn = fns[f]; const std::string full = std::string(fn.cls) + "::" + fn.name; unsigned failuresRequired = 0, exploratory = 0;
            if (!only.empty() && only != full) { pair += kPcCount * kRcCount * fn.cases; continue; }
            for (unsigned pc = 0; pc < kPcCount; ++pc) for (unsigned rc = 0; rc < kRcCount; ++rc) {
                guardPc = kPcBits[pc]; guardRc = rc;
                const Modes m = MakeModes(pc, rc); const bool required = (fn.mask >> pc) & 1u;
                std::vector<json> original(fn.cases), candidate(fn.cases);
                setAll(false);   // todos los hooks del banco a la vez: GetNodeHeadingFromVector y GetAngleBetweenPoints llaman a GetRadianAngleBetweenPoints
                for (int i = 0; i < fn.cases; ++i) original[i] = RunCase(fn, unsigned(f), unsigned(i), m, false, hooks[f]);
                setAll(true);
                for (int i = 0; i < fn.cases; ++i) candidate[i] = RunCase(fn, unsigned(f), unsigned(i), m, true, hooks[f]);
                setAll(false);
                for (int i = 0; i < fn.cases; ++i, ++pair) {
                    const bool equal = original[i]["out"] == candidate[i]["out"];
                    auto abiOk = [](const json& side) { for (auto& [k, v] : side["abi"].items()) if (v != true) return false; return true; };
                    const bool accepted = equal && abiOk(original[i]) && abiOk(candidate[i]);
                    if (!accepted) { if (required) { ++failuresRequired; passed = false; } else ++exploratory; }
                    // las filas aceptadas llevan el resultado completo (cabe: 12288 por funcion); las rechazadas, ademas, los dos lados
                    Write({{"event", "comparison"}, {"pair", pair}, {"function", full}, {"case", i}, {"pc", kPcBits[pc]}, {"rc", kRcNames[rc]}, {"required", required},
                           {"equal", equal}, {"accepted", accepted}, {"state_restored", true}, {"original", original[i]}, {"candidate", candidate[i]}});
                }
            }
            Write({{"event", "function_summary"}, {"function", full}, {"cases", fn.cases * kPcCount * kRcCount}, {"mask", fn.mask}, {"failures", failuresRequired}, {"exploratory_diffs", exploratory}});
        }
    }
    // Restauracion y comprobacion final de todo el estado tocado
    for (size_t f = 0; f < fns.size(); ++f) if (hooks[f]) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    *ExeErrno() = savedErrno; *reinterpret_cast<uint32*>(0xC1CB30) = savedG0; *reinterpret_cast<uint32*>(0xC1CB34) = savedG1;
    bool restored = CwSame(ReadCw(), savedCw) && (_mm_getcsr() & 0xFFC0u) == (savedCsr & 0xFFC0u) && ReadTop() == 0 && *ExeErrno() == savedErrno
                 && *reinterpret_cast<uint32*>(0xC1CB30) == savedG0 && *reinterpret_cast<uint32*>(0xC1CB34) == savedG1;
    for (size_t f = 0; f < fns.size(); ++f) if (hooks[f]) restored &= hooks[f]->Hooked() == enabled[f] && hooks[f]->Locked() == locked[f];
    Write({{"event", passed && restored && firewallTrips == 0 ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored}, {"firewall_trips", firewallTrips.load()}});
}
}
