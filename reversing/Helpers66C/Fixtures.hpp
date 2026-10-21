#pragma once
// Fixtures del banco Helpers66C (lote 66, cinco ayudantes sueltos del EXE): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp.
//
//   0 AEMFOpenSoundMedia          0x4E7990  cdecl (char* filename, void** outMovie, void** outMedia), OSErr en AX                                  (hook CAEMFDecoder / OpenSoundMedia)
//   1 AEMFGetMediaSoundFormat     0x4E7A80  cdecl (void* media, void** outAtom, tQTSoundFormatInfo* info), OSErr en AX; ECX de entrada = media     (hook CAEMFDecoder / GetMediaSoundFormat)
//   2 GetDXVersion                0x745650  cdecl () -> DWORD en EAX (0, 0x700, 0x800, 0x801, 0x900)                                              (hook Ps / GetDXVersion)
//   3 RwTextureGtaStreamRead      0x730E60  cdecl (RwStream*) -> RwTexture* en EAX                                                                (hook RwHelper / RwTextureGtaStreamRead)
//   4 SkinGetBonePositionsToTable 0x735360  cdecl (RpClump*, RwV3d* table), sin retorno                                                           (hook RwHelper / SkinGetBonePositionsToTable)
//
// Todos los floats viajan como patrones de 32 bits sin signo.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Hp66Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFunctions = 5;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// OSErr devuelto por un colaborador de 32 bits: el EXE solo mira AX, asi que los 16 bits altos son basura. ok: low16 = 0; si no, low16 != 0.
inline u32 OsErr(u32 n, u32 k, bool ok) {
    static constexpr u32 errs[]{1u, 0xFFFFu, 0x8000u, 0xF827u, 0xF828u, 0x7FFFu, 0xFF19u, 0x0100u};
    const u32 upper = Rand(n, k) % 3 == 0 ? 0u : (Rand(n, k + 1) & 0xFFFF0000u);
    const u32 low = ok ? 0u : (Rand(n, k + 2) % 2 ? errs[Rand(n, k + 3) % std::size(errs)] : (1u + Rand(n, k + 4) % 0xFFFFu));
    return upper | low;
}
inline u32 Token(u32 n, u32 k, u32 base) { return base + (Rand(n, k) & 0xFFFF0u); }   // puntero/handle falso, nunca nulo

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
// 0: las dos salidas (void*) viven en una arena de ventana 0x10 con guardas de 0x40: 0x40 + 0x10 + 0x40 = 0x90
constexpr u32 kOutWindow = 0x10, kOutArena = kGuard + kOutWindow + kGuard;                  // 0x90
// 1: la informacion de formato (0x44 bytes, ventana 0x50) y los bloques de handles (celdas maestras y bloques de datos)
constexpr u32 kInfoWindow = 0x50, kInfoArena = kGuard + kInfoWindow + kGuard;               // 0x40 + 0x50 + 0x40 = 0xD0
constexpr u32 kHandleArena = 0x100;
// 3: textura (RwTexture 0x58, ventana 0x60)
constexpr u32 kTexWindow = 0x60, kTexArena = kGuard + kTexWindow + kGuard;                  // 0x40 + 0x60 + 0x40 = 0x140
// 4: tabla de 41 vectores de 12 bytes (0x1EC, ventana 0x200), matrices 41 * 0x40 (0xA40), nodos 41 * 0x10 (0x290, ventana 0x2A0), jerarquia (0x30) y atomo (0x40)
constexpr u32 kTableWindow = 0x200, kTableArena = kGuard + kTableWindow + kGuard;           // 0x40 + 0x200 + 0x40 = 0x280
constexpr u32 kMatWindow = 0xA40, kMatArena = kGuard + kMatWindow + kGuard;                 // 0x40 + 0xA40 + 0x40 = 0xAC0
constexpr u32 kNodeWindow = 0x2A0, kNodeArena = kGuard + kNodeWindow + kGuard;              // 0x40 + 0x2A0 + 0x40 = 0x320
constexpr u32 kSmallWindow = 0x40, kSmallArena = kGuard + kSmallWindow + kGuard;            // 0x40 + 0x40 + 0x40 = 0xC0
constexpr u32 kMaxBones = 41;

