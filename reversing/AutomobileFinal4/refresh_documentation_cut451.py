"""Corte documental 451 (+99 funciones: AutomobileFinal4); conserva byte a byte el Markdown congelado y no toca documentation-frozen-exceptions.json."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'complete': ['CAutomobile'],
 'groups_drop': 1,
 'intro': 'noventa y nueve de CAutomobile (AutomobileFinal4)',
 'kind': 'CPU en el juego completo con partida cargada, coches falsos y colaboradores doblados con traza (banco por pares, '
         'passed=true global)',
 'lim': {'CAutomobile::AddMovingCollisionSpeed': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 49 '
                                                 'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                                 'propia en el catálogo); ',
         'CAutomobile::BlowUpCar': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 41 de la vtable '
                                   '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::BlowUpCarCutSceneNoExtras': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura '
                                                   '42 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                                   'propia en el catálogo); ',
         'CAutomobile::BlowUpCarsInPath': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::BoostJumpControl': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::BreakTowLink': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 62 de la '
                                      'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                      'catálogo); ',
         'CAutomobile::ClearHeliOrientation': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::CloseAllDoors': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::CloseBoot': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::Constructor': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::CustomCarPlate_AfterRenderingStop': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::CustomCarPlate_BeforeRenderingStart': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::Destructor': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::DoBurstAndSoftGroundRatios': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura '
                                                    '52 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                                    'propia en el catálogo); ',
         'CAutomobile::DoHoverSuspensionRatios': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 69 '
                                                 'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                                 'propia en el catálogo); ',
         'CAutomobile::DoSoftGroundResistance': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::FindWheelWidth': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 63 de la '
                                        'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                        'catálogo); ',
         'CAutomobile::FireTruckControl': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::Fix': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 50 de la vtable '
                             '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::FixDoor': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::FixPanel': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::FixTyre': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::GetAllWheelsOffGround': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::GetCarPitch': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::GetCarRoll': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::GetComponentWorldPosition': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura '
                                                   '25 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                                   'propia en el catálogo); ',
         'CAutomobile::GetDooorAngleOpenRatio': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 30 '
                                                'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia '
                                                'en el catálogo); ',
         'CAutomobile::GetDooorAngleOpenRatioU32': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura '
                                                   '29 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                                   'propia en el catálogo); ',
         'CAutomobile::GetHeightAboveRoad': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 53 de '
                                            'la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                            'catálogo); ',
         'CAutomobile::GetMovingCollisionOffset': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::GetNumContactWheels': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 55 de '
                                             'la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                             'catálogo); ',
         'CAutomobile::GetTowBarPos': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 60 de la '
                                      'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                      'catálogo); ',
         'CAutomobile::GetTowHitchPos': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 59 de la '
                                        'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                        'catálogo); ',
         'CAutomobile::HydraulicControl': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::IsComponentPresent': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 26 de '
                                            'la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                            'catálogo); ',
         'CAutomobile::IsDoorClosed': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 36 de la '
                                      'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                      'catálogo); ',
         'CAutomobile::IsDoorClosedU32': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 35 de la '
                                         'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                         'catálogo); ',
         'CAutomobile::IsDoorFullyOpen': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 34 de la '
                                         'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                         'catálogo); ',
         'CAutomobile::IsDoorFullyOpenU32': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 33 de '
                                            'la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                            'catálogo); ',
         'CAutomobile::IsDoorMissing': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 38 de la '
                                       'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                       'catálogo); ',
         'CAutomobile::IsDoorMissingU32': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 37 de la '
                                          'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                          'catálogo); ',
         'CAutomobile::IsDoorReady': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 32 de la '
                                     'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                     'catálogo); ',
         'CAutomobile::IsDoorReadyU32': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 31 de la '
                                        'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                        'catálogo); ',
         'CAutomobile::IsInAir': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::IsOpenTopCar': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 39 de la '
                                      'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                      'catálogo); ',
         'CAutomobile::IsRoomForPedToLeaveCar': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 45 '
                                                'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia '
                                                'en el catálogo); ',
         'CAutomobile::KnockPedOutCar': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::Load': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 65 de la vtable '
                              '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::NitrousControl': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::OpenDoor': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 27 de la vtable '
                                  '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::PlaceOnRoadProperly': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::PlayCarHorn': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 54 de la '
                                     'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                     'catálogo); ',
         'CAutomobile::PlayHornIfNecessary': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::PopBoot': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::PopBootUsingPhysics': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::PopDoor': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::PopPanel': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ProcessAI': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 66 de la vtable '
                                   '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::ProcessAutoBusDoors': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ProcessBuoyancy': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ProcessCarOnFireAndExplode': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ProcessCarWheelPair': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ProcessControl': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 10 de la '
                                        'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                        'catálogo); ',
         'CAutomobile::ProcessControlCollisionCheck': 'coche falso y colaboradores doblados con traza sin crédito; virtual: '
                                                      'ranura 23 de la vtable 0x871120 de CAutomobile (original por vtable, sin '
                                                      'dirección propia en el catálogo); ',
         'CAutomobile::ProcessControlInputs': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 24 de '
                                              'la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en '
                                              'el catálogo); ',
         'CAutomobile::ProcessEntityCollision': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 22 '
                                                'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia '
                                                'en el catálogo); ',
         'CAutomobile::ProcessFlyingCarStuff': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 68 '
                                               'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia '
                                               'en el catálogo); ',
         'CAutomobile::ProcessHarvester': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ProcessSuspension': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 70 de la '
                                           'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                           'catálogo); ',
         'CAutomobile::ProcessSwingingDoor': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::RcbanditCheck1CarWheels': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::RcbanditCheckHitWheels': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ReduceHornCounter': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::RemoveBonnetInPedCollision': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::ResetSuspension': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 67 de la '
                                         'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                         'catálogo); ',
         'CAutomobile::Save': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 64 de la vtable '
                              '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::ScanForCrimes': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetAllTaxiLights': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetBumperDamage': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetBusDoorTimer': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetDoorDamage': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetHeliOrientation': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetPanelDamage': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetTaxiLight': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetTotalDamage': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetTowLink': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 61 de la vtable '
                                    '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::SetupDamageAfterLoad': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 51 de '
                                              'la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en '
                                              'el catálogo); ',
         'CAutomobile::SetupModelNodes': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::SetupSuspensionLines': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 48 de '
                                              'la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en '
                                              'el catálogo); ',
         'CAutomobile::SpawnFlyingComponent': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::StopNitroEffect': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::TankControl': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::Teleport': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 14 de la vtable '
                                  '0x871120 de CAutomobile (original por vtable, sin dirección propia en el catálogo); ',
         'CAutomobile::TellHeliToGoToCoors': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::TellPlaneToGoToCoors': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::TowTruckControl': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::UpdateMovingCollision': 'coche falso y colaboradores doblados con traza sin crédito; ',
         'CAutomobile::VehicleDamage': 'coche falso y colaboradores doblados con traza sin crédito; virtual: ranura 56 de la '
                                       'vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                       'catálogo); ',
         'CAutomobile::dmgDrawCarCollidingParticles': 'coche falso y colaboradores doblados con traza sin crédito; '},
 'n': 99,
 'new_word': 'Mil quinientas trece',
 'note': 'AutomobileFinal4 añade la aprobación de las 99 funciones de CAutomobile, 24192 pares con cero diferencias. La corrida '
         '20261010-142611-264011 tiene passed=true global y evento complete con state_restored=true, saves y configuraciones '
         'intactos, con el banco (hooks de la clase apagados) frente a la candidata dentro del juego completo con partida '
         'cargada. Límites: coches falsos de CAutomobile con vtable falsa, model infos, handling, colmodels y marcos RenderWare '
         'falsos; RenderWare, audio, partículas, mundo, física, colisión y peatones doblados con traza sin crédito; globales '
         'tocados guardados y restaurados en cada ejecución; operator new/delete sobre una arena de 128 KB; generador aleatorio '
         'del EXE. Son aprobaciones históricas con dependency_review=null; la vigencia sigue en 0 y CAutomobile queda sin '
         'pendientes.',
 'old_appr': 1414,
 'old_groups': 4,
 'old_pend': 296,
 'old_valid': 8150,
 'old_word': 'Mil cuatrocientas catorce',
 'pairs': {'CAutomobile::AddMovingCollisionSpeed': 256,
           'CAutomobile::BlowUpCar': 256,
           'CAutomobile::BlowUpCarCutSceneNoExtras': 256,
           'CAutomobile::BlowUpCarsInPath': 128,
           'CAutomobile::BoostJumpControl': 256,
           'CAutomobile::BreakTowLink': 128,
           'CAutomobile::ClearHeliOrientation': 64,
           'CAutomobile::CloseAllDoors': 256,
           'CAutomobile::CloseBoot': 192,
           'CAutomobile::Constructor': 512,
           'CAutomobile::CustomCarPlate_AfterRenderingStop': 128,
           'CAutomobile::CustomCarPlate_BeforeRenderingStart': 128,
           'CAutomobile::Destructor': 128,
           'CAutomobile::DoBurstAndSoftGroundRatios': 256,
           'CAutomobile::DoHoverSuspensionRatios': 128,
           'CAutomobile::DoSoftGroundResistance': 256,
           'CAutomobile::FindWheelWidth': 192,
           'CAutomobile::FireTruckControl': 256,
           'CAutomobile::Fix': 256,
           'CAutomobile::FixDoor': 256,
           'CAutomobile::FixPanel': 256,
           'CAutomobile::FixTyre': 128,
           'CAutomobile::GetAllWheelsOffGround': 128,
           'CAutomobile::GetCarPitch': 256,
           'CAutomobile::GetCarRoll': 256,
           'CAutomobile::GetComponentWorldPosition': 192,
           'CAutomobile::GetDooorAngleOpenRatio': 192,
           'CAutomobile::GetDooorAngleOpenRatioU32': 192,
           'CAutomobile::GetHeightAboveRoad': 128,
           'CAutomobile::GetMovingCollisionOffset': 256,
           'CAutomobile::GetNumContactWheels': 128,
           'CAutomobile::GetTowBarPos': 256,
           'CAutomobile::GetTowHitchPos': 256,
           'CAutomobile::HydraulicControl': 384,
           'CAutomobile::IsComponentPresent': 128,
           'CAutomobile::IsDoorClosed': 192,
           'CAutomobile::IsDoorClosedU32': 192,
           'CAutomobile::IsDoorFullyOpen': 192,
           'CAutomobile::IsDoorFullyOpenU32': 192,
           'CAutomobile::IsDoorMissing': 192,
           'CAutomobile::IsDoorMissingU32': 192,
           'CAutomobile::IsDoorReady': 192,
           'CAutomobile::IsDoorReadyU32': 192,
           'CAutomobile::IsInAir': 256,
           'CAutomobile::IsOpenTopCar': 128,
           'CAutomobile::IsRoomForPedToLeaveCar': 256,
           'CAutomobile::KnockPedOutCar': 64,
           'CAutomobile::Load': 64,
           'CAutomobile::NitrousControl': 384,
           'CAutomobile::OpenDoor': 384,
           'CAutomobile::PlaceOnRoadProperly': 256,
           'CAutomobile::PlayCarHorn': 128,
           'CAutomobile::PlayHornIfNecessary': 128,
           'CAutomobile::PopBoot': 192,
           'CAutomobile::PopBootUsingPhysics': 128,
           'CAutomobile::PopDoor': 256,
           'CAutomobile::PopPanel': 256,
           'CAutomobile::ProcessAI': 512,
           'CAutomobile::ProcessAutoBusDoors': 256,
           'CAutomobile::ProcessBuoyancy': 512,
           'CAutomobile::ProcessCarOnFireAndExplode': 384,
           'CAutomobile::ProcessCarWheelPair': 1024,
           'CAutomobile::ProcessControl': 512,
           'CAutomobile::ProcessControlCollisionCheck': 256,
           'CAutomobile::ProcessControlInputs': 384,
           'CAutomobile::ProcessEntityCollision': 512,
           'CAutomobile::ProcessFlyingCarStuff': 128,
           'CAutomobile::ProcessHarvester': 256,
           'CAutomobile::ProcessSuspension': 512,
           'CAutomobile::ProcessSwingingDoor': 384,
           'CAutomobile::RcbanditCheck1CarWheels': 256,
           'CAutomobile::RcbanditCheckHitWheels': 192,
           'CAutomobile::ReduceHornCounter': 128,
           'CAutomobile::RemoveBonnetInPedCollision': 192,
           'CAutomobile::ResetSuspension': 192,
           'CAutomobile::Save': 64,
           'CAutomobile::ScanForCrimes': 256,
           'CAutomobile::SetAllTaxiLights': 64,
           'CAutomobile::SetBumperDamage': 256,
           'CAutomobile::SetBusDoorTimer': 128,
           'CAutomobile::SetDoorDamage': 384,
           'CAutomobile::SetHeliOrientation': 64,
           'CAutomobile::SetPanelDamage': 256,
           'CAutomobile::SetTaxiLight': 64,
           'CAutomobile::SetTotalDamage': 256,
           'CAutomobile::SetTowLink': 256,
           'CAutomobile::SetupDamageAfterLoad': 256,
           'CAutomobile::SetupModelNodes': 192,
           'CAutomobile::SetupSuspensionLines': 256,
           'CAutomobile::SpawnFlyingComponent': 384,
           'CAutomobile::StopNitroEffect': 128,
           'CAutomobile::TankControl': 384,
           'CAutomobile::Teleport': 192,
           'CAutomobile::TellHeliToGoToCoors': 128,
           'CAutomobile::TellPlaneToGoToCoors': 192,
           'CAutomobile::TowTruckControl': 256,
           'CAutomobile::UpdateMovingCollision': 384,
           'CAutomobile::VehicleDamage': 768,
           'CAutomobile::dmgDrawCarCollidingParticles': 192},
 'pending_counts': {'CAEVehicleAudioEntity': 102, 'CStreaming': 94, 'Win': 1},
 'summary': 'reversing/AutomobileFinal4/validation-summary.json'}
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
(HERE/'documentation-refresh-cut451.json').write_text(json.dumps({'date':'2026-10-10','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
