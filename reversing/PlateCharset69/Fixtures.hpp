#pragma once
// Fixtures del banco PlateCharset69: CCustomCarPlateMgr::LoadPlatecharsetDat (0x6FDC00), cdecl (const char* filename, uint8* data), AL = 1 / 0.
// Solo C++ estandar (sin tipos del juego): lo incluye Core.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
// Reparto: 16384 casos; familia = n % 16 (1024 casos por familia). Cada caso es un archivo ficticio: la lista de lineas que devolvera el falso de CFileLoader::LoadLine.
// Nunca se generan lineas vacias ni formadas solo por separadores (' ' y '\t'): en el EXE llevan a atoi(NULL), que accede a la direccion 0 (ver README.md).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Pc69Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ arena de salida
// [relleno 0x50][region de salida 40 dwords][guarda 0x40]: `data` apunta a arena + dataOff (0x40..0x50); el archivo ficticio tiene como maximo 40 lineas, asi que el EXE nunca pasa de 40 dwords.
constexpr u32 kGuard = 0x40;
constexpr u32 kOutWords = 40;
constexpr u32 kOutBytes = kOutWords * 4;
constexpr u32 kArena = 0x50 + kOutBytes + kGuard;
constexpr u32 kDataOffsets[8]{0x40, 0x41, 0x42, 0x43, 0x44, 0x48, 0x4C, 0x50};
constexpr u32 kMaxLines = 40;
constexpr u32 kMaxLineBytes = 560;          // el falso de LoadLine tiene un buffer de 1024 bytes
constexpr u32 kLineBuf = 1024;
constexpr u32 kNameBuf = 128;

inline void FillArena(u8* arena, u32 size, u32 mode, u32 seed) {
    switch (mode) {
    case 1: std::memset(arena, 0x00, size); break;
    case 2: std::memset(arena, 0xFF, size); break;
    case 3: std::memset(arena, 0xCD, size); break;
    case 4: std::memset(arena, 0xA5, size); break;
    default: {
        const u32 s = mode == 5 ? seed ^ 0x5A5A5A5Au : seed;
        for (u32 i = 0; i < size / 4; ++i) { const u32 w = Rand(s, i % 127); std::memcpy(arena + 4 * i, &w, 4); }
        break;
    }
    }
}

// ------------------------------------------------------------------------------------------------ caso
struct Case {
    u32 n, family;
    u32 dataOff;            // desplazamiento de `data` en la arena
    u32 mode, seedArena;    // relleno previo de la arena
    u32 openRet;            // lo que devuelve el falso de OpenFile (0 = no se pudo abrir)
    u32 closeRet;           // lo que devuelve el falso de CloseFile (solo su EAX alto se deja ver en el EAX de salida del EXE)
    u32 eax, ecx, edx, stack;
    std::string name;       // filename
    std::vector<std::string> lines;
};

inline const char* const kNames[]{"PLATECHARSET.DAT", "platecharset.dat", "", "a", "X Y", "DATA\\PLATE.DAT", "nombre-largo-nombre-largo-nombre-largo-nombre-largo-0123456789.dat", "\x80\xFF\x01", "..", "plate.dat\t"};
// Campos numericos
inline const char* const kEdge[]{"0", "1", "2", "9", "10", "99", "100", "127", "128", "200", "254", "255", "256", "257", "258", "300", "511", "512", "513", "1000", "4095", "4096", "32767", "32768", "32769",
    "65279", "65280", "65535", "65536", "65537", "65791", "65792", "131071", "131072", "131327", "16777215", "16777216", "16777471", "-0", "-1", "-2", "-127", "-128", "-254", "-255", "-256", "-257",
    "-65535", "-65536", "-65537", "-65791", "+0", "+1", "+254", "+255", "+256", "+65536", "+65791", "00", "000255", "0255", "0256", "00000000000256", "0000000000000000000000000000000000000000255",
    "255abc", "256abc", "12abc", "abc", "x", "0x10", "0xFF", "1e3", "1.5", "255.9", "256.9", "1,2", "1-2", "--5", "+-5", "-+5", "++5", "5-", "5+", "-", "+", "\v12", "\f9", "\r7", "\v\f\r1", "\n3", "12\r", "12\v", "255\r",
    "\v-5", "\f+255", "\r\v\f-65536", "\x7F""5", "\x01""5", "5\x01", "\xA0""5", "5\xA0", "\xFF", "\x80""1"};
