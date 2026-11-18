"""Diagnostico de una corrida: por funcion, pares con EXE != DLL, lados fuera del oraculo y primeros ejemplos.  python -I diag.py <comparisons.jsonl> [max_ejemplos]"""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as o
mx = int(sys.argv[2]) if len(sys.argv) > 2 else 2
stat = {}
for line in open(sys.argv[1], encoding='utf-8'):
    if '"event":"comparison"' not in line:
        continue
    r = json.loads(line)
    f = r['function']
    s = stat.setdefault(f, dict(n=0, diff=0, exe_bad=0, dll_bad=0, ex=[]))
    s['n'] += 1
    e = o.expected(r['input'])
    ob, cb = r['original'], r['candidate']
    d = ob != cb
    xe = o.check_side(r['input'], ob['observed'], e, ob)
    xc = o.check_side(r['input'], cb['observed'], e, cb)
    s['diff'] += d; s['exe_bad'] += bool(xe); s['dll_bad'] += bool(xc)
    if (d or xe or xc) and len(s['ex']) < mx:
        gates = {k: (ob.get(k), cb.get(k)) for k in o.__dict__ and ('stack_restored','nonvolatile_preserved','direction_flag_clear','seh_restored','route_ok','collaborators_dll','globals_restored','floating_state_restored') if ob.get(k) is not True or cb.get(k) is not True}
        s['ex'].append((r['case'], 'EXE!=DLL' if d else '', ('exe:' + xe[:230]) if xe else '', ('dll:' + xc[:230]) if xc else '', gates))
for f, s in stat.items():
    print(f, 'n', s['n'], 'EXE!=DLL', s['diff'], 'EXE fuera del oraculo', s['exe_bad'], 'DLL fuera', s['dll_bad'])
    for x in s['ex']:
        print('   ', x)
