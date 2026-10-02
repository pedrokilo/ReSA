"""Run ProcessPed comparisons with the shared test-copy runner."""
import json
import math
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case, digest
PROBE_HASH = 'ee06d2428331ffa1e2b1c3fb49ff43e86ff2e944206c14dbfc98b868e88439fa'
TOTAL_PAIRS = 312  # 144 originales + 168 de la fase C (m_bTaskAborting / !m_Anim)

def compare(a, b):
    if isinstance(a, dict):
        if not isinstance(b, dict) or a.keys() != b.keys(): raise ValueError('Object mismatch')
        error = 0.
        for k in a:
            error = max(error, compare(a[k], b[k]))
        return error
    if isinstance(a, list):
        if not isinstance(b, list) or len(a) != len(b): raise ValueError('Array mismatch')
        return max((compare(x, y) for x, y in zip(a, b)), default=0.)
    if isinstance(a, bool) or isinstance(b, bool):
        if a != b: raise ValueError('Discrete mismatch')
        return 0.
    if isinstance(a, float) or isinstance(b, float):
        if not isinstance(a, (int, float)) or not isinstance(b, (int, float)) or not math.isfinite(a) or not math.isfinite(b):
            raise ValueError('Invalid numeric result')
        error = abs(a - b)
        if error > 1e-5: raise ValueError('Numeric mismatch')
        return error
    if a != b: raise ValueError('Value mismatch')
    return 0.

def analyze(path):
    if not path.exists(): return None
    rows = [json.loads(line) for line in path.read_text().splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('assertion', 'error') for r in rows): raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    completed = [r for r in rows if r['event'] == 'complete']
    if not completed: return None
    pairs = [r for r in rows if r['event'] == 'comparison']
    if len(pairs) != TOTAL_PAIRS or {r['pair'] for r in pairs} != set(range(TOTAL_PAIRS)):
        raise RuntimeError('Cobertura incompleta o duplicada.')
    ready = [r for r in rows if r['event'] == 'player_ready']
    if len(ready) != 1 or not ready[0]['independent_inputs']: raise RuntimeError('Entradas no independientes.')
    maximum = 0.
    for r in pairs:
        if (not r['equal'] or r['original_candidate_calls'] != 0 or r['candidate_calls'] != 1
                or r['original_rng_calls'] != r['candidate_rng_calls']):
            raise RuntimeError('Caso divergente: ' + str(r['pair']))
        maximum = max(maximum, compare(r['original'], r['candidate']))
    if len(completed) != 1 or completed[0]['pairs'] != TOTAL_PAIRS or not completed[0]['state_restored']:
        raise RuntimeError('Estado no restaurado.')
    return dict(pairs=TOTAL_PAIRS, all_equal=True, max_error=maximum, tolerance=1e-5,
                candidate_calls=TOTAL_PAIRS, state_restored=True, log_sha256=digest(path))

if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        result = analyze(Path(sys.argv[2]))
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result, indent=2))
    else:
        sys.exit(run_case('sitatdesk', PROBE_HASH, analyze, 'CTaskInteriorSitAtDesk::ProcessPed'))
