"""Compuertas independientes del banco PlateCharset69: CCustomCarPlateMgr::LoadPlatecharsetDat (0x6FDC00), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, los cuatro colaboradores de archivo (CFileMgr::SetDir 0x5387D0, OpenFile 0x538900,
CFileLoader::LoadLine(FILESTREAM) 0x536F80 y CFileMgr::CloseFile 0x5389D0) con hook y cuerpo en la DLL, sustituidos por falsos en AMBOS lados, todas las compuertas de ABI/ruta/SEH/DF/restauracion
en AMBOS lados, fp_difference vacio, la salida del lado EXE (AL, traza de los falsos, CRC de la arena, dwords escritos, buffer de linea tras strtok) igual a la que modela fixture_oracle.py desde el
desensamblado, 16 familias x 1024 casos, cobertura de ramas (coverage.json, medida con fixture_selfcheck.py) y la restauracion final (hooks, entradas vigiladas, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta terminado).
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
EXPECTED = {name: oracle.CASES for name in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ADDRESS))
STUBBED = [(0x5387D0, True), (0x538900, True), (0x536F80, True), (0x5389D0, True)]
WATCHED = {a for a, _ in STUBBED} | {0x6FDC00}
FAMILY_SIZE = oracle.CASES // 16
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    if not COVER:
        raise RuntimeError('Falta coverage.json (python -I fixture_selfcheck.py --write)')
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado; solo AL se compara en observed')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators'][NAMES[0]]
    got = [(s['exe'], s['hooked']) for s in col['stubbed']]
    if col['dll_real'] or got != STUBBED or not all(s['dll'] for s in col['stubbed']):
        raise RuntimeError('Colaboradores sustituidos inesperados: ' + json.dumps(col))
    if len(b['watched_entries']) != len(WATCHED) + 1:       # las cuatro de los colaboradores, la del EXE de la funcion y el cuerpo de la DLL
        raise RuntimeError('Entradas vigiladas inesperadas')
    if not WATCHED <= {w['address'] for w in b['watched_entries']}:
        raise RuntimeError('Faltan entradas vigiladas')
    for key, want in oracle.LAYOUT.items():
        if b['layout'][key] != want:
            raise RuntimeError('La arena del banco no coincide con las constantes del oraculo: %s' % key)
    seen = set()
    fam = {}
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in HOOKS or case in seen or row['input']['func'] != 0 or row['input']['n'] != case:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
        for side in (row['original'], row['candidate']):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, _, stats = oracle.expected(row['input'])
        for label, side in (('EXE', row['original']), ('DLL', row['candidate'])):
            diff = [k for k in want if side['observed'].get(k) != want[k]]
            if diff:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (case, label, diff, side['observed'], want))
        f = row['input']['family']
        fam[f] = fam.get(f, 0) + 1
        for k in stats:
            cov[k] = cov.get(k, 0) + 1
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    if len(fam) != 16 or any(v != FAMILY_SIZE for v in fam.values()):
        raise RuntimeError('Cobertura por familia incorrecta: %s' % fam)
    short = {k: (cov.get(k, 0), m) for k, m in COVER.items() if cov.get(k, 0) < m}
    if short:
        raise RuntimeError('Cobertura de ramas insuficiente: ' + json.dumps(short))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('platecharset69', manifest['binaries']['asi'], analyze, 'PlateCharset69', timeout=3600))
