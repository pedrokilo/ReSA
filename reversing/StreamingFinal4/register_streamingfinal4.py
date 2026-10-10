"""Registro histórico de las 94 funciones de CStreaming del banco StreamingFinal4 (corrida 20261010-143008-897454). Re-audita en streaming la corrida, conserva la evidencia en runs/ y añade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'addr': {'CStreaming::AddEntity': '0x00409650',
          'CStreaming::AddImageToList': '0x00407610',
          'CStreaming::AddLodsToRequestList': '0x0040C520',
          'CStreaming::AddModelsToRequestList': '0x0040D3F0',
          'CStreaming::AddToLoadedVehiclesList': '0x00408000',
          'CStreaming::AreAnimsUsedByRequestedModels': '0x00407AD0',
          'CStreaming::AreTexturesUsedByRequestedModels': '0x00409A90',
          'CStreaming::ClearFlagForAll': '0x00407A40',
          'CStreaming::ClearSlots': '0x0040BAA0',
          'CStreaming::ConvertBufferToObject': '0x0040C6B0',
          'CStreaming::DeleteAllRwObjects': '0x004090A0',
          'CStreaming::DeleteLeastUsedEntityRwObject': '0x00409760',
          'CStreaming::DeleteRwObjectsAfterDeath': '0x00409210',
          'CStreaming::DeleteRwObjectsBehindCamera': '0x0040D7C0',
          'CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>': '0x00409940',
          'CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>': '0x00407A70',
          'CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>': '0x004099E0',
          'CStreaming::DisableCopBikes': '0x00407D10',
          'CStreaming::FindMIPedSlotForInterior': '0x00407FB0',
          'CStreaming::FinishLoadingLargeFile': '0x00408CB0',
          'CStreaming::FlushChannels': '0x0040E460',
          'CStreaming::FlushRequestList': '0x0040E4E0',
          'CStreaming::GetDefaultCabDriverModel': '0x00407D50',
          'CStreaming::GetDefaultCopCarModel': '0x00407C50',
          'CStreaming::GetDefaultCopModel': '0x00407C00',
          'CStreaming::GetDefaultFiremanModel': '0x00407D40',
          'CStreaming::GetDefaultMedicModel': '0x00407D20',
          'CStreaming::GetNextFileOnCd': '0x00408E20',
          'CStreaming::HasSpecialCharLoaded': '0x00407F00',
          'CStreaming::HasVehicleUpgradeLoaded': '0x00407820',
          'CStreaming::Init2': '0x005B8AD0',
          'CStreaming::InitImageList': '0x004083C0',
          'CStreaming::InstanceLoadedModels': '0x004084F0',
          'CStreaming::IsCarModelNeededInCurrentZone': '0x00407DD0',
          'CStreaming::IsVeryBusy': '0x004076A0',
          'CStreaming::Load': '0x005D29E0',
          'CStreaming::LoadAllRequestedModels': '0x0040EA10',
          'CStreaming::LoadCdDirectory-char': '0x005B6170',
          'CStreaming::LoadCdDirectory-void': '0x005B82C0',
          'CStreaming::LoadInitialPeds': '0x0040D3D0',
          'CStreaming::LoadInitialWeapons': '0x0040A120',
          'CStreaming::LoadRequestedModels': '0x0040E3A0',
          'CStreaming::LoadScene': '0x0040EB70',
          'CStreaming::LoadSceneCollision': '0x0040ED80',
          'CStreaming::LoadZoneVehicle': '0x0040B4B0',
          'CStreaming::MakeSpaceFor': '0x0040E120',
          'CStreaming::PossiblyStreamCarOutAfterCreation': '0x0040BA70',
          'CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-': '0x0040C270',
          'CStreaming::ProcessLoadingChannel': '0x0040E170',
          'CStreaming::PurgeRequestList': '0x0040C1E0',
          'CStreaming::ReInit': '0x0040E560',
          'CStreaming::ReadIniFile': '0x005BCCD0',
          'CStreaming::ReclassifyLoadedCars': '0x0040AFA0',
          'CStreaming::RemoveAllUnusedModels': '0x0040CF80',
          'CStreaming::RemoveBigBuildings': '0x004093B0',
          'CStreaming::RemoveBuildingsNotInArea': '0x004094B0',
          'CStreaming::RemoveCarModel': '0x004080F0',
          'CStreaming::RemoveCurrentZonesModels': '0x0040B080',
          'CStreaming::RemoveDodgyPedsFromRandomSlots': '0x0040BE60',
          'CStreaming::RemoveEntity': '0x00409710',
          'CStreaming::RemoveInappropriatePedModels': '0x0040B3A0',
          'CStreaming::RemoveLeastUsedModel': '0x0040CFD0',
          'CStreaming::RemoveLoadedVehicle': '0x0040C020',
          'CStreaming::RemoveLoadedZoneModel': '0x0040B340',
          'CStreaming::RemoveModel': '0x004089A0',
          'CStreaming::RemoveReferencedTxds': '0x0040D2F0',
          'CStreaming::RemoveTxdModel': '0x0040C180',
          'CStreaming::RenderEntity': '0x004096D0',
          'CStreaming::RequestBigBuildings': '0x00409430',
          'CStreaming::RequestFile': '0x0040A080',
          'CStreaming::RequestFilesInChannel': '0x00409050',
          'CStreaming::RequestModel': '0x004087E0',
          'CStreaming::RequestModelStream': '0x0040CBA0',
          'CStreaming::RequestSpecialChar': '0x0040B450',
          'CStreaming::RequestSpecialModel': '0x00409D10',
          'CStreaming::RequestTxdModel': '0x00407100',
          'CStreaming::RequestVehicleUpgrade': '0x00408C70',
          'CStreaming::RetryLoadFile': '0x004076C0',
          'CStreaming::Save': '0x005D29A0',
          'CStreaming::SetMissionDoesntRequireModel': '0x00409C90',
          'CStreaming::SetModelIsDeletable': '0x00409C10',
          'CStreaming::Shutdown': '0x004084B0',
          'CStreaming::StreamAmbulanceAndMedic': '0x0040A2A0',
          'CStreaming::StreamCopModels': '0x0040A150',
          'CStreaming::StreamFireEngineAndFireman': '0x0040A400',
          'CStreaming::StreamOneNewCar': '0x0040B4F0',
          'CStreaming::StreamPedsForInterior': '0x0040BBB0',
          'CStreaming::StreamPedsIntoRandomSlots': '0x0040BDA0',
          'CStreaming::StreamVehiclesAndPeds': '0x0040B700',
          'CStreaming::StreamVehiclesAndPeds_Always': '0x0040B650',
          'CStreaming::StreamZoneModels': '0x0040A560',
          'CStreaming::StreamZoneModels_Gangs': '0x0040AA10',
          'CStreaming::Update': '0x0040E670',
          'CStreaming::WeAreTryingToPhaseVehicleOut': '0x00407F80'},
 'allow_bank_change': [],
 'allow_src_change': [],
 'bad': {},
 'bank': 'StreamingFinal4',
 'common': 'Banco por pares sencillo dentro del juego completo con partida cargada (original con los hooks de la clase apagados '
           'frente a candidata con los hooks activos), 20992 pares de CPU aislados para las 94 funciones de CStreaming, TODOS '
           'los pares iguales, número de pares igual a la tabla del banco, passed=true global, evento "complete" natural con '
           'state_restored, saves sin cambios, configuraciones restauradas. Cada caso compara el retorno, la traza de '
           'colaboradores doblados con argumentos en bruto, el hash del mundo falso, el de la arena de operator new/delete, el '
           'de los globales guardados y los campos propios de cada función; la palabra de control x87 es de 24 bits; el '
           'generador de números aleatorios se sustituye por el del EXE; se ignoran los hilos ajenos. Los dobles con traza NO '
           'tienen crédito. Solo se valida la parte propia de cada función. Fuente probada == árbol de trabajo en el momento del '
           'registro (Streaming.cpp y Streaming.h coinciden con el manifest de compilación del probe). Evidencia histórica '
           'aprobada con límites explícitos; sin acreditación vigente de dependencias. Límites del banco (StreamingFinal4): '
           'mundo falso y determinista (model infos con vtable propia, pools de edificios, dummies, objetos y peatones, nodos de '
           'lista, pool de TXD, sectores, información de streaming con listas de cargados, pedidos y canales, grupos de '
           'población, zona, cámara, jugador y ajustes); dominio del fixture acotado al dominio real del EXE: cámara dentro de '
           '±2900, tamaños de modelo pedidos no nulos, enlaces de lista válidos y tabla de conductores de taxi real; CdStream, '
           'RenderWare, cargadores, almacenes (TXD/COL/IPL/animaciones/rutas/scripts), población, ficheros, directorio y memoria '
           'se sustituyen por dobles trazados en la dirección del EXE y en el símbolo de la DLL, sin crédito; cada global tocado '
           'se guarda y se restaura en cada ejecución; operator new/delete sobre una arena. Los casos en que el EXE lee pila sin '
           'inicializar quedan fuera del dominio del fixture (excepciones por lectura de pila sin inicializar del EXE); no se '
           'compara ese estado indeterminado. Consulte el README del banco para la lista completa de límites.',
 'error': None,
 'expect': {'CStreaming::AddEntity': 256,
            'CStreaming::AddImageToList': 256,
            'CStreaming::AddLodsToRequestList': 256,
            'CStreaming::AddModelsToRequestList': 256,
            'CStreaming::AddToLoadedVehiclesList': 256,
            'CStreaming::AreAnimsUsedByRequestedModels': 256,
            'CStreaming::AreTexturesUsedByRequestedModels': 256,
            'CStreaming::ClearFlagForAll': 128,
            'CStreaming::ClearSlots': 256,
            'CStreaming::ConvertBufferToObject': 512,
            'CStreaming::DeleteAllRwObjects': 128,
            'CStreaming::DeleteLeastUsedEntityRwObject': 256,
            'CStreaming::DeleteRwObjectsAfterDeath': 128,
            'CStreaming::DeleteRwObjectsBehindCamera': 256,
            'CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>': 256,
            'CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>': 256,
            'CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>': 256,
            'CStreaming::DisableCopBikes': 64,
            'CStreaming::FindMIPedSlotForInterior': 128,
            'CStreaming::FinishLoadingLargeFile': 256,
            'CStreaming::FlushChannels': 128,
            'CStreaming::FlushRequestList': 128,
            'CStreaming::GetDefaultCabDriverModel': 128,
            'CStreaming::GetDefaultCopCarModel': 256,
            'CStreaming::GetDefaultCopModel': 128,
            'CStreaming::GetDefaultFiremanModel': 64,
            'CStreaming::GetDefaultMedicModel': 64,
            'CStreaming::GetNextFileOnCd': 512,
            'CStreaming::HasSpecialCharLoaded': 64,
            'CStreaming::HasVehicleUpgradeLoaded': 256,
            'CStreaming::Init2': 64,
            'CStreaming::InitImageList': 64,
            'CStreaming::InstanceLoadedModels': 128,
            'CStreaming::IsCarModelNeededInCurrentZone': 512,
            'CStreaming::IsVeryBusy': 64,
            'CStreaming::Load': 64,
            'CStreaming::LoadAllRequestedModels': 256,
            'CStreaming::LoadCdDirectory-char': 512,
            'CStreaming::LoadCdDirectory-void': 128,
            'CStreaming::LoadInitialPeds': 64,
            'CStreaming::LoadInitialWeapons': 64,
            'CStreaming::LoadRequestedModels': 256,
            'CStreaming::LoadScene': 128,
            'CStreaming::LoadSceneCollision': 64,
            'CStreaming::LoadZoneVehicle': 128,
            'CStreaming::MakeSpaceFor': 256,
            'CStreaming::PossiblyStreamCarOutAfterCreation': 128,
            'CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-': 512,
            'CStreaming::ProcessLoadingChannel': 512,
            'CStreaming::PurgeRequestList': 128,
            'CStreaming::ReInit': 64,
            'CStreaming::ReadIniFile': 256,
            'CStreaming::ReclassifyLoadedCars': 128,
            'CStreaming::RemoveAllUnusedModels': 128,
            'CStreaming::RemoveBigBuildings': 128,
            'CStreaming::RemoveBuildingsNotInArea': 256,
            'CStreaming::RemoveCarModel': 256,
            'CStreaming::RemoveCurrentZonesModels': 128,
            'CStreaming::RemoveDodgyPedsFromRandomSlots': 128,
            'CStreaming::RemoveEntity': 256,
            'CStreaming::RemoveInappropriatePedModels': 128,
            'CStreaming::RemoveLeastUsedModel': 256,
            'CStreaming::RemoveLoadedVehicle': 256,
            'CStreaming::RemoveLoadedZoneModel': 128,
            'CStreaming::RemoveModel': 512,
            'CStreaming::RemoveReferencedTxds': 128,
            'CStreaming::RemoveTxdModel': 128,
            'CStreaming::RenderEntity': 256,
            'CStreaming::RequestBigBuildings': 128,
            'CStreaming::RequestFile': 256,
            'CStreaming::RequestFilesInChannel': 128,
            'CStreaming::RequestModel': 512,
            'CStreaming::RequestModelStream': 512,
            'CStreaming::RequestSpecialChar': 256,
            'CStreaming::RequestSpecialModel': 512,
            'CStreaming::RequestTxdModel': 256,
            'CStreaming::RequestVehicleUpgrade': 128,
            'CStreaming::RetryLoadFile': 256,
            'CStreaming::Save': 64,
            'CStreaming::SetMissionDoesntRequireModel': 256,
            'CStreaming::SetModelIsDeletable': 256,
            'CStreaming::Shutdown': 64,
            'CStreaming::StreamAmbulanceAndMedic': 128,
            'CStreaming::StreamCopModels': 256,
            'CStreaming::StreamFireEngineAndFireman': 128,
            'CStreaming::StreamOneNewCar': 512,
            'CStreaming::StreamPedsForInterior': 256,
            'CStreaming::StreamPedsIntoRandomSlots': 256,
            'CStreaming::StreamVehiclesAndPeds': 256,
            'CStreaming::StreamVehiclesAndPeds_Always': 256,
            'CStreaming::StreamZoneModels': 512,
            'CStreaming::StreamZoneModels_Gangs': 512,
            'CStreaming::Update': 256,
            'CStreaming::WeAreTryingToPhaseVehicleOut': 128},
 'extra_unregistered': [],
 'new': ['CStreaming::AddEntity',
         'CStreaming::AddImageToList',
         'CStreaming::AddLodsToRequestList',
         'CStreaming::AddModelsToRequestList',
         'CStreaming::AddToLoadedVehiclesList',
         'CStreaming::AreAnimsUsedByRequestedModels',
         'CStreaming::AreTexturesUsedByRequestedModels',
         'CStreaming::ClearFlagForAll',
         'CStreaming::ClearSlots',
         'CStreaming::ConvertBufferToObject',
         'CStreaming::DeleteAllRwObjects',
         'CStreaming::DeleteLeastUsedEntityRwObject',
         'CStreaming::DeleteRwObjectsAfterDeath',
         'CStreaming::DeleteRwObjectsBehindCamera',
         'CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>',
         'CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>',
         'CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>',
         'CStreaming::DisableCopBikes',
         'CStreaming::FindMIPedSlotForInterior',
         'CStreaming::FinishLoadingLargeFile',
         'CStreaming::FlushChannels',
         'CStreaming::FlushRequestList',
         'CStreaming::GetDefaultCabDriverModel',
         'CStreaming::GetDefaultCopCarModel',
         'CStreaming::GetDefaultCopModel',
         'CStreaming::GetDefaultFiremanModel',
         'CStreaming::GetDefaultMedicModel',
         'CStreaming::GetNextFileOnCd',
         'CStreaming::HasSpecialCharLoaded',
         'CStreaming::HasVehicleUpgradeLoaded',
         'CStreaming::Init2',
         'CStreaming::InitImageList',
         'CStreaming::InstanceLoadedModels',
         'CStreaming::IsCarModelNeededInCurrentZone',
         'CStreaming::IsVeryBusy',
         'CStreaming::Load',
         'CStreaming::LoadAllRequestedModels',
         'CStreaming::LoadCdDirectory-char',
         'CStreaming::LoadCdDirectory-void',
         'CStreaming::LoadInitialPeds',
         'CStreaming::LoadInitialWeapons',
         'CStreaming::LoadRequestedModels',
         'CStreaming::LoadScene',
         'CStreaming::LoadSceneCollision',
         'CStreaming::LoadZoneVehicle',
         'CStreaming::MakeSpaceFor',
         'CStreaming::PossiblyStreamCarOutAfterCreation',
         'CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-',
         'CStreaming::ProcessLoadingChannel',
         'CStreaming::PurgeRequestList',
         'CStreaming::ReInit',
         'CStreaming::ReadIniFile',
         'CStreaming::ReclassifyLoadedCars',
         'CStreaming::RemoveAllUnusedModels',
         'CStreaming::RemoveBigBuildings',
         'CStreaming::RemoveBuildingsNotInArea',
         'CStreaming::RemoveCarModel',
         'CStreaming::RemoveCurrentZonesModels',
         'CStreaming::RemoveDodgyPedsFromRandomSlots',
         'CStreaming::RemoveEntity',
         'CStreaming::RemoveInappropriatePedModels',
         'CStreaming::RemoveLeastUsedModel',
         'CStreaming::RemoveLoadedVehicle',
         'CStreaming::RemoveLoadedZoneModel',
         'CStreaming::RemoveModel',
         'CStreaming::RemoveReferencedTxds',
         'CStreaming::RemoveTxdModel',
         'CStreaming::RenderEntity',
         'CStreaming::RequestBigBuildings',
         'CStreaming::RequestFile',
         'CStreaming::RequestFilesInChannel',
         'CStreaming::RequestModel',
         'CStreaming::RequestModelStream',
         'CStreaming::RequestSpecialChar',
         'CStreaming::RequestSpecialModel',
         'CStreaming::RequestTxdModel',
         'CStreaming::RequestVehicleUpgrade',
         'CStreaming::RetryLoadFile',
         'CStreaming::Save',
         'CStreaming::SetMissionDoesntRequireModel',
         'CStreaming::SetModelIsDeletable',
         'CStreaming::Shutdown',
         'CStreaming::StreamAmbulanceAndMedic',
         'CStreaming::StreamCopModels',
         'CStreaming::StreamFireEngineAndFireman',
         'CStreaming::StreamOneNewCar',
         'CStreaming::StreamPedsForInterior',
         'CStreaming::StreamPedsIntoRandomSlots',
         'CStreaming::StreamVehiclesAndPeds',
         'CStreaming::StreamVehiclesAndPeds_Always',
         'CStreaming::StreamZoneModels',
         'CStreaming::StreamZoneModels_Gangs',
         'CStreaming::Update',
         'CStreaming::WeAreTryingToPhaseVehicleOut'],
 'notes': 'StreamingFinal4 registra las 94 funciones de CStreaming (20992 pares, 0 diferencias). Los límites se declaran por '
          'función; véanse function_scope y el README del banco. No quedan funciones pendientes en CStreaming.',
 'passed': True,
 'pending_after': [],
 'probe_prefix': '3588d002',
 'provenance': 'Probe StreamingFinal4 compilado exacto (hashes de ASI/PDB en build-manifest.json); fuentes de la clase '
               '(Streaming.cpp y Streaming.h) con hash del manifest de compilación e iguales al árbol de trabajo en el registro; '
               'log completo de 20992 filas reauditado offline en streaming y conservado comprimido con gzip con el hash '
               'original.',
 'readme_section': '## Aprobación (10/10/2026)\n'
                   '\n'
                   'Corrida `20261010-143008-897454`: passed=true, 20992 pares, 94 funciones registradas con 0 fallos, evento '
                   'complete (state_restored), saves y configuraciones intactos. Registrada con `register_streamingfinal4.py` '
                   '(evidencia en `runs/`, `validation-summary.json`). Con este banco CStreaming queda sin pendientes.\n'
                   '\n'
                   '### Alcance y límites\n'
                   '- Dominio del fixture: cámara dentro de ±2900, tamaños de modelo pedidos no nulos, enlaces de lista válidos '
                   'y tabla de conductores de taxi real.\n'
                   '- Excepciones por lectura de pila sin inicializar del EXE: quedan fuera del dominio comparado.\n'
                   '- Mundo falso y determinista; CdStream, RenderWare, cargadores, almacenes, población, ficheros, directorio y '
                   'memoria doblados con traza sin crédito.\n'
                   '- Aprobación histórica con `dependency_review=null`; sin acreditación vigente de dependencias.\n'
                   '\n'
                   '### Funciones (94)\n'
                   '| Función | Pares |\n'
                   '| --- | ---: |\n'
                   '| CStreaming::AddEntity | 256 |\n'
                   '| CStreaming::AddImageToList | 256 |\n'
                   '| CStreaming::AddLodsToRequestList | 256 |\n'
                   '| CStreaming::AddModelsToRequestList | 256 |\n'
                   '| CStreaming::AddToLoadedVehiclesList | 256 |\n'
                   '| CStreaming::AreAnimsUsedByRequestedModels | 256 |\n'
                   '| CStreaming::AreTexturesUsedByRequestedModels | 256 |\n'
                   '| CStreaming::ClearFlagForAll | 128 |\n'
                   '| CStreaming::ClearSlots | 256 |\n'
                   '| CStreaming::ConvertBufferToObject | 512 |\n'
                   '| CStreaming::DeleteAllRwObjects | 128 |\n'
                   '| CStreaming::DeleteLeastUsedEntityRwObject | 256 |\n'
                   '| CStreaming::DeleteRwObjectsAfterDeath | 128 |\n'
                   '| CStreaming::DeleteRwObjectsBehindCamera | 256 |\n'
                   '| CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>> | 256 |\n'
                   '| CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>> | 256 |\n'
                   '| CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>> | 256 |\n'
                   '| CStreaming::DisableCopBikes | 64 |\n'
                   '| CStreaming::FindMIPedSlotForInterior | 128 |\n'
                   '| CStreaming::FinishLoadingLargeFile | 256 |\n'
                   '| CStreaming::FlushChannels | 128 |\n'
                   '| CStreaming::FlushRequestList | 128 |\n'
                   '| CStreaming::GetDefaultCabDriverModel | 128 |\n'
                   '| CStreaming::GetDefaultCopCarModel | 256 |\n'
                   '| CStreaming::GetDefaultCopModel | 128 |\n'
                   '| CStreaming::GetDefaultFiremanModel | 64 |\n'
                   '| CStreaming::GetDefaultMedicModel | 64 |\n'
                   '| CStreaming::GetNextFileOnCd | 512 |\n'
                   '| CStreaming::HasSpecialCharLoaded | 64 |\n'
                   '| CStreaming::HasVehicleUpgradeLoaded | 256 |\n'
                   '| CStreaming::Init2 | 64 |\n'
                   '| CStreaming::InitImageList | 64 |\n'
                   '| CStreaming::InstanceLoadedModels | 128 |\n'
                   '| CStreaming::IsCarModelNeededInCurrentZone | 512 |\n'
                   '| CStreaming::IsVeryBusy | 64 |\n'
                   '| CStreaming::Load | 64 |\n'
                   '| CStreaming::LoadAllRequestedModels | 256 |\n'
                   '| CStreaming::LoadCdDirectory-char | 512 |\n'
                   '| CStreaming::LoadCdDirectory-void | 128 |\n'
                   '| CStreaming::LoadInitialPeds | 64 |\n'
                   '| CStreaming::LoadInitialWeapons | 64 |\n'
                   '| CStreaming::LoadRequestedModels | 256 |\n'
                   '| CStreaming::LoadScene | 128 |\n'
                   '| CStreaming::LoadSceneCollision | 64 |\n'
                   '| CStreaming::LoadZoneVehicle | 128 |\n'
                   '| CStreaming::MakeSpaceFor | 256 |\n'
                   '| CStreaming::PossiblyStreamCarOutAfterCreation | 128 |\n'
                   '| CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>- | 512 |\n'
                   '| CStreaming::ProcessLoadingChannel | 512 |\n'
                   '| CStreaming::PurgeRequestList | 128 |\n'
                   '| CStreaming::ReInit | 64 |\n'
                   '| CStreaming::ReadIniFile | 256 |\n'
                   '| CStreaming::ReclassifyLoadedCars | 128 |\n'
                   '| CStreaming::RemoveAllUnusedModels | 128 |\n'
                   '| CStreaming::RemoveBigBuildings | 128 |\n'
                   '| CStreaming::RemoveBuildingsNotInArea | 256 |\n'
                   '| CStreaming::RemoveCarModel | 256 |\n'
                   '| CStreaming::RemoveCurrentZonesModels | 128 |\n'
                   '| CStreaming::RemoveDodgyPedsFromRandomSlots | 128 |\n'
                   '| CStreaming::RemoveEntity | 256 |\n'
                   '| CStreaming::RemoveInappropriatePedModels | 128 |\n'
                   '| CStreaming::RemoveLeastUsedModel | 256 |\n'
                   '| CStreaming::RemoveLoadedVehicle | 256 |\n'
                   '| CStreaming::RemoveLoadedZoneModel | 128 |\n'
                   '| CStreaming::RemoveModel | 512 |\n'
                   '| CStreaming::RemoveReferencedTxds | 128 |\n'
                   '| CStreaming::RemoveTxdModel | 128 |\n'
                   '| CStreaming::RenderEntity | 256 |\n'
                   '| CStreaming::RequestBigBuildings | 128 |\n'
                   '| CStreaming::RequestFile | 256 |\n'
                   '| CStreaming::RequestFilesInChannel | 128 |\n'
                   '| CStreaming::RequestModel | 512 |\n'
                   '| CStreaming::RequestModelStream | 512 |\n'
                   '| CStreaming::RequestSpecialChar | 256 |\n'
                   '| CStreaming::RequestSpecialModel | 512 |\n'
                   '| CStreaming::RequestTxdModel | 256 |\n'
                   '| CStreaming::RequestVehicleUpgrade | 128 |\n'
                   '| CStreaming::RetryLoadFile | 256 |\n'
                   '| CStreaming::Save | 64 |\n'
                   '| CStreaming::SetMissionDoesntRequireModel | 256 |\n'
                   '| CStreaming::SetModelIsDeletable | 256 |\n'
                   '| CStreaming::Shutdown | 64 |\n'
                   '| CStreaming::StreamAmbulanceAndMedic | 128 |\n'
                   '| CStreaming::StreamCopModels | 256 |\n'
                   '| CStreaming::StreamFireEngineAndFireman | 128 |\n'
                   '| CStreaming::StreamOneNewCar | 512 |\n'
                   '| CStreaming::StreamPedsForInterior | 256 |\n'
                   '| CStreaming::StreamPedsIntoRandomSlots | 256 |\n'
                   '| CStreaming::StreamVehiclesAndPeds | 256 |\n'
                   '| CStreaming::StreamVehiclesAndPeds_Always | 256 |\n'
                   '| CStreaming::StreamZoneModels | 512 |\n'
                   '| CStreaming::StreamZoneModels_Gangs | 512 |\n'
                   '| CStreaming::Update | 256 |\n'
                   '| CStreaming::WeAreTryingToPhaseVehicleOut | 128 |\n',
 'registry_before': 8249,
 'rejected': [],
 'run_id': '20261010-143008-897454',
 'source_changes': {},
 'specific': {'CStreaming::AddEntity': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::AddImageToList': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                            'crédito); ',
              'CStreaming::AddLodsToRequestList': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::AddModelsToRequestList': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza '
                                                    '(sin crédito); ',
              'CStreaming::AddToLoadedVehiclesList': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza '
                                                     '(sin crédito); ',
              'CStreaming::AreAnimsUsedByRequestedModels': 'Corpus 256; mundo falso de streaming y colaboradores doblados con '
                                                           'traza (sin crédito); ',
              'CStreaming::AreTexturesUsedByRequestedModels': 'Corpus 256; mundo falso de streaming y colaboradores doblados con '
                                                              'traza (sin crédito); ',
              'CStreaming::ClearFlagForAll': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::ClearSlots': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::ConvertBufferToObject': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::DeleteAllRwObjects': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                'crédito); ',
              'CStreaming::DeleteLeastUsedEntityRwObject': 'Corpus 256; mundo falso de streaming y colaboradores doblados con '
                                                           'traza (sin crédito); ',
              'CStreaming::DeleteRwObjectsAfterDeath': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza '
                                                       '(sin crédito); ',
              'CStreaming::DeleteRwObjectsBehindCamera': 'Corpus 256; mundo falso de streaming y colaboradores doblados con '
                                                         'traza (sin crédito); ',
              'CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>': 'Corpus 256; mundo falso de '
                                                                                                   'streaming y colaboradores '
                                                                                                   'doblados con traza (sin '
                                                                                                   'crédito); ',
              'CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>': 'Corpus 256; mundo falso de streaming y '
                                                                                       'colaboradores doblados con traza (sin '
                                                                                       'crédito); ',
              'CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>': 'Corpus 256; mundo falso de '
                                                                                                   'streaming y colaboradores '
                                                                                                   'doblados con traza (sin '
                                                                                                   'crédito); ',
              'CStreaming::DisableCopBikes': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::FindMIPedSlotForInterior': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza '
                                                      '(sin crédito); ',
              'CStreaming::FinishLoadingLargeFile': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza '
                                                    '(sin crédito); ',
              'CStreaming::FlushChannels': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                           'crédito); ',
              'CStreaming::FlushRequestList': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                              'crédito); ',
              'CStreaming::GetDefaultCabDriverModel': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza '
                                                      '(sin crédito); ',
              'CStreaming::GetDefaultCopCarModel': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::GetDefaultCopModel': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                'crédito); ',
              'CStreaming::GetDefaultFiremanModel': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                    'crédito); ',
              'CStreaming::GetDefaultMedicModel': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::GetNextFileOnCd': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::HasSpecialCharLoaded': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::HasVehicleUpgradeLoaded': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza '
                                                     '(sin crédito); ',
              'CStreaming::Init2': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::InitImageList': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                           'crédito); ',
              'CStreaming::InstanceLoadedModels': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::IsCarModelNeededInCurrentZone': 'Corpus 512; mundo falso de streaming y colaboradores doblados con '
                                                           'traza (sin crédito); ',
              'CStreaming::IsVeryBusy': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::Load': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::LoadAllRequestedModels': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza '
                                                    '(sin crédito); ',
              'CStreaming::LoadCdDirectory-char': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::LoadCdDirectory-void': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::LoadInitialPeds': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::LoadInitialWeapons': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                'crédito); ',
              'CStreaming::LoadRequestedModels': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                 'crédito); ',
              'CStreaming::LoadScene': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::LoadSceneCollision': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                'crédito); ',
              'CStreaming::LoadZoneVehicle': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::MakeSpaceFor': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                          'crédito); ',
              'CStreaming::PossiblyStreamCarOutAfterCreation': 'Corpus 128; mundo falso de streaming y colaboradores doblados '
                                                               'con traza (sin crédito); ',
              'CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-': 'Corpus 512; mundo falso de streaming y '
                                                                                        'colaboradores doblados con traza (sin '
                                                                                        'crédito); ',
              'CStreaming::ProcessLoadingChannel': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::PurgeRequestList': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                              'crédito); ',
              'CStreaming::ReInit': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::ReadIniFile': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                         'crédito); ',
              'CStreaming::ReclassifyLoadedCars': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::RemoveAllUnusedModels': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::RemoveBigBuildings': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                'crédito); ',
              'CStreaming::RemoveBuildingsNotInArea': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza '
                                                      '(sin crédito); ',
              'CStreaming::RemoveCarModel': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                            'crédito); ',
              'CStreaming::RemoveCurrentZonesModels': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza '
                                                      '(sin crédito); ',
              'CStreaming::RemoveDodgyPedsFromRandomSlots': 'Corpus 128; mundo falso de streaming y colaboradores doblados con '
                                                            'traza (sin crédito); ',
              'CStreaming::RemoveEntity': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                          'crédito); ',
              'CStreaming::RemoveInappropriatePedModels': 'Corpus 128; mundo falso de streaming y colaboradores doblados con '
                                                          'traza (sin crédito); ',
              'CStreaming::RemoveLeastUsedModel': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::RemoveLoadedVehicle': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                 'crédito); ',
              'CStreaming::RemoveLoadedZoneModel': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::RemoveModel': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                         'crédito); ',
              'CStreaming::RemoveReferencedTxds': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                  'crédito); ',
              'CStreaming::RemoveTxdModel': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                            'crédito); ',
              'CStreaming::RenderEntity': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                          'crédito); ',
              'CStreaming::RequestBigBuildings': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                 'crédito); ',
              'CStreaming::RequestFile': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                         'crédito); ',
              'CStreaming::RequestFilesInChannel': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::RequestModel': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                          'crédito); ',
              'CStreaming::RequestModelStream': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                'crédito); ',
              'CStreaming::RequestSpecialChar': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                'crédito); ',
              'CStreaming::RequestSpecialModel': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                 'crédito); ',
              'CStreaming::RequestTxdModel': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::RequestVehicleUpgrade': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::RetryLoadFile': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                           'crédito); ',
              'CStreaming::Save': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::SetMissionDoesntRequireModel': 'Corpus 256; mundo falso de streaming y colaboradores doblados con '
                                                          'traza (sin crédito); ',
              'CStreaming::SetModelIsDeletable': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                 'crédito); ',
              'CStreaming::Shutdown': 'Corpus 64; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::StreamAmbulanceAndMedic': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza '
                                                     '(sin crédito); ',
              'CStreaming::StreamCopModels': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::StreamFireEngineAndFireman': 'Corpus 128; mundo falso de streaming y colaboradores doblados con traza '
                                                        '(sin crédito); ',
              'CStreaming::StreamOneNewCar': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                             'crédito); ',
              'CStreaming::StreamPedsForInterior': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::StreamPedsIntoRandomSlots': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza '
                                                       '(sin crédito); ',
              'CStreaming::StreamVehiclesAndPeds': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin '
                                                   'crédito); ',
              'CStreaming::StreamVehiclesAndPeds_Always': 'Corpus 256; mundo falso de streaming y colaboradores doblados con '
                                                          'traza (sin crédito); ',
              'CStreaming::StreamZoneModels': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza (sin '
                                              'crédito); ',
              'CStreaming::StreamZoneModels_Gangs': 'Corpus 512; mundo falso de streaming y colaboradores doblados con traza '
                                                    '(sin crédito); ',
              'CStreaming::Update': 'Corpus 256; mundo falso de streaming y colaboradores doblados con traza (sin crédito); ',
              'CStreaming::WeAreTryingToPhaseVehicleOut': 'Corpus 128; mundo falso de streaming y colaboradores doblados con '
                                                          'traza (sin crédito); '},
 'tag': 'streamingfinal4',
 'total': 20992,
 'types': {'CStreaming::AddEntity': 'S',
           'CStreaming::AddImageToList': 'S',
           'CStreaming::AddLodsToRequestList': 'S',
           'CStreaming::AddModelsToRequestList': 'S',
           'CStreaming::AddToLoadedVehiclesList': 'S',
           'CStreaming::AreAnimsUsedByRequestedModels': 'S',
           'CStreaming::AreTexturesUsedByRequestedModels': 'S',
           'CStreaming::ClearFlagForAll': 'S',
           'CStreaming::ClearSlots': 'S',
           'CStreaming::ConvertBufferToObject': 'S',
           'CStreaming::DeleteAllRwObjects': 'S',
           'CStreaming::DeleteLeastUsedEntityRwObject': 'S',
           'CStreaming::DeleteRwObjectsAfterDeath': 'S',
           'CStreaming::DeleteRwObjectsBehindCamera': 'S',
           'CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>': 'S',
           'CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>': 'S',
           'CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>': 'S',
           'CStreaming::DisableCopBikes': 'S',
           'CStreaming::FindMIPedSlotForInterior': 'S',
           'CStreaming::FinishLoadingLargeFile': 'S',
           'CStreaming::FlushChannels': 'S',
           'CStreaming::FlushRequestList': 'S',
           'CStreaming::GetDefaultCabDriverModel': 'S',
           'CStreaming::GetDefaultCopCarModel': 'S',
           'CStreaming::GetDefaultCopModel': 'S',
           'CStreaming::GetDefaultFiremanModel': 'S',
           'CStreaming::GetDefaultMedicModel': 'S',
           'CStreaming::GetNextFileOnCd': 'S',
           'CStreaming::HasSpecialCharLoaded': 'S',
           'CStreaming::HasVehicleUpgradeLoaded': 'S',
           'CStreaming::Init2': 'S',
           'CStreaming::InitImageList': 'S',
           'CStreaming::InstanceLoadedModels': 'S',
           'CStreaming::IsCarModelNeededInCurrentZone': 'S',
           'CStreaming::IsVeryBusy': 'S',
           'CStreaming::Load': 'S',
           'CStreaming::LoadAllRequestedModels': 'S',
           'CStreaming::LoadCdDirectory-char': 'S',
           'CStreaming::LoadCdDirectory-void': 'S',
           'CStreaming::LoadInitialPeds': 'S',
           'CStreaming::LoadInitialWeapons': 'S',
           'CStreaming::LoadRequestedModels': 'S',
           'CStreaming::LoadScene': 'S',
           'CStreaming::LoadSceneCollision': 'S',
           'CStreaming::LoadZoneVehicle': 'S',
           'CStreaming::MakeSpaceFor': 'S',
           'CStreaming::PossiblyStreamCarOutAfterCreation': 'S',
           'CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-': 'S',
           'CStreaming::ProcessLoadingChannel': 'S',
           'CStreaming::PurgeRequestList': 'S',
           'CStreaming::ReInit': 'S',
           'CStreaming::ReadIniFile': 'S',
           'CStreaming::ReclassifyLoadedCars': 'S',
           'CStreaming::RemoveAllUnusedModels': 'S',
           'CStreaming::RemoveBigBuildings': 'S',
           'CStreaming::RemoveBuildingsNotInArea': 'S',
           'CStreaming::RemoveCarModel': 'S',
           'CStreaming::RemoveCurrentZonesModels': 'S',
           'CStreaming::RemoveDodgyPedsFromRandomSlots': 'S',
           'CStreaming::RemoveEntity': 'S',
           'CStreaming::RemoveInappropriatePedModels': 'S',
           'CStreaming::RemoveLeastUsedModel': 'S',
           'CStreaming::RemoveLoadedVehicle': 'S',
           'CStreaming::RemoveLoadedZoneModel': 'S',
           'CStreaming::RemoveModel': 'S',
           'CStreaming::RemoveReferencedTxds': 'S',
           'CStreaming::RemoveTxdModel': 'S',
           'CStreaming::RenderEntity': 'S',
           'CStreaming::RequestBigBuildings': 'S',
           'CStreaming::RequestFile': 'S',
           'CStreaming::RequestFilesInChannel': 'S',
           'CStreaming::RequestModel': 'S',
           'CStreaming::RequestModelStream': 'S',
           'CStreaming::RequestSpecialChar': 'S',
           'CStreaming::RequestSpecialModel': 'S',
           'CStreaming::RequestTxdModel': 'S',
           'CStreaming::RequestVehicleUpgrade': 'S',
           'CStreaming::RetryLoadFile': 'S',
           'CStreaming::Save': 'S',
           'CStreaming::SetMissionDoesntRequireModel': 'S',
           'CStreaming::SetModelIsDeletable': 'S',
           'CStreaming::Shutdown': 'S',
           'CStreaming::StreamAmbulanceAndMedic': 'S',
           'CStreaming::StreamCopModels': 'S',
           'CStreaming::StreamFireEngineAndFireman': 'S',
           'CStreaming::StreamOneNewCar': 'S',
           'CStreaming::StreamPedsForInterior': 'S',
           'CStreaming::StreamPedsIntoRandomSlots': 'S',
           'CStreaming::StreamVehiclesAndPeds': 'S',
           'CStreaming::StreamVehiclesAndPeds_Always': 'S',
           'CStreaming::StreamZoneModels': 'S',
           'CStreaming::StreamZoneModels_Gangs': 'S',
           'CStreaming::Update': 'S',
           'CStreaming::WeAreTryingToPhaseVehicleOut': 'S'}}
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
