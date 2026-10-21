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
# Constantes de disposicion segun el oraculo / el desensamblado (sumas a mano): 0xC87E48 + 16 * 0x18 = 0xC87FC8; (0xC87FC8 - 0xC87C60) / 4 = 218; (0xC87FE8 - 0xC87C60) / 4 = 226
LAYOUT = {'region_words': 226, 'tex_base': 0xC87C68, 'tex_stride': 0x1C, 'ib_base': 0xC87E48, 'ib_stride': 0x18, 'ib_buffers': 16, 'gen': 0xC87FC8, 'gen_word': 218,
          'region_base': 0xC87C60, 'max_events': 520}
assert 0xC87E48 + 16 * 0x18 == 0xC87FC8 and (0xC87FC8 - 0xC87C60) // 4 == 218 and (0xC87FE8 - 0xC87C60) // 4 == 226
assert oracle.TEX_BASE == LAYOUT['tex_base'] and oracle.IB_BASE == LAYOUT['ib_base'] and oracle.GEN == LAYOUT['gen'] and oracle.GEN_WORD == LAYOUT['gen_word'] and oracle.MAX_EVENTS == LAYOUT['max_events']


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcd3d63-'))
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
        if k == 'R':
            mode, seed, kk, val = json.loads(js)
            if oracle.ret_val(mode, seed, kk) != val:
                raise SystemExit('RetVal distinto entre C++ y Python: modo %d semilla %d k %d: %d / %d' % (mode, seed, kk, val, oracle.ret_val(mode, seed, kk)))
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
