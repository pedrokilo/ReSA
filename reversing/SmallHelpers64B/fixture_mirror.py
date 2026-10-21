"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de arena
del C++ (ultima linea) frente a las del oraculo y la cobertura frente a los minimos de run_local.py. Con --evidence <comparisons.jsonl> comprueba ademas que las filas `input`
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
ARENAS = {'num_groups': oracle.NUM_GROUPS, 'num_ped_groups': oracle.NUM_PED_GROUPS, 'trans_base': oracle.TRANS_BASE, 'trans_guard': oracle.TRANS_GUARD, 'trans_main': oracle.TRANS_MAIN,
          'trans_bytes': oracle.TRANS_BYTES, 'ped_base': oracle.PED_BASE, 'ped_guard': oracle.PED_GUARD, 'ped_main': oracle.PED_MAIN, 'ped_bytes': oracle.PED_BYTES,
          'task_obj': oracle.TASK_OBJ, 'task_obj_bytes': oracle.TASK_OBJ_BYTES, 'task_arena': oracle.TASK_ARENA, 'addr_verts': oracle.ADDR_VERTS, 'addr_indices': oracle.ADDR_INDICES}


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcsmall64b-'))
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
        raise SystemExit('Constantes distintas entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), ARENAS.get(k)) for k in ARENAS if sizes.get(k) != ARENAS.get(k)})
    per = {n: [] for n in oracle.NAMES}
    for l in lines:
        k, js = l.split('\t', 1)
        per[oracle.NAMES[int(k)]].append(json.loads(js))
    return per


def coverage(per):
    cov = {}
    for name in oracle.NAMES:
        rows = per[name]
        assert len(rows) == oracle.CASES, (name, len(rows))
        c = {}
        for n, row in enumerate(rows):
            want, stats = oracle.expected(name, row, n, None)
            for key, v in stats.items():
                if v is True:
                    c[key] = c.get(key, 0) + 1
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
        print('las filas input del juego son identicas a las del espejo (3 x %d)' % oracle.CASES)
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
