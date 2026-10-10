"""Documentary cut444 (+72 funciones: CheatFinal3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 946,
 'old_valid': 7682,
 'old_pend': 764,
 'old_groups': 10,
 'n': 72,
 'groups_drop': 1,
 'old_word': 'Novecientas cuarenta y seis',
 'new_word': 'Mil dieciocho',
 'intro': 'setenta y dos de CCheat (CheatFinal3)',
 'complete': ['CCheat'],
 'pending_counts': {},
 'summary': 'reversing/CheatFinal3/validation-summary.json',
 'pairs': {'CCheat::AddToCheatString': 1024,
           'CCheat::AdrenalineCheat': 128,
           'CCheat::AllCarsAreGreatCheat': 128,
           'CCheat::AllCarsAreShitCheat': 128,
           'CCheat::ApacheCheat': 64,
           'CCheat::ApplyCheat': 256,
           'CCheat::BeachPartyCheat': 192,
           'CCheat::BlackCarsCheat': 128,
           'CCheat::BlowUpCarsCheat': 256,
           'CCheat::CloudyWeatherCheat': 64,
           'CCheat::CountrysideInvasionCheat': 192,
           'CCheat::DoCheats': 512,
           'CCheat::DozerCheat': 64,
           'CCheat::DrivebyCheat': 192,
           'CCheat::DuskCheat': 128,
           'CCheat::ElvisLivesCheat': 128,
           'CCheat::EverybodyAttacksPlayerCheat': 256,
           'CCheat::ExtraSunnyWeatherCheat': 64,
           'CCheat::FastTimeCheat': 192,
           'CCheat::FatCheat': 128,
           'CCheat::FlyboyCheat': 64,
           'CCheat::FoggyWeatherCheat': 64,
           'CCheat::FunhouseCheat': 192,
           'CCheat::GangLandCheat': 128,
           'CCheat::GangsCheat': 128,
           'CCheat::GolfcartCheat': 64,
           'CCheat::HandleSpecialCheats': 512,
           'CCheat::HearseCheat': 64,
           'CCheat::IsZoneStreamingAllowed': 256,
           'CCheat::JetpackCheat': 192,
           'CCheat::LoveConquersAllCheat': 128,
           'CCheat::LovefistCheat': 64,
           'CCheat::MayhemCheat': 256,
           'CCheat::MidnightCheat': 128,
           'CCheat::MoneyArmourHealthCheat': 192,
           'CCheat::MonsterTruckCheat': 64,
           'CCheat::MuscleCheat': 128,
           'CCheat::NinjaCheat': 192,
           'CCheat::NotWantedCheat': 128,
           'CCheat::ParachuteCheat': 128,
           'CCheat::PinkCarsCheat': 128,
           'CCheat::PredatorCheat': 64,
           'CCheat::QuadCheat': 64,
           'CCheat::RainyWeatherCheat': 64,
           'CCheat::ResetCheats': 128,
           'CCheat::RiotCheat': 128,
           'CCheat::SandstormCheat': 64,
           'CCheat::SkinnyCheat': 128,
           'CCheat::SlowTimeCheat': 192,
           'CCheat::StaminaCheat': 64,
           'CCheat::StockCar2Cheat': 64,
           'CCheat::StockCar3Cheat': 64,
           'CCheat::StockCar4Cheat': 64,
           'CCheat::StockCarCheat': 64,
           'CCheat::StormCheat': 64,
           'CCheat::StuntPlaneCheat': 64,
           'CCheat::SuicideCheat': 192,
           'CCheat::SunnyWeatherCheat': 64,
           'CCheat::TankCheat': 64,
           'CCheat::TankerCheat': 192,
           'CCheat::TrashmasterCheat': 64,
           'CCheat::VehicleCheat': 1024,
           'CCheat::VehicleSkillsCheat': 64,
           'CCheat::VillagePeopleCheat': 192,
           'CCheat::VortexCheat': 64,
           'CCheat::WantedCheat': 64,
           'CCheat::WantedLevelDownCheat': 64,
           'CCheat::WantedLevelUpCheat': 192,
           'CCheat::WeaponCheat1': 192,
           'CCheat::WeaponCheat2': 192,
           'CCheat::WeaponCheat3': 192,
           'CCheat::WeaponSkillsCheat': 64},
 'kind': 'CPU en el juego completo con partida cargada, jugadores, pools y lista de referencias falsos y colaboradores doblados con traza (banco '
         'simple, passed=true global)',
 'lim': {'CCheat::AddToCheatString': 'fuente corregida: cadena completa acortada por el final, primer indice de la tabla (0x438500..0x438527, '
                                     '0x438510), m_bHasPlayerCheated antes del truco (0x438576), ejecucion en linea; ',
         'CCheat::AdrenalineCheat': 'fuente corregida: al desactivar solo limpia el bit 1 de [m_pPlayerData+0x34] (0x4398A8); ',
         'CCheat::AllCarsAreGreatCheat': '',
         'CCheat::AllCarsAreShitCheat': '',
         'CCheat::ApacheCheat': '',
         'CCheat::ApplyCheat': '',
         'CCheat::BeachPartyCheat': 'fuente corregida: sin SetTextureAndModel previo (0x439261..0x439289); ',
         'CCheat::BlackCarsCheat': '',
         'CCheat::BlowUpCarsCheat': 'fuente corregida: pool de la ultima ranura a la primera (0x439D90); ',
         'CCheat::CloudyWeatherCheat': '',
         'CCheat::CountrysideInvasionCheat': '',
         'CCheat::DoCheats': '',
         'CCheat::DozerCheat': '',
         'CCheat::DrivebyCheat': '',
         'CCheat::DuskCheat': 'fuente corregida: 21:00:00 al activar (0x43954C); ',
         'CCheat::ElvisLivesCheat': '',
         'CCheat::EverybodyAttacksPlayerCheat': 'fuente corregida: pool de la ultima ranura a la primera (0x439CB4); ',
         'CCheat::ExtraSunnyWeatherCheat': 'ejecutada de verdad; la traza sale de ForceWeatherNow (0x72A4F0); ',
         'CCheat::FastTimeCheat': 'denormales fuera de este caso (solo NaN/Inf en las escalas de tiempo); ',
         'CCheat::FatCheat': '',
         'CCheat::FlyboyCheat': '',
         'CCheat::FoggyWeatherCheat': '',
         'CCheat::FunhouseCheat': 'fuente corregida: sin SetTextureAndModel previo (0x439776) y rama activa/inactiva invertida (0x439735); ',
         'CCheat::GangLandCheat': '',
         'CCheat::GangsCheat': '',
         'CCheat::GolfcartCheat': '',
         'CCheat::HandleSpecialCheats': '',
         'CCheat::HearseCheat': '',
         'CCheat::IsZoneStreamingAllowed': '',
         'CCheat::JetpackCheat': '',
         'CCheat::LoveConquersAllCheat': '',
         'CCheat::LovefistCheat': '',
         'CCheat::MayhemCheat': 'fuente corregida: sin std::swap ni rama de desactivacion, pool de la ultima ranura a la primera (0x439B4A); ',
         'CCheat::MidnightCheat': '',
         'CCheat::MoneyArmourHealthCheat': 'fuente corregida: vida/armadura maximas del jugador 0 y dinero del jugador en foco (0x438E40; '
                                           'HealthCheat 0x438D60); ',
         'CCheat::MonsterTruckCheat': '',
         'CCheat::MuscleCheat': '',
         'CCheat::NinjaCheat': 'fuente corregida: rama activa/inactiva invertida (0x439E68); ',
         'CCheat::NotWantedCheat': 'fuente corregida: sin bWantedByPolice = false (0x4396C0..0x4396E2); ',
         'CCheat::ParachuteCheat': '',
         'CCheat::PinkCarsCheat': '',
         'CCheat::PredatorCheat': 'fuente corregida: el EXE es un ret (0x4395A0); ',
         'CCheat::QuadCheat': '',
         'CCheat::RainyWeatherCheat': 'ejecutada de verdad; la traza sale de ForceWeatherNow (0x72A4F0); ',
         'CCheat::ResetCheats': '',
         'CCheat::RiotCheat': '',
         'CCheat::SandstormCheat': '',
         'CCheat::SkinnyCheat': '',
         'CCheat::SlowTimeCheat': 'sin denormales; NaN/Inf en las escalas de tiempo; ',
         'CCheat::StaminaCheat': '',
         'CCheat::StockCar2Cheat': '',
         'CCheat::StockCar3Cheat': '',
         'CCheat::StockCar4Cheat': '',
         'CCheat::StockCarCheat': '',
         'CCheat::StormCheat': '',
         'CCheat::StuntPlaneCheat': '',
         'CCheat::SuicideCheat': '',
         'CCheat::SunnyWeatherCheat': '',
         'CCheat::TankCheat': '',
         'CCheat::TankerCheat': 'cola (posicion, estado, CWorld::Add, PlaceOnRoadProperly, SetTowLink) verificada solo por ensamblador: operator new '
                                'devuelve nulo y el constructor 0x6D03A0 no se ejecuta; fuente corregida: nulo tras new CTrailer (0x43A5F3) y angulo '
                                '0x405F66F3 (0x43A616); ',
         'CCheat::TrashmasterCheat': '',
         'CCheat::VehicleCheat': 'cola (posicion, orientacion, estado, m_nDoorLock, CWorld::Add, ClearSpaceForMissionEntity, PlaceOnRoadProperly, '
                                 'SetTowLink) verificada solo por ensamblador: operator new devuelve nulo y los constructores de vehiculo (0x6B0A90, '
                                 '0x6C8D60, 0x6CE370, 0x6C4190, 0x6C8E20, 0x6F2940, 0x6BF430, 0x6BF820, 0x6D03A0) no se ejecutan; tipos de vehiculo '
                                 '9 y 10 excluidos por AV 0x43A2A8; fuente corregida: cuenta >50 -> nulo (0x43A0E2..0x43A105), banderas antes de '
                                 'RequestModel (0x43A128/0x43A14E), tabla de saltos (0x43A17B), pi/2 (0x858FE4), PlaceOnRoadProperly solo '
                                 'CBike/CAutomobile (0x43A426..0x43A456); ',
         'CCheat::VehicleSkillsCheat': '',
         'CCheat::VillagePeopleCheat': 'fuente corregida: rama activa/inactiva invertida (0x439DE5); ',
         'CCheat::VortexCheat': '',
         'CCheat::WantedCheat': '',
         'CCheat::WantedLevelDownCheat': '',
         'CCheat::WantedLevelUpCheat': 'fuente corregida: sin comprobacion de nulo (0x438E90); ',
         'CCheat::WeaponCheat1': 'fuente corregida: FindPlayerPed(-1) (0x43863D), juego al jugador 1 (0x438736), nueve modelos deletables '
                                 '(0x438825..0x43887F); ',
         'CCheat::WeaponCheat2': 'fuente corregida: FindPlayerPed(-1) (0x43890D); ',
         'CCheat::WeaponCheat3': 'fuente corregida: FindPlayerPed(-1) (0x438B9D); ',
         'CCheat::WeaponSkillsCheat': ''},
 'note': 'CheatFinal3 añade la aprobación de las setenta y dos funciones de CCheat, 11584 pares con cero diferencias. La corrida '
         '20261009-200638-812639 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, saves y configuraciones '
         'intactos, con el banco (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Límites: 3 jugadores falsos con '
         'CPlayerPedData, CWanted y CPedClothesDesc, pools de peds y vehiculos falsos intercambiados solo durante la llamada, lista de referencias '
         'falsa; operator new de CVehicle devuelve nulo; denormales/NaN solo en las escalas de tiempo; colaboradores doblados con traza (sin '
         'credito); HealthCheat (sin hook) se probo en el banco pero no se registra. Correcciones de fuente por función en el README del banco y en '
         'el alcance de cada función del registro. CCheat::HealthCheat (sin hook) se probó en el banco (256 pares) pero no se registra. No quedan '
         'pendientes en CCheat. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. '
         '[Resumen](../reversing/CheatFinal3/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut444.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
