#pragma once
// Fixtures del banco ScriptsBrainsStart31: CScriptsForBrains::StartNewStreamedScriptBrain en 0x46B270
// (thiscall, 3 argumentos en pila: index (byte), entity, bHasAScriptBrain (byte); `ret 0Ch`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y se compilan solas con -DFIXTURE_DUMP (cl) para volcar las 4096 filas `input`
// y comprobar las constantes y la cobertura contra fixture_oracle.py (selftest). Cada fila `input` lleva todo lo que el oraculo necesita.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace ScriptsBrainsStart31Fix {
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
// `this` = tabla de 70 cerebros (como en los otros bancos). Script devuelto por el falso de StartNewStreamedScript: arena de sizeof(CRunningScript) = 0xE0 bytes con guardas.
// Pools FALSOS (el EXE lee los punteros globales 0xB74490 [peds] y 0xB7449C [objetos] y, de cada pool, [pool] = almacenamiento y [pool + 4] = estados de slot):
//   peds: 4 slots de 0x7C4 bytes; objetos: 4 slots de 0x19C bytes (los tamanos con que el juego construye sus pools, Pools.cpp). Cada almacenamiento va en una arena con 0x40 de guarda.
constexpr u32 kBrains = 70, kBrainBytes = 20;
constexpr u32 kThisOffset = 0x40, kThisBytes = kBrains * kBrainBytes, kThisArena = kThisOffset + kThisBytes + 0x40;   // 0x5F8
constexpr u32 kScriptOffset = 0x40, kScriptBytes = 0xE0, kScriptArena = kScriptOffset + kScriptBytes + 0x40;           // 0x40 + 0xE0 + 0x40 = 0x160
constexpr u32 kSlots = 4, kPedSize = 0x7C4, kObjSize = 0x19C;
constexpr u32 kPedOffset = 0x40, kPedBytes = kSlots * kPedSize, kPedArena = kPedOffset + kPedBytes + 0x40;             // 0x40 + 0x1F10 + 0x40 = 0x1F90
constexpr u32 kObjOffset = 0x40, kObjBytes = kSlots * kObjSize, kObjArena = kObjOffset + kObjBytes + 0x40;             // 0x40 + 0x670 + 0x40 = 0x6F0
static_assert(kThisArena == 0x5F8 && kScriptArena == 0x160 && kPedBytes == 0x1F10 && kPedArena == 0x1F90 && kObjBytes == 0x670 && kObjArena == 0x6F0);
// campos que toca la funcion: CRunningScript +0x3C (local 0), +0x40 (local 1), +0xC9 (tipo externo); CPed +0x474 (flags), +0x794 (u16 cerebro a cargar); CObject +0x140 (flags)
constexpr u32 kScrVar0 = 0x3C, kScrVar1 = 0x40, kScrExt = 0xC9, kPedFlagsAt = 0x474, kPedLoadAt = 0x794, kObjFlagsAt = 0x140;
constexpr u32 kBrainScriptAt = 0, kBrainTypeAt = 2;

inline constexpr std::array<u32, 12> kTypes{0, 3, 5, 1, 4, 2, 0xFF, 6, 7, 0x7F, 0x80, 100};   // 0xFF = -1, 0x80 = -128 (int8)
inline constexpr std::array<u32, 10> kScriptIdx{0, 1, 81, 0xFFFF, 0x8000, 0x7FFF, 255, 256, 82, 1000};   // m_StreamedScriptIndex (int16); el falso lo registra con extension de signo
inline constexpr std::array<u32, 8> kLoad{0, 0xFFFF, 0x8000, 1, 0x7FFF, 0x1234, 0xFF00, 0x00FF};
inline constexpr std::array<u32, 8> kBadPtr{0, 0xDEADBEEF, 0xFFFFFFFF, 1, 0x80000000, 0x00400000, 0x7C4, 0x12345678};

