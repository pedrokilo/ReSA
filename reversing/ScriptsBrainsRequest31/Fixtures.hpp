#pragma once
// Fixtures del banco ScriptsBrainsRequest31: CScriptsForBrains::RequestAttractorScriptBrainWithThisName en 0x46AA80
// (thiscall, 1 argumento en pila: name; `ret 4`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y se compilan solas con -DFIXTURE_DUMP (cl) para volcar las 4096 filas `input`
// y comprobar las constantes y la cobertura contra fixture_oracle.py (selftest). Cada fila `input` lleva todo lo que el oraculo necesita.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace ScriptsBrainsRequest31Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u64 Fnv64(const u8* p, std::size_t n) {
    u64 h = 0xCBF29CE484222325ull;
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; }
    return h;
}
inline std::string Hex64(u64 v) { char b[20]; std::snprintf(b, sizeof b, "%016llx", static_cast<unsigned long long>(v)); return b; }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// RNG del EXE (rand 0x821B1E): estado' = estado * 0x343FD + 0x269EC3; salida = (estado' >> 16) & 0x7FFF. srand del EXE = 0x821B11.
constexpr u32 NextState(u32 x) { return x * 0x343FDu + 0x269EC3u; }
constexpr u32 RandOutput(u32 state) { return (NextState(state) >> 16) & 0x7FFFu; }   // primera salida de rand() desde `state`
constexpr u32 RollOf(u32 r) { return (r * 100u) >> 15; }                              // (int)(r * 2^-15 * 100.0f): r * 100 < 2^22, exacto en float

inline std::string Num(long long v) { return std::to_string(v); }

// ------------------------------------------------------------------------------------------------ disposicion de memoria
// `this` = tabla de 70 cerebros de 0x14 bytes dentro de una arena con 0x40 bytes de guarda a cada lado; `name` = cadena en otra arena con 0x20 de guarda a cada lado
// (la funcion solo reenvia el puntero a GetIndexOfScriptBrainWithThisName, sustituido por un falso).
constexpr u32 kBrains = 70, kBrainBytes = 20;
constexpr u32 kThisOffset = 0x40, kThisBytes = kBrains * kBrainBytes, kThisArena = kThisOffset + kThisBytes + 0x40;   // 0x40 + 0x578 + 0x40 = 0x5F8
constexpr u32 kNameOffset = 0x20, kNameBytes = 0x20, kNameArena = kNameOffset + kNameBytes + 0x20;                    // 0x20 + 0x20 + 0x20 = 0x60
// Memoria REAL del juego que toca la funcion (se guarda y se restaura en cada caso):
//   CTheScripts::StreamedScripts.m_aScripts: 82 entradas de 32 bytes desde 0xA47B60; m_NumberOfUsers (uint8) en +4 de cada entrada (0xA47B64).
//   CStreaming::ms_aInfoForModel: el EXE lee el estado de carga de la entrada de modelo 0x6676 + script en 0x964E08 + script * 20 (ventana de 82 * 20 bytes).
constexpr u32 kScripts = 82, kScriptStride = 32, kUsersAt = 4, kInfoStride = 20;
constexpr u32 kScriptWin = kScripts * kScriptStride, kInfoWin = kScripts * kInfoStride;                                   // 0xA40, 0x668
constexpr u32 kScriptWinAddr = 0xA47B60, kInfoWinAddr = 0x964E08, kModelBase = 0x6676;
static_assert(kThisBytes == 0x578 && kThisArena == 0x5F8 && kNameArena == 0x60 && kScriptWin == 0xA40 && kInfoWin == 0x668);
constexpr u32 kBrainScriptAt = 0;   // int16 m_StreamedScriptIndex

inline constexpr std::array<u32, 8> kNegIdx{0xFFFF, 0x8000, 0xFFFE, 0xFF00, 0x8001, 0x9000, 0xFFF0, 0xC000};
inline constexpr std::array<u8, 14> kStates{0, 1, 1, 1, 2, 3, 4, 5, 0x11, 0x80, 0x81, 0xFF, 0x7F, 0x10};
inline constexpr std::array<u8, 8> kUsersSpecial{0, 1, 2, 0x7F, 0x80, 0xFE, 0xFF, 0x10};

struct F {
    u32 kind, eax, edx, stack, retEax;      // retEax: valor completo de EAX que devuelve el falso de GetIndex (el original solo mira AX)
    u32 idx16;                              // AX del retorno
    u32 script;                             // script seleccionado (0..81); -1 si no hay
    u8 brains[kThisBytes];
    u8 name[kNameBytes];
    u8 users[kScripts];                     // m_NumberOfUsers de cada script antes de la llamada
    u8 state[kScripts];                     // estado de carga de cada modelo de script antes de la llamada
    u64 thisFnv, nameFnv;
};

inline u64 ArenaFnv(const u8* body, u32 offset, u32 bytes, u32 arena) {
    std::vector<u8> a(arena, 0xC3);
    std::memcpy(a.data() + offset, body, bytes);
    return Fnv64(a.data(), arena);
}
inline void Put16(u8* p, u32 v) { const u16 w = u16(v); std::memcpy(p, &w, 2); }
inline u16 Get16(const u8* p) { u16 v; std::memcpy(&v, p, 2); return v; }

