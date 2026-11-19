import re, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import json
import runtime_runner
from runtime_runner import run_case, digest, terminal_event
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{m[0]}::{m[1]}': int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src)}


def stream_common(path):
    """Mismas comprobaciones que probe_case.analyzer, pero en flujo (como Fn232A): solo se conservan los eventos de control y los identificadores de par."""
    path = Path(path)
    if not terminal_event(path):
        return None
    total = sum(EXPECTED.values())
    ctrl, pairs, npairs, bad = [], set(), 0, None
    counts = {f: 0 for f in EXPECTED}
    with path.open(encoding='utf-8') as fh:
        for line in fh:
            if not line.endswith('\n'):
                continue
            r = json.loads(line)
            if r['event'] != 'comparison':
                ctrl.append(r)
                continue
            npairs += 1
            pairs.add(r['pair'])
            if bad is None and not (r['equal'] and r['accepted'] and r['original'] == r['candidate']):
                bad = r['pair']
            counts[r['function']] = counts.get(r['function'], 0) + 1
    fails = ['%s: %s/%s' % (r['function'], r['failures'], r['cases']) for r in ctrl if r['event'] == 'function_summary' and r['failures']]
    finished = any(r['event'] in ('complete', 'error', 'assertion') for r in ctrl)
    if fails and finished:
        raise RuntimeError('Funciones con diferencias: ' + '; '.join(fails))
    if any(r['event'] in ('assertion', 'error') for r in ctrl):
        raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    completed = [r for r in ctrl if r['event'] == 'complete']
    if not completed:
        return None
    if npairs != total or pairs != set(range(total)):
        raise RuntimeError('Cobertura incompleta.')
    if bad is not None:
        raise RuntimeError('Diferencia en ' + str(bad))
    if counts != EXPECTED:
        raise RuntimeError('Reparto de pares inesperado: ' + json.dumps(counts))
    if len(completed) != 1 or not completed[0]['state_restored']:
        raise RuntimeError('Estado no restaurado.')
    return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=digest(path))


def main(here, prefix, title, timeout=300):
    here = Path(here).resolve()
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(stream_common(Path(sys.argv[2])), indent=2)); return 0
    probe_hash = json.loads((here/'artifacts/build-manifest.json').read_text())['binaries']['asi']
    return run_case(prefix, probe_hash, stream_common, title, timeout=timeout)


if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'misc8', 'Transmisión, cañones de agua, FX y superficies (lista de 8)', timeout=1500))
