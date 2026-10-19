#pragma once
// Fixtures del banco ScriptsBrainsCheckNew31: CScriptsForBrains::CheckIfNewEntityNeedsScript en 0x46FF20
// (thiscall, 3 argumentos en pila: entity, attachType (int8, el EXE lee BL), unused; `ret 0Ch`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y se compilan solas con -DFIXTURE_DUMP (cl) para volcar las 4096 filas `input`
// y comprobar las constantes y la cobertura contra fixture_oracle.py (selftest). Cada fila `input` lleva todo lo que el oraculo necesita.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace ScriptsBrainsCheckNew31Fix {
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
// `this` = tabla de 70 cerebros de 0x14 bytes (tScriptForBrains) dentro de una arena con 0x40 bytes de guarda a cada lado.
constexpr u32 kBrains = 70, kBrainBytes = 20;
constexpr u32 kThisOffset = 0x40, kThisBytes = kBrains * kBrainBytes, kThisArena = kThisOffset + kThisBytes + 0x40;   // 0x40 + 0x578 + 0x40 = 0x5F8
// entidad (ped u objeto): se reserva el tamano del slot del pool de peds (0x7C4); el EXE lee +0x22 (modelo, u16), +0x140 (flags de objeto) y +0x474 (flags de ped)
constexpr u32 kEntOffset = 0x40, kEntBytes = 0x7C4, kEntArena = kEntOffset + kEntBytes + 0x40;                      // 0x40 + 0x7C4 + 0x40 = 0x844
static_assert(kThisBytes == 0x578 && kThisArena == 0x5F8 && kEntArena == 0x844);
constexpr u32 kBrainTypeAt = 2, kBrainModelAt = 0xC, kBrainPctAt = 0xE;
constexpr u32 kEntModelAt = 0x22, kEntObjFlagsAt = 0x140, kEntPedFlagsAt = 0x474;
constexpr u32 kPedBits = 0x1800000u, kObjBits = 0x300000u;   // ped: 0x800000 (tiene cerebro) | 0x1000000 (esperando carga); objeto: 0x100000 | 0x200000

inline constexpr std::array<u16, 16> kModels{0, 1, 0x7FFF, 0x8000, 0xFFFF, 0x00FF, 0x0100, 0x0101, 0x8001, 0x7F7F, 0xFF00, 2, 0x1234, 0xFFFE, 0x4000, 0x00C0};
inline constexpr std::array<u8, 14> kObjAttach{1, 1, 1, 2, 3, 4, 5, 0x7F, 0x80, 0xFF, 0x40, 0x10, 0xFE, 0x81};
inline constexpr std::array<u16, 8> kBigPct{0x8000, 0xFFFF, 0x7FFF, 0x8001, 101, 0x0100, 0xFF00, 100};

struct F {
    u32 kind, seed, eax, edx, stack, attachArg, unusedArg;
    u8 attach;                 // byte bajo de attachArg (int8 del original); los 24 bits altos de attachArg son basura
    u32 model, objFlags, pedFlags;
    u8 brains[kThisBytes];
    u8 ent[kEntBytes];
    u64 thisFnv, entFnv;
};

inline u64 ArenaFnv(const u8* body, u32 offset, u32 bytes, u32 arena) {
    std::vector<u8> a(arena, 0xC3);
    std::memcpy(a.data() + offset, body, bytes);
    return Fnv64(a.data(), arena);
}
inline u16 Get16(const u8* p) { u16 v; std::memcpy(&v, p, 2); return v; }
inline u32 Get32(const u8* p) { u32 v; std::memcpy(&v, p, 4); return v; }
inline void Put16(u8* p, u32 v) { const u16 w = u16(v); std::memcpy(p, &w, 2); }
inline void Put32(u8* p, u32 v) { std::memcpy(p, &v, 4); }

