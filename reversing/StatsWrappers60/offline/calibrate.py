"""Calibra los minimos de cobertura de run_local.py con el conteo exacto del oraculo sobre las filas del arnes offline (las fixtures son deterministas).

Uso: python -I calibrate.py <carpeta con find.jsonl build.jsonl resp.jsonl cons.jsonl> [--write]
Sin --write imprime el diccionario MINIMUMS; con --write sustituye el literal en ../run_local.py (la linea que empieza por `MINIMUMS = `).
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import fixture_oracle as oracle

FILES = ((oracle.NF, 'find.jsonl'), (oracle.NB, 'build.jsonl'), (oracle.NR, 'resp.jsonl'), (oracle.NC, 'cons.jsonl'))


def main(folder, write):
    mins = {}
    for name, fname in FILES:
        cov = {}
        for line in (Path(folder) / fname).read_text(encoding='utf-8').splitlines():
            r = json.loads(line)
            _, stats = oracle.expected(r['function'], r['input'], r['case'])
            for k, v in stats.items():
                if v is True:
                    cov[k] = cov.get(k, 0) + 1
        mins[name] = {k: max(1, (v * 6) // 10) for k, v in sorted(cov.items())}
    text = json.dumps(mins, indent=4, sort_keys=False)
    if write:
        p = HERE.parent / 'run_local.py'
        src = p.read_text(encoding='utf-8')
        lines = src.split('\n')
        i = next(k for k, l in enumerate(lines) if l.startswith('MINIMUMS = '))
        j = i
        if lines[i].rstrip().endswith('{') or lines[i].strip() == 'MINIMUMS = __MINIMUMS__':
            pass
        # el literal ocupa desde la linea i hasta la primera linea que cierra con `}` en la columna 0
        if lines[i].strip() != 'MINIMUMS = __MINIMUMS__':
            j = next(k for k in range(i, len(lines)) if lines[k] == '}')
        lines[i:j + 1] = ['MINIMUMS = ' + text]
        p.write_text('\n'.join(lines), encoding='utf-8')
    else:
        print(text)


if __name__ == '__main__':
    main(sys.argv[1], '--write' in sys.argv)
