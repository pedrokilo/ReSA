"""Compuertas del banco HeatSeek72 (lote 72): CVehicle::ScanAndMarkTargetForHeatSeekingMissile (EXE 0x6E0400, hook S), 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

NAMES = ['CVehicle::ScanAndMarkTargetForHeatSeekingMissile']
CASES = 20480   # 16384 (familias 0..15) + 4096 (familias 16..19: AL de la vision distinto de 0/1 y CPlane real)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6E0400}
# (direccion del EXE, hooked): PickTargetForHeatSeekingMissile, GetIsLineOfSightClear, MarkTarget, ClearCrossHairImmediately (hooked=False: el hook queda apagado, la entrada del EXE es codigo original)
STUBBED = {NAMES[0]: [(0x73F910, False), (0x56A490, False), (0x742BF0, False), (0x742CA0, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6E0400}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = [e[0] for e in events]
    plane = inp['sub_type'] == 4
    los = 2 in k
    mark = 4 in k
    clear = 3 in k

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('target_null', inp['tgt_kind'] == 0)
    add('target_null_plane', inp['tgt_kind'] == 0 and plane)
    add('target_null_other', inp['tgt_kind'] == 0 and not plane)
    add('target_entity', inp['tgt_kind'] == 1)
    add('target_self', inp['tgt_kind'] == 2)
    add('target_compact_pos', inp['tgt_kind'] == 1 and inp['tgt_mat'] == 0)
    add('plane', plane)
    add('plane_low_zero', plane and (inp['pstate'] & 0x7FFFFFFF) == 0)
    add('plane_low_nonzero', plane and (inp['pstate'] & 0x7FFFFFFF) != 0)
    add('los_called', los)
    add('los_called_plane', los and plane)
    add('los_called_other', los and not plane)
    add('los_called_slack_wrap', los and plane and (inp['pstate'] & 0x7FFFFFFF) == 0 and inp['time'] < 1)
    add('los_skipped_target', inp['tgt_kind'] != 0 and not los)
    add('los_skipped_plane_bit_set', inp['tgt_kind'] != 0 and not los and plane and (inp['pstate'] >> 31) == 1)
    add('los_skipped_plane_bit_clear', inp['tgt_kind'] != 0 and not los and plane and (inp['pstate'] >> 31) == 0)
    add('los_skipped_other', inp['tgt_kind'] != 0 and not los and not plane)
    al = inp['los_al'] & 0xFF   # el EXE decide con `test al,al`: visible = AL != 0
    add('los_visible', los and al != 0)
    add('los_blocked', los and al == 0)
    add('los_visible_plane', los and plane and al != 0)
    add('los_blocked_plane', los and plane and al == 0)
    # AL distinto de 0/1 devuelto por GetIsLineOfSightClear (el EXE guarda solo el bit 0 en el estado del avion y decide con AL != 0)
    add('los_al_gt1', los and al > 1)
    add('los_al_2', los and al == 2)
    add('los_al_0x80', los and al == 0x80)
    add('los_al_0xff', los and al == 0xFF)
    add('los_al_even_visible', los and al > 1 and al & 1 == 0)
    add('los_al_odd_gt1', los and al > 1 and al & 1 == 1)
    add('los_al_gt1_plane', los and plane and al > 1)
    add('los_al_even_visible_plane', los and plane and al > 1 and al & 1 == 0)
    add('los_al_odd_gt1_plane', los and plane and al > 1 and al & 1 == 1)
    add('los_al_gt1_hi_garbage', los and al > 1 and inp['los_hi'] != 0)
    add('los_al_gt1_mark', los and al > 1 and mark)
    add('los_al_gt1_clear_status', los and al > 1 and clear and obs['eax'] != 0)
    add('los_al_even_plane_then_hidden_state', los and plane and al > 1 and al & 1 == 0 and inp['time'] > 1)
    # CPlane real (estado +0x9E4 y subtipo por los miembros reales)
    add('real_plane', inp['real'] == 1)
    add('real_plane_los_called', inp['real'] == 1 and los)
    add('real_plane_los_skipped_bit_set', inp['real'] == 1 and inp['tgt_kind'] != 0 and not los and (inp['pstate'] >> 31) == 1)
    add('real_plane_los_skipped_bit_clear', inp['real'] == 1 and inp['tgt_kind'] != 0 and not los and (inp['pstate'] >> 31) == 0)
    add('real_plane_low_zero', inp['real'] == 1 and (inp['pstate'] & 0x7FFFFFFF) == 0)
    add('real_plane_mark', inp['real'] == 1 and mark)
    add('real_plane_state_written', inp['real'] == 1 and los and obs['plane_state'] != inp['pstate'])
    add('real_plane_al_gt1', inp['real'] == 1 and los and al > 1)
    add('real_plane_target_null', inp['real'] == 1 and inp['tgt_kind'] == 0)
    add('real_plane_target_self', inp['real'] == 1 and inp['tgt_kind'] == 2)
    add('los_time_gt_1', los and plane and inp['time'] > 1)
    add('los_time_le_1', los and plane and inp['time'] <= 1)
    add('los_flags_mutated', los and (inp['mut_self'] != 0 or inp['mut_tgt'] != 0))
    add('los_mut_bit0', los and ((inp['mut_self'] | inp['mut_tgt']) & 1) == 1)
    add('los_hi_garbage', los and inp['los_hi'] != 0)
    add('mark', mark)
    add('mark_after_los', mark and los)
    add('mark_without_los', mark and not los)
    add('mark_plane', mark and plane)
    add('clear', clear)
    add('clear_hidden', clear and obs['eax'] == 0 and inp['tgt_kind'] != 0)
    add('clear_status', clear and obs['eax'] != 0)
    add('status_nonplayer', (inp['status'] & 0xF8) != 0)
    add('status_nonplayer_visible_target', (inp['status'] & 0xF8) != 0 and clear and obs['eax'] != 0)
    add('ent_null', inp['ent_kind'] == 0)
    add('ent_target', inp['ent_kind'] == 1)
    add('ent_other', inp['ent_kind'] == 2)
    add('ent_raw', inp['ent_kind'] == 3)
    add('time_edge_1000', inp['time'] in (999, 1000, 1001, 1002, 1003))
    add('time_wrap', inp['time'] < 2)
    add('special_float', any(is_special(inp[a]) for a in ('vpos_x', 'vpos_y', 'vpos_z', 'vdir_x', 'vdir_y', 'vdir_z', 'tpos_x', 'tpos_y', 'tpos_z')))
    add('sub_other_odd', inp['sub_type'] in (0x80000004, 0x104, 0x40000004))
    add('ret_nonnull', obs['eax'] != 0)
    add('ret_null', obs['eax'] == 0)


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
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # los cuatro colaboradores tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
        if any(s['dll'] is not True for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 20 or short:
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
        sys.exit(runtime_runner.run_case('heatseek72', manifest['binaries']['asi'], analyze, 'HeatSeek72', timeout=3600))
