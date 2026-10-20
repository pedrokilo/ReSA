"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de disposicion
y los valores del falso (RetVal de C++ frente a ret_val de Python) y la cobertura frente a los minimos de run_local.py.
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
# Constantes de disposicion segun el desensamblado: CGarage 0xD8; izquierda 0x28, derecha 0x2C, frente 0x30, fondo 0x34, z 0x08, topZ 0x1C, puerta 0x38, tiempo 0x3C, objetivo 0x40, tipo 0x4C,
# estado 0x4D, banderas 0x4E; reloj 0xB7CB84, contador de fotogramas 0xB7CB4C, jugador en foco 0xB7CD74, dinero 0xB7CE50 (+ 0x190 por jugador), CGarages 0x96C008/9/A/0x96BFDC/E0/E4,
# camara 0xB6F844/48, tienda de bombas 0x8E6940, mando 0xB73458 + 0x10A / 0x10E; 48 clases de colaborador (kKinds = 49)
LAYOUT = {'garage': 0xD8, 'veh': 0xA18, 'max_veh': 8, 'left': 0x28, 'right': 0x2C, 'front': 0x30, 'back': 0x34, 'posz': 0x08, 'topz': 0x1C, 'door': 0x38, 'time': 0x3C, 'target': 0x40, 'type': 0x4C,
          'state': 0x4D, 'flags': 0x4E, 'timer': 0xB7CB84, 'frame': 0xB7CB4C, 'focus': 0xB7CD74, 'money': 0xB7CE50, 'nores': 0x96C008, 'free': 0x96C009, 'bombs': 0x96C00A, 'lastg': 0x96BFDC,
          'lasth': 0x96BFE0, 'cam': 0x96BFE4, 'camin': 0xB6F844, 'camout': 0xB6F848, 'bombshop': 0x8E6940, 'padw': 0xB73458 + 0x10A, 'padb': 0xB73458 + 0x10E, 'kinds': 49}


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcgu63-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return res.stdout.splitlines()


def coverage():
    lines = build_and_run()
    sizes = json.loads(lines.pop().split('	', 1)[1])
    if sizes != LAYOUT:
        raise SystemExit('Disposicion distinta entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), LAYOUT.get(k)) for k in LAYOUT if sizes.get(k) != LAYOUT.get(k)})
    rows = []
    for l in lines:
        k, js = l.split('	', 1)
        if k == 'R':
            seed, kk, val = json.loads(js)
            if oracle.rand(seed, kk * 4 + 2) != val:
                raise SystemExit('Rand distinto entre C++ y Python: semilla %d k %d: %d / %d' % (seed, kk, val, oracle.rand(seed, kk * 4 + 2)))
            continue
        rows.append(json.loads(js))
    assert len(rows) == oracle.CASES, len(rows)
    cov = {}
    for n, row in enumerate(rows):
        try:
            want, stats = oracle.expected(oracle.NAME, row, n)
        except Exception as e:
            raise SystemExit('Fixture incoherente en el caso %d: %r' % (n, e))
        for key, v in stats.items():
            if v is True:
                cov[key] = cov.get(key, 0) + 1
        cov['family%d' % row['fam']] = cov.get('family%d' % row['fam'], 0) + 1
        cov['stack_seed'] = cov.get('stack_seed', 0) + 1
    return cov


if __name__ == '__main__':
    cov = coverage()
    try:
        import run_local
        short = run_local.check_minimums(cov)
    except ImportError:
        short = {}
    print(json.dumps(dict(sorted(cov.items()))))
    if short:
        raise SystemExit('Cobertura por debajo de los minimos: ' + json.dumps(short))
    print('espejo ok')