// ================================================================================================ 0: AEMFOpenSoundMedia (0x4E7990)
struct C0 {
    u32 family{}, filename{}, errInfo{}, errOpen{}, refNum{}, errNew{}, movieTok{}, errClose{}, trackTok{}, mediaTok{}, specSeed{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C0 MakeC0(u32 n) {
    C0 c; const u32 f = n % 16; c.family = f;
    c.filename = Rand(n, 1) % 8 == 0 ? 0u : Token(n, 2, 0x04000000u);
    c.errInfo = OsErr(n, 3, f != 0);
    c.errOpen = OsErr(n, 6, f != 1);
    c.refNum = Rand(n, 9) % 4 == 0 ? (Rand(n, 10) & 0xFFFFu) : (Rand(n, 11) % 64);
    c.errNew = OsErr(n, 12, f != 2);
    c.movieTok = Rand(n, 15) % 16 == 0 ? 0u : Token(n, 16, 0x05000000u);
    c.errClose = OsErr(n, 17, Rand(n, 20) % 2);   // da igual: el EXE lo ignora (se cierra siempre)
    c.trackTok = f == 3 ? 0u : Token(n, 21, 0x06000000u);
    c.mediaTok = f == 4 ? 0u : Token(n, 22, 0x07000000u);
    c.specSeed = Rand(n, 23);
    c.seed = Rand(n, 24);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC0(const C0& c) {
    Js j; j.U("family", c.family).U("filename", c.filename).U("err_info", c.errInfo).U("err_open", c.errOpen).U("ref_num", c.refNum).U("err_new", c.errNew).U("movie", c.movieTok)
     .U("err_close", c.errClose).U("track", c.trackTok).U("media", c.mediaTok).U("spec_seed", c.specSeed).U("seed", c.seed)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1: AEMFGetMediaSoundFormat (0x4E7A80)
struct C1 {
    u32 family{}, media{}, descNull{}, ext2Null{}, moviesErr{}, waveErr{}, extMode{}, size{}, newPtrNull{}, memErr{}, seed{}, dispRet{};
    u32 eax{}, edx{}, stack{};
};
inline C1 MakeC1(u32 n) {
    C1 c; const u32 f = n % 16; c.family = f;
    c.media = Token(n, 1, 0x07000000u);
    const bool movieFails = f < 4;
    const bool waveFails = f >= 4 && f < 8;
    c.moviesErr = OsErr(n, 2, !movieFails);
    c.descNull = movieFails && Rand(n, 5) % 3 == 0;              // la descripcion solo puede ser nula si no se la desreferencia
    c.waveErr = OsErr(n, 6, !waveFails);
    c.ext2Null = waveFails && Rand(n, 9) % 3 == 0;               // la extension solo puede ser nula si no se usa
    // 0 = la extension sigue siendo la segunda NewHandle; 1 = GetSoundDescriptionExtension la sustituye por otro handle valido; 2 = la deja nula (solo con error)
    c.extMode = waveFails && Rand(n, 10) % 4 == 0 ? 2u : (!waveFails && Rand(n, 11) % 4 == 0 ? 1u : 0u);
    static constexpr u32 sizes[]{0u, 1u, 16u, 0x20u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x1000u};
    c.size = Rand(n, 12) % 3 == 0 ? sizes[Rand(n, 13) % std::size(sizes)] : Rand(n, 14);
    c.newPtrNull = Rand(n, 15) % 6 == 0;
    c.memErr = OsErr(n, 16, f % 3 != 0);
    c.seed = Rand(n, 20);
    c.dispRet = Rand(n, 21);
    c.eax = SeedReg(n, 70); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC1(const C1& c) {
    Js j; j.U("family", c.family).U("media", c.media).U("desc_null", c.descNull).U("ext2_null", c.ext2Null).U("movies_err", c.moviesErr).U("wave_err", c.waveErr).U("ext_mode", c.extMode)
     .U("size", c.size).U("new_ptr_null", c.newPtrNull).U("mem_err", c.memErr).U("seed", c.seed).U("disp_ret", c.dispRet)
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 2: GetDXVersion (0x745650)
struct C2 {
    u32 family{}, ddraw{}, createExPtr{}, hr{}, d3d8{}, dpn{}, d3d9{}, release{}, freeRet[6]{}, dbgRet{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C2 MakeC2(u32 n) {
    C2 c; const u32 f = n % 16; c.family = f;
    // 0 DDRAW no carga; 1 sin DirectDrawCreateEx; 2 HRESULT negativo; 3 sin D3D8; 4 sin dpnhpast; 5 sin D3D9; 6.. todo carga
    const u32 stage = f < 7 ? f : 6u;
    c.ddraw = stage == 0 ? 0u : Token(n, 1, 0x10000000u);
    c.createExPtr = stage == 1 ? 0u : 0x0A000000u + (Rand(n, 2) & 0xFFF0u);
    static constexpr u32 hrOk[]{0u, 1u, 0x7FFFFFFFu, 0x00000100u};
    static constexpr u32 hrBad[]{0x80000000u, 0xFFFFFFFFu, 0x80004005u, 0x887600B3u, 0xFFFFFFFEu};
    c.hr = stage == 2 ? hrBad[Rand(n, 3) % std::size(hrBad)] : (Rand(n, 4) % 2 ? hrOk[Rand(n, 5) % std::size(hrOk)] : (Rand(n, 6) & 0x7FFFFFFFu));
    c.d3d8 = stage == 3 ? 0u : Token(n, 7, 0x11000000u);
    c.dpn = stage == 4 ? 0u : Token(n, 8, 0x12000000u);
    c.d3d9 = stage == 5 ? 0u : Token(n, 9, 0x13000000u);
    c.release = Rand(n, 10);
    for (u32 i = 0; i < 6; ++i) c.freeRet[i] = Rand(n, 20 + i);
    c.dbgRet = Rand(n, 30);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j; j.U("family", c.family).U("ddraw", c.ddraw).U("create_ex", c.createExPtr).U("hr", c.hr).U("d3d8", c.d3d8).U("dpn", c.dpn).U("d3d9", c.d3d9).U("release", c.release)
     .A("free_ret", std::vector<u32>(c.freeRet, c.freeRet + 6)).U("dbg_ret", c.dbgRet).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 3: RwTextureGtaStreamRead (0x730E60)
struct C3 {
    u32 family{}, stream{}, findRet{}, chunkLen{}, version{}, t0{}, d0{}, t1{}, d1{}, nativeRet{}, texPresent{}, gameState{}, count{}, avg{}, filter{}, refCount{}, aniso{};
    u32 anisoGet{}, quality{}, anisoMax{}, setRet{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline u32 TimeValue(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 1u, 2u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFFu, 0x00FFFFFFu, 0x01000001u, 0x40000000u, 0xC0000000u, 1000u, 0x12345678u};
    return Rand(n, k) % 3 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : (Rand(n, k + 2) >> (Rand(n, k + 3) % 24));
}
inline u32 AvgValue(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFFA00000u, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0x00800000u, 0x7F000000u};
    return Rand(n, k) % 3 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : (Rand(n, k + 2) % 2 ? (Rand(n, k + 3) & 0x807FFFFFu) | ((100u + Rand(n, k + 4) % 60u) << 23) : Rand(n, k + 5));
}
inline C3 MakeC3(u32 n) {
    C3 c; const u32 f = n % 16; c.family = f;
    c.stream = Token(n, 1, 0x01000000u);
    c.findRet = f == 0 ? 0u : (Rand(n, 2) % 2 ? 1u : Rand(n, 3) | 1u);
    c.chunkLen = Rand(n, 4) % 4 == 0 ? 0u : (Rand(n, 5) % 2 ? Rand(n, 6) % 0x10000u : Rand(n, 7));
    c.version = Rand(n, 8);
    c.t0 = TimeValue(n, 10); c.d0 = TimeValue(n, 14); c.t1 = TimeValue(n, 18); c.d1 = TimeValue(n, 22);
    c.nativeRet = f == 1 ? 0u : (Rand(n, 26) % 2 ? 1u : (Rand(n, 27) | 1u));
    c.texPresent = f == 2 ? 0u : 1u;
    c.gameState = f >= 12 ? Rand(n, 28) % 20u : (f % 5 == 4 ? Rand(n, 29) % 20u : 8u);
    if (Rand(n, 30) % 8 == 0 && c.gameState != 8) c.gameState = (Rand(n, 31) % 2) ? 0xFFFFFFFFu : 0x80000008u;
    static constexpr u32 counts[]{0u, 1u, 2u, 99u, 0x7FFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 1000000u};
    c.count = Rand(n, 32) % 2 ? counts[Rand(n, 33) % std::size(counts)] : Rand(n, 34) % 5000u;
    c.avg = AvgValue(n, 35);
    static constexpr u32 filters[]{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0x101u, 0x103u, 0xFFu, 0x11u, 0x31u, 0x33u};
    c.filter = ((Rand(n, 40) & 0xFFFFFF00u) * (Rand(n, 41) % 2)) | (Rand(n, 42) % 2 ? filters[Rand(n, 43) % std::size(filters)] : (1u + 2u * (Rand(n, 44) % 2)));
    c.refCount = Rand(n, 45);
    static constexpr u32 anisos[]{0u, 1u, 2u, 0xFFu, 0x80u, 0x100u, 0x7Fu};
    c.aniso = f >= 8 && f < 12 ? 0u : (Rand(n, 46) % 4 ? 1u : anisos[Rand(n, 47) % std::size(anisos)]);
    // AL del colaborador con basura en los bits altos (signo: 0x80.. es -128)
    static constexpr u32 als[]{0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0x81u, 0x10u};
    c.anisoGet = ((Rand(n, 48) & 0xFFFFFF00u) * (Rand(n, 49) % 2)) | als[Rand(n, 50) % std::size(als)];
    static constexpr u32 qualities[]{0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 1u, 2u, 3u};
    c.quality = qualities[Rand(n, 51) % std::size(qualities)];
    c.anisoMax = ((Rand(n, 52) & 0xFFFFFF00u) * (Rand(n, 53) % 2)) | (Rand(n, 54) % 2 ? 16u : Rand(n, 55) & 0xFFu);
    c.setRet = Rand(n, 56);
    c.seed = Rand(n, 57);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC3(const C3& c) {
    Js j; j.U("family", c.family).U("stream", c.stream).U("find_ret", c.findRet).U("chunk_len", c.chunkLen).U("version", c.version).U("t0", c.t0).U("d0", c.d0).U("t1", c.t1).U("d1", c.d1)
     .U("native_ret", c.nativeRet).U("tex_present", c.texPresent).U("game_state", c.gameState).U("count", c.count).U("avg", c.avg).U("filter", c.filter).U("ref_count", c.refCount)
     .U("aniso", c.aniso).U("aniso_get", c.anisoGet).U("quality", c.quality).U("aniso_max", c.anisoMax).U("set_ret", c.setRet).U("seed", c.seed)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 4: SkinGetBonePositionsToTable (0x735360)
struct C4 {
    u32 family{}, tableNull{}, numBones{}, hierNull{}, clump{}, geometry{}, skin{}, flags[kMaxBones]{}, seed{}, maxDepth{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C4 MakeC4(u32 n) {
    C4 c; const u32 f = n % 16; c.family = f;
    c.tableNull = f == 0;
    static constexpr u32 fewBones[]{0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFBu, 2u, 0xFFFFFF00u};
    if (f < 3) c.numBones = fewBones[Rand(n, 1) % std::size(fewBones)];
    else if (f < 8) c.numBones = 2u + Rand(n, 2) % 7u;
    else if (f < 14) c.numBones = 9u + Rand(n, 3) % 32u;   // hasta 40
    else c.numBones = 40u + Rand(n, 4) % 2u;               // 40 o 41 (el maximo de la arena)
    const i32 nb = i32(c.numBones);
    c.hierNull = nb <= 1 && Rand(n, 5) % 2;
    c.clump = Rand(n, 6) % 8 == 0 ? 0u : Token(n, 7, 0x03000000u);
    c.geometry = Token(n, 8, 0x04000000u);
    c.skin = Token(n, 9, 0x05000000u);
    // banderas de nodo: 1 = POP, 2 = PUSH, resto basura; nunca POP sin PUSH previo (leeria la pila sin inicializar del EXE) y profundidad <= maxDepth (la pila del EXE tiene 32 enteros)
    c.maxDepth = f % 4 == 0 ? 3u : (f % 4 == 1 ? 24u : 8u);
    u32 depth = 0;
    for (u32 i = 0; i < kMaxBones; ++i) {
        const u32 op = i == 0 ? 0u : Rand(n, 100 + i) % 6;   // el nodo 0 nunca se lee (el bucle empieza en 1); 0,1: nada, 2: push, 3: pop, 4: push+pop, 5: push
        u32 flags = 0;
        if (op == 2 || op == 5) { if (depth < c.maxDepth) { flags = 2; ++depth; } }
        else if (op == 3) { if (depth > 0) { flags = 1; --depth; } }
        else if (op == 4) { if (depth < c.maxDepth) flags = 3; }
        if (f >= 8 && i % 7 == 3) flags |= (Rand(n, 200 + i) % 2 ? 0x4u : 0x8u) | ((Rand(n, 210 + i) % 3 == 0) ? 0xFFFFFFF0u : 0u);   // otras banderas: no influyen
        c.flags[i] = flags;
    }
    c.seed = Rand(n, 11);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC4(const C4& c) {
    Js j; j.U("family", c.family).U("table_null", c.tableNull).U("num_bones", c.numBones).U("hier_null", c.hierNull).U("clump", c.clump).U("geometry", c.geometry).U("skin", c.skin)
     .A("flags", std::vector<u32>(c.flags, c.flags + kMaxBones)).U("seed", c.seed).U("max_depth", c.maxDepth).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}
