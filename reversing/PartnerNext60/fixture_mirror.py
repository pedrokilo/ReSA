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
#   this 0x40 + 0x74 + 0x4C = 0x100; ped/partner 0x40 + 0x4E8 + 0x40 = 0x568; matrices 0x20 + 0x48 + 0x20 = 0x88; inteligencias 0x20 + 0x40 + 0x20 = 0x80; tareas falsas 0x20 + 0x30 + 0x30 = 0x80;
#   tarea del partner 0x20 + 0x60 + 0x20 = 0xA0; objetos reservados 0x20 + 0x40 + 0x20 = 0x80; secuencia 0x10 + 0x20 + 0x10 = 0x40
ARENAS = {'this_bytes': 0x74, 'this_off': 0x40, 'this_arena': 0x100, 'sub_at': 8, 'task_id_at': 0x34, 'partner_at': 0x38, 'point_at': 0x40, 'target_at': 0x4C, 'lead_at': 0x58, 'state_at': 0x59,
          'first_at': 0x5A, 'upd_at': 0x5B, 'face_at': 0x5D, 'streamed_at': 0x6E, 'counter_at': 0x70, 'ped_off': 0x40, 'ped_bytes': 0x4E8, 'ped_arena': 0x568, 'placement_at': 4, 'heading_at': 0x10,
          'matrix_at': 0x14, 'intel_at': 0x47C, 'shift_at': 0x4D8, 'mat_off': 0x20, 'mat_bytes': 0x48, 'mat_arena': 0x88, 'intel_off': 0x20, 'intel_bytes': 0x40, 'intel_arena': 0x80,
          'def_at_intel': 0x14, 'task_off': 0x20, 'task_bytes': 0x30, 'task_arena': 0x80, 'other_off': 0x20, 'other_bytes': 0x60, 'other_arena': 0xA0, 'other_partner_at': 0x38,
          'other_state_at': 0x59, 'other_first_at': 0x5A, 'other_point_at': 0x40, 'alloc_off': 0x20, 'alloc_bytes': 0x40, 'alloc_arena': 0x80, 'seq_off': 0x10, 'seq_bytes': 0x20, 'seq_arena': 0x40,
          'time_addr': 0xB7CB84, 'max_events': 16, 'vt_slots': 16, 'size_stand': 0x20, 'size_wander': 0x38, 'size_goto': 0x28, 'size_turn': 0x28}
assert (0x40 + 0x74 + 0x4C == 0x100 and 0x40 + 0x4E8 + 0x40 == 0x568 and 0x20 + 0x48 + 0x20 == 0x88 and 0x20 + 0x40 + 0x20 == 0x80 and 0x20 + 0x30 + 0x30 == 0x80
       and 0x20 + 0x60 + 0x20 == 0xA0 and 0x10 + 0x20 + 0x10 == 0x40)


def build_and_run(nb=0):
    out = Path(tempfile.mkdtemp(prefix='pcpartnernext60-'))
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
