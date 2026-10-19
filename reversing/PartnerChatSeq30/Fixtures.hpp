#pragma once
// Fixtures del banco PartnerChatSeq30: CTaskComplexPartnerChat::GetPartnerSequence (0x684380, thiscall sin argumentos, `ret`, marco SEH propio).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee de `this`: m_partner (+0x38), m_point x/y/z (+0x40/+0x44/+0x48), el BYTE m_leadSpeaker (+0x58), el int8 m_updateDirectionCount (+0x5B) y el BYTE
// m_conversationEnabled (+0x74); escribe (+0x74 si no es el primero en hablar; +0x5B si la conversacion se reinicia). Reserva en el pool de tareas (CTask::operator new 0x61A5A0) CUATRO tareas:
//   1. CTaskComplexSequence (0x40) -> 0x632BD0     2. CTaskComplexTurnToFaceEntityOrCoord (0x28) -> 0x66B890 (partner, 0.5f, 0.001f)
//   3./4. segun la rama: conversacion: dos CTaskComplexChat (0x20) -> 0x682CB0 (esJefe, partner, etapa, contexto); sin conversacion: CTaskSimpleChat(duracion) (0x34) -> 0x5F65A0
//   y CTaskSimpleStandStill(duracion, 0, 0, 8.0f) (0x20) -> 0x62F310, en orden segun m_leadSpeaker.
// Si no es el primero en hablar llama a CPedIntelligence::FindTaskByType(TASK_COMPLEX_PARTNER_CHAT = 0x4B4) (0x600EE0) sobre [partner+0x47C] y copia el byte +0x74 de la tarea devuelta.
// Duraciones: seed = (int)((((n<<4) + z) + y) + x) & 0x7F (x87, 24 bits, _ftol 0x821B40), r = seed/128, t1 = (int)(r*3000+3500), t2 = (int)((1-r)*3000+3500).
// Contextos: palabras sin signo en 0xB613EC + 2*(6-2n) y + 2*(7-2n); la longitud (palabra con signo en 0xB613E8) se compara con 3-n; si coinciden n=0.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerChatSeq30Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4608;   // 16 familias x 288

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kThisBytes = 0x78;                        // sizeof(CTaskComplexPartnerChat)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;      // 0x40 + 0x78 = 0xB8 <= 0x100
constexpr u32 kPartnerAt = 0x38, kPointAt = 0x40, kLeadAt = 0x58, kNAt = 0x5B, kConvAt = 0x74;
constexpr u32 kEntBytes = 0x480;                        // el partner falso: CPed hasta m_pIntelligence (+0x47C) inclusive
constexpr u32 kEntOff = 0x40, kEntArena = 0x600;        // 0x40 + 0x480 = 0x4C0 <= 0x600
constexpr u32 kIntAt = 0x47C;                           // CPed::m_pIntelligence
constexpr u32 kIntBytes = 0x20;                         // CPedIntelligence falsa (solo se pasa como `this` al falso de FindTaskByType)
constexpr u32 kIntOff = 0x20, kIntArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kOtherBytes = 0x78;                       // la tarea "otra" que devuelve el falso de FindTaskByType (byte +0x74 = m_conversationEnabled)
constexpr u32 kOtherOff = 0x40, kOtherArena = 0x100;    // 0x40 + 0x78 = 0xB8 <= 0x100
constexpr u32 kNodeSize = 8;                            // CReference { m_pNext, m_ppEntity }
constexpr u32 kNodeOff = 0x20, kNodeArena = 0x80;       // 3 nodos: 0x20 + 0x18 = 0x38 <= 0x80
constexpr u32 kSlot = 0x80;                             // sizeof del almacenamiento de una ranura del pool de tareas
constexpr u32 kWindowMax = 4;                           // primeras ranuras libres, en orden de busqueda, que se registran (= nº de reservas de la funcion)
constexpr u32 kAllocs = 4;
constexpr u32 kMinCap = 64;
constexpr u32 kConvSaveAt = 0xB613E0, kConvSaveBytes = 0x24;   // region de s_Conversation que el banco sobrescribe y restaura: [0xB613E0, 0xB61404)
constexpr u32 kConvBase = 0xB613EC;                            // &s_Conversation[0]; s_ConversationLength en 0xB613E8 (indice de palabra -2)
constexpr int kConvIdxMin = -4, kConvIdxMax = 9;               // indices de palabra tocados con n en [-1, 5]
constexpr u32 kTypeSeq = 0xF4, kTypeTurn = 0x398, kTypeChat = 0x4C2, kTypeSChat = 0x1A0, kTypeStand = 0xCB;
constexpr u32 kFindType = 0x4B4;                        // TASK_COMPLEX_PARTNER_CHAT

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, lead, hasPartner, etype, temp, proc, ipl, refsLen, dupKind, poolMode, poolK;
    u32 conv, otherConv, convEff, nByte;
    u32 pt[3];
    u32 length, wA, wB;
    u32 eax, edx, stack;
};

