"""Stopped-validation documentary cut131; no source comments or frozen inputs changed."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[3];D=Path(__file__).parent;sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0');assert sum(x['validated']=='1' for x in rows)==6867 and sum(pending.values())==1579
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()};refs={p:x['referenced_by'] for p,x in old['frozen_markdown_exceptions'].items()}
# Only top-level bank freezes and captured artifact manifests: do not traverse junction archives.
js=[]
for bank in (R/'reversing').iterdir():
 if bank.is_dir():
  js.extend(bank.glob('*freeze*.json'));js.extend(bank.glob('*frozen*.json'));js.extend(bank.glob('artifacts/manifest*.json'))
def walk(obj,reference):
 if isinstance(obj,dict):
  for key,value in obj.items():
   if key in ('inputs','bank_sources') and isinstance(value,dict):
    for p,h in value.items():
     if p.endswith('.md') and isinstance(h,str) and (R/p).exists() and sha(R/p)==h:frozen[p]=h;refs.setdefault(p,[]).append(reference)
   walk(value,reference)
 elif isinstance(obj,list):
  for x in obj:walk(x,reference)
for p in js:
 try:walk(json.loads(p.read_text(encoding='utf-8-sig')),p.relative_to(R).as_posix())
 except (ValueError,OSError):pass
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8');s=s.replace('124 aprobaciones nuevas','131 aprobaciones nuevas').replace('124 identidades nuevas','131 identidades nuevas').replace('6860 entradas','6867 entradas').replace('1586 pendientes','1579 pendientes').replace('**58 grupos**',f'**{len(pending)} grupos**').replace('Ciento veinticuatro aprobaciones nuevas','Ciento treinta y una aprobaciones nuevas')
s=s.replace('Las sondas posteriores pendientes no se suman.','Las validaciones se detuvieron por solicitud del usuario. Las sondas preparadas, compiladas o pendientes no se suman.')
reg=json.loads((R/'reversing/validation-registry.json').read_text());extra=[]
for x in reg['functions'][-7:]:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 extra.append(f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | Pools privados y referencias/handles finitos, con salida natural y restauración de estado. Sin actores reales ni acreditación de dependencias. |")
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+'\n'.join(extra)+s[pos:]
marker='## Pendientes por clase';note='Se añaden únicamente siete aprobaciones de CPools: CheckBuildingAtomics (256), GetPedRef/GetVehicleRef/GetObjectRef (256 cada una) y GetPed/GetVehicle/GetObject (768 cada una). Las pruebas acreditan sus fixtures privados; la vigencia sigue en 0. UpdateMouse V2 quedó compilado con revisión estática de ABI y calls, sin replay ni aprobación. Distance/Train y las demás sondas pendientes tampoco amplían el registro.\n\nSe liberó espacio trasladando cinco carpetas de artefactos a un archivo en C:, con hashes verificados y junctions que mantienen las rutas de evidencia. [Registro del traslado](../reversing/RequestedValidation20261008/Diagnostics/disk-recovery-20261008.json). No se eliminaron evidencias acreditadas.\n\n'
s=s.replace(marker,note+marker);a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
plan=json.loads((D/'documentation-plan-107.json').read_text());eligible=set(plan['eligible_current_markdown'])
for name in subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard'],cwd=R,text=True).splitlines():
 if name.endswith('.md') and not any(part in ('artifacts','build','thirdparty','third-party','runs','preparation-archive','baseline-v1','archive','build-history') or 'archive' in part or part.startswith('baseline-before') for part in Path(name).parts):eligible.add(name)
updated=[]
for name in sorted(eligible):
 if name in frozen or name=='docs/CurrentStatus.md':continue
 p=R/name
 if not p.exists():continue
 try:s=p.read_text(encoding='utf-8-sig')
 except UnicodeDecodeError:s=p.read_text(encoding='cp1252')
 s=re.sub(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*','',s,flags=re.S);link=posixpath.relpath('docs/CurrentStatus.md',posixpath.dirname(name) or '.')
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en {len(pending)} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6867,'pending':1579,'pending_groups':len(pending),'current':0,'new_approvals':131}
(R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').write_text(json.dumps({'snapshot':'6867 stopped validation documentary checkpoint','frozen_markdown_exceptions':{p:{'sha256':h,'referenced_by':sorted(set(refs[p]))} for p,h in frozen.items()}},indent=2)+'\n',encoding='utf8')
(R/'reversing/RequestedValidation20261008/documentation-refresh.json').write_text(json.dumps({'date':'2026-10-08','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
