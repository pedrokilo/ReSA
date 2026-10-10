"""Registro historico de las 91 funciones de CCamera del banco CameraFinal4 (corrida 20261010-130000-406558). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'addr': {'CCamera::AddShakeSimple': '0x0050D240',
          'CCamera::AllowShootingWith2PlayersInCar': '0x0050C280',
          'CCamera::ApplyVehicleCameraTweaks': '0x0050A480',
          'CCamera::CalculateGroundHeight': '0x00514B80',
          'CCamera::CalculateMirroredMatrix': '0x0050B380',
          'CCamera::CamShake': '0x0050A9F0',
          'CCamera::CamShakeNoPos': '0x0050A970',
          'CCamera::CameraGenericModeSpecialCases': '0x0050CD30',
          'CCamera::CameraPedAimModeSpecialCases': '0x0050CDA0',
          'CCamera::CameraPedModeSpecialCases': '0x0050CD80',
          'CCamera::CameraVehicleModeSpecialCases': '0x0050CDE0',
          'CCamera::ClearPlayerWeaponMode': '0x0050AB10',
          'CCamera::ConeCastCollisionResolve': '0x0051A5D0',
          'CCamera::ConsiderPedAsDucking': '0x0050CEB0',
          'CCamera::Constructor': '0x0051A450',
          'CCamera::DealWithMirrorBeforeConstructRenderList': '0x0050B510',
          'CCamera::DeleteCutSceneCamDataMemory': '0x005B24A0',
          'CCamera::DontProcessObbeCinemaCamera': '0x0050AB40',
          'CCamera::DrawBordersForWideScreen': '0x00514860',
          'CCamera::Enable1rstPersonCamCntrlsScript': '0x0050AC00',
          'CCamera::Enable1rstPersonWeaponsCamera': '0x0050AC10',
          'CCamera::Fade': '0x0050AC20',
          'CCamera::Find3rdPersonCamTargetVector': '0x00514970',
          'CCamera::Find3rdPersonQuickAimPitch': '0x0050AD40',
          'CCamera::FindCamFOV': '0x0050AD20',
          'CCamera::FinishCutscene': '0x00514950',
          'CCamera::GetArrPosForVehicleType': '0x0050AF00',
          'CCamera::GetCutSceneFinishTime': '0x0050AD90',
          'CCamera::GetFading': '0x0050ADE0',
          'CCamera::GetFadingDirection': '0x0050ADF0',
          'CCamera::GetGameCamPosition': '0x0050AE50',
          'CCamera::GetLookDirection': '0x0050AE90',
          'CCamera::GetLookingForwardFirstPerson': '0x0050AED0',
          'CCamera::GetLookingLRBFirstPerson': '0x0050AE60',
          'CCamera::GetPositionAlongSpline': '0x0050AF80',
          'CCamera::GetRoughDistanceToGround': '0x00516B00',
          'CCamera::GetScreenFadeStatus': '0x0050AE20',
          'CCamera::GetScreenRect': '0x0050AB50',
          'CCamera::Get_Just_Switched_Status': '0x0050AE10',
          'CCamera::Init': '0x005BC520',
          'CCamera::InitCameraVehicleTweaks': '0x0050A3B0',
          'CCamera::InitialiseCameraForDebugMode': '0x0050AF90',
          'CCamera::InitialiseScriptableComponents': '0x0050D2D0',
          'CCamera::IsExtraEntityToIgnore': '0x0050CE80',
          'CCamera::LerpFOV': '0x0050D280',
          'CCamera::ProcessFOVLerp-1': '0x00516500',
          'CCamera::ProcessFade': '0x0050B5D0',
          'CCamera::ProcessMusicFade': '0x0050B6D0',
          'CCamera::ProcessObbeCinemaCameraPed': '0x0050B880',
          'CCamera::ProcessScriptedCommands': '0x00516AE0',
          'CCamera::ProcessWideScreenOn': '0x0050B890',
          'CCamera::RenderMotionBlur': '0x0050B8F0',
          'CCamera::ResetDuckingSystem': '0x0050CEF0',
          'CCamera::Restore': '0x0050B930',
          'CCamera::RestoreCameraAfterMirror': '0x0051A5A0',
          'CCamera::RestoreWithJumpCut': '0x0050BAB0',
          'CCamera::SetCamCollisionVarDataSet': '0x0050CB60',
          'CCamera::SetCamCutSceneOffSet': '0x0050BD20',
          'CCamera::SetCamPositionForFixedMode': '0x0050BEC0',
          'CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString': '0x0050BD40',
          'CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString': '0x0050BDA0',
          'CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString': '0x0050BD70',
          'CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString': '0x0050BE30',
          'CCamera::SetCameraUpForMirror': '0x0051A560',
          'CCamera::SetColVarsAimWeapon': '0x0050CBF0',
          'CCamera::SetColVarsPed': '0x0050CC50',
          'CCamera::SetColVarsVehicle': '0x0050CCA0',
          'CCamera::SetFadeColour': '0x0050BF00',
          'CCamera::SetMotionBlur': '0x0050BF40',
          'CCamera::SetMotionBlurAlpha': '0x0050BF80',
          'CCamera::SetNearClipScript': '0x0050BF90',
          'CCamera::SetNewPlayerWeaponMode': '0x0050BFB0',
          'CCamera::SetParametersForScriptInterpolation': '0x0050C030',
          'CCamera::SetPercentAlongCutScene': '0x0050C070',
          'CCamera::SetRwCamera': '0x0050C100',
          'CCamera::SetWideScreenOff': '0x0050C150',
          'CCamera::SetWideScreenOn': '0x0050C140',
          'CCamera::SetZoomValueCamStringScript': '0x0050C1B0',
          'CCamera::SetZoomValueFollowPedScript': '0x0050C160',
          'CCamera::StartCooperativeCamMode': '0x0050C260',
          'CCamera::StartTransition': '0x00515200',
          'CCamera::StartTransitionWhenNotFinishedInter': '0x00515BC0',
          'CCamera::StopCooperativeCamMode': '0x0050C270',
          'CCamera::StoreValuesDuringInterPol': '0x0050C290',
          'CCamera::TakeControl': '0x0050C7C0',
          'CCamera::TakeControlAttachToEntity': '0x0050C910',
          'CCamera::TakeControlNoEntity': '0x0050C8B0',
          'CCamera::TakeControlWithSpline': '0x0050CAE0',
          'CCamera::UpdateAimingCoors': '0x0050CB10',
          'CCamera::UpdateTargetEntity': '0x0050C360',
          'CCamera::Using1stPersonWeaponMode': '0x0050BFF0'},
 'allow_bank_change': [],
 'allow_src_change': [],
 'bad': {},
 'bank': 'CameraFinal4',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the '
           'hooks on), 19648 isolated CPU pairs for the 91 functions of CCamera, ALL pairs equal, pair counts equal to the bank '
           'table, passed=true globally, natural "complete" event with state_restored and layout_ok, saves unchanged, configs '
           'restored. Each case compares the return value, the trace of doubled collaborators with raw arguments, the hash of '
           'the fake world and of the saved globals, plus per-function fields; the x87 control word is 24 bits; foreign threads '
           'ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == current '
           'working tree at registration (class sources match the probe build manifest). Historical evidence approved with '
           'explicit limits; no current dependency accreditation. Bank limits (CameraFinal4): real TheCamera (0xD78 bytes) saved '
           'and restored, static fake world (peds, vehicles, matrices, tasks with own vtable, handling data, RwCamera, building '
           'with collision model in unused ModelInfo slot 19995, fake CWorld::Players); traced EntryPatch doubles without credit '
           'for reference management, FindPlayerPed/Vehicle, GetATanOfXY, task queries, audio, ProcessVerticalLine, cone cast, '
           'CSprite2d::DrawRect, RwRenderStateSet, CMatrix operator=/Invert/SetScale/Attach, CPlaceable::SetMatrix, operator '
           'delete, CCam::Init and class collaborators where needed (GetSimplestActiveTask is NOT doubled); fixture domain: '
           'booleans 0/1, moderate finite floats, m_nExtraEntitiesCount 0..2. See the bank README for the full list of masked '
           'fields and out-of-fixture domains.',
 'error': None,
 'expect': {'CCamera::AddShakeSimple': 256,
            'CCamera::AllowShootingWith2PlayersInCar': 64,
            'CCamera::ApplyVehicleCameraTweaks': 256,
            'CCamera::CalculateGroundHeight': 512,
            'CCamera::CalculateMirroredMatrix': 512,
            'CCamera::CamShake': 512,
            'CCamera::CamShakeNoPos': 256,
            'CCamera::CameraGenericModeSpecialCases': 128,
            'CCamera::CameraPedAimModeSpecialCases': 128,
            'CCamera::CameraPedModeSpecialCases': 64,
            'CCamera::CameraVehicleModeSpecialCases': 256,
            'CCamera::ClearPlayerWeaponMode': 64,
            'CCamera::ConeCastCollisionResolve': 512,
            'CCamera::ConsiderPedAsDucking': 128,
            'CCamera::Constructor': 64,
            'CCamera::DealWithMirrorBeforeConstructRenderList': 256,
            'CCamera::DeleteCutSceneCamDataMemory': 128,
            'CCamera::DontProcessObbeCinemaCamera': 64,
            'CCamera::DrawBordersForWideScreen': 256,
            'CCamera::Enable1rstPersonCamCntrlsScript': 64,
            'CCamera::Enable1rstPersonWeaponsCamera': 64,
            'CCamera::Fade': 512,
            'CCamera::Find3rdPersonCamTargetVector': 512,
            'CCamera::Find3rdPersonQuickAimPitch': 256,
            'CCamera::FindCamFOV': 64,
            'CCamera::FinishCutscene': 256,
            'CCamera::GetArrPosForVehicleType': 128,
            'CCamera::GetCutSceneFinishTime': 128,
            'CCamera::GetFading': 64,
            'CCamera::GetFadingDirection': 64,
            'CCamera::GetGameCamPosition': 64,
            'CCamera::GetLookDirection': 128,
            'CCamera::GetLookingForwardFirstPerson': 128,
            'CCamera::GetLookingLRBFirstPerson': 128,
            'CCamera::GetPositionAlongSpline': 64,
            'CCamera::GetRoughDistanceToGround': 128,
            'CCamera::GetScreenFadeStatus': 128,
            'CCamera::GetScreenRect': 256,
            'CCamera::Get_Just_Switched_Status': 64,
            'CCamera::Init': 128,
            'CCamera::InitCameraVehicleTweaks': 128,
            'CCamera::InitialiseCameraForDebugMode': 64,
            'CCamera::InitialiseScriptableComponents': 64,
            'CCamera::IsExtraEntityToIgnore': 256,
            'CCamera::LerpFOV': 256,
            'CCamera::ProcessFOVLerp-1': 512,
            'CCamera::ProcessFade': 512,
            'CCamera::ProcessMusicFade': 512,
            'CCamera::ProcessObbeCinemaCameraPed': 64,
            'CCamera::ProcessScriptedCommands': 256,
            'CCamera::ProcessWideScreenOn': 128,
            'CCamera::RenderMotionBlur': 64,
            'CCamera::ResetDuckingSystem': 256,
            'CCamera::Restore': 512,
            'CCamera::RestoreCameraAfterMirror': 128,
            'CCamera::RestoreWithJumpCut': 512,
            'CCamera::SetCamCollisionVarDataSet': 128,
            'CCamera::SetCamCutSceneOffSet': 64,
            'CCamera::SetCamPositionForFixedMode': 64,
            'CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString': 128,
            'CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString': 256,
            'CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString': 128,
            'CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString': 256,
            'CCamera::SetCameraUpForMirror': 128,
            'CCamera::SetColVarsAimWeapon': 128,
            'CCamera::SetColVarsPed': 256,
            'CCamera::SetColVarsVehicle': 256,
            'CCamera::SetFadeColour': 256,
            'CCamera::SetMotionBlur': 128,
            'CCamera::SetMotionBlurAlpha': 64,
            'CCamera::SetNearClipScript': 64,
            'CCamera::SetNewPlayerWeaponMode': 64,
            'CCamera::SetParametersForScriptInterpolation': 128,
            'CCamera::SetPercentAlongCutScene': 256,
            'CCamera::SetRwCamera': 128,
            'CCamera::SetWideScreenOff': 64,
            'CCamera::SetWideScreenOn': 64,
            'CCamera::SetZoomValueCamStringScript': 256,
            'CCamera::SetZoomValueFollowPedScript': 128,
            'CCamera::StartCooperativeCamMode': 64,
            'CCamera::StartTransition': 1024,
            'CCamera::StartTransitionWhenNotFinishedInter': 256,
            'CCamera::StopCooperativeCamMode': 64,
            'CCamera::StoreValuesDuringInterPol': 256,
            'CCamera::TakeControl': 512,
            'CCamera::TakeControlAttachToEntity': 512,
            'CCamera::TakeControlNoEntity': 256,
            'CCamera::TakeControlWithSpline': 128,
            'CCamera::UpdateAimingCoors': 64,
            'CCamera::UpdateTargetEntity': 1024,
            'CCamera::Using1stPersonWeaponMode': 128},
 'extra_unregistered': [],
 'new': ['CCamera::AddShakeSimple',
         'CCamera::AllowShootingWith2PlayersInCar',
         'CCamera::ApplyVehicleCameraTweaks',
         'CCamera::CalculateGroundHeight',
         'CCamera::CalculateMirroredMatrix',
         'CCamera::CamShake',
         'CCamera::CamShakeNoPos',
         'CCamera::CameraGenericModeSpecialCases',
         'CCamera::CameraPedAimModeSpecialCases',
         'CCamera::CameraPedModeSpecialCases',
         'CCamera::CameraVehicleModeSpecialCases',
         'CCamera::ClearPlayerWeaponMode',
         'CCamera::ConeCastCollisionResolve',
         'CCamera::ConsiderPedAsDucking',
         'CCamera::Constructor',
         'CCamera::DealWithMirrorBeforeConstructRenderList',
         'CCamera::DeleteCutSceneCamDataMemory',
         'CCamera::DontProcessObbeCinemaCamera',
         'CCamera::DrawBordersForWideScreen',
         'CCamera::Enable1rstPersonCamCntrlsScript',
         'CCamera::Enable1rstPersonWeaponsCamera',
         'CCamera::Fade',
         'CCamera::Find3rdPersonCamTargetVector',
         'CCamera::Find3rdPersonQuickAimPitch',
         'CCamera::FindCamFOV',
         'CCamera::FinishCutscene',
         'CCamera::GetArrPosForVehicleType',
         'CCamera::GetCutSceneFinishTime',
         'CCamera::GetFading',
         'CCamera::GetFadingDirection',
         'CCamera::GetGameCamPosition',
         'CCamera::GetLookDirection',
         'CCamera::GetLookingForwardFirstPerson',
         'CCamera::GetLookingLRBFirstPerson',
         'CCamera::GetPositionAlongSpline',
         'CCamera::GetRoughDistanceToGround',
         'CCamera::GetScreenFadeStatus',
         'CCamera::GetScreenRect',
         'CCamera::Get_Just_Switched_Status',
         'CCamera::Init',
         'CCamera::InitCameraVehicleTweaks',
         'CCamera::InitialiseCameraForDebugMode',
         'CCamera::InitialiseScriptableComponents',
         'CCamera::IsExtraEntityToIgnore',
         'CCamera::LerpFOV',
         'CCamera::ProcessFOVLerp-1',
         'CCamera::ProcessFade',
         'CCamera::ProcessMusicFade',
         'CCamera::ProcessObbeCinemaCameraPed',
         'CCamera::ProcessScriptedCommands',
         'CCamera::ProcessWideScreenOn',
         'CCamera::RenderMotionBlur',
         'CCamera::ResetDuckingSystem',
         'CCamera::Restore',
         'CCamera::RestoreCameraAfterMirror',
         'CCamera::RestoreWithJumpCut',
         'CCamera::SetCamCollisionVarDataSet',
         'CCamera::SetCamCutSceneOffSet',
         'CCamera::SetCamPositionForFixedMode',
         'CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString',
         'CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString',
         'CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString',
         'CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString',
         'CCamera::SetCameraUpForMirror',
         'CCamera::SetColVarsAimWeapon',
         'CCamera::SetColVarsPed',
         'CCamera::SetColVarsVehicle',
         'CCamera::SetFadeColour',
         'CCamera::SetMotionBlur',
         'CCamera::SetMotionBlurAlpha',
         'CCamera::SetNearClipScript',
         'CCamera::SetNewPlayerWeaponMode',
         'CCamera::SetParametersForScriptInterpolation',
         'CCamera::SetPercentAlongCutScene',
         'CCamera::SetRwCamera',
         'CCamera::SetWideScreenOff',
         'CCamera::SetWideScreenOn',
         'CCamera::SetZoomValueCamStringScript',
         'CCamera::SetZoomValueFollowPedScript',
         'CCamera::StartCooperativeCamMode',
         'CCamera::StartTransition',
         'CCamera::StartTransitionWhenNotFinishedInter',
         'CCamera::StopCooperativeCamMode',
         'CCamera::StoreValuesDuringInterPol',
         'CCamera::TakeControl',
         'CCamera::TakeControlAttachToEntity',
         'CCamera::TakeControlNoEntity',
         'CCamera::TakeControlWithSpline',
         'CCamera::UpdateAimingCoors',
         'CCamera::UpdateTargetEntity',
         'CCamera::Using1stPersonWeaponMode'],
 'notes': 'CameraFinal4 registers the 91 functions of CCamera (19648 pairs, 0 differences). Limits are declared per function; '
          'see scope and the bank README. No pending functions remain in CCamera.',
 'passed': True,
 'pending_after': [],
 'probe_prefix': '36e08144',
 'provenance': 'Exact compiled CameraFinal4 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe '
               'build manifest and equal to the working tree at registration; full 19648-row log reaudited offline in streaming '
               'and preserved gzipped with the original hash.',
 'readme_section': '## Aprobación (10/10/2026)\n'
                   '\n'
                   'Corrida `20261010-130000-406558`: passed=true, 19648 pares, 91 funciones registradas con 0 fallos, evento '
                   'complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con '
                   '`register_camerafinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CCamera queda '
                   'sin pendientes. Límites declarados en el alcance de cada función y en las secciones anteriores de este '
                   'README.',
 'registry_before': 7975,
 'rejected': [],
 'run_id': '20261010-130000-406558',
 'source_changes': {'source/game_sa/Camera.cpp': 'Correcciones contra el EXE (Init, Constructor, StartTransition, '
                                                 'UpdateTargetEntity, TakeControl, TakeControlAttachToEntity, Restore, '
                                                 'RestoreWithJumpCut, ProcessFade, ProcessMusicFade, Fade, CamShake, '
                                                 'CamShakeNoPos, AddShakeSimple, LerpFOV, ProcessFOVLerp-1, GetScreenRect, '
                                                 'DrawBordersForWideScreen, GetCutSceneFinishTime, SetPercentAlongCutScene, '
                                                 'SetParametersForScriptInterpolation, Find3rdPerson*, CalculateMirroredMatrix, '
                                                 'CalculateGroundHeight, ConeCastCollisionResolve, ResetDuckingSystem, '
                                                 'Camera*ModeSpecialCases, IsExtraEntityToIgnore, SetColVarsPed, '
                                                 'SetZoomValue*Script, SetCameraDirectly*ForAPed, SetMotionBlur, '
                                                 'ApplyVehicleCameraTweaks, InitialiseCameraForDebugMode, '
                                                 'DeleteCutSceneCamDataMemory); ver git diff y el README del banco.',
                    'source/game_sa/Camera.h': 'Cambios de cabecera del banco (ver git diff).'},
 'specific': {'CCamera::AddShakeSimple': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                         '(0x50D240/0x50D280/0x516500): clock added without float rounding. ',
              'CCamera::AllowShootingWith2PlayersInCar': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::ApplyVehicleCameraTweaks': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                                   '(0x50BF40/0x50A480/0x50AF90/0x5B24A0): 32-bit args, negative ModelID '
                                                   'skipped, debug code removed, unsized delete. ',
              'CCamera::CalculateGroundHeight': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                                '(0x514B80): init flag 0xB70040 bit 0, model from CModelInfo, default returns '
                                                'bbBottom. ',
              'CCamera::CalculateMirroredMatrix': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                                  '(0x50B380): dot product summed z,y,x. ',
              'CCamera::CamShake': 'Corpus 512; fake world and traced doubles (no credit). source corrected (0x50A9F0/0x50A970): '
                                   'fild*0.001f, EXE distance and clamps. ',
              'CCamera::CamShakeNoPos': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                        '(0x50A9F0/0x50A970): fild*0.001f, EXE distance and clamps. ',
              'CCamera::CameraGenericModeSpecialCases': 'Corpus 128; fake world and traced doubles (no credit). source corrected '
                                                        '(0x50CD30/0x50CE80): held entity stored; only m_nExtraEntitiesCount '
                                                        'entries walked. ',
              'CCamera::CameraPedAimModeSpecialCases': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::CameraPedModeSpecialCases': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::CameraVehicleModeSpecialCases': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                                        '(0x50CEF0/0x50CDE0): squared-speed compares with EXE constants. ',
              'CCamera::ClearPlayerWeaponMode': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::ConeCastCollisionResolve': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                                   '(0x51A5D0): destination and sphere parameters of the EXE. ',
              'CCamera::ConsiderPedAsDucking': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::Constructor': 'Corpus 64; fake world and traced doubles (no credit). source corrected (0x51A450): '
                                      'hand-transcribed EXE writes only. CCam::Init doubled: the contents of the three CCam are '
                                      'not validated. ',
              'CCamera::DealWithMirrorBeforeConstructRenderList': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CCamera::DeleteCutSceneCamDataMemory': 'Corpus 128; fake world and traced doubles (no credit). source corrected '
                                                      '(0x50BF40/0x50A480/0x50AF90/0x5B24A0): 32-bit args, negative ModelID '
                                                      'skipped, debug code removed, unsized delete. ',
              'CCamera::DontProcessObbeCinemaCamera': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::DrawBordersForWideScreen': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                                   '(0x50AB50/0x514860): EXE arithmetic and CRect field order. ',
              'CCamera::Enable1rstPersonCamCntrlsScript': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::Enable1rstPersonWeaponsCamera': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::Fade': 'Corpus 512; fake world and traced doubles (no credit). source corrected (0x50AC20): '
                               'min(max(d*0.3,0.3),d) with EXE compares. ',
              'CCamera::Find3rdPersonCamTargetVector': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                                       '(0x50AD40/0x514970): x87 fptan/fpatan in EXE order. ',
              'CCamera::Find3rdPersonQuickAimPitch': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                                     '(0x50AD40/0x514970): x87 fptan/fpatan in EXE order. ',
              'CCamera::FindCamFOV': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::FinishCutscene': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CCamera::GetArrPosForVehicleType': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::GetCutSceneFinishTime': 'Corpus 128; fake world and traced doubles (no credit). source corrected '
                                                '(0x50AD90/0x50C070): pointer to the active cam instead of a copy. ',
              'CCamera::GetFading': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::GetFadingDirection': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::GetGameCamPosition': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::GetLookDirection': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::GetLookingForwardFirstPerson': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::GetLookingLRBFirstPerson': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::GetPositionAlongSpline': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::GetRoughDistanceToGround': 'Corpus 128; fake world and traced doubles (no credit). CalculateGroundHeight '
                                                   'doubled. ',
              'CCamera::GetScreenFadeStatus': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::GetScreenRect': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                        '(0x50AB50/0x514860): EXE arithmetic and CRect field order. ',
              'CCamera::Get_Just_Switched_Status': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::Init': 'Corpus 128; fake world and traced doubles (no credit). source corrected (0x5BC520): rewritten '
                               'from the asm (rep stos 0x358 dwords, CCam::Init x3, constants). CCam::Init doubled: the contents '
                               'of the three CCam are not validated. ',
              'CCamera::InitCameraVehicleTweaks': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::InitialiseCameraForDebugMode': 'Corpus 64; fake world and traced doubles (no credit). source corrected '
                                                       '(0x50BF40/0x50A480/0x50AF90/0x5B24A0): 32-bit args, negative ModelID '
                                                       'skipped, debug code removed, unsized delete. ',
              'CCamera::InitialiseScriptableComponents': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::IsExtraEntityToIgnore': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                                '(0x50CD30/0x50CE80): held entity stored; only m_nExtraEntitiesCount entries '
                                                'walked. ',
              'CCamera::LerpFOV': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                  '(0x50D240/0x50D280/0x516500): clock added without float rounding. ',
              'CCamera::ProcessFOVLerp-1': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                           '(0x50D240/0x50D280/0x516500): clock added without float rounding. ',
              'CCamera::ProcessFade': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                      '(0x50B5D0/0x50B6D0): step*0.02f, _ftol2 truncation. ',
              'CCamera::ProcessMusicFade': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                           '(0x50B5D0/0x50B6D0): step*0.02f, _ftol2 truncation. ',
              'CCamera::ProcessObbeCinemaCameraPed': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::ProcessScriptedCommands': 'Corpus 256; fake world and traced doubles (no credit). its three '
                                                  'collaborators (ProcessVectorMoveLinear-1, ProcessVectorTrackLinear-1, '
                                                  'ProcessFOVLerp-1) doubled: only order and receiver validated. ',
              'CCamera::ProcessWideScreenOn': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::RenderMotionBlur': 'Corpus 64; fake world and traced doubles (no credit). the EXE calls a bare ret '
                                           '(0x71D700). ',
              'CCamera::ResetDuckingSystem': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                             '(0x50CEF0/0x50CDE0): squared-speed compares with EXE constants. ',
              'CCamera::Restore': 'Corpus 512; fake world and traced doubles (no credit). source corrected (0x50B930/0x50BAB0): '
                                  'player inline, FindPlayerVehicle twice, missing flags restored. ',
              'CCamera::RestoreCameraAfterMirror': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::RestoreWithJumpCut': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                             '(0x50B930/0x50BAB0): player inline, FindPlayerVehicle twice, missing flags '
                                             'restored. ',
              'CCamera::SetCamCollisionVarDataSet': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::SetCamCutSceneOffSet': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::SetCamPositionForFixedMode': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString': 'Corpus 128; fake world and traced doubles (no '
                                                                           'credit). ',
              'CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString': 'Corpus 256; fake world and traced doubles '
                                                                                   '(no credit). source corrected '
                                                                                   '(0x50BDA0/0x50BE30): GetATanOfXY heading and '
                                                                                   'TheCamera target. ',
              'CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString': 'Corpus 128; fake world and traced doubles (no '
                                                                            'credit). ',
              'CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString': 'Corpus 256; fake world and traced doubles '
                                                                                    '(no credit). source corrected '
                                                                                    '(0x50BDA0/0x50BE30): GetATanOfXY heading '
                                                                                    'and TheCamera target. ',
              'CCamera::SetCameraUpForMirror': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::SetColVarsAimWeapon': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::SetColVarsPed': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                        '(0x50CC50/0x50C160/0x50C1B0): EXE conditions. ',
              'CCamera::SetColVarsVehicle': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CCamera::SetFadeColour': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CCamera::SetMotionBlur': 'Corpus 128; fake world and traced doubles (no credit). source corrected '
                                        '(0x50BF40/0x50A480/0x50AF90/0x5B24A0): 32-bit args, negative ModelID skipped, debug '
                                        'code removed, unsized delete. ',
              'CCamera::SetMotionBlurAlpha': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::SetNearClipScript': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::SetNewPlayerWeaponMode': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::SetParametersForScriptInterpolation': 'Corpus 128; fake world and traced doubles (no credit). source '
                                                              'corrected (0x50C030): *0.01f. ',
              'CCamera::SetPercentAlongCutScene': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                                  '(0x50AD90/0x50C070): pointer to the active cam instead of a copy. ',
              'CCamera::SetRwCamera': 'Corpus 128; fake world and traced doubles (no credit). the EXE also calls 0x71D710 '
                                      '(returns 1). ',
              'CCamera::SetWideScreenOff': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::SetWideScreenOn': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::SetZoomValueCamStringScript': 'Corpus 256; fake world and traced doubles (no credit). source corrected '
                                                      '(0x50CC50/0x50C160/0x50C1B0): EXE conditions. ',
              'CCamera::SetZoomValueFollowPedScript': 'Corpus 128; fake world and traced doubles (no credit). source corrected '
                                                      '(0x50CC50/0x50C160/0x50C1B0): EXE conditions. ',
              'CCamera::StartCooperativeCamMode': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::StartTransition': 'Corpus 1024; fake world and traced doubles (no credit). source corrected (0x515200): '
                                          'EXE angle constants, fpatan weapon modes, FindPlayerVehicle x3. ',
              'CCamera::StartTransitionWhenNotFinishedInter': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CCamera::StopCooperativeCamMode': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::StoreValuesDuringInterPol': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CCamera::TakeControl': 'Corpus 512; fake world and traced doubles (no credit). source corrected (0x50C7C0): type '
                                      'read from the current target entity, final FindPlayerPed(-1). ',
              'CCamera::TakeControlAttachToEntity': 'Corpus 512; fake world and traced doubles (no credit). source corrected '
                                                    '(0x50C910): rebuilt (was calling the EXE). ',
              'CCamera::TakeControlNoEntity': 'Corpus 256; fake world and traced doubles (no credit). ',
              'CCamera::TakeControlWithSpline': 'Corpus 128; fake world and traced doubles (no credit). ',
              'CCamera::UpdateAimingCoors': 'Corpus 64; fake world and traced doubles (no credit). ',
              'CCamera::UpdateTargetEntity': 'Corpus 1024; fake world and traced doubles (no credit). source corrected '
                                             '(0x50C360): rewritten from the asm. ',
              'CCamera::Using1stPersonWeaponMode': 'Corpus 128; fake world and traced doubles (no credit). '},
 'tag': 'camerafinal4',
 'total': 19648,
 'types': {'CCamera::AddShakeSimple': 'S',
           'CCamera::AllowShootingWith2PlayersInCar': 'S',
           'CCamera::ApplyVehicleCameraTweaks': 'S',
           'CCamera::CalculateGroundHeight': 'S',
           'CCamera::CalculateMirroredMatrix': 'S',
           'CCamera::CamShake': 'S',
           'CCamera::CamShakeNoPos': 'S',
           'CCamera::CameraGenericModeSpecialCases': 'S',
           'CCamera::CameraPedAimModeSpecialCases': 'S',
           'CCamera::CameraPedModeSpecialCases': 'S',
           'CCamera::CameraVehicleModeSpecialCases': 'S',
           'CCamera::ClearPlayerWeaponMode': 'S',
           'CCamera::ConeCastCollisionResolve': 'S',
           'CCamera::ConsiderPedAsDucking': 'S',
           'CCamera::Constructor': 'S',
           'CCamera::DealWithMirrorBeforeConstructRenderList': 'S',
           'CCamera::DeleteCutSceneCamDataMemory': 'S',
           'CCamera::DontProcessObbeCinemaCamera': 'S',
           'CCamera::DrawBordersForWideScreen': 'S',
           'CCamera::Enable1rstPersonCamCntrlsScript': 'S',
           'CCamera::Enable1rstPersonWeaponsCamera': 'S',
           'CCamera::Fade': 'S',
           'CCamera::Find3rdPersonCamTargetVector': 'S',
           'CCamera::Find3rdPersonQuickAimPitch': 'S',
           'CCamera::FindCamFOV': 'S',
           'CCamera::FinishCutscene': 'S',
           'CCamera::GetArrPosForVehicleType': 'S',
           'CCamera::GetCutSceneFinishTime': 'S',
           'CCamera::GetFading': 'S',
           'CCamera::GetFadingDirection': 'S',
           'CCamera::GetGameCamPosition': 'S',
           'CCamera::GetLookDirection': 'S',
           'CCamera::GetLookingForwardFirstPerson': 'S',
           'CCamera::GetLookingLRBFirstPerson': 'S',
           'CCamera::GetPositionAlongSpline': 'S',
           'CCamera::GetRoughDistanceToGround': 'S',
           'CCamera::GetScreenFadeStatus': 'S',
           'CCamera::GetScreenRect': 'S',
           'CCamera::Get_Just_Switched_Status': 'S',
           'CCamera::Init': 'S',
           'CCamera::InitCameraVehicleTweaks': 'S',
           'CCamera::InitialiseCameraForDebugMode': 'S',
           'CCamera::InitialiseScriptableComponents': 'S',
           'CCamera::IsExtraEntityToIgnore': 'S',
           'CCamera::LerpFOV': 'S',
           'CCamera::ProcessFOVLerp-1': 'S',
           'CCamera::ProcessFade': 'S',
           'CCamera::ProcessMusicFade': 'S',
           'CCamera::ProcessObbeCinemaCameraPed': 'S',
           'CCamera::ProcessScriptedCommands': 'S',
           'CCamera::ProcessWideScreenOn': 'S',
           'CCamera::RenderMotionBlur': 'S',
           'CCamera::ResetDuckingSystem': 'S',
           'CCamera::Restore': 'S',
           'CCamera::RestoreCameraAfterMirror': 'S',
           'CCamera::RestoreWithJumpCut': 'S',
           'CCamera::SetCamCollisionVarDataSet': 'S',
           'CCamera::SetCamCutSceneOffSet': 'S',
           'CCamera::SetCamPositionForFixedMode': 'S',
           'CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString': 'S',
           'CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString': 'S',
           'CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString': 'S',
           'CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString': 'S',
           'CCamera::SetCameraUpForMirror': 'S',
           'CCamera::SetColVarsAimWeapon': 'S',
           'CCamera::SetColVarsPed': 'S',
           'CCamera::SetColVarsVehicle': 'S',
           'CCamera::SetFadeColour': 'S',
           'CCamera::SetMotionBlur': 'S',
           'CCamera::SetMotionBlurAlpha': 'S',
           'CCamera::SetNearClipScript': 'S',
           'CCamera::SetNewPlayerWeaponMode': 'S',
           'CCamera::SetParametersForScriptInterpolation': 'S',
           'CCamera::SetPercentAlongCutScene': 'S',
           'CCamera::SetRwCamera': 'S',
           'CCamera::SetWideScreenOff': 'S',
           'CCamera::SetWideScreenOn': 'S',
           'CCamera::SetZoomValueCamStringScript': 'S',
           'CCamera::SetZoomValueFollowPedScript': 'S',
           'CCamera::StartCooperativeCamMode': 'S',
           'CCamera::StartTransition': 'S',
           'CCamera::StartTransitionWhenNotFinishedInter': 'S',
           'CCamera::StopCooperativeCamMode': 'S',
           'CCamera::StoreValuesDuringInterPol': 'S',
           'CCamera::TakeControl': 'S',
           'CCamera::TakeControlAttachToEntity': 'S',
           'CCamera::TakeControlNoEntity': 'S',
           'CCamera::TakeControlWithSpline': 'S',
           'CCamera::UpdateAimingCoors': 'S',
           'CCamera::UpdateTargetEntity': 'S',
           'CCamera::Using1stPersonWeaponMode': 'S'}}
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
