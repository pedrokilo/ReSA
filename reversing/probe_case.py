"""Runner y analizador comunes para bancos con eventos comparison/complete y campo `accepted`."""
import json
import sys
from pathlib import Path
from runtime_runner import run_case, digest, terminal_event

def analyzer(expected: dict):
    total = sum(expected.values())
    def analyze(path):
        # Read the complete evidence only when its terminal record has arrived.
        # Acceptance below still checks every pair and the restored state.
        if not terminal_event(path): return None
        rows = [json.loads(s) for s in path.read_text().splitlines(keepends=True) if s.endswith('\n')]
        fails = [f"{r['function']}: {r['failures']}/{r['cases']}" for r in rows if r['event'] == 'function_summary' and r['failures']]
        finished = any(r['event'] in ('complete', 'error', 'assertion') for r in rows)
        if fails and finished: raise RuntimeError('Funciones con diferencias: ' + '; '.join(fails))
        if any(r['event'] in ('assertion', 'error') for r in rows): raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
        completed = [r for r in rows if r['event'] == 'complete']
        if not completed: return None
        pairs = [r for r in rows if r['event'] == 'comparison']
        if len(pairs) != total or {r['pair'] for r in pairs} != set(range(total)): raise RuntimeError('Cobertura incompleta.')
        counts = {f: 0 for f in expected}
        for r in pairs:
            if not (r['equal'] and r['accepted'] and r['original'] == r['candidate']): raise RuntimeError('Diferencia en ' + str(r['pair']))
            counts[r['function']] = counts.get(r['function'], 0) + 1
        if counts != expected: raise RuntimeError('Reparto de pares inesperado: ' + json.dumps(counts))
        if len(completed) != 1 or not completed[0]['state_restored']: raise RuntimeError('Estado no restaurado.')
        return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=digest(path))
    return analyze

def main(here, prefix, title, expected, timeout=300):
    here = Path(here).resolve()
    analyze = analyzer(expected)
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2)); return 0
    probe_hash = json.loads((here/'artifacts/build-manifest.json').read_text())['binaries']['asi']
    return run_case(prefix, probe_hash, analyze, title, timeout=timeout)
