"""Documentary cut134 (Pad Distance/Train/NOP Hex V2 registered); preserves byte-exact frozen Markdown."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[3];D=Path(__file__).parent;sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0');assert sum(x['validated']=='1' for x in rows)==6870 and sum(pending.values())==1576
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()};refs={p:x['referenced_by'] for p,x in old['frozen_markdown_exceptions'].items()}
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
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('131 aprobaciones nuevas','134 aprobaciones nuevas'),('131 identidades nuevas','134 identidades nuevas'),('6867 entradas','6870 entradas'),('1579 pendientes','1576 pendientes'),('Ciento treinta y una aprobaciones nuevas','Ciento treinta y cuatro aprobaciones nuevas'),('el getter de radio frontend, hora','el getter de radio frontend y Distance/Train/NOP de CPad, hora'),('quedan 1603 en 61 grupos',f'quedan 1576 en {len(pending)} grupos'),(' Distance/Train y las demás sondas pendientes tampoco amplían el registro.',' Las demás sondas pendientes tampoco amplían el registro.')):
 assert a in s,a;s=s.replace(a,b)
s=s.replace('**58 grupos**',f'**{len(pending)} grupos**');s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());extra=[]
for x in reg['functions'][-3:]:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 extra.append(f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU privado, entradas finitas con x87 PC24/redondeo al más cercano, excepciones enmascaradas y MXCSR 0x1f80; FindPlayerVehicle sustituido por stub (sólo multiplicidad y argumentos, sin crédito del helper); cámara con matriz prestada y restaurada; sin recursos ni gameplay; el NOP sólo prueba la ausencia de efectos. |")
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+'\n'.join(extra)+s[pos:]
marker='## Pendientes por clase';note='Distance/Train/NOP Hex V2 añade tres aprobaciones de CPad: StartShake_Distance (10240), StartShake_Train (2688) y ProcessPCSpecificStuff (1024), 13952 pares con cero diferencias, salida natural 0 y restauración de harness, estado, saves y configuraciones (17692 rangos). Una auditoría independiente la declaró apta con estos límites: sólo entradas finitas (sin NaN/Inf) bajo x87 PC24/round-nearest con excepciones enmascaradas y MXCSR 0x1f80; FindPlayerVehicle sustituido por stub; sin recursos ni gameplay; cámara con matriz prestada y restaurada; el NOP sólo prueba "sin efectos"; el entorno x87 se verifica por banco (28 bytes FNSTENV antes igual a después), no por fila. Son aprobaciones históricas con dependency_review=null; la vigencia sigue en 0. El intento [20261008-170728-931505](../reversing/RequestedPadDistanceTrainNopHexV2/runs/20261008-170728-931505/enriched-summary.json) (disco lleno) y el [baseline Hex V1](../reversing/RequestedPadDistanceTrainNopHex/runs/20261008-164703-580513/enriched-summary.json) se conservan como rechazados. [Resumen](../reversing/RequestedPadDistanceTrainNopHexV2/validation-summary.json).\n\n'
s=s.replace(marker,note+marker);a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
plan=json.loads((D/'documentation-plan-107.json').read_text());eligible=set(plan['eligible_current_markdown'])
tracked=set(subprocess.check_output(['git','ls-files','--cached'],cwd=R,text=True).splitlines())
for name in subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard'],cwd=R,text=True).splitlines():
 if name.endswith('.md') and not any(part in ('artifacts','build','thirdparty','third-party','runs','preparation-archive','baseline-v1','archive','build-history') or 'archive' in part or part.startswith('baseline-before') for part in Path(name).parts):
  if name in tracked or name in eligible:eligible.add(name)
updated=[]
for name in sorted(eligible):
 if name in frozen or name=='docs/CurrentStatus.md':continue
 p=R/name
 if not p.exists():continue
 try:s=p.read_text(encoding='utf-8-sig')
 except UnicodeDecodeError:s=p.read_text(encoding='cp1252')
 s=re.sub(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*','',s,flags=re.S);link=posixpath.relpath('docs/CurrentStatus.md',posixpath.dirname(name) or '.')
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6870 con evidencia histórica, 0 con vigencia acreditada y 1576 pendientes en {len(pending)} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6870,'pending':1576,'pending_groups':len(pending),'current':0,'new_approvals':134}
(R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').write_text(json.dumps({'snapshot':'6870 documentary checkpoint after Pad Distance/Train/NOP Hex V2','frozen_markdown_exceptions':{p:{'sha256':h,'referenced_by':sorted(set(refs[p]))} for p,h in frozen.items()}},indent=2)+'\n',encoding='utf8')
(R/'reversing/RequestedValidation20261008/documentation-refresh.json').write_text(json.dumps({'date':'2026-10-08','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
