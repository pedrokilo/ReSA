"""Autoprueba cruzada del banco LeaveCarFirst70 SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 16384 filas `input`, comprueba las constantes
de arena contra la suma a mano de este archivo, pasa cada fila por el oraculo (fixture_oracle.py) y calcula la cobertura de ramas con las mismas claves que run_local.py.

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
import fixture_oracle as oracle

VCVARS = next(Path('C:/Program Files/Microsoft Visual Studio').glob('*/*/VC/Auxiliary/Build/vcvars64.bat'))
# constantes de arena sumadas a mano (guarda + ventana + guarda)
LAYOUT = {'guard': 0x40, 'task': 0x40 + 0x34 + 0x40, 'ped': 0x40 + 0x5A0 + 0x40, 'veh': 0x40 + 0x5A0 + 0x40, 'grp': 0x40 + 0x40 + 0x40, 'ev': 0x40 + 0x20 + 0x40, 'alloc': 0x40 + 0x70 + 0x40, 'cases': 16384}


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
    for n, row in enumerate(rows):
        _, stats = oracle.expected(oracle.NAME, row, n)
        for k, v in stats.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
        fam[row['family']] = fam.get(row['family'], 0) + 1
    assert len(fam) == 16 and min(fam.values()) == max(fam.values()) == 1024, fam
    for k in sorted(cov):
        print('%-32s %d' % (k, cov[k]))
    missing = [k for k in ('pause_player', 'arrested_state', 'arrested_flag_only', 'arrested_not_player', 'not_in_car', 'not_driver', 'no_group', 'not_leader', 'leader_event', 'leader_event_null',
                           'radio', 'driver_player', 'audio_target', 'audio_other', 'audio_self', 'audio_null', 'built_boat', 'built_drive', 'built_pause', 'new_task_null_pause', 'new_task_null_boat',
                           'new_task_null_drive', 'player_false_garbage', 'leader_false_garbage', 'vtype_low_byte_5', 'state_low_byte_3f', 'delay_extreme') if cov.get(k, 0) < 100]
    assert not missing, missing
    if '--write' in sys.argv:
        data = {k: max(1, int(v * 0.5)) for k, v in sorted(cov.items())}
        (HERE / 'coverage.json').write_text(json.dumps(data, indent=2) + '\n')
        print('coverage.json escrito')
    print('ok')


if __name__ == '__main__':
    main()
