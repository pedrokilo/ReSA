"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo, comprueba las constantes de disposicion,
el valor del falso de floor (FloorRetBits de C++ frente a floor_ret_bits de Python), el ensanchado float->double, la formula de Garbage y la cobertura frente a los minimos de run_local.py.
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
# Constantes de disposicion segun el oraculo (sumas a mano): 0x100 + 10 * 0x10 = 0x1A0; 0x400 + 4 * 350 + 4 = 0x968 <= 0x980; 0x980 + 64 = 0x9C0 <= 0xA00
LAYOUT = {'arena_words': 1024, 'frames': 0x100, 'frame_stride': 0x10, 'nodes': 10, 'found': 0x1A0, 'task_slot': 0x240, 'facial_slot': 0x244, 'indices': 0x400, 'index_count': 96,
          'name_zone': 0x980, 'name_bytes': 64, 'vector': 0xA00, 'value_slot': 0xA20, 'max_events': 64}
assert 0x100 + 10 * 0x10 == 0x1A0 and 0x400 + 4 * 350 + 4 <= 0x980 and 0x980 + 64 <= 0xA00 and 0xA20 + 4 <= 0x1000
assert (oracle.ARENA_WORDS, oracle.FRAMES, oracle.FRAME_STRIDE, oracle.NODES, oracle.FOUND, oracle.TASK_SLOT, oracle.FACIAL_SLOT, oracle.INDICES, oracle.INDEX_COUNT, oracle.NAME_ZONE,
        oracle.NAME_BYTES, oracle.VECTOR, oracle.VALUE_SLOT, oracle.MAX_EVENTS) == tuple(LAYOUT[k] for k in ('arena_words', 'frames', 'frame_stride', 'nodes', 'found', 'task_slot',
        'facial_slot', 'indices', 'index_count', 'name_zone', 'name_bytes', 'vector', 'value_slot', 'max_events'))


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pch65a-'))
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
    names = oracle.NAMES
    per = {n: [] for n in names}
    for l in lines:
        k, js = l.split('\t', 1)
        if k == 'F':
            mode, seed, lo, hi, val = json.loads(js)
            if oracle.floor_ret_bits(mode, seed, lo, hi) != val:
                raise SystemExit('FloorRetBits distinto entre C++ y Python: %s' % js)
            continue
        if k == 'W':
            x, val = json.loads(js)
            if oracle.widen_float(x) != val:
                raise SystemExit('WidenFloat distinto entre C++ y Python: %s' % js)
            continue
        if k == 'G':
            fn, case, w, val = json.loads(js)
            if oracle.garbage(oracle.image_seed(fn, case), w) != val:
                raise SystemExit('Garbage distinto entre C++ y Python: %s' % js)
            continue
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
            c['family%d' % row['fam']] = c.get('family%d' % row['fam'], 0) + 1
            c['stack_seed'] = c.get('stack_seed', 0) + 1
        cov[name] = c
    return cov


if __name__ == '__main__':
    cov = coverage()
    try:
        import run_local
        short = run_local.check_minimums(cov)
    except ImportError:
        short = {}
    for name, c in cov.items():
        print(name, json.dumps(dict(sorted(c.items()))))
    if short:
        raise SystemExit('Cobertura por debajo de los minimos: ' + json.dumps(short))
    print('espejo ok')
