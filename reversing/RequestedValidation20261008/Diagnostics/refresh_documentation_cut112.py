"""Authorized cut112 documentary update; preserves byte-exact frozen Markdown."""
import csv,collections,hashlib,json,posixpath,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(r['class'] for r in rows if r['validated']=='0');assert sum(r['validated']=='1' for r in rows)==6848 and sum(pending.values())==1598 and len(pending)==60
plan=json.loads((Path(__file__).parent/'documentation-plan-107.json').read_text());old=json.loads((ROOT/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text())
frozen={p:r['actual_sha256'] for p,r in plan['exact_frozen_markdown'].items()}
for p,r in old['frozen_markdown_exceptions'].items():
 if (ROOT/p).exists() and sha(ROOT/p)==r['sha256']:frozen[p]=r['sha256']
for p,h in frozen.items():assert sha(ROOT/p)==h,p
p=ROOT/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8');s=s.replace('107 aprobaciones nuevas','112 aprobaciones nuevas').replace('107 identidades nuevas','112 identidades nuevas').replace('6843 entradas','6848 entradas').replace('1603 pendientes','1598 pendientes').replace('**61 grupos**','**60 grupos**').replace('Ciento siete aprobaciones nuevas','Ciento doce aprobaciones nuevas')
s=s.replace('Reconcile y Update, hora','Reconcile, Update, StartShake y cuatro despachos CPU, hora')
registry=json.loads((ROOT/'reversing/validation-registry.json').read_text(encoding='utf8'));added=[]
for r in registry['functions'][-5:]:
 name=r['hook']['class']+'::'+r['hook']['fn_name'];e=r['evidence']
 if '| '+name+' |' in s:continue
 scope=('Estados privados, relojes uint32 y duraciones signed; gates frontend/cutscene. Sin controlador real ni vibración del sistema.' if name=='CPad::StartShake' else 'Despacho CPU privado, orden/trazas y bytes/retorno definidos. Sin implementación de colaboradores, gráficos ni ciclo de vida de recursos.')
 added.append(f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | {scope} |")
marker='## Pendientes por clase';heading=s.index(marker);table_end=s.rfind('| CPad::Update |');assert table_end>=0;insert=s.index('\n',table_end);s=s[:insert]+'\n'+'\n'.join(added)+s[insert:]
note='StartShake añadió 89600 casos con cero diferencias, incluyendo relojes envueltos y duraciones negativas como caracterización finita; compara arenas completas y FP sin normalizar bytes. Las cuatro funciones de DispatchShadow añadieron 5120 casos CPU privados: Initialise/Shutdown/PluginAttach de CCustomBuildingRenderer y ReturnRealTimeShadow. Son aprobaciones históricas con dependency_review=null; no acreditan gráficos, helpers ni recursos reales. Los bancos posteriores de Arrest/Fx/Distance/Geometry continúan pendientes hasta sus replays y auditorías.\n\n'
if note not in s:s=s.replace(marker,note+marker)
a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
updated=[]
for name in plan['eligible_current_markdown']:
 if name in frozen or name=='docs/CurrentStatus.md':continue
 p=ROOT/name
 if not p.exists():continue
 try:s=p.read_text(encoding='utf-8-sig')
 except UnicodeDecodeError:s=p.read_text(encoding='cp1252')
 s=re.sub(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*','',s,flags=re.S)
 link=posixpath.relpath('docs/CurrentStatus.md',posixpath.dirname(name) or '.')
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n'
 p.write_text(prefix+s,encoding='utf8');updated.append(name)
for name,h in frozen.items():assert sha(ROOT/name)==h,name
exceptions={'snapshot':'6848 documentary checkpoint; exact current frozen Markdown and dated archived evidence','frozen_markdown_exceptions':{p:{'sha256':h,'referenced_by':([x['reference'] for x in plan['exact_frozen_markdown'].get(p,{}).get('exact_references',[])] or old['frozen_markdown_exceptions'].get(p,{}).get('referenced_by',[]))} for p,h in frozen.items()}}
(ROOT/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').write_text(json.dumps(exceptions,indent=2)+'\n',encoding='utf8')
(ROOT/'reversing/RequestedValidation20261008/documentation-refresh.json').write_text(json.dumps({'date':'2026-10-08','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(ROOT/'docs/hooks.csv'),'snapshot':{'historical':6848,'pending':1598,'pending_groups':60,'current':0,'new_approvals':112}},indent=2)+'\n',encoding='utf8')
print('Updated',len(updated),'prefixes; frozen',len(frozen),'exact; central112rows.')
