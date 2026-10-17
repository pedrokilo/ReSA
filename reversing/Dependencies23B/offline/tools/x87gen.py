"""Traduce las instrucciones x87 de un rango del EXE a llamadas de la maquina FlcX87 (flc_soft.h), decodificando por los bytes (SDM) y no por el mnemonico.
Uso: python x87gen.py <inicio hex> <fin hex>   (fin exclusivo). Las instrucciones no x87 se imprimen como comentario.
Semantica de los registros: ST(i) con i = 0 en la cima."""
import sys, struct, capstone

EXE = open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe', 'rb').read()
pe = struct.unpack_from('<I', EXE, 0x3c)[0]
nsec = struct.unpack_from('<H', EXE, pe + 6)[0]
opt = struct.unpack_from('<H', EXE, pe + 20)[0]
base = struct.unpack_from('<I', EXE, pe + 24 + 28)[0]
secs = []
o = pe + 24 + opt
for i in range(nsec):
    vs, va, rs, rp = struct.unpack_from('<IIII', EXE, o + 8 + i * 40)
    secs.append((va, vs, rp, rs))


def off(a):
    r = a - base
    for va, vs, rp, rs in secs:
        if va <= r < va + max(vs, rs):
            return rp + r - va


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = False

ARITH = {0: ('+', 'add'), 1: ('*', 'mul'), 4: ('-', 'sub'), 5: ('-', 'subr'), 6: ('/', 'div'), 7: ('/', 'divr')}


def mem_name(op):
    return op.split('ptr')[-1].strip() if 'ptr' in op else op


