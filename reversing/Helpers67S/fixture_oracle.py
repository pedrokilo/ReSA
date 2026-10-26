"""Oraculo del banco Helpers67S, modelado a partir del DESENSAMBLADO del EXE (no del C++ de la DLL).

Una funcion (16384 casos); la fila `input` que registra el banco basta para calcular lo que debe observarse (`expected(row)`), igual para el EXE y para la DLL:

  CDecisionMakerTypes::CDecisionMakerTypes 0x4650F0 (constructor thiscall, ECX = this, `ret`). Marco SEH propio (push 0xFFFFFFFF / 0x837CD9 / fs:[0]), que se retira al salir.
        [this+0] = 0; `eh vector constructor iterator` (0x821D30) de 20 elementos de 0x99C bytes en this+4 con el constructor 0x4650A0 (41 x 0x6040C0 = CDecision::SetDefault y despues
        41 x 0x600530 = SetDefault) y el destructor 0x4634C0 (`ret`); los decision makers por defecto en this+0xC1B4, 0xCB50, 0xD4EC, 0xDE88 y 0xE824, cada uno 41 x 0x6040C0 + 41 x 0x600530
        en linea. SetDefault (0x600530): 6 x { [d+4i] = 0xFFFFFFFF; [d+0x18+4i] = 0; word [d+0x30+2i] = 0 } = una decision de 0x3C bytes: 24 bytes 0xFF y 36 bytes 0.
        Globales: 0xC0AFE0..0xC0AFF3 (5 dwords) = 0; 0xC0AFF4..0xC0B01B (10 dwords de valor 0x00010001: ScriptReferenceIndex[20] = 1); 0xC0B01C..0xC0B02F (5 dwords) = 0.
        NO toca this+0xC034..0xC1B3 (m_EventIndices) ni 0xC0B030 (instancia). Salida: EAX = this, ECX = fs:[0] de entrada, EDX = 0.

Autoprueba: `python -I fixture_oracle.py` (imprime «ok»).
"""
import struct
import zlib

CASES = 16384
NAMES = ['CDecisionMakerTypes::CDecisionMakerTypes']
ADDRESS = [0x4650F0]
NAME_OF = dict(enumerate(NAMES))
M32 = 0xFFFFFFFF

# Constantes de arena: DEBEN coincidir con Fixtures.hpp (fixture_selfcheck.py las compara con el volcado compilado).
LAYOUT = dict(arena=0xF250, obj_bytes=0xF1C0, region_addr=0xC0AFD0, region_bytes=0x70)
GUARD = 0x40
OBJ_BYTES = 0xF1C0
EVENTS_OFF, DEFAULTS_OFF = 0xC034, 0xC1B4
FLAGS_OFF, INDEX_OFF, ACTIVE_OFF, INSTANCE_OFF = 0x10, 0x24, 0x4C, 0x60
TILE = 127
DECISION = b'\xff' * 24 + b'\x00' * 36


class Ambiguous(Exception):
    """sin uso en este banco (no hay aritmetica de coma flotante); se conserva para la interfaz comun de run_local.py"""


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


_tiles = {}


def tile(seed):
    t = _tiles.get(seed)
    if t is None:
        t = _tiles[seed] = b''.join(struct.pack('<I', rnd(seed, j)) for j in range(TILE))
    return t