inline const char* const kOverflow[]{"2147483647", "2147483648", "2147483649", "2147483903", "2147483904", "4294967295", "4294967296", "4294967297", "4294967551", "4294967552", "4294967553", "4294967295",
    "-2147483647", "-2147483648", "-2147483649", "-2147483905", "-4294967295", "-4294967296", "-4294967297", "-4294967551", "-4294967552", "99999999999", "-99999999999", "18446744073709551615",
    "18446744073709551616", "18446744073709551617", "18446744073709551871", "123456789012345678901234567890", "-123456789012345678901234567890", "+4294967297", "+4294967296", "00004294967297",
    "429496729600", "429496729700", "42949672960000000000", "10000000000", "10000000001", "10000000255", "1000000000255", "9999999999999999999999999999999999999999"};
inline const char* const kSeps[]{" ", "\t", "  ", " \t", "\t ", "\t\t", "   ", " \t \t "};
inline const char* const kTexts[]{"", " ", "the end", "comentario", "1 2 3", "0 0 0", "255 255 255", "  ", "\t", ";", ";;", "x"};
inline const char* const kNearEnd[]{";the end ", ";the ends", ";the en", ";The end", ";the  end", ";the end\t", ";the_end", ";THE END", ";the end;", ";;the end", ";the endx", ";the", ";the e", ";"};

struct Gen {
    u32 c, k{0};
    u32 R() { return Rand(c, ++k); }
    u32 R(u32 m) { return R() % m; }
    bool P(u32 num, u32 den) { return R(den) < num; }
};

inline std::string Dec(Gen& g) { return std::to_string(g.R(256)); }
inline std::string Garbage(Gen& g) {
    static const char set[] = "0123456789+-abcxX.,;\v\f\r\x01\x7F\x80\xA0\xFF_/\\";
    std::string t;
    const u32 len = 1 + g.R(8);
    for (u32 i = 0; i < len; ++i) t += set[g.R(sizeof(set) - 1)];
    return t;
}
inline std::string Digits(Gen& g) {
    std::string t;
    const u32 s = g.R(4);
    if (s == 1) t += '-'; else if (s == 2) t += '+';
    const u32 len = 1 + g.R(22);
    for (u32 i = 0; i < len; ++i) t += char('0' + g.R(10));
    return t;
}
// style: 0 decimal 0..255, 1 bordes, 2 desbordamiento, 3 basura, 4 digitos largos, 5 mezcla
inline std::string Tok(Gen& g, u32 style) {
    if (style == 5) style = g.R(5);
    switch (style) {
    case 0: return Dec(g);
    case 1: return kEdge[g.R(std::size(kEdge))];
    case 2: return kOverflow[g.R(std::size(kOverflow))];
    case 3: return Garbage(g);
    default: return Digits(g);
    }
}
inline bool OnlySeps(const std::string& s) { for (char ch : s) if (ch != ' ' && ch != '\t') return false; return true; }
// Linea de `nf` campos; sep: -1 aleatorio, si no un indice de kSeps; lead/trail: separadores al principio y al final
inline std::string Line(Gen& g, u32 nf, u32 style, int sep = -1, bool lead = false, bool trail = false) {
    std::string s;
    if (lead) s += kSeps[g.R(std::size(kSeps))];
    for (u32 i = 0; i < nf; ++i) {
        if (i) s += kSeps[sep < 0 ? g.R(std::size(kSeps)) : u32(sep)];
        s += Tok(g, style);
    }
    if (trail) s += kSeps[g.R(std::size(kSeps))];
    if (s.size() > kMaxLineBytes) s.resize(kMaxLineBytes);
    if (OnlySeps(s)) s += "0";
    return s;
}
inline std::string Comment(Gen& g) {
    std::string s = ";";
    s += kTexts[g.R(std::size(kTexts))];
    if (g.P(1, 3)) s += Tok(g, 5);
    return s;
}
inline std::string NearEnd(Gen& g) { return kNearEnd[g.R(std::size(kNearEnd))]; }
inline const char* const kEnd = ";the end";

