"""Deriva, SOLO del ejecutable original (sin mirar el C++ del proyecto), la tabla de ConvertBoneTag2BoneName (0x4D56F0) para el oraculo de Fn183A.

Lectura del desensamblado (dumpbin): eax = dword de pila; si eax > 0xC9 (con signo): sub 0x12D -> 0x12D da la cadena 0x85ED68, 0x12E da 0x85ED74, el resto nulo; si eax == 0xC9: 0x85ED80;
si eax > 0x36 (sin signo): nulo; si no: indice = byte [0x4D5864 + eax]; salto indirecto por la tabla de dwords 0x4D57EC (cada destino es `mov eax, imm32; ret`).
Imprime el diccionario {etiqueta: cadena} y el SHA256 del EXE leido.  python -I derive_bonenames.py <ruta-del-gta-sa.exe>
"""
import hashlib, json, struct, sys
data = open(sys.argv[1], 'rb').read()
pe = struct.unpack_from('<I', data, 0x3C)[0]
nsec = struct.unpack_from('<H', data, pe + 6)[0]
opt = struct.unpack_from('<H', data, pe + 20)[0]
base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + opt + 40 * i
    vsize, va, rsize, raw = struct.unpack_from('<IIII', data, o + 8)
    secs.append((base + va, max(vsize, rsize), raw))


def off(va):
    for s, n, r in secs:
        if s <= va < s + n:
            return r + va - s
    raise ValueError(hex(va))


def u8(va): return data[off(va)]
def u32(va): return struct.unpack_from('<I', data, off(va))[0]
def cstr(va):
    o = off(va); e = data.index(b'\0', o); return data[o:e].decode('ascii')


def target_string(t):
    assert u8(t) == 0xB8 and u8(t + 5) == 0xC3, hex(t)     # mov eax, imm32 ; ret
    return cstr(u32(t + 1))


table = {}
for tag in range(0, 0x37):
    t = u32(0x4D57EC + 4 * u8(0x4D5864 + tag))
    if u8(t) == 0x33 and u8(t + 1) == 0xC0:                  # xor eax,eax ; ret
        continue
    table[tag] = target_string(t)
table[0xC9] = cstr(0x85ED80)
table[0x12D] = cstr(0x85ED68)
table[0x12E] = cstr(0x85ED74)
print(json.dumps({'sha256': hashlib.sha256(data).hexdigest(), 'table': table}, indent=1))
