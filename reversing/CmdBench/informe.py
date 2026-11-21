"""Resume uno o varios registros de CmdBench (comparisons.jsonl): por comando, casos, diferencias y su tipo.

Uso: python informe.py <carpeta-o-jsonl> [...] [--csv salida.csv] [--detalle COMANDO_ID]
Tipos de diferencia por caso: exc (una sola excepcion), ret (OpcodeResult), ip, cond, andor, out (variables de salida) y mem (bytes de .data/fixtures).
"""
import csv
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path


def rows_of(path):
    p = Path(path)
    if p.is_dir():
        p = p / 'comparisons.jsonl'
    for line in p.open('r', encoding='utf-8', errors='replace'):
        if not line.endswith('\n'):
            continue
        try:
            yield json.loads(line)
        except ValueError:
            pass


def kinds(r):
    c, e = r['cpp'], r['exe']
    k = []
    if c['exc'] != e['exc']:
        k.append('exc_cpp' if c['exc'] else 'exc_exe')
    if not c['exc'] and not e['exc']:
        if c['ret'] != e['ret']:
            k.append('ret')
        if c['ip'] != e['ip']:
            k.append('ip')
        if c['cond'] != e['cond']:
            k.append('cond')
        if c['andor'] != e['andor']:
            k.append('andor')
        if c.get('out') != e.get('out'):
            k.append('out')
        if c.get('seed') != e.get('seed'):
            k.append('rng')
        if r['diff_sample']:
            k.append('mem')
    return k or ['?']


def main():
    if '--desde' in sys.argv:   # carpetas de resultados-cmdbench desde AAAAMMDD-HHMMSS
        d = sys.argv[sys.argv.index('--desde') + 1]
        root = Path(r'E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cmdbench')
        sys.argv = [x for x in sys.argv if x not in ('--desde', d)] + [str(f) for f in sorted(root.iterdir()) if f.name >= d and (f / 'comparisons.jsonl').exists()]
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    out_csv = sys.argv[sys.argv.index('--csv') + 1] if '--csv' in sys.argv else None
    detail = int(sys.argv[sys.argv.index('--detalle') + 1]) if '--detalle' in sys.argv else None
    if out_csv:
        args = [a for a in args if a != out_csv]
    if detail is not None:
        args = [a for a in args if a != str(detail)]
    summ, kind, names = {}, defaultdict(Counter), {}
    shown = Counter()
    for a in args:
        for r in rows_of(a):
            ev = r.get('event')
            if ev == 'command_summary':
                summ[r['command']] = r
                names[r['command']] = r['name']
            elif ev == 'comparison':
                for k in kinds(r):
                    kind[r['command']][k] += 1
                if detail == r['command'] and shown[detail] < 6:
                    shown[detail] += 1
                    print(json.dumps({k: r[k] for k in ('case', 'mode', 'input', 'cpp', 'exe', 'diff_sample')}, ensure_ascii=False)[:1800])
    if detail is not None:
        return
    rows = []
    for cmd, s in sorted(summ.items()):
        rows.append([cmd, s['name'], s['cases'], s['equal'], s['diffs'], s['exc_cpp'], s['exc_exe'], s.get('assert_cpp', 0), s['exe_wrote_out'], s['exe_flag_set'], s['exe_mem_changed'],
                     ' '.join(f'{k}={v}' for k, v in sorted(kind[cmd].items()))])
    w = csv.writer(open(out_csv, 'w', newline='', encoding='utf-8') if out_csv else sys.stdout, lineterminator='\n')
    w.writerow(['id', 'comando', 'casos', 'iguales', 'diferencias', 'exc_cpp', 'exc_exe', 'assert_cpp', 'exe_escribe_salida', 'exe_pone_bandera', 'exe_cambia_memoria', 'tipos'])
    w.writerows(rows)


if __name__ == '__main__':
    main()
