"""Contraste del oraculo con los BYTES del EXE (sin juego): emula con un intérprete minimo x86 (capstone) la funcion 0x5D44B0..0x5D4617 y compara, en los 4608 casos de Fixtures.hpp,
la memoria final del ped / matriz, la entrada/salida resuelta y la traza de colaboradores con lo que predice fixture_oracle.py.

  python -I fixture_emulate.py [volcado]     # volcado: salida de fixture_dump.exe (si falta, se compila como en fixture_mirror.py)

Los colaboradores (SetCharCreatedBy, GetWeaponInfo, RequestModel, LoadAllRequestedModels, GiveWeapon, SetCurrentWeapon) se emulan con los mismos efectos que los falsos del banco
(Core.hpp) y destruyen EAX/ECX/EDX al volver (la funcion no puede depender de ellos). Requiere `capstone` y el EXE canonico D:\\Grand Theft Auto San Andreas\\gta-sa.exe. Imprime `ok`.
"""
import re
import struct
import sys
import tempfile
from pathlib import Path

import capstone
from capstone import x86_const as X

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as O
import fixture_mirror as M

EXE = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
START, END = 0x5D44B0, 0x5D461A
M32 = 0xFFFFFFFF
PED, MAT, SAVE, STATES, POOL, STACK, INFO, GLOBALS = 0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 0x60000000, 0x70000000, 0x96A7D0
STATES_AT = STATES + 16
STORAGE = 0x58000000
RET_MAGIC = 0xDEAD0000


def load_code():
    d = open(EXE, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3C)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
    for i in range(nsec):
        o = pe + 24 + opt + 40 * i
        name = d[o:o + 8].rstrip(b'\0')
        vs, va, rs, ro = struct.unpack_from('<IIII', d, o + 8)
        if name == b'.text':
            code = d[ro + (START - base - va):ro + (END - base - va)]
            md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
            md.detail = True
            return {i.address: i for i in md.disasm(code, START)}
    raise SystemExit('sin .text')


class Mem:
    def __init__(self):
        self.r = []

    def add(self, base, data):
        b = bytearray(data)
        self.r.append((base, b))
        return b

    def find(self, a, n):
        for base, b in self.r:
            if base <= a and a + n <= base + len(b):
                return b, a - base
        raise RuntimeError('acceso fuera de la memoria emulada: %#x (%d)' % (a, n))

    def read(self, a, n):
        b, o = self.find(a & M32, n)
        return int.from_bytes(b[o:o + n], 'little')

    def write(self, a, n, v):
        b, o = self.find(a & M32, n)
        b[o:o + n] = (v & ((1 << (8 * n)) - 1)).to_bytes(n, 'little')


REGS32 = {X.X86_REG_EAX: 'eax', X.X86_REG_ECX: 'ecx', X.X86_REG_EDX: 'edx', X.X86_REG_EBX: 'ebx', X.X86_REG_ESP: 'esp', X.X86_REG_EBP: 'ebp', X.X86_REG_ESI: 'esi', X.X86_REG_EDI: 'edi'}
REGS8 = {X.X86_REG_AL: 'eax', X.X86_REG_CL: 'ecx', X.X86_REG_DL: 'edx'}


class Cpu:
    def __init__(self, code):
        self.code = code
        self.reg = {k: 0 for k in ('eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi')}
        self.zf = self.sf = False
        self.fp = []
        self.mem = None
        self.nev = 0

    def ea(self, op):
        m = op.mem
        a = m.disp
        if m.base:
            a += self.reg[REGS32[m.base]]
        if m.index:
            a += self.reg[REGS32[m.index]] * m.scale
        return a & M32

    def get(self, op):
        if op.type == X.X86_OP_REG:
            if op.reg in REGS32:
                return self.reg[REGS32[op.reg]]
            return self.reg[REGS8[op.reg]] & 0xFF
        if op.type == X.X86_OP_IMM:
            return op.imm & M32
        return self.mem.read(self.ea(op), op.size)

    def put(self, op, v):
        if op.type == X.X86_OP_REG:
            if op.reg in REGS32:
                self.reg[REGS32[op.reg]] = v & M32
            else:
                k = REGS8[op.reg]
                self.reg[k] = (self.reg[k] & 0xFFFFFF00) | (v & 0xFF)
        else:
            self.mem.write(self.ea(op), op.size, v)

    def push(self, v):
        self.reg['esp'] = (self.reg['esp'] - 4) & M32
        self.mem.write(self.reg['esp'], 4, v)

    def pop(self):
        v = self.mem.read(self.reg['esp'], 4)
        self.reg['esp'] = (self.reg['esp'] + 4) & M32
        return v

    def flags(self, res, size):
        m = (1 << (8 * size)) - 1
        res &= m
        self.zf = res == 0
        self.sf = bool(res >> (8 * size - 1))