// kind (n % 16): 0 ped con 0x800000 (retorno previo); 1 ped con 0x1000000; 2 ped sin coincidencias; 3 ped, una coincidencia que acierta; 4 ped, una que falla (umbral);
//   5 ped, 2-4 coincidencias, acierta la j-esima; 6 ped, 2-4 coincidencias, ninguna acierta (recorre los 70); 7 objeto con 0x100000/0x200000/ambos (retorno previo);
//   8 objeto attach 1; 9 objeto attach distinto de 1; 10 coincidencias en los indices 0 y 69; 11 modelos limite y casi-iguales; 12 porcentajes >= 0x8000 (sin signo);
//   13 solo tipos distintos (modelo igual, porcentaje 100); 14 aleatorio libre; 15 la region de flags del otro tipo vale lo contrario (se ignora).
// Las tiradas se colocan sobre la semilla real: pct == roll falla (limite), pct == roll + 1 acierta (limite).
inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.kind = n % 16;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n); f.seed = Rand(c, 1); f.unusedArg = Rand(c, 2);
    FillWords(f.brains, kThisBytes, c ^ 0x5A5Au);
    FillWords(f.ent, kEntBytes, c ^ 0xA5A5u);

    u8 A = 0;
    switch (f.kind) {
    case 7: A = kObjAttach[Rand(c, 4) % kObjAttach.size()]; break;
    case 8: A = 1; break;
    case 9: A = kObjAttach[3 + Rand(c, 4) % (kObjAttach.size() - 3)]; break;
    case 12: case 13: case 15: A = (Rand(c, 3) & 1) ? kObjAttach[Rand(c, 4) % kObjAttach.size()] : 0; break;
    case 14: A = u8(Rand(c, 4)); if ((Rand(c, 3) & 3) == 0) A = 0; break;
    default: break;
    }
    const bool obj = A != 0;
    f.attach = A;
    f.attachArg = ((Rand(c, 5) & 1) ? (Rand(c, 6) << 8) : 0u) | A;    // 24 bits altos: basura (el original los ignora; solo se usa BL)

    int m = 0, stop = -1;          // m coincidencias; stop = indice (en orden de recorrido) de la que acierta, -1 ninguna
    u32 pedBits = 0, objBits = 0;  // bits que se fuerzan en la region que SI se mira (objBits: bit0 = 0x100000, bit1 = 0x200000)
    switch (f.kind) {
    case 0: pedBits = 0x800000u | ((Rand(c, 7) & 1) ? 0x1000000u : 0u); m = 1 + int(Rand(c, 8) % 3); stop = 0; break;
    case 1: pedBits = 0x1000000u; m = 1 + int(Rand(c, 8) % 3); stop = 0; break;
    case 2: m = 0; break;
    case 3: m = 1; stop = 0; break;
    case 4: m = 1; stop = -1; break;
    case 5: m = 2 + int(Rand(c, 8) % 3); stop = int(Rand(c, 9) % u32(m)); break;
    case 6: m = 2 + int(Rand(c, 8) % 3); stop = -1; break;
    case 7: objBits = 1 + Rand(c, 7) % 3; m = 1 + int(Rand(c, 8) % 3); stop = 0; break;
    case 8: case 9: m = 1 + int(Rand(c, 8) % 3); stop = (Rand(c, 9) % 4 == 0) ? -1 : int(Rand(c, 10) % u32(m)); break;
    case 10: m = 2; stop = (Rand(c, 9) % 3 == 0) ? -1 : int(Rand(c, 10) % 2); break;
    case 11: m = 1; stop = 0; break;
    case 12: m = 1; stop = 0; break;
    case 13: m = 0; break;
    case 14: m = 0; break;      // las coincidencias de este tipo salen del azar por cerebro (ver abajo)
    default: m = 1; stop = 0; break;
    }
    // modelo de la entidad
    const u16 M = (f.kind == 11 || (Rand(c, 12) % 4 == 0)) ? kModels[Rand(c, 13) % kModels.size()] : u16(Rand(c, 13));
    f.model = M;
    // flags: la region que se mira lleva los bits pedidos (kind 14: se dejan los aleatorios); la otra queda aleatoria completa (kind 15: con los bits "tentadores")
    u32 pedFlags = Get32(f.ent + kEntPedFlagsAt), objFlags = Get32(f.ent + kEntObjFlagsAt);
    if (f.kind != 14) {
        if (!obj) {
            pedFlags = (pedFlags & ~kPedBits) | pedBits;
            if (f.kind == 15) objFlags |= kObjBits;
        } else {
            objFlags = (objFlags & ~kObjBits) | ((objBits & 1) ? 0x100000u : 0u) | ((objBits & 2) ? 0x200000u : 0u);
            if (f.kind == 15) pedFlags |= kPedBits;
        }
    }
    f.pedFlags = pedFlags; f.objFlags = objFlags;
    Put16(f.ent + kEntModelAt, M); Put32(f.ent + kEntObjFlagsAt, objFlags); Put32(f.ent + kEntPedFlagsAt, pedFlags);

    // posiciones de las coincidencias (distintas, en orden de recorrido)
    bool isMatch[kBrains]{};
    int pos[8]{};
    if (f.kind == 10) { pos[0] = 0; pos[1] = 69; }
    else {
        int perm[kBrains];
        for (u32 i = 0; i < kBrains; ++i) perm[i] = int(i);
        for (int i = int(kBrains) - 1; i > 0; --i) { const int j = int(Rand(c, 100 + u32(i)) % u32(i + 1)); const int t = perm[i]; perm[i] = perm[j]; perm[j] = t; }
        for (int q = 0; q < m; ++q) pos[q] = perm[q];
        for (int i = 0; i < m; ++i) for (int j = i + 1; j < m; ++j) if (pos[j] < pos[i]) { const int t = pos[i]; pos[i] = pos[j]; pos[j] = t; }
    }
    for (int q = 0; q < m; ++q) isMatch[pos[q]] = true;

    auto setBrain = [&](u32 i, u8 type, u16 model, u16 pct) {
        u8* b = f.brains + i * kBrainBytes;
        b[kBrainTypeAt] = type; Put16(b + kBrainModelAt, model); Put16(b + kBrainPctAt, pct);
    };
    auto temptingPct = [&](u32 k) -> u16 {
        switch (Rand(c, k) % 5) { case 0: return 100; case 1: return 0xFFFF; case 2: return 0x8000; case 3: return 0; default: return u16(Rand(c, k + 1)); }
    };
    if (f.kind == 14) {
        for (u32 i = 0; i < kBrains; ++i) {
            const u8 t = (Rand(c, 400 + i) % 3 == 0) ? A : u8(Rand(c, 500 + i));
            const u16 md = (Rand(c, 600 + i) % 3 == 0) ? M : u16(Rand(c, 700 + i));
            setBrain(i, t, md, temptingPct(800 + i * 2));
        }
    } else {
        // tiradas reales: cada coincidencia consume una hasta que una acierta
        u32 roll[8]{};
        u32 s = f.seed;
        for (int q = 0; q < m; ++q) { s = NextState(s); roll[q] = RollOf((s >> 16) & 0x7FFFu); }
        int q = 0;
        for (u32 i = 0; i < kBrains; ++i) {
            if (isMatch[i]) {
                u16 pct;
                if (stop >= 0 && q == stop) {
                    switch (f.kind == 12 ? 1u : Rand(c, 900 + u32(q)) % 4) {
                    case 0: pct = u16(roll[q] + 1); break;                                                       // limite: roll < pct es cierto
                    case 1: pct = kBigPct[Rand(c, 910 + u32(q)) % kBigPct.size()]; break;
                    case 2: pct = u16(roll[q] + 1 + Rand(c, 920 + u32(q)) % (0xFFFFu - roll[q])); break;
                    default: pct = 100; break;
                    }
                } else if (stop >= 0 && q > stop) {
                    pct = (Rand(c, 930 + u32(q)) & 1) ? kBigPct[Rand(c, 940 + u32(q)) % kBigPct.size()] : u16(Rand(c, 950 + u32(q)));
                } else {
                    switch (Rand(c, 960 + u32(q)) % 4) {
                    case 0: pct = u16(roll[q]); break;                                                           // limite: roll < pct es falso
                    case 1: pct = 0; break;
                    case 2: pct = u16(Rand(c, 970 + u32(q)) % (roll[q] + 1)); break;
                    default: pct = roll[q] ? u16(roll[q] - 1) : u16(0); break;
                    }
                }
                setBrain(i, A, M, pct);
                ++q;
            } else {
                u32 mode = Rand(c, 1000 + i) % 3;
                if (f.kind == 13) mode = 0;
                if (f.kind == 11 && mode == 0) mode = 1;
                u8 type = A; u16 model = M;
                if (mode == 0) {
                    type = u8(A + 1 + Rand(c, 1100 + i) % 255);                       // != A (suma modulo 256 de 1..255)
                    model = (f.kind == 13 || (Rand(c, 1200 + i) & 1)) ? M : u16(Rand(c, 1300 + i));
                } else if (mode == 1) {
                    switch (Rand(c, 1400 + i) % 4) {
                    case 0: model = u16(M ^ (1u << (Rand(c, 1500 + i) % 16))); break;
                    case 1: model = u16(M + 1); break;
                    case 2: model = u16(~M); break;
                    default: model = u16(M ^ (0x100u << (Rand(c, 1600 + i) % 8))); break;
                    }
                } else {
                    type = u8(Rand(c, 1700 + i)); model = u16(Rand(c, 1800 + i));
                    if (type == A && model == M) model = u16(M ^ 1u);
                }
                setBrain(i, type, model, f.kind == 13 ? u16(100) : temptingPct(1900 + i * 2));
            }
        }
    }
    f.thisFnv = ArenaFnv(f.brains, kThisOffset, kThisBytes, kThisArena);
    f.entFnv = ArenaFnv(f.ent, kEntOffset, kEntBytes, kEntArena);
    return f;
}

