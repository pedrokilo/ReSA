"""Registro historico de CCam y CHud (36 funciones) del banco simple CamHud1 (corrida 20261009-155715-312758). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'CamHud1',
 'run_id': '20261009-155715-312758',
 'probe_prefix': '9b8015c8',
 'tag': 'camhud1',
 'registry_before': 7193,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CHud::HelpMessageDisplayed': 64,
            'CHud::GetYPosBasedOnHealth': 512,
            'CHud::SetMessage': 320,
            'CHud::SetBigMessage': 1024,
            'CHud::SetVehicleName': 64,
            'CHud::SetZoneName': 256,
            'CHud::ResetWastedText': 64,
            'CHud::GetRidOfAllHudMessages': 256,
            'CHud::ReInitialise': 128,
            'CHud::SetHelpMessage': 768,
            'CHud::SetHelpMessageStatUpdate': 512,
            'CHud::SetHelpMessageWithNumber': 512,
            'CHud::DrawFadeState': 1024,
            'CHud::RenderArmorBar': 512,
            'CHud::RenderBreathBar': 512,
            'CHud::RenderHealthBar': 512,
            'CHud::DrawAmmo': 1024,
            'CHud::DrawBustedWastedMessage': 768,
            'CHud::DrawAreaName': 1024,
            'CHud::DrawVehicleName': 1024,
            'CHud::DrawMissionTitle': 768,
            'CHud::DrawOddJobMessage': 768,
            'CHud::DrawScriptText': 512,
            'CHud::DrawTripSkip': 128,
            'CCam::WellBufferMe': 1024,
            'CCam::DoCamBump': 128,
            'CCam::GetVectorsReadyForRW': 128,
            'CCam::Constructor': 64,
            'CCam::Init': 64,
            'CCam::CacheLastSettingsDWCineyCam': 128,
            'CCam::Finalise_DW_CineyCams': 512,
            'CCam::GetCoreDataForDWCineyCamMode': 256,
            'CCam::Get_TwoPlayer_AimVector': 768,
            'CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown': 768,
            'CCam::Process_Editor': 1024,
            'CCam::Process_Fixed': 768},
 'total': 18688,
 'new': ['CCam::CacheLastSettingsDWCineyCam',
         'CCam::Constructor',
         'CCam::DoCamBump',
         'CCam::Finalise_DW_CineyCams',
         'CCam::GetCoreDataForDWCineyCamMode',
         'CCam::GetVectorsReadyForRW',
         'CCam::Get_TwoPlayer_AimVector',
         'CCam::Init',
         'CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown',
         'CCam::Process_Editor',
         'CCam::Process_Fixed',
         'CCam::WellBufferMe',
         'CHud::DrawAmmo',
         'CHud::DrawAreaName',
         'CHud::DrawBustedWastedMessage',
         'CHud::DrawFadeState',
         'CHud::DrawMissionTitle',
         'CHud::DrawOddJobMessage',
         'CHud::DrawScriptText',
         'CHud::DrawTripSkip',
         'CHud::DrawVehicleName',
         'CHud::GetRidOfAllHudMessages',
         'CHud::GetYPosBasedOnHealth',
         'CHud::HelpMessageDisplayed',
         'CHud::ReInitialise',
         'CHud::RenderArmorBar',
         'CHud::RenderBreathBar',
         'CHud::RenderHealthBar',
         'CHud::ResetWastedText',
         'CHud::SetBigMessage',
         'CHud::SetHelpMessage',
         'CHud::SetHelpMessageStatUpdate',
         'CHud::SetHelpMessageWithNumber',
         'CHud::SetMessage',
         'CHud::SetVehicleName',
         'CHud::SetZoneName'],
 'rejected': [],
 'addr': {'CCam::CacheLastSettingsDWCineyCam': '0x0050D7A0',
          'CCam::Constructor': '0x00517730',
          'CCam::DoCamBump': '0x0050CB30',
          'CCam::Finalise_DW_CineyCams': '0x0050DD70',
          'CCam::GetCoreDataForDWCineyCamMode': '0x00517130',
          'CCam::GetVectorsReadyForRW': '0x00509CE0',
          'CCam::Get_TwoPlayer_AimVector': '0x00513E40',
          'CCam::Init': '0x0050E490',
          'CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown': '0x00513BE0',
          'CCam::Process_Editor': '0x0050F3F0',
          'CCam::Process_Fixed': '0x0051D470',
          'CCam::WellBufferMe': '0x00509AE0',
          'CHud::DrawAmmo': '0x005893B0',
          'CHud::DrawAreaName': '0x0058AA50',
          'CHud::DrawBustedWastedMessage': '0x0058CA50',
          'CHud::DrawFadeState': '0x0058D580',
          'CHud::DrawMissionTitle': '0x0058D240',
          'CHud::DrawOddJobMessage': '0x0058CC80',
          'CHud::DrawScriptText': '0x0058C080',
          'CHud::DrawTripSkip': '0x0058A160',
          'CHud::DrawVehicleName': '0x0058AEA0',
          'CHud::GetRidOfAllHudMessages': '0x00588A50',
          'CHud::GetYPosBasedOnHealth': '0x00588B60',
          'CHud::HelpMessageDisplayed': '0x00588B50',
          'CHud::ReInitialise': '0x00588880',
          'CHud::RenderArmorBar': '0x005890A0',
          'CHud::RenderBreathBar': '0x00589190',
          'CHud::RenderHealthBar': '0x00589270',
          'CHud::ResetWastedText': '0x00589070',
          'CHud::SetBigMessage': '0x00588FC0',
          'CHud::SetHelpMessage': '0x00588BE0',
          'CHud::SetHelpMessageStatUpdate': '0x00588D40',
          'CHud::SetHelpMessageWithNumber': '0x00588E30',
          'CHud::SetMessage': '0x00588F60',
          'CHud::SetVehicleName': '0x00588F50',
          'CHud::SetZoneName': '0x00588BB0'},
 'types': {'CCam::CacheLastSettingsDWCineyCam': 'S',
           'CCam::Constructor': 'S',
           'CCam::DoCamBump': 'S',
           'CCam::Finalise_DW_CineyCams': 'S',
           'CCam::GetCoreDataForDWCineyCamMode': 'S',
           'CCam::GetVectorsReadyForRW': 'S',
           'CCam::Get_TwoPlayer_AimVector': 'S',
           'CCam::Init': 'S',
           'CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown': 'S',
           'CCam::Process_Editor': 'S',
           'CCam::Process_Fixed': 'S',
           'CCam::WellBufferMe': 'S',
           'CHud::DrawAmmo': 'S',
           'CHud::DrawAreaName': 'S',
           'CHud::DrawBustedWastedMessage': 'S',
           'CHud::DrawFadeState': 'S',
           'CHud::DrawMissionTitle': 'S',
           'CHud::DrawOddJobMessage': 'S',
           'CHud::DrawScriptText': 'S',
           'CHud::DrawTripSkip': 'S',
           'CHud::DrawVehicleName': 'S',
           'CHud::GetRidOfAllHudMessages': 'S',
           'CHud::GetYPosBasedOnHealth': 'S',
           'CHud::HelpMessageDisplayed': 'S',
           'CHud::ReInitialise': 'S',
           'CHud::RenderArmorBar': 'S',
           'CHud::RenderBreathBar': 'S',
           'CHud::RenderHealthBar': 'S',
           'CHud::ResetWastedText': 'S',
           'CHud::SetBigMessage': 'S',
           'CHud::SetHelpMessage': 'S',
           'CHud::SetHelpMessageStatUpdate': 'S',
           'CHud::SetHelpMessageWithNumber': 'S',
           'CHud::SetMessage': 'S',
           'CHud::SetVehicleName': 'S',
           'CHud::SetZoneName': 'S'},
 'pending_after': ['CCam::Process_1stPerson',
                   'CCam::Process_Rocket',
                   'CHud::Draw',
                   'CHud::DrawAfterFade',
                   'CHud::DrawRadar',
                   'CHud::DrawWeaponIcon',
                   'CHud::Initialise',
                   'CHud::Shutdown'],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled CamHud1 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree '
               'at registration; full 18688-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 18688 isolated CPU pairs for '
           '36 pending functions of CCam, CHud, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural "complete" event with '
           'state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case index) in both legs and compares the '
           'return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 bits) and the whole trace of doubled collaborators with raw '
           'arguments; foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == current working tree at '
           'registration (class sources match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Limits: '
           "GXT texts only with characters 0x20..0x7D (never '~', except the '~1~' of SetHelpMessageWithNumber); floats moderate (no Inf, no signalling NaN); fake peds, "
           'vehicles (fake vtable, slot 14 = Teleport), RW camera and entity; CFont, CSprite2d, CMessages::AddToPreviousBriefArray, CWaterLevel::GetWaterLevel, '
           'CShadows, rand and others doubled with trace. NOT covered by the bank and NOT registered: CHud::Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, Initialise, '
           'Shutdown, CCam::Process_Rocket, CCam::Process_1stPerson (real RenderState table, files/textures, skeletal hierarchy, real world).',
 'specific': {'CCam::CacheLastSettingsDWCineyCam': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake '
                                                   'CHud/CCam state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::Constructor': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                   'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::DoCamBump': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                 'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::Finalise_DW_CineyCams': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake '
                                             'CHud/CCam state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::GetCoreDataForDWCineyCamMode': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake '
                                                    'CHud/CCam state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::GetVectorsReadyForRW': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                            'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::Get_TwoPlayer_AimVector': 'Corpus 768; now returns CEntity* (the EXE leaves the entity in EAX; no EXE caller reads it).',
              'CCam::Init': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state compared '
                            'as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                   'README); fake CHud/CCam state compared as hex, gString/gGxtString and the full fake CCam (0x238 '
                                                                   'bytes with guards).',
              'CCam::Process_Editor': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                      'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::Process_Fixed': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                     'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CCam::WellBufferMe': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                    'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawAmmo': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawAreaName': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                    'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawBustedWastedMessage': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake '
                                               'CHud/CCam state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawFadeState': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                     'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawMissionTitle': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                        'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawOddJobMessage': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                         'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawScriptText': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                      'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawTripSkip': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                    'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::DrawVehicleName': 'Corpus 1024; domain: HUD states 0..4 only (the EXE leaves the alpha uninitialised outside 1..4).',
              'CHud::GetRidOfAllHudMessages': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake '
                                              'CHud/CCam state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::GetYPosBasedOnHealth': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                            'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::HelpMessageDisplayed': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                            'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::ReInitialise': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                    'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::RenderArmorBar': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                      'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::RenderBreathBar': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                       'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::RenderHealthBar': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam '
                                       'state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::ResetWastedText': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                       'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::SetBigMessage': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                     'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::SetHelpMessage': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                      'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::SetHelpMessageStatUpdate': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake '
                                                'CHud/CCam state compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::SetHelpMessageWithNumber': 'Corpus 512; GXT texts with characters 0x20..0x7D, "~1~" inserted in half of the cases.',
              'CHud::SetMessage': 'Corpus 320; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                  'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::SetVehicleName': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                      'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).',
              'CHud::SetZoneName': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README); fake CHud/CCam state '
                                   'compared as hex, gString/gGxtString and the full fake CCam (0x238 bytes with guards).'},
 'source_changes': {'source/game_sa/Hud.cpp': 'CHud functions corrected against the EXE before the run (sprintf/direct writes instead of sprintf_s 0xFE fill, '
                                              'out-of-range message writes, x87 PC24); see git diff and reversing/CamHud1/README.md',
                    'source/game_sa/Cam.cpp': 'CCam functions corrected against the EXE (Process_Editor without NOTSA spdlog logs, Get_TwoPlayer_AimVector returns '
                                              'CEntity*, raw field offsets); see git diff',
                    'source/game_sa/Cam.h': 'Get_TwoPlayer_AimVector signature (CEntity*)'},
 'notes': 'CamHud1 registers only the 36 functions with cases in comparisons.jsonl (24 of CHud, 12 of CCam). CHud::Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, '
          'Initialise, Shutdown, CCam::Process_Rocket and Process_1stPerson stay pending (outside the bank: real RenderState table, HUD.TXD/textures, skeletal '
          'hierarchy, real world; their fixes were only checked by reading the assembly). CHud keeps 6 pending and CCam 2.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-155715-312758`: passed=true, 18688 pares, 36 funciones con 0 fallos (24 de CHud y 12 de CCam), evento complete (state_restored), '
                   'begin con known=true y 31 sitios correctos, saves y configuraciones intactos. Registrada con `register_camhud1.py` (evidencia en `runs/`, '
                   '`validation-summary.json`). Solo se registran las funciones con casos en la corrida. Siguen PENDIENTES las ocho funciones fuera del banco (ver la '
                   'sección «Fuera del banco»): CHud::Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, Initialise, Shutdown, CCam::Process_Rocket y Process_1stPerson; '
                   'CHud queda con 6 pendientes y CCam con 2. Límites: textos GXT sin `~`, floats moderados, peds/vehículos/cámara RW falsos y colaboradores doblados '
                   '(sin crédito).'}
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
assert begin is not None and events.get('begin')==1 and begin.get('known') is True and isinstance(begin.get('sites'),list) and len(begin['sites'])>0 and layout is None
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