inline constexpr u8 kLeadValues[]{0, 1, 2, 3, 0x10, 0x7F, 0x80, 0x81, 0xFE, 0xFF};
inline constexpr u8 kByteEdges[]{1, 2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF};
inline constexpr u32 kFloatEdges[]{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x4B800000u, 0x4B800001u, 0x4B000001u, 0x4B7FFFFFu, 0x4B000000u, 0x4EFFFFFFu, 0x4F000000u, 0xCF000000u,
    0x5F000000u, 0x5F7FFFFFu, 0xDF000000u, 0x00000001u, 0x007FFFFFu, 0x00800000u, 0x3F000000u, 0x3FC00000u, 0x40400000u, 0xC0400000u,
    0x42FE0000u, 0x42FF0000u, 0x43000000u, 0x3EFFFFFFu, 0x7E967699u, 0xFE967699u, 0x4F7FFFFFu, 0x4C000000u, 0x4A800000u, 0xCB800000u};
inline constexpr u32 kCtxEdges[]{0u, 1u, 2u, 0x7FFFu, 0x8000u, 0x8001u, 0xFFFFu, 0xFFFEu, 0x00FFu, 0x0100u};

inline u32 FloatBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// extremeP: probabilidad (sobre 100) de elegir un valor de borde
inline u32 PtBits(u32 n, u32 k, u32 extremeP) {
    const u32 r = Rand(n, k);
    if ((r >> 8) % 100 < extremeP) return kFloatEdges[Rand(n, k + 700) % std::size(kFloatEdges)];
    return FloatBits(float(i32(Rand(n, k + 300) % 80001u) - 40000) * 0.125f);   // multiplos exactos de 1/8 en [-5000, 5000]
}

