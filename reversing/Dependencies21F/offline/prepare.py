"""Prepara el arnes nativo (fuera del juego): extrae del EXE los bytes de 0x7305E0 y 0x730740 y del fuente de la clase las dos funciones reconstruidas.

Uso:  python prepare.py <gta_sa.exe> <repo>      (escribe exe_tex.bin, exe_ib.bin y body.inc en la carpeta actual)
Compilar con build.bat (cl x86) y ejecutar harness.exe: compara 2 x 200000 casos aleatorios (memoria completa y traza de Release con hash de los globales en cada llamada).
"""
import struct
import sys
from pathlib import Path

exe = Path(sys.argv[1]).read_bytes()
repo = Path(sys.argv[2])
pe = struct.unpack_from('<I', exe, 0x3c)[0]
nsec = struct.unpack_from('<H', exe, pe + 6)[0]
opt = struct.unpack_from('<H', exe, pe + 20)[0]
base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
secs = [struct.unpack_from('<IIII', exe, pe + 24 + opt + 8 + i * 40) for i in range(nsec)]


def offset(address):
    rva = address - base
    for vsize, va, rsize, rptr in secs:
        if va <= rva < va + max(vsize, rsize):
            return rptr + rva - va
    raise ValueError(hex(address))


for name, address, size in (('exe_tex.bin', 0x7305E0, 0x77), ('exe_ib.bin', 0x730740, 0xAD)):
    Path(name).write_bytes(exe[offset(address):offset(address) + size])
source = (repo / 'source/game_sa/RenderWare/D3DResourceSystem.cpp').read_text(encoding='utf-8')
start, end = source.index('namespace {\n// IUnknown::Release'), source.index('// 0x7306A0')
Path('body.inc').write_text(source[start:end], encoding='utf-8')
print('ok')
