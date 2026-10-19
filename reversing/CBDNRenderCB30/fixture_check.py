"""Autoprueba cruzada del banco CBDNRenderCB30 (sin juego): compila fixture_dump.cpp con cl (Fixtures.hpp REAL), comprueba que las constantes de disposicion coinciden con las del
oraculo Python, que las 6144 filas `input` son validas para el oraculo y calcula la cobertura (la que calibra MINIMUMS en run_local.py).
Uso:  python -I fixture_check.py              (constantes + validez de las filas + cobertura)
      python -I fixture_check.py --synthetic  (ademas, un registro .jsonl SINTETICO perfecto que ejercita run_local.analyze: compuertas, oraculo y cobertura minima)"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

SYN_ARGS = ['0x28']
SYN_OFFSETS = {'env': 0x28}
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'


def dump(args=()):
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / 'fixture_dump.exe'
        cmd = f'call "{VCVARS}" >nul && cl /nologo /EHsc /std:c++20 /Fe:"{exe}" /Fo:"{tmp}\\\\" "{HERE / "fixture_dump.cpp"}"'
        r = subprocess.run('cmd /s /c "' + cmd + '"', capture_output=True, text=True, cwd=str(HERE), shell=False)
        if r.returncode:
            raise SystemExit('cl fallo:\n' + r.stdout + r.stderr)
        out = subprocess.run([str(exe), *args], capture_output=True, text=True, check=True).stdout
    return [json.loads(line) for line in out.splitlines()]


def main():
    rows = dump()
    consts = rows[0]
    assert consts.pop('event') == 'constants'
    assert consts == oracle.K, {k: (consts.get(k), oracle.K.get(k)) for k in set(consts) | set(oracle.K) if consts.get(k) != oracle.K.get(k)}
    inputs = rows[1:]
    assert len(inputs) == oracle.CASES[oracle.NAME] == consts['kCases']
    cov = {}
    for n, inp in enumerate(inputs):
        assert inp['n'] == n
        want, st = oracle.expected(oracle.NAME, inp, n)
        for k, v in st.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
        cov['g%d' % inp['group']] = cov.get('g%d' % inp['group'], 0) + 1
    print(json.dumps(cov, indent=1, sort_keys=True))
    print('ok: %d filas, constantes iguales' % len(inputs))


def synthetic(rows):
    """Registro de una corrida PERFECTA inventada (cada lado = lo que modela el oraculo): comprueba que run_local.analyze acepta lo esperado y rechaza una celda alterada."""
    import copy
    import run_local
    inputs = rows[1:]
    out = [{'event': 'begin', 'registered_hook': True, 'registered': {run_local.NAME: True}, 'bodies': {run_local.NAME: 1}, 'functions': 1, 'pairs': len(inputs), 'cases': dict(oracle.CASES),
            'direct_functions': [], 'compare_dead_x87_registers': False, 'compare_volatile_registers': False, 'collaborators_stubbed': True,
            'collaborators': {'exe_real': [0x821B40], 'stubbed': [{'exe': a, 'dll': 1} for a in sorted(run_local.STUBBED)]}, 'plugin_offsets': SYN_OFFSETS, 'watched_entries': [{'address': a, 'bytes': '00'} for a in sorted(run_local.WATCHED)],
            'initial_fp': 'ab', 'initial_mxcsr': 1}]
    gates = dict(stack_restored=True, nonvolatile_preserved=True, direction_flag_clear=True, route_ok=True, globals_restored=True, floating_state_restored=True)
    for n, inp in enumerate(inputs):
        want, _ = oracle.expected(oracle.NAME, inp, n)
        side = dict(gates, observed=copy.deepcopy(want))
        out.append({'event': 'comparison', 'function': oracle.NAME, 'case': n, 'pair': n, 'input': inp, 'original': side, 'candidate': copy.deepcopy(side), 'fp_difference': [],
                    'original_residue': {'eax': 1, 'ecx': 2, 'edx': 3, 'eflags': 0}, 'candidate_residue': {'eax': 1, 'ecx': 9, 'edx': 3, 'eflags': 0}, 'equal': True, 'accepted': True})
    out.append({'event': 'function_summary', 'function': oracle.NAME, 'cases': len(inputs), 'failures': 0, 'state_restored': True})
    out.append({'event': 'global_state_final', 'floating_state': 'ab', 'mxcsr': 1, 'state_restored': True, 'watched_intact': True})
    out.append({'event': 'complete', 'pairs': len(inputs), 'state_restored': True, 'floating_state_restored': True, 'entries_intact': True})
    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / 'comparisons.jsonl'
        assert run_local.analyze(path) is None                                       # el registro aun no existe
        path.write_text(''.join(json.dumps(r) + '\n' for r in out), encoding='utf-8')
        res = run_local.analyze(path)
        assert res and res['all_equal'] and res['pairs'] == len(inputs), res
        bad = copy.deepcopy(out)
        k = next(i for i, r in enumerate(bad) if r['event'] == 'comparison' and r['case'] == 7)
        bad[k]['candidate']['observed']['trace'] = []
        bad[k]['original'] = copy.deepcopy(bad[k]['candidate'])
        path.write_text(''.join(json.dumps(r) + '\n' for r in bad), encoding='utf-8')
        try:
            run_local.analyze(path)
            raise AssertionError('debia rechazar una salida distinta de la del oraculo')
        except RuntimeError as e:
            assert 'oraculo' in str(e), e
    print('ok: run_local.analyze acepta la corrida sintetica perfecta y rechaza una salida alterada')


if __name__ == '__main__':
    main()
    if '--synthetic' in sys.argv:
        sys.path.insert(0, str(HERE.parent))
        synthetic(dump(SYN_ARGS))

