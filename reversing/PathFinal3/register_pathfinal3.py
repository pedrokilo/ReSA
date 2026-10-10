"""Registro historico de las 54 funciones de CPathFind del banco PathFinal3 (corrida 20261009-195539-016084). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'PathFinal3',
 'run_id': '20261009-195539-016084',
 'probe_prefix': '077d3e19',
 'tag': 'pathfinal3',
 'registry_before': 7496,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CPathFind::AddDynamicLinkBetween2Nodes_For1Node': 1024,
            'CPathFind::AddInteriorLink': 512,
            'CPathFind::AddInteriorLinkToExternalNode': 256,
            'CPathFind::AddNodeToList': 512,
            'CPathFind::AddNodeToNewInterior': 512,
            'CPathFind::AreNodesLoadedForArea': 1024,
            'CPathFind::CountNeighboursToBeSwitchedOff': 512,
            'CPathFind::FindLinkBetweenNodes': 512,
            'CPathFind::FindNearestExteriorNodeToInteriorNode': 256,
            'CPathFind::FindNodeClosestToCoorsFavourDirection': 1024,
            'CPathFind::FindNodeCoorsForScript-LinkedNode': 1024,
            'CPathFind::FindNodeCoorsForScript-TwoNodes': 1024,
            'CPathFind::FindRegionForCoors': 1024,
            'CPathFind::FindStartPointOfRegion-': 256,
            'CPathFind::FindXCoorsForRegion': 128,
            'CPathFind::FindXRegionForCoors': 1024,
            'CPathFind::FindYCoorsForRegion': 128,
            'CPathFind::FindYRegionForCoors': 1024,
            'CPathFind::GetPathNode': 256,
            'CPathFind::HaveRequestedNodesBeenLoaded': 256,
            'CPathFind::Init': 128,
            'CPathFind::IsAreaNodesAvailable': 256,
            'CPathFind::IsWaterNodeNearby': 768,
            'CPathFind::Load': 256,
            'CPathFind::LoadPathFindData-Area': 512,
            'CPathFind::LoadPathFindData-FromStream': 512,
            'CPathFind::LoadSceneForPathNodes': 256,
            'CPathFind::MakeRequestForNodesToBeLoaded': 512,
            'CPathFind::MarkRegionsForCoors': 1024,
            'CPathFind::MarkRoadNodeAsDontWander': 256,
            'CPathFind::ReInit': 64,
            'CPathFind::ReleaseRequestedNodes': 64,
            'CPathFind::RemoveInterior': 768,
            'CPathFind::RemoveNodeFromList': 256,
            'CPathFind::ReturnInteriorNodeIndex': 256,
            'CPathFind::Save': 256,
            'CPathFind::SetLinksBridgeLights': 512,
            'CPathFind::SetPathsNeededAtPosition': 128,
            'CPathFind::Shutdown': 128,
            'CPathFind::StartNewInterior': 256,
            'CPathFind::SwitchOffNodeAndNeighbours': 1024,
            'CPathFind::SwitchPedRoadsOffInArea': 512,
            'CPathFind::SwitchRoadsOffInArea': 768,
            'CPathFind::SwitchRoadsOffInAreaForOneRegion': 768,
            'CPathFind::TakeWidthIntoAccountForWandering': 512,
            'CPathFind::TestCrossesRoad': 512,
            'CPathFind::TestForPedTrafficLight': 512,
            'CPathFind::These2NodesAreAdjacent': 512,
            'CPathFind::ThisNodeHasToBeSwitchedOff': 128,
            'CPathFind::ThisNodeWillLeadIntoADeadEnd': 512,
            'CPathFind::TidyUpNodeSwitchesAfterMission': 128,
            'CPathFind::UnLoadPathFindData': 256,
            'CPathFind::UnMarkAllRoadNodesAsDontWander': 256,
            'CPathFind::UpdateStreaming': 1024},
 'total': 26880,
 'new': ['CPathFind::AddDynamicLinkBetween2Nodes_For1Node',
         'CPathFind::AddInteriorLink',
         'CPathFind::AddInteriorLinkToExternalNode',
         'CPathFind::AddNodeToList',
         'CPathFind::AddNodeToNewInterior',
         'CPathFind::AreNodesLoadedForArea',
         'CPathFind::CountNeighboursToBeSwitchedOff',
         'CPathFind::FindLinkBetweenNodes',
         'CPathFind::FindNearestExteriorNodeToInteriorNode',
         'CPathFind::FindNodeClosestToCoorsFavourDirection',
         'CPathFind::FindNodeCoorsForScript-LinkedNode',
         'CPathFind::FindNodeCoorsForScript-TwoNodes',
         'CPathFind::FindRegionForCoors',
         'CPathFind::FindStartPointOfRegion-',
         'CPathFind::FindXCoorsForRegion',
         'CPathFind::FindXRegionForCoors',
         'CPathFind::FindYCoorsForRegion',
         'CPathFind::FindYRegionForCoors',
         'CPathFind::GetPathNode',
         'CPathFind::HaveRequestedNodesBeenLoaded',
         'CPathFind::Init',
         'CPathFind::IsAreaNodesAvailable',
         'CPathFind::IsWaterNodeNearby',
         'CPathFind::Load',
         'CPathFind::LoadPathFindData-Area',
         'CPathFind::LoadPathFindData-FromStream',
         'CPathFind::LoadSceneForPathNodes',
         'CPathFind::MakeRequestForNodesToBeLoaded',
         'CPathFind::MarkRegionsForCoors',
         'CPathFind::MarkRoadNodeAsDontWander',
         'CPathFind::ReInit',
         'CPathFind::ReleaseRequestedNodes',
         'CPathFind::RemoveInterior',
         'CPathFind::RemoveNodeFromList',
         'CPathFind::ReturnInteriorNodeIndex',
         'CPathFind::Save',
         'CPathFind::SetLinksBridgeLights',
         'CPathFind::SetPathsNeededAtPosition',
         'CPathFind::Shutdown',
         'CPathFind::StartNewInterior',
         'CPathFind::SwitchOffNodeAndNeighbours',
         'CPathFind::SwitchPedRoadsOffInArea',
         'CPathFind::SwitchRoadsOffInArea',
         'CPathFind::SwitchRoadsOffInAreaForOneRegion',
         'CPathFind::TakeWidthIntoAccountForWandering',
         'CPathFind::TestCrossesRoad',
         'CPathFind::TestForPedTrafficLight',
         'CPathFind::These2NodesAreAdjacent',
         'CPathFind::ThisNodeHasToBeSwitchedOff',
         'CPathFind::ThisNodeWillLeadIntoADeadEnd',
         'CPathFind::TidyUpNodeSwitchesAfterMission',
         'CPathFind::UnLoadPathFindData',
         'CPathFind::UnMarkAllRoadNodesAsDontWander',
         'CPathFind::UpdateStreaming'],
 'rejected': [],
 'addr': {'CPathFind::AddDynamicLinkBetween2Nodes_For1Node': '0x0044E000',
          'CPathFind::AddInteriorLink': '0x0044DED0',
          'CPathFind::AddInteriorLinkToExternalNode': '0x0044DF30',
          'CPathFind::AddNodeToList': '0x0044D1E0',
          'CPathFind::AddNodeToNewInterior': '0x00450E90',
          'CPathFind::AreNodesLoadedForArea': '0x0044DD10',
          'CPathFind::CountNeighboursToBeSwitchedOff': '0x004504F0',
          'CPathFind::FindLinkBetweenNodes': '0x00451350',
          'CPathFind::FindNearestExteriorNodeToInteriorNode': '0x00450F30',
          'CPathFind::FindNodeClosestToCoorsFavourDirection': '0x0044FCE0',
          'CPathFind::FindNodeCoorsForScript-LinkedNode': '0x004505E0',
          'CPathFind::FindNodeCoorsForScript-TwoNodes': '0x00450780',
          'CPathFind::FindRegionForCoors': '0x0044D830',
          'CPathFind::FindStartPointOfRegion-': '0x0044D930',
          'CPathFind::FindXCoorsForRegion': '0x0044D8F0',
          'CPathFind::FindXRegionForCoors': '0x0044D890',
          'CPathFind::FindYCoorsForRegion': '0x0044D910',
          'CPathFind::FindYRegionForCoors': '0x0044D8C0',
          'CPathFind::GetPathNode': '0x00420AC0',
          'CPathFind::HaveRequestedNodesBeenLoaded': '0x00450DB0',
          'CPathFind::Init': '0x0044D080',
          'CPathFind::IsAreaNodesAvailable': '0x00420AA0',
          'CPathFind::IsWaterNodeNearby': '0x00450DE0',
          'CPathFind::Load': '0x005D3500',
          'CPathFind::LoadPathFindData-Area': '0x00452F40',
          'CPathFind::LoadPathFindData-FromStream': '0x004529F0',
          'CPathFind::LoadSceneForPathNodes': '0x0044DE00',
          'CPathFind::MakeRequestForNodesToBeLoaded': '0x00450D70',
          'CPathFind::MarkRegionsForCoors': '0x0044DB60',
          'CPathFind::MarkRoadNodeAsDontWander': '0x00450560',
          'CPathFind::ReInit': '0x0044E4E0',
          'CPathFind::ReleaseRequestedNodes': '0x0044DD00',
          'CPathFind::RemoveInterior': '0x0044E1A0',
          'CPathFind::RemoveNodeFromList': '0x0044D1B0',
          'CPathFind::ReturnInteriorNodeIndex': '0x00451300',
          'CPathFind::Save': '0x005D34C0',
          'CPathFind::SetLinksBridgeLights': '0x0044D960',
          'CPathFind::SetPathsNeededAtPosition': '0x0044DCD0',
          'CPathFind::Shutdown': '0x00450950',
          'CPathFind::StartNewInterior': '0x0044DE80',
          'CPathFind::SwitchOffNodeAndNeighbours': '0x00452160',
          'CPathFind::SwitchPedRoadsOffInArea': '0x00452F00',
          'CPathFind::SwitchRoadsOffInArea': '0x00452C80',
          'CPathFind::SwitchRoadsOffInAreaForOneRegion': '0x00452820',
          'CPathFind::TakeWidthIntoAccountForWandering': '0x004509A0',
          'CPathFind::TestCrossesRoad': '0x0044D790',
          'CPathFind::TestForPedTrafficLight': '0x0044D480',
          'CPathFind::These2NodesAreAdjacent': '0x0044D230',
          'CPathFind::ThisNodeHasToBeSwitchedOff': '0x0044D3E0',
          'CPathFind::ThisNodeWillLeadIntoADeadEnd': '0x0044D310',
          'CPathFind::TidyUpNodeSwitchesAfterMission': '0x0044D3B0',
          'CPathFind::UnLoadPathFindData': '0x0044D0F0',
          'CPathFind::UnMarkAllRoadNodesAsDontWander': '0x0044D400',
          'CPathFind::UpdateStreaming': '0x00450A60'},
 'types': {'CPathFind::AddDynamicLinkBetween2Nodes_For1Node': 'S',
           'CPathFind::AddInteriorLink': 'S',
           'CPathFind::AddInteriorLinkToExternalNode': 'S',
           'CPathFind::AddNodeToList': 'S',
           'CPathFind::AddNodeToNewInterior': 'S',
           'CPathFind::AreNodesLoadedForArea': 'S',
           'CPathFind::CountNeighboursToBeSwitchedOff': 'S',
           'CPathFind::FindLinkBetweenNodes': 'S',
           'CPathFind::FindNearestExteriorNodeToInteriorNode': 'S',
           'CPathFind::FindNodeClosestToCoorsFavourDirection': 'S',
           'CPathFind::FindNodeCoorsForScript-LinkedNode': 'S',
           'CPathFind::FindNodeCoorsForScript-TwoNodes': 'S',
           'CPathFind::FindRegionForCoors': 'S',
           'CPathFind::FindStartPointOfRegion-': 'S',
           'CPathFind::FindXCoorsForRegion': 'S',
           'CPathFind::FindXRegionForCoors': 'S',
           'CPathFind::FindYCoorsForRegion': 'S',
           'CPathFind::FindYRegionForCoors': 'S',
           'CPathFind::GetPathNode': 'S',
           'CPathFind::HaveRequestedNodesBeenLoaded': 'S',
           'CPathFind::Init': 'S',
           'CPathFind::IsAreaNodesAvailable': 'S',
           'CPathFind::IsWaterNodeNearby': 'S',
           'CPathFind::Load': 'S',
           'CPathFind::LoadPathFindData-Area': 'S',
           'CPathFind::LoadPathFindData-FromStream': 'S',
           'CPathFind::LoadSceneForPathNodes': 'S',
           'CPathFind::MakeRequestForNodesToBeLoaded': 'S',
           'CPathFind::MarkRegionsForCoors': 'S',
           'CPathFind::MarkRoadNodeAsDontWander': 'S',
           'CPathFind::ReInit': 'S',
           'CPathFind::ReleaseRequestedNodes': 'S',
           'CPathFind::RemoveInterior': 'S',
           'CPathFind::RemoveNodeFromList': 'S',
           'CPathFind::ReturnInteriorNodeIndex': 'S',
           'CPathFind::Save': 'S',
           'CPathFind::SetLinksBridgeLights': 'S',
           'CPathFind::SetPathsNeededAtPosition': 'S',
           'CPathFind::Shutdown': 'S',
           'CPathFind::StartNewInterior': 'S',
           'CPathFind::SwitchOffNodeAndNeighbours': 'S',
           'CPathFind::SwitchPedRoadsOffInArea': 'S',
           'CPathFind::SwitchRoadsOffInArea': 'S',
           'CPathFind::SwitchRoadsOffInAreaForOneRegion': 'S',
           'CPathFind::TakeWidthIntoAccountForWandering': 'S',
           'CPathFind::TestCrossesRoad': 'S',
           'CPathFind::TestForPedTrafficLight': 'S',
           'CPathFind::These2NodesAreAdjacent': 'S',
           'CPathFind::ThisNodeHasToBeSwitchedOff': 'S',
           'CPathFind::ThisNodeWillLeadIntoADeadEnd': 'S',
           'CPathFind::TidyUpNodeSwitchesAfterMission': 'S',
           'CPathFind::UnLoadPathFindData': 'S',
           'CPathFind::UnMarkAllRoadNodesAsDontWander': 'S',
           'CPathFind::UpdateStreaming': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'extra_unregistered': [],
 'provenance': 'Exact compiled PathFinal3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and '
               'equal to the working tree at registration; full 26880-row log reaudited offline in streaming and preserved gzipped with the original '
               'hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 26880 '
           'isolated CPU pairs for 54 functions of CPathFind, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural '
           '"complete" event with state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case '
           'index) in both legs and compares the return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 '
           'bits) and the whole trace of doubled collaborators with raw arguments; foreign threads ignored. Traced doubles get NO credit. Only the '
           'own part of each function is validated. Source tested == current working tree at registration (class sources match the probe build '
           'manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Padding and uninitialised bytes '
           'masked. Limits are declared per function in the function-specific scope and in the bank README.',
 'specific': {'CPathFind::AddDynamicLinkBetween2Nodes_For1Node': 'Corpus 1024; source reviewed against the EXE and corrected before the run where '
                                                                 'needed (see git diff). fuente corregida: primer bloque libre (area -1) y '
                                                                 'comparaciones con signo (0x44E000/0x44E043). Limites del banco: ThePaths y las '
                                                                 'globales vecinas se sobrescriben con datos deterministas en un arena propio y se '
                                                                 'restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                                 'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::AddInteriorLink': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'filas llenas incluidas (el EXE escribe en row[6], 0x44DEE6..0x44DEF7); fuente corregida: filas de 6 '
                                            'bytes y primer byte con signo (0x44DED0). Limites del banco: ThePaths y las globales vecinas se '
                                            'sobrescriben con datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                            'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza '
                                            '(sin credito).',
              'CPathFind::AddInteriorLinkToExternalNode': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                          '(see git diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con '
                                                          'datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                                          'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                                          'doblados con traza (sin credito).',
              'CPathFind::AddNodeToList': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                          'fuente corregida: cubo dist & 511 truncado a 16 bits (0x44D1E0 and eax,1FFh; movsx). Limites del banco: '
                                          'ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y se '
                                          'restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free '
                                          'simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::AddNodeToNewInterior': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). fuente corregida: bDontWander en el array 0x96EC40 y filas de 6 bytes de ConnectsToGiven '
                                                 '(0x450E90). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                 'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                 'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin '
                                                 'credito).',
              'CPathFind::AreNodesLoadedForArea': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). fuente corregida: sin CRect, regiones sueltas (0x44DD10). Limites del banco: ThePaths y '
                                                  'las globales vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; '
                                                  'areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free '
                                                  'simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::CountNeighboursToBeSwitchedOff': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed '
                                                           '(see git diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con '
                                                           'datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                                           'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                                           'doblados con traza (sin credito).',
              'CPathFind::FindLinkBetweenNodes': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). solo enlaces existentes o areas sin cargar (el EXE busca sin limite de m_nNumLinks, '
                                                 '0x4513B8..0x4513C7); fuente corregida: array de enlaces del area de la direccion (0x451350). '
                                                 'Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un '
                                                 'arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                 'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::FindNearestExteriorNodeToInteriorNode': 'Corpus 256; source reviewed against the EXE and corrected before the run where '
                                                                  'needed (see git diff). fuente corregida: busqueda sobre ThePaths (0x450F30). '
                                                                  'Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                                  'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                                                  'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                                                  'doblados con traza (sin credito).',
              'CPathFind::FindNodeClosestToCoorsFavourDirection': 'Corpus 1024; source reviewed against the EXE and corrected before the run where '
                                                                  'needed (see git diff). mitad alta de la salida normalizada cuando el area es '
                                                                  '0xFFFF (basura de pila del EXE); fuente reescrita con x87 en orden, mejor '
                                                                  'puntuacion 10000.0 (0x461C4000) (0x44FCE0). Limites del banco: ThePaths y las '
                                                                  'globales vecinas se sobrescriben con datos deterministas en un arena propio y se '
                                                                  'restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                                  'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin '
                                                                  'credito).',
              'CPathFind::FindNodeCoorsForScript-LinkedNode': 'Corpus 1024; source reviewed against the EXE and corrected before the run where '
                                                              'needed (see git diff). fuente corregida: primer enlace en m_pNodeLinks[area][base], '
                                                              'normalizacion 2D (0x44E480), escala ancho*0.0625+2.7 (0x4505E0). Limites del banco: '
                                                              'ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena '
                                                              'propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close '
                                                              'y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::FindNodeCoorsForScript-TwoNodes': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed '
                                                            '(see git diff). fuente corregida: rumbo fpatan(-dx,dy)*57.29578, CVector::Normalise de '
                                                            '3 componentes (0x59C910), perpendicular derecho (0x450780). Limites del banco: ThePaths '
                                                            'y las globales vecinas se sobrescriben con datos deterministas en un arena propio y se '
                                                            'restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                            'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::FindRegionForCoors': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). fuente corregida: (p+3000)*0.0013333333f truncado y acotado a [0,7] (0x44D830). Limites del '
                                               'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y '
                                               'se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                               'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::FindStartPointOfRegion-': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                    'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                    'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza '
                                                    '(sin credito).',
              'CPathFind::FindXCoorsForRegion': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas '
                                                'en un arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::FindXRegionForCoors': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). fuente corregida: (p+3000)*0.0013333333f truncado y acotado a [0,7] (0x44D890). Limites del '
                                                'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y '
                                                'se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::FindYCoorsForRegion': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas '
                                                'en un arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::FindYRegionForCoors': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). fuente corregida: (p+3000)*0.0013333333f truncado y acotado a [0,7] (0x44D8C0). Limites del '
                                                'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y '
                                                'se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::GetPathNode': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                        'Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena '
                                        'propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                        'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::HaveRequestedNodesBeenLoaded': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see '
                                                         'git diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                         'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                         'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con '
                                                         'traza (sin credito).',
              'CPathFind::Init': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). fuente '
                                 'corregida: NumTempExternalNodes es la global 0x96F02C (0x44D080). Limites del banco: ThePaths y las globales '
                                 'vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                 'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin '
                                 'credito).',
              'CPathFind::IsAreaNodesAvailable': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas '
                                                 'en un arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close '
                                                 'y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::IsWaterNodeNearby': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                              'fuente corregida: distancia 2D estricta (0x450E1E..0x450E58). Limites del banco: ThePaths y las '
                                              'globales vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; areas '
                                              'falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; '
                                              'colaboradores doblados con traza (sin credito).',
              'CPathFind::Load': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites del '
                                 'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; '
                                 'areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                 'doblados con traza (sin credito).',
              'CPathFind::LoadPathFindData-Area': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). RwStream, CTimer::Suspend/Resume y Malloc simulados (sprintf real, nombre de fichero y '
                                                  'gString comparados); fuente corregida: sprintf del CRT (0x452F40) y RwStreamClose(stream,0) '
                                                  '(0x452F7D). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                  'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                  'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza '
                                                  '(sin credito).',
              'CPathFind::LoadPathFindData-FromStream': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). grafos de los fixtures con <=1 enlace o bidireccionales (*outNext2 nulo, '
                                                        '0x452250); fichero servido por RwStream simulado; fuente corregida: Malloc/Free del EXE '
                                                        '(0x72F420/0x72F430) y reinicio de 16 enlaces dinamicos del area (0x452C4B). Limites del '
                                                        'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena '
                                                        'propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                        'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::LoadSceneForPathNodes': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). fuente corregida: bucle x exterior, y interior, modelo 0x63A7+x+y*8 (0x44DE48..0x44DE5D). '
                                                  'Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un '
                                                  'arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                  'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::MakeRequestForNodesToBeLoaded': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed '
                                                          '(see git diff). CStreaming::RequestModel/RemoveModel y FindPlayerPed doblados con traza; '
                                                          'sin cambios de fuente. Limites del banco: ThePaths y las globales vecinas se sobrescriben '
                                                          'con datos deterministas en un arena propio y se restauran; areas falsas; pool de '
                                                          'vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; '
                                                          'colaboradores doblados con traza (sin credito).',
              'CPathFind::MarkRegionsForCoors': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). fuente corregida: ToBeStreamed por columnas [x*8+y] con cuatro regiones sueltas (0x44DB60). '
                                                'Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un '
                                                'arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::MarkRoadNodeAsDontWander': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). fuente corregida: solo comprueba el area (0x450560 cmp ax,0FFFFh). Limites del banco: '
                                                     'ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y '
                                                     'se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                     'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::ReInit': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites del '
                                   'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y se '
                                   'restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; '
                                   'colaboradores doblados con traza (sin credito).',
              'CPathFind::ReleaseRequestedNodes': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas '
                                                  'en un arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close '
                                                  'y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::RemoveInterior': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                           'reproduce el desplazamiento erroneo de R* (0x44E272..0x44E27F) con base+k acotado a 4..56; fuente '
                                           'reescrita (0x44E1A0, 0x44E1F1) con CMemoryMgr::Free. Limites del banco: ThePaths y las globales vecinas '
                                           'se sobrescriben con datos deterministas en un arena propio y se restauran; areas falsas; pool de '
                                           'vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con '
                                           'traza (sin credito).',
              'CPathFind::RemoveNodeFromList': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en '
                                               'un arena propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                               'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::ReturnInteriorNodeIndex': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). fuente corregida: sin el parametro unkn (ret 0Ch), 0x451300/0x451321. Limites del banco: '
                                                    'ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y se '
                                                    'restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                    'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::Save': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites del '
                                 'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; '
                                 'areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                 'doblados con traza (sin credito).',
              'CPathFind::SetLinksBridgeLights': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). fuente corregida: limites estrictos x>xMin && x<xMax sin CRect (0x44D960). Limites del '
                                                 'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio '
                                                 'y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                 'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::SetPathsNeededAtPosition': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                     'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                     'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza '
                                                     '(sin credito).',
              'CPathFind::Shutdown': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                     'del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y se '
                                     'restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; '
                                     'colaboradores doblados con traza (sin credito).',
              'CPathFind::StartNewInterior': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena '
                                             'propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                             'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::SwitchOffNodeAndNeighbours': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see '
                                                       'git diff). grafos bidireccionales o con <=1 enlace por nodo porque el EXE escribe en '
                                                       '*outNext2 nulo (0x452248/0x452250); hook de CountNeighboursToBeSwitchedOff apagado durante '
                                                       'esta prueba porque el EXE conserva ECX (0x4521D0); fuente corregida: vecino con '
                                                       'bBackToOriginal (0x4521F4..0x452239) y CountNeighbours sobre el vecino. Limites del banco: '
                                                       'ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena propio y '
                                                       'se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                       'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::SwitchPedRoadsOffInArea': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). grafos bidireccionales o con <=1 enlace por nodo (*outNext2 nulo, 0x452248/0x452250); '
                                                    'sin cambios de fuente. Limites del banco: ThePaths y las globales vecinas se sobrescriben con '
                                                    'datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                    'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza '
                                                    '(sin credito).',
              'CPathFind::SwitchRoadsOffInArea': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). grafos bidireccionales o con <=1 enlace por nodo (*outNext2 nulo, 0x452248/0x452250); '
                                                 'fuente corregida: comparaciones ordenadas como el asm. Limites del banco: ThePaths y las globales '
                                                 'vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; areas falsas; '
                                                 'pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; '
                                                 'colaboradores doblados con traza (sin credito).',
              'CPathFind::SwitchRoadsOffInAreaForOneRegion': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). grafos bidireccionales o con <=1 enlace por nodo (*outNext2 nulo, '
                                                             '0x452248/0x452250); fuente corregida: comparaciones ordenadas como el asm y copia de '
                                                             '0x1C bytes. Limites del banco: ThePaths y las globales vecinas se sobrescriben con '
                                                             'datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                                             'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                                             'doblados con traza (sin credito).',
              'CPathFind::TakeWidthIntoAccountForWandering': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). fuente corregida: & 0xF y >> 4 con signo, solo comprueba el area '
                                                             '(0x4509A0). Limites del banco: ThePaths y las globales vecinas se sobrescriben con '
                                                             'datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                                             'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                                             'doblados con traza (sin credito).',
              'CPathFind::TestCrossesRoad': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena '
                                            'propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                            'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::TestForPedTrafficLight': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                   'diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                   'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                   'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza '
                                                   '(sin credito).',
              'CPathFind::These2NodesAreAdjacent': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                   'diff). fuente corregida: array de enlaces del area de la direccion (0x44D230). Limites del '
                                                   'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena '
                                                   'propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                   'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::ThisNodeHasToBeSwitchedOff': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                       'git diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con datos '
                                                       'deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                       'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con '
                                                       'traza (sin credito).',
              'CPathFind::ThisNodeWillLeadIntoADeadEnd': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see '
                                                         'git diff). fuente corregida: comportamiento 0 y >10 (0x44D36B..0x44D378). Limites del '
                                                         'banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un arena '
                                                         'propio y se restauran; areas falsas; pool de vehiculos falso; RwStreamOpen/Read/Close y '
                                                         'CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito).',
              'CPathFind::TidyUpNodeSwitchesAfterMission': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                           '(see git diff). fuente corregida: minimo con signo (0x44D3B0 cmp eax,36h; jl). Limites '
                                                           'del banco: ThePaths y las globales vecinas se sobrescriben con datos deterministas en un '
                                                           'arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                                           'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con '
                                                           'traza (sin credito).',
              'CPathFind::UnLoadPathFindData': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). fuente corregida: CMemoryMgr::Free (0x44D0F0). Limites del banco: ThePaths y las globales '
                                               'vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; areas falsas; '
                                               'pool de vehiculos falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                               'doblados con traza (sin credito).',
              'CPathFind::UnMarkAllRoadNodesAsDontWander': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                           '(see git diff). Limites del banco: ThePaths y las globales vecinas se sobrescriben con '
                                                           'datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos '
                                                           'falso; RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores '
                                                           'doblados con traza (sin credito).',
              'CPathFind::UpdateStreaming': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'FindPlayerPed solo a traves del doble; fuente corregida: ToBeStreamedForScript global 0x96EF90, pool de '
                                            'vehiculos de la ultima ranura a la primera (0x450B40), rep stos por columnas (0x450CA5/0x450CBF), bucle '
                                            'final x+y*8 (0x450D00..0x450D4D). Limites del banco: ThePaths y las globales vecinas se sobrescriben '
                                            'con datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
                                            'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin '
                                            'credito).'},
 'source_changes': {'source/game_sa/PathFind.cpp': 'Fuente de CPathFind corregida contra el EXE antes de la corrida (correcciones por funcion en el '
                                                   'README del banco y en el alcance de cada funcion; ver git diff)',
                    'source/game_sa/PathFind.h': 'Fuente de CPathFind corregida contra el EXE antes de la corrida (correcciones por funcion en el '
                                                 'README del banco y en el alcance de cada funcion; ver git diff)'},
 'notes': 'PathFinal3 registers 54 functions of CPathFind (26880 pairs, 0 differences). Limits are declared per function; see scope and the bank '
          'README.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-195539-016084`: passed=true, 26880 pares, 54 funciones registradas con 0 fallos, evento complete '
                   '(state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_pathfinal3.py` (evidencia en `runs/`, '
                   '`validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.'}
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