def run(code, save, ped, mat, states_byte, infos, has_matrix, exit_idx, trace_out):
    mem = Mem()
    pedb = mem.add(PED, ped)
    matb = mem.add(MAT, mat)
    mem.add(SAVE, save)
    st = bytearray(16 + 2048 + 64)
    mem.add(STATES, st)
    mem.add(POOL, struct.pack('<II', STORAGE, STATES_AT) + bytes(24))
    glob = mem.add(GLOBALS, bytes(0x20))
    glob[0x96A7D8 - GLOBALS:0x96A7D8 - GLOBALS + 4] = struct.pack('<I', POOL)
    stack = mem.add(STACK, bytes(0x1000))
    mem.add(INFO, bytes(0x100))
    for i, (t, m1, m2) in enumerate(infos):
        mem.write(INFO + 0x10 * i + 0xC, 4, m1 & M32)
        mem.write(INFO + 0x10 * i + 0x10, 4, m2 & M32)
    mem.write(INFO + 0xF0, 4, M32)
    # el estado del byte de salida
    if exit_idx != -1:
        mem.write(STATES_AT + exit_idx, 1, states_byte)
    if has_matrix:
        mem.write(PED + 0x14, 4, MAT)
    cpu = Cpu(code)
    cpu.mem = mem
    cpu.reg['esp'] = STACK + 0x800
    cpu.reg.update(ebx=0x11111111, esi=0x22222222, edi=0x33333333, ebp=0x44444444, eax=0x55555555, edx=0x66666666)
    cpu.reg['ecx'] = SAVE
    cpu.push(PED)
    cpu.push(RET_MAGIC)
    esp0 = cpu.reg['esp'] + 8
    ncalls = [0]
    info_idx = {t: i for i, (t, _, _) in enumerate(infos)}

    def digest():
        return O.digest(bytearray(pedb), bytearray(matb))

    def stub(addr):
        esp = cpu.reg['esp']
        arg = lambda k: mem.read(esp + 4 * (k + 1), 4)      # [esp] = direccion de retorno
        self_label = 1 if cpu.reg['ecx'] == PED else 0xBAD
        pop = 0
        eax = 0
        if addr == 0x5E47E0:
            trace_out.append([O.EV_CREATED, arg(0) & 0xFF, self_label, 0, 0, digest()]); pop = 4
            pedb[0x72D], pedb[0x72E], pedb[0x2F] = 0xA1, 0xA2, 0xA3
        elif addr == 0x743C60:
            t, skill = arg(0), arg(1) & 0xFF
            trace_out.append([O.EV_INFO, t, skill, 0, 0, digest()])
            eax = INFO + 0x10 * info_idx[t] if t in info_idx else INFO + 0xF0 - 0
            cpu.caller_pops = 8
        elif addr == 0x4087E0:
            trace_out.append([O.EV_REQUEST, arg(0), arg(1), 0, 0, digest()])
            cpu.caller_pops = 8
        elif addr == 0x40EA10:
            trace_out.append([O.EV_LOAD, arg(0) & 0xFF, 0, 0, 0, digest()])
            cpu.caller_pops = 4
        elif addr == 0x5E6080:
            trace_out.append([O.EV_GIVE, self_label, arg(0), arg(1), arg(2) & 0xFF, digest()]); pop = 12
            k = ncalls[0] % 13
            mem.write(PED + 0x5A0 + 0x1C * k + 8, 4, 0xC0DE0000 | ncalls[0])
            ncalls[0] += 1
        elif addr == 0x5E61F0:
            slot = arg(0)
            trace_out.append([O.EV_SETCUR, self_label, slot, 0, 0, digest()]); pop = 4
            pedb[0x718] = slot & 0xFF
            pedb[0x2F] = 0xA5
        else:
            raise RuntimeError('llamada inesperada a %#x' % addr)
        ret = cpu.pop()
        cpu.reg['esp'] = (cpu.reg['esp'] + pop) & M32
        cpu.reg.update(eax=eax, ecx=0xC1C1C1C1, edx=0xD2D2D2D2)      # la funcion no puede depender de los volatiles
        return ret

    pc = START
    saved = {k: cpu.reg[k] for k in ('ebx', 'esi', 'edi', 'ebp')}
    for _ in range(100000):
        ins = code[pc]
        mn = ins.mnemonic
        ops = ins.operands
        nxt = ins.address + ins.size
        if mn == 'push':
            cpu.push(cpu.get(ops[0]) if ops[0].type != X.X86_OP_IMM else ops[0].imm & M32)
        elif mn == 'pop':
            cpu.put(ops[0], cpu.pop())
        elif mn == 'mov':
            cpu.put(ops[0], cpu.get(ops[1]))
        elif mn == 'movsx':
            v = cpu.get(ops[1])
            if ops[1].size == 1:
                v = v - 256 if v & 0x80 else v
            cpu.put(ops[0], v & M32)
        elif mn == 'lea':
            cpu.put(ops[0], cpu.ea(ops[1]))
        elif mn == 'xor':
            r = cpu.get(ops[0]) ^ cpu.get(ops[1]); cpu.put(ops[0], r); cpu.flags(r, ops[0].size)
        elif mn == 'add':
            r = (cpu.get(ops[0]) + cpu.get(ops[1])) & M32; cpu.put(ops[0], r); cpu.flags(r, 4)
        elif mn == 'dec':
            r = (cpu.get(ops[0]) - 1) & M32; cpu.put(ops[0], r); cpu.flags(r, 4)
        elif mn == 'imul':
            a = cpu.get(ops[1]); b = cpu.get(ops[2]) if len(ops) > 2 else cpu.get(ops[0])
            cpu.put(ops[0], (a * b) & M32)
        elif mn == 'test':
            cpu.flags(cpu.get(ops[0]) & cpu.get(ops[1]), ops[0].size)
        elif mn == 'cmp':
            sz = ops[0].size
            b = cpu.get(ops[1]) & ((1 << (8 * sz)) - 1)
            cpu.flags(cpu.get(ops[0]) - b, sz)
        elif mn == 'je':
            if cpu.zf: nxt = ops[0].imm
        elif mn == 'jne':
            if not cpu.zf: nxt = ops[0].imm
        elif mn == 'jns':
            if not cpu.sf: nxt = ops[0].imm
        elif mn == 'jmp':
            nxt = ops[0].imm
        elif mn == 'fld':
            v = cpu.get(ops[0])
            if (v & 0x7F800000) == 0x7F800000 and (v & 0x7FFFFF) and not v & 0x400000:
                v |= 0x400000                                  # fld de 32 bits silencia un NaN de senalizacion
            cpu.fp.append(v)
        elif mn == 'fstp':
            cpu.put(ops[0], cpu.fp.pop())
        elif mn == 'call':
            tgt = ops[0].imm
            cpu.push(nxt)
            ret = stub(tgt)
            nxt = ret
            if getattr(cpu, 'caller_pops', 0):               # cdecl: la funcion retira los argumentos con `add esp, N`
                cpu.caller_pops = 0
        elif mn == 'ret':
            ret = cpu.pop()
            cpu.reg['esp'] = (cpu.reg['esp'] + (ops[0].imm if ops else 0)) & M32
            assert ret == RET_MAGIC, hex(ret)
            break
        else:
            raise RuntimeError('instruccion sin emular: ' + mn)
        pc = nxt
    else:
        raise RuntimeError('no termina')
    assert cpu.reg['esp'] == esp0, 'ESP'
    assert all(cpu.reg[k] == v for k, v in saved.items()), 'registros no volatiles'
    assert not cpu.fp, 'pila x87 no vacia'
    return bytearray(pedb), bytearray(matb), mem.read(PED + 0x78C, 4)


