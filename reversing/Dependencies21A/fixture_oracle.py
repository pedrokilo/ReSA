"""Fixture finito y oraculo independiente del printf del CRT del EXE (0x821982).

Las tablas de este fichero son la fuente unica: `python fixture_oracle.py --emit` regenera FixtureTables.hpp (el banco C++ las incluye, no las repite).
El oraculo predice el texto exacto solo para los formatos enteros/cadena/caracter (marcados `exact`); los formatos de coma flotante dependen del motor
de conversion del CRT del EXE y solo se comparan entre EXE y DLL (el texto de ambos lados se registra igualmente).
"""
import struct
import sys
from pathlib import Path

CASES = 4096
MODES = 2          # 0: buffer propio del flujo (_stbuf devuelve 0); 1: stdout sin buffer + tty -> buffer temporal de _stbuf y volcado por _write doblado
# Reparto: formato = n % 32, modo = (n // 32) % 2, variante = n // 64 (64 variantes).

# (texto, tipos de argumento, exact)
#  i = int32, q = int64 (dos palabras), d = double (dos palabras), s = char*, w = wchar_t*, c = int (caracter), p = int (puntero), * = int pequeno (ancho/precision)
FORMATS = (
    ("That's a really big step\n", "", True),
    ("", "", True),
    ("%d", "i", True),
    ("%u", "i", True),
    ("%x", "i", True),
    ("%X|%08x", "ii", True),
    ("%o", "i", True),
    ("%#x %#o", "ii", True),
    ("%+d|% d|%-6d|", "iii", True),
    ("%hd %hu", "ii", True),
    ("%ld %lu", "ii", True),
    ("%I64d", "q", True),
    ("%I64u %I64x", "qq", True),
    ("%c%c", "cc", True),
    ("%s", "s", True),
    ("[%10s][%-10s]", "ss", True),
    ("[%.3s]", "s", False),
    ("%s%s%s", "sss", True),
    ("%%|%5d%%", "i", True),
    ("%*d", "*i", True),
    ("%p", "p", True),
    ("%ls", "w", True),
    ("Car ModelIndex (slot:%d) has changed from %d into %d\n", "iii", True),
    ("%f", "d", False),
    ("%e", "d", False),
    ("%g", "d", False),
    ("%10.4f|%-12.3e|", "dd", False),
    ("%.*f", "*d", False),
    ("%G %E", "dd", False),
    ("%#.0f", "d", False),
    ("%5.1f%%", "d", False),
    ("%d %s %f %c %x\n", "isdcx".replace("x", "i"), False),
)
assert len(FORMATS) == 32

INTS = (0, 1, 0xFFFFFFFF, 2, 0xFFFFFFFE, 127, 128, 255, 256, 32767, 32768, 65535, 65536, 0xFFFF8000, 0x7FFFFFFF, 0x80000000,
        0x80000001, 0xFFFFFFFF - 98, 12345678, 0xFF439EB2, 0x3B9ACA00, 0xDEADBEEF, 65, 0x7F, 10, 99, 100, 999, 0xFFFFFF9D, 0x0000FFFF,
        0x00010000, 0xFFFF0000)
CHARS = (65, 97, 48, 32, 126, 33, 0xE9, 0xFF, 0x80, 0x7F, 10, 9, 1, 0x41, 0x7A, 0x5C)
STARS = (0, 1, 2, 5, 10, 20, 30, 39)
QWORDS = (0, 1, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0x8000000000000000, 0x8000000000000001, 0x00000000FFFFFFFF, 0x0000000100000000,
          0xDEADBEEFCAFEBABE, 123456789012345678, 0xFFFFFFFF00000000, 99999999999)
