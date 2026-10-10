"""Registro histórico de las 99 funciones de CAutomobile del banco AutomobileFinal4 (corrida 20261010-142611-264011). Re-audita en streaming la corrida, conserva la evidencia en runs/ y añade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'addr': {'CAutomobile::AddMovingCollisionSpeed': '0x00000000',
          'CAutomobile::BlowUpCar': '0x00000000',
          'CAutomobile::BlowUpCarCutSceneNoExtras': '0x00000000',
          'CAutomobile::BlowUpCarsInPath': '0x006AF110',
          'CAutomobile::BoostJumpControl': '0x006A3A60',
          'CAutomobile::BreakTowLink': '0x00000000',
          'CAutomobile::ClearHeliOrientation': '0x006A2460',
          'CAutomobile::CloseAllDoors': '0x006A4520',
          'CAutomobile::CloseBoot': '0x006AFA20',
          'CAutomobile::Constructor': '0x006B0A90',
          'CAutomobile::CustomCarPlate_AfterRenderingStop': '0x006A2F30',
          'CAutomobile::CustomCarPlate_BeforeRenderingStart': '0x006A2F00',
          'CAutomobile::Destructor': '0x006A61E0',
          'CAutomobile::DoBurstAndSoftGroundRatios': '0x00000000',
          'CAutomobile::DoHoverSuspensionRatios': '0x00000000',
          'CAutomobile::DoSoftGroundResistance': '0x006A4AF0',
          'CAutomobile::FindWheelWidth': '0x00000000',
          'CAutomobile::FireTruckControl': '0x00729B60',
          'CAutomobile::Fix': '0x00000000',
          'CAutomobile::FixDoor': '0x006A35A0',
          'CAutomobile::FixPanel': '0x006A3670',
          'CAutomobile::FixTyre': '0x006A3580',
          'CAutomobile::GetAllWheelsOffGround': '0x006A2F70',
          'CAutomobile::GetCarPitch': '0x006A6050',
          'CAutomobile::GetCarRoll': '0x006A6010',
          'CAutomobile::GetComponentWorldPosition': '0x00000000',
          'CAutomobile::GetDooorAngleOpenRatio': '0x00000000',
          'CAutomobile::GetDooorAngleOpenRatioU32': '0x00000000',
          'CAutomobile::GetHeightAboveRoad': '0x00000000',
          'CAutomobile::GetMovingCollisionOffset': '0x006A2150',
          'CAutomobile::GetNumContactWheels': '0x00000000',
          'CAutomobile::GetTowBarPos': '0x00000000',
          'CAutomobile::GetTowHitchPos': '0x00000000',
          'CAutomobile::HydraulicControl': '0x006A07A0',
          'CAutomobile::IsComponentPresent': '0x00000000',
          'CAutomobile::IsDoorClosed': '0x00000000',
          'CAutomobile::IsDoorClosedU32': '0x00000000',
          'CAutomobile::IsDoorFullyOpen': '0x00000000',
          'CAutomobile::IsDoorFullyOpenU32': '0x00000000',
          'CAutomobile::IsDoorMissing': '0x00000000',
          'CAutomobile::IsDoorMissingU32': '0x00000000',
          'CAutomobile::IsDoorReady': '0x00000000',
          'CAutomobile::IsDoorReadyU32': '0x00000000',
          'CAutomobile::IsInAir': '0x006A6140',
          'CAutomobile::IsOpenTopCar': '0x00000000',
          'CAutomobile::IsRoomForPedToLeaveCar': '0x00000000',
          'CAutomobile::KnockPedOutCar': '0x006A44C0',
          'CAutomobile::Load': '0x00000000',
          'CAutomobile::NitrousControl': '0x006A3EA0',
          'CAutomobile::OpenDoor': '0x00000000',
          'CAutomobile::PlaceOnRoadProperly': '0x006AF420',
          'CAutomobile::PlayCarHorn': '0x00000000',
          'CAutomobile::PlayHornIfNecessary': '0x006A3820',
          'CAutomobile::PopBoot': '0x006AF910',
          'CAutomobile::PopBootUsingPhysics': '0x006A44D0',
          'CAutomobile::PopDoor': '0x006ADEF0',
          'CAutomobile::PopPanel': '0x006ADF80',
          'CAutomobile::ProcessAI': '0x00000000',
          'CAutomobile::ProcessAutoBusDoors': '0x006A38A0',
          'CAutomobile::ProcessBuoyancy': '0x006A8C00',
          'CAutomobile::ProcessCarOnFireAndExplode': '0x006A7090',
          'CAutomobile::ProcessCarWheelPair': '0x006A4EC0',
          'CAutomobile::ProcessControl': '0x00000000',
          'CAutomobile::ProcessControlCollisionCheck': '0x00000000',
          'CAutomobile::ProcessControlInputs': '0x00000000',
          'CAutomobile::ProcessEntityCollision': '0x00000000',
          'CAutomobile::ProcessFlyingCarStuff': '0x00000000',
          'CAutomobile::ProcessHarvester': '0x006A9680',
          'CAutomobile::ProcessSuspension': '0x00000000',
          'CAutomobile::ProcessSwingingDoor': '0x006A9D70',
          'CAutomobile::RcbanditCheck1CarWheels': '0x006B3F70',
          'CAutomobile::RcbanditCheckHitWheels': '0x006B45E0',
          'CAutomobile::ReduceHornCounter': '0x006A29A0',
          'CAutomobile::RemoveBonnetInPedCollision': '0x006AA200',
          'CAutomobile::ResetSuspension': '0x00000000',
          'CAutomobile::Save': '0x00000000',
          'CAutomobile::ScanForCrimes': '0x006ADFF0',
          'CAutomobile::SetAllTaxiLights': '0x006A3760',
          'CAutomobile::SetBumperDamage': '0x006B1350',
          'CAutomobile::SetBusDoorTimer': '0x006A3860',
          'CAutomobile::SetDoorDamage': '0x006B1600',
          'CAutomobile::SetHeliOrientation': '0x006A2450',
          'CAutomobile::SetPanelDamage': '0x006B1480',
          'CAutomobile::SetTaxiLight': '0x006A3740',
          'CAutomobile::SetTotalDamage': '0x006A27F0',
          'CAutomobile::SetTowLink': '0x00000000',
          'CAutomobile::SetupDamageAfterLoad': '0x00000000',
          'CAutomobile::SetupModelNodes': '0x006A0770',
          'CAutomobile::SetupSuspensionLines': '0x00000000',
          'CAutomobile::SpawnFlyingComponent': '0x006A8580',
          'CAutomobile::StopNitroEffect': '0x006A3E60',
          'CAutomobile::TankControl': '0x006AE850',
          'CAutomobile::Teleport': '0x00000000',
          'CAutomobile::TellHeliToGoToCoors': '0x006A2390',
          'CAutomobile::TellPlaneToGoToCoors': '0x006A2470',
          'CAutomobile::TowTruckControl': '0x006A40F0',
          'CAutomobile::UpdateMovingCollision': '0x006A1460',
          'CAutomobile::VehicleDamage': '0x00000000',
          'CAutomobile::dmgDrawCarCollidingParticles': '0x006A6DC0'},
 'allow_bank_change': [],
 'allow_src_change': [],
 'bad': {},
 'bank': 'AutomobileFinal4',
 'common': 'Banco por pares sencillo dentro del juego completo con partida cargada (original con los hooks de la clase apagados '
           'frente a candidata con los hooks activos), 24192 pares de CPU aislados para las 99 funciones de CAutomobile, TODOS '
           'los pares iguales, número de pares igual a la tabla del banco, passed=true global, evento "complete" natural con '
           'state_restored, saves sin cambios, configuraciones restauradas. Cada caso compara el retorno, la traza de '
           'colaboradores doblados con argumentos en bruto, el hash del mundo falso, el de la arena de operator new/delete, el '
           'de los globales guardados y los campos propios de cada función; la palabra de control x87 es de 24 bits; el '
           'generador de números aleatorios se sustituye por el del EXE; se ignoran los hilos ajenos. Los dobles con traza NO '
           'tienen crédito. Solo se valida la parte propia de cada función. Fuente probada == árbol de trabajo en el momento del '
           'registro (Automobile.cpp y Automobile.h coinciden con el manifest de compilación del probe). Evidencia histórica '
           'aprobada con límites explícitos; sin acreditación vigente de dependencias. Límites del banco (AutomobileFinal4): '
           'coches falsos de CAutomobile (0x988 bytes con vtable falsa copiada de las direcciones del EXE), model infos, datos '
           'de handling, colmodels y marcos RenderWare falsos; todo lo que toca estado real (RenderWare, audio, partículas, '
           'mundo, física, colisión, peatones) se sustituye por dobles trazados en la dirección del EXE y en el símbolo de la '
           'DLL, sin crédito; cada global tocado se guarda y se restaura en cada ejecución; operator new/delete sobre una arena '
           'de 128 KB. Consulte el README del banco para la lista completa de límites.',
 'error': None,
 'expect': {'CAutomobile::AddMovingCollisionSpeed': 256,
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
 'extra_unregistered': [],
 'new': ['CAutomobile::AddMovingCollisionSpeed',
         'CAutomobile::BlowUpCar',
         'CAutomobile::BlowUpCarCutSceneNoExtras',
         'CAutomobile::BlowUpCarsInPath',
         'CAutomobile::BoostJumpControl',
         'CAutomobile::BreakTowLink',
         'CAutomobile::ClearHeliOrientation',
         'CAutomobile::CloseAllDoors',
         'CAutomobile::CloseBoot',
         'CAutomobile::Constructor',
         'CAutomobile::CustomCarPlate_AfterRenderingStop',
         'CAutomobile::CustomCarPlate_BeforeRenderingStart',
         'CAutomobile::Destructor',
         'CAutomobile::DoBurstAndSoftGroundRatios',
         'CAutomobile::DoHoverSuspensionRatios',
         'CAutomobile::DoSoftGroundResistance',
         'CAutomobile::FindWheelWidth',
         'CAutomobile::FireTruckControl',
         'CAutomobile::Fix',
         'CAutomobile::FixDoor',
         'CAutomobile::FixPanel',
         'CAutomobile::FixTyre',
         'CAutomobile::GetAllWheelsOffGround',
         'CAutomobile::GetCarPitch',
         'CAutomobile::GetCarRoll',
         'CAutomobile::GetComponentWorldPosition',
         'CAutomobile::GetDooorAngleOpenRatio',
         'CAutomobile::GetDooorAngleOpenRatioU32',
         'CAutomobile::GetHeightAboveRoad',
         'CAutomobile::GetMovingCollisionOffset',
         'CAutomobile::GetNumContactWheels',
         'CAutomobile::GetTowBarPos',
         'CAutomobile::GetTowHitchPos',
         'CAutomobile::HydraulicControl',
         'CAutomobile::IsComponentPresent',
         'CAutomobile::IsDoorClosed',
         'CAutomobile::IsDoorClosedU32',
         'CAutomobile::IsDoorFullyOpen',
         'CAutomobile::IsDoorFullyOpenU32',
         'CAutomobile::IsDoorMissing',
         'CAutomobile::IsDoorMissingU32',
         'CAutomobile::IsDoorReady',
         'CAutomobile::IsDoorReadyU32',
         'CAutomobile::IsInAir',
         'CAutomobile::IsOpenTopCar',
         'CAutomobile::IsRoomForPedToLeaveCar',
         'CAutomobile::KnockPedOutCar',
         'CAutomobile::Load',
         'CAutomobile::NitrousControl',
         'CAutomobile::OpenDoor',
         'CAutomobile::PlaceOnRoadProperly',
         'CAutomobile::PlayCarHorn',
         'CAutomobile::PlayHornIfNecessary',
         'CAutomobile::PopBoot',
         'CAutomobile::PopBootUsingPhysics',
         'CAutomobile::PopDoor',
         'CAutomobile::PopPanel',
         'CAutomobile::ProcessAI',
         'CAutomobile::ProcessAutoBusDoors',
         'CAutomobile::ProcessBuoyancy',
         'CAutomobile::ProcessCarOnFireAndExplode',
         'CAutomobile::ProcessCarWheelPair',
         'CAutomobile::ProcessControl',
         'CAutomobile::ProcessControlCollisionCheck',
         'CAutomobile::ProcessControlInputs',
         'CAutomobile::ProcessEntityCollision',
         'CAutomobile::ProcessFlyingCarStuff',
         'CAutomobile::ProcessHarvester',
         'CAutomobile::ProcessSuspension',
         'CAutomobile::ProcessSwingingDoor',
         'CAutomobile::RcbanditCheck1CarWheels',
         'CAutomobile::RcbanditCheckHitWheels',
         'CAutomobile::ReduceHornCounter',
         'CAutomobile::RemoveBonnetInPedCollision',
         'CAutomobile::ResetSuspension',
         'CAutomobile::Save',
         'CAutomobile::ScanForCrimes',
         'CAutomobile::SetAllTaxiLights',
         'CAutomobile::SetBumperDamage',
         'CAutomobile::SetBusDoorTimer',
         'CAutomobile::SetDoorDamage',
         'CAutomobile::SetHeliOrientation',
         'CAutomobile::SetPanelDamage',
         'CAutomobile::SetTaxiLight',
         'CAutomobile::SetTotalDamage',
         'CAutomobile::SetTowLink',
         'CAutomobile::SetupDamageAfterLoad',
         'CAutomobile::SetupModelNodes',
         'CAutomobile::SetupSuspensionLines',
         'CAutomobile::SpawnFlyingComponent',
         'CAutomobile::StopNitroEffect',
         'CAutomobile::TankControl',
         'CAutomobile::Teleport',
         'CAutomobile::TellHeliToGoToCoors',
         'CAutomobile::TellPlaneToGoToCoors',
         'CAutomobile::TowTruckControl',
         'CAutomobile::UpdateMovingCollision',
         'CAutomobile::VehicleDamage',
         'CAutomobile::dmgDrawCarCollidingParticles'],
 'notes': 'AutomobileFinal4 registra las 99 funciones de CAutomobile (24192 pares, 0 diferencias). Los límites se declaran por '
          'función; véanse function_scope y el README del banco. No quedan funciones pendientes en CAutomobile.',
 'passed': True,
 'pending_after': [],
 'probe_prefix': '57daff12',
 'provenance': 'Probe AutomobileFinal4 compilado exacto (hashes de ASI/PDB en build-manifest.json); fuentes de la clase '
               '(Automobile.cpp y Automobile.h) con hash del manifest de compilación e iguales al árbol de trabajo en el '
               'registro; log completo de 24192 filas reauditado offline en streaming y conservado comprimido con gzip con el '
               'hash original.',
 'readme_section': '## Aprobación (10/10/2026)\n'
                   '\n'
                   'Corrida `20261010-142611-264011`: passed=true, 24192 pares, 99 funciones registradas con 0 fallos, evento '
                   'complete (state_restored), saves y configuraciones intactos. Registrada con `register_automobilefinal4.py` '
                   '(evidencia en `runs/`, `validation-summary.json`). Con este banco CAutomobile queda sin pendientes. Límites '
                   'declarados en el alcance de cada función y en las secciones anteriores de este README.',
 'registry_before': 8150,
 'rejected': [],
 'run_id': '20261010-142611-264011',
 'source_changes': {},
 'specific': {'CAutomobile::AddMovingCollisionSpeed': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); '
                                                      'virtual: ranura 49 de la vtable 0x871120 de CAutomobile (original por '
                                                      'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::BlowUpCar': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                        'ranura 41 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                        'propia en el catálogo); ',
              'CAutomobile::BlowUpCarCutSceneNoExtras': 'Corpus 256; coche falso y colaboradores doblados con traza (sin '
                                                        'crédito); virtual: ranura 42 de la vtable 0x871120 de CAutomobile '
                                                        '(original por vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::BlowUpCarsInPath': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::BoostJumpControl': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::BreakTowLink': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                           'ranura 62 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                           'propia en el catálogo); ',
              'CAutomobile::ClearHeliOrientation': 'Corpus 64; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::CloseAllDoors': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::CloseBoot': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::Constructor': 'Corpus 512; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::CustomCarPlate_AfterRenderingStop': 'Corpus 128; coche falso y colaboradores doblados con traza (sin '
                                                                'crédito); ',
              'CAutomobile::CustomCarPlate_BeforeRenderingStart': 'Corpus 128; coche falso y colaboradores doblados con traza '
                                                                  '(sin crédito); ',
              'CAutomobile::Destructor': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::DoBurstAndSoftGroundRatios': 'Corpus 256; coche falso y colaboradores doblados con traza (sin '
                                                         'crédito); virtual: ranura 52 de la vtable 0x871120 de CAutomobile '
                                                         '(original por vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::DoHoverSuspensionRatios': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); '
                                                      'virtual: ranura 69 de la vtable 0x871120 de CAutomobile (original por '
                                                      'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::DoSoftGroundResistance': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::FindWheelWidth': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                             'ranura 63 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                             'propia en el catálogo); ',
              'CAutomobile::FireTruckControl': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::Fix': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); virtual: ranura 50 '
                                  'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                  'catálogo); ',
              'CAutomobile::FixDoor': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::FixPanel': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::FixTyre': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::GetAllWheelsOffGround': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::GetCarPitch': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::GetCarRoll': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::GetComponentWorldPosition': 'Corpus 192; coche falso y colaboradores doblados con traza (sin '
                                                        'crédito); virtual: ranura 25 de la vtable 0x871120 de CAutomobile '
                                                        '(original por vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::GetDooorAngleOpenRatio': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); '
                                                     'virtual: ranura 30 de la vtable 0x871120 de CAutomobile (original por '
                                                     'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::GetDooorAngleOpenRatioU32': 'Corpus 192; coche falso y colaboradores doblados con traza (sin '
                                                        'crédito); virtual: ranura 29 de la vtable 0x871120 de CAutomobile '
                                                        '(original por vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::GetHeightAboveRoad': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); '
                                                 'virtual: ranura 53 de la vtable 0x871120 de CAutomobile (original por vtable, '
                                                 'sin dirección propia en el catálogo); ',
              'CAutomobile::GetMovingCollisionOffset': 'Corpus 256; coche falso y colaboradores doblados con traza (sin '
                                                       'crédito); ',
              'CAutomobile::GetNumContactWheels': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); '
                                                  'virtual: ranura 55 de la vtable 0x871120 de CAutomobile (original por vtable, '
                                                  'sin dirección propia en el catálogo); ',
              'CAutomobile::GetTowBarPos': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                           'ranura 60 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                           'propia en el catálogo); ',
              'CAutomobile::GetTowHitchPos': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                             'ranura 59 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                             'propia en el catálogo); ',
              'CAutomobile::HydraulicControl': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::IsComponentPresent': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); '
                                                 'virtual: ranura 26 de la vtable 0x871120 de CAutomobile (original por vtable, '
                                                 'sin dirección propia en el catálogo); ',
              'CAutomobile::IsDoorClosed': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                           'ranura 36 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                           'propia en el catálogo); ',
              'CAutomobile::IsDoorClosedU32': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); '
                                              'virtual: ranura 35 de la vtable 0x871120 de CAutomobile (original por vtable, sin '
                                              'dirección propia en el catálogo); ',
              'CAutomobile::IsDoorFullyOpen': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); '
                                              'virtual: ranura 34 de la vtable 0x871120 de CAutomobile (original por vtable, sin '
                                              'dirección propia en el catálogo); ',
              'CAutomobile::IsDoorFullyOpenU32': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); '
                                                 'virtual: ranura 33 de la vtable 0x871120 de CAutomobile (original por vtable, '
                                                 'sin dirección propia en el catálogo); ',
              'CAutomobile::IsDoorMissing': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                            'ranura 38 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                            'propia en el catálogo); ',
              'CAutomobile::IsDoorMissingU32': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); '
                                               'virtual: ranura 37 de la vtable 0x871120 de CAutomobile (original por vtable, '
                                               'sin dirección propia en el catálogo); ',
              'CAutomobile::IsDoorReady': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                          'ranura 32 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                          'propia en el catálogo); ',
              'CAutomobile::IsDoorReadyU32': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                             'ranura 31 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                             'propia en el catálogo); ',
              'CAutomobile::IsInAir': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::IsOpenTopCar': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                           'ranura 39 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                           'propia en el catálogo); ',
              'CAutomobile::IsRoomForPedToLeaveCar': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); '
                                                     'virtual: ranura 45 de la vtable 0x871120 de CAutomobile (original por '
                                                     'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::KnockPedOutCar': 'Corpus 64; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::Load': 'Corpus 64; coche falso y colaboradores doblados con traza (sin crédito); virtual: ranura 65 '
                                   'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                   'catálogo); ',
              'CAutomobile::NitrousControl': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::OpenDoor': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                       'ranura 27 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                       'propia en el catálogo); ',
              'CAutomobile::PlaceOnRoadProperly': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::PlayCarHorn': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                          'ranura 54 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                          'propia en el catálogo); ',
              'CAutomobile::PlayHornIfNecessary': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::PopBoot': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::PopBootUsingPhysics': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::PopDoor': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::PopPanel': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::ProcessAI': 'Corpus 512; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                        'ranura 66 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                        'propia en el catálogo); ',
              'CAutomobile::ProcessAutoBusDoors': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::ProcessBuoyancy': 'Corpus 512; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::ProcessCarOnFireAndExplode': 'Corpus 384; coche falso y colaboradores doblados con traza (sin '
                                                         'crédito); ',
              'CAutomobile::ProcessCarWheelPair': 'Corpus 1024; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::ProcessControl': 'Corpus 512; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                             'ranura 10 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                             'propia en el catálogo); ',
              'CAutomobile::ProcessControlCollisionCheck': 'Corpus 256; coche falso y colaboradores doblados con traza (sin '
                                                           'crédito); virtual: ranura 23 de la vtable 0x871120 de CAutomobile '
                                                           '(original por vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::ProcessControlInputs': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); '
                                                   'virtual: ranura 24 de la vtable 0x871120 de CAutomobile (original por '
                                                   'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::ProcessEntityCollision': 'Corpus 512; coche falso y colaboradores doblados con traza (sin crédito); '
                                                     'virtual: ranura 22 de la vtable 0x871120 de CAutomobile (original por '
                                                     'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::ProcessFlyingCarStuff': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); '
                                                    'virtual: ranura 68 de la vtable 0x871120 de CAutomobile (original por '
                                                    'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::ProcessHarvester': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::ProcessSuspension': 'Corpus 512; coche falso y colaboradores doblados con traza (sin crédito); '
                                                'virtual: ranura 70 de la vtable 0x871120 de CAutomobile (original por vtable, '
                                                'sin dirección propia en el catálogo); ',
              'CAutomobile::ProcessSwingingDoor': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::RcbanditCheck1CarWheels': 'Corpus 256; coche falso y colaboradores doblados con traza (sin '
                                                      'crédito); ',
              'CAutomobile::RcbanditCheckHitWheels': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::ReduceHornCounter': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::RemoveBonnetInPedCollision': 'Corpus 192; coche falso y colaboradores doblados con traza (sin '
                                                         'crédito); ',
              'CAutomobile::ResetSuspension': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); '
                                              'virtual: ranura 67 de la vtable 0x871120 de CAutomobile (original por vtable, sin '
                                              'dirección propia en el catálogo); ',
              'CAutomobile::Save': 'Corpus 64; coche falso y colaboradores doblados con traza (sin crédito); virtual: ranura 64 '
                                   'de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección propia en el '
                                   'catálogo); ',
              'CAutomobile::ScanForCrimes': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetAllTaxiLights': 'Corpus 64; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetBumperDamage': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetBusDoorTimer': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetDoorDamage': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetHeliOrientation': 'Corpus 64; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetPanelDamage': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetTaxiLight': 'Corpus 64; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetTotalDamage': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetTowLink': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                         'ranura 61 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                         'propia en el catálogo); ',
              'CAutomobile::SetupDamageAfterLoad': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); '
                                                   'virtual: ranura 51 de la vtable 0x871120 de CAutomobile (original por '
                                                   'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::SetupModelNodes': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::SetupSuspensionLines': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); '
                                                   'virtual: ranura 48 de la vtable 0x871120 de CAutomobile (original por '
                                                   'vtable, sin dirección propia en el catálogo); ',
              'CAutomobile::SpawnFlyingComponent': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::StopNitroEffect': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::TankControl': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::Teleport': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                       'ranura 14 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                       'propia en el catálogo); ',
              'CAutomobile::TellHeliToGoToCoors': 'Corpus 128; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::TellPlaneToGoToCoors': 'Corpus 192; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::TowTruckControl': 'Corpus 256; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::UpdateMovingCollision': 'Corpus 384; coche falso y colaboradores doblados con traza (sin crédito); ',
              'CAutomobile::VehicleDamage': 'Corpus 768; coche falso y colaboradores doblados con traza (sin crédito); virtual: '
                                            'ranura 56 de la vtable 0x871120 de CAutomobile (original por vtable, sin dirección '
                                            'propia en el catálogo); ',
              'CAutomobile::dmgDrawCarCollidingParticles': 'Corpus 192; coche falso y colaboradores doblados con traza (sin '
                                                           'crédito); '},
 'tag': 'automobilefinal4',
 'total': 24192,
 'types': {'CAutomobile::AddMovingCollisionSpeed': 'V',
           'CAutomobile::BlowUpCar': 'V',
           'CAutomobile::BlowUpCarCutSceneNoExtras': 'V',
           'CAutomobile::BlowUpCarsInPath': 'S',
           'CAutomobile::BoostJumpControl': 'S',
           'CAutomobile::BreakTowLink': 'V',
           'CAutomobile::ClearHeliOrientation': 'S',
           'CAutomobile::CloseAllDoors': 'S',
           'CAutomobile::CloseBoot': 'S',
           'CAutomobile::Constructor': 'S',
           'CAutomobile::CustomCarPlate_AfterRenderingStop': 'S',
           'CAutomobile::CustomCarPlate_BeforeRenderingStart': 'S',
           'CAutomobile::Destructor': 'S',
           'CAutomobile::DoBurstAndSoftGroundRatios': 'V',
           'CAutomobile::DoHoverSuspensionRatios': 'V',
           'CAutomobile::DoSoftGroundResistance': 'S',
           'CAutomobile::FindWheelWidth': 'V',
           'CAutomobile::FireTruckControl': 'S',
           'CAutomobile::Fix': 'V',
           'CAutomobile::FixDoor': 'S',
           'CAutomobile::FixPanel': 'S',
           'CAutomobile::FixTyre': 'S',
           'CAutomobile::GetAllWheelsOffGround': 'S',
           'CAutomobile::GetCarPitch': 'S',
           'CAutomobile::GetCarRoll': 'S',
           'CAutomobile::GetComponentWorldPosition': 'V',
           'CAutomobile::GetDooorAngleOpenRatio': 'V',
           'CAutomobile::GetDooorAngleOpenRatioU32': 'V',
           'CAutomobile::GetHeightAboveRoad': 'V',
           'CAutomobile::GetMovingCollisionOffset': 'S',
           'CAutomobile::GetNumContactWheels': 'V',
           'CAutomobile::GetTowBarPos': 'V',
           'CAutomobile::GetTowHitchPos': 'V',
           'CAutomobile::HydraulicControl': 'S',
           'CAutomobile::IsComponentPresent': 'V',
           'CAutomobile::IsDoorClosed': 'V',
           'CAutomobile::IsDoorClosedU32': 'V',
           'CAutomobile::IsDoorFullyOpen': 'V',
           'CAutomobile::IsDoorFullyOpenU32': 'V',
           'CAutomobile::IsDoorMissing': 'V',
           'CAutomobile::IsDoorMissingU32': 'V',
           'CAutomobile::IsDoorReady': 'V',
           'CAutomobile::IsDoorReadyU32': 'V',
           'CAutomobile::IsInAir': 'S',
           'CAutomobile::IsOpenTopCar': 'V',
           'CAutomobile::IsRoomForPedToLeaveCar': 'V',
           'CAutomobile::KnockPedOutCar': 'S',
           'CAutomobile::Load': 'V',
           'CAutomobile::NitrousControl': 'S',
           'CAutomobile::OpenDoor': 'V',
           'CAutomobile::PlaceOnRoadProperly': 'S',
           'CAutomobile::PlayCarHorn': 'V',
           'CAutomobile::PlayHornIfNecessary': 'S',
           'CAutomobile::PopBoot': 'S',
           'CAutomobile::PopBootUsingPhysics': 'S',
           'CAutomobile::PopDoor': 'S',
           'CAutomobile::PopPanel': 'S',
           'CAutomobile::ProcessAI': 'V',
           'CAutomobile::ProcessAutoBusDoors': 'S',
           'CAutomobile::ProcessBuoyancy': 'S',
           'CAutomobile::ProcessCarOnFireAndExplode': 'S',
           'CAutomobile::ProcessCarWheelPair': 'S',
           'CAutomobile::ProcessControl': 'V',
           'CAutomobile::ProcessControlCollisionCheck': 'V',
           'CAutomobile::ProcessControlInputs': 'V',
           'CAutomobile::ProcessEntityCollision': 'V',
           'CAutomobile::ProcessFlyingCarStuff': 'V',
           'CAutomobile::ProcessHarvester': 'S',
           'CAutomobile::ProcessSuspension': 'V',
           'CAutomobile::ProcessSwingingDoor': 'S',
           'CAutomobile::RcbanditCheck1CarWheels': 'S',
           'CAutomobile::RcbanditCheckHitWheels': 'S',
           'CAutomobile::ReduceHornCounter': 'S',
           'CAutomobile::RemoveBonnetInPedCollision': 'S',
           'CAutomobile::ResetSuspension': 'V',
           'CAutomobile::Save': 'V',
           'CAutomobile::ScanForCrimes': 'S',
           'CAutomobile::SetAllTaxiLights': 'S',
           'CAutomobile::SetBumperDamage': 'S',
           'CAutomobile::SetBusDoorTimer': 'S',
           'CAutomobile::SetDoorDamage': 'S',
           'CAutomobile::SetHeliOrientation': 'S',
           'CAutomobile::SetPanelDamage': 'S',
           'CAutomobile::SetTaxiLight': 'S',
           'CAutomobile::SetTotalDamage': 'S',
           'CAutomobile::SetTowLink': 'V',
           'CAutomobile::SetupDamageAfterLoad': 'V',
           'CAutomobile::SetupModelNodes': 'S',
           'CAutomobile::SetupSuspensionLines': 'V',
           'CAutomobile::SpawnFlyingComponent': 'S',
           'CAutomobile::StopNitroEffect': 'S',
           'CAutomobile::TankControl': 'S',
           'CAutomobile::Teleport': 'V',
           'CAutomobile::TellHeliToGoToCoors': 'S',
           'CAutomobile::TellPlaneToGoToCoors': 'S',
           'CAutomobile::TowTruckControl': 'S',
           'CAutomobile::UpdateMovingCollision': 'S',
           'CAutomobile::VehicleDamage': 'V',
           'CAutomobile::dmgDrawCarCollidingParticles': 'S'}}
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
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='diferencias frente al EXE en la corrida; no registrada, sin crédito') for f in REJECTED}
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
     dependency_review=None,scope=COMMON+' Específico de la función: '+SPECIFIC[name],review_reference='reversing/%s/README.md'%BANK))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.%s.tmp'%CFG['tag']);temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
# README: only append the approval section at the end (append mode, bank file otherwise untouched)
readme=HERE/'README.md';assert readme.exists(),'falta README.md del banco';raw_r=readme.read_bytes();nl='\r\n' if b'\r\n' in raw_r else '\n'
section=CFG['readme_section'].replace('\n',nl)
assert '## Aprobación' not in raw_r.decode('utf-8')
with open(readme,'ab') as fh:fh.write((nl+section+nl).encode('utf-8'))
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp),changed_src=list(CHANGED_SRC),changed_bank=list(CHANGED_BANK)),indent=2))