// Familias (n % 16, 288 casos cada una); lead = m_leadSpeaker != 0, conv = la rama de conversacion efectiva:
//  0 lead, sin conversacion, sin partner      1 lead, conversacion, sin partner        2 lead, sin conversacion, peaton         3 lead, conversacion, peaton
//  4 NO lead, la otra tarea con conversacion  5 NO lead, la otra sin conversacion      6 peaton con 1-2 referencias previas (a veces ya registrada), ramas al azar
//  7 vehiculo / objeto / tipos 0,5,6,7        8 edificios (sin IPL, con IPL, temporal, procedural), ramas al azar
//  9 conversacion con reinicio frecuente (3-n == longitud)   10 conversacion con contextos de borde (0x8000..0xFFFF)   11 semilla con flotantes extremos (sin conversacion en el 70 %)
// 12..15: pool casi agotado / con vuelta: 12 una sola ranura libre, 13 dos, 14 tres, 15 cuatro o vuelta del cursor; ramas y lead al azar (partner nulo solo si lead).
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    // --- jefe / conversacion
    bool lead = Rand(n, 2) % 2 == 0;
    bool conv = Rand(n, 3) % 2 == 0;
    switch (f) {
    case 0: lead = true; conv = false; break;
    case 1: lead = true; conv = true; break;
    case 2: lead = true; conv = false; break;
    case 3: lead = true; conv = true; break;
    case 4: lead = false; conv = true; break;
    case 5: lead = false; conv = false; break;
    case 9: case 10: conv = true; break;
    case 11: conv = Rand(n, 3) % 10 >= 7; break;
    default: break;
    }
    const u32 lr = Rand(n, 4) % 8;
    const u32 leadRand = Rand(n, 6) & 0xFFu;
    c.lead = !lead ? 0u : lr < 3 ? 1u : lr < 6 ? u32(kLeadValues[1 + Rand(n, 5) % (std::size(kLeadValues) - 1)]) : (leadRand ? leadRand : 0x40u);
    const u32 nz = Rand(n, 7) % 3 == 0 ? u32(kByteEdges[Rand(n, 8) % std::size(kByteEdges)]) : (Rand(n, 9) % 2 ? 1u : (1u + Rand(n, 10) % 255u));
    const u32 anyByte = Rand(n, 11) % 4 == 0 ? u32(kByteEdges[Rand(n, 12) % std::size(kByteEdges)]) : (Rand(n, 13) & 0xFFu);
    if (lead) {   // el byte m_conversationEnabled decide la rama
        c.conv = conv ? nz : 0u;
        c.otherConv = anyByte;
    } else {      // la rama la decide la otra tarea; el byte propio inicial se sobrescribe
        c.conv = Rand(n, 75) % 3 == 0 ? 0u : anyByte;
        c.otherConv = conv ? nz : 0u;
    }
    c.convEff = conv;
    // --- partner
    c.hasPartner = 1;
    if (f == 0 || f == 1) c.hasPartner = 0;
    if (f >= 12 && lead) c.hasPartner = Rand(n, 14) % 2;
    // --- entidad y referencias (como PartnerShoveSeq28)
    if (c.hasPartner) {
        c.etype = 3;
        switch (f) {
        case 6: c.refsLen = 1 + Rand(n, 15) % 2; c.dupKind = Rand(n, 16) % 3 == 0 ? 1u + Rand(n, 17) % 2 : 0u; break;
        case 7: {
            constexpr u32 t[8]{2, 4, 0, 5, 6, 7, 2, 4};
            c.etype = t[Rand(n, 18) % 8]; c.refsLen = Rand(n, 19) % 3; c.temp = Rand(n, 20) % 2; c.proc = Rand(n, 21) % 2; c.ipl = Rand(n, 22) % 2 ? 0u : (Rand(n, 23) & 0xFFu);
            break;
        }
        case 8: {
            c.etype = 1;
            const u32 v = Rand(n, 24) % 4;
            if (v == 1) c.ipl = 1 + Rand(n, 25) % 255;                                              // con IPL: registra
            else if (v == 2) { c.temp = 1; c.ipl = Rand(n, 25) % 2 ? 0u : 1 + Rand(n, 26) % 255; }  // temporal: registra
            else if (v == 3) { c.proc = 1; c.ipl = Rand(n, 25) % 2 ? 0u : 1 + Rand(n, 26) % 255; }  // procedural: registra
            c.refsLen = Rand(n, 19) % 3;                                                            // v == 0: sin IPL, ni temporal ni procedural: NO registra
            break;
        }
        default:
            c.refsLen = Rand(n, 19) % 3;
            if (f >= 12) {
                constexpr u32 t[5]{1, 2, 3, 4, 3};
                c.etype = t[Rand(n, 18) % 5];
                if (c.etype == 1) { c.temp = Rand(n, 20) % 2; c.ipl = Rand(n, 22) % 2 ? 0u : 1 + Rand(n, 23) % 255; }
            }
            if (f != 2 && f != 3 && f != 4 && f != 5 && c.etype == 3 && c.refsLen && Rand(n, 16) % 4 == 0) c.dupKind = 1u + Rand(n, 17) % 2;
            break;
        }
    }
    // --- n (m_updateDirectionCount, int8)
    if (conv) {
        constexpr int tab[]{0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, -1, 5, 5};
        c.nByte = u32(tab[Rand(n, 27) % std::size(tab)]) & 0xFFu;
    } else if (Rand(n, 27) % 2) {
        c.nByte = Rand(n, 28) % 5;
    } else {
        constexpr int e[]{-128, 127, -1, 1, 8, -8, 64, -64};
        c.nByte = Rand(n, 28) % 3 == 0 ? u32(e[Rand(n, 29) % std::size(e)]) & 0xFFu : (Rand(n, 30) & 0xFFu);
    }
    const int ni = int(i32(c.nByte << 24) >> 24);
    // --- semilla
    const u32 extremeP = f == 11 ? 75 : (f == 8 || f >= 12) ? 15 : 8;
    for (u32 j = 0; j < 3; ++j) c.pt[j] = PtBits(n, 31 + j, extremeP);
    // --- contexto y longitud
    auto ctxWord = [&](u32 k) {
        return Rand(n, k) % (f == 10 ? 2u : 5u) == 0 ? kCtxEdges[Rand(n, k + 1) % std::size(kCtxEdges)] : (Rand(n, k + 2) % 3 ? Rand(n, k + 3) % 0x400u : (Rand(n, k + 4) & 0xFFFFu));
    };
    c.wA = ctxWord(40);
    c.wB = ctxWord(50);
    const bool reset = conv && Rand(n, 60) % 100 < (f == 9 ? 70u : 35u);
    if (reset) {
        c.length = u32(3 - ni) & 0xFFFFu;
        if (ni == 4) c.wA = 0xFFFFu;   // con n = 4 la "longitud" (0xB613E8) es la propia palabra del contexto A
    } else {
        c.length = (Rand(n, 61) % 4 == 0 ? u32(3 - ni + 1) : Rand(n, 62)) & 0xFFFFu;
        if (c.length == (u32(3 - ni) & 0xFFFFu)) c.length = (c.length + 1) & 0xFFFFu;
    }
    // --- pool: 0 natural (10 % libres), 1 todo libre, 2 k libres al azar, 3 k libres justo detras del cursor, 4 vuelta (cursor al final), 5 libres solo en/antes del cursor, 6 mitad libres.
    const u32 pr = Rand(n, 64);
    if (f <= 11) {
        const u32 modes[7]{0, 0, 1, 6, 2, 3, 4};
        c.poolMode = modes[pr % 7];
        c.poolK = 4 + (pr >> 8) % 4;
    } else if (f == 12) { constexpr u32 m[3]{2, 3, 5}; c.poolMode = m[pr % 3]; c.poolK = 1; }
    else if (f == 13) { constexpr u32 m[4]{2, 3, 4, 5}; c.poolMode = m[pr % 4]; c.poolK = 2; }
    else if (f == 14) { constexpr u32 m[4]{2, 3, 4, 5}; c.poolMode = m[pr % 4]; c.poolK = 3; }
    else {
        constexpr u32 m[4]{2, 3, 4, 5};
        c.poolMode = m[pr % 4];
        c.poolK = 4 + (pr >> 8) % 2;
        if (pr % 5 == 0) { c.poolMode = 4; c.poolK = 1 + (pr >> 12) % 4; }
    }
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    return c;
}

