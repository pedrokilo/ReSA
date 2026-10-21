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
# Las regiones de memoria real, los tamanos de pool y el numero de clases de colaborador se contrastan con Fixtures.hpp (kRegions, kKinds, ...).


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pcmc64-'))
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
    want = {'kinds': oracle.KINDS, 'entries': oracle.K_ENTRIES, 'slots': oracle.K_SLOTS, 'veh': 0xA18, 'ped': 0x7C4, 'obj': 0x19C, 'regions': [list(r) for r in oracle.REGIONS],
            'seq': [oracle.SEQ_BASE, oracle.SEQ_STRIDE], 'fx': oracle.FX_BASE, 'pad': oracle.PAD}
    if sizes != want:
        raise SystemExit('Disposicion distinta entre Fixtures.hpp y el oraculo: %s' % {k: (sizes.get(k), want.get(k)) for k in want if sizes.get(k) != want.get(k)})
    rows = []
    for l in lines:
        k, js = l.split('	', 1)
        if k == 'R':
            seed, kk, val, bg = json.loads(js)
            if oracle.rand(seed, kk * 4 + 2) != val or oracle.bg_byte(seed, 0xA4448A + kk) != bg:
                raise SystemExit('Rand/BgByte distinto entre C++ y Python: semilla %d k %d' % (seed, kk))
            continue
        rows.append(json.loads(js))
    assert len(rows) == oracle.CASES_P + oracle.CASES_C, len(rows)
    cov = {}
    for row in rows:
        name = oracle.NAME_P if row['fn'] == 0 else oracle.NAME_C
        try:
            want_out, stats = oracle.expected(name, row, row['n'])
        except Exception as e:
            raise SystemExit('Fixture incoherente en %s caso %d: %r' % (name, row['n'], e))
        for key, v in stats.items():
            if v is True:
                cov[name + ':' + key] = cov.get(name + ':' + key, 0) + 1
        cov[name + ':cases'] = cov.get(name + ':cases', 0) + 1
        cov['stack_seed'] = cov.get('stack_seed', 0) + 1
    return cov


if __name__ == '__main__':
    cov = coverage()
    try:
        import run_local
        short = run_local.check_minimums(cov)
    except ImportError:
        short = {}
    print(json.dumps(dict(sorted(cov.items())), indent=0))
    if short:
        raise SystemExit('Cobertura por debajo de los minimos: ' + json.dumps(short))
    print('espejo ok')
