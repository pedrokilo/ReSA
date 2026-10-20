"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), pasa cada fila `input` por el oraculo (que falla si una palabra que la funcion lee
no esta especificada en el caso), comprueba las constantes de disposicion del C++ (ultima linea) frente a las del oraculo y la cobertura frente a los minimos de run_local.py.
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
# Constantes de disposicion segun el oraculo / el desensamblado (sumas a mano; ver README.md)
LAYOUT = {'region_words': 226, 'tex_base': oracle.TEX_BASE, 'tex_stride': oracle.TEX_STRIDE, 'ib_base': oracle.IB_BASE, 'ib_stride': oracle.IB_STRIDE, 'gen_base': oracle.GEN_BASE,
          'free_tex': oracle.FREE_TEX, 'free_ib': oracle.FREE_IB, 'tex_buffers': 17, 'ib_buffers': 16, 'stacks': 34, 'stack_slots': 32, 'stack_guard': 8, 'stack_words': 48,
          'res_count': 128, 'arg_res': oracle.ARG, 'res_words': 8, 'new_blocks': 20, 'new_block_words': 16, 'out_words': 4, 'stack_of_tex_16': 16, 'stack_of_ib_0': 17, 'stack_of_ib_16': 33,
          'region_base': 0xC87C60, 'use_addr': 0x8D6084, 'device_addr': 0xC97C28}
# 0xC87C68 + 17 * 0x1C = 0xC87E44; 0xC87E48 + 16 * 0x18 = 0xC87FC8; (0xC87FE8 - 0xC87C60) / 4 = 226; 8 + 32 + 8 = 48
assert 0xC87C68 + 17 * 0x1C == 0xC87E44 and 0xC87E48 + 16 * 0x18 == 0xC87FC8 and (0xC87FE8 - 0xC87C60) // 4 == 226 and 8 + 32 + 8 == 48
assert (0xC87C68 - 0xC87C60) // 4 == oracle.TEX_BASE and (0xC87E48 - 0xC87C60) // 4 == oracle.IB_BASE and (0xC87FC8 - 0xC87C60) // 4 == oracle.GEN_BASE
assert (0xC87FE0 - 0xC87C60) // 4 == oracle.FREE_TEX and (0xC87FE4 - 0xC87C60) // 4 == oracle.FREE_IB


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcd3d62-'))
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
        per[names[int(k)]].append(json.loads(js))
    cov = {}
    for name in names:
        rows = per[name]
        assert len(rows) == oracle.CASES, (name, len(rows))
        c = {}
        for n, row in enumerate(rows):
            try:
                want, stats = oracle.expected(name, row, n)
            except KeyError as e:
                raise SystemExit('Fixture incoherente en %s caso %d: %s' % (name, n, e))
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
