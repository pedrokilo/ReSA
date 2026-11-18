#pragma once
// Fixtures del banco Fn239A (lote 239A): 16384 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego (fixture_dump.cpp).
//
// 1 funcion del EXE enganchada SIN validar: despachador de comandos de script de CRunningScript (thiscall, un argumento de pila = numero de comando, `ret 4`, resultado en AL):
//   idx  direccion  funcion                                          comandos
//    0   0x481300   CRunningScript::ProcessCommands800To899         801..899
//
// Caso n: comando kSchedule[n % kBand] (los 99 comandos 801..899, algunos repetidos, y 5 comandos fuera de rango que el EXE rechaza con -1).
// El resto del caso (semilla, parametros de script segun fixture_types.py, reserva nula de tareas, semillas de pila y registros) sale de un mezclador fijo: el oraculo (emu_oracle.py) lee las filas tal cual.
// Familia = n % 16: solo cambia la semilla con la que se llena la pila bajo ESP y los registros EAX/EDX de entrada (cobertura de residuos de pila y registros).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "GenTables.hpp"

namespace F239AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
constexpr unsigned kBand = sizeof(F239AGen::kSchedule) / sizeof(F239AGen::kSchedule[0]);

inline u32 Mix(u32 x) {
    x ^= x >> 16; x *= 0x85EBCA6Bu; x ^= x >> 13; x *= 0xC2B2AE35u; x ^= x >> 16;
    return x;
}
inline u32 Hash(u32 seed, u32 k) { return Mix(seed ^ (k * 0x9E3779B1u)); }

struct C {
    unsigned fn{}, n{}, family{};
    i32 cmd{};
    u32 seed{}, stack{}, eax{}, edx{};
    int na{};
    std::array<u32, 32> params{};
};

inline constexpr u32 kStackSeeds[16] = {0xA5A5A5A5u, 0u, 0xFFFFFFFFu, 0xCDCDCDCDu, 0x12345678u, 0xDEADBEEFu, 0x00FF00FFu, 0x80000001u, 0x0F0F0F0Fu, 0x7FFFFFFFu, 0x31415926u, 0xFEDCBA98u, 0x01010101u, 0xC0FFEE00u, 0x55555555u, 0xAAAAAAAAu};

inline bool NullAllocOk(i32 cmd) {
    for (u32 c : F239AGen::kNullAllocOk) if (u32(cmd) == c) return true;
    return false;
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n; c.family = n % kFamilies;
    const unsigned k = n % kBand;
    c.cmd = i32(F239AGen::kSchedule[k]);
    c.seed = Mix(n * 0x9E3779B1u + fn * 0x85EBCA6Bu + 17u);
    c.stack = kStackSeeds[c.family];
    c.eax = Hash(c.seed, 900); c.edx = Hash(c.seed, 901);
    c.na = NullAllocOk(c.cmd) && (Hash(c.seed, 902) & 1);
    static constexpr u32 kSmall[8] = {0xFFFFFFFEu, 0xFFFFFFFFu, 0u, 1u, 2u, 3u, 0x7FFFFFFFu, 0x80000000u};
    static constexpr u32 kFl[12] = {0x00000000u, 0x3F800000u, 0xBF800000u, 0x40490FDBu, 0xC0490FDBu, 0x42B40000u, 0x43B40000u, 0xC2C80000u, 0x41200000u, 0x3DCCCCCDu, 0x3F000000u, 0xC2C80000u};
    static constexpr u32 kHand[3][2] = {{0x0005u, 0x0106u}, {0x0003u, 0x0104u}, {0x0009u, 0x010Au}};      // ped, vehiculo, objeto: manejadores validos
    static constexpr u32 kBad[3][2] = {{0x0006u, 0x0105u}, {0x0004u, 0x0103u}, {0x000Au, 0x0109u}};       // mismo indice, byte de estado distinto
    const char* types = (c.cmd >= F239AGen::kBases[fn] && c.cmd < F239AGen::kBases[fn] + int(F239AGen::kNCmd)) ? F239AGen::kCmdTypes[fn][c.cmd - F239AGen::kBases[fn]] : "";
    unsigned idx = 0;
    for (const char* p = types; *p;) {
        const char* q = p;
        while (*q && *q != ',') ++q;
        const std::string t(p, q);
        const u32 h = Hash(c.seed, 100 + idx);
        auto kindOf = [](char ch) { return ch == 'P' ? 0 : ch == 'V' ? 1 : 2; };
        u32 v = 0;
        if (t == "P" || t == "V" || t == "O") v = kHand[kindOf(t[0])][h & 1];
        else if (t == "Pn" || t == "Vn" || t == "On") { const unsigned r = (h >> 1) % 5; v = r < 2 ? kHand[kindOf(t[0])][r] : r < 4 ? kBad[kindOf(t[0])][r - 2] : kBad[kindOf(t[0])][h & 1]; }
        else if (t == "Pm" || t == "Vm") { const unsigned r = (h >> 1) % 3; v = r < 2 ? kHand[kindOf(t[0])][r] : 0xFFFFFFFFu; }
        else if (t == "Pq") v = ((h >> 1) % 5) < 4 ? kHand[0][h & 1] : kBad[0][h & 1];
        else if (t == "n") { static constexpr u32 b[4] = {0, 0, 1, 0}; v = b[h & 3]; }
        else if (t == "i") v = kSmall[h & 7];
        else if (t == "I") v = Mix(h);
        else if (t == "f") v = kFl[(h >> 3) % 12];
        else if (t == "b") { static constexpr u32 b[4] = {0, 1, 2, 0x100}; v = b[h & 3]; }
        else if (t == "t") v = (h >> 3) & 1;
        else if (t == "M") { static constexpr u32 m[8] = {0u, 1u, 5u, 100u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFDu, 0xFFFFFFF9u}; v = m[(h >> 3) & 7]; }
        else if (t == "q") { static constexpr u32 m[8] = {0xFFFFFFFFu, 0u, 1u, 2u, 3u, 0xFFFFFFFEu, 0xFFFFFFFFu, 1u}; v = m[(h >> 3) & 7]; }
        else if (t == "a") v = F239AGen::kAngles[(h >> 3) % (sizeof(F239AGen::kAngles) / 4)];
        else if (t == "s") v = F239AGen::kSpeeds[(h >> 3) % (sizeof(F239AGen::kSpeeds) / 4)];
        else if (t == "g") { static constexpr u32 g[8] = {0u, 0u, 0x3F800000u, 0x3F800000u, 0u, 0x3F800000u, 0xBF800000u, 0x40000000u}; v = g[(h >> 3) & 7]; }
        else std::abort();
        c.params[idx++] = v;
        p = *q ? q + 1 : q;
    }
    for (unsigned j = idx; j < 32; ++j) c.params[j] = Hash(c.seed, 200 + j);
    return c;
}

inline std::string JsonC(const C& c) {
    std::string s;
    char b[96];
    std::snprintf(b, sizeof b, "{\"fn\":%u,\"n\":%u,\"family\":%u,\"cmd\":%d,\"seed\":%u,", c.fn, c.n, c.family, c.cmd, c.seed);
    s += b;
    std::snprintf(b, sizeof b, "\"stack\":%u,\"eax\":%u,\"edx\":%u,\"na\":%d,\"params\":[", c.stack, c.eax, c.edx, c.na);
    s += b;
    for (unsigned i = 0; i < 32; ++i) { std::snprintf(b, sizeof b, i ? ",%u" : "%u", c.params[i]); s += b; }
    s += "]}";
    return s;
}
}
