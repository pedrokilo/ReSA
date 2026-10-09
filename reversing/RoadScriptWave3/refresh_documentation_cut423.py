"""Documentary cut423 (+3 funciones por aprobacion parcial: CRoadBlocks::Init, CRoadBlocks::GenerateRoadBlockPedsForCar y CRunningScript::GivePedScriptedTask-OG via RoadScriptWave3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=420;NEW_APPR=423;OLD_VALID=7156;OLD_PEND=1290;OLD_GROUPS=33
assert VALID==OLD_VALID+3 and PEND==OLD_PEND-3,(VALID,PEND)
assert 'distribuidas en **%d grupos**'%OLD_GROUPS in s
assert GROUPS==OLD_GROUPS-1,GROUPS
assert 'CRoadBlocks' not in pending and pending['CRunningScript']==1
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),('Cuatrocientas veinte aprobaciones nuevas','Cuatrocientas veintitrés aprobaciones nuevas'),
 ('(ClothesWave3), hora','(ClothesWave3) y tres de CRoadBlocks y CRunningScript (RoadScriptWave3, aprobación parcial por función), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/RoadScriptWave3/validation-summary.json']
assert len(new)==3 and sum(x['evidence']['pairs'] for x in new)==2176
LIM={'CRoadBlocks::Init':'fichero en memoria (OpenFile/Read/CloseFile doblados), rama de fallo de apertura no probada; fuente sin cambios (MATCH); ','CRoadBlocks::GenerateRoadBlockPedsForCar':'fuente corregida antes (SetCurrentWeapon(PISTOL) también civiles y bandas, m_bDontPursuit, sin asignaciones de temporizadores, arma de banda con SetCurrentWeapon(ranura), orden GetColModel/GetDefaultCopCarModel); solo pedType COP o GANG1..10; CMatrix::SetRotate doblado; ','CRunningScript::GivePedScriptedTask-OG':'fuente sin cambios (MATCH); script, pool de peds, tareas y grupo falsos con colaboradores doblados; '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con buferes, peds, tareas y script falsos y colaboradores doblados con trazas (aprobación parcial por función; la corrida global tiene passed=false por CRunningScript::SetCharCoordinates, rechazada); {LIM[name]}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase'
note=('RoadScriptWave3 añade la aprobación PARCIAL, por función, de 3 funciones: CRoadBlocks::Init (128 pares), CRoadBlocks::GenerateRoadBlockPedsForCar (1024) y CRunningScript::GivePedScriptedTask-OG (1024), 2176 pares con cero diferencias, banco simple (hook de la función apagado) frente a la candidata dentro del juego completo con partida cargada. La corrida 20261009-104926-137526 (3200 pares) tiene passed=false por CRunningScript::SetCharCoordinates (109/1024), RECHAZADA: no se registra ni recibe crédito. El banco no escribe el evento complete cuando hay diferencias; cerró con un evento error (3200 pares, layout_ok=true, state_restored=true, sin motivo), es decir, la corrida terminó con normalidad; saves y configuraciones intactos y el evento layout (bits y desplazamientos DLL frente a EXE) correcto. '
 'Según hooks.csv CRoadBlocks queda COMPLETA (sin pendientes) y CRunningScript conserva 1 pendiente (SetCharCoordinates). '
 'Correcciones de fuente previas a la corrida (RoadBlocks.cpp, GenerateRoadBlockPedsForCar): 0x461592 SetCurrentWeapon(PISTOL) también civiles y bandas; 0x461637 CCopPed::m_bDontPursuit = true; 0x46168D..0x4616B4 sin asignaciones de temporizadores; 0x46180B arma de banda con SetCurrentWeapon(ranura); 0x4613F8 orden GetColModel y después GetDefaultCopCarModel; Init y GivePedScriptedTask-OG coinciden con el EXE sin cambios. '
 'Límites: Init con fichero en memoria (OpenFile/Read/CloseFile doblados) y rama de fallo de apertura no probada; GenerateRoadBlockPedsForCar solo con pedType COP o GANG1..10; CMatrix::SetRotate doblado; los demás colaboradores doblados con traza no reciben crédito. '
 'Aviso: RunningScript.cpp se modificó en el árbol de trabajo después de la corrida (solo dentro de SetCharCoordinates, trabajo sobre la función rechazada); el registro conserva el hash probado y la intacta GivePedScriptedTask-OG se infiere del diff contra HEAD. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/RoadScriptWave3/validation-summary.json).')+chr(10)+chr(10)
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; {VALID} con evidencia histórica, 0 con vigencia acreditada y {PEND} pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p_,h in frozen.items():assert sha(R/p_)==h,p_
snapshot={'historical':VALID,'pending':PEND,'pending_groups':GROUPS,'current':0,'new_approvals':NEW_APPR}
(HERE/'documentation-refresh-cut423.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
