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
sys.path.insert(0, str(Path(__file__).resolve().parent))
import riesgo_comandos_script as riesgo_mod

RAIZ = Path(__file__).resolve().parent.parent
ENUM = RAIZ / 'source/game_sa/Enums/eScriptCommands.h'
CMDS = RAIZ / 'source/game_sa/Scripts/Commands'
SALIDA = RAIZ / 'docs/ComandosScript.csv'
SALIDA_MD = RAIZ / 'docs/ComandosScript.md'

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


BANCO_CSV = RAIZ / 'reversing/CmdBench/RESULTADOS.csv'
BANCO_HALLAZGOS = RAIZ / 'reversing/CmdBench/hallazgos.json'


def cargar_banco():
    """Resultados del banco por comando (reversing/CmdBench): {id: fila} y hallazgos {id: {...}}; vacios si no existen."""
    res, hall = {}, {}
    if BANCO_CSV.exists():
        for r in csv.DictReader(open(BANCO_CSV, encoding='utf-8')):
            res[int(r['id'])] = r
    if BANCO_HALLAZGOS.exists():
        import json
        dd = json.loads(BANCO_HALLAZGOS.read_text(encoding='utf-8'))
        hall = {int(k): v for k, v in dd.get('corregidos', dd).items()}
        global ABIERTOS
        ABIERTOS = dd.get('abiertos', {})
    return res, hall


BANCO, HALLAZGOS = {}, {}
ABIERTOS = {}