def fill_arena(mode, seed, size):
    if mode == 1:
        return bytes(size)
    if mode == 2:
        return b'\xff' * size
    if mode == 3:
        return b'\xcd' * size
    if mode == 4:
        return b'\xa5' * size
    s = (seed ^ 0x5A5A5A5A) if mode == 5 else seed
    t = tile(s)
    return (t * (size // len(t) + 1))[:size]


def expected_ctor(row):
    off = row['obj_off']
    arena = fill_arena(row['mode'], row['seed'], LAYOUT['arena'])
    obj = bytearray(arena[off:off + OBJ_BYTES])
    obj[0:4] = b'\0\0\0\0'
    obj[4:EVENTS_OFF] = DECISION * 820
    obj[DEFAULTS_OFF:OBJ_BYTES] = DECISION * 205
    region = bytearray(bytes.fromhex(row['region_pre']))
    assert len(region) == LAYOUT['region_bytes']
    region[FLAGS_OFF:FLAGS_OFF + 20] = bytes(20)
    region[INDEX_OFF:INDEX_OFF + 40] = b'\x01\x00' * 20
    region[ACTIVE_OFF:ACTIVE_OFF + 20] = bytes(20)
    mode, gmode = row['mode'], row['gmode']
    stats = {'family': row['family'],
             'misaligned': off & 3 != 0, 'late_offset': off >= 0x48,
             'arena_noise': mode in (0, 5), 'arena_noise5': mode == 5, 'arena_zero': mode == 1, 'arena_ff': mode == 2, 'arena_cd': mode == 3, 'arena_a5': mode == 4,
             'g_noise': gmode == 0, 'g_zero': gmode == 1, 'g_ff': gmode == 2, 'g_final': gmode == 3, 'g_inverse': gmode == 4, 'g_80': gmode == 5,
             'eax_zero': row['eax'] == 0, 'edx_ffff': row['edx'] == M32}
    pre = bytes.fromhex(row['region_pre'])
    stats['globals_changed'] = pre[FLAGS_OFF:ACTIVE_OFF + 20] != bytes(region[FLAGS_OFF:ACTIVE_OFF + 20])
    return {'obj_crc': zlib.crc32(bytes(obj)), 'rest_ok': True, 'region': bytes(region).hex(), 'eax_is_this': True, 'ecx_is_head': True, 'edx_zero': True}, stats


def expected(fn, row, case=None):
    assert fn == NAMES[0]
    return expected_ctor(row)


# ------------------------------------------------------------------------------------------------ autoprueba
def selftest():
    # crc32: valor conocido
    assert zlib.crc32(b'123456789') == 0xCBF43926
    # tabla de la baldosa: una palabra del ruido
    assert tile(7)[:4] == struct.pack('<I', rnd(7, 0)) and len(tile(7)) == TILE * 4
    # arena constante
    assert fill_arena(1, 0, 8) == bytes(8) and fill_arena(2, 0, 4) == b'\xff' * 4
    assert fill_arena(0, 5, 0xF250)[TILE * 4:TILE * 4 + 4] == tile(5)[:4]
    # caso a mano: arena de ceros, region de ceros salvo la instancia
    region = bytearray(0x70)
    struct.pack_into('<I', region, INSTANCE_OFF, 0xDEADBEEF)
    row = dict(func=0, n=0, family=0, obj_off=0x40, mode=1, seed=1, gmode=1, gseed=0, region_pre=bytes(region).hex(), eax=0, edx=0, stack_seed=0)
    obs, st = expected_ctor(row)
    obj = bytearray(OBJ_BYTES)
    for d in range(820):
        obj[4 + 60 * d:4 + 60 * d + 24] = b'\xff' * 24
    for d in range(205):
        obj[DEFAULTS_OFF + 60 * d:DEFAULTS_OFF + 60 * d + 24] = b'\xff' * 24
    assert obs['obj_crc'] == zlib.crc32(bytes(obj))
    r = bytes.fromhex(obs['region'])
    assert r[INDEX_OFF:INDEX_OFF + 4] == b'\x01\x00\x01\x00' and r[INDEX_OFF + 40:INDEX_OFF + 44] == bytes(4) and r[INSTANCE_OFF:INSTANCE_OFF + 4] == struct.pack('<I', 0xDEADBEEF)
    assert r[:FLAGS_OFF] == bytes(FLAGS_OFF) and r[FLAGS_OFF:INDEX_OFF] == bytes(INDEX_OFF - FLAGS_OFF) and st['globals_changed'] is True
    # el contador y los indices de eventos: ruido conservado
    row['mode'] = 0
    row['seed'] = 3
    obs2, _ = expected_ctor(row)
    arena = fill_arena(0, 3, LAYOUT['arena'])
    obj2 = bytearray(arena[0x40:0x40 + OBJ_BYTES])
    obj2[0:4] = bytes(4)
    obj2[4:EVENTS_OFF] = DECISION * 820
    obj2[DEFAULTS_OFF:] = DECISION * 205
    assert obs2['obj_crc'] == zlib.crc32(bytes(obj2)) and bytes(obj2[EVENTS_OFF:DEFAULTS_OFF]) == arena[0x40 + EVENTS_OFF:0x40 + DEFAULTS_OFF]
    # desalineado: los eventos siguen siendo los de la arena, desplazados
    row['obj_off'] = 0x43
    obs3, _ = expected_ctor(row)
    assert obs3['obj_crc'] != obs2['obj_crc']
    return 'ok'


if __name__ == '__main__':
    print(selftest())
