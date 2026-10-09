"""Registro historico de CAERadioTrackManager y CFont (61 funciones) del banco simple RadioFont1 (corrida 20261009-161801-628784). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'RadioFont1',
 'run_id': '20261009-161801-628784',
 'probe_prefix': 'b426cd0a',
 'tag': 'radiofont1',
 'registry_before': 7295,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CAERadioTrackManager::AddAdvertIndexToHistory': 512,
            'CAERadioTrackManager::AddDJBanterIndexToHistory': 512,
            'CAERadioTrackManager::AddIdentIndexToHistory': 512,
            'CAERadioTrackManager::AddMusicTrackIndexToHistory': 512,
            'CAERadioTrackManager::CheckForMissionStatsChanges': 1024,
            'CAERadioTrackManager::CheckForPause': 512,
            'CAERadioTrackManager::CheckForStationRetuneDuringPause': 512,
            'CAERadioTrackManager::ChooseTracksForStation': 512,
            'CAERadioTrackManager::DisplayRadioStationName': 512,
            'CAERadioTrackManager::GetCurrentRadioStationID': 128,
            'CAERadioTrackManager::GetRadioStationListenTimes': 64,
            'CAERadioTrackManager::GetRadioStationName': 128,
            'CAERadioTrackManager::GetRadioStationNameKey': 128,
            'CAERadioTrackManager::HasRadioRetuneJustStarted': 64,
            'CAERadioTrackManager::Initialise': 256,
            'CAERadioTrackManager::InitialiseRadioStationID': 128,
            'CAERadioTrackManager::IsRadioOn': 256,
            'CAERadioTrackManager::Load': 128,
            'CAERadioTrackManager::PlayRadioAnnouncement': 64,
            'CAERadioTrackManager::Reset': 256,
            'CAERadioTrackManager::ResetStatistics': 128,
            'CAERadioTrackManager::RetuneRadio': 256,
            'CAERadioTrackManager::Save': 128,
            'CAERadioTrackManager::SetBassEnhanceOnOff': 256,
            'CAERadioTrackManager::SetBassSetting': 256,
            'CAERadioTrackManager::SetRadioAutoRetuneOnOff': 64,
            'CAERadioTrackManager::StartRadio-manual': 1024,
            'CAERadioTrackManager::StartRadio-with-settings': 512,
            'CAERadioTrackManager::StartTrackPlayback': 128,
            'CFont::DrawFonts': 64,
            'CFont::GetCharacterSize': 512,
            'CFont::GetLetterIdPropValue': 512,
            'CFont::GetNumberLines': 256,
            'CFont::GetStringWidth': 512,
            'CFont::GetTextRect': 256,
            'CFont::InitPerFrame': 64,
            'CFont::Initialise': 64,
            'CFont::LoadFontValues': 128,
            'CFont::ParseToken': 512,
            'CFont::PrintString': 256,
            'CFont::PrintStringFromBottom': 256,
            'CFont::ProcessStringToDisplay': 256,
            'CFont::SetAlphaFade': 64,
            'CFont::SetBackground': 64,
            'CFont::SetBackgroundColor': 64,
            'CFont::SetCentreSize': 64,
            'CFont::SetColor': 256,
            'CFont::SetDropColor': 256,
            'CFont::SetDropShadowPosition': 64,
            'CFont::SetEdge': 64,
            'CFont::SetFontStyle': 128,
            'CFont::SetJustify': 64,
            'CFont::SetOrientation': 128,
            'CFont::SetProportional': 64,
            'CFont::SetRightJustifyWrap': 64,
            'CFont::SetScale': 128,
            'CFont::SetScaleForCurrentLanguage': 128,
            'CFont::SetSlant': 64,
            'CFont::SetSlantRefPoint': 64,
            'CFont::SetWrapx': 64,
            'CFont::Shutdown': 64},
 'total': 14976,
 'new': ['CAERadioTrackManager::AddAdvertIndexToHistory',
         'CAERadioTrackManager::AddDJBanterIndexToHistory',
         'CAERadioTrackManager::AddIdentIndexToHistory',
         'CAERadioTrackManager::AddMusicTrackIndexToHistory',
         'CAERadioTrackManager::CheckForMissionStatsChanges',
         'CAERadioTrackManager::CheckForPause',
         'CAERadioTrackManager::CheckForStationRetuneDuringPause',
         'CAERadioTrackManager::ChooseTracksForStation',
         'CAERadioTrackManager::DisplayRadioStationName',
         'CAERadioTrackManager::GetCurrentRadioStationID',
         'CAERadioTrackManager::GetRadioStationListenTimes',
         'CAERadioTrackManager::GetRadioStationName',
         'CAERadioTrackManager::GetRadioStationNameKey',
         'CAERadioTrackManager::HasRadioRetuneJustStarted',
         'CAERadioTrackManager::Initialise',
         'CAERadioTrackManager::InitialiseRadioStationID',
         'CAERadioTrackManager::IsRadioOn',
         'CAERadioTrackManager::Load',
         'CAERadioTrackManager::PlayRadioAnnouncement',
         'CAERadioTrackManager::Reset',
         'CAERadioTrackManager::ResetStatistics',
         'CAERadioTrackManager::RetuneRadio',
         'CAERadioTrackManager::Save',
         'CAERadioTrackManager::SetBassEnhanceOnOff',
         'CAERadioTrackManager::SetBassSetting',
         'CAERadioTrackManager::SetRadioAutoRetuneOnOff',
         'CAERadioTrackManager::StartRadio-manual',
         'CAERadioTrackManager::StartRadio-with-settings',
         'CAERadioTrackManager::StartTrackPlayback',
         'CFont::DrawFonts',
         'CFont::GetCharacterSize',
         'CFont::GetLetterIdPropValue',
         'CFont::GetNumberLines',
         'CFont::GetStringWidth',
         'CFont::GetTextRect',
         'CFont::InitPerFrame',
         'CFont::Initialise',
         'CFont::LoadFontValues',
         'CFont::ParseToken',
         'CFont::PrintString',
         'CFont::PrintStringFromBottom',
         'CFont::ProcessStringToDisplay',
         'CFont::SetAlphaFade',
         'CFont::SetBackground',
         'CFont::SetBackgroundColor',
         'CFont::SetCentreSize',
         'CFont::SetColor',
         'CFont::SetDropColor',
         'CFont::SetDropShadowPosition',
         'CFont::SetEdge',
         'CFont::SetFontStyle',
         'CFont::SetJustify',
         'CFont::SetOrientation',
         'CFont::SetProportional',
         'CFont::SetRightJustifyWrap',
         'CFont::SetScale',
         'CFont::SetScaleForCurrentLanguage',
         'CFont::SetSlant',
         'CFont::SetSlantRefPoint',
         'CFont::SetWrapx',
         'CFont::Shutdown'],
 'rejected': [],
 'addr': {'CAERadioTrackManager::AddAdvertIndexToHistory': '0x004E9760',
          'CAERadioTrackManager::AddDJBanterIndexToHistory': '0x004E97B0',
          'CAERadioTrackManager::AddIdentIndexToHistory': '0x004E9720',
          'CAERadioTrackManager::AddMusicTrackIndexToHistory': '0x004E96C0',
          'CAERadioTrackManager::CheckForMissionStatsChanges': '0x004E8410',
          'CAERadioTrackManager::CheckForPause': '0x004EA590',
          'CAERadioTrackManager::CheckForStationRetuneDuringPause': '0x004EB890',
          'CAERadioTrackManager::ChooseTracksForStation': '0x004EB180',
          'CAERadioTrackManager::DisplayRadioStationName': '0x004E9E50',
          'CAERadioTrackManager::GetCurrentRadioStationID': '0x004E83F0',
          'CAERadioTrackManager::GetRadioStationListenTimes': '0x004E83E0',
          'CAERadioTrackManager::GetRadioStationName': '0x004E9E10',
          'CAERadioTrackManager::GetRadioStationNameKey': '0x004E8380',
          'CAERadioTrackManager::HasRadioRetuneJustStarted': '0x004E8370',
          'CAERadioTrackManager::Initialise': '0x005B9390',
          'CAERadioTrackManager::InitialiseRadioStationID': '0x004E8330',
          'CAERadioTrackManager::IsRadioOn': '0x004E8350',
          'CAERadioTrackManager::Load': '0x005D40E0',
          'CAERadioTrackManager::PlayRadioAnnouncement': '0x004E8400',
          'CAERadioTrackManager::Reset': '0x004E7F80',
          'CAERadioTrackManager::ResetStatistics': '0x004E8200',
          'CAERadioTrackManager::RetuneRadio': '0x004E8290',
          'CAERadioTrackManager::Save': '0x005D3EE0',
          'CAERadioTrackManager::SetBassEnhanceOnOff': '0x004E9DB0',
          'CAERadioTrackManager::SetBassSetting': '0x004E82F0',
          'CAERadioTrackManager::SetRadioAutoRetuneOnOff': '0x004E82E0',
          'CAERadioTrackManager::StartRadio-manual': '0x004EB3C0',
          'CAERadioTrackManager::StartRadio-with-settings': '0x004EB550',
          'CAERadioTrackManager::StartTrackPlayback': '0x004EA640',
          'CFont::DrawFonts': '0x0071A210',
          'CFont::GetCharacterSize': '0x00719750',
          'CFont::GetLetterIdPropValue': '0x00718770',
          'CFont::GetNumberLines': '0x0071A5E0',
          'CFont::GetStringWidth': '0x0071A0E0',
          'CFont::GetTextRect': '0x0071A620',
          'CFont::InitPerFrame': '0x00719800',
          'CFont::Initialise': '0x005BA690',
          'CFont::LoadFontValues': '0x007187C0',
          'CFont::ParseToken': '0x00718F00',
          'CFont::PrintString': '0x0071A700',
          'CFont::PrintStringFromBottom': '0x0071A820',
          'CFont::ProcessStringToDisplay': '0x0071A600',
          'CFont::SetAlphaFade': '0x00719500',
          'CFont::SetBackground': '0x007195C0',
          'CFont::SetBackgroundColor': '0x007195E0',
          'CFont::SetCentreSize': '0x007194E0',
          'CFont::SetColor': '0x00719430',
          'CFont::SetDropColor': '0x00719510',
          'CFont::SetDropShadowPosition': '0x00719570',
          'CFont::SetEdge': '0x00719590',
          'CFont::SetFontStyle': '0x00719490',
          'CFont::SetJustify': '0x00719600',
          'CFont::SetOrientation': '0x00719610',
          'CFont::SetProportional': '0x007195B0',
          'CFont::SetRightJustifyWrap': '0x007194F0',
          'CFont::SetScale': '0x00719380',
          'CFont::SetScaleForCurrentLanguage': '0x007193A0',
          'CFont::SetSlant': '0x00719420',
          'CFont::SetSlantRefPoint': '0x00719400',
          'CFont::SetWrapx': '0x007194D0',
          'CFont::Shutdown': '0x007189B0'},
 'types': {'CAERadioTrackManager::AddAdvertIndexToHistory': 'S',
           'CAERadioTrackManager::AddDJBanterIndexToHistory': 'S',
           'CAERadioTrackManager::AddIdentIndexToHistory': 'S',
           'CAERadioTrackManager::AddMusicTrackIndexToHistory': 'S',
           'CAERadioTrackManager::CheckForMissionStatsChanges': 'S',
           'CAERadioTrackManager::CheckForPause': 'S',
           'CAERadioTrackManager::CheckForStationRetuneDuringPause': 'S',
           'CAERadioTrackManager::ChooseTracksForStation': 'S',
           'CAERadioTrackManager::DisplayRadioStationName': 'S',
           'CAERadioTrackManager::GetCurrentRadioStationID': 'S',
           'CAERadioTrackManager::GetRadioStationListenTimes': 'S',
           'CAERadioTrackManager::GetRadioStationName': 'S',
           'CAERadioTrackManager::GetRadioStationNameKey': 'S',
           'CAERadioTrackManager::HasRadioRetuneJustStarted': 'S',
           'CAERadioTrackManager::Initialise': 'S',
           'CAERadioTrackManager::InitialiseRadioStationID': 'S',
           'CAERadioTrackManager::IsRadioOn': 'S',
           'CAERadioTrackManager::Load': 'S',
           'CAERadioTrackManager::PlayRadioAnnouncement': 'S',
           'CAERadioTrackManager::Reset': 'S',
           'CAERadioTrackManager::ResetStatistics': 'S',
           'CAERadioTrackManager::RetuneRadio': 'S',
           'CAERadioTrackManager::Save': 'S',
           'CAERadioTrackManager::SetBassEnhanceOnOff': 'S',
           'CAERadioTrackManager::SetBassSetting': 'S',
           'CAERadioTrackManager::SetRadioAutoRetuneOnOff': 'S',
           'CAERadioTrackManager::StartRadio-manual': 'S',
           'CAERadioTrackManager::StartRadio-with-settings': 'S',
           'CAERadioTrackManager::StartTrackPlayback': 'S',
           'CFont::DrawFonts': 'S',
           'CFont::GetCharacterSize': 'S',
           'CFont::GetLetterIdPropValue': 'S',
           'CFont::GetNumberLines': 'S',
           'CFont::GetStringWidth': 'S',
           'CFont::GetTextRect': 'S',
           'CFont::InitPerFrame': 'S',
           'CFont::Initialise': 'S',
           'CFont::LoadFontValues': 'S',
           'CFont::ParseToken': 'S',
           'CFont::PrintString': 'S',
           'CFont::PrintStringFromBottom': 'S',
           'CFont::ProcessStringToDisplay': 'S',
           'CFont::SetAlphaFade': 'S',
           'CFont::SetBackground': 'S',
           'CFont::SetBackgroundColor': 'S',
           'CFont::SetCentreSize': 'S',
           'CFont::SetColor': 'S',
           'CFont::SetDropColor': 'S',
           'CFont::SetDropShadowPosition': 'S',
           'CFont::SetEdge': 'S',
           'CFont::SetFontStyle': 'S',
           'CFont::SetJustify': 'S',
           'CFont::SetOrientation': 'S',
           'CFont::SetProportional': 'S',
           'CFont::SetRightJustifyWrap': 'S',
           'CFont::SetScale': 'S',
           'CFont::SetScaleForCurrentLanguage': 'S',
           'CFont::SetSlant': 'S',
           'CFont::SetSlantRefPoint': 'S',
           'CFont::SetWrapx': 'S',
           'CFont::Shutdown': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled RadioFont1 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working '
               'tree at registration; full 14976-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 14976 isolated CPU pairs for '
           '61 pending functions of CAERadioTrackManager, CFont, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural "complete" event '
           'with state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case index) in both legs and compares the '
           'return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 bits) and the whole trace of doubled collaborators with raw '
           'arguments; foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == current working tree at '
           'registration (class sources match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Limits: '
           "GXT texts well formed (every '~' closed, the EXE does not check NUL); GetRadioStationNameKey only with ids 0..13; StartRadio only with ids >= 0; Shutdown "
           'with a FindTxdSlot double that always returns a valid slot (the SafeRemoveTxdSlot(-1) branch is out); LoadFontValues with font id 0 or 1, complete sections '
           'and no real file (DATA\\FONTS.DAT replaced by fixture lines); floats without signalling NaN; the real AERadioTrackManager is never touched, a fake '
           '0x370-byte object is used; audio, CTxdStore, CFileMgr, CSprite2d, CStats, RNG and other collaborators doubled with trace (no credit).',
 'specific': {'CAERadioTrackManager::AddAdvertIndexToHistory': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                               'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::AddDJBanterIndexToHistory': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                 'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::AddIdentIndexToHistory': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                              'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::AddMusicTrackIndexToHistory': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                   'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::CheckForMissionStatsChanges': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                   'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::CheckForPause': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README, '
                                                     'section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::CheckForStationRetuneDuringPause': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the '
                                                                        'bank README, section on source fixes); fake object with 16-byte guards plus hashed global '
                                                                        'regions.',
              'CAERadioTrackManager::ChooseTracksForStation': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                              'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::DisplayRadioStationName': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                               'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::GetCurrentRadioStationID': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::GetRadioStationListenTimes': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                  'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::GetRadioStationName': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, '
                                                           'section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::GetRadioStationNameKey': 'Corpus 128; domain: ids 0..13 only (the DLL has a NOTSA assert outside the range; the EXE overflows the '
                                                              '8-byte buffer).',
              'CAERadioTrackManager::HasRadioRetuneJustStarted': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                 'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::Initialise': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section '
                                                  'on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::InitialiseRadioStationID': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::IsRadioOn': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                                 'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::Load': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                            'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::PlayRadioAnnouncement': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, '
                                                             'section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::Reset': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                             'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::ResetStatistics': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, '
                                                       'section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::RetuneRadio': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section '
                                                   'on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::Save': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                            'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::SetBassEnhanceOnOff': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, '
                                                           'section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::SetBassSetting': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, '
                                                      'section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::SetRadioAutoRetuneOnOff': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                               'README, section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CAERadioTrackManager::StartRadio-manual': 'Corpus 1024; domain: ids >= 0 only (the EXE indexes the state array with sign, the DLL aborts in debug).',
              'CAERadioTrackManager::StartRadio-with-settings': 'Corpus 512; domain: ids >= 0 only.',
              'CAERadioTrackManager::StartTrackPlayback': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, '
                                                          'section on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::DrawFonts': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                  'fake object with 16-byte guards plus hashed global regions.',
              'CFont::GetCharacterSize': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                         'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::GetLetterIdPropValue': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                             'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::GetNumberLines': 'Corpus 256; GXT texts well formed.',
              'CFont::GetStringWidth': 'Corpus 512; GXT texts well formed; GetScriptLetterSize real only with scriptText=true.',
              'CFont::GetTextRect': 'Corpus 256; GXT texts well formed.',
              'CFont::InitPerFrame': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                     'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::Initialise': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                   'fake object with 16-byte guards plus hashed global regions.',
              'CFont::LoadFontValues': 'Corpus 128; domain: font id 0 or 1, complete sections, no whitespace-only lines; DATA\\FONTS.DAT replaced by fixture lines (no '
                                       'real file opened).',
              'CFont::ParseToken': 'Corpus 512; GXT texts well formed (every ~ closed).',
              'CFont::PrintString': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                    'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::PrintStringFromBottom': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                              'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::ProcessStringToDisplay': 'Corpus 256; GXT texts well formed.',
              'CFont::SetAlphaFade': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                     'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetBackground': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                      'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetBackgroundColor': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                           'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetCentreSize': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                      'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetColor': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                 'fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetDropColor': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                     'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetDropShadowPosition': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                              'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetEdge': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                'fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetFontStyle': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                     'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetJustify': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                   'fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetOrientation': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                       'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetProportional': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                        'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetRightJustifyWrap': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on '
                                            'source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetScale': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                 'fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetScaleForCurrentLanguage': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README, section '
                                                   'on source fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetSlant': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                 'fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetSlantRefPoint': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source '
                                         'fixes); fake object with 16-byte guards plus hashed global regions.',
              'CFont::SetWrapx': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README, section on source fixes); '
                                 'fake object with 16-byte guards plus hashed global regions.',
              'CFont::Shutdown': 'Corpus 64; FindTxdSlot double always returns a valid slot (7); the branch where CTxdStore::SafeRemoveTxdSlot (FIX_BUGS) omits '
                                 'RemoveTxdSlot(-1), which the EXE executes, is outside the bank.'},
 'source_changes': {'source/game_sa/Audio/Managers/AERadioTrackManager.cpp': 'Reset, Initialise, GetCurrentRadioStationID, CheckForMissionStatsChanges, '
                                                                             'DisplayRadioStationName, GetRadioStationName, StartRadio (manual and with settings), '
                                                                             'ChooseTracksForStation, Add{Ident,Advert,DJBanter}IndexToHistory corrected against the '
                                                                             'EXE; see git diff',
                    'source/game_sa/Audio/Managers/AERadioTrackManager.h': 'declaration adjustments (see git diff)',
                    'source/game_sa/Font.cpp': 'SetColor, SetDropColor, SetOrientation, InitPerFrame, Shutdown, GetStringWidth, ParseToken, GetTextRect, PrintString, '
                                               'GetCharacterSize, GetHeight, GetLetterIdPropValue, LoadFontValues corrected against the EXE; see git diff'},
 'notes': 'RadioFont1 registers the 61 functions with cases (29 of CAERadioTrackManager and 32 of CFont); both classes are left without pending entries.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-161801-628784`: passed=true, 14976 pares, 61 funciones con 0 fallos (29 de CAERadioTrackManager y 32 de CFont), evento complete '
                   '(state_restored), begin con layout y sitios correctos, saves y configuraciones intactos. Registrada con `register_radiofont1.py` (evidencia en '
                   '`runs/`, `validation-summary.json`). Con esto CAERadioTrackManager y CFont quedan COMPLETAS. Límites: textos GXT bien formados; '
                   'GetRadioStationNameKey solo con identificadores 0..13; StartRadio solo con identificadores >= 0; Shutdown con FindTxdSlot siempre válido (la rama de '
                   'SafeRemoveTxdSlot(-1) queda fuera); LoadFontValues con fuente 0 o 1 y sin fichero real; objeto falso (nunca el AERadioTrackManager real); '
                   'colaboradores doblados sin crédito.'}
BANK=CFG['bank'];RUN_ID=CFG['run_id']
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')/('resultados-'+BANK.lower())/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT=CFG['expect'];TOTAL=sum(EXPECT.values());NEW=CFG['new'];REJECTED=CFG['rejected'];PASSED=CFG['passed']
assert TOTAL==CFG['total'] and set(NEW)|set(REJECTED)==set(EXPECT) and not set(NEW)&set(REJECTED)
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
assert begin is not None and events.get('begin')==1 and begin.get('layout') is True and begin.get('sites_ok') is True and begin.get('bad_sites')==[] and layout is None
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
out=dict(passed=PASSED,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=len(NEW),functions=PAIRS,covered_functions={k:True for k in NEW},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=PASSED,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,reaudit=reaudit,source_corrections=SOURCE_CHANGES,notes=CFG['notes'])
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
