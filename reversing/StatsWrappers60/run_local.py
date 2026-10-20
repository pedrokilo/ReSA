"""Compuertas independientes del banco StatsWrappers60: CStats::FindLeastFavoriteRadioStation (0x559010, 4096 pares), BuildStatLine (0x559230, 6144), UpdateRespectStat (0x55BC50, 6144)
y ConstructStatLine (0x55A780, 8192): 24576 pares en total.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: los cuatro hooks registrados y con el cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS
lados (route_ok incluye que 0x69F7E0, 0x719240 y 0x821BB5 del EXE no esten enganchados), fp_difference vacio, la salida de CADA lado (buffers de texto con hash y prefijo, traza de
llamadas a los colaboradores sustituidos, valores de las estadisticas, nada fuera de lo permitido) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las
fixtures (minimos calibrados con el volcado exacto del generador: las fixtures son deterministas) y la restauracion final (hooks, entradas de codigo vigiladas, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no ha terminado).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NF: 0x559010, oracle.NB: 0x559230, oracle.NR: 0x55BC50, oracle.NC: 0x55A780}
EXE_ONLY = {0x69F7E0, 0x719240, 0x821BB5}
STUBBED = {0x558E40, 0x56D220, 0x56D200, 0x558EC0, 0x558FA0, 0x559010, 0x5591E0, 0x559540, 0x559560, 0x559230, 0x6A0050, 0x69F7E0, 0x718660, 0x719240, 0x718600, 0x821BB5,
           0x507010, 0x56E210, 0x5E3B60, 0x49CDA0, 0x5595F0, 0x55B980, 0x559760}
WATCHED = {0x559010, 0x559230, 0x55BC50, 0x55A780, 0x821BB5, 0x69F7E0, 0x719240, 0x558E40, 0x56D220, 0x56D200, 0x558EC0, 0x558FA0, 0x5591E0, 0x559540, 0x559560, 0x6A0050, 0x718660, 0x718600,
           0x507010, 0x56E210, 0x5E3B60, 0x49CDA0, 0x5595F0, 0x55B980, 0x559760}

# Minimos de cobertura por funcion: calibrados (offline/calibrate.py) con el conteo EXACTO que da el oraculo sobre el volcado del generador C++ (las fixtures son deterministas):
# 60 % de lo observado (al menos 1).
MINIMUMS = {
    "CStats::FindLeastFavoriteRadioStation": {
        "fam0": 153,
        "fam1": 153,
        "fam10": 153,
        "fam11": 153,
        "fam12": 153,
        "fam13": 153,
        "fam14": 153,
        "fam15": 153,
        "fam2": 153,
        "fam3": 153,
        "fam4": 153,
        "fam5": 153,
        "fam6": 153,
        "fam7": 153,
        "fam8": 153,
        "fam9": 153,
        "idx1": 729,
        "idx10": 132,
        "idx11": 119,
        "idx12": 300,
        "idx2": 177,
        "idx3": 157,
        "idx4": 123,
        "idx5": 121,
        "idx6": 103,
        "idx7": 265,
        "idx8": 114,
        "idx9": 111,
        "idx_first": 729,
        "min_outside": 450,
        "tie_min": 844
    },
    "CStats::BuildStatLine": {
        "g0": 153,
        "g1": 153,
        "g10": 153,
        "g11": 153,
        "g12": 153,
        "g13": 153,
        "g14": 153,
        "g15": 153,
        "g16": 153,
        "g17": 153,
        "g18": 153,
        "g19": 153,
        "g2": 153,
        "g20": 153,
        "g21": 153,
        "g22": 153,
        "g23": 153,
        "g3": 153,
        "g4": 153,
        "g5": 153,
        "g6": 153,
        "g7": 153,
        "g8": 153,
        "g9": 153,
        "kg": 88,
        "lbs": 82,
        "line_null": 153,
        "nan_arg": 667,
        "single_m0": 168,
        "single_m1": 174,
        "single_m2": 163,
        "single_m3": 167,
        "single_m4": 168,
        "single_m5": 171,
        "single_m6": 166,
        "single_m7": 168,
        "single_m8": 163,
        "single_m9": 168,
        "single_none": 321,
        "single_null": 153,
        "single_oob": 167,
        "snan_arg": 63,
        "two_m0": 185,
        "two_m1": 180,
        "two_m3": 185,
        "two_other": 520,
        "type1_ge10": 326,
        "type1_lt10": 134
    },
    "CStats::UpdateRespectStat": {
        "arg0_nonzero": 230,
        "cheat": 876,
        "dec_both": 78,
        "dec_noval": 2,
        "dec_show": 344,
        "dec_thr_false": 256,
        "dec_thr_true": 246,
        "decrease": 505,
        "f0": 230,
        "f1": 230,
        "f10": 230,
        "f11": 230,
        "f12": 230,
        "f13": 230,
        "f14": 230,
        "f15": 230,
        "f2": 230,
        "f3": 230,
        "f4": 230,
        "f5": 230,
        "f6": 230,
        "f7": 230,
        "f8": 230,
        "f9": 230,
        "high_nan": 82,
        "inc_low": 301,
        "inc_noval": 911,
        "inc_show": 1287,
        "inc_thr_false": 942,
        "inc_thr_true": 931,
        "increase": 2785,
        "low_nan": 72,
        "money_clamped": 612,
        "respect_nan": 60,
        "same_int": 165,
        "sum_negative": 564,
        "territory_clamped": 2557,
        "total_clamped": 1771
    },
    "CStats::ConstructStatLine": {
        "beyond": 2368,
        "fmt0": 2698,
        "fmt1": 199,
        "fmt10": 925,
        "fmt2": 190,
        "fmt3": 1232,
        "fmt4": 520,
        "fmt5": 424,
        "fmt6": 191,
        "fmt7": 793,
        "fmt8": 176,
        "fmt9": 913,
        "found": 2547,
        "german_keep": 806,
        "german_skip": 75,
        "h0": 551,
        "h1": 376,
        "h10": 322,
        "h11": 307,
        "h12": 330,
        "h1_header": 19,
        "h2": 383,
        "h3": 388,
        "h4": 279,
        "h5": 434,
        "h6": 408,
        "h7": 246,
        "h8": 240,
        "h9": 372,
        "h_girls": 46,
        "hi320": 537,
        "hi321": 28,
        "hi322": 172,
        "hi323": 482,
        "hi324": 237,
        "hi325": 237,
        "hi326": 426,
        "hi327": 258,
        "hi328": 498,
        "hi329": 449,
        "hi330": 290,
        "hi331": 345,
        "hi332": 204,
        "hi333": 182,
        "hi334": 138,
        "hi335": 34,
        "hi336": 3,
        "hi337": 271,
        "pr_ST_PR01": 10,
        "pr_ST_PR02": 4,
        "pr_ST_PR03": 6,
        "pr_ST_PR05": 1,
        "pr_ST_PR07": 6,
        "pr_ST_PR09": 3,
        "pr_ST_PR16": 2,
        "pr_ST_PR19": 102,
        "real": 2600,
        "skip_nonpositive": 2560,
        "special": 3935,
        "synth": 2314,
        "t0": 297,
        "t1": 328,
        "t2": 328,
        "t3": 291,
        "t4": 319,
        "t5": 313,
        "t6": 333,
        "t7": 2703
    }
}


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    text = path.read_text(encoding='utf-8')
    rows = [json.loads(line) for line in text.splitlines() if line.endswith('}')]
    if not any(r.get('event') in ('complete', 'error', 'assertion') for r in rows):
        return None                                   # corrida sin terminar
    bad = [r for r in rows if r.get('event') == 'comparison' and not r.get('equal')]
    if bad:
        info = {'pares_distintos': len(bad), 'por_funcion': {}, 'ejemplos': []}
        for r in bad:
            info['por_funcion'][r['function']] = info['por_funcion'].get(r['function'], 0) + 1
            if len(info['ejemplos']) < 12:
                a, c = r['original'], r['candidate']
                keys = sorted(k for k in set(a.get('observed', {})) | set(c.get('observed', {})) if a.get('observed', {}).get(k) != c.get('observed', {}).get(k))
                gate_keys = sorted(k for k in set(a) | set(c) if k != 'observed' and a.get(k) != c.get(k))
                info['ejemplos'].append({'pair': r['pair'], 'funcion': r['function'], 'caso': r['case'], 'claves': keys, 'compuertas': gate_keys})
        (path.parent / 'diferencias.json').write_text(json.dumps(info, indent=1), encoding='utf-8')
        raise RuntimeError('Diferencias EXE/DLL (desglose en diferencias.json): ' + json.dumps(info['por_funcion']))
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin la marca de colaboradores sustituidos (este banco sustituye 23 entradas de colaboradores)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 4:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    col = b['collaborators']
    if set(col['exe_only']) != EXE_ONLY or {p['exe'] for p in col['stubbed']} != STUBBED:
        raise RuntimeError('Lista de colaboradores inesperada: %s' % json.dumps(col))
    for p in col['stubbed']:
        if p['exe'] not in EXE_ONLY and not p['dll']:
            raise RuntimeError('Cuerpo de la DLL sin resolver para el colaborador %#x' % p['exe'])
    cov = {n: {} for n in NAMES}
    seen = {n: set() for n in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        name, case = row['function'], row['case']
        if name not in EXPECTED or case in seen[name]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (name, case))
        seen[name].add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (name, case, {k: side.get(k) for k in GATES}))
            if side['observed'].get('trace_dropped'):
                raise RuntimeError('Traza desbordada en %s %d' % (name, case))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (name, case, row['fp_difference']))
        want, stats = oracle.expected(name, row['input'], case)      # oracle.Fail = fixture invalido (depende de comportamiento indefinido)
        for label, side in (('EXE', a), ('DLL', c)):
            diff = oracle.compare(side['observed'], want)
            if diff:
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), claves %s:\n  obtenido %s\n  esperado %s' % (
                    name, case, label, diff, {k: str(side['observed'].get(k))[:300] for k in diff}, {k: str(want[k])[:300] for k in diff}))
        for key, value in stats.items():
            if value is True:
                cov[name][key] = cov[name].get(key, 0) + 1
        cov[name]['residue_differs'] = cov[name].get('residue_differs', 0) + (row['original_residue'] != row['candidate_residue'])
    for name in NAMES:
        if seen[name] != set(range(EXPECTED[name])):
            raise RuntimeError('Cobertura incompleta en %s' % name)
    shortfalls = {n: {k: (cov[n].get(k, 0), m) for k, m in MINIMUMS[n].items() if cov[n].get(k, 0) < m} for n in NAMES}
    shortfalls = {n: v for n, v in shortfalls.items() if v}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('statswrappers60', manifest['binaries']['asi'], analyze, 'StatsWrappers60', timeout=3600))
