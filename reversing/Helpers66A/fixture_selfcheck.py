"""Autoprueba cruzada del banco Helpers66A SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 5 x 4096 filas `input`, comprueba las constantes
de arena contra la suma a mano de este archivo, pasa cada fila por el oraculo (fixture_oracle.py) y calcula la cobertura de ramas con las mismas claves que run_local.py.

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
import run_local

VCVARS = next(Path('C:/Program Files/Microsoft Visual Studio').glob('*/*/VC/Auxiliary/Build/vcvars64.bat'))
# constantes de arena sumadas a mano (guarda + ventana + guarda)
LAYOUT = {'guard': 0x40, 'ent': 0x40 + 0x40 + 0x40, 'mat': 0x40 + 0x40 + 0x40, 'vec': 0x40 + 0x10 + 0x40, 'ped': 0x40 + 0x7A0 + 0x40, 'player': 0x40 + 0x40 + 0x40,
          'task': 0x40 + 0x44 + 0x40, 'cam': 0x40 + 0x88 + 0x40, 'smoke': 0x40 + 0x20 + 0x40, 'groups': 8 * 0x2D4, 'cases': 4096}


def dump_rows():
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / 'fixture_dump.exe'
        cmd = f'"{VCVARS}" >nul && cl /nologo /EHsc /O2 /std:c++20 /I"{HERE}" /Fo"{tmp}\\\\" /Fe"{exe}" "{HERE / "fixture_dump.cpp"}"'
        subprocess.run(f'cmd /s /c "{cmd}"', check=True, stdout=subprocess.DEVNULL)
        out = subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout
    return [json.loads(line) for line in out.splitlines()]


def pseudo_row(name, inp, exp):
    """Fila con el aspecto de la del banco, construida con el resultado del oraculo (para reutilizar run_local.tally)."""
    obs = {'events': exp['events']}
    if 'al' in exp:
        obs['al'] = exp['al']
    if name == run_local.NAMES[1]:
        obs['changes'] = [[], [], [], [], [], [1] if exp['stored'] else [], exp['prio_changes']]
    return {'input': inp, 'candidate': {'observed': obs}}


def main():
    rows = dump_rows()
    layout = rows[0]['layout']
    assert layout == LAYOUT, (layout, LAYOUT)
    cov = {n: {} for n in run_local.NAMES}
    fam = {n: {} for n in run_local.NAMES}
    for r in rows[1:]:
        name = run_local.NAMES[r['func']]
        inp = r['row']
        exp = oracle.expected(name, inp)
        run_local.tally(name, pseudo_row(name, inp, exp), cov[name])
        fam[name][inp['family']] = fam[name].get(inp['family'], 0) + 1
    for name in run_local.NAMES:
        assert len(fam[name]) == 16 and all(v == 256 for v in fam[name].values()), (name, fam[name])
        print(name)
        for key in sorted(cov[name]):
            print('   %-24s %5d' % (key, cov[name][key]))
    if '--write' in sys.argv:
        out = {n: {k: max(1, int(v * 0.6)) for k, v in cov[n].items()} for n in run_local.NAMES}
        (HERE / 'coverage.json').write_text(json.dumps(out, indent=1), encoding='utf-8')
    print('ok')


if __name__ == '__main__':
    main()
