"""Informe de diferencias de una corrida: python -I diff_report.py <comparisons.jsonl> [max_por_funcion]"""
import json, sys, collections
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as oracle
path = sys.argv[1]; mx = int(sys.argv[2]) if len(sys.argv) > 2 else 3
shown = collections.Counter(); stats = collections.Counter(); oracle_bad = collections.Counter()
for line in open(path, encoding='utf-8'):
    r = json.loads(line)
    if r.get('event') != 'comparison':
        continue
    fn = r['function']
    o, c = r['original'], r['candidate']
    gates = [k for k in ('stack_restored','nonvolatile_preserved','direction_flag_clear','seh_restored','route_ok','collaborators_dll','globals_restored','floating_state_restored') if not (o.get(k) is True and c.get(k) is True)]
    why = None
    if o != c or gates or r['fp_difference']:
        stats[fn] += 1
        if shown[fn] < mx:
            shown[fn] += 1
            print('== EXE vs DLL', fn, 'caso', r['case'], 'gates', gates, 'fp', r['fp_difference'])
            for k in ('ret', 'events', 'changes', 'gchanges'):
                if o['observed'].get(k) != c['observed'].get(k):
                    print('  ', k, '\n     EXE:', json.dumps(o['observed'].get(k))[:900], '\n     DLL:', json.dumps(c['observed'].get(k))[:900])
            print('   input a[:34]=', r['input']['a'][:34])
    bad = oracle.check_row(r)
    if bad:
        oracle_bad[(fn, bad[0])] += 1
        if oracle_bad[(fn, bad[0])] <= 2:
            print('-- ORACULO', fn, r['case'], bad)
print('pares distintos por funcion:', dict(stats))
print('lados fuera del oraculo:', {f'{k[0]}:{k[1]}': v for k, v in oracle_bad.items()})
