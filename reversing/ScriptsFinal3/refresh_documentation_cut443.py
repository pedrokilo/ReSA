"""Documentary cut443 (+69 funciones: ScriptsFinal3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 877,
 'old_valid': 7613,
 'old_pend': 833,
 'old_groups': 11,
 'n': 69,
 'groups_drop': 1,
 'old_word': 'Ochocientas setenta y siete',
 'new_word': 'Novecientas cuarenta y seis',
 'intro': 'sesenta y nueve de CTheScripts (ScriptsFinal3)',
 'complete': ['CTheScripts'],
 'pending_counts': {},
 'summary': 'reversing/ScriptsFinal3/validation-summary.json',
 'pairs': {'CTheScripts::AddScriptCheckpoint': 256,
           'CTheScripts::AddScriptEffectSystem': 128,
           'CTheScripts::AddScriptSearchLight': 256,
           'CTheScripts::AddScriptSphere': 128,
           'CTheScripts::AddToBuildingSwapArray': 256,
           'CTheScripts::AddToInvisibilitySwapArray': 256,
           'CTheScripts::AddToListOfConnectedLodObjects': 256,
           'CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels': 256,
           'CTheScripts::AddToSwitchJumpTable': 128,
           'CTheScripts::AddToVehicleModelsBlockedByScript': 128,
           'CTheScripts::AddToWaitingForScriptBrainArray': 256,
           'CTheScripts::AttachSearchlightToSearchlightObject': 256,
           'CTheScripts::CheckStreamedScriptVersion': 64,
           'CTheScripts::CleanUpThisObject': 128,
           'CTheScripts::CleanUpThisPed': 512,
           'CTheScripts::CleanUpThisVehicle': 128,
           'CTheScripts::ClearAllSuppressedCarModels': 64,
           'CTheScripts::ClearAllVehicleModelsBlockedByScript': 64,
           'CTheScripts::ClearSpaceForMissionEntity': 512,
           'CTheScripts::DoScriptSetupAfterPoolsHaveLoaded': 128,
           'CTheScripts::DrawDebugAngledSquare': 128,
           'CTheScripts::DrawDebugSquare-': 128,
           'CTheScripts::DrawScriptSpheres': 128,
           'CTheScripts::DrawScriptSpritesAndRectangles': 512,
           'CTheScripts::GetActualScriptThingIndex': 1024,
           'CTheScripts::GetNewUniqueScriptThingIndex': 1024,
           'CTheScripts::GetScriptIndexFromPointer': 64,
           'CTheScripts::GetUniqueScriptThingIndex': 128,
           'CTheScripts::HasCarModelBeenSuppressed': 128,
           'CTheScripts::HasVehicleModelBeenBlockedByScript': 128,
           'CTheScripts::HighlightImportantAngledArea': 512,
           'CTheScripts::HighlightImportantArea-': 512,
           'CTheScripts::Init': 256,
           'CTheScripts::InitialiseAllConnectLodObjects': 64,
           'CTheScripts::InitialiseConnectLodObjects': 128,
           'CTheScripts::InitialiseSpecialAnimGroup': 128,
           'CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels': 64,
           'CTheScripts::IsPedStopped': 512,
           'CTheScripts::IsPlayerOnAMission': 128,
           'CTheScripts::IsVehicleStopped': 256,
           'CTheScripts::Load': 128,
           'CTheScripts::PrintListSizes': 64,
           'CTheScripts::Process': 512,
           'CTheScripts::ProcessAllSearchLights': 512,
           'CTheScripts::ProcessWaitingForScriptBrainArray': 512,
           'CTheScripts::ReadMultiScriptFileOffsetsFromScript': 128,
           'CTheScripts::ReadObjectNamesFromScript': 128,
           'CTheScripts::ReinitialiseSwitchStatementData': 64,
           'CTheScripts::RemoveFromWaitingForScriptBrainArray': 256,
           'CTheScripts::RemoveScriptCheckpoint': 256,
           'CTheScripts::RemoveScriptEffectSystem': 128,
           'CTheScripts::RemoveScriptSearchLight': 256,
           'CTheScripts::RemoveScriptSphere': 128,
           'CTheScripts::RemoveScriptTextureDictionary': 128,
           'CTheScripts::RemoveThisPed': 256,
           'CTheScripts::RenderAllSearchLights': 256,
           'CTheScripts::Save': 128,
           'CTheScripts::ScriptAttachAnimGroupToCharModel': 128,
           'CTheScripts::ScriptConnectLodsFunction': 256,
           'CTheScripts::ScriptDebugCircle2D': 256,
           'CTheScripts::ScriptDebugLine3D': 64,
           'CTheScripts::StartNewScript-indexed': 256,
           'CTheScripts::StartNewScript-last-idle': 256,
           'CTheScripts::StartTestScript': 128,
           'CTheScripts::UndoBuildingSwaps': 128,
           'CTheScripts::UndoEntityInvisibilitySettings': 128,
           'CTheScripts::UpdateObjectIndices': 256,
           'CTheScripts::UseSwitchJumpTable': 512,
           'CTheScripts::WipeLocalVariableMemoryForMissionScript': 64},
 'kind': 'CPU en el juego completo con partida cargada, pools, entidades, tareas, flujo de guardado y ficheros falsos o doblados (banco simple, '
         'passed=true global)',
 'lim': {'CTheScripts::AddScriptCheckpoint': '',
         'CTheScripts::AddScriptEffectSystem': '',
         'CTheScripts::AddScriptSearchLight': '',
         'CTheScripts::AddScriptSphere': '',
         'CTheScripts::AddToBuildingSwapArray': '',
         'CTheScripts::AddToInvisibilitySwapArray': '',
         'CTheScripts::AddToListOfConnectedLodObjects': '',
         'CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels': '',
         'CTheScripts::AddToSwitchJumpTable': '',
         'CTheScripts::AddToVehicleModelsBlockedByScript': '',
         'CTheScripts::AddToWaitingForScriptBrainArray': '',
         'CTheScripts::AttachSearchlightToSearchlightObject': '',
         'CTheScripts::CheckStreamedScriptVersion': 'ficheros doblados; ',
         'CTheScripts::CleanUpThisObject': '',
         'CTheScripts::CleanUpThisPed': 'cola del EXE 0x486522..0x48663F (CTaskComplexSequence/LeaveAnyCar/Wander y CEventScriptCommand con call '
                                        '61A5A0) NO validada por el banco: los constructores de tareas son en linea y el destructor de '
                                        'CEventScriptCommand no tiene direccion tomable; el fixture solo llega a ramas donde la tarea buscada ya '
                                        'existe y la correccion del orden de reserva queda sin validar; ',
         'CTheScripts::CleanUpThisVehicle': '',
         'CTheScripts::ClearAllSuppressedCarModels': '',
         'CTheScripts::ClearAllVehicleModelsBlockedByScript': '',
         'CTheScripts::ClearSpaceForMissionEntity': '',
         'CTheScripts::DoScriptSetupAfterPoolsHaveLoaded': '',
         'CTheScripts::DrawDebugAngledSquare': '',
         'CTheScripts::DrawDebugSquare-': '',
         'CTheScripts::DrawScriptSpheres': '',
         'CTheScripts::DrawScriptSpritesAndRectangles': 'rectangulos de script con ScriptRawRect; dibujo doblado; ',
         'CTheScripts::GetActualScriptThingIndex': '',
         'CTheScripts::GetNewUniqueScriptThingIndex': '',
         'CTheScripts::GetScriptIndexFromPointer': '',
         'CTheScripts::GetUniqueScriptThingIndex': '',
         'CTheScripts::HasCarModelBeenSuppressed': '',
         'CTheScripts::HasVehicleModelBeenBlockedByScript': '',
         'CTheScripts::HighlightImportantAngledArea': '',
         'CTheScripts::HighlightImportantArea-': '',
         'CTheScripts::Init': 'main.scm sintetico servido por lectura de fichero simulada; rama de paquete de misiones (CheckMissionPackValidMenu '
                              '0x468E30) no probada; TXD solo con ranuras usadas (0x46925C); ',
         'CTheScripts::InitialiseAllConnectLodObjects': '',
         'CTheScripts::InitialiseConnectLodObjects': '',
         'CTheScripts::InitialiseSpecialAnimGroup': '',
         'CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels': '',
         'CTheScripts::IsPedStopped': '',
         'CTheScripts::IsPlayerOnAMission': '',
         'CTheScripts::IsVehicleStopped': '',
         'CTheScripts::Load': 'siempre con al menos un script activo (el EXE desreferencia pActiveScripts nulo, 0x5D4F13); flujo grabado antes con '
                              'el Save del EXE y consumido en memoria; ',
         'CTheScripts::PrintListSizes': '',
         'CTheScripts::Process': '',
         'CTheScripts::ProcessAllSearchLights': '',
         'CTheScripts::ProcessWaitingForScriptBrainArray': '',
         'CTheScripts::ReadMultiScriptFileOffsetsFromScript': 'ficheros doblados (CFileMgr simulado); ',
         'CTheScripts::ReadObjectNamesFromScript': 'ficheros doblados (CFileMgr simulado); ',
         'CTheScripts::ReinitialiseSwitchStatementData': '',
         'CTheScripts::RemoveFromWaitingForScriptBrainArray': '',
         'CTheScripts::RemoveScriptCheckpoint': '',
         'CTheScripts::RemoveScriptEffectSystem': '',
         'CTheScripts::RemoveScriptSearchLight': '',
         'CTheScripts::RemoveScriptSphere': '',
         'CTheScripts::RemoveScriptTextureDictionary': 'TXD solo con ranuras usadas (0x465A8C); ',
         'CTheScripts::RemoveThisPed': '',
         'CTheScripts::RenderAllSearchLights': '',
         'CTheScripts::Save': 'siempre con al menos un script activo porque el EXE desreferencia pActiveScripts nulo (0x5D4F13); flujo solo en '
                              'memoria; ',
         'CTheScripts::ScriptAttachAnimGroupToCharModel': '',
         'CTheScripts::ScriptConnectLodsFunction': '',
         'CTheScripts::ScriptDebugCircle2D': '',
         'CTheScripts::ScriptDebugLine3D': '',
         'CTheScripts::StartNewScript-indexed': 'ficheros doblados; ',
         'CTheScripts::StartNewScript-last-idle': 'ficheros doblados; ',
         'CTheScripts::StartTestScript': 'ficheros doblados; ',
         'CTheScripts::UndoBuildingSwaps': '',
         'CTheScripts::UndoEntityInvisibilitySettings': '',
         'CTheScripts::UpdateObjectIndices': '',
         'CTheScripts::UseSwitchJumpTable': '',
         'CTheScripts::WipeLocalVariableMemoryForMissionScript': ''},
 'note': 'ScriptsFinal3 añade la aprobación de las sesenta y nueve funciones de CTheScripts, 16384 pares con cero diferencias. La corrida '
         '20261009-203734-459814 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, saves y configuraciones '
         'intactos, con el banco (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Límites: pools de peds, '
         'edificios, objetos, dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
         '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y '
         'sin credito. Correcciones de fuente por función en el README del banco y en el alcance de cada función del registro. No quedan pendientes '
         'en CTheScripts. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. '
         '[Resumen](../reversing/ScriptsFinal3/validation-summary.json).'}
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=CFG['old_appr'];NEW_APPR=OLD_APPR+CFG['n'];OLD_VALID=CFG['old_valid'];OLD_PEND=CFG['old_pend'];OLD_GROUPS=CFG['old_groups']
assert VALID==OLD_VALID+CFG['n'] and PEND==OLD_PEND-CFG['n'],(VALID,PEND)
assert 'distribuidas en **%d grupos**'%OLD_GROUPS in s
assert GROUPS==OLD_GROUPS-CFG['groups_drop'],GROUPS
for c in CFG['complete']:assert c not in pending,c
for c,k in CFG['pending_counts'].items():assert pending[c]==k,(c,pending[c])
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
INTRO=', hora de Ciudad de México.**'
assert INTRO in s and s.index(INTRO)<4000
s=s.replace(INTRO,' y '+CFG['intro']+INTRO,1)
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),(CFG['old_word']+' aprobaciones nuevas',CFG['new_word']+' aprobaciones nuevas')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']==CFG['summary']]
assert len(new)==CFG['n'] and {x['hook']['class']+'::'+x['hook']['fn_name']:x['evidence']['pairs'] for x in new}==CFG['pairs']
LIM=CFG['lim']
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | {CFG['kind']}; {LIM[name]}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase'
note=CFG['note']+chr(10)+chr(10)
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
(HERE/'documentation-refresh-cut443.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
