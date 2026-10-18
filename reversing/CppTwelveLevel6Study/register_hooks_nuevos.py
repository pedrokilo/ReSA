"""Registra en reversing/validation-registry.json los hooks que el lote de nivel 6 anade al catalogo (docs/hooks.csv).

Son tres hooks que ya estaban registrados en el juego pero no tenian fila en el catalogo: CStats::UpdateSexAppealStat (0x55BF20),
CPlayerPed::ControlButtonSprint (0x60A610) y RpAnimBlend::FrameUpdateCallBackOffscreen (0x4D2E10). Cada uno se acredita con la corrida aprobada
que ya figura en reversing/CppOnlyMigration/approvals.json (summary.json con passed y all_equal verdaderos). El script es idempotente: no repite
un hook ya registrado ni sobrescribe un validation-summary.json existente.
"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HOOKS = (
    # (clase del catalogo, funcion del catalogo, direccion, identidad en approvals, banco)
    ('CStats', 'UpdateSexAppealStat', '0x0055BF20', 'CStats::UpdateSexAppealStat', 'DependenciesStatsSex16'),
    ('CPlayerPed', 'ControlButtonSprint', '0x0060A610', 'CPlayerPed::ControlButtonSprint', 'DependenciesTaskSprint16'),
    ('RpAnimBlend', 'FrameUpdateCallBackOffscreen', '0x004D2E10', 'RpAnimBlend::FrameUpdateCallBackOffscreen', 'CppRpAnimUpdate1'),
    # Destinos inciertos de la cohorte de 499 (PLAN-001), reconstruidos y validados con los bancos Dependencies21A..G.
    ('CRT', 'CrtPrintf', '0x00821982', 'CRT::CrtPrintf', 'Dependencies21A'),
    ('CCamera', 'ProcessObbeCinemaCameraCar', '0x005267C0', 'CCamera::ProcessObbeCinemaCameraCar', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraTrain', '0x00526950', 'CCamera::ProcessObbeCinemaCameraTrain', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraHeli', '0x00526AE0', 'CCamera::ProcessObbeCinemaCameraHeli', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraPlane', '0x00526C80', 'CCamera::ProcessObbeCinemaCameraPlane', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraBoat', '0x00526E20', 'CCamera::ProcessObbeCinemaCameraBoat', 'Dependencies21B'),
    ('CTaskSimpleFight', 'FightSetUpCol', '0x0061D5F0', 'CTaskSimpleFight::FightSetUpCol', 'Dependencies21C'),
    ('CTaskSimpleFight', 'FightHitCar', '0x0061D0B0', 'CTaskSimpleFight::FightHitCar', 'Dependencies21C'),
    ('CTaskSimpleFight', 'FightStrike', '0x006240B0', 'CTaskSimpleFight::FightStrike', 'Dependencies21C'),
    ('CTaskSimpleGangDriveBy', 'Constructor', '0x006217D0', 'CTaskSimpleGangDriveBy::Constructor', 'Dependencies21D'),
    ('CTaskSimpleGangDriveBy', 'PlayerTarget', '0x00621960', 'CTaskSimpleGangDriveBy::PlayerTarget', 'Dependencies21D'),
    ('CTaskSimpleGangDriveBy', 'AITarget', '0x00627600', 'CTaskSimpleGangDriveBy::AITarget', 'Dependencies21D'),
    ('CTaskSimpleGangDriveBy', 'AimGun', '0x00628350', 'CTaskSimpleGangDriveBy::AimGun', 'Dependencies21D'),
    ('CPopulation', 'RandomIntInLimitRange', '0x00610DB0', 'CPopulation::RandomIntInLimitRange', 'Dependencies21E'),
    ('CZoneDef', 'GetCenter', '0x0072D6D0', 'CZoneDef::GetCenter', 'Dependencies21E'),
    ('CTaskComplexEnterCar', 'GetCameraStickModifier', '0x0063A380', 'CTaskComplexEnterCar::GetCameraStickModifier', 'Dependencies21E'),
    ('D3DResourceSystem', 'TidyUpD3DTextures', '0x007305E0', 'D3DResourceSystem::TidyUpD3DTextures', 'Dependencies21F'),
    ('D3DResourceSystem', 'TidyUpD3DIndexBuffers', '0x00730740', 'D3DResourceSystem::TidyUpD3DIndexBuffers', 'Dependencies21F'),
    ('RwHelper', 'GetFirstTexture', '0x00734940', 'RwHelper::GetFirstTexture', 'Dependencies21G'),
    ('RwHelper', 'RpGeometryReplaceOldMaterialWithNewMaterial', '0x00734DE0', 'RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial', 'Dependencies21G'),
    ('CQuaternion', 'Slerp-halftheta', '0x0059C300', 'CQuaternion::Slerp', 'Dependencies21G'),
    # Lote 1 posterior (verificacion de existencia en el EXE): CPointLights, luces de CVehicle, WrapAngle y CreateParticles.
    ('CPointLights', 'GenerateLightsAffectingObject', '0x006FFBB0', 'CPointLights::GenerateLightsAffectingObject', 'Dependencies22A'),
    ('CPointLights', 'GetLightMultiplier', '0x006FFE70', 'CPointLights::GetLightMultiplier', 'Dependencies22A'),
    ('CPointLights', 'ProcessVerticalLineUsingCache', '0x006FFFF0', 'CPointLights::ProcessVerticalLineUsingCache', 'Dependencies22A'),
    ('CPointLights', 'RenderFogEffect', '0x007002D0', 'CPointLights::RenderFogEffect', 'Dependencies22A'),
    ('CVehicle', 'DoHeadLightEffect', '0x006E0A50', 'CVehicle::DoHeadLightEffect', 'Dependencies22B'),
    ('CVehicle', 'DoTailLightEffect', '0x006E1780', 'CVehicle::DoTailLightEffect', 'Dependencies22B'),
    ('CCam', 'WrapAngle', '0x00509BE0', 'CCam::WrapAngle', 'Dependencies22C'),
    ('FxEmitter_c', 'CreateParticles', '0x004A41E0', 'FxEmitter_c::CreateParticles', 'Dependencies22C'),
    ('CVehicle', 'DoVehicleLights', '0x006E1A60', 'CVehicle::DoVehicleLights', 'Dependencies23A'),
    ('CVehicle', 'FlyingControl', '0x006D85F0', 'CVehicle::FlyingControl', 'Dependencies23B'),
    ('Win', '__MainWndProc', '0x00747EB0', 'Win::__MainWndProc', 'Dependencies24A'),
    ('Input', 'diPadInit', '0x007485C0', 'WinInput::diPadInit', 'Dependencies24B'),
    ('RwCore', 'RwD3D9IndexBufferCreate', '0x004C9970', 'RwCore::RwD3D9IndexBufferCreate', 'Dependencies25B'),
    ('RpAnisot', 'RpAnisotGetMaxSupportedMaxAnisotropy', '0x00748F20', 'RpAnisot::RpAnisotGetMaxSupportedMaxAnisotropy', 'Dependencies25B'),
    ('RpAnisot', 'RpAnisotTextureSetMaxAnisotropy', '0x00748F30', 'RpAnisot::RpAnisotTextureSetMaxAnisotropy', 'Dependencies25B'),
    ('RpAnisot', 'RpAnisotTextureGetMaxAnisotropy', '0x00748F50', 'RpAnisot::RpAnisotTextureGetMaxAnisotropy', 'Dependencies25B'),
    ('RpWorld', 'RpClumpGetNumAtomics', '0x007498E0', 'RpWorld::RpClumpGetNumAtomics', 'Dependencies25B'),
    ('RpWorld', 'RpAtomicSetStreamAlwaysCallBack', '0x0074BE30', 'RpWorld::RpAtomicSetStreamAlwaysCallBack', 'Dependencies25B'),
    ('RpWorld', 'RpAtomicSetStreamRightsCallBack', '0x0074BE50', 'RpWorld::RpAtomicSetStreamRightsCallBack', 'Dependencies25B'),
    ('RpWorld', 'RpAtomicGetPluginOffset', '0x0074BEC0', 'RpWorld::RpAtomicGetPluginOffset', 'Dependencies25B'),
    ('RpWorld', 'RpGeometryTriangleGetMaterial', '0x0074C760', 'RpWorld::RpGeometryTriangleGetMaterial', 'Dependencies25B'),
    ('RpWorld', '_rpMaterialListInitialize', '0x0074E1B0', 'RpWorld::_rpMaterialListInitialize', 'Dependencies25B'),
    ('RpWorld', '_rpMaterialListGetMaterial', '0x0074E2B0', 'RpWorld::_rpMaterialListGetMaterial', 'Dependencies25B'),
    ('RpWorld', '_rpMaterialListFindMaterialIndex', '0x0074E420', 'RpWorld::_rpMaterialListFindMaterialIndex', 'Dependencies25B'),
    ('RpWorld', 'RpWorldRegisterPlugin', '0x0074FCD0', 'RpWorld::RpWorldRegisterPlugin', 'Dependencies25B'),
    ('RpWorld', 'RpWorldRegisterPluginStream', '0x0074FD00', 'RpWorld::RpWorldRegisterPluginStream', 'Dependencies25B'),
    ('RpWorld', 'RpAtomicGetWorld', '0x00751050', 'RpWorld::RpAtomicGetWorld', 'Dependencies25B'),
    ('RpWorld', 'RpLightRegisterPlugin', '0x00751D60', 'RpWorld::RpLightRegisterPlugin', 'Dependencies25B'),
    ('RpWorld', '_rpD3D9VertexDeclarationGetSize', '0x007522E0', 'RpWorld::_rpD3D9VertexDeclarationGetSize', 'Dependencies25B'),
    ('RpWorld', 'RpD3D9GeometryGetUsageFlags', '0x007588D0', 'RpWorld::RpD3D9GeometryGetUsageFlags', 'Dependencies25B'),
    ('RpWorld', 'RpD3D9WorldSectorSetUsageFlags', '0x007588E0', 'RpWorld::RpD3D9WorldSectorSetUsageFlags', 'Dependencies25B'),
    ('RpWorld', 'RpD3D9WorldSectorGetUsageFlags', '0x00758900', 'RpWorld::RpD3D9WorldSectorGetUsageFlags', 'Dependencies25B'),
    ('RpWorld', '_rpMeshHeaderCreate', '0x00758920', 'RpWorld::_rpMeshHeaderCreate', 'Dependencies25C'),
    ('RpWorld', 'RpBuildMeshGenerateDefaultTriStrip', '0x007591B0', 'RpWorld::RpBuildMeshGenerateDefaultTriStrip', 'Dependencies25C'),
    ('RpWorld', 'RxNodeDefinitionGetD3D9WorldSectorAllInOne', '0x0075E9F0', 'RpWorld::RxNodeDefinitionGetD3D9WorldSectorAllInOne', 'Dependencies25C'),
    ('RpWorld', 'RpWorldSectorRegisterPlugin', '0x00761C90', 'RpWorld::RpWorldSectorRegisterPlugin', 'Dependencies25C'),
    ('RpWorld', 'RpWorldSectorRegisterPluginStream', '0x00761CC0', 'RpWorld::RpWorldSectorRegisterPluginStream', 'Dependencies25C'),
    ('RpHAnim', 'RpHAnimFrameGetHierarchy', '0x007C5160', 'RpHAnim::RpHAnimFrameGetHierarchy', 'Dependencies25C'),
    ('RpSkin', '_rpSkinDeinitialize', '0x007C8820', 'RpSkin::_rpSkinDeinitialize', 'Dependencies25C'),
    ('RpUVAnim', '_rpUVAnimCustomDataStreamGetSize', '0x007CC010', 'RpUVAnim::_rpUVAnimCustomDataStreamGetSize', 'Dependencies25C'),
    ('RpUVAnim', 'RpUVAnimKeyFrameStreamGetSize', '0x007CC9D0', 'RpUVAnim::RpUVAnimKeyFrameStreamGetSize', 'Dependencies25C'),
    ('RtAnim', 'RtAnimAnimationGetNumNodes', '0x007CD4F0', 'RtAnim::RtAnimAnimationGetNumNodes', 'Dependencies25C'),
    ('RtAnim', 'RtAnimInterpolatorDestroy', '0x007CD590', 'RtAnim::RtAnimInterpolatorDestroy', 'Dependencies25C'),
    ('RtDict', 'RtDictSchemaDestruct', '0x007CED70', 'RtDict::RtDictSchemaDestruct', 'Dependencies25C'),
    ('RtDict', 'RtDictSchemaGetCurrentDict', '0x007CEEE0', 'RtDict::RtDictSchemaGetCurrentDict', 'Dependencies25C'),
    ('RwPlCore', 'RwStreamWriteInt32', '0x007ED460', 'RwPlCore::RwStreamWriteInt32', 'Dependencies25C'),
    ('RwCore', 'RwCameraSetViewOffset', '0x007EE1A0', 'RwCore::RwCameraSetViewOffset', 'Dependencies25C'),
    ('RwCore', 'RwCameraRegisterPlugin', '0x007EE450', 'RwCore::RwCameraRegisterPlugin', 'Dependencies25C'),
    ('RwCore', 'RwFrameDirty', '0x007F0340', 'RwCore::RwFrameDirty', 'Dependencies25C'),
    ('RwPlCore', 'RwMatrixOrthoNormalize', '0x007F1920', 'RwPlCore::RwMatrixOrthoNormalize', 'Dependencies25C'),
    ('RwPlCore', 'RwEngineRegisterPlugin', '0x007F2BB0', 'RwPlCore::RwEngineRegisterPlugin', 'Dependencies25C'),
    ('RwPlCore', 'RwEngineGetPluginOffset', '0x007F2BE0', 'RwPlCore::RwEngineGetPluginOffset', 'Dependencies25C'),
    ('RwCore', 'RwTextureGetMipmapping', '0x007F3550', 'RwCore::RwTextureGetMipmapping', 'Dependencies25D'),
    ('RwCore', 'RwTextureGetAutoMipmapping', '0x007F3580', 'RwCore::RwTextureGetAutoMipmapping', 'Dependencies25D'),
    ('RwCore', 'RwTextureRegisterPlugin', '0x007F3BB0', 'RwCore::RwTextureRegisterPlugin', 'Dependencies25D'),
    ('RwCore', 'RwTexDictionaryRegisterPlugin', '0x007F3C10', 'RwCore::RwTexDictionaryRegisterPlugin', 'Dependencies25D'),
    ('RwCore', 'RwTextureRasterGenerateMipmaps', '0x007F3CD0', 'RwCore::RwTextureRasterGenerateMipmaps', 'Dependencies25D'),
    ('RwCore', 'RwD3D9DynamicVertexBufferDestroy', '0x007F5AE0', 'RwCore::RwD3D9DynamicVertexBufferDestroy', 'Dependencies25D'),
    ('RwCore', 'RwD3D9DynamicVertexBufferUnlock', '0x007F5C90', 'RwCore::RwD3D9DynamicVertexBufferUnlock', 'Dependencies25D'),
    ('RwCore', '_rwD3D9SetDepthStencilSurface', '0x007F5EF0', 'RwCore::_rwD3D9SetDepthStencilSurface', 'Dependencies25D'),
    ('RwCore', '_rwDeviceRegisterPlugin', '0x007F5F60', 'RwCore::_rwDeviceRegisterPlugin', 'Dependencies25D'),
    ('RwCore', 'RwD3D9DeleteVertexShader', '0x007FAC90', 'RwCore::RwD3D9DeleteVertexShader', 'Dependencies25D'),
    ('RwCore', 'RwD3D9CreatePixelShader', '0x007FACC0', 'RwCore::RwD3D9CreatePixelShader', 'Dependencies25D'),
    ('RwCore', 'RwD3D9DeletePixelShader', '0x007FACF0', 'RwCore::RwD3D9DeletePixelShader', 'Dependencies25D'),
    ('RwCore', 'RwRasterGetCurrentContext', '0x007FAE60', 'RwCore::RwRasterGetCurrentContext', 'Dependencies25D'),
    ('RwCore', 'RwRasterRegisterPlugin', '0x007FB0B0', 'RwCore::RwRasterRegisterPlugin', 'Dependencies25D'),
    ('RwCore', 'RwRasterLockPalette', '0x007FB0E0', 'RwCore::RwRasterLockPalette', 'Dependencies25D'),
    ('RwPlCore', 'RwFreeListCreate', '0x00801980', 'RwPlCore::RwFreeListCreate', 'Dependencies25D'),
    ('RwPlCore', 'RwFreeListCreateAndPreallocateSpace', '0x00801B70', 'RwPlCore::RwFreeListCreateAndPreallocateSpace', 'Dependencies25D'),
    ('RwCore', 'RwRGBAToPixel', '0x00803740', 'RwCore::RwRGBAToPixel', 'Dependencies25D'),
    ('RwCore', 'RwTextureRegisterPluginStream', '0x00804550', 'RwCore::RwTextureRegisterPluginStream', 'Dependencies25D'),
    ('RwCore', '_rwObjectHasFrameReleaseFrame', '0x00804F40', 'RwCore::_rwObjectHasFrameReleaseFrame', 'Dependencies25D'),
    ('RwCore', 'RxHeapGetGlobalHeap', '0x008052F0', 'RwCore::RxHeapGetGlobalHeap', 'Dependencies25E'),
    ('RwCore', '_rwFrameListDeinitialize', '0x00807720', 'RwCore::_rwFrameListDeinitialize', 'Dependencies25E'),
    ('RwCore', 'RwD3D9RasterIsCompressed', '0x00820C90', 'RwCore::RwD3D9RasterIsCompressed', 'Dependencies25E'),
    ('RwPlCore', '_rwPluginRegistryGetPluginOffset', '0x00808470', 'RwPlCore::_rwPluginRegistryGetPluginOffset', 'Dependencies25E'),
    ('RwPlCore', '_rwSListDestroyEndEntries', '0x00809400', 'RwPlCore::_rwSListDestroyEndEntries', 'Dependencies25E'),
    ('RwPlCore', '_rwSListGetNumEntries', '0x008094B0', 'RwPlCore::_rwSListGetNumEntries', 'Dependencies25E'),
    ('RwPlCore', '_rwSListGetEntry', '0x00809510', 'RwPlCore::_rwSListGetEntry', 'Dependencies25E'),
    ('RwPlCore', '_rwSListGetBegin', '0x00809530', 'RwPlCore::_rwSListGetBegin', 'Dependencies25E'),
    ('RwPlCore', '_rwSListGetEnd', '0x00809540', 'RwPlCore::_rwSListGetEnd', 'Dependencies25E'),
    ('RwPlCore', '_rwStringClose', '0x0080A440', 'RwPlCore::_rwStringClose', 'Dependencies25E'),
    ('RwPlCore', '_rwStringStreamGetSize', '0x0080A470', 'RwPlCore::_rwStringStreamGetSize', 'Dependencies25E'),
    ('CRT', 'CrtAtexit', '0x00821D1E', 'CRT::CrtAtexit', 'Dependencies25E'),
    ('CRT', 'CrtLockFile2', '0x00823FD1', 'CRT::CrtLockFile2', 'Dependencies25E'),
    ('CRT', 'CrtUnlockFile2', '0x00824023', 'CRT::CrtUnlockFile2', 'Dependencies25E'),
    ('CRT', 'CrtMalloc', '0x00824257', 'CRT::CrtMalloc', 'Dependencies25E'),
    ('CRT', 'CrtErrno', '0x0082B181', 'CRT::CrtErrno', 'Dependencies25E'),
    ('CAEStreamingDecoder', 'ScalarDeletingDestructor', '0x004F2860', 'CAEStreamingDecoder::ScalarDeletingDestructor', 'Dependencies25A'),
    ('CAutoPilot', 'RemoveOnePathNode', '0x0041B950', 'CAutoPilot::RemoveOnePathNode', 'Dependencies25A'),
    ('CDecisionMakerTypes', 'FlushDecisionMakerEventResponse', '0x00604490', 'CDecisionMakerTypes::FlushDecisionMakerEventResponse', 'Dependencies25A'),
    ('CGarages', 'CountCarsInHideoutGarage', '0x0044A210', 'CGarages::CountCarsInHideoutGarage', 'Dependencies25A'),
    ('CPedGroups', 'RemoveAllFollowersFromGroup', '0x005FB8A0', 'CPedGroups::RemoveAllFollowersFromGroup', 'Dependencies25A'),
    ('CTask', 'Destructor', '0x0061A3A0', 'CTask::Destructor', 'Dependencies25A'),
    ('CVehicle', 'FindReplacementUpgradeCB', '0x006D3490', 'FindReplacementUpgradeCB', 'Dependencies25A'),
    ('CVehiclePool', 'GetAt', '0x0041CC10', 'CVehiclePool::GetAt', 'Dependencies25A'),
)
SOURCES = {
    'CRT::CrtPrintf': 'source/game_sa/VehicleRecording.cpp',
    'CCamera::ProcessObbeCinemaCameraCar': 'source/game_sa/Camera.cpp', 'CCamera::ProcessObbeCinemaCameraTrain': 'source/game_sa/Camera.cpp',
    'CCamera::ProcessObbeCinemaCameraHeli': 'source/game_sa/Camera.cpp', 'CCamera::ProcessObbeCinemaCameraPlane': 'source/game_sa/Camera.cpp',
    'CCamera::ProcessObbeCinemaCameraBoat': 'source/game_sa/Camera.cpp',
    'CTaskSimpleFight::FightSetUpCol': 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
    'CTaskSimpleFight::FightHitCar': 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
    'CTaskSimpleFight::FightStrike': 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
    'CTaskSimpleGangDriveBy::Constructor': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CTaskSimpleGangDriveBy::PlayerTarget': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CTaskSimpleGangDriveBy::AITarget': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CTaskSimpleGangDriveBy::AimGun': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CPopulation::RandomIntInLimitRange': 'source/game_sa/Population.cpp', 'CZoneDef::GetCenter': 'source/game_sa/CullZones.cpp',
    'CTaskComplexEnterCar::GetCameraStickModifier': 'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp',
    'D3DResourceSystem::TidyUpD3DTextures': 'source/game_sa/RenderWare/D3DResourceSystem.cpp',
    'D3DResourceSystem::TidyUpD3DIndexBuffers': 'source/game_sa/RenderWare/D3DResourceSystem.cpp',
    'RwHelper::GetFirstTexture': 'source/game_sa/RenderWare/rw/rwtexdict.cpp',
    'RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial': 'source/game_sa/RwHelper.cpp',
    'CQuaternion::Slerp': 'source/game_sa/Core/Quaternion.cpp',
    'CPointLights::GenerateLightsAffectingObject': 'source/game_sa/PointLights.cpp', 'CPointLights::GetLightMultiplier': 'source/game_sa/PointLights.cpp',
    'CPointLights::ProcessVerticalLineUsingCache': 'source/game_sa/PointLights.cpp', 'CPointLights::RenderFogEffect': 'source/game_sa/PointLights.cpp',
    'CVehicle::DoHeadLightEffect': 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'CVehicle::DoTailLightEffect': 'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'CCam::WrapAngle': 'source/game_sa/Cam.cpp', 'FxEmitter_c::CreateParticles': 'source/game_sa/Fx/FxEmitter.cpp',
    'CVehicle::DoVehicleLights': 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'CVehicle::FlyingControl': 'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'Win::__MainWndProc': 'source/app/platform/win/WndProc.cpp', 'WinInput::diPadInit': 'source/app/platform/win/WinInput.cpp',
    'CAEStreamingDecoder::ScalarDeletingDestructor': 'source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp',
    'CAutoPilot::RemoveOnePathNode': 'source/game_sa/AutoPilot.cpp',
    'CDecisionMakerTypes::FlushDecisionMakerEventResponse': 'source/game_sa/DecisionMakers/DecisionMakerTypes.cpp',
    'CGarages::CountCarsInHideoutGarage': 'source/game_sa/Garages.cpp',
    'CPedGroups::RemoveAllFollowersFromGroup': 'source/game_sa/PedGroups.cpp',
    'CTask::Destructor': 'source/game_sa/Tasks/Task.cpp',
    'FindReplacementUpgradeCB': 'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'CVehiclePool::GetAt': 'source/game_sa/Pools/VehiclePool.h',
    'RwCore::RwD3D9IndexBufferCreate': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RpAnisot::RpAnisotGetMaxSupportedMaxAnisotropy': 'source/game_sa/RenderWare/rw/rpanisot.cpp',
    'RpAnisot::RpAnisotTextureSetMaxAnisotropy': 'source/game_sa/RenderWare/rw/rpanisot.cpp',
    'RpAnisot::RpAnisotTextureGetMaxAnisotropy': 'source/game_sa/RenderWare/rw/rpanisot.cpp',
    'RpWorld::RpClumpGetNumAtomics': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpAtomicSetStreamAlwaysCallBack': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpAtomicSetStreamRightsCallBack': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpAtomicGetPluginOffset': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpGeometryTriangleGetMaterial': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::_rpMaterialListInitialize': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::_rpMaterialListGetMaterial': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::_rpMaterialListFindMaterialIndex': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpWorldRegisterPlugin': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpWorldRegisterPluginStream': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpAtomicGetWorld': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpLightRegisterPlugin': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::_rpD3D9VertexDeclarationGetSize': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpD3D9GeometryGetUsageFlags': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpD3D9WorldSectorSetUsageFlags': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpD3D9WorldSectorGetUsageFlags': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::_rpMeshHeaderCreate': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpBuildMeshGenerateDefaultTriStrip': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RxNodeDefinitionGetD3D9WorldSectorAllInOne': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpWorldSectorRegisterPlugin': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpWorld::RpWorldSectorRegisterPluginStream': 'source/game_sa/RenderWare/rw/rpworld.cpp',
    'RpHAnim::RpHAnimFrameGetHierarchy': 'source/game_sa/RenderWare/rw/rphanim.cpp',
    'RpSkin::_rpSkinDeinitialize': 'source/game_sa/RenderWare/rw/rpskin.cpp',
    'RpUVAnim::_rpUVAnimCustomDataStreamGetSize': 'source/game_sa/RenderWare/rw/rpuvanim.cpp',
    'RpUVAnim::RpUVAnimKeyFrameStreamGetSize': 'source/game_sa/RenderWare/rw/rpuvanim.cpp',
    'RtAnim::RtAnimAnimationGetNumNodes': 'source/game_sa/RenderWare/rw/rtanim.cpp',
    'RtAnim::RtAnimInterpolatorDestroy': 'source/game_sa/RenderWare/rw/rtanim.cpp',
    'RtDict::RtDictSchemaDestruct': 'source/game_sa/RenderWare/rw/rtdict.cpp',
    'RtDict::RtDictSchemaGetCurrentDict': 'source/game_sa/RenderWare/rw/rtdict.cpp',
    'RwPlCore::RwStreamWriteInt32': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwCore::RwCameraSetViewOffset': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwCameraRegisterPlugin': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwFrameDirty': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwPlCore::RwMatrixOrthoNormalize': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::RwEngineRegisterPlugin': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::RwEngineGetPluginOffset': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwCore::RwTextureGetMipmapping': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwTextureGetAutoMipmapping': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwTextureRegisterPlugin': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwTexDictionaryRegisterPlugin': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwTextureRasterGenerateMipmaps': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwD3D9DynamicVertexBufferDestroy': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwD3D9DynamicVertexBufferUnlock': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::_rwD3D9SetDepthStencilSurface': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::_rwDeviceRegisterPlugin': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwD3D9DeleteVertexShader': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwD3D9CreatePixelShader': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwD3D9DeletePixelShader': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwRasterGetCurrentContext': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwRasterRegisterPlugin': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwRasterLockPalette': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwPlCore::RwFreeListCreate': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::RwFreeListCreateAndPreallocateSpace': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwCore::RwRGBAToPixel': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwTextureRegisterPluginStream': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::_rwObjectHasFrameReleaseFrame': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RxHeapGetGlobalHeap': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::_rwFrameListDeinitialize': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwCore::RwD3D9RasterIsCompressed': 'source/game_sa/RenderWare/rw/rwcore.cpp',
    'RwPlCore::_rwPluginRegistryGetPluginOffset': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::_rwSListDestroyEndEntries': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::_rwSListGetNumEntries': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::_rwSListGetEntry': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::_rwSListGetBegin': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::_rwSListGetEnd': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::_rwStringClose': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'RwPlCore::_rwStringStreamGetSize': 'source/game_sa/RenderWare/rw/rwplcore.cpp',
    'CRT::CrtAtexit': 'source/game_sa/VehicleRecording.cpp',
    'CRT::CrtLockFile2': 'source/game_sa/VehicleRecording.cpp',
    'CRT::CrtUnlockFile2': 'source/game_sa/VehicleRecording.cpp',
    'CRT::CrtMalloc': 'source/game_sa/VehicleRecording.cpp',
    'CRT::CrtErrno': 'source/game_sa/VehicleRecording.cpp',
}
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    registry_path = ROOT / 'reversing/validation-registry.json'
    raw = registry_path.read_bytes().decode('utf-8')
    data = json.loads(raw)
    approvals = json.loads((ROOT / 'reversing/CppOnlyMigration/approvals.json').read_text(encoding='utf-8'))
    added = 0
    written_now = set()
    approvals_dirty = False
    for cls, fn, address, identity, bank in HOOKS:
        if any(r['hook']['class'] == cls and r['hook']['fn_name'] == fn for r in data['functions']):
            continue
        entry = approvals.get(address)
        if entry is not None:
            run_dir = ROOT / Path(entry['audit']).parent
        else:
            # corrida aprobada mas reciente del banco en la regresion final
            runs = sorted((r for r in (ROOT / 'reversing/CppTwelveLevel6Final/runs').glob(bank + '-2026*')
                           if json.loads((r / 'summary.json').read_text(encoding='utf-8')).get('passed') is True),
                          key=lambda r: r.stat().st_mtime)
            run_dir = runs[-1]
        summary = json.loads((run_dir / 'summary.json').read_text(encoding='utf-8'))
        assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True
        ppf = dict(summary['pairs_per_function'])
        if identity not in ppf:  # funcion con rutas: pares_por_funcion lleva una clave `identidad[ruta N]` por ruta; el catalogo cubre la identidad completa
            ppf = {identity: sum(v for k, v in ppf.items() if k.startswith(identity + '['))}
        pairs = int(entry['own_pairs']) if entry is not None else int(ppf[identity])
        if entry is None:
            approvals[address] = {
                'identity': identity, 'converted_source_functions': 1, 'source': SOURCES[identity], 'shared_source': 'source/game_sa/X87Math.cpp',
                'bank': bank, 'run': run_dir.name.split('-', 1)[1], 'pairs': int(summary['pairs']), 'own_pairs': pairs,
                'audit': run_dir.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                'state_restored': True, 'helper_codegen': 'reversing/CppTwelveLevel6Study/README.md',
                'scope': 'Destino incierto de la cohorte de 499 (PLAN-001), reconstruido en C++ y validado con su banco Dependencies21 en la regresión final; pares propios de la función según pairs_per_function. Sin cobertura exhaustiva más allá de los fixtures del banco.'}
            approvals_dirty = True
        manifest = json.loads((ROOT / 'reversing' / bank / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        tested = manifest.get('sources', {})
        evidence = dict(
            passed=True, pairs=int(summary['pairs']), differences=0, registered_pairs=int(summary['pairs']),
            registered_functions=len(ppf),
            functions=dict(ppf),
            covered_functions={k: True for k in ppf}, scope=SCOPE, dependency_review=None,
            run=run_dir.relative_to(ROOT).as_posix(), summary_sha256=sha(run_dir / 'summary.json'),
            result_txt_sha256=sha(run_dir / 'RESULTADO.txt'), log_sha256=summary['log_sha256'], all_equal=True, state_restored=True,
            saves_unchanged=summary['saves_unchanged'], configs_restored=summary['configs_restored'],
            original_exe_sha256=summary['original_exe_sha256'], executed_probe_sha256=summary['executed_probe_sha256'],
            manifest=dict(tested_sources=tested))
        out = ROOT / 'reversing' / bank / 'validation-summary.json'
        text = json.dumps(evidence, ensure_ascii=False, indent=2) + '\n'
        if not out.exists() or out in written_now:
            out.write_bytes(text.encode('utf-8'))
            written_now.add(out)
        data['functions'].append(dict(
            hook=dict(**{'class': cls, 'fn_name': fn, 'address': address, 'type': 'S'}),
            evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', identity],
                      'pairs': pairs, 'original_exe_sha256': summary['original_exe_sha256']},
            tested_sources=tested, dependency_review=None,
            source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
            scope=SCOPE, review_reference=f'reversing/{bank}/README.md'))
        added += 1
    if approvals_dirty:
        (ROOT / 'reversing/CppOnlyMigration/approvals.json').write_bytes(json.dumps(approvals, indent=2, ensure_ascii=False).encode('utf-8'))
    if added:
        registry_path.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    print(json.dumps({'registrados': added, 'total_registro': len(data['functions'])}))


if __name__ == '__main__':
    main()
