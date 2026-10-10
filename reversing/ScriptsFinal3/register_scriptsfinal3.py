"""Registro historico de las 69 funciones de CTheScripts del banco ScriptsFinal3 (corrida 20261009-203734-459814). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'ScriptsFinal3',
 'run_id': '20261009-203734-459814',
 'probe_prefix': '7a39303c',
 'tag': 'scriptsfinal3',
 'registry_before': 7613,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CTheScripts::AddScriptCheckpoint': 256,
            'CTheScripts::AddScriptEffectSystem': 128,
            'CTheScripts::AddScriptSearchLight': 256,
            'CTheScripts::AddScriptSphere': 128,
            'CTheScripts::AddToBuildingSwapArray': 256,
            'CTheScripts::AddToInvisibilitySwapArray': 256,
            'CTheScripts::AddToListOfConnectedLodObjects': 256,
            'CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels': 256,
            'CTheScripts::AddToSwitchJumpTable': 128,
            'CTheScripts::AddToVehicleModelsBlockedByScript': 128,
            'CTheScripts::AddToWaitingForScriptBrainArray': 256,
            'CTheScripts::AttachSearchlightToSearchlightObject': 256,
            'CTheScripts::CheckStreamedScriptVersion': 64,
            'CTheScripts::CleanUpThisObject': 128,
            'CTheScripts::CleanUpThisPed': 512,
            'CTheScripts::CleanUpThisVehicle': 128,
            'CTheScripts::ClearAllSuppressedCarModels': 64,
            'CTheScripts::ClearAllVehicleModelsBlockedByScript': 64,
            'CTheScripts::ClearSpaceForMissionEntity': 512,
            'CTheScripts::DoScriptSetupAfterPoolsHaveLoaded': 128,
            'CTheScripts::DrawDebugAngledSquare': 128,
            'CTheScripts::DrawDebugSquare-': 128,
            'CTheScripts::DrawScriptSpheres': 128,
            'CTheScripts::DrawScriptSpritesAndRectangles': 512,
            'CTheScripts::GetActualScriptThingIndex': 1024,
            'CTheScripts::GetNewUniqueScriptThingIndex': 1024,
            'CTheScripts::GetScriptIndexFromPointer': 64,
            'CTheScripts::GetUniqueScriptThingIndex': 128,
            'CTheScripts::HasCarModelBeenSuppressed': 128,
            'CTheScripts::HasVehicleModelBeenBlockedByScript': 128,
            'CTheScripts::HighlightImportantAngledArea': 512,
            'CTheScripts::HighlightImportantArea-': 512,
            'CTheScripts::Init': 256,
            'CTheScripts::InitialiseAllConnectLodObjects': 64,
            'CTheScripts::InitialiseConnectLodObjects': 128,
            'CTheScripts::InitialiseSpecialAnimGroup': 128,
            'CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels': 64,
            'CTheScripts::IsPedStopped': 512,
            'CTheScripts::IsPlayerOnAMission': 128,
            'CTheScripts::IsVehicleStopped': 256,
            'CTheScripts::Load': 128,
            'CTheScripts::PrintListSizes': 64,
            'CTheScripts::Process': 512,
            'CTheScripts::ProcessAllSearchLights': 512,
            'CTheScripts::ProcessWaitingForScriptBrainArray': 512,
            'CTheScripts::ReadMultiScriptFileOffsetsFromScript': 128,
            'CTheScripts::ReadObjectNamesFromScript': 128,
            'CTheScripts::ReinitialiseSwitchStatementData': 64,
            'CTheScripts::RemoveFromWaitingForScriptBrainArray': 256,
            'CTheScripts::RemoveScriptCheckpoint': 256,
            'CTheScripts::RemoveScriptEffectSystem': 128,
            'CTheScripts::RemoveScriptSearchLight': 256,
            'CTheScripts::RemoveScriptSphere': 128,
            'CTheScripts::RemoveScriptTextureDictionary': 128,
            'CTheScripts::RemoveThisPed': 256,
            'CTheScripts::RenderAllSearchLights': 256,
            'CTheScripts::Save': 128,
            'CTheScripts::ScriptAttachAnimGroupToCharModel': 128,
            'CTheScripts::ScriptConnectLodsFunction': 256,
            'CTheScripts::ScriptDebugCircle2D': 256,
            'CTheScripts::ScriptDebugLine3D': 64,
            'CTheScripts::StartNewScript-indexed': 256,
            'CTheScripts::StartNewScript-last-idle': 256,
            'CTheScripts::StartTestScript': 128,
            'CTheScripts::UndoBuildingSwaps': 128,
            'CTheScripts::UndoEntityInvisibilitySettings': 128,
            'CTheScripts::UpdateObjectIndices': 256,
            'CTheScripts::UseSwitchJumpTable': 512,
            'CTheScripts::WipeLocalVariableMemoryForMissionScript': 64},
 'total': 16384,
 'new': ['CTheScripts::AddScriptCheckpoint',
         'CTheScripts::AddScriptEffectSystem',
         'CTheScripts::AddScriptSearchLight',
         'CTheScripts::AddScriptSphere',
         'CTheScripts::AddToBuildingSwapArray',
         'CTheScripts::AddToInvisibilitySwapArray',
         'CTheScripts::AddToListOfConnectedLodObjects',
         'CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels',
         'CTheScripts::AddToSwitchJumpTable',
         'CTheScripts::AddToVehicleModelsBlockedByScript',
         'CTheScripts::AddToWaitingForScriptBrainArray',
         'CTheScripts::AttachSearchlightToSearchlightObject',
         'CTheScripts::CheckStreamedScriptVersion',
         'CTheScripts::CleanUpThisObject',
         'CTheScripts::CleanUpThisPed',
         'CTheScripts::CleanUpThisVehicle',
         'CTheScripts::ClearAllSuppressedCarModels',
         'CTheScripts::ClearAllVehicleModelsBlockedByScript',
         'CTheScripts::ClearSpaceForMissionEntity',
         'CTheScripts::DoScriptSetupAfterPoolsHaveLoaded',
         'CTheScripts::DrawDebugAngledSquare',
         'CTheScripts::DrawDebugSquare-',
         'CTheScripts::DrawScriptSpheres',
         'CTheScripts::DrawScriptSpritesAndRectangles',
         'CTheScripts::GetActualScriptThingIndex',
         'CTheScripts::GetNewUniqueScriptThingIndex',
         'CTheScripts::GetScriptIndexFromPointer',
         'CTheScripts::GetUniqueScriptThingIndex',
         'CTheScripts::HasCarModelBeenSuppressed',
         'CTheScripts::HasVehicleModelBeenBlockedByScript',
         'CTheScripts::HighlightImportantAngledArea',
         'CTheScripts::HighlightImportantArea-',
         'CTheScripts::Init',
         'CTheScripts::InitialiseAllConnectLodObjects',
         'CTheScripts::InitialiseConnectLodObjects',
         'CTheScripts::InitialiseSpecialAnimGroup',
         'CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels',
         'CTheScripts::IsPedStopped',
         'CTheScripts::IsPlayerOnAMission',
         'CTheScripts::IsVehicleStopped',
         'CTheScripts::Load',
         'CTheScripts::PrintListSizes',
         'CTheScripts::Process',
         'CTheScripts::ProcessAllSearchLights',
         'CTheScripts::ProcessWaitingForScriptBrainArray',
         'CTheScripts::ReadMultiScriptFileOffsetsFromScript',
         'CTheScripts::ReadObjectNamesFromScript',
         'CTheScripts::ReinitialiseSwitchStatementData',
         'CTheScripts::RemoveFromWaitingForScriptBrainArray',
         'CTheScripts::RemoveScriptCheckpoint',
         'CTheScripts::RemoveScriptEffectSystem',
         'CTheScripts::RemoveScriptSearchLight',
         'CTheScripts::RemoveScriptSphere',
         'CTheScripts::RemoveScriptTextureDictionary',
         'CTheScripts::RemoveThisPed',
         'CTheScripts::RenderAllSearchLights',
         'CTheScripts::Save',
         'CTheScripts::ScriptAttachAnimGroupToCharModel',
         'CTheScripts::ScriptConnectLodsFunction',
         'CTheScripts::ScriptDebugCircle2D',
         'CTheScripts::ScriptDebugLine3D',
         'CTheScripts::StartNewScript-indexed',
         'CTheScripts::StartNewScript-last-idle',
         'CTheScripts::StartTestScript',
         'CTheScripts::UndoBuildingSwaps',
         'CTheScripts::UndoEntityInvisibilitySettings',
         'CTheScripts::UpdateObjectIndices',
         'CTheScripts::UseSwitchJumpTable',
         'CTheScripts::WipeLocalVariableMemoryForMissionScript'],
 'rejected': [],
 'addr': {'CTheScripts::AddScriptCheckpoint': '0x004935A0',
          'CTheScripts::AddScriptEffectSystem': '0x00492F90',
          'CTheScripts::AddScriptSearchLight': '0x00493000',
          'CTheScripts::AddScriptSphere': '0x00483B30',
          'CTheScripts::AddToBuildingSwapArray': '0x00481140',
          'CTheScripts::AddToInvisibilitySwapArray': '0x00481200',
          'CTheScripts::AddToListOfConnectedLodObjects': '0x00470980',
          'CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels': '0x00474750',
          'CTheScripts::AddToSwitchJumpTable': '0x00470390',
          'CTheScripts::AddToVehicleModelsBlockedByScript': '0x0046B200',
          'CTheScripts::AddToWaitingForScriptBrainArray': '0x0046AB60',
          'CTheScripts::AttachSearchlightToSearchlightObject': '0x004934F0',
          'CTheScripts::CheckStreamedScriptVersion': '0x00464FF0',
          'CTheScripts::CleanUpThisObject': '0x004866C0',
          'CTheScripts::CleanUpThisPed': '0x00486300',
          'CTheScripts::CleanUpThisVehicle': '0x00486670',
          'CTheScripts::ClearAllSuppressedCarModels': '0x0046A7C0',
          'CTheScripts::ClearAllVehicleModelsBlockedByScript': '0x0046A840',
          'CTheScripts::ClearSpaceForMissionEntity': '0x00486B00',
          'CTheScripts::DoScriptSetupAfterPoolsHaveLoaded': '0x005D3390',
          'CTheScripts::DrawDebugAngledSquare': '0x00486990',
          'CTheScripts::DrawDebugSquare-': '0x00486840',
          'CTheScripts::DrawScriptSpheres': '0x004810E0',
          'CTheScripts::DrawScriptSpritesAndRectangles': '0x00464980',
          'CTheScripts::GetActualScriptThingIndex': '0x004839A0',
          'CTheScripts::GetNewUniqueScriptThingIndex': '0x00483720',
          'CTheScripts::GetScriptIndexFromPointer': '0x00464D20',
          'CTheScripts::GetUniqueScriptThingIndex': '0x004810C0',
          'CTheScripts::HasCarModelBeenSuppressed': '0x0046A810',
          'CTheScripts::HasVehicleModelBeenBlockedByScript': '0x0046A890',
          'CTheScripts::HighlightImportantAngledArea': '0x00485EF0',
          'CTheScripts::HighlightImportantArea-': '0x00485E00',
          'CTheScripts::Init': '0x00468D50',
          'CTheScripts::InitialiseAllConnectLodObjects': '0x00470960',
          'CTheScripts::InitialiseConnectLodObjects': '0x00470940',
          'CTheScripts::InitialiseSpecialAnimGroup': '0x00474710',
          'CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels': '0x00474730',
          'CTheScripts::IsPedStopped': '0x00486110',
          'CTheScripts::IsPlayerOnAMission': '0x00464D50',
          'CTheScripts::IsVehicleStopped': '0x004861F0',
          'CTheScripts::Load': '0x005D4FD0',
          'CTheScripts::PrintListSizes': '0x004646D0',
          'CTheScripts::Process': '0x0046A000',
          'CTheScripts::ProcessAllSearchLights': '0x004939F0',
          'CTheScripts::ProcessWaitingForScriptBrainArray': '0x0046CF00',
          'CTheScripts::ReadMultiScriptFileOffsetsFromScript': '0x004867C0',
          'CTheScripts::ReadObjectNamesFromScript': '0x00486720',
          'CTheScripts::ReinitialiseSwitchStatementData': '0x00470370',
          'CTheScripts::RemoveFromWaitingForScriptBrainArray': '0x0046ABC0',
          'CTheScripts::RemoveScriptCheckpoint': '0x004936C0',
          'CTheScripts::RemoveScriptEffectSystem': '0x00492FD0',
          'CTheScripts::RemoveScriptSearchLight': '0x00493160',
          'CTheScripts::RemoveScriptSphere': '0x00483BA0',
          'CTheScripts::RemoveScriptTextureDictionary': '0x00465A40',
          'CTheScripts::RemoveThisPed': '0x00486240',
          'CTheScripts::RenderAllSearchLights': '0x00493E30',
          'CTheScripts::Save': '0x005D4C40',
          'CTheScripts::ScriptAttachAnimGroupToCharModel': '0x00474800',
          'CTheScripts::ScriptConnectLodsFunction': '0x00470A20',
          'CTheScripts::ScriptDebugCircle2D': '0x00485C20',
          'CTheScripts::ScriptDebugLine3D': '0x00485DE0',
          'CTheScripts::StartNewScript-indexed': '0x00464C90',
          'CTheScripts::StartNewScript-last-idle': '0x00464C20',
          'CTheScripts::StartTestScript': '0x00464D40',
          'CTheScripts::UndoBuildingSwaps': '0x00481290',
          'CTheScripts::UndoEntityInvisibilitySettings': '0x004812D0',
          'CTheScripts::UpdateObjectIndices': '0x00486780',
          'CTheScripts::UseSwitchJumpTable': '0x004703C0',
          'CTheScripts::WipeLocalVariableMemoryForMissionScript': '0x00464BB0'},
 'types': {'CTheScripts::AddScriptCheckpoint': 'S',
           'CTheScripts::AddScriptEffectSystem': 'S',
           'CTheScripts::AddScriptSearchLight': 'S',
           'CTheScripts::AddScriptSphere': 'S',
           'CTheScripts::AddToBuildingSwapArray': 'S',
           'CTheScripts::AddToInvisibilitySwapArray': 'S',
           'CTheScripts::AddToListOfConnectedLodObjects': 'S',
           'CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels': 'S',
           'CTheScripts::AddToSwitchJumpTable': 'S',
           'CTheScripts::AddToVehicleModelsBlockedByScript': 'S',
           'CTheScripts::AddToWaitingForScriptBrainArray': 'S',
           'CTheScripts::AttachSearchlightToSearchlightObject': 'S',
           'CTheScripts::CheckStreamedScriptVersion': 'S',
           'CTheScripts::CleanUpThisObject': 'S',
           'CTheScripts::CleanUpThisPed': 'S',
           'CTheScripts::CleanUpThisVehicle': 'S',
           'CTheScripts::ClearAllSuppressedCarModels': 'S',
           'CTheScripts::ClearAllVehicleModelsBlockedByScript': 'S',
           'CTheScripts::ClearSpaceForMissionEntity': 'S',
           'CTheScripts::DoScriptSetupAfterPoolsHaveLoaded': 'S',
           'CTheScripts::DrawDebugAngledSquare': 'S',
           'CTheScripts::DrawDebugSquare-': 'S',
           'CTheScripts::DrawScriptSpheres': 'S',
           'CTheScripts::DrawScriptSpritesAndRectangles': 'S',
           'CTheScripts::GetActualScriptThingIndex': 'S',
           'CTheScripts::GetNewUniqueScriptThingIndex': 'S',
           'CTheScripts::GetScriptIndexFromPointer': 'S',
           'CTheScripts::GetUniqueScriptThingIndex': 'S',
           'CTheScripts::HasCarModelBeenSuppressed': 'S',
           'CTheScripts::HasVehicleModelBeenBlockedByScript': 'S',
           'CTheScripts::HighlightImportantAngledArea': 'S',
           'CTheScripts::HighlightImportantArea-': 'S',
           'CTheScripts::Init': 'S',
           'CTheScripts::InitialiseAllConnectLodObjects': 'S',
           'CTheScripts::InitialiseConnectLodObjects': 'S',
           'CTheScripts::InitialiseSpecialAnimGroup': 'S',
           'CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels': 'S',
           'CTheScripts::IsPedStopped': 'S',
           'CTheScripts::IsPlayerOnAMission': 'S',
           'CTheScripts::IsVehicleStopped': 'S',
           'CTheScripts::Load': 'S',
           'CTheScripts::PrintListSizes': 'S',
           'CTheScripts::Process': 'S',
           'CTheScripts::ProcessAllSearchLights': 'S',
           'CTheScripts::ProcessWaitingForScriptBrainArray': 'S',
           'CTheScripts::ReadMultiScriptFileOffsetsFromScript': 'S',
           'CTheScripts::ReadObjectNamesFromScript': 'S',
           'CTheScripts::ReinitialiseSwitchStatementData': 'S',
           'CTheScripts::RemoveFromWaitingForScriptBrainArray': 'S',
           'CTheScripts::RemoveScriptCheckpoint': 'S',
           'CTheScripts::RemoveScriptEffectSystem': 'S',
           'CTheScripts::RemoveScriptSearchLight': 'S',
           'CTheScripts::RemoveScriptSphere': 'S',
           'CTheScripts::RemoveScriptTextureDictionary': 'S',
           'CTheScripts::RemoveThisPed': 'S',
           'CTheScripts::RenderAllSearchLights': 'S',
           'CTheScripts::Save': 'S',
           'CTheScripts::ScriptAttachAnimGroupToCharModel': 'S',
           'CTheScripts::ScriptConnectLodsFunction': 'S',
           'CTheScripts::ScriptDebugCircle2D': 'S',
           'CTheScripts::ScriptDebugLine3D': 'S',
           'CTheScripts::StartNewScript-indexed': 'S',
           'CTheScripts::StartNewScript-last-idle': 'S',
           'CTheScripts::StartTestScript': 'S',
           'CTheScripts::UndoBuildingSwaps': 'S',
           'CTheScripts::UndoEntityInvisibilitySettings': 'S',
           'CTheScripts::UpdateObjectIndices': 'S',
           'CTheScripts::UseSwitchJumpTable': 'S',
           'CTheScripts::WipeLocalVariableMemoryForMissionScript': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'extra_unregistered': [],
 'provenance': 'Exact compiled ScriptsFinal3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and '
               'equal to the working tree at registration; full 16384-row log reaudited offline in streaming and preserved gzipped with the original '
               'hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 16384 '
           'isolated CPU pairs for 69 functions of CTheScripts, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural '
           '"complete" event with state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case '
           'index) in both legs and compares the return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 '
           'bits) and the whole trace of doubled collaborators with raw arguments; foreign threads ignored. Traced doubles get NO credit. Only the '
           'own part of each function is validated. Source tested == current working tree at registration (class sources match the probe build '
           'manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Padding and uninitialised bytes '
           'masked. Limits are declared per function in the function-specific scope and in the bank README.',
 'specific': {'CTheScripts::AddScriptCheckpoint': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                  'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                  'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                  'doblados con traza y sin credito.',
              'CTheScripts::AddScriptEffectSystem': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                    'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                    '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                    'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::AddScriptSearchLight': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                   'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                   'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                   'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                   'doblados con traza y sin credito.',
              'CTheScripts::AddScriptSphere': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                              'Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                              'eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de '
                                              'script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con '
                                              'traza y sin credito.',
              'CTheScripts::AddToBuildingSwapArray': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                     'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                     '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                     'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::AddToInvisibilitySwapArray': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see '
                                                         'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                         'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                         '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                         'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::AddToListOfConnectedLodObjects': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                             'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                             'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                             'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels': 'Corpus 256; source reviewed against the EXE and corrected before the '
                                                                               'run where needed (see git diff). Limites del banco: pools de peds, '
                                                                               'edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                                                               'eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                                               '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado '
                                                                               'en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza '
                                                                               'y sin credito.',
              'CTheScripts::AddToSwitchJumpTable': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                   'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                   'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                   'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                   'doblados con traza y sin credito.',
              'CTheScripts::AddToVehicleModelsBlockedByScript': 'Corpus 128; source reviewed against the EXE and corrected before the run where '
                                                                'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                                                'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, '
                                                                'flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con '
                                                                'traza y sin credito.',
              'CTheScripts::AddToWaitingForScriptBrainArray': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                              '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                              'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                              'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                              'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::AttachSearchlightToSearchlightObject': 'Corpus 256; source reviewed against the EXE and corrected before the run where '
                                                                   'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                   'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                                                   'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, '
                                                                   'CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                                   'doblados con traza y sin credito.',
              'CTheScripts::CheckStreamedScriptVersion': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see '
                                                         'git diff). ficheros doblados. Limites del banco: pools de peds, edificios, objetos, '
                                                         'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales '
                                                         'falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado '
                                                         'en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::CleanUpThisObject': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                'doblados con traza y sin credito.',
              'CTheScripts::CleanUpThisPed': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'cola del EXE 0x486522..0x48663F (CTaskComplexSequence/LeaveAnyCar/Wander y CEventScriptCommand con '
                                             'call 61A5A0) NO validada por el banco: los constructores de tareas son en linea y el destructor de '
                                             'CEventScriptCommand no tiene direccion tomable; el fixture solo llega a ramas donde la tarea buscada '
                                             'ya existe y la correccion del orden de reserva queda sin validar. Limites del banco: pools de peds, '
                                             'edificios, objetos, dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                             'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado '
                                             'en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::CleanUpThisVehicle': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                 'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                 'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                 'doblados con traza y sin credito.',
              'CTheScripts::ClearAllSuppressedCarModels': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see '
                                                          'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                          'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                          '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                          'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::ClearAllVehicleModelsBlockedByScript': 'Corpus 64; source reviewed against the EXE and corrected before the run where '
                                                                   'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                   'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                                                   'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, '
                                                                   'CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                                   'doblados con traza y sin credito.',
              'CTheScripts::ClearSpaceForMissionEntity': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see '
                                                         'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                         'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                         '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                         'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::DoScriptSetupAfterPoolsHaveLoaded': 'Corpus 128; source reviewed against the EXE and corrected before the run where '
                                                                'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                                                'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, '
                                                                'flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con '
                                                                'traza y sin credito.',
              'CTheScripts::DrawDebugAngledSquare': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                    'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                    '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                    'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::DrawDebugSquare-': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                               'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                               'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                               'doblados con traza y sin credito.',
              'CTheScripts::DrawScriptSpheres': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                'doblados con traza y sin credito.',
              'CTheScripts::DrawScriptSpritesAndRectangles': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). rectangulos de script con ScriptRawRect; dibujo doblado. Limites del '
                                                             'banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                                             'eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, '
                                                             'mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, '
                                                             'sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::GetActualScriptThingIndex': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                        'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                        '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                        'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::GetNewUniqueScriptThingIndex': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed '
                                                           '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                           'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                           'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                           'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::GetScriptIndexFromPointer': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                        'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                        '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                        'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::GetUniqueScriptThingIndex': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                        'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                        '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                        'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::HasCarModelBeenSuppressed': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                        'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                        '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                        'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::HasVehicleModelBeenBlockedByScript': 'Corpus 128; source reviewed against the EXE and corrected before the run where '
                                                                 'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                 'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                                                 'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, '
                                                                 'flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con '
                                                                 'traza y sin credito.',
              'CTheScripts::HighlightImportantAngledArea': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed '
                                                           '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                           'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                           'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                           'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::HighlightImportantArea-': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see '
                                                      'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                      'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                      '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                      'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::Init': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). main.scm '
                                   'sintetico servido por lectura de fichero simulada; rama de paquete de misiones (CheckMissionPackValidMenu '
                                   '0x468E30) no probada; TXD solo con ranuras usadas (0x46925C). Limites del banco: pools de peds, edificios, '
                                   'objetos, dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                   'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, '
                                   'sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::InitialiseAllConnectLodObjects': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                             'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                             'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                             'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::InitialiseConnectLodObjects': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                          '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                          'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                          'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                          'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::InitialiseSpecialAnimGroup': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                         'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                         'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                         '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                         'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels': 'Corpus 64; source reviewed against the EXE and corrected before the '
                                                                              'run where needed (see git diff). Limites del banco: pools de peds, '
                                                                              'edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                                                              'eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                                              '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado '
                                                                              'en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y '
                                                                              'sin credito.',
              'CTheScripts::IsPedStopped': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                           'Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                           'eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de '
                                           'script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza '
                                           'y sin credito.',
              'CTheScripts::IsPlayerOnAMission': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                 'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                 'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                 'doblados con traza y sin credito.',
              'CTheScripts::IsVehicleStopped': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                               'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                               'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                               'doblados con traza y sin credito.',
              'CTheScripts::Load': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). siempre '
                                   'con al menos un script activo (el EXE desreferencia pActiveScripts nulo, 0x5D4F13); flujo grabado antes con el '
                                   'Save del EXE y consumido en memoria. Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor '
                                   'RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                   'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con '
                                   'traza y sin credito.',
              'CTheScripts::PrintListSizes': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                             'eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de '
                                             'script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con '
                                             'traza y sin credito.',
              'CTheScripts::Process': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                      'del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, eventos y grupos '
                                      'falsos con tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de '
                                      'guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::ProcessAllSearchLights': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                     'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                     '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                     'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::ProcessWaitingForScriptBrainArray': 'Corpus 512; source reviewed against the EXE and corrected before the run where '
                                                                'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                                                'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, '
                                                                'flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con '
                                                                'traza y sin credito.',
              'CTheScripts::ReadMultiScriptFileOffsetsFromScript': 'Corpus 128; source reviewed against the EXE and corrected before the run where '
                                                                   'needed (see git diff). ficheros doblados (CFileMgr simulado). Limites del banco: '
                                                                   'pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                                                   'eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, '
                                                                   'mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                                   'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::ReadObjectNamesFromScript': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). ficheros doblados (CFileMgr simulado). Limites del banco: pools de peds, '
                                                        'edificios, objetos, dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con '
                                                        'tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, '
                                                        'flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin '
                                                        'credito.',
              'CTheScripts::ReinitialiseSwitchStatementData': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed '
                                                              '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                              'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                              'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                              'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::RemoveFromWaitingForScriptBrainArray': 'Corpus 256; source reviewed against the EXE and corrected before the run where '
                                                                   'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                   'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas '
                                                                   'virtuales falsas; colaboradores (referencias, mundo, cerebros de script, '
                                                                   'CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                                   'doblados con traza y sin credito.',
              'CTheScripts::RemoveScriptCheckpoint': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                     'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                     '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                     'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::RemoveScriptEffectSystem': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                       'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                       'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                       '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                       'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::RemoveScriptSearchLight': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see '
                                                      'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                      'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                      '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                      'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::RemoveScriptSphere': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                 'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                 'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                 'doblados con traza y sin credito.',
              'CTheScripts::RemoveScriptTextureDictionary': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                            '(see git diff). TXD solo con ranuras usadas (0x465A8C). Limites del banco: pools de '
                                                            'peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, eventos y grupos '
                                                            'falsos con tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de '
                                                            'script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                            'doblados con traza y sin credito.',
              'CTheScripts::RemoveThisPed': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, '
                                            'eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de '
                                            'script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza '
                                            'y sin credito.',
              'CTheScripts::RenderAllSearchLights': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                    'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                    '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                    'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::Save': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). siempre '
                                   'con al menos un script activo porque el EXE desreferencia pActiveScripts nulo (0x5D4F13); flujo solo en memoria. '
                                   'Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, tareas, eventos y '
                                   'grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, '
                                   'flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::ScriptAttachAnimGroupToCharModel': 'Corpus 128; source reviewed against the EXE and corrected before the run where '
                                                               'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, dummies '
                                                               'y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales '
                                                               'falsas; colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de '
                                                               'guardado en memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin '
                                                               'credito.',
              'CTheScripts::ScriptConnectLodsFunction': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                                        'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores '
                                                        '(referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, '
                                                        'marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::ScriptDebugCircle2D': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                  'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                  'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                  'doblados con traza y sin credito.',
              'CTheScripts::ScriptDebugLine3D': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                'doblados con traza y sin credito.',
              'CTheScripts::StartNewScript-indexed': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). ficheros doblados. Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                     'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                     'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, '
                                                     'TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::StartNewScript-last-idle': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see '
                                                       'git diff). ficheros doblados. Limites del banco: pools de peds, edificios, objetos, dummies '
                                                       'y TXD, motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                       'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                       'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::StartTestScript': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                              'ficheros doblados. Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, '
                                              'entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, '
                                              'mundo, cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo '
                                              '2D) doblados con traza y sin credito.',
              'CTheScripts::UndoBuildingSwaps': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                'doblados con traza y sin credito.',
              'CTheScripts::UndoEntityInvisibilitySettings': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, '
                                                             'motor RW, entidades, tareas, eventos y grupos falsos con tablas virtuales falsas; '
                                                             'colaboradores (referencias, mundo, cerebros de script, CFileMgr, flujo de guardado en '
                                                             'memoria, TXD, marcadores, sombras, dibujo 2D) doblados con traza y sin credito.',
              'CTheScripts::UpdateObjectIndices': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                  'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                  'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                  'doblados con traza y sin credito.',
              'CTheScripts::UseSwitchJumpTable': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). Limites del banco: pools de peds, edificios, objetos, dummies y TXD, motor RW, entidades, '
                                                 'tareas, eventos y grupos falsos con tablas virtuales falsas; colaboradores (referencias, mundo, '
                                                 'cerebros de script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, dibujo 2D) '
                                                 'doblados con traza y sin credito.',
              'CTheScripts::WipeLocalVariableMemoryForMissionScript': 'Corpus 64; source reviewed against the EXE and corrected before the run where '
                                                                      'needed (see git diff). Limites del banco: pools de peds, edificios, objetos, '
                                                                      'dummies y TXD, motor RW, entidades, tareas, eventos y grupos falsos con '
                                                                      'tablas virtuales falsas; colaboradores (referencias, mundo, cerebros de '
                                                                      'script, CFileMgr, flujo de guardado en memoria, TXD, marcadores, sombras, '
                                                                      'dibujo 2D) doblados con traza y sin credito.'},
 'source_changes': {'source/game_sa/Scripts/TheScripts.cpp': 'Fuente de CTheScripts corregida contra el EXE antes de la corrida (correcciones por '
                                                             'funcion en el README del banco y en el alcance de cada funcion; ver git diff)',
                    'source/game_sa/Scripts/TheScripts.h': 'Fuente de CTheScripts corregida contra el EXE antes de la corrida (correcciones por '
                                                           'funcion en el README del banco y en el alcance de cada funcion; ver git diff)'},
 'notes': 'ScriptsFinal3 registers 69 functions of CTheScripts (16384 pairs, 0 differences). Limits are declared per function; see scope and the '
          'bank README.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-203734-459814`: passed=true, 16384 pares, 69 funciones registradas con 0 fallos, evento complete '
                   '(state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_scriptsfinal3.py` (evidencia en `runs/`, '
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
assert layout is not None and layout.get('ok') is True and events.get('layout')==1 and begin is not None and events.get('begin')==1 and begin.get('known') is True and isinstance(begin.get('sites'),list) and len(begin['sites'])>0
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
