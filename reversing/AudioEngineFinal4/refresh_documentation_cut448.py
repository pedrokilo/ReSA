"""Documentary cut448 (+82 funciones: AudioEngineFinal4); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'complete': ['CAudioEngine'],
 'groups_drop': 1,
 'intro': 'ochenta y dos de CAudioEngine (AudioEngineFinal4)',
 'kind': 'CPU en el juego completo con partida cargada, CAudioEngine falso y colaboradores doblados con traza (banco por pares, '
         'passed=true global)',
 'lim': {'CAudioEngine::AttachMissionAudioToObject': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                     'con traza sin crédito; ',
         'CAudioEngine::AttachMissionAudioToPed': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                  'con traza sin crédito; ',
         'CAudioEngine::AttachMissionAudioToPhysical': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                       'doblados con traza sin crédito; ',
         'CAudioEngine::ClearMissionAudio': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                            'traza sin crédito; ',
         'CAudioEngine::Constructor': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                      'crédito; fuente corregida (0x507670): construcción en el sitio con las escrituras del EXE '
                                      'en vez de los constructores de C++. el constructor del EXE se ejecuta con relleno 0xCD y '
                                      '0x00 para deducir los bytes escritos; vtables como escrita/no escrita. ',
         'CAudioEngine::Destructor': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                     'crédito; el estado de partida es el del constructor; con referencias vivas el EXE llama a '
                                     'CEntity::CleanUpOldReference (fuera del fixture). ',
         'CAudioEngine::DisableEffectsLoading': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                'traza sin crédito; ',
         'CAudioEngine::DisplayRadioStationName': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                  'con traza sin crédito; ',
         'CAudioEngine::DoesAmbienceTrackOverrideRadio': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                         'doblados con traza sin crédito; ',
         'CAudioEngine::EnableEffectsLoading': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                               'traza sin crédito; ',
         'CAudioEngine::GetBeatInfo': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                      'crédito; ',
         'CAudioEngine::GetBeatTrackStatus': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                             'traza sin crédito; ',
         'CAudioEngine::GetCurrentRadioStationID': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                   'con traza sin crédito; ',
         'CAudioEngine::GetCutsceneTrackStatus': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                 'traza sin crédito; ',
         'CAudioEngine::GetMissionAudioEvent': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                               'traza sin crédito; ',
         'CAudioEngine::GetMissionAudioLoadingStatus': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                       'doblados con traza sin crédito; ',
         'CAudioEngine::GetMissionAudioPosition': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                  'con traza sin crédito; ',
         'CAudioEngine::GetRadioStationListenTimes': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                     'con traza sin crédito; ',
         'CAudioEngine::GetRadioStationName': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                              'traza sin crédito; ',
         'CAudioEngine::GetRadioStationNameKey': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                 'traza sin crédito; ',
         'CAudioEngine::HasRadioRetuneJustStarted': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                    'con traza sin crédito; ',
         'CAudioEngine::Initialise': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                     'crédito; fuente corregida (0x5B9C60): StaticInitialise tras hardware y managers, sin '
                                     'StaticInitialise de WaterCannon y OpenFile nulo vuelve sin CloseFile. partes inlinadas del '
                                     'EXE (canal, managers de cutscene y ambiente, lectura de EVENTVOL.DAT simulada) frente a '
                                     'las llamadas de la DLL; relleno de tAmbienceSettings a cero antes de hashear. ',
         'CAudioEngine::InitialisePostLoading': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                'traza sin crédito; CAEGlobalWeaponAudioEntity real: solo se comparan los bytes '
                                                'que escribe el EXE; el CPhysical interno no se compara; Service trazado. ',
         'CAudioEngine::InitialiseRadioStationID': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                   'con traza sin crédito; ',
         'CAudioEngine::IsAmbienceRadioActive': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                'traza sin crédito; ',
         'CAudioEngine::IsAmbienceTrackActive': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                'traza sin crédito; ',
         'CAudioEngine::IsBeatInfoPresent': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                            'traza sin crédito; ',
         'CAudioEngine::IsCutsceneTrackActive': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                'traza sin crédito; ',
         'CAudioEngine::IsLoadingTuneActive': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                              'traza sin crédito; ',
         'CAudioEngine::IsMissionAudioSampleFinished': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                       'doblados con traza sin crédito; ',
         'CAudioEngine::IsRadioOn': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                    'crédito; ',
         'CAudioEngine::IsRadioRetuneInProgress': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                  'con traza sin crédito; ',
         'CAudioEngine::IsVehicleRadioActive': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                               'traza sin crédito; ',
         'CAudioEngine::PauseAllSounds': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                         'sin crédito; ',
         'CAudioEngine::PauseBeatTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                         'sin crédito; argumentos solo 0/1. ',
         'CAudioEngine::PlayLoadedMissionAudio': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                 'traza sin crédito; ',
         'CAudioEngine::PlayPreloadedBeatTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                 'traza sin crédito; argumentos solo 0/1. ',
         'CAudioEngine::PlayPreloadedCutsceneTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                     'con traza sin crédito; ',
         'CAudioEngine::PreloadBeatTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                           'sin crédito; ',
         'CAudioEngine::PreloadCutsceneTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                               'traza sin crédito; ',
         'CAudioEngine::PreloadMissionAudio': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                              'traza sin crédito; ',
         'CAudioEngine::ReportBulletHit': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                          'sin crédito; ',
         'CAudioEngine::ReportCollision': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                          'sin crédito; ',
         'CAudioEngine::ReportDoorMovement': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                             'traza sin crédito; ',
         'CAudioEngine::ReportFrontendAudioEvent': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                   'con traza sin crédito; ',
         'CAudioEngine::ReportGlassCollisionEvent': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                    'con traza sin crédito; ',
         'CAudioEngine::ReportMissionAudioEvent-obj': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                      'doblados con traza sin crédito; eventos de misión > 0xFFFF fuera del '
                                                      'fixture (la DLL trunca a uint16). ',
         'CAudioEngine::ReportMissionAudioEvent-ped': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                      'doblados con traza sin crédito; eventos de misión > 0xFFFF fuera del '
                                                      'fixture (la DLL trunca a uint16). ',
         'CAudioEngine::ReportMissionAudioEvent-phs': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                      'doblados con traza sin crédito; eventos de misión > 0xFFFF fuera del '
                                                      'fixture (la DLL trunca a uint16). ',
         'CAudioEngine::ReportMissionAudioEvent-vec': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                      'doblados con traza sin crédito; eventos de misión > 0xFFFF fuera del '
                                                      'fixture (la DLL trunca a uint16). ',
         'CAudioEngine::ReportMissionAudioEvent-veh': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                      'doblados con traza sin crédito; eventos de misión > 0xFFFF fuera del '
                                                      'fixture (la DLL trunca a uint16). ',
         'CAudioEngine::ReportObjectDestruction': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                  'con traza sin crédito; ',
         'CAudioEngine::ReportWaterSplash-phs': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                'traza sin crédito; ',
         'CAudioEngine::ReportWaterSplash-vec': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                'traza sin crédito; ',
         'CAudioEngine::ReportWeaponEvent': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                            'traza sin crédito; ',
         'CAudioEngine::Reset': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                'crédito; ',
         'CAudioEngine::ResetSoundEffects': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                            'traza sin crédito; ',
         'CAudioEngine::ResetStatistics': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                          'sin crédito; ',
         'CAudioEngine::Restart': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                  'crédito; ',
         'CAudioEngine::ResumeAllSounds': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                          'sin crédito; ',
         'CAudioEngine::RetuneRadio': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                      'crédito; ',
         'CAudioEngine::SayPedless': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                     'crédito; ',
         'CAudioEngine::Service': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                  'crédito; fuente corregida (0x50779A..0x5077A6): se restituye la llamada a '
                                  'GetChannelPlayTimes(canal, nullptr). m_nBackgroundAudioChannel negativo fuera del fixture. ',
         'CAudioEngine::ServiceLoadingTune': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                             'traza sin crédito; ',
         'CAudioEngine::SetBassEnhanceOnOff': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                              'traza sin crédito; ',
         'CAudioEngine::SetEffectsFaderScalingFactor': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                       'doblados con traza sin crédito; ',
         'CAudioEngine::SetEffectsMasterVolume': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                                 'traza sin crédito; ',
         'CAudioEngine::SetMissionAudioPosition': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                  'con traza sin crédito; ',
         'CAudioEngine::SetMusicFaderScalingFactor': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                     'con traza sin crédito; ',
         'CAudioEngine::SetMusicMasterVolume': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                               'traza sin crédito; ',
         'CAudioEngine::SetNonStreamFaderScalingFactor': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                         'doblados con traza sin crédito; ',
         'CAudioEngine::SetRadioAutoRetuneOnOff': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados '
                                                  'con traza sin crédito; ',
         'CAudioEngine::SetRadioBassSetting': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                              'traza sin crédito; ',
         'CAudioEngine::SetStreamFaderScalingFactor': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores '
                                                      'doblados con traza sin crédito; ',
         'CAudioEngine::Shutdown': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                   'crédito; delete/delete[] trazados sin liberar; la entidad de arma falsa se destruye de '
                                   'verdad en ambos lados (vtable enmascarada); 0xBD00F8 nulo fuera del fixture. ',
         'CAudioEngine::StartLoadingTune': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                           'sin crédito; ',
         'CAudioEngine::StartRadio-': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                      'crédito; ',
         'CAudioEngine::StartRadio-1': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                       'crédito; ',
         'CAudioEngine::StopAmbienceTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                            'traza sin crédito; ',
         'CAudioEngine::StopBeatTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza '
                                        'sin crédito; ',
         'CAudioEngine::StopCutsceneTrack': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con '
                                            'traza sin crédito; ',
         'CAudioEngine::StopRadio': 'CAudioEngine falso en memoria propia y mundo falso, colaboradores doblados con traza sin '
                                    'crédito; '},
 'n': 82,
 'new_word': 'Mil doscientas treinta y nueve',
 'note': 'AudioEngineFinal4 añade la aprobación de las 82 funciones de CAudioEngine, 17280 pares con cero diferencias. La '
         'corrida 20261010-125652-130519 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, '
         'saves y configuraciones intactos, con el banco (hooks de la clase apagados) frente a la candidata dentro del juego '
         'completo con partida cargada. Límites: CAudioEngine falso en memoria propia (0x1FD8 bytes) y mundo falso estático; '
         'colaboradores (hardware, entidades '
         'Frontend/Script/Colisión/GlobalWeapon/Puerta/Pedless/PedSpeech/PoliceScanner/Weather, managers de radio, cutscene y '
         'ambiente, CAESoundManager, FindPlayerPed y, en Initialise, pantalla de carga, CFileMgr e inicializaciones estáticas) '
         'doblados con traza sin crédito; enmascarados las vtables de los objetos embebidos y el relleno de tAmbienceSettings; '
         'en InitialisePostLoading solo se comparan los bytes que escribe el EXE y no el CPhysical interno; fuera del fixture '
         'm_nBackgroundAudioChannel negativo en Service, 0xBD00F8 nulo en Shutdown, eventos de misión > 0xFFFF e ids de emisora '
         'fuera de int8; el Destructor parte del estado que deja el constructor. Correcciones de fuente contra el EXE en '
         'AudioEngine.cpp/.h y AEAudioEntity.cpp (Constructor, Initialise, Service; ver el README del banco). No quedan '
         'pendientes en CAudioEngine. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. '
         '[Resumen](../reversing/AudioEngineFinal4/validation-summary.json).',
 'old_appr': 1157,
 'old_groups': 7,
 'old_pend': 553,
 'old_valid': 7893,
 'old_word': 'Mil ciento cincuenta y siete',
 'pairs': {'CAudioEngine::AttachMissionAudioToObject': 128,
           'CAudioEngine::AttachMissionAudioToPed': 128,
           'CAudioEngine::AttachMissionAudioToPhysical': 128,
           'CAudioEngine::ClearMissionAudio': 128,
           'CAudioEngine::Constructor': 64,
           'CAudioEngine::Destructor': 64,
           'CAudioEngine::DisableEffectsLoading': 64,
           'CAudioEngine::DisplayRadioStationName': 64,
           'CAudioEngine::DoesAmbienceTrackOverrideRadio': 64,
           'CAudioEngine::EnableEffectsLoading': 64,
           'CAudioEngine::GetBeatInfo': 128,
           'CAudioEngine::GetBeatTrackStatus': 128,
           'CAudioEngine::GetCurrentRadioStationID': 128,
           'CAudioEngine::GetCutsceneTrackStatus': 128,
           'CAudioEngine::GetMissionAudioEvent': 128,
           'CAudioEngine::GetMissionAudioLoadingStatus': 128,
           'CAudioEngine::GetMissionAudioPosition': 128,
           'CAudioEngine::GetRadioStationListenTimes': 64,
           'CAudioEngine::GetRadioStationName': 128,
           'CAudioEngine::GetRadioStationNameKey': 128,
           'CAudioEngine::HasRadioRetuneJustStarted': 64,
           'CAudioEngine::Initialise': 1024,
           'CAudioEngine::InitialisePostLoading': 512,
           'CAudioEngine::InitialiseRadioStationID': 128,
           'CAudioEngine::IsAmbienceRadioActive': 64,
           'CAudioEngine::IsAmbienceTrackActive': 64,
           'CAudioEngine::IsBeatInfoPresent': 128,
           'CAudioEngine::IsCutsceneTrackActive': 64,
           'CAudioEngine::IsLoadingTuneActive': 64,
           'CAudioEngine::IsMissionAudioSampleFinished': 128,
           'CAudioEngine::IsRadioOn': 64,
           'CAudioEngine::IsRadioRetuneInProgress': 64,
           'CAudioEngine::IsVehicleRadioActive': 64,
           'CAudioEngine::PauseAllSounds': 64,
           'CAudioEngine::PauseBeatTrack': 64,
           'CAudioEngine::PlayLoadedMissionAudio': 128,
           'CAudioEngine::PlayPreloadedBeatTrack': 128,
           'CAudioEngine::PlayPreloadedCutsceneTrack': 64,
           'CAudioEngine::PreloadBeatTrack': 512,
           'CAudioEngine::PreloadCutsceneTrack': 512,
           'CAudioEngine::PreloadMissionAudio': 128,
           'CAudioEngine::ReportBulletHit': 256,
           'CAudioEngine::ReportCollision': 512,
           'CAudioEngine::ReportDoorMovement': 128,
           'CAudioEngine::ReportFrontendAudioEvent': 256,
           'CAudioEngine::ReportGlassCollisionEvent': 256,
           'CAudioEngine::ReportMissionAudioEvent-obj': 128,
           'CAudioEngine::ReportMissionAudioEvent-ped': 128,
           'CAudioEngine::ReportMissionAudioEvent-phs': 256,
           'CAudioEngine::ReportMissionAudioEvent-vec': 128,
           'CAudioEngine::ReportMissionAudioEvent-veh': 128,
           'CAudioEngine::ReportObjectDestruction': 128,
           'CAudioEngine::ReportWaterSplash-phs': 256,
           'CAudioEngine::ReportWaterSplash-vec': 256,
           'CAudioEngine::ReportWeaponEvent': 256,
           'CAudioEngine::Reset': 1024,
           'CAudioEngine::ResetSoundEffects': 128,
           'CAudioEngine::ResetStatistics': 64,
           'CAudioEngine::Restart': 64,
           'CAudioEngine::ResumeAllSounds': 64,
           'CAudioEngine::RetuneRadio': 256,
           'CAudioEngine::SayPedless': 256,
           'CAudioEngine::Service': 1024,
           'CAudioEngine::ServiceLoadingTune': 1024,
           'CAudioEngine::SetBassEnhanceOnOff': 64,
           'CAudioEngine::SetEffectsFaderScalingFactor': 256,
           'CAudioEngine::SetEffectsMasterVolume': 256,
           'CAudioEngine::SetMissionAudioPosition': 128,
           'CAudioEngine::SetMusicFaderScalingFactor': 256,
           'CAudioEngine::SetMusicMasterVolume': 256,
           'CAudioEngine::SetNonStreamFaderScalingFactor': 256,
           'CAudioEngine::SetRadioAutoRetuneOnOff': 64,
           'CAudioEngine::SetRadioBassSetting': 128,
           'CAudioEngine::SetStreamFaderScalingFactor': 256,
           'CAudioEngine::Shutdown': 512,
           'CAudioEngine::StartLoadingTune': 64,
           'CAudioEngine::StartRadio-': 256,
           'CAudioEngine::StartRadio-1': 256,
           'CAudioEngine::StopAmbienceTrack': 256,
           'CAudioEngine::StopBeatTrack': 256,
           'CAudioEngine::StopCutsceneTrack': 512,
           'CAudioEngine::StopRadio': 128},
 'pending_counts': {},
 'summary': 'reversing/AudioEngineFinal4/validation-summary.json'}
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
(HERE/'documentation-refresh-cut448.json').write_text(json.dumps({'date':'2026-10-10','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
