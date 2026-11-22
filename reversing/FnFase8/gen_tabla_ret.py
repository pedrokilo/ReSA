r"""FASE 8: tabla de `ret n` por entrada de funcion original (para el modo --censo, que simula el retorno de una llamada a codigo original ausente).
Salida: salida/retn.bin = uint16 por byte de [0x401000, 0x858000): 0xFFFF = no es entrada conocida; otro = bytes que saca el `ret` (0 si `ret`).
Entradas: reversing/FnIND1A/datos/funciones_originales.bin (20.093) + docs/CodigoVivoExe/funciones-vivas.csv (columna va). Descenso recursivo sin seguir `call`.
Uso: python -I gen_tabla_ret.py <gta_sa.exe> [salida]"""
import csv, struct, sys
from collections import Counter
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_IMM
raiz = Path(__file__).resolve().parents[2]
d = Path(sys.argv[1]).read_bytes()
out = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(__file__).parent / 'salida' / 'retn.bin'
pe = struct.unpack_from('<I', d, 60)[0]
ns, = struct.unpack_from('<H', d, pe + 6); opt, = struct.unpack_from('<H', d, pe + 20)
TI, TF = 0x401000, 0x858000
mem = bytearray(TF - TI)
for i in range(ns):
    p = pe + 24 + opt + 40 * i
    vs, rva, rs, rp = struct.unpack_from('<IIII', d, p + 8)
    va = 0x400000 + rva
    if TI <= va < TF:
        n = min(vs, rs); mem[va - TI: va - TI + n] = d[rp: rp + n]
ent = set(struct.unpack('<%dI' % (len(Path(raiz / 'reversing/FnIND1A/datos/funciones_originales.bin').read_bytes()) // 4), Path(raiz / 'reversing/FnIND1A/datos/funciones_originales.bin').read_bytes()))
f = raiz / 'docs/CodigoVivoExe/funciones-vivas.csv'
for r in csv.DictReader(open(f, encoding='utf-8')):
    try: ent.add(int(r['va'], 16))
    except Exception: pass
ent = {a for a in ent if TI <= a < TF}
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True
tabla = [0xFFFF] * (TF - TI)
desc = 0
for e in sorted(ent):
    vistos = set(); cola = [e]; rets = Counter(); n = 0
    while cola and n < 4000:
        a = cola.pop()
        while TI <= a < TF and a not in vistos and n < 4000:
            ins = next(md.disasm(bytes(mem[a - TI: a - TI + 16]), a, 1), None)
            if ins is None: break
            vistos.add(a); n += 1
            m = ins.mnemonic
            if m in ('ret', 'retn'):
                rets[ins.operands[0].imm if ins.operands else 0] += 1; break
            if m == 'int3': break
            if m == 'jmp':
                if ins.operands and ins.operands[0].type == X86_OP_IMM:
                    t = ins.operands[0].imm
                    if e <= t < e + 0x6000: cola.append(t)
                break
            if m.startswith('j') and ins.operands and ins.operands[0].type == X86_OP_IMM:
                t = ins.operands[0].imm
                if e <= t < e + 0x6000: cola.append(t)
            a += ins.size
    if rets: tabla[e - TI] = rets.most_common(1)[0][0]
    else: desc += 1
out.parent.mkdir(exist_ok=True)
out.write_bytes(struct.pack('<%dH' % len(tabla), *tabla))
print(len(ent), 'entradas;', desc, 'sin ret alcanzable;', out)
