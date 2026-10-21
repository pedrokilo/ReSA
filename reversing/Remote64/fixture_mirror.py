"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de disposicion,
las formulas de relleno de la arena y el Rand de C++ frente al de Python, y la cobertura frente a los minimos de run_local.py.
Uso:  python -I fixture_mirror.py        (imprime la cobertura medida; falla si algun minimo no se alcanza)
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
# Constantes de disposicion segun el desensamblado: vehiculo 0xA18 (division del pool), matriz CMatrix pos en 0x30, jugador en foco 0xB7CD74, reloj 0xB7CB84, pool de vehiculos [0xB74494],
# CPlayerInfo 0xB7CD98 + 0x190 por jugador (remoto +0xB0, banderas +0xDD/DE/DF, tiempo +0xE0), camara 0xB6F028, CMissionCleanup 0xA90850; 18 clases de colaborador
LAYOUT = {'veh': 0xA18, 'max_veh': 6, 'mat': 24, 'states': 48, 'base': 24, 'focus': 0xB7CD74, 'timer': 0xB7CB84, 'pool': 0xB74494, 'players': 0xB7CD98, 'stride': 0x190, 'remote': 0xB0,
          'dd': 0xDD, 'de': 0xDE, 'df': 0xDF, 'time': 0xE0, 'camera': 0xB6F028, 'cleanup': 0xA90850, 'none': 0xFF, 'kinds': 18}


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcr64-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return res.stdout.splitlines()


def coverage():
    lines = build_and_run()
    sizes = json.loads(lines.pop().split('\t', 1)[1])
    if sizes != LAYOUT:
        raise SystemExit('Disposicion distinta entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), LAYOUT.get(k)) for k in LAYOUT if sizes.get(k) != LAYOUT.get(k)})
    rows = {0: [], 1: []}
    for l in lines:
        k, js = l.split('\t', 1)
        if k == 'R':
            seed, kk, val = json.loads(js)
            if oracle.rand(seed, kk * 4 + 2) != val:
                raise SystemExit('Rand distinto entre C++ y Python: %d %d: %d / %d' % (seed, kk, val, oracle.rand(seed, kk * 4 + 2)))
            continue
        if k == 'J':
            a, b, c, d = json.loads(js)
            if (oracle.veh_junk(77, 3, 100), oracle.mat_junk(77, 3, 5), oracle.veh_junk(0xFFFF, 5, 645), oracle.mat_junk(123456, 2, 23)) != (a, b, c, d):
                raise SystemExit('Formulas de relleno distintas entre C++ y Python')
            continue
        rows[int(k[1:])].append(json.loads(js))
    cov = {name: {} for name in oracle.NAMES}
    for fn, name in enumerate(oracle.NAMES):
        assert len(rows[fn]) == oracle.CASES_BY_NAME[name], (name, len(rows[fn]))
        for n, row in enumerate(rows[fn]):
            try:
                want, stats = oracle.expected(name, row, n)
            except Exception as e:
                raise SystemExit('Fixture incoherente en %s caso %d: %r' % (name, n, e))
            for key, v in stats.items():
                if v is True:
                    cov[name][key] = cov[name].get(key, 0) + 1
            cov[name]['family%d' % row['fam']] = cov[name].get('family%d' % row['fam'], 0) + 1
            cov[name]['stack_seed'] = cov[name].get('stack_seed', 0) + 1
    return cov


if __name__ == '__main__':
    cov = coverage()
    try:
        import run_local
        short = run_local.check_minimums(cov)
    except (ImportError, AttributeError):
        short = {}
    print(json.dumps({k: dict(sorted(v.items())) for k, v in cov.items()}))
    if short:
        raise SystemExit('Cobertura por debajo de los minimos: ' + json.dumps(short))
    print('espejo ok')