// ------------------------------------------------------------------------------------------------ memoria de contexto que escribe el banco
// Orden de escritura (el orden importa cuando se solapan: con n = 4 el indice -2 es a la vez la longitud y el contexto A): longitud (indice -2), contexto A (6-2n), contexto B (7-2n).
struct ConvWrite { int idx; u32 value; };
inline std::array<ConvWrite, 3> ConvWrites(const C& c) {
    const int n = int(i32(c.nByte << 24) >> 24);
    return {ConvWrite{-2, c.length & 0xFFFFu}, ConvWrite{6 - 2 * n, c.wA & 0xFFFFu}, ConvWrite{7 - 2 * n, c.wB & 0xFFFFu}};
}

// ------------------------------------------------------------------------------------------------ pool sintetico
struct Pool {
    std::vector<u8> states;   // bit 7 = libre, bits 0..6 = id
    long last{};              // m_LastFreeSlot
    u32 freeTotal{};
    struct W { u32 idx, state; };
    std::vector<W> win;       // hasta kWindowMax primeras ranuras libres en orden de busqueda del EXE (last+1..cap-1, luego 0..cap-1)
};

inline u8 IdOf(u32 n, u32 i) {
    const u32 r = Rand(n, 40000 + i);
    constexpr u8 edge[4]{0, 1, 0x7E, 0x7F};
    return r % 8 == 0 ? edge[(r >> 8) % 4] : u8((r >> 8) & 0x7F);
}

