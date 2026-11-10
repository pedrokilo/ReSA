#pragma once
// Fixtures del banco Fn144A (lote 144A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                     forma
//   0   0x69FFA0   CKeyArray::Destructor (cuerpo; el listado decia CText)     thiscall() `ret`, 24 B: [ecx] != 0 -> delete[] (0x8213AE), [ecx] = 0; [ecx+4] = 0
//   1   0x69FFD0   CData::Destructor (cuerpo; idem, mismo codigo)             idem
//   2   0x4D8670   CAEBankLoader::ScalarDeletingDestructor (listado: CAEAudioHardware)   thiscall(flags) `ret 4`, 27 B: call 4DFB20; (flags&1) -> operator delete 0x8214BD; eax = this
//   3   0x4D8690   CAEMP3TrackLoader::ScalarDeletingDestructor                idem; cuerpo 0x4E0940
//   4   0x47C0E0   CCarGenerator::SwitchOnWithCount (listado: CTheCarGenerators)  thiscall(count) `ret 4`: call 6F32C0 (SwitchOn); word [esi+1Ah] = word del argumento
//   5   0x47E050   CColPoint::GetSurfaceTypeB (listado: CTheCarGenerators)    thiscall() `ret`: mov al,[ecx+23h]
//   6   0x47E070   CCutsceneMgr::SetCutsceneOffset (listado: CTheCarGenerators)  cdecl(const CVector*) `ret`: copia 3 dwords al 0xBC4034
//   7   0x4A9110   Particle_c::Constructor (listado: FxManager_c)             thiscall() `ret`: call 4A8DB0 (ListItem_c: [ecx] = [ecx+4] = 0); eax = this
//   8   0x4A9340   FxSystem_c::ScalarDeletingDestructor (listado: FxManager_c)  idem destructor con borrado; cuerpo 0x4AA260
//   9   0x4A94E0   FxEmitterPrt_c::Constructor                                idem Particle_c
//  10   0x4A9C10   FxMemoryPool_c::Constructor                                thiscall() `ret`: mov eax,ecx
//  11   0x4A9C20   FxMemoryPool_c::Destructor                                 `ret`
//  12   0x4B21F0   CEventAreaCodes::GetEventType (V, ranura 1)                mov eax,43h
//  13   0x4B2200   CEventAreaCodes::GetLifeTime (V, ranura 3)                 xor eax,eax
//  14   0x4B2390   CEventLeaderEntryExit::TakesPriorityOver (V, ranura 11)    mov al,1; ret 4
//  15   0x4B23A0   CEventLeaderEntryExit::AffectsPedGroup (V, ranura 6)       [this+0Ch] != 0 && CPedGroupMembership::GetLeader (0x5F69A0) == [this+0Ch]; ret 4
//  16   0x4E3350   CAEPedSpeechSlot::Reset (listado: CAEPedAudioEntity)       thiscall() `ret`: reinicia los campos del hueco
//  17   0x4E34C0   CAEPedSpeechAudioEntity::Destructor (cuerpo; listado: CAEPedAudioEntity)  mov [ecx],859988h; add ecx,8; jmp 4EF660
//  18   0x630F50   CTaskSimpleTired::GetTaskType (V, ranura 4)                mov eax,0DBh
//  19   0x630FF0   CTaskSimpleTired::MakeAbortable (V, ranura 6)              ret 0Ch
//  20   0x5BA020   CDecisionMaker::From (listado: CVehicleAnimGroup)          thiscall(src) `ret 4`: 41 x CDecision::From (0x6006B0)
//  21   0x6E3E30   WaterCreature_c::Constructor (listado: CVehicleAnimGroup)  thiscall() `ret`: call 4A8DB0; [esi+8] = 0; eax = this
//  22   0x506DC0   CAEGlobalWeaponAudioEntity::ScalarDeletingDestructor (listado: CAudioEngine)  idem destructor con borrado; cuerpo 0x506C50
//  23   0x507400   CAudioEngine::StopPoliceScanner                            thiscall(arg) `ret 4`: push arg; call 4E6DC0 (StopScanner)
//  24   0x5074B0   CAEExplosionAudioEntity::Constructor (listado: CAudioEngine)  mov eax,ecx; [eax+10h] = 0; [eax] = 862E60h; [eax+7Ch] = 0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F144AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Region de memoria global de la funcion 6 (ms_cutsceneAssociations 0xBC4020 y ms_cutsceneOffset 0xBC4034): [0xBC4020, 0xBC4050)
constexpr u32 kRegionAt = 0xBC4020u;
constexpr u32 kRegionBytes = 0x30u;
constexpr u32 kOffsetAt = 0xBC4034u;

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanPair = 8;          // +0 y +4 (constructores de ListItem_c, destructores de CKeyArray/CData)
constexpr u32 kSpanWater = 0xC;       // +0, +4 y +8
constexpr u32 kSpanPool = 0xC;        // FxMemoryPool_c (no se escribe, pero el objeto mide 0xC)
constexpr u32 kSpanGen = 0x1C;        // CCarGenerator: +0x14..+0x1B
constexpr u32 kSpanColPoint = 0x24;   // +0x23
constexpr u32 kSpanVector = 0xC;      // CVector fuente
constexpr u32 kSpanEvent = 0x10;      // CEventLeaderEntryExit: m_ped en +0xC
constexpr u32 kSpanSlot = 0x1C;       // CAEPedSpeechSlot
constexpr u32 kSpanSpeech = 0x10;     // +0 y this + 8 como etiqueta
constexpr u32 kSpanTired = 0x1C;      // CTaskSimpleTired: m_TiredAnim en +0x18
constexpr u32 kSpanPed = 0x1C;        // el ped sintetico: +0x18 = clump
constexpr u32 kSpanAnim = 0x20;       // el objeto de animacion: +0x1C
constexpr u32 kSpanDecision = 0x99C;  // CDecisionMaker (41 x 0x3C)
constexpr u32 kSpanExplosion = 0x80;  // CAEExplosionAudioEntity

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
}

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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Indices (ver la cabecera)
inline constexpr unsigned kIdxKeyDtor = 0, kIdxDataDtor = 1, kIdxBankSdd = 2, kIdxMp3Sdd = 3, kIdxGenCount = 4, kIdxSurfB = 5, kIdxCutOffset = 6, kIdxParticleCtor = 7, kIdxFxSystemSdd = 8,
                          kIdxEmitterCtor = 9, kIdxPoolCtor = 10, kIdxPoolDtor = 11, kIdxAreaType = 12, kIdxAreaLife = 13, kIdxLeaderPrio = 14, kIdxLeaderGroup = 15, kIdxSlotReset = 16,
                          kIdxSpeechDtor = 17, kIdxTiredType = 18, kIdxTiredAbort = 19, kIdxDecisionFrom = 20, kIdxWaterCtor = 21, kIdxWeaponSdd = 22, kIdxStopScanner = 23,
                          kIdxExplosionCtor = 24;
