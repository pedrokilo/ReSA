"""Autoprueba del banco LeaveCarAbort70 SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 16384 filas `input`, comprueba las constantes
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
LAYOUT = {'guard': 0x40, 'task': 0x40 + 0x34 + 0x40, 'sub': 0x40 + 0x40 + 0x40, 'veh': 0x40 + 0x600 + 0x40, 'ped': 0x900 + 0x2000 + 0x40, 'mat': 0x40 + 0x48 + 0x40,
          'ev': 0x40 + 0x50 + 0x40, 'cases': 16384}


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
        al, ev = run_local.model(row)
        run_local.tally(row, [[k] for k in ev], al, cov)
        fam[row['family']] = fam.get(row['family'], 0) + 1
        # el modelo debe ser coherente con la prioridad
        if row['veh_null']:
            assert (al, ev) == (1, [])
    assert len(fam) == 16 and min(fam.values()) >= 1000, fam
    for k in sorted(cov):
        print('%-32s %d' % (k, cov[k]))
    missing = [k for k in ('no_vehicle', 'prio_other', 'imm_not_in_vehicle', 'imm_in_vehicle', 'imm_door_given', 'imm_door_computed', 'die_drive_false', 'die_abort_ok', 'die_abort_fail',
                           'wait_abort', 'wait_not_killed', 'getup_abort', 'getup_not_killed', 'jump_type', 'jump_no_event', 'jump_event_other', 'jump_final_abort', 'jump_abort_fail', 'jump_air',
                           'jump_water_first', 'jump_parachute_wheels', 'jump_parachute_plane', 'jump_parachute_fast', 'jump_parachute_player', 'jump_parachute_not_player', 'jump_parachute_vert',
                           'jump_parachute_blend', 'jump_parachute_ground', 'jump_wheels_nonzero', 'jump_slot_negative', 'jump_slot_oob', 'jump_water_blend', 'jump_water_no_blend', 'mag_nan', 'vz_nan',
                           'final_no_event', 'final_drive_abort', 'final_drive_other', 'result_true', 'result_false', 'result_odd') if cov.get(k, 0) < 100]
    assert not missing, missing
    if '--write' in sys.argv:
        data = {run_local.NAMES[0]: {k: max(1, int(v * 0.5)) for k, v in sorted(cov.items())}}
        (HERE / 'coverage.json').write_text(json.dumps(data, indent=2) + chr(10))
        print('coverage.json escrito')
    print('ok')


if __name__ == '__main__':
    main()