// kind (n % 16): 0 indice -1; 1 indice negativo (bit 15) distinto de -1; 2 script cargado (estado 1); 3 estado 0; 4 estados 2..4; 5 estados no canonicos (5..255 salvo 1);
//   6 usuarios 255 (desborda a 0); 7 usuarios 0/1/127/128/254; 8 indice de cerebro 0 o 69; 9 script 0 u 81; 10 vecinos (script +-1) con estados/usuarios contrarios;
//   11 todo aleatorio; 12 EAX del falso con basura en los 16 bits altos; 13 cerebro con tipo != 5 (el falso decide: solo cuenta el indice); 14 estado 1 con usuarios 255; 15 aleatorio.
inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.kind = n % 16;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.brains, kThisBytes, c ^ 0x5A5Au);
    FillWords(f.name, kNameBytes, c ^ 0xA5A5u);
    for (u32 i = 0; i < kBrains; ++i) Put16(f.brains + i * kBrainBytes + kBrainScriptAt, Rand(c, 200 + i) % kScripts);   // todos con scripts validos distintos del azar
    for (u32 e = 0; e < kScripts; ++e) {
        f.users[e] = (Rand(c, 300 + e) % 5 == 0) ? kUsersSpecial[Rand(c, 400 + e) % kUsersSpecial.size()] : u8(Rand(c, 500 + e));
        f.state[e] = (Rand(c, 600 + e) % 3 == 0) ? kStates[Rand(c, 700 + e) % kStates.size()] : u8(Rand(c, 800 + e) % 6 == 0 ? Rand(c, 900 + e) : 1);
    }
    const bool neg = f.kind <= 1;
    u32 idx = f.kind == 8 ? ((Rand(c, 1) & 1) ? 0u : 69u) : Rand(c, 1) % kBrains;
    u32 s = f.kind == 9 ? ((Rand(c, 2) & 1) ? 0u : 81u) : Rand(c, 2) % kScripts;
    f.idx16 = neg ? (f.kind == 0 ? 0xFFFFu : kNegIdx[Rand(c, 3) % kNegIdx.size()]) : idx;
    f.script = neg ? 0xFFFFFFFFu : s;
    f.retEax = f.kind == 12 ? ((Rand(c, 4) | 1u) << 16 | f.idx16) : ((Rand(c, 5) & 1) ? (Rand(c, 6) << 16 | f.idx16) : f.idx16);
    if (!neg) {
        Put16(f.brains + idx * kBrainBytes + kBrainScriptAt, s);
        f.brains[idx * kBrainBytes + 2] = (f.kind == 13) ? u8(Rand(c, 7)) : u8(5);   // tipo del cerebro (no lo mira la funcion)
        switch (f.kind) {
        case 2: f.state[s] = 1; break;
        case 3: f.state[s] = 0; break;
        case 4: f.state[s] = u8(2 + Rand(c, 8) % 3); break;
        case 5: { u8 v = u8(Rand(c, 8)); if (v == 1) v = 0xFF; f.state[s] = v; break; }
        case 6: f.users[s] = 255; break;
        case 7: f.users[s] = kUsersSpecial[Rand(c, 9) % 5]; break;
        case 10:
            if (s > 0) { f.users[s - 1] = 0xFF; f.state[s - 1] = (f.state[s] == 1) ? u8(0) : u8(1); }
            if (s < kScripts - 1) { f.users[s + 1] = 0xFF; f.state[s + 1] = (f.state[s] == 1) ? u8(0) : u8(1); }
            break;
        case 14: f.state[s] = 1; f.users[s] = 255; break;
        default: break;
        }
    }
    f.thisFnv = ArenaFnv(f.brains, kThisOffset, kThisBytes, kThisArena);
    f.nameFnv = ArenaFnv(f.name, kNameOffset, kNameBytes, kNameArena);
    return f;
}

inline std::string ArrayJson(const u8* p, u32 n) {
    std::string s = "[";
    for (u32 i = 0; i < n; ++i) { if (i) s += ','; s += Num(p[i]); }
    return s + ']';
}
inline std::string InputJson(const F& f) {
    std::string s = "{";
    auto kv = [&](const char* k, long long v) { s += '"'; s += k; s += "\":"; s += Num(v); s += ','; };
    kv("kind", f.kind); kv("eax", f.eax); kv("edx", f.edx); kv("stack_seed", f.stack); kv("ret_eax", f.retEax); kv("idx16", f.idx16);
    s += "\"brain_script\":[";
    for (u32 i = 0; i < kBrains; ++i) { if (i) s += ','; s += Num(Get16(f.brains + i * kBrainBytes + kBrainScriptAt)); }
    s += "],\"users\":" + ArrayJson(f.users, kScripts) + ",\"state\":" + ArrayJson(f.state, kScripts);
    s += ",\"this_fnv\":\"" + Hex64(f.thisFnv) + "\",\"name_fnv\":\"" + Hex64(f.nameFnv) + "\"}";
    return s;
}
}  // namespace ScriptsBrainsRequest31Fix

#ifdef FIXTURE_DUMP
int main() {
    using namespace ScriptsBrainsRequest31Fix;
    std::printf("{\"consts\":{\"cases\":%u,\"this_offset\":%u,\"this_bytes\":%u,\"this_arena\":%u,\"name_offset\":%u,\"name_bytes\":%u,\"name_arena\":%u,\"scripts\":%u,\"script_stride\":%u,\"users_at\":%u,\"info_stride\":%u,\"script_win\":%u,\"info_win\":%u,\"script_win_addr\":%u,\"info_win_addr\":%u,\"model_base\":%u}}\n",
                kCases, kThisOffset, kThisBytes, kThisArena, kNameOffset, kNameBytes, kNameArena, kScripts, kScriptStride, kUsersAt, kInfoStride, kScriptWin, kInfoWin, kScriptWinAddr, kInfoWinAddr, kModelBase);
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"case\":%u,\"input\":%s}\n", n, InputJson(MakeF(n)).c_str());
    return 0;
}
#endif
