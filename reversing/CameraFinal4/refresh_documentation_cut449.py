"""Documentary cut449 (+91 funciones: CameraFinal4); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'complete': ['CCamera'],
 'groups_drop': 1,
 'intro': 'noventa y una de CCamera (CameraFinal4)',
 'kind': 'CPU en el juego completo con partida cargada, TheCamera real, mundo falso estático y colaboradores doblados con traza '
         '(banco por pares, passed=true global)',
 'lim': {'CCamera::AddShakeSimple': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                    'corregida (0x50D240/0x50D280/0x516500): reloj sin redondeo a float. ',
         'CCamera::AllowShootingWith2PlayersInCar': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                    'crédito; ',
         'CCamera::ApplyVehicleCameraTweaks': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; fuente corregida (0x50BF40/0x50A480/0x50AF90/0x5B24A0): args de 32 bits, '
                                              'ModelID negativo, sin código de depuración, delete sin tamaño. ',
         'CCamera::CalculateGroundHeight': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                           'fuente corregida (0x514B80): bandera 0xB70040, modelo de CModelInfo, caso por '
                                           'defecto. ',
         'CCamera::CalculateMirroredMatrix': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                             'crédito; fuente corregida (0x50B380): producto escalar z,y,x. ',
         'CCamera::CamShake': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                              'corregida (0x50A9F0/0x50A970): fild*0.001f y distancia del EXE. ',
         'CCamera::CamShakeNoPos': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                   'corregida (0x50A9F0/0x50A970): fild*0.001f y distancia del EXE. ',
         'CCamera::CameraGenericModeSpecialCases': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                   'crédito; fuente corregida (0x50CD30/0x50CE80): entidad sostenida; solo '
                                                   'm_nExtraEntitiesCount entradas. ',
         'CCamera::CameraPedAimModeSpecialCases': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                  'crédito; ',
         'CCamera::CameraPedModeSpecialCases': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                               'crédito; ',
         'CCamera::CameraVehicleModeSpecialCases': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                   'crédito; fuente corregida (0x50CEF0/0x50CDE0): cuadrado de la velocidad con '
                                                   'constantes del EXE. ',
         'CCamera::ClearPlayerWeaponMode': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                           'crédito; ',
         'CCamera::ConeCastCollisionResolve': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; fuente corregida (0x51A5D0): destino y esferas del EXE. ',
         'CCamera::ConsiderPedAsDucking': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::Constructor': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                 'corregida (0x51A450): solo las escrituras del EXE. CCam::Init doblado: no se valida el '
                                 'contenido de las tres CCam. ',
         'CCamera::DealWithMirrorBeforeConstructRenderList': 'TheCamera real, mundo falso estático y colaboradores doblados con '
                                                             'traza sin crédito; ',
         'CCamera::DeleteCutSceneCamDataMemory': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                 'crédito; fuente corregida (0x50BF40/0x50A480/0x50AF90/0x5B24A0): args de 32 '
                                                 'bits, ModelID negativo, sin código de depuración, delete sin tamaño. ',
         'CCamera::DontProcessObbeCinemaCamera': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                 'crédito; ',
         'CCamera::DrawBordersForWideScreen': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; fuente corregida (0x50AB50/0x514860): aritmética y orden del CRect del '
                                              'EXE. ',
         'CCamera::Enable1rstPersonCamCntrlsScript': 'TheCamera real, mundo falso estático y colaboradores doblados con traza '
                                                     'sin crédito; ',
         'CCamera::Enable1rstPersonWeaponsCamera': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                   'crédito; ',
         'CCamera::Fade': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente corregida '
                          '(0x50AC20): comparaciones del EXE. ',
         'CCamera::Find3rdPersonCamTargetVector': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                  'crédito; fuente corregida (0x50AD40/0x514970): x87 en el orden del EXE. ',
         'CCamera::Find3rdPersonQuickAimPitch': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                'crédito; fuente corregida (0x50AD40/0x514970): x87 en el orden del EXE. ',
         'CCamera::FindCamFOV': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::FinishCutscene': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::GetArrPosForVehicleType': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                             'crédito; ',
         'CCamera::GetCutSceneFinishTime': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                           'fuente corregida (0x50AD90/0x50C070): puntero en vez de copia de la cámara. ',
         'CCamera::GetFading': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::GetFadingDirection': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::GetGameCamPosition': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::GetLookDirection': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::GetLookingForwardFirstPerson': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                  'crédito; ',
         'CCamera::GetLookingLRBFirstPerson': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; ',
         'CCamera::GetPositionAlongSpline': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                            'crédito; ',
         'CCamera::GetRoughDistanceToGround': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; CalculateGroundHeight doblada. ',
         'CCamera::GetScreenFadeStatus': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::GetScreenRect': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                   'corregida (0x50AB50/0x514860): aritmética y orden del CRect del EXE. ',
         'CCamera::Get_Just_Switched_Status': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; ',
         'CCamera::Init': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente corregida '
                          '(0x5BC520): reescrita desde el asm. CCam::Init doblado: no se valida el contenido de las tres CCam. ',
         'CCamera::InitCameraVehicleTweaks': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                             'crédito; ',
         'CCamera::InitialiseCameraForDebugMode': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                  'crédito; fuente corregida (0x50BF40/0x50A480/0x50AF90/0x5B24A0): args de 32 '
                                                  'bits, ModelID negativo, sin código de depuración, delete sin tamaño. ',
         'CCamera::InitialiseScriptableComponents': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                    'crédito; ',
         'CCamera::IsExtraEntityToIgnore': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                           'fuente corregida (0x50CD30/0x50CE80): entidad sostenida; solo m_nExtraEntitiesCount '
                                           'entradas. ',
         'CCamera::LerpFOV': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                             'corregida (0x50D240/0x50D280/0x516500): reloj sin redondeo a float. ',
         'CCamera::ProcessFOVLerp-1': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                      'fuente corregida (0x50D240/0x50D280/0x516500): reloj sin redondeo a float. ',
         'CCamera::ProcessFade': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                 'corregida (0x50B5D0/0x50B6D0): paso*0.02f y _ftol2. ',
         'CCamera::ProcessMusicFade': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                      'fuente corregida (0x50B5D0/0x50B6D0): paso*0.02f y _ftol2. ',
         'CCamera::ProcessObbeCinemaCameraPed': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                'crédito; ',
         'CCamera::ProcessScriptedCommands': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                             'crédito; sus tres colaboradores doblados: solo orden y receptor. ',
         'CCamera::ProcessWideScreenOn': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::RenderMotionBlur': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; el '
                                      'EXE llama a un ret (0x71D700). ',
         'CCamera::ResetDuckingSystem': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                        'fuente corregida (0x50CEF0/0x50CDE0): cuadrado de la velocidad con constantes del EXE. ',
         'CCamera::Restore': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                             'corregida (0x50B930/0x50BAB0): jugador en línea, banderas restituidas. ',
         'CCamera::RestoreCameraAfterMirror': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; ',
         'CCamera::RestoreWithJumpCut': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                        'fuente corregida (0x50B930/0x50BAB0): jugador en línea, banderas restituidas. ',
         'CCamera::SetCamCollisionVarDataSet': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                               'crédito; ',
         'CCamera::SetCamCutSceneOffSet': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetCamPositionForFixedMode': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                'crédito; ',
         'CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString': 'TheCamera real, mundo falso estático y colaboradores '
                                                                      'doblados con traza sin crédito; ',
         'CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString': 'TheCamera real, mundo falso estático y '
                                                                              'colaboradores doblados con traza sin crédito; '
                                                                              'fuente corregida (0x50BDA0/0x50BE30): rumbo con '
                                                                              'GetATanOfXY y objetivo de TheCamera. ',
         'CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString': 'TheCamera real, mundo falso estático y colaboradores '
                                                                       'doblados con traza sin crédito; ',
         'CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString': 'TheCamera real, mundo falso estático y '
                                                                               'colaboradores doblados con traza sin crédito; '
                                                                               'fuente corregida (0x50BDA0/0x50BE30): rumbo con '
                                                                               'GetATanOfXY y objetivo de TheCamera. ',
         'CCamera::SetCameraUpForMirror': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetColVarsAimWeapon': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetColVarsPed': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                   'corregida (0x50CC50/0x50C160/0x50C1B0): condiciones del EXE. ',
         'CCamera::SetColVarsVehicle': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetFadeColour': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetMotionBlur': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                   'corregida (0x50BF40/0x50A480/0x50AF90/0x5B24A0): args de 32 bits, ModelID negativo, sin '
                                   'código de depuración, delete sin tamaño. ',
         'CCamera::SetMotionBlurAlpha': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetNearClipScript': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetNewPlayerWeaponMode': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                            'crédito; ',
         'CCamera::SetParametersForScriptInterpolation': 'TheCamera real, mundo falso estático y colaboradores doblados con '
                                                         'traza sin crédito; fuente corregida (0x50C030): *0.01f. ',
         'CCamera::SetPercentAlongCutScene': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                             'crédito; fuente corregida (0x50AD90/0x50C070): puntero en vez de copia de la '
                                             'cámara. ',
         'CCamera::SetRwCamera': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; el EXE '
                                 'llama además a 0x71D710 (devuelve 1). ',
         'CCamera::SetWideScreenOff': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetWideScreenOn': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::SetZoomValueCamStringScript': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                 'crédito; fuente corregida (0x50CC50/0x50C160/0x50C1B0): condiciones del EXE. ',
         'CCamera::SetZoomValueFollowPedScript': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                                 'crédito; fuente corregida (0x50CC50/0x50C160/0x50C1B0): condiciones del EXE. ',
         'CCamera::StartCooperativeCamMode': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                             'crédito; ',
         'CCamera::StartTransition': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                     'fuente corregida (0x515200): constantes de ángulo, fpatan y FindPlayerVehicle x3 del EXE. ',
         'CCamera::StartTransitionWhenNotFinishedInter': 'TheCamera real, mundo falso estático y colaboradores doblados con '
                                                         'traza sin crédito; ',
         'CCamera::StopCooperativeCamMode': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                            'crédito; ',
         'CCamera::StoreValuesDuringInterPol': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                               'crédito; ',
         'CCamera::TakeControl': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; fuente '
                                 'corregida (0x50C7C0): tipo de la entidad objetivo actual. ',
         'CCamera::TakeControlAttachToEntity': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                               'crédito; fuente corregida (0x50C910): reconstruida (antes llamaba al EXE). ',
         'CCamera::TakeControlNoEntity': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::TakeControlWithSpline': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                           'crédito; ',
         'CCamera::UpdateAimingCoors': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; ',
         'CCamera::UpdateTargetEntity': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin crédito; '
                                        'fuente corregida (0x50C360): reescrita desde el asm. ',
         'CCamera::Using1stPersonWeaponMode': 'TheCamera real, mundo falso estático y colaboradores doblados con traza sin '
                                              'crédito; '},
 'n': 91,
 'new_word': 'Mil trescientas treinta',
 'note': 'CameraFinal4 añade la aprobación de las 91 funciones de CCamera, 19648 pares con cero diferencias. La corrida '
         '20261010-130000-406558 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, saves y '
         'configuraciones intactos, con el banco (hooks de la clase apagados) frente a la candidata dentro del juego completo '
         'con partida cargada. Límites: TheCamera real guardada y restaurada en cada ejecución y mundo falso estático (peds, '
         'vehículos, matrices, tareas con vtable propia, datos de handling, RwCamera, edificio con modelo de colisión en una '
         'ranura sin uso, CWorld::Players falso); colaboradores doblados con traza sin crédito (referencias, '
         'FindPlayerPed/Vehicle, GetATanOfXY, consultas de tareas, audio, ProcessVerticalLine, cono de colisión, '
         'CSprite2d::DrawRect, RwRenderStateSet, operaciones de CMatrix, CPlaceable::SetMatrix, operator delete, CCam::Init y '
         'colaboradores de la clase); no se valida el contenido de las tres CCam en Init/Constructor; ProcessScriptedCommands '
         'dobla sus tres colaboradores y GetRoughDistanceToGround dobla CalculateGroundHeight; dominio del fixture con booleanos '
         '0/1, floats finitos moderados y m_nExtraEntitiesCount 0..2. Correcciones de fuente contra el EXE en Camera.cpp/.h (ver '
         'el README del banco). No quedan pendientes en CCamera. Aprobación histórica con dependency_review=null; la vigencia '
         'sigue en 0. [Resumen](../reversing/CameraFinal4/validation-summary.json).',
 'old_appr': 1239,
 'old_groups': 6,
 'old_pend': 471,
 'old_valid': 7975,
 'old_word': 'Mil doscientas treinta y nueve',
 'pairs': {'CCamera::AddShakeSimple': 256,
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
 'pending_counts': {},
 'summary': 'reversing/CameraFinal4/validation-summary.json'}
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
(HERE/'documentation-refresh-cut449.json').write_text(json.dumps({'date':'2026-10-10','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
