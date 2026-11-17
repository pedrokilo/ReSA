"""Informe de progreso (docs/ReversedClasses.md): validado, reconstruido sin validar, código del juego y RenderWare por separado.

Lo llama contrib/hooks2md.py. Entrada: docs/hooks.csv (filas con reversed/validated), el mapa del EXE (contrib/exe_coverage.py), el mapa de
RenderWare (docs/RenderWareExe/funciones.csv), docs/JuegoExe/excluidas.csv y los informes de lotes pendientes de validar
(reversing/PendienteValidar/*.md y su PLAN_VALIDACION.md).
"""
import csv
import re
import datetime
import re
from collections import Counter, defaultdict
from pathlib import Path

GAME_LO, GAME_HI = 0x401000, 0x74FFFF
RW_LO, RW_HI = 0x750000, 0x81FFFF
TOP_GAME_CLASSES = 40


def _region(address):
    if GAME_LO <= address <= GAME_HI:
        return 'juego'
    if RW_LO <= address <= RW_HI:
        return 'renderware'
    return 'otro'


def _pct(a, b):
    return f'{a / b:.2%}' if b else 'n/d'


def _bar(a, b, n=20):
    k = round(n * a / b) if b else 0
    return '█' * k + '░' * (n - k)


def _table(head, rows, right=()):
    out = ['| ' + ' | '.join(head) + ' |',
           '| ' + ' | '.join('---:' if i in right else '---' for i in range(len(head))) + ' |']
    out += ['| ' + ' | '.join(str(c) for c in r) + ' |' for r in rows]
    return out


