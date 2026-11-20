"""Utilidades para leer el EXE original (solo lectura) y recorrer funciones de libpng/zlib."""
import struct, csv, re, collections
import capstone
import os
EXE = os.environ.get('GTASA_EXE', 'E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe')
data = open(EXE, 'rb').read()
e_lfanew = struct.unpack_from('<I', data, 0x3C)[0]
nsec = struct.unpack_from('<H', data, e_lfanew+6)[0]
optsz = struct.unpack_from('<H', data, e_lfanew+20)[0]
BASE = struct.unpack_from('<I', data, e_lfanew+24+28)[0]
secs = []
for i in range(nsec):
    o = e_lfanew+24+optsz+i*40
    name = data[o:o+8].rstrip(b'\0').decode()
    vsz, va, rsz, rptr = struct.unpack_from('<IIII', data, o+8)
    secs.append((name, BASE+va, vsz, rptr, rsz))
def read(va, n):
    for name, sva, vsz, rptr, rsz in secs:
        if sva <= va < sva+max(vsz, rsz):
            off = va - sva
            if off >= rsz: return b'\0'*n
            return data[rptr+off: rptr+min(off+n, rsz)]
    return b''
def section_of(va):
    for name, sva, vsz, rptr, rsz in secs:
        if sva <= va < sva+vsz: return name
    return None
def cstring(va, maxn=200):
    b = read(va, maxn)
    i = b.find(b'\0')
    if i < 1: return None
    s = b[:i]
    if all(32 <= c < 127 or c in (9,10,13) for c in s): return s.decode()
    return None
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
def disasm(start, end):
    code = read(start, end-start)
    return list(md.disasm(code, start))
