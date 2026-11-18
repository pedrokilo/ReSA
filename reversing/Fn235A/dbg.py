"""Depuracion de la autoprueba: python -I dbg.py <fn> [n...]   (vuelca los casos de la funcion y muestra los invalidos)"""
import json, subprocess, sys, tempfile
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
import fixture_selfcheck as sc
CACHE = HERE / 'artifacts' / 'rows.jsonl'
def rows():
    CACHE.parent.mkdir(exist_ok=True)
    src = HERE / 'Fixtures.hpp'
    if not CACHE.exists() or CACHE.stat().st_mtime < src.stat().st_mtime:
        r = sc.dump_rows()
        CACHE.write_text('\n'.join(json.dumps(x) for x in r), encoding='utf-8')
    return [json.loads(l) for l in CACHE.read_text(encoding='utf-8').splitlines()]
if __name__ == '__main__':
    fn = int(sys.argv[1])
    sel = [r for r in rows() if r['fn'] == fn]
    bad = 0
    for r in sel:
        try:
            oracle.expected(r)
        except oracle.Bad as ex:
            bad += 1
            if bad <= 3 or r['n'] in map(int, sys.argv[2:]):
                print(r['n'], ex, 'a=', r['a'][:32], 'r=', r['r'][:12])
    print('invalidos', bad)