inline constexpr bool IsSdd(unsigned fn) { return fn == kIdxBankSdd || fn == kIdxMp3Sdd || fn == kIdxFxSystemSdd || fn == kIdxWeaponSdd; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la region global; el resto por funcion
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case kIdxKeyDtor: case kIdxDataDtor:
        // a[3] = puntero m_data (crudo: nunca se desreferencia; el falso de delete[] no lo lee); a[4] = dword previo en +4 (m_size)
        c.a[0] = Offset(g, kind, kSpanPair);
        c.a[3] = g.Pk({0u, 0u, 0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0xDEADBEEFu, 0x00400000u + g.R(0x800000), 0x00B74000u + g.R(0x1000), g.W(), g.W(), g.W() | 1u, 0xFFFFFFFCu, 4u});
        c.a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x10u, g.W(), g.W(), 0x80000000u});
        break;
    case kIdxBankSdd: case kIdxMp3Sdd: case kIdxFxSystemSdd: case kIdxWeaponSdd:
        // a[3] = dword previo en [this]; a[4] = banderas (argumento de pila, solo importa el bit 0); a[5] = dword que el falso del cuerpo deja en [this]; a[6] = EAX del falso del cuerpo (no es this)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x859988u, 0x862E5Cu, 0x85F3CCu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, g.W(), g.W(), g.W() | 1u, g.W() & ~1u});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 0x859988u, 0x86D48Cu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[6] = Raw(g);
        break;
    case kIdxGenCount:
        // a[3] = dword previo en +0x18 (con la palabra +0x1A); a[4] = el dword argumento (el EXE lee solo su palabra baja); a[5] = palabra que el falso de SwitchOn deja en +0x1A; a[6] = dword previo en +0x14
        c.a[0] = Offset(g, kind, kSpanGen);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0xFFFF0000u, 0x0000FFFFu, g.W(), g.W()});
        c.a[4] = g.Pk({0u, 1u, 2u, 99u, 100u, 101u, 0xFFFFu, 0x10000u, 0x10001u, 0xFFFFFFFFu, 0x8000u, 0xABCD1234u, g.W() & 0xFFu, g.W() & 0xFFFFu, g.W(), g.W()});
        c.a[5] = g.Pk({0xFFFFu, 0xFFFFu, 0u, 1u, g.W() & 0xFFFFu, g.W() & 0xFFFFu, 0x8000u});
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W()});
        break;
    case kIdxSurfB:         // a[3] = dword previo en +0x20 (el byte +0x23 es el tipo B)
        c.a[0] = Offset(g, kind, kSpanColPoint);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0xFF000000u, 0x01000000u, 0x80000000u, 0x7F000000u, g.W(), g.W(), g.W(), 0x00FFFFFFu});
        break;
    case kIdxCutOffset: {
        // a[3] = selector de la fuente: 0 vector en la arena A, 1 el propio destino, 2 destino + 4, 3 destino - 4, 4 destino + 8, 5 destino - 8, 6 destino + 0xC, 7 destino - 0xC;
        // a[4..6] = los tres dwords del vector fuente (arena A); la region global se rellena con Rand(a[15], k) (k = indice de dword)
        c.a[0] = Offset(g, kind, kSpanVector);
        c.a[3] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
        const u32 specials[] = {0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 1u, 0x3F800000u, 0xFFFFFFFFu};
        for (int k = 0; k < 3; ++k) c.a[4 + k] = g.P(30) ? specials[g.R(10)] : g.W();
        if (g.P(8)) for (int k = 0; k < 3; ++k) c.a[4 + k] = g.P(50) ? 0u : 0x80000000u;   // vectores nulos (con ceros de los dos signos)
        break;
    }
    case kIdxParticleCtor: case kIdxEmitterCtor:   // a[3], a[4] = dwords previos en +0 y +4
        c.a[0] = Offset(g, kind, kSpanPair);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W(), 0xDEADBEEFu});
        break;
    case kIdxWaterCtor:     // a[3..5] = dwords previos en +0, +4 y +8
        c.a[0] = Offset(g, kind, kSpanWater);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        break;
    case kIdxPoolCtor: case kIdxPoolDtor:   // a[3..5] = dwords previos (no se escriben)
        c.a[0] = Offset(g, kind, kSpanPool);
        c.a[3] = Raw(g); c.a[4] = Raw(g); c.a[5] = Raw(g);
        break;
    case kIdxAreaType: case kIdxAreaLife: case kIdxTiredType:   // a[3] = dword en [this] (no se lee)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        break;
    case kIdxLeaderPrio:    // a[3] = dword en [this]; a[4] = argumento (referencia al otro evento: no se lee)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kIdxLeaderGroup: {
        // a[3] = selector de m_ped (+0xC): 0 nulo, 1 crudo (a[4]), 2 puntero a la arena C (desplazamiento a[5]); a[6] = selector del grupo: 0 puntero a la arena B (desplazamiento a[7]), 1 crudo a[8];
        // a[9] = lo que devuelve el falso de GetLeader: 0 el propio m_ped, 1 otro puntero de la arena C (a[10]), 2 nulo, 3 crudo distinto de m_ped (a[11])
        c.a[0] = Offset(g, kind, kSpanEvent);
        c.a[3] = g.Pk({0u, 1u, 1u, 2u, 2u, 2u});
        u32 raw = Raw(g);
        if (!raw) raw = 0x00B74000u + g.R(0x1000);
        c.a[4] = raw;
        c.a[5] = Offset(g, g.R(4), 4);
        c.a[6] = g.Pk({0u, 0u, 0u, 1u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Raw(g);
        c.a[9] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        c.a[10] = Offset(g, g.R(4), 4);
        c.a[11] = g.W() | 1u;
        break;
    }
    case kIdxSlotReset:     // a[3] = dword previo en +0x14; a[4] = dword previo en +0x18 (las tres banderas y el relleno +0x1B); el resto lo da el relleno de la arena
        c.a[0] = Offset(g, kind, kSpanSlot);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0xFFFF0000u, 0x0000FFFFu, g.W(), g.W()});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x01010101u, 0x01000000u, 0x00000101u, g.W(), g.W()});
        break;
    case kIdxSpeechDtor:    // a[3] = vtable previa en [this]; a[4] = EAX del falso de CAESound::Destructor
        c.a[0] = Offset(g, kind, kSpanSpeech);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x859988u, 0x85F310u, 0x85F311u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        break;
    case kIdxTiredAbort: {
        // a[3] = selector de m_TiredAnim (+0x18): 0 nulo, 1 puntero a la arena C (desplazamiento a[4]), 2 crudo no nulo (a[5]); a[6] = desplazamiento del ped en la arena B; a[7] = dword del clump (+0x18 del ped);
        // a[8] = lo que devuelve el falso de RpAnimBlendClumpGetAssociation: 0 igual a m_TiredAnim (si es de la arena; si no, nulo), 1 otro puntero de la arena C (a[10]), 2 nulo, 3 crudo distinto (a[9]);
        // a[11] = prioridad; el evento es ~a[11]
        c.a[0] = Offset(g, kind, kSpanTired);
        c.a[3] = g.Pk({0u, 1u, 1u, 1u, 2u});
        c.a[4] = Offset(g, g.R(4), kSpanAnim);
        u32 raw = Raw(g);
        if (!raw) raw = 0x00B74000u + g.R(0x1000);
        c.a[5] = raw;
        c.a[6] = Offset(g, g.R(4), kSpanPed);
        c.a[7] = Raw(g);
        c.a[8] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        c.a[10] = Offset(g, g.R(4), kSpanAnim);
        c.a[9] = c.a[5] ^ 0x1234u;
        c.a[11] = Raw(g);
        break;
    }
    case kIdxDecisionFrom:  // a[3] = selector del origen: 0 arena B (a[4]), 1 crudo (a[5])
        c.a[0] = Offset(g, kind, kSpanDecision);
        c.a[3] = g.Pk({0u, 0u, 0u, 1u});
        c.a[4] = Offset(g, g.R(4), kSpanDecision);
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 4u, 0x80000000u, g.W(), g.W(), 0x00400000u + g.R(0x800000), 0xFFFFFFC4u});
        break;
    case kIdxStopScanner:   // a[3] = argumento (dword completo; solo cuenta su byte bajo)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), g.W() & ~0xFFu, g.W() | 1u});
        break;
    case kIdxExplosionCtor: // a[3] = dword previo en +0; a[4] = dword previo en +0x10; a[5] = dword previo en +0x7C
        c.a[0] = Offset(g, kind, kSpanExplosion);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x862E60u, 0x859988u, g.W(), g.W()});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 0xFFu, 0x100u, 0xFFFFFF00u, g.W(), g.W()});
        break;
    default:
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.H("s", c.s);
    return j.End();
}
}
