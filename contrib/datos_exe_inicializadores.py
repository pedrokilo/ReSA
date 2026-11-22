"""Decodifica los inicializadores estaticos de _initterm (docs/CodigoVivoExe/fase8-inicializadores.csv) que dependen de DatosExe:
140 constructores con atexit, 35 asignaciones de datos y las constantes x87 raras.

Cada inicializador se ejecuta con un emulador minimo (registros, pila, bucles `dec/jne`, `rep stosd`, lecturas de .data/.rdata del
EXE canonico) que anota los EFECTOS: escrituras a memoria (direccion, ancho, valor) y llamadas (destino, ecx, argumentos de pila).
Con eso cada fila queda clasificada:
  ctor        mov ecx, OBJ; call CTOR ; atexit(funclet)         -> objeto OBJ construido por CTOR y destruido por el funclet
  ctor-vec    vector constructor iterator (0x821D30) sobre OBJ  -> N elementos de tamano S
  dtor-solo   atexit(funclet) sin constructor                   -> objeto en cero con destructor
  escrituras  solo mov/rep stos/bucles                          -> valores iniciales
  otro        lo que el emulador no sabe (llamadas a otras funciones, x87)
y se cruza con docs/hooks.csv para saber si el constructor/destructor ya es C++ (clase y nombre).

Salida: docs/DatosExe/inicializadores.csv (una fila por inicializador) y docs/DatosExe/inicializadores-escrituras.csv (escrituras
resumidas por rangos). La generacion del C++ (source/game_sa/DatosExe/StaticInit.cpp) la hace contrib/datos_exe_staticinit.py.

Uso: python -X utf8 contrib/datos_exe_inicializadores.py
"""
import csv
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from pe_inventory import PE  # noqa: E402
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM, CS_OP_REG  # noqa: E402
from capstone import x86 as X  # noqa: E402

RAIZ = Path(__file__).resolve().parent.parent
EXE = Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')
ATEXIT = 0x821D1E
VEC_CTOR = 0x821D30
UNK = None

REG32 = {X.X86_REG_EAX: 'eax', X.X86_REG_ECX: 'ecx', X.X86_REG_EDX: 'edx', X.X86_REG_EBX: 'ebx', X.X86_REG_ESI: 'esi',
         X.X86_REG_EDI: 'edi', X.X86_REG_EBP: 'ebp', X.X86_REG_ESP: 'esp'}
SUB = {X.X86_REG_AL: ('eax', 0, 8), X.X86_REG_CL: ('ecx', 0, 8), X.X86_REG_DL: ('edx', 0, 8), X.X86_REG_AX: ('eax', 0, 16),
       X.X86_REG_CX: ('ecx', 0, 16), X.X86_REG_DX: ('edx', 0, 16)}


