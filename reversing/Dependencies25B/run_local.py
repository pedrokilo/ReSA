"""Compuertas independientes del banco Dependencies25B: 20 funciones de biblioteca de RenderWare (rwcore.cpp / rpanisot.cpp / rpworld.cpp), 1024 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI de CADA funcion, colaboradores sustituidos en ambos lados, todas las
compuertas de ABI/x87/ruta, `fp_difference` vacio, el numero y el orden de los casos, retorno / traza de colaboradores / hash del arena final de CADA lado iguales a los de la
simulacion independiente (fixture_oracle.py, a partir de la entrada completa registrada en la fila), cobertura minima de cada clase de estres y restauracion final (globales
de plugin, entradas parcheadas, hooks y x87). No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza un registro (por ejemplo el que escribe offline/obj/g25b.exe).
"""
import json
import sys
from collections import Counter
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

EXPECTED = {name: oracle.CASES for name in oracle.NAMES}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored')
M32 = 0xFFFFFFFF


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    if begin[0].get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if begin[0]['cases'] != oracle.CASES or begin[0]['pairs'] != sum(EXPECTED.values()) or begin[0]['functions'] != len(oracle.NAMES):
        raise RuntimeError('Numero de casos o de pares inesperado')
    if set(begin[0]['bodies']) != set(oracle.NAMES) or not all(begin[0]['bodies'].values()):
        raise RuntimeError('Cuerpo RH sin resolver')
    if 'registered' in begin[0] and not all(begin[0]['registered'].values()):
        raise RuntimeError('Hook RH no registrado: ' + json.dumps(begin[0]['registered']))
    seen = {name: set() for name in oracle.NAMES}
    cov = {name: Counter() for name in oracle.NAMES}
    distinct = {name: {'ret': set(), 'glob': set(), 'args0': set(), 'flags': set()} for name in oracle.NAMES}
    residue_differs = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        fid = oracle.NAMES.index(fn)
        if case in seen[fn]:
            raise RuntimeError('Caso repetido %s %d' % (fn, case))
        seen[fn].add(case)
        for side in ('original', 'candidate'):
            result = row[side]
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o compuerta fallida en %s %d (%s)' % (fn, case, side))
        a, b = row['original'], row['candidate']
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if row['original_residue'] != row['candidate_residue']:
            residue_differs += 1   # informativo: ningun llamador del EXE lee ECX/EDX tras estas llamadas (ver g25b-classification.md)
        inp = row['input']
        if inp['fid'] != fid or inp['case'] != case or len(inp['args']) != inp['nargs']:
            raise RuntimeError('Entrada inconsistente en %s %d' % (fn, case))
        ret, trace, digest = oracle.simulate(fid, inp)
        want_ret = ret if ret is None or ret is oracle.ANY else ret & oracle.expected_ret_mask(fid)
        for name, side in (('original', a), ('candidate', b)):
            if want_ret is not oracle.ANY and side['ret'] != want_ret:
                raise RuntimeError('Retorno de %s distinto del oraculo en %s %d: %s / %s' % (name, fn, case, side['ret'], want_ret))
            if side['trace'] != [t & M32 for t in trace]:
                raise RuntimeError('Traza de colaboradores de %s distinta del oraculo en %s %d: %s / %s' % (name, fn, case, side['trace'], trace))
            if side['state'] != digest:
                raise RuntimeError('Arena final de %s distinto del oraculo en %s %d' % (name, fn, case))
        # ---- cobertura (calculada con la entrada registrada, no con las formulas de las fixtures)
        c, args, glob = cov[fn], inp['args'], inp['glob']
        d = distinct[fn]
        if glob is not None:
            d['glob'].add(glob)
        if args:
            d['args0'].add(args[0])
        if fid in (oracle.IDX_BUF, oracle.ALWAYS, oracle.RIGHTS, oracle.PLUGOFF, oracle.WREG, oracle.WSTREAM, oracle.LREG):
            d['ret'].add(inp['stub_ret'])
        if fid == oracle.IDX_BUF:
            c['ret_ok' if (inp['stub_ret'] & 0x80000000) == 0 else 'ret_fail'] += 1
            c['pp_null' if args[1] == 0 else 'pp_set'] += 1
        elif fid == oracle.MAXSUP:
            c['byte_negative' if a['ret'] >= 0x80 else 'byte_positive'] += 1
            d['args0'].add(a['ret'])
        elif fid in (oracle.ANSET, oracle.ANGET):
            c['tex_null' if args[0] == 0 else 'tex_set'] += 1
            if glob >= 0x7FFFFFFF or glob == 0x80000000 or glob >= 0xFFFFFFF0:
                c['glob_extreme'] += 1
            if fid == oracle.ANSET:
                c['val_high_garbage'] += (args[1] >> 8) != 0
                d['flags'].add(args[1] & 0xFF)
            else:
                d['flags'].add(a['ret'])
        elif fid == oracle.CLUMP:
            c['n%d' % a['ret']] += 1
        elif fid == oracle.TRIMAT:
            ident = int.from_bytes(bytes.fromhex(inp['init'])[16 + 0x86:16 + 0x88], 'little')
            c['no_material' if ident == 0xFFFF else 'high_id' if ident >= 0x8000 else 'low_id'] += 1
        elif fid == oracle.MATINIT:
            pass
        elif fid == oracle.MATGET:
            c['index_wraps'] += args[1] >= 0x20000000 and args[1] < 0xFFFF0000
            c['index_negative'] += args[1] >= 0xFFFF0000
        elif fid == oracle.MATFIND:
            n = int.from_bytes(bytes.fromhex(inp['init'])[16 + 0x24:16 + 0x28], 'little')
            sn = n - (1 << 32) if n & 0x80000000 else n
            c['empty_or_negative' if sn <= 0 else 'found' if a['ret'] != M32 else 'absent'] += 1
        elif fid == oracle.VDECL:
            t = args[0]
            c['in_table' if ret is not oracle.ANY else 'other'] += 1
            c['wrapped'] += (t >> 30) != 0 and (t >> 30) != 3 or ((t >> 30) == 3 and t < 0xFFFF0000)
            c['negative'] += t >= 0xFFFF0000
        elif fid == oracle.SECSET:
            d['flags'].add(args[1])
    for fn in EXPECTED:
        if seen[fn] != set(range(oracle.CASES)):
            raise RuntimeError('Cobertura incompleta de ' + fn)
    N = oracle
    minimum = {
        N.NAMES[N.IDX_BUF]: {'ret_ok': 300, 'ret_fail': 300, 'pp_null': 200, 'pp_set': 500},
        N.NAMES[N.MAXSUP]: {'byte_negative': 400, 'byte_positive': 400},
        N.NAMES[N.ANSET]: {'tex_null': 400, 'tex_set': 400, 'glob_extreme': 150, 'val_high_garbage': 900},
        N.NAMES[N.ANGET]: {'tex_null': 400, 'tex_set': 400, 'glob_extreme': 150},
        N.NAMES[N.CLUMP]: {'n%d' % k: 60 for k in range(13)},
        N.NAMES[N.TRIMAT]: {'no_material': 30, 'high_id': 60, 'low_id': 700},
        N.NAMES[N.MATGET]: {'index_wraps': 300, 'index_negative': 100},
        N.NAMES[N.MATFIND]: {'found': 200, 'absent': 100, 'empty_or_negative': 250},
        N.NAMES[N.VDECL]: {'in_table': 60, 'wrapped': 250, 'negative': 100},
    }
    short = {}
    for fn, wants in minimum.items():
        for key, want in wants.items():
            if cov[fn][key] < want:
                short[fn + ':' + key] = (cov[fn][key], want)
    wanted_distinct = {N.NAMES[N.IDX_BUF]: ('ret', 16), N.NAMES[N.ALWAYS]: ('ret', 14), N.NAMES[N.RIGHTS]: ('ret', 14), N.NAMES[N.PLUGOFF]: ('ret', 14), N.NAMES[N.WREG]: ('ret', 14),
                       N.NAMES[N.WSTREAM]: ('ret', 14), N.NAMES[N.LREG]: ('ret', 14), N.NAMES[N.ANSET]: ('glob', 16), N.NAMES[N.ANGET]: ('glob', 16), N.NAMES[N.ATOMWORLD]: ('glob', 16),
                       N.NAMES[N.GEOUSE]: ('glob', 16), N.NAMES[N.SECSET]: ('glob', 16), N.NAMES[N.SECGET]: ('glob', 16), N.NAMES[N.MATINIT]: ('args0', 60), N.NAMES[N.MAXSUP]: ('args0', 256),
                       N.NAMES[N.ANSET] + '/flags': ('flags', 32), N.NAMES[N.SECSET] + '/flags': ('flags', 30), N.NAMES[N.WREG]: ('ret', 14)}
    for key, (field, want) in wanted_distinct.items():
        fn = key.split('/')[0]
        if len(distinct[fn][field]) < want:
            short[key + ':' + field] = (len(distinct[fn][field]), want)
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != sum(EXPECTED.values()) or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    gfinal = [r for r in rows if r['event'] == 'global_state_final']
    if len(gfinal) != 1 or gfinal[0].get('state_restored') is not True:
        raise RuntimeError('Estado global final no restaurado')
    if 'globals' in gfinal[0] and gfinal[0]['globals'] != begin[0]['initial_globals']:
        raise RuntimeError('Los desplazamientos de plugin globales no volvieron a su valor inicial')
    summary['coverage'] = {fn: dict(c) for fn, c in cov.items() if c}
    summary['compare_dead_x87_registers'] = True
    summary['residue_pairs_where_ecx_edx_differ'] = residue_differs
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies25b', manifest['binaries']['asi'], analyze, 'Dependencies25B', timeout=3600))
