#pragma once
// Falsos de los colaboradores del banco StatsWrappers60 (identicos en el lado del EXE y en el de la DLL) y modelo de memoria de los casos.
// Solo necesita <windows.h> y Fixtures.hpp: las incluyen Core.hpp (banco dentro del juego) y el arnes offline (reversing/StatsWrappers60/offline).
//
// Cada falso registra un evento en la traza (tipo + argumentos) y devuelve un valor determinista que fija el caso (`ctx`). Ninguno reserva memoria ni escribe JSON.
// Los falsos de texto (CText::Get, GxtCharToAscii, sprintf del CRT del EXE) NO formatean como el juego: escriben una representacion fija que el oraculo reproduce
// (ver README.md); lo que se valida es QUE se llama, en que orden y con que argumentos.
#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"

namespace Stw60 {
using namespace StatsWrappers60Fix;

// ------------------------------------------------------------------------------------------------ traza
enum Kind : u32 {
    K_GET = 1, K_G2A, K_STRCPY, K_STRIP, K_A2G, K_SPF, K_GETSTAT, K_METRIC, K_GERMAN, K_BUILD, K_MINS, K_SECS, K_PROGRESS, K_POPULATE, K_FINDMOST, K_FINDLEAST,
    K_STATIONKEY, K_FINDPED, K_WEAPSKILL, K_TAGGED, K_THRESH, K_DISPLAY, K_CHECKMSG
};
struct Ev { u32 kind, a, b, c; u64 d, e, f; };
inline std::array<Ev, 512> evs{};
inline unsigned nev{}, dropped{};
inline void Push(u32 kind, u32 a = 0, u32 b = 0, u32 c = 0, u64 d = 0, u64 e = 0, u64 f = 0) {
    if (nev < evs.size()) { evs[nev] = Ev{kind, a, b, c, d, e, f}; ++nev; } else { ++dropped; }
}
// Etiqueta de un buffer de texto del juego (1 gString, 2 gString2, 3 gGxtString, 4 gGxtString2; 0 otro): los punteros reales cambian entre ejecuciones.
inline u32 Label(const void* p) {
    const auto a = reinterpret_cast<std::uintptr_t>(p);
    return a == kAddrGString ? 1u : a == kAddrGString2 ? 2u : a == kAddrGxt1 ? 3u : a == kAddrGxt2 ? 4u : 0u;
}

// ------------------------------------------------------------------------------------------------ contexto del caso
struct Ctx {
    bool metric{}, german{}, populate{}, threshold{};
    u32 most{}, least{}, progress{}, mins{}, secs{};
    std::array<u32, 10> ws{};
    i32 tagged{};
    u8* ped{};
};
inline Ctx ctx{};
inline char textPool[16][160];
inline char asciiPool[16][176];
inline unsigned textNext{}, asciiNext{};

// ------------------------------------------------------------------------------------------------ falsos (cdecl salvo indicacion)
inline void __cdecl TraceGetStat(u32 id) { Push(K_GETSTAT, id & 0xFFFFu); }
// CStats::GetStatValue (0x558E40): igual que el original (la x87 recibe el float con fld o el entero EXACTO con fild) mas la traza.
__declspec(naked) inline void __cdecl FakeGetStat() {
    __asm {
        movzx eax, word ptr [esp+4]
        push eax
        call TraceGetStat
        add esp, 4
        movzx eax, word ptr [esp+4]
        cmp eax, 0x52
        jae ints
        fld dword ptr [eax*4+0xB79380]
        ret
    ints:
        fild dword ptr [eax*4+0xB78E20]
        ret
    }
}
inline bool __cdecl FakeMetric() { Push(K_METRIC); return ctx.metric; }
inline bool __cdecl FakeGerman() { Push(K_GERMAN); return ctx.german; }
inline bool __cdecl FakePopulate() { Push(K_POPULATE); return ctx.populate; }
inline int __cdecl FakeFindMost() { Push(K_FINDMOST); return static_cast<int>(ctx.most); }
inline int __cdecl FakeFindLeast() { Push(K_FINDLEAST); return static_cast<int>(ctx.least); }
inline float __cdecl FakeProgress() { Push(K_PROGRESS); return BitsToFloat(ctx.progress); }
inline int __cdecl FakeMins(int v) { Push(K_MINS, static_cast<u32>(v)); return static_cast<int>(static_cast<u32>(v) ^ ctx.mins); }
inline int __cdecl FakeSecs(int v) { Push(K_SECS, static_cast<u32>(v)); return static_cast<int>(static_cast<u32>(v) + ctx.secs); }
inline int __cdecl FakeTagged() { Push(K_TAGGED); return ctx.tagged; }
inline bool __cdecl FakeThreshold(float* p, u32 rangeBits) { Push(K_THRESH, reinterpret_cast<std::uintptr_t>(p) == 0x8CDEC4u ? 1u : 0u, rangeBits); return ctx.threshold; }
inline void __cdecl FakeDisplay(u32 state, u32 stat, u32 valueBits) { Push(K_DISPLAY, state, stat, valueBits); }
inline void __cdecl FakeCheckMsg() { Push(K_CHECKMSG); }
inline void* __cdecl FakeFindPed(i32 id) { Push(K_FINDPED, static_cast<u32>(id)); return ctx.ped; }
// CPed::GetWeaponSkill(type) es thiscall con un argumento en pila (`ret 4`): __fastcall con tres parametros lo reproduce.
inline u32 __fastcall FakeWeaponSkill(void* self, void*, i32 type) {
    Push(K_WEAPSKILL, static_cast<u32>(type), self == ctx.ped ? 1u : 0u);
    const u32 index = static_cast<u32>(type) - 22u;
    return index < 10u ? ctx.ws[index] : 0xDEADBE01u;
}
// CAudioEngine::GetRadioStationNameKey(id, out) (0x507010) es thiscall con dos argumentos en pila (`ret 8`).
inline void __fastcall FakeStationKey(void*, void*, i32 id, char* out) {
    Push(K_STATIONKEY, static_cast<u32>(id));
    static const char hex[] = "0123456789ABCDEF";
    // El buffer del EXE son 8 bytes de pila: "RK" + 5 digitos hexadecimales (los 20 bits bajos del id) + NUL.
    out[0] = 'R'; out[1] = 'K';
    for (int i = 0; i < 5; ++i) out[2 + i] = hex[(static_cast<u32>(id) >> (16 - 4 * i)) & 15];
    out[7] = 0;
}
// CText::Get(key) (0x6A0050) es thiscall con un argumento en pila (`ret 4`): "G[" + clave + "]" en un buffer rotatorio.
inline const u8* __fastcall FakeGet(void*, void*, const char* key) {
    Push(K_GET, Fnv32(key));
    char* d = textPool[textNext++ & 15];
    unsigned n = 0;
    d[n++] = 'G'; d[n++] = '[';
    for (const char* p = key; *p && n < 150; ++p) d[n++] = *p;
    d[n++] = ']'; d[n] = 0;
    return reinterpret_cast<const u8*>(d);
}
// GxtCharToAscii(text, offset) (0x69F7E0): "A[" + texto desde `offset` + "]".
inline const char* __cdecl FakeG2A(const u8* text, u8 offset) {
    const char* t = reinterpret_cast<const char*>(text);
    Push(K_G2A, Fnv32(t), offset);
    char* d = asciiPool[asciiNext++ & 15];
    unsigned n = 0;
    d[n++] = 'A'; d[n++] = '[';
    for (const char* p = t + offset; *p && n < 168; ++p) d[n++] = *p;
    d[n++] = ']'; d[n] = 0;
    return d;
}
// GxtCharStrcpy(dst, src) (0x718660): copia con el NUL y devuelve dst.
inline u8* __cdecl FakeStrcpy(u8* dst, const u8* src) {
    Push(K_STRCPY, Label(dst), Fnv32(reinterpret_cast<const char*>(src)));
    std::memmove(dst, src, std::strlen(reinterpret_cast<const char*>(src)) + 1);
    return dst;
}
// 0x719240 (quita las marcas ~): aqui no cambia el texto, solo se traza.
inline void __cdecl FakeStrip(u8* text) { Push(K_STRIP, Label(text), Fnv32(reinterpret_cast<const char*>(text))); }
// AsciiToGxtChar(src, dst) (0x718600).
inline void __cdecl FakeA2G(const char* src, u8* dst) {
    Push(K_A2G, Fnv32(src), Label(dst));
    std::memmove(dst, src, std::strlen(src) + 1);
}
// sprintf del CRT estatico del EXE (0x821BB5). Solo reconoce %d, %s, %.2f / %0.2f (double) y %%; escribe el texto con los valores en una representacion fija:
// %d en decimal con signo, %s tal cual, %.2f como "{" + los 16 digitos hexadecimales de los bits del double + "}".
inline int __cdecl FakeSprintf(char* dst, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    u64 args[3]{};
    unsigned na = 0;
    char out[400];
    unsigned o = 0;
    auto put = [&](const char* s) { for (; *s && o < 380; ++s) out[o++] = *s; };
    for (const char* p = fmt; *p;) {
        if (*p != '%') { if (o < 380) out[o++] = *p; ++p; continue; }
        ++p;
        if (*p == '%') { if (o < 380) out[o++] = '%'; ++p; continue; }
        if (*p == 'd') {
            const int v = va_arg(ap, int);
            char tmp[16]; std::snprintf(tmp, sizeof tmp, "%d", v); put(tmp);
            if (na < 3) args[na++] = static_cast<u32>(v);
            ++p; continue;
        }
        if (*p == 's') {
            const char* s = va_arg(ap, const char*);
            put(s);
            if (na < 3) args[na++] = Fnv32(s);
            ++p; continue;
        }
        if (*p == '0' || *p == '.') {
            while (*p && *p != 'f') ++p;
            if (*p == 'f') ++p;
            const double v = va_arg(ap, double);
            u64 bits; std::memcpy(&bits, &v, 8);
            char tmp[24]; std::snprintf(tmp, sizeof tmp, "{%016llx}", static_cast<unsigned long long>(bits)); put(tmp);
            if (na < 3) args[na++] = bits;
            continue;
        }
        if (o < 380) out[o++] = '?';
        ++p;
    }
    va_end(ap);
    out[o] = 0;
    if (o > 340) { out[340] = 0; o = 340; }
    std::memcpy(dst, out, o + 1);
    Push(K_SPF, Label(dst), Fnv32(fmt), na, args[0], args[1], args[2]);
    return static_cast<int>(o);
}
// BuildStatLine(line, p1, metrics, p2, type) (0x559230) en ConstructStatLine: traza el texto de la linea y lo que apuntan los dos punteros.
inline void __cdecl FakeBuild(const char* line, const u32* p1, i32 metrics, const u32* p2, i32 type) {
    Push(K_BUILD, Fnv32(line), p1 ? *p1 : 0xFFFFFFFFu, static_cast<u32>(metrics), p2 ? *p2 : 0xFFFFFFFFull, static_cast<u32>(type));
}

// ------------------------------------------------------------------------------------------------ regiones de memoria del juego
struct Region { u32 address, size; };
inline constexpr Region kRegions[] = {
    {0xB71510, 704},        // gString2 + gString
    {0xC1AED8, 1104},       // gGxtString2 + gGxtString
    {0xB78E20, 0x6C0},      // estadisticas: enteros por id, lista de emisoras, muertes por tipo, floats (0xB78E20..0xB794E0)
    {0xB7CB84, 4},          // CTimer::m_snTimeInMilliseconds
    {0xB7CD74, 1},          // CWorld::PlayerInFocus
    {0xB7CD98, 0x320},      // CWorld::Players[2]
    {0x96AB2C, 0x78},       // bandas (0x96AB2C..0x96AB44) y territorio (0x96AB9C)
    {0x96917F, 1},          // CCheat::m_aCheatsActive[CHEAT_MAX_RESPECT]
    {0x8CDEC4, 8},          // ultimos valores de respeto mostrados
    {0xA9A89C, 4}, {0xA9AD70, 8},
    {0x8CD8A0, 0x5B6},      // tablas de lineas (0x8CD8A0..0x8CDE56)
};
inline constexpr unsigned kNumRegions = sizeof(kRegions) / sizeof(kRegions[0]);
struct Snapshot {
    std::vector<u8> data[kNumRegions];
    void Take() { for (unsigned i = 0; i < kNumRegions; ++i) { data[i].assign(kRegions[i].size, 0); std::memcpy(data[i].data(), reinterpret_cast<const void*>(static_cast<std::uintptr_t>(kRegions[i].address)), kRegions[i].size); } }
    void Restore() const { for (unsigned i = 0; i < kNumRegions; ++i) std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(kRegions[i].address)), data[i].data(), kRegions[i].size); }
};
// Primera direccion que difiere de `reference` fuera de los rangos permitidos (0 si ninguna).
inline u32 FirstOutside(const Snapshot& reference, const std::vector<Region>& allowed) {
    for (unsigned i = 0; i < kNumRegions; ++i) {
        const u8* now = reinterpret_cast<const u8*>(static_cast<std::uintptr_t>(kRegions[i].address));
        for (u32 k = 0; k < kRegions[i].size; ++k) {
            if (now[k] == reference.data[i][k]) continue;
            const u32 a = kRegions[i].address + k;
            bool ok = false;
            for (const Region& r : allowed) if (a >= r.address && a < r.address + r.size) { ok = true; break; }
            if (!ok) return a;
        }
    }
    return 0;
}
inline u8* Mem(u32 address) { return reinterpret_cast<u8*>(static_cast<std::uintptr_t>(address)); }
inline void Poke32(u32 address, u32 v) { std::memcpy(Mem(address), &v, 4); }
inline u32 Peek32(u32 address) { u32 v; std::memcpy(&v, Mem(address), 4); return v; }
inline void FillBuffers(u32 pat) {
    // byte(buffer, offset) = 0x21 + ((pat + 31*buffer + 17*offset) % 90), con buffer 0 = gString2, 1 = gString, 2 = gGxtString2, 3 = gGxtString
    const struct { u32 address, size; } b[4] = { {kAddrGString2, kLenGString}, {kAddrGString, kLenGString}, {kAddrGxt2, kLenGxt}, {kAddrGxt1, kLenGxt} };
    for (unsigned i = 0; i < 4; ++i)
        for (u32 k = 0; k < b[i].size; ++k) Mem(b[i].address)[k] = static_cast<u8>(0x21 + ((pat + 31u * i + 17u * k) % 90u));
}
inline void ResetTrace() { nev = 0; dropped = 0; textNext = 0; asciiNext = 0; }
}
