"""Congela la lista de bloqueos a revisar sin cambiar el catálogo."""
import csv
import json
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
saved=HERE/'catalog-locks-before.csv'
if saved.exists():
    rows=list(csv.DictReader(saved.open(encoding='utf-8-sig')))
else:
    rows=[r for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')) if r['locked']=='1']
import sys
sys.path.insert(0,str(HERE))
from retired import EXPECTED_REVIEWS,FROZEN_TOTAL,RETIRED
assert len(rows)==FROZEN_TOTAL,'El inventario inicial debe conservar sus 421 filas'
rows=[r for r in rows if (r['class'],r['fn_name']) not in RETIRED]
assert len(rows)==EXPECTED_REVIEWS,'El inventario revisado debe ser el congelado menos las bajas justificadas'
header='#pragma once\nnamespace HookUnlockAuditLive {\nstruct Target { const char* cls; const char* name; };\ninline constexpr Target targets[]={\n'
header+=''.join('{'+json.dumps(r['class'])+','+json.dumps(r['fn_name'])+'},\n' for r in rows)
(HERE/'Targets.hpp').write_text(header+'};\n}\n',encoding='utf-8',newline='\n')
if not saved.exists():
    with saved.open('w',encoding='utf-8',newline='') as stream:
        writer=csv.DictWriter(stream,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)
print(len(rows))
