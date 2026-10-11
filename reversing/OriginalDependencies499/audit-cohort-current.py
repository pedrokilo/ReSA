"""Independent streaming audit; writes only AUDIT_COHORT_CURRENT.md."""
import collections,csv,gzip,hashlib,json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
read=lambda p:json.loads(p.read_text(encoding='utf-8'))
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for chunk in iter(lambda:f.read(1024*1024),b''):h.update(chunk)
 return h.hexdigest()
a=read(HERE/'approved.json');snapshot=sha(HERE/'approved.json');hooks=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig',newline='')));registry=read(ROOT/'reversing/validation-registry.json')['functions'];problems=[];drifts=[];rows=[];total=0
for address,item in a.items():
 found=[h for h in hooks if int(h['address'],16)==int(address,16) and h['class']+'::'+h['fn_name']==item['identity']]
 if len(found)!=1 or found[0]['validated']!='1' or found[0]['reversed']!='1':problems.append(('catalog',address+' '+item['identity']))
 rr=[r for r in registry if int(r['hook']['address'],16)==int(address,16) and r['hook']['class']+'::'+r['hook']['fn_name']==item['identity']]
 if len(rr)!=1 or rr[0]['evidence']['path']!=item['validation_summary']:problems.append(('registry',address+' identity/evidence'))
 elif rr[0]['evidence']['sha256']!=sha(ROOT/item['validation_summary']):problems.append(('registry',address+' evidence SHA'))
gates={'route_ok','hook_route_ok','helper_route_ok','library_route_ok','stack_restored','registers_preserved','nonvolatile_preserved','nonvolatile_registers_preserved','fp_preserved','seh_restored','contract_ok','readonly_ok','guards_ok','protected_state_unchanged','expected_effects_correct','return_domain_ok','fixture_valid','read_range_ok','path_canaries_ok','optional_outputs_ok','callbacks_real'}
for path in sorted({x['validation_summary']for x in a.values()}):
 s=read(ROOT/path);b=(ROOT/path).parent;run=ROOT/s['run'];r=read(run/'summary.json');au=read(run/'approval-audit.json');err=set();counts=collections.Counter();cases=collections.defaultdict(set);pairids=set();beg=[];ends=[];summ={};n=diff=null=0;h=hashlib.sha256()
 def req(x,msg):
  if not x:err.add(msg)
 with gzip.open(run/'comparisons.jsonl.gz','rb')as f:
  for line in f:
   h.update(line);e=json.loads(line);ev=e.get('event')
   if ev=='comparison':
    n+=1;fn=e['function'];counts[fn]+=1
    req(e['pair']not in pairids,'duplicate pair');pairids.add(e['pair']);req(e.get('case')not in cases[fn],'duplicate case');cases[fn].add(e.get('case'))
    o,c=e.get('original'),e.get('candidate');null+=not isinstance(o,dict)or not isinstance(c,dict);diff+=o!=c or e.get('equal')is not True or e.get('accepted')is not True
    for v in (o,c):
     if not isinstance(v,dict):continue
     req(any(k in v for k in ('route_ok','hook_route_ok')),'missing percall route gate')
     for k in gates.intersection(v):req(v[k]is True,'false gate '+k)
    if 'state_restored'in e:req(e['state_restored']is True,'perpair restore')
   elif ev=='begin':beg.append(e)
   elif ev=='complete':ends.append(e)
   elif ev=='function_summary':req(e['function']not in summ,'duplicate function_summary');summ[e['function']]=e
   elif ev=='error':err.add('error event')
 req(null==diff==0,'nonNULL/equal/accepted');req(dict(counts)==s['functions']==r['pairs_per_function'],'function counts');req(n==s['pairs']==r['pairs']==au['pairs'],'total pairs');req(set(counts)=={x['identity']for x in a.values()if x['validation_summary']==path},'approved identities')
 for fn,cnt in counts.items():
  req(cases[fn]==set(range(cnt)),'cases consecutive '+fn);req(fn in summ and ('complete_paths'not in summ[fn]or summ[fn]['complete_paths']is True) and s.get('covered_functions',{}).get(fn)is True and summ[fn].get('failures')==0 and summ[fn].get('cases')==cnt,'coverage summary '+fn)
 req(len(beg)==1 and beg[0].get('registered_hook')is True,'begin registered');req(len(ends)==1 and ends[0].get('pairs')==n and ends[0].get('state_restored')is True,'complete restore')
 for k in ('passed','all_equal','state_restored','saves_unchanged','configs_restored'):req(s.get(k)is True and r.get(k)is True,'summary '+k)
 req(h.hexdigest()==s['log_sha256']==r['log_sha256']==au['log_sha256'],'log SHA');req(sha(run/'summary.json')==s['summary_sha256'],'summary SHA');req(sha(run/'RESULTADO.txt')==s['result_txt_sha256'],'result SHA');req(sha(run/'comparisons.jsonl.gz')==s['comparison_storage']['compressed_sha256'],'gzip SHA')
 req(s['executed_probe_sha256']==r['executed_probe_sha256']==au['executed_probe_sha256'],'ASI identity');req(s['original_exe_sha256']==r['original_exe_sha256']==au['original_exe_sha256']=='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b','EXE identity')
 for group in ('tested_sources','bank_sources'):
  req(s['manifest'][group]==au[group],'historical manifest '+group)
  for file,digest in s['manifest'][group].items():
   p=ROOT/file if group=='tested_sources'else b/file
   req(p.exists(),'missing manifest '+file)
   if p.exists()and sha(p)!=digest:
    if group=='bank_sources':err.add('instrument changed '+file)
    else:drifts.append((b.name,file,digest,sha(p)))
 header=(b/'LiveComparison.hpp').read_text(encoding='utf-8');decl={cl+'::'+fn:int(ad,16)for cl,fn,ad in re.findall(r'\{\s*"([^"]+)"\s*,\s*"([^"]+)"\s*,\s*(0x[0-9a-fA-F]+)',header)}
 for ad,it in a.items():
  if it['validation_summary']==path:
   declared=decl.get(it['identity']);single=re.search(r'constexpr uintptr_t address\s*=\s*(0x[0-9a-fA-F]+)',header)
   if declared is None and single and len(counts)==1:declared=int(single.group(1),16)
   req(declared==int(ad,16),'declared identity '+it['identity'])
 rows.append((b.name,len(counts),n,diff,null,len(err),h.hexdigest()));total+=n;problems.extend((b.name,e)for e in sorted(err))
