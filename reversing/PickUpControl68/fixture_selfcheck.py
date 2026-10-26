"""Autoprueba del banco PickUpControl68 SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 16384 filas `input`, comprueba las constantes
de arena contra la suma a mano de este archivo y calcula la cobertura de ramas prevista con el modelo (events_model/predict) de run_local.py.

    python -I fixture_selfcheck.py            # imprime la cobertura prevista y 'ok'
    python -I fixture_selfcheck.py --write    # ademas escribe coverage.json con los minimos (50 % de lo previsto, con un minimo de 1)
Necesita Visual Studio (vcvars64.bat); solo compila C++ estandar, no el proyecto.
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import run_local

VCVARS = next(Path('C:/Program Files/Microsoft Visual Studio').glob('*/*/VC/Auxiliary/Build/vcvars64.bat'))
# constantes de arena sumadas a mano (guarda + ventana + guarda)
LAYOUT = {'guard': 0x40, 'task': 0x40 + 0x34 + 0x40, 'sub': 0x40 + 0x40 + 0x40, 'ped': 0x40 + 0x490 + 0x40, 'mat': 0x40 + 0x48 + 0x40, 'pad': 0x40 + 0x40 + 0x40,
          'block': 0x40 + 0x20 + 0x40, 'groups': 0x40 + 256 * 0x14 + 0x40, 'cases': 16384}


def dump_rows():
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / 'fixture_dump.exe'
        cmd = f'"{VCVARS}" >nul && cl /nologo /EHsc /O2 /std:c++20 /I"{HERE}" /Fo"{tmp}\\\\" /Fe"{exe}" "{HERE / "fixture_dump.cpp"}"'
        subprocess.run(f'cmd /s /c "{cmd}"', check=True, stdout=subprocess.DEVNULL)
        out = subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout
    lines = [json.loads(x) for x in out.splitlines()]
    return lines[0]['layout'], [x['row'] for x in lines[1:]]


def main():
    layout, rows = dump_rows()
    assert layout == LAYOUT, (layout, LAYOUT)
    assert len(rows) == LAYOUT['cases']
    cov = {}
    fam = {}
    for row in rows:
        ev = run_local.events_model(row)
        ret = run_local.predict(row)
        assert (run_local.EV_ABORT in ev) == (ret == 0 and row['sub_kind'] == 1), (row, ev, ret)
        run_local.tally(row, [[k] for k in ev], ret, cov)
        fam[row['family']] = fam.get(row['family'], 0) + 1
    assert len(fam) == 16 and min(fam.values()) >= 1000, fam
    for k in sorted(cov):
        print('%-32s %d' % (k, cov[k]))
    missing = [k for k in ('block_request', 'block_addref', 'block_by_name', 'type_other', 'pick_assoc', 'pick_dropped', 'pick_in_time', 'pick_abort', 'goto_abort_time', 'goto_no_playerdata',
                           'goto_no_pad', 'goto_jump_abort', 'goto_sprint_abort', 'goto_short_stick', 'goto_angle_abort', 'goto_angle_return', 'goto_adjust_minus', 'goto_adjust_plus',
                           'goto_no_adjust', 'goto_nan_angle', 'goto_nan_heading', 'result_subtask', 'result_null', 'no_subtask') if cov.get(k, 0) < 100]
    assert not missing, missing
    if '--write' in sys.argv:
        data = {run_local.NAMES[0]: {k: max(1, int(v * 0.5)) for k, v in sorted(cov.items())}}
        (HERE / 'coverage.json').write_text(json.dumps(data, indent=2) + '\n')
        print('coverage.json escrito')
    print('ok')


if __name__ == '__main__':
    main()