inline Case Make(unsigned n) {
    Case f{};
    Gen g{n * 8u + 0x10001u};
    f.n = n; f.family = n % kFamilies;
    const u32 fam = f.family;
    f.dataOff = kDataOffsets[g.R(8)];
    if (fam == 0) f.dataOff = 0x40;
    f.mode = g.P(1, 3) ? 5u : 0u;
    if (fam >= 2 && fam <= 5) f.mode = fam - 1;
    else if (fam >= 14) f.mode = g.R(6);
    f.seedArena = 1u + g.R(61);
    f.name = kNames[g.R(std::size(kNames))];
    if (fam == 0 || fam == 1) f.name = kNames[0];
    f.openRet = 0x00C0DE00u + g.R(0x100) * 4u;
    f.closeRet = g.P(1, 3) ? 0u : g.R();
    f.eax = g.P(1, 5) ? 0u : g.R();
    f.ecx = g.R();
    f.edx = g.P(1, 5) ? 0xFFFFFFFFu : g.R();
    f.stack = fam == 14 ? StackSeed((n / 16) % 12) : StackSeed(g.R(12));
    auto& L = f.lines;
    auto good = [&](u32 style) { L.push_back(Line(g, 3, style, g.P(1, 2) ? 0 : -1)); };
    switch (fam) {
    case 0: {   // lineas buenas de tres campos decimales, fin de archivo
        const u32 cnt = 1 + g.R(12);
        for (u32 i = 0; i < cnt; ++i) good(0);
        break;
    }
    case 1: {   // buenas, ";the end" y lineas que ya no se leen
        const u32 cnt = g.R(10);
        for (u32 i = 0; i < cnt; ++i) good(g.P(1, 4) ? 5u : 0u);
        L.push_back(kEnd);
        const u32 extra = g.R(6);
        for (u32 i = 0; i < extra; ++i) { if (g.P(1, 3)) L.push_back(Comment(g)); else good(5); }
        break;
    }
    case 2: {   // comentarios entre lineas buenas y terminadores casi iguales (no terminan)
        const u32 cnt = 1 + g.R(14);
        for (u32 i = 0; i < cnt; ++i) {
            const u32 r = g.R(4);
            if (r == 0) L.push_back(Comment(g)); else if (r == 1) L.push_back(NearEnd(g)); else good(g.P(1, 3) ? 5u : 0u);
        }
        if (g.P(1, 2)) L.push_back(kEnd);
        break;
    }
    case 3: {   // bordes numericos
        const u32 cnt = 1 + g.R(10);
        for (u32 i = 0; i < cnt; ++i) good(1);
        break;
    }
    case 4: {   // desbordamientos de 32 bits
        const u32 cnt = 1 + g.R(10);
        for (u32 i = 0; i < cnt; ++i) good(g.P(1, 5) ? 4u : 2u);
        break;
    }
    case 5: {   // numero de campos variable (1..9): la primera linea con menos de tres corta
        const u32 cnt = 1 + g.R(10);
        for (u32 i = 0; i < cnt; ++i) L.push_back(Line(g, 1 + g.R(9), g.R(3) == 0 ? 1u : 0u, -1, g.P(1, 4), g.P(1, 4)));
        break;
    }
    case 6: {   // buenas y despues una linea de uno o dos campos; a veces un comentario o ";the end" detras
        const u32 cnt = g.R(7);
        for (u32 i = 0; i < cnt; ++i) good(g.P(1, 3) ? 1u : 0u);
        L.push_back(Line(g, 1 + g.R(2), g.R(6) == 0 ? 3u : 0u, -1, g.P(1, 3), g.P(1, 3)));
        const u32 extra = g.R(3);
        for (u32 i = 0; i < extra; ++i) { if (g.P(1, 2)) L.push_back(kEnd); else good(0); }
        break;
    }
    case 7: {   // separadores: principio, final, varios, solo tabuladores
        const u32 cnt = 1 + g.R(10);
        for (u32 i = 0; i < cnt; ++i) L.push_back(Line(g, 3 + g.R(2), g.P(1, 3) ? 1u : 0u, int(g.R(std::size(kSeps))), g.P(1, 2), g.P(1, 2)));
        if (g.P(1, 3)) L.push_back(kEnd);
        break;
    }
    case 8: {   // OpenFile devuelve NULL (el falso de LoadLine devuelve NULL para un archivo nulo); tambien archivos que si abren
        if (g.P(2, 3)) f.openRet = 0;
        const u32 cnt = g.R(5);
        for (u32 i = 0; i < cnt; ++i) good(0);
        if (g.P(1, 3)) L.push_back(kEnd);
        break;
    }
    case 9: {   // sin lineas, solo comentarios, terminador inmediato
        const u32 r = g.R(4);
        if (r == 1) { const u32 cnt = 1 + g.R(5); for (u32 i = 0; i < cnt; ++i) L.push_back(Comment(g)); }
        else if (r == 2) { L.push_back(kEnd); good(0); }
        else if (r == 3) { L.push_back(Comment(g)); L.push_back(kEnd); }
        break;
    }
    case 10: {  // campos basura (caracteres de control, signos, letras, bytes altos)
        const u32 cnt = 1 + g.R(10);
        for (u32 i = 0; i < cnt; ++i) L.push_back(Line(g, 3 + g.R(3), 3, -1, g.P(1, 3), g.P(1, 3)));
        break;
    }
    case 11: {  // lineas largas: muchos campos y numeros largos
        const u32 cnt = 1 + g.R(6);
        for (u32 i = 0; i < cnt; ++i) L.push_back(Line(g, 3 + g.R(60), g.R(3) == 0 ? 4u : 0u, -1, g.P(1, 3), g.P(1, 3)));
        break;
    }
    case 12: {  // casi o exactamente la capacidad de la region de salida (40 lineas buenas)
        const u32 r1 = g.R(3);
        const u32 r2 = g.R(3);
        const u32 cnt = kMaxLines - r1 * r2;
        for (u32 i = 0; i < cnt; ++i) good(g.P(1, 4) ? 1u : 0u);
        break;
    }
    case 13: {  // mezcla aleatoria de todo
        const u32 cnt = 1 + g.R(24);
        for (u32 i = 0; i < cnt; ++i) {
            const u32 r = g.R(10);
            if (r == 0) L.push_back(Comment(g));
            else if (r == 1) L.push_back(NearEnd(g));
            else if (r == 2) L.push_back(Line(g, 1 + g.R(6), 5, -1, g.P(1, 3), g.P(1, 3)));
            else good(5);
        }
        if (g.P(1, 4)) L.push_back(kEnd);
        break;
    }
    case 14: {  // registros y pila extremos con archivos sencillos
        const u32 cnt = g.R(8);
        for (u32 i = 0; i < cnt; ++i) good(g.P(1, 3) ? 1u : 0u);
        if (g.P(1, 2)) L.push_back(kEnd);
        if (g.P(1, 8)) f.openRet = 0;
        break;
    }
    default: {  // mezcla con terminador cercano, signos y espacios en blanco del CRT
        const u32 cnt = 1 + g.R(20);
        for (u32 i = 0; i < cnt; ++i) {
            const u32 r = g.R(8);
            if (r == 0) L.push_back(NearEnd(g));
            else if (r == 1) L.push_back(Comment(g));
            else if (r == 2) L.push_back(Line(g, 3 + g.R(3), 1, int(g.R(std::size(kSeps))), g.P(1, 2), g.P(1, 2)));
            else good(g.P(1, 2) ? 1u : 5u);
        }
        if (g.P(1, 3)) L.push_back(kEnd);
        break;
    }
    }
    if (L.size() > kMaxLines) L.resize(kMaxLines);
    return f;
}

