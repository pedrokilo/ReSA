"""Resumen de diferencias de una corrida de FnPW1 (sin juego): compara cada lado (original = EXE, candidato = DLL) con el oraculo y clasifica los pares que difieren.

    python -I analizar_diferencias.py <comparisons.jsonl> [n_ejemplos]

Imprime: pares, pares EXE != DLL, EXE != oraculo, DLL != oraculo, compuertas falladas, y una tabla por (ruta con/sin rueda pinchada, campos que difieren).
"""
import collections
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as oracle

KEYS = ('rand_state', 'ev_hash', 'ev_count', 'events', 'wheel_state', 'flags')
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')


def hexs(ev):
    return [hex(x) for x in ev]


def main(path, examples=2):
    n = exe_dll = exe_or = dll_or = gates = 0
    cat = collections.Counter()
    shown = collections.Counter()
    fam = collections.Counter()
    for line in open(path, encoding='utf-8'):
        if '"event":"comparison"' not in line.replace(' ', ''):
            continue
        row = json.loads(line)
        n += 1
        e = oracle.expected(row['input'])
        a, c = row['original']['observed'], row['candidate']['observed']
        ok_a = all(a[k] == e[k] for k in KEYS) and not any(a['changes'])
        ok_c = all(c[k] == e[k] for k in KEYS) and not any(c['changes'])
        same = all(a[k] == c[k] for k in KEYS)
        exe_or += not ok_a
        dll_or += not ok_c
        exe_dll += not same
        gates += not all(row[s].get(k) is True for s in ('original', 'candidate') for k in GATES)
        if ok_c:
            continue
        bad = tuple(k for k in KEYS if c[k] != e[k])
        key = ('rueda pinchada' if 'burst' in e['path'].split('+') else 'sin pinchazo', bad)
        cat[key] += 1
        fam[row['input']['family']] += 1
        if shown[key] < examples:
            shown[key] += 1
            print('ejemplo', key, 'caso', row['case'], 'familia', row['input']['family'], e['path'])
            print('   esperado', [hexs(ev) for ev in e['events']], e['wheel_state'], e['flags'], hex(e['rand_state']))
            print('   DLL     ', [hexs(ev) for ev in c['events']], c['wheel_state'], c['flags'], hex(c['rand_state']))
    print('pares', n, 'EXE!=DLL', exe_dll, 'EXE!=oraculo', exe_or, 'DLL!=oraculo', dll_or, 'compuertas falladas', gates)
    for key, count in cat.most_common():
        print(count, key)
    print('por familia', sorted(fam.items()))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 2))