struct F {
    u32 kind, eax, edx, stack;
    u32 indexArg, hasArg;                    // dword de pila; el EXE usa solo el byte bajo (indice 0..69; bHasAScriptBrain 0/1), los 24 bits altos son basura
    u32 index, type, scriptIdx;              // type: int8 del cerebro como byte; scriptIdx: int16 como u16
    u32 entPool;                             // 0 puntero no valido (no se usa), 1 slot de ped, 2 slot de objeto, 3 puntero basura; entSlot 0..3; entPtrBad si entPool == 3 (o 0)
    u32 entSlot, entPtrBad;
    u8 pedState[kSlots], objState[kSlots];   // estados de slot (byte crudo: bit 7 = vacio, 0..6 = ref)
    u8 brains[kThisBytes];
    u8 script[kScriptBytes];
    u8 ped[kPedBytes];
    u8 obj[kObjBytes];
    u32 pedFlags, pedLoad, objFlags;         // valores iniciales de los campos que cambia la funcion, en el slot elegido
    u64 thisFnv;
};

inline void Put16(u8* p, u32 v) { const u16 w = u16(v); std::memcpy(p, &w, 2); }
inline void Put32(u8* p, u32 v) { std::memcpy(p, &v, 4); }
inline u16 Get16(const u8* p) { u16 v; std::memcpy(&v, p, 2); return v; }
inline u32 Get32(const u8* p) { u32 v; std::memcpy(&v, p, 4); return v; }
inline u64 ArenaFnv(const u8* body, u32 offset, u32 bytes, u32 arena) {
    std::vector<u8> a(arena, 0xC3);
    std::memcpy(a.data() + offset, body, bytes);
    return Fnv64(a.data(), arena);
}

// kind (n % 16): 0,1 tipo 0 (ped); 2 tipo 3; 3,4 tipo 5 (bHasAScriptBrain 0 y 1); 5,6 tipo 1 (objeto); 7 tipo 4; 8 tipo 2; 9 tipo -1; 10 tipos 6/7/127/-128/100; 11 tipo 5 con cerebro a cargar 0xFFFF/0/0x8000;
//   12 flags de ped extremos (todo a 1 / todo a 0 / solo 0x800000 / solo 0x1000000); 13 punteros basura con tipos que no usan la entidad; 14 slots 0 y 3 (referencia con indice alto) y estados 0x80/0x7F/0xFF;
//   15 tipo aleatorio (0..255) con entidad valida de ambos pools.
inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.kind = n % 16;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.brains, kThisBytes, c ^ 0x5A5Au);
    FillWords(f.script, kScriptBytes, c ^ 0x3C3Cu);
    FillWords(f.ped, kPedBytes, c ^ 0xA5A5u);
    FillWords(f.obj, kObjBytes, c ^ 0x6969u);
    u32 type;
    switch (f.kind) {
    case 0: case 1: type = 0; break;
    case 2: type = 3; break;
    case 3: case 4: type = 5; break;
    case 5: case 6: type = 1; break;
    case 7: type = 4; break;
    case 8: type = 2; break;
    case 9: type = 0xFF; break;
    case 10: type = kTypes[7 + Rand(c, 1) % 5]; break;
    case 11: type = 5; break;
    case 12: type = kTypes[Rand(c, 1) % 5]; break;
    case 13: type = kTypes[5 + Rand(c, 1) % 7]; break;
    case 14: type = kTypes[Rand(c, 1) % 5]; break;
    default: type = Rand(c, 1) & 0xFF; break;
    }
    f.type = type;
    const bool pedType = type == 0 || type == 3 || type == 5, objType = type == 1 || type == 4;
    f.index = Rand(c, 2) % kBrains;
    f.indexArg = ((Rand(c, 3) & 1) ? (Rand(c, 4) << 8) : 0u) | f.index;
    f.hasArg = ((Rand(c, 5) & 1) ? (Rand(c, 6) << 8) : 0u) | ((f.kind == 3) ? 0u : (f.kind == 4) ? 1u : (Rand(c, 7) & 1));
    f.scriptIdx = (Rand(c, 8) % 3 == 0) ? kScriptIdx[Rand(c, 9) % kScriptIdx.size()] : (Rand(c, 9) % 82);
    u8* b = f.brains + f.index * kBrainBytes;
    Put16(b + kBrainScriptAt, f.scriptIdx); b[kBrainTypeAt] = u8(type);
    for (u32 i = 0; i < kSlots; ++i) {
        f.pedState[i] = u8(Rand(c, 10 + i)); f.objState[i] = u8(Rand(c, 20 + i));
        if (f.kind == 14) { static constexpr u8 sp[4]{0x80, 0x7F, 0xFF, 0x00}; f.pedState[i] = sp[Rand(c, 30 + i) % 4]; f.objState[i] = sp[Rand(c, 40 + i) % 4]; }
    }
    f.entSlot = f.kind == 14 ? ((Rand(c, 11) & 1) ? 0u : 3u) : Rand(c, 11) % kSlots;
    if (pedType) f.entPool = 1;
    else if (objType) f.entPool = 2;
    else f.entPool = f.kind == 13 ? (Rand(c, 12) % 2 ? 3u : 0u) : (Rand(c, 12) % 4);   // tipos que no usan la entidad: cualquiera, valida o no
    f.entPtrBad = kBadPtr[Rand(c, 13) % kBadPtr.size()];
    // campos de la entidad elegida
    u32 pf = Get32(f.ped + f.entSlot * kPedSize + kPedFlagsAt), pl = Get16(f.ped + f.entSlot * kPedSize + kPedLoadAt), of = Get32(f.obj + f.entSlot * kObjSize + kObjFlagsAt);
    if (f.kind == 12) { static constexpr u32 v[4]{0xFFFFFFFFu, 0u, 0x800000u, 0x1000000u}; pf = v[Rand(c, 14) % 4]; of = v[Rand(c, 15) % 4]; }
    if (f.kind == 11) pl = kLoad[Rand(c, 16) % kLoad.size()];
    else if (Rand(c, 17) % 4 == 0) pl = kLoad[Rand(c, 18) % kLoad.size()];
    f.pedFlags = pf; f.pedLoad = pl; f.objFlags = of;
    Put32(f.ped + f.entSlot * kPedSize + kPedFlagsAt, pf); Put16(f.ped + f.entSlot * kPedSize + kPedLoadAt, pl); Put32(f.obj + f.entSlot * kObjSize + kObjFlagsAt, of);
    f.thisFnv = ArenaFnv(f.brains, kThisOffset, kThisBytes, kThisArena);
    return f;
}

