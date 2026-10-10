"""Corrige sólo flags locked del inventario inicial, con evidencia runtime verificada."""
import argparse
import csv
import hashlib
import json
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
APPROVED={('CEntryExit','GetPositionRelativeToOutsideWorld'),('CPostEffects','ImmediateModeRenderStatesStore')}
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('run',type=Path)
args=parser.parse_args()
summary=json.loads((args.run/'summary.json').read_text())
assert all(summary.get(k) is True for k in ('passed','all_equal','state_restored','saves_unchanged','configs_restored','posteffects_passed'))
log=args.run/'comparisons.jsonl'
assert hashlib.sha256(log.read_bytes()).hexdigest()==summary['log_sha256']
events=[json.loads(s) for s in log.read_text().splitlines()]
reviews={(r['class'],r['fn_name']):r for r in events if r['event']=='catalog_lock_review'}
before=list(csv.DictReader((HERE/'catalog-locks-before.csv').open(encoding='utf-8-sig')))
assert len(reviews)==len(before)==421
runtime={(r['class'],r['fn_name']):r for r in events if r['event']=='runtime_hook'}
assert len(runtime)==sum(r['event']=='runtime_hook' for r in events), 'Identidad runtime ambigua'
with (ROOT/'docs/hooks.csv').open(encoding='utf-8-sig',newline='') as stream:
    reader=csv.DictReader(stream);fields=reader.fieldnames;rows=list(reader)
changes=[]
for row in rows:
    key=(row['class'],row['fn_name'])
    if key not in reviews:continue
    observed=reviews[key]['runtime_locked']
    expected=False if key in APPROVED else observed
    if row['locked']!=str(int(expected)):
        changes.append(key);row['locked']=str(int(expected))
with (ROOT/'docs/hooks.csv').open('w',encoding='utf-8',newline='') as stream:
    writer=csv.DictWriter(stream,fieldnames=fields,lineterminator='\n');writer.writeheader();writer.writerows(rows)
assert sum(r['locked']=='1' for r in rows)==6
decision=[]
for row in before:
    key=(row['class'],row['fn_name']);r=reviews[key]
    if key in APPROVED:
        verdict='DESBLOQUEO REAL APROBADO EN BANCO Y FOTOGRAMAS'
        reason='EXE/DLL, pila y estado restaurados; contratos del EXE contrastados; alcance y límites en README'
        final=0
    elif not r['runtime_locked']:
        verdict='CATALOGO OBSOLETO: FUENTE Y RUNTIME YA DESBLOQUEADOS'
        reason='No se retira ningún bloqueo de la fuente; no acredita nueva seguridad de conmutación'
        final=0
    else:
        verdict='BLOQUEO CONSERVADO: SEGURIDAD OFF NO ACREDITADA'
        reason=('Concurrencia/audio y mezclas individuales no cubiertas' if row['class']=='CAEVehicleAudioEntity'
                else 'Bucle/render activo, transiciones menú/partida/foreground no cubiertas')
        final=1
    decision.append(dict(class_name=key[0],fn_name=key[1],address=row['address'],type=row['type'],
        catalog_locked_before=1,runtime_locked_observed=int(r['runtime_locked']),locked_final=final,
        decision=verdict,reason=reason,run=args.run.name,evidence='runs/'+args.run.name+'/comparisons.jsonl.gz'))
with (HERE/'lock-decisions.csv').open('w',encoding='utf-8',newline='') as stream:
    writer=csv.DictWriter(stream,fieldnames=list(decision[0]),lineterminator='\n');writer.writeheader();writer.writerows(decision)
print(json.dumps(dict(reviewed=421,flags_changed=len(changes),remaining_locks=6,real_unlocks=2,
                     previously_unlocked=413,validated=sum(r['validated']=='1' for r in rows))))
