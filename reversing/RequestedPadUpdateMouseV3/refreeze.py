"""CASO-CONGELADOS: recalcula los SHA256 de `prepared-freeze.json` sobre los ficheros actuales (banco y ficheros compartidos).

El congelado original guardo los hash de bytes con fin de linea CRLF (copia de trabajo de Windows) y de las versiones de `reversing/ProbeRuntime.hpp` y `reversing/runtime_runner.py` de octubre; git
guarda esos ficheros con LF (`* text=auto eol=lf`), de modo que en cualquier copia limpia (worktree) los hashes no coinciden y `build_probe.py` aborta antes de compilar. Este guion es la unica
accion permitida sobre el congelado: no cambia el corpus, el modelo ni el alcance, solo los hashes, y anota la fecha y la causa.

    python -B reversing/RequestedPadUpdateMouseV3/refreeze.py
"""
import hashlib
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
path = HERE / 'prepared-freeze.json'
raw = path.read_bytes()
freeze = json.loads(raw.decode('utf-8'))
for name in freeze['inputs']:
    freeze['inputs'][name] = hashlib.sha256((ROOT / name).read_bytes()).hexdigest()
freeze['refrozen'] = {'date': '2026-11-10', 'reason': 'hashes recalculados sobre ficheros con LF y sobre las versiones actuales de ProbeRuntime.hpp/runtime_runner.py (CASO-CONGELADOS)'}
path.write_bytes((json.dumps(freeze, indent=2) + '\n').encode('utf-8'))
print('refrozen', len(freeze['inputs']), 'entradas')
