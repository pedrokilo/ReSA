"""Documentary cut195 (quince funciones de CRoadBlocks, CPools y CIdleCam registradas via WorldWave2); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
assert sum(x['validated']=='1' for x in rows)==6931 and sum(pending.values())==1515
assert pending['CRoadBlocks']==2 and pending['CPools']==7 and pending['CIdleCam']==5
GROUPS=len(pending);assert GROUPS==41
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('180 aprobaciones nuevas','195 aprobaciones nuevas'),('180 identidades nuevas','195 identidades nuevas'),('6916 entradas','6931 entradas'),('1530 pendientes','1515 pendientes'),
 ('quedan 1530 en 41 grupos','quedan 1515 en 41 grupos'),('Ciento ochenta aprobaciones nuevas','Ciento noventa y cinco aprobaciones nuevas'),
 ('(RequestedPadProcessPad), hora','(RequestedPadProcessPad) y quince de CRoadBlocks, CPools y CIdleCam (WorldWave2), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/WorldWave2/validation-summary.json']
assert len(new)==15 and sum(x['evidence']['pairs'] for x in new)==6336
LIM={'CRoadBlocks::GenerateRoadBlocks':'la primera corrida (317/1024) halló una diferencia real de la fuente (IsAreaLoaded con CNodeAddress convertido a bool en vez de m_wAreaId; cierre del bucle interior con la barricada del propio nodo; |dot| seguro frente a NaN), corregida antes de la corrida aprobada; ','CRoadBlocks::ClearSpaceForRoadBlockObject':'fuente corregida antes (tipo distinto devuelve false y comprobación de objectType); ','CPools::MakeSureSlotInObjectPoolIsEmpty':'implementada en la fuente antes de la corrida; ','CPools::CheckPoolsEmpty':'solo se compara el estado (la DLL no llama al printf del EXE); ','CIdleCam::SetTarget':'floats denormales normalizados en el fixture (ruido SSE DAZ/FTZ frente a x87); ','CIdleCam::IsTargetValid':'fuente corregida antes (suma z, y, x); ','CIdleCam::ProcessIdleCamTicker':'fuente corregida antes (_ftol2); ','CIdleCam::GetLookAtPositionOnTarget':'fuente corregida antes (vecUp por referencia); '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo, objetos y pools falsos con dobles trazados; {LIM.get(name,'')}x87 PC24, traza de llamadas con argumentos crudos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='WorldWave2 añade la aprobación de 15 funciones, CRoadBlocks::{RegisterScriptRoadBlock, ClearScriptRoadBlocks, GetRoadBlockNodeInfo, ClearSpaceForRoadBlockObject, GenerateRoadBlocks}, CPools::{MakeSureSlotInObjectPoolIsEmpty, CheckPoolsEmpty} y CIdleCam::{Init, Reset, ProcessIdleCamTicker, GetLookAtPositionOnTarget, IdleCamGeneralProcess, SetTarget, SetTargetPlayer, IsTargetValid} (6336 pares: 512, 128, 768, 1024, 1024, 512, 128, 64, 128, 256, 512, 256, 384, 128 y 512; 0 diferencias, evento de cierre con state_restored, saves y configuraciones intactos), con el banco simple (hook apagado) frente a la candidata dentro del juego completo; según hooks.csv no queda completa ninguna clase (CRoadBlocks conserva 2 pendientes, CPools 7 y CIdleCam 5). Límites: bloque global de CRoadBlocks, áreas de rutas 61 a 63, pool de objetos de 4 ranuras y objeto/cámara falsos de CIdleCam, todo restaurado; son dobles trazados, sin crédito de esos colaboradores, FindObjectsIntersectingCube, CWorld::Add y Remove, CanBeDeleted, rand y GetRandomNumber, FindPlayer*, CreateRoadBlockBetween2Points, ProcessLineOfSight, RemoveIfThisIsAProjectile, CObject::operator new y constructor, RegisterReference, CleanUpOldReference y GetIsLineOfSightClear. Fuera del banco y sin crédito: CRoadBlocks::Init y GenerateRoadBlockPedsForCar, Initialise, ShutDown, Save, Load, LoadPedPool, LoadObjectPool y LoadVehiclePool de CPools, CIdleCam::FinaliseIdleCamera, ProcessSlerp, ProcessTargetSelection, Run y Process, y CCam entera. Ruido del banco: SetTarget con floats denormales (SSE DAZ/FTZ frente a x87), normalizados en el fixture. Antes de la corrida aprobada se corrigieron la fuente (RoadBlocks.cpp: ClearSpaceForRoadBlockObject, GeneratePeds y GenerateRoadBlocks; IdleCam.cpp: vecUp por referencia, _ftol2 y suma z,y,x; Pools.cpp: MakeSureSlotInObjectPoolIsEmpty implementada y LoadVehiclePool, LoadPedPool, LoadObjectPool y ShutDown corregidas fuera del banco; ClearRwObjectPointer añadido a CEntity; sin confirmar en git al registrar). Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/WorldWave2/validation-summary.json).\n\n'
assert marker in s;s=s.replace(marker,note+marker,1);a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
plan=json.loads((D/'documentation-plan-107.json').read_text());eligible=set(plan['eligible_current_markdown'])
tracked=set(subprocess.check_output(['git','ls-files','--cached'],cwd=R,text=True).splitlines())
for name_ in subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard'],cwd=R,text=True).splitlines():
 if name_.endswith('.md') and not any(part in ('artifacts','build','thirdparty','third-party','runs','preparation-archive','baseline-v1','archive','build-history') or 'archive' in part or part.startswith('baseline-before') for part in Path(name_).parts):
  if name_ in tracked or name_ in eligible:eligible.add(name_)
updated=[]
for name_ in sorted(eligible):
 if name_ in frozen or name_=='docs/CurrentStatus.md':continue
 p=R/name_
 if not p.exists():continue
 try:s=p.read_text(encoding='utf-8-sig')
 except UnicodeDecodeError:s=p.read_text(encoding='cp1252')
 s=re.sub(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*','',s,flags=re.S);link=posixpath.relpath('docs/CurrentStatus.md',posixpath.dirname(name_) or '.')
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6931 con evidencia histórica, 0 con vigencia acreditada y 1515 pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6931,'pending':1515,'pending_groups':GROUPS,'current':0,'new_approvals':195}
(HERE/'documentation-refresh-cut195.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
