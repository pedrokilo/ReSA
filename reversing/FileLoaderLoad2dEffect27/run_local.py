"""Compuertas independientes del banco FileLoaderLoad2dEffect27 (CFileLoader::Load2dEffect 0x5B7670), 6144 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados
(route_ok incluye que sscanf 0x8220AD, _ftol2 0x821B40 y strncpy 0x821F40 del EXE no esten enganchados), fp_difference vacio, la salida de CADA lado (bloque de 0x40 bytes y las dos
entradas siguientes, contador, nada fuera de las tres entradas, traza Push/Find/Set/Store/Add2dEffect/RwTextureRead/Malloc/Pop, nombres, bloque de Malloc, tabla de modelos y linea sin
tocar) igual a la que modela fixture_oracle.py desde el desensamblado (con comodines SOLO donde el valor depende del CRT: ver README.md), la cobertura de las fixtures y la restauracion
final (hook, entradas de codigo vigiladas, x87).

Si hay diferencias EXE/DLL, ANTES de abortar se escribe un desglose por grupo / familia / clase de CRT (diferencias_crt.json junto a comparisons.jsonl, y en el mensaje de error), para
distinguir una diferencia real de logica de una diferencia entre el sscanf del EXE y el de la DLL. Nada se oculta: el banco falla igualmente.
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

NAMES = oracle.NAMES
EXPECTED = dict(oracle.CASES)
NAME = oracle.NAME
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x5B7670}
WATCHED = {0x5B7670, 0x8220AD, 0x821B40, 0x821F40, 0x4C5A60, 0x4C4D20, 0x7F3AC0, 0x72F420, 0x7316A0, 0x731850, 0x7319C0, 0x7316B0}
STUBBED = {0x4C5A60, 0x4C4D20, 0x7316A0, 0x731850, 0x7319C0, 0x7316B0, 0x7F3AC0, 0x72F420}

# Minimos de cobertura. Calibrados con fixture_mirror.py (espejo en Python del generador) dejando ~40-50 % de margen: el generador C++ reparte las mismas familias pero el orden
# exacto del RNG puede diferir. NO se comprobaron contra el generador C++ (el banco no se compilo): si alguno falla por poco, bajar el umbral o ajustar la familia en Fixtures.hpp.
MINIMUMS = {
    'type_oob': 300, 'type_ge256': 120, 'type_neg': 50, 'type_byte_differs': 150, 'type_lowbyte_valid': 80, 'type_sens': 30,
    'crt_sens_float': 150, 'crt_sens_int': 200, 'crt_unknown': 20, 'crt_plain_fail': 60, 'crt_wild_bytes': 400, 'crt_sens_value': 300,
    'attr_name_ge8': 130, 'attr_name_ge31': 25, 'attr_name_ge64': 7,
    'int_name_ge16': 45, 'int_name_ge32': 5, 'int_names0': 15, 'int_names1': 15, 'int_names2': 15, 'int_names3': 15, 'int_names4': 180,
    'light_adjacent': 16, 'light_empty_name': 40, 'light_flag4': 170, 'light_long_name': 35, 'light_noflag4_hibits': 2, 'light_tail_corona': 16, 'light_tail_shadow': 32,
    'part_len_1_7': 80, 'part_len_8_47': 110, 'part_len_48p': 35, 'part_len_63p': 20, 'part_missing': 18, 'part_name': 230,
    'road_default13': 50, 'road_full': 160, 'road_masked': 48, 'road_size_frac_edge': 25, 'road_size_ge256': 90, 'road_size_huge': 8, 'road_size_neg': 24,
    'road_trunc_early': 50, 'road_truncated': 90,
    'slot_name_ge31': 23, 'slot_name_ge8': 125, 'slot_word_hi': 130,
    't10_false': 25, 't10_true': 200,
    'gen_halfway': 150, 'gen_int_overflow': 160, 'gen_exotic_float': 32, 'gen_exotic_int': 32, 'gen_type_overflow': 36, 'gen_masked': 48, 'gen_fail_tail': 32,
    'mask0x12': 32, 'mask0x13': 32, 'mask0x14': 32, 'mask0x15': 48, 'mask0x2e': 48,
    'mi_null': 250, 'id_0': 150, 'id_19999': 150, 'pre_all_zero': 200, 'pre_all_ff': 200,
}
for _k in range(12):
    MINIMUMS['g%d' % _k] = 500
for _k in range(16):
    MINIMUMS['fam%d' % _k] = 350
for _k in range(11):
    MINIMUMS['branch%d' % _k] = 500
MINIMUMS['branch-1'] = 400
for _k in range(13):
    MINIMUMS['k%d' % _k] = 300


def crt_class(stats):
    if stats is None:
        return 'fixture_invalido'
    parts = [k for k in ('crt_unknown', 'crt_sens_float', 'crt_sens_int', 'crt_plain_fail') if stats.get(k)]
    return '+'.join(parts) if parts else 'solo_texto_comun'


def breakdown(rows):
    bad = [r for r in rows if r.get('event') == 'comparison' and not r.get('equal')]
    out = {'pares_distintos': len(bad), 'por_grupo': {}, 'por_familia': {}, 'por_clase_crt': {}, 'por_clave': {}, 'ejemplos': []}
    for r in bad:
        inp, a, c = r['input'], r['original'], r['candidate']
        try:
            _, stats = oracle.expected(NAME, inp, r['case'])
        except RuntimeError:
            stats = None
        cls = crt_class(stats)
        keys = sorted(set(k for k in set(a) | set(c) if a.get(k) != c.get(k)))
        okeys = sorted(k for k in set(a.get('observed', {})) | set(c.get('observed', {})) if a.get('observed', {}).get(k) != c.get('observed', {}).get(k))
        out['por_grupo'][str(inp['group'])] = out['por_grupo'].get(str(inp['group']), 0) + 1
        out['por_familia']['g%d/f%d' % (inp['group'], inp['family'])] = out['por_familia'].get('g%d/f%d' % (inp['group'], inp['family']), 0) + 1
        out['por_clase_crt'][cls] = out['por_clase_crt'].get(cls, 0) + 1
        for k in okeys or keys:
            out['por_clave'][k] = out['por_clave'].get(k, 0) + 1
        if len(out['ejemplos']) < 12:
            ex = {'pair': r['pair'], 'grupo': inp['group'], 'familia': inp['family'], 'clase_crt': cls, 'linea': inp['line'][:160], 'claves': okeys or keys}
            if okeys:
                k = okeys[0]
                ex['original'] = str(a['observed'].get(k))[:200]
                ex['candidato'] = str(c['observed'].get(k))[:200]
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
        (path.parent / 'diferencias_crt.json').write_text(json.dumps(info, indent=1), encoding='utf-8')
        raise RuntimeError('Diferencias EXE/DLL (desglose en diferencias_crt.json): ' + json.dumps({k: info[k] for k in ('pares_distintos', 'por_grupo', 'por_clase_crt', 'por_clave')}))
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin la marca de colaboradores sustituidos (este banco sustituye 8 colaboradores y ejecuta sscanf/_ftol2/strncpy reales)')
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
    if set(col['exe_real']) != {0x8220AD, 0x821B40, 0x821F40} or {p['exe'] for p in col['stubbed']} != STUBBED or any(not p['dll'] for p in col['stubbed']):
        raise RuntimeError('Lista de colaboradores inesperada o cuerpo de la DLL sin resolver: %s' % json.dumps(col))
    crt = b['crt']
    if crt.get('exe_sscanf') != 0x8220AD or crt.get('exe_sscanf_in_game_image') is not True or not crt['probe']['rows']:
        raise RuntimeError('Sondeo del CRT ausente o el sscanf del EXE no esta en la imagen del juego')
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
        want, stats = oracle.expected(NAME, inp, case)      # RuntimeError = fixture invalido (depende de comportamiento indefinido)
        for label, side in (('EXE', a), ('DLL', c)):
            diff = oracle.compare(side['observed'], want)
            if diff:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s), claves %s, clase CRT %s, linea %r:\n  obtenido %s\n  esperado %s' % (
                    case, label, diff, crt_class(stats), inp['line'][:160], {k: side['observed'].get(k) for k in diff}, {k: want[k] for k in diff}))
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
        for bit, nm in ((1, 'gen_halfway'), (2, 'gen_int_overflow'), (4, 'gen_exotic_float'), (8, 'gen_exotic_int'), (16, 'gen_type_overflow'), (32, 'gen_masked'), (64, 'gen_fail_tail')):
            if inp['crt'] & bit:
                cov[nm] = cov.get(nm, 0) + 1
        for bit in range(64):
            if (inp['mask'] >> bit) & 1:
                cov['mask%#x' % bit] = cov.get('mask%#x' % bit, 0) + 1
        cov['k%d' % inp['store_k']] = cov.get('k%d' % inp['store_k'], 0) + 1
        cov['mi_null'] = cov.get('mi_null', 0) + (inp['mi_label'] == 15)
        cov['id_0'] = cov.get('id_0', 0) + (inp['model_id'] == 0)
        cov['id_19999'] = cov.get('id_19999', 0) + (inp['model_id'] == 19999)
        pre = bytes.fromhex(inp['pre_entry'])
        cov['pre_all_zero'] = cov.get('pre_all_zero', 0) + (pre == bytes(64))
        cov['pre_all_ff'] = cov.get('pre_all_ff', 0) + (pre == b'\xff' * 64)
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
    summary['crt_probe_differences'] = crt['probe']['differences']
    summary['crt_probe_rows'] = len(crt['probe']['rows'])
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fileloader2dfx27', manifest['binaries']['asi'], analyze, 'FileLoaderLoad2dEffect27', timeout=1800))