class Emu:
    def __init__(self, pe, md):
        self.pe, self.md = pe, md
        self.r = {k: 0 for k in REG32.values()}
        self.stack = []
        self.writes = []      # (addr, size, value|None)
        self.calls = []       # (destino, ecx, [argumentos de pila, el tope primero])
        self.zf = False
        self.sf = False
        self.ok = True
        self.motivo = ''
        self.mem = {}

    def rd_img(self, addr, size):
        s = self.pe.section(addr)
        if s is None or s['name'] not in ('.data', '.rdata'):
            return ('ld', addr, size)     # valor de memoria del proceso en ese momento (.bss escrito por otro inicializador)
        o = self.pe.offset(addr, size)
        if o is None:
            return ('ld', addr, size)     # parte .bss de la seccion .data (sin bytes en el archivo)
        return int.from_bytes(self.pe.data[o:o + size], 'little')

    def get(self, op):
        if op.type == CS_OP_IMM:
            return op.imm & 0xFFFFFFFF
        if op.type == CS_OP_REG:
            if op.reg in REG32:
                return self.r[REG32[op.reg]]
            if op.reg in SUB:
                n, sh, bits = SUB[op.reg]
                v = self.r[n]
                return v if not isinstance(v, int) else (v >> sh) & ((1 << bits) - 1)
            return UNK
        if op.type == CS_OP_MEM:
            a = self.addr(op)
            if not isinstance(a, int):
                return UNK
            if a in self.mem:
                return self.mem[a]
            return self.rd_img(a, op.size)
        return UNK

    def addr(self, op):
        m = op.mem
        if m.segment != 0:
            return UNK
        a = m.disp
        if m.base != X.X86_REG_INVALID:
            v = self.r.get(REG32.get(m.base, ''), UNK)
            if not isinstance(v, int):
                return UNK
            a += v
        if m.index != X.X86_REG_INVALID:
            v = self.r.get(REG32.get(m.index, ''), UNK)
            if not isinstance(v, int):
                return UNK
            a += v * m.scale
        return a & 0xFFFFFFFF

    def put(self, op, val, size=4):
        if op.type == CS_OP_REG:
            if op.reg in REG32:
                self.r[REG32[op.reg]] = val
            elif op.reg in SUB:
                n, sh, bits = SUB[op.reg]
                old = self.r[n]
                if isinstance(val, tuple):
                    self.r[n] = val          # carga simbolica (mov ax, [mem]); el almacenamiento posterior del mismo ancho la recupera
                elif not isinstance(old, int) or not isinstance(val, int):
                    self.r[n] = UNK
                else:
                    mask = ((1 << bits) - 1) << sh
                    self.r[n] = (old & ~mask) | ((val << sh) & mask)
            else:
                self.ok, self.motivo = False, f'registro {op.reg}'
        elif op.type == CS_OP_MEM:
            a = self.addr(op)
            if not isinstance(a, int):
                self.ok, self.motivo = False, 'escritura a direccion desconocida'
                return
            self.writes.append((a, op.size, val))
            self.mem[a] = val

    def run(self, ini, fin):
        pe, md = self.pe, self.md
        o = pe.offset(ini, 1)
        code = pe.data[o:o + (fin - ini) + 16]
        ins_by = {i.address: i for i in md.disasm(code, ini)}
        pc, pasos = ini, 0
        while pc in ins_by and pasos < 400000 and self.ok:
            pasos += 1
            i = ins_by[pc]
            mn = i.mnemonic
            ops = i.operands
            nxt = pc + i.size
            if mn in ('ret', 'retn'):
                return
            if mn == 'push':
                self.stack.append(self.get(ops[0]))
            elif mn == 'pop':
                v = self.stack.pop() if self.stack else UNK
                self.put(ops[0], v)
            elif mn == 'mov':
                self.put(ops[0], self.get(ops[1]))
            elif mn in ('movzx', 'movsx'):
                v = self.get(ops[1])
                self.put(ops[0], v if mn == 'movzx' else UNK)
            elif mn == 'xor' and ops[0].type == CS_OP_REG and ops[1].type == CS_OP_REG and ops[0].reg == ops[1].reg:
                self.put(ops[0], 0)
                self.zf, self.sf = True, False
            elif mn in ('or', 'and', 'xor', 'add', 'sub'):
                a, b = self.get(ops[0]), self.get(ops[1])
                if not isinstance(a, int) or not isinstance(b, int):
                    v = UNK
                else:
                    v = {'or': a | b, 'and': a & b, 'xor': a ^ b, 'add': a + b, 'sub': a - b}[mn] & 0xFFFFFFFF
                if ops[0].type == CS_OP_REG and ops[0].reg in SUB:
                    v = UNK if v is UNK else v & 0xFF
                    if mn == 'or' and ops[1].type == CS_OP_IMM:
                        v = (self.get(ops[0]) | (b & 0xFF)) & 0xFF
                self.put(ops[0], v)
                self.zf, self.sf = (v == 0) if v is not UNK else False, bool(v & 0x80000000) if v is not UNK else False
            elif mn in ('dec', 'inc'):
                a = self.get(ops[0])
                v = UNK if not isinstance(a, int) else (a + (1 if mn == 'inc' else -1)) & 0xFFFFFFFF
                self.put(ops[0], v)
                self.zf = (v == 0) if v is not UNK else False
            elif mn == 'cmp':
                a, b = self.get(ops[0]), self.get(ops[1])
                if not isinstance(a, int) or not isinstance(b, int):
                    self.ok, self.motivo = False, 'cmp con valor desconocido'
                else:
                    d = (a - b) & 0xFFFFFFFF
                    self.zf = d == 0
                    sa, sb = a - (1 << 32 if a & 0x80000000 else 0), b - (1 << 32 if b & 0x80000000 else 0)
                    self.sf = sa < sb
            elif mn == 'lea':
                a = self.addr(ops[1])
                self.put(ops[0], a)
            elif mn in ('jne', 'jl', 'je', 'jmp'):
                t = ops[0].imm & 0xFFFFFFFF
                tomar = {'jne': not self.zf, 'je': self.zf, 'jl': self.sf, 'jmp': True}[mn]
                if mn == 'jmp' and not (ini <= t < fin + 16):
                    # salto de cola a otra funcion: es una llamada
                    self.calls.append((t, self.r['ecx'], list(reversed(self.stack))))
                    return
                if tomar:
                    nxt = t
            elif mn == 'call':
                if ops[0].type == CS_OP_IMM:
                    self.calls.append((ops[0].imm & 0xFFFFFFFF, self.r['ecx'], list(reversed(self.stack))))
                    self.stack.clear()
                    self.r['eax'] = UNK
                else:
                    self.ok, self.motivo = False, 'call indirecto'
            elif mn == 'rep stosd' or (mn == 'rep' and 'stosd' in i.op_str) or mn.startswith('rep stos'):
                n, a, v = self.r['ecx'], self.r['edi'], self.r['eax']
                if not all(isinstance(q, int) for q in (n, a, v)):
                    self.ok, self.motivo = False, 'rep stos desconocido'
                else:
                    for k in range(n):
                        self.writes.append((a + 4 * k, 4, v))
                    self.r['edi'] = a + 4 * n
                    self.r['ecx'] = 0
            elif mn in ('nop', 'cdq'):
                pass
            elif mn.startswith('f'):
                self.ok, self.motivo = False, 'x87'
            else:
                self.ok, self.motivo = False, f'instruccion {mn}'
            pc = nxt
        if pc not in ins_by:
            self.ok, self.motivo = self.ok and True, self.motivo


