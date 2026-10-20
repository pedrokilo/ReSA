"""Autoprueba de run_local.py (sin el juego): fabrica un comparisons.jsonl «perfecto» con la forma exacta que escribe Core.hpp (salidas tomadas del oraculo, ambos lados iguales, compuertas
verdaderas, restauracion correcta) y comprueba que `run_local.analyze` lo acepta, que devuelve None si el registro aun no existe o esta incompleto y que RECHAZA una salida alterada
(un byte de cambios, un argumento de un evento, la velocidad, una compuerta). Uso:  python -I selftest_run_local.py   (imprime «ok»).
"""
import copy
import json
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402
import fixture_mirror  # noqa: E402
import run_local  # noqa: E402

ADDRS = [0x10000000 + 0x2000 * l for l in range(oracle.L_COUNT)]


def observed(name, want, inp):
    if name == oracle.NAME_CS:
        return {'ret': want['ret'], 'changes': want['changes'], 'bools': [inp['bools'][0] & 0xFF, inp['bools'][1] & 0xFF], 'bools_unchanged': True}
    obs = {'ret': None, 'events': [run_local.pad(e) for e in want['events']], 'changes': want['changes'], 'others_unchanged': True}
    if name == oracle.NAME_PS:
        obs['bools'] = want['bools']
    else:
        obs.update({'speed': want['speed'], 'rand_calls': want['rand_calls'], 'globals_unchanged': True})
    return obs


def side(obs):
    return {'observed': obs, 'post_floating_state': '00' * 108, 'post_mxcsr_control': 0, 'stack_restored': True, 'nonvolatile_preserved': True, 'direction_flag_clear': True, 'seh_restored': True,
            'route_ok': True, 'globals_restored': True, 'floating_state_restored': True}


def build_log(path, mutate=None):
    lines = fixture_mirror.build_and_run()
    lines.pop()
    rows = []
    stubbed = [{'exe': a, 'dll': 0 if a in run_local.EXE_ONLY else a + 0x10000000} for a in run_local.STUBBED]
    rows.append({'event': 'waiting_for_player'})
    rows.append({'event': 'begin', 'registered_hook': True, 'registered': {n: True for n in oracle.NAMES}, 'functions': 3, 'pairs': oracle.CASES, 'cases': oracle.CASES_BY_NAME, 'direct_functions': [],
                 'compare_dead_x87_registers': False, 'compare_volatile_registers': False, 'collaborators_stubbed': True, 'collaborators': {'exe_real': [0x406DA0, 0x40FE60, 0x40FEC0, 0x821B1E], 'stubbed': stubbed},
                 'watched_entries': [{'address': a, 'bytes': '00'} for a in sorted(run_local.WATCHED)], 'bodies': {n: 0x10000000 + i for i, n in enumerate(oracle.NAMES)}, 'asi_module': 0x10000000,
                 'original_module': 0x400000, 'initial_fp': '00' * 108, 'initial_mxcsr': 0x1F80, 'initial_rand_state': 1234})
    counts = {n: 0 for n in oracle.NAMES}
    pair = 0
    for line in lines:
        name = fixture_mirror.FUNCS[line[0]]
        n = counts[name]
        counts[name] += 1
        inp = json.loads(line[2:])
        inp['addrs'] = ADDRS
        want, _ = oracle.expected(name, inp, n)
        o = observed(name, want, inp)
        row = {'event': 'comparison', 'function': name, 'case': n, 'pair': pair, 'fn_pair': n, 'input': inp, 'original': side(o), 'candidate': side(copy.deepcopy(o)), 'fp_difference': [],
               'original_residue': {'eax': 1, 'ecx': 2, 'edx': 3, 'eflags': 4}, 'candidate_residue': {'eax': 5, 'ecx': 6, 'edx': 7, 'eflags': 4}, 'equal': True, 'accepted': True}
        pair += 1
        if mutate:
            mutate(row, name, n)
        rows.append(row)
        if n + 1 == oracle.CASES_BY_NAME[name]:
            rows.append({'event': 'function_summary', 'function': name, 'cases': n + 1, 'failures': 0, 'state_restored': True})
    rows.append({'event': 'global_state_final', 'floating_state': '00' * 108, 'mxcsr': 0x1F80, 'state_restored': True, 'watched_intact': True, 'globals_intact': True, 'rand_state': 1234})
    rows.append({'event': 'complete', 'pairs': pair, 'state_restored': True, 'floating_state_restored': True, 'entries_intact': True})
    path.write_text('\n'.join(json.dumps(r) for r in rows) + '\n', encoding='utf-8')


def expect_reject(tmp, label, mutate):
    p = tmp / (label + '.jsonl')
    build_log(p, mutate)
    try:
        run_local.analyze(p)
    except RuntimeError:
        return
    raise SystemExit('run_local.analyze acepto una salida alterada: ' + label)


def main():
    tmp = Path(tempfile.mkdtemp(prefix='pedwrappers60-runlocal-'))
    if run_local.analyze(tmp / 'no-existe.jsonl') is not None:
        raise SystemExit('analyze debe devolver None si el registro no existe')
    p = tmp / 'ok.jsonl'
    build_log(p)
    text = p.read_text(encoding='utf-8')
    (tmp / 'incompleto.jsonl').write_text(text[:len(text) // 2].rsplit('\n', 1)[0] + '\n', encoding='utf-8')
    if run_local.analyze(tmp / 'incompleto.jsonl') is not None:
        raise SystemExit('analyze debe devolver None si el registro esta incompleto')
    summary = run_local.analyze(p)
    if summary['pairs'] != oracle.CASES or not summary['all_equal']:
        raise SystemExit('resumen inesperado')

    def flip_change(row, name, n):
        if name == oracle.NAME_PC and n == 77 and row['original']['observed']['changes']:
            row['original']['observed']['changes'][0][2] ^= 1

    def flip_event(row, name, n):
        if name == oracle.NAME_PC and n == 91:
            for e in row['candidate']['observed']['events']:
                if e[0] == oracle.K_SHADOW or e[0] == oracle.K_PEDCOUNT or e[0] == oracle.K_AE_SERVICE:
                    e[1] ^= 1
                    return
            raise SystemExit('el caso 91 no tiene eventos que alterar')

    def flip_speed(row, name, n):
        if name == oracle.NAME_PC and n == 5:
            row['candidate']['observed']['speed'][0] ^= 0x100

    def flip_gate(row, name, n):
        if name == oracle.NAME_PS and n == 9:
            row['original']['route_ok'] = False

    def flip_ret(row, name, n):
        if name == oracle.NAME_CS and n == 3:
            row['candidate']['observed']['ret'] ^= 1

    def flip_bool(row, name, n):
        if name == oracle.NAME_PS and n == 4:
            row['original']['observed']['bools'][1] ^= 1

    for label, fn in (('cambio', flip_change), ('evento', flip_event), ('velocidad', flip_speed), ('compuerta', flip_gate), ('retorno', flip_ret), ('bool', flip_bool)):
        expect_reject(tmp, label, fn)
    print('ok (analyze acepta el registro perfecto, devuelve None si falta o esta incompleto y rechaza 6 alteraciones)')


if __name__ == '__main__':
    main()
