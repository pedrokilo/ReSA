#!/usr/bin/env python3
"""Inventario de los comandos de script (opcodes) del juego: lo que hooks.csv no registra.

Cada comando de SCM (0x0000..COMMAND_HIGHEST_VANILLA_ID) se atiende en el EXE dentro de un despachador por centenas
(ProcessCommandsNNNtoMMM, tabla 0x8A6168), sin una funcion propia por comando; por eso no aparece en hooks.csv. Este script
cruza eScriptCommands.h con los REGISTER_COMMAND_* de source/game_sa/Scripts/Commands y escribe docs/ComandosScript.csv.

Estados:
  reescrito       manejador C++ propio (REGISTER_COMMAND_HANDLER / REGISTER_UNSUPPORTED...)
  nop             REGISTER_COMMAND_NOP: el comando no hace nada, como en el original
  no_implementado REGISTER_COMMAND_UNIMPLEMENTED: el propio juego tampoco lo implementa
  original        sin manejador propio: se ejecuta el codigo del EXE
Uso: python contrib/inventario_comandos_script.py [--exe RUTA]   (la ruta del EXE solo anade la direccion del despachador)
"""
import argparse, csv, re, struct, sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
ENUM = RAIZ / 'source/game_sa/Enums/eScriptCommands.h'
CMDS = RAIZ / 'source/game_sa/Scripts/Commands'
SALIDA = RAIZ / 'docs/ComandosScript.csv'

def leer_enum():
    ids = {}
    for linea in ENUM.read_text(encoding='utf-8', errors='replace').splitlines():
        m = re.match(r'\s*(COMMAND_[A-Z0-9_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*,?', linea)
        if m and m.group(1) not in ids: ids[m.group(1)] = int(m.group(2), 0)
        if 'COMMAND_HIGHEST_VANILLA_ID' in linea: break
    return ids

def leer_registros():
    reg = {}
    patron = re.compile(r'^\s*(REGISTER_COMMAND_HANDLER|REGISTER_UNSUPPORTED_COMMAND_HANDLER|REGISTER_COMMAND_UNIMPLEMENTED|REGISTER_COMMAND_NOP)\s*\(\s*(COMMAND_[A-Z0-9_]+)\s*(?:,\s*(.*?))?\)\s*;?\s*(//.*)?$')
    for f in sorted(CMDS.rglob('*.cpp')) + sorted(CMDS.rglob('*.hpp')):
        if 'CLEO' in f.parts: continue   # comandos de CLEO: fuera del juego original
        for n, linea in enumerate(f.read_text(encoding='utf-8', errors='replace').splitlines(), 1):
            if linea.lstrip().startswith('//'): continue
            m = patron.match(linea)
            if not m: continue
            tipo, cmd, fn = m.group(1), m.group(2), (m.group(3) or '').strip()
            if tipo == 'REGISTER_COMMAND_UNIMPLEMENTED' or 'NotImplemented' in fn: estado = 'no_implementado'
            elif tipo == 'REGISTER_COMMAND_NOP': estado = 'nop'
            else: estado = 'reescrito'
            reg.setdefault(cmd, []).append((estado, f.relative_to(RAIZ).as_posix(), n, fn))
    return reg

def despachadores(exe):
    if not exe: return {}
    d = Path(exe).read_bytes()
    pe = struct.unpack('<I', d[0x3c:0x40])[0]; ns = struct.unpack('<H', d[pe+6:pe+8])[0]; opt = struct.unpack('<H', d[pe+20:pe+22])[0]
    base = struct.unpack('<I', d[pe+52:pe+56])[0]; so = pe + 24 + opt
    def off(va):
        for i in range(ns):
            s = d[so+40*i:so+40*i+40]; vs, vaddr, rs, ro = struct.unpack('<IIII', s[8:24])
            if vaddr <= va-base < vaddr+max(vs, rs): return ro+va-base-vaddr
    out = {}
    for slot in range(0, 28):
        o = off(0x8A6168 + 4*slot)
        if o:
            v = struct.unpack('<I', d[o:o+4])[0]
            if 0x401000 <= v < 0x860000: out[slot] = v
    return out

def catalogo_despachadores():
    out = {}
    f = RAIZ / 'docs/hooks.csv'
    if f.exists():
        for r in csv.DictReader(open(f, encoding='utf-8')):
            if r['class'] == 'CRunningScript' and r['fn_name'].startswith('ProcessCommands') and r['fn_name'] not in ('ProcessCommands',):
                out[int(r['address'], 16)] = (r['reversed'], r['validated'])
    return out

def main():
    ap = argparse.ArgumentParser(); ap.add_argument('--exe', default='E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe')
    a = ap.parse_args()
    cat = catalogo_despachadores(); ids = leer_enum(); reg = leer_registros(); disp = despachadores(a.exe if Path(a.exe).exists() else None)
    filas = []; cuenta = {}; cuenta_disp = {}
    for nombre, cid in sorted(ids.items(), key=lambda kv: kv[1]):
        r = reg.get(nombre)
        if r: estado, arch, lin, fn = r[0]; dup = len(r) > 1
        else: estado, arch, lin, fn, dup = 'original', '', '', '', False
        cuenta[estado] = cuenta.get(estado, 0) + 1
        da = disp.get(cid // 100); dc = cat.get(da) if da else None
        dr = ('si' if dc and dc[0] == '1' else 'no') if da else ''
        dv = ('si' if dc and dc[1] == '1' else 'no') if da else ''
        if estado == 'original': cuenta_disp[dr] = cuenta_disp.get(dr, 0) + 1
        filas.append([f'0x{cid:04X}', cid, nombre, estado, fn, f'{arch}:{lin}' if arch else '', f'0x{disp[cid//100]:08X}' if cid//100 in disp else '', 'si' if dup else '', dr, dv])
    with open(SALIDA, 'w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh); w.writerow(['id_hex', 'id', 'comando', 'estado', 'manejador_cpp', 'fuente', 'despachador_exe', 'registro_duplicado', 'despachador_reescrito', 'despachador_validado'])
        w.writerows(filas)
    print(f'{len(filas)} comandos -> {SALIDA.relative_to(RAIZ)}')
    for k, v in sorted(cuenta.items()): print(f'  {k:16} {v}')
    print('  de los "original": despachador reescrito en hooks.csv:', cuenta_disp)
    ext = sorted(set(reg) - set(ids))
    if ext: print(f'  registros fuera del enum vanilla: {len(ext)} (comandos propios o de CLEO)')
if __name__ == '__main__': sys.exit(main())
