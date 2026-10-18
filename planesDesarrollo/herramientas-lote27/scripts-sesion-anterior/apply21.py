import csv, json
from pathlib import Path
ROOT=Path('D:/Original/gta-reversed-master'); H=ROOT/'reversing/OriginalDependencies499'
banks={}
for b in 'ABCDEFG':
    s=json.loads((ROOT/f'reversing/Dependencies21{b}/validation-summary.json').read_text(encoding='utf-8'))
    for fn,n in s['functions'].items(): banks[fn]=(f'Dependencies21{b}',n,s['run'].split('/')[-1],f'reversing/Dependencies21{b}/validation-summary.json')
hooks={int(r['address'],16):r['class']+'::'+r['fn_name'] for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig',newline=''))}
prog=list(csv.DictReader((H/'progress.csv').open(encoding='utf-8',newline='')))
unk=[r['address'] for r in prog if r['category']=='unknown']
assert len(unk)==21
def bank_of(ident):
    k=ident if ident in banks else ident.split('-')[0]
    return banks[k]
info={}
for a in unk:
    ident=hooks[int(a,16)]; bank,n,run,summ=bank_of(ident)
    cat='library' if a=='0x00821982' else 'game'
    info[a]=dict(identity=ident,category=cat,bank=bank,pairs=n,run=run,summary=summ)
def note(i,full):
    cat='biblioteca (CRT estática de MSVC), reconstruida como envoltorio C++ y contada como biblioteca con seguimiento separado' if i['category']=='library' else 'función del juego'
    t=(f"Resuelta en PLAN-001 (17/10/2026): {i['identity']}; {cat}. Reconstruida en C++ con hook RH; banco {i['bank']}, "
       f"{i['pairs']} pares RH apagado/activo, 0 diferencias")
    if full: t+=f" (corrida reversing/CppTwelveLevel6Final/runs/{i['run']})"
    return t+". Clasificación y evidencias en reversing/OriginalDependencies499/review-uncertain-21.md."
# progress.csv
fields=list(prog[0])
for r in prog:
    i=info.get(r['address'])
    if not i: continue
    r['identity']=i['identity']; r['category']=i['category']; r['implementation']='implemented'
    r['evidence']=r['evidence']+' | Resolución PLAN-001: '+f"{i['bank']}, {i['pairs']} pares, corrida {i['run']}."
    r['notes']=note(i,True)
    r['runtime_status']='library_validated' if i['category']=='library' else 'passed'
    r['runtime_evidence']=i['summary']
with (H/'progress.csv').open('w',encoding='utf-8',newline='') as f:
    w=csv.DictWriter(f,fieldnames=fields); w.writeheader(); w.writerows(prog)
# classification csvs
for name in ('classification-a.csv','classification-b.csv','classification-c.csv'):
    rows=list(csv.DictReader((H/name).open(encoding='utf-8',newline='')));fl=list(rows[0])
    for r in rows:
        i=info.get(r['address'])
        if i: r['identity']=i['identity'];r['category']=i['category'];r['implementation']='implemented';r['notes']=note(i,False)
    with (H/name).open('w',encoding='utf-8',newline='') as f:
        w=csv.DictWriter(f,fieldnames=fl); w.writeheader(); w.writerows(rows)
# approved.json
ap=json.loads((H/'approved.json').read_text(encoding='utf-8'))
for a,i in info.items():
    if i['category']=='game': ap[a]={'identity':i['identity'],'validation_summary':i['summary']}
(H/'approved.json').write_bytes((json.dumps(ap,indent=2,ensure_ascii=False)+'\n').replace('\n','\r\n').encode('utf-8'))
# progress.json
cats={};impl={}
for r in prog: cats[r['category']]=cats.get(r['category'],0)+1; impl[r['implementation']]=impl.get(r['implementation'],0)+1
pj=json.loads((H/'progress.json').read_text(encoding='utf-8'))
passed=sum(r['runtime_status']=='passed' for r in prog)
pj.update(categories=cats,implementation=impl,runtime_passed=passed,runtime_pending=499-passed,
 game_runtime_pending=sum(r['category']=='game' and r['runtime_status']!='passed' for r in prog),unknown_category_pending=0,library_runtime_validated=1)
(H/'progress.json').write_text(json.dumps(pj,indent=2)+'\n',encoding='utf-8')
print(cats,impl,passed,pj)
json.dump(info,open('C:/Users/pedro/AppData/Local/Temp/claude/D--Original-gta-reversed-master/c66a4e30-afba-4db2-93b7-ac1baf6b4246/scratchpad/info21.json','w'),indent=1)
