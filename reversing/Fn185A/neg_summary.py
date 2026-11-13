"""Resumen de una ronda de la prueba negativa de Fn185A (lote 185A): por funcion, cuantos pares tienen diferencias EXE/DLL, y comprueba que el lado EXE coincide con el oraculo independiente.

    python -I neg_summary.py <carpeta-de-resultados-de-la-ronda> <carpeta-de-salida>

Escribe diferencias-por-funcion.txt en la carpeta de salida y copia RESULTADO.txt y summary.json (el comparisons.jsonl crudo no se archiva: solo 2048 casos por funcion).
"""
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402


def main(src, dst):
    src, dst = Path(src), Path(dst)
    dst.mkdir(parents=True, exist_ok=True)
    per = {n: 0 for n in oracle.NAMES}
    total = bad = exe_off_oracle = 0
    for line in (src / 'comparisons.jsonl').read_text(encoding='utf-8').splitlines():
        if '"event":"begin"' in line:
            oracle.set_script_head(json.loads(line).get('script_head'))
        if '"event":"comparison"' not in line:
            continue
        r = json.loads(line)
        total += 1
        if r['original'] != r['candidate'] or not all(r['original'].get(k) is True for k in ('route_ok', 'collaborators_dll')):
            per[r['function']] += 1
            bad += 1
        if oracle.check_side(r['input'], r['original']['observed'], oracle.expected(r['input']), r['original']):
            exe_off_oracle += 1
    clean = [n for n, v in per.items() if v == 0]
    (dst / 'diferencias-por-funcion.txt').write_text(json.dumps(per) + '\npares %d con diferencias %d lado EXE fuera del oraculo %d\nfunciones SIN diferencias: %s\n' % (total, bad, exe_off_oracle, clean), encoding='utf-8')
    for n in ('RESULTADO.txt', 'summary.json'):
        shutil.copyfile(src / n, dst / n)
    print(open(dst / 'diferencias-por-funcion.txt', encoding='utf-8').read())


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
