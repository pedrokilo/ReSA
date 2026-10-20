"""Verifica filas del arnes offline (harness.exe) o del banco con el oraculo: ambos lados (EXE y funcion C++) deben coincidir con fixture_oracle.py.

Uso: python -I check_rows.py <filas.jsonl> [--stats]
Imprime las primeras discrepancias y (con --stats) la cobertura (conteo de banderas del oraculo).
"""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import fixture_oracle as oracle


def main(path, show_stats):
    bad = 0
    total = 0
    cov = {}
    first = []
    for line in Path(path).read_text(encoding='utf-8').splitlines():
        if not line.endswith('}'):
            continue
        r = json.loads(line)
        if r.get('event') != 'comparison':
            continue
        total += 1
        try:
            want, stats = oracle.expected(r['function'], r['input'], r['case'])
        except oracle.Fail as exc:
            bad += 1
            first.append((r['case'], 'fixture invalido: %s' % exc))
            continue
        for k, v in stats.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
        for label in ('original', 'candidate'):
            obs = r[label]['observed']
            diff = oracle.compare(obs, want)
            if diff:
                bad += 1
                if len(first) < 12:
                    first.append((r['case'], label, diff, {k: obs.get(k) for k in diff[:2]}, {k: want[k] for k in diff[:2]}))
        if not r['equal']:
            bad += 1
            if len(first) < 12:
                first.append((r['case'], 'EXE != DLL'))
    print('%s: %d filas, %d discrepancias' % (path, total, bad))
    for f in first:
        print('  ', str(f)[:900])
    if show_stats:
        print(json.dumps(dict(sorted(cov.items())), indent=0))
    return bad


if __name__ == '__main__':
    sys.exit(1 if main(sys.argv[1], '--stats' in sys.argv) else 0)
