"""Convierte las filas del arnes offline en un registro con el formato del banco dentro del juego (begin / comparison / function_summary / global_state_final / complete) para
ejercitar run_local.py (--analyze) sin ejecutar GTA. NO es evidencia: es solo una prueba de las compuertas del analizador.

Uso: python -I make_fake_log.py <carpeta con find/build/resp/cons.jsonl> <salida.jsonl>
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import fixture_oracle as oracle
import run_local

FILES = ((oracle.NF, 'find.jsonl'), (oracle.NB, 'build.jsonl'), (oracle.NR, 'resp.jsonl'), (oracle.NC, 'cons.jsonl'))


def main(folder, out):
    rows = []
    fp = '00' * 108
    stubbed = [{'exe': a, 'dll': 0 if a in run_local.EXE_ONLY else 0x20000000 + a} for a in sorted(run_local.STUBBED)]
    rows.append({'event': 'begin', 'registered_hook': True, 'registered': {n: True for n in run_local.HOOKS}, 'functions': 4, 'pairs': sum(run_local.EXPECTED.values()),
                 'cases': run_local.EXPECTED, 'direct_functions': [], 'compare_dead_x87_registers': False, 'compare_volatile_registers': False, 'x87_comparison': 'x',
                 'collaborators_stubbed': True, 'collaborators': {'exe_only': sorted(run_local.EXE_ONLY), 'stubbed': stubbed},
                 'watched_entries': [{'address': a, 'bytes': '00'} for a in sorted(run_local.WATCHED)], 'bodies': {n: 1 for n in run_local.HOOKS}, 'asi_module': 1,
                 'original_module': 2, 'initial_fp': fp, 'initial_mxcsr': 0})
    pair = 0
    for name, fname in FILES:
        n = 0
        for line in (Path(folder) / fname).read_text(encoding='utf-8').splitlines():
            r = json.loads(line)
            for side in ('original', 'candidate'):
                r[side]['route_ok'] = True
                r[side]['globals_restored'] = True
                r[side]['floating_state_restored'] = True
            r['pair'] = pair
            r['accepted'] = r['equal']
            pair += 1
            n += 1
            rows.append(r)
        rows.append({'event': 'function_summary', 'function': name, 'cases': n, 'failures': 0, 'state_restored': True})
    rows.append({'event': 'global_state_final', 'floating_state': fp, 'mxcsr': 0, 'state_restored': True, 'watched_intact': True})
    rows.append({'event': 'complete', 'pairs': pair, 'state_restored': True, 'floating_state_restored': True, 'entries_intact': True})
    Path(out).write_text('\n'.join(json.dumps(r) for r in rows) + '\n', encoding='utf-8')


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
