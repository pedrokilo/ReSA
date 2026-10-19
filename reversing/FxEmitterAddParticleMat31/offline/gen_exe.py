"""Extrae del EXE canonico (D:/Grand Theft Auto San Andreas/gta-sa.exe) los bytes de FxEmitter_c::AddParticle(matriz) (0x4A4050) y de _ftol2 (0x821B40), con la lista de reubicaciones (operandos absolutos y llamadas), para que cphaddmat31.cpp los ejecute en un proceso aislado
(sin el juego) frente al codigo C++. Uso: python -I gen_exe.py  ->  exebytes.inc"""
import struct
import capstone

EXE = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
exe = open(EXE, 'rb').read()
pe = struct.unpack_from('<I', exe, 0x3C)[0]
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


CONST_LO, CONST_HI = 0x858000, 0x85B000      # .rdata con las constantes float/double que cargan las funciones
TABLE = 0xBB3E00                              # tabla de senos del juego (256 floats)
FUNCS = {
    'ADD': (0x4A4050, 0x4A41E0),
    'FTOL': (0x821B40, 0x821BB5),
}
CALLS = {0x4A93C0: 'GETPARTICLE', 0x4A9400: 'FREEUP', 0x821B1E: 'RAND', 0x821B40: 'FTOL', 0x4A9DC0: 'GETRW', 0x7ED9B0: 'NORMALIZE', 0x7EDDF0: 'TRANSFORM',
         0x4A1660: 'ROTATE', 0x4A21D0: 'UPDATEPARTICLE', 0x4A8DF0: 'ADDITEM', 0x4A4960: 'PEI', 0x4A9440: 'CREATE', 0x4A9460: 'DESTROY', 0x7F18A0: 'RWUPDATE',
         0x7F18B0: 'RWMULTIPLY', 0x4A2580: 'CREATEPARTICLE'}
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
out = []
for name, (lo, hi) in FUNCS.items():
    code = exe[off(lo):off(lo) + (hi - lo)]
    patches = []
    for i in md.disasm(code, lo):
        if i.mnemonic == 'call' and i.operands[0].type == capstone.x86.X86_OP_IMM:
            t = i.operands[0].imm
            assert t in CALLS, (name, hex(i.address), hex(t))
            patches.append((i.address - lo + 1, 1, CALLS[t]))
            continue
        for op in i.operands:
            if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0:
                d = op.mem.disp & 0xFFFFFFFF
                if CONST_LO <= d < CONST_HI and op.mem.index == 0:
                    patches.append((i.address - lo + i.disp_offset, 0, 'CONST'))
                elif d == TABLE:
                    patches.append((i.address - lo + i.disp_offset, 0, 'TABLE'))
                else:
                    assert False, (name, hex(i.address), i.op_str)
    out.append('static const unsigned char k%s[] = {%s};' % (name, ','.join('0x%02x' % b for b in code)))
    out.append('static const Patch k%sPatches[] = {%s};' % (name, ','.join('{%d,%d,"%s"}' % p for p in patches) + (',' if patches else '') + '{0,-1,""}'))
out.append('static const unsigned char kConst[] = {%s};' % ','.join('0x%02x' % b for b in exe[off(CONST_LO):off(CONST_HI)]))
open('exebytes.inc', 'w').write('struct Patch { unsigned off; int kind; const char* sym; };\n' + '\n'.join(out) + '\n')
print('ok', {k: hi - lo for k, (lo, hi) in FUNCS.items()})
