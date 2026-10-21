"""Compuertas del banco Helpers66C (lote 66: AEMFOpenSoundMedia 0x4E7990, AEMFGetMediaSoundFormat 0x4E7A80, GetDXVersion 0x745650, RwTextureGtaStreamRead 0x730E60 y
SkinGetBonePositionsToTable 0x735360): 4096 pares por funcion.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio (el estado x87 de RwTextureGtaStreamRead forma parte de la comparacion), ranuras de importacion encontradas en la DLL para las cuatro APIs de
GetDXVersion, cobertura por familia de las fixtures y cobertura de ramas medida sobre las entradas y los eventos de los colaboradores (los falsos registran sus argumentos, en orden).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

NAMES = ['AEMFOpenSoundMedia', 'AEMFGetMediaSoundFormat', 'GetDXVersion', 'RwTextureGtaStreamRead', 'SkinGetBonePositionsToTable']
EXPECTED = {n: 4096 for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x4E7990, NAMES[1]: 0x4E7A80, NAMES[2]: 0x745650, NAMES[3]: 0x730E60, NAMES[4]: 0x735360}
STUBBED = {
    NAMES[0]: [(0x818970, False), (0x81DAA0, False), (0x81B680, False), (0x81AB80, False), (0x81A9E0, False), (0x81BC60, False), (0x81A0E0, False)],
    NAMES[1]: [(0x818DB0, False), (0x81A300, False), (0x819860, False), (0x81C600, False), (0x8192B0, False), (0x818F90, False), (0x818E70, False), (0x818D70, False), (0x819390, False),
               (0x818FB0, False), (0x819270, False)],
    NAMES[2]: [],
    NAMES[3]: [(0x7ED2D0, False), (0x561A80, True), (0x561A40, True), (0x748F50, True), (0x748F20, True), (0x748F30, True)],
    NAMES[4]: [(0x749B70, False), (0x7C7550, False), (0x7C77E0, False), (0x7C7810, False), (0x7F2070, False), (0x7EDD90, False)],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
# Cobertura minima (calibrada con la primera corrida; con margen): [funcion] -> {clave: minimo de casos}
COVER = {
    NAMES[0]: {'info_fail': 150, 'open_fail': 150, 'new_fail': 150, 'no_track': 150, 'no_media': 150, 'success': 1500, 'close_after_fail': 150, 'refnum_high_garbage': 100},
    NAMES[1]: {'movies_fail': 700, 'wave_fail': 700, 'copy_done': 1500, 'ext_replaced': 150, 'mem_err': 300, 'ptr_null': 150, 'desc_null': 100, 'two_disposals': 1500},
    NAMES[2]: {'ret_0': 600, 'ret_700': 150, 'ret_800': 150, 'ret_801': 150, 'ret_900': 1000, 'hr_negative': 200, 'free_null': 100},
    NAMES[3]: {'find_fail': 150, 'native_fail': 150, 'null_texture': 150, 'stats': 1800, 'no_stats': 400, 'filter_nearest': 200, 'filter_mip': 200, 'aniso_set': 300, 'aniso_skip_quality': 200,
               'aniso_skip_level': 200, 'aniso_off': 300, 'avg_nan': 80, 'count_wrap': 80},
    NAMES[4]: {'table_null': 150, 'few_bones': 300, 'many_bones': 800, 'push_flag': 1500, 'pop_flag': 1000, 'hier_null': 40},
}


def kinds(side):
    return [e[0] for e in side['observed'].get('events', [])]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def tally(fn, row, cov):
    inp = row['input']
    cand = row['candidate']
    obs = cand['observed']
    k = kinds(cand)
    ev = obs.get('events', [])

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    if fn == NAMES[0]:
        ax = obs['ax']
        add('info_fail', 2 in k and 3 not in k)
        add('open_fail', 3 in k and 4 not in k)
        add('new_fail', 4 in k and 6 not in k)
        add('close_after_fail', 5 in k and 6 not in k)
        add('no_track', 6 in k and 7 not in k)
        add('no_media', 7 in k and ax == 0xF828)
        add('success', 7 in k and ax == 0)
        add('refnum_high_garbage', inp['ref_num'] > 0xFFFF or inp['ref_num'] >= 0x8000)
    elif fn == NAMES[1]:
        add('movies_fail', 13 in k and 14 not in k)
        add('wave_fail', 14 in k and 15 not in k)
        add('copy_done', 19 in k)
        add('ext_replaced', inp['ext_mode'] == 1 and 19 in k)
        add('mem_err', 18 in k and (obs['ax'] != 0) and 19 in k)
        add('ptr_null', 17 in k and inp['new_ptr_null'] == 1)
        add('desc_null', inp['desc_null'] == 1)
        add('two_disposals', k.count(21) >= 2)
    elif fn == NAMES[2]:
        eax = obs['eax']
        add('ret_0', eax == 0)
        add('ret_700', eax == 0x700)
        add('ret_800', eax == 0x800)
        add('ret_801', eax == 0x801)
        add('ret_900', eax == 0x900)
        add('hr_negative', inp['hr'] >= 0x80000000)
        add('free_null', any(e[0] == 33 and e[1] == 0 for e in ev))
    elif fn == NAMES[3]:
        add('find_fail', inp['find_ret'] == 0)
        add('native_fail', inp['find_ret'] != 0 and 44 in k and inp['native_ret'] == 0)
        add('null_texture', inp['find_ret'] != 0 and inp['native_ret'] != 0 and inp['tex_present'] == 0)
        add('stats', k.count(42) == 2)
        add('no_stats', inp['find_ret'] != 0 and inp['native_ret'] != 0 and k.count(42) == 1)
        low = inp['filter'] & 0xFF
        got = inp['tex_present'] != 0 and inp['find_ret'] != 0 and inp['native_ret'] != 0
        add('filter_nearest', got and low == 1)
        add('filter_mip', got and low == 3)
        add('aniso_set', 47 in k)
        add('aniso_off', got and (inp['aniso'] & 0xFF) == 0)
        add('aniso_skip_level', 45 in k and 47 not in k and (inp['aniso_get'] & 0xFF) < 1 or (45 in k and 47 not in k and (inp['aniso_get'] & 0x80)))
        add('aniso_skip_quality', 45 in k and 47 not in k and 0 < (inp['aniso_get'] & 0xFF) < 0x80 and (inp['quality'] if inp['quality'] < 0x80000000 else inp['quality'] - 2**32) < 2)
        add('avg_nan', k.count(42) == 2 and is_nan(inp['avg']))
        add('count_wrap', k.count(42) == 2 and inp['count'] in (0x7FFFFFFF, 0xFFFFFFFF, 0x80000000, 0xFFFFFFFE))
    elif fn == NAMES[4]:
        nb = inp['num_bones'] if inp['num_bones'] < 0x80000000 else inp['num_bones'] - 2**32
        add('table_null', inp['table_null'] == 1)
        add('few_bones', inp['table_null'] == 0 and nb <= 1)
        add('many_bones', inp['table_null'] == 0 and nb >= 20)
        add('hier_null', inp['hier_null'] == 1)
        flags = inp['flags'][1:max(nb, 1)] if inp['table_null'] == 0 else []
        add('push_flag', any(f & 2 for f in flags))
        add('pop_flag', any(f & 1 for f in flags))


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
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if col[fn]['dll_real'] or got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    slots = b['import_slots']
    if set(slots) != {'LoadLibraryA', 'GetProcAddress', 'FreeLibrary', 'OutputDebugStringA'} or not all(s['dll_slots'] >= 1 and s['exe_value'] for s in slots.values()):
        raise RuntimeError('Ranuras de importacion de GetDXVersion sin resolver en la DLL')
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(fn, row, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(4096)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 16 or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        for key, minimum in COVER.get(fn, {}).items():
            if cov[fn].get(key, 0) < minimum:
                raise RuntimeError('Cobertura de ramas insuficiente en %s: %s %d < %d' % (fn, key, cov[fn].get(key, 0), minimum))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('helpers66c', manifest['binaries']['asi'], analyze, 'Helpers66C', timeout=3600))
