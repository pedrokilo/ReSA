# Triaje estatico: para cada funcion de RenderWare con hook activo y validated=0 compara las direcciones absolutas y los destinos de llamada
# que aparecen en el EXE con los que aparecen en el cuerpo C++ (literales hexadecimales, globales con nombre y funciones con direccion conocida).
import csv, re, os, sys, glob
import desens_exe as X
import capstone
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
rows = list(csv.DictReader(open(os.path.join(ROOT, 'docs', 'hooks.csv'), encoding='utf-8')))
name2addr = {}
for r in rows:
    name2addr.setdefault(r['fn_name'], int(r['address'], 16))
for r in csv.DictReader(open(os.path.join(ROOT, 'docs', 'RenderWareExe', 'funciones.csv'), encoding='utf-8')):
    if r['nombre']: name2addr.setdefault(r['nombre'], int(r['va'], 16))
src = ''
files = glob.glob(os.path.join(ROOT, 'source', 'game_sa', 'RenderWare', '**', '*.cpp'), recursive=True) + glob.glob(os.path.join(ROOT, 'source', 'game_sa', '*.cpp'))
texts = {}
for f in files:
    texts[f] = open(f, encoding='utf-8', errors='replace').read()
alltext = '\n'.join(texts.values())
glob_ref = {}
for m in re.finditer(r'(\w+)\s*=\s*(?:StaticRef|\*?reinterpret_cast)[^;(]*\(\s*(?:0x)?(0x[0-9A-Fa-f]{6,8}|[0-9A-Fa-f]{6,8})\s*\)', alltext):
    glob_ref[m.group(1)] = int(m.group(2), 16)
for m in re.finditer(r'(?:constexpr\s+\w+\s+|const\s+\w+\s+)(k?\w+)\s*=\s*(0x[0-9A-Fa-f]{6,8})\s*;', alltext):
    glob_ref.setdefault(m.group(1), int(m.group(2), 16))
IMPL_ENGINE = {0xC97B24, 0xC980D8, 0xC97B24}
def body_of(name):
    best = None
    for f, t in texts.items():
        for m in re.finditer(r'^[A-Za-z_][^\n;{}]*[\s\*&]' + re.escape(name) + r'\s*\([^;{]*\)\s*(?:const\s*)?\{', t, re.M):
            i = m.end(); d = 1
            while i < len(t) and d:
                c = t[i]
                d += (c == '{') - (c == '}'); i += 1
            b = t[m.start():i]
            if best is None or len(b) > len(best): best = b
    return best
def cpp_set(b):
    s = set()
    for m in re.finditer(r'0x([0-9A-Fa-f]{5,8})\b', b): s.add(int(m.group(1), 16))
    for m in re.finditer(r'\b(\w+)\b', b):
        w = m.group(1)
        if w in glob_ref: s.add(glob_ref[w])
        elif w in name2addr: s.add(name2addr[w])
    return s
def exe_set(a, size):
    code = X.read(a, size)
    adr, off = set(), set()
    X.md.detail = True
    for i in X.md.disasm(code, a):
        for op in i.operands:
            if op.type == capstone.x86.X86_OP_IMM:
                v = op.imm & 0xFFFFFFFF
                if 0x400000 <= v < 0xE00000: adr.add(v)
            elif op.type == capstone.x86.X86_OP_MEM:
                d = op.mem.disp & 0xFFFFFFFF
                if op.mem.base == 0 and op.mem.index == 0 and 0x400000 <= d < 0xE00000: adr.add(d)
                elif 0x48 <= d <= 0xBC and i.mnemonic in ('call', 'mov'): off.add(d)
                elif op.mem.base != 0 and 0x400000 <= d < 0xE00000: adr.add(d)
    return adr, off
