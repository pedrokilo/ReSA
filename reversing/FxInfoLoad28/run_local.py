"""Compuertas independientes del banco FxInfoLoad28 (FxInfoManager_c::Load 0x5C0B70), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados
(route_ok incluye que el sscanf 0x8220AD del EXE no este enganchado), fp_difference vacio, la salida de CADA lado (objeto de 0x14 bytes con guardas, arena de punteros, FxInfo falsos,
FxInfo "basura", flujo sin tocar, cursor, traza resumida en longitud + hash FNV-1a 64 + cabeza y cola) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las
fixtures y la restauracion final (hook, entradas de codigo vigiladas, x87).

Si hay diferencias EXE/DLL, ANTES de abortar se escribe un desglose por clase de fixture (diferencias.json junto a comparisons.jsonl y en el mensaje de error). Nada se oculta: el banco
falla igualmente. `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o la corrida no termino).
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = oracle.NAMES
EXPECTED = dict(oracle.CASES)
NAME = oracle.NAME
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x5C0B70}
WATCHED = {0x5C0B70, 0x8220AD, 0x4AB250, 0x4A9CA0, 0x4A7B00}
STUBBED = {0x4AB250, 0x4A9CA0, 0x4A7B00}

# Minimos de cobertura. Calibrados con fixture_dump.cpp (compila Fixtures.hpp REAL, sin el juego) + fixture_mirror.py sobre los 4608 casos, dejando ~35-50 % de margen.
# Valores medidos: body_lines 2146, both_bits 860, junk_offsets 1138, long_lines 1123, move_from_render 1134, move_set 3474, mv_neg 104, n0 217, n_ge128 267, n_ge256 125, n_le0 308,
# n_neg 91, no_render 995, num_unparsed 327, retype_changes_offsets 511, retyped 1473, time_read 3482, time_skipped_version 568, tm_weird 2517, unknown_name 2245, ver_lt1 618,
# type_1001 1170, type_2001 817, type_4001 1009, type_8001 862, objs_0 373, objs_1 537, objs_8 1924, entries 91829.
MINIMUMS = {
    'body_lines': 1300, 'both_bits': 500, 'junk_offsets': 650, 'long_lines': 650, 'move_from_render': 650, 'move_set': 2000, 'mv_neg': 60,
    'n0': 130, 'n_ge128': 160, 'n_ge256': 75, 'n_le0': 180, 'n_neg': 55, 'render_from_count': 580, 'render_set': 2100, 'num_unparsed': 190,
    'retype_changes_offsets': 300, 'retyped': 850, 'time_read': 2000, 'time_skipped_version': 330, 'tm_weird': 1450, 'tm_sens': 700, 'tm_unparsed': 1100,
    'unknown_name': 1300, 'version_lt1': 360, 'ver_ge1': 2300,
    'type_1001': 650, 'type_2001': 450, 'type_4001': 580, 'type_8001': 480,
    'objs_0': 210, 'objs_1': 300, 'objs_2': 240, 'objs_3': 200, 'objs_4': 180, 'objs_5': 140, 'objs_6': 120, 'objs_7': 110, 'objs_8': 1100,
    'entries': 52000,
}
for _k in range(32):
    MINIMUMS['kind%d' % _k] = 140


def breakdown(rows):
    bad = [r for r in rows if r.get('event') == 'comparison' and not r.get('equal')]
    out = {'pares_distintos': len(bad), 'por_clase': {}, 'por_clave': {}, 'ejemplos': []}
    for r in bad:
        inp, a, c = r['input'], r['original'], r['candidate']
        keys = sorted(set(k for k in set(a) | set(c) if a.get(k) != c.get(k)))
        okeys = sorted(k for k in set(a.get('observed', {})) | set(c.get('observed', {})) if a.get('observed', {}).get(k) != c.get('observed', {}).get(k))
        out['por_clase'][str(inp['kind'])] = out['por_clase'].get(str(inp['kind']), 0) + 1
        for k in okeys or keys:
            out['por_clave'][k] = out['por_clave'].get(k, 0) + 1
        if len(out['ejemplos']) < 12:
            ex = {'pair': r['pair'], 'clase': inp['kind'], 'version': inp['version'], 'flujo': inp['stream'][:200], 'claves': okeys or keys}
            if okeys:
                k = okeys[0]
                ex['original'] = str(a['observed'].get(k))[:300]
                ex['candidato'] = str(c['observed'].get(k))[:300]
            out['ejemplos'].append(ex)
    return out


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    text = path.read_text(encoding='utf-8')
    rows = [json.loads(line) for line in text.splitlines() if line.endswith('}')]
    if not any(r.get('event') in ('complete', 'error', 'assertion') for r in rows):
        return None                                   # corrida sin terminar
    bad = [r for r in rows if r.get('event') == 'comparison' and not r.get('equal')]
    if bad:
        info = breakdown(rows)
        (path.parent / 'diferencias.json').write_text(json.dumps(info, indent=1), encoding='utf-8')
        raise RuntimeError('Diferencias EXE/DLL (desglose en diferencias.json): ' + json.dumps({k: info[k] for k in ('pares_distintos', 'por_clase', 'por_clave')}))
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin la marca de colaboradores sustituidos (este banco sustituye ReadLine, GetMem, AddFxInfo y el Load virtual)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    col = b['collaborators']
    if set(col['exe_real']) != {0x8220AD} or {p['exe'] for p in col['stubbed']} != STUBBED or any(not p['dll'] for p in col['stubbed']) or col.get('virtual_load_fake') is not True:
        raise RuntimeError('Lista de colaboradores inesperada o cuerpo de la DLL sin resolver: %s' % json.dumps(col))
    if b.get('mempool_this') != 0xA9AF2C or b.get('mempool_this_expected') != 0xA9AF2C:
        raise RuntimeError('El pool FX de la DLL no esta en 0xA9AF2C: %s' % json.dumps({k: b.get(k) for k in ('mempool_this', 'mempool_this_expected')}))
    crt = b['crt']
    if crt.get('exe_sscanf') != 0x8220AD or crt.get('exe_sscanf_in_game_image') is not True:
        raise RuntimeError('El sscanf del EXE no esta en la imagen del juego')
    seen, cov = set(), {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        inp = row['input']
        want, stats = oracle.expected(NAME, inp, case)      # Fail = fixture invalido (depende de comportamiento indefinido)
        for label, side in (('EXE', a), ('DLL', c)):
            diff = oracle.compare(side['observed'], want)
            if diff:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s), claves %s, clase %d, flujo %r:\n  obtenido %s\n  esperado %s' % (
                    case, label, diff, inp['kind'], inp['stream'][:160], {k: side['observed'].get(k) for k in diff}, {k: want[k] for k in diff}))
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
        cov['kind%d' % inp['kind']] = cov.get('kind%d' % inp['kind'], 0) + 1
        cov['ver_ge1'] = cov.get('ver_ge1', 0) + (inp['version'] >= 1)
        cov['entries'] = cov.get('entries', 0) + len(want['objs'])
        cov['render_set'] = cov.get('render_set', 0) + (not stats.get('render_from_count'))
        cov['move_set'] = cov.get('move_set', 0) + (not stats.get('move_from_render'))
        types = {o[0] for o in want['objs']}
        for t in (0x1001, 0x2001, 0x4001, 0x8001):
            cov['type_%x' % t] = cov.get('type_%x' % t, 0) + (t in types)
        cov['residue_differs'] = cov.get('residue_differs', 0) + (row['original_residue'] != row['candidate_residue'])
    if seen != set(range(EXPECTED[NAME])):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fxinfoload28', manifest['binaries']['asi'], analyze, 'FxInfoLoad28', timeout=1800))
