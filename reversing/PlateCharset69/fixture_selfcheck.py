"""Autoprueba cruzada del banco PlateCharset69 SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 16384 filas `input`, comprueba las constantes de
arena contra la suma a mano de este archivo, pasa cada fila por el oraculo (fixture_oracle.py: no debe aparecer ninguna linea vacia o de solo separadores, que en el EXE llevan a atoi(NULL))
y calcula la cobertura prevista con las mismas claves que run_local.py.

    python -I fixture_selfcheck.py            # imprime la cobertura prevista y 'ok'
    python -I fixture_selfcheck.py --write    # ademas escribe coverage.json con los minimos (60 % de lo previsto, con un minimo de 1)
Necesita Visual Studio (vcvars64.bat); solo compila C++ estandar, no el proyecto.
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VCVARS = next(Path('C:/Program Files/Microsoft Visual Studio').glob('*/*/VC/Auxiliary/Build/vcvars64.bat'))
# constantes de arena sumadas a mano: relleno 0x50 + 40 dwords + guarda 0x40
LAYOUT = {'arena': 0x50 + 40 * 4 + 0x40, 'out_words': 40, 'max_lines': 40, 'line_buf': 1024}


def dump_rows():
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / 'fixture_dump.exe'
        cmd = f'"{VCVARS}" >nul && cl /nologo /EHsc /O2 /std:c++20 /I"{HERE}" /Fo"{tmp}\\\\" /Fe"{exe}" "{HERE / "fixture_dump.cpp"}"'
        subprocess.run(f'cmd /s /c "{cmd}"', check=True, stdout=subprocess.DEVNULL)
        out = subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout
    return [json.loads(line) for line in out.splitlines()]


def main():
    rows = dump_rows()
    assert rows[0]['layout'] == LAYOUT, (rows[0]['layout'], LAYOUT)
    rows = rows[1:]
    assert len(rows) == oracle.CASES and [r['n'] for r in rows] == list(range(oracle.CASES))
    cov = {}
    fam = {}
    for r in rows:
        obs, words, st = oracle.expected(r)
        assert len(words) <= 40
        fam[r['family']] = fam.get(r['family'], 0) + 1
        for k in st:
            cov[k] = cov.get(k, 0) + 1
    assert len(fam) == 16 and all(v == oracle.CASES // 16 for v in fam.values()), fam
    for k in sorted(cov):
        print('   %-18s %5d' % (k, cov[k]))
    if '--write' in sys.argv:
        out = {k: max(1, int(v * 0.6)) for k, v in cov.items()}
        (HERE / 'coverage.json').write_text(json.dumps(out, indent=1), encoding='utf-8')
    print('ok')


if __name__ == '__main__':
    main()
