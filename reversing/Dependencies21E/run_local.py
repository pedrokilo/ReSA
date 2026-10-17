"""Compuertas independientes de Dependencies21E: rutas, ABI, x87, fixtures, oraculos exactos y cobertura de 0x610DB0, 0x72D6D0 y 0x63A380."""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixtures as X

F1 = 'CPopulation::RandomIntInLimitRange'
F2 = 'CZoneDef::GetCenter'
F3 = 'CTaskComplexEnterCar::GetCameraStickModifier'
EXPECTED = {F1: X.CASES1, F2: X.CASES2, F3: X.CASES3}
GATES = ('route_ok', 'collaborators_ok', 'stack_restored', 'nonvolatile_preserved', 'seh_restored', 'fixture_contract', 'memory_restored', 'floating_state_restored')
M32 = 0xFFFFFFFF
PADS = 0xB73458


def oracle1(f):
    """Valor exacto de EAX: lower + trunc(float24(random16 * 2^-15 * (upper - lower))); el producto es exacto en double y se redondea una sola vez a 24 bits."""
    rng = X.s32(f['upper'] - f['lower'])
    value = X.f32(((f['random'] & 0xFFFF) * rng) / 32768.0)
    return (f['lower'] + int(value)) & M32


def oracle2(f):
    """Bytes exactos de la salida: todos los pasos son enteros de 16 bits y mitades, exactos en float."""
    z = f['zone']
    x = (z[4] + z[2]) * 0.5 + z[0]
    y = (z[5] + z[3]) * 0.5 + z[1]
    zz = (z[6] + z[7]) * 0.5
    return struct.pack('<fff', x, y, zz).hex()


