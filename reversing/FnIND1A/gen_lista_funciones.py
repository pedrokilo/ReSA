r"""IND1A: lista de entradas de funcion del .text original (0x401000-0x857000) para el contador por INT3.
Fuentes: destinos de `call rel32`/`jmp rel32` dentro del .text (filtrados: precedidos por ret/int3/nop/relleno o alineados a 16),
punteros a codigo guardados en .rdata/.data (vtables, tablas), mapas del proyecto (JuegoExe/funciones.csv, RenderWareExe/funciones.csv),
IND0A (cierre_codigo.csv, dependencias_codigo.csv). Salida: datos/funciones_originales.bin (uint32 LE, ordenado) y .csv.
Uso: python -I gen_lista_funciones.py <gta_sa.exe>
"""
import csv, struct, sys
from pathlib import Path
raiz = Path(__file__).resolve().parents[2]
d = Path(sys.argv[1]).read_bytes()
pe = struct.unpack_from('<I', d, 60)[0]
ns, = struct.unpack_from('<H', d, pe + 6)
opt, = struct.unpack_from('<H', d, pe + 20)
secs = []
for i in range(ns):
    p = pe + 24 + opt + 40 * i
    vs, rva, rs, rp = struct.unpack_from('<IIII', d, p + 8)
    secs.append((0x400000 + rva, vs, rp, rs))
TI, TF = 0x401000, 0x857000
mem = bytearray(0xCB1000 - 0x400000)
for sv, vs, rp, rs in secs:
    mem[sv - 0x400000: sv - 0x400000 + min(rs, vs)] = d[rp: rp + min(rs, vs)]
def b(a): return mem[a - 0x400000]
def parece_inicio(a):
    if not (TI <= a < TF): return False
    if a % 16 == 0: return True
    p = b(a - 1)
    if p in (0xC3, 0xCC, 0x90): return True
    if a - 3 >= TI and b(a - 3) == 0xC2 and b(a - 1) == 0: return True
    return False
cand = {}
def marca(a, fuente):
    if a not in cand: cand[a] = fuente
# 1. call/jmp rel32 del .text
t = mem[TI - 0x400000: TF - 0x400000]
for i in range(len(t) - 5):
    if t[i] in (0xE8, 0xE9):
        dst = (TI + i + 5 + struct.unpack_from('<i', t, i + 1)[0]) & 0xFFFFFFFF
        if parece_inicio(dst): marca(dst, 'call/jmp')
# 2. punteros en datos
for a in range(0x857000, 0xCB1000 - 3, 4):
    v = struct.unpack_from('<I', mem, a - 0x400000)[0]
    if parece_inicio(v) and b(v) not in (0x00,): marca(v, 'puntero')
# 3. mapas
def add(path, col, fuente):
    p = raiz / path
    if not p.exists(): return
    for r in csv.DictReader(open(p, encoding='utf-8')):
        v = (r.get(col) or '').strip()
        if v.startswith('0x'):
            a = int(v, 16)
            if TI <= a < TF: marca(a, fuente)
add('docs/JuegoExe/funciones.csv', 'va', 'mapa-juego'); add('docs/RenderWareExe/funciones.csv', 'va', 'mapa-rw')
add('reversing/FnIND0A/datos/cierre_codigo.csv', 'funcion', 'IND0A-cierre'); add('reversing/FnIND0A/datos/dependencias_codigo.csv', 'direccion', 'IND0A-dep')
# Filtro 1: relleno estricto. Alinear a 16 NO basta (0x855B90 caia dentro de un `fstp [mem]`): el byte anterior debe ser ret/int3/nop
# (o `ret imm16`) para las fuentes heuristicas (call/jmp, puntero); los mapas del proyecto y de IND0A se aceptan tal cual.
def relleno(a):
    p = b(a - 1)
    if p in (0xC3, 0xCC, 0x90): return True
    return a - 3 >= TI and b(a - 3) == 0xC2 and b(a - 1) == 0
for a in [a for a, f in cand.items() if f in ('call/jmp', 'puntero') and not relleno(a)]:
    del cand[a]
# Filtro 2: descenso recursivo desde las semillas (mapas + candidatos con relleno); un candidato que cae DENTRO de una instruccion
# ya decodificada de otra funcion se descarta (un INT3 ahi corromperia el codigo).
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL, CS_GRP_RET
md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = True
inicio = bytearray(TF - TI)    # 1 = comienzo de instruccion decodificada
dentro = bytearray(TF - TI)    # 1 = byte interior (no comienzo) de una instruccion decodificada
cola = [a for a, f in cand.items() if f not in ('call/jmp', 'puntero')] + [a for a, f in cand.items() if f in ('call/jmp', 'puntero')]
visto = set()
while cola:
    a = cola.pop()
    while TI <= a < TF and a not in visto:
        visto.add(a)
        if inicio[a - TI]: break
        ins = next(md.disasm(bytes(mem[a - 0x400000: a - 0x400000 + 16]), a), None)
        if ins is None: break
        inicio[a - TI] = 1
        for k in range(1, ins.size):
            if a - TI + k < TF - TI: dentro[a - TI + k] = 1
        g = ins.groups
        if ins.mnemonic in ('jmp', 'call') or CS_GRP_JUMP in g or CS_GRP_CALL in g:
            if ins.operands and ins.operands[0].type == 2:  # inmediato
                t = ins.operands[0].imm & 0xFFFFFFFF
                if TI <= t < TF and t not in visto: cola.append(t)
        if ins.mnemonic in ('ret', 'retn', 'jmp', 'int3', 'hlt') or CS_GRP_RET in g: break
        a += ins.size
descartadas = [a for a in cand if dentro[a - TI]]
for a in descartadas: del cand[a]
print('descartadas por caer dentro de una instruccion:', len(descartadas))
dirs = sorted(cand)
out = raiz / 'reversing/FnIND1A/datos'
out.mkdir(exist_ok=True)
(out / 'funciones_originales.bin').write_bytes(b''.join(struct.pack('<I', a) for a in dirs))
with open(out / 'funciones_originales.csv', 'w', encoding='utf-8', newline='') as f:
    w = csv.writer(f); w.writerow(['va', 'fuente'])
    for a in dirs: w.writerow([f'0x{a:08X}', cand[a]])
import collections
print(len(dirs), collections.Counter(cand.values()))