inline Pool MakePool(const C& c, u32 cap, u32 n) {
    Pool p;
    p.states.assign(cap, 0);
    std::vector<u8> isFree(cap, 0);
    auto pickLast = [&]() -> long {
        switch (Rand(n, 80) % 7) {
        case 0: return 0l;
        case 1: return long(cap) - 1;
        case 2: return long(cap) - 2;
        case 3: return 1l;
        default: return long(Rand(n, 81) % cap);
        }
    };
    switch (c.poolMode) {
    case 0: for (u32 i = 0; i < cap; ++i) isFree[i] = Rand(n, 20000 + i) % 10 == 0; p.last = pickLast(); break;
    case 1: std::fill(isFree.begin(), isFree.end(), u8(1)); p.last = pickLast(); break;
    case 6: for (u32 i = 0; i < cap; ++i) isFree[i] = Rand(n, 20000 + i) % 2 == 0; p.last = pickLast(); break;
    case 2: {
        p.last = pickLast();
        for (u32 j = 0, placed = 0; placed < c.poolK && j < 4096; ++j) {
            const u32 i = Rand(n, 300 + j) % cap;
            if (!isFree[i]) { isFree[i] = 1; ++placed; }
        }
        break;
    }
    case 3: {   // k libres consecutivas justo detras del cursor
        p.last = pickLast();
        for (u32 j = 0; j < c.poolK; ++j) isFree[(u32(p.last) + 1 + j) % cap] = 1;
        break;
    }
    case 4: {   // el cursor esta al final: libres al principio del pool
        const long lasts[3]{long(cap) - 1, long(cap) - 2, long(cap) - 3};
        p.last = lasts[Rand(n, 82) % 3];
        for (u32 j = 0; j < c.poolK; ++j) isFree[j] = 1;
        break;
    }
    case 5: {   // libres solo en el cursor y antes de el: se encuentran tras dar la vuelta
        p.last = long(kAllocs + Rand(n, 83) % (cap - 2 * kAllocs));
        for (u32 j = 0; j < c.poolK && long(j) <= p.last; ++j) isFree[u32(p.last) - j] = 1;
        break;
    }
    default: break;
    }
    for (u32 i = 0; i < cap; ++i) {
        p.states[i] = u8((isFree[i] ? 0x80u : 0u) | IdOf(n, i));
        p.freeTotal += isFree[i];
    }
    u32 start = u32(p.last) + 1;
    if (start == cap) start = 0;
    for (u32 pass = 0; pass < 2 && p.win.size() < kWindowMax; ++pass) {
        const u32 from = pass == 0 ? start : 0, to = pass == 0 ? cap : start;
        for (u32 i = from; i < to && p.win.size() < kWindowMax; ++i)
            if (isFree[i]) p.win.push_back({i, p.states[i]});
    }
    return p;
}
}  // namespace PartnerChatSeq30Fix
