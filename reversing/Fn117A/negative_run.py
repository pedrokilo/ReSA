"""Corrida de la PRUEBA NEGATIVA del banco Fn117A: deja correr TODOS los pares aunque haya diferencias (el analizador normal aborta en la primera) y cuenta, por funcion, los pares en los que
el EXE y la DLL difieren o algun lado no coincide con el oraculo independiente. Solo se usa con fuentes mutadas a proposito (ver README.md); no acredita aprobacion.

    python negative_run.py          (con la sonda ya compilada con build_probe.py)
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

LOG = runtime_runner.GAME / 'fn117a-runtime.jsonl'


def analyze(path):
    path = Path(path)
    if not runtime_runner.terminal_event(path):
        return None
    diff, oracle_bad, pairs, first = {}, {}, 0, {}
    with path.open(encoding='utf-8') as stream:
        for line in stream:
            if '"event":"comparison"' not in line:
                continue
            row = json.loads(line)
            fn = row['function']
            pairs += 1
            if row['original'] != row['candidate']:
                diff[fn] = diff.get(fn, 0) + 1
                first.setdefault(fn, row['case'])
            elif oracle.check_row(row):
                oracle_bad[fn] = oracle_bad.get(fn, 0) + 1
                first.setdefault(fn, row['case'])
    return {'passed': True, 'negative_pairs': pairs, 'pairs_exe_dll_differ': diff, 'pairs_oracle_differ_only': oracle_bad, 'first_case': first}


if __name__ == '__main__':
    manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
    sys.exit(runtime_runner.run_case('fn117a', manifest['binaries']['asi'], analyze, 'Fn117A-negativa', timeout=3600, stop_before_analysis=True))