DOUBLES = (0x0000000000000000, 0x8000000000000000, 0x3FF0000000000000, 0xBFF0000000000000, 0x3FE0000000000000, 0x46293E5939A08CEA,
           0xC6293E5939A08CEA, 0x39B4484BFEEBC2A0, 0x7FEFFFFFFFFFFFFF, 0xFFEFFFFFFFFFFFFF, 0x0010000000000000, 0x0000000000000001,
           0x000FFFFFFFFFFFFF, 0x7FF0000000000000, 0xFFF0000000000000, 0x7FF8000000001234, 0xFFF8000000000001, 0x7FF0000000000001,
           0xFFF0000000000001, 0x419D6F34548B4000, 0x400921FB54442D18, 0x4004000000000000, 0x3FB999999999999A, 0x4059000000000000,
           0x408F3FFBE76C8B44, 0x3F40624DD2F1A9FC, 0x430C6BF526340000, 0x4341C37937E08000, 0x54B249AD2594C37D, 0xA6E4B2C1B5E34B5D,
           0x47EFFFFFE0000000, 0x4170000010000000)
# Cadenas (None = puntero nulo). Todas ASCII salvo las dos de bytes altos (latin-1).
STRINGS = (b"", b"a", b"Hello", b"That's a really big step", b"x" * 300, b"%d %s", b"tab\there\n", None, b"ab", b"\xe9\xff", b"0123456789",
           b"  spaces  ", b"%%", b"y" * 100, b"A" * 9, b"B" * 10)
WIDES = ("", "a", "Hello", "That's", "w" * 120, "%d", None, "0123456789")


def kinds_words(kinds):
    return sum(2 if k in 'qd' else 1 for k in kinds)


