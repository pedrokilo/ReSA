"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de arena
del C++ (ultima linea) frente a las del oraculo/README y la cobertura frente a los minimos de run_local.py.
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
# Tamanos de arena y desplazamientos dados por supuestos (sumas a mano guarda + objeto + guarda; ver README.md)
ARENAS = {'heap_pages': 48, 'page_bytes': 0x1000, 'heap_this_bytes': 0x48, 'commonsize_bytes': 0x3C, 'commonsize_count': 64, 'free_node_bytes': 0x18,
          'entity_bytes': 0x38, 'entity_off': 0x40, 'entity_arena': 0x40 + 0x38 + 0x40, 'matrix_bytes': 0x48, 'matrix_off': 0x40, 'matrix_arena': 0x40 + 0x48 + 0x40,
          'entity_placement_at': 4, 'entity_matrix_at': 0x14, 'matrix_pos_at': 0x30,
          'node_bytes': 0x0A, 'node_off': 0x40, 'node_arena': 0x40 + 0x0A + 0x36, 'node_light_at': 8, 'node_found_at': 9,
          'task_bytes': 0x80, 'task_off': 0x40, 'task_arena': 0x40 + 0x80 + 0x40, 'ped_bytes': 0x79C, 'ped_off': 0x40, 'ped_arena': 0x40 + 0x79C + 0x40,
          'intel_bytes': 0x40, 'intel_off': 0x40, 'intel_arena': 0x40 + 0x40 + 0x40, 'prim_bytes': 0x20, 'prim_off': 0x40, 'prim_arena': 0x40 + 0x20 + 0x40,
          'task_pos_at': 0x64, 'task_aim_at': 0x70, 'task_speed_at': 0x74, 'task_bits_at': 0x78, 'task_timer_at': 0x7C,
          'ped_placement_at': 4, 'ped_matrix_at': 0x14, 'ped_intel_at': 0x47C, 'ped_shift_at': 0x4D8, 'ped_currot_at': 0x558, 'ped_aimrot_at': 0x55C, 'ped_type_at': 0x598,
          'intel_primary_at': 0x14, 'stand_time': 20000, 'stand_blend': 0x41000000}
assert 0x40 + 0x38 + 0x40 == 0xB8 and 0x40 + 0x48 + 0x40 == 0xC8 and 0x40 + 0x0A + 0x36 == 0x80 and 0x40 + 0x80 + 0x40 == 0x100 and 0x40 + 0x79C + 0x40 == 0x81C and 0x40 + 0x40 + 0x40 == 0xC0 and 0x40 + 0x20 + 0x40 == 0xA0


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcmisc61-'))
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
    if sizes != ARENAS:
        raise SystemExit('Tamanos de arena distintos entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), ARENAS.get(k)) for k in ARENAS if sizes.get(k) != ARENAS.get(k)})
    names = oracle.NAMES
    per = {n: [] for n in names}
    for l in lines:
        k, js = l.split('\t', 1)
        per[names[int(k)]].append(json.loads(js))
    cov = {}
    for name in names:
        rows = per[name]
        assert len(rows) == oracle.CASES, (name, len(rows))
        c = {}
        for n, row in enumerate(rows):
            want, stats = oracle.expected(name, row, n)
            for key, v in stats.items():
                if v is True:
                    c[key] = c.get(key, 0) + 1
        cov[name] = c
    return cov


def main():
    cov = coverage()
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
