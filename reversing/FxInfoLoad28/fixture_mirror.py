"""Comprobacion de los fixtures del banco FxInfoLoad28 SIN el juego.

1) Compilar y ejecutar la herramienta estandar fixture_dump.cpp (incluye Fixtures.hpp real, no un espejo):
     cl /nologo /EHsc /std:c++20 /O2 /I<banco> <banco>\\fixture_dump.cpp /Fe:fixture_dump.exe  &&  fixture_dump.exe > casos.jsonl
2) python -I fixture_mirror.py casos.jsonl   -> 0 casos invalidos (el oraculo no lanza Fail) y la cobertura que usa run_local.py::MINIMUMS.
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle


def coverage(rows):
    cov, invalid = {}, []
    for n, inp in enumerate(rows):
        try:
            want, stats = oracle.expected(oracle.NAME, inp, n)
        except oracle.Fail as e:
            invalid.append((n, str(e)))
            continue
        for k, v in stats.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
        cov['kind%d' % inp['kind']] = cov.get('kind%d' % inp['kind'], 0) + 1
        cov['ver_ge1'] = cov.get('ver_ge1', 0) + (inp['version'] >= 1)
        cov['ver_lt1'] = cov.get('ver_lt1', 0) + (inp['version'] < 1)
        tb = sum(1 for o in want['objs'] if o[1] is None)
        cov['tm_unknown_objs'] = cov.get('tm_unknown_objs', 0) + tb
        cov['entries'] = cov.get('entries', 0) + len(want['objs'])
        cov['move_set'] = cov.get('move_set', 0) + (bytes.fromhex(want['mgr'])[8] != 0xFF and not stats.get('move_from_render'))
        cov['render_set'] = cov.get('render_set', 0) + (not stats.get('render_from_count'))
        cov['no_move'] = cov.get('no_move', 0) + bool(stats.get('move_from_render'))
        cov['no_render'] = cov.get('no_render', 0) + bool(stats.get('render_from_count'))
        types = {o[0] for o in want['objs']}
        for t in (0x1001, 0x2001, 0x4001, 0x8001):
            cov['type_%x' % t] = cov.get('type_%x' % t, 0) + (t in types)
    return cov, invalid


if __name__ == '__main__':
    rows = [json.loads(line) for line in Path(sys.argv[1]).read_text(encoding='ascii').splitlines() if line.strip()]
    cov, invalid = coverage(rows)
    print(json.dumps({'cases': len(rows), 'invalid': len(invalid), 'first_invalid': invalid[:10], 'coverage': dict(sorted(cov.items()))}, indent=1))
    sys.exit(1 if invalid else 0)
