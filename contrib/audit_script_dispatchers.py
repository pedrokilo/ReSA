#!/usr/bin/env python3
"""Auditoria de despachadores de comandos de script (ProcessCommands<N>To<N+99>).

Para cada despachador compara (a) los comandos que el EXE atiende de verdad (tabla de saltos del switch, destino distinto del 'default'),
(b) los `case COMMAND_X:` del despachador C++ reconstruido y (c) los manejadores registrados con REGISTER_COMMAND_HANDLER/NOP/... en
los `RegisterHandlers()` que RunningScript.cpp llama de verdad. Un comando que el EXE atiende y que no tiene ni caso ni manejador cae en
`default -> OR_INTERRUPT` sin leer sus parametros: desalinea la IP del script (popup 'Unknown param type').

Uso: python contrib/audit_script_dispatchers.py [--exe <gta_sa.exe>] [--csv salida.csv]
"""
import csv, os, re, struct, sys
import capstone

RAIZ = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
EXE = 'E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe'
if '--exe' in sys.argv: EXE = sys.argv[sys.argv.index('--exe') + 1]
SCR = os.path.join(RAIZ, 'source/game_sa/Scripts')

d = open(EXE, 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]; ns = struct.unpack_from('<H', d, pe + 6)[0]; so = struct.unpack_from('<H', d, pe + 20)[0]
secs = []
for i in range(ns):
    n, vs, va, rs, rp = struct.unpack_from('<8sIIII', d, pe + 24 + so + i * 40 + 0); secs.append((va, vs, rs, rp))
def off(a):
    r = a - 0x400000
    for va, vs, rs, rp in secs:
        if va <= r < va + max(vs, rs): return rp + r - va
def u32(a): return struct.unpack_from('<I', d, off(a))[0]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True

