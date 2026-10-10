"""Registro historico de las 82 funciones de CAudioEngine del banco AudioEngineFinal4 (corrida 20261010-125652-130519). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'addr': {'CAudioEngine::AttachMissionAudioToObject': '0x00507320',
          'CAudioEngine::AttachMissionAudioToPed': '0x00507310',
          'CAudioEngine::AttachMissionAudioToPhysical': '0x00507330',
          'CAudioEngine::ClearMissionAudio': '0x005072F0',
          'CAudioEngine::Constructor': '0x00507670',
          'CAudioEngine::Destructor': '0x00506CD0',
          'CAudioEngine::DisableEffectsLoading': '0x00506E80',
          'CAudioEngine::DisplayRadioStationName': '0x00507030',
          'CAudioEngine::DoesAmbienceTrackOverrideRadio': '0x00507270',
          'CAudioEngine::EnableEffectsLoading': '0x00506E90',
          'CAudioEngine::GetBeatInfo': '0x005071B0',
          'CAudioEngine::GetBeatTrackStatus': '0x00507170',
          'CAudioEngine::GetCurrentRadioStationID': '0x00507040',
          'CAudioEngine::GetCutsceneTrackStatus': '0x00507160',
          'CAudioEngine::GetMissionAudioEvent': '0x005072D0',
          'CAudioEngine::GetMissionAudioLoadingStatus': '0x005072A0',
          'CAudioEngine::GetMissionAudioPosition': '0x005072E0',
          'CAudioEngine::GetRadioStationListenTimes': '0x00507020',
          'CAudioEngine::GetRadioStationName': '0x00507000',
          'CAudioEngine::GetRadioStationNameKey': '0x00507010',
          'CAudioEngine::HasRadioRetuneJustStarted': '0x00506FE0',
          'CAudioEngine::Initialise': '0x005B9C60',
          'CAudioEngine::InitialisePostLoading': '0x005078F0',
          'CAudioEngine::InitialiseRadioStationID': '0x00506FC0',
          'CAudioEngine::IsAmbienceRadioActive': '0x00507280',
          'CAudioEngine::IsAmbienceTrackActive': '0x00507210',
          'CAudioEngine::IsBeatInfoPresent': '0x005071D0',
          'CAudioEngine::IsCutsceneTrackActive': '0x00507150',
          'CAudioEngine::IsLoadingTuneActive': '0x00506D90',
          'CAudioEngine::IsMissionAudioSampleFinished': '0x005072C0',
          'CAudioEngine::IsRadioOn': '0x00506FD0',
          'CAudioEngine::IsRadioRetuneInProgress': '0x00506FF0',
          'CAudioEngine::IsVehicleRadioActive': '0x00507050',
          'CAudioEngine::PauseAllSounds': '0x00507430',
          'CAudioEngine::PauseBeatTrack': '0x00507200',
          'CAudioEngine::PlayLoadedMissionAudio': '0x005072B0',
          'CAudioEngine::PlayPreloadedBeatTrack': '0x00507180',
          'CAudioEngine::PlayPreloadedCutsceneTrack': '0x00507070',
          'CAudioEngine::PreloadBeatTrack': '0x00507F40',
          'CAudioEngine::PreloadCutsceneTrack': '0x00507E30',
          'CAudioEngine::PreloadMissionAudio': '0x00507290',
          'CAudioEngine::ReportBulletHit': '0x00506EC0',
          'CAudioEngine::ReportCollision': '0x00506EB0',
          'CAudioEngine::ReportDoorMovement': '0x00506F50',
          'CAudioEngine::ReportFrontendAudioEvent': '0x00506EA0',
          'CAudioEngine::ReportGlassCollisionEvent': '0x00506EE0',
          'CAudioEngine::ReportMissionAudioEvent-obj': '0x00507350',
          'CAudioEngine::ReportMissionAudioEvent-ped': '0x00507370',
          'CAudioEngine::ReportMissionAudioEvent-phs': '0x005073B0',
          'CAudioEngine::ReportMissionAudioEvent-vec': '0x00507340',
          'CAudioEngine::ReportMissionAudioEvent-veh': '0x00507390',
          'CAudioEngine::ReportObjectDestruction': '0x00506ED0',
          'CAudioEngine::ReportWaterSplash-phs': '0x00506F00',
          'CAudioEngine::ReportWaterSplash-vec': '0x00506F10',
          'CAudioEngine::ReportWeaponEvent': '0x00506F40',
          'CAudioEngine::Reset': '0x00507A90',
          'CAudioEngine::ResetSoundEffects': '0x00507C30',
          'CAudioEngine::ResetStatistics': '0x00506DA0',
          'CAudioEngine::Restart': '0x00506DB0',
          'CAudioEngine::ResumeAllSounds': '0x00507440',
          'CAudioEngine::RetuneRadio': '0x00507E10',
          'CAudioEngine::SayPedless': '0x005073C0',
          'CAudioEngine::Service': '0x00507750',
          'CAudioEngine::ServiceLoadingTune': '0x005078A0',
          'CAudioEngine::SetBassEnhanceOnOff': '0x00506F90',
          'CAudioEngine::SetEffectsFaderScalingFactor': '0x00506E50',
          'CAudioEngine::SetEffectsMasterVolume': '0x00506E10',
          'CAudioEngine::SetMissionAudioPosition': '0x00507300',
          'CAudioEngine::SetMusicFaderScalingFactor': '0x00506E40',
          'CAudioEngine::SetMusicMasterVolume': '0x00506DE0',
          'CAudioEngine::SetNonStreamFaderScalingFactor': '0x00506E60',
          'CAudioEngine::SetRadioAutoRetuneOnOff': '0x00506F80',
          'CAudioEngine::SetRadioBassSetting': '0x00506FA0',
          'CAudioEngine::SetStreamFaderScalingFactor': '0x00506E70',
          'CAudioEngine::Shutdown': '0x00507CB0',
          'CAudioEngine::StartLoadingTune': '0x00507410',
          'CAudioEngine::StartRadio-': '0x00507DF0',
          'CAudioEngine::StartRadio-1': '0x00507DC0',
          'CAudioEngine::StopAmbienceTrack': '0x00507220',
          'CAudioEngine::StopBeatTrack': '0x005071A0',
          'CAudioEngine::StopCutsceneTrack': '0x00507080',
          'CAudioEngine::StopRadio': '0x00506F70'},
 'allow_bank_change': [],
 'allow_src_change': [],
 'bad': {},
 'bank': 'AudioEngineFinal4',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the '
           'hooks on), 17280 isolated CPU pairs for the 82 functions of CAudioEngine, ALL pairs equal, pair counts equal to the '
           'bank table, passed=true globally, natural "complete" event with state_restored and layout_ok, saves unchanged, '
           'configs restored. Each case compares the return value, the trace of doubled collaborators with raw arguments, the '
           'hash of the fake world and of the saved globals, plus per-function fields; the x87 control word is 24 bits; foreign '
           'threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == '
           'current working tree at registration (class sources match the probe build manifest). Historical evidence approved '
           'with explicit limits; no current dependency accreditation. Bank limits (AudioEngineFinal4): fake CAudioEngine in own '
           'memory (0x1FD8 bytes) and a fake world (opaque entities, finite vectors); every collaborator (CAEAudioHardware, '
           'Frontend/Script/Collision/GlobalWeapon/Door/Pedless/PedSpeech/PoliceScanner/Weather entities, '
           'radio/cutscene/ambience managers, CAESoundManager, FindPlayerPed and, for Initialise, loading screen, CFileMgr and '
           'static initialisers) replaced by traced EntryPatch doubles without credit; real globals saved and restored; masked: '
           'embedded-object vtables, tAmbienceSettings padding; out of fixture: mission events above 0xFFFF, radio station ids '
           'outside int8. See the bank README for the full list of masked fields and out-of-fixture domains.',
 'error': None,
 'expect': {'CAudioEngine::AttachMissionAudioToObject': 128,
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
 'extra_unregistered': [],
 'new': ['CAudioEngine::AttachMissionAudioToObject',
         'CAudioEngine::AttachMissionAudioToPed',
         'CAudioEngine::AttachMissionAudioToPhysical',
         'CAudioEngine::ClearMissionAudio',
         'CAudioEngine::Constructor',
         'CAudioEngine::Destructor',
         'CAudioEngine::DisableEffectsLoading',
         'CAudioEngine::DisplayRadioStationName',
         'CAudioEngine::DoesAmbienceTrackOverrideRadio',
         'CAudioEngine::EnableEffectsLoading',
         'CAudioEngine::GetBeatInfo',
         'CAudioEngine::GetBeatTrackStatus',
         'CAudioEngine::GetCurrentRadioStationID',
         'CAudioEngine::GetCutsceneTrackStatus',
         'CAudioEngine::GetMissionAudioEvent',
         'CAudioEngine::GetMissionAudioLoadingStatus',
         'CAudioEngine::GetMissionAudioPosition',
         'CAudioEngine::GetRadioStationListenTimes',
         'CAudioEngine::GetRadioStationName',
         'CAudioEngine::GetRadioStationNameKey',
         'CAudioEngine::HasRadioRetuneJustStarted',
         'CAudioEngine::Initialise',
         'CAudioEngine::InitialisePostLoading',
         'CAudioEngine::InitialiseRadioStationID',
         'CAudioEngine::IsAmbienceRadioActive',
         'CAudioEngine::IsAmbienceTrackActive',
         'CAudioEngine::IsBeatInfoPresent',
         'CAudioEngine::IsCutsceneTrackActive',
         'CAudioEngine::IsLoadingTuneActive',
         'CAudioEngine::IsMissionAudioSampleFinished',
         'CAudioEngine::IsRadioOn',
         'CAudioEngine::IsRadioRetuneInProgress',
         'CAudioEngine::IsVehicleRadioActive',
         'CAudioEngine::PauseAllSounds',
         'CAudioEngine::PauseBeatTrack',
         'CAudioEngine::PlayLoadedMissionAudio',
         'CAudioEngine::PlayPreloadedBeatTrack',
         'CAudioEngine::PlayPreloadedCutsceneTrack',
         'CAudioEngine::PreloadBeatTrack',
         'CAudioEngine::PreloadCutsceneTrack',
         'CAudioEngine::PreloadMissionAudio',
         'CAudioEngine::ReportBulletHit',
         'CAudioEngine::ReportCollision',
         'CAudioEngine::ReportDoorMovement',
         'CAudioEngine::ReportFrontendAudioEvent',
         'CAudioEngine::ReportGlassCollisionEvent',
         'CAudioEngine::ReportMissionAudioEvent-obj',
         'CAudioEngine::ReportMissionAudioEvent-ped',
         'CAudioEngine::ReportMissionAudioEvent-phs',
         'CAudioEngine::ReportMissionAudioEvent-vec',
         'CAudioEngine::ReportMissionAudioEvent-veh',
         'CAudioEngine::ReportObjectDestruction',
         'CAudioEngine::ReportWaterSplash-phs',
         'CAudioEngine::ReportWaterSplash-vec',
         'CAudioEngine::ReportWeaponEvent',
         'CAudioEngine::Reset',
         'CAudioEngine::ResetSoundEffects',
         'CAudioEngine::ResetStatistics',
         'CAudioEngine::Restart',
         'CAudioEngine::ResumeAllSounds',
         'CAudioEngine::RetuneRadio',
         'CAudioEngine::SayPedless',
         'CAudioEngine::Service',
         'CAudioEngine::ServiceLoadingTune',
         'CAudioEngine::SetBassEnhanceOnOff',
         'CAudioEngine::SetEffectsFaderScalingFactor',
         'CAudioEngine::SetEffectsMasterVolume',
         'CAudioEngine::SetMissionAudioPosition',
         'CAudioEngine::SetMusicFaderScalingFactor',
         'CAudioEngine::SetMusicMasterVolume',
         'CAudioEngine::SetNonStreamFaderScalingFactor',
         'CAudioEngine::SetRadioAutoRetuneOnOff',
         'CAudioEngine::SetRadioBassSetting',
         'CAudioEngine::SetStreamFaderScalingFactor',
         'CAudioEngine::Shutdown',
         'CAudioEngine::StartLoadingTune',
         'CAudioEngine::StartRadio-',
         'CAudioEngine::StartRadio-1',
         'CAudioEngine::StopAmbienceTrack',
         'CAudioEngine::StopBeatTrack',
         'CAudioEngine::StopCutsceneTrack',
         'CAudioEngine::StopRadio'],
 'notes': 'AudioEngineFinal4 registers the 82 functions of CAudioEngine (17280 pairs, 0 differences). Limits are declared per '
          'function; see scope and the bank README. No pending functions remain in CAudioEngine.',
 'passed': True,
 'pending_after': [],
 'probe_prefix': 'fa90b4ae',
 'provenance': 'Exact compiled AudioEngineFinal4 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the '
               'probe build manifest and equal to the working tree at registration; full 17280-row log reaudited offline in '
               'streaming and preserved gzipped with the original hash.',
 'readme_section': '## Aprobación (10/10/2026)\n'
                   '\n'
                   'Corrida `20261010-125652-130519`: passed=true, 17280 pares, 82 funciones registradas con 0 fallos, evento '
                   'complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con '
                   '`register_audioenginefinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco '
                   'CAudioEngine queda sin pendientes. Límites declarados en el alcance de cada función y en las secciones '
                   'anteriores de este README.',
 'registry_before': 7893,
 'rejected': [],
 'run_id': '20261010-125652-130519',
 'source_changes': {'source/game_sa/Audio/AudioEngine.cpp': 'Constructor reconstruido en el sitio, Initialise, Service y '
                                                            'comentarios de dirección (SetEffectsFaderScalingFactor 0x506E50, '
                                                            'IsLoadingTuneActive 0x506D90); ver git diff y el README del banco.',
                    'source/game_sa/Audio/AudioEngine.h': 'Constructor() pasa a AudioEngine.cpp.',
                    'source/game_sa/Audio/Entities/AEAudioEntity.cpp': 'CAEAudioEntity::StaticInitialise: con OpenFile nulo '
                                                                       'vuelve sin CloseFile (Initialise 0x5B9D59..0x5B9DB0).'},
 'specific': {'CAudioEngine::AttachMissionAudioToObject': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::AttachMissionAudioToPed': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::AttachMissionAudioToPhysical': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::ClearMissionAudio': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::Constructor': 'Corpus 64; fake world and traced doubles (no credit). source corrected (0x507670): '
                                           'built in place with the EXE writes instead of the C++ constructors. EXE constructor '
                                           'run once with 0xCD and 0x00 fill to deduce written bytes; vtables compared as '
                                           'written/not written; candidate may write nothing else but zeros. ',
              'CAudioEngine::Destructor': 'Corpus 64; fake world and traced doubles (no credit). starting state is the one left '
                                          'by the EXE constructor; with live references the EXE would call '
                                          'CEntity::CleanUpOldReference on the real engine (out of fixture). ',
              'CAudioEngine::DisableEffectsLoading': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::DisplayRadioStationName': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::DoesAmbienceTrackOverrideRadio': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::EnableEffectsLoading': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetBeatInfo': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetBeatTrackStatus': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetCurrentRadioStationID': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetCutsceneTrackStatus': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetMissionAudioEvent': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetMissionAudioLoadingStatus': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetMissionAudioPosition': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetRadioStationListenTimes': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetRadioStationName': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::GetRadioStationNameKey': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::HasRadioRetuneJustStarted': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::Initialise': 'Corpus 1024; fake world and traced doubles (no credit). source corrected (0x5B9C60): '
                                          'StaticInitialise order after hardware/managers, no WaterCannon static init, null '
                                          'OpenFile returns without CloseFile. inlined EXE parts (channel allocation, cutscene '
                                          'and ambience managers, EVENTVOL.DAT read with simulated OpenFile/Read/CloseFile) '
                                          'compared against the DLL calls to the manager initialisers; tAmbienceSettings padding '
                                          '(0x8AC185..87) zeroed before hashing; the 45401-byte buffer is freed and 0xBD00F8 '
                                          'restored. ',
              'CAudioEngine::InitialisePostLoading': 'Corpus 512; fake world and traced doubles (no credit). '
                                                     'CAEGlobalWeaponAudioEntity (+CPhysical) really constructed; only the bytes '
                                                     'the EXE constructor writes are compared (+0x10, +0x7C..7E, +0x84..9F, '
                                                     '+0xA4), non-null vtable and m_Physical; the internal CPhysical is not '
                                                     'compared; Service traced. ',
              'CAudioEngine::InitialiseRadioStationID': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsAmbienceRadioActive': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsAmbienceTrackActive': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsBeatInfoPresent': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsCutsceneTrackActive': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsLoadingTuneActive': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsMissionAudioSampleFinished': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsRadioOn': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsRadioRetuneInProgress': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::IsVehicleRadioActive': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::PauseAllSounds': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::PauseBeatTrack': 'Corpus 64; fake world and traced doubles (no credit). arguments only 0/1. ',
              'CAudioEngine::PlayLoadedMissionAudio': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::PlayPreloadedBeatTrack': 'Corpus 128; fake world and traced doubles (no credit). arguments only '
                                                      '0/1. ',
              'CAudioEngine::PlayPreloadedCutsceneTrack': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::PreloadBeatTrack': 'Corpus 512; fake world and traced doubles (no credit). ',
              'CAudioEngine::PreloadCutsceneTrack': 'Corpus 512; fake world and traced doubles (no credit). ',
              'CAudioEngine::PreloadMissionAudio': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportBulletHit': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportCollision': 'Corpus 512; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportDoorMovement': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportFrontendAudioEvent': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportGlassCollisionEvent': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportMissionAudioEvent-obj': 'Corpus 128; fake world and traced doubles (no credit). mission '
                                                           'events above 0xFFFF out of fixture (DLL truncates to uint16). ',
              'CAudioEngine::ReportMissionAudioEvent-ped': 'Corpus 128; fake world and traced doubles (no credit). mission '
                                                           'events above 0xFFFF out of fixture (DLL truncates to uint16). ',
              'CAudioEngine::ReportMissionAudioEvent-phs': 'Corpus 256; fake world and traced doubles (no credit). mission '
                                                           'events above 0xFFFF out of fixture (DLL truncates to uint16). ',
              'CAudioEngine::ReportMissionAudioEvent-vec': 'Corpus 128; fake world and traced doubles (no credit). mission '
                                                           'events above 0xFFFF out of fixture (DLL truncates to uint16). ',
              'CAudioEngine::ReportMissionAudioEvent-veh': 'Corpus 128; fake world and traced doubles (no credit). mission '
                                                           'events above 0xFFFF out of fixture (DLL truncates to uint16). ',
              'CAudioEngine::ReportObjectDestruction': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportWaterSplash-phs': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportWaterSplash-vec': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::ReportWeaponEvent': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::Reset': 'Corpus 1024; fake world and traced doubles (no credit). ',
              'CAudioEngine::ResetSoundEffects': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::ResetStatistics': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::Restart': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::ResumeAllSounds': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::RetuneRadio': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::SayPedless': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::Service': 'Corpus 1024; fake world and traced doubles (no credit). source corrected '
                                       '(0x50779A..0x5077A6): GetChannelPlayTimes(channel,nullptr) call restored. negative '
                                       'm_nBackgroundAudioChannel out of fixture (DLL indexes a std::array in '
                                       'GetChannelPlayTimes). ',
              'CAudioEngine::ServiceLoadingTune': 'Corpus 1024; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetBassEnhanceOnOff': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetEffectsFaderScalingFactor': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetEffectsMasterVolume': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetMissionAudioPosition': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetMusicFaderScalingFactor': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetMusicMasterVolume': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetNonStreamFaderScalingFactor': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetRadioAutoRetuneOnOff': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetRadioBassSetting': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CAudioEngine::SetStreamFaderScalingFactor': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::Shutdown': 'Corpus 512; fake world and traced doubles (no credit). delete/delete[] traced without '
                                        'freeing; the fake CAEGlobalWeaponAudioEntity is really destroyed on both sides (vtable '
                                        'masked); null 0xBD00F8 out of fixture (the EXE calls delete[] unconditionally). ',
              'CAudioEngine::StartLoadingTune': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CAudioEngine::StartRadio-': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::StartRadio-1': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::StopAmbienceTrack': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::StopBeatTrack': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CAudioEngine::StopCutsceneTrack': 'Corpus 512; fake world and traced doubles (no credit). ',
              'CAudioEngine::StopRadio': 'Corpus 128; fake world and traced doubles (no credit). '},
 'tag': 'audioenginefinal4',
 'total': 17280,
 'types': {'CAudioEngine::AttachMissionAudioToObject': 'S',
           'CAudioEngine::AttachMissionAudioToPed': 'S',
           'CAudioEngine::AttachMissionAudioToPhysical': 'S',
           'CAudioEngine::ClearMissionAudio': 'S',
           'CAudioEngine::Constructor': 'S',
           'CAudioEngine::Destructor': 'S',
           'CAudioEngine::DisableEffectsLoading': 'S',
           'CAudioEngine::DisplayRadioStationName': 'S',
           'CAudioEngine::DoesAmbienceTrackOverrideRadio': 'S',
           'CAudioEngine::EnableEffectsLoading': 'S',
           'CAudioEngine::GetBeatInfo': 'S',
           'CAudioEngine::GetBeatTrackStatus': 'S',
           'CAudioEngine::GetCurrentRadioStationID': 'S',
           'CAudioEngine::GetCutsceneTrackStatus': 'S',
           'CAudioEngine::GetMissionAudioEvent': 'S',
           'CAudioEngine::GetMissionAudioLoadingStatus': 'S',
           'CAudioEngine::GetMissionAudioPosition': 'S',
           'CAudioEngine::GetRadioStationListenTimes': 'S',
           'CAudioEngine::GetRadioStationName': 'S',
           'CAudioEngine::GetRadioStationNameKey': 'S',
           'CAudioEngine::HasRadioRetuneJustStarted': 'S',
           'CAudioEngine::Initialise': 'S',
           'CAudioEngine::InitialisePostLoading': 'S',
           'CAudioEngine::InitialiseRadioStationID': 'S',
           'CAudioEngine::IsAmbienceRadioActive': 'S',
           'CAudioEngine::IsAmbienceTrackActive': 'S',
           'CAudioEngine::IsBeatInfoPresent': 'S',
           'CAudioEngine::IsCutsceneTrackActive': 'S',
           'CAudioEngine::IsLoadingTuneActive': 'S',
           'CAudioEngine::IsMissionAudioSampleFinished': 'S',
           'CAudioEngine::IsRadioOn': 'S',
           'CAudioEngine::IsRadioRetuneInProgress': 'S',
           'CAudioEngine::IsVehicleRadioActive': 'S',
           'CAudioEngine::PauseAllSounds': 'S',
           'CAudioEngine::PauseBeatTrack': 'S',
           'CAudioEngine::PlayLoadedMissionAudio': 'S',
           'CAudioEngine::PlayPreloadedBeatTrack': 'S',
           'CAudioEngine::PlayPreloadedCutsceneTrack': 'S',
           'CAudioEngine::PreloadBeatTrack': 'S',
           'CAudioEngine::PreloadCutsceneTrack': 'S',
           'CAudioEngine::PreloadMissionAudio': 'S',
           'CAudioEngine::ReportBulletHit': 'S',
           'CAudioEngine::ReportCollision': 'S',
           'CAudioEngine::ReportDoorMovement': 'S',
           'CAudioEngine::ReportFrontendAudioEvent': 'S',
           'CAudioEngine::ReportGlassCollisionEvent': 'S',
           'CAudioEngine::ReportMissionAudioEvent-obj': 'S',
           'CAudioEngine::ReportMissionAudioEvent-ped': 'S',
           'CAudioEngine::ReportMissionAudioEvent-phs': 'S',
           'CAudioEngine::ReportMissionAudioEvent-vec': 'S',
           'CAudioEngine::ReportMissionAudioEvent-veh': 'S',
           'CAudioEngine::ReportObjectDestruction': 'S',
           'CAudioEngine::ReportWaterSplash-phs': 'S',
           'CAudioEngine::ReportWaterSplash-vec': 'S',
           'CAudioEngine::ReportWeaponEvent': 'S',
           'CAudioEngine::Reset': 'S',
           'CAudioEngine::ResetSoundEffects': 'S',
           'CAudioEngine::ResetStatistics': 'S',
           'CAudioEngine::Restart': 'S',
           'CAudioEngine::ResumeAllSounds': 'S',
           'CAudioEngine::RetuneRadio': 'S',
           'CAudioEngine::SayPedless': 'S',
           'CAudioEngine::Service': 'S',
           'CAudioEngine::ServiceLoadingTune': 'S',
           'CAudioEngine::SetBassEnhanceOnOff': 'S',
           'CAudioEngine::SetEffectsFaderScalingFactor': 'S',
           'CAudioEngine::SetEffectsMasterVolume': 'S',
           'CAudioEngine::SetMissionAudioPosition': 'S',
           'CAudioEngine::SetMusicFaderScalingFactor': 'S',
           'CAudioEngine::SetMusicMasterVolume': 'S',
           'CAudioEngine::SetNonStreamFaderScalingFactor': 'S',
           'CAudioEngine::SetRadioAutoRetuneOnOff': 'S',
           'CAudioEngine::SetRadioBassSetting': 'S',
           'CAudioEngine::SetStreamFaderScalingFactor': 'S',
           'CAudioEngine::Shutdown': 'S',
           'CAudioEngine::StartLoadingTune': 'S',
           'CAudioEngine::StartRadio-': 'S',
           'CAudioEngine::StartRadio-1': 'S',
           'CAudioEngine::StopAmbienceTrack': 'S',
           'CAudioEngine::StopBeatTrack': 'S',
           'CAudioEngine::StopCutsceneTrack': 'S',
           'CAudioEngine::StopRadio': 'S'}}
BANK=CFG['bank'];RUN_ID=CFG['run_id']
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')/('resultados-'+BANK.lower())/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT=CFG['expect'];TOTAL=sum(EXPECT.values());NEW=CFG['new'];REJECTED=CFG['rejected'];PASSED=CFG['passed']
EXTRA=CFG.get('extra_unregistered',[])
assert TOTAL==CFG['total'] and set(NEW)|set(REJECTED)|set(EXTRA)==set(EXPECT) and not set(NEW)&set(REJECTED) and not set(EXTRA)&set(NEW)
assert summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith(CFG['probe_prefix']) and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl')
if PASSED:
    assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True and summary['pairs']==TOTAL and summary['pairs_per_function']==EXPECT and log_sha==summary['log_sha256']
else:
    assert summary['passed'] is False and summary['error']==CFG['error'] and 'pairs' not in summary
events={};per={};bad={};fsum={};cases=set();complete=None;layout=None;err=None;begin=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='error':err=row
        elif ev=='layout':layout=row
        elif ev=='begin':begin=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
term=complete if PASSED else err
if PASSED:
    assert err is None and complete is not None and complete['pairs']==TOTAL and complete['state_restored'] is True and complete.get('layout_ok',True) is True
else:
    assert complete is None and err is not None and set(err)<={'event','layout_ok','pairs','state_restored'} and err['pairs']==TOTAL and err['state_restored'] is True and err.get('layout_ok') is True
assert layout is not None and layout.get('ok') is True and events.get('layout')==1 and begin is None
if begin is not None and isinstance(begin.get('sites'),list):assert all(x.get('ok') is True for x in begin['sites'])
assert events.get('comparison')==TOTAL and events.get('function_summary')==len(EXPECT) and len(cases)==TOTAL
assert events.get('case_start',TOTAL)==TOTAL and events.get('complete',0)+events.get('error',0)==1
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad==CFG['bad'],bad
for f in NEW:assert f not in bad
PAIRS={f:per[f] for f in NEW}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW+REJECTED:
    cls,fn=name.split('::',1);m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR==CFG['addr'],ADDR
for name,t in CFG['types'].items():assert TYPE[name]==t,(name,TYPE[name])
CLS=sorted({n.split('::',1)[0] for n in NEW})
pend_after=[r['class']+'::'+r['fn_name'] for r in rows if r['class'] in CLS and r['validated']=='0' and (r['class']+'::'+r['fn_name']) not in NEW]
assert sorted(pend_after)==sorted(CFG['pending_after']),pend_after
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
CHANGED_SRC={};CHANGED_BANK={}
for p,h in manifest['sources'].items():
    if sha(ROOT/p)!=h:CHANGED_SRC[p]=dict(tested_sha256=h,current_sha256=sha(ROOT/p))
for n,h in manifest['instrumentation'].items():
    if sha(HERE/n)!=h:CHANGED_BANK[n]=dict(tested_sha256=h,current_sha256=sha(HERE/n))
assert set(CHANGED_SRC)<=set(CFG['allow_src_change']),CHANGED_SRC
assert set(CHANGED_BANK)<=set(CFG['allow_bank_change']),CHANGED_BANK
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
assert sha(HERE/('artifacts/gta_reversed-%s-probe.asi'%BANK.lower()))==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/('gta_reversed-%s-probe.pdb'%BANK.lower());assert sha(pdb)==manifest['binaries']['pdb']
assert not RUN.exists();RUN.mkdir(parents=True)
shutil.copy2(SRC/'summary.json',RUN/'summary.json');shutil.copy2(SRC/'RESULTADO.txt',RUN/'RESULTADO.txt')
raw=(SRC/'comparisons.jsonl').read_bytes();assert hashlib.sha256(raw).hexdigest()==log_sha
with open(RUN/'comparisons.jsonl.gz','wb') as out:
    with gzip.GzipFile(filename='',mode='wb',fileobj=out,compresslevel=9,mtime=0) as z:z.write(raw)
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(1<<20),b''):digest.update(block)
assert digest.hexdigest()==log_sha
assert sha(RUN/'summary.json')==sha(SRC/'summary.json') and sha(RUN/'RESULTADO.txt')==sha(SRC/'RESULTADO.txt')
storage=dict(uncompressed_sha256=log_sha,uncompressed_bytes=len(raw),compressed_path=rel(RUN/'comparisons.jsonl.gz'),compressed_sha256=sha(RUN/'comparisons.jsonl.gz'),compression='gzip',gzip_mtime=0,lossless_roundtrip_sha256=digest.hexdigest(),original_location=str(SRC/'comparisons.jsonl'))
COMMON=CFG['common'];SPECIFIC=CFG['specific'];SOURCE_CHANGES=CFG['source_changes']
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the run; not registered, no credit (another agent is diagnosing it)') for f in REJECTED}
reaudit=dict(comparison_events=TOTAL,complete_events=events.get('complete',0),terminal_error_events=events.get('error',0),terminal_event=term,layout_events=events.get('layout',0),begin_events=events.get('begin',0),begin_checked=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=len(EXPECT),functions_with_zero_failures=len(NEW),unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per)
out=dict(passed=PASSED,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=len(NEW),not_registered_extra=EXTRA,functions=PAIRS,covered_functions={k:True for k in NEW},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=PASSED,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,reaudit=reaudit,source_corrections=SOURCE_CHANGES,notes=CFG['notes'])
if not PASSED:
    out.update(partial_per_function_approval=True,rejected_functions=rejected)
if CHANGED_SRC or CHANGED_BANK:
    out.update(source_changed_after_run=CHANGED_SRC,bank_changed_after_run=CHANGED_BANK)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/('reversing/validation-registry.backup-before-%s.json'%CFG['tag'])
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
assert n==CFG['registry_before'],n
for name in NEW:
    cls,fn=name.split('::',1);hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources=dict(manifest['sources']),
     source_provenance=CFG['provenance'],
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/%s/README.md'%BANK))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.%s.tmp'%CFG['tag']);temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
# README: only append the approval section at the end (append mode, bank file otherwise untouched)
readme=HERE/'README.md';raw_r=readme.read_bytes();nl='\r\n' if b'\r\n' in raw_r else '\n'
section=CFG['readme_section'].replace('\n',nl)
assert '## Aprobación' not in raw_r.decode('utf-8')
with open(readme,'ab') as fh:fh.write((nl+section+nl).encode('utf-8'))
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp),changed_src=list(CHANGED_SRC),changed_bank=list(CHANGED_BANK)),indent=2))
