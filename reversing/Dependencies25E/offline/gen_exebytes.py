"""Genera exebytes.inc (bytes reales de las 16 funciones del EXE, con la lista de palabras absolutas a reubicar) y cpp_bodies.inc (cuerpos C++ extraidos tal cual de los
fuentes del parche, con toda direccion absoluta del EXE desplazada por gDelta).

En el arnes el codigo y los datos del EXE viven en un bloque reubicado (el espacio bajo de direcciones de un proceso de 32 bits no esta libre de forma fiable):
gDelta = base_del_bloque - 0x400000. Los saltos y llamadas relativos no necesitan cambios; las direcciones absolutas (operandos de memoria sin base ni indice e
inmediatos en 0x400000..0xE00000) se listan en `reloc` y el arnes les suma gDelta.

Uso:  python gen_exebytes.py [ruta\\gta-sa.exe | -] [raiz-del-arbol-con-el-parche]
La raiz por defecto es el arbol de trabajo (D:\\Original\\gta-reversed-master); pasa la carpeta `b` del parche para probar la reconstruccion antes de aplicarla.
"""
import re
import struct
import sys
from pathlib import Path

import capstone
from capstone.x86 import X86_OP_IMM, X86_OP_MEM

HERE = Path(__file__).resolve().parent
exe = open(sys.argv[1] if len(sys.argv) > 1 and sys.argv[1] != '-' else r'D:\Grand Theft Auto San Andreas\gta-sa.exe', 'rb').read()
ROOT = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(r'D:\Original\gta-reversed-master')
pe = struct.unpack_from('<I', exe, 0x3c)[0]
nsec = struct.unpack_from('<H', exe, pe + 6)[0]
opt = struct.unpack_from('<H', exe, pe + 20)[0]
base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
secs = []
o = pe + 24 + opt
for i in range(nsec):
    vs, va, rs, rp = struct.unpack_from('<IIII', exe, o + 8 + i * 40)
    secs.append((va, vs, rp, rs))


def off(a):
    r = a - base
    for va, vs, rp, rs in secs:
        if va <= r < va + max(vs, rs):
            return rp + r - va
    raise KeyError(hex(a))


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
LOW, HIGH = 0x400000, 0xE00000   # rango de direcciones absolutas del EXE (codigo, importaciones y datos)


def relocs(addr, code):
    """Desplazamientos (dentro de la funcion) de las palabras de 32 bits que son direcciones absolutas del EXE."""
    out = []
    for i in md.disasm(code, addr):
        for op in i.operands:
            if op.type == X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and LOW <= op.mem.disp < HIGH:
                assert i.disp_size == 4
                out.append(i.address - addr + i.disp_offset)
            elif op.type == X86_OP_IMM and i.mnemonic not in ('call', 'jmp') and not i.mnemonic.startswith('j') and LOW <= op.imm < HIGH:
                assert i.imm_size == 4
                out.append(i.address - addr + i.imm_offset)
    return out


# (direccion, bytes) de las 16 funciones
FUNCS = [(0x8052F0, 6), (0x807720, 34), (0x820C90, 23), (0x808470, 34), (0x809400, 12), (0x8094B0, 8), (0x809510, 17), (0x809530, 7), (0x809540, 16), (0x80A440, 1), (0x80A470, 34),
         (0x821D1E, 18), (0x823FD1, 35), (0x824023, 35), (0x824257, 18), (0x82B181, 9)]
text = 'static const struct { unsigned addr, len; unsigned char bytes[64]; unsigned nreloc, reloc[8]; } kExeFunctions[] = {\n'
for a, n in FUNCS:
    p = off(a)
    code = bytes(exe[p:p + n])
    rel = relocs(a, code)
    text += '  {0x%X, %d, {%s}, %d, {%s}},\n' % (a, n, ','.join('0x%02x' % x for x in code), len(rel), ','.join(str(r) for r in rel) or '0')
    print('0x%X: palabras absolutas en los desplazamientos %s' % (a, rel))
text += '};\n'
(HERE / 'exebytes.inc').write_text(text)

# cuerpos C++ tal cual estan en los fuentes
SOURCES = {
    'source/game_sa/RenderWare/rw/rwcore.cpp': ['RxHeapGetGlobalHeap', '_rwFrameListDeinitialize', 'RwD3D9RasterIsCompressed'],
    'source/game_sa/RenderWare/rw/rwplcore.cpp': ['_rwPluginRegistryGetPluginOffset', '_rwSListDestroyEndEntries', '_rwSListGetNumEntries', '_rwSListGetEntry', '_rwSListGetBegin',
                                                 '_rwSListGetEnd', '_rwStringClose', '_rwStringStreamGetSize'],
    'source/game_sa/VehicleRecording.cpp': ['CrtAtexit', 'CrtLockFile2', 'CrtUnlockFile2', 'CrtMalloc', 'CrtErrno'],
}


def shift(body):
    """Toda direccion absoluta del EXE (0x4xxxxx..0xDxxxxx) en el codigo pasa a (direccion + gDelta); los comentarios no se tocan."""
    lines = []
    for line in body.split('\n'):
        code, sep, comment = line.partition('//')
        code = re.sub(r'\b0x([4-9A-Da-d][0-9A-Fa-f]{5})\b', r'(0x\1 + gDelta)', code)
        lines.append(code + sep + comment)
    return '\n'.join(lines)


out = ''
for path, names in SOURCES.items():
    src = (ROOT / path).read_text(encoding='utf-8').replace('\r\n', '\n')
    for name in names:
        # comentarios contiguos + cabecera + cuerpo hasta la llave de columna 0
        m = re.search(r'((?://[^\n]*\n)*)((?:static )?[\w\*: <>]+?[\s\*]%s\([^\n]*\{\n(?:.*?\n)??\}\n)' % re.escape(name), src, re.S)
        if not m:
            raise SystemExit('no encuentro ' + name + ' en ' + path)
        out += m.group(1) + shift(m.group(2)) + '\n'
(HERE / 'cpp_bodies.inc').write_text(out, encoding='utf-8')
print('exebytes.inc y cpp_bodies.inc generados')
