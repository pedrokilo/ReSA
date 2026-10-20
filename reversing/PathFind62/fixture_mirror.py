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
ARENAS = {'this_bytes': 0x3C80, 'this_off': 0x40, 'this_arena': 0x40 + 0x3C80 + 0x40, 'node_bytes': 0x1C, 'max_nodes': 24, 'nodes_bytes': 0x2A0, 'nodes_off': 0x40,
          'nodes_arena': 0x40 + 0x2A0 + 0x40, 'max_links': 160, 'links_bytes': 0x280, 'links_off': 0x40, 'links_arena': 0x40 + 0x280 + 0x40, 'max_slots': 5,
          'malloc_bytes': 0x2800, 'malloc_off': 0x40, 'malloc_arena': 0x40 + 0x2800 + 0x40, 'glob_a_addr': 0x96EA98, 'glob_a_bytes': 0x96F050 - 0x96EA98,
          'glob_b_addr': 0x977B70, 'glob_b_bytes': 0x30, 'off_path_nodes': 0x804, 'off_node_links': 0xA44, 'off_num_nodes': 0xFA4, 'off_num_veh': 0x10C4, 'off_num_ped': 0x11E4,
          'off_interior_ids': 0x3548, 'script_n': 8, 'ground_n': 12}
assert 0x40 + 0x3C80 + 0x40 == 0x3D00 and 0x40 + 0x2A0 + 0x40 == 0x320 and 0x40 + 0x280 + 0x40 == 0x300 and 0x40 + 0x2800 + 0x40 == 0x2880 and 0x96F050 - 0x96EA98 == 0x5B8
assert (oracle.THIS_OFF, oracle.THIS_ARENA, oracle.NODES_OFF, oracle.NODES_ARENA, oracle.LINKS_OFF, oracle.LINKS_ARENA, oracle.MALLOC_OFF, oracle.MALLOC_ARENA, oracle.MALLOC_BYTES,
        oracle.GLOB_A_ADDR, oracle.GLOB_A_BYTES, oracle.GLOB_B_ADDR, oracle.GLOB_B_BYTES) == (0x40, 0x3D00, 0x40, 0x320, 0x40, 0x300, 0x40, 0x2880, 0x2800, 0x96EA98, 0x5B8, 0x977B70, 0x30)
assert (oracle.SMALL_OBJ, oracle.SMALL_ARENA) == (0x40, 0x90)


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcpathfind62-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return res.stdout.splitlines()


def coverage(lines=None):
    lines = lines or build_and_run()
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
            c['stack_seed'] = c.get('stack_seed', 0) + 1
            for key in ('family',):
                c['%s%d' % (key, row['family'])] = c.get('%s%d' % (key, row['family']), 0) + 1
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