// Fila `input` (la misma que escribe el banco); el oraculo trabaja solo con ella.
inline std::string InputJson(const F& f) {
    std::string s = "{";
    auto kv = [&](const char* k, long long v) { s += '"'; s += k; s += "\":"; s += Num(v); s += ','; };
    kv("kind", f.kind); kv("seed", f.seed); kv("eax", f.eax); kv("edx", f.edx); kv("stack_seed", f.stack); kv("attach_arg", f.attachArg); kv("unused", f.unusedArg);
    kv("ent_model", f.model); kv("ent_obj_flags", f.objFlags); kv("ent_ped_flags", f.pedFlags);
    s += "\"brains\":[";
    for (u32 i = 0; i < kBrains; ++i) {
        const u8* b = f.brains + i * kBrainBytes;
        if (i) s += ',';
        s += '[' + Num(b[kBrainTypeAt]) + ',' + Num(Get16(b + kBrainModelAt)) + ',' + Num(Get16(b + kBrainPctAt)) + ']';
    }
    s += "],\"this_fnv\":\"" + Hex64(f.thisFnv) + "\",\"ent_fnv\":\"" + Hex64(f.entFnv) + "\"}";
    return s;
}
}  // namespace ScriptsBrainsCheckNew31Fix

#ifdef FIXTURE_DUMP
int main() {
    using namespace ScriptsBrainsCheckNew31Fix;
    std::printf("{\"consts\":{\"cases\":%u,\"this_offset\":%u,\"this_bytes\":%u,\"this_arena\":%u,\"ent_offset\":%u,\"ent_bytes\":%u,\"ent_arena\":%u}}\n",
                kCases, kThisOffset, kThisBytes, kThisArena, kEntOffset, kEntBytes, kEntArena);
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"case\":%u,\"input\":%s}\n", n, InputJson(MakeF(n)).c_str());
    return 0;
}
#endif
