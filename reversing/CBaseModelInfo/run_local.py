"""Autonomous full flag-domain comparison using the shared guarded runner."""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case, digest
PROBE_HASH = '9926f9f24abc3d14ca3b6b8bd3676c6fccdc2ca560add9cfcd1de233faeb3fa9'

def analyze(path):
    if not path.exists(): return None
    data = path.read_text(encoding='utf-8')
    if '"event":"error"' in data or '"event":"assertion"' in data:
        raise RuntimeError('Error registrado dentro del motor.')
    if '"event":"complete"' not in data: return None
    rows = [json.loads(line) for line in data.splitlines(keepends=True) if line.endswith('\n')]
    pairs = [r for r in rows if r['event'] == 'comparison']
    ready = [r for r in rows if r['event'] == 'player_ready']
    completed = [r for r in rows if r['event'] == 'complete']
    if len(ready) != 1 or not ready[0]['independent_inputs']: raise RuntimeError('Entradas no independientes.')
    if len(pairs) != 65536 or {r['flags'] for r in pairs} != set(range(65536)): raise RuntimeError('Cobertura incompleta o duplicada.')
    for r in pairs:
        if not (r['equal'] and r['original'] == r['candidate'] and r['candidate'] in (0, 1) and r['input_unchanged'] and r['stack_balanced'] and r['original_candidate_calls'] == 0 and r['candidate_calls'] == 1):
            raise RuntimeError('Diferencia en flags ' + str(r['flags']))
    if len(completed) != 1 or completed[0]['pairs'] != 65536 or not completed[0]['state_restored']: raise RuntimeError('Estado no restaurado.')
    return dict(pairs=65536, all_equal=True, full_eax=True, independent_inputs=True, inputs_unchanged=True, stack_balanced=True, candidate_calls=65536, state_restored=True, log_sha256=digest(path))

if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        result = analyze(Path(sys.argv[2]))
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result, indent=2))
    else: sys.exit(run_case('basemodelinfo', PROBE_HASH, analyze, 'CBaseModelInfo::SwaysInWind'))
