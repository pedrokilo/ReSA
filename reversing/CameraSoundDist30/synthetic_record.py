"""Prueba sin juego de run_local.analyze: fabrica un registro .jsonl con la MISMA forma que escribe Core.hpp (begin, comparison x4096, function_summary, global_state_final, complete), usando el
oraculo como lado EXE y como lado DLL sobre las filas `input` del volcado compilado de Fixtures.hpp, y comprueba que analyze() lo acepta (y rechaza uno corrompido).
No sustituye a la corrida real: solo comprueba que los nombres de campos de Core.hpp, el oraculo y run_local concuerdan.

    python -B synthetic_record.py
"""
import copy
import json
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
import fixture_selfcheck as selfcheck
import run_local

FP = '00' * 108


def side(observed):
    return {'observed': observed, 'post_floating_state': FP, 'post_mxcsr_control': 0, 'stack_restored': True, 'nonvolatile_preserved': True, 'direction_flag_clear': True,
            'route_ok': True, 'traps_zero': True, 'globals_restored': True, 'floating_state_restored': True}


def concrete(want):
    out = dict(want)
    out['written'] = want['written'].replace('?', '0')
    out['trace'] = [0 if x is None else x for x in want['trace']]
    return out


def build():
    rows = selfcheck.dump_rows()
    lines = [{'event': 'begin', 'registered_hook': True, 'registered': {'CCamera::UpdateSoundDistances': True}, 'functions': 1, 'pairs': oracle.CASES,
              'cases': {oracle.NAME: oracle.CASES}, 'direct_functions': [], 'compare_dead_x87_registers': False, 'compare_volatile_registers': False, 'collaborators_stubbed': True,
              'collaborators': {'exe': [0x5674E0], 'dll': [0x10001000]}, 'bodies': {'CCamera::UpdateSoundDistances': 0x10002000}, 'initial_fp': FP, 'initial_frame': 7,
              'layout': {'arena': oracle.LAYOUT['arena'], 'obj_offset': oracle.LAYOUT['guard'], 'obj_bytes': oracle.LAYOUT['obj_bytes'], 'entity_offset': oracle.LAYOUT['guard'] + oracle.LAYOUT['obj_bytes'] + oracle.LAYOUT['guard'], 'matrix_offset': oracle.LAYOUT['arena'] - oracle.LAYOUT['guard'] - oracle.LAYOUT['matrix_bytes']}}]
    for case, inp in enumerate(rows[1:]):
        want, _ = oracle.expected(inp, case)
        s = side(concrete(want))
        lines.append({'event': 'comparison', 'function': oracle.NAME, 'case': case, 'pair': case, 'input': inp, 'original': s, 'candidate': copy.deepcopy(s), 'fp_difference': [],
                      'original_residue': {}, 'candidate_residue': {}, 'equal': True, 'accepted': True})
    lines.append({'event': 'function_summary', 'function': oracle.NAME, 'cases': oracle.CASES, 'failures': 0, 'state_restored': True})
    lines.append({'event': 'global_state_final', 'floating_state': FP, 'state_restored': True, 'watched_intact': True, 'frame': 7})
    lines.append({'event': 'complete', 'pairs': oracle.CASES, 'state_restored': True, 'floating_state_restored': True, 'entries_intact': True})
    return lines


if __name__ == '__main__':
    lines = build()
    with tempfile.TemporaryDirectory() as tmp:
        good = Path(tmp) / 'good.jsonl'
        good.write_text(''.join(json.dumps(r) + '\n' for r in lines), encoding='utf-8')
        summary = run_local.analyze(good)
        assert summary and summary['pairs'] == oracle.CASES, summary
        bad = [copy.deepcopy(r) for r in lines]
        victim = next(r for r in bad if r['event'] == 'comparison' and r['input']['frame'] % 12 == 0)
        for key in ('original', 'candidate'):
            victim[key]['observed']['written'] = 'ff' * 12
        victim['original'] = copy.deepcopy(victim['candidate'])
        badfile = Path(tmp) / 'bad.jsonl'
        badfile.write_text(''.join(json.dumps(r) + '\n' for r in bad), encoding='utf-8')
        try:
            run_local.analyze(badfile)
        except RuntimeError as error:
            assert 'oraculo' in str(error)
        else:
            raise AssertionError('un registro corrupto no se rechazo')
    print('ok')
