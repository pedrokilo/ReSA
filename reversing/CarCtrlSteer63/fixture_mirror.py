"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de disposicion
(segun el desensamblado) y el generador pseudoaleatorio, y mide la cobertura frente a los minimos de run_local.py / minimums.json.
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
# Disposicion del vehiculo segun el desensamblado: posicion (placement) 0x04, matriz 0x14, velocidad 0x44, estilo 0x3B9, mision 0x3BA, crucero 0x3D0, banderas 0x428, conductor 0x460,
# temporizador de policias 0x4DC, subtipo 0x594, temporizador 0x818; inteligencia del ped 0x47C; matriz: top 0x10, pos 0x30; CTimer::ms_fTimeStep 0xB7CB5C; 10 clases de colaborador (kKinds = 11)
LAYOUT = {'veh': 0x900, 'pos': 0x04, 'mat': 0x14, 'speed': 0x44, 'style': 0x3B9, 'mission': 0x3BA, 'cruise': 0x3D0, 'flags': 0x428, 'driver': 0x460, 'cops': 0x4DC, 'sub': 0x594, 'timer': 0x818,
          'ped_intel': 0x47C, 'mtop': 0x10, 'mpos': 0x30, 'step': 0xB7CB5C, 'kinds': 11, 'per_fn': 4096}


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='cs63-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return res.stdout.splitlines()


def rand(c, k):
    def mix(x):
        x &= 0xFFFFFFFF
        x ^= x >> 16; x = (x * 0x7FEB352D) & 0xFFFFFFFF; x ^= x >> 15; x = (x * 0x846CA68B) & 0xFFFFFFFF; x ^= x >> 16
        return x
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 54321) & 0xFFFFFFFF)


def coverage():
    lines = build_and_run()
    sizes = json.loads(lines.pop().split('\t', 1)[1])
    if sizes != LAYOUT:
        raise SystemExit('Disposicion distinta entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), LAYOUT.get(k)) for k in LAYOUT if sizes.get(k) != LAYOUT.get(k)})
    rows = []
    for l in lines:
        k, js = l.split('\t', 1)
        if k == 'R':
            seed, kk, val = json.loads(js)
            if rand(seed, kk * 4 + 2) != val:
                raise SystemExit('Rand distinto entre C++ y Python: semilla %d k %d: %d / %d' % (seed, kk, val, rand(seed, kk * 4 + 2)))
            continue
        rows.append(json.loads(js))
    assert len(rows) == oracle.CASES, len(rows)
    cov = {}
    for n, row in enumerate(rows):
        try:
            want, stats = oracle.expected(oracle.NAMES[row['fn']], row, n)
        except Exception as e:
            raise SystemExit('Fixture incoherente en el caso %d: %r' % (n, e))
        for key, v in stats.items():
            if v is True:
                cov[key] = cov.get(key, 0) + 1
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
