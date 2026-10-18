"""Oraculo independiente de CAEAudioHardware::PlaySound (0x4D86B0), hecho a partir del desensamblado del EXE (no de la reconstruccion en C++).

simulate(inp) recibe la entrada COMPLETA de un caso, tal como la registra el banco en `input` (argumentos de pila, cuenta de la ranura, valores de los colaboradores falsos, FSW inicial), y devuelve
lo que la funcion debe producir: traza de colaboradores, cambios del arena de CAEAudioHardware, cambios de los canales falsos y los efectos x87. No regenera las fixtures de Fixtures.hpp.

Traza (sucesos de 8 palabras): 1 GetSoundBuffer [1, 0x20, sonido & FFFF, ranura & FFFF] | 2 GetLoopOffset [2, 0x20, sonido, ranura] | 3 SetAudioBuffer [3, canal, buffer, tamano, f88, f8c, bucle & FFFF, tasa & FFFF]
| 4 GetLength [4, canal] | 5 SetNotInRoom [5, canal, onStreet] | 6 Play [6, canal, posicion & FFFF, flags & FF, bits de la velocidad]; canal = 0x100 + indice.
"""
import struct
from fractions import Fraction

M32 = 0xFFFFFFFF
BANK = 0x20
CHAN = 0x100
FLAGS_AT = 0x06           # CAEAudioHardware::m_awChannelFlags
CHANNEL_FLAGS_AT = 0x28   # CAEAudioChannel::m_nFlags
ONE_HUNDREDTH = Fraction(struct.unpack('<f', struct.pack('<I', 0x3C23D70A))[0])   # float en 0x858C58 (0.01f)


def sext16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def sext32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def round24(x):
    """Redondeo al par a 24 bits de mantisa (x87 con precision de 24 bits); devuelve (valor, inexacto)."""
    if x == 0:
        return Fraction(0), False
    sign = -1 if x < 0 else 1
    ax = abs(x)
    e = ax.numerator.bit_length() - ax.denominator.bit_length()
    if Fraction(2) ** e > ax:
        e -= 1
    if Fraction(2) ** (e + 1) <= ax:
        e += 1
    scale = Fraction(2) ** (23 - e)
    y = ax * scale
    q = y.numerator // y.denominator
    r = y - q
    if r > Fraction(1, 2) or (r == Fraction(1, 2) and q & 1):
        q += 1
    out = Fraction(q) / scale
    return sign * out, out != ax


def percent_position(percent, length):
    """Posicion en porcentaje (fild, fmul 0.01f, fimul longitud, fstp float, floor, _ftol2). Devuelve (palabra baja del entero, PE, entero floor)."""
    a, pe1 = round24(Fraction(percent) * ONE_HUNDREDTH)
    b, pe2 = round24(a * length)
    fl = b.numerator // b.denominator          # floor exacto
    return fl & M32, pe1 or pe2, fl


def ext80(d):
    """Entero -> registro x87 de 10 bytes (mantisa de 64 bits con bit entero explicito)."""
    if d == 0:
        return bytes(10)
    a = abs(d)
    e = a.bit_length() - 1
    return (a << (63 - e)).to_bytes(8, 'little') + (((0x8000 if d < 0 else 0) | (16383 + e))).to_bytes(2, 'little')


def prime_register(i):
    r = bytearray(10)
    for k in range(8):
        r[k] = (0x11 * (i + 1) + k) & 0xFF
    r[7] |= 0x80
    r[8] = i
    r[9] = 0x40
    return bytes(r)


def simulate(inp):
    """Devuelve dict(path, trace, arena_diff, chan_diff, percent: (pe, floor) | None, reached)."""
    a = inp['args']
    chan = sext16(a[0])
    out = dict(path=None, trace=[], arena_diff=[], chan_diff=[], percent=None, idx=None, clamped=False)
    if chan < 0:
        out['path'] = 'channel_negative'
        return out
    slot = a[1] & 0xFFFF
    if slot >= (inp['nslot'] & 0xFFFF):
        out['path'] = 'slot_out_of_range'
        return out
    sfx, bank = a[2] & 0xFFFF, a[3] & 0xFFFF
    trace = out['trace']
    trace.append([1, BANK, sfx, bank, 0, 0, 0, 0])
    if (inp['buf'] & M32) == 0:
        out['path'] = 'no_buffer'
        return out
    trace.append([2, BANK, sfx, bank, 0, 0, 0, 0])
    idx = (slot + chan) & 0xFFFF
    out['idx'] = idx
    if idx > 0x40:
        out['path'] = 'index_over_64'
        return out
    ch = CHAN + idx
    trace.append([3, ch, inp['buf'] & M32, inp['size'] & M32, sfx, bank, inp['loop'] & 0xFFFF, inp['rate'] & 0xFFFF])
    if not inp['setret']:
        out['path'] = 'set_audio_buffer_failed'
        return out
    flags = a[5] & 0xFFFF
    pos = sext16(a[4])
    if pos < 0:
        pos = 0
    length = sext32(inp['length'])
    trace.append([4, ch, 0, 0, 0, 0, 0, 0])
    percent = None
    if flags & 8 and pos > 0:
        low, pe, fl = percent_position(pos, length)
        percent = (pe, fl)
        pos = low
    out['clamped'] = sext16(pos) > length
    if out['clamped']:
        pos = length & M32
    if inp['n3d'] & 0xFF:
        trace.append([5, ch, 0 if flags & 0x200 else 1, 0, 0, 0, 0, 0])
    trace.append([6, ch, pos & 0xFFFF, flags & 0xFF, a[6] & M32, 0, 0, 0])
    out['percent'] = percent
    out['path'] = 'full'
    flags_at = FLAGS_AT + 2 * (chan + slot)
    out['arena_diff'] = [[flags_at, flags.to_bytes(2, 'little').hex()]]
    out['chan_diff'] = [[idx, [[CHANNEL_FLAGS_AT, (sext16(flags) & M32).to_bytes(4, 'little').hex()]]]]
    return out


def expected_float_state(inp, sim):
    """Efectos x87 que se pueden predecir sin el EXE: TOP = 0, sticky de precision (PE) tras el porcentaje, y los diez bytes de ST0..ST7 (T-2 = R, T-1 = fraccion = +0 tras el floor)."""
    regs = [prime_register(i) for i in range(8)]
    pe = bool(inp['fsw'] & 0x20)
    if sim['percent'] is not None:
        inexact, fl = sim['percent']
        pe = pe or inexact
        regs[6] = ext80(fl)
        regs[7] = bytes(10)
    return dict(pe=pe, regs=regs, sticky=inp['fsw'] & 0x1F)


def parse_fp(hex_image):
    b = bytes.fromhex(hex_image)
    return dict(fcw=struct.unpack_from('<H', b, 0)[0], fsw=struct.unpack_from('<H', b, 4)[0], ftw=struct.unpack_from('<H', b, 8)[0], regs=[b[28 + 10 * i:38 + 10 * i] for i in range(8)])
