"""Documentary cut445 (+55 funciones: PopFinal3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 1018,
 'old_valid': 7754,
 'old_pend': 692,
 'old_groups': 9,
 'n': 55,
 'groups_drop': 0,
 'old_word': 'Mil dieciocho',
 'new_word': 'Mil setenta y tres',
 'intro': 'cincuenta y cinco de CPopulation (PopFinal3)',
 'complete': [],
 'pending_counts': {'CPopulation': 9},
 'summary': 'reversing/PopFinal3/validation-summary.json',
 'pairs': {'CPopulation::AddExistingPedInCar': 64,
           'CPopulation::ArePedStatsCompatible': 1024,
           'CPopulation::CanJeerAtStripper': 128,
           'CPopulation::CanSolicitPlayerInCar': 128,
           'CPopulation::CanSolicitPlayerOnFoot': 64,
           'CPopulation::ChooseCivilianCoupleOccupations': 512,
           'CPopulation::ChooseCivilianOccupation': 1024,
           'CPopulation::ChooseCivilianOccupationForVehicle': 512,
           'CPopulation::ChooseGangOccupation': 64,
           'CPopulation::ChoosePolicePedOccupation': 64,
           'CPopulation::ConvertAllObjectsToDummyObjects': 128,
           'CPopulation::ConvertToDummyObject': 256,
           'CPopulation::ConvertToRealObject': 256,
           'CPopulation::DealWithZoneChange': 64,
           'CPopulation::DoesCarGroupHaveModelId': 256,
           'CPopulation::FindCarMultiplierMotorway': 64,
           'CPopulation::FindDistanceToNearestPedOfType': 256,
           'CPopulation::FindDummyDistForModel': 256,
           'CPopulation::FindNumberOfPedsWeCanPlaceOnBenches': 512,
           'CPopulation::FindPedDensityMultiplierCullZone': 64,
           'CPopulation::FindPedMultiplierMotorway': 64,
           'CPopulation::FindPedRaceFromName': 256,
           'CPopulation::FindSpecificDriverModelForCar_ToUse': 256,
           'CPopulation::GeneratePedsAtStartOfGame': 128,
           'CPopulation::Initialise': 128,
           'CPopulation::IsCorrectTimeOfDayForEffect': 256,
           'CPopulation::IsFemale': 128,
           'CPopulation::IsMale': 128,
           'CPopulation::IsSecurityGuard': 64,
           'CPopulation::IsSkateable': 256,
           'CPopulation::IsSunbather-ModelID': 128,
           'CPopulation::LoadCarGroups': 64,
           'CPopulation::LoadPedGroups': 64,
           'CPopulation::LoadSpecificDriverModelsForCar': 128,
           'CPopulation::ManageAllPopulation': 256,
           'CPopulation::ManageDummy': 256,
           'CPopulation::ManagePed': 1024,
           'CPopulation::ManagePopulation': 256,
           'CPopulation::MoveCarsAndPedsOutOfAbandonedZones': 64,
           'CPopulation::PedCreationDistMultiplier': 128,
           'CPopulation::PedMICanBeCreatedAtAttractor': 128,
           'CPopulation::PedMICanBeCreatedAtThisAttractor': 1024,
           'CPopulation::PedMICanBeCreatedInInterior': 128,
           'CPopulation::PickGangCar': 128,
           'CPopulation::PickRiotRoadBlockCar': 256,
           'CPopulation::PlaceGangMembers': 128,
           'CPopulation::PlaceMallPedsAsStationaryGroup': 128,
           'CPopulation::RemoveAllRandomPeds': 128,
           'CPopulation::RemovePed': 128,
           'CPopulation::RemovePedsIfThePoolGetsFull': 256,
           'CPopulation::RemoveSpecificDriverModelsForCar': 128,
           'CPopulation::Shutdown': 64,
           'CPopulation::TestRoomForDummyObject': 256,
           'CPopulation::Update': 512,
           'CPopulation::UpdatePedCount': 256},
 'kind': 'CPU en el juego completo con partida cargada, pools de peds, objetos y dummies falsos y colaboradores doblados con traza (banco por pares, '
         'passed=true global)',
 'lim': {'CPopulation::AddExistingPedInCar': '',
         'CPopulation::ArePedStatsCompatible': '',
         'CPopulation::CanJeerAtStripper': '',
         'CPopulation::CanSolicitPlayerInCar': '',
         'CPopulation::CanSolicitPlayerOnFoot': '',
         'CPopulation::ChooseCivilianCoupleOccupations': 'salidas con -1 (el EXE lee locales sin inicializar en otras ramas); ',
         'CPopulation::ChooseCivilianOccupation': '',
         'CPopulation::ChooseCivilianOccupationForVehicle': '',
         'CPopulation::ChooseGangOccupation': '',
         'CPopulation::ChoosePolicePedOccupation': '',
         'CPopulation::ConvertAllObjectsToDummyObjects': '',
         'CPopulation::ConvertToDummyObject': 'fixtures sin objetos de cristal sin dummy (el EXE no comprueba el dummy nulo, 0x6146F0); ',
         'CPopulation::ConvertToRealObject': 'el dummy nunca es nulo (el EXE no lo comprueba); colaboradores doblados; ',
         'CPopulation::DealWithZoneChange': '',
         'CPopulation::DoesCarGroupHaveModelId': '',
         'CPopulation::FindCarMultiplierMotorway': '',
         'CPopulation::FindDistanceToNearestPedOfType': '',
         'CPopulation::FindDummyDistForModel': '',
         'CPopulation::FindNumberOfPedsWeCanPlaceOnBenches': '',
         'CPopulation::FindPedDensityMultiplierCullZone': '',
         'CPopulation::FindPedMultiplierMotorway': '',
         'CPopulation::FindPedRaceFromName': '',
         'CPopulation::FindSpecificDriverModelForCar_ToUse': '',
         'CPopulation::GeneratePedsAtStartOfGame': '',
         'CPopulation::Initialise': '',
         'CPopulation::IsCorrectTimeOfDayForEffect': '',
         'CPopulation::IsFemale': '',
         'CPopulation::IsMale': '',
         'CPopulation::IsSecurityGuard': '',
         'CPopulation::IsSkateable': '',
         'CPopulation::IsSunbather-ModelID': '',
         'CPopulation::LoadCarGroups': 'lee ficheros de datos reales en solo lectura (tablas rellenas de basura antes y comparadas completas); ',
         'CPopulation::LoadPedGroups': 'lee ficheros de datos reales en solo lectura (tablas rellenas de basura antes y comparadas completas); ',
         'CPopulation::LoadSpecificDriverModelsForCar': '',
         'CPopulation::ManageAllPopulation': '',
         'CPopulation::ManageDummy': '',
         'CPopulation::ManagePed': '',
         'CPopulation::ManagePopulation': '',
         'CPopulation::MoveCarsAndPedsOutOfAbandonedZones': '',
         'CPopulation::PedCreationDistMultiplier': '',
         'CPopulation::PedMICanBeCreatedAtAttractor': '',
         'CPopulation::PedMICanBeCreatedAtThisAttractor': '',
         'CPopulation::PedMICanBeCreatedInInterior': '',
         'CPopulation::PickGangCar': '',
         'CPopulation::PickRiotRoadBlockCar': 'grupos del fixture solo con IDs de vehiculo validos (el EXE recorre 23 ranuras sin mirar el '
                                              'recuento); ',
         'CPopulation::PlaceGangMembers': '',
         'CPopulation::PlaceMallPedsAsStationaryGroup': '',
         'CPopulation::RemoveAllRandomPeds': '',
         'CPopulation::RemovePed': '',
         'CPopulation::RemovePedsIfThePoolGetsFull': '',
         'CPopulation::RemoveSpecificDriverModelsForCar': '',
         'CPopulation::Shutdown': '',
         'CPopulation::TestRoomForDummyObject': 'el dummy nunca es nulo (el EXE no lo comprueba en 0x612320); ',
         'CPopulation::Update': '',
         'CPopulation::UpdatePedCount': ''},
 'note': 'PopFinal3 añade la aprobación de las cincuenta y cinco funciones de CPopulation, 13376 pares con cero diferencias. La corrida '
         '20261009-202521-590640 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, saves y configuraciones '
         'intactos, con el banco (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Límites: pools de peds (0x7C4), '
         'objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con '
         'traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0). Correcciones de fuente por función en el README del banco y en '
         'el alcance de cada función del registro. Quedan 9 pendientes en CPopulation: AddDeadPedInFrontOfCar, AddPed, AddPedAtAttractor, '
         'AddPedInCar, CreateWaitingCoppers, GeneratePedsAtAttractors, PlaceCouple, PopulateInterior, TestSafeForRealObject. Aprobación histórica '
         'con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/PopFinal3/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut445.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