def comprimir(writes):
    """[(addr, size, val)] -> [(addr, size, val, count, stride)] agrupando escrituras del mismo ancho y valor con paso constante."""
    out = []
    for a, s, v in writes:
        if out:
            o = out[-1]
            if o['size'] == s and o['val'] == v:
                if o['count'] == 1 and a > o['addr'] and a - o['addr'] <= 0x100:
                    o['stride'] = a - o['addr']
                    o['count'] = 2
                    continue
                if o['count'] > 1 and a == o['addr'] + o['stride'] * o['count']:
                    o['count'] += 1
                    continue
        out.append(dict(addr=a, size=s, val=v, count=1, stride=0))
    return [(o['addr'], o['size'], o['val'], o['count'], o['stride']) for o in out]


def main():
    pe = PE(EXE)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    hk = {}
    with (RAIZ / 'docs/hooks.csv').open(encoding='utf-8-sig', newline='') as s:
        for r in csv.DictReader(s):
            try:
                hk[int(r['address'], 16)] = (r['class'], r['fn_name'])
            except ValueError:
                pass
    filas = list(csv.DictReader((RAIZ / 'docs/CodigoVivoExe/fase8-inicializadores.csv').open(encoding='utf-8')))
    salida, esc = [], []
    cnt = Counter()
    for f in filas:
        if f['clase'].startswith('constante x87') and 'fcos' not in f['patron']:
            continue
        va, nb = int(f['va'], 16), int(f['bytes'])
        e = Emu(pe, md)
        e.run(va, va + nb)
        fila = dict(tabla=f['tabla'], indice=f['indice'], va=f['va'], bytes=nb, clase_fase8=f['clase'], emulacion='ok' if e.ok else e.motivo)
        atx = [c for c in e.calls if c[0] == ATEXIT]
        otras = [c for c in e.calls if c[0] != ATEXIT]
        dtor_funclet = atx[0][2][0] if atx and atx[0][2] else None
        obj = ctor = elem_ctor = elem_dtor = cnt_v = size_v = None
        tipo = 'otro'
        if e.ok:
            if len(otras) == 1 and otras[0][0] == VEC_CTOR and len(otras[0][2]) >= 5:
                a = otras[0][2]            # tope primero: obj, size, count, ctor, dtor
                obj, size_v, cnt_v, elem_ctor, elem_dtor = a[0], a[1], a[2], a[3], a[4]
                tipo = 'ctor-vec'
            elif len(otras) == 1 and otras[0][1] not in (None, 0):
                obj, ctor = otras[0][1], otras[0][0]
                tipo = 'ctor'
            elif not otras and atx and not e.writes:
                tipo = 'dtor-solo'
            elif not otras and not atx and e.writes:
                tipo = 'escrituras'
            elif not otras and atx and e.writes:
                tipo = 'escrituras+dtor'
        # destructor: el funclet es `mov ecx, OBJ; call/jmp DTOR` o una llamada al destructor de vectores
        dobj = dtor = None
        if dtor_funclet:
            d = Emu(pe, md)
            d.run(dtor_funclet, dtor_funclet + 64)
            if d.calls:
                dtor = d.calls[0][0]
                dobj = d.calls[0][1]
                dargs = d.calls[0][2]
                fila['dtor_args'] = ' '.join(f'0x{x:X}' if x is not None else '?' for x in dargs)
        if obj is None and dobj:
            obj = dobj
        fila.update(tipo=tipo, objeto=f'0x{obj:08X}' if obj else '', ctor=f'0x{ctor:08X}' if ctor else '', ctor_cpp='', elem_ctor=f'0x{elem_ctor:08X}' if elem_ctor else '',
                    elem_dtor=f'0x{elem_dtor:08X}' if elem_dtor else '', cantidad=cnt_v or '', tam_elem=size_v or '',
                    dtor_funclet=f'0x{dtor_funclet:08X}' if dtor_funclet else '', dtor=f'0x{dtor:08X}' if dtor else '', dtor_cpp='',
                    escrituras=len(e.writes), llamadas_otras=' '.join(f'0x{c[0]:X}' for c in otras))
        for campo, a in (('ctor_cpp', ctor), ('ctor_cpp', elem_ctor), ('dtor_cpp', dtor), ('dtor_cpp', elem_dtor)):
            if a and a in hk and not fila[campo]:
                fila[campo] = f'{hk[a][0]}::{hk[a][1]}'
        salida.append(fila)
        cnt[(f['clase'], tipo)] += 1
        for a, s, v, n, st in comprimir(e.writes):
            esc.append(dict(tabla=f['tabla'], indice=f['indice'], va=f['va'], direccion=f'0x{a:08X}', elementos=n, paso=st, ancho=s,
                            valor=('?' if v is None else f'0x{v:X}' if isinstance(v, int) else f'=[0x{v[1]:X}]/{v[2]}')))
    out = RAIZ / 'docs/DatosExe'
    campos = ['tabla', 'indice', 'va', 'bytes', 'clase_fase8', 'emulacion', 'tipo', 'objeto', 'ctor', 'ctor_cpp', 'elem_ctor', 'elem_dtor', 'cantidad',
              'tam_elem', 'dtor_funclet', 'dtor', 'dtor_cpp', 'dtor_args', 'escrituras', 'llamadas_otras']
    with (out / 'inicializadores.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos, extrasaction='ignore')
        w.writeheader()
        w.writerows(salida)
    with (out / 'inicializadores-escrituras.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=['tabla', 'indice', 'va', 'direccion', 'elementos', 'paso', 'ancho', 'valor'])
        w.writeheader()
        w.writerows(esc)
    for k, v in sorted(cnt.items()):
        print(v, k)
    print('con ctor C++:', sum(1 for r in salida if r['ctor_cpp'] or r['elem_ctor'] and r.get('ctor_cpp')))


if __name__ == '__main__':
    main()
