"""Clasifica las funciones del código del juego (0x401000–0x74FFFF) que el mapa aún da por pendientes: docs/JuegoExe/faltantes-clasificadas.csv.

Para cada función pendiente (no reemplazada y no excluida) indica:
  * tiene_hook: ya hay una fila con su dirección en docs/hooks.csv (el mapa no la cuenta cuando la clase es de biblioteca, p. ej. RpWorld);
  * categoria: salto_puro, usercall_registros, funclet_catch, mencionada_otro o sin_informe, según lo que digan los informes
    reversing/PendienteValidar/<lote>.md que citan su dirección;
  * posible_auxiliar_sin_hook: el informe habla de una auxiliar/función local reconstruida sin hook;
  * lotes_que_la_citan.
"""
import csv
import glob
import re
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import exe_coverage as ec  # noqa: E402


def main(root='.'):
    root = Path(root)
    out, st = ec.compute(root, internals=True)
    funcs, cov = st['funcs'], st['covered']
    ex = set((out.get('game_excluded') or {}).keys())
    pend = sorted(va for va in funcs if ec.region(va) == 'juego' and va not in cov and va not in ex)
    hooks = {}
    with open(root / 'docs/hooks.csv', encoding='utf-8', newline='') as f:
        for r in csv.reader(f):
            if len(r) > 6:
                hooks.setdefault(r[2].lower(), []).append(r[0])
    docs = {Path(f).stem: open(f, encoding='utf-8').read().split('\n') for f in glob.glob(str(root / 'reversing/PendienteValidar/*.md'))}
    rows, cat = [], Counter()
    for va in pend:
        key = f'{va:X}'.upper()
        c = [(s, l) for s, L in docs.items() for l in L if re.search(r'0x0*' + key + r'\b', l, re.I)]
        txt = ' '.join(l for _, l in c).lower()
        if not c:
            k = 'sin_informe'
        elif re.search(r'salto puro|jmp puro|un unico jmp', txt) and funcs[va]['size'] <= 8:
            k = 'salto_puro'
        elif re.search(r'usercall|registro|esi|edi|ebx|pushad|eax =|eax=', txt):
            k = 'usercall_registros'
        elif re.search(r'catch|funclet|manejador', txt):
            k = 'funclet_catch'
        else:
            k = 'mencionada_otro'
        aux = bool(re.search(r'auxiliar|sin hook|helper|funcion local|funciones propias', txt))
        h = hooks.get(f'0x{va:08x}')
        rows.append([f'0x{va:08X}', funcs[va]['size'], 'si' if h else '', h[0] if h else '', k, 'si' if aux else '', ','.join(sorted({s for s, _ in c}))[:80]])
        cat[('con_hook' if h else 'sin_hook', k)] += 1
    with open(root / 'docs/JuegoExe/faltantes-clasificadas.csv', 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['va', 'bytes', 'tiene_hook', 'clase_del_hook', 'categoria', 'posible_auxiliar_sin_hook', 'lotes_que_la_citan'])
        w.writerows(rows)
    print(len(pend), 'pendientes en el mapa;', sum(v for (a, _), v in cat.items() if a == 'con_hook'), 'ya tienen hook')
    for k, v in sorted(cat.items()):
        print(k, v)


if __name__ == '__main__':
    main(Path(__file__).resolve().parents[1])