inline std::string HexStr(const std::string& s) {
    constexpr char d[] = "0123456789abcdef";
    std::string o;
    o.reserve(s.size() * 2);
    for (unsigned char ch : s) { o += d[ch >> 4]; o += d[ch & 15]; }
    return o;
}
// Fila `input` en JSON (sin dependencias)
inline std::string JsonOf(const Case& f) {
    std::string s = "{\"func\":0,\"n\":" + std::to_string(f.n) + ",\"family\":" + std::to_string(f.family) + ",\"data_off\":" + std::to_string(f.dataOff) + ",\"mode\":" + std::to_string(f.mode) +
        ",\"seed\":" + std::to_string(f.seedArena) + ",\"name\":\"" + HexStr(f.name) + "\",\"open_ret\":" + std::to_string(f.openRet) + ",\"close_ret\":" + std::to_string(f.closeRet) +
        ",\"eax\":" + std::to_string(f.eax) + ",\"ecx\":" + std::to_string(f.ecx) + ",\"edx\":" + std::to_string(f.edx) + ",\"stack_seed\":" + std::to_string(f.stack) + ",\"lines\":[";
    for (size_t i = 0; i < f.lines.size(); ++i) { if (i) s += ','; s += '"' + HexStr(f.lines[i]) + '"'; }
    return s + "]}";
}
}
