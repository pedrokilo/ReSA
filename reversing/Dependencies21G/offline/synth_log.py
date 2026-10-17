"""Genera un registro sintetico con las mismas claves que LiveComparison.hpp y comprueba que run_local.py lo acepta (y rechaza mutaciones)."""
import json
import shutil
import struct
import sys
from pathlib import Path

W = Path(__file__).resolve().parent
SANDBOX = W / 'sandbox'
if SANDBOX.exists():
    shutil.rmtree(SANDBOX)
(SANDBOX / 'Dependencies21G').mkdir(parents=True)
REV = Path(r'D:\Original\gta-reversed-master\reversing')
for name in ('probe_case.py', 'runtime_runner.py'):
    shutil.copy(REV / name, SANDBOX / name)
for name in ('Fixtures.hpp', 'Arenas.hpp', 'fixture_oracle.py', 'run_local.py'):
    shutil.copy(W.parent / name, SANDBOX / 'Dependencies21G' / name)
sys.path.insert(0, str(SANDBOX))
sys.path.insert(0, str(SANDBOX / 'Dependencies21G'))
import fixture_oracle as o
import run_local as rl

FT, RP, SL = rl.FT, rl.RP, rl.SL
FPHEX = 'ab' * 108
BASE_FT, BASE_RP = 0x0BC10010, 0x0BC20010


def common(out):
    out.update(post_floating_state=FPHEX, post_mxcsr=0x1F80, fp_ip_route_ok=True, stack_restored=True, nonvolatile_preserved=True, route_ok=True,
               globals_restored=True, floating_state_restored=True)
    return out


def guarded(arena):
    return rl.guarded(arena)


def build_rows():
    rows = [{'event': 'waiting_for_player'}]
    rows.append({'event': 'begin', 'registered_hook': True, 'functions': 3, 'pairs': 6656, 'cases': {'GetFirstTexture': 1024, 'Replace': 1536, 'Slerp': 4096},
                 'compare_dead_x87_registers': True, 'collaborators_stubbed': True, 'bodies': {'GetFirstTexture': 1, 'Replace': 2, 'Slerp': 3}, 'asi_module': 1, 'original_module': 2})
    pair = 0
    for c in range(o.FIRST_TEXTURE_CASES):
        f = o.first_texture_fixture(c)
        exp = o.expected_first_texture(f)
        side = common({'input': {k: f[k] for k in ('case', 'count', 'pad', 'order', 'junk', 'prev')}, 'base': BASE_FT, 'state': guarded(o.build_first_texture(BASE_FT, f)),
                       'eax_offset': exp, 'expected_offset': exp})
        rows.append({'event': 'comparison', 'function': FT, 'case': c, 'pair': pair, 'original': side, 'candidate': json.loads(json.dumps(side)), 'fp_difference': [],
                     'original_residue': {'eax': 1, 'ecx': 2, 'edx': 3}, 'candidate_residue': {'eax': 1, 'ecx': 9, 'edx': 8}, 'equal': True, 'accepted': True})
        pair += 1
    rows.append({'event': 'function_summary', 'function': FT, 'cases': 1024, 'failures': 0, 'state_restored': True})
    for c in range(o.REPLACE_CASES):
        f = o.replace_fixture(c)
        arena, rep, trace = o.oracle_replace(BASE_RP, f)
        side = common({'input': {'case': c, 'meshes': f['meshes'], 'materials': f['materials'], 'old': f['old'], 'new': f['new'], 'absent': f['absent'],
                                 'first_mesh_offset': f['first_mesh_offset'], 'serial': f['serial'], 'mesh_mat': f['mesh_mat'], 'list_mat': f['list_mat'], 'ref_counts': f['ref_counts']},
                       'base': BASE_RP, 'state': guarded(arena), 'returned': int(rep), 'trace': trace})
        rows.append({'event': 'comparison', 'function': RP, 'case': c, 'pair': pair, 'original': side, 'candidate': json.loads(json.dumps(side)), 'fp_difference': [],
                     'original_residue': {'eax': 1, 'ecx': 2, 'edx': 3}, 'candidate_residue': {'eax': 1, 'ecx': 2, 'edx': 3}, 'equal': True, 'accepted': True})
        pair += 1
    rows.append({'event': 'function_summary', 'function': RP, 'cases': 1536, 'failures': 0, 'state_restored': True})
    for c in range(o.SLERP_CASES):
        f = o.slerp_fixture(c)
        path = o.slerp_path(c)
        buf, target = rl.slerp_expected_state(f, path == 'copy')
        if path == 'copy':
            buf[target:target + 16] = struct.pack('<4I', *f['dst'])
        else:
            buf[target:target + 16] = bytes([0x11]) * 16   # resultado cualquiera: no hay oraculo de valores
        side = common({'input': {'case': c, 'theta': f['theta'], 't': f['t'], 'inv': f['inv'], 'quat': f['quat'], 'alias': f['alias'], 'from': f['src'], 'to': f['dst']},
                       'state': bytes(buf).hex(), 'path': path})
        rows.append({'event': 'comparison', 'function': SL, 'case': c, 'pair': pair, 'original': side, 'candidate': json.loads(json.dumps(side)), 'fp_difference': [],
                     'original_residue': {'eax': 1, 'ecx': 2, 'edx': 3}, 'candidate_residue': {'eax': 1, 'ecx': 2, 'edx': 3}, 'equal': True, 'accepted': True})
        pair += 1
    rows.append({'event': 'function_summary', 'function': SL, 'cases': 4096, 'failures': 0, 'state_restored': True})
    rows.append({'event': 'global_state_final', 'state_restored': True})
    rows.append({'event': 'complete', 'pairs': pair, 'state_restored': True, 'floating_state_restored': True, 'entries_intact': True})
    return rows


