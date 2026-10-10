"""Registro histórico de las 84 funciones de CVisibilityPlugins del banco VisPluginsFinal4 (corrida 20261010-130234-135622). Re-audita en streaming la corrida, conserva la evidencia en runs/ y añade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'addr': {'CVisibilityPlugins::AddWeaponPedForPC': '0x005E46D0',
          'CVisibilityPlugins::AtomicCopyConstructor': '0x00732170',
          'CVisibilityPlugins::AtomicDestructor': '0x007321A0',
          'CVisibilityPlugins::CalculateFadingAtomicAlpha': '0x00732500',
          'CVisibilityPlugins::ClearAtomicFlag': '0x00732310',
          'CVisibilityPlugins::ClearClumpForAllAtomicsFlag': '0x00732350',
          'CVisibilityPlugins::ClumpConstructor': '0x00732E10',
          'CVisibilityPlugins::ClumpCopyConstructor': '0x00732200',
          'CVisibilityPlugins::ClumpDestructor': '0x00732220',
          'CVisibilityPlugins::DefaultVisibilityCB': '0x00732A30',
          'CVisibilityPlugins::FrameConstructor': '0x007321B0',
          'CVisibilityPlugins::FrameCopyConstructor': '0x007321D0',
          'CVisibilityPlugins::FrameDestructor': '0x007321F0',
          'CVisibilityPlugins::FrustumSphereCB': '0x00732A40',
          'CVisibilityPlugins::GetAtomicId': '0x00732370',
          'CVisibilityPlugins::GetClumpAlpha': '0x00732B20',
          'CVisibilityPlugins::GetClumpModelInfo': '0x00732AC0',
          'CVisibilityPlugins::GetDistanceSquaredFromCamera-Frame': '0x00732C80',
          'CVisibilityPlugins::GetDistanceSquaredFromCamera-RwV3d': '0x00732CC0',
          'CVisibilityPlugins::GetDotProductWithCameraVector': '0x007326D0',
          'CVisibilityPlugins::GetFrameHierarchyId': '0x00732A20',
          'CVisibilityPlugins::GetModelInfo': '0x00732260',
          'CVisibilityPlugins::GetModelInfoIndex': '0x00732250',
          'CVisibilityPlugins::GetUserValue': '0x007323A0',
          'CVisibilityPlugins::InitAlphaAtomicList': '0x00734530',
          'CVisibilityPlugins::InitAlphaEntityList': '0x00734540',
          'CVisibilityPlugins::Initialise': '0x00733A20',
          'CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList': '0x00733E10',
          'CVisibilityPlugins::InsertEntityIntoReallyDrawLastList': '0x00733E50',
          'CVisibilityPlugins::InsertEntityIntoSortedList': '0x00734570',
          'CVisibilityPlugins::InsertEntityIntoUnderwaterList': '0x00733D90',
          'CVisibilityPlugins::InsertObjectIntoSortedList': '0x00733DD0',
          'CVisibilityPlugins::IsAtomicVisible': '0x00732990',
          'CVisibilityPlugins::IsClumpVisible': '0x00732AE0',
          'CVisibilityPlugins::PluginAttach': '0x00732E30',
          'CVisibilityPlugins::RenderAlphaAtomics': '0x00733E90',
          'CVisibilityPlugins::RenderAtomicWithAlpha': '0x00732480',
          'CVisibilityPlugins::RenderAtomicWithAlphaCB': '0x00732660',
          'CVisibilityPlugins::RenderBoatAlphaAtomics': '0x00733EC0',
          'CVisibilityPlugins::RenderEntity': '0x00732B40',
          'CVisibilityPlugins::RenderFadingAtomic': '0x00732610',
          'CVisibilityPlugins::RenderFadingClump': '0x00732680',
          'CVisibilityPlugins::RenderFadingClumpCB': '0x00733630',
          'CVisibilityPlugins::RenderFadingEntities': '0x00733F10',
          'CVisibilityPlugins::RenderFadingUnderwaterEntities': '0x007337D0',
          'CVisibilityPlugins::RenderHeliRotorAlphaCB': '0x007340B0',
          'CVisibilityPlugins::RenderHeliTailRotorAlphaCB': '0x00734170',
          'CVisibilityPlugins::RenderObjNormalAtomic': '0x007323E0',
          'CVisibilityPlugins::RenderOrderedList': '0x007337A0',
          'CVisibilityPlugins::RenderPedCB': '0x007335B0',
          'CVisibilityPlugins::RenderPlayerCB': '0x00732870',
          'CVisibilityPlugins::RenderReallyDrawLastObjects': '0x00733800',
          'CVisibilityPlugins::RenderTrainHiDetailAlphaCB': '0x00734240',
          'CVisibilityPlugins::RenderTrainHiDetailCB': '0x00733330',
          'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB': '0x00733F80',
          'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle': '0x00734370',
          'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat': '0x007344A0',
          'CVisibilityPlugins::RenderVehicleHiDetailCB': '0x00733240',
          'CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle': '0x00733420',
          'CVisibilityPlugins::RenderVehicleHiDetailCB_Boat': '0x00733550',
          'CVisibilityPlugins::RenderVehicleLoDetailCB_Boat': '0x007334F0',
          'CVisibilityPlugins::RenderVehicleReallyLowDetailCB': '0x007331E0',
          'CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle': '0x00732820',
          'CVisibilityPlugins::RenderWeaponCB': '0x00733670',
          'CVisibilityPlugins::RenderWeaponPedsForPC': '0x00732F30',
          'CVisibilityPlugins::RenderWheelAtomicCB': '0x007323C0',
          'CVisibilityPlugins::SetAlphaCB': '0x007323B0',
          'CVisibilityPlugins::SetAtomicFlag': '0x007322B0',
          'CVisibilityPlugins::SetAtomicId': '0x00732290',
          'CVisibilityPlugins::SetAtomicRenderCallback': '0x007328A0',
          'CVisibilityPlugins::SetClumpAlpha': '0x00732B00',
          'CVisibilityPlugins::SetClumpForAllAtomicsFlag': '0x007322F0',
          'CVisibilityPlugins::SetClumpModelInfo': '0x00733750',
          'CVisibilityPlugins::SetFrameHierarchyId': '0x00732A00',
          'CVisibilityPlugins::SetModelInfoIndex': '0x00732230',
          'CVisibilityPlugins::SetRenderWareCamera': '0x007328C0',
          'CVisibilityPlugins::SetTextureCB': '0x00732850',
          'CVisibilityPlugins::SetUserValue': '0x00732380',
          'CVisibilityPlugins::SetupVehicleVariables': '0x00733160',
          'CVisibilityPlugins::Shutdown': '0x00732EB0',
          'CVisibilityPlugins::VehicleVisibilityCB': '0x007336F0',
          'CVisibilityPlugins::VehicleVisibilityCB_BigVehicle': '0x00732AB0',
          'CVisibilityPlugins::clearClumpForAllAtomicsFlagCB': '0x00732330',
          'CVisibilityPlugins::setClumpForAllAtomicsFlagCB': '0x007322D0'},
 'allow_bank_change': [],
 'allow_src_change': [],
 'bad': {},
 'bank': 'VisPluginsFinal4',
 'common': 'Banco por pares sencillo dentro del juego completo con partida cargada (original con los hooks de la clase apagados '
           'frente a candidata con los hooks activos), 18176 pares de CPU aislados para las 84 funciones de CVisibilityPlugins, '
           'TODOS los pares iguales, número de pares igual a la tabla del banco, passed=true global, evento "complete" natural '
           'con state_restored y layout_ok, saves sin cambios, configuraciones restauradas. Cada caso compara el retorno, la '
           'traza de colaboradores doblados con argumentos en bruto, el hash del mundo falso, el de la arena de operator '
           'new/delete, el de los globales guardados y los campos propios de cada función; la palabra de control x87 es de 24 '
           'bits; se ignoran los hilos ajenos. Los dobles con traza NO tienen crédito. Solo se valida la parte propia de cada '
           'función. Fuente probada == árbol de trabajo en el momento del registro (las fuentes de la clase coinciden con el '
           'manifest de compilación del probe). Evidencia histórica aprobada con límites explícitos; sin acreditación vigente de '
           'dependencias. Límites del banco (VisPluginsFinal4): mundo falso estático (atomics, frames, clumps, geometrías con '
           'tablas de materiales, texturas, model infos con vtable propia, CColModel, entidades, peds con arma, CWeaponInfo, '
           'jerarquía de animación y cámara RW); se guardan y restauran las variables y listas de la clase (0xC88000..0xC88260), '
           'los offsets de plugin, multiplicadores de cámara, clip lejano, escala LOD, CGame::currArea, MI_GRASSHOUSE, textura '
           'de piel del jugador 0, model infos 450..465 y el puntero de estados de render; primitivas RenderWare, luces, '
           'CRenderer::RenderOneNonRoad, CPed/CWeaponInfo y operator new/delete doblados con traza sin crédito '
           '(AtomicDefaultRenderCallBack se traza con el hash de alfas y banderas vistas al renderizar); fixtures sin sNaN (NaN '
           'silenciosos solo donde la comparación forma parte de lo probado). Consulte el README del banco para la lista '
           'completa de límites.',
 'error': None,
 'expect': {'CVisibilityPlugins::AddWeaponPedForPC': 192,
            'CVisibilityPlugins::AtomicCopyConstructor': 96,
            'CVisibilityPlugins::AtomicDestructor': 96,
            'CVisibilityPlugins::CalculateFadingAtomicAlpha': 512,
            'CVisibilityPlugins::ClearAtomicFlag': 96,
            'CVisibilityPlugins::ClearClumpForAllAtomicsFlag': 96,
            'CVisibilityPlugins::ClumpConstructor': 96,
            'CVisibilityPlugins::ClumpCopyConstructor': 96,
            'CVisibilityPlugins::ClumpDestructor': 96,
            'CVisibilityPlugins::DefaultVisibilityCB': 96,
            'CVisibilityPlugins::FrameConstructor': 96,
            'CVisibilityPlugins::FrameCopyConstructor': 96,
            'CVisibilityPlugins::FrameDestructor': 96,
            'CVisibilityPlugins::FrustumSphereCB': 256,
            'CVisibilityPlugins::GetAtomicId': 96,
            'CVisibilityPlugins::GetClumpAlpha': 96,
            'CVisibilityPlugins::GetClumpModelInfo': 96,
            'CVisibilityPlugins::GetDistanceSquaredFromCamera-Frame': 256,
            'CVisibilityPlugins::GetDistanceSquaredFromCamera-RwV3d': 256,
            'CVisibilityPlugins::GetDotProductWithCameraVector': 512,
            'CVisibilityPlugins::GetFrameHierarchyId': 96,
            'CVisibilityPlugins::GetModelInfo': 96,
            'CVisibilityPlugins::GetModelInfoIndex': 96,
            'CVisibilityPlugins::GetUserValue': 96,
            'CVisibilityPlugins::InitAlphaAtomicList': 96,
            'CVisibilityPlugins::InitAlphaEntityList': 96,
            'CVisibilityPlugins::Initialise': 96,
            'CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList': 256,
            'CVisibilityPlugins::InsertEntityIntoReallyDrawLastList': 256,
            'CVisibilityPlugins::InsertEntityIntoSortedList': 384,
            'CVisibilityPlugins::InsertEntityIntoUnderwaterList': 256,
            'CVisibilityPlugins::InsertObjectIntoSortedList': 256,
            'CVisibilityPlugins::IsAtomicVisible': 256,
            'CVisibilityPlugins::IsClumpVisible': 96,
            'CVisibilityPlugins::PluginAttach': 192,
            'CVisibilityPlugins::RenderAlphaAtomics': 256,
            'CVisibilityPlugins::RenderAtomicWithAlpha': 384,
            'CVisibilityPlugins::RenderAtomicWithAlphaCB': 256,
            'CVisibilityPlugins::RenderBoatAlphaAtomics': 256,
            'CVisibilityPlugins::RenderEntity': 512,
            'CVisibilityPlugins::RenderFadingAtomic': 256,
            'CVisibilityPlugins::RenderFadingClump': 256,
            'CVisibilityPlugins::RenderFadingClumpCB': 256,
            'CVisibilityPlugins::RenderFadingEntities': 256,
            'CVisibilityPlugins::RenderFadingUnderwaterEntities': 256,
            'CVisibilityPlugins::RenderHeliRotorAlphaCB': 256,
            'CVisibilityPlugins::RenderHeliTailRotorAlphaCB': 256,
            'CVisibilityPlugins::RenderObjNormalAtomic': 256,
            'CVisibilityPlugins::RenderOrderedList': 192,
            'CVisibilityPlugins::RenderPedCB': 256,
            'CVisibilityPlugins::RenderPlayerCB': 256,
            'CVisibilityPlugins::RenderReallyDrawLastObjects': 256,
            'CVisibilityPlugins::RenderTrainHiDetailAlphaCB': 384,
            'CVisibilityPlugins::RenderTrainHiDetailCB': 384,
            'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB': 512,
            'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle': 512,
            'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat': 256,
            'CVisibilityPlugins::RenderVehicleHiDetailCB': 512,
            'CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle': 384,
            'CVisibilityPlugins::RenderVehicleHiDetailCB_Boat': 256,
            'CVisibilityPlugins::RenderVehicleLoDetailCB_Boat': 256,
            'CVisibilityPlugins::RenderVehicleReallyLowDetailCB': 256,
            'CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle': 256,
            'CVisibilityPlugins::RenderWeaponCB': 256,
            'CVisibilityPlugins::RenderWeaponPedsForPC': 512,
            'CVisibilityPlugins::RenderWheelAtomicCB': 256,
            'CVisibilityPlugins::SetAlphaCB': 128,
            'CVisibilityPlugins::SetAtomicFlag': 96,
            'CVisibilityPlugins::SetAtomicId': 96,
            'CVisibilityPlugins::SetAtomicRenderCallback': 128,
            'CVisibilityPlugins::SetClumpAlpha': 96,
            'CVisibilityPlugins::SetClumpForAllAtomicsFlag': 96,
            'CVisibilityPlugins::SetClumpModelInfo': 192,
            'CVisibilityPlugins::SetFrameHierarchyId': 96,
            'CVisibilityPlugins::SetModelInfoIndex': 96,
            'CVisibilityPlugins::SetRenderWareCamera': 256,
            'CVisibilityPlugins::SetTextureCB': 96,
            'CVisibilityPlugins::SetUserValue': 96,
            'CVisibilityPlugins::SetupVehicleVariables': 256,
            'CVisibilityPlugins::Shutdown': 192,
            'CVisibilityPlugins::VehicleVisibilityCB': 256,
            'CVisibilityPlugins::VehicleVisibilityCB_BigVehicle': 192,
            'CVisibilityPlugins::clearClumpForAllAtomicsFlagCB': 96,
            'CVisibilityPlugins::setClumpForAllAtomicsFlagCB': 96},
 'extra_unregistered': [],
 'new': ['CVisibilityPlugins::AddWeaponPedForPC',
         'CVisibilityPlugins::AtomicCopyConstructor',
         'CVisibilityPlugins::AtomicDestructor',
         'CVisibilityPlugins::CalculateFadingAtomicAlpha',
         'CVisibilityPlugins::ClearAtomicFlag',
         'CVisibilityPlugins::ClearClumpForAllAtomicsFlag',
         'CVisibilityPlugins::ClumpConstructor',
         'CVisibilityPlugins::ClumpCopyConstructor',
         'CVisibilityPlugins::ClumpDestructor',
         'CVisibilityPlugins::DefaultVisibilityCB',
         'CVisibilityPlugins::FrameConstructor',
         'CVisibilityPlugins::FrameCopyConstructor',
         'CVisibilityPlugins::FrameDestructor',
         'CVisibilityPlugins::FrustumSphereCB',
         'CVisibilityPlugins::GetAtomicId',
         'CVisibilityPlugins::GetClumpAlpha',
         'CVisibilityPlugins::GetClumpModelInfo',
         'CVisibilityPlugins::GetDistanceSquaredFromCamera-Frame',
         'CVisibilityPlugins::GetDistanceSquaredFromCamera-RwV3d',
         'CVisibilityPlugins::GetDotProductWithCameraVector',
         'CVisibilityPlugins::GetFrameHierarchyId',
         'CVisibilityPlugins::GetModelInfo',
         'CVisibilityPlugins::GetModelInfoIndex',
         'CVisibilityPlugins::GetUserValue',
         'CVisibilityPlugins::InitAlphaAtomicList',
         'CVisibilityPlugins::InitAlphaEntityList',
         'CVisibilityPlugins::Initialise',
         'CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList',
         'CVisibilityPlugins::InsertEntityIntoReallyDrawLastList',
         'CVisibilityPlugins::InsertEntityIntoSortedList',
         'CVisibilityPlugins::InsertEntityIntoUnderwaterList',
         'CVisibilityPlugins::InsertObjectIntoSortedList',
         'CVisibilityPlugins::IsAtomicVisible',
         'CVisibilityPlugins::IsClumpVisible',
         'CVisibilityPlugins::PluginAttach',
         'CVisibilityPlugins::RenderAlphaAtomics',
         'CVisibilityPlugins::RenderAtomicWithAlpha',
         'CVisibilityPlugins::RenderAtomicWithAlphaCB',
         'CVisibilityPlugins::RenderBoatAlphaAtomics',
         'CVisibilityPlugins::RenderEntity',
         'CVisibilityPlugins::RenderFadingAtomic',
         'CVisibilityPlugins::RenderFadingClump',
         'CVisibilityPlugins::RenderFadingClumpCB',
         'CVisibilityPlugins::RenderFadingEntities',
         'CVisibilityPlugins::RenderFadingUnderwaterEntities',
         'CVisibilityPlugins::RenderHeliRotorAlphaCB',
         'CVisibilityPlugins::RenderHeliTailRotorAlphaCB',
         'CVisibilityPlugins::RenderObjNormalAtomic',
         'CVisibilityPlugins::RenderOrderedList',
         'CVisibilityPlugins::RenderPedCB',
         'CVisibilityPlugins::RenderPlayerCB',
         'CVisibilityPlugins::RenderReallyDrawLastObjects',
         'CVisibilityPlugins::RenderTrainHiDetailAlphaCB',
         'CVisibilityPlugins::RenderTrainHiDetailCB',
         'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB',
         'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle',
         'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat',
         'CVisibilityPlugins::RenderVehicleHiDetailCB',
         'CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle',
         'CVisibilityPlugins::RenderVehicleHiDetailCB_Boat',
         'CVisibilityPlugins::RenderVehicleLoDetailCB_Boat',
         'CVisibilityPlugins::RenderVehicleReallyLowDetailCB',
         'CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle',
         'CVisibilityPlugins::RenderWeaponCB',
         'CVisibilityPlugins::RenderWeaponPedsForPC',
         'CVisibilityPlugins::RenderWheelAtomicCB',
         'CVisibilityPlugins::SetAlphaCB',
         'CVisibilityPlugins::SetAtomicFlag',
         'CVisibilityPlugins::SetAtomicId',
         'CVisibilityPlugins::SetAtomicRenderCallback',
         'CVisibilityPlugins::SetClumpAlpha',
         'CVisibilityPlugins::SetClumpForAllAtomicsFlag',
         'CVisibilityPlugins::SetClumpModelInfo',
         'CVisibilityPlugins::SetFrameHierarchyId',
         'CVisibilityPlugins::SetModelInfoIndex',
         'CVisibilityPlugins::SetRenderWareCamera',
         'CVisibilityPlugins::SetTextureCB',
         'CVisibilityPlugins::SetUserValue',
         'CVisibilityPlugins::SetupVehicleVariables',
         'CVisibilityPlugins::Shutdown',
         'CVisibilityPlugins::VehicleVisibilityCB',
         'CVisibilityPlugins::VehicleVisibilityCB_BigVehicle',
         'CVisibilityPlugins::clearClumpForAllAtomicsFlagCB',
         'CVisibilityPlugins::setClumpForAllAtomicsFlagCB'],
 'notes': 'VisPluginsFinal4 registra las 84 funciones de CVisibilityPlugins (18176 pares, 0 diferencias). Los límites se '
          'declaran por función; véanse function_scope y el README del banco. No quedan funciones pendientes en '
          'CVisibilityPlugins.',
 'passed': True,
 'pending_after': [],
 'probe_prefix': 'a1ed2435',
 'provenance': 'Probe VisPluginsFinal4 compilado exacto (hashes de ASI/PDB en build-manifest.json); fuentes de la clase con hash '
               'del manifest de compilación e iguales al árbol de trabajo en el registro; log completo de 18176 filas reauditado '
               'offline en streaming y conservado comprimido con gzip con el hash original.',
 'readme_section': '## Aprobación (10/10/2026)\n'
                   '\n'
                   'Corrida `20261010-130234-135622`: passed=true, 18176 pares, 84 funciones registradas con 0 fallos, evento '
                   'complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con '
                   '`register_vispluginsfinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco '
                   'CVisibilityPlugins queda sin pendientes. Límites declarados en el alcance de cada función y en las secciones '
                   'anteriores de este README.',
 'registry_before': 8066,
 'rejected': [],
 'run_id': '20261010-130234-135622',
 'source_changes': {'source/game_sa/VisibilityPlugins.cpp': 'Correcciones contra el EXE (IsClumpVisible, '
                                                            'RenderAtomicWithAlphaCB, IsAtomicVisible, RenderHeli*AlphaCB, '
                                                            'GetDistanceSquaredFromCamera, GetDotProductWithCameraVector, '
                                                            'RenderObjNormalAtomic, SetupVehicleVariables, '
                                                            'CalculateFadingAtomicAlpha, RenderAtomicWithAlpha, '
                                                            'SetAtomicRenderCallback, SetRenderWareCamera, VehicleVisibilityCB, '
                                                            'RenderVehicleCB_ControlRenderMultiPassLOD, '
                                                            'InsertEntityIntoSortedList, RenderEntity, variables globales de '
                                                            'distancia/ángulo en 0xC88024/0xC88020); ver git diff y el README '
                                                            'del banco.'},
 'specific': {'CVisibilityPlugins::AddWeaponPedForPC': 'Corpus 192; mundo falso estático y colaboradores RenderWare doblados con '
                                                       'traza (sin crédito). ',
              'CVisibilityPlugins::AtomicCopyConstructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                           'con traza (sin crédito). ',
              'CVisibilityPlugins::AtomicDestructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                      'traza (sin crédito). ',
              'CVisibilityPlugins::CalculateFadingAtomicAlpha': 'Corpus 512; mundo falso estático y colaboradores RenderWare '
                                                                'doblados con traza (sin crédito). fuente corregida '
                                                                '(0x732500-0x7325B8): selección < exacta, factor 0.06666667f, '
                                                                'recorte solo si > 1.0, _ftol2 de 64 bits. ',
              'CVisibilityPlugins::ClearAtomicFlag': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                     'traza (sin crédito). ',
              'CVisibilityPlugins::ClearClumpForAllAtomicsFlag': 'Corpus 96; mundo falso estático y colaboradores RenderWare '
                                                                 'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::ClumpConstructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                      'traza (sin crédito). ',
              'CVisibilityPlugins::ClumpCopyConstructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                          'con traza (sin crédito). ',
              'CVisibilityPlugins::ClumpDestructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                     'traza (sin crédito). ',
              'CVisibilityPlugins::DefaultVisibilityCB': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). ',
              'CVisibilityPlugins::FrameConstructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                      'traza (sin crédito). ',
              'CVisibilityPlugins::FrameCopyConstructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                          'con traza (sin crédito). ',
              'CVisibilityPlugins::FrameDestructor': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                     'traza (sin crédito). ',
              'CVisibilityPlugins::FrustumSphereCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados con '
                                                     'traza (sin crédito). ',
              'CVisibilityPlugins::GetAtomicId': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                 '(sin crédito). ',
              'CVisibilityPlugins::GetClumpAlpha': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                   'traza (sin crédito). ',
              'CVisibilityPlugins::GetClumpModelInfo': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                       'traza (sin crédito). ',
              'CVisibilityPlugins::GetDistanceSquaredFromCamera-Frame': 'Corpus 256; mundo falso estático y colaboradores '
                                                                        'RenderWare doblados con traza (sin crédito). fuente '
                                                                        'corregida (0x732CA7): suma (z²+y²)+x². ',
              'CVisibilityPlugins::GetDistanceSquaredFromCamera-RwV3d': 'Corpus 256; mundo falso estático y colaboradores '
                                                                        'RenderWare doblados con traza (sin crédito). fuente '
                                                                        'corregida (0x732CDA): suma (z²+y²)+x². ',
              'CVisibilityPlugins::GetDotProductWithCameraVector': 'Corpus 512; mundo falso estático y colaboradores RenderWare '
                                                                   'doblados con traza (sin crédito). fuente corregida '
                                                                   '(0x7326E0-0x732776): productos escalares en orden y*y + x*x '
                                                                   '+ z*z. ',
              'CVisibilityPlugins::GetFrameHierarchyId': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). ',
              'CVisibilityPlugins::GetModelInfo': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                  '(sin crédito). ',
              'CVisibilityPlugins::GetModelInfoIndex': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                       'traza (sin crédito). ',
              'CVisibilityPlugins::GetUserValue': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                  '(sin crédito). el retorno es AX: se enmascara a 16 bits. ',
              'CVisibilityPlugins::InitAlphaAtomicList': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). ',
              'CVisibilityPlugins::InitAlphaEntityList': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). ',
              'CVisibilityPlugins::Initialise': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                '(sin crédito). operator new/delete doblados con traza sobre una arena de 128 '
                                                'KB, sin crédito. ',
              'CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList': 'Corpus 256; mundo falso estático y colaboradores '
                                                                        'RenderWare doblados con traza (sin crédito). solo se '
                                                                        'compara AL del retorno bool. ',
              'CVisibilityPlugins::InsertEntityIntoReallyDrawLastList': 'Corpus 256; mundo falso estático y colaboradores '
                                                                        'RenderWare doblados con traza (sin crédito). solo se '
                                                                        'compara AL del retorno bool. ',
              'CVisibilityPlugins::InsertEntityIntoSortedList': 'Corpus 384; mundo falso estático y colaboradores RenderWare '
                                                                'doblados con traza (sin crédito). fuente corregida '
                                                                '(0x734570-0x73458B): movsx del índice de la entidad. solo se '
                                                                'compara AL del retorno bool. ',
              'CVisibilityPlugins::InsertEntityIntoUnderwaterList': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                    'doblados con traza (sin crédito). solo se compara AL del '
                                                                    'retorno bool. ',
              'CVisibilityPlugins::InsertObjectIntoSortedList': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                'doblados con traza (sin crédito). solo se compara AL del '
                                                                'retorno bool. ',
              'CVisibilityPlugins::IsAtomicVisible': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados con '
                                                     'traza (sin crédito). fuente corregida (0x7329C4-0x7329D8): '
                                                     'RwV3dTransformPoints transforma la esfera en el sitio. solo se compara AL '
                                                     'del retorno bool. ',
              'CVisibilityPlugins::IsClumpVisible': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                    'traza (sin crédito). fuente corregida (0x732AE0): llama a la callback del '
                                                    'plugin con el clump (la DLL devolvía el puntero como bool). solo se compara '
                                                    'AL del retorno bool. ',
              'CVisibilityPlugins::PluginAttach': 'Corpus 192; mundo falso estático y colaboradores RenderWare doblados con '
                                                  'traza (sin crédito). los tres callbacks de plugin (constructor, copia y '
                                                  'destructor) se ejecutan de verdad sobre objetos de prueba, de modo que '
                                                  'AtomicConstructor (0x732150, sin hook) queda comparado; solo se compara AL '
                                                  'del retorno bool. ',
              'CVisibilityPlugins::RenderAlphaAtomics': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                        'con traza (sin crédito). ',
              'CVisibilityPlugins::RenderAtomicWithAlpha': 'Corpus 384; mundo falso estático y colaboradores RenderWare doblados '
                                                           'con traza (sin crédito). fuente corregida (0x7324B6-0x7324EF): '
                                                           'comparación con signo contra el entero completo y restauración de '
                                                           'atrás hacia delante. ',
              'CVisibilityPlugins::RenderAtomicWithAlphaCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                             'doblados con traza (sin crédito). fuente corregida (0x73266B): '
                                                             'data apunta al entero con la alfa. ',
              'CVisibilityPlugins::RenderBoatAlphaAtomics': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                            'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderEntity': 'Corpus 512; mundo falso estático y colaboradores RenderWare doblados con '
                                                  'traza (sin crédito). fuente corregida (0x732B50): movsx del índice de la '
                                                  'entidad frente a movzx del global. ',
              'CVisibilityPlugins::RenderFadingAtomic': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                        'con traza (sin crédito). ',
              'CVisibilityPlugins::RenderFadingClump': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados con '
                                                       'traza (sin crédito). ',
              'CVisibilityPlugins::RenderFadingClumpCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). ',
              'CVisibilityPlugins::RenderFadingEntities': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                          'con traza (sin crédito). ',
              'CVisibilityPlugins::RenderFadingUnderwaterEntities': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                    'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderHeliRotorAlphaCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                            'doblados con traza (sin crédito). fuente corregida '
                                                            '(0x7340E8-0x734137): orden dist + 20*dot(pos-cámara, at del '
                                                            'clump). ',
              'CVisibilityPlugins::RenderHeliTailRotorAlphaCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                'doblados con traza (sin crédito). fuente corregida '
                                                                '(0x7341C2-0x734202): ((-dot_right) - dot_up) + dist con suma '
                                                                'y*y + x*x + z*z. ',
              'CVisibilityPlugins::RenderObjNormalAtomic': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                           'con traza (sin crédito). fuente corregida (0x732420-0x73243A): '
                                                           'producto escalar en orden y*y + x*x + z*z. ',
              'CVisibilityPlugins::RenderOrderedList': 'Corpus 192; mundo falso estático y colaboradores RenderWare doblados con '
                                                       'traza (sin crédito). ',
              'CVisibilityPlugins::RenderPedCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                 '(sin crédito). ',
              'CVisibilityPlugins::RenderPlayerCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados con '
                                                    'traza (sin crédito). ',
              'CVisibilityPlugins::RenderReallyDrawLastObjects': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                 'doblados con traza (sin crédito). no se compara el rastreo de '
                                                                 'Tracy. ',
              'CVisibilityPlugins::RenderTrainHiDetailAlphaCB': 'Corpus 384; mundo falso estático y colaboradores RenderWare '
                                                                'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderTrainHiDetailCB': 'Corpus 384; mundo falso estático y colaboradores RenderWare doblados '
                                                           'con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB': 'Corpus 512; mundo falso estático y colaboradores RenderWare '
                                                                  'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle': 'Corpus 512; mundo falso estático y colaboradores '
                                                                             'RenderWare doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat': 'Corpus 256; mundo falso estático y colaboradores '
                                                                       'RenderWare doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleHiDetailCB': 'Corpus 512; mundo falso estático y colaboradores RenderWare '
                                                             'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle': 'Corpus 384; mundo falso estático y colaboradores '
                                                                        'RenderWare doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleHiDetailCB_Boat': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                  'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleLoDetailCB_Boat': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                  'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleReallyLowDetailCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare '
                                                                    'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle': 'Corpus 256; mundo falso estático y colaboradores '
                                                                               'RenderWare doblados con traza (sin crédito). ',
              'CVisibilityPlugins::RenderWeaponCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados con '
                                                    'traza (sin crédito). ',
              'CVisibilityPlugins::RenderWeaponPedsForPC': 'Corpus 512; mundo falso estático y colaboradores RenderWare doblados '
                                                           'con traza (sin crédito). no se compara el rastreo de Tracy. ',
              'CVisibilityPlugins::RenderWheelAtomicCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). ',
              'CVisibilityPlugins::SetAlphaCB': 'Corpus 128; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                '(sin crédito). ',
              'CVisibilityPlugins::SetAtomicFlag': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                   'traza (sin crédito). ',
              'CVisibilityPlugins::SetAtomicId': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                 '(sin crédito). ',
              'CVisibilityPlugins::SetAtomicRenderCallback': 'Corpus 128; mundo falso estático y colaboradores RenderWare '
                                                             'doblados con traza (sin crédito). fuente corregida '
                                                             '(0x7328A6-0x7328B8): guarda 0x7491C0 y no su envoltorio. ',
              'CVisibilityPlugins::SetClumpAlpha': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                   'traza (sin crédito). ',
              'CVisibilityPlugins::SetClumpForAllAtomicsFlag': 'Corpus 96; mundo falso estático y colaboradores RenderWare '
                                                               'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::SetClumpModelInfo': 'Corpus 192; mundo falso estático y colaboradores RenderWare doblados con '
                                                       'traza (sin crédito). ',
              'CVisibilityPlugins::SetFrameHierarchyId': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). ',
              'CVisibilityPlugins::SetModelInfoIndex': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con '
                                                       'traza (sin crédito). ',
              'CVisibilityPlugins::SetRenderWareCamera': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). fuente corregida (0x7328C0-0x732988): v*v en '
                                                         'float, sin powf. ',
              'CVisibilityPlugins::SetTextureCB': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                  '(sin crédito). ',
              'CVisibilityPlugins::SetUserValue': 'Corpus 96; mundo falso estático y colaboradores RenderWare doblados con traza '
                                                  '(sin crédito). ',
              'CVisibilityPlugins::SetupVehicleVariables': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                           'con traza (sin crédito). fuente corregida (0x733190-0x7331D4): suma '
                                                           '(z²+y²)+x², atan2 por fpatan x87, sqrt en float, cámara menos '
                                                           'posición del frame; variables globales en 0xC88024/0xC88020. ',
              'CVisibilityPlugins::Shutdown': 'Corpus 192; mundo falso estático y colaboradores RenderWare doblados con traza '
                                              '(sin crédito). operator new/delete doblados con traza sobre una arena de 128 KB, '
                                              'sin crédito. ',
              'CVisibilityPlugins::VehicleVisibilityCB': 'Corpus 256; mundo falso estático y colaboradores RenderWare doblados '
                                                         'con traza (sin crédito). fuente corregida (0x73372B-0x73373C): '
                                                         'condiciones !(a > b) y !(a < b) (con NaN el EXE toma la otra rama). ',
              'CVisibilityPlugins::VehicleVisibilityCB_BigVehicle': 'Corpus 192; mundo falso estático y colaboradores RenderWare '
                                                                    'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::clearClumpForAllAtomicsFlagCB': 'Corpus 96; mundo falso estático y colaboradores RenderWare '
                                                                   'doblados con traza (sin crédito). ',
              'CVisibilityPlugins::setClumpForAllAtomicsFlagCB': 'Corpus 96; mundo falso estático y colaboradores RenderWare '
                                                                 'doblados con traza (sin crédito). '},
 'tag': 'vispluginsfinal4',
 'total': 18176,
 'types': {'CVisibilityPlugins::AddWeaponPedForPC': 'S',
           'CVisibilityPlugins::AtomicCopyConstructor': 'S',
           'CVisibilityPlugins::AtomicDestructor': 'S',
           'CVisibilityPlugins::CalculateFadingAtomicAlpha': 'S',
           'CVisibilityPlugins::ClearAtomicFlag': 'S',
           'CVisibilityPlugins::ClearClumpForAllAtomicsFlag': 'S',
           'CVisibilityPlugins::ClumpConstructor': 'S',
           'CVisibilityPlugins::ClumpCopyConstructor': 'S',
           'CVisibilityPlugins::ClumpDestructor': 'S',
           'CVisibilityPlugins::DefaultVisibilityCB': 'S',
           'CVisibilityPlugins::FrameConstructor': 'S',
           'CVisibilityPlugins::FrameCopyConstructor': 'S',
           'CVisibilityPlugins::FrameDestructor': 'S',
           'CVisibilityPlugins::FrustumSphereCB': 'S',
           'CVisibilityPlugins::GetAtomicId': 'S',
           'CVisibilityPlugins::GetClumpAlpha': 'S',
           'CVisibilityPlugins::GetClumpModelInfo': 'S',
           'CVisibilityPlugins::GetDistanceSquaredFromCamera-Frame': 'S',
           'CVisibilityPlugins::GetDistanceSquaredFromCamera-RwV3d': 'S',
           'CVisibilityPlugins::GetDotProductWithCameraVector': 'S',
           'CVisibilityPlugins::GetFrameHierarchyId': 'S',
           'CVisibilityPlugins::GetModelInfo': 'S',
           'CVisibilityPlugins::GetModelInfoIndex': 'S',
           'CVisibilityPlugins::GetUserValue': 'S',
           'CVisibilityPlugins::InitAlphaAtomicList': 'S',
           'CVisibilityPlugins::InitAlphaEntityList': 'S',
           'CVisibilityPlugins::Initialise': 'S',
           'CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList': 'S',
           'CVisibilityPlugins::InsertEntityIntoReallyDrawLastList': 'S',
           'CVisibilityPlugins::InsertEntityIntoSortedList': 'S',
           'CVisibilityPlugins::InsertEntityIntoUnderwaterList': 'S',
           'CVisibilityPlugins::InsertObjectIntoSortedList': 'S',
           'CVisibilityPlugins::IsAtomicVisible': 'S',
           'CVisibilityPlugins::IsClumpVisible': 'S',
           'CVisibilityPlugins::PluginAttach': 'S',
           'CVisibilityPlugins::RenderAlphaAtomics': 'S',
           'CVisibilityPlugins::RenderAtomicWithAlpha': 'S',
           'CVisibilityPlugins::RenderAtomicWithAlphaCB': 'S',
           'CVisibilityPlugins::RenderBoatAlphaAtomics': 'S',
           'CVisibilityPlugins::RenderEntity': 'S',
           'CVisibilityPlugins::RenderFadingAtomic': 'S',
           'CVisibilityPlugins::RenderFadingClump': 'S',
           'CVisibilityPlugins::RenderFadingClumpCB': 'S',
           'CVisibilityPlugins::RenderFadingEntities': 'S',
           'CVisibilityPlugins::RenderFadingUnderwaterEntities': 'S',
           'CVisibilityPlugins::RenderHeliRotorAlphaCB': 'S',
           'CVisibilityPlugins::RenderHeliTailRotorAlphaCB': 'S',
           'CVisibilityPlugins::RenderObjNormalAtomic': 'S',
           'CVisibilityPlugins::RenderOrderedList': 'S',
           'CVisibilityPlugins::RenderPedCB': 'S',
           'CVisibilityPlugins::RenderPlayerCB': 'S',
           'CVisibilityPlugins::RenderReallyDrawLastObjects': 'S',
           'CVisibilityPlugins::RenderTrainHiDetailAlphaCB': 'S',
           'CVisibilityPlugins::RenderTrainHiDetailCB': 'S',
           'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB': 'S',
           'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle': 'S',
           'CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat': 'S',
           'CVisibilityPlugins::RenderVehicleHiDetailCB': 'S',
           'CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle': 'S',
           'CVisibilityPlugins::RenderVehicleHiDetailCB_Boat': 'S',
           'CVisibilityPlugins::RenderVehicleLoDetailCB_Boat': 'S',
           'CVisibilityPlugins::RenderVehicleReallyLowDetailCB': 'S',
           'CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle': 'S',
           'CVisibilityPlugins::RenderWeaponCB': 'S',
           'CVisibilityPlugins::RenderWeaponPedsForPC': 'S',
           'CVisibilityPlugins::RenderWheelAtomicCB': 'S',
           'CVisibilityPlugins::SetAlphaCB': 'S',
           'CVisibilityPlugins::SetAtomicFlag': 'S',
           'CVisibilityPlugins::SetAtomicId': 'S',
           'CVisibilityPlugins::SetAtomicRenderCallback': 'S',
           'CVisibilityPlugins::SetClumpAlpha': 'S',
           'CVisibilityPlugins::SetClumpForAllAtomicsFlag': 'S',
           'CVisibilityPlugins::SetClumpModelInfo': 'S',
           'CVisibilityPlugins::SetFrameHierarchyId': 'S',
           'CVisibilityPlugins::SetModelInfoIndex': 'S',
           'CVisibilityPlugins::SetRenderWareCamera': 'S',
           'CVisibilityPlugins::SetTextureCB': 'S',
           'CVisibilityPlugins::SetUserValue': 'S',
           'CVisibilityPlugins::SetupVehicleVariables': 'S',
           'CVisibilityPlugins::Shutdown': 'S',
           'CVisibilityPlugins::VehicleVisibilityCB': 'S',
           'CVisibilityPlugins::VehicleVisibilityCB_BigVehicle': 'S',
           'CVisibilityPlugins::clearClumpForAllAtomicsFlagCB': 'S',
           'CVisibilityPlugins::setClumpForAllAtomicsFlagCB': 'S'}}
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
readme=HERE/'README.md';raw_r=readme.read_bytes();nl='\r\n' if b'\r\n' in raw_r else '\n'
section=CFG['readme_section'].replace('\n',nl)
assert '## Aprobación' not in raw_r.decode('utf-8')
with open(readme,'ab') as fh:fh.write((nl+section+nl).encode('utf-8'))
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp),changed_src=list(CHANGED_SRC),changed_bank=list(CHANGED_BANK)),indent=2))
