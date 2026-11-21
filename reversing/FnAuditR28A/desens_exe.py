# Desensamblador de solo lectura del EXE para la auditoria: python dis.py 7FAF50 [bytes]
import sys, struct, csv, os, capstone
EXE = r'D:\Original\_crt1a_work\gta_sa.exe'  # copia identica (SHA1 786d5a9e...) del EXE original 1.0
data = open(EXE, 'rb').read()
e = struct.unpack_from('<I', data, 0x3C)[0]
n = struct.unpack_from('<H', data, e+6)[0]; osz = struct.unpack_from('<H', data, e+20)[0]
BASE = struct.unpack_from('<I', data, e+24+28)[0]
secs = []
for i in range(n):
    o = e+24+osz+i*40
    vsz, va, rsz, rp = struct.unpack_from('<IIII', data, o+8)
    secs.append((BASE+va, vsz, rp, rsz))
def read(va, k):
    for sva, vsz, rp, rsz in secs:
        if sva <= va < sva+max(vsz, rsz):
            off = va-sva
            return data[rp+off: rp+min(off+k, rsz)]
    return b''
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
SIZES = {}
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'RenderWareExe', 'funciones.csv')
if os.path.exists(p):
    for r in csv.DictReader(open(p, encoding='utf-8')):
        SIZES[int(r['va'], 16)] = int(r['bytes'])
def dis(a, size=None):
    # sin tamano conocido: se detiene en el primer ret sin saltos hacia delante pendientes
    size = size or SIZES.get(a, 600)
    far = a
    for i in md.disasm(read(a, size), a):
        print('  %08X  %s %s' % (i.address, i.mnemonic, i.op_str))
        if i.mnemonic.startswith('j') and i.op_str.startswith('0x'):
            t = int(i.op_str, 16)
            if t > far: far = t
        if i.mnemonic == 'ret' and i.address >= far: break
if __name__ == '__main__':
    for arg in sys.argv[1:]:
        a = int(arg.split(':')[0], 16); s = int(arg.split(':')[1]) if ':' in arg else None
        print('== %08X (%s bytes)' % (a, s or SIZES.get(a, 160))); dis(a, s)