def decode(b, ins):
    """Devuelve una cadena C++ o None si no es x87."""
    op = b[0]
    if not (0xD8 <= op <= 0xDF):
        return None
    modrm = b[1]
    mod = modrm >> 6
    reg = (modrm >> 3) & 7
    rm = modrm & 7
    opstr = ins.op_str
    m = mem_name(opstr)
    if mod != 3:  # operando de memoria
        if op == 0xD8:      # m32 float: fadd fmul fcom fcomp fsub fsubr fdiv fdivr
            if reg == 2: return 'fp.FcomMem(%s, false);' % m
            if reg == 3: return 'fp.FcomMem(%s, true);' % m
            sym, nm = ARITH[reg]
            return 'fp.F%s(%s);   // ST0 = %s' % (nm, m, {'add': 'ST0 + m', 'mul': 'ST0 * m', 'sub': 'ST0 - m', 'subr': 'm - ST0', 'div': 'ST0 / m', 'divr': 'm / ST0'}[nm])
        if op == 0xDC:      # m64
            if reg in (2, 3): return 'fp.FcomMemD(%s, %s);' % (m, 'true' if reg == 3 else 'false')
            sym, nm = ARITH[reg]
            return 'fp.F%sD(%s);' % (nm, m)
        if op == 0xD9:
            return {0: 'fp.Fld(%s);' % m, 2: 'fp.Fst(%s);' % m, 3: 'fp.Fstp(%s);' % m, 5: 'fp.Fldcw();', 7: 'fp.Fnstcw();'}.get(reg, '// ?? D9 /%d' % reg)
        if op == 0xDD:
            return {0: 'fp.FldD(%s);' % m, 2: 'fp.FstD(%s);' % m, 3: 'fp.FstpD(%s);' % m}.get(reg, '// ?? DD /%d' % reg)
        if op == 0xDB:
            return {0: 'fp.FldI(%s);' % m, 2: 'fp.Fist(%s);' % m, 3: 'fp.Fistp(%s);' % m, 5: 'fp.FldExt80(%s);' % m}.get(reg, '// ?? DB /%d' % reg)
        if op == 0xDF:
            return {0: 'fp.FldI16(%s);' % m, 5: 'fp.FldI64(%s);' % m, 7: 'fp.Fistp64(%s);' % m}.get(reg, '// ?? DF /%d' % reg)
        if op == 0xDA:      # m32int
            sym, nm = ARITH.get(reg, (None, None))
            if nm: return 'fp.Fi%sI(%s);' % (nm, m)
            return '// ?? DA /%d' % reg
        if op == 0xDE:      # m16int
            return '// ?? DE mem'
        return None
    # registro
    i = rm
    if op == 0xD8:
        if reg == 2: return 'fp.FcomSt(%d, false);' % i
        if reg == 3: return 'fp.FcomSt(%d, true);' % i
        sym, nm = ARITH[reg]
        # ST0 = ST0 op ST(i) ; subr/divr invertidos
        if nm in ('subr', 'divr'): return "fp.Arith('%s', 0, %d, 0, false);" % (sym, i)
        return "fp.Arith('%s', 0, 0, %d, false);" % (sym, i)
    if op == 0xDC:
        # DC: ST(i) = ST(i) op ST0 para add/mul; para sub/div: DC E0+i = FSUBR(ST(i)=ST0-ST(i)), DC E8+i = FSUB (ST(i)=ST(i)-ST0), DC F0+i = FDIVR(ST0/ST(i)), DC F8+i = FDIV (ST(i)/ST0)
        if reg in (0, 1): return "fp.Arith('%s', %d, %d, 0, false);" % ('+' if reg == 0 else '*', i, i)
        if reg == 4: return "fp.Arith('-', %d, 0, %d, false);" % (i, i)   # ST(i) = ST0 - ST(i)
        if reg == 5: return "fp.Arith('-', %d, %d, 0, false);" % (i, i)   # ST(i) = ST(i) - ST0
        if reg == 6: return "fp.Arith('/', %d, 0, %d, false);" % (i, i)
        if reg == 7: return "fp.Arith('/', %d, %d, 0, false);" % (i, i)
    if op == 0xDE:
        if reg in (0, 1): return "fp.Arith('%s', %d, %d, 0, true);" % ('+' if reg == 0 else '*', i, i)
        if reg == 3 and modrm == 0xD9: return 'fp.Fcompp();'
        if reg == 4: return "fp.Arith('-', %d, 0, %d, true);" % (i, i)
        if reg == 5: return "fp.Arith('-', %d, %d, 0, true);" % (i, i)
        if reg == 6: return "fp.Arith('/', %d, 0, %d, true);" % (i, i)
        if reg == 7: return "fp.Arith('/', %d, %d, 0, true);" % (i, i)
    if op == 0xD9:
        if reg == 0: return 'fp.FldSt(%d);' % i
        if reg == 1: return 'fp.Fxch(%d);' % i
        tbl = {0xE0: 'fp.Fchs();', 0xE1: 'fp.Fabs();', 0xE8: 'fp.FldOne();', 0xEE: 'fp.FldZero();', 0xFA: 'fp.Fsqrt();', 0xFE: 'fp.Fsin();', 0xFF: 'fp.Fcos();', 0xF3: 'fp.Fpatan();', 0xEB: 'fp.FldPi();'}
        return tbl.get(modrm, '// ?? D9 %02X' % modrm)
    if op == 0xDD:
        if reg == 3: return 'fp.FstpSt(%d);' % i
        if reg == 2: return 'fp.FstSt(%d);' % i
    if op == 0xDF and modrm == 0xE0:
        return 'fp.Fnstsw();'
    return '// ?? %s' % ins.mnemonic


def main():
    a = int(sys.argv[1], 16)
    e = int(sys.argv[2], 16)
    code = EXE[off(a):off(a) + (e - a) + 16]
    for ins in md.disasm(code, a):
        if ins.address >= e:
            break
        t = decode(bytes(ins.bytes), ins)
        if t:
            print('    %-48s // %x: %s %s' % (t, ins.address, ins.mnemonic, ins.op_str))
        else:
            print('    // %x: %s %s' % (ins.address, ins.mnemonic, ins.op_str))


if __name__ == '__main__':
    main()