def write(rows, name):
    p = W / name
    p.write_text(''.join(json.dumps(r) + '\n' for r in rows), encoding='utf-8')
    return p


rows = build_rows()
good = write(rows, 'synth_good.jsonl')
print('bueno:', json.dumps(rl.analyze(good))[:300])
# mutaciones: cada una debe ser rechazada
def mutate(fn, label):
    r = json.loads(json.dumps(rows))
    fn(r)
    p = write(r, 'synth_bad.jsonl')
    try:
        rl.analyze(p)
        print('NO RECHAZADA:', label)
    except Exception as e:
        print('rechazada (%s): %s' % (label, str(e)[:100]))

def first(r, f, name):
    return next(x for x in r if x.get('function') == f and x['event'] == 'comparison' and x['case'] == name)

mutate(lambda r: first(r, SL, 5)['candidate'].update(post_floating_state='cd' * 108), 'x87 distinto (original != candidate)')
mutate(lambda r: first(r, RP, 7)['original'].update(returned=1 - first(r, RP, 7)['original']['returned']) or first(r, RP, 7)['candidate'].update(returned=first(r, RP, 7)['original']['returned']), 'retorno distinto del oraculo')
mutate(lambda r: [first(r, FT, 10)[s].update(eax_offset=-1) for s in ('original', 'candidate')], 'eax distinto del oraculo')
mutate(lambda r: [first(r, RP, 11)[s].update(trace=[1, 2, 3]) for s in ('original', 'candidate')], 'traza distinta')
mutate(lambda r: first(r, SL, 3)['candidate'].update(route_ok=False), 'compuerta route_ok')
mutate(lambda r: next(x for x in r if x['event'] == 'begin').update(compare_dead_x87_registers=False), 'compuerta x87 relajada')
mutate(lambda r: r.remove(next(x for x in r if x.get('function') == SL and x['event'] == 'comparison' and x['case'] == 100)), 'caso ausente')
mutate(lambda r: first(r, SL, 8)['original'].update(path='math' if first(r, SL, 8)['original']['path'] == 'copy' else 'copy'), 'rama distinta')
