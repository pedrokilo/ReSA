"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de arena
del C++ (ultima linea) frente a las del oraculo/README y la cobertura frente a los minimos de run_local.py. Con --evidence <comparisons.jsonl> comprueba ademas que las filas `input`
registradas en el juego son IDENTICAS a las del espejo (el generador compilado en el juego y el compilado aparte dan los mismos casos).
Uso:  python -I fixture_mirror.py [--evidence <comparisons.jsonl>]   (imprime la cobertura medida; falla si algun minimo no se alcanza)
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')
# Tamanos de arena y de los arreglos del juego dados por supuestos (sumas a mano guarda + objeto + guarda; ver README.md)
ARENAS = {'vertex_bytes': 20, 'vertex_count': 1021, 'quad_bytes': 10, 'quad_count': 301, 'tri_bytes': 8, 'tri_count': 6, 'block_words': 144, 'combo_words': 700,
          'depth_obj': 0x40, 'depth_obj_bytes': 0x40, 'depth_arena': 0x40 + 0x40 + 0x40, 'line_obj': 0x40, 'line_obj_bytes': 0x10, 'line_arena': 0x40 + 0x10 + 0x30, 'null_out': 0xFFFFFFFF}
assert 0x40 + 0x40 + 0x40 == 0xC0 and 0x40 + 0x10 + 0x30 == 0x80 and 6 * 8 == 48 and 301 * 10 == 3010 and 1021 * 20 == 20420


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcwater62-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return res.stdout.splitlines()


def mirror_rows():
    lines = build_and_run()
    sizes = json.loads(lines.pop().split('\t', 1)[1])
    if sizes != ARENAS:
        raise SystemExit('Tamanos de arena distintos entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), ARENAS.get(k)) for k in ARENAS if sizes.get(k) != ARENAS.get(k)})
    per = {n: [] for n in oracle.NAMES}
    for l in lines:
        k, js = l.split('\t', 1)
        per[oracle.NAMES[int(k)]].append(json.loads(js))
    return per


class _Obs(dict):
    pass


def coverage(per):
    cov = {}
    for name in oracle.NAMES:
        rows = per[name]
        assert len(rows) == oracle.CASES, (name, len(rows))
        c = {}
        for n, row in enumerate(rows):
            # el puntero del objeto de GetWaterDepth es una direccion del juego: aqui basta una direccion alineada ficticia
            want, stats = oracle.expected(name, row, n, {'obj_ptr': 0x1234560} if name == oracle.NAME_DEPTH else None)
            for key, v in stats.items():
                if v is True:
                    c[key] = c.get(key, 0) + 1
            for key in ('family',):
                if key in row:
                    c['%s%d' % (key, row[key])] = c.get('%s%d' % (key, row[key]), 0) + 1
            c['stack_seed'] = c.get('stack_seed', 0) + (1 if row['stack_seed'] in (0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000) else 0)
        cov[name] = c
    return cov


def main():
    per = mirror_rows()
    if len(sys.argv) == 3 and sys.argv[1] == '--evidence':
        seen = {n: {} for n in oracle.NAMES}
        for line in Path(sys.argv[2]).read_text(encoding='utf-8').splitlines():
            r = json.loads(line)
            if r['event'] == 'comparison':
                seen[r['function']][r['case']] = r['input']
        for name in oracle.NAMES:
            if len(seen[name]) != oracle.CASES:
                raise SystemExit('Evidencia incompleta en ' + name)
            bad = [n for n in range(oracle.CASES) if seen[name][n] != per[name][n]]
            if bad:
                raise SystemExit('Las filas input del juego difieren del espejo en %s: casos %s' % (name, bad[:10]))
        print('las filas input del juego son identicas a las del espejo (5 x %d)' % oracle.CASES)
    cov = coverage(per)
    import run_local
    short = {}
    for name in oracle.NAMES:
        for k, m in run_local.MINIMUMS[name].items():
            if cov[name].get(k, 0) < m:
                short['%s:%s' % (name, k)] = (cov[name].get(k, 0), m)
    print(json.dumps(dict(coverage={n: dict(sorted(c.items())) for n, c in cov.items()}, shortfalls=short), indent=1))
    if short:
        raise SystemExit('cobertura insuficiente: %s' % short)
    print('ok')


if __name__ == '__main__':
    main()
