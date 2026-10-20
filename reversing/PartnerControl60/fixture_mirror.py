"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de arena
del C++ (ultima linea) frente a las del oraculo y la cobertura frente a los minimos de run_local.py.
Uso:  python -I fixture_mirror.py [numero de bloques de animacion, por defecto 120; el juego usa el valor de 0xB4EA30]
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
# Tamanos de arena que se dan por supuestos (suma a mano guarda + objeto + guarda):
#   this 0x40 + 0x74 + 0x4C = 0x100; subtarea 0x20 + 0x30 + 0x30 = 0x80; ped 0x10 + 0x20 + 0x10 = 0x40; partner 0x40 + 0x500 + 0x40 = 0x580; inteligencia 0x20 + 0x40 + 0x20 = 0x80; otra tarea 0x20 + 0x60 + 0x20 = 0xA0
ARENAS = {'this_bytes': 0x74, 'this_off': 0x40, 'this_arena': 0x100, 'sub_at': 8, 'task_id_at': 0x34, 'partner_at': 0x38, 'state_at': 0x59, 'completed_at': 0x5C, 'counter_at': 0x70,
          'sub_off': 0x20, 'sub_bytes': 0x30, 'sub_arena': 0x80, 'ped_off': 0x10, 'ped_bytes': 0x20, 'ped_arena': 0x40, 'part_off': 0x40, 'part_bytes': 0x500, 'part_arena': 0x580,
          'intel_at_partner': 0x47C, 'intel_off': 0x20, 'intel_bytes': 0x40, 'intel_arena': 0x80, 'other_off': 0x20, 'other_bytes': 0x60, 'other_arena': 0xA0, 'other_partner_at': 0x38,
          'ik': 0xC15448, 'goto_type': 0x387, 'max_events': 12, 'vt_slots': 16}
assert 0x40 + 0x74 + 0x4C == 0x100 and 0x20 + 0x30 + 0x30 == 0x80 and 0x10 + 0x20 + 0x10 == 0x40 and 0x40 + 0x500 + 0x40 == 0x580 and 0x20 + 0x40 + 0x20 == 0x80 and 0x20 + 0x60 + 0x20 == 0xA0


def build_and_run(nb=0):
    out = Path(tempfile.mkdtemp(prefix='pcpartnercontrol60-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return [json.loads(l) for l in res.stdout.splitlines() if l.strip()]


def main(nb=0):
    rows = build_and_run(nb)
    sizes = rows.pop()['sizes']
    if sizes != ARENAS:
        raise SystemExit('Tamanos de arena distintos entre Fixtures.hpp y el oraculo: %s != %s' % (sizes, ARENAS))
    assert len(rows) == oracle.CASES, len(rows)
    cov = {}
    for n, row in enumerate(rows):
        want, stats = oracle.expected(oracle.NAME, row, n)
        for k, v in stats.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
    import run_local
    short = {k: (cov.get(k, 0), m) for k, m in run_local.MINIMUMS[oracle.NAME].items() if cov.get(k, 0) < m}
    print(json.dumps(dict(cases=len(rows), coverage=dict(sorted(cov.items())), shortfalls=short), indent=1))
    if short:
        raise SystemExit('cobertura insuficiente: %s' % short)
    print('ok')


if __name__ == '__main__':
    main(int(sys.argv[1]) if len(sys.argv) > 1 else 0)
