"""Contraste de Fixtures.hpp sin juego: compila fixture_dump.cpp (solo C++ estandar, NO es el proyecto), vuelca los 4608 casos y los pasa por el oraculo.

  python -I fixture_mirror.py            # compila con MSVC (vcvars32 de Visual Studio), vuelca, valida y resume la cobertura
  python -I fixture_mirror.py dump.txt   # usa un volcado ya hecho

Comprueba: 0 casos invalidos para el oraculo (tipos sin tabla, indices fuera de rango, ...), reparto por familia, y que la cobertura real del generador supere los MINIMOS de run_local.py.
Imprime `ok` y la cobertura. Es la herramienta con la que se calibraron los minimos de run_local.py.
"""
import json
import os
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'


def build_dump(tmp):
    for n in ('Fixtures.hpp', 'fixture_dump.cpp'):
        (tmp / n).write_bytes((HERE / n).read_bytes())
    cmd = 'call "%s" >nul 2>&1 & cl /nologo /EHsc /std:c++20 /W4 fixture_dump.cpp /Fe:fd.exe' % VCVARS
    env = dict(os.environ)
    env['PATH'] = r'C:\Program Files (x86)\Microsoft Visual Studio\Installer;' + env.get('PATH', '')   # vcvars32.bat necesita vswhere.exe
    subprocess.run('cmd /s /c "%s"' % cmd, cwd=tmp, check=True, env=env, stdout=subprocess.DEVNULL)
    with open(tmp / 'dump.txt', 'wb') as out:
        subprocess.run([str(tmp / 'fd.exe')], cwd=tmp, check=True, stdout=out)
    return tmp / 'dump.txt'


def parse(line):
    t = line.split()
    n, fam, save = int(t[0]), int(t[1]), t[2]
    (has_matrix, ped_mode, ped_seed, ped_const, mat_mode, mat_seed, mat_const, st_mode, st_seed, st_const, state_valid, exit_idx, state_byte, eax, edx, stack) = [int(x) for x in t[3:19]]
    infos = [[int(v) for v in x.split(',')] for x in t[19:]]
    return n, dict(family=fam, save=save, has_matrix=has_matrix, ped_mode=ped_mode, ped_seed=ped_seed, ped_const=ped_const, mat_mode=mat_mode, mat_seed=mat_seed, mat_const=mat_const,
                   states_mode=st_mode, states_seed=st_seed, states_const=st_const, state_valid=state_valid, state_byte=state_byte, exit_idx=exit_idx, infos=infos, eax=eax, edx=edx,
                   stack_seed=stack)


def main():
    if len(sys.argv) > 1:
        dump = Path(sys.argv[1])
        lines = dump.read_text().splitlines()
    else:
        with tempfile.TemporaryDirectory() as td:
            lines = build_dump(Path(td)).read_text().splitlines()
    if len(lines) != oracle.CASES[oracle.NAME]:
        raise SystemExit('Numero de casos inesperado: %d' % len(lines))
    import run_local
    cov = {}
    for line in lines:
        n, inp = parse(line)
        want, st = oracle.expected(oracle.NAME, inp, n)
        for k in run_local.coverage(inp, st):
            cov[k] = cov.get(k, 0) + 1
    short = {k: (cov.get(k, 0), m) for k, m in run_local.MINIMUMS.items() if cov.get(k, 0) < m}
    print(json.dumps(dict(sorted(cov.items())), indent=0))
    if short:
        raise SystemExit('Minimos por debajo: ' + json.dumps(short))
    print('ok')


if __name__ == '__main__':
    main()
