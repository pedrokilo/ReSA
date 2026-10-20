"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), compara las constantes de arena / reparto de casos del C++ con la suma a mano de
fixture_oracle.py (autoprueba cruzada) y comprueba que ningun caso pide una combinacion que el EXE desreferenciaria sin comprobar nulo.
Uso:  python -I fixture_mirror.py
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


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pckillped62-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'm.exe'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if res.returncode != 0 or not res.stdout.strip():
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + res.stdout[-2000:] + res.stderr[-2000:])
    return [json.loads(l) for l in res.stdout.splitlines() if l.strip()]


def main():
    rows = build_and_run()
    dump = rows.pop(0)
    oracle.selftest(dump)
    assert dump['bytes'] == oracle.REGION_BYTES, (dump['bytes'], oracle.REGION_BYTES)
    assert len(rows) == oracle.CASES, len(rows)
    bad = []
    counts = {}
    for n, r in enumerate(rows):
        counts[r['route']] = counts.get(r['route'], 0) + 1
        if r['route'] == 3 and r['task_id'] == oracle.T_DESTROY and r['tgt_null']:
            bad.append((n, 'DestroyCar con objetivo nulo'))
        if r['route'] == 3 and r['task_id'] == oracle.T_ARMED and r['alloc_null']:
            bad.append((n, 'Armed con reserva nula'))
        if r['route'] == 0 and r['t_in_veh'] and not r['t_veh'] and r['sub_type'] in (oracle.T_MELEE, oracle.T_DESTROY):
            bad.append((n, 'Control: melee/destruir con objetivo en vehiculo nulo'))
    assert counts == {i: c for i, c in enumerate(oracle.CASES_BY_NAME.values())}, counts
    if bad:
        raise SystemExit('fixtures peligrosas: %s' % bad[:5])
    print(json.dumps(dict(cases=len(rows), per_route=counts, arena=dump['arena'])))
    print('ok')


if __name__ == '__main__':
    main()
