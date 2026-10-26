"""Autoprueba cruzada del banco PedLeaf69 SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 4 x 4096 filas `input`, comprueba las constantes
de arena contra la suma a mano de este archivo, pasa cada fila por el oraculo (fixture_oracle.py) y calcula la cobertura prevista con las mismas claves que run_local.py.

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
# constantes de arena sumadas a mano (guarda + ventana + guarda)
LAYOUT = {'guard': 0x40, 'intel': 0x40 + 0x2C0 + 0x40, 'ent': 0x40 + 0x40 + 0x40, 'mat': 0x40 + 0x40 + 0x40, 'vec': 0x40 + 0x10 + 0x40, 'stats': 0x40 + 44 * 0x34 + 0x40, 'cases': 4096}


def dump_rows():
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / 'fixture_dump.exe'
        cmd = f'"{VCVARS}" >nul && cl /nologo /EHsc /O2 /std:c++20 /I"{HERE}" /Fo"{tmp}\\\\" /Fe"{exe}" "{HERE / "fixture_dump.cpp"}"'
        subprocess.run(f'cmd /s /c "{cmd}"', check=True, stdout=subprocess.DEVNULL)
        out = subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout
    return [json.loads(line) for line in out.splitlines()]


def main():
    rows = dump_rows()
    layout = rows[0]['layout']
    assert layout == LAYOUT, (layout, LAYOUT)
    cov = {n: {} for n in oracle.NAMES}
    fam = {n: {} for n in oracle.NAMES}
    for r in rows[1:]:
        name = oracle.NAMES[r['func']]
        inp = r['row']
        _, stats = oracle.expected(name, inp)
        for k, v in stats.items():
            if k != 'family' and v:
                cov[name][k] = cov[name].get(k, 0) + 1
        fam[name][inp['family']] = fam[name].get(inp['family'], 0) + 1
    for name in oracle.NAMES:
        assert len(fam[name]) == 16 and all(v == 256 for v in fam[name].values()), (name, fam[name])
        print(name)
        for key in sorted(cov[name]):
            print('   %-24s %5d' % (key, cov[name][key]))
    if '--write' in sys.argv:
        out = {n: {k: max(1, int(v * 0.6)) for k, v in cov[n].items()} for n in oracle.NAMES}
        (HERE / 'coverage.json').write_text(json.dumps(out, indent=1), encoding='utf-8')
    print('ok')


if __name__ == '__main__':
    main()
