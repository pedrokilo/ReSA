"""Autoprueba del banco Fn238A SIN juego: genera los casos (fixture_cases.py) y los pasa por el oraculo (fixture_oracle.py); cuenta cobertura por comando y rechaza los casos que el modelo no puede evaluar.

    python -I fixture_selfcheck.py [casos_por_funcion]
"""
import sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_cases as G
import fixture_oracle as O

if __name__ == '__main__':
    cases = int(sys.argv[1]) if len(sys.argv) > 1 else 16384
    cov, bad, fam = {}, {}, {}
    n_ok = 0
    for fn in (0, 1):
        for n in range(cases):
            c = G.make_case(fn, n)
            try:
                e = O.expected(c)
            except O.Bad as ex:
                bad.setdefault((fn, c['args'][0] if fn else 0), []).append((n, str(ex)))
                continue
            except Exception as ex:
                bad.setdefault((fn, 'EXC'), []).append((n, repr(ex)))
                continue
            n_ok += 1
            for m in set(e['marks']):
                cov[m] = cov.get(m, 0) + 1
            fam[(fn, c['family'])] = fam.get((fn, c['family']), 0) + 1
    for k, v in bad.items():
        print('CASOS INVALIDOS', k, len(v), v[:2])
    cmds = {k: v for k, v in cov.items() if k.startswith('cmd')}
    print(n_ok, 'casos evaluables;', len(cmds), 'comandos distintos; minimo por comando', min(cmds.values()) if cmds else 0)
    print({k: v for k, v in cov.items() if not k.startswith('cmd') and not k.startswith('family')})
    print('ok' if not bad else 'CON PROBLEMAS')
