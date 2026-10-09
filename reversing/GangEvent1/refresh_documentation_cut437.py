"""Documentary cut437 (+62 funciones: GangEvent1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 620,
 'old_valid': 7356,
 'old_pend': 1090,
 'old_groups': 19,
 'n': 62,
 'groups_drop': 2,
 'old_word': 'Seiscientas veinte',
 'new_word': 'Seiscientas ochenta y dos',
 'intro': 'sesenta y dos de CGroupEventHandler y CGangWars (GangEvent1)',
 'complete': ['CGangWars', 'CGroupEventHandler'],
 'pending_counts': {},
 'summary': 'reversing/GangEvent1/validation-summary.json',
 'pairs': {'CGroupEventHandler::IsKillTaskAppropriate': 256,
           'CGroupEventHandler::ComputeWalkAlongsideResponse': 256,
           'CGroupEventHandler::ComputeStareResponse': 384,
           'CGroupEventHandler::ComputeResponseVehicleDamage': 384,
           'CGroupEventHandler::ComputeResponseShotFired': 256,
           'CGroupEventHandler::ComputeResponseSexyPed': 384,
           'CGroupEventHandler::ComputeResponseSeenCop': 256,
           'CGroupEventHandler::ComputeResponsePlayerCommand': 256,
           'CGroupEventHandler::ComputeResponsePedThreat': 384,
           'CGroupEventHandler::ComputeResponsePedFriend': 384,
           'CGroupEventHandler::ComputeResponseNewGangMember': 256,
           'CGroupEventHandler::ComputeResponseLeaderExitedCar': 512,
           'CGroupEventHandler::ComputeResponseLeaderEnterExit': 256,
           'CGroupEventHandler::ComputeResponseGunAimedAt': 384,
           'CGroupEventHandler::ComputeResponseGather': 192,
           'CGroupEventHandler::ComputeResponseDraggedOutCar': 384,
           'CGroupEventHandler::ComputeResponseDanger': 192,
           'CGroupEventHandler::ComputeResponseDamage': 384,
           'CGroupEventHandler::ComputeMemberResponses': 384,
           'CGroupEventHandler::ComputeLeanOnVehicleResponse': 192,
           'CGroupEventHandler::ComputeKillThreatsBasicResponse': 256,
           'CGroupEventHandler::ComputeKillPlayerBasicResponse': 256,
           'CGroupEventHandler::ComputeHassleThreatResponse': 384,
           'CGroupEventHandler::ComputeHassleSexyPedResponse': 256,
           'CGroupEventHandler::ComputeHandSignalResponse': 192,
           'CGroupEventHandler::ComputeGreetResponse': 512,
           'CGroupEventHandler::ComputeFleePedResponse': 384,
           'CGroupEventHandler::ComputeDrivebyResponse': 128,
           'CGroupEventHandler::ComputeEventResponseTasks': 768,
           'CGroupEventHandler::ComputeDoDealResponse': 512,
           'CGangWars::InitAtStartOfGame': 64,
           'CGangWars::AttackWaveOvercome': 384,
           'CGangWars::CalculateTimeTillNextAttack': 64,
           'CGangWars::CanPlayerStartAGangWarHere': 384,
           'CGangWars::CheerVictory': 192,
           'CGangWars::ClearSpecificZonesToTriggerGangWar': 64,
           'CGangWars::ClearTheStreets': 256,
           'CGangWars::CreateDefendingGroup': 512,
           'CGangWars::DoStuffWhenPlayerVictorious': 128,
           'CGangWars::DoesPlayerControlThisZone': 192,
           'CGangWars::DontCreateCivilians': 64,
           'CGangWars::EndGangWar': 192,
           'CGangWars::GangWarFightingGoingOn': 64,
           'CGangWars::GangWarGoingOn': 64,
           'CGangWars::Load': 192,
           'CGangWars::MakeEnemyGainInfluenceInZone': 384,
           'CGangWars::MakePlayerGainInfluenceInZone': 384,
           'CGangWars::PedStreamedInForThisGang': 256,
           'CGangWars::PickStreamedInPedForThisGang': 384,
           'CGangWars::PickZoneToAttack': 512,
           'CGangWars::ReleaseCarsInAttackWave': 192,
           'CGangWars::ReleasePedsInAttackWave': 384,
           'CGangWars::Save': 192,
           'CGangWars::SetGangWarsActive': 192,
           'CGangWars::SetSpecificZoneToTriggerGangWar': 128,
           'CGangWars::StartDefensiveGangWar': 256,
           'CGangWars::StartOffensiveGangWar': 512,
           'CGangWars::StrengthenPlayerInfluenceInZone': 256,
           'CGangWars::SwitchGangWarsActive': 64,
           'CGangWars::TellGangMembersTo': 384,
           'CGangWars::TellStreamingWhichGangsAreNeeded': 128,
           'CGangWars::UpdateTerritoryUnderControlPercentage': 384},
 'kind': 'CPU en el juego completo con partida cargada, peds/grupos/vehículos falsos y colaboradores doblados con traza (banco simple, passed=true global)',
 'lim': {'CGangWars::AttackWaveOvercome': '',
         'CGangWars::CalculateTimeTillNextAttack': '',
         'CGangWars::CanPlayerStartAGangWarHere': '',
         'CGangWars::CheerVictory': '',
         'CGangWars::ClearSpecificZonesToTriggerGangWar': '',
         'CGangWars::ClearTheStreets': '',
         'CGangWars::CreateDefendingGroup': '',
         'CGangWars::DoStuffWhenPlayerVictorious': '',
         'CGangWars::DoesPlayerControlThisZone': '',
         'CGangWars::DontCreateCivilians': '',
         'CGangWars::EndGangWar': '',
         'CGangWars::GangWarFightingGoingOn': '',
         'CGangWars::GangWarGoingOn': '',
         'CGangWars::InitAtStartOfGame': '',
         'CGangWars::Load': 'CGangWarsSaveStructure::Construct corregido en la fuente y sin enmascarar; ',
         'CGangWars::MakeEnemyGainInfluenceInZone': '',
         'CGangWars::MakePlayerGainInfluenceInZone': '',
         'CGangWars::PedStreamedInForThisGang': '',
         'CGangWars::PickStreamedInPedForThisGang': '',
         'CGangWars::PickZoneToAttack': '',
         'CGangWars::ReleaseCarsInAttackWave': '',
         'CGangWars::ReleasePedsInAttackWave': '',
         'CGangWars::Save': 'CGangWarsSaveStructure::Construct corregido en la fuente y sin enmascarar; ',
         'CGangWars::SetGangWarsActive': '',
         'CGangWars::SetSpecificZoneToTriggerGangWar': '',
         'CGangWars::StartDefensiveGangWar': '',
         'CGangWars::StartOffensiveGangWar': '',
         'CGangWars::StrengthenPlayerInfluenceInZone': '',
         'CGangWars::SwitchGangWarsActive': '',
         'CGangWars::TellGangMembersTo': '',
         'CGangWars::TellStreamingWhichGangsAreNeeded': '',
         'CGangWars::UpdateTerritoryUnderControlPercentage': '',
         'CGroupEventHandler::ComputeDoDealResponse': '',
         'CGroupEventHandler::ComputeDrivebyResponse': '',
         'CGroupEventHandler::ComputeEventResponseTasks': '',
         'CGroupEventHandler::ComputeFleePedResponse': '',
         'CGroupEventHandler::ComputeGreetResponse': '',
         'CGroupEventHandler::ComputeHandSignalResponse': '',
         'CGroupEventHandler::ComputeHassleSexyPedResponse': '',
         'CGroupEventHandler::ComputeHassleThreatResponse': '',
         'CGroupEventHandler::ComputeKillPlayerBasicResponse': '',
         'CGroupEventHandler::ComputeKillThreatsBasicResponse': '',
         'CGroupEventHandler::ComputeLeanOnVehicleResponse': '',
         'CGroupEventHandler::ComputeMemberResponses': '',
         'CGroupEventHandler::ComputeResponseDamage': '',
         'CGroupEventHandler::ComputeResponseDanger': '',
         'CGroupEventHandler::ComputeResponseDraggedOutCar': '',
         'CGroupEventHandler::ComputeResponseGather': '',
         'CGroupEventHandler::ComputeResponseGunAimedAt': '',
         'CGroupEventHandler::ComputeResponseLeaderEnterExit': 'ALCANCE LIMITADO: solo el camino ProcessPed==true (el otro llega a CAnimManager::BlendAnimation con un '
                                                               'peatón falso); ',
         'CGroupEventHandler::ComputeResponseLeaderExitedCar': '',
         'CGroupEventHandler::ComputeResponseNewGangMember': '',
         'CGroupEventHandler::ComputeResponsePedFriend': '',
         'CGroupEventHandler::ComputeResponsePedThreat': '',
         'CGroupEventHandler::ComputeResponsePlayerCommand': '',
         'CGroupEventHandler::ComputeResponseSeenCop': '',
         'CGroupEventHandler::ComputeResponseSexyPed': '',
         'CGroupEventHandler::ComputeResponseShotFired': '',
         'CGroupEventHandler::ComputeResponseVehicleDamage': '',
         'CGroupEventHandler::ComputeStareResponse': '',
         'CGroupEventHandler::ComputeWalkAlongsideResponse': '',
         'CGroupEventHandler::IsKillTaskAppropriate': ''},
 'note': 'GangEvent1 añade la aprobación de las sesenta y dos funciones pendientes de CGroupEventHandler (30) y CGangWars (32), 17792 pares con cero diferencias. La '
         'corrida 20261009-162300-336352 tiene passed=true global y evento complete con state_restored=true, saves y configuraciones intactos, con el banco simple (hook '
         'apagado) frente a la candidata dentro del juego completo con partida cargada. Con ellas CGroupEventHandler y CGangWars quedan COMPLETAS en hooks.csv (sin '
         'pendientes). Límites: ComputeResponseLeaderEnterExit solo con ProcessPed==true (el otro camino llega a CAnimManager::BlendAnimation con un peatón falso); '
         'peds, inteligencias, grupos y vehículos falsos con pools falsos intercambiados solo durante la llamada; tareas y eventos en arenas con 0xCD (relleno no '
         'escrito ignorado) y sin delete de los objetos devueltos (solo destructor); colaboradores doblados con traza (sin crédito). CGangWarsSaveStructure::Construct '
         'se corrigió en la fuente y ya no se enmascara. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. '
         '[Resumen](../reversing/GangEvent1/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut437.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