inline std::string StateJson(const u8* p) { std::string s = "["; for (u32 i = 0; i < kSlots; ++i) { if (i) s += ','; s += Num(p[i]); } return s + ']'; }
inline std::string InputJson(const F& f) {
    std::string s = "{";
    auto kv = [&](const char* k, long long v) { s += '"'; s += k; s += "\":"; s += Num(v); s += ','; };
    kv("kind", f.kind); kv("eax", f.eax); kv("edx", f.edx); kv("stack_seed", f.stack); kv("index_arg", f.indexArg); kv("has_arg", f.hasArg);
    kv("index", f.index); kv("type", f.type); kv("script_idx", f.scriptIdx); kv("ent_pool", f.entPool); kv("ent_slot", f.entSlot); kv("ent_ptr_bad", f.entPtrBad);
    kv("ped_flags", f.pedFlags); kv("ped_load", f.pedLoad); kv("obj_flags", f.objFlags);
    kv("script_var0", Get32(f.script + kScrVar0)); kv("script_var1", Get32(f.script + kScrVar1)); kv("script_ext", f.script[kScrExt]);
    s += "\"ped_state\":" + StateJson(f.pedState) + ",\"obj_state\":" + StateJson(f.objState) + ",\"this_fnv\":\"" + Hex64(f.thisFnv) + "\"}";
    return s;
}
}  // namespace ScriptsBrainsStart31Fix

#ifdef FIXTURE_DUMP
int main() {
    using namespace ScriptsBrainsStart31Fix;
    std::printf("{\"consts\":{\"cases\":%u,\"this_offset\":%u,\"this_bytes\":%u,\"this_arena\":%u,\"script_offset\":%u,\"script_bytes\":%u,\"script_arena\":%u,\"slots\":%u,\"ped_size\":%u,\"obj_size\":%u,"
                "\"ped_offset\":%u,\"ped_bytes\":%u,\"ped_arena\":%u,\"obj_offset\":%u,\"obj_bytes\":%u,\"obj_arena\":%u}}\n",
                kCases, kThisOffset, kThisBytes, kThisArena, kScriptOffset, kScriptBytes, kScriptArena, kSlots, kPedSize, kObjSize, kPedOffset, kPedBytes, kPedArena, kObjOffset, kObjBytes, kObjArena);
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"case\":%u,\"input\":%s}\n", n, InputJson(MakeF(n)).c_str());
    return 0;
}
#endif
