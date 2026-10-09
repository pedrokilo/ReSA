"""Registro historico de CPostEffects y FxSystem_c (66 funciones) del banco simple FxPost1 (corrida 20261009-161301-487563). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'FxPost1',
 'run_id': '20261009-161301-487563',
 'probe_prefix': 'a8908ec2',
 'tag': 'fxpost1',
 'registry_before': 7229,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'FxSystem_c::AddOffsetPos': 256,
            'FxSystem_c::AttachToBone': 256,
            'FxSystem_c::Constructor': 128,
            'FxSystem_c::CopyParentMatrix': 128,
            'FxSystem_c::Destructor': 256,
            'FxSystem_c::DoFxAudio': 512,
            'FxSystem_c::EnablePrim': 128,
            'FxSystem_c::Exit': 256,
            'FxSystem_c::ForAllParticles': 512,
            'FxSystem_c::GetBoundingSphereLcl': 128,
            'FxSystem_c::GetBoundingSphereWld': 512,
            'FxSystem_c::GetCompositeMatrix': 256,
            'FxSystem_c::GetPlayStatus': 64,
            'FxSystem_c::Init': 512,
            'FxSystem_c::IsVisible': 512,
            'FxSystem_c::Kill': 256,
            'FxSystem_c::Pause': 128,
            'FxSystem_c::Play': 512,
            'FxSystem_c::PlayAndKill': 256,
            'FxSystem_c::ResetBoundingSphere': 256,
            'FxSystem_c::SetBoundingSphere': 256,
            'FxSystem_c::SetConstTime': 256,
            'FxSystem_c::SetLocalParticles': 128,
            'FxSystem_c::SetMatrix': 128,
            'FxSystem_c::SetMustCreatePrts': 128,
            'FxSystem_c::SetOffsetPos': 256,
            'FxSystem_c::SetRateMult': 256,
            'FxSystem_c::SetTimeMult': 256,
            'FxSystem_c::SetVelAdd': 128,
            'FxSystem_c::SetZTestEnable': 128,
            'FxSystem_c::Stop': 256,
            'CPostEffects::CCTV': 256,
            'CPostEffects::Close': 64,
            'CPostEffects::ColourFilter': 256,
            'CPostEffects::DarknessFilter': 256,
            'CPostEffects::DoScreenModeDependentInitializations': 128,
            'CPostEffects::DrawQuad': 512,
            'CPostEffects::FilterFX_RestoreDayNightBalance': 128,
            'CPostEffects::FilterFX_StoreAndSetDayNightBalance': 128,
            'CPostEffects::Fog': 512,
            'CPostEffects::Grain': 192,
            'CPostEffects::HeatHazeFXInit': 256,
            'CPostEffects::ImmediateModeFilterStuffInitialize': 256,
            'CPostEffects::ImmediateModeRenderStatesReStore': 128,
            'CPostEffects::ImmediateModeRenderStatesSet': 64,
            'CPostEffects::ImmediateModeRenderStatesStore': 128,
            'CPostEffects::InfraredVision': 256,
            'CPostEffects::InfraredVisionRestoreLightsForHeatObjects': 64,
            'CPostEffects::InfraredVisionSetLightsForDefaultObjects': 64,
            'CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects': 512,
            'CPostEffects::Initialise': 64,
            'CPostEffects::IsVisionFXActive': 64,
            'CPostEffects::NightVision': 256,
            'CPostEffects::NightVisionSetLights': 64,
            'CPostEffects::Render': 1024,
            'CPostEffects::ScriptCCTVSwitch': 64,
            'CPostEffects::ScriptDarknessFilterSwitch': 128,
            'CPostEffects::ScriptHeatHazeFXSwitch': 64,
            'CPostEffects::ScriptInfraredVisionSwitch': 64,
            'CPostEffects::ScriptNightVisionSwitch': 64,
            'CPostEffects::ScriptResetForEffects': 64,
            'CPostEffects::SetFilterMainColour': 256,
            'CPostEffects::SetSpeedFXManualSpeedCurrentFrame': 256,
            'CPostEffects::SetupBackBufferVertex': 256,
            'CPostEffects::UnderWaterRipple': 512,
            'CPostEffects::Update': 128},
 'total': 15488,
 'new': ['CPostEffects::CCTV',
         'CPostEffects::Close',
         'CPostEffects::ColourFilter',
         'CPostEffects::DarknessFilter',
         'CPostEffects::DoScreenModeDependentInitializations',
         'CPostEffects::DrawQuad',
         'CPostEffects::FilterFX_RestoreDayNightBalance',
         'CPostEffects::FilterFX_StoreAndSetDayNightBalance',
         'CPostEffects::Fog',
         'CPostEffects::Grain',
         'CPostEffects::HeatHazeFXInit',
         'CPostEffects::ImmediateModeFilterStuffInitialize',
         'CPostEffects::ImmediateModeRenderStatesReStore',
         'CPostEffects::ImmediateModeRenderStatesSet',
         'CPostEffects::ImmediateModeRenderStatesStore',
         'CPostEffects::InfraredVision',
         'CPostEffects::InfraredVisionRestoreLightsForHeatObjects',
         'CPostEffects::InfraredVisionSetLightsForDefaultObjects',
         'CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects',
         'CPostEffects::Initialise',
         'CPostEffects::IsVisionFXActive',
         'CPostEffects::NightVision',
         'CPostEffects::NightVisionSetLights',
         'CPostEffects::Render',
         'CPostEffects::ScriptCCTVSwitch',
         'CPostEffects::ScriptDarknessFilterSwitch',
         'CPostEffects::ScriptHeatHazeFXSwitch',
         'CPostEffects::ScriptInfraredVisionSwitch',
         'CPostEffects::ScriptNightVisionSwitch',
         'CPostEffects::ScriptResetForEffects',
         'CPostEffects::SetFilterMainColour',
         'CPostEffects::SetSpeedFXManualSpeedCurrentFrame',
         'CPostEffects::SetupBackBufferVertex',
         'CPostEffects::UnderWaterRipple',
         'CPostEffects::Update',
         'FxSystem_c::AddOffsetPos',
         'FxSystem_c::AttachToBone',
         'FxSystem_c::Constructor',
         'FxSystem_c::CopyParentMatrix',
         'FxSystem_c::Destructor',
         'FxSystem_c::DoFxAudio',
         'FxSystem_c::EnablePrim',
         'FxSystem_c::Exit',
         'FxSystem_c::ForAllParticles',
         'FxSystem_c::GetBoundingSphereLcl',
         'FxSystem_c::GetBoundingSphereWld',
         'FxSystem_c::GetCompositeMatrix',
         'FxSystem_c::GetPlayStatus',
         'FxSystem_c::Init',
         'FxSystem_c::IsVisible',
         'FxSystem_c::Kill',
         'FxSystem_c::Pause',
         'FxSystem_c::Play',
         'FxSystem_c::PlayAndKill',
         'FxSystem_c::ResetBoundingSphere',
         'FxSystem_c::SetBoundingSphere',
         'FxSystem_c::SetConstTime',
         'FxSystem_c::SetLocalParticles',
         'FxSystem_c::SetMatrix',
         'FxSystem_c::SetMustCreatePrts',
         'FxSystem_c::SetOffsetPos',
         'FxSystem_c::SetRateMult',
         'FxSystem_c::SetTimeMult',
         'FxSystem_c::SetVelAdd',
         'FxSystem_c::SetZTestEnable',
         'FxSystem_c::Stop'],
 'rejected': [],
 'addr': {'CPostEffects::CCTV': '0x00702F40',
          'CPostEffects::Close': '0x007010C0',
          'CPostEffects::ColourFilter': '0x00703650',
          'CPostEffects::DarknessFilter': '0x00702F00',
          'CPostEffects::DoScreenModeDependentInitializations': '0x007046D0',
          'CPostEffects::DrawQuad': '0x00700EC0',
          'CPostEffects::FilterFX_RestoreDayNightBalance': '0x007034D0',
          'CPostEffects::FilterFX_StoreAndSetDayNightBalance': '0x007034B0',
          'CPostEffects::Fog': '0x00704150',
          'CPostEffects::Grain': '0x007037C0',
          'CPostEffects::HeatHazeFXInit': '0x00701450',
          'CPostEffects::ImmediateModeFilterStuffInitialize': '0x00703CC0',
          'CPostEffects::ImmediateModeRenderStatesReStore': '0x00700E00',
          'CPostEffects::ImmediateModeRenderStatesSet': '0x00700D70',
          'CPostEffects::ImmediateModeRenderStatesStore': '0x00700CC0',
          'CPostEffects::InfraredVision': '0x00703F80',
          'CPostEffects::InfraredVisionRestoreLightsForHeatObjects': '0x00701410',
          'CPostEffects::InfraredVisionSetLightsForDefaultObjects': '0x00701430',
          'CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects': '0x00701320',
          'CPostEffects::Initialise': '0x00704630',
          'CPostEffects::IsVisionFXActive': '0x007034F0',
          'CPostEffects::NightVision': '0x007011C0',
          'CPostEffects::NightVisionSetLights': '0x007012E0',
          'CPostEffects::Render': '0x007046E0',
          'CPostEffects::ScriptCCTVSwitch': '0x007011B0',
          'CPostEffects::ScriptDarknessFilterSwitch': '0x00701170',
          'CPostEffects::ScriptHeatHazeFXSwitch': '0x00701160',
          'CPostEffects::ScriptInfraredVisionSwitch': '0x00701140',
          'CPostEffects::ScriptNightVisionSwitch': '0x00701120',
          'CPostEffects::ScriptResetForEffects': '0x007010F0',
          'CPostEffects::SetFilterMainColour': '0x00703520',
          'CPostEffects::SetSpeedFXManualSpeedCurrentFrame': '0x00700BE0',
          'CPostEffects::SetupBackBufferVertex': '0x007043D0',
          'CPostEffects::UnderWaterRipple': '0x007039C0',
          'CPostEffects::Update': '0x007046A0',
          'FxSystem_c::AddOffsetPos': '0x004AA690',
          'FxSystem_c::AttachToBone': '0x004AA400',
          'FxSystem_c::Constructor': '0x004AAF00',
          'FxSystem_c::CopyParentMatrix': '0x004AA890',
          'FxSystem_c::Destructor': '0x004AA260',
          'FxSystem_c::DoFxAudio': '0x004AAC90',
          'FxSystem_c::EnablePrim': '0x004AA610',
          'FxSystem_c::Exit': '0x004AA840',
          'FxSystem_c::ForAllParticles': '0x004AA930',
          'FxSystem_c::GetBoundingSphereLcl': '0x004AAB50',
          'FxSystem_c::GetBoundingSphereWld': '0x004AAAD0',
          'FxSystem_c::GetCompositeMatrix': '0x004AA8C0',
          'FxSystem_c::GetPlayStatus': '0x004AA900',
          'FxSystem_c::Init': '0x004AA750',
          'FxSystem_c::IsVisible': '0x004AAF30',
          'FxSystem_c::Kill': '0x004AA3F0',
          'FxSystem_c::Pause': '0x004AA370',
          'FxSystem_c::Play': '0x004AA2F0',
          'FxSystem_c::PlayAndKill': '0x004AA3D0',
          'FxSystem_c::ResetBoundingSphere': '0x004AABF0',
          'FxSystem_c::SetBoundingSphere': '0x004AAB80',
          'FxSystem_c::SetConstTime': '0x004AA6C0',
          'FxSystem_c::SetLocalParticles': '0x004AA910',
          'FxSystem_c::SetMatrix': '0x004AA630',
          'FxSystem_c::SetMustCreatePrts': '0x004AAC70',
          'FxSystem_c::SetOffsetPos': '0x004AA660',
          'FxSystem_c::SetRateMult': '0x004AA6F0',
          'FxSystem_c::SetTimeMult': '0x004AA710',
          'FxSystem_c::SetVelAdd': '0x004AA730',
          'FxSystem_c::SetZTestEnable': '0x004AAC50',
          'FxSystem_c::Stop': '0x004AA390'},
 'types': {'CPostEffects::CCTV': 'S',
           'CPostEffects::Close': 'S',
           'CPostEffects::ColourFilter': 'S',
           'CPostEffects::DarknessFilter': 'S',
           'CPostEffects::DoScreenModeDependentInitializations': 'S',
           'CPostEffects::DrawQuad': 'S',
           'CPostEffects::FilterFX_RestoreDayNightBalance': 'S',
           'CPostEffects::FilterFX_StoreAndSetDayNightBalance': 'S',
           'CPostEffects::Fog': 'S',
           'CPostEffects::Grain': 'S',
           'CPostEffects::HeatHazeFXInit': 'S',
           'CPostEffects::ImmediateModeFilterStuffInitialize': 'S',
           'CPostEffects::ImmediateModeRenderStatesReStore': 'S',
           'CPostEffects::ImmediateModeRenderStatesSet': 'S',
           'CPostEffects::ImmediateModeRenderStatesStore': 'S',
           'CPostEffects::InfraredVision': 'S',
           'CPostEffects::InfraredVisionRestoreLightsForHeatObjects': 'S',
           'CPostEffects::InfraredVisionSetLightsForDefaultObjects': 'S',
           'CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects': 'S',
           'CPostEffects::Initialise': 'S',
           'CPostEffects::IsVisionFXActive': 'S',
           'CPostEffects::NightVision': 'S',
           'CPostEffects::NightVisionSetLights': 'S',
           'CPostEffects::Render': 'S',
           'CPostEffects::ScriptCCTVSwitch': 'S',
           'CPostEffects::ScriptDarknessFilterSwitch': 'S',
           'CPostEffects::ScriptHeatHazeFXSwitch': 'S',
           'CPostEffects::ScriptInfraredVisionSwitch': 'S',
           'CPostEffects::ScriptNightVisionSwitch': 'S',
           'CPostEffects::ScriptResetForEffects': 'S',
           'CPostEffects::SetFilterMainColour': 'S',
           'CPostEffects::SetSpeedFXManualSpeedCurrentFrame': 'S',
           'CPostEffects::SetupBackBufferVertex': 'S',
           'CPostEffects::UnderWaterRipple': 'S',
           'CPostEffects::Update': 'S',
           'FxSystem_c::AddOffsetPos': 'S',
           'FxSystem_c::AttachToBone': 'S',
           'FxSystem_c::Constructor': 'S',
           'FxSystem_c::CopyParentMatrix': 'S',
           'FxSystem_c::Destructor': 'S',
           'FxSystem_c::DoFxAudio': 'S',
           'FxSystem_c::EnablePrim': 'S',
           'FxSystem_c::Exit': 'S',
           'FxSystem_c::ForAllParticles': 'S',
           'FxSystem_c::GetBoundingSphereLcl': 'S',
           'FxSystem_c::GetBoundingSphereWld': 'S',
           'FxSystem_c::GetCompositeMatrix': 'S',
           'FxSystem_c::GetPlayStatus': 'S',
           'FxSystem_c::Init': 'S',
           'FxSystem_c::IsVisible': 'S',
           'FxSystem_c::Kill': 'S',
           'FxSystem_c::Pause': 'S',
           'FxSystem_c::Play': 'S',
           'FxSystem_c::PlayAndKill': 'S',
           'FxSystem_c::ResetBoundingSphere': 'S',
           'FxSystem_c::SetBoundingSphere': 'S',
           'FxSystem_c::SetConstTime': 'S',
           'FxSystem_c::SetLocalParticles': 'S',
           'FxSystem_c::SetMatrix': 'S',
           'FxSystem_c::SetMustCreatePrts': 'S',
           'FxSystem_c::SetOffsetPos': 'S',
           'FxSystem_c::SetRateMult': 'S',
           'FxSystem_c::SetTimeMult': 'S',
           'FxSystem_c::SetVelAdd': 'S',
           'FxSystem_c::SetZTestEnable': 'S',
           'FxSystem_c::Stop': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled FxPost1 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree '
               'at registration; full 15488-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 15488 isolated CPU pairs for '
           '66 pending functions of CPostEffects, FxSystem_c, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural "complete" event with '
           'state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case index) in both legs and compares the '
           'return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 bits) and the whole trace of doubled collaborators with raw '
           'arguments; foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == current working tree at '
           'registration (class sources match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Limits: '
           'fake RwEngineInstance device (nothing draws), fake FxSystem_c/primitives/particles with a fixed arena for CMemoryMgr::Malloc, RW raster/camera/lights/RNG '
           'and audio doubled with trace; fixtures without signalling NaN, with at most one NaN operand in two-operand operations, and without inputs that make the EXE '
           'read uninitialised locals or loop forever. CPostEffects::Render has LIMITED SCOPE: the photo-saving branch (0x705301..0x7053E3, real file I/O) is outside '
           'the bank and avoided by the fixtures. No pending function of the two classes was left out of the bank (FxSystem_c::Update, AddParticle, GetBoundingBox, '
           'UpdateBoundingBoxCB, HeatHazeFX, Radiosity, SpeedFX, DrawQuadSet* and RasterCreatePostEffects are not in the pending list).',
 'specific': {'CPostEffects::CCTV': 'Corpus 256; domain excludes height >= 448 (the EXE would loop without end).',
              'CPostEffects::Close': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                     'corrections); fake objects with 16-byte guards.',
              'CPostEffects::ColourFilter': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                            'corrections); fake objects with 16-byte guards.',
              'CPostEffects::DarknessFilter': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                              'of corrections); fake objects with 16-byte guards.',
              'CPostEffects::DoScreenModeDependentInitializations': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                    'README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::DrawQuad': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                        'corrections); fake objects with 16-byte guards.',
              'CPostEffects::FilterFX_RestoreDayNightBalance': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                               'README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::FilterFX_StoreAndSetDayNightBalance': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                   'README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::Fog': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                   'corrections); fake objects with 16-byte guards.',
              'CPostEffects::Grain': 'Corpus 192; RNG (65536 calls) summarised in a hash; only the low byte of the strength argument compared (the only one reaching the '
                                     'alpha channel of DrawQuad).',
              'CPostEffects::HeatHazeFXInit': 'Corpus 256; RNG summarised in a hash; srand is a no-op without trace.',
              'CPostEffects::ImmediateModeFilterStuffInitialize': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                  'README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ImmediateModeRenderStatesReStore': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                'README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ImmediateModeRenderStatesSet': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README '
                                                            'and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ImmediateModeRenderStatesStore': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                              'README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::InfraredVision': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                              'of corrections); fake objects with 16-byte guards.',
              'CPostEffects::InfraredVisionRestoreLightsForHeatObjects': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the '
                                                                         'bank README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::InfraredVisionSetLightsForDefaultObjects': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the '
                                                                        'bank README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see '
                                                                             'the bank README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::Initialise': 'Corpus 64; RNG summarised in a hash; srand is a no-op without trace (seeds depend on the time).',
              'CPostEffects::IsVisionFXActive': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                                'of corrections); fake objects with 16-byte guards.',
              'CPostEffects::NightVision': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                           'corrections); fake objects with 16-byte guards.',
              'CPostEffects::NightVisionSetLights': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and the '
                                                    'list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::Render': 'Corpus 1024; LIMITED SCOPE: the photo-saving branch (0x705301..0x7053E3, real file I/O: fopen of Gallery, '
                                      'JPegCompressScreenToFile, CFileMgr::SetDirMyDocuments) is NOT covered; the fixtures avoid it (s_WaitForOneFrame set to false when '
                                      'it would enter); the branches without I/O (clearing by m_bIsSaveDone, arming s_WaitForOneFrame and s_SavePhotoToGallery) ARE '
                                      'compared; UnderWaterRipple colour compared without its high byte; the sub-effects are doubled.',
              'CPostEffects::ScriptCCTVSwitch': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                                'of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ScriptDarknessFilterSwitch': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README '
                                                          'and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ScriptHeatHazeFXSwitch': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and the '
                                                      'list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ScriptInfraredVisionSwitch': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and '
                                                          'the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ScriptNightVisionSwitch': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and '
                                                       'the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::ScriptResetForEffects': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and the '
                                                     'list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::SetFilterMainColour': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the '
                                                   'list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::SetSpeedFXManualSpeedCurrentFrame': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank '
                                                                 'README and the list of corrections); fake objects with 16-byte guards.',
              'CPostEffects::SetupBackBufferVertex': 'Corpus 256; RwRasterCreate failure branch (0x7044B5..0x7044CF: EXE printf vs DLL spdlog) not generated.',
              'CPostEffects::UnderWaterRipple': 'Corpus 512; domain excludes yoffset > 0 (the EXE reads uninitialised locals).',
              'CPostEffects::Update': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                      'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::AddOffsetPos': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                          'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::AttachToBone': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                          'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Constructor': 'Corpus 128; only bytes [0,0x7C) and the two FireAE dwords the EXE writes (sys+0x8C, sys+0x100) are compared; the rest of '
                                         'm_FireAE (default CAESound in the DLL) and the vptr are masked.',
              'FxSystem_c::CopyParentMatrix': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                              'of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Destructor': 'Corpus 256; only bytes [0,0x7C) are compared; the vptr (DLL vs EXE) is masked; heap pointers of m_Prims masked (content '
                                        'compared).',
              'FxSystem_c::DoFxAudio': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                       'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::EnablePrim': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                        'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Exit': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                  'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::ForAllParticles': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                             'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::GetBoundingSphereLcl': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the '
                                                  'list of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::GetBoundingSphereWld': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the '
                                                  'list of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::GetCompositeMatrix': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                                'of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::GetPlayStatus': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                           'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Init': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                  'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::IsVisible': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                       'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Kill': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                  'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Pause': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                   'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Play': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                  'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::PlayAndKill': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                         'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::ResetBoundingSphere': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the '
                                                 'list of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetBoundingSphere': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                               'of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetConstTime': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                          'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetLocalParticles': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                               'of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetMatrix': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                       'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetMustCreatePrts': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list '
                                               'of corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetOffsetPos': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                          'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetRateMult': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                         'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetTimeMult': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                         'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetVelAdd': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                       'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::SetZTestEnable': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                            'corrections); fake objects with 16-byte guards.',
              'FxSystem_c::Stop': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see the bank README and the list of '
                                  'corrections); fake objects with 16-byte guards.'},
 'source_changes': {'source/game_sa/Fx/FxSystem.cpp': 'FxSystem_c::Init, Play, ForAllParticles, SetBoundingSphere, ResetBoundingSphere, AttachToBone corrected against '
                                                      'the EXE and the signed-byte counter loops (Play, Stop, Init, Exit, ForAllParticles); see git diff',
                    'source/game_sa/PostEffects.cpp': 'CPostEffects::Initialise, SetupBackBufferVertex, ImmediateModeFilterStuffInitialize, HeatHazeFXInit, NightVision, '
                                                      'InfraredVision, InfraredVisionStoreAndSetLightsForHeatObjects, Fog, CCTV, Grain, UnderWaterRipple and Render '
                                                      '(several points) corrected against the EXE; see git diff',
                    'source/game_sa/PostEffects.h': 'declaration adjustment (see git diff)'},
 'notes': 'FxPost1 registers the 66 functions with cases (31 of FxSystem_c and 35 of CPostEffects); both classes are left without pending entries. CPostEffects::Render '
          'is registered with LIMITED SCOPE: the photo-saving branch (0x705301..0x7053E3, real file I/O) is not covered.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-161301-487563`: passed=true, 15488 pares, 66 funciones con 0 fallos (31 de FxSystem_c y 35 de CPostEffects), evento complete '
                   '(state_restored), begin con known=true, layout y 44 sitios correctos, saves y configuraciones intactos. Registrada con `register_fxpost1.py` '
                   '(evidencia en `runs/`, `validation-summary.json`). Con esto FxSystem_c y CPostEffects quedan COMPLETAS. **Alcance limitado de '
                   '`CPostEffects::Render`**: la rama de guardado de foto (0x705301..0x7053E3, E/S real de fichero) no está cubierta y los fixtures la evitan; las ramas '
                   'sin E/S sí se comparan. Límites: dispositivo RW falso, colaboradores doblados (sin crédito), RNG resumido en hash en '
                   'Grain/Initialise/HeatHazeFXInit, Constructor/Destructor de FxSystem_c con máscaras.'}
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
assert begin is not None and events.get('begin')==1 and begin.get('known') is True and isinstance(begin.get('layout'),dict) and isinstance(begin.get('sites'),list) and len(begin['sites'])>0 and layout is None
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