def decode(scenario):
    return scenario % 32, (scenario // 32) % MODES, scenario // (32 * MODES)


def arguments(scenario):
    """Palabras de 32 bits que se apilan (el primer elemento es el primer argumento tras el formato) y los valores logicos por argumento."""
    fmt, mode, v = decode(scenario)
    text, kinds, exact = FORMATS[fmt]
    words, logical = [], []
    for k, kind in enumerate(kinds):
        if kind == 'i' or kind == 'p':
            x = INTS[(v + 7 * k) % len(INTS)]
            words.append(x)
            logical.append(('i', x))
        elif kind == 'c':
            x = CHARS[(v + 3 * k) % len(CHARS)]
            words.append(x)
            logical.append(('c', x))
        elif kind == '*':
            x = STARS[(v + 5 * k) % len(STARS)]
            words.append(x)
            logical.append(('*', x))
        elif kind == 'q':
            x = QWORDS[(v + 5 * k) % len(QWORDS)]
            words += [x & 0xFFFFFFFF, x >> 32]
            logical.append(('q', x))
        elif kind == 'd':
            x = DOUBLES[(v + 11 * k) % len(DOUBLES)]
            words += [x & 0xFFFFFFFF, x >> 32]
            logical.append(('d', x))
        elif kind == 's':
            idx = (v + 3 * k) % len(STRINGS)
            words.append(('s', idx))          # el puntero lo pone el banco; aqui se registra el indice
            logical.append(('s', STRINGS[idx]))
        elif kind == 'w':
            idx = (v + 3 * k) % len(WIDES)
            words.append(('w', idx))
            logical.append(('w', WIDES[idx]))
    return fmt, mode, text, kinds, exact, words, logical


def s32(x):
    x &= 0xFFFFFFFF
    return x - (1 << 32) if x & 0x80000000 else x


def expected(scenario):
    """Texto (bytes) que debe producir el printf del EXE, o None si el formato no tiene prediccion exacta."""
    fmt, mode, text, kinds, exact, words, logical = arguments(scenario)
    if not exact:
        return None
    L = [x for _, x in logical]
    if fmt == 0:
        return b"That's a really big step\n"
    if fmt == 1:
        return b""
    if fmt == 2:
        return b"%d" % s32(L[0])
    if fmt == 3:
        return b"%d" % (L[0] & 0xFFFFFFFF)
    if fmt == 4:
        return b"%x" % (L[0] & 0xFFFFFFFF)
    if fmt == 5:
        return b"%X|%08x" % (L[0] & 0xFFFFFFFF, L[1] & 0xFFFFFFFF)
    if fmt == 6:
        return b"%o" % (L[0] & 0xFFFFFFFF)

    def alt_x(x):
        x &= 0xFFFFFFFF
        return b"0" if x == 0 else b"0x%x" % x

    def alt_o(x):
        x &= 0xFFFFFFFF
        return b"0" if x == 0 else b"0%o" % x
    if fmt == 7:
        return alt_x(L[0]) + b" " + alt_o(L[1])
    if fmt == 8:
        return b"%+d|% d|%-6d|" % (s32(L[0]), s32(L[1]), s32(L[2]))
    if fmt == 9:
        h = L[0] & 0xFFFF
        return b"%d %d" % (h - 0x10000 if h & 0x8000 else h, L[1] & 0xFFFF)
    if fmt == 10:
        return b"%d %d" % (s32(L[0]), L[1] & 0xFFFFFFFF)
    if fmt == 11:
        q = L[0]
        return b"%d" % (q - (1 << 64) if q & (1 << 63) else q)
    if fmt == 12:
        return b"%d %x" % (L[0], L[1])
    if fmt == 13:
        return bytes([L[0] & 0xFF, L[1] & 0xFF])
    if fmt in (14, 15, 17):
        def cs(b):
            return b"(null)" if b is None else b
        if fmt == 14:
            return cs(L[0])
        if fmt == 15:
            return b"[" + cs(L[0]).rjust(10) + b"][" + cs(L[1]).ljust(10) + b"]"
        return cs(L[0]) + cs(L[1]) + cs(L[2])
    if fmt == 18:
        return b"%|" + (b"%5d" % s32(L[0])) + b"%"
    if fmt == 19:
        return (b"%*d" % (L[0], s32(L[1])))
    if fmt == 20:
        return b"%08X" % (L[0] & 0xFFFFFFFF)
    if fmt == 21:
        return b"(null)" if L[0] is None else L[0].encode('latin-1')
    if fmt == 22:
        return b"Car ModelIndex (slot:%d) has changed from %d into %d\n" % (s32(L[0]), s32(L[1]), s32(L[2]))
    return None


def emit_cpp():
    def arr(name, typ, values, fmt):
        return "inline constexpr %s %s[]{%s};\n" % (typ, name, ", ".join(fmt(v) for v in values))
    out = ["// Generado por fixture_oracle.py --emit; no editar a mano.\n#pragma once\n#include <cstdint>\nnamespace Dependencies21ALive {\n"]
    out.append("struct FormatCase { const char* text; const char* kinds; bool exact; };\n")
    out.append("inline constexpr FormatCase kFormats[]{%s};\n" % ", ".join('{"%s", "%s", %s}' % (
        t.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n"), k, "true" if e else "false") for t, k, e in FORMATS))
    out.append(arr("kInts", "uint32_t", INTS, lambda x: "0x%08Xu" % x))
    out.append(arr("kChars", "uint32_t", CHARS, lambda x: "0x%02Xu" % x))
    out.append(arr("kStars", "uint32_t", STARS, lambda x: "%du" % x))
    out.append(arr("kQwords", "uint64_t", QWORDS, lambda x: "0x%016XULL" % x))
    out.append(arr("kDoubles", "uint64_t", DOUBLES, lambda x: "0x%016XULL" % x))
    out.append("struct NarrowString { const char* text; unsigned size; bool null; };\n")
    out.append("inline constexpr NarrowString kStrings[]{%s};\n" % ", ".join(
        "{nullptr, 0, true}" if s is None else '{"%s", %d, false}' % ("".join("\\x%02x" % c for c in s) if any(c > 126 or c < 32 for c in s) else
                                                                 s.decode().replace("\\", "\\\\").replace('"', '\\"'), len(s)) for s in STRINGS))
    out.append("struct WideString { const wchar_t* text; bool null; };\n")
    out.append("inline constexpr WideString kWides[]{%s};\n" % ", ".join(
        "{nullptr, true}" if s is None else '{L"%s", false}' % s.replace("\\", "\\\\").replace('"', '\\"') for s in WIDES))
    out.append("}\n")
    return "".join(out)


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == '--emit':
        Path(__file__).with_name('FixtureTables.hpp').write_text(emit_cpp(), encoding='utf-8', newline='\n')
    else:
        exact = sum(1 for n in range(CASES) if expected(n) is not None)
        print('casos con texto exacto:', exact, 'de', CASES)
