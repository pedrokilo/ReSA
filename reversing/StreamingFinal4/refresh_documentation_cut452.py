"""Corte documental 452 (+94 funciones: StreamingFinal4); conserva byte a byte el Markdown congelado y no toca documentation-frozen-exceptions.json."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'complete': ['CStreaming'],
 'groups_drop': 1,
 'intro': 'noventa y cuatro de CStreaming (StreamingFinal4)',
 'kind': 'CPU en el juego completo con partida cargada, mundo falso de streaming y colaboradores doblados con traza (banco por '
         'pares, passed=true global)',
 'lim': {'CStreaming::AddEntity': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::AddImageToList': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::AddLodsToRequestList': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::AddModelsToRequestList': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::AddToLoadedVehiclesList': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::AreAnimsUsedByRequestedModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::AreTexturesUsedByRequestedModels': 'mundo falso de streaming y colaboradores doblados con traza sin '
                                                         'crédito; ',
         'CStreaming::ClearFlagForAll': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::ClearSlots': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::ConvertBufferToObject': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::DeleteAllRwObjects': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::DeleteLeastUsedEntityRwObject': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::DeleteRwObjectsAfterDeath': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::DeleteRwObjectsBehindCamera': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>': 'mundo falso de streaming y '
                                                                                              'colaboradores doblados con traza '
                                                                                              'sin crédito; ',
         'CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>': 'mundo falso de streaming y colaboradores '
                                                                                  'doblados con traza sin crédito; ',
         'CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>': 'mundo falso de streaming y '
                                                                                              'colaboradores doblados con traza '
                                                                                              'sin crédito; ',
         'CStreaming::DisableCopBikes': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::FindMIPedSlotForInterior': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::FinishLoadingLargeFile': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::FlushChannels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::FlushRequestList': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::GetDefaultCabDriverModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::GetDefaultCopCarModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::GetDefaultCopModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::GetDefaultFiremanModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::GetDefaultMedicModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::GetNextFileOnCd': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::HasSpecialCharLoaded': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::HasVehicleUpgradeLoaded': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::Init2': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::InitImageList': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::InstanceLoadedModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::IsCarModelNeededInCurrentZone': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::IsVeryBusy': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::Load': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadAllRequestedModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadCdDirectory-char': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadCdDirectory-void': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadInitialPeds': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadInitialWeapons': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadRequestedModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadScene': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadSceneCollision': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::LoadZoneVehicle': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::MakeSpaceFor': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::PossiblyStreamCarOutAfterCreation': 'mundo falso de streaming y colaboradores doblados con traza sin '
                                                          'crédito; ',
         'CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-': 'mundo falso de streaming y colaboradores '
                                                                                   'doblados con traza sin crédito; ',
         'CStreaming::ProcessLoadingChannel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::PurgeRequestList': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::ReInit': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::ReadIniFile': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::ReclassifyLoadedCars': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveAllUnusedModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveBigBuildings': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveBuildingsNotInArea': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveCarModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveCurrentZonesModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveDodgyPedsFromRandomSlots': 'mundo falso de streaming y colaboradores doblados con traza sin '
                                                       'crédito; ',
         'CStreaming::RemoveEntity': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveInappropriatePedModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveLeastUsedModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveLoadedVehicle': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveLoadedZoneModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveReferencedTxds': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RemoveTxdModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RenderEntity': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestBigBuildings': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestFile': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestFilesInChannel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestModelStream': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestSpecialChar': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestSpecialModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestTxdModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RequestVehicleUpgrade': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::RetryLoadFile': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::Save': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::SetMissionDoesntRequireModel': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::SetModelIsDeletable': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::Shutdown': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamAmbulanceAndMedic': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamCopModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamFireEngineAndFireman': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamOneNewCar': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamPedsForInterior': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamPedsIntoRandomSlots': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamVehiclesAndPeds': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamVehiclesAndPeds_Always': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamZoneModels': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::StreamZoneModels_Gangs': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::Update': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; ',
         'CStreaming::WeAreTryingToPhaseVehicleOut': 'mundo falso de streaming y colaboradores doblados con traza sin crédito; '},
 'n': 94,
 'new_word': 'Mil seiscientas siete',
 'note': 'StreamingFinal4 añade la aprobación de las 94 funciones de CStreaming, 20992 pares con cero diferencias. La corrida '
         '20261010-143008-897454 tiene passed=true global y evento complete con state_restored=true, saves y configuraciones '
         'intactos, con el banco (hooks de la clase apagados) frente a la candidata dentro del juego completo con partida '
         'cargada. Límites: mundo falso y determinista de streaming (model infos, pools, sectores, listas de cargados y pedidos, '
         'canales, cámara, jugador); dominio del fixture con cámara dentro de ±2900, tamaños no nulos, enlaces de lista válidos '
         'y tabla de conductores de taxi real; excepciones por lectura de pila sin inicializar del EXE fuera del dominio '
         'comparado; CdStream, RenderWare, cargadores, almacenes, población, ficheros y memoria doblados con traza sin crédito; '
         'globales tocados guardados y restaurados en cada ejecución; generador aleatorio del EXE. Son aprobaciones históricas '
         'con dependency_review=null; la vigencia sigue en 0 y CStreaming queda sin pendientes.',
 'old_appr': 1513,
 'old_groups': 3,
 'old_pend': 197,
 'old_valid': 8249,
 'old_word': 'Mil quinientas trece',
 'pairs': {'CStreaming::AddEntity': 256,
           'CStreaming::AddImageToList': 256,
           'CStreaming::AddLodsToRequestList': 256,
           'CStreaming::AddModelsToRequestList': 256,
           'CStreaming::AddToLoadedVehiclesList': 256,
           'CStreaming::AreAnimsUsedByRequestedModels': 256,
           'CStreaming::AreTexturesUsedByRequestedModels': 256,
           'CStreaming::ClearFlagForAll': 128,
           'CStreaming::ClearSlots': 256,
           'CStreaming::ConvertBufferToObject': 512,
           'CStreaming::DeleteAllRwObjects': 128,
           'CStreaming::DeleteLeastUsedEntityRwObject': 256,
           'CStreaming::DeleteRwObjectsAfterDeath': 128,
           'CStreaming::DeleteRwObjectsBehindCamera': 256,
           'CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>': 256,
           'CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>': 256,
           'CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>': 256,
           'CStreaming::DisableCopBikes': 64,
           'CStreaming::FindMIPedSlotForInterior': 128,
           'CStreaming::FinishLoadingLargeFile': 256,
           'CStreaming::FlushChannels': 128,
           'CStreaming::FlushRequestList': 128,
           'CStreaming::GetDefaultCabDriverModel': 128,
           'CStreaming::GetDefaultCopCarModel': 256,
           'CStreaming::GetDefaultCopModel': 128,
           'CStreaming::GetDefaultFiremanModel': 64,
           'CStreaming::GetDefaultMedicModel': 64,
           'CStreaming::GetNextFileOnCd': 512,
           'CStreaming::HasSpecialCharLoaded': 64,
           'CStreaming::HasVehicleUpgradeLoaded': 256,
           'CStreaming::Init2': 64,
           'CStreaming::InitImageList': 64,
           'CStreaming::InstanceLoadedModels': 128,
           'CStreaming::IsCarModelNeededInCurrentZone': 512,
           'CStreaming::IsVeryBusy': 64,
           'CStreaming::Load': 64,
           'CStreaming::LoadAllRequestedModels': 256,
           'CStreaming::LoadCdDirectory-char': 512,
           'CStreaming::LoadCdDirectory-void': 128,
           'CStreaming::LoadInitialPeds': 64,
           'CStreaming::LoadInitialWeapons': 64,
           'CStreaming::LoadRequestedModels': 256,
           'CStreaming::LoadScene': 128,
           'CStreaming::LoadSceneCollision': 64,
           'CStreaming::LoadZoneVehicle': 128,
           'CStreaming::MakeSpaceFor': 256,
           'CStreaming::PossiblyStreamCarOutAfterCreation': 128,
           'CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-': 512,
           'CStreaming::ProcessLoadingChannel': 512,
           'CStreaming::PurgeRequestList': 128,
           'CStreaming::ReInit': 64,
           'CStreaming::ReadIniFile': 256,
           'CStreaming::ReclassifyLoadedCars': 128,
           'CStreaming::RemoveAllUnusedModels': 128,
           'CStreaming::RemoveBigBuildings': 128,
           'CStreaming::RemoveBuildingsNotInArea': 256,
           'CStreaming::RemoveCarModel': 256,
           'CStreaming::RemoveCurrentZonesModels': 128,
           'CStreaming::RemoveDodgyPedsFromRandomSlots': 128,
           'CStreaming::RemoveEntity': 256,
           'CStreaming::RemoveInappropriatePedModels': 128,
           'CStreaming::RemoveLeastUsedModel': 256,
           'CStreaming::RemoveLoadedVehicle': 256,
           'CStreaming::RemoveLoadedZoneModel': 128,
           'CStreaming::RemoveModel': 512,
           'CStreaming::RemoveReferencedTxds': 128,
           'CStreaming::RemoveTxdModel': 128,
           'CStreaming::RenderEntity': 256,
           'CStreaming::RequestBigBuildings': 128,
           'CStreaming::RequestFile': 256,
           'CStreaming::RequestFilesInChannel': 128,
           'CStreaming::RequestModel': 512,
           'CStreaming::RequestModelStream': 512,
           'CStreaming::RequestSpecialChar': 256,
           'CStreaming::RequestSpecialModel': 512,
           'CStreaming::RequestTxdModel': 256,
           'CStreaming::RequestVehicleUpgrade': 128,
           'CStreaming::RetryLoadFile': 256,
           'CStreaming::Save': 64,
           'CStreaming::SetMissionDoesntRequireModel': 256,
           'CStreaming::SetModelIsDeletable': 256,
           'CStreaming::Shutdown': 64,
           'CStreaming::StreamAmbulanceAndMedic': 128,
           'CStreaming::StreamCopModels': 256,
           'CStreaming::StreamFireEngineAndFireman': 128,
           'CStreaming::StreamOneNewCar': 512,
           'CStreaming::StreamPedsForInterior': 256,
           'CStreaming::StreamPedsIntoRandomSlots': 256,
           'CStreaming::StreamVehiclesAndPeds': 256,
           'CStreaming::StreamVehiclesAndPeds_Always': 256,
           'CStreaming::StreamZoneModels': 512,
           'CStreaming::StreamZoneModels_Gangs': 512,
           'CStreaming::Update': 256,
           'CStreaming::WeAreTryingToPhaseVehicleOut': 128},
 'pending_counts': {'CAEVehicleAudioEntity': 102, 'Win': 1},
 'summary': 'reversing/StreamingFinal4/validation-summary.json'}
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
(HERE/'documentation-refresh-cut452.json').write_text(json.dumps({'date':'2026-10-10','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
