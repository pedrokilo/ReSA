"""Documentary cut179 (siete funciones de CTaskSimpleFightingControl y CTaskComplexProstituteSolicit registradas via TaskFinal2); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
CLASSES=('CTaskSimpleFightingControl','CTaskComplexProstituteSolicit')
assert sum(x['validated']=='1' for x in rows)==6915 and sum(pending.values())==1531 and all(pending[c]==0 for c in CLASSES)
GROUPS=len(pending);assert GROUPS==42
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('172 aprobaciones nuevas','179 aprobaciones nuevas'),('172 identidades nuevas','179 identidades nuevas'),('6908 entradas','6915 entradas'),('1538 pendientes','1531 pendientes'),('distribuidas en **44 grupos**','distribuidas en **42 grupos**'),
 ('quedan 1538 en 44 grupos','quedan 1531 en 42 grupos'),('Ciento setenta y dos aprobaciones nuevas','Ciento setenta y nueve aprobaciones nuevas'),
 ('y cinco de CGenericGameStorage (StorageFinal1), hora','y cinco de CGenericGameStorage (StorageFinal1) y siete de CTaskSimpleFightingControl y CTaskComplexProstituteSolicit (TaskFinal2), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/TaskFinal2/validation-summary.json']
NAMES=['CTaskSimpleFightingControl::CalcMoveCommand','CTaskSimpleFightingControl::Clone','CTaskSimpleFightingControl::MakeAbortable','CTaskComplexProstituteSolicit::Clone','CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute','CTaskComplexProstituteSolicit::IsTaskValid','CTaskComplexProstituteSolicit::MakeAbortable']
assert [x['hook']['class']+'::'+x['hook']['fn_name'] for x in new]==NAMES
LIM={
 'CTaskSimpleFightingControl::CalcMoveCommand':'retornos -1, 0, 3, 7, 8, 9 y 10, fronteras de ángulo, envolturas, rand con bits bajos controlados, pasos de tiempo extremos (5e8, negativo, 0) y vehículos con planos fijados; la fuente se corrigió antes (7 diferencias: rumbo y envoltura, umbral estricto 0x8D2E60, rand del EXE siempre consumido, umbral 0.8 contra la magnitud, suma z²+x²+y² con > 0.7 estricto, _ftol2 de 64 bits y fpatan) y la rama de vehículo llama a ComputeEntityBoundingBoxPlanes (0x5F3660).',
 'CTaskSimpleFightingControl::Clone':'hueco virtual 1; la tarea nueva del pool se compara y se borra.',
 'CTaskSimpleFightingControl::MakeAbortable':'hueco virtual 6; prioridades fuera de rango, evento nulo, tipos 0..6 y otros, prioridades de evento 21/22/23 y normales en la frontera 0.707; la fuente se corrigió antes: prioridades distintas de 0/1/2 caen al final en vez de NOTSA_UNREACHABLE, GetEventType repetido, producto escalar (y·ny+z·nz)+x·nx y umbral 0.707.',
 'CTaskComplexProstituteSolicit::Clone':'hueco virtual 1; solo con cliente no nulo (el constructor del EXE y de la DLL desreferencian el cliente); la tarea nueva se compara y se borra.',
 'CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute':'retorno void; objeto y traza comparados.',
 'CTaskComplexProstituteSolicit::IsTaskValid':'casos por etapa que falla (17 condiciones en el orden del EXE) más un segundo fallo aleatorio, con fronteras exactas (distancia al cuadrado 100, up.z = -0.9, right.z = ±0.8).',
 'CTaskComplexProstituteSolicit::MakeAbortable':'hueco virtual 6; evento y prioridad guionizados.'}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con partida cargada, objetos falsos en búferes propios; x87 a 24 bits; {LIM[name]} |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='TaskFinal2 añade la aprobación de 7 funciones, 3 de CTaskSimpleFightingControl (CalcMoveCommand, Clone y MakeAbortable) y 4 de CTaskComplexProstituteSolicit (Clone, GetRidOfPlayerProstitute, IsTaskValid y MakeAbortable), 3840 pares (1024, 128, 1024, 128, 256, 1024 y 256; 0 diferencias, evento de cierre con state_restored, saves y configuraciones intactos), con el banco simple original (hook apagado) frente a la candidata dentro del juego completo; según hooks.csv las dos clases quedan completas. Límites: peatones, entidad objetivo, vehículos, inteligencia, datos de jugador, tareas falsas y evento falso en búferes propios; son dobles trazados, sin crédito de esos colaboradores, rand 0x821B1E, FindPlayerPed, ComputeEntityBoundingBoxPlanes, ComputeEntityHitSide, IsPlayerOnAMission e IsCoopGameGoingOn, que solo actúan en el hilo del banco; el destructor de CTaskComplexProstituteSolicit corre con FindPlayerPed nulo al borrar las tareas creadas por Clone (salida anticipada), de modo que el borrado solo comprueba que el pool queda limpio; fuera del dominio, CTaskComplexProstituteSolicit::Clone con cliente nulo. Antes de la corrida se corrigió la fuente de CTaskSimpleFightingControl::CalcMoveCommand y MakeAbortable a lo que hace el EXE (ver las filas) y CTaskComplexProstituteSolicit no cambió; esos cambios seguían sin confirmar en git al registrar. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/TaskFinal2/validation-summary.json).\n\n'
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6915 con evidencia histórica, 0 con vigencia acreditada y 1531 pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6915,'pending':1531,'pending_groups':GROUPS,'current':0,'new_approvals':179}
(HERE/'documentation-refresh-cut179.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