def main():
    code = load_code()
    if len(sys.argv) > 1:
        lines = Path(sys.argv[1]).read_text().splitlines()
    else:
        with tempfile.TemporaryDirectory() as td:
            lines = M.build_dump(Path(td)).read_text().splitlines()
    bad = 0
    for line in lines:
        n, inp = M.parse(line)
        want, st = O.expected(O.NAME, inp, n)
        save = bytes.fromhex(inp['save'])
        ped = bytearray(O.fill_pre(O.PED, inp['ped_mode'], inp['ped_seed'], inp['ped_const']))
        mat = bytearray(O.fill_pre(O.MAT, inp['mat_mode'], inp['mat_seed'], inp['mat_const']))
        ped[0x14:0x18] = bytes(4)
        ped[0x78C:0x790] = bytes(4)
        trace = []
        # en la emulacion el digest se calcula con el puntero de matriz real en ped+0x14 (no entra en el resumen)
        pedo, mato, enex_raw = run(code, save, ped, mat, inp['state_byte'], [i for i in inp['infos']], bool(inp['has_matrix']), inp['exit_idx'], trace)
        # (el emulador numera los bloques de info en el orden de `infos`; el tipo 0xF0 es el defecto)
        pedo[0x14:0x18] = bytes(4)
        enex = None if enex_raw == 0 else ((enex_raw - STORAGE + (1 << 31)) % (1 << 32)) - (1 << 31)
        got = {
            'ped_head': bytes(pedo[0:0x40]).hex(), 'ped_hp': bytes(pedo[0x540:0x550]).hex(), 'ped_wep': bytes(pedo[0x5A0:0x720]).hex(), 'ped_style': bytes(pedo[0x728:0x730]).hex(),
            'ped_rest': O.rest_hash(pedo), 'enex': enex, 'mat': bytes(mato).hex(), 'trace': trace,
        }
        diff = [k for k in got if got[k] != want[k]]
        if diff:
            bad += 1
            if bad <= 3:
                print('caso', n, 'familia', inp['family'], 'difiere en', diff)
                for k in diff:
                    print('  emulado ', str(got[k])[:200])
                    print('  oraculo ', str(want[k])[:200])
    if bad:
        raise SystemExit('%d casos distintos' % bad)
    print('ok (%d casos emulados contra los bytes del EXE)' % len(lines))


if __name__ == '__main__':
    main()
