"""Documentary cut311 (44 funciones registradas via MiscWave2); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=267;NEW_APPR=311;OLD_VALID=7003;OLD_PEND=1443;OLD_GROUPS=38
assert VALID==OLD_VALID+44 and PEND==OLD_PEND-44,(VALID,PEND)
assert 'quedan %d en %d grupos'%(OLD_PEND,OLD_GROUPS) in s
assert True
assert GROUPS==OLD_GROUPS-0,GROUPS
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'quedan {OLD_PEND} en {OLD_GROUPS} grupos',f'quedan {PEND} en {GROUPS} grupos'),('Doscientas sesenta y siete aprobaciones nuevas','Trescientas once aprobaciones nuevas'),(f'distribuidas en **38 grupos**',f'distribuidas en **{GROUPS} grupos**'),
 ('(VehWave2), hora','(VehWave2) y cuarenta y cuatro de CCarEnterExit y CRunningScript (MiscWave2), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/MiscWave2/validation-summary.json']
assert len(new)==44 and sum(x['evidence']['pairs'] for x in new)==19264
LIM={'CCarEnterExit::AddInCarAnim': 'fuente corregida antes (SitInBoat y dos llamadas a la ranura virtual 47); ', 'CCarEnterExit::QuitEnteringCar': 'fuente corregida antes; ', 'CCarEnterExit::IsClearToDriveAway': 'fuente corregida antes; ', 'CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel': 'fuente corregida antes (resultado invertido); ', 'CCarEnterExit::IsCarQuickJackPossible': 'solo resultado y memoria (sin traza virtual); ', 'CRunningScript::ReadTextLabelFromScript': 'fuente corregida antes (strncpy del CRT); ', 'CRunningScript::Init': 'fuente corregida antes (memcpy de 7); ', 'CRunningScript::DoDeathArrestCheck': 'fuente corregida antes (IP=IPStack[0]); solo con profundidad de pila >= 1; ', 'CRunningScript::Process': 'fuente corregida antes (IP=IPStack[0]); solo NOP y WAIT; ', 'CRunningScript::ProcessOneCommand': 'solo NOP y WAIT; ScriptParams excluido de la comparación; ', 'CRunningScript::GetPointerToScriptVariable': 'fuente corregida antes (nullptr); ', 'CRunningScript::GetPointerToLocalVariable': 'fuente corregida antes (32 bits); ', 'CRunningScript::GetPointerToLocalArrayElement': 'fuente corregida antes (32 bits); ', 'CRunningScript::ReadArrayInformation': 'fuente corregida antes (32 bits); '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con vehículo, peatones, modelo y scripts falsos con dobles trazados; {LIM.get(name,'')}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='MiscWave2 añade la aprobación de 44 funciones: 22 de CCarEnterExit (ComputeDoorFlag, ComputeOppositeDoorFlag, ComputePassengerIndexFromCarDoor, ComputeSlowJackedPed, ComputeTargetDoorToEnterAsPassenger, ComputeTargetDoorToExit, IsCarDoorInUse, IsCarSlowJackRequired, IsVehicleHealthy, IsPedHealthy, CarHasDoorToOpen, CarHasDoorToClose, CarHasPartiallyOpenDoor, IsCarDoorReady, CarHasOpenableDoor, IsCarQuickJackPossible, IsClearToDriveAway, IsPathToDoorBlockedByVehicleCollisionModel, RemoveGetInAnims, RemoveCarSitAnim, AddInCarAnim, QuitEnteringCar) y 22 de CRunningScript (Init, AddScriptToList, RemoveScriptFromList, UpdatePC, UpdateCompareFlag, GetPointerToLocalVariable, GetPointerToLocalArrayElement, ReadArrayInformation, GetPointerToScriptVariable, GetIndexOfGlobalVariable, CollectParameters, CollectNextParameterWithoutIncreasingPC, StoreParameters, ReadParametersForNewlyStartedScript, ReadTextLabelFromScript, GetCorrectPedModelIndexForEmergencyServiceType, ThisIsAValidRandomPed, IsPedDead, GetPadState, DoDeathArrestCheck, ProcessOneCommand, Process), 19264 pares con cero diferencias, evento de cierre con state_restored, saves y configuraciones intactos, con el banco simple (hooks de la clase apagados) frente a la candidata dentro del juego completo con partida cargada. Según hooks.csv @@CLS@@. Límites: FUERA del banco y sin crédito GetNearestCarDoor, IsVehicleStealable, MakeUndraggedDriverPedLeaveCar, SetPedInCarDirect y SetAnimOffsetForEnterOrExitVehicle (CCarEnterExit) y SetCharCoordinates y GivePedScriptedTask-OG (CRunningScript), que dependen de colaboradores y pools reales; ProcessOneCommand y Process solo ejecutan NOP y WAIT. Son dobles trazados, sin crédito de esos colaboradores, ProcessLineOfSight, BlendAnimation, las funciones de asociación de animación, StopNonPartialAnims, RestartNonPartialAnims, ClearSmallMessagesOnly e IsCutsceneSkipButtonBeingPressed. Antes de la corrida aprobada se corrigieron las fuentes CarEnterExit.cpp (AddInCarAnim, QuitEnteringCar, IsClearToDriveAway, IsPathToDoorBlockedByVehicleCollisionModel, GetNearestCarDoor, IsVehicleStealable, SetAnimOffsetForEnterOrExitVehicle) y RunningScript.cpp (ReadTextLabelFromScript, Init, DoDeathArrestCheck, Process, GetPointerToScriptVariable, Local*/ReadArrayInformation); sin confirmar en git al registrar. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/MiscWave2/validation-summary.json).\n\n'
note=note.replace('@@CLS@@','ninguna de las dos clases queda completa (CCarEnterExit conserva %d y CRunningScript %d pendientes)'%(pending['CCarEnterExit'],pending['CRunningScript']))
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
(HERE/'documentation-refresh-cut311.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
