"""Compuertas independientes del banco Dependencies24B: WinInput::diPadInit (0x7485C0), 2048 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaboradores sustituidos en ambos lados (EntryPatch del
falso de 0x7477C0 y de 0x530B00 en el EXE y en la DLL), todas las compuertas de ABI/ruta/x87, la entrada de cada fila igual a la fixture del oraculo, la salida de CADA
lado (tabla de pads final, punteros de dispositivo finales y traza completa de colaboradores, con el hash de la tabla en cada llamada y la cabecera del DIPROPDWORD que
ve cada GetProperty) igual a la que modela fixture_oracle.py desde el desensamblado, ninguna trampa de vtable, la cobertura de las fixtures de estres y la restauracion
final (hook, tabla de pads, punteros de dispositivo, x87). No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true. EAX/ECX/EDX al volver solo se
registran: ningun llamador del EXE los lee (0x748813 los sobrescribe; ver classification en g24b-classification.md).
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

FN = 'WinInput::diPadInit'
EXPECTED = {FN: oracle.CASES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'route_ok', 'traps_zero', 'globals_restored', 'floating_state_restored')


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado o sin colaboradores sustituidos')
    if begin[0].get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if begin[0]['pairs'] != oracle.CASES or begin[0]['cases'] != {'diPadInit': oracle.CASES}:
        raise RuntimeError('Numero de pares inesperado')
    if not all(begin[0]['bodies'].values()):
        raise RuntimeError('Cuerpo RH sin resolver')
    seen = set()
    cov = dict(mode=[0] * 6, mutator=[0] * 8, table_kind=[0] * 5, prop_mode=[0] * 4, hr_caps={}, hr_prop={}, garbage1=0, garbage2=0, dev2_header_residue=0, mutated=0,
               dev1_present=0, dev2_present=0, both=0, init_calls=0, stack_seed_special=0, residue_eax_differs=0, residue_regs_differ=0)
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != FN or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %d' % case)
        seen.add(case)
        a, b = row['original'], row['candidate']
        for side in (a, b):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        f = oracle.make(case)
        want_in = oracle.input_row(f)
        want_out, stats = oracle.observed_row(f)
        for side in (a, b):
            if side['input'] != want_in:
                raise RuntimeError('Fixture distinta de la del oraculo en %d' % case)
            if side['observed'] != want_out:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, 'EXE' if side is a else 'DLL', side['observed'], want_out))
        cov['mode'][f['dev_mode']] += 1
        cov['mutator'][f['mutator']] += 1
        cov['table_kind'][f['table_kind']] += 1
        for i in range(2):
            cov['prop_mode'][f['prop_mode'][i]] += 1
            cov['hr_caps'][f['caps_hr'][i]] = cov['hr_caps'].get(f['caps_hr'][i], 0) + 1
            cov['hr_prop'][f['prop_hr'][i]] = cov['hr_prop'].get(f['prop_hr'][i], 0) + 1
        for k in ('garbage1', 'garbage2', 'dev2_header_residue', 'mutated', 'init_calls'):
            cov[k] += stats[k]
        d1, d2 = want_out['dev1'], want_out['dev2']
        cov['dev1_present'] += d1 != 0
        cov['dev2_present'] += d2 != 0
        cov['both'] += d1 != 0 and d2 != 0
        cov['stack_seed_special'] += case < len(oracle.SEEDS)
        ro, rc = row['original_residue'], row['candidate_residue']
        cov['residue_regs_differ'] += ro != rc
        cov['residue_eax_differs'] += ro['eax'] != rc['eax']   # informativo: ningun llamador del EXE lee EAX/ECX/EDX
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    minimum = dict(garbage1=150, garbage2=150, dev2_header_residue=250, mutated=300, init_calls=1000, dev1_present=1000, dev2_present=1000, both=500)
    short = {k: (cov[k], v) for k, v in minimum.items() if cov[k] < v}
    short.update({'mode %d' % i: (n, 300) for i, n in enumerate(cov['mode']) if n < 300})
    short.update({'mutator %d' % i: (n, 250) for i, n in enumerate(cov['mutator']) if n < 250})
    short.update({'table_kind %d' % i: (n, 300) for i, n in enumerate(cov['table_kind']) if n < 300})
    short.update({'prop_mode %d' % i: (n, 100) for i, n in enumerate(cov['prop_mode']) if n < 100})
    short.update({'hr_caps %#x' % h: (cov['hr_caps'].get(h, 0), 60) for h in oracle.HR if cov['hr_caps'].get(h, 0) < 60})
    short.update({'hr_prop %#x' % h: (cov['hr_prop'].get(h, 0), 60) for h in oracle.HR if cov['hr_prop'].get(h, 0) < 60})
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != oracle.CASES or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['table'] != begin[0]['initial_table'] or state[0]['dev1'] != begin[0]['initial_dev1'] or state[0]['dev2'] != begin[0]['initial_dev2']:
        raise RuntimeError('Tabla de pads o punteros de DirectInput no restaurados')
    cov['hr_caps'] = {hex(k): v for k, v in cov['hr_caps'].items()}
    cov['hr_prop'] = {hex(k): v for k, v in cov['hr_prop'].items()}
    summary['coverage'] = cov
    summary['compare_dead_x87_registers'] = True
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies24b', manifest['binaries']['asi'], analyze, 'Dependencies24B', timeout=1800))
