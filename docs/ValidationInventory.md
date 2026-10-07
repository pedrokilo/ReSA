# Inventario de validación de funciones reconstruidas

[Panel](ReversedClasses.md) · [Registro y método](../reversing/ValidationRegistry/README.md)

Universo de esta revisión: **7,947 entradas reversed=1**, de 8,441 registradas. Las 494 pendientes de reconstrucción quedan fuera de este inventario, pero siguen dentro del denominador del panel.

| Estado | Entradas |
| --- | ---: |
| `validated=1` · evidencia aprobada vinculada | 6073 |
| Vigencia acreditada para su alcance | 0 |
| Evidencia histórica vinculada; revisión pendiente | 6073 |
| Sin evidencia aprobada vinculada en este registro | 1874 |

`validated=1` registra que existe evidencia aprobada al menos una vez; la vigencia actual se calcula aparte con hashes de fuentes y revisión de dependencias. Sin vincular no significa nunca probada ni fallida. Hay bancos todavía no importados. Cada entrada tiene una sola clasificación; los pares y las repeticiones no multiplican funciones. Las funciones virtuales pueden tener dirección cero en el CSV: se identifican también por clase, nombre y tipo.

## Siguiente tanda con evidencia disponible

Revisar fuentes, versión, configuración y dependencias; repetir el banco cuando no se pueda confirmar su vigencia. Esta lista no autoriza desbloquear el lote aplazado descrito en la [continuidad vigente](../reversing/MEJORAS_DEL_PROCESO.md#continuidad-vigente). No se marca toda una clase por validar una de sus funciones.

| Clase | Función registrada | Evidencia |
| --- | --- | --- |
| App | `AppEventHandler` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| App | `DefinedState` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| App | `DefinedState2d` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| App | `DoRWStuffEndOfFrame` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| App | `DoRWStuffStartOfFrame` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| App | `DoRWStuffStartOfFrame_Horizon` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| App | `PluginAttach` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| BoneNodeManager_c | `Constructor` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNodeManager_c | `Destructor` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNodeManager_c | `Exit` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNodeManager_c | `GetBoneNode` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNodeManager_c | `GetNumBoneNodesLeft` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNodeManager_c | `Init` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNodeManager_c | `Reset` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNodeManager_c | `ReturnBoneNode` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `AddChild` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `BlendKeyframe` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `CalcWldMat` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `ClampLimitsCurrent` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `ClampLimitsDefault` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `Constructor` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `Destructor` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `EulerToQuat` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `GetIdFromBoneTag` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `GetLimits` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `GetSpeed` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `Init` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `InitLimits` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `Limit` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `QuatToEuler` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `SetLimits` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BoneNode_c | `SetSpeed` | [Resumen](../reversing/BoneNodes/validation-summary.json) |
| BreakManager_c | `Add` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakManager_c | `Exit` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakManager_c | `Init` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakManager_c | `Render` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakManager_c | `ResetAll` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakManager_c | `Update` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakObject_c | `CalcGroupCenter` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `DoCollision` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `DoCollisionResponse` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `Exit` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `Init` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `Render` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `SetBreakInfo` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `SetGroupData` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakObject_c | `Update` | [Resumen](../reversing/BreakObj/validation-summary.json) |
| BreakablePlugin | `BreakableConstructor` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakablePlugin | `BreakableDestructor` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakablePlugin | `BreakablePluginAttach` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakablePlugin | `BreakableStreamGetSize` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakablePlugin | `BreakableStreamRead` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| BreakablePlugin | `BreakableStreamWrite` | [Resumen](../reversing/BreakBatch/validation-summary.json) |
| C2dEffect | `DestroyAtomic` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `PluginAttach` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `Roadsign_GetNumLettersFromFlags` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `Roadsign_GetNumLinesFromFlags` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `Roadsign_GetPaletteIDFromFlags` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `RpGeometryGet2dFxAtIndex` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `RpGeometryGet2dFxCount` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `Rwt2dEffectPluginDataChunkGetSizeCallBack` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `Rwt2dEffectPluginDataChunkReadCallBack` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `Rwt2dEffectPluginDataChunkWriteCallBack` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `Shutdown` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `t2dEffectPluginConstructor` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `t2dEffectPluginCopyConstructor` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C2dEffect | `t2dEffectPluginDestructor` | [Resumen](../reversing/TwoDEffect/validation-summary.json) |
| C3dMarker | `AddMarker` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarker | `DeleteMarkerObject` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarker | `IsZCoordinateUpToDate` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarker | `Render` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarker | `SetZCoordinateIfNotUpToDate` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarker | `UpdateZCoordinate` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `DirectionArrowFindFirstFreeSlot` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `DirectionArrowSet` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `DirectionArrowsDraw` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `DirectionArrowsInit` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `ForceRender` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `Init` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `LoadUser3dMarkers` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `PlaceMarker` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `PlaceMarkerCone` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `PlaceMarkerSet` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `Render` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `SaveUser3dMarkers` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `Shutdown` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `Update` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `User3dMarkerAtomicCB` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `User3dMarkerDelete` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `User3dMarkerDeleteAll` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `User3dMarkerFindFirstFreeSlot` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `User3dMarkerSet` | [Resumen](../reversing/Markers/validation-summary.json) |
| C3dMarkers | `User3dMarkersDraw` | [Resumen](../reversing/Markers/validation-summary.json) |
| CAEAmbienceTrackManager | `IsAmbienceRadioActive` | [Resumen](../reversing/AmbienceTrack/validation-summary.json) |
| CAEAmbienceTrackManager | `IsAmbienceTrackActive` | [Resumen](../reversing/AmbienceTrack/validation-summary.json) |
| CAEAmbienceTrackManager | `PlaySpecialMissionAmbienceTrack` | [Resumen](../reversing/AmbienceTrack/validation-summary.json) |
| CAEAmbienceTrackManager | `Reset` | [Resumen](../reversing/AmbienceTrack/validation-summary.json) |
| CAEAmbienceTrackManager | `Service` | [Resumen](../reversing/AmbienceTrack/validation-summary.json) |
| CAEAmbienceTrackManager | `StopSpecialMissionAmbienceTrack` | [Resumen](../reversing/AmbienceTrack/validation-summary.json) |
| CAEAmbienceTrackManager | `UpdateAmbienceTrackAndVolume` | [Resumen](../reversing/AudioCompletion/validation-summary.json) |
| CAEAudioChannel | `ConvertFromBytesToMS` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `ConvertFromMsToBytes` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `GetCurrentPlaybackPosition` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `Lost` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `SetFrequency` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `SetFrequencyScalingFactor` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `SetNotInRoom` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `SetOriginalFrequency` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `SetPosition` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `SetReverbAndDepth` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `SetVolume` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioChannel | `UpdateStatus` | [Resumen](../reversing/AudioChannel/validation-summary.json) |
| CAEAudioEntity | `Destructor` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioEntity | `UpdateParameters` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioEnvironment | `GetDirectionalMikeAttenuation` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioEnvironment | `GetDistanceAttenuation` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioEnvironment | `GetDopplerRelativeFrequency` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioEnvironment | `GetPositionRelativeToCamera-placeable` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioEnvironment | `GetPositionRelativeToCamera-vec` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioEnvironment | `GetReverbEnvironmentAndDepth` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioHardware | `AllocateChannels` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `CheckDVD` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `Constructor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `Destructor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `DisableBassEq` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `DisableEffectsLoading` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `EnableBassEq` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `EnableEffectsLoading` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetActiveTrackID` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetBeatInfo` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetChannelPlayTimes` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetDVDDriveLetter` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetEffectsFaderScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetEffectsMasterScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetMusicMasterScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetNumAvailableChannels` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetPlayingTrackID` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetSoundBankLoadingStatus` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetSoundHeadroom` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetSoundLoadingStatus` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetTrackLengthMs` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetTrackPlayTime` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetVirtualChannelSoundLengths` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `GetVirtualChannelSoundLoopStartTimes` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `InitDirectSoundListener` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `IsSoundBankLoaded` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `IsSoundLoaded` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `IsStreamingFromDVD` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `LoadSound` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `LoadSoundBank` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `PauseAllSounds` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `PlayTrack` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `Query3DSoundEffects` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `RequestVirtualChannelSoundInfo` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `RescaleChannelVolumes` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `ResumeAllSounds` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `Service` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetBassSetting` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetChannelFlags` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetChannelFrequencyScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetChannelPosition` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetChannelVolume` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetEffectsFaderScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetEffectsMasterScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetMusicFaderScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetMusicMasterScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetNonStreamFaderScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `SetStreamFaderScalingFactor` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `StartTrackPlayback` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `StopSound` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `StopTrack` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `Terminate` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioHardware | `UpdateReverbEnvironment` | [Resumen](../reversing/AudioHw54/validation-summary.json) |
| CAEAudioUtility | `AudioLog10` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `ConvertFromBytesToMS` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `ConvertFromMSToBytes` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `FindVehicleOfPlayer` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `GetBankAndSoundFromScriptSlotAudioEvent` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `GetCurrentTimeInMS` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `GetPiecewiseLinear` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `GetRandomNumberInRange<float>-float` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `GetRandomNumberInRange<int32>-int` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `ResolveProbability` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEAudioUtility | `StaticInitialise` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEBankLoader | `CalculateBankSlotsInfosOffsets` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEBankLoader | `Deconstructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEBankLoader | `GetBankLookup` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEBankLoader | `LoadBankLookupFile` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEBankLoader | `LoadBankSlotFile` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEBankLoader | `LoadSFXPakLookupFile` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAECollisionAudioEntity | `AddCollisionSoundToList` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `ChooseCollisionSoundID` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `GetCollisionSoundStatus` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `Initialise` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `InitialisePostLoading` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `PlayBulletHitCollisionSound` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `PlayLoopingCollisionSound` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `PlayOneShotCollisionSound` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `ReportBulletHit` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `ReportCollision` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `ReportGlassCollisionEvent` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `ReportObjectDestruction` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `ReportWaterSplash-at-position` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `ReportWaterSplash-for-physical` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `Reset` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `Service` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `UpdateLoopingCollisionSound` | [Resumen](../reversing/Audio17/validation-summary.json) |
| CAECollisionAudioEntity | `UpdateParameters` | [Resumen](../reversing/FixAudio/validation-summary.json) |
| CAECutsceneTrackManager | `GetCutsceneTrackStatus` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAECutsceneTrackManager | `Initialise` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAECutsceneTrackManager | `IsCutsceneTrackActive` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAECutsceneTrackManager | `PauseTrack` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAECutsceneTrackManager | `PlayPreloadedCutsceneTrack` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAECutsceneTrackManager | `PreloadBeatTrack` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAECutsceneTrackManager | `Service` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAECutsceneTrackManager | `StopCutsceneTrack` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAEDataStream | `AddRef` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Clone` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Commit` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Constructor` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEDataStream | `CopyTo` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Destructor` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEDataStream | `FillBuffer` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEDataStream | `GetCurrentPosition` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEDataStream | `Initialise` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEDataStream | `LockRegion` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `QueryInterface` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Read` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Release` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Revert` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Seek-OG` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEDataStream | `Seek-istream` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `SetSize` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Stat` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `UnlockRegion` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDataStream | `Write` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEDoorAudioEntity | `AddAudioEvent-1` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEDoorAudioEntity | `AddAudioEvent-2` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEDoorAudioEntity | `PlayDoorSound` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEDoorAudioEntity | `Reset` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEDoorAudioEntity | `StaticInitialise` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEDoorAudioEntity | `UpdateParameters` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEExplosionAudioEntity | `AddAudioEvent` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEExplosionAudioEntity | `StaticInitialise` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEExplosionAudioEntity | `UpdateParameters` | [Resumen](../reversing/Small2/validation-summary.json) |
| CAEFireAudioEntity | `AddAudioEvent` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEFireAudioEntity | `Initialise` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEFireAudioEntity | `PlayFireSounds` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEFireAudioEntity | `PlayWaterSounds` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEFireAudioEntity | `StaticInitialise` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEFireAudioEntity | `Terminate` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEFireAudioEntity | `UpdateParameters` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEGlobalWeaponAudioEntity | `AddAudioEvent` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEGlobalWeaponAudioEntity | `Constructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEGlobalWeaponAudioEntity | `Destructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEGlobalWeaponAudioEntity | `ProjectileFire` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEGlobalWeaponAudioEntity | `ServiceAmbientGunFire` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEGlobalWeaponAudioEntity | `UpdateParameters` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAEMFDecoder | `Constructor` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAEMFDecoder | `Destructor` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAEMFDecoder | `FillBuffer` | [Resumen](../reversing/MFDec/validation-summary.json) |
| CAEMFDecoder | `GetSampleRate` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAEMFDecoder | `GetStreamID` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAEMFDecoder | `GetStreamLengthMs` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAEMFDecoder | `GetStreamPlayTimeMs` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAEMFDecoder | `InitLibrary` | [Resumen](../reversing/MFDec/validation-summary.json) |
| CAEMFDecoder | `Initialise` | [Resumen](../reversing/MFDec/validation-summary.json) |
| CAEMFDecoder | `SetCursor` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAEMP3BankLoader | `Constructor` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `GetBankSlotBuffer` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `GetSoundBankLoadingStatus` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `GetSoundBuffer` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `GetSoundHeadroom` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `GetSoundLoadingStatus` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `Initialise` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `IsSoundBankLoaded` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `IsSoundLoaded` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `LoadSound` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `LoadSoundBank` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `Service` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3BankLoader | `UpdateVirtualChannels` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEMP3TrackLoader | `Constructor` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `Deconstructor` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `GetTrackInfo` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `Initialise` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `IsCurrentAudioStreamAvailable` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `LoadStreamPackTable` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `LoadTrackLookupTable` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEPedAudioEntity | `UpdateParameters` | [Resumen](../reversing/FixAudio/validation-summary.json) |
| CAEPedSpeechAudioEntity | `AddSayEvent` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `AddScriptSayEvent` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `CanPedHoldConversation` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `CanPedSayGlobalContext` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `CanWePlayGlobalSpeechContext` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `CanWePlayScriptedSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `Constructor` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `DisableAllPedSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `DisablePedSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `DisablePedSpeechForScriptSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `EnableAllPedSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `EnablePedSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `EnablePedSpeechForScriptSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetAllocatedVoice` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetAudioPedType` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetCurrentCJMood` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetNextMoodToUse` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetNextPlayTime` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetNumSlotsPlayingContext` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetPedTalking` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetPedType` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetRepeatTime` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetSexForSpecialPed` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetSexFromModel` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetSoundAndBankIDs` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetSoundAndBankIDsForScriptedSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetSpecificSpeechContext` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetSpeechContextVolumeOffset` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetVoice` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetVoiceAndTypeForSpecialPed` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetVoiceAndTypeFromModel` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `GetVoiceForMood` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `Initialise` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `IsCJDressedInForGangSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `IsGlobalContextImportantForInterupting` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `IsGlobalContextImportantForStreaming` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `IsGlobalContextImportantForWidescreen` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `IsGlobalContextPain` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `IsGlobalContextUberImportant` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `IsPedFemaleForAudio` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `LoadAndPlaySpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `PlayLoadedSound` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `ReleasePedConversation` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `ReleasePlayerConversation` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `RequestPedConversation` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `RequestPlayerConversation` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `ReservePedConversationSpeechSlots` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `ReservePlayerConversationSpeechSlot` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `Reset` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `Service` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `SetCJMood` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `SetNextPlayTime` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `SetUpConversation` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `StaticInitialise` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `StopCurrentSpeech` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `Terminate` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `UpdateParameters` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedSpeechAudioEntity | `WillPedChatAboutTopic` | [Resumen](../reversing/PedSpeech58/validation-summary.json) |
| CAEPedWeaponAudioEntity | `AddAudioEvent` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAEPedWeaponAudioEntity | `Constructor` | [Resumen](../reversing/WeapAudio/validation-summary.json) |
| CAEPedWeaponAudioEntity | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAEPedWeaponAudioEntity | `Service` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAEPedWeaponAudioEntity | `Terminate` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `AddSayEvent` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `Constructor` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `GetAllocatedVoice` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `GetPedType` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `Initialise` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `IsPedFemaleForAudio` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `PlayLoadedSound` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `StopCurrentSpeech` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `Terminate` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `UpdateParameters` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEPedlessSpeechAudioEntity | `WillPedChatAboutTopic` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAESmoothFadeThread | `CancelFade` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `Initialise` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `InitialiseRequestSlots` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `RequestFade` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `Service` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `SetBufferVolume` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `SmoothFadeProc` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `Start` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `Stop` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESmoothFadeThread | `WaitForExit` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CAESound | `CalculateFrequency` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `CalculateVolume` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `GetRelativePlaybackFrequencyWithDoppler` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `GetRelativePosition-orginal` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `GetSlowMoFrequencyScalingFactor` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `Initialise` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `NewVPSLEntry` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `RegisterWithPhysicalEntity` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `SetFlags` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `SetPosition` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `SoundHasFinished` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `StopSound` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `StopSoundAndForget` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `UnregisterWithPhysicalEntity` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `UpdateFrequency` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `UpdateParameters` | [Resumen](../reversing/Audio18/validation-summary.json) |
| CAESound | `UpdatePlayTime` | [Resumen](../reversing/FixAudio/validation-summary.json) |
| CAESound | `operator=` | [Resumen](../reversing/CAESound/validation-summary.json) |
| CAESoundManager | `AreSoundsOfThisEventPlayingForThisEntity` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `AreSoundsOfThisEventPlayingForThisEntityAndPhysical` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `AreSoundsPlayingInBankSlot` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `CancelSoundsInBankSlot` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `CancelSoundsOfThisEventPlayingForThisEntity` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `CancelSoundsOfThisEventPlayingForThisEntityAndPhysical` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `CancelSoundsOwnedByAudioEntity` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `GetVirtualChannelForPhysicalChannel` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `Initialise` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `PauseManually` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `RequestNewSound` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `Reset` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `Service` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAESoundManager | `Terminate` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStaticChannel | `GetLength` | [Resumen](../reversing/FixAudio/validation-summary.json) |
| CAEStaticChannel | `GetPlayTime` | [Resumen](../reversing/FixAudio/validation-summary.json) |
| CAEStaticChannel | `IsSoundPlaying` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAEStaticChannel | `Play` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAEStaticChannel | `Service` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAEStaticChannel | `SetAudioBuffer` | [Resumen](../reversing/AudioCompletion/validation-summary.json) |
| CAEStaticChannel | `Stop` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAEStaticChannel | `SynchPlayback` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CAEStreamThread | `GetActiveTrackID` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `GetPlayingTrackID` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `GetTrackLengthMs` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `GetTrackPlayTime` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `Initialise` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `MainLoop` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `Pause` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `PlayTrack` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `Resume` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `Service` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `Start` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `Stop` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `StopTrack` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamThread | `WaitForExit` | [Resumen](../reversing/Audio14/validation-summary.json) |
| CAEStreamTransformer | `Initialise` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CAEStreamTransformer | `TransformBuffer` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CAEStreamingChannel | `GetLength` | [Resumen](../reversing/FixAudio/validation-summary.json) |
| CAEStreamingChannel | `GetPlayTime` | [Resumen](../reversing/FixAudio/validation-summary.json) |
| CAEStreamingChannel | `SynchPlayback` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CAEStreamingDecoder | `Constructor` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CAEStreamingDecoder | `Destructor` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CAETrackLoader | `GetDataStream` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CAETwinLoopSoundEntity | `DoSoundsSwitchThisFrame` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAETwinLoopSoundEntity | `Initialise` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAETwinLoopSoundEntity | `PlayTwinLoopSound` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAETwinLoopSoundEntity | `StopSoundAndForget` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAETwinLoopSoundEntity | `SwapSounds` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAETwinLoopSoundEntity | `UpdateParameters` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAETwinLoopSoundEntity | `UpdateTwinLoopSound` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEUserRadioTrackManager | `DeleteUserTracksInfo` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `GetAudioFileType` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `GetTrackPath` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `GetUserTrackPlayMode` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `Initialise` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `IsShortcut` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `LoadUserTrack` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `ReadUserTracks` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `ResolveShortcut-` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `ScanUserTracks` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `SelectUserTrackIndex` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `SetUserTrackIndex` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `Shutdown` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `WriteUserTracksFile-` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEUserRadioTrackManager | `WriteUserTracksThread` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAEVorbisDecoder | `CloseCallback` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `Constructor` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `Destructor` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `FillBuffer` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `GetSampleRate` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `GetStreamID` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `GetStreamLengthMs` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `GetStreamPlayTimeMs` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `Initialise` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `ReadCallback` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `SeekCallback` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `SetCursor` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEVorbisDecoder | `TellCallback` | [Resumen](../reversing/Audio13/validation-summary.json) |
| CAEWMADecoder | `Constructor` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `Destructor` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `FillBuffer` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `GetSampleRate` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `GetStreamID` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `GetStreamLengthMs` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `GetStreamPlayTimeMs` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `InitLibrary` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `Initialise` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `SelectStreamIndex` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWMADecoder | `SetCursor` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CAEWaterCannonAudioEntity | `ClearSplashInfo` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Constructor` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Initialise` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Service` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `SetSplashInfo` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Terminate` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `UpdateParameters` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaveDecoder | `Constructor` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `Destructor` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `FillBuffer` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `GetSampleRate` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `GetStreamID` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `GetStreamLengthMs` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `GetStreamPlayTimeMs` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `Initialise` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWaveDecoder | `SetCursor` | [Resumen](../reversing/Core9/validation-summary.json) |
| CAEWeaponAudioEntity | `Constructor` | [Resumen](../reversing/WeapAudio/validation-summary.json) |
| CAEWeaponAudioEntity | `Initialise` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayCameraSound` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayChainsawStopSound` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayFlameThrowerIdleGasLoop` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayFlameThrowerSounds` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayGoggleSound` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayGunSounds` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayMiniGunFireSounds` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayMiniGunStopSound` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `PlayWeaponLoopSound` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `ReportChainsawEvent` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `ReportStealthKill` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `Reset` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `StopFlameThrowerIdleGasLoop` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `Terminate` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `UpdateParameters` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `WeaponFire` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeaponAudioEntity | `WeaponReload` | [Resumen](../reversing/Audio20/validation-summary.json) |
| CAEWeatherAudioEntity | `AddAudioEvent` | [Resumen](../reversing/AudioCompletion/validation-summary.json) |
| CAEWeatherAudioEntity | `Service` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CAEWeatherAudioEntity | `StaticInitialise` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CAEWeatherAudioEntity | `StaticReset` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CAEWeatherAudioEntity | `UpdateParameters` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CAccidentManager | `GetInstance` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAccidentManager | `GetNearestFreeAccident` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAccidentManager | `GetNearestFreeAccidentExceptThisOne` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAccidentManager | `GetNumberOfFreeAccidents` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAccidentManager | `ReportAccident` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAcquaintance | `ClearAcquaintances` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAcquaintance | `ClearAsAcquaintance` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAcquaintance | `GetAcquaintances` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAcquaintance | `SetAcquaintances` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAcquaintance | `SetAsAcquaintance` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CActiveOccluder | `IsPointBehindOccluder` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CActiveOccluder | `IsPointWithinOcclusionArea` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CAnimBlendAssocGroup | `Constructor` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `CopyAnimation-AnimationId` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `CopyAnimation-Name` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `CreateAssociations-1` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `CreateAssociations-2` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `CreateAssociations-3` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `DestroyAssociations` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `Destructor` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `GetAnimBlock` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `GetAnimation-AnimationId` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `GetAnimation-Name` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `GetAnimationId` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `GetNumAnimations` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `InitEmptyAssociations` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssocGroup | `IsCreated` | [Resumen](../reversing/Audio15/validation-summary.json) |
| CAnimBlendAssociation | `AllocateAnimBlendNodeArray` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Constructor0-` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Constructor1-clump_hier` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Constructor2-blend` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Constructor3-static_blend` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `FreeAnimBlendNodeArray` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Init-0` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Init-1` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Init-2` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `ReferenceAnimBlock` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `SetBlend` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `SetBlendTo` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `SetCurrentTime` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `SetDeleteCallback` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `SetFinishCallback` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `Start` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `SyncAnimation` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `UpdateBlend` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `UpdateTime` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendAssociation | `UpdateTimeStep` | [Resumen](../reversing/Anim20/validation-summary.json) |
| CAnimBlendClumpData | `Constructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `Destructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `ForAllFrames` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `ForAllFramesInSPR` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `LoadFramesIntoSPR` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `SetNumberOfBones` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendHierarchy | `AllocSequenceBlock` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `CalcTotalTime` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CAnimBlendHierarchy | `CalcTotalTimeCompressed` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CAnimBlendHierarchy | `CompressKeyframes` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `Constructor` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `Destructor` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `FindSequence` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `GetSequenceBlock` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `MoveMemory` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `Print` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `RemoveAnimSequences` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `RemoveQuaternionFlips` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `RemoveUncompressedData` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `SetName` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `Shutdown` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendHierarchy | `Uncompress` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `CalcDeltas` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `CalcDeltasCompressed` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `CalcTheta` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `FindKeyFrame` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `GetCurrentTranslation` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `GetCurrentTranslationCompressed` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `GetEndTranslation` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `GetEndTranslationCompressed` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `Init` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `NextKeyFrame` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `NextKeyFrameCompressed` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `NextKeyFrameNoCalc` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `SetupKeyFrameCompressed` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `Update` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `UpdateCompressed` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendNode | `UpdateTime` | [Resumen](../reversing/Anim16/validation-summary.json) |
| CAnimBlendSequence | `CompressKeyframes` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `Constructor` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `Destructor` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `GetCKeyFrame` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `GetDataSize` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `GetUKeyFrame` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `MoveMemory` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `Print` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `RemoveQuaternionFlips` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `RemoveUncompressedData` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `SetBoneTag` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `SetName` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `SetNumFrames` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendSequence | `Uncompress` | [Resumen](../reversing/Render14/validation-summary.json) |
| CAnimBlendStaticAssociation | `AllocateSequenceArray` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Constructor1` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Constructor2` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Destructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `FreeSequenceArray` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Init` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimManager | `AddAnimAssocDefinition` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `AddAnimBlockRef` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `AddAnimToAssocDefinition` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `AddAnimation-hier` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `AddAnimation-id` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `AddAnimationAndSync` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `BlendAnimation-hier` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `BlendAnimation-id` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `CreateAnimAssocGroups` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `CreateAnimAssociation` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimAssociation-by-id` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimAssociation-by-name` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimBlockName` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimGroupName` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimation-0` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimation-1` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimationBlock-` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetAnimationBlockIndex-by-name` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetFirstAssocGroup` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `GetNumRefsToAnimBlock` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `Initialise` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `LoadAnimFile` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `LoadAnimFiles` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `ReadAnimAssociationDefinitions` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `RegisterAnimBlock` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `RemoveAnimBlock` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `RemoveAnimBlockRefWithoutDelete` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `RemoveFromUncompressedCache` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `RemoveLastAnimFile` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `Shutdown` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimManager | `UncompressAnimation` | [Resumen](../reversing/AnimMgr31/validation-summary.json) |
| CAnimatedBuilding | `PreRender` | [Resumen](../reversing/World52/validation-summary.json) |
| CAnimatedBuilding | `ProcessCollision` | [Resumen](../reversing/World52/validation-summary.json) |
| CAnimatedBuilding | `ProcessControl` | [Resumen](../reversing/World52/validation-summary.json) |
| CAtomicModelInfo | `AsAtomicModelInfoPtr` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `DeleteRwObject` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `GetAtomicFromDistance` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `GetModelType` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `GetRwModelType` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `Init` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `SetAtomic` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `SetAtomicModelInfoFlags` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAtomicModelInfo | `SetupVehicleUpgradeFlags` | [Resumen](../reversing/Models9/validation-summary.json) |
| CAudioZones | `Init` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `RegisterAudioBox` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `RegisterAudioSphere` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `SwitchAudioZone` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `Update` | [Resumen](../reversing/World43/validation-summary.json) |
| CBaseModelInfo | `Add2dEffect` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `AddRef` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `AddTexDictionaryRef` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `AsAtomicModelInfoPtr` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `AsDamageAtomicModelInfoPtr` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `AsLodAtomicModelInfoPtr` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `ClearTexDictionary` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `ConvertAnimFileIndex` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `DeleteCollisionModel` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `Get2dEffect` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `GetAnimFileIndex` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `GetIsDrawLast` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `GetTimeInfo` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `HasBeenPreRendered` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `HasComplexHierarchy` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `IncreaseAlpha` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `Init` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `Init2dEffects` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `IsBackfaceCulled` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `IsBreakableStatuePart` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `IsLod` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `IsRoad` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `IsTagModel` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `RemoveRef` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `RemoveTexDictionaryRef` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SetAnimFile` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SetBaseModelInfoFlags` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SetColModel` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SetHasBeenPreRendered` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SetIsLod` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SetOwnsColModel` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SetTexDictionary` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `Shutdown` | [Resumen](../reversing/Model34/validation-summary.json) |
| CBaseModelInfo | `SwaysInWind` | [Resumen](../reversing/CBaseModelInfo/validation-summary.json) |
| CBirds | `CreateNumberOfBirds` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `HandleGunShot` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Init` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Render` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Shutdown` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Update` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBmx | `BurstTyre` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `Constructor` | [Resumen](../reversing/VehCtor62/validation-summary.json) |
| CBmx | `FindWheelWidth` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `LaunchBunnyHopCB` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `PreRender` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `ProcessAI` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `ProcessBunnyHop` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `ProcessControl` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `ProcessDrivingAnims` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBmx | `SetUpWheelColModel` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CBoat | `AddWakePoint` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `BlowUpCar` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `CheckForSkippingCalculations` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `Constructor` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `DebugCode` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `Destructor` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `DisplayHandlingData` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `FillBoatList` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `GetBoatAtomicObjectCB` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `GetComponentWorldPosition` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `IsComponentPresent` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `IsSectorAffectedByWake` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `IsVertexAffectedByWake` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `ModifyHandlingValue` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `PreRender` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `ProcessControl` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `ProcessControlInputs` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `ProcessOpenDoor` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `PruneWakeTrail` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `Render` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `SetModelIndex` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `SetupModelNodes` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBoat | `Teleport` | [Resumen](../reversing/Boat23/validation-summary.json) |
| CBouncingPanel | `ProcessPanel` | [Resumen](../reversing/World52/validation-summary.json) |
| CBouncingPanel | `ResetPanel` | [Resumen](../reversing/World52/validation-summary.json) |
| CBouncingPanel | `SetPanel` | [Resumen](../reversing/World52/validation-summary.json) |
| CBox | `Recalc` | [Resumen](../reversing/Core70/validation-summary.json) |
| CBox | `Set` | [Resumen](../reversing/Core70/validation-summary.json) |
| CBridge | `FindBridgeEntities` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBridge | `Init` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBridge | `ShouldLightsBeFlashing` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBridge | `Update` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBrightLights | `Init` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBrightLights | `RegisterOne` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBrightLights | `Render` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBrightLights | `RenderOutGeometryBuffer` | [Resumen](../reversing/Render23/validation-summary.json) |
| CBuilding | `IsBuildingPointerValid` | [Resumen](../reversing/Core70/validation-summary.json) |
| CBuilding | `ReplaceWithNewModel` | [Resumen](../reversing/Core70/validation-summary.json) |
| CBuildingPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CBulletInfo | `AddBullet` | [Resumen](../reversing/Data23/validation-summary.json) |
| CBulletInfo | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CBulletInfo | `Shutdown` | [Resumen](../reversing/Data23/validation-summary.json) |
| CBulletInfo | `Update` | [Resumen](../reversing/Data23/validation-summary.json) |
| CBulletTraces | `AddTrace-` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CBulletTraces | `AddTrace-Wrapper` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CBulletTraces | `Init` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CBulletTraces | `Render` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CBulletTraces | `Update` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CCarAI | `AddAmbulanceOccupants` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `AddFiretruckOccupants` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `AddPoliceCarOccupants` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `BackToCruisingIfNoWantedLevel` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `CarHasReasonToStop` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `EntitiesGoHeadOn` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `FindPoliceBikeMissionForWantedLevel` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `FindPoliceBoatMissionForWantedLevel` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `FindPoliceCarMissionForWantedLevel` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `FindPoliceCarSpeedForWantedLevel` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `FindSwitchDistanceClose` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `FindSwitchDistanceFar` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `GetCarToGoToCoors` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `GetCarToGoToCoorsAccurate` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `GetCarToGoToCoorsRacing` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `GetCarToGoToCoorsStraightLine` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `GetCarToParkAtCoors` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `MakeWayForCarWithSiren` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `MellowOutChaseSpeed` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `MellowOutChaseSpeedBoat` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `TellCarToBlockOtherCar` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `TellCarToFollowOtherCar` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `TellCarToRamOtherCar` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `TellOccupantsToLeaveCar` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarAI | `UpdateCarAI` | [Resumen](../reversing/CarAI25/validation-summary.json) |
| CCarCtrl | `ChooseBoatModel` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `ChooseCarModelToLoad` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `ChooseGangCarModel` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `ChoosePoliceCarModel` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `CreateCarForScript` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `FindMaxSteerAngle` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `GenerateRandomCars` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `GetNewVehicleDependingOnCarModel` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `Init` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `InitSequence` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `IsAnyoneParking` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `IsThisVehicleInteresting` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `JoinCarWithRoadAccordingToMission` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `PossiblyFireHSMissile` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `PruneVehiclesOfInterest` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `ReInit` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `RemoveCarsIfThePoolGetsFull` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `RemoveDistantCars` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `RemoveFromInterestingVehicleList` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `ScriptGenerateOneEmergencyServicesCar` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `SlowCarDownForObject` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarCtrl | `SlowCarOnRailsDownForTrafficAndLights` | [Resumen](../reversing/CarCtrl22/validation-summary.json) |
| CCarFXRenderer | `CustomCarPipeAtomicSetup` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `CustomCarPipeClumpSetup` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `Initialise` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `InitialiseDirtTexture` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `IsCCPCPipelineAttached` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `PreRenderUpdate` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `RegisterPlugins` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `SetCustomFXAtomicRenderPipelinesVMICB` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `SetFxEnvMapLightMult` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarFXRenderer | `Shutdown` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CCarGenerator | `CalcNextGen` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCarGenerator | `CheckForBlockage` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCarGenerator | `CheckIfWithinRangeOfAnyPlayers` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCarGenerator | `DoInternalProcessing` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCarGenerator | `Process` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCarGenerator | `Setup` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCarGenerator | `SwitchOff` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCarGenerator | `SwitchOn` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoint | `Render` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CCheckpoints | `DeleteCP` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoints | `Init` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoints | `PlaceMarker` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoints | `Render` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoints | `SetHeading` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoints | `Shutdown` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoints | `Update` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCheckpoints | `UpdatePos` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCivilianPed | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CCivilianPed | `Destructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CCivilianPed | `ProcessControl` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CClock | `GetGameClockMinutesUntil` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `GetIsTimeInRange` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `Initialise` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `NormaliseGameClock` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `OffsetClockByADay` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `RestoreClock` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `SetGameClock` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `StoreClock` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClock | `Update` | [Resumen](../reversing/Core9/validation-summary.json) |
| CClothes | `ConstructPedModel` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `GetDefaultPlayerMotionGroup` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `GetDependentTexture` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `GetPlayerMotionGroupToLoad` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `GetTextureDependency` | [Resumen](../reversing/CClothes/validation-summary.json) |
| CClothes | `Init` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `LoadClothesFile` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `RebuildCutscenePlayer` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `RebuildPlayer` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `RebuildPlayerIfNeeded` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClothes | `RequestMotionGroupAnims` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CClouds | `Init` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFogInit` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFogRender` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFog_Create` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFog_Delete` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFog_GetFXIntensity` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFog_GetFirstFreeSlot` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFog_GetWind` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `MovingFog_Update` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `Render` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `RenderBottomFromHeight` | [Resumen](../reversing/CClouds/validation-summary.json) |
| CClouds | `RenderSkyPolys` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `SetUpOneSkyPoly` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `Shutdown` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `Update` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `VolumetricCloudsGetMaxDistance` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `VolumetricCloudsInit` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `VolumetricCloudsRender` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `VolumetricClouds_Create` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `VolumetricClouds_Delete` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClouds | `VolumetricClouds_GetFirstFreeSlot` | [Resumen](../reversing/Clouds21/validation-summary.json) |
| CClumpModelInfo | `AtomicSetupLightingCB` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `ConvertAnimFileIndex` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `CreateInstance-mat` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `CreateInstance-void` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `DeleteRwObject` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `FillFrameArray` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `FillFrameArrayCB` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `FindFrameFromIdCB` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `FindFrameFromNameCB` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `FindFrameFromNameWithoutIdCB` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `GetAnimFileIndex` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `GetBoundingBox` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `GetFrameFromId` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `GetFrameFromName` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `GetModelType` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `GetRwModelType` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `Init` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `SetAnimFile` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `SetAtomicRendererCB` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `SetClump` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `SetClumpModelInfoFlags` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `SetFrameIds` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `SetHierarchyForSkinAtomic` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CClumpModelInfo | `Shutdown` | [Resumen](../reversing/Clump24/validation-summary.json) |
| CColAccel | `addCacheCol` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `addColDef` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `addIPLEntity` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `cacheIPLSection` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `cacheLoadCol` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `endCache` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `getColDef` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `getIplDef` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `isCacheLoading` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `setIplDef` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColAccel | `startCache` | [Resumen](../reversing/Audio11/validation-summary.json) |
| CColLine | `Set` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CColModel | `AllocateData-params` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `AllocateData-void` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `CalculateTrianglePlanes` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `MakeMultipleAlloc` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `RemoveCollisionVolumes` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `RemoveTrianglePlanes` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `operator delete` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `operator new` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModel | `operator=` | [Resumen](../reversing/Models9/validation-summary.json) |
| CColModelPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CColSphere | `IntersectEdge` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `IntersectPoint` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `IntersectRay` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `IntersectSphere` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `Set` | [Resumen](../reversing/World43/validation-summary.json) |
| CColStore | `AddColSlot` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `AddCollisionNeededAtPosn` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `AddRef` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `BoundingBoxesPostProcess` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `EnsureCollisionIsInMemory` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `GetBoundingBox` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `HasCollisionLoaded` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `IncludeModelIndex` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `Initialise` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `LoadAllBoundingBoxes` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `LoadAllCollision` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `LoadCol-` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `LoadCol-2` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `LoadCollision` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `RemoveAllCollision` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `RemoveCol` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `RemoveColSlot` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `RemoveRef` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `RequestCollision` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `SetCollisionRequired` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `SetIfCollisionIsRequired` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `SetIfCollisionIsRequiredReducedBB` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColStore | `Shutdown` | [Resumen](../reversing/ColStore23/validation-summary.json) |
| CColTrianglePlane | `GetNormal-Out` | [Resumen](../reversing/Core70/validation-summary.json) |
| CColTrianglePlane | `Set` | [Resumen](../reversing/Core70/validation-summary.json) |
| CCollision | `BuildCacheOfCameraCollision` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `CalculateColPointInsideBox` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `CalculateTrianglePlanes-colData` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `CameraConeCastVsWorldCollision` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `CheckCameraCollisionBuildings` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `CheckCameraCollisionObjects` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `CheckCameraCollisionPeds` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `CheckCameraCollisionVehicles` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `CheckPeds` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `Closest3` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `ClosestPointOnLine` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `ClosestPointOnPoly` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `ClosestPointsOnPoly` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `ClosestSquaredDistanceBetweenFiniteLines` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `DistAlongLine2D` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `DistToLine` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `DistToLineSqr` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `DistToMathematicalLine` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `DistToMathematicalLine2D` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `GetBoundingBoxFromTwoSpheres` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `GetPrincipleAxis` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `Init` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `IsStoredPolyStillValidVerticalLine` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `IsThisVehicleSittingOnMe` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `PointInPoly` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `PointInTriangle` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `ProcessColModels` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessDiscCollision` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessLineBox` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessLineOfSight` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `ProcessLineSphere` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessLineTriangle` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessSphereBox` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessSphereSphere` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessSphereTriangle` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessVerticalLine` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ProcessVerticalLineTriangle` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `RayPolyPOP` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `RemoveTrianglePlanes-colData` | [Resumen](../reversing/ColProc59b/validation-summary.json) |
| CCollision | `ResetMadeInvisibleObjects` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `Shutdown` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `SortOutCollisionAfterLoad` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `SphereCastVersusVsPoly` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `SphereCastVsBBox` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `SphereCastVsCaches` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `SphereCastVsEntity` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `SphereCastVsSphere` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `Test2DLineAgainst2DLine` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestLineBox` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestLineBox_DW` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestLineOfSight` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollision | `TestLineSphere` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestLineTriangle` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestSphereBox` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestSphereSphere` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestSphereTriangle` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `TestVerticalLineBox` | [Resumen](../reversing/ColGeom59a/validation-summary.json) |
| CCollision | `Update` | [Resumen](../reversing/ColWorld59c/validation-summary.json) |
| CCollisionData | `CalculateTrianglePlanes` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionData | `Copy` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionData | `GetLinkPtr` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionData | `GetShadTrianglePoint` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionData | `GetTrianglePoint` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionData | `RemoveCollisionVolumes` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionData | `RemoveTrianglePlanes` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionData | `SetLinkPtr` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CCollisionPlugin | `PluginAttach` | [Resumen](../reversing/Core70/validation-summary.json) |
| CCollisionPlugin | `SetModelInfo` | [Resumen](../reversing/Core70/validation-summary.json) |
| CColourSet | `Constructor` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CColourSet | `Interpolate` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCompressedMatrixNotAligned | `CompressFromFullMatrix` | [Resumen](../reversing/Core70/validation-summary.json) |
| CCompressedMatrixNotAligned | `DecompressIntoFullMatrix` | [Resumen](../reversing/Core70/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonDown_AllStates` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonDown_DebugStuff` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonDown_Driving` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonDown_FirstPersonOnly` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonDown_ThirdPersonOnly` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonUp` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonUp_All_Player_States` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectControllerStateOn_ButtonUp_DebugStuff` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectPadFromKeyBoard` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `AffectPadFromMouse` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `ClearSettingsAssociatedWithAction` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `ClearSimButtonPressCheckers` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `Constructor` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `DeleteMatching1rst3rdPersonControls` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `DeleteMatching1rstPersonControls` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `DeleteMatching3rdPersonControls` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `DeleteMatchingActionInitiators` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `DeleteMatchingCommonControls` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `DeleteMatchingVehicleControls` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `DeleteMatchingVehicle_3rdPersonControls` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetActionType` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetButtonComboText` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetControllerKeyAssociatedWithAction` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetControllerSettingTextJoystick` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetControllerSettingTextKeyBoard` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetControllerSettingTextMouse` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetControllerSettingTextWithOrderNumber` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetFirstKeyboardControllerText` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetGxtStringOfCommandKeys` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetIsActionAButtonCombo` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetIsKeyBlank` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetIsKeyboardKeyDown` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetIsKeyboardKeyJustDown` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetIsMouseButtonDown` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetIsMouseButtonJustUp` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetIsMouseButtonUp` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetJoyButtonJustDown` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetJoyButtonJustUp` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetMouseButtonAssociatedWithAction` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `GetNumOfSettingsForAction` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `InitDefaultControlConfigJoyPad` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `InitDefaultControlConfigMouse` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `InitDefaultControlConfiguration` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `InitialiseControllerActionNameArray` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `LoadSettings` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `MakeControllerActionsBlank` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `ReinitControls` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `ResetSettingOrder` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `SaveSettings` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `SetControllerKeyAssociatedWithAction` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `SetMouseButtonAssociatedWithAction` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `StoreMouseButtonState` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `UpdateJoyButtonState` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `UpdateJoyInConfigMenus_ButtonDown` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `UpdateJoyInConfigMenus_ButtonUp` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerConfigManager | `UpdateJoy_ButtonDown` | [Resumen](../reversing/CtrlConfig58/validation-summary.json) |
| CControllerState | `CheckForInput` | [Resumen](../reversing/Core70/validation-summary.json) |
| CControllerState | `Clear` | [Resumen](../reversing/Core70/validation-summary.json) |
| CConversationForPed | `IsPlayerInPositionForConversation` | [Resumen](../reversing/ConversationForPed/validation-summary.json) |
| CConversationForPed | `Update` | [Resumen](../reversing/ConversationForPed/validation-summary.json) |
| CConversationNode | `ClearRecursively` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CConversations | `AwkwardSay` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `Clear` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `DoneSettingUpConversation` | [Resumen](../reversing/CConversations/validation-summary.json) |
| CConversations | `IsConversationAtNode` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `IsConversationGoingOn` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `IsPlayerInPositionForConversation` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `RemoveConversationForPed` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `SetUpConversationNode` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `StartSettingUpConversation` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CConversations | `Update` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CCopPed | `AddCriminalToKill` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `ClearCriminalsToKill` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `Constructor` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `Destructor` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `ProcessControl` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `RemoveCriminalToKill` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `SetPartner` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCoronas | `DoSunAndMoon` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `Init` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `RegisterCorona-texture` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `RegisterCorona-type` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `Render` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `RenderOutGeometryBufferForReflections` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `RenderReflections` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `RenderSunReflection` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `Shutdown` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `Update` | [Resumen](../reversing/World11/validation-summary.json) |
| CCoronas | `UpdateCoronaCoors` | [Resumen](../reversing/World11/validation-summary.json) |
| CCover | `AddCoverPoint` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CCover | `CalculateHorizontalSize` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `DoLineCheckWithinObject` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `DoesCoverPointStillProvideCover` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `Find2HighestPoints` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `FindAndReserveCoverPoint` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `FindCoordinatesCoverPoint` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `FindCoverPointsForThisBuilding` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `FindDirFromVector` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `FindVectorFromDir` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `FindVectorFromFirstToMissingVertex` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `Init` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `RemoveCoverPointIfEntityLost` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `RemoveCoverPointsForThisEntity` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `ShouldThisBuildingHaveItsCoverPointsCreated` | [Resumen](../reversing/World16/validation-summary.json) |
| CCover | `Update` | [Resumen](../reversing/World16/validation-summary.json) |
| CCoverPoint | `CanAccommodateAnotherPed` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CCoverPoint | `ReleaseCoverPointForPed` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CCoverPoint | `ReserveCoverPointForPed` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CCranes | `InitCranes` | [Resumen](../reversing/World43/validation-summary.json) |
| CCranes | `IsThisCarBeingCarriedByAnyCrane` | [Resumen](../reversing/World43/validation-summary.json) |
| CCranes | `IsThisCarBeingTargettedByAnyCrane` | [Resumen](../reversing/World43/validation-summary.json) |
| CCranes | `IsThisCarPickedUp` | [Resumen](../reversing/World43/validation-summary.json) |
| CCranes | `UpdateCranes` | [Resumen](../reversing/World43/validation-summary.json) |
| CCredits | `PrintCreditSpace` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCredits | `PrintCreditText` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCredits | `Render` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCredits | `RenderCredits` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCredits | `Start` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCredits | `Stop` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCreepingFire | `SetReadyToBurn` | [Resumen](../reversing/World52/validation-summary.json) |
| CCreepingFire | `TryToStartFireAtCoors` | [Resumen](../reversing/World52/validation-summary.json) |
| CCreepingFire | `Update` | [Resumen](../reversing/World52/validation-summary.json) |
| CCrime | `FindImmediateDetectionRange` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCrime | `ReportCrime` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCullZones | `AddCullZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `AddMirrorAttributeZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `AddTunnelAttributeZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `Cam1stPersonForPlayer` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `CamCloseInForPlayer` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `CamNoRain` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `CamStairsForPlayer` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `DoExtraAirResistanceForPlayer` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `DoINeedToLoadCollision` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `FewerCars` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `FewerPeds` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `FindAttributesForCoors` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `FindMirrorAttributesForCoors` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `FindTunnelAttributesForCoors` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `FindZoneWithStairsAttributeForPlayer` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `InRoomForAudio` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `Init` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `NoPolice` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `PlayerNoRain` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `PoliceAbandonCars` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCullZones | `Update` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CCurrentVehicle | `Display` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCurrentVehicle | `Process` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCurves | `CalcCorrectedDist` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `CalcCurvePoint` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `CalcSpeedScaleFactor` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `CalcSpeedVariationInBend` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `DistForLineToCrossOtherLine` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `TestCurves` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCustomBuildingDNPipeline | `CreateCustomObjPipe` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `CreatePipe` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `CustomPipeAtomicSetup` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `DestroyPipe` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `ExtraVertColourPluginAttach` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `NVCPipelineProcess` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `PreRenderUpdate-Atomic` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `PreRenderUpdate-Clump` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `SetFxEnvTexture` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `pluginExtraVertColourConstructorCB` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `pluginExtraVertColourDestructorCB` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `pluginExtraVertColourStreamGetSizeCB` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `pluginExtraVertColourStreamReadCB` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingDNPipeline | `pluginExtraVertColourStreamWriteCB` | [Resumen](../reversing/Render14/validation-summary.json) |
| CCustomBuildingPipeline | `CreateCustomObjPipe` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CreatePipe` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeAtomicSetup` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeMaterialSetup` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeReinstanceCB` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeRenderCB` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `DestroyPipe` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomCarEnvMapPipeline | `AllocEnvMapPipeAtomicData` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `CreateCustomObjPipe` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `CreatePipe` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `CustomPipeAtomicSetup` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `CustomPipeInstanceCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `CustomPipeMaterialSetup` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `CustomPipeRenderCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `DestroyPipe` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `GetFxEnvScaleX` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `GetFxEnvScaleY` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `GetFxEnvShininess` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `GetFxEnvTransSclX` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `GetFxEnvTransSclY` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `GetFxSpecSpecularity` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `GetFxSpecTexture` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `PreRenderUpdate` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `RegisterPlugin` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `SetCustomEnvMapPipeMaterialDataDefaults` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `SetFxEnvScale` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `SetFxEnvShininess` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `SetFxEnvTexture` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `SetFxEnvTransScl` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvAtmConstructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvAtmCopyConstructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvAtmDestructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvMatConstructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvMatCopyConstructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvMatDestructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvMatStreamGetSizeCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvMatStreamReadCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginEnvMatStreamWriteCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginSpecMatConstructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginSpecMatCopyConstructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginSpecMatDestructorCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginSpecMatStreamGetSizeCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginSpecMatStreamReadCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarEnvMapPipeline | `pluginSpecMatStreamWriteCB` | [Resumen](../reversing/EnvMap37/validation-summary.json) |
| CCustomCarPlateMgr | `AtomicSetCarplateTextureCB` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `AtomicUpgradeSetCarplateTextureCB` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `CreatePlateTexture` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `GeneratePlateText` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `GetMapRegionPlateDesign` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `Initialise` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `MaterialSetCarplateTextureCB` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `MaterialUpgradeSetCarplateTextureCB` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `RenderLicenseplateTextToRaster` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `SetupClump` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `SetupClumpAfterVehicleUpgrade` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `SetupMaterialPlateTexture` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `SetupMaterialPlatebackTexture` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomCarPlateMgr | `Shutdown` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CCustomRoadsignMgr | `CreateRoadsignAtomic` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `CreateRoadsignAtomicA` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `CreateRoadsignTexture` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `Initialise` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `RenderRoadsignAtomic` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `RoadsignGenerateTextRaster` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `SetAtomicAlpha` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `SetupRoadsignAtomic` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCustomRoadsignMgr | `Shutdown` | [Resumen](../reversing/Models9/validation-summary.json) |
| CCutsceneMgr | `AddCutsceneHead` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `AppendToNextCutscene` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `AttachObjectToBone` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `AttachObjectToFrame` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `AttachObjectToParent` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `BuildCutscenePlayer` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `CalculateBoundingSphereRadiusCB` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `CreateCutsceneObject` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `DeleteCutsceneData` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `DeleteCutsceneData_overlay` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `FinishCutscene` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `GetCutsceneTimeInMilleseconds` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `HasCutsceneFinished` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `HideRequestedObjects` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `Initialise` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `IsCutsceneSkipButtonBeingPressed` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `LoadAnimationUncompressed` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `LoadCutsceneData` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `LoadCutsceneData_loading` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `LoadCutsceneData_overlay` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `LoadCutsceneData_postload` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `LoadCutsceneData_preload` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `LoadEverythingBecauseCutsceneDeletedAllOfIt` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `RemoveCutscenePlayer` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `RemoveEverythingBecauseCutsceneDoesntFitInMemory` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `SetCutsceneAnim` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `SetCutsceneAnimToLoop` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `SetHeadAnim` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `SetupCutsceneToStart` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `Shutdown` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `SkipCutscene` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `StartCutscene` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `Update` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `UpdateCutsceneObjectBoundingBox` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneMgr | `Update_overlay` | [Resumen](../reversing/Cutscene35/validation-summary.json) |
| CCutsceneObject | `PreRender` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CCutsceneObject | `ProcessControl` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CCutsceneObject | `RemoveLighting` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CCutsceneObject | `SetModelIndex` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CCutsceneObject | `SetupCarPipeAtomicsForClump` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CCutsceneObject | `SetupLighting` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CDamageAtomicModelInfo | `AsDamageAtomicModelInfoPtr` | [Resumen](../reversing/Data23/validation-summary.json) |
| CDamageAtomicModelInfo | `DeleteRwObject` | [Resumen](../reversing/Data23/validation-summary.json) |
| CDamageAtomicModelInfo | `Init` | [Resumen](../reversing/Data23/validation-summary.json) |
| CDamageAtomicModelInfo | `SetDamagedAtomic` | [Resumen](../reversing/Data23/validation-summary.json) |
| CDamageManager | `ApplyDamage` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `FuckCarCompletely` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetAeroplaneCompStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetCarNodeIndexFromDoor` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetCarNodeIndexFromPanel` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetComponentGroup` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetDoorStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetDoorStatus_Component` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetEngineStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetLightStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetPanelStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `GetWheelStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `Init` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `ProgressAeroplaneDamage` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `ProgressDoorDamage` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `ProgressEngineDamage` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `ProgressPanelDamage` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `ProgressWheelDamage` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `ResetDamageStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `ResetDamageStatusAndWheelDamage` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `SetAeroplaneCompStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `SetDoorStatus-` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `SetDoorStatus_Component` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `SetEngineStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `SetLightStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `SetPanelStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDamageManager | `SetWheelStatus` | [Resumen](../reversing/Damage27/validation-summary.json) |
| CDarkel | `CalcFade` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `CheckDamagedWeaponType` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `DealWithWeaponChangeAtEndOfFrenzy` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `DrawMessages` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `FailKillFrenzy` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `FindTotalPedsKilledByPlayer` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `FrenzyOnGoing` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `Init` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `QueryModelsKilledByPlayer` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `ReadStatus` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `RegisterCarBlownUpByPlayer` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `RegisterKillByPlayer` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `RegisterKillNotByPlayer` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `ResetModelsKilledByPlayer` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `ResetOnPlayerDeath` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `StartFrenzy` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `ThisPedShouldBeKilledForFrenzy` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `ThisVehicleShouldBeKilledForFrenzy` | [Resumen](../reversing/Game19/validation-summary.json) |
| CDarkel | `Update` | [Resumen](../reversing/Game19/validation-summary.json) |
| CData | `Load` | [Resumen](../reversing/Core70/validation-summary.json) |
| CData | `Unload` | [Resumen](../reversing/Core70/validation-summary.json) |
| CDebug | `DebugDisplayTextBuffer` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CDecisionMakerTypesFileLoader | `GetGrpDMName` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CDecisionMakerTypesFileLoader | `GetPedDMName` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CDecisionMakerTypesFileLoader | `LoadDecisionMaker-enum` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CDecisionMakerTypesFileLoader | `LoadDecisionMaker-ptr` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CDecisionMakerTypesFileLoader | `LoadDefaultDecisionMaker` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CDecisionMakerTypesFileLoader | `ReStart` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CDecisionMakerTypesFileLoader | `UnloadDecisionMaker` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CDirectory | `AddItem-` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `Constructor-Capacity` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `Constructor-Empty` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `Destructor` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `FindItem-` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `FindItem-ByHash` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `FindItem-ByName` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `Init` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `ReadDirFile` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDirectory | `WriteDirFile` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CDoor | `GetAngleOpenRatio` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CDoor | `IsClosed` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CDoor | `IsFullyOpen` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CDoor | `Open` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CDoor | `Process` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CDoor | `ProcessImpact` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CDraw | `CalculateAspectRatio` | [Resumen](../reversing/Core70/validation-summary.json) |
| CDraw | `SetFOV` | [Resumen](../reversing/Core70/validation-summary.json) |
| CDummy | `IsDummyPointerValid` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CDummyObject | `CreateObject` | [Resumen](../reversing/Core70/validation-summary.json) |
| CDummyObject | `UpdateFromObject` | [Resumen](../reversing/Core70/validation-summary.json) |
| CDummyPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CEntity | `Add-rect` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `AttachToRwObject` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `BuildWindSockMatrix` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `CalculateBBProjection` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `CleanUpOldReference-` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `Constructor` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `CreateEffects` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `CreateRwObject` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `DeleteRwObject` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `DestroyEffects` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `Destructor` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `DetachFromRwObject` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `DoesNotCollideWithFlyers` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `FindTriggerPointCoors` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `FlagToDestroyWhenNextProcessed` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `GetBoundCentre-ref` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetBoundCentre-vec` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetBoundRect` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetColModel` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetDistanceFromCentreOfMassToBaseOfModel` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetIsBoundingBoxOnScreen` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetIsOnScreen` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetIsStatic` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `GetIsTouching-ent` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `GetIsTouching-vec` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `GetIsTypePhysical` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `GetModellingMatrix` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `GetRandom2dEffect` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `HasPreRenderEffects` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `IsEntityOccluded` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `IsEntityPointerValid` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `IsVisible` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `LivesInThisNonOverlapSector` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `MaterialUpdateUVAnimCB` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `ModifyMatrixForBannerInWind` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `ModifyMatrixForCrane` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `ModifyMatrixForTreeInWind` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `PreRender` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `PreRenderForGlassWindow` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `ProcessCollision` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `ProcessControl` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `ProcessLightsForEntity` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `ProcessShift` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `PruneReferences` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `RegisterReference-` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `Remove` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `RemoveEscalatorsForEntity` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `RemoveLighting` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `Render` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `RenderEffects` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `ResolveReferences` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `SetAtomicAlpha` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `SetCompAlphaCB` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `SetIsStatic` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `SetModelIndex` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `SetModelIndexNoCreate` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `SetRwObjectAlpha` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `SetupBigBuilding` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `SetupLighting` | [Resumen](../reversing/CEntC/validation-summary.json) |
| CEntity | `SpecialEntityCalcCollisionSteps` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `SpecialEntityPreCollisionStuff` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `Teleport` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `TestCollision` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `TransformFromObjectSpace-ptr` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `TransformFromObjectSpace-ref` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `TreatAsPlayerForCollisions` | [Resumen](../reversing/CEntB/validation-summary.json) |
| CEntity | `UpdateAnim` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `UpdateRpHAnim` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `UpdateRwFrame` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntity | `UpdateRwMatrix` | [Resumen](../reversing/CEntA/validation-summary.json) |
| CEntityScanner | `Clear` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntityScanner | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntityScanner | `Destructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntityScanner | `ScanForEntitiesInRange` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntitySeekPosCalculatorFixedPos | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CEntitySeekPosCalculatorRadiusAngleOffset | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CEntitySeekPosCalculatorStandard | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CEntitySeekPosCalculatorXYOffset | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CEntryExit | `FindValidTeleportPoint` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `GenerateAmbientPeds` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `GetEntryExitToDisplayNameOf` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `GetPositionRelativeToOutsideWorld` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `IsInArea` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `ProcessStealableObjects` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `RequestAmbientPeds` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `RequestObjectsInFrustum` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `TransitionFinished` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `TransitionStarted` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExit | `WarpGangWithPlayer` | [Resumen](../reversing/World11/validation-summary.json) |
| CEntryExitManager | `AddEntryExitToStack` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `AddOne` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `DeleteOne` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `EnableBurglaryHouses` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `FindNearestDoor` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `FindNearestEntryExit` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `GetEntryExitIndex` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `GetPositionRelativeToOutsideWorld` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `Init` | [Resumen](../reversing/EnExInit/validation-summary.json) |
| CEntryExitManager | `LinkEntryExit` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `Load` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `PostEntryExitsCreation` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `ResetAreaCodeForVisibleObjects` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `Save` | [Resumen](../reversing/EnExInit/validation-summary.json) |
| CEntryExitManager | `SetAreaCodeForVisibleObjects` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `Shutdown` | [Resumen](../reversing/EnExInit/validation-summary.json) |
| CEntryExitManager | `ShutdownForRestart` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryExitManager | `Update` | [Resumen](../reversing/World18/validation-summary.json) |
| CEntryInfoList | `Flush` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CEntryInfoNode | `operator delete` | [Resumen](../reversing/Core70/validation-summary.json) |
| CEntryInfoNode | `operator new` | [Resumen](../reversing/Core70/validation-summary.json) |
| CEntryInfoNodePool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CEscalator | `AddThisOne` | [Resumen](../reversing/World43/validation-summary.json) |
| CEscalator | `Constructor` | [Resumen](../reversing/World43/validation-summary.json) |
| CEscalator | `Destructor` | [Resumen](../reversing/World43/validation-summary.json) |
| CEscalator | `SwitchOff` | [Resumen](../reversing/World43/validation-summary.json) |
| CEscalator | `Update` | [Resumen](../reversing/World43/validation-summary.json) |
| CEscalators | `AddOne` | [Resumen](../reversing/Render23/validation-summary.json) |
| CEscalators | `Init` | [Resumen](../reversing/Render23/validation-summary.json) |
| CEscalators | `Shutdown` | [Resumen](../reversing/Render23/validation-summary.json) |
| CEscalators | `Update` | [Resumen](../reversing/Render23/validation-summary.json) |
| CEvent | `CalcSoundLevelIncrement` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEvent | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEvent | `GetSoundLevel` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventAcquaintancePed | `AffectsPed` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventAcquaintancePed | `AffectsPedGroup` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventAcquaintancePed | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventAcquaintancePed | `TakesPriorityOver` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventAcquaintancePedHate | `Constructor2` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventAcquaintancePedHateBadlyLit | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventAcquaintancePedHateBadlyLit | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventAreaCodes | `AffectsPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventAreaCodes | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventAreaCodes | `TakesPriorityOver` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventAttractor | `AffectsPed` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventAttractor | `CloneEditable` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventAttractor | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventAttractor | `IsEffectActive` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventBuildingCollision | `AffectsPed` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventBuildingCollision | `CanTreatBuildingAsObject` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventBuildingCollision | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventBuildingCollision | `IsHeadOnCollision` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventCarUpsideDown | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventCarUpsideDown | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventCopCarBeingStolen | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventCopCarBeingStolen | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventDamage | `AffectsPed` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `AffectsPedGroup` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `CanBeInterruptedBySameEvent` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `CloneEditable` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `ComputeBodyPartToRemove` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `ComputeDamageAnim` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `ComputeDeathAnim` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `Constructor-` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `Constructor-1` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `DoInformVehicleOccupants` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `From` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `GetEventPriority` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `GetEventType` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `GetLifeTime` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `GetLocalSoundLevel` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `GetSourceEntity` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `IsCriminalEvent` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `ProcessDamage` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `ReportCriminalEvent` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDamage | `TakesPriorityOver` | [Resumen](../reversing/Event20/validation-summary.json) |
| CEventDanger | `AffectsPed` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDanger | `AffectsPedGroup` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDanger | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDanger | `GetSourceEntity` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDontJoinPlayerGroup | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventEditableResponse | `Clone` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `ComputeResponseTaskOfType` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `ComputeResponseTaskType-group` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `ComputeResponseTaskType-ped` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `Constructor` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `HasEditableResponse` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `InformGroup` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `InformRespectedFriends` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `InformVehicleOccupants` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `TriggerLookAt` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEditableResponse | `WillRespond` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CEventEscalator | `AffectsPed` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventGlobalGroup | `AddEventsToGroup` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventGlobalGroup | `AddEventsToPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventGlobalGroup | `GetSoundLevel` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventGroup | `Add-` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `Constructor` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `Flush` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `GetEventOfType` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `GetHighestPriorityEvent` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `HasEvent` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `HasEventOfType` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `HasScriptCommandOfTaskType` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `Remove` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `RemoveInvalidEvents` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `Reorganise` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGroup | `TickEvents` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CEventGunShot | `AffectsPed` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `CloneEditable` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `Constructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `IsCriminalEvent` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `TakesPriorityOver` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShotWhizzedBy | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventGunShotWhizzedBy | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventHandlerHistory | `ClearAllEvents` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `ClearNonTempEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `ClearTempEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `GetCurrentEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `GetCurrentEventPriority` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `IsRespondingToEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `RecordCurrentEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `StoreActiveEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `TakesPriorityOverCurrentEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHandlerHistory | `TickStoredEvent` | [Resumen](../reversing/Sys10/validation-summary.json) |
| CEventHitByWaterCannon | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventHitByWaterCannon | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventInAir | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventInAir | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventKnockOffBike | `AffectsPed` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventKnockOffBike | `CalcForcesAndAnims` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventKnockOffBike | `Constructor-first` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventKnockOffBike | `Constructor-second` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventKnockOffBike | `From` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventKnockOffBike | `ReportCriminalEvent` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventKnockOffBike | `SetPedOutCar` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventKnockOffBike | `SetPedSafePosition` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventLeaderEnteredCarAsDriver | `AffectsPedGroup` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventLeaderEnteredCarAsDriver | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventLeaderEntryExit | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventLeaderExitedCarAsDriver | `AffectsPedGroup` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventLeaderExitedCarAsDriver | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventLeaderQuitEnteringCarAsDriver | `AffectsPedGroup` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventLeaderQuitEnteringCarAsDriver | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventLeanOnVehicle | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventLeanOnVehicle | `IsValid` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventNewGangMember | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventObjectCollision | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventObjectCollision | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventOnFire | `AffectsPed` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventPassObject | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPassObject | `IsValid` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPedCollisionWithPed | `AffectsPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventPedCollisionWithPed | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventPedCollisionWithPed | `TakesPriorityOver` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventPedCollisionWithPlayer | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventPlayerCollisionWithPed | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventPlayerCommandToGroup | `AffectsPedGroup` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPlayerCommandToGroup | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPlayerCommandToGroupAttack | `AffectsPedGroup` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPlayerCommandToGroupAttack | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPlayerCommandToGroupGather | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CEventPotentialWalkIntoFire | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPotentialWalkIntoFire | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPotentialWalkIntoObject | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPotentialWalkIntoObject | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPotentialWalkIntoPed | `AffectsPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventPotentialWalkIntoPed | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventPotentialWalkIntoPed | `TakesPriorityOver` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventPotentialWalkIntoVehicle | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventPotentialWalkIntoVehicle | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventScriptCommand | `AffectsPed` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptCommand | `Clone` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptCommand | `CloneScriptTask` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptCommand | `Constructor` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptCommand | `GetEventPriority` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptCommand | `GetEventType` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptCommand | `IsValid` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptCommand | `TakesPriorityOver` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CEventScriptedAttractor | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventSeenCop | `Constructor1` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventSeenPanickedPed | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventSeenPanickedPed | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventSoundQuiet | `AffectsPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventSoundQuiet | `CloneEditable` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventSoundQuiet | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventStuckInAir | `AffectsPed` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventStuckInAir | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventStuckInAir | `GetEventPriority` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventStuckInAir | `TakesPriorityOver` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventVehicleCollision | `AffectsPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleCollision | `Clone` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleCollision | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleDamage | `AffectsPed` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventVehicleDamage | `Constructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventVehicleDamage | `GetSourceEntity` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventVehicleDamage | `IsCriminalEvent` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventVehicleDamage | `ReportCriminalEvent` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventVehicleDamageCollision | `CloneEditable` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventVehicleDamageCollision | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventVehicleDamageWeapon | `CloneEditable` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventVehicleDamageWeapon | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventVehicleDied | `AffectsPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleDied | `Clone` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleDied | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleHitAndRun | `Clone` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleHitAndRun | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleHitAndRun | `ReportCriminalEvent` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleOnFire | `AffectsPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleOnFire | `CloneEditable` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleOnFire | `Constructor` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventVehicleToSteal | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventVehicleToSteal | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CExplosion | `AddExplosion` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `ClearAllExplosions` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `DoesExplosionMakeSound` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `GetExplosionActiveCounter` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `GetExplosionPosition` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `GetExplosionType` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `Initialise` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `RemoveAllExplosionsInArea` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `ResetExplosionActiveCounter` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `Shutdown` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `TestForExplosionInArea` | [Resumen](../reversing/World12/validation-summary.json) |
| CExplosion | `Update` | [Resumen](../reversing/World12/validation-summary.json) |
| CFallingGlassPane | `Render` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CFallingGlassPane | `Update` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CFileLoader | `AddTexDictionaries` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `FindRelatedModelInfoCB` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `FinishLoadClumpFile` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LinkLods` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadAnimatedClumpObject` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadAtomicFile-` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadAtomicFile-stream` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadAudioZone` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCarGenerator-0` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCarGenerator-1` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCarPathNode` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadClumpFile-` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadClumpFile-1` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadClumpObject` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCollisionFile-Buffer` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCollisionFile-File` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCollisionFileFirstTime` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCollisionModel` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCollisionModelVer2` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCollisionModelVer3` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCollisionModelVer4` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadCullZone` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadEntryExit` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadGarage` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadLevel` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadLine-Bufer` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadLine-File` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadObject` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadObjectInstance-file` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadObjectInstance-inst` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadObjectTypes` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadOcclusionVolume` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadPathHeader` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadPedObject` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadPedPathNode` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadPickup` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadScene` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadStuntJump` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadTXDParent` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadTexDictionary` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadTimeCyclesModifier` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadTimeObject` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadVehicleObject` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadWeaponObject` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `LoadZone` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `SetRelatedModelInfoCB` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileLoader | `StartLoadClumpFile` | [Resumen](../reversing/FileLoader47/validation-summary.json) |
| CFileMgr | `ChangeDir` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `CloseFile` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `GetErrorReadWrite` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `GetTotalSize` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `InitUserDirectories` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `Initialise` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `LoadFile` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `OpenFile` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `OpenFileForAppending` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `OpenFileForWriting` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `Read` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `ReadLine` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `Seek` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `SetDir` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `SetDirMyDocuments` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `Tell` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFileMgr | `Write` | [Resumen](../reversing/Sys17/validation-summary.json) |
| CFire | `Constructor` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `CreateFxSysForStrength` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `Extinguish` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `Initialise` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `ProcessFire` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFireManager | `ClearAllScriptFireFlags` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `Constructor` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `CreateAllFxSystems` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `DestroyAllFxSystems` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `Destructor` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `ExtinguishPoint` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `ExtinguishPointWithWater` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `FindNearestFire` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `GetNextFreeFire` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `GetNumFiresInArea` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `GetNumFiresInRange` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `GetNumOfNonScriptFires` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `GetScriptFireCoords` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `Init` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `IsScriptFireExtinguished` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `PlentyFiresAvailable` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `RemoveAllScriptFires` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `RemoveScriptFire` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `SetScriptFireAudio` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `Shutdown` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `StartFire-` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `StartFire-NoTarget` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `StartScriptFire` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CFireManager | `Update` | [Resumen](../reversing/Fire24/validation-summary.json) |
| CGame | `CameraDestroy` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `CanSeeOutSideFromCurrArea` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `CanSeeWaterFromCurrArea` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `DrasticTidyUpMemory` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `FinalShutdown` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `Init1` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `Init2` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `Init3` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `Initialise` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `InitialiseCoreDataAfterRW` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `InitialiseEssentialsAfterRW` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `InitialiseOnceBeforeRW` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `InitialiseRenderWare` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `InitialiseWhenRestarting` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `Process` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `ReInitGameObjectVariables` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `ReloadIPLs` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `ShutDownForRestart` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `Shutdown` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `ShutdownRenderWare` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `TidyUpMemory` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGame | `ValidateVersion` | [Resumen](../reversing/Game22/validation-summary.json) |
| CGameLogic | `CalcDistanceToForbiddenTrainCrossing` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `ClearSkip` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `DoWeaponStuffAtStartOf2PlayerGame` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `FindCityClosestToPoint` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `ForceDeathRestart` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `InitAtStartOfGame` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `IsCoopGameGoingOn` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `IsPlayerAllowedToGoInThisDirection` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `IsPlayerUse2PlayerControls` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `IsPointWithinLineArea` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `IsSkipWaitingForScriptToFadeIn` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `LaRiotsActiveHere` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `Load` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `PassTime` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `Remove2ndPlayerIfPresent` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `ResetStuffUponResurrection` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `RestorePedsWeapons` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `RestorePlayerStuffDuringResurrection` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `Save` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `SetPlayerWantedLevelForForbiddenTerritories` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `SetUpSkip` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `SkipCanBeActivated` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `SortOutStreamingAndMemory` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `StopPlayerMovingFromDirection` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `StorePedsWeapons` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `Update` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGameLogic | `UpdateSkip` | [Resumen](../reversing/GameLogic27/validation-summary.json) |
| CGamma | `Init` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CGamma | `SetGamma` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CGangs | `ChooseGangPedModel` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGangs | `GetWillAttackPlayerWithCops` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGangs | `Initialise` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGangs | `Load` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGangs | `Save` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGangs | `SetGangPedModelOverride` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGangs | `SetGangWeapons` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGangs | `SetWillAttackPlayerWithCops` | [Resumen](../reversing/Data8a/validation-summary.json) |
| CGarage | `BuildRotatedDoorMatrix` | [Resumen](../reversing/Data23/validation-summary.json) |
| CGarage | `CloseThisGarage` | [Resumen](../reversing/Data23/validation-summary.json) |
| CGarage | `OpenThisGarage` | [Resumen](../reversing/Data23/validation-summary.json) |
| CGarage | `StoreAndRemoveCarsForThisHideOut` | [Resumen](../reversing/Data23/validation-summary.json) |
| CGarages | `ActivateGarage` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `AllRespraysCloseOrOpen` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `ChangeGarageType` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `CloseHideOutGaragesBeforeSave` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `DeActivateGarage` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `GetGarageNumberByName` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `Init` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `Init_AfterRestart` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `IsCarSprayable` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `IsGarageClosed` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `IsGarageOpen` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `IsModelIndexADoor` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `IsPointWithinHideOutGarage` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `Load` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `PlayerArrestedOrDied` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `PrintMessages` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `Save` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `SetTargetCarForMissionGarage` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `Shutdown` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `TriggerMessage` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGarages | `Update` | [Resumen](../reversing/Garages21/validation-summary.json) |
| CGeneral | `GetATanOfXY` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `GetAngleBetweenPoints` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `GetNodeHeadingFromVector` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `GetRadianAngleBetweenPoints-` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `GetRandomNumberInRange<float>-` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `GetRandomNumberInRange_int32` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `LimitAngle` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `LimitRadianAngle` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGeneral | `SolveQuadratic` | [Resumen](../reversing/Core9/validation-summary.json) |
| CGlass | `AskForObjectToBeRenderedInGlass` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `BreakGlassPhysically` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `CalcAlphaWithNormal` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CGlass | `CarWindscreenShatters` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `FindFreePane` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `FindWindowSectorList<CPtrListSingleLink<CPhysical*>>` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `GeneratePanesForWindow` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `HasGlassBeenShatteredAtCoors` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `Init` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `IsObjectGlass` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `Render` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `RenderHiLightPolys` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `RenderReflectionPolys` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `RenderShatteredPolys` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `Update` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `WasGlassHitByBullet` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `WindowRespondsToCollision` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `WindowRespondsToExplosion` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGlass | `WindowRespondsToSoftCollision` | [Resumen](../reversing/Render19/validation-summary.json) |
| CGrassRenderer | `AddTriPlant` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `DrawTriPlants` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `FlushTriPlantBuffer` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `GenPointInTriangle` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `GetPlantModelsTab` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `Initialise` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `SetCloseFarAlphaDist` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `SetCurrentScanCode` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `SetGlobalCameraPos` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `SetGlobalWindBending` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `SetPlantModelsTab` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGrassRenderer | `Shutdown` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CGridRef | `GetAreaName` | [Resumen](../reversing/World43/validation-summary.json) |
| CGridRef | `GetArtistBugstarID` | [Resumen](../reversing/World43/validation-summary.json) |
| CGridRef | `GetGridRefPositions-` | [Resumen](../reversing/World43/validation-summary.json) |
| CGridRef | `GetGridRefPositions-vec` | [Resumen](../reversing/World43/validation-summary.json) |
| CGridRef | `Init` | [Resumen](../reversing/World43/validation-summary.json) |
| CHandObject | `PreRender` | [Resumen](../reversing/World52/validation-summary.json) |
| CHandObject | `ProcessControl` | [Resumen](../reversing/World52/validation-summary.json) |
| CHandObject | `Render` | [Resumen](../reversing/World52/validation-summary.json) |
| CHandShaker | `Process` | [Resumen](../reversing/World52/validation-summary.json) |
| CHandShaker | `Reset` | [Resumen](../reversing/World52/validation-summary.json) |
| CHandShaker | `SetDefaults` | [Resumen](../reversing/World52/validation-summary.json) |
| CHeli | `AddHeliSearchLight` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `BurstTyre` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `Fix` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `InitHelis` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `Post_SearchLightCone` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `Pre_SearchLightCone` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `Render` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `RenderAllHeliSearchLights` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `SetUpWheelColModel` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `SwitchPoliceHelis` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHeli | `TestSniperCollision` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CHudColours | `Constructor` | [Resumen](../reversing/World43/validation-summary.json) |
| CHudColours | `GetIntColour` | [Resumen](../reversing/World43/validation-summary.json) |
| CHudColours | `GetRGB` | [Resumen](../reversing/World43/validation-summary.json) |
| CHudColours | `GetRGBA` | [Resumen](../reversing/World43/validation-summary.json) |
| CHudColours | `SetRGBAValue` | [Resumen](../reversing/World43/validation-summary.json) |
| CInformFriendsEventQueue | `Add` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformFriendsEventQueue | `Flush` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformFriendsEventQueue | `Init` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformFriendsEventQueue | `Process` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformGroupEvent | `Constructor` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformGroupEvent | `Destructor` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformGroupEvent | `Process` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformGroupEventQueue | `Add` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformGroupEventQueue | `Flush` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformGroupEventQueue | `Init` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CInformGroupEventQueue | `Process` | [Resumen](../reversing/InformQueues/validation-summary.json) |
| CIniFile | `LoadIniFile` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| CIplDefPool | `New` | [Resumen](../reversing/AppBatch/validation-summary.json) |
| CIplStore | `AddIplSlot` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `AddIplsNeededAtPosn` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `ClearIplsNeededAtPosn` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `EnableDynamicStreaming` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `EnsureIplsAreInMemory` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `FindIplSlot` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `GetBoundingBox` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `GetIplEntityIndexArray` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `GetIplName` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `GetNewIplEntityIndexArray` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `HaveIplsLoaded` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `IncludeEntity` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `Initialise` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CIplStore | `Load` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `LoadAllRemainingIpls` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `LoadIpl` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CIplStore | `LoadIplBoundingBox` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CIplStore | `LoadIpls` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CIplStore | `RemoveAllIpls` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `RemoveIpl` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CIplStore | `RemoveIplAndIgnore` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `RemoveIplSlot` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CIplStore | `RemoveIplWhenFarAway` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `RemoveRelatedIpls` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `RequestIplAndIgnore` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `RequestIpls` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `Save` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `SetIfInteriorIplIsRequired` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `SetIfIplIsRequired` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `SetIfIplIsRequiredReducedBB` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `SetIplsRequired` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `SetIsInterior` | [Resumen](../reversing/IplStore/validation-summary.json) |
| CIplStore | `SetupRelatedIpls` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CIplStore | `Shutdown` | [Resumen](../reversing/IplStore2/validation-summary.json) |
| CKeyArray | `BinarySearch` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyArray | `Load` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyArray | `Search` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyArray | `Unload` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyGen | `AppendStringToKey` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyGen | `GetKey-null-terminated` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyGen | `GetKey-sized` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyGen | `GetUppercaseKey-` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CKeyboardState | `Clear` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLinkedUpgradeList | `AddUpgradeLink` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLinkedUpgradeList | `FindOtherUpgrade` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadMonitor | `BeginFrame` | [Resumen](../reversing/LoadMonitor/validation-summary.json) |
| CLoadMonitor | `Constructor` | [Resumen](../reversing/LoadMonitor/validation-summary.json) |
| CLoadMonitor | `Destructor` | [Resumen](../reversing/LoadMonitor/validation-summary.json) |
| CLoadMonitor | `EndFrame` | [Resumen](../reversing/LoadMonitor/validation-summary.json) |
| CLoadMonitor | `EndTimer` | [Resumen](../reversing/LoadMonitor/validation-summary.json) |
| CLoadMonitor | `Render` | [Resumen](../reversing/LoadMonitor/validation-summary.json) |
| CLoadMonitor | `StartTimer` | [Resumen](../reversing/LoadMonitor/validation-summary.json) |
| CLoadedCarGroup | `AddMember` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadedCarGroup | `Clear` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadedCarGroup | `CountMembers` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadedCarGroup | `GetMember` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadedCarGroup | `PickLeastUsedModel` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadedCarGroup | `PickRandomCar` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CLoadedCarGroup | `RemoveMember` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadedCarGroup | `SortBasedOnUsage` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLoadingScreen | `Continue` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `DisplayMessage` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `DisplayNextSplash` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `DisplayPCScreen` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `DoPCScreenChange` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `DoPCTitleFadeIn` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `DoPCTitleFadeOut` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `GetClockTime` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `Init` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `IsActive` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `LoadSplashes` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `LoadingScreen` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `NewChunkLoaded` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `Pause` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `RenderLoadingBar` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `RenderSplash` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `SetLoadingBarMsg` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `Shutdown` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `StartFading` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLoadingScreen | `Update` | [Resumen](../reversing/Loading/validation-summary.json) |
| CLocalisation | `Blood` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `FrenchGame` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `GermanGame` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `Initialise` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `KickingWhenDown` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `KillFrenzy` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `KillPeds` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `KnockDownPeds` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `Metric` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `PedsOnFire` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `Porn` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `Prostitutes` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `ScreamsFromKills` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `SetFrenchGame` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `SetGermanGame` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `SetNormalGame` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `ShootLimbs` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLocalisation | `StealFromDeadPed` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLodAtomicModelInfo | `AsLodAtomicModelInfoPtr` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLodAtomicModelInfo | `GetModelType` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLodAtomicModelInfo | `Init` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
| CLodTimeModelInfo | `GetModelType` | [Resumen](../reversing/Small2/validation-summary.json) |
| CLodTimeModelInfo | `GetTimeInfo` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMaths | `GetCosFast` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMaths | `GetSinFast` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMaths | `InitMathsTables` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLink | `Insert` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLink | `Remove` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `AddToList1` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `AddToList2` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `GetNumFree` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `GetNumUsed1` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `GetNumUsed2` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `Init` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `MoveToFreeList` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `MoveToList1` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `MoveToList2` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMatrixLinkList | `Shutdown` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMenuManager | `AdditionalOptionInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CentreMousePointer` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckCodesForControls` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckForMenuClosing` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckFrontEndDownInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckFrontEndLeftInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckFrontEndRightInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckFrontEndUpInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckHover` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckMissionPackValidMenu` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `CheckRedefineControlInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `CheckSliderMovement` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `Constructor` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `Destructor` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `DisplayHelperText` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DisplaySlider` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DoSettingsBeforeStartingAGame` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `DrawBackground` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawControllerBound` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawControllerScreenExtraText` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawControllerSetupScreen` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawFrontEnd` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawQuitGameScreen` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawStandardMenus` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawWindow` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `DrawWindowedText` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `GetNumberOfMenuOptions` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `HasLanguageChanged` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `Initialise` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `InitialiseChangedLanguageSettings` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `JumpToGenericMessageScreen` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `LoadAllTextures` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `LoadSettings` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `MessageScreen` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `PrintBriefs` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `PrintMap` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `PrintRadioStationList` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `PrintStats` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `Process` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `ProcessFileActions` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `ProcessMenuOptions` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `ProcessMissionPackNewGame` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `ProcessPCMenuOptions` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `ProcessStreaming` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `ProcessUserInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `RedefineScreenUserInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `ResetHelperText` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `SaveLoadFileError_SetUpErrorScreen` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `SaveSettings` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `SaveStatsToFile` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `ScrollRadioStations` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuManager | `SetDefaultPreferences` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `SetFrontEndRenderStates` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `SetHelperText` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `SmallMessageScreen` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `StretchX` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `StretchY` | [Resumen](../reversing/MenuMgrA/validation-summary.json) |
| CMenuManager | `SwapTexturesRound` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `SwitchToNewScreen` | [Resumen](../reversing/MenuMgrC/validation-summary.json) |
| CMenuManager | `UnloadTextures` | [Resumen](../reversing/MenuMgrD/validation-summary.json) |
| CMenuManager | `UserInput` | [Resumen](../reversing/MenuMgrB/validation-summary.json) |
| CMenuSystem | `ActivateItems` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `ActivateOneItem` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `CheckForAccept` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `CheckForSelected` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `CreateNewMenu` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `DisplayGridMenu` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `DisplayStandardMenu` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `FillGridWithCarColours` | [Resumen](../reversing/CMenuSystem/validation-summary.json) |
| CMenuSystem | `GetCarColourFromGrid` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `HighlightOneItem` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `Initialise` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `Input` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `InputGridMenu` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `InputStandardMenu` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `InsertMenu` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `InsertOneMenuItemWithNumber` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `Process` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `SetActiveMenuItem` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `SetColumnOrientation` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `SetColumnWidth` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMenuSystem | `SwitchOffMenu` | [Resumen](../reversing/MenuSystem2/validation-summary.json) |
| CMessages | `AddBigMessage` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddBigMessageQ` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddBigMessageWithNumber` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddBigMessageWithNumberQ` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddMessageJump` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddMessageJumpQWithNumber` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddMessageJumpQWithString` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddMessageQ` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddMessageWithNumberQ` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddMessageWithStringQ` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `AddToPreviousBriefArray` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `ClearAllMessagesDisplayedByGame` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `ClearMessages` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `ClearPreviousBriefArray` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `ClearSmallMessagesOnly` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `ClearThisBigPrint` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `ClearThisPrint` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `ClearThisPrintBigNow` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `CutString` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `Display` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `GetStringLength` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `Init` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `InsertNumberInString` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `InsertPlayerControlKeysInString` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `InsertStringInString` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `Process` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `StringCompare` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMessages | `StringCopy` | [Resumen](../reversing/Messages/validation-summary.json) |
| CMirrors | `BeforeConstructRenderList` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMirrors | `BeforeMainRender` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMirrors | `BuildCamMatrix` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMirrors | `BuildCameraMatrixForScreens` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMirrors | `CreateBuffer` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMirrors | `Init` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMirrors | `RenderMirrorBuffer` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMirrors | `ShutDown` | [Resumen](../reversing/Mirrors/validation-summary.json) |
| CMissionTextOffsets | `Load` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddAtomicModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddClumpModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddDamageAtomicModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddLodAtomicModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddLodTimeModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddPedModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddTimeModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddVehicleModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `AddWeaponModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `GetModelInfo-full` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `GetModelInfo-minmax` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `GetModelInfoFromHashKey` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `GetModelInfoUInt16` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `Initialise` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsBikeModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsBmxModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsBoatModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsCarModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsFakePlaneModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsHeliModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsMonsterTruckModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsPlaneModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsQuadBikeModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsTrailerModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsTrainModel` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `IsVehicleModelType` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `ReInit2dEffects` | [Resumen](../reversing/Small2/validation-summary.json) |
| CModelInfo | `ShutDown` | [Resumen](../reversing/Small2/validation-summary.json) |
| CMotionBlurStreaks | `Init` | [Resumen](../reversing/Render23/validation-summary.json) |
| CMotionBlurStreaks | `RegisterStreak` | [Resumen](../reversing/Render23/validation-summary.json) |
| CMotionBlurStreaks | `Render` | [Resumen](../reversing/Render23/validation-summary.json) |
| CMotionBlurStreaks | `Update` | [Resumen](../reversing/Render23/validation-summary.json) |
| CMouseControllerState | `CheckForInput` | [Resumen](../reversing/World52/validation-summary.json) |
| CMouseControllerState | `Clear` | [Resumen](../reversing/World52/validation-summary.json) |
| CMouseControllerState | `Constructor` | [Resumen](../reversing/World52/validation-summary.json) |
| CMovingThings | `Init` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CMovingThings | `Render` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CMovingThings | `Render_BeforeClouds` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CMovingThings | `Shutdown` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CMovingThings | `Update` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CNodeRoute | `operator delete` | [Resumen](../reversing/Core70/validation-summary.json) |
| CNodeRoute | `operator new` | [Resumen](../reversing/Core70/validation-summary.json) |
| CNodeRoutePool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CObject | `AddToControlCodeList` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `CanBeDeleted` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `CanBeTargetted` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `CanBeUsedToTakeCoverBehind` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Create-dummy` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Create-intbool` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `CreateRwObject` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `DeleteAllMissionObjects` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `DeleteAllTempObjects` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `DeleteAllTempObjectsInArea` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `DoBurnEffect` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Explode` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `GetLightingFromCollisionBelow` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `GetObjectCarriedWithRope` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `GetRopeHeight` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `GrabObjectToCarryWithRope` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Init` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `IsObjectPointerValid` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `IsObjectPointerValid_NotInWorld` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Load` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `LockDoor` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ObjectDamage` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ObjectFireDamage` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `PreRender` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ProcessControl` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ProcessControlLogic` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ProcessGarageDoorBehaviour` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ProcessSamSiteBehaviour` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ProcessTrainCrossingBehaviour` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `RefModelInfo` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ReleaseObjectCarriedWithRope` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `RemoveFromControlCodeList` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `RemoveLighting` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Render` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `ResetDoorAngle` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Save` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SetIsStatic` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SetMatrixForTrainCrossing` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SetObjectTargettable` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SetRelatedDummy` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SetRemapTexture` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SetRopeHeight` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SetupLighting` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SpecialEntityCalcCollisionSteps` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `SpecialEntityPreCollisionStuff` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `Teleport` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `TryToExplode` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObject | `TryToFreeUpTempObjects` | [Resumen](../reversing/Object48/validation-summary.json) |
| CObjectData | `Initialise` | [Resumen](../reversing/Core70/validation-summary.json) |
| CObjectData | `SetObjectData` | [Resumen](../reversing/Core70/validation-summary.json) |
| CObjectPool | `GetAt` | [Resumen](../reversing/Core70/validation-summary.json) |
| CObjectPool | `New` | [Resumen](../reversing/Core70/validation-summary.json) |
| CObjectSaveStructure | `Construct` | [Resumen](../reversing/Core70/validation-summary.json) |
| CObjectSaveStructure | `Extract` | [Resumen](../reversing/Core70/validation-summary.json) |
| COccluder | `NearCamera` | [Resumen](../reversing/World52/validation-summary.json) |
| COccluder | `ProcessLineSegment` | [Resumen](../reversing/World52/validation-summary.json) |
| COccluder | `ProcessOneOccluder` | [Resumen](../reversing/World52/validation-summary.json) |
| COcclusion | `AddOne` | [Resumen](../reversing/World43/validation-summary.json) |
| COcclusion | `Init` | [Resumen](../reversing/World43/validation-summary.json) |
| COcclusion | `IsPositionOccluded` | [Resumen](../reversing/World43/validation-summary.json) |
| COcclusion | `OccluderHidesBehind` | [Resumen](../reversing/World43/validation-summary.json) |
| COcclusion | `ProcessBeforeRendering` | [Resumen](../reversing/World43/validation-summary.json) |
| COnscreenCounterEntry | `ProcessForDisplayCounter` | [Resumen](../reversing/Misc70/validation-summary.json) |
| COnscreenCounterEntry | `SetColourID` | [Resumen](../reversing/Misc70/validation-summary.json) |
| COnscreenTimer | `AddClock` | [Resumen](../reversing/Misc6/validation-summary.json) |
| COnscreenTimer | `AddCounter` | [Resumen](../reversing/Misc6/validation-summary.json) |
| COnscreenTimer | `ClearClock` | [Resumen](../reversing/Misc6/validation-summary.json) |
| COnscreenTimer | `ClearCounter` | [Resumen](../reversing/Misc6/validation-summary.json) |
| COnscreenTimer | `SetClockBeepCountdownSecs` | [Resumen](../reversing/Misc6/validation-summary.json) |
| COnscreenTimer | `SetCounterFlashWhenFirstDisplayed` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CPPTriPlantBuffer | `ChangeCurrentPlantModelsSet` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPPTriPlantBuffer | `Flush` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPPTriPlantBuffer | `GetPPTriPlantPtr` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPPTriPlantBuffer | `GetPlantModelsTab` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPPTriPlantBuffer | `IncreaseBufferIndex` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPPTriPlantBuffer | `SetGrassMaterialCB` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPPTriPlantBuffer | `SetPlantModelsTab` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPathNode | `GetPosition` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPed | `AddGogglesModel` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `AddWeaponModel` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `AttachPedToBike` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `AttachPedToEntity` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `CalculateNewOrientation` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `CalculateNewVelocity` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `CanBeArrested` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `CanBeDeleted` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `CanBeDeletedEvenInVehicle` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `CanPedHoldConversation` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `CanPedReturnToState` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `CanSeeEntity` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `CanSetPedState` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `CanStrafeOrMouseControl` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `CanThrowEntityThatThisPedIsHolding` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `CanUseTorsoWhenLooking` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `ClearAimFlag` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `ClearAll` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `ClearLook` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `ClearLookFlag` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `ClearWeapon` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `ClearWeapons` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `Constructor` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `CreateDeadPedMoney` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `CreateDeadPedPickupCoors-` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `CreateDeadPedWeaponPickups` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `DeadPedMakesTyresBloody` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `DeleteRwObject` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `Destructor` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `DettachPedFromEntity` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `DisablePedSpeech` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `DisablePedSpeechForScriptSpeech` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `DoFootLanded` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `DoGunFlash` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `DoWeHaveWeaponAvailable` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `DoesLOSBulletHitPed` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `Dress` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `DropEntityThatThisPedIsHolding` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `EnablePedSpeech` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `EnablePedSpeechForScriptSpeech` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `FlagToDestroyWhenNextProcessed` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `GetBikeRidingSkill` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `GetBonePosition-Original` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `GetEntityThatThisPedIsHolding` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GetHoldingTask` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GetLocalDirection` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `GetPedTalking` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `GetTransformedBonePosition` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `GetWalkAnimSpeed` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `GetWeaponSkill-Current` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GetWeaponSkill-WeaponType` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GetWeaponSlot` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GiveDelayedWeapon` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GiveObjectToPedToHold` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GiveWeapon-` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GiveWeaponAtStartOfFight` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GiveWeaponWhenJoiningGang` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `GrantAmmo` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `Initialise` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `IsAlive` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `IsPedHeadAbovePos` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `IsPedInControl` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `IsPedShootable` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `IsPlayer` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `IsPlayingHandSignal` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `IsPointerValid` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `IsWearingGoggles` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `KillPedWithCar` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `Load` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `Look` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `MakeTyresMuddySectorList<CPtrListSingleLink<CPhysical*>>` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `OurPedCanSeeThisEntity` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `PedCanPickUpPickUp` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `PedIsInvolvedInConversation` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `PedIsReadyForConversation` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `PlayFootSteps` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `PositionAnyPedOutOfCollision` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `PositionAttachedPed` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `PositionPedOutOfCollision` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `PreRender` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `PreRenderAfterTest` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `ProcessBuoyancy` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `PutOnGoggles` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `ReleaseCoverPoint` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `RemoveBodyPart` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `RemoveGogglesModel` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `RemoveLighting` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `RemoveWeaponAnims` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `RemoveWeaponForScriptedCutscene` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `RemoveWeaponModel` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `RemoveWeaponWhenEnteringVehicle` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `Render` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `ReplaceWeaponForScriptedCutscene` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `ReplaceWeaponWhenExitingVehicle` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `RequestDelayedWeapon` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `ResetGunFlashAlpha` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `RestartNonPartialAnims` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `RestoreHeadingRate` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `RestoreHeadingRateCB` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `Save` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `Say` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `SayScript` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `SetAimFlag-Entity` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetAimFlag-Heading` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetAmmo` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `SetCharCreatedBy` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `SetCurrentWeapon-Slot` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `SetCurrentWeapon-WepType` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `SetGunFlashAlpha` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `SetIdle` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetLook-Entity` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetLook-Heading` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetLookFlag-Entity` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetLookFlag-Heading` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetLookTimer` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetModelIndex` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `SetMoveAnim` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetMoveAnimSpeed` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetMoveState` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SetPedAtomicVisibilityCB` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `SetPedDefaultDecisionMaker` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `SetPedPositionInCar` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `SetPedState` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `SetPedStats` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `SetRadioStation` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `SetStayInSamePlace` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `SetWeaponSkill` | [Resumen](../reversing/CPedA/validation-summary.json) |
| CPed | `SetupLighting` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `ShoulderBoneRotation` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `SpawnFlyingComponent` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `StopNonPartialAnims` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `StopPlayingHandSignal` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `TakeOffGoggles` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `Teleport` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `TurnBody` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `Undress` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `Update` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `UpdatePosition` | [Resumen](../reversing/CPedE/validation-summary.json) |
| CPed | `UpdateStatEnteringVehicle` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `UpdateStatLeavingVehicle` | [Resumen](../reversing/CPedD/validation-summary.json) |
| CPed | `UseGroundColModel` | [Resumen](../reversing/CPedB/validation-summary.json) |
| CPed | `WorkOutHeadingForMovingFirstPerson` | [Resumen](../reversing/CPedC/validation-summary.json) |
| CPed | `operator delete-anon` | [Resumen](../reversing/FixCore/validation-summary.json) |
| CPed | `operator new-anon` | [Resumen](../reversing/FixCore/validation-summary.json) |
| CPed | `operator new-poolIndexed` | [Resumen](../reversing/FixCore/validation-summary.json) |
| CPedAttractor | `BroadcastArrival` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `BroadcastDeparture` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `ComputeAttractHeading-v` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `ComputeAttractPos-v` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `ComputeAttractTime` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `ComputeDeltaHeading` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `ComputeDeltaPos` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `Constructor` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `DeRegisterPed` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `Destructor` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `GetHeadOfQueue` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `GetQueueSlot` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `GetTailOfQueue` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `GetTaskForPed` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `HasEmptySlot` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `IsAtHeadOfQueue` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `IsRegisteredWithPed` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `RegisterPed` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractor | `SetTaskForPed` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedAttractorManager | `BroadcastArrival-` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `BroadcastDeparture-` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `DeRegisterPed-` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `GetPedAttractorManager` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `GetPedUsingEffect-` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `GetRelevantAttractor-` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `HasEmptySlot` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `HasQueueTailArrivedAtSlot` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `IsApproachable` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `IsPedRegisteredWithEffect-Effect` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `IsPedRegisteredWithEffect-Ped` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `RegisterPedWithAttractor` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `RemoveEffect-` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorManager | `RestoreStuffFromMem` | [Resumen](../reversing/Mix14/validation-summary.json) |
| CPedAttractorPedPlacer | `PlacePedAtEffect` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CPedAttractorPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPedClothesDesc | `Constructor` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `GetIsWearingBalaclava` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `HasVisibleNewHairCut` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `HasVisibleTattoo` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `Initialise` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `SetModel-modelHash` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `SetModel-modelName` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedGroup | `Constructor` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `Destructor` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `FindDistanceToFurthestMember` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `FindDistanceToNearestMember` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `Flush` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `GetClosestGroupPed` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `IsAnyoneUsingCar` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `PlayerGaveCommand_Attack` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `PlayerGaveCommand_Gather` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `Process` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `RemoveAllFollowers` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroup | `Teleport` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorChat | `AllocateDefaultTasks` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorChat | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | `AllocateDefaultTasks` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowLimited | `AllocateDefaultTasks` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowLimited | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorRandom | `AllocateDefaultTasks` | [Resumen](../reversing/GroupTasksCompletion/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorRandom | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | `AllocateDefaultTasks` | [Resumen](../reversing/GroupTasksCompletion/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorStandStill | `AllocateDefaultTasks` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorStandStill | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupIntelligence | `AddEvent-` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `ComputeDefaultTasks` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `ComputeEventResponseTasks` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `Constructor` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `Flush` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `FlushTasks` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPedGroupIntelligence | `GetTaskDefault` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `GetTaskMain` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `GetTaskScriptCommand` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `GetTaskSecondary` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `GetTaskSecondarySlot` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `IsCurrentEventValid` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `IsGroupResponding` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `Process` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `ProcessIgnorePlayerGroup` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `ReportAllBarScriptTasksFinished` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `ReportAllTasksFinished` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `ReportFinishedTask-Impl` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `ReportFinishedTask-Wrapper` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `SetDefaultTaskAllocator` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `SetDefaultTaskAllocatorType` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `SetEventResponseTaskAllocator` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `SetGroupDecisionMakerType` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `SetPrimaryTaskAllocator` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `SetScriptCommandTask` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupIntelligence | `SetTask` | [Resumen](../reversing/PGI26/validation-summary.json) |
| CPedGroupMembership | `AddFollower` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `AddMember` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `AppointNewLeader` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `CountMembers` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `CountMembersExcludingLeader` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `From` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `GetLeader` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `GetMember` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `GetObjectForPedToHold` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `IsFollower` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `IsLeader` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `IsMember` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `Process` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `RemoveAllFollowers` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `RemoveMember-ByMemIdx` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `RemoveMember-ByPed` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `RemoveNFollowers` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroupMembership | `SetLeader` | [Resumen](../reversing/Group18/validation-summary.json) |
| CPedGroups | `Process` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroups | `RemoveGroup` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedIK | `Constructor` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `GetWorldMatrix` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `MoveLimb-non-normalized` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `MoveLimb-normalized` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `PitchForSlope` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `PointGunAtPosition` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `PointGunInDirection` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `RotateTorso` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIK | `RotateTorsoForArm` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CPedIntelligence | `AddTaskEventResponseNonTemp` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `AddTaskEventResponseTemp` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `AddTaskPhysResponse` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `AddTaskPrimaryMaybeInGroup` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `AreFriends` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `CanSeeEntityWithLights` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `ClearTaskDuckSecondary` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `ClearTasks` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `Constructor` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `Destructor` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `FindRespectedFriendInInformRange` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `FindTaskByType` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `FlushImmediately` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `FlushIntelligence` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetActivePrimaryTask` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetEffectInUse` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetMoveStateFromGoToTask` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetPedEntities` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskClimb` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskDuck` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskFighting` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskHold` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskInAir` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskJetPack` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskSwim` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskThrow` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetTaskUseGun` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `GetUsingParachute` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `IsFriendlyWith` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `IsInACarOrEnteringOne` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `IsInSeeingRange` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `IsInterestingEntity` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `IsPedGoingForCarDoor` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `IsRespondingToEvent` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `IsThreatenedBy` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `LookAtInterestingEntities` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `Process` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `ProcessAfterPreRender` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `ProcessAfterProcCol` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `ProcessEventHandler` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `ProcessFirst` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `ProcessStaticCounter` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `RecordEventForScript` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `Respects` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `RestorePedDecisionMakerType` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `SetEffectInUse` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `SetHearingRange` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `SetPedDecisionMakerType` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `SetPedDecisionMakerTypeInGroup` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `SetSeeingRange` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `SetTaskDuckSecondary` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligence | `TestForStealthKill` | [Resumen](../reversing/PedIntel52/validation-summary.json) |
| CPedIntelligencePool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPedList | `BuildListFromGroup_NoLeader` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedList | `BuildListFromGroup_NotInCar_NoLeader` | [Resumen](../reversing/CPedList/validation-summary.json) |
| CPedList | `BuildListOfPedsOfPedType` | [Resumen](../reversing/CPedList/validation-summary.json) |
| CPedList | `Empty` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedList | `ExtractPedsWithGuns` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedList | `RemovePedsAttackingPedType` | [Resumen](../reversing/CPedList/validation-summary.json) |
| CPedList | `RemovePedsThatDontListenToPlayer` | [Resumen](../reversing/CPedList/validation-summary.json) |
| CPedModelInfo | `AddXtraAtomics` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `AnimatePedColModelSkinned` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `AnimatePedColModelSkinnedWorld` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `CreateHitColModelSkinned` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `DeleteRwObject` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `GetModelType` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `IncrementVoice` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `SetClump` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedModelInfo | `SetFaceTexture` | [Resumen](../reversing/Models9/validation-summary.json) |
| CPedPlacement | `IsPositionClearForPed-OG` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CPedPlacement | `IsPositionClearOfCars-Ped` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CPedPlacement | `IsPositionClearOfCars-Pos` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CPedPool | `GetAtRef` | [Resumen](../reversing/Core70/validation-summary.json) |
| CPedPool | `New` | [Resumen](../reversing/Core70/validation-summary.json) |
| CPedScriptedTaskRecord | `GetRecordAssociatedWithEvent` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPedScriptedTaskRecord | `GetStatus-` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPedScriptedTaskRecord | `GetStatus-opcode` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPedScriptedTaskRecord | `GetVacantSlot` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPedScriptedTaskRecord | `Process` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPedScriptedTaskRecordData | `AssociateWithEvent` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedScriptedTaskRecordData | `AssociateWithTask` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedScriptedTaskRecordData | `Constructor` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedScriptedTaskRecordData | `Flush` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedScriptedTaskRecordData | `Set-CEventScriptCommand` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedScriptedTaskRecordData | `Set-CTask` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedScriptedTaskRecordData | `SetAsAttractorScriptTask` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedScriptedTaskRecordData | `SetAsGroupTask` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CPedShelterAttractor | `BroadcastDeparture` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CPedShelterAttractor | `ComputeAttractHeading` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CPedShelterAttractor | `ComputeAttractPos` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CPedShelterAttractor | `GetDisplacement` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CPedStats | `GetPedStatType` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedStats | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedStats | `LoadPedStats` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedStats | `Shutdown` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedTaskPair | `Flush` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPedToPlayerConversations | `Clear` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CPedToPlayerConversations | `EndConversation` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CPedToPlayerConversations | `Update` | [Resumen](../reversing/PedToPlayerConversations/validation-summary.json) |
| CPedType | `ClearPedTypeAcquaintances` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `ClearPedTypeAsAcquaintance` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `FindPedType` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `GetPedFlag` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `GetPedTypeAcquaintances-CAcquaintance` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `GetPedTypeAcquaintances-uint32` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `Initialise` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `Load` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `LoadPedData` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `PoliceDontCareAboutCrimesAgainstPedType` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `Save` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `SetPedTypeAsAcquaintance` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPedType | `Shutdown` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CPhysical | `Add` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `AddCollisionRecord` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `AddToMovingList` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyAirResistance` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyCollision-1` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyCollision-2` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyCollisionAlt` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyForce` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyFriction-other` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyFriction-self` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyFriction-void` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyFrictionForce` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyFrictionMoveForce` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyGravity` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyMoveForce-vec` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyMoveSpeed` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplySoftCollision-1` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplySoftCollision-2` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplySpeed` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplySpringCollision` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplySpringCollisionAlt` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplySpringDampening` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyTurnForce` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ApplyTurnSpeed` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `AttachEntityToEntity-quat` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `AttachEntityToEntity-vec` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `CanPhysicalBeDamaged` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `CheckCollision` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `CheckCollision_SimpleCar` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `Constructor` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `Destructor` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `DettachAutoAttachedEntity` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `DettachEntityFromEntity` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `GetBoundRect` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `GetHasCollidedWith` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `GetHasCollidedWithAnyObject` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `GetLightingFromCol` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `GetLightingTotal` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `GetSpeed` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `PlacePhysicalRelativeToOtherPhysical` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `PositionAttachedEntity` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ProcessCollision` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ProcessCollisionSectorList` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ProcessCollisionSectorList_SimpleCar` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ProcessControl` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ProcessEntityCollision` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ProcessShift` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `ProcessShiftSectorList` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `Remove` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `RemoveAndAdd` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `RemoveFromMovingList` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `RemoveRefsToEntity` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `SetDamagedPieceRecord` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `SkipPhysics` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `TestCollision` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPhysical | `UnsetIsInSafePosition` | [Resumen](../reversing/Physical56/validation-summary.json) |
| CPickup | `ExtractAmmoFromPickup` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `FindStringForTextIndex` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `FindTextIndexForString` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `GetPosn` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `GetRidOfObjects` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `GiveUsAPickUpObject` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `IsVisible` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `PickUpShouldBeInvisible` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `ProcessGunShot` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `Remove` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `SetPosn` | [Resumen](../reversing/World12/validation-summary.json) |
| CPickup | `Update` | [Resumen](../reversing/World12/validation-summary.json) |
| CPlaceName | `GetForMap` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CPlaceName | `Process` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CPlaceable | `AllocateMatrix` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `AllocateStaticMatrix` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `FreeStaticMatrix` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `GetHeading` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `GetMatrix` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `GetRoll` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `InitMatrixArray` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `IsWithinArea-xy` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `IsWithinArea-xyz` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `RemoveMatrix` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `SetHeading` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `SetMatrix` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `SetOrientation-xyz` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `SetPosn-vector` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `SetPosn-xyz` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaceable | `ShutdownMatrixArray` | [Resumen](../reversing/World16/validation-summary.json) |
| CPlaneTrail | `Init` | [Resumen](../reversing/World52/validation-summary.json) |
| CPlaneTrail | `RegisterPoint` | [Resumen](../reversing/World52/validation-summary.json) |
| CPlaneTrail | `Render` | [Resumen](../reversing/World52/validation-summary.json) |
| CPlaneTrails | `Init` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPlaneTrails | `RegisterPoint` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPlaneTrails | `Render` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPlaneTrails | `Update` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPlantColEntEntry | `AddEntry` | [Resumen](../reversing/Core70/validation-summary.json) |
| CPlantColEntEntry | `ReleaseEntry` | [Resumen](../reversing/Core70/validation-summary.json) |
| CPlantLocTri | `Add` | [Resumen](../reversing/World52/validation-summary.json) |
| CPlantLocTri | `GetPlantDensity` | [Resumen](../reversing/World52/validation-summary.json) |
| CPlantLocTri | `Release` | [Resumen](../reversing/World52/validation-summary.json) |
| CPlantMgr | `CalculateWindBending` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `Initialise` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `MoveColEntToList` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `MoveLocTriToList` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `PreUpdateOnceForNewCameraPos` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `ReloadConfig` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `Render` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `SetPlantFriendlyFlagInAtomicMI` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `Shutdown` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `Update` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `UpdateAmbientColor` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_ColEntityCache_Add` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_ColEntityCache_FindInCache` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_ColEntityCache_Remove` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_ColEntityCache_Update` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_ProcessEntryCollisionDataSections` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_ProcessEntryCollisionDataSections_AddLocTris` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_ProcessEntryCollisionDataSections_RemoveLocTris` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantMgr | `_UpdateLocTris` | [Resumen](../reversing/Plant19/validation-summary.json) |
| CPlantSurfPropMgr | `AllocSurfProperties` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlantSurfPropMgr | `GetSurfProperties` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlantSurfPropMgr | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlantSurfPropMgr | `LoadPlantsDat` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlayerInfo | `AddHealth` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `ArrestPlayer` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `BlowUpRCBuggy` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `CancelPlayerEnteringCars` | [Resumen](../reversing/FixPeds/validation-summary.json) |
| CPlayerInfo | `Clear` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `Constructor` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `DeletePlayerSkin` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `EvaluateCarPosition` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `FindClosestCarSectorList` | [Resumen](../reversing/CPlayerInfo/validation-summary.json) |
| CPlayerInfo | `FindObjectToSteal` | [Resumen](../reversing/CPlayerInfo/validation-summary.json) |
| CPlayerInfo | `GetPos_Hook` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `GetSpeed_Hook` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `GivePlayerParachute` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `IsPlayerInRemoteMode` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `IsRestartingAfterArrest` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `IsRestartingAfterDeath` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `IsRestartingAfterMissionFailed` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `KillPlayer` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `Load` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `LoadPlayerSkin` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `MakePlayerSafe` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `PlayerFailedCriticalMission` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `Process` | [Resumen](../reversing/CPlayerInfo/validation-summary.json) |
| CPlayerInfo | `Save` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `SetLastTargetVehicle` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `SetPlayerSkin` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `StreamParachuteWeapon` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerInfo | `WorkOutEnergyFromHunger` | [Resumen](../reversing/Player28/validation-summary.json) |
| CPlayerPed | `AnnoyPlayerPed` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `Busted` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `CanIKReachThisTarget` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `CanPlayerStartMission` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `CheatWantedLevel` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `Clear3rdPersonMouseTarget` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ClearAdrenaline` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ClearWeaponTarget` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `DeactivatePlayerPed` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `DisbandPlayerGroup` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `DoStuffToGoOnFire` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `DoesPlayerWantNewWeapon` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `DoesTargetHaveToBeBroken` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `EvaluateTarget` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `FindPedToAttack` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `FindTargetPriority` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ForceGroupToAlwaysFollow` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ForceGroupToNeverFollow` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `GetButtonSprintResults` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `GetPadFromPlayer` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `GetPlayerInfoForThisPlayerPed` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `GetWantedLevel` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `GetWeaponRadiusOnScreen` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `HandlePlayerBreath` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `HandleSprintEnergy` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `IsHidden` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `LOSBlockedBetweenPeds` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `Load` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `MakeChangesForNewWeapon-` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `MakeChangesForNewWeapon-BySlot` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `MakeGroupRespondToPlayerTakingDamage` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `MakePlayerGroupDisappear` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `MakePlayerGroupReappear` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `PedCanBeTargettedVehicleWise` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `PickWeaponAllowedFor2Player` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `PlayerHasJustAttackedSomeone` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ProcessControl` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ProcessPlayerWeapon` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ReApplyMoveAnims` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ReactivatePlayerPed` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `RemovePlayerPed` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ResetPlayerBreath` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `ResetSprintEnergy` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `Save` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `SetMoveAnim` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `SetPlayerMoveBlendRatio` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `SetWantedLevel` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `SetWantedLevelNoDrop` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `TellGroupToStartFollowingPlayer` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPed | `UpdateCameraWeaponModes` | [Resumen](../reversing/PlayerPed51/validation-summary.json) |
| CPlayerPedData | `AllocateData` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPlayerPedData | `Constructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPlayerPedData | `DeAllocateData` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPlayerPedData | `Destructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPlayerPedData | `SetInitialState` | [Resumen](../reversing/Events43/validation-summary.json) |
| CPlayerSkin | `GetSkinTexture` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPlayerSkin | `Initialise` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPlayerSkin | `RenderFrontendSkinEdit` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPlayerSkin | `Shutdown` | [Resumen](../reversing/Render23/validation-summary.json) |
| CPointRoutePool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPopCycle | `FindNewPedType` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `GetCurrentPercOther_Peds` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `Initialise` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `IsPedAppropriateForCurrentZone` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `IsPedInGroup` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `PedIsAcceptableInCurrentZone` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `PickARandomGroupOfOtherPeds` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CPopCycle | `PickGangToCreateMembersOf` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `PickPedMIToStreamInForCurrentZone` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `PlayerKilledADealer` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `Update` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `UpdateAreaDodgyness` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `UpdateDealerStrengths` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `UpdateIsGangArea` | [Resumen](../reversing/World15/validation-summary.json) |
| CPopCycle | `UpdatePercentages` | [Resumen](../reversing/World15/validation-summary.json) |
| CPtrNodeDoubleLinkPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPtrNodeSingleLinkPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CQuadBike | `Constructor` | [Resumen](../reversing/VehCtor62/validation-summary.json) |
| CQuadBike | `Fix` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `GetRideAnimData` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `PreRender` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `ProcessAI` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `ProcessControl` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `ProcessControlInputs` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `ProcessDrivingAnims` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `ProcessSuspension` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `ResetSuspension` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `SetupDamageAfterLoad` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadBike | `SetupSuspensionLines` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CQuadTreeNode<T> | `AddItem` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `DeleteItem-` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `DeleteItem-rect` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `FindSector-rect` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `FindSector-vec` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `ForAllMatching-rect` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `ForAllMatching-vec` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `GetAll` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `GetAllMatching-rect` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `GetAllMatching-vec` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `InSector` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CQuadTreeNode<T> | `InitPool` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CRealTimeShadow | `Constructor` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRealTimeShadow | `Create` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRealTimeShadow | `Destroy` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRealTimeShadow | `Destructor` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRealTimeShadow | `DrawBorderAroundTexture` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRealTimeShadow | `GetShadowRwTexture` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRealTimeShadow | `SetLightProperties` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRealTimeShadow | `Update` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `GetCenter-` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `IsFlipped` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `IsPointInside-` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `IsPointInside-Tolerance` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `Resize` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `Restrict` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `SetFromCenter` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CRect | `StretchToPoint` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CReferences | `Init` | [Resumen](../reversing/Events23/validation-summary.json) |
| CReferences | `ListSize` | [Resumen](../reversing/Events23/validation-summary.json) |
| CReferences | `PruneAllReferencesInWorld` | [Resumen](../reversing/Events23/validation-summary.json) |
| CReferences | `RemoveReferencesToPlayer` | [Resumen](../reversing/Events23/validation-summary.json) |
| CRegisteredCorona | `Update` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CRegisteredMotionBlurStreak | `Render` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CRegisteredMotionBlurStreak | `Update` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CRenderer | `AddEntityToRenderList` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `AddToLodDontRenderList` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `AddToLodRenderList` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ConstructRenderList` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `GetLodDontRenderListBase` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `GetLodRenderListBase` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `GetObjectsInFrustum` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `PreRender` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ProcessLodRenderLists` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RemoveVehiclePedLights` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RenderEverythingBarRoads` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RenderFadingInEntities` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RenderFadingInUnderwaterEntities` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RenderFirstPersonVehicle` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RenderOneNonRoad` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RenderOneRoad` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RenderRoads` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RequestObjectsInDirection` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `RequestObjectsInFrustum` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ResetLodRenderLists` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ScanBigBuildingList` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ScanPtrList_RequestModels<CPtrListSingleLink<CPhysical*>>` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ScanSectorList` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ScanSectorList_ListModels` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ScanSectorList_ListModelsVisible` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ScanSectorList_RequestModels` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ScanWorld` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `SetupBigBuildingVisibility` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `SetupEntityVisibility` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `SetupLightingForEntity` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `SetupMapEntityVisibility` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `SetupScanLists` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CRenderer | `ShouldModelBeStreamed` | [Resumen](../reversing/Renderer33/validation-summary.json) |
| CReplay | `CanWeFindPoolIndexForPed` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `CanWeFindPoolIndexForVehicle` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `CreatePlayerPed` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `DealWithNewPedPacket` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `DisableReplays` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `Display` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `EmptyPedsAndVehiclePools_NoDestructors` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `EmptyReplayBuffer` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `EnableReplays` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `FastForwardToTime` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `FindFirstFocusCoordinate` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `FindPoolIndexForPed` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `FindPoolIndexForVehicle` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `FindSizeOfPacket` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `FinishPlayback` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `GoToNextBlock` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `Init` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `InitialisePedPoolConversionTable` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `InitialisePoolConversionTables` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `InitialiseVehiclePoolConversionTable` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `IsThisPedUsedInRecording` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `IsThisVehicleUsedInRecording` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `MarkEverythingAsNew` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `NumberFramesAvailableToPlay` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `PlayBackThisFrame` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `PlayBackThisFrameInterpolation` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `PlayReplayFromHD` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `ProcessLookAroundCam` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `ProcessPedUpdate` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `ProcessReplayCamera` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `RecordPedDeleted` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `RecordThisFrame` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `RecordVehicleDeleted` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `RestoreClothesDesc` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `RestorePlayerInfoVariables` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `RestoreStuffFromMem` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `RetrievePedAnimation` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `SaveReplayToHD` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `ShouldStandardCameraBeProcessed` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `StoreClothesDesc` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `StorePedAnimation` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `StorePedUpdate` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `StorePlayerInfoVariables` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `StoreStuffInMem` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `StreamAllNecessaryCarsAndPeds` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `TriggerPlayback` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CReplay | `Update` | [Resumen](../reversing/Replay47/validation-summary.json) |
| CRestart | `AddHospitalRestartPoint` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `AddPoliceRestartPoint` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `CancelOverrideRestart` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `ClearRespawnPointForDurationOfMission` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `FindClosestHospitalRestartPoint` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `FindClosestPoliceRestartPoint` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `Initialise` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `Load` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `OverrideNextRestart` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `Save` | [Resumen](../reversing/World11/validation-summary.json) |
| CRestart | `SetRespawnPointForDurationOfMission` | [Resumen](../reversing/World11/validation-summary.json) |
| CRopes | `CreateRopeForSwatPed` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `FindCoorsAlongRope` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `FindPickupHeight` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `FindRope` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `Init` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `IsCarriedByRope` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `RegisterRope` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `Render` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `SetSpeedOfTopNode` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `Shutdown` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CRopes | `Update` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CScriptResourceManager | `AddToResourceManager` | [Resumen](../reversing/Data23/validation-summary.json) |
| CScriptResourceManager | `HasResourceBeenRequested` | [Resumen](../reversing/Data23/validation-summary.json) |
| CScriptResourceManager | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CScriptResourceManager | `RemoveFromResourceManager` | [Resumen](../reversing/Data23/validation-summary.json) |
| CScripted2dEffects | `AddScripted2DEffect` | [Resumen](../reversing/World43/validation-summary.json) |
| CScripted2dEffects | `GetEffectPairs` | [Resumen](../reversing/World43/validation-summary.json) |
| CScripted2dEffects | `GetIndex` | [Resumen](../reversing/World43/validation-summary.json) |
| CScripted2dEffects | `Init` | [Resumen](../reversing/CScripted2dEffects/validation-summary.json) |
| CScripted2dEffects | `ReturnScripted2DEffect` | [Resumen](../reversing/World43/validation-summary.json) |
| CScriptedBrainTaskEntry | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CScriptedBrainTaskStore | `Clear-ped` | [Resumen](../reversing/Events23/validation-summary.json) |
| CScriptedBrainTaskStore | `Clear-task` | [Resumen](../reversing/Events23/validation-summary.json) |
| CScriptedBrainTaskStore | `GetTask` | [Resumen](../reversing/Events23/validation-summary.json) |
| CScriptedBrainTaskStore | `SetTask` | [Resumen](../reversing/Events23/validation-summary.json) |
| CScriptsForBrains | `GetIndexOfScriptBrainWithThisName` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CScriptsForBrains | `HasAttractorScriptBrainWithThisNameLoaded` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CScriptsForBrains | `Init` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CScriptsForBrains | `IsObjectWithinBrainActivationRange` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CScriptsForBrains | `StartAttractorScriptBrainWithThisName` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CScriptsForBrains | `StartOrRequestNewStreamedScriptBrain` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CSetPiece | `TryToGenerateCopCar` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CSetPiece | `TryToGenerateCopPed` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CSetPiece | `Update` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CSetPieces | `AddOne` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Init` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Load` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Save` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Update` | [Resumen](../reversing/World43/validation-summary.json) |
| CShadowCamera | `Constructor` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `Create` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `Destroy` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `Destructor` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `DrawBlurryRaster2` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `DrawOutlineBorder` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `GetRwRenderRaster` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `GetRwRenderTexture` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `Im2DRenderQuad` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `InvertRaster` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `MakeGradientRaster` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `RasterBlur` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `RasterResample` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `SetCenter` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `SetFrustum` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `SetLight` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `Update-Atomic` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `Update-Clump` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShadowCamera | `atomicQuickRender` | [Resumen](../reversing/Render19/validation-summary.json) |
| CShinyTexts | `Init` | [Resumen](../reversing/Render23/validation-summary.json) |
| CShinyTexts | `RegisterOne` | [Resumen](../reversing/Render23/validation-summary.json) |
| CShinyTexts | `Render` | [Resumen](../reversing/Render23/validation-summary.json) |
| CShinyTexts | `RenderOutGeometryBuffer` | [Resumen](../reversing/Render23/validation-summary.json) |
| CShopping | `Buy` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `FindItem` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `FindSection` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `FindSectionInSection` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `GetExtraInfo` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `GetItemIndex` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `GetKey` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `GetNameTag` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `GetNextSection` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `GetPrice` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `GetPriceSectionFromName` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `HasPlayerBought` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `IncrementStat` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `IncrementStat2` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `Init` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `Load` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `LoadPrices` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `LoadShop` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `LoadStats` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `RemoveLoadedPrices` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `RemoveLoadedShop` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `RestoreClothesState` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `RestoreVehicleMods` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `Save` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `SetCurrentProperty` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `SetPlayerHasBought` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `ShutdownForRestart` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `StoreClothesState` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `StoreVehicleMods` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShopping | `UpdateStats` | [Resumen](../reversing/Shop30/validation-summary.json) |
| CShotInfo | `AddShot` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CShotInfo | `GetFlameThrowerShotPosn` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CShotInfo | `Initialise` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CShotInfo | `Shutdown` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CShotInfo | `Update` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CSimpleVariablesSaveStructure | `Construct` | [Resumen](../reversing/Core70/validation-summary.json) |
| CSimpleVariablesSaveStructure | `Extract` | [Resumen](../reversing/Core70/validation-summary.json) |
| CSkidmarks | `Clear` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSkidmarks | `Init` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSkidmarks | `RegisterOne-` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSkidmarks | `RegisterOne-EC0` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSkidmarks | `Render` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSkidmarks | `Shutdown` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSkidmarks | `Update` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialFX | `AddWeaponStreak` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialFX | `Init` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialFX | `Render` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialFX | `Render2DFXs` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialFX | `ReplayStarted` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialFX | `Shutdown` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialFX | `Update` | [Resumen](../reversing/Data7/validation-summary.json) |
| CSpecialPlateHandler | `Add` | [Resumen](../reversing/Data23/validation-summary.json) |
| CSpecialPlateHandler | `Find` | [Resumen](../reversing/Data23/validation-summary.json) |
| CSpecialPlateHandler | `Init` | [Resumen](../reversing/Data23/validation-summary.json) |
| CSpecialPlateHandler | `Remove` | [Resumen](../reversing/Data23/validation-summary.json) |
| CSprite | `CalcHorizonCoors` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `CalcScreenCoors` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `FlushSpriteBuffer` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `InitSpriteBuffer` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `Initialise` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderBufferedOneXLUSprite` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderBufferedOneXLUSprite2D` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderBufferedOneXLUSprite_Rotate_2Colours` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderBufferedOneXLUSprite_Rotate_Aspect` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderBufferedOneXLUSprite_Rotate_Dimension` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderOneXLUSprite` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderOneXLUSprite2D` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderOneXLUSprite_Rotate_Aspect` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `RenderOneXLUSprite_Triangle` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite | `Set4Vertices2D-CRect` | [Resumen](../reversing/World15/validation-summary.json) |
| CSprite2d | `Constructor` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `Delete` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `Destructor` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `Draw-CRectCRGBA` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `Draw-CRectCRGBA4` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `Draw-ffffCRGBA` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `Draw-ffffffffCRGBA` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `Draw2DPolygon` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawAnyRect` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawBarChart` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawCircleAtNearClip` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawRect-CRGBA` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawRect-CRGBA4` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawRectXLU` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawTxRect` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `DrawWithBilinearOffset` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `InitPerFrame` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `IsVertexBufferEmpty` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `IsVertexBufferFull` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `RenderVertexBuffer` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetAddressing` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetAddressingUV` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetMaskVertices` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetRecipNearClip` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetRenderState` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetTexture-` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetTexture-mask` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetVertices-CRectCRGBA4` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetVertices-CRectCRGBA4ffffffff` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetVertices-RwD3D9Vertex` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetVertices-ffffffffCRGBA` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetVertices-ifCRGBA` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CSprite2d | `SetVertices-iffCRGBA` | [Resumen](../reversing/Sprite2d33/validation-summary.json) |
| CStaticShadow | `Free` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CStats | `CheckForThreshold` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `ConvertToMins` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `ConvertToSecs` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `DecrementStat` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `DisplayScriptStatUpdateMessage` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `FindCriminalRatingNumber` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `FindMaxNumberOfGroupMembers` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `GetPercentageProgress` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `GetStatID` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `GetStatValue--OG` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `GetTimesMissionAttempted` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `IncrementStat` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `Init` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `IsStatCapped` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `IsStatFloat` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `Load` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `LoadActionReactionStats` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `ModifyStat` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `ProcessReactionStatsOnDecrement` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `ProcessReactionStatsOnIncrement` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `RegisterBestPosition` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `RegisterFastestTime` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `RegisterMissionAttempted` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `RegisterMissionPassed` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `SafeToShowThisStat` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `Save` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `SetNewRecordStat` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `SetStatValue` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `UpdateStatsWhenFighting` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `UpdateStatsWhenOnMotorBike` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `UpdateStatsWhenRunning` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStats | `UpdateStatsWhenSprinting` | [Resumen](../reversing/Stats32/validation-summary.json) |
| CStencilShadowObject | `Constructor` | [Resumen](../reversing/World52/validation-summary.json) |
| CStencilShadowObject | `Destroy` | [Resumen](../reversing/World52/validation-summary.json) |
| CStencilShadowObject | `Destructor` | [Resumen](../reversing/World52/validation-summary.json) |
| CStencilShadows | `GraphicsHighQuality` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `Init` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `Process` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `RegisterStencilShadows` | [Resumen](../reversing/StencilReg62/validation-summary.json) |
| CStencilShadows | `Render` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `RenderBuffer` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `RenderForObject` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `RenderForVehicle` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `RenderStencilShadows` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `Shutdown` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `SunSetPositionFromEntity` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `UpdateHierarchy` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStencilShadows | `sub_710CC0` | [Resumen](../reversing/Misc61b/validation-summary.json) |
| CStreamedScripts | `FindStreamedScript` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `FindStreamedScriptQuiet` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `GetProperIndexFromIndexUsedByScript` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `GetStreamedScriptFilename` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `GetStreamedScriptWithThisStartAddress` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `Initialise` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `LoadStreamedScript` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `ReInitialise` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `ReadStreamedScriptData` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `RegisterScript` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `RemoveStreamedScriptFromMemory` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamedScripts | `StartNewStreamedScript` | [Resumen](../reversing/Sys12/validation-summary.json) |
| CStreamingInfo | `AddToList` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `GetCdPosn` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `GetCdPosnAndSize` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `InList` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `Init` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `RemoveFromList` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `SetCdPosnAndSize` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStuckCarCheck | `AddCarToCheck` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuckCarCheck | `AttemptToWarpVehicle` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuckCarCheck | `ClearStuckFlagForCar` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuckCarCheck | `HasCarBeenStuckForAWhile` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuckCarCheck | `Init` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuckCarCheck | `IsCarInStuckCarArray` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuckCarCheck | `Process` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuckCarCheck | `RemoveCarFromCheck` | [Resumen](../reversing/Data8b/validation-summary.json) |
| CStuntJumpManager | `AddOne` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Init` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Load` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Save` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Shutdown` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `ShutdownForRestart` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Update` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTagManager | `AddTag` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `FindTagDesc` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `GetAlpha-Entity` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `GetAlpha-RpAtomic` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `GetNearestTag` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `GetPercentageTagged` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `GetPercentageTaggedInArea` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `GetTagPos` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `Init` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `IsTag` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `Load` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `RenderTagForPC` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `Save` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `SetAlpha-Entity` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `SetAlpha-RpAtomic` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `SetAlphaInArea` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `SetupAtomic` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `ShutdownForRestart` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTagManager | `UpdateNumTagged` | [Resumen](../reversing/Game19/validation-summary.json) |
| CTask | `IsTaskPtr` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTask | `StopTimer` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTask | `operator delete` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTask | `operator new` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskAllocator | `Abort` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskAllocator | `IsFinished` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskAllocator | `ProcessGroup` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskAllocatorAttack | `AllocateTasks` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorAttack | `Constructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorAttack | `Destructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorAttack | `GetType` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorAttack | `ProcessGroup` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorKillOnFoot | `AllocateTasks` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskAllocatorKillOnFoot | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskAllocatorKillOnFoot | `Destructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskAllocatorKillOnFoot | `GetType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskAllocatorKillThreatsBasic | `AllocateTasks` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskAllocatorKillThreatsBasic | `ComputeClosestPeds` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskAllocatorKillThreatsBasic | `Constructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskAllocatorKillThreatsBasic | `Destructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskAllocatorKillThreatsBasic | `GetType` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskAllocatorKillThreatsBasic | `ProcessGroup` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskAllocatorKillThreatsBasicRandomGroup | `AllocateTasks` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskAllocatorKillThreatsBasicRandomGroup | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskAllocatorKillThreatsDriveby | `AllocateTasks` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorKillThreatsDriveby | `Constructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorKillThreatsDriveby | `Destructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorKillThreatsDriveby | `GetType` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorKillThreatsDriveby | `ProcessGroup` | [Resumen](../reversing/Events43/validation-summary.json) |
| CTaskAllocatorPlayerCommandAttack | `AllocateTasks` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskAllocatorPlayerCommandAttack | `GetType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskAllocatorPlayerCommandAttack | `ProcessGroup` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskAllocatorPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CTaskComplexAttractorPartnerWait | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexAttractorPartnerWait | `ControlSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexAttractorPartnerWait | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexAttractorPartnerWait | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexAttractorPartnerWait | `GetTaskType` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `Clone` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `ComputeAvoidSphere` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `ComputeDetourTarget` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `ComputeRouteRoundSphere` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `ComputeSphere` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `Constructor` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `ControlSubTask` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `CreateFirstSubTask` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `CreateNextSubTask` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `Destructor` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `GetTaskType` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `MakeAbortable` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `NearbyPedsInSphere` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `QuitIK` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexAvoidOtherPedWhileWandering | `SetUpIK` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskComplexBeCop | `Clone` | [Resumen](../reversing/BeCop60/validation-summary.json) |
| CTaskComplexBeCop | `Destructor` | [Resumen](../reversing/BeCop60/validation-summary.json) |
| CTaskComplexBeCop | `GetTaskType` | [Resumen](../reversing/BeCop60/validation-summary.json) |
| CTaskComplexBeInCouple | `AbortArmIK` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `ControlSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `CreateNextSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `GetTaskType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInCouple | `MakeAbortable` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexBeInGroup | `Clone` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `Constructor` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `ControlSubTask` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `CreateFirstSubTask` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `CreateNextSubTask` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `Destructor` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `GetTaskType` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `MakeAbortable` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `MonitorMainGroupTask` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeInGroup | `MonitorSecondaryGroupTask` | [Resumen](../reversing/Data10/validation-summary.json) |
| CTaskComplexBeStill | `Clone` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `CreateNextSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `Destructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `GetTaskType` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexCarDrive | `Clone` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `Constructor_0` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `Constructor_1` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `ControlSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `CreateFirstSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `CreateNextSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `CreateSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `CreateSubTaskCannotGetInCar` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `Destructor` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `Drive` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDrive | `SetUpCar` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarDriveMission | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveMission | `Constructor` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveMission | `Destructor` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveMission | `GetTaskType` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveMission | `SetUpCar` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveMissionFleeScene | `Clone` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveMissionFleeScene | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveMissionFleeScene | `Destructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveMissionFleeScene | `GetTaskType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveMissionKillPed | `Clone` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveMissionKillPed | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveMissionKillPed | `Destructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveMissionKillPed | `GetTaskType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCarDriveWander | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveWander | `Constructor` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveWander | `CreateSubTaskCannotGetInCar` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveWander | `GetTaskType` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarDriveWander | `SetUpCar` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `Clone` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `Constructor` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `ControlSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `CreateFirstSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `CreateNextSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `CreateSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `CreateTaskUtilityLineUpPedWithCar` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `Destructor` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `GetTaskType` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `MakeAbortable` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOut | `PrepareVehicleForPedExit` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `Clone` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `Constructor` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `ControlSubTask` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `CreateFirstSubTask` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `CreateNextSubTask` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `CreateSubTask` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `Destructor` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `GetTaskType` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CTaskComplexCarSlowBeDraggedOutAndStandUp | `MakeAbortable` | [Resumen](../reversing/DragOut60/validation-summary.json) |
| CTaskComplexChat | `Clone` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexChat | `Constructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexChat | `ControlSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexChat | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexChat | `CreateNextSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexChat | `Destructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexChat | `GetTaskType` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexClimb | `Clone` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexClimb | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexClimb | `Destructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexClimb | `GetTaskType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexCopInCar | `Clone` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `Constructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `ControlSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `CreateNextSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `CreateSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `Destructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `GetTaskType` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCopInCar | `MakeAbortable` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `ControlSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `CreateNextSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `CreateSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `GetTaskType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexCrossRoadLookAndAchieveHeading | `MakeAbortable` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `ControlSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `CreateNextSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `CreateSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `GetTaskType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCar | `MakeAbortable` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `CalculateSearchPositionAndRanges` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `Clone` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `Constructor` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `ControlSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `CreateNextSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `CreateSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `Destructor` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `GetTaskType` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarArmed | `MakeAbortable` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `CalculateSearchPositionAndRanges` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `Clone` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `Constructor` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `ControlSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `CreateFirstSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `CreateNextSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `CreateSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `Destructor` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `GetTaskType` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDestroyCarMelee | `MakeAbortable` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexDie | `Constructor` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexDie | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexDie | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexDie | `MakeAbortable` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexDie | `SayDeathSample` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexDieInCar | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDieInCar | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDieInCar | `CreateNextSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDieInCar | `CreateSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDieInCar | `MakeAbortable` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDieInCar | `PreparePedVehicleForPedDeath` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDiveFromAttachedEntityAndGetUp | `Clone` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDiveFromAttachedEntityAndGetUp | `Constructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDiveFromAttachedEntityAndGetUp | `ControlSubTask` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CTaskComplexDiveFromAttachedEntityAndGetUp | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDiveFromAttachedEntityAndGetUp | `CreateNextSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDiveFromAttachedEntityAndGetUp | `Destructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDiveFromAttachedEntityAndGetUp | `GetTaskType` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDragPedFromCar | `ControlSubTask` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskComplexDragPedFromCar | `CreateFirstSubTask` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskComplexDriveFireTruck | `Clone` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDriveFireTruck | `Constructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDriveFireTruck | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDriveFireTruck | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDriveFireTruck | `CreateNextSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDriveFireTruck | `CreateSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexDrivePointRoute | `Clone` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDrivePointRoute | `Constructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDrivePointRoute | `ControlSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDrivePointRoute | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDrivePointRoute | `CreateNextSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDrivePointRoute | `Destructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDrivePointRoute | `GetTaskType` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexDriveToPoint | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexDriveToPoint | `Constructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexDriveToPoint | `CreateSubTaskCannotGetInCar` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexDriveToPoint | `Drive` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexDriveToPoint | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexDriveToPoint | `IsTargetBlocked-` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexDriveToPoint | `IsTargetBlocked-entities` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexDriveToPoint | `SetUpCar` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `ControlSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `GetTaskType` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `Clone` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `Constructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `ControlSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `CreateNextSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `CreateSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `Destructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterBoatAsDriver | `GetTaskType` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexEnterCar | `Constructor` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `ControlSubTask` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `CreateFirstSubTask` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `CreateNextSubTask` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `CreateNextSubTask_AfterSimpleCarAlign` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `CreateSubTask` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `CreateTaskUtilityLineUpPedWithCar` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `Destructor` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `GetCameraAvoidVehicle` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `GetTargetPos` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `MakeAbortable` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `PreparePedForVehicleEnter` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `PrepareVehicleForPedEnter` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCar | `SetVehicleFlags` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexEnterCarAsDriver | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsDriver | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsDriver | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `Clone` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `Constructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `ControlSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `CreateNextSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `Destructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `GetTaskType` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `MakeAbortable` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsDriverTimed | `StopTimer` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassenger | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsPassenger | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsPassenger | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `Clone` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `Constructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `ControlSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `CreateNextSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `Destructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `GetTaskType` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `MakeAbortable` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerTimed | `StopTimer` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `Constructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `ControlSubTask` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `CreateNextSubTask` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `CreateSubTask` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `Destructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEnterCarAsPassengerWait | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexEvasiveCower | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveCower | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveCower | `ControlSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveCower | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveCower | `CreateNextSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveCower | `CreateSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveCower | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveCower | `GetTaskType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `Clone` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `Constructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `ControlSubTask` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `CreateNextSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `CreateSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `Destructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `GetTaskType` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveDiveAndGetUp | `MakeAbortable` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexEvasiveStep | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveStep | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveStep | `ControlSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveStep | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveStep | `CreateNextSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveStep | `CreateSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveStep | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexEvasiveStep | `GetTaskType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexExtinguishFires | `Clone` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexExtinguishFires | `Constructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexExtinguishFires | `ControlSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexExtinguishFires | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexExtinguishFires | `CreateNextSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexExtinguishFires | `Destructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexExtinguishFires | `GetNearestFire` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexExtinguishFires | `GetTaskType` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFacial | `Clone` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `Constructor` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `ControlSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `CreateNextSubTask` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `GetTaskType` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `MakeAbortable` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `SetRequest` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFacial | `StopAll` | [Resumen](../reversing/CTasks9a/validation-summary.json) |
| CTaskComplexFallAndGetUp | `Constructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `Constructor2` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `ControlSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `CreateNextSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `CreateSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `IsFalling` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `MakeAbortable` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallAndGetUp | `SetDownTime` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexFallToDeath | `CalcFall` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `Constructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `CreateNextSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `MakeAbortable` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `Clone` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `Constructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `ControlSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `CreateNextSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `CreateSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `Destructor` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CTaskComplexFleeAnyMeans | `GetTaskType` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `Clone` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `Constructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `ControlSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `CreateNextSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `CreateSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `Destructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeEntity | `GetTaskType` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleePoint | `Clone` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `ComputeTargetPoint` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `Constructor` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `ControlSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `CreateFirstSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `CreateNextSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `CreateSubTask` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `Destructor` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `GetTaskType` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `MakeAbortable` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleePoint | `SetFleePosition` | [Resumen](../reversing/CTasks11/validation-summary.json) |
| CTaskComplexFleeShooting | `Clone` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeShooting | `Constructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeShooting | `ControlSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeShooting | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeShooting | `CreateNextSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeShooting | `Destructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeShooting | `GetTaskType` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFleeShooting | `MakeAbortable` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `Clone` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `Constructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `ControlSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `CreateNextSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `CreateSubTask` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `Destructor` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowLeaderAnyMeans | `GetTaskType` | [Resumen](../reversing/CTasks8a/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `CalcBlendRatio` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `CalcGoToTaskType` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `CanGoStraightThere` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `Clone` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `ComputePathNodes` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `ComputeRoute` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `Constructor` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `ControlSubTask` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `CreateFirstSubTask` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `CreateNextSubTask` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `CreateSubTask` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `Destructor` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `GetLastWaypoint` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `GetNextWaypoint` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `GetSubTaskType` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `GetTaskType` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `MakeAbortable` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `SetTarget` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowNodeRoute | `StopTimer` | [Resumen](../reversing/Route19/validation-summary.json) |
| CTaskComplexFollowPedFootsteps | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexFollowPointRoute | `CalcBlendRatio` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `CalcGoToTaskType` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `Clone` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `Constructor` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `ControlSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `CreateFirstSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `CreateNextSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `CreateTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `GetLastWaypoint` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `GetNextWaypoint` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `GetSubTaskType` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `GetTaskType` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexFollowPointRoute | `MakeAbortable` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangFollower | `SetCanEnterLeadersCar` | [Resumen](../reversing/GroupTasksCompletion/validation-summary.json) |
| CTaskComplexGangLeader | `Clone` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `Constructor` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `ControlSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `CreateFirstSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `CreateNextSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `Destructor` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `DoGangAbuseSpeech` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `GetRandomGangAmbientAnim` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `GetTaskType` | [Resumen](../reversing/GangLeader/validation-summary.json) |
| CTaskComplexGangLeader | `MakeAbortable` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `ScanForStuff` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `ShouldLoadGangAnims` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGangLeader | `TryToPassObject` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `Clone` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `Constructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `ControlSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `CreateNextSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `Destructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `GetTaskType` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetOnBoatSeat | `MakeAbortable` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexGetUpAndStandStill | `Constructor` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexGetUpAndStandStill | `ControlSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexGetUpAndStandStill | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexGetUpAndStandStill | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexGetUpAndStandStill | `CreateSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexGoToBoatSteeringWheel | `Clone` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToBoatSteeringWheel | `Constructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToBoatSteeringWheel | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToBoatSteeringWheel | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToBoatSteeringWheel | `CreateNextSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToBoatSteeringWheel | `Destructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `Clone` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `ComputeRouteToDoor` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `Constructor` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `ControlSubTask` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `CreateFirstSubTask` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `CreateNextSubTask` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `CreateSubTask` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `Destructor` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `GetTaskType` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `IsVehicleInRange` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToCarDoorAndStandStill | `MakeAbortable` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskComplexGoToPointAiming | `Clone` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAiming | `Constructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAiming | `ControlSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAiming | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAiming | `CreateNextSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAiming | `CreateSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAiming | `Destructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAiming | `GetTaskType` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `Clone` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `Constructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `ControlSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `CreateNextSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `GetTaskType` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `GoToPoint` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | `Clone` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | `Constructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | `ControlSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | `CreateNextSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | `Destructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | `GetTaskType` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `Clone` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `Constructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `MakeAbortable` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `StopTimer` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAnyMeans | `Constructor-1` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAnyMeans | `Constructor-2` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAnyMeans | `ControlSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAnyMeans | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAnyMeans | `CreateNextSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAnyMeans | `CreateSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointAnyMeans | `Destructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGoToPointShooting | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexGoToPointShooting | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexGoToPointShooting | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexGotoDoorAndOpen | `Clone` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGotoDoorAndOpen | `Constructor-0` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGotoDoorAndOpen | `Constructor-1` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGotoDoorAndOpen | `ControlSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGotoDoorAndOpen | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGotoDoorAndOpen | `CreateNextSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexGotoDoorAndOpen | `Destructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexHitByGunResponse | `Clone` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitByGunResponse | `Constructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitByGunResponse | `ControlSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitByGunResponse | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitByGunResponse | `CreateNextSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitByGunResponse | `Destructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitByGunResponse | `GetTaskType` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitByGunResponse | `MakeAbortable` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexHitResponse | `Clone` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexHitResponse | `Constructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexHitResponse | `ControlSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexHitResponse | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexHitResponse | `CreateNextSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexHitResponse | `Destructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexHitResponse | `GetTaskType` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInAirAndLand | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexInAirAndLand | `ControlSubTask` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexInAirAndLand | `CreateFirstSubTask` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexInAirAndLand | `CreateNextSubTask` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexInWater | `Clone` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInWater | `Constructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInWater | `ControlSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInWater | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInWater | `CreateNextSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInWater | `Destructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInWater | `GetTaskType` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `Clone` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `ComputeTargetPos` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `Constructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `ControlSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `CreateNextSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `CreateSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `Destructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDeadPed | `GetTaskType` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `Clone` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `Constructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `ControlSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `CreateNextSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `Destructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `GetTaskType` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexInvestigateDisturbance | `MakeAbortable` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexJump | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `CreateNextSubTask` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `CreateSubTask` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `MakeAbortable` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `Clone` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `ComputeNearestLivingGroupPed` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `Constructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `ControlSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `CreateNextSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `CreateSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `Destructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedGroupOnFoot | `GetTaskType` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexKillPedOnFoot | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `CalculateSearchPositionAndRanges` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `Clone` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `Constructor` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `ControlSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `CreateFirstSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `CreateNextSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `CreateSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `Destructor` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `GetTaskType` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootMelee | `MakeAbortable` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexKillPedOnFootStealth | `Clone` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexKillPedOnFootStealth | `Constructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexKillPedOnFootStealth | `ControlSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexKillPedOnFootStealth | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexKillPedOnFootStealth | `CreateNextSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexKillPedOnFootStealth | `Destructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexKillPedOnFootStealth | `GetTaskType` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveAnyCar | `Clone` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveAnyCar | `Constructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveAnyCar | `ControlSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveAnyCar | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveAnyCar | `CreateNextSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveAnyCar | `Destructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveAnyCar | `GetTaskType` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexLeaveBoat | `Clone` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveBoat | `Constructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveBoat | `ControlSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveBoat | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveBoat | `CreateNextSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveBoat | `CreateSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveBoat | `Destructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveBoat | `GetTaskType` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndDie | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexLeaveCarAndDie | `CreateFirstSubTask` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexLeaveCarAndDie | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `Clone` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `Constructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `ControlSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `CreateNextSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `CreateSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `Destructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndFlee | `GetTaskType` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `Clone` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `Constructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `ControlSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `CreateNextSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `CreateSubTask` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `Destructor` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAndWander | `GetTaskType` | [Resumen](../reversing/CTasks8b/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `Clone` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `Constructor` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `ControlSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `CreateFirstSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `CreateNextSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `CreateSubTask` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `Destructor` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `GetNextPassengerToLeave` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `GetTaskType` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexLeaveCarAsPassengerWait | `SetPedInZerothPassengerSeat` | [Resumen](../reversing/CTasks10a/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `CalcTargetPosWithOffset` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `Clone` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `Constructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `ControlSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `CreateDealWithNextAccidentTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `CreateNextSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `CreateSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMedicTreatInjuredPed | `FindNearestAccident` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `Clone` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `Constructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `ControlSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `CreateNextSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `CreateSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `Destructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexMoveBackAndJump | `GetTaskType` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `Clone` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `Constructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `ControlSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `CreateNextSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `Destructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `GetTaskType` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLights | `MakeAbortable` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `ControlSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `MakeAbortable` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexOnFire | `Clone` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `ComputeFireDamage` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `Constructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `ControlSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `CreateNextSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `CreateSubTask` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `Destructor` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOnFire | `GetTaskType` | [Resumen](../reversing/CTasks9b/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `Clone` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `Destructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `GetTaskType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexOpenPassengerDoor | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexOpenPassengerDoor | `Constructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexOpenPassengerDoor | `Destructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexOpenPassengerDoor | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskComplexPartner | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexPartnerChat | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexPartnerDeal | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexPartnerGreet | `Clone` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexPartnerGreet | `Constructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexPartnerGreet | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexPartnerGreet | `Destructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexPartnerGreet | `GetPartnerSequence` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexPartnerGreet | `GetTaskType` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexPartnerGreet | `StreamRequiredAnims` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexPartnerShove | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexPassObject | `AbortIK` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPassObject | `Clone` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPassObject | `Constructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPassObject | `ControlSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPassObject | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPassObject | `CreateNextSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPassObject | `Destructor` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CTaskComplexPassObject | `GetTaskType` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPassObject | `MakeAbortable` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexPolicePursuit | `ClearPursuit` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `Clone` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `Constructor` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `ControlSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `CreateFirstSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `CreateNextSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `CreateSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `Destructor` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `GetTaskType` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `MakeAbortable` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `PersistPursuit` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `SetPursuit` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexPolicePursuit | `SetWeapon` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `Clone` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `Constructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `ControlSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `CreateNextSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `CreateSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `Destructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexReactToGunAimedAt | `GetTaskType` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexRoadRage | `Clone` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `Constructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `ControlSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `CreateNextSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `CreateSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `Destructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `GetTaskType` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexRoadRage | `MakeAbortable` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `Clone` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `Constructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `ControlSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `CreateNextSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `CreateSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `Destructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `GetTaskType` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexScreamInCarThenLeave | `MakeAbortable` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `Clone` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `Constructor_Grp` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `Constructor_Ped` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `ControlSubTask` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `CreateFirstSubTask` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `CreateNextSubTask` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `Destructor` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `GetCoverPed` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `GetTargetPed` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekCoverUntilTargetDead | `GetTaskType` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `Clone` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `Constructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `ControlSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `CreateNextSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `CreateSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `Destructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `GetTaskType` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | `MakeAbortable` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityAiming | `Clone` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSeekEntityAiming | `Constructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSeekEntityAiming | `ControlSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSeekEntityAiming | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSeekEntityAiming | `CreateNextSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSeekEntityAiming | `Destructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSeekEntityAiming | `GetTaskType` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSeekEntityAnyMeansXYOffset | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexSeekEntityAnyMeansXYOffset | `ControlSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexSeekEntityAnyMeansXYOffset | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexSeekEntityAnyMeansXYOffset | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexSeekEntityAnyMeansXYOffset | `GetTaskType` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `Clone` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `Constructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `ControlSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `CreateNextSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `CreateSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `Destructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `GetTaskType` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityRadiusAngleOffset | `MakeAbortable` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `Clone` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `Constructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `ControlSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `CreateNextSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `CreateSubTask` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `Destructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `GetTaskType` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSeekEntityStandard | `MakeAbortable` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskComplexSequence | `AddTask-0` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `AddTask-1` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `Clone` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `Constructor` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `Contains` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `ControlSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `CreateFirstSubTask` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `CreateNextSubTask-Internal` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `CreateNextSubTask-V` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `DeRegister` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `Flush` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `MakeAbortable` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexSequence | `SetCanBeEmptied` | [Resumen](../reversing/CTasks13/validation-summary.json) |
| CTaskComplexShuffleSeats | `Clone` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `Constructor` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `ControlSubTask` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `CreateFirstSubTask` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `CreateNextSubTask` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `CreateSubTask` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `CreateTaskUtilityLineUpPedWithCar` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `Destructor` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `GetTaskType` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexShuffleSeats | `MakeAbortable` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskComplexSignalAtPed | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexSignalAtPed | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexSignalAtPed | `ControlSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexSignalAtPed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexSignalAtPed | `CreateNextSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexSignalAtPed | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexSignalAtPed | `GetTaskType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `Clone` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `Constructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `ControlSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `CreateNextSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `CreateSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `Destructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `GetTaskType` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSitDownThenIdleThenStandUp | `MakeAbortable` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexSmartFleeEntity | `Clone` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSmartFleeEntity | `Constructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSmartFleeEntity | `ControlSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSmartFleeEntity | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSmartFleeEntity | `CreateNextSubTask` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSmartFleeEntity | `Destructor` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexSmartFleeEntity | `GetTaskType` | [Resumen](../reversing/CTasks7b/validation-summary.json) |
| CTaskComplexStareAtPed | `Clone` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStareAtPed | `Constructor` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStareAtPed | `ControlSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStareAtPed | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStareAtPed | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStareAtPed | `Destructor` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStareAtPed | `GetTaskType` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `Clone` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `Constructor` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `ControlSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `CreateSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `Destructor` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStealCar | `GetTaskType` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStuckInAir | `Clone` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStuckInAir | `ControlSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStuckInAir | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStuckInAir | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStuckInAir | `CreateSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexStuckInAir | `GetTaskType` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `CalcMoveRatio` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `CalcTargetPos` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `Clone` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `Constructor` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `ControlSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `Destructor` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `GetTaskType` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `MakeAbortable` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTrackEntity | `SetOffsetPos` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTreatAccident | `Clone` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTreatAccident | `ComputeHeading` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTreatAccident | `Constructor` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTreatAccident | `ControlSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTreatAccident | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTreatAccident | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTreatAccident | `CreateSubTask` | [Resumen](../reversing/TasksBatch/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `Clone` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `ComputeTargetHeading` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `Constructor-Coords` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `Constructor-Entity` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `ControlSubTask` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `Destructor` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexTurnToFaceEntityOrCoord | `GetTaskType` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseAttractor | `Clone` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractor | `Constructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractor | `ControlSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractor | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractor | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractor | `Destructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractor | `GetTaskType` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractorPartner | `Clone` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractorPartner | `Constructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractorPartner | `ControlSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractorPartner | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractorPartner | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractorPartner | `Destructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseAttractorPartner | `GetTaskType` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `Clone` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `ComputeClosestFreeScriptedEffect` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `Constructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `ControlSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `CreateNextSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `Destructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractor | `GetTaskType` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorRun | `Clone` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorRun | `Constructor` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorRun | `Destructor` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorRun | `GetTaskType` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | `Clone` | [Resumen](../reversing/CTaskComplexUseClosestFreeScriptedAttractorSprint/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | `Constructor` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | `Destructor` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | `GetTaskType` | [Resumen](../reversing/CTaskComplexUseClosestFreeScriptedAttractorSprint/validation-summary.json) |
| CTaskComplexUseEffect | `Clone` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffect | `Constructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffect | `ControlSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffect | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffect | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffect | `Destructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffect | `GetTaskType` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffect | `MakeAbortable` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseEffectRunning | `Clone` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseEffectRunning | `GetTaskType` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseEffectSprinting | `Clone` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseEffectSprinting | `GetTaskType` | [Resumen](../reversing/TasksBatch2/validation-summary.json) |
| CTaskComplexUseGoggles | `Clone` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseGoggles | `ControlSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseGoggles | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseGoggles | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseGoggles | `GetTaskType` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `ClearPartnership` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `Clone` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `Constructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `ControlSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `CreateSubTask` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `Destructor` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `GetTaskType` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `SeekPartnership` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUsePairedAttractor | `SetPartnership` | [Resumen](../reversing/TasksBatch3/validation-summary.json) |
| CTaskComplexUseScriptedAttractor | `Clone` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedAttractor | `Constructor` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedAttractor | `ControlSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedAttractor | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedAttractor | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedAttractor | `Destructor` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedAttractor | `GetTaskType` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `Clone` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `Constructor` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `ControlSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `Destructor` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `GetTaskType` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseScriptedBrain | `MakeAbortable` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSequence | `Clone` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSequence | `Constructor` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSequence | `ControlSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSequence | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSequence | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSequence | `MakeAbortable` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSwatRope | `Clone` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSwatRope | `Constructor` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSwatRope | `ControlSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSwatRope | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSwatRope | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSwatRope | `CreateSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseSwatRope | `MakeAbortable` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseWaterCannon | `Clone` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseWaterCannon | `Constructor` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseWaterCannon | `ControlSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseWaterCannon | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexUseWaterCannon | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch4/validation-summary.json) |
| CTaskComplexWaitAtAttractor | `Clone` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitAtAttractor | `Constructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitAtAttractor | `ControlSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitAtAttractor | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitAtAttractor | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitAtAttractor | `Destructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitAtAttractor | `GetTaskType` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `Clone` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `Constructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `ControlSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `CreateSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `Destructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBackup | `GetTaskType` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBus | `Clone` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBus | `Constructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBus | `ControlSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBus | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBus | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBus | `Destructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForBus | `GetTaskType` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForDryWeather | `Clone` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForDryWeather | `Constructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForDryWeather | `ControlSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForDryWeather | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForDryWeather | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForDryWeather | `Destructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWaitForDryWeather | `GetTaskType` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `Clone` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `Constructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `ControlSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `CreateFirstSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `CreateNextSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `CreateSubTask` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `Destructor` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `GetTaskType` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkAlongsidePed | `MakeAbortable` | [Resumen](../reversing/TasksBatch5/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `Clone` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `ComputeCrapRoute` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `ComputeMoveDir` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `ComputeRoute` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `Constructor` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `ControlSubTask` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `CreateFirstSubTask` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `CreateNextSubTask` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `CreateSubTask` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `Destructor` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `GetTaskType` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundBuildingAttempt | `MakeAbortable` | [Resumen](../reversing/Mix12/validation-summary.json) |
| CTaskComplexWalkRoundCar | `Clone` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `ComputeRoute` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `ComputeRouteRoundBigCar` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `ComputeRouteRoundSmallCar` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `Constructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `ControlSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `CreateNextSubTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `CreateRouteTask` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `Destructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `GetTaskType` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `GoingForDoor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundCar | `SetNewVehicle` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `Clone` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `ComputeDetourTarget` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `Constructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `ControlSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `CreateFirstSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `CreateNextSubTask` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `Destructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundFire | `GetTaskType` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskComplexWalkRoundObject | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexWander | `ComputeTargetHeading` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `ComputeTargetPos` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `Constructor` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `ControlSubTask` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `CreateFirstSubTask` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `CreateNextSubTask` | [Resumen](../reversing/CTaskComplexWander/validation-summary.json) |
| CTaskComplexWander | `CreateSubTask` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `GetWanderTaskByPedType` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `ScanForBlockedNode-` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `ScanForBlockedNode-1` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `ScanForBlockedNodes` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `UpdateDir` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `UpdatePathNodes` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWander | `ValidNodes` | [Resumen](../reversing/CTasks14a/validation-summary.json) |
| CTaskComplexWanderCop | `Constructor` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `ControlSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `CreateFirstSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `CreateNextSubTask` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `LookForCarAlarms` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `LookForCriminals` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `LookForStolenCopCars` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `ScanForStuff` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCop | `ShouldPursuePlayer` | [Resumen](../reversing/CTasks9c/validation-summary.json) |
| CTaskComplexWanderCriminal | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderCriminal | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderCriminal | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderCriminal | `GetWanderType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderCriminal | `LookForCarsToSteal` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderCriminal | `ScanForStuff` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderFlee | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexWanderFlee | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexWanderFlee | `ControlSubTask` | [Resumen](../reversing/FixHooks/validation-summary.json) |
| CTaskComplexWanderFlee | `Destructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexWanderFlee | `GetWanderType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexWanderFlee | `ScanForStuff` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexWanderGang | `Clone` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderGang | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderGang | `GetWanderType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderGang | `ScanForStuff` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderMedic | `Clone` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderMedic | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderMedic | `GetWanderType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderMedic | `ScanForStuff` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexWanderProstitute | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderProstitute | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderProstitute | `CreateFirstSubTask` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderProstitute | `GetWanderType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskComplexWanderProstitute | `ScanForStuff` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskInteriorBeInHouse | `Clone` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInHouse | `Constructor` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInHouse | `ControlSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInHouse | `CreateFirstSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInHouse | `CreateNextSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInHouse | `Destructor` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInHouse | `GetInfoForPedToUse` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInHouse | `GetTaskType` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `Clone` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `Constructor` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `ControlSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `CreateFirstSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `CreateNextSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `Destructor` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `GetInfoForPedToUse` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInOffice | `GetTaskType` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorBeInShop | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskInteriorBeInShop | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskInteriorBeInShop | `Destructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskInteriorBeInShop | `GetTaskType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskInteriorBeInShop | `GetWanderType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskInteriorBeInShop | `ScanForStuff` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskInteriorGoToInfo | `Clone` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorGoToInfo | `Constructor` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorGoToInfo | `ControlSubTask` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorGoToInfo | `CreateFirstSubTask` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorGoToInfo | `CreateNextSubTask` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorGoToInfo | `Destructor` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorGoToInfo | `GetTaskType` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorLieInBed | `Clone` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorLieInBed | `Constructor` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CTaskInteriorLieInBed | `Destructor` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorLieInBed | `FinishAnimCB` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorLieInBed | `GetTaskType` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorLieInBed | `MakeAbortable` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorLieInBed | `ProcessPed` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorShopKeeper | `Clone` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorShopKeeper | `Constructor` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorShopKeeper | `ControlSubTask` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorShopKeeper | `CreateFirstSubTask` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorShopKeeper | `CreateNextSubTask` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorShopKeeper | `Destructor` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorShopKeeper | `GetTaskType` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTaskInteriorSitAtDesk | `Clone` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `Constructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `Destructor` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `FinishAnimCB` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `GetTaskType` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `MakeAbortable` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `ProcessPed` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `StartRandomLoopAnim` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorSitAtDesk | `StartRandomOneOffAnim` | [Resumen](../reversing/CTasks9d/validation-summary.json) |
| CTaskInteriorUseInfo | `Clone` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorUseInfo | `Constructor` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorUseInfo | `ControlSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorUseInfo | `CreateFirstSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorUseInfo | `CreateNextSubTask` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorUseInfo | `Destructor` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorUseInfo | `GetTaskType` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskInteriorUseInfo | `MakeAbortable` | [Resumen](../reversing/Fx8/validation-summary.json) |
| CTaskLeanOnVehicle | `Constructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskLeanOnVehicle | `Destructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskLeanOnVehicle | `FinishAnimCB` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskLeanOnVehicle | `MakeAbortable` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskLeanOnVehicle | `ProcessPed` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskManager | `AddSubTasks` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `ClearTaskEventResponse` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `Constructor` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `Destructor` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `FindActiveTaskByType` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `FindTaskByType` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `Flush` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `FlushImmediately` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `GetActiveTask` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `GetSimplestActiveTask` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `GetTaskSecondary` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CTaskManager | `HasTaskSecondary` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `ManageTasks` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `ParentsControlChildren` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `SetNextSubTask` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `SetTask` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `SetTaskSecondary` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskManager | `StopTimers` | [Resumen](../reversing/Tasks18/validation-summary.json) |
| CTaskPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CTaskSequences | `CleanUpForShutdown` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSequences | `Init` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimpleAchieveHeading | `Clone` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAchieveHeading | `GetTaskType` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAchieveHeading | `MakeAbortable` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAchieveHeading | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAchieveHeading | `SetUpIK` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAffectSecondaryBehaviour | `Clone` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAffectSecondaryBehaviour | `Constructor` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAffectSecondaryBehaviour | `GetTaskType` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAffectSecondaryBehaviour | `MakeAbortable` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAffectSecondaryBehaviour | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleAnim | `MakeAbortable` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskSimpleArrestPed | `MakeAbortable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleArrestPed | `ProcessPed` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleArrestPed | `StartAnim` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleBeHit | `Clone` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBeHit | `Constructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBeHit | `Destructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBeHit | `FinishAnimBeHitCB` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBeHit | `GetTaskType` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBeHit | `MakeAbortable` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBeHit | `ProcessPed` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBeHit | `StartAnim` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `Clone` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `Constructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `Destructor` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `FinishAnimBikeHitCB` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `GetTaskType` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `MakeAbortable` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `ProcessPed` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleBikeJacked | `SetPedPosition` | [Resumen](../reversing/CTasks8c/validation-summary.json) |
| CTaskSimpleCarAlign | `Clone` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `Constructor` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `Destructor` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `FinishAnimCarAlignCB` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `FixHeading` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `GetTaskType` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `MakeAbortable` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `ProcessPed` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `SetPedPosition` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarAlign | `StartAnim` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `Clone` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `ComputeAnimID` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `Constructor` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `Destructor` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `FinishAnimCarCloseDoorFromInsideCB` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `GetTaskType` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `MakeAbortable` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `ProcessPed` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `SetPedPosition` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromInside | `StartAnim` | [Resumen](../reversing/CTasks10b/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `Clone` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `ComputeAnimID` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `Constructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `Destructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `FinishAnimCarCloseDoorFromOutsideCB` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `GetTaskType` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `MakeAbortable` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `ProcessPed` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `SetPedPosition` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarCloseDoorFromOutside | `StartAnim` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarDrive | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `Constructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `Destructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `MakeAbortable` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `ProcessArmBopping` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `ProcessBopping` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `ProcessHeadBopping` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `ProcessPed` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `SetPedPosition` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `StartBopping` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `TriggerIK` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDrive | `UpdateBopping` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDriveTimed | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDriveTimed | `Constructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDriveTimed | `Destructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDriveTimed | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarDriveTimed | `ProcessPed` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarFallOut | `Clone` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `Constructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `Destructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `FinishAnimFallOutCB` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `GetTaskType` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `MakeAbortable` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `ProcessPed` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `SetPedPosition` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarFallOut | `StartAnim` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `Clone` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `Constructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `Destructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `FinishAnimCarGetInCB` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `GetTaskType` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `MakeAbortable` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `ProcessPed` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `SetPedPosition` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetIn | `StartAnim` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarGetOut | `Clone` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `ComputeAnimID_ToHook` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `Constructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `Destructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `FinishAnimCarGetOutCB` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `GetTaskType` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `MakeAbortable` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `ProcessPed` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `SetPedPosition` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGetOut | `StartAnim` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `Destructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `GetTaskType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `MakeAbortable` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `ProcessPed` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarJumpOut | `Clone` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `ComputeAnimID_ToHook` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `Constructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `Destructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `FinishAnimCarJumpOutCB` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `GetTaskType` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `MakeAbortable` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `ProcessPed` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `SetPedPosition` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarJumpOut | `StartAnim` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `Clone` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `ComputeAnimID_ToHook` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `Constructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `Destructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `FinishAnimCarOpenDoorFromOutsideCB` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `GetTaskType` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `MakeAbortable` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `ProcessPed` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `SetPedPosition` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenDoorFromOutside | `StartAnim` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `Clone` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `Constructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `Destructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `FinishAnimCarOpenLockedDoorFromOutsideCB` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `GetTaskType` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `MakeAbortable` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `ProcessPed` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `SetPedPosition` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarOpenLockedDoorFromOutside | `StartAnim` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSetPedInAsDriver | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsDriver | `Constructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsDriver | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsDriver | `MakeAbortable` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsDriver | `ProcessPed` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsPassenger | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsPassenger | `Destructor` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsPassenger | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsPassenger | `MakeAbortable` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedInAsPassenger | `ProcessPed` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedOut | `Clone` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedOut | `GetTaskType` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedOut | `MakeAbortable` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedOut | `PositionPedOutOfCollision` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedOut | `ProcessPed` | [Resumen](../reversing/CTasks61a/validation-summary.json) |
| CTaskSimpleCarSetPedSlowDraggedOut | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetPedSlowDraggedOut | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetPedSlowDraggedOut | `Destructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetPedSlowDraggedOut | `GetTaskType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetPedSlowDraggedOut | `MakeAbortable` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetPedSlowDraggedOut | `ProcessPed` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetTempAction | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetTempAction | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetTempAction | `Destructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetTempAction | `GetTaskType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetTempAction | `MakeAbortable` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarSetTempAction | `ProcessPed` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarShuffle | `Clone` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `Constructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `Destructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `FinishAnimCarShuffleCB` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `GetTaskType` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `MakeAbortable` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `ProcessPed` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `SetPedPosition` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarShuffle | `StartAnim` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `Clone` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `Constructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `Destructor` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `FinishAnimCarSlowBeDraggedOutCB` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `GetTaskType` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `MakeAbortable` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `ProcessPed` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `SetPedPosition` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowBeDraggedOut | `StartAnim` | [Resumen](../reversing/STasks9/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `Clone` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `ComputeAnimID_Wrapper` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `Constructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `Destructor` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `FinishAnimCarSlowDragPedOutCB` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `GetTaskType` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `MakeAbortable` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `ProcessPed` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `SetPedPosition` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarSlowDragPedOut | `StartAnim` | [Resumen](../reversing/STasks10/validation-summary.json) |
| CTaskSimpleCarWaitForDoorNotToBeInUse | `Clone` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleCarWaitForDoorNotToBeInUse | `Constructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleCarWaitForDoorNotToBeInUse | `Destructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleCarWaitForDoorNotToBeInUse | `GetTaskType` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleCarWaitForDoorNotToBeInUse | `MakeAbortable` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleCarWaitForDoorNotToBeInUse | `ProcessPed` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleCarWaitForDoorNotToBeInUse | `SetPedPosition` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleCarWaitToSlowDown | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarWaitToSlowDown | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarWaitToSlowDown | `Destructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarWaitToSlowDown | `GetTaskType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarWaitToSlowDown | `MakeAbortable` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarWaitToSlowDown | `ProcessPed` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleChat | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleChat | `Destructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleChat | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleChoking | `Clone` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskSimpleChoking | `Constructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskSimpleChoking | `Destructor` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskSimpleChoking | `GetTaskType` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskSimpleChoking | `MakeAbortable` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskSimpleChoking | `ProcessPed` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskSimpleChoking | `UpdateChoke` | [Resumen](../reversing/CTasks61b/validation-summary.json) |
| CTaskSimpleClearLookAt | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `Destructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `GetTaskType` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClimb | `Constructor` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `CreateColModel` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `DeleteAnimCB` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `GetCameraStickModifier` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `GetCameraTargetPos` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `MakeAbortable` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `ProcessPed` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `ScanToGrab` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `ScanToGrabSectorList<CPtrListSingleLink<CPhysical*>>` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `Shutdown` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `StartAnim` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `StartSpeech` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `TestForClimb` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `TestForStandUp` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleClimb | `TestForVault` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleCower | `Clone` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleCower | `Constructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleCower | `Destructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleCower | `IsInterruptable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleCreateCarAndGetIn | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleCreateCarAndGetIn | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleCreateCarAndGetIn | `Destructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleCreateCarAndGetIn | `GetTaskType` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleCreateCarAndGetIn | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleCreateCarAndGetIn | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDie | `Constructor-1` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleDie | `Constructor-2` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleDie | `Constructor-3` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleDie | `FinishAnimDieCB` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleDie | `MakeAbortable` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleDie | `ProcessPed` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleDie | `StartAnim` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleDieInCar | `Clone` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleDieInCar | `Constructor` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleDieInCar | `Destructor` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleDieInCar | `GetTaskType` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleDieInCar | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleDoHandSignal | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDoHandSignal | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDoHandSignal | `Destructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDoHandSignal | `GetTaskType` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDoHandSignal | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDoHandSignal | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDrown | `ProcessPed` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskSimpleDrownInCar | `ProcessPed` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskSimpleDuck | `CanPedDuck` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `Clone` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `Constructor` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `ControlDuckMove` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `DeleteDuckAnimCB` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `Destructor` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `ForceStopMove` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `GetTaskType` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `IsTaskInUseByOtherTasks` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `MakeAbortable` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `ProcessPed` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `RestartTask` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `SetControlType` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `SetDuckTimer` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuck | `SetMoveAnim` | [Resumen](../reversing/CTasks15/validation-summary.json) |
| CTaskSimpleDuckToggle | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `Destructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `GetTaskType` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `Clone` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `Constructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `Destructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `FinishAnimEvasiveDiveCB` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `GetTaskType` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `MakeAbortable` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `ProcessPed` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveDive | `StartAnim` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `Clone` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `Constructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `Destructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `FinishAnimEvasiveStepCB` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `GetTaskType` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `MakeAbortable` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `ProcessPed` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleEvasiveStep | `StartAnim` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleFacial | `Clone` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleFacial | `GetAnimId` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleFacial | `GetTaskType` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleFacial | `MakeAbortable` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleFacial | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleFall | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleFall | `FinishFallAnimCB` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleFall | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleFall | `ProcessFall` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleFall | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleFall | `StartAnim` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleFinishBrain | `Clone` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimpleFinishBrain | `GetTaskType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimpleGetUp | `Constructor` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleGetUp | `FinishGetUpAnimCB` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleGetUp | `MakeAbortable` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleGetUp | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleGetUp | `StartAnim` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleGiveCPR | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGiveCPR | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGiveCPR | `FinishGiveCPRAnimCB` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGiveCPR | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGiveCPR | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGiveCPR | `ReviveDeadPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGoTo | `Constructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleGoTo | `HasCircledTarget` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleGoTo | `QuitIK` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleGoTo | `SetUpIK` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleGoToPoint | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGoToPoint | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGoToPoint | `GetTaskType` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGoToPoint | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGoToPoint | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGoToPoint | `UpdatePoint` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `BaseRatio` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `Clone` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `Constructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `Finish` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `MakeAbortable` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `ProcessPed` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `SetBlendedMoveAnim` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGoToPointFine | `SetTargetPos` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleGunControl | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleGunControl | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleGunControl | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleGunControl | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleGunControl | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleGunControl | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHandsUp | `Clone` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHandsUp | `Constructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHandsUp | `Destructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHandsUp | `IsInterruptable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHitByGunFromFront | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromFront | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromFront | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromLeft | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromLeft | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromLeft | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromRear | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromRear | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromRear | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromRight | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromRight | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitByGunFromRight | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromBack | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromBack | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromBack | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromBehind | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromBehind | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromBehind | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromFront | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromFront | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromFront | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromLeft | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromLeft | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromLeft | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromRight | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromRight | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitFromRight | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitHead | `Constructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHitHead | `FinishAnimCB` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHitHead | `MakeAbortable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHitHead | `ProcessPed` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleHitWall | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitWall | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHitWall | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleHoldEntity | `CanThrowEntity` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `ChoosePutDownHeight` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `Clone` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `Constructor-1` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `Constructor-2` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `Constructor-3` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `DropEntity` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `FinishAnimHoldEntityCB` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `MakeAbortable` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `PlayAnim` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `ProcessPed` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `ReleaseEntity` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `SetPedPosition` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHoldEntity | `StartAnim` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleIKChain | `BlendOut` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `Clone` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `Constructor` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `CreateIKChain` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `Destructor` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `GetIKChain` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `GetTaskType` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `MakeAbortable` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKChain | `ProcessPed` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleIKLookAt | `Clone` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKLookAt | `Constructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKLookAt | `CreateIKChain` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKLookAt | `Destructor-` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKLookAt | `GetLookAtEntity` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKLookAt | `GetLookAtOffset` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKLookAt | `GetTaskType` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKLookAt | `UpdateLookAtInfo` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `AddIKChainTask` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `Clone` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `Constructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `Destructor` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `GetTaskAtSlot` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `GetTaskType` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `MakeAbortable` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKManager | `ProcessPed` | [Resumen](../reversing/STasks8a/validation-summary.json) |
| CTaskSimpleIKPointArm | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleIKPointArm | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleIKPointArm | `CreateIKChain` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleIKPointArm | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleIKPointArm | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleIKPointArm | `UpdatePointArmInfo` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleInAir | `Constructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleInAir | `DeleteAnimCB` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleInAir | `MakeAbortable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleInAir | `ProcessPed` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleJetPack | `ApplyRollAndPitch` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `Clone` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `Constructor` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `Destructor` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `DoJetPackEffect` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `DropJetPack` | [Resumen](../reversing/CTaskSimpleJetPack/validation-summary.json) |
| CTaskSimpleJetPack | `GetTaskType` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `MakeAbortable` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `ProcessAnims` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `ProcessControlInput` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `ProcessPed` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `ProcessThrust` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `RenderJetPack` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJetPack | `StopJetPackEffect` | [Resumen](../reversing/CTasks14b/validation-summary.json) |
| CTaskSimpleJump | `CheckIfJumpBlocked` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleJump | `Clone` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleJump | `Constructor` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleJump | `JumpAnimFinishCB` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleJump | `Launch` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleJump | `MakeAbortable` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleJump | `ProcessPed` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleJump | `StartLaunchAnim` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleKillPedWithCar | `Clone` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleKillPedWithCar | `Constructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleKillPedWithCar | `DamageCarBonnet` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleKillPedWithCar | `Destructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleKillPedWithCar | `GetTaskType` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleKillPedWithCar | `MakeAbortable` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleKillPedWithCar | `ProcessPed` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleLand | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleLand | `FinishAnimCB` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleLand | `LeftFootLanded` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleLand | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleLand | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleLand | `RightFootLanded` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleLeaveGroup | `Constructor` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimpleLeaveGroup | `Destructor` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimpleLookAbout | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleLookAbout | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleLookAbout | `IsInterruptable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleNone | `Clone` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleNone | `GetTaskType` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleNone | `MakeAbortable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleNone | `ProcessPed` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePause | `Clone` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePause | `Constructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePause | `MakeAbortable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePause | `ProcessPed` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePickUpBike | `Clone` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `Constructor` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `Destructor` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `FinishAnimPickUpBikeCB` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `GetTaskType` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `MakeAbortable` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `ProcessPed` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `SetPedPosition` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpBike | `StartAnim` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimplePickUpEntity | `Clone` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePickUpEntity | `Constructor-1` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePickUpEntity | `Constructor-2` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePickUpEntity | `GetTaskType` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimplePlayHandSignalAnim | `ProcessPed` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimplePlayHandSignalAnim | `StartAnim` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimplePlayerOnFire | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimplePlayerOnFire | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimplePlayerOnFire | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimplePlayerOnFire | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimplePlayerOnFire | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimplePlayerOnFire | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimplePutDownEntity | `Clone` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimplePutDownEntity | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimplePutDownEntity | `GetTaskType` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleRunAnim | `Clone` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleRunAnim | `Constructor` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleRunAnim | `Constructor2` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleRunAnim | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleRunAnim | `StartAnim` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleRunNamedAnim | `Clone` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskSimpleRunNamedAnim | `Constructor-Anim` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskSimpleRunNamedAnim | `Constructor-Default` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskSimpleRunNamedAnim | `Destructor` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskSimpleRunNamedAnim | `GetTaskType` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskSimpleRunNamedAnim | `OffsetPedPosition` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskSimpleRunNamedAnim | `ProcessPed` | [Resumen](../reversing/CTasks61c/validation-summary.json) |
| CTaskSimpleRunTimedAnim | `Clone` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleRunTimedAnim | `Constructor-1` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleRunTimedAnim | `Constructor-2` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleRunTimedAnim | `Destructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleRunTimedAnim | `GetTaskType` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleRunTimedAnim | `ProcessPed` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleRunTimedAnim | `StartAnim` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSay | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSay | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSay | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSay | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSay | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSay | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleScratchHead | `Clone` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimpleScratchHead | `IsInterruptable` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTaskSimpleSetCharDecisionMaker | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharDecisionMaker | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharDecisionMaker | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharDecisionMaker | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharDecisionMaker | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharDecisionMaker | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleSetKindaStayInSamePlace | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetKindaStayInSamePlace | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetKindaStayInSamePlace | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetKindaStayInSamePlace | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetKindaStayInSamePlace | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetKindaStayInSamePlace | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetPedAsAutoDriver | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetPedAsAutoDriver | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetPedAsAutoDriver | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetPedAsAutoDriver | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetPedAsAutoDriver | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetPedAsAutoDriver | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleSetStayInSamePlace | `Clone` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleSetStayInSamePlace | `Constructor` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleSetStayInSamePlace | `GetTaskType` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleSetStayInSamePlace | `MakeAbortable` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleSetStayInSamePlace | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleShakeFist | `Clone` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleShakeFist | `Constructor` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleShakeFist | `Destructor` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleShakeFist | `FinishAnimShakeFistCB` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleShakeFist | `GetTaskType` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleShakeFist | `MakeAbortable` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleShakeFist | `ProcessPed` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleShakeFist | `StartAnim` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `Clone` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `Constructor` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `Destructor` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `FinishAnimSitDownCB` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `GetTaskType` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `MakeAbortable` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `ProcessPed` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitDown | `StartAnim` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleSitIdle | `Clone` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSitIdle | `Constructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSitIdle | `Destructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSitIdle | `GetTaskType` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSitIdle | `MakeAbortable` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSitIdle | `ProcessPed` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSitIdle | `StartAnim` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStandStill | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleStandStill | `MakeAbortable` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleStandStill | `ProcessPed` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleStandUp | `Clone` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStandUp | `Constructor` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStandUp | `Destructor` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStandUp | `FinishAnimStandUpCB` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStandUp | `GetTaskType` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStandUp | `MakeAbortable` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStandUp | `ProcessPed` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStandUp | `StartAnim` | [Resumen](../reversing/STasks8b/validation-summary.json) |
| CTaskSimpleStealthKill | `Clone` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `Constructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `FinishAnimStealthKillCB` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `GetTaskType` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `MakeAbortable` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `ManageAnim` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `ProcessPed` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleSwim | `ApplyRollAndPitch` | [Resumen](../reversing/FixCore/validation-summary.json) |
| CTaskSimpleSwim | `Constructor` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `CreateFxSystem` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `DestroyFxSystem` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `MakeAbortable` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `ProcessControlAI` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `ProcessControlInput` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `ProcessEffects` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `ProcessPed` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `ProcessSwimAnims` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleSwim | `ProcessSwimmingResistance` | [Resumen](../reversing/Mix11/validation-summary.json) |
| CTaskSimpleThrowControl | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowProjectile | `Clone` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `Constructor` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `ControlThrow` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `Destructor` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `FinishAnimThrowProjectileCB` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `GetTaskType` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `MakeAbortable` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `ProcessPed` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleThrowProjectile | `StartAnim` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CTaskSimpleTired | `Constructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleTired | `Destructor` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleTired | `StartAnim` | [Resumen](../reversing/SimpleTasks52/validation-summary.json) |
| CTaskSimpleTogglePedThreatScanner | `Clone` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleTogglePedThreatScanner | `Constructor` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleTogglePedThreatScanner | `GetTaskType` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleTogglePedThreatScanner | `MakeAbortable` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleTogglePedThreatScanner | `ProcessPed` | [Resumen](../reversing/STasks43/validation-summary.json) |
| CTaskSimpleTurn180 | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleTurn180 | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleTurn180 | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleTurn180 | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleTurn180 | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleTurn180 | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleUninterruptable | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskSimpleUseAtm | `Clone` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleUseAtm | `Constructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleUseAtm | `Destructor` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleUseAtm | `IsInterruptable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleUseGun | `AbortIK` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `AimGun` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `ClearAnim` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `Clone` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `ControlGun` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `ControlGunMove` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `FinishGunAnimCB` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `FireGun` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `GetTaskType` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `MakeAbortable` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `PlayerPassiveControlGun` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `ProcessPed` | [Resumen](../reversing/CTaskSimpleUseGun/validation-summary.json) |
| CTaskSimpleUseGun | `RemoveStanceAnims` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `RequirePistolWhip` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `Reset` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `SetMoveAnim` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `SetPedPosition` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `StartAnim` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleUseGun | `StartCountDown` | [Resumen](../reversing/Gun19/validation-summary.json) |
| CTaskSimpleWaitForBus | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitForBus | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitForBus | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitForBus | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitForBus | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitForBus | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilAreaCodesMatch | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilAreaCodesMatch | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilAreaCodesMatch | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilAreaCodesMatch | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilAreaCodesMatch | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilAreaCodesMatch | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | `Clone` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | `GetTaskType` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | `MakeAbortable` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | `ProcessPed` | [Resumen](../reversing/STasks19/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsInCar | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsInCar | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsInCar | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsInCar | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsInCar | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsInCar | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsOutCar | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsOutCar | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsOutCar | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsOutCar | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsOutCar | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleWaitUntilPedIsOutCar | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskTimer | `IsOutOfTime` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CTempColModels | `Initialise` | [Resumen](../reversing/Core70/validation-summary.json) |
| CTempColModels | `Shutdown` | [Resumen](../reversing/Core70/validation-summary.json) |
| CText | `Constructor` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `Destructor` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `Get` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `GetNameOfLoadedMissionText` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `Load` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `LoadMissionPackText` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `LoadMissionText` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `ReadChunkHeader` | [Resumen](../reversing/Models9/validation-summary.json) |
| CText | `Unload` | [Resumen](../reversing/Models9/validation-summary.json) |
| CTheCarGenerators | `CreateCarGenerator` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Get` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Init` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Load` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Process` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `RemoveCarGenerators` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Save` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheZones | `AssignZoneInfoForThisZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `Calc2DDistanceBetween2Zones` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `CreateZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `FillZonesWithGangColours` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `FindSmallestZoneForPosition` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `FindZone-` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `FindZoneByLabel` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `GetLevelFromPosition` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `GetMapZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `GetNavigationZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `GetZoneInfo-` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `Init` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `InitZonesPopulationSettings` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `Load` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `PointLiesWithinZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `PostZoneCreation` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `ResetZonesRevealed` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `Save` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `SetZoneRadarColours` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `Update` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTheZones | `ZoneIsEntirelyContainedWithinOtherZone` | [Resumen](../reversing/Zones21/validation-summary.json) |
| CTimeCycle | `AddOne` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `CalcColoursForPoint` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `FindFarClipForCoors` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `FindTimeCycleBox` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientBlue` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientBlue_BeforeBrightness` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientBlue_Obj` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientGreen` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientGreen_BeforeBrightness` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientGreen_Obj` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientRed` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientRed_BeforeBrightness` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `GetAmbientRed_Obj` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `InitForRestart` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `Initialise` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `SetConstantParametersForPostFX` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `Shutdown` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `StartExtraColour` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `StopExtraColour` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeCycle | `Update` | [Resumen](../reversing/Time20/validation-summary.json) |
| CTimeInfo | `FindOtherTimeModel` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `GetOtherTimeModel` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `GetTimeOff` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `GetTimeOn` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `SetOtherTimeModel` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `SetTimes` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeModelInfo | `GetModelType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTimeModelInfo | `GetTimeInfo` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTimer | `EndUserPause` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `GetCurrentTimeInCycles` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `GetCyclesPerFrame` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `GetCyclesPerMillisecond` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `GetIsSlowMotionActive` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `Initialise` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `Resume` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `Shutdown` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `StartUserPause` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `Stop` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `Suspend` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `Update` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTimer | `UpdateVariables` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrafficLights | `DisplayActualLight` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `FindOrientationForTrafficLightType` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `FindTrafficLightType` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `FindTrafficLightTypeFromOrientation` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `IsMITrafficLight` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `LightForCars1` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `LightForCars1_Visual` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `LightForCars2` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `LightForCars2_Visual` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `LightForPeds` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `ShouldCarStopForBridge` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrafficLights | `ShouldCarStopForLight` | [Resumen](../reversing/World12/validation-summary.json) |
| CTrailer | `BreakTowLink` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `Constructor` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `GetTowBarPos` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `GetTowHitchPos` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `PreRender` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `ProcessAI` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `ProcessControl` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `ProcessEntityCollision` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `ProcessSuspension` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `ResetSuspension` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `ScanForTowLink` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `SetTowLink` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTrailer | `SetupSuspensionLines` | [Resumen](../reversing/Sys13/validation-summary.json) |
| CTxdStore | `AddRef` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `AddTxdSlot` | [Resumen](../reversing/FixCore/validation-summary.json) |
| CTxdStore | `Create` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `FindTxdSlot-hash` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `FindTxdSlot-name` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `FinishLoadTxd` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `GameShutdown` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `GetNumRefs` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `GetParentTxdSlot` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `GetTxd` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `Initialise` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `LoadTxd-filename` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `LoadTxd-stream` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `PopCurrentTxd` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `PushCurrentTxd` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `RemoveIfRefCountIsGreaterThanOne` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `RemoveRef` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `RemoveRefWithoutDelete` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `RemoveTxd` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `RemoveTxdSlot` | [Resumen](../reversing/FixCore/validation-summary.json) |
| CTxdStore | `SetCurrentTxd` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `SetupTxdParent` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `Shutdown` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `StartLoadTxd` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `TxdStoreFindCB` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CTxdStore | `TxdStoreLoadCB` | [Resumen](../reversing/Txd26/validation-summary.json) |
| CUpsideDownCarCheck | `AddCarToCheck` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `HasCarBeenUpsideDownForAWhile` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `IsCarUpsideDown-vehicle` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `RemoveCarFromCheck` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `UpdateTimers` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUserDisplay | `Init` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CUserDisplay | `Process` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CVehicle | `ActivateBomb` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `ActivateBombWhenEntered` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `AddDamagedVehicleParticles` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `AddExhaustParticles` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `AddPassenger-Auto-Seat` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `AddPassenger-Fixed-Seat` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `AddWaterSplashParticles` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `AddWheelDirtAndWater` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `ApplyBoatWaterResistance` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `BladeColSectorList<CPtrListSingleLink<CEntity*>>` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `CalculateLightingFromCollision` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `CanBeDeleted` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanBeDriven` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanDoorsBeDamaged` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanPedEnterCar` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanPedJumpOutCar` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanPedLeanOut` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanPedOpenLocks` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanPedStepOutCar` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CanVehicleBeDamaged` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `CarHasRoof` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `ChangeLawEnforcerState` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `ClearGettingInFlags` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `ClearGettingOutFlags` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `ClearVehicleUpgradeFlags` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `ClearWindowOpenFlag` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `Constructor` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `CopyObjectsCB` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `CreateReplacementAtomic` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `CreateUpgradeAtomic` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `CustomCarPlate_TextureCreate` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `CustomCarPlate_TextureDestroy` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `DeleteRwObject` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `DestroyVehicleAndDriverAndPassengers` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `Destructor` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `DoBladeCollision` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `DoBoatSplashes` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `DoFixedMachineGuns` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `DoHeadLightBeam` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `DoHeadLightReflection` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `DoHeadLightReflectionSingle` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `DoHeadLightReflectionTwin` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `DoPlaneGunFireFX` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `ExtinguishCarFire` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `FillVehicleWithPeds` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `FindTyreNearestPoint` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `FirePlaneGuns` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `FireUnguidedMissile` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetFiringRateMultiplier` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetHeightAboveRoad` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `GetPlaneGunsAutoAimAngle` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetPlaneGunsPosition` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetPlaneGunsRateOfFire` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetPlaneNumGuns` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetPlaneOrdnancePosition` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetPlaneOrdnanceRateOfFire` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetRemapIndex` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `GetReplacementUpgrade` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `GetRopeHeightForHeli` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `GetSpareHasslePosId` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `GetSpecialColModel` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `GetTowBarPos` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `GetTowHitchPos` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `GetUpgrade` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `GetVehicleAppearance` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `GetVehicleLightsStatus` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `HeightAboveCeiling` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `InitWinch` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `IsDriver-ModelID` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsDriver-Ped` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsLawEnforcementVehicle` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsOnItsSide` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsPassenger-ModelID` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsPassenger-Ped` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsSphereTouchingVehicle` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `IsUpsideDown` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsVehicleNormal` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `IsVehiclePointerValid` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `KillPedsGettingInVehicle` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `KillPedsInVehicle` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `Load` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `MakeDirty` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `PickRandomPassenger` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `PickUpEntityWithWinch` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `PreRender` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `PreRenderDriverAndPassengers` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `ProcessBoatControl` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `ProcessCarAlarm` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `ProcessDelayedExplosion` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `ProcessDrivingAnims` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `ProcessOpenDoor` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `ProcessWheel` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `ProcessWheelRotation` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `QueryPickedUpEntityWithWinch` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `ReactToVehicleDamage` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `ReleasePickedUpEntityWithWinch` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `RemoveAllUpgrades` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `RemoveAllUpgradesCB` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `RemoveDriver` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `RemoveLighting` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `RemoveObjectsCB-Frame` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `RemoveObjectsCB-Object` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `RemovePassenger` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `RemoveReplacementUpgrade` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `RemoveUpgrade` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `RemoveWinch` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `Render` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `RenderDriverAndPassengers` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `ResetAfterRender` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `Save` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `SelectPlaneWeapon` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `SetCollisionLighting` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `SetComponentAtomicAlpha` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `SetComponentRotation` | [Resumen](../reversing/FixCore/validation-summary.json) |
| CVehicle | `SetComponentVisibility` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `SetDriver` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `SetFiringRateMultiplier` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `SetGettingInFlags` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `SetGettingOutFlags` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `SetHasslePosId` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `SetModelIndex` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `SetRemap` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `SetRopeHeightForHeli` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `SetTransmissionRotation` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `SetUpDriver` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `SetVehicleAtomicVisibilityCB-Frame` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `SetVehicleAtomicVisibilityCB-Object` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `SetVehicleCreatedBy` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `SetVehicleUpgradeFlags` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `SetWindowOpenFlag` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicle | `SetupLighting` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `SetupPassenger` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `SetupRender` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `SetupUpgradesAfterLoad` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `Shutdown` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `SpecialEntityCalcCollisionSteps` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `SpecialEntityPreCollisionStuff` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `UpdateClumpAlpha` | [Resumen](../reversing/CVehC/validation-summary.json) |
| CVehicle | `UpdateLightingFromStoredPolys` | [Resumen](../reversing/CVehD/validation-summary.json) |
| CVehicle | `UpdatePassengerList` | [Resumen](../reversing/CVehB/validation-summary.json) |
| CVehicle | `UpdateWinch` | [Resumen](../reversing/CVehE/validation-summary.json) |
| CVehicle | `UsesSiren` | [Resumen](../reversing/CVehA/validation-summary.json) |
| CVehicleAnimGroup | `ComputeAnimDoorOffsets` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroup | `ComputeCriticalBlendTime` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroup | `CopyAnimGroup` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroup | `GetGroup` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroup | `InitAnimGroup` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroupData | `GetAnimDoorOffset` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroupData | `GetGroupForAnim` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroupData | `GetInOutTimings` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroupData | `UsesKartDrivingAnims` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehicleAnimGroupData | `UsesTruckDrivingAnims` | [Resumen](../reversing/Events43/validation-summary.json) |
| CVehiclePool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CVehicleRecording | `ChangeCarPlaybackToUseAI` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `FindIndexWithFileNameNumber` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `HasRecordingFileBeenLoaded` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `Init` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `InitAtStartOfGame` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `InterpolateInfoForCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `IsPlaybackGoingOnForCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `IsPlaybackPausedForCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `Load` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `PausePlaybackRecordedCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `RegisterRecordingFile` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `RemoveAllRecordingsThatArentUsed` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `RemoveRecordingFile` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `Render` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `RenderLineSegment` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `RequestRecordingFile` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `RestoreInfoForCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `RestoreInfoForMatrix` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `SaveOrRetrieveDataForThisFrame` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `SetPlaybackSpeed` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `SetRecordingToPointClosestToCoors` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `ShutDown` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `SkipForwardInRecording` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `SkipToEndAndStopPlaybackRecordedCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `SmoothRecording` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `StartPlaybackRecordedCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `StopPlaybackRecordedCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `StopPlaybackWithIndex` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleRecording | `UnpausePlaybackRecordedCar` | [Resumen](../reversing/VehRec29/validation-summary.json) |
| CVehicleSaveStructure | `Construct` | [Resumen](../reversing/Core70/validation-summary.json) |
| CVehicleSaveStructure | `Extract` | [Resumen](../reversing/Core70/validation-summary.json) |
| CWanted | `AddCrimeToQ` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `AreArmyRequired` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `AreFbiRequired` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `AreMiamiViceRequired` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `AreSwatRequired` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `CanCopJoinPursuit-func` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `CanCopJoinPursuit-method` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `CheatWantedLevel` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `ClearQdCrimes` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `ClearWantedLevelAndGoOnParole` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `ComputePursuitCopToDisplace` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `Initialise` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `InitialiseStaticVariables` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `IsClosestCop` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `IsInPursuit` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `NumOfHelisRequired` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `RegisterCrime` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `RegisterCrime_Immediately` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `RemoveExcessPursuitCops` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `RemovePursuitCop-func` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `RemovePursuitCop-method` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `ReportCrimeNow` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `Reset` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `ResetPolicePursuit` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `SetMaximumWantedLevel` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `SetPursuitCop` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `SetWantedLevel` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `SetWantedLevelNoDrop` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `Update` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `UpdateCrimesQ` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `UpdateEachFrame` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `UpdateWantedLevel` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWanted | `WorkOutPolicePresence` | [Resumen](../reversing/Wanted33/validation-summary.json) |
| CWaterCannon | `Init` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `PushPeds` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `Render` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `Update_NewInput` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `Update_OncePerFrame` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannons | `Init` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWaterCannons | `Render` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWaterCannons | `Update` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWaterCannons | `UpdateOne` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWeaponEffects | `ClearCrossHair` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `ClearCrossHairImmediately` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `ClearCrossHairs` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `ClearCrossHairsImmediately` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `Init` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `IsLockedOn` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `MarkTarget` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `Render` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponEffects | `Shutdown` | [Resumen](../reversing/Ped9/validation-summary.json) |
| CWeaponInfo | `FindWeaponFireType` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `FindWeaponType` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `GetCrouchReloadAnimationID` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `GetSkillStatIndex` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `GetTargetHeadRange` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `GetWeaponInfo-original` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `GetWeaponReloadTime` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `Initialise` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `LoadWeaponData` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponInfo | `Shutdown` | [Resumen](../reversing/Data10/validation-summary.json) |
| CWeaponModelInfo | `GetModelType` | [Resumen](../reversing/World52/validation-summary.json) |
| CWeaponModelInfo | `Init` | [Resumen](../reversing/World52/validation-summary.json) |
| CWeaponModelInfo | `SetClump` | [Resumen](../reversing/World52/validation-summary.json) |
| CWeather | `AddRain` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `AddSandStormParticles` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `FindWeatherTypesList` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `ForceWeather` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `ForceWeatherNow` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `ForecastWeather` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `Init` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `IsRainy` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `ReleaseWeather` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `RenderRainStreaks` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `SetWeatherToAppropriateTypeNow` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `Update` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWeather | `UpdateInTunnelness` | [Resumen](../reversing/Misc61a/validation-summary.json) |
| CWindModifiers | `FindWindModifier` | [Resumen](../reversing/Core70/validation-summary.json) |
| CWindModifiers | `RegisterOne` | [Resumen](../reversing/Core70/validation-summary.json) |
| C_PcSave | `DeleteSlot` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `GenerateGameFilename` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `PopulateSlotInfo` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `SaveSlot` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `SetSaveDirectory` | [Resumen](../reversing/PcSave/validation-summary.json) |
| CdStream | `CdStreamGetStatus` | [Resumen](../reversing/Core9/validation-summary.json) |
| CdStream | `CdStreamInit` | [Resumen](../reversing/Core9/validation-summary.json) |
| CdStream | `CdStreamInitThread` | [Resumen](../reversing/Core9/validation-summary.json) |
| CdStream | `CdStreamOpen` | [Resumen](../reversing/Core9/validation-summary.json) |
| CdStream | `CdStreamRead` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| CdStream | `CdStreamRemoveImages` | [Resumen](../reversing/Core9/validation-summary.json) |
| CdStream | `CdStreamShutdown` | [Resumen](../reversing/Core9/validation-summary.json) |
| CdStream | `CdStreamSync` | [Resumen](../reversing/Core9/validation-summary.json) |
| CdStream | `CdStreamThread` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `AsciiToGxtChar` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `CalcScreenCoors-VV` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `CalcScreenCoors-VVff` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `DoesInfiniteLineTouchScreen` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `IsPointInsideLine` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `LittleTest` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `MakeUpperCase-` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `MakeUpperCase-in` | [Resumen](../reversing/Core9/validation-summary.json) |
| Common | `WriteRaster` | [Resumen](../reversing/Core9/validation-summary.json) |
| FurnitureGroup_c | `AddFurniture` | [Resumen](../reversing/Misc6/validation-summary.json) |
| FurnitureGroup_c | `AddSubGroup` | [Resumen](../reversing/Misc6/validation-summary.json) |
| FurnitureGroup_c | `Exit` | [Resumen](../reversing/Misc6/validation-summary.json) |
| FurnitureGroup_c | `GetFurniture` | [Resumen](../reversing/Misc6/validation-summary.json) |
| FurnitureGroup_c | `GetRandomId` | [Resumen](../reversing/Misc6/validation-summary.json) |
| FurnitureGroup_c | `Init` | [Resumen](../reversing/Misc6/validation-summary.json) |
| FurnitureManager_c | `Exit` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| FurnitureManager_c | `GetFurniture` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| FurnitureManager_c | `GetGroupId` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| FurnitureManager_c | `GetRandomId` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| FurnitureManager_c | `GetSubGroupId` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| FurnitureManager_c | `Init` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| FurnitureManager_c | `LoadFurniture` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| FurnitureSubGroup_c | `AddFurniture` | [Resumen](../reversing/FurnitureSubGroup_c/validation-summary.json) |
| FurnitureSubGroup_c | `GetFurniture` | [Resumen](../reversing/FurnitureSubGroup_c/validation-summary.json) |
| FurnitureSubGroup_c | `GetRandomId` | [Resumen](../reversing/FurnitureSubGroup_c/validation-summary.json) |
| FxEmitterPrt_c | `operator new[]` | [Resumen](../reversing/Pools56/validation-summary.json) |
| FxInfoManager_c | `AddFxInfo` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxInfoManager_c | `ProcessEmissionInfo` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxInfoManager_c | `ProcessMovementInfo` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxInfoManager_c | `ProcessRenderInfo` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxManager_c | `CalcFrustumInfo` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `Constructor` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `CreateFxSystem-1` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `CreateFxSystem-2` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `CreateFxSystem-3` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `CreateFxSystem-4` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `DestroyAllFxSystems` | [Resumen](../reversing/FxManager/validation-summary.json) |
| FxManager_c | `DestroyFxSystem` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `Destructor` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `Exit` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `FindFxSystemBP` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `FreeUpParticle` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `FxRwMatrixCreate` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `FxRwMatrixDestroy` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `GetFrustumInfo` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `GetParticle` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `Init` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `LoadFxProject` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `LoadFxSystemBP` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `Render` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `ReturnParticle` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `SetWindData` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `ShouldCreate` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `UnloadFxProject` | [Resumen](../reversing/Fx25/validation-summary.json) |
| FxManager_c | `Update` | [Resumen](../reversing/FxManager/validation-summary.json) |
| FxMemoryPool_c | `Exit` | [Resumen](../reversing/Fx43/validation-summary.json) |
| FxMemoryPool_c | `GetMem` | [Resumen](../reversing/Fx43/validation-summary.json) |
| FxMemoryPool_c | `Init` | [Resumen](../reversing/Fx43/validation-summary.json) |
| FxMemoryPool_c | `Optimise` | [Resumen](../reversing/Fx43/validation-summary.json) |
| FxMemoryPool_c | `Reset` | [Resumen](../reversing/Fx43/validation-summary.json) |
| FxPrimBP_c | `GetRWMatrix` | [Resumen](../reversing/Core70/validation-summary.json) |
| FxPrimBP_c | `Load` | [Resumen](../reversing/Core70/validation-summary.json) |
| FxSphere_c | `GetDistToPlane` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxSphere_c | `IsCollision` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxSphere_c | `operator delete-` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxSphere_c | `operator new` | [Resumen](../reversing/Misc8/validation-summary.json) |
| FxSystemBP_c | `Constructor` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `Destructor` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `FreePrtFromSystem` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `GetBoundingSphere` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `Load` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `Render` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `SetBoundingSphere` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `Unload` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `Update` | [Resumen](../reversing/Data10/validation-summary.json) |
| FxSystemBP_c | `operator new` | [Resumen](../reversing/Data10/validation-summary.json) |
| Fx_c | `RenderAddTri_` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Fx_c | `RenderBegin` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Fx_c | `RenderEnd` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Fx_c | `RotateVecAboutVec` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Fx_c | `RotateVecIntoVec` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Fx_c | `TriggerBulletSplash` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Fx_c | `TriggerFootSplash` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Fx_c | `TriggerWaterSplash` | [Resumen](../reversing/Fx8/validation-summary.json) |
| Game | `FrontendIdle` | [Resumen](../reversing/GameIdle/validation-summary.json) |
| Game | `GameInit` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `Idle` | [Resumen](../reversing/GameIdle/validation-summary.json) |
| Game | `InitialiseGame` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `Render2dStuff` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `RenderDebugShit` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `RenderEffects` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `RenderMenus` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `RenderScene` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `RwInitialize` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `RwTerminate` | [Resumen](../reversing/Sys12/validation-summary.json) |
| Game | `TheGame` | [Resumen](../reversing/Sys12/validation-summary.json) |
| IKChainManager_c | `AbortLookAt` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `AbortPointArm` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `AddIKChain` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `CanAccept` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `CanAcceptLookAt` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `Exit` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `GetLookAtEntity` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `GetLookAtOffset` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `Init` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `IsArmPointing` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `IsFacingTarget` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `IsLooking` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `LookAt` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `PointArm` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `RemoveIKChain` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `Reset` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChainManager_c | `Update` | [Resumen](../reversing/Sys17/validation-summary.json) |
| IKChain_c | `ClampLimits` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `Exit` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `GetBoneNodeFromTag` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `GetBoneSpeed` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `GetLimits` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `GetPriority` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `Init` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `IsAtTarget` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `IsFacingTarget` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `MoveBonesToTarget` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `SetBlend` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `SetBoneSpeed` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `SetLimits` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `SetOffsetBoneTag` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `SetOffsetPos` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `SetupBones` | [Resumen](../reversing/ValidationRegistry/ik-validation.json) |
| IKChain_c | `Update` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `UpdateEntity` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `UpdateOffset` | [Resumen](../reversing/IK20/validation-summary.json) |
| IKChain_c | `UpdateTarget` | [Resumen](../reversing/IK20/validation-summary.json) |
| Input | `AttachInputDevices` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `HandleKeyDown` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `HandleKeyUp` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `KeyboardHandler` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `PadHandler` | [Resumen](../reversing/Fx43/validation-summary.json) |
| InteriorManager_c | `ActivatePeds` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `AddSameGroupEffectInfos` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `AreAnimsLoaded` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `Exit` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `FindStealableObjectId-1` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `FindStealableObjectId-2` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `GetBoundingBox` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `GetInteriorFromPool` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `GetPedsInterior` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `GetPedsInteriorGroup` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `GetVectorsInterior` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `GetVisibleEffects` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `HasInteriorHadStealDataSetup` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `Init` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `IsGroupActive` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `IsInteriorEffectVisible` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `PruneVisibleEffects` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `ReturnInteriorToPool` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `SetEntryExitPtr` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `SetStealableObjectStolen` | [Resumen](../reversing/Interior21/validation-summary.json) |
| InteriorManager_c | `Update` | [Resumen](../reversing/Interior21/validation-summary.json) |
| JPegPlugin | `JPegCompressScreen` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegCompressScreenToBuffer` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegCompressScreenToFile` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegDecompressToRaster` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegDecompressToVramFromBuffer` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Light | `ActivateDirectional` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `AddAnExtraDirectionalLight` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `DeActivateDirectional` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `GetDayNightBalance` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `LightsCreate` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `LightsDestroy` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `LightsEnable` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `ReSetAmbientAndDirectionalColours` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `RemoveExtraDirectionalLights` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `RestoreLightsForInfraredVisionHeatObjects` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetAmbientAndDirectionalColours` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetAmbientColours-color` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetAmbientColours-void` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetAmbientColoursToIndicateRoadGroup` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetBrightMarkerColours` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetDirectionalColours` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetFlashyColours` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetFlashyColours_Mild` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetFullAmbient` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetLightColoursForPedsCarsAndObjects` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetLightsForInfraredVisionDefaultObjects` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetLightsForInfraredVisionHeatObjects` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetLightsForNightVision` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `SetLightsWithTimeOfDayColour` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `StoreAndSetLightsForInfraredVisionHeatObjects` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `WorldReplaceNormalLightsWithScorched` | [Resumen](../reversing/Light27/validation-summary.json) |
| Light | `WorldReplaceScorchedLightsWithNormal` | [Resumen](../reversing/Light27/validation-summary.json) |
| ModelIndices | `InitModelIndices` | [Resumen](../reversing/Core70/validation-summary.json) |
| ModelIndices | `MatchAllModelStrings` | [Resumen](../reversing/Core70/validation-summary.json) |
| NodeNamePlugin | `GetFrameNodeName` | [Resumen](../reversing/World52/validation-summary.json) |
| NodeNamePlugin | `NodeNamePluginAttach` | [Resumen](../reversing/World52/validation-summary.json) |
| NodeNamePlugin | `SetFrameNodeName` | [Resumen](../reversing/World52/validation-summary.json) |
| PipelinePlugin | `GetPipelineID` | [Resumen](../reversing/World52/validation-summary.json) |
| PipelinePlugin | `PipelinePluginAttach` | [Resumen](../reversing/World52/validation-summary.json) |
| PipelinePlugin | `SetPipelineID` | [Resumen](../reversing/World52/validation-summary.json) |
| ProcObjectMan_c | `Exit` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcObjectMan_c | `GetEntityFromPool` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcObjectMan_c | `Init` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcObjectMan_c | `LoadDataFile` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcObjectMan_c | `ProcessTriangleAdded` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcObjectMan_c | `ProcessTriangleRemoved` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcObjectMan_c | `ReturnEntityToPool` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcObjectMan_c | `Update` | [Resumen](../reversing/Fx8/validation-summary.json) |
| ProcSurfaceInfo_c | `AddObject` | [Resumen](../reversing/Misc8/validation-summary.json) |
| ProcSurfaceInfo_c | `AddObjects` | [Resumen](../reversing/Misc8/validation-summary.json) |
| ProcSurfaceInfo_c | `Exit` | [Resumen](../reversing/Misc8/validation-summary.json) |
| ProcSurfaceInfo_c | `Init` | [Resumen](../reversing/Misc8/validation-summary.json) |
| Ps | `CheckDirectSound` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `GetVideoMemInfo` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `InitialiseLanguage` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psAlwaysOnTop` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psCameraBeginUpdate` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psCameraShowRaster` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psDebugMessageHandler` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psErrorMessage` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psGrabScreen` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psInitialize` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psInstallFileSystem` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psMouseSetPos` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psMouseSetVisibility` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psNativeTextureSupport` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psPathGetSeparator` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psPathnameCreate` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psPathnameDestroy` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psSelectDevice` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psTerminate` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psTimer` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psWarningMessage` | [Resumen](../reversing/Ps22/validation-summary.json) |
| Ps | `psWindowSetText` | [Resumen](../reversing/Ps22/validation-summary.json) |
| RenderBuffer | `ClearRenderBuffer` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RenderBuffer | `RenderStuffInBuffer` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RenderBuffer | `StartStoring` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RenderBuffer | `StopStoring` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RpAnimBlend | `ClumpAnimConstruct` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `ClumpAnimCopy` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `ClumpAnimDestruct` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `FrameUpdateCallBackCompressedNonSkinned` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `FrameUpdateCallBackCompressedSkinned` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `FrameUpdateCallBackNonSkinned` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `FrameUpdateCallBackSkinned` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendAllocateData` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpAddAssociation` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpExtractAssociations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpFillFrameArray` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpFindBone` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpFindFrame` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpFindFrameFromHashKey` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetAssociation-AnimId` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetAssociation-AnimName` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetAssociation-Hier` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetFirstAssociation-` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetFirstAssociation-Flags` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetMainAssociation` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetMainAssociation_N` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetMainPartialAssociation` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetMainPartialAssociation_N` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetNumAssociations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetNumNonPartialAssociations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGetNumPartialAssociations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpGiveAssociations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpInit` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpIsInitialized` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpPauseAllAnimations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpRemoveAllAssociations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpRemoveAssociations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpSetBlendDeltas` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpUnPauseAllAnimations` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpUpdateAnimations` | [Resumen](../reversing/ValidationRegistry/rpanimblend-validation.json) |
| RpAnimBlend | `RpAnimBlendCreateAnimationForHierarchy` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendFrameGetName` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendFrameSetName` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendGetNextAssociation-Any` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendGetNextAssociation-Flags` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendKeyFrameInterpolate` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendPluginAttach` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| RpAnimBlend | `RtAnimBlendKeyFrameApply` | [Resumen](../reversing/RpAnim43/validation-summary.json) |
| Rs | `RsAlwaysOnTop` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsCameraBeginUpdate` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsErrorMessage` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsEventHandler` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsGrabScreen` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsInitialize` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsInputDeviceAttach` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsKeyFromScanCode` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsKeyboardEventHandler` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsMouseEventHandler` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsMouseSetPos` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsMouseSetVisibility` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsPadEventHandler` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsPathGetSeparator` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsPathnameCreate` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsPathnameDestroy` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsRegisterImageLoader` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsRwInitialize` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsRwTerminate` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsSelectDevice` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsSetDebug` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsTerminate` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsTimer` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsWarningMessage` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `RsWindowSetText` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `rsCommandLine` | [Resumen](../reversing/Rs27/validation-summary.json) |
| Rs | `rsPreInitCommandLine` | [Resumen](../reversing/Rs27/validation-summary.json) |
| RtAnim | `RtAnimInterpolatorSetCurrentAnim` | [Resumen](../reversing/FixBugs/validation-summary.json) |
| RwCore | `RwCameraBeginUpdate` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RwCore | `RwCameraClear` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RwCore | `RwCameraEndUpdate` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RwCore | `RwCameraShowRaster` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RwHelper | `AtomicRemoveAnimFromSkinCB` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `Get2DEffectAtomic` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetAnimHierarchyFromClump` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetAnimHierarchyFromSkinClump` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetEventGlobalGroup` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetFirstAtomic` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetFirstAtomicCallback` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetFirstChild` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetFirstFrameCallback` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetFirstObject` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetFirstObjectCallback` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `GetNameAndDamage` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `RemoveRefsCB` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `RpAtomicConvertGeometryToTL` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `RpAtomicConvertGeometryToTS` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `RpClumpConvertGeometryToTL` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `RpClumpConvertGeometryToTS` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `SetFilterModeOnAtomicsTextures` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `SetFilterModeOnClumpsTextures` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `SkinAtomicGetHAnimHierarchCB` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `atomicConvertGeometryToTL` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `atomicConvertGeometryToTS` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `forceLinearFilteringAtomicsCB` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| RwHelper | `forceLinearFilteringMatTexturesCB` | [Resumen](../reversing/RwHelper24/validation-summary.json) |
| Securom | `SecuromDisplayInit` | [Resumen](../reversing/Misc70/validation-summary.json) |
| Securom | `SecuromStateDisplay` | [Resumen](../reversing/Misc70/validation-summary.json) |
| SurfaceInfos_c | `CanClimb` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CantSprintOn` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesObjects` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesPlants` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesSparks` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesWheelDust` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesWheelGrass` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesWheelGravel` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesWheelMud` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesWheelSand` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `CreatesWheelSpray` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetAdhesionGroup` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetAdhesiveLimit` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetBulletFx` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetFlammability` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetFrictionEffect` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetRoughness` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetSkidmarkType` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetSurfaceIdFromName` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetTyreGrip` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `GetWetMultiplier` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `Init` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioConcrete` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioGrass` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioGravel` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioLongGrass` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioMetal` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioSand` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioTile` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioWater` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsAudioWood` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsBeach` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsGlass` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsPavement` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsSand` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsSeeThrough` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsShallowWater` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsShootThrough` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsSkateable` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsSoftLanding` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsStairs` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsSteepSlope` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `IsWater` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `LeavesFootsteps` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `MakesCarClean` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `MakesCarDirty` | [Resumen](../reversing/Surface47/validation-summary.json) |
| SurfaceInfos_c | `ProducesFootDust` | [Resumen](../reversing/Surface47/validation-summary.json) |
| VideoMode | `FreeVideoModeList` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| VideoMode | `GetVideoModeList` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| VideoMode | `IsVideoModeExclusive` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| VideoMode | `SetVideoMode` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| VideoPlayer | `OnGraphNotify` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| VideoPlayer | `Play` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| VideoPlayer | `Release` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| VideoPlayer | `Shutdown` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| VideoPlayer | `UpdateWindow` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| VideoPlayer | `WaitState` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| WaterCreatureManager_c | `CanAddWaterCreatureAtPos` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| WaterCreatureManager_c | `Exit` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| WaterCreatureManager_c | `GetRandomWaterCreatureId` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| WaterCreatureManager_c | `Init` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| WaterCreatureManager_c | `TryToExitGroup` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| WaterCreatureManager_c | `TryToFreeUpWaterCreatures` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| WaterCreatureManager_c | `Update` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| WaterCreature_c | `Exit` | [Resumen](../reversing/World52/validation-summary.json) |
| WaterCreature_c | `Init` | [Resumen](../reversing/World52/validation-summary.json) |
| WaterCreature_c | `Update` | [Resumen](../reversing/World52/validation-summary.json) |
| Win | `GTATranslateKey` | [Resumen](../reversing/Misc6/validation-summary.json) |
| Win | `GTATranslateShiftKey` | [Resumen](../reversing/Misc6/validation-summary.json) |
| Win | `IsAlreadyRunning` | [Resumen](../reversing/Misc6/validation-summary.json) |
| Win | `IsForegroundApp` | [Resumen](../reversing/Misc6/validation-summary.json) |
| Win | `MessageLoop` | [Resumen](../reversing/Misc6/validation-summary.json) |
| cBuoyancy | `AddSplashParticles` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `CalcBuoyancyForce` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `FindWaterLevel` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `FindWaterLevelNorm` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `PreCalcSetup` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `ProcessBuoyancy` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `ProcessBuoyancyBoat` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `SimpleCalcBuoyancy` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cBuoyancy | `SimpleSumBuoyancyData` | [Resumen](../reversing/Buoy9/validation-summary.json) |
| cHandlingDataMgr | `ConvertBikeDataToGameUnits` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `ConvertBikeDataToWorldUnits` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `ConvertDataToGameUnits` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `ConvertDataToWorldUnits` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `FindExactWord` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `GetBoatPointer` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `GetFlyingPointer` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `GetHandlingId` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `HasFrontWheelDrive` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `HasRearWheelDrive` | [Resumen](../reversing/World11/validation-summary.json) |
| cHandlingDataMgr | `LoadHandlingData` | [Resumen](../reversing/World11/validation-summary.json) |
| cTransmission | `CalculateDriveAcceleration` | [Resumen](../reversing/Misc8/validation-summary.json) |
| cTransmission | `CalculateGearForSimpleCar` | [Resumen](../reversing/Misc8/validation-summary.json) |
| cTransmission | `DisplayGearRatios` | [Resumen](../reversing/Misc8/validation-summary.json) |
| cTransmission | `InitGearRatios` | [Resumen](../reversing/Misc8/validation-summary.json) |

## Inventario por clase

<details>
<summary>Entradas reconstruidas de todas las clases registradas</summary>

| Clase | Reconstruidas | Vigentes | Históricas por revisar | Sin vincular |
| --- | ---: | ---: | ---: | ---: |
| `App` | 7 | 0 | 7 | 0 |
| `BoneNodeManager_c` | 8 | 0 | 8 | 0 |
| `BoneNode_c` | 17 | 0 | 17 | 0 |
| `BreakManager_c` | 6 | 0 | 6 | 0 |
| `BreakObject_c` | 9 | 0 | 9 | 0 |
| `BreakablePlugin` | 6 | 0 | 6 | 0 |
| `C2dEffect` | 14 | 0 | 14 | 0 |
| `C3dMarker` | 6 | 0 | 6 | 0 |
| `C3dMarkers` | 20 | 0 | 20 | 0 |
| `CAEAmbienceTrackManager` | 7 | 0 | 7 | 0 |
| `CAEAudioChannel` | 12 | 0 | 12 | 0 |
| `CAEAudioEntity` | 2 | 0 | 2 | 0 |
| `CAEAudioEnvironment` | 6 | 0 | 6 | 0 |
| `CAEAudioHardware` | 54 | 0 | 53 | 1 |
| `CAEAudioUtility` | 11 | 0 | 11 | 0 |
| `CAEBankLoader` | 6 | 0 | 6 | 0 |
| `CAECollisionAudioEntity` | 18 | 0 | 18 | 0 |
| `CAECutsceneTrackManager` | 8 | 0 | 8 | 0 |
| `CAEDataStream` | 20 | 0 | 20 | 0 |
| `CAEDoorAudioEntity` | 6 | 0 | 6 | 0 |
| `CAEExplosionAudioEntity` | 3 | 0 | 3 | 0 |
| `CAEFireAudioEntity` | 7 | 0 | 7 | 0 |
| `CAEFrontendAudioEntity` | 4 | 0 | 0 | 4 |
| `CAEGlobalWeaponAudioEntity` | 6 | 0 | 6 | 0 |
| `CAEMFDecoder` | 10 | 0 | 10 | 0 |
| `CAEMP3BankLoader` | 13 | 0 | 13 | 0 |
| `CAEMP3TrackLoader` | 7 | 0 | 7 | 0 |
| `CAEPedAudioEntity` | 16 | 0 | 1 | 15 |
| `CAEPedSpeechAudioEntity` | 58 | 0 | 58 | 0 |
| `CAEPedWeaponAudioEntity` | 5 | 0 | 5 | 0 |
| `CAEPedlessSpeechAudioEntity` | 11 | 0 | 11 | 0 |
| `CAEPoliceScannerAudioEntity` | 13 | 0 | 0 | 13 |
| `CAERadioTrackManager` | 29 | 0 | 0 | 29 |
| `CAEScriptAudioEntity` | 8 | 0 | 0 | 8 |
| `CAESmoothFadeThread` | 10 | 0 | 10 | 0 |
| `CAESound` | 18 | 0 | 18 | 0 |
| `CAESoundManager` | 14 | 0 | 14 | 0 |
| `CAEStaticChannel` | 8 | 0 | 8 | 0 |
| `CAEStreamThread` | 14 | 0 | 14 | 0 |
| `CAEStreamTransformer` | 2 | 0 | 2 | 0 |
| `CAEStreamingChannel` | 21 | 0 | 3 | 18 |
| `CAEStreamingDecoder` | 2 | 0 | 2 | 0 |
| `CAETrackLoader` | 1 | 0 | 1 | 0 |
| `CAETwinLoopSoundEntity` | 7 | 0 | 7 | 0 |
| `CAEUserRadioTrackManager` | 15 | 0 | 15 | 0 |
| `CAEVehicleAudioEntity` | 102 | 0 | 0 | 102 |
| `CAEVorbisDecoder` | 13 | 0 | 13 | 0 |
| `CAEWMADecoder` | 11 | 0 | 11 | 0 |
| `CAEWaterCannonAudioEntity` | 7 | 0 | 7 | 0 |
| `CAEWaveDecoder` | 9 | 0 | 9 | 0 |
| `CAEWeaponAudioEntity` | 19 | 0 | 19 | 0 |
| `CAEWeatherAudioEntity` | 5 | 0 | 5 | 0 |
| `CAccidentManager` | 5 | 0 | 5 | 0 |
| `CAcquaintance` | 5 | 0 | 5 | 0 |
| `CActiveOccluder` | 2 | 0 | 2 | 0 |
| `CAnimBlendAssocGroup` | 15 | 0 | 15 | 0 |
| `CAnimBlendAssociation` | 20 | 0 | 20 | 0 |
| `CAnimBlendClumpData` | 6 | 0 | 6 | 0 |
| `CAnimBlendHierarchy` | 16 | 0 | 16 | 0 |
| `CAnimBlendNode` | 16 | 0 | 16 | 0 |
| `CAnimBlendSequence` | 14 | 0 | 14 | 0 |
| `CAnimBlendStaticAssociation` | 6 | 0 | 6 | 0 |
| `CAnimManager` | 31 | 0 | 31 | 0 |
| `CAnimatedBuilding` | 3 | 0 | 3 | 0 |
| `CAtomicModelInfo` | 9 | 0 | 9 | 0 |
| `CAudioEngine` | 82 | 0 | 0 | 82 |
| `CAudioZones` | 5 | 0 | 5 | 0 |
| `CAutomobile` | 99 | 0 | 0 | 99 |
| `CBaseModelInfo` | 34 | 0 | 34 | 0 |
| `CBike` | 22 | 0 | 0 | 22 |
| `CBirds` | 6 | 0 | 6 | 0 |
| `CBmx` | 10 | 0 | 10 | 0 |
| `CBoat` | 23 | 0 | 23 | 0 |
| `CBouncingPanel` | 3 | 0 | 3 | 0 |
| `CBox` | 2 | 0 | 2 | 0 |
| `CBridge` | 4 | 0 | 4 | 0 |
| `CBrightLights` | 4 | 0 | 4 | 0 |
| `CBuilding` | 2 | 0 | 2 | 0 |
| `CBuildingPool` | 1 | 0 | 1 | 0 |
| `CBulletInfo` | 4 | 0 | 4 | 0 |
| `CBulletTraces` | 5 | 0 | 5 | 0 |
| `CCam` | 14 | 0 | 0 | 14 |
| `CCamera` | 91 | 0 | 0 | 91 |
| `CCarAI` | 25 | 0 | 25 | 0 |
| `CCarCtrl` | 22 | 0 | 22 | 0 |
| `CCarEnterExit` | 27 | 0 | 0 | 27 |
| `CCarFXRenderer` | 10 | 0 | 10 | 0 |
| `CCarGenerator` | 8 | 0 | 8 | 0 |
| `CCheat` | 72 | 0 | 0 | 72 |
| `CCheckpoint` | 1 | 0 | 1 | 0 |
| `CCheckpoints` | 8 | 0 | 8 | 0 |
| `CCivilianPed` | 3 | 0 | 3 | 0 |
| `CClock` | 9 | 0 | 9 | 0 |
| `CClothes` | 11 | 0 | 11 | 0 |
| `CClothesBuilder` | 15 | 0 | 0 | 15 |
| `CClouds` | 21 | 0 | 21 | 0 |
| `CClumpModelInfo` | 24 | 0 | 24 | 0 |
| `CColAccel` | 11 | 0 | 11 | 0 |
| `CColLine` | 1 | 0 | 1 | 0 |
| `CColModel` | 9 | 0 | 9 | 0 |
| `CColModelPool` | 1 | 0 | 1 | 0 |
| `CColSphere` | 5 | 0 | 5 | 0 |
| `CColStore` | 23 | 0 | 23 | 0 |
| `CColTrianglePlane` | 2 | 0 | 2 | 0 |
| `CCollision` | 59 | 0 | 58 | 1 |
| `CCollisionData` | 8 | 0 | 8 | 0 |
| `CCollisionPlugin` | 2 | 0 | 2 | 0 |
| `CColourSet` | 2 | 0 | 2 | 0 |
| `CCompressedMatrixNotAligned` | 2 | 0 | 2 | 0 |
| `CControllerConfigManager` | 58 | 0 | 58 | 0 |
| `CControllerState` | 2 | 0 | 2 | 0 |
| `CConversationForPed` | 2 | 0 | 2 | 0 |
| `CConversationNode` | 1 | 0 | 1 | 0 |
| `CConversations` | 10 | 0 | 10 | 0 |
| `CCopPed` | 7 | 0 | 7 | 0 |
| `CCoronas` | 11 | 0 | 11 | 0 |
| `CCover` | 16 | 0 | 16 | 0 |
| `CCoverPoint` | 3 | 0 | 3 | 0 |
| `CCranes` | 5 | 0 | 5 | 0 |
| `CCredits` | 6 | 0 | 6 | 0 |
| `CCreepingFire` | 3 | 0 | 3 | 0 |
| `CCrime` | 2 | 0 | 2 | 0 |
| `CCullZones` | 21 | 0 | 21 | 0 |
| `CCurrentVehicle` | 2 | 0 | 2 | 0 |
| `CCurves` | 6 | 0 | 6 | 0 |
| `CCustomBuildingDNPipeline` | 14 | 0 | 14 | 0 |
| `CCustomBuildingPipeline` | 7 | 0 | 7 | 0 |
| `CCustomBuildingRenderer` | 3 | 0 | 0 | 3 |
| `CCustomCarEnvMapPipeline` | 37 | 0 | 37 | 0 |
| `CCustomCarPlateMgr` | 14 | 0 | 14 | 0 |
| `CCustomRoadsignMgr` | 9 | 0 | 9 | 0 |
| `CCutsceneMgr` | 35 | 0 | 35 | 0 |
| `CCutsceneObject` | 6 | 0 | 6 | 0 |
| `CDamageAtomicModelInfo` | 4 | 0 | 4 | 0 |
| `CDamageManager` | 27 | 0 | 27 | 0 |
| `CDarkel` | 19 | 0 | 19 | 0 |
| `CData` | 2 | 0 | 2 | 0 |
| `CDebug` | 1 | 0 | 1 | 0 |
| `CDecision` | 1 | 0 | 0 | 1 |
| `CDecisionMakerTypesFileLoader` | 7 | 0 | 7 | 0 |
| `CDirectory` | 10 | 0 | 10 | 0 |
| `CDoor` | 6 | 0 | 6 | 0 |
| `CDraw` | 2 | 0 | 2 | 0 |
| `CDummy` | 1 | 0 | 1 | 0 |
| `CDummyObject` | 2 | 0 | 2 | 0 |
| `CDummyPool` | 1 | 0 | 1 | 0 |
| `CEntity` | 70 | 0 | 70 | 0 |
| `CEntityScanner` | 4 | 0 | 4 | 0 |
| `CEntitySeekPosCalculatorFixedPos` | 1 | 0 | 1 | 0 |
| `CEntitySeekPosCalculatorRadiusAngleOffset` | 1 | 0 | 1 | 0 |
| `CEntitySeekPosCalculatorStandard` | 1 | 0 | 1 | 0 |
| `CEntitySeekPosCalculatorXYOffset` | 1 | 0 | 1 | 0 |
| `CEntryExit` | 11 | 0 | 11 | 0 |
| `CEntryExitManager` | 18 | 0 | 18 | 0 |
| `CEntryInfoList` | 1 | 0 | 1 | 0 |
| `CEntryInfoNode` | 2 | 0 | 2 | 0 |
| `CEntryInfoNodePool` | 1 | 0 | 1 | 0 |
| `CEscalator` | 5 | 0 | 5 | 0 |
| `CEscalators` | 4 | 0 | 4 | 0 |
| `CEvent` | 3 | 0 | 3 | 0 |
| `CEventAcquaintancePed` | 4 | 0 | 4 | 0 |
| `CEventAcquaintancePedHate` | 1 | 0 | 1 | 0 |
| `CEventAcquaintancePedHateBadlyLit` | 2 | 0 | 2 | 0 |
| `CEventAreaCodes` | 3 | 0 | 3 | 0 |
| `CEventAttractor` | 4 | 0 | 4 | 0 |
| `CEventBuildingCollision` | 4 | 0 | 4 | 0 |
| `CEventCarUpsideDown` | 2 | 0 | 2 | 0 |
| `CEventCopCarBeingStolen` | 2 | 0 | 2 | 0 |
| `CEventDamage` | 20 | 0 | 20 | 0 |
| `CEventDanger` | 4 | 0 | 4 | 0 |
| `CEventDontJoinPlayerGroup` | 1 | 0 | 1 | 0 |
| `CEventEditableResponse` | 11 | 0 | 11 | 0 |
| `CEventEscalator` | 1 | 0 | 1 | 0 |
| `CEventGlobalGroup` | 3 | 0 | 3 | 0 |
| `CEventGroup` | 12 | 0 | 12 | 0 |
| `CEventGunShot` | 5 | 0 | 5 | 0 |
| `CEventGunShotWhizzedBy` | 2 | 0 | 2 | 0 |
| `CEventHandler` | 69 | 0 | 0 | 69 |
| `CEventHandlerHistory` | 10 | 0 | 10 | 0 |
| `CEventHitByWaterCannon` | 2 | 0 | 2 | 0 |
| `CEventInAir` | 2 | 0 | 2 | 0 |
| `CEventKnockOffBike` | 8 | 0 | 8 | 0 |
| `CEventLeaderEnteredCarAsDriver` | 2 | 0 | 2 | 0 |
| `CEventLeaderEntryExit` | 1 | 0 | 1 | 0 |
| `CEventLeaderExitedCarAsDriver` | 2 | 0 | 2 | 0 |
| `CEventLeaderQuitEnteringCarAsDriver` | 2 | 0 | 2 | 0 |
| `CEventLeanOnVehicle` | 2 | 0 | 2 | 0 |
| `CEventNewGangMember` | 1 | 0 | 1 | 0 |
| `CEventObjectCollision` | 2 | 0 | 2 | 0 |
| `CEventOnFire` | 1 | 0 | 1 | 0 |
| `CEventPassObject` | 2 | 0 | 2 | 0 |
| `CEventPedCollisionWithPed` | 3 | 0 | 3 | 0 |
| `CEventPedCollisionWithPlayer` | 1 | 0 | 1 | 0 |
| `CEventPlayerCollisionWithPed` | 1 | 0 | 1 | 0 |
| `CEventPlayerCommandToGroup` | 2 | 0 | 2 | 0 |
| `CEventPlayerCommandToGroupAttack` | 2 | 0 | 2 | 0 |
| `CEventPlayerCommandToGroupGather` | 1 | 0 | 1 | 0 |
| `CEventPool` | 1 | 0 | 1 | 0 |
| `CEventPotentialWalkIntoFire` | 2 | 0 | 2 | 0 |
| `CEventPotentialWalkIntoObject` | 2 | 0 | 2 | 0 |
| `CEventPotentialWalkIntoPed` | 3 | 0 | 3 | 0 |
| `CEventPotentialWalkIntoVehicle` | 2 | 0 | 2 | 0 |
| `CEventScriptCommand` | 8 | 0 | 8 | 0 |
| `CEventScriptedAttractor` | 1 | 0 | 1 | 0 |
| `CEventSeenCop` | 1 | 0 | 1 | 0 |
| `CEventSeenPanickedPed` | 2 | 0 | 2 | 0 |
| `CEventSoundQuiet` | 3 | 0 | 3 | 0 |
| `CEventStuckInAir` | 4 | 0 | 4 | 0 |
| `CEventVehicleCollision` | 3 | 0 | 3 | 0 |
| `CEventVehicleDamage` | 5 | 0 | 5 | 0 |
| `CEventVehicleDamageCollision` | 2 | 0 | 2 | 0 |
| `CEventVehicleDamageWeapon` | 2 | 0 | 2 | 0 |
| `CEventVehicleDied` | 3 | 0 | 3 | 0 |
| `CEventVehicleHitAndRun` | 3 | 0 | 3 | 0 |
| `CEventVehicleOnFire` | 3 | 0 | 3 | 0 |
| `CEventVehicleToSteal` | 2 | 0 | 2 | 0 |
| `CExplosion` | 12 | 0 | 12 | 0 |
| `CFallingGlassPane` | 2 | 0 | 2 | 0 |
| `CFileLoader` | 47 | 0 | 47 | 0 |
| `CFileMgr` | 17 | 0 | 17 | 0 |
| `CFire` | 5 | 0 | 5 | 0 |
| `CFireManager` | 24 | 0 | 24 | 0 |
| `CFont` | 32 | 0 | 0 | 32 |
| `CFormation` | 1 | 0 | 0 | 1 |
| `CGame` | 22 | 0 | 22 | 0 |
| `CGameLogic` | 27 | 0 | 27 | 0 |
| `CGamma` | 2 | 0 | 2 | 0 |
| `CGangWars` | 32 | 0 | 0 | 32 |
| `CGangs` | 8 | 0 | 8 | 0 |
| `CGarage` | 4 | 0 | 4 | 0 |
| `CGarages` | 21 | 0 | 21 | 0 |
| `CGeneral` | 9 | 0 | 9 | 0 |
| `CGenericGameStorage` | 5 | 0 | 0 | 5 |
| `CGlass` | 19 | 0 | 19 | 0 |
| `CGrassRenderer` | 12 | 0 | 12 | 0 |
| `CGridRef` | 5 | 0 | 5 | 0 |
| `CGroupEventHandler` | 30 | 0 | 0 | 30 |
| `CHandObject` | 3 | 0 | 3 | 0 |
| `CHandShaker` | 3 | 0 | 3 | 0 |
| `CHeli` | 11 | 0 | 11 | 0 |
| `CHud` | 30 | 0 | 0 | 30 |
| `CHudColours` | 5 | 0 | 5 | 0 |
| `CIdleCam` | 13 | 0 | 0 | 13 |
| `CInformFriendsEventQueue` | 4 | 0 | 4 | 0 |
| `CInformGroupEvent` | 3 | 0 | 3 | 0 |
| `CInformGroupEventQueue` | 4 | 0 | 4 | 0 |
| `CIniFile` | 1 | 0 | 1 | 0 |
| `CInterestingEvents` | 2 | 0 | 0 | 2 |
| `CIplDefPool` | 1 | 0 | 1 | 0 |
| `CIplStore` | 34 | 0 | 34 | 0 |
| `CKeyArray` | 4 | 0 | 4 | 0 |
| `CKeyGen` | 4 | 0 | 4 | 0 |
| `CKeyboardState` | 1 | 0 | 1 | 0 |
| `CLinkedUpgradeList` | 2 | 0 | 2 | 0 |
| `CLoadMonitor` | 7 | 0 | 7 | 0 |
| `CLoadedCarGroup` | 8 | 0 | 8 | 0 |
| `CLoadingScreen` | 20 | 0 | 20 | 0 |
| `CLocalisation` | 18 | 0 | 18 | 0 |
| `CLodAtomicModelInfo` | 3 | 0 | 3 | 0 |
| `CLodTimeModelInfo` | 2 | 0 | 2 | 0 |
| `CMaths` | 3 | 0 | 3 | 0 |
| `CMatrixLink` | 2 | 0 | 2 | 0 |
| `CMatrixLinkList` | 10 | 0 | 10 | 0 |
| `CMenuManager` | 61 | 0 | 61 | 0 |
| `CMenuSystem` | 21 | 0 | 21 | 0 |
| `CMessages` | 28 | 0 | 28 | 0 |
| `CMirrors` | 8 | 0 | 8 | 0 |
| `CMissionTextOffsets` | 1 | 0 | 1 | 0 |
| `CModelInfo` | 28 | 0 | 28 | 0 |
| `CMonsterTruck` | 5 | 0 | 0 | 5 |
| `CMotionBlurStreaks` | 4 | 0 | 4 | 0 |
| `CMouseControllerState` | 3 | 0 | 3 | 0 |
| `CMovingThings` | 5 | 0 | 5 | 0 |
| `CNodeRoute` | 2 | 0 | 2 | 0 |
| `CNodeRoutePool` | 1 | 0 | 1 | 0 |
| `CObject` | 48 | 0 | 48 | 0 |
| `CObjectData` | 2 | 0 | 2 | 0 |
| `CObjectPool` | 2 | 0 | 2 | 0 |
| `CObjectSaveStructure` | 2 | 0 | 2 | 0 |
| `COccluder` | 3 | 0 | 3 | 0 |
| `COcclusion` | 5 | 0 | 5 | 0 |
| `COnscreenCounterEntry` | 2 | 0 | 2 | 0 |
| `COnscreenTimer` | 6 | 0 | 6 | 0 |
| `CPPTriPlantBuffer` | 7 | 0 | 7 | 0 |
| `CPad` | 75 | 0 | 0 | 75 |
| `CPathFind` | 54 | 0 | 0 | 54 |
| `CPathNode` | 1 | 0 | 1 | 0 |
| `CPed` | 145 | 0 | 145 | 0 |
| `CPedAttractor` | 19 | 0 | 19 | 0 |
| `CPedAttractorManager` | 14 | 0 | 14 | 0 |
| `CPedAttractorPedPlacer` | 1 | 0 | 1 | 0 |
| `CPedAttractorPool` | 1 | 0 | 1 | 0 |
| `CPedClothesDesc` | 7 | 0 | 7 | 0 |
| `CPedDamageResponseCalculator` | 2 | 0 | 0 | 2 |
| `CPedGeometryAnalyser` | 9 | 0 | 0 | 9 |
| `CPedGroup` | 12 | 0 | 12 | 0 |
| `CPedGroupDefaultTaskAllocatorChat` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorFollowAnyMeans` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorFollowLimited` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorRandom` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorSitInLeaderCar` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorStandStill` | 2 | 0 | 2 | 0 |
| `CPedGroupIntelligence` | 26 | 0 | 26 | 0 |
| `CPedGroupMembership` | 18 | 0 | 18 | 0 |
| `CPedGroupPlacer` | 1 | 0 | 0 | 1 |
| `CPedGroups` | 2 | 0 | 2 | 0 |
| `CPedIK` | 9 | 0 | 9 | 0 |
| `CPedIntelligence` | 52 | 0 | 52 | 0 |
| `CPedIntelligencePool` | 1 | 0 | 1 | 0 |
| `CPedList` | 7 | 0 | 7 | 0 |
| `CPedModelInfo` | 9 | 0 | 9 | 0 |
| `CPedPlacement` | 3 | 0 | 3 | 0 |
| `CPedPool` | 2 | 0 | 2 | 0 |
| `CPedScriptedTaskRecord` | 5 | 0 | 5 | 0 |
| `CPedScriptedTaskRecordData` | 8 | 0 | 8 | 0 |
| `CPedShelterAttractor` | 4 | 0 | 4 | 0 |
| `CPedStats` | 4 | 0 | 4 | 0 |
| `CPedTaskPair` | 1 | 0 | 1 | 0 |
| `CPedToPlayerConversations` | 3 | 0 | 3 | 0 |
| `CPedType` | 13 | 0 | 13 | 0 |
| `CPhysical` | 56 | 0 | 56 | 0 |
| `CPickup` | 12 | 0 | 12 | 0 |
| `CPickups` | 35 | 0 | 0 | 35 |
| `CPlaceName` | 2 | 0 | 2 | 0 |
| `CPlaceable` | 16 | 0 | 16 | 0 |
| `CPlane` | 14 | 0 | 0 | 14 |
| `CPlaneTrail` | 3 | 0 | 3 | 0 |
| `CPlaneTrails` | 4 | 0 | 4 | 0 |
| `CPlantColEntEntry` | 2 | 0 | 2 | 0 |
| `CPlantLocTri` | 3 | 0 | 3 | 0 |
| `CPlantMgr` | 19 | 0 | 19 | 0 |
| `CPlantSurfPropMgr` | 4 | 0 | 4 | 0 |
| `CPlayerInfo` | 28 | 0 | 28 | 0 |
| `CPlayerPed` | 51 | 0 | 50 | 1 |
| `CPlayerPedData` | 5 | 0 | 5 | 0 |
| `CPlayerSkin` | 4 | 0 | 4 | 0 |
| `CPointRoutePool` | 1 | 0 | 1 | 0 |
| `CPools` | 16 | 0 | 0 | 16 |
| `CPopCycle` | 15 | 0 | 15 | 0 |
| `CPopulation` | 64 | 0 | 0 | 64 |
| `CPostEffects` | 35 | 0 | 0 | 35 |
| `CProjectileInfo` | 1 | 0 | 0 | 1 |
| `CPtrNodeDoubleLinkPool` | 1 | 0 | 1 | 0 |
| `CPtrNodeSingleLinkPool` | 1 | 0 | 1 | 0 |
| `CQuadBike` | 12 | 0 | 12 | 0 |
| `CQuadTreeNode<T>` | 12 | 0 | 12 | 0 |
| `CRadar` | 63 | 0 | 0 | 63 |
| `CRealTimeShadow` | 8 | 0 | 8 | 0 |
| `CRealTimeShadowManager` | 5 | 0 | 0 | 5 |
| `CRect` | 8 | 0 | 8 | 0 |
| `CReferences` | 4 | 0 | 4 | 0 |
| `CRegisteredCorona` | 1 | 0 | 1 | 0 |
| `CRegisteredMotionBlurStreak` | 2 | 0 | 2 | 0 |
| `CRenderer` | 33 | 0 | 33 | 0 |
| `CReplay` | 47 | 0 | 47 | 0 |
| `CRestart` | 11 | 0 | 11 | 0 |
| `CRoadBlocks` | 7 | 0 | 0 | 7 |
| `CRope` | 5 | 0 | 0 | 5 |
| `CRopes` | 11 | 0 | 11 | 0 |
| `CRunningScript` | 24 | 0 | 0 | 24 |
| `CScriptResourceManager` | 4 | 0 | 4 | 0 |
| `CScripted2dEffects` | 5 | 0 | 5 | 0 |
| `CScriptedBrainTaskEntry` | 1 | 0 | 1 | 0 |
| `CScriptedBrainTaskStore` | 4 | 0 | 4 | 0 |
| `CScriptsForBrains` | 6 | 0 | 6 | 0 |
| `CSetPiece` | 3 | 0 | 3 | 0 |
| `CSetPieces` | 5 | 0 | 5 | 0 |
| `CShadowCamera` | 19 | 0 | 19 | 0 |
| `CShadows` | 21 | 0 | 0 | 21 |
| `CShinyTexts` | 4 | 0 | 4 | 0 |
| `CShopping` | 30 | 0 | 30 | 0 |
| `CShotInfo` | 5 | 0 | 5 | 0 |
| `CSimpleVariablesSaveStructure` | 2 | 0 | 2 | 0 |
| `CSkidmarks` | 7 | 0 | 7 | 0 |
| `CSpecialFX` | 7 | 0 | 7 | 0 |
| `CSpecialPlateHandler` | 4 | 0 | 4 | 0 |
| `CSprite` | 15 | 0 | 15 | 0 |
| `CSprite2d` | 33 | 0 | 33 | 0 |
| `CStaticShadow` | 1 | 0 | 1 | 0 |
| `CStats` | 32 | 0 | 32 | 0 |
| `CStencilShadowObject` | 3 | 0 | 3 | 0 |
| `CStencilShadows` | 13 | 0 | 13 | 0 |
| `CStreamedScripts` | 12 | 0 | 12 | 0 |
| `CStreaming` | 94 | 0 | 0 | 94 |
| `CStreamingInfo` | 7 | 0 | 7 | 0 |
| `CStuckCarCheck` | 8 | 0 | 8 | 0 |
| `CStuntJumpManager` | 7 | 0 | 7 | 0 |
| `CTagManager` | 19 | 0 | 19 | 0 |
| `CTask` | 4 | 0 | 4 | 0 |
| `CTaskAllocator` | 3 | 0 | 3 | 0 |
| `CTaskAllocatorAttack` | 5 | 0 | 5 | 0 |
| `CTaskAllocatorKillOnFoot` | 4 | 0 | 4 | 0 |
| `CTaskAllocatorKillThreatsBasic` | 6 | 0 | 6 | 0 |
| `CTaskAllocatorKillThreatsBasicRandomGroup` | 2 | 0 | 2 | 0 |
| `CTaskAllocatorKillThreatsDriveby` | 5 | 0 | 5 | 0 |
| `CTaskAllocatorPlayerCommandAttack` | 3 | 0 | 3 | 0 |
| `CTaskAllocatorPool` | 1 | 0 | 1 | 0 |
| `CTaskComplexArrestPed` | 4 | 0 | 0 | 4 |
| `CTaskComplexAttractorPartnerWait` | 5 | 0 | 5 | 0 |
| `CTaskComplexAvoidOtherPedWhileWandering` | 15 | 0 | 15 | 0 |
| `CTaskComplexBeCop` | 3 | 0 | 3 | 0 |
| `CTaskComplexBeInCouple` | 9 | 0 | 9 | 0 |
| `CTaskComplexBeInGroup` | 10 | 0 | 10 | 0 |
| `CTaskComplexBeStill` | 6 | 0 | 6 | 0 |
| `CTaskComplexCarDrive` | 11 | 0 | 11 | 0 |
| `CTaskComplexCarDriveMission` | 5 | 0 | 5 | 0 |
| `CTaskComplexCarDriveMissionFleeScene` | 4 | 0 | 4 | 0 |
| `CTaskComplexCarDriveMissionKillPed` | 4 | 0 | 4 | 0 |
| `CTaskComplexCarDriveWander` | 5 | 0 | 5 | 0 |
| `CTaskComplexCarSlowBeDraggedOut` | 11 | 0 | 11 | 0 |
| `CTaskComplexCarSlowBeDraggedOutAndStandUp` | 9 | 0 | 9 | 0 |
| `CTaskComplexChat` | 7 | 0 | 7 | 0 |
| `CTaskComplexClimb` | 4 | 0 | 4 | 0 |
| `CTaskComplexCopInCar` | 9 | 0 | 9 | 0 |
| `CTaskComplexCrossRoadLookAndAchieveHeading` | 9 | 0 | 9 | 0 |
| `CTaskComplexDestroyCar` | 9 | 0 | 9 | 0 |
| `CTaskComplexDestroyCarArmed` | 10 | 0 | 10 | 0 |
| `CTaskComplexDestroyCarMelee` | 10 | 0 | 10 | 0 |
| `CTaskComplexDie` | 5 | 0 | 5 | 0 |
| `CTaskComplexDieInCar` | 6 | 0 | 6 | 0 |
| `CTaskComplexDiveFromAttachedEntityAndGetUp` | 7 | 0 | 7 | 0 |
| `CTaskComplexDragPedFromCar` | 2 | 0 | 2 | 0 |
| `CTaskComplexDriveFireTruck` | 6 | 0 | 6 | 0 |
| `CTaskComplexDrivePointRoute` | 7 | 0 | 7 | 0 |
| `CTaskComplexDriveToPoint` | 8 | 0 | 8 | 0 |
| `CTaskComplexEnterAnyCarAsDriver` | 5 | 0 | 5 | 0 |
| `CTaskComplexEnterBoatAsDriver` | 8 | 0 | 8 | 0 |
| `CTaskComplexEnterCar` | 14 | 0 | 14 | 0 |
| `CTaskComplexEnterCarAsDriver` | 3 | 0 | 3 | 0 |
| `CTaskComplexEnterCarAsDriverTimed` | 9 | 0 | 9 | 0 |
| `CTaskComplexEnterCarAsPassenger` | 3 | 0 | 3 | 0 |
| `CTaskComplexEnterCarAsPassengerTimed` | 9 | 0 | 9 | 0 |
| `CTaskComplexEnterCarAsPassengerWait` | 8 | 0 | 8 | 0 |
| `CTaskComplexEvasiveCower` | 8 | 0 | 8 | 0 |
| `CTaskComplexEvasiveDiveAndGetUp` | 9 | 0 | 9 | 0 |
| `CTaskComplexEvasiveStep` | 8 | 0 | 8 | 0 |
| `CTaskComplexExtinguishFires` | 8 | 0 | 8 | 0 |
| `CTaskComplexFacial` | 9 | 0 | 9 | 0 |
| `CTaskComplexFallAndGetUp` | 9 | 0 | 9 | 0 |
| `CTaskComplexFallToDeath` | 6 | 0 | 6 | 0 |
| `CTaskComplexFleeAnyMeans` | 8 | 0 | 8 | 0 |
| `CTaskComplexFleeEntity` | 8 | 0 | 8 | 0 |
| `CTaskComplexFleePoint` | 11 | 0 | 11 | 0 |
| `CTaskComplexFleeShooting` | 8 | 0 | 8 | 0 |
| `CTaskComplexFollowLeaderAnyMeans` | 8 | 0 | 8 | 0 |
| `CTaskComplexFollowLeaderInFormation` | 1 | 0 | 0 | 1 |
| `CTaskComplexFollowNodeRoute` | 19 | 0 | 19 | 0 |
| `CTaskComplexFollowPedFootsteps` | 1 | 0 | 1 | 0 |
| `CTaskComplexFollowPointRoute` | 13 | 0 | 13 | 0 |
| `CTaskComplexGangFollower` | 1 | 0 | 1 | 0 |
| `CTaskComplexGangJoinRespond` | 2 | 0 | 0 | 2 |
| `CTaskComplexGangLeader` | 13 | 0 | 13 | 0 |
| `CTaskComplexGetOnBoatSeat` | 8 | 0 | 8 | 0 |
| `CTaskComplexGetUpAndStandStill` | 5 | 0 | 5 | 0 |
| `CTaskComplexGoToAttractor` | 2 | 0 | 0 | 2 |
| `CTaskComplexGoToBoatSteeringWheel` | 6 | 0 | 6 | 0 |
| `CTaskComplexGoToCarDoorAndStandStill` | 11 | 0 | 11 | 0 |
| `CTaskComplexGoToPointAiming` | 8 | 0 | 8 | 0 |
| `CTaskComplexGoToPointAndStandStill` | 7 | 0 | 7 | 0 |
| `CTaskComplexGoToPointAndStandStillAndAchieveHeading` | 7 | 0 | 7 | 0 |
| `CTaskComplexGoToPointAndStandStillTimed` | 6 | 0 | 6 | 0 |
| `CTaskComplexGoToPointAnyMeans` | 7 | 0 | 7 | 0 |
| `CTaskComplexGoToPointShooting` | 3 | 0 | 3 | 0 |
| `CTaskComplexGotoDoorAndOpen` | 7 | 0 | 7 | 0 |
| `CTaskComplexHitByGunResponse` | 8 | 0 | 8 | 0 |
| `CTaskComplexHitPedWithCar` | 2 | 0 | 0 | 2 |
| `CTaskComplexHitResponse` | 7 | 0 | 7 | 0 |
| `CTaskComplexInAirAndLand` | 4 | 0 | 4 | 0 |
| `CTaskComplexInWater` | 7 | 0 | 7 | 0 |
| `CTaskComplexInvestigateDeadPed` | 9 | 0 | 9 | 0 |
| `CTaskComplexInvestigateDisturbance` | 8 | 0 | 8 | 0 |
| `CTaskComplexJump` | 6 | 0 | 6 | 0 |
| `CTaskComplexKillCriminal` | 9 | 0 | 0 | 9 |
| `CTaskComplexKillPedFromBoat` | 2 | 0 | 0 | 2 |
| `CTaskComplexKillPedGroupOnFoot` | 9 | 0 | 9 | 0 |
| `CTaskComplexKillPedOnFoot` | 1 | 0 | 1 | 0 |
| `CTaskComplexKillPedOnFootArmed` | 9 | 0 | 0 | 9 |
| `CTaskComplexKillPedOnFootMelee` | 10 | 0 | 10 | 0 |
| `CTaskComplexKillPedOnFootStealth` | 7 | 0 | 7 | 0 |
| `CTaskComplexLeaveAnyCar` | 7 | 0 | 7 | 0 |
| `CTaskComplexLeaveBoat` | 8 | 0 | 8 | 0 |
| `CTaskComplexLeaveCarAndDie` | 3 | 0 | 3 | 0 |
| `CTaskComplexLeaveCarAndFlee` | 8 | 0 | 8 | 0 |
| `CTaskComplexLeaveCarAndWander` | 8 | 0 | 8 | 0 |
| `CTaskComplexLeaveCarAsPassengerWait` | 10 | 0 | 10 | 0 |
| `CTaskComplexMedicTreatInjuredPed` | 9 | 0 | 9 | 0 |
| `CTaskComplexMoveBackAndJump` | 8 | 0 | 8 | 0 |
| `CTaskComplexObserveTrafficLights` | 8 | 0 | 8 | 0 |
| `CTaskComplexObserveTrafficLightsAndAchieveHeading` | 5 | 0 | 5 | 0 |
| `CTaskComplexOnFire` | 9 | 0 | 9 | 0 |
| `CTaskComplexOpenDriverDoor` | 4 | 0 | 4 | 0 |
| `CTaskComplexOpenPassengerDoor` | 4 | 0 | 4 | 0 |
| `CTaskComplexPartner` | 1 | 0 | 1 | 0 |
| `CTaskComplexPartnerChat` | 1 | 0 | 1 | 0 |
| `CTaskComplexPartnerDeal` | 1 | 0 | 1 | 0 |
| `CTaskComplexPartnerGreet` | 7 | 0 | 7 | 0 |
| `CTaskComplexPartnerShove` | 1 | 0 | 1 | 0 |
| `CTaskComplexPassObject` | 9 | 0 | 9 | 0 |
| `CTaskComplexPlayHandSignalAnim` | 3 | 0 | 0 | 3 |
| `CTaskComplexPolicePursuit` | 13 | 0 | 13 | 0 |
| `CTaskComplexProstituteSolicit` | 7 | 0 | 0 | 7 |
| `CTaskComplexReactToGunAimedAt` | 8 | 0 | 8 | 0 |
| `CTaskComplexRoadRage` | 9 | 0 | 9 | 0 |
| `CTaskComplexScreamInCarThenLeave` | 9 | 0 | 9 | 0 |
| `CTaskComplexSeekCoverUntilTargetDead` | 10 | 0 | 10 | 0 |
| `CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>` | 9 | 0 | 9 | 0 |
| `CTaskComplexSeekEntityAiming` | 7 | 0 | 7 | 0 |
| `CTaskComplexSeekEntityAnyMeansXYOffset` | 5 | 0 | 5 | 0 |
| `CTaskComplexSeekEntityRadiusAngleOffset` | 9 | 0 | 9 | 0 |
| `CTaskComplexSeekEntityStandard` | 9 | 0 | 9 | 0 |
| `CTaskComplexSequence` | 13 | 0 | 13 | 0 |
| `CTaskComplexShuffleSeats` | 10 | 0 | 10 | 0 |
| `CTaskComplexSignalAtPed` | 7 | 0 | 7 | 0 |
| `CTaskComplexSitDownThenIdleThenStandUp` | 9 | 0 | 9 | 0 |
| `CTaskComplexSmartFleeEntity` | 7 | 0 | 7 | 0 |
| `CTaskComplexSmartFleePoint` | 2 | 0 | 0 | 2 |
| `CTaskComplexStareAtPed` | 7 | 0 | 7 | 0 |
| `CTaskComplexStealCar` | 8 | 0 | 8 | 0 |
| `CTaskComplexStuckInAir` | 6 | 0 | 6 | 0 |
| `CTaskComplexSunbathe` | 2 | 0 | 0 | 2 |
| `CTaskComplexTrackEntity` | 11 | 0 | 11 | 0 |
| `CTaskComplexTreatAccident` | 7 | 0 | 7 | 0 |
| `CTaskComplexTurnToFaceEntityOrCoord` | 9 | 0 | 9 | 0 |
| `CTaskComplexUseAttractor` | 7 | 0 | 7 | 0 |
| `CTaskComplexUseAttractorPartner` | 7 | 0 | 7 | 0 |
| `CTaskComplexUseClosestFreeScriptedAttractor` | 8 | 0 | 8 | 0 |
| `CTaskComplexUseClosestFreeScriptedAttractorRun` | 4 | 0 | 4 | 0 |
| `CTaskComplexUseClosestFreeScriptedAttractorSprint` | 4 | 0 | 4 | 0 |
| `CTaskComplexUseEffect` | 8 | 0 | 8 | 0 |
| `CTaskComplexUseEffectRunning` | 2 | 0 | 2 | 0 |
| `CTaskComplexUseEffectSprinting` | 2 | 0 | 2 | 0 |
| `CTaskComplexUseGoggles` | 5 | 0 | 5 | 0 |
| `CTaskComplexUsePairedAttractor` | 11 | 0 | 11 | 0 |
| `CTaskComplexUseScriptedAttractor` | 7 | 0 | 7 | 0 |
| `CTaskComplexUseScriptedBrain` | 8 | 0 | 8 | 0 |
| `CTaskComplexUseSequence` | 6 | 0 | 6 | 0 |
| `CTaskComplexUseSwatRope` | 7 | 0 | 7 | 0 |
| `CTaskComplexUseWaterCannon` | 5 | 0 | 5 | 0 |
| `CTaskComplexWaitAtAttractor` | 7 | 0 | 7 | 0 |
| `CTaskComplexWaitForBackup` | 8 | 0 | 8 | 0 |
| `CTaskComplexWaitForBus` | 7 | 0 | 7 | 0 |
| `CTaskComplexWaitForDryWeather` | 7 | 0 | 7 | 0 |
| `CTaskComplexWalkAlongsidePed` | 9 | 0 | 9 | 0 |
| `CTaskComplexWalkRoundBuildingAttempt` | 12 | 0 | 12 | 0 |
| `CTaskComplexWalkRoundCar` | 13 | 0 | 13 | 0 |
| `CTaskComplexWalkRoundFire` | 8 | 0 | 8 | 0 |
| `CTaskComplexWalkRoundObject` | 1 | 0 | 1 | 0 |
| `CTaskComplexWander` | 14 | 0 | 14 | 0 |
| `CTaskComplexWanderCop` | 9 | 0 | 9 | 0 |
| `CTaskComplexWanderCriminal` | 6 | 0 | 6 | 0 |
| `CTaskComplexWanderFlee` | 6 | 0 | 6 | 0 |
| `CTaskComplexWanderGang` | 4 | 0 | 4 | 0 |
| `CTaskComplexWanderMedic` | 4 | 0 | 4 | 0 |
| `CTaskComplexWanderProstitute` | 5 | 0 | 5 | 0 |
| `CTaskComplexWanderStandard` | 3 | 0 | 0 | 3 |
| `CTaskGangHasslePed` | 2 | 0 | 0 | 2 |
| `CTaskGangHassleVehicle` | 2 | 0 | 0 | 2 |
| `CTaskGoToVehicleAndLean` | 2 | 0 | 0 | 2 |
| `CTaskInteriorBeInHouse` | 8 | 0 | 8 | 0 |
| `CTaskInteriorBeInOffice` | 8 | 0 | 8 | 0 |
| `CTaskInteriorBeInShop` | 6 | 0 | 6 | 0 |
| `CTaskInteriorGoToInfo` | 7 | 0 | 7 | 0 |
| `CTaskInteriorLieInBed` | 7 | 0 | 7 | 0 |
| `CTaskInteriorShopKeeper` | 7 | 0 | 7 | 0 |
| `CTaskInteriorSitAtDesk` | 9 | 0 | 9 | 0 |
| `CTaskInteriorUseInfo` | 8 | 0 | 8 | 0 |
| `CTaskLeanOnVehicle` | 5 | 0 | 5 | 0 |
| `CTaskManager` | 18 | 0 | 18 | 0 |
| `CTaskPool` | 1 | 0 | 1 | 0 |
| `CTaskSequences` | 2 | 0 | 2 | 0 |
| `CTaskSimpleAchieveHeading` | 5 | 0 | 5 | 0 |
| `CTaskSimpleAffectSecondaryBehaviour` | 5 | 0 | 5 | 0 |
| `CTaskSimpleAnim` | 1 | 0 | 1 | 0 |
| `CTaskSimpleArrestPed` | 3 | 0 | 3 | 0 |
| `CTaskSimpleBeHit` | 8 | 0 | 8 | 0 |
| `CTaskSimpleBikeJacked` | 8 | 0 | 8 | 0 |
| `CTaskSimpleCarAlign` | 10 | 0 | 10 | 0 |
| `CTaskSimpleCarCloseDoorFromInside` | 10 | 0 | 10 | 0 |
| `CTaskSimpleCarCloseDoorFromOutside` | 10 | 0 | 10 | 0 |
| `CTaskSimpleCarDrive` | 13 | 0 | 13 | 0 |
| `CTaskSimpleCarDriveTimed` | 5 | 0 | 5 | 0 |
| `CTaskSimpleCarFallOut` | 9 | 0 | 9 | 0 |
| `CTaskSimpleCarGetIn` | 9 | 0 | 9 | 0 |
| `CTaskSimpleCarGetOut` | 10 | 0 | 10 | 0 |
| `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse` | 6 | 0 | 6 | 0 |
| `CTaskSimpleCarJumpOut` | 10 | 0 | 10 | 0 |
| `CTaskSimpleCarOpenDoorFromOutside` | 10 | 0 | 10 | 0 |
| `CTaskSimpleCarOpenLockedDoorFromOutside` | 9 | 0 | 9 | 0 |
| `CTaskSimpleCarSetPedInAsDriver` | 5 | 0 | 5 | 0 |
| `CTaskSimpleCarSetPedInAsPassenger` | 5 | 0 | 5 | 0 |
| `CTaskSimpleCarSetPedOut` | 5 | 0 | 5 | 0 |
| `CTaskSimpleCarSetPedSlowDraggedOut` | 6 | 0 | 6 | 0 |
| `CTaskSimpleCarSetTempAction` | 6 | 0 | 6 | 0 |
| `CTaskSimpleCarShuffle` | 9 | 0 | 9 | 0 |
| `CTaskSimpleCarSlowBeDraggedOut` | 9 | 0 | 9 | 0 |
| `CTaskSimpleCarSlowDragPedOut` | 10 | 0 | 10 | 0 |
| `CTaskSimpleCarWaitForDoorNotToBeInUse` | 7 | 0 | 7 | 0 |
| `CTaskSimpleCarWaitToSlowDown` | 6 | 0 | 6 | 0 |
| `CTaskSimpleChat` | 3 | 0 | 3 | 0 |
| `CTaskSimpleChoking` | 7 | 0 | 7 | 0 |
| `CTaskSimpleClearLookAt` | 6 | 0 | 6 | 0 |
| `CTaskSimpleClimb` | 15 | 0 | 15 | 0 |
| `CTaskSimpleCower` | 4 | 0 | 4 | 0 |
| `CTaskSimpleCreateCarAndGetIn` | 6 | 0 | 6 | 0 |
| `CTaskSimpleDie` | 7 | 0 | 7 | 0 |
| `CTaskSimpleDieInCar` | 5 | 0 | 5 | 0 |
| `CTaskSimpleDoHandSignal` | 6 | 0 | 6 | 0 |
| `CTaskSimpleDrown` | 1 | 0 | 1 | 0 |
| `CTaskSimpleDrownInCar` | 1 | 0 | 1 | 0 |
| `CTaskSimpleDuck` | 15 | 0 | 15 | 0 |
| `CTaskSimpleDuckToggle` | 6 | 0 | 6 | 0 |
| `CTaskSimpleEvasiveDive` | 8 | 0 | 8 | 0 |
| `CTaskSimpleEvasiveStep` | 8 | 0 | 8 | 0 |
| `CTaskSimpleFacial` | 5 | 0 | 5 | 0 |
| `CTaskSimpleFall` | 6 | 0 | 6 | 0 |
| `CTaskSimpleFightingControl` | 6 | 0 | 0 | 6 |
| `CTaskSimpleFinishBrain` | 2 | 0 | 2 | 0 |
| `CTaskSimpleGangDriveBy` | 1 | 0 | 0 | 1 |
| `CTaskSimpleGetUp` | 5 | 0 | 5 | 0 |
| `CTaskSimpleGiveCPR` | 6 | 0 | 6 | 0 |
| `CTaskSimpleGoTo` | 4 | 0 | 4 | 0 |
| `CTaskSimpleGoToPoint` | 6 | 0 | 6 | 0 |
| `CTaskSimpleGoToPointFine` | 8 | 0 | 8 | 0 |
| `CTaskSimpleGunControl` | 6 | 0 | 6 | 0 |
| `CTaskSimpleHandsUp` | 4 | 0 | 4 | 0 |
| `CTaskSimpleHitByGunFromFront` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitByGunFromLeft` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitByGunFromRear` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitByGunFromRight` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitFromBack` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitFromBehind` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitFromFront` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitFromLeft` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitFromRight` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHitHead` | 4 | 0 | 4 | 0 |
| `CTaskSimpleHitWall` | 3 | 0 | 3 | 0 |
| `CTaskSimpleHoldEntity` | 14 | 0 | 14 | 0 |
| `CTaskSimpleHurtPedWithCar` | 6 | 0 | 6 | 0 |
| `CTaskSimpleIKChain` | 9 | 0 | 9 | 0 |
| `CTaskSimpleIKLookAt` | 8 | 0 | 8 | 0 |
| `CTaskSimpleIKManager` | 8 | 0 | 8 | 0 |
| `CTaskSimpleIKPointArm` | 6 | 0 | 6 | 0 |
| `CTaskSimpleInAir` | 4 | 0 | 4 | 0 |
| `CTaskSimpleJetPack` | 14 | 0 | 14 | 0 |
| `CTaskSimpleJump` | 8 | 0 | 8 | 0 |
| `CTaskSimpleKillPedWithCar` | 7 | 0 | 7 | 0 |
| `CTaskSimpleLand` | 6 | 0 | 6 | 0 |
| `CTaskSimpleLeaveGroup` | 2 | 0 | 2 | 0 |
| `CTaskSimpleLookAbout` | 3 | 0 | 3 | 0 |
| `CTaskSimpleNone` | 4 | 0 | 4 | 0 |
| `CTaskSimplePause` | 4 | 0 | 4 | 0 |
| `CTaskSimplePickUpBike` | 9 | 0 | 9 | 0 |
| `CTaskSimplePickUpEntity` | 4 | 0 | 4 | 0 |
| `CTaskSimplePlayHandSignalAnim` | 2 | 0 | 2 | 0 |
| `CTaskSimplePlayerOnFire` | 6 | 0 | 6 | 0 |
| `CTaskSimplePlayerOnFoot` | 11 | 0 | 0 | 11 |
| `CTaskSimplePutDownEntity` | 3 | 0 | 3 | 0 |
| `CTaskSimpleRunAnim` | 5 | 0 | 5 | 0 |
| `CTaskSimpleRunNamedAnim` | 7 | 0 | 7 | 0 |
| `CTaskSimpleRunTimedAnim` | 7 | 0 | 7 | 0 |
| `CTaskSimpleSay` | 6 | 0 | 6 | 0 |
| `CTaskSimpleScratchHead` | 2 | 0 | 2 | 0 |
| `CTaskSimpleSetCharDecisionMaker` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetCharIgnoreWeaponRangeFlag` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetKindaStayInSamePlace` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetPedAsAutoDriver` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetStayInSamePlace` | 5 | 0 | 5 | 0 |
| `CTaskSimpleShakeFist` | 8 | 0 | 8 | 0 |
| `CTaskSimpleSitDown` | 8 | 0 | 8 | 0 |
| `CTaskSimpleSitIdle` | 7 | 0 | 7 | 0 |
| `CTaskSimpleStandStill` | 3 | 0 | 3 | 0 |
| `CTaskSimpleStandUp` | 8 | 0 | 8 | 0 |
| `CTaskSimpleStealthKill` | 7 | 0 | 7 | 0 |
| `CTaskSimpleSwim` | 11 | 0 | 11 | 0 |
| `CTaskSimpleThrowControl` | 6 | 0 | 6 | 0 |
| `CTaskSimpleThrowProjectile` | 9 | 0 | 9 | 0 |
| `CTaskSimpleTired` | 3 | 0 | 3 | 0 |
| `CTaskSimpleTogglePedThreatScanner` | 5 | 0 | 5 | 0 |
| `CTaskSimpleTurn180` | 6 | 0 | 6 | 0 |
| `CTaskSimpleUninterruptable` | 1 | 0 | 1 | 0 |
| `CTaskSimpleUseAtm` | 4 | 0 | 4 | 0 |
| `CTaskSimpleUseGun` | 19 | 0 | 19 | 0 |
| `CTaskSimpleWaitForBus` | 6 | 0 | 6 | 0 |
| `CTaskSimpleWaitUntilAreaCodesMatch` | 6 | 0 | 6 | 0 |
| `CTaskSimpleWaitUntilLeaderAreaCodesMatch` | 4 | 0 | 4 | 0 |
| `CTaskSimpleWaitUntilPedIsInCar` | 6 | 0 | 6 | 0 |
| `CTaskSimpleWaitUntilPedIsOutCar` | 6 | 0 | 6 | 0 |
| `CTaskTimer` | 1 | 0 | 1 | 0 |
| `CTaskUtilityLineUpPedWithCar` | 2 | 0 | 0 | 2 |
| `CTempColModels` | 2 | 0 | 2 | 0 |
| `CText` | 9 | 0 | 9 | 0 |
| `CTheCarGenerators` | 7 | 0 | 7 | 0 |
| `CTheScripts` | 69 | 0 | 0 | 69 |
| `CTheZones` | 21 | 0 | 21 | 0 |
| `CTimeCycle` | 20 | 0 | 20 | 0 |
| `CTimeInfo` | 6 | 0 | 6 | 0 |
| `CTimeModelInfo` | 2 | 0 | 2 | 0 |
| `CTimer` | 13 | 0 | 13 | 0 |
| `CTrafficLights` | 12 | 0 | 12 | 0 |
| `CTrailer` | 13 | 0 | 13 | 0 |
| `CTrain` | 20 | 0 | 0 | 20 |
| `CTxdStore` | 26 | 0 | 26 | 0 |
| `CUpsideDownCarCheck` | 5 | 0 | 5 | 0 |
| `CUserDisplay` | 2 | 0 | 2 | 0 |
| `CVehicle` | 142 | 0 | 142 | 0 |
| `CVehicleAnimGroup` | 5 | 0 | 5 | 0 |
| `CVehicleAnimGroupData` | 5 | 0 | 5 | 0 |
| `CVehicleModelInfo` | 75 | 0 | 0 | 75 |
| `CVehiclePool` | 1 | 0 | 1 | 0 |
| `CVehicleRecording` | 29 | 0 | 29 | 0 |
| `CVehicleSaveStructure` | 2 | 0 | 2 | 0 |
| `CVisibilityPlugins` | 84 | 0 | 0 | 84 |
| `CWanted` | 33 | 0 | 33 | 0 |
| `CWaterCannon` | 5 | 0 | 5 | 0 |
| `CWaterCannons` | 4 | 0 | 4 | 0 |
| `CWaterLevel` | 25 | 0 | 0 | 25 |
| `CWeapon` | 35 | 0 | 0 | 35 |
| `CWeaponEffects` | 9 | 0 | 9 | 0 |
| `CWeaponInfo` | 10 | 0 | 10 | 0 |
| `CWeaponModelInfo` | 3 | 0 | 3 | 0 |
| `CWeather` | 13 | 0 | 13 | 0 |
| `CWindModifiers` | 2 | 0 | 2 | 0 |
| `CWorld` | 94 | 0 | 0 | 94 |
| `C_PcSave` | 5 | 0 | 5 | 0 |
| `CdStream` | 9 | 0 | 9 | 0 |
| `Common` | 9 | 0 | 9 | 0 |
| `FurnitureGroup_c` | 6 | 0 | 6 | 0 |
| `FurnitureManager_c` | 7 | 0 | 7 | 0 |
| `FurnitureSubGroup_c` | 3 | 0 | 3 | 0 |
| `FxEmitterBP_c` | 2 | 0 | 0 | 2 |
| `FxEmitterPrt_c` | 1 | 0 | 1 | 0 |
| `FxInfoManager_c` | 4 | 0 | 4 | 0 |
| `FxManager_c` | 25 | 0 | 25 | 0 |
| `FxMemoryPool_c` | 5 | 0 | 5 | 0 |
| `FxPrimBP_c` | 2 | 0 | 2 | 0 |
| `FxSphere_c` | 4 | 0 | 4 | 0 |
| `FxSystemBP_c` | 10 | 0 | 10 | 0 |
| `FxSystem_c` | 31 | 0 | 0 | 31 |
| `Fx_c` | 8 | 0 | 8 | 0 |
| `Game` | 12 | 0 | 12 | 0 |
| `IKChainManager_c` | 17 | 0 | 17 | 0 |
| `IKChain_c` | 20 | 0 | 20 | 0 |
| `Input` | 5 | 0 | 5 | 0 |
| `InteriorGroup_c` | 2 | 0 | 0 | 2 |
| `InteriorManager_c` | 21 | 0 | 21 | 0 |
| `JPegPlugin` | 5 | 0 | 5 | 0 |
| `Light` | 27 | 0 | 27 | 0 |
| `ModelIndices` | 2 | 0 | 2 | 0 |
| `NodeNamePlugin` | 3 | 0 | 3 | 0 |
| `PipelinePlugin` | 3 | 0 | 3 | 0 |
| `ProcObjectMan_c` | 8 | 0 | 8 | 0 |
| `ProcSurfaceInfo_c` | 4 | 0 | 4 | 0 |
| `Ps` | 22 | 0 | 22 | 0 |
| `RenderBuffer` | 4 | 0 | 4 | 0 |
| `RpAnimBlend` | 43 | 0 | 43 | 0 |
| `Rs` | 27 | 0 | 27 | 0 |
| `RtAnim` | 1 | 0 | 1 | 0 |
| `RwCore` | 4 | 0 | 4 | 0 |
| `RwHelper` | 24 | 0 | 24 | 0 |
| `Securom` | 2 | 0 | 2 | 0 |
| `SurfaceInfos_c` | 47 | 0 | 47 | 0 |
| `VideoMode` | 4 | 0 | 4 | 0 |
| `VideoPlayer` | 6 | 0 | 6 | 0 |
| `WaterCreatureManager_c` | 7 | 0 | 7 | 0 |
| `WaterCreature_c` | 3 | 0 | 3 | 0 |
| `Win` | 6 | 0 | 5 | 1 |
| `cBuoyancy` | 9 | 0 | 9 | 0 |
| `cHandlingDataMgr` | 11 | 0 | 11 | 0 |
| `cTransmission` | 4 | 0 | 4 | 0 |

</details>

Regenerar desde la raíz: `python contrib/validation_inventory.py`. El informe no cambia hooks.csv ni acredita funciones por sí mismo.