def oracle3_flow(f):
    """(traza esperada de colaboradores, eleccion final, llega a la aritmetica) por la logica entera del EXE (0x63A380..0x63A4C8)."""
    if f['car_null'] or f['car_subtype'] == 3 or f['sub_null']:
        return [], f['choice0'], False
    trace = []
    kinds = (802, 802, 804, 820, 807, 808)
    matched = False
    for kind in kinds:
        trace.append(['task_type', 'sub', 'null'])
        if f['sub_kind'] == kind:
            matched = True
            break
    if not matched:
        return trace, f['choice0'], False
    pad = 'pad1' if f['ped_type'] == 1 else 'pad0'
    choice = f['choice0']
    trace.append(['aim_left_right', pad, 'ped'])
    if abs(f['aim_lr']) > 20:
        choice = 3
    else:
        trace.append(['aim_up_down', pad, 'ped'])
        if abs(f['aim_ud']) > 20:
            choice = 3
        else:
            trace.append(['accelerate', pad, 'null'])
            if f['accel']:
                choice = 2
            else:
                trace.append(['brake', pad, 'null'])
                if f['brake']:
                    choice = 1
    if choice == 3:
        return trace, 3, False
    trace.append(['col_model', 'car', 'null'])
    return trace, choice, True


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1:
        raise RuntimeError('Falta el evento begin')
    b = begin[0]
    if not (b.get('registered_hook') is True and b.get('ready') is True and b.get('routes_ready') is True and b.get('collaborators_stubbed') is True):
        raise RuntimeError('Inicio sin hooks registrados, rutas demostradas o colaboradores falsos')
    if b.get('pairs') != sum(EXPECTED.values()) or b.get('cases') != EXPECTED or b.get('functions') != 3:
        raise RuntimeError('Numero de casos inesperado')
    if b.get('compare_dead_x87_registers') is not True:
        raise RuntimeError('La igualdad de los registros x87 muertos no esta activa')
    if b['asi_module'] == b['original_module'] or not b['asi_module']:
        raise RuntimeError('Modulo de la DLL no distinguido del EXE')
    for name in ('aim_left_right', 'aim_up_down', 'accelerate', 'brake', 'col_model'):
        if b['collaborators'][name].get('active') is not True:
            raise RuntimeError('Colaborador %s sin parche doble activo' % name)
    initial_fp = bytes.fromhex(b['initial_fp'])
    fcw = struct.unpack_from('<H', initial_fp, 0)[0]
    pairs, seen_fn = set(), {F1: set(), F2: set(), F3: set()}
    fp_fields = {}
    paths, stress = {}, {'nan': set(), 'snan': 0, 'denormal': 0}
    flow_hits = {'math': 0, 'exit': 0}
    for row in rows:
        if row['event'].startswith('first_chance_av') or row.get('pump_state_intact') is False:
            raise RuntimeError('Acceso invalido o estado alterado durante el banco')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in EXPECTED or case in seen_fn[fn] or row['pair'] in pairs:
            raise RuntimeError('Par repetido o funcion inesperada')
        seen_fn[fn].add(case)
        pairs.add(row['pair'])
        base_top = None
        for side in ('original', 'candidate'):
            r = row[side]
            if not all(r.get(g) is True for g in GATES):
                raise RuntimeError('Compuerta fallida en %s %d (%s)' % (fn, case, side))
            fp = r['fp']
            f_in = r['input']
            fixture = {F1: X.fx1, F2: X.fx2, F3: X.fx3}[fn](case)
            if f_in != fixture:
                raise RuntimeError('La fixture registrada no es la del oraculo en %s %d' % (fn, case))
            if len(fp.get('raw_registers', '')) != 160:
                raise RuntimeError('Registros ST crudos ausentes en %s %d (%s)' % (fn, case, side))
            if fp['fcw'] != fcw or fp['ftw'] != 0xFFFF or fp['valid_registers']:
                raise RuntimeError('Pila x87 no vacia o FCW alterada en %s %d (%s)' % (fn, case, side))
            base_top = (X.FP_STATUS[fixture['fp']] >> 11) & 7
            if fp['top'] != base_top:
                raise RuntimeError('TOP descompensado en %s %d (%s)' % (fn, case, side))
            if fn == F1:
                if r['eax'] != oracle1(fixture) or r['rand_calls'] != 1:
                    raise RuntimeError('EAX/RNG distinto del oraculo en %s %d (%s): %#x' % (fn, case, side, r['eax']))
            elif fn == F2:
                if r['eax_is_out'] is not True or r['out'] != oracle2(fixture):
                    raise RuntimeError('Salida distinta del oraculo en %s %d (%s)' % (fn, case, side))
            else:
                trace, choice, math = oracle3_flow(fixture)
                if r['trace'] != trace or r['choice'] != choice:
                    raise RuntimeError('Traza/eleccion distinta del oraculo en %s %d (%s): %s %d' % (fn, case, side, r['trace'], r['choice']))
                if r['vertical'] != fixture['vertical'] or r['horizontal'] != fixture['horizontal']:
                    raise RuntimeError('Vertical/horizontal alterados en %d (%s)' % (case, side))
                if not math and (r['alpha_stick'] != fixture['alpha'] or r['beta_stick'] != fixture['beta']):
                    raise RuntimeError('Salida modificada en una ruta sin aritmetica en %d (%s)' % (case, side))
                if r['camera_limit'] != fixture['limit']:
                    raise RuntimeError('Limite de camara alterado en %d (%s)' % (case, side))
        if fn == F3:
            flow_hits['math' if oracle3_flow(X.fx3(case))[2] else 'exit'] += 1
            for tag in X.paths3(X.fx3(case)):
                paths[tag] = paths.get(tag, 0) + 1
        for field in row.get('fp_difference', []):
            fp_fields[field] = fp_fields.get(field, 0) + 1
        for d in ('fcw', 'fsw', 'ftw', 'dead_registers'):
            if d in row.get('fp_difference', []):
                raise RuntimeError('Estado x87 distinto (%s) en %s %d' % (d, fn, case))
    for fn, cases in seen_fn.items():
        if cases != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta de %s' % fn)
    missing = X.REQUIRED_PATHS3 - set(paths)
    if missing:
        raise RuntimeError('Faltan rutas de 0x63A380: ' + ', '.join(sorted(missing)))
    if flow_hits['math'] < 800 or flow_hits['exit'] < 800:
        raise RuntimeError('Reparto insuficiente entre rutas con y sin aritmetica: %s' % flow_hits)
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != sum(EXPECTED.values()) or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['paths_0x63A380'] = {k: paths[k] for k in sorted(paths)}
    summary['fp_informational_differences'] = fp_fields
    summary['oracles'] = 'EAX exacto (0x610DB0), bytes de salida exactos (0x72D6D0), traza/eleccion/escrituras (0x63A380); aritmetica de 0x63A380 decidida por la igualdad EXE/DLL'
    summary['compare_dead_x87_registers'] = True
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependencies21e', manifest['binaries']['asi'], analyze, 'Dependencies21E', timeout=1800))
