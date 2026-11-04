"""Compuertas del banco Fn95A (lote 95A): nueve metodos de D3DTextureBuffer y D3DIndexDataBuffer (EXE 0x72FE80, 0x72FF60, 0x72FFF0, 0x730020, 0x7300A0, 0x730AD0, 0x730270, 0x730330, 0x7303B0; hook S), 147456 pares (16384 por funcion).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: valor devuelto, ST0 byte a byte, eventos de los falsos con todos sus argumentos,
bytes cambiados de las arenas), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el modelo (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
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

NAMES = ['D3DTextureBuffer::Setup', 'D3DTextureBuffer::Pop', 'D3DTextureBuffer::Push', 'D3DTextureBuffer::Resize', 'D3DTextureBuffer::GetTotalDataSize',
         'D3DTextureBuffer::PushWithoutIncreasingCounter', 'D3DIndexDataBuffer::Pop', 'D3DIndexDataBuffer::Resize', 'D3DIndexDataBuffer::GetTotalDataSize']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x72FE80, 0x72FF60, 0x72FFF0, 0x730020, 0x7300A0, 0x730AD0, 0x730270, 0x730330, 0x7303B0)))
# (direccion del EXE, hooked): operator new (0x82119A) y operator delete (0x8214BD) de la CRT, sin hook RH en la DLL (la DLL los llama por la direccion del EXE: el mismo parche)
NEW, DELETE = 0x82119A, 0x8214BD
STUBBED = {n: [] for n in NAMES}
STUBBED[NAMES[0]] = [(NEW, False)]
STUBBED[NAMES[3]] = [(NEW, False), (DELETE, False)]
STUBBED[NAMES[7]] = [(NEW, False), (DELETE, False)]
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | set(HOOKS.values())
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}

KEYS = {
    NAMES[0]: ['cap_zero', 'cap_16', 'cap_other', 'cap_big'],
    NAMES[1]: ['empty', 'found', 'not_found', 'one_level', 'multi_level', 'swap', 'no_swap', 'desc_mismatch', 'levels_mismatch'],
    NAMES[2]: ['stored', 'full', 'stored_guard', 'counter_incremented'],
    NAMES[3]: ['same_capacity', 'grows', 'shrinks', 'copies', 'releases'],
    NAMES[4]: ['generic', 'single_format', 'x87', 'x87_single', 'plain', 'plain_single', 'unsigned_fixup', 'empty', 'negative_size'],
    NAMES[5]: ['stored', 'full', 'stored_guard'],
    NAMES[6]: ['empty', 'found', 'not_found', 'swap', 'no_swap'],
    NAMES[7]: ['same_capacity', 'grows', 'shrinks', 'copies', 'releases'],
    NAMES[8]: ['generic', 'single_format', 'empty'],
}


def tally(inp, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    e = oracle.expected(inp)
    for m in set(e['marks']):
        cov[m] = cov.get(m, 0) + 1


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    b = [r for r in rows if r['event'] == 'begin']
    if len(b) != 1 or b[0].get('registered_hook') is not True or b[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = b[0]
    if b.get('rng_native') is not True:
        raise RuntimeError('rand() del EXE no nativo')
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # los colaboradores (CRT) no tienen hook RH en la DLL: la DLL los llama por la direccion del EXE, que lleva el mismo falso
        if any(s['dll'] is not False or s['hooked'] is not False for s in col[fn]['stubbed']):
            raise RuntimeError('Colaborador de la CRT con hook inesperado en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        for side in (row['original'], row['candidate']):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if row['original'] != row['candidate']:
            raise RuntimeError('EXE y DLL difieren en %s %d' % (fn, case))
        bad = oracle.check_row(row)
        if bad:
            raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS[fn]}
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fn95a', manifest['binaries']['asi'], analyze, 'Fn95A', timeout=7200))