def escribir_md(filas, cuenta, cuenta_disp):
    import collections
    por_arch = {}
    for f in filas:
        arch = f[5].split(':')[0].split('/')[-1].replace('Commands.cpp', '') if f[5] else '(sin manejador propio)'
        d = por_arch.setdefault(arch, {'reescrito': 0, 'nop': 0, 'no_implementado': 0, 'original': 0}); d[f[3]] += 1
    por_disp = {}
    for f in filas:
        if f[3] != 'original' or not f[6]: continue
        d = por_disp.setdefault(f[6], [0, f[8], f[9]]); d[0] += 1
    L = ['# Comandos de script (opcodes)', '',
         'Inventario de los comandos de SCM del juego (`docs/ComandosScript.csv`, generado por `contrib/inventario_comandos_script.py`).',
         'No forman parte de `hooks.csv`: el EXE los atiende dentro de despachadores por centenas (`ProcessCommandsNNNtoMMM`) y no tienen',
         'una funcion con direccion propia. Los contadores de `ReversedClasses.md` **no los incluyen**.', '',
         '## Resumen', '', '| Estado | Comandos | Significado |', '| --- | ---: | --- |',
         f"| reescrito | {cuenta.get('reescrito', 0)} | manejador C++ propio; **sin banco de validacion contra el EXE** |",
         f"| nop | {cuenta.get('nop', 0)} | el comando no hace nada, como en el original |",
         f"| no implementado | {cuenta.get('no_implementado', 0)} | el propio juego no lo implementa |",
         f"| original | {cuenta.get('original', 0)} | sin manejador propio: pasa por el despachador de su centena |",
         f"| **Total** | **{len(filas)}** | |", '',
         f"De los {cuenta.get('original', 0)} comandos *original*: despachador reescrito en `hooks.csv` = {cuenta_disp.get('si', 0)}; despachador del EXE = {cuenta_disp.get('no', 0)}; sin despachador identificado = {cuenta_disp.get('', 0)}.", '',
         '## Por categoria (archivo de `Scripts/Commands`)', '', '| Categoria | Reescritos | Nop | No impl. | Original |', '| --- | ---: | ---: | ---: | ---: |']
    for k, d in sorted(por_arch.items()): L.append(f"| {k} | {d['reescrito']} | {d['nop']} | {d['no_implementado']} | {d['original']} |")
    L += ['', '## Despachadores del EXE que atienden comandos sin manejador propio', '', '| Despachador | Comandos | Reescrito en hooks.csv | Validado |', '| --- | ---: | --- | --- |']
    for k, d in sorted(por_disp.items()): L.append(f"| `{k}` | {d[0]} | {d[1]} | {d[2]} |")
    L += ['', '## Comandos reescritos (sin validar por banco)', '', '| Id | Comando | Manejador | Riesgo | Fuente |', '| --- | --- | --- | --- | --- |']
    for f in filas:
        if f[3] == 'reescrito': L.append(f"| {f[0]} | `{f[2]}` | `{f[4]}` | {f[10] if len(f) > 10 else ''} | {f[5]} |")
    rs = [f for f in filas if f[3] == 'reescrito']
    cnt = collections.Counter(f[10] for f in rs)
    L += ['', '## Riesgo (Fase 0 del banco de comandos)', '',
          'Clasificacion de los reescritos por analisis de texto del manejador C++ (`contrib/riesgo_comandos_script.py`, reproducible; columnas `riesgo`, `devuelve`, `mundo`, `motivo_riesgo` del CSV).',
          '- **alto**: devuelve valor al script (variable, varias variables o control de flujo) y lee/escribe estado del mundo. Un error de tipo o de semantica cambia el flujo del script sin ruido (caso `GET_CHAR_AREA_VISIBLE`).',
          '- **medio**: escribe estado del mundo sin devolver valor, condiciones (bandera) que leen el mundo, o valores calculados.',
          '- **bajo**: puro o trivial (aritmetica, comparaciones, asignaciones de variables de script).', '',
          '| Riesgo | Comandos |', '| --- | ---: |'] + [f'| {k} | {cnt.get(k, 0)} |' for k in ('alto', 'medio', 'bajo')] + ['',
          '| Retorno / mundo | Comandos |', '| --- | ---: |']
    for (dv, mu), n in sorted(collections.Counter((f[11], f[12]) for f in rs).items(), key=lambda kv: -kv[1]): L.append(f'| devuelve={dv}, mundo={mu} | {n} |')
    L += ['', '| Categoria | alto | medio | bajo |', '| --- | ---: | ---: | ---: |']
    por = {}
    for f in rs:
        arch = f[5].split(':')[0].split('/')[-1].replace('Commands.cpp', ''); por.setdefault(arch, collections.Counter())[f[10]] += 1
    for k, c in sorted(por.items()): L.append(f"| {k} | {c.get('alto', 0)} | {c.get('medio', 0)} | {c.get('bajo', 0)} |")
    cb = collections.Counter((BANCO.get(f[1]) or {}).get('estado', 'no_probado') for f in rs)
    L += ['', '## Banco por comando (CmdBench)', '',
          'Banco de validacion de los 668 manejadores reescritos contra el manejador original del EXE (`reversing/CmdBench`, ver su README): mismo script sintetico y parametros deterministas, estado de partida restaurado entre el lado C++ y el lado EXE; se compara retorno, IP, bandera, variables de salida y todos los bytes cambiados en `.data` y en las fixtures. Resultados: `reversing/CmdBench/RESULTADOS.csv`.', '',
          '| Estado del banco | Comandos |', '| --- | ---: |'] + [f'| {k} | {cb.get(k, 0)} |' for k in ('validado', 'sin_efecto', 'diferencia', 'no_original', 'no_registrado', 'no_probado')] + ['']
    if HALLAZGOS:
        L += ['### Diferencias halladas y corregidas', '', '| Comando | Causa | Correccion |', '| --- | --- | --- |']
        for cid, h in sorted(HALLAZGOS.items()):
            L.append(f"| `{h.get('comando', cid)}` ({cid}) | {h['causa']} | {h['correccion']} |")
        L.append('')
    if ABIERTOS:
        L += ['### Diferencias abiertas (no corregidas) por causa', '', '| Causa | Comandos | Descripcion |', '| --- | --- | --- |']
        for k, v in ABIERTOS.items():
            L.append(f"| {k} | {', '.join(str(i) for i in v['ids'])} | {v['descripcion']} |")
        L.append('')
    nop = [f for f in rs if (BANCO.get(f[1]) or {}).get('estado', 'no_probado') in ('no_probado', 'sin_efecto', 'no_original', 'no_registrado')]
    if nop:
        L += ['### No probados o sin efecto observable', '', '| Id | Comando | Estado | Motivo |', '| --- | --- | --- | --- |']
        for f in nop:
            b = BANCO.get(f[1]) or {}
            L.append(f"| {f[0]} | `{f[2]}` | {b.get('estado', 'no_probado')} | {b.get('nota') or 'sin resultados del banco'} |")
        L.append('')
    L += ['', '## Pendiente', '',
          '- Banco por comando: hecho (`reversing/CmdBench`, ver su README). Quedan las diferencias abiertas, los comandos no probados y los `sin_efecto` de la seccion anterior; `GTASA_CMD_BYPASS` (archivo con ids) sigue permitiendo volver al manejador original de cualquier comando en el juego normal.',
          '- `GET_CHAR_AREA_VISIBLE` (0x09E8) fue el primer error hallado en este inventario (devolvia un booleano en vez del id del interior; ropero de la casa de CJ): el banco lo detecta si se revierte la correccion.',
          '- Los 14 comandos de `Path` y `Draw` con manejador C++ no se registran (faltan `c::path::RegisterHandlers()` y `c::draw::RegisterHandlers()` en `RunningScript.cpp`): el juego usa el original del EXE y su codigo C++ esta inactivo.', '']
    SALIDA_MD.write_text(chr(10).join(L), encoding='utf-8')

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
    global BANCO, HALLAZGOS
    BANCO, HALLAZGOS = cargar_banco()
    riesgos = riesgo_mod.clasificar_filas([dict(estado=f[3], id_hex=f[0], fuente=f[5], manejador_cpp=f[4]) for f in filas])
    for f in filas:
        r = riesgos.get(f[0])
        f.extend([r['riesgo'], r['devuelve'], r['mundo'], r['motivo']] if r else ['', '', '', ''])
        b = BANCO.get(f[1]) if f[3] == 'reescrito' else None
        f.extend([b['estado'], b['casos'], b['diferencias'], (HALLAZGOS.get(f[1], {}).get('estado_final') or b['nota'] or b['tipos'])] if b else (['no_probado', '0', '0', 'sin resultados del banco'] if f[3] == 'reescrito' else ['', '', '', '']))
    with open(SALIDA, 'w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh, lineterminator=chr(10)); w.writerow(['id_hex', 'id', 'comando', 'estado', 'manejador_cpp', 'fuente', 'despachador_exe', 'registro_duplicado', 'despachador_reescrito', 'despachador_validado', 'riesgo', 'devuelve', 'mundo', 'motivo_riesgo', 'banco_estado', 'banco_casos', 'banco_dif', 'banco_nota'])
        w.writerows(filas)
    escribir_md(filas, cuenta, cuenta_disp)
    print(f'{len(filas)} comandos -> {SALIDA.relative_to(RAIZ)} y {SALIDA_MD.relative_to(RAIZ)}')
    for k, v in sorted(cuenta.items()): print(f'  {k:16} {v}')
    print('  de los "original": despachador reescrito en hooks.csv:', cuenta_disp)
    ext = sorted(set(reg) - set(ids))
    if ext: print(f'  registros fuera del enum vanilla: {len(ext)} (comandos propios o de CLEO)')
if __name__ == '__main__': sys.exit(main())