out=['# Auditoría independiente de la cohorte admitida actual','','Instantánea approved SHA256 `'+snapshot+'`. Gzip leído en streaming; no builds/juego ni cambios a fuentes, instrumentos o catálogo. Auditoría histórica de43 conservada.', '',f'**{len(a)} funciones / {len(rows)} bancos / {total} pares; {len(problems)} hallazgos.**','','| Banco | Funciones | Pares | Diferencias | NULL | Hallazgos |','|---|---:|---:|---:|---:|---:|']
out +=[f'| {b} | {f} | {p} | {d} | {z} | {e} |'for b,f,p,d,z,e,h in rows]
out+=['','Cada identidad approved se cruza una a una con hooks.csv (reversed/validated=1) y registry (identidad, evidencia y SHA). Pares y casos únicos/consecutivos; original/candidate dict completos iguales y accepted/equal true. Gates de ruta deben estar presentes; todos los gates de ABI/contrato/FP/restauración presentes se exigen verdaderos. Un begin registrado, resúmenes sin fallos y covered_functions verdadero (complete_paths también verdadero cuando existe; los bancos históricos no emiten ese campo) y un complete/restored. Huellas log/gzip/summary/result y manifiestos históricos verificadas. Partidas/configuraciones se comprueban por flags históricos de ejecución; no se reconstruyen snapshots posteriores inexistentes.','','## Evolución de fuentes','','Derivas de tested_sources respecto al estado actual son esperadas por tandas posteriores; no invalidan evidencia histórica. Instrumentación debe coincidir exactamente.','','| Banco | Fuente | SHA histórico | SHA actual |','|---|---|---|---|']+[f'| {b} | {f} | {old} | {new} |'for b,f,old,new in drifts]
out+=['','## Hallazgos','']+([f'- {b}: {p}'for b,p in problems]or['Sin discrepancias detectadas.'])+['','## Logs verificados','']+[f'- {b}: `{h}`'for b,f,p,d,z,e,h in rows]
(HERE/'AUDIT_COHORT_CURRENT.md').write_text('\n'.join(out)+'\n',encoding='utf-8');print(json.dumps({'functions':len(a),'banks':len(rows),'pairs':total,'problems':problems,'source_drifts':len(drifts)}))
