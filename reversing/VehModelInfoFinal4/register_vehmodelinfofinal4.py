"""Registro historico de las 75 funciones de CVehicleModelInfo del banco VehModelInfoFinal4 (corrida 20261010-005102-024078). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'addr': {'CVehicleModelInfo::AddRemap': '0x004C86D0',
          'CVehicleModelInfo::AddWheelUpgrade': '0x004C8700',
          'CVehicleModelInfo::AssignRemapTxd': '0x004C9360',
          'CVehicleModelInfo::ChooseComponent': '0x004C8040',
          'CVehicleModelInfo::ChooseComponent-Global': '0x004C7FB0',
          'CVehicleModelInfo::ChooseSecondComponent': '0x004C8120',
          'CVehicleModelInfo::ChooseVehicleColour': '0x004C8500',
          'CVehicleModelInfo::ClearAtomicFlagCB': '0x004C7BB0',
          'CVehicleModelInfo::CollapseFramesCB': '0x004C8E30',
          'CVehicleModelInfo::ConvertAnimFileIndex': '0x00000000',
          'CVehicleModelInfo::CountCompsInRule': '0x004C7F80',
          'CVehicleModelInfo::CreateInstance': '0x00000000',
          'CVehicleModelInfo::DeleteRwObject': '0x00000000',
          'CVehicleModelInfo::DeleteVehicleColourTextures': '0x004C8770',
          'CVehicleModelInfo::DisableEnvMap': '0x004C97E0',
          'CVehicleModelInfo::FindTextureCB': '0x004C7510',
          'CVehicleModelInfo::GetAnimFileIndex': '0x00000000',
          'CVehicleModelInfo::GetCustomCarPlateText': '0x004C8970',
          'CVehicleModelInfo::GetListOfComponentsNotUsedByRules': '0x004C7E50',
          'CVehicleModelInfo::GetMatFXEffectMaterialCB': '0x004C8810',
          'CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors': '0x004C89B0',
          'CVehicleModelInfo::GetModelType': '0x00000000',
          'CVehicleModelInfo::GetNumDoors': '0x004C73C0',
          'CVehicleModelInfo::GetNumRemaps': '0x004C86B0',
          'CVehicleModelInfo::GetNumWheelUpgrades': '0x004C8740',
          'CVehicleModelInfo::GetOkAndDamagedAtomicCB': '0x004C7BD0',
          'CVehicleModelInfo::GetOriginalCompPosition': '0x004C7DD0',
          'CVehicleModelInfo::GetWheelPosn': '0x004C7D20',
          'CVehicleModelInfo::GetWheelUpgrade': '0x004C8750',
          'CVehicleModelInfo::HasAlphaMaterialCB': '0x004C77C0',
          'CVehicleModelInfo::HideAllComponentsAtomicCB': '0x004C7790',
          'CVehicleModelInfo::HideDamagedAtomicCB': '0x004C7720',
          'CVehicleModelInfo::Init': '0x00000000',
          'CVehicleModelInfo::IsUpgradeAvailable': '0x004C8200',
          'CVehicleModelInfo::IsValidCompRule': '0x004C7E10',
          'CVehicleModelInfo::LoadEnvironmentMaps': '0x004C8780',
          'CVehicleModelInfo::LoadVehicleColours': '0x005B6890',
          'CVehicleModelInfo::LoadVehicleUpgrades': '0x005B65A0',
          'CVehicleModelInfo::MoveObjectsCB': '0x004C7700',
          'CVehicleModelInfo::PreprocessHierarchy': '0x004C8E60',
          'CVehicleModelInfo::ReduceMaterialsInVehicle': '0x004C8BD0',
          'CVehicleModelInfo::RemoveWindowAlphaCB': '0x004C83B0',
          'CVehicleModelInfo::ResetEditableMaterials': '0x004C8460',
          'CVehicleModelInfo::SetAnimFile': '0x00000000',
          'CVehicleModelInfo::SetAtomicFlagCB': '0x004C7B90',
          'CVehicleModelInfo::SetAtomicRenderCallbacks': '0x004C7B10',
          'CVehicleModelInfo::SetAtomicRendererCB': '0x004C77E0',
          'CVehicleModelInfo::SetAtomicRendererCB_Boat': '0x004C79A0',
          'CVehicleModelInfo::SetAtomicRendererCB_Heli': '0x004C7A30',
          'CVehicleModelInfo::SetAtomicRendererCB_Plane': '0x004C7930',
          'CVehicleModelInfo::SetAtomicRendererCB_RealHeli': '0x004C7870',
          'CVehicleModelInfo::SetAtomicRendererCB_Train': '0x004C7AA0',
          'CVehicleModelInfo::SetCarCustomPlate': '0x004C9450',
          'CVehicleModelInfo::SetClump': '0x00000000',
          'CVehicleModelInfo::SetCustomCarPlateText': '0x004C8980',
          'CVehicleModelInfo::SetDirtTextures': '0x005D5DB0',
          'CVehicleModelInfo::SetEditableMaterials': '0x004C8430',
          'CVehicleModelInfo::SetEditableMaterialsCB-RpAtomic': '0x004C83E0',
          'CVehicleModelInfo::SetEditableMaterialsCB-RpMaterial': '0x004C8220',
          'CVehicleModelInfo::SetEnvMapCoeff': '0x004C9800',
          'CVehicleModelInfo::SetEnvMapCoeffAtomicCB': '0x004C9430',
          'CVehicleModelInfo::SetEnvMapCoeffCB': '0x004C88B0',
          'CVehicleModelInfo::SetEnvironmentMapAtomicCB': '0x004C9410',
          'CVehicleModelInfo::SetEnvironmentMapCB': '0x004C8840',
          'CVehicleModelInfo::SetRenderPipelines': '0x004C8900',
          'CVehicleModelInfo::SetRenderPipelinesCB': '0x004C88F0',
          'CVehicleModelInfo::SetVehicleColour': '0x004C84B0',
          'CVehicleModelInfo::SetVehicleComponentFlags': '0x004C7C10',
          'CVehicleModelInfo::SetupCommonData': '0x005B8F00',
          'CVehicleModelInfo::SetupLightFlags': '0x004C8C90',
          'CVehicleModelInfo::ShutdownEnvironmentMaps': '0x004C87D0',
          'CVehicleModelInfo::ShutdownLightTexture': '0x004C7470',
          'CVehicleModelInfo::StopUsingCommonVehicleTexDicationary': '0x004C75C0',
          'CVehicleModelInfo::StoreAtomicUsedMaterialsCB': '0x004C8B60',
          'CVehicleModelInfo::UseCommonVehicleTexDicationary': '0x004C75A0'},
 'allow_bank_change': [],
 'allow_src_change': [],
 'bad': {},
 'bank': 'VehModelInfoFinal4',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the hooks on), '
           '16384 isolated CPU pairs for the 75 functions of CVehicleModelInfo, ALL pairs equal, pair counts equal to the bank table, '
           'passed=true globally, natural "complete" event with state_restored and layout_ok, saves unchanged, configs restored. Each case '
           'rebuilds the same deterministic fixture in both legs and compares the return value, the trace of doubled collaborators with '
           'raw arguments, the hash of the fake world, of the arena and of the saved globals, plus per-function fields; the x87 control '
           'word is 24 bits; foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. '
           'Source tested == current working tree at registration (class sources match the probe build manifest). Historical evidence '
           'approved with explicit limits; no current dependency accreditation. Bank limits (VehModelInfoFinal4): static fake world (model '
           'infos with the EXE vtable, CVehicleStructure in a fake pool at 0xB4E680, fake TXD 0xC8800C, clumps with type byte 2, frame '
           'trees, atomics, geometries, meshes, materials and textures; plugin offsets read from the EXE); traced stubs at the EXE address '
           'and at the DLL symbol for RenderWare primitives, CFileMgr/CFileLoader, CTxdStore, CModelInfo::GetModelInfo, random numbers '
           '(0x821B1E + CGeneral::GetRandomNumber), operator new/delete, CTimer, scratchpad, the CClumpModelInfo base and the class '
           'subfunctions inside CreateInstance/SetClump, none of which gets credit. Masked: SetCarCustomPlate (the EXE passes SetupClump a '
           'dword with garbage in the 3 high bytes; only the low byte is traced); SetupCommonData pool byte 0x11 (m_DealWithNoMemory) and '
           'the DLL 0xFD filler are not compared (header and states only); the fake CVehicleStructure pool storage is zeroed before '
           'SetClump. Outside the fixture: CountCompsInRule with negatives (infinite loop in the EXE), rule nibbles 6..14 (overflow the '
           'local list), GetMaximumNumberOfPassengersFromNumberOfDoors(-1) (reads ms_modelInfoPtrs[-1]), LoadVehicleColours with colour '
           "lines without '#' (the EXE walks the stack), more than 6 extras. Limits are declared per function in the function-specific "
           'scope and in the bank README.',
 'error': None,
 'expect': {'CVehicleModelInfo::AddRemap': 256,
            'CVehicleModelInfo::AddWheelUpgrade': 256,
            'CVehicleModelInfo::AssignRemapTxd': 256,
            'CVehicleModelInfo::ChooseComponent': 512,
            'CVehicleModelInfo::ChooseComponent-Global': 512,
            'CVehicleModelInfo::ChooseSecondComponent': 512,
            'CVehicleModelInfo::ChooseVehicleColour': 512,
            'CVehicleModelInfo::ClearAtomicFlagCB': 128,
            'CVehicleModelInfo::CollapseFramesCB': 256,
            'CVehicleModelInfo::ConvertAnimFileIndex': 128,
            'CVehicleModelInfo::CountCompsInRule': 256,
            'CVehicleModelInfo::CreateInstance': 512,
            'CVehicleModelInfo::DeleteRwObject': 256,
            'CVehicleModelInfo::DeleteVehicleColourTextures': 64,
            'CVehicleModelInfo::DisableEnvMap': 128,
            'CVehicleModelInfo::FindTextureCB': 512,
            'CVehicleModelInfo::GetAnimFileIndex': 64,
            'CVehicleModelInfo::GetCustomCarPlateText': 128,
            'CVehicleModelInfo::GetListOfComponentsNotUsedByRules': 512,
            'CVehicleModelInfo::GetMatFXEffectMaterialCB': 128,
            'CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors': 512,
            'CVehicleModelInfo::GetModelType': 64,
            'CVehicleModelInfo::GetNumDoors': 256,
            'CVehicleModelInfo::GetNumRemaps': 128,
            'CVehicleModelInfo::GetNumWheelUpgrades': 128,
            'CVehicleModelInfo::GetOkAndDamagedAtomicCB': 128,
            'CVehicleModelInfo::GetOriginalCompPosition': 128,
            'CVehicleModelInfo::GetWheelPosn': 256,
            'CVehicleModelInfo::GetWheelUpgrade': 128,
            'CVehicleModelInfo::HasAlphaMaterialCB': 128,
            'CVehicleModelInfo::HideAllComponentsAtomicCB': 128,
            'CVehicleModelInfo::HideDamagedAtomicCB': 256,
            'CVehicleModelInfo::Init': 64,
            'CVehicleModelInfo::IsUpgradeAvailable': 256,
            'CVehicleModelInfo::IsValidCompRule': 256,
            'CVehicleModelInfo::LoadEnvironmentMaps': 128,
            'CVehicleModelInfo::LoadVehicleColours': 256,
            'CVehicleModelInfo::LoadVehicleUpgrades': 256,
            'CVehicleModelInfo::MoveObjectsCB': 64,
            'CVehicleModelInfo::PreprocessHierarchy': 512,
            'CVehicleModelInfo::ReduceMaterialsInVehicle': 256,
            'CVehicleModelInfo::RemoveWindowAlphaCB': 128,
            'CVehicleModelInfo::ResetEditableMaterials': 128,
            'CVehicleModelInfo::SetAnimFile': 256,
            'CVehicleModelInfo::SetAtomicFlagCB': 128,
            'CVehicleModelInfo::SetAtomicRenderCallbacks': 128,
            'CVehicleModelInfo::SetAtomicRendererCB': 256,
            'CVehicleModelInfo::SetAtomicRendererCB_Boat': 128,
            'CVehicleModelInfo::SetAtomicRendererCB_Heli': 128,
            'CVehicleModelInfo::SetAtomicRendererCB_Plane': 128,
            'CVehicleModelInfo::SetAtomicRendererCB_RealHeli': 128,
            'CVehicleModelInfo::SetAtomicRendererCB_Train': 128,
            'CVehicleModelInfo::SetCarCustomPlate': 128,
            'CVehicleModelInfo::SetClump': 256,
            'CVehicleModelInfo::SetCustomCarPlateText': 256,
            'CVehicleModelInfo::SetDirtTextures': 128,
            'CVehicleModelInfo::SetEditableMaterials': 256,
            'CVehicleModelInfo::SetEditableMaterialsCB-RpAtomic': 256,
            'CVehicleModelInfo::SetEditableMaterialsCB-RpMaterial': 512,
            'CVehicleModelInfo::SetEnvMapCoeff': 256,
            'CVehicleModelInfo::SetEnvMapCoeffAtomicCB': 64,
            'CVehicleModelInfo::SetEnvMapCoeffCB': 256,
            'CVehicleModelInfo::SetEnvironmentMapAtomicCB': 64,
            'CVehicleModelInfo::SetEnvironmentMapCB': 256,
            'CVehicleModelInfo::SetRenderPipelines': 128,
            'CVehicleModelInfo::SetRenderPipelinesCB': 64,
            'CVehicleModelInfo::SetVehicleColour': 128,
            'CVehicleModelInfo::SetVehicleComponentFlags': 512,
            'CVehicleModelInfo::SetupCommonData': 128,
            'CVehicleModelInfo::SetupLightFlags': 256,
            'CVehicleModelInfo::ShutdownEnvironmentMaps': 128,
            'CVehicleModelInfo::ShutdownLightTexture': 128,
            'CVehicleModelInfo::StopUsingCommonVehicleTexDicationary': 64,
            'CVehicleModelInfo::StoreAtomicUsedMaterialsCB': 256,
            'CVehicleModelInfo::UseCommonVehicleTexDicationary': 64},
 'extra_unregistered': [],
 'new': ['CVehicleModelInfo::AddRemap',
         'CVehicleModelInfo::AddWheelUpgrade',
         'CVehicleModelInfo::AssignRemapTxd',
         'CVehicleModelInfo::ChooseComponent',
         'CVehicleModelInfo::ChooseComponent-Global',
         'CVehicleModelInfo::ChooseSecondComponent',
         'CVehicleModelInfo::ChooseVehicleColour',
         'CVehicleModelInfo::ClearAtomicFlagCB',
         'CVehicleModelInfo::CollapseFramesCB',
         'CVehicleModelInfo::ConvertAnimFileIndex',
         'CVehicleModelInfo::CountCompsInRule',
         'CVehicleModelInfo::CreateInstance',
         'CVehicleModelInfo::DeleteRwObject',
         'CVehicleModelInfo::DeleteVehicleColourTextures',
         'CVehicleModelInfo::DisableEnvMap',
         'CVehicleModelInfo::FindTextureCB',
         'CVehicleModelInfo::GetAnimFileIndex',
         'CVehicleModelInfo::GetCustomCarPlateText',
         'CVehicleModelInfo::GetListOfComponentsNotUsedByRules',
         'CVehicleModelInfo::GetMatFXEffectMaterialCB',
         'CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors',
         'CVehicleModelInfo::GetModelType',
         'CVehicleModelInfo::GetNumDoors',
         'CVehicleModelInfo::GetNumRemaps',
         'CVehicleModelInfo::GetNumWheelUpgrades',
         'CVehicleModelInfo::GetOkAndDamagedAtomicCB',
         'CVehicleModelInfo::GetOriginalCompPosition',
         'CVehicleModelInfo::GetWheelPosn',
         'CVehicleModelInfo::GetWheelUpgrade',
         'CVehicleModelInfo::HasAlphaMaterialCB',
         'CVehicleModelInfo::HideAllComponentsAtomicCB',
         'CVehicleModelInfo::HideDamagedAtomicCB',
         'CVehicleModelInfo::Init',
         'CVehicleModelInfo::IsUpgradeAvailable',
         'CVehicleModelInfo::IsValidCompRule',
         'CVehicleModelInfo::LoadEnvironmentMaps',
         'CVehicleModelInfo::LoadVehicleColours',
         'CVehicleModelInfo::LoadVehicleUpgrades',
         'CVehicleModelInfo::MoveObjectsCB',
         'CVehicleModelInfo::PreprocessHierarchy',
         'CVehicleModelInfo::ReduceMaterialsInVehicle',
         'CVehicleModelInfo::RemoveWindowAlphaCB',
         'CVehicleModelInfo::ResetEditableMaterials',
         'CVehicleModelInfo::SetAnimFile',
         'CVehicleModelInfo::SetAtomicFlagCB',
         'CVehicleModelInfo::SetAtomicRenderCallbacks',
         'CVehicleModelInfo::SetAtomicRendererCB',
         'CVehicleModelInfo::SetAtomicRendererCB_Boat',
         'CVehicleModelInfo::SetAtomicRendererCB_Heli',
         'CVehicleModelInfo::SetAtomicRendererCB_Plane',
         'CVehicleModelInfo::SetAtomicRendererCB_RealHeli',
         'CVehicleModelInfo::SetAtomicRendererCB_Train',
         'CVehicleModelInfo::SetCarCustomPlate',
         'CVehicleModelInfo::SetClump',
         'CVehicleModelInfo::SetCustomCarPlateText',
         'CVehicleModelInfo::SetDirtTextures',
         'CVehicleModelInfo::SetEditableMaterials',
         'CVehicleModelInfo::SetEditableMaterialsCB-RpAtomic',
         'CVehicleModelInfo::SetEditableMaterialsCB-RpMaterial',
         'CVehicleModelInfo::SetEnvMapCoeff',
         'CVehicleModelInfo::SetEnvMapCoeffAtomicCB',
         'CVehicleModelInfo::SetEnvMapCoeffCB',
         'CVehicleModelInfo::SetEnvironmentMapAtomicCB',
         'CVehicleModelInfo::SetEnvironmentMapCB',
         'CVehicleModelInfo::SetRenderPipelines',
         'CVehicleModelInfo::SetRenderPipelinesCB',
         'CVehicleModelInfo::SetVehicleColour',
         'CVehicleModelInfo::SetVehicleComponentFlags',
         'CVehicleModelInfo::SetupCommonData',
         'CVehicleModelInfo::SetupLightFlags',
         'CVehicleModelInfo::ShutdownEnvironmentMaps',
         'CVehicleModelInfo::ShutdownLightTexture',
         'CVehicleModelInfo::StopUsingCommonVehicleTexDicationary',
         'CVehicleModelInfo::StoreAtomicUsedMaterialsCB',
         'CVehicleModelInfo::UseCommonVehicleTexDicationary'],
 'notes': 'VehModelInfoFinal4 registers the 75 functions of CVehicleModelInfo (16384 pairs, 0 differences). Limits are declared per '
          'function; see scope and the bank README. No pending functions remain in CVehicleModelInfo.',
 'passed': True,
 'pending_after': [],
 'probe_prefix': 'ecc9f388',
 'provenance': 'Exact compiled VehModelInfoFinal4 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build '
               'manifest and equal to the working tree at registration; full 16384-row log reaudited offline in streaming and preserved '
               'gzipped with the original hash.',
 'readme_section': '## Aprobación (10/10/2026)\n'
                   '\n'
                   'Corrida `20261010-005102-024078`: passed=true, 16384 pares, 75 funciones registradas con 0 fallos, evento complete '
                   '(state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_vehmodelinfofinal4.py` '
                   '(evidencia en `runs/`, `validation-summary.json`). Con este banco CVehicleModelInfo queda sin pendientes. Limites '
                   'declarados en el alcance de cada funcion y en las secciones anteriores de este README.',
 'registry_before': 7818,
 'rejected': [],
 'run_id': '20261010-005102-024078',
 'source_changes': {'source/game_sa/Models/VehicleModelInfo.cpp': 'Correcciones contra el EXE (GetNumDoors, SetAnimFile, FindTextureCB, '
                                                                  'SetAtomicRendererCB_Boat, SetVehicleComponentFlags, '
                                                                  'PreprocessHierarchy, GetWheelPosn, CountCompsInRule, ChooseComponent '
                                                                  'x3, ChooseVehicleColour, AddRemap, CreateInstance, '
                                                                  'SetCustomCarPlateText, GetMaximumNumberOfPassengersFromNumberOfDoors, '
                                                                  'GetMatFXEffectMaterialCB, SetEnvironmentMapCB, SetEnvMapCoeffCB, '
                                                                  'SetEnvMapCoeff, ReduceMaterialsInVehicle, AssignRemapTxd, '
                                                                  'LoadEnvironmentMaps, ShutdownEnvironmentMaps, LoadVehicleColours, '
                                                                  'LoadVehicleUpgrades, SetupCommonData, CVehicleStructure); ver git diff '
                                                                  'y el README del banco.',
                    'source/game_sa/Models/VehicleModelInfo.h': 'Se quita static de las 6 funciones globales para poder llamarlas desde el '
                                                                'banco.'},
 'specific': {'CVehicleModelInfo::AddRemap': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C86F4): no bounds '
                                             'check. ',
              'CVehicleModelInfo::AddWheelUpgrade': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::AssignRemapTxd': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C937B/4C93BD): '
                                                   'isdigit on the unsigned byte and strncpy. ',
              'CVehicleModelInfo::ChooseComponent': 'Corpus 512; fake world and traced stubs (no credit). source corrected '
                                                    '(4C7FC2..4C8100): random from 0x407180 = GetRandomIntInRange, signed m_nNumExtras '
                                                    '(4C80E2). ',
              'CVehicleModelInfo::ChooseComponent-Global': 'Corpus 512; fake world and traced stubs (no credit). source corrected: random '
                                                           'from 0x407180 = GetRandomIntInRange. ',
              'CVehicleModelInfo::ChooseSecondComponent': 'Corpus 512; fake world and traced stubs (no credit). source corrected (4C80B7, '
                                                          '4C81C1): random from 0x407180, signed m_nNumExtras. ',
              'CVehicleModelInfo::ChooseVehicleColour': 'Corpus 512; fake world and traced stubs (no credit). source corrected (4C8570): '
                                                        'movzx index of the byte, array unchecked. ',
              'CVehicleModelInfo::ClearAtomicFlagCB': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::CollapseFramesCB': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::ConvertAnimFileIndex': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::CountCompsInRule': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C7F9B): sar '
                                                     'ecx,4; negatives outside the fixture. ',
              'CVehicleModelInfo::CreateInstance': 'Corpus 512; fake world and traced stubs (no credit). source corrected (4C96E2/4C9750): '
                                                   'no bounds check; class subfunctions doubled with trace. ',
              'CVehicleModelInfo::DeleteRwObject': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::DeleteVehicleColourTextures': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::DisableEnvMap': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::FindTextureCB': 'Corpus 512; fake world and traced stubs (no credit). source corrected (4C7560): '
                                                  'unbounded strcpy. ',
              'CVehicleModelInfo::GetAnimFileIndex': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetCustomCarPlateText': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetListOfComponentsNotUsedByRules': 'Corpus 512; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetMatFXEffectMaterialCB': 'Corpus 128; fake world and traced stubs (no credit). source corrected '
                                                             '(4C8816/4C8823): two calls to RpMatFXMaterialGetEffects. ',
              'CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors': 'Corpus 512; fake world and traced stubs (no credit). '
                                                                                  'source corrected (4C89BF, 4C8A31): low byte of the '
                                                                                  'handling id, unsigned m_nNumDoors on that path; -1 '
                                                                                  'outside the fixture. ',
              'CVehicleModelInfo::GetModelType': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetNumDoors': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C73C0): returns '
                                                'the byte sign-extended (movsx). ',
              'CVehicleModelInfo::GetNumRemaps': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetNumWheelUpgrades': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetOkAndDamagedAtomicCB': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetOriginalCompPosition': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::GetWheelPosn': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C7D5A): rep '
                                                 'movsd of 0x40 bytes (sizeof(RwMatrix)). ',
              'CVehicleModelInfo::GetWheelUpgrade': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::HasAlphaMaterialCB': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::HideAllComponentsAtomicCB': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::HideDamagedAtomicCB': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::Init': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::IsUpgradeAvailable': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::IsValidCompRule': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::LoadEnvironmentMaps': 'Corpus 128; fake world and traced stubs (no credit). source corrected '
                                                        '(4C8781..4C8793): FindTxdSlot before PushCurrentTxd. ',
              'CVehicleModelInfo::LoadVehicleColours': 'Corpus 256; fake world and traced stubs (no credit). source corrected (5B68E0): no '
                                                       '10-character limit, unsigned compare, no FIX_BUGS for "77.93,96", red/green/blue '
                                                       'persist between lines; lines without # outside the fixture. ',
              'CVehicleModelInfo::LoadVehicleUpgrades': 'Corpus 256; fake world and traced stubs (no credit). source corrected (5B65BE): '
                                                        'opens the file before zeroing the counters, persistent iWheelSet, no VERIFY. ',
              'CVehicleModelInfo::MoveObjectsCB': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::PreprocessHierarchy': 'Corpus 512; fake world and traced stubs (no credit). source corrected (4C8E6F, '
                                                        '4C919A): low byte of the handling id, default callback 0x7491C0, m_apExtras '
                                                        'indexed with movsx. ',
              'CVehicleModelInfo::ReduceMaterialsInVehicle': 'Corpus 256; fake world and traced stubs (no credit). source corrected '
                                                             '(4C8C02/4C8C54/4C8C59): CTimer calls dropped, signed numExtras. ',
              'CVehicleModelInfo::RemoveWindowAlphaCB': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::ResetEditableMaterials': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetAnimFile': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C767E): call '
                                                '8229B6 is _stricmp ("NULL" ignored) and strcpy instead of strcpy_s (4C76C0). ',
              'CVehicleModelInfo::SetAtomicFlagCB': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetAtomicRenderCallbacks': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetAtomicRendererCB': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetAtomicRendererCB_Boat': 'Corpus 128; fake world and traced stubs (no credit). source corrected '
                                                             '(4C79B1): the material walk precedes the name comparison. ',
              'CVehicleModelInfo::SetAtomicRendererCB_Heli': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetAtomicRendererCB_Plane': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetAtomicRendererCB_RealHeli': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetAtomicRendererCB_Train': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetCarCustomPlate': 'Corpus 128; fake world and traced stubs (no credit). only the low byte of the dword '
                                                      'passed to SetupClump is traced. ',
              'CVehicleModelInfo::SetClump': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetCustomCarPlateText': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C898F): '
                                                          'strncpy(..,8) (NOTSA deviation removed). ',
              'CVehicleModelInfo::SetDirtTextures': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetEditableMaterials': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetEditableMaterialsCB-RpAtomic': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetEditableMaterialsCB-RpMaterial': 'Corpus 512; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetEnvMapCoeff': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C9819): 64-bit '
                                                   'truncation (_ftol2). ',
              'CVehicleModelInfo::SetEnvMapCoeffAtomicCB': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetEnvMapCoeffCB': 'Corpus 256; fake world and traced stubs (no credit). source corrected '
                                                     '(4C88C7..4C88DC): x87 fild, fadd 2^32 if negative, fmul 0.001f, with __asm. ',
              'CVehicleModelInfo::SetEnvironmentMapAtomicCB': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetEnvironmentMapCB': 'Corpus 256; fake world and traced stubs (no credit). source corrected (4C8840): '
                                                        'compares the full dword with 0xFFFF and calls '
                                                        'RpMatFXMaterialSetEffects(material,0) directly. ',
              'CVehicleModelInfo::SetRenderPipelines': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetRenderPipelinesCB': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetVehicleColour': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::SetVehicleComponentFlags': 'Corpus 512; fake world and traced stubs (no credit). source corrected '
                                                             '(4C7C1B): low byte of the handling id (movzx). ',
              'CVehicleModelInfo::SetupCommonData': 'Corpus 128; fake world and traced stubs (no credit). source corrected (5B8F93): no '
                                                    'assert(vehicleTxd); pool byte 0x11 and DLL 0xFD filler masked. ',
              'CVehicleModelInfo::SetupLightFlags': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::ShutdownEnvironmentMaps': 'Corpus 128; fake world and traced stubs (no credit). source corrected '
                                                            '(4C87D9): clears the global byte 0xB4E6BC. ',
              'CVehicleModelInfo::ShutdownLightTexture': 'Corpus 128; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::StopUsingCommonVehicleTexDicationary': 'Corpus 64; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::StoreAtomicUsedMaterialsCB': 'Corpus 256; fake world and traced stubs (no credit). ',
              'CVehicleModelInfo::UseCommonVehicleTexDicationary': 'Corpus 64; fake world and traced stubs (no credit). '},
 'tag': 'vehmodelinfofinal4',
 'total': 16384,
 'types': {'CVehicleModelInfo::AddRemap': 'S',
           'CVehicleModelInfo::AddWheelUpgrade': 'S',
           'CVehicleModelInfo::AssignRemapTxd': 'S',
           'CVehicleModelInfo::ChooseComponent': 'S',
           'CVehicleModelInfo::ChooseComponent-Global': 'S',
           'CVehicleModelInfo::ChooseSecondComponent': 'S',
           'CVehicleModelInfo::ChooseVehicleColour': 'S',
           'CVehicleModelInfo::ClearAtomicFlagCB': 'S',
           'CVehicleModelInfo::CollapseFramesCB': 'S',
           'CVehicleModelInfo::ConvertAnimFileIndex': 'V',
           'CVehicleModelInfo::CountCompsInRule': 'S',
           'CVehicleModelInfo::CreateInstance': 'V',
           'CVehicleModelInfo::DeleteRwObject': 'V',
           'CVehicleModelInfo::DeleteVehicleColourTextures': 'S',
           'CVehicleModelInfo::DisableEnvMap': 'S',
           'CVehicleModelInfo::FindTextureCB': 'S',
           'CVehicleModelInfo::GetAnimFileIndex': 'V',
           'CVehicleModelInfo::GetCustomCarPlateText': 'S',
           'CVehicleModelInfo::GetListOfComponentsNotUsedByRules': 'S',
           'CVehicleModelInfo::GetMatFXEffectMaterialCB': 'S',
           'CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors': 'S',
           'CVehicleModelInfo::GetModelType': 'V',
           'CVehicleModelInfo::GetNumDoors': 'S',
           'CVehicleModelInfo::GetNumRemaps': 'S',
           'CVehicleModelInfo::GetNumWheelUpgrades': 'S',
           'CVehicleModelInfo::GetOkAndDamagedAtomicCB': 'S',
           'CVehicleModelInfo::GetOriginalCompPosition': 'S',
           'CVehicleModelInfo::GetWheelPosn': 'S',
           'CVehicleModelInfo::GetWheelUpgrade': 'S',
           'CVehicleModelInfo::HasAlphaMaterialCB': 'S',
           'CVehicleModelInfo::HideAllComponentsAtomicCB': 'S',
           'CVehicleModelInfo::HideDamagedAtomicCB': 'S',
           'CVehicleModelInfo::Init': 'V',
           'CVehicleModelInfo::IsUpgradeAvailable': 'S',
           'CVehicleModelInfo::IsValidCompRule': 'S',
           'CVehicleModelInfo::LoadEnvironmentMaps': 'S',
           'CVehicleModelInfo::LoadVehicleColours': 'S',
           'CVehicleModelInfo::LoadVehicleUpgrades': 'S',
           'CVehicleModelInfo::MoveObjectsCB': 'S',
           'CVehicleModelInfo::PreprocessHierarchy': 'S',
           'CVehicleModelInfo::ReduceMaterialsInVehicle': 'S',
           'CVehicleModelInfo::RemoveWindowAlphaCB': 'S',
           'CVehicleModelInfo::ResetEditableMaterials': 'S',
           'CVehicleModelInfo::SetAnimFile': 'V',
           'CVehicleModelInfo::SetAtomicFlagCB': 'S',
           'CVehicleModelInfo::SetAtomicRenderCallbacks': 'S',
           'CVehicleModelInfo::SetAtomicRendererCB': 'S',
           'CVehicleModelInfo::SetAtomicRendererCB_Boat': 'S',
           'CVehicleModelInfo::SetAtomicRendererCB_Heli': 'S',
           'CVehicleModelInfo::SetAtomicRendererCB_Plane': 'S',
           'CVehicleModelInfo::SetAtomicRendererCB_RealHeli': 'S',
           'CVehicleModelInfo::SetAtomicRendererCB_Train': 'S',
           'CVehicleModelInfo::SetCarCustomPlate': 'S',
           'CVehicleModelInfo::SetClump': 'V',
           'CVehicleModelInfo::SetCustomCarPlateText': 'S',
           'CVehicleModelInfo::SetDirtTextures': 'S',
           'CVehicleModelInfo::SetEditableMaterials': 'S',
           'CVehicleModelInfo::SetEditableMaterialsCB-RpAtomic': 'S',
           'CVehicleModelInfo::SetEditableMaterialsCB-RpMaterial': 'S',
           'CVehicleModelInfo::SetEnvMapCoeff': 'S',
           'CVehicleModelInfo::SetEnvMapCoeffAtomicCB': 'S',
           'CVehicleModelInfo::SetEnvMapCoeffCB': 'S',
           'CVehicleModelInfo::SetEnvironmentMapAtomicCB': 'S',
           'CVehicleModelInfo::SetEnvironmentMapCB': 'S',
           'CVehicleModelInfo::SetRenderPipelines': 'S',
           'CVehicleModelInfo::SetRenderPipelinesCB': 'S',
           'CVehicleModelInfo::SetVehicleColour': 'S',
           'CVehicleModelInfo::SetVehicleComponentFlags': 'S',
           'CVehicleModelInfo::SetupCommonData': 'S',
           'CVehicleModelInfo::SetupLightFlags': 'S',
           'CVehicleModelInfo::ShutdownEnvironmentMaps': 'S',
           'CVehicleModelInfo::ShutdownLightTexture': 'S',
           'CVehicleModelInfo::StopUsingCommonVehicleTexDicationary': 'S',
           'CVehicleModelInfo::StoreAtomicUsedMaterialsCB': 'S',
           'CVehicleModelInfo::UseCommonVehicleTexDicationary': 'S'}}
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