def classify_hooks(hooks_csv):
    """Devuelve {clase: [region, reemplazadas, validadas]} y los totales por región. Las filas virtuales (dirección 0) heredan la región de su clase."""
    rows = []
    with open(hooks_csv, encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            if r['reversed'] != '1':
                continue
            try:
                a = int(r['address'], 16)
            except ValueError:
                a = 0
            rows.append((r['class'], a, r['validated'] == '1'))
    votes = defaultdict(Counter)
    for c, a, _ in rows:
        if a:
            votes[c][_region(a)] += 1
    region_of = {c: v.most_common(1)[0][0] for c, v in votes.items()}
    per = {}
    for c, a, v in rows:
        reg = _region(a) if a else region_of.get(c, 'juego')
        e = per.setdefault(c, [region_of.get(c, reg), 0, 0])
        e[1] += 1
        e[2] += v
    tot = {k: [0, 0] for k in ('juego', 'renderware', 'otro')}
    for c, (reg, n, v) in per.items():
        tot[reg][0] += n
        tot[reg][1] += v
    return per, tot


def lots(root):
    """Oleadas de validación del juego y lotes pendientes (su informe sigue en reversing/PendienteValidar)."""
    base = Path(root) / 'reversing/PendienteValidar'
    plan = base / 'PLAN_VALIDACION.md'
    waves, wave = [], None
    if plan.exists():
        for line in plan.read_text(encoding='utf-8').splitlines():
            m = re.match(r'### (V\d) · ', line)
            if m:
                wave = [m.group(1), []]
                waves.append(wave)
                continue
            m = re.match(r'\| (\d+A) \| (\d+) \|', line)
            if m and wave:
                wave[1].append((m.group(1), int(m.group(2))))
    # Un lote está pendiente si alguna de las direcciones de su informe sigue en hooks.csv con validated=0 (el archivo del informe puede quedar de un lote ya validado).
    unvalidated = set()
    rows = Path(root) / 'docs/hooks.csv'
    if rows.exists():
        with open(rows, encoding='utf-8-sig', newline='') as f:
            for r in csv.DictReader(f):
                if r['reversed'] == '1' and r['validated'] != '1':
                    try:
                        unvalidated.add(int(r['address'], 16))
                    except ValueError:
                        pass
    pending = set()
    for p in base.glob('*.md'):
        addrs = {int(a, 16) for a in re.findall(r'\|\s*`?(0x[0-9A-Fa-f]{6,8})`?\s*\|', p.read_text(encoding='utf-8', errors='replace'))}
        if addrs & unvalidated:
            pending.add(p.stem)
    running = set()
    marker = base / 'en_curso.txt'
    if marker.exists():
        running = set(marker.read_text(encoding='utf-8').split())
    return waves, pending, running


def rw_modules(root):
    path = Path(root) / 'docs/RenderWareExe/funciones.csv'
    mods = {}
    if path.exists():
        with open(path, encoding='utf-8', newline='') as f:
            for r in csv.DictReader(f):
                m = mods.setdefault(r['clase_modulo'], Counter())
                m['n'] += 1
                m[r['estado']] += 1
    return mods


def excluded_summary(root):
    path = Path(root) / 'docs/JuegoExe/excluidas.csv'
    kinds = Counter()
    if path.exists():
        with open(path, encoding='utf-8', newline='') as f:
            for r in csv.DictReader(f):
                t = r['motivo'].lower()
                if '__usercall' in t or 'registros' in t:
                    kinds['__usercall con registros que C++ no puede leer'] += 1
                elif 'jmp' in t or 'salto' in t:
                    kinds['saltos puros a otra dirección'] += 1
                else:
                    kinds['fragmentos internos de otra función y otros'] += 1
    return kinds


def build(root, input_path, coverage, validation, notsa_count=0):
    root = Path(root)
    now = datetime.datetime.now(datetime.timezone.utc)
    fecha = now.strftime('%d/%m/%Y')
    per, tot = classify_hooks(input_path)
    g, rw = coverage['regions']['juego'], coverage['regions']['renderware']
    gm = coverage['game_remaining_total']
    g_rep, g_val = tot['juego']
    r_rep, r_val = tot['renderware']
    o_rep, o_val = tot['otro']
    all_rep = g_rep + r_rep + o_rep
    all_val = g_val + r_val + o_val
    mods = rw_modules(root)
    rw_total = sum(m['n'] for m in mods.values())
    rw_link = sum(m['enlazada'] for m in mods.values())
    rw_nr = sum(m['no_reconstruible'] for m in mods.values())
    rw_pend = sum(m['n'] - m['reemplazada'] - m['enlazada'] - m['no_reconstruible'] for m in mods.values())
    excl = excluded_summary(root)
    waves, pending, running = lots(root)

    L = ['# Progreso del reversing', '',
         '[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md) · [Registro de validaciones](../reversing/ValidationRegistry/README.md)', '',
         f'> **Fuente:** [hooks.csv](hooks.csv) y el análisis estático del EXE · **Generado:** {now:%Y-%m-%d %H:%M} UTC', '',
         '**Índice:** [1. Dónde estamos](#donde-estamos) · [2. Código del juego](#juego) · [3. RenderWare](#renderware) · [4. Qué falta por validar](#por-validar) · [5. Clases del juego sin validar](#clases) · [6. Cómo leer y regenerar](#metodo)', '',
         '<a id="donde-estamos"></a>', '', '## 1. Dónde estamos', '',
         '@@CORE_START@@', 'Tres conceptos distintos, que no se mezclan en ninguna cifra:', '',
         '- **Reconstruida:** la función existe en C++ puro, tiene su hook en la DLL y compila. Es lo que mide el EXE (`hooks.csv`, `reversed=1`).',
         '- **Validada:** además se comparó con el EXE dentro del juego (≥16.384 pares sin diferencias, oráculo independiente y prueba negativa de dos rondas) y su evidencia está en el registro (`validated=1`).',
         '- **Reconstruida sin validar:** integrada y activa, pero sin banco todavía (`validated=0`). Compila, pero no se ha ejecutado en el juego contra el original.', '',
         'Y dos zonas del EXE que se miden por separado: el **código del juego** (0x401000–0x74FFFF, el trabajo propio del proyecto) y **RenderWare y D3D** (0x750000–0x81FFFF, el SDK enlazado dentro del EXE).', '',
         f'Cifras al {fecha}.', '']
    L += _table(['Zona', 'Funciones del EXE', 'Reconstruidas', 'Validadas', 'Reconstruidas sin validar', 'Faltan'], [
        ['**Código del juego**', f"{g['functions']:,}", f"{g['replaced']:,} ({_pct(g['replaced'], g['functions'])})", f'{g_val:,}', f'{g_rep - g_val:,}', f"{gm['functions']:,}"],
        ['**RenderWare y D3D**', f'{rw_total:,}', f"{rw['replaced']:,} ({_pct(rw['replaced'], rw_total)})", f'{r_val:,}', f'{r_rep - r_val:,}', f'{rw_pend:,}'],
    ], right=(1, 2, 3, 4, 5))
    L += ['', f'En total hay **{all_rep:,} entradas reconstruidas, {all_val:,} validadas y {all_rep - all_val:,} sin validar** en el catálogo ({validation["registered"]:,} con evidencia registrada).'
          + (f' Quedan además {o_rep:,} entradas fuera de ambas zonas (CRT y utilidades).' if o_rep else ''), '']

    L += ['### Fase del proceso', '']
    L += _table(['Fase', 'Estado'], [
        ['Reconstruir el código del juego en C++ puro', f"**Hecha**: {_pct(g['replaced'], g['functions'])} de las funciones y {_pct(g['replaced_bytes'], g['bytes'])} de los bytes; faltan {gm['functions']} y hay {len(coverage['game_excluded'])} excluidas por no ser reconstruibles"],
        ['Reconstruir el SDK de RenderWare', f'**Hecha** en lo reconstruible: {rw["replaced"]:,} funciones; {rw_link:,} son libpng, zlib y D3DX9 (se enlaza la original) y {rw_nr} no son reconstruibles'],
        ['**Validar el código del juego**', f'**En curso**: {g_val:,} de {g_rep:,} reconstruidas ({_pct(g_val, g_rep)}); el resto va por oleadas de lotes (detalle en el [informe](ReversedClasses.md#por-validar))'],
        ['Validar RenderWare', f'**Apenas empezada**: {r_val:,} de {r_rep:,} ({_pct(r_val, r_rep)}); falta el plan de bancos (dispositivo D3D falso, flujos y listas libres falsos)'],
        ['Pase final de bancos viejos', 'Pendiente: repetir los bancos antiguos cuyos colaboradores ahora tienen hook o cuyo código cambió'],
        ['Ejecutable propio', 'Después: sustituir RenderWare, D3D y CRT por bibliotecas y quitar la dependencia del EXE ([hoja de ruta](ProjectRoadmap.md))'],
    ])
    L.append('@@CORE_END@@')

    L += ['', '<a id="juego"></a>', '', '## 2. Código del juego (0x401000–0x74FFFF)', '']
    L += _table(['Medida', 'Valor', 'Progreso'], [
        ['Funciones reconstruidas', f"{g['replaced']:,} / {g['functions']:,} · {g['replaced'] / g['functions']:.2%}", f"`{_bar(g['replaced'], g['functions'])}`"],
        ['Bytes de código reconstruidos', f"{g['replaced_bytes']:,} / {g['bytes']:,} · {g['replaced_bytes'] / g['bytes']:.2%}", f"`{_bar(g['replaced_bytes'], g['bytes'])}`"],
        ['Entradas validadas (de las reconstruidas)', f'{g_val:,} / {g_rep:,} · {g_val / g_rep:.2%}', f'`{_bar(g_val, g_rep)}`'],
        ['Entradas reconstruidas sin validar', f'{g_rep - g_val:,}', ''],
    ], right=())
    L += ['', f"**Faltan {gm['functions']} funciones ({gm['bytes']:,} bytes)**, listadas en [pendientes-por-clase.md](JuegoExe/pendientes-por-clase.md), y **{len(coverage['game_excluded'])} excluidas** y documentadas en [excluidas.csv](JuegoExe/excluidas.csv) porque no se pueden reconstruir en C++ puro: "
          + '; '.join(f'{v} {k}' for k, v in excl.most_common()) + '.', '',
          'Todo lo reconstruido está en C++: en `source/game_sa` no queda ningún `__asm` ni función `naked` ([estado de la migración](../reversing/CppOnlyMigration/README.md)). Cada validada tiene su banco `FnNNNA` y su evidencia en `reversing/CppTwelveLevel6Final/runs`.', '',
          '<a id="cobertura-exe"></a>', '',
          f"Detalle del mapa del EXE (estados A–D por función): [JuegoExe/README.md](JuegoExe/README.md). El mapa de RenderWare está en [RenderWareExe/README.md](RenderWareExe/README.md).", '']

    L += ['<a id="renderware"></a>', '', '## 3. RenderWare y D3D (0x750000–0x81FFFF)', '',
          f"Región de {rw_total:,} funciones y {rw['bytes']:,} bytes: el SDK de RenderWare 3.7, el controlador D3D9, D3DX9 (compilador de shaders), libpng, zlib y libjpeg, enlazados dentro del EXE.", '']
    L += _table(['Medida', 'Funciones'], [
        ['**Reconstruidas en C++** (hook en la DLL)', f"**{rw['replaced']:,}**"],
        ['Validadas', f'{r_val:,}'],
        ['Reconstruidas sin validar', f'{r_rep - r_val:,}'],
        ['Enlazadas (libpng, zlib y D3DX9: se enlaza la original, no se reconstruyen)', f'{rw_link:,}'],
        ['No reconstruibles en C++ puro (puentes de QuickTime, carga retardada, 3DNow!, ya enganchadas por el modo ventana)', f'{rw_nr}'],
        ['**Pendientes de reconstruir**', f'**{rw_pend}**'],
    ], right=(1,))
    L += ['', '**Por módulo** (clase de hook; lo reconstruido y lo validado):', '']
    rw_rows = sorted(((c, v[1], v[2]) for c, v in per.items() if v[0] == 'renderware'), key=lambda x: (-(x[1] - x[2]), x[0]))
    L += _table(['Módulo', 'Reconstruidas', 'Validadas', 'Sin validar'], [[c, n, v, n - v] for c, n, v in rw_rows], right=(1, 2, 3))
    L += ['', 'Nada de RenderWare se ha ejecutado todavía contra el original: el [plan de bancos](../reversing/PendienteValidar/PLAN_VALIDACION.md) de esta zona es la siguiente gran tarea de validación. Funciones por módulo y su estado: [RenderWareExe/pendientes-por-clase.md](RenderWareExe/pendientes-por-clase.md).', '']

    L += ['<a id="por-validar"></a>', '', '## 4. Qué falta por validar (código del juego)', '',
          'Los lotes de validación van por oleadas y cada uno tiene su informe en [reversing/PendienteValidar](../reversing/PendienteValidar). Un lote está validado cuando su informe desaparece de esa carpeta.', '']
    rows = []
    for wname, items in waves:
        pend = [(lot, n) for lot, n in items if lot in pending or lot in running]
        done = len(items) - len(pend)
        rows.append([wname, f'{items[0][0]}–{items[-1][0]}' if items else '', len(items), done, len(pend), sum(n for _, n in pend)])
    L += _table(['Oleada', 'Lotes', 'Lotes totales', 'Validados', 'Por validar', 'Funciones por validar'], rows, right=(2, 3, 4, 5))
    todo = [lot for _, items in waves for lot, _ in items if (lot in pending or lot in running) and lot not in running]
    if running:
        L += ['', 'En validación ahora: ' + ', '.join(sorted(running)) + '.']
    if todo:
        L += ['', 'Lotes por validar: ' + ', '.join(todo) + '.']
    L += ['', 'Los lotes de RenderWare (R1A–R34A) se validarán en una oleada propia, aún sin plan.', '']

    L += ['<a id="clases"></a>', '', '## 5. Clases del juego con funciones sin validar', '',
          f'Solo se listan las clases del código del juego que todavía tienen alguna función reconstruida sin validar (las {TOP_GAME_CLASSES} con más). El resto de las clases del juego está completa y validada. Lista completa: [no-validadas-por-clase.csv](JuegoExe/no-validadas-por-clase.csv).', '']
    game_rows = sorted(((c, v[1], v[2]) for c, v in per.items() if v[0] == 'juego' and v[1] != v[2]), key=lambda x: (-(x[1] - x[2]), x[0]))
    L += _table(['Clase', 'Reconstruidas', 'Validadas', 'Sin validar'], [[c, n, v, n - v] for c, n, v in game_rows[:TOP_GAME_CLASSES]], right=(1, 2, 3))
    with open(root / 'docs/JuegoExe/no-validadas-por-clase.csv', 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['clase', 'reconstruidas', 'validadas', 'sin_validar'])
        w.writerows([[c, n, v, n - v] for c, n, v in game_rows])
    if len(game_rows) > TOP_GAME_CLASSES:
        rest = game_rows[TOP_GAME_CLASSES:]
        L += ['', f'Otras {len(rest)} clases con {sum(n - v for _, n, v in rest)} funciones sin validar entre todas (cada una con {max(n - v for _, n, v in rest)} o menos).']
    complete = [c for c, v in per.items() if v[0] == 'juego' and v[1] == v[2]]
    L += ['', f'Clases del juego completas y validadas: **{len(complete):,}** de {len([1 for v in per.values() if v[0] == "juego"]):,}.', '']

    L += ['<a id="metodo"></a>', '', '## 6. Cómo leer y regenerar', '',
          '- **Reconstruida** = `reversed=1` en [hooks.csv](hooks.csv); **validada** = `validated=1`, con evidencia aprobada vinculada en el registro. Validada no implica vigente para el árbol actual: la vigencia se consulta en el [inventario](ValidationInventory.md).',
          '- **Zona** = dirección de la función en el EXE; las entradas virtuales (dirección 0) heredan la zona de su clase.',
          '- Los porcentajes de este informe dicen siempre de qué miden (funciones, bytes o entradas del catálogo).', '',
          'Regenerar (sincroniza primero `validated` desde el registro):', '',
          '```powershell', 'python contrib/sync_validated_flags.py', 'python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md', '```', '',
          'El formato vive en [contrib/progress_report.py](../contrib/progress_report.py). La historia de entregas anteriores está en [Historial.md](archivo/Historial.md).', '']
    es = lambda x: re.sub(r'(\d)\.(\d{1,2})%', r'\1,\2%', re.sub(r'(?<=\d),(?=\d{3}(?!\d))', '.', x))
    L = [es(x) for x in L]
    i, j = L.index('@@CORE_START@@'), L.index('@@CORE_END@@')
    core = L[i + 1:j]
    report = [x for x in L if x not in ('@@CORE_START@@', '@@CORE_END@@')]
    readme = [x.replace('(ProjectRoadmap.md)', '(docs/ProjectRoadmap.md)').replace('(JuegoExe/', '(docs/JuegoExe/').replace('(ReversedClasses.md', '(docs/ReversedClasses.md') for x in core]
    return report, readme
