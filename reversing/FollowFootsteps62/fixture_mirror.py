"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de arena
del C++ (ultima linea) frente a las del oraculo/README y la cobertura frente a los minimos de run_local.py.
Uso:  python -I fixture_mirror.py
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
#   this 0x20 + 0x20 + 0x20 = 0x60; subtarea 0x20 + 0x30 + 0x20 = 0x70; peaton/objetivo 0x40 + 0x480 + 0x40 = 0x500; matriz 0x10 + 0x48 + 0x18 = 0x70; inteligencia 0x20 + 0x278 + 0x20 = 0x2B8;
#   ruta 0x20 + 0x64 + 0x24 = 0xA8; tarea de GetSimplestActiveTask 0x10 + 0x30 + 0x10 = 0x50; tarea reservada por operator new 0x20 + 0x50 + 0x20 = 0x90
ARENAS = {'this_off': 0x20, 'this_bytes': 0x20, 'this_arena': 0x60, 'sub_at': 8, 'target_at': 0xC, 'create_timer_at': 0x10, 'los_timer_at': 0x14, 'route_at': 0x18, 'move_at': 0x1C, 'flags_at': 0x1E,
          'sub_off': 0x20, 'sub_bytes': 0x30, 'sub_arena': 0x70, 'sub_move_at': 8, 'ped_off': 0x40, 'ped_bytes': 0x480, 'ped_arena': 0x500, 'place_at': 4, 'mat_at': 0x14, 'ped_flags_at': 0x46C, 'intel_at': 0x47C,
          'mat_off': 0x10, 'mat_bytes': 0x48, 'mat_arena': 0x70, 'mat_pos_at': 0x30, 'intel_off': 0x20, 'intel_bytes': 0x278, 'intel_arena': 0x2B8, 'counter_at': 0x274, 'mgr_at': 4,
          'route_off': 0x20, 'route_bytes': 0x64, 'route_arena': 0xA8, 'go_off': 0x10, 'go_bytes': 0x30, 'go_arena': 0x50, 'alloc_off': 0x20, 'alloc_bytes': 0x50, 'alloc_arena': 0x90,
          'max_events': 20, 'vt_slots': 16, 'timer': 0xB7CB84, 'cases': 4096, 'fns': 5}
assert 0x20 + 0x20 + 0x20 == 0x60 and 0x20 + 0x30 + 0x20 == 0x70 and 0x40 + 0x480 + 0x40 == 0x500 and 0x10 + 0x48 + 0x18 == 0x70 and 0x20 + 0x278 + 0x20 == 0x2B8
assert 0x20 + 0x64 + 0x24 == 0xA8 and 0x10 + 0x30 + 0x10 == 0x50 and 0x20 + 0x50 + 0x20 == 0x90


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='fffootsteps62-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return [json.loads(l) for l in res.stdout.splitlines() if l.strip()]


def main():
    rows = build_and_run()
    sizes = rows.pop()['sizes']
    if sizes != ARENAS:
        raise SystemExit('Tamanos de arena distintos entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), ARENAS.get(k)) for k in set(sizes) | set(ARENAS) if sizes.get(k) != ARENAS.get(k)})
    assert len(rows) == oracle.CASES * len(oracle.FUNCS), len(rows)
    import run_local
    report = {}
    short_all = {}
    for fi, fn in enumerate(oracle.FUNCS):
        cov = {}
        for n in range(oracle.CASES):
            row = rows[fi * oracle.CASES + n]
            assert row['fn'] == fi
            want, stats = oracle.expected(fn, row, n)
            for k, v in stats.items():
                if v is True:
                    cov[k] = cov.get(k, 0) + 1
        mins = run_local.MINIMUMS[fn]
        short = {k: (cov.get(k, 0), m) for k, m in mins.items() if cov.get(k, 0) < m}
        report[fn] = dict(coverage=dict(sorted(cov.items())), shortfalls=short)
        if short:
            short_all[fn] = short
    print(json.dumps(report, indent=1))
    if short_all:
        raise SystemExit('cobertura insuficiente: %s' % short_all)
    print('ok')


if __name__ == '__main__':
    main()