# direcciones de los despachadores (tabla s_OriginalCommandHandlerTable 0x8A6168 + 4*k) leidas del propio EXE
TABLA = 0x8A6168
def tabla_exe():
    # direcciones de los despachadores: las que RunningScript.cpp engancha con RH_ScopedInstall(ProcessCommandsNToM, 0x...)
    t = open(os.path.join(RAIZ, 'source/game_sa/Scripts/RunningScript.cpp'), encoding='utf-8').read()
    r = {int(m.group(1)) // 100: int(m.group(2), 16) for m in re.finditer(r'RH_ScopedInstall\(ProcessCommands(\d+)To\d+,\s*(0x[0-9A-Fa-f]+)', t)}
    return [r.get(k) for k in range(27)]

def saltos(addr):
    """Lee el switch del prologo del despachador: (n_casos-1, primer_comando, tabla_de_indices|None, tabla_de_saltos, destino_default)."""
    code = d[off(addr):off(addr) + 0x300]
    ins = list(md.disasm(code, addr))
    ja = next((k for k, i in enumerate(ins[:70]) if i.mnemonic == 'ja'), None)
    if ja is None: return (None,) * 5
    dflt = int(ins[ja].op_str, 0)
    kc = max(k for k in range(ja) if ins[k].mnemonic == 'cmp')
    reg, imm = [x.strip() for x in ins[kc].op_str.split(',')]; ncmp = int(imm, 0)
    base = 0
    for i in ins[:kc]:
        m = re.match(r'%s, \[e[a-z]{2} - (0x[0-9a-f]+)\]$' % reg, i.op_str) if i.mnemonic == 'lea' else None
        if m: base = int(m.group(1), 16)
        if i.mnemonic == 'add' and i.op_str.startswith(reg + ', '):
            v = int(i.op_str.split(',')[1], 0)
            if v < 0: base = -v
            elif v >= 0xffff0000: base = (1 << 32) - v
    idx = jt = None
    for i in ins[ja + 1:ja + 6]:
        if i.mnemonic == 'movzx' and 'byte ptr' in i.op_str:
            m = re.search(r'\+ (0x[0-9a-f]+)\]', i.op_str); idx = int(m.group(1), 16)
        if i.mnemonic == 'jmp' and '*4' in i.op_str:
            m = re.search(r'\+ (0x[0-9a-f]+)\]', i.op_str); jt = int(m.group(1), 16); break
    return ncmp, base, idx, jt, dflt

def todo(addr, k):
    ncmp, base, idx, jt, dflt = saltos(addr)
    if ncmp is None or jt is None or dflt is None: return None
    base = base or 0
    res = {}
    for v in range(ncmp + 1):
        ti = d[off(idx) + v] if idx else v
        dest = u32(jt + 4 * ti)
        res[v + base] = dest
    return res, dflt, base

# --- C++ ---
cmds = {}
with open(os.path.join(RAIZ, 'docs/ComandosScript.csv'), encoding='utf-8-sig') as f:
    for r in csv.DictReader(f): cmds[r['comando']] = r
by_id = {int(r['id']): r for r in cmds.values()}

def cuerpo(src, k):
    m = re.search(r'OpcodeResult CRunningScript::ProcessCommands%dTo%d\(' % (k * 100, k * 100 + 99), src)
    if not m: return None
    i = src.find(') {' + chr(10), m.end()); j = src.find(chr(10) + '}' + chr(10), i)
    return src[i:j]

ficheros = {}
for dp, _, fs in os.walk(SCR):
    for fn in fs:
        if fn.endswith('.cpp'):
            p = os.path.join(dp, fn); ficheros[p] = open(p, encoding='utf-8', errors='replace').read().replace(chr(13)+chr(10), chr(10))
cases = {}
for k in range(27):
    for p, s in ficheros.items():
        c = cuerpo(s, k)
        if c is not None:
            cases[k] = set(re.findall(r'case\s+(COMMAND_\w+)\s*:', c)); cases[k] |= set(int(x, 0) for x in re.findall(r'case\s+(0x[0-9A-Fa-f]+|\d+)\s*:', c)); break

# registro real: qué namespaces llama RunningScript.cpp, qué comandos registra cada fichero de Commands/
rs = ficheros[os.path.join(SCR, 'RunningScript.cpp')]
llamados = set(re.findall(r'^\s*c::(\w+)::RegisterHandlers\(\);', rs, re.M))
registrados = {}  # comando -> (namespace, macro, fichero)
for p, s in ficheros.items():
    if os.sep + 'Commands' + os.sep not in p or 'CLEO' in p: continue
    m = re.search(r'([\w:]+)::RegisterHandlers\(\)\s*\{', s)
    if m: nsn = m.group(1).split('::')[-1]
    else:
        m = re.search(r'void\s+RegisterHandlers\(\)', s)
        if not m: continue
        nsn = [x.split('::')[-1] for x in re.findall(r'namespace\s+([\w:]+)\s*\{', s[:m.start()])][-1]
    for m in re.finditer(r'(REGISTER_COMMAND_\w+)\(\s*(COMMAND_\w+)', s):
        registrados[m.group(2)] = (nsn, m.group(1), os.path.relpath(p, RAIZ))

tabla = tabla_exe()
filas = []; resumen = []
for k in range(27):
    if tabla[k] is None: resumen.append((k, '-', 'sin RH_ScopedInstall')); continue
    r = todo(tabla[k], k)
    if r is None: resumen.append((k, hex(tabla[k]), 'no se pudo leer el switch')); continue
    res, dflt, base = r
    atendidos = [c for c, dest in res.items() if dest != dflt and k * 100 <= c <= k * 100 + 99]
    huerfanos = []
    for c in atendidos:
        row = by_id.get(c)
        if not row: continue
        nombre = row['comando']
        en_caso = nombre in cases.get(k, set()) or c in cases.get(k, set())
        reg = registrados.get(nombre)
        con_manejador = bool(reg) and reg[1] != 'REGISTER_COMMAND_UNIMPLEMENTED' and reg[0] in llamados
        sin_registrar = bool(reg) and reg[1] != 'REGISTER_COMMAND_UNIMPLEMENTED' and reg[0] not in llamados
        if not en_caso and not con_manejador:
            huerfanos.append(c)
            filas.append([k, hex(tabla[k]), c, nombre, row['estado'], 'ns %s no llamado' % reg[0] if sin_registrar else ('sin manejador' if not reg else reg[1]), hex(res[c])])
    resumen.append((k, hex(tabla[k]), 'base=%d n=%d EXE atiende %d, casos C++ %d, huerfanos %d' % (base, len(res), len(atendidos), len(cases.get(k, ())), len(huerfanos))))
for r in resumen: print(*r)
print('\nHUERFANOS (el EXE los atiende; ni caso C++ ni manejador registrado y llamado):')
for f in filas: print(*f)
print('\ntotal', len(filas), '; namespaces llamados:', sorted(llamados))
nsall = set(v[0] for v in registrados.values() if v[0]); print('namespaces con manejadores sin llamar:', sorted(nsall - llamados))
if '--csv' in sys.argv:
    with open(sys.argv[sys.argv.index('--csv') + 1], 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f); w.writerow(['despachador', 'direccion_exe', 'id', 'comando', 'estado_csv', 'motivo', 'destino_exe']); w.writerows(filas)
