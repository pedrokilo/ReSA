# Inventario de validación de funciones reconstruidas

[Panel](ReversedClasses.md) · [Registro y método](../reversing/ValidationRegistry/README.md)

Universo de esta revisión: **7,857 entradas reversed=1**, de 8,440 registradas. Las 583 pendientes de reconstrucción quedan fuera de este inventario, pero siguen dentro del denominador del panel.

| Estado | Entradas |
| --- | ---: |
| `validated=1` · evidencia aprobada vinculada | 1901 |
| Vigencia acreditada para su alcance | 0 |
| Evidencia histórica vinculada; revisión pendiente | 1901 |
| Sin evidencia aprobada vinculada en este registro | 5956 |

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
| CAEMP3TrackLoader | `Constructor` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `Deconstructor` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `GetTrackInfo` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `Initialise` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `IsCurrentAudioStreamAvailable` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `LoadStreamPackTable` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEMP3TrackLoader | `LoadTrackLookupTable` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEPedWeaponAudioEntity | `AddAudioEvent` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAEPedWeaponAudioEntity | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAEPedWeaponAudioEntity | `Service` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAEPedWeaponAudioEntity | `Terminate` | [Resumen](../reversing/Data23/validation-summary.json) |
| CAESound | `operator=` | [Resumen](../reversing/CAESound/validation-summary.json) |
| CAEStaticChannel | `SetAudioBuffer` | [Resumen](../reversing/AudioCompletion/validation-summary.json) |
| CAEStreamTransformer | `Initialise` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CAEStreamTransformer | `TransformBuffer` | [Resumen](../reversing/Misc70/validation-summary.json) |
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
| CAEWaterCannonAudioEntity | `ClearSplashInfo` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Constructor` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Initialise` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Service` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `SetSplashInfo` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `Terminate` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CAEWaterCannonAudioEntity | `UpdateParameters` | [Resumen](../reversing/Audio7/validation-summary.json) |
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
| CAnimBlendClumpData | `Constructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `Destructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `ForAllFrames` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `ForAllFramesInSPR` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `LoadFramesIntoSPR` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendClumpData | `SetNumberOfBones` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `AllocateSequenceArray` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Constructor1` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Constructor2` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Destructor` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `FreeSequenceArray` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimBlendStaticAssociation | `Init` | [Resumen](../reversing/AnimAudio6/validation-summary.json) |
| CAnimatedBuilding | `PreRender` | [Resumen](../reversing/World52/validation-summary.json) |
| CAnimatedBuilding | `ProcessCollision` | [Resumen](../reversing/World52/validation-summary.json) |
| CAnimatedBuilding | `ProcessControl` | [Resumen](../reversing/World52/validation-summary.json) |
| CAudioZones | `Init` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `RegisterAudioBox` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `RegisterAudioSphere` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `SwitchAudioZone` | [Resumen](../reversing/World43/validation-summary.json) |
| CAudioZones | `Update` | [Resumen](../reversing/World43/validation-summary.json) |
| CBaseModelInfo | `SwaysInWind` | [Resumen](../reversing/CBaseModelInfo/validation-summary.json) |
| CBirds | `CreateNumberOfBirds` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `HandleGunShot` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Init` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Render` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Shutdown` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CBirds | `Update` | [Resumen](../reversing/Misc6/validation-summary.json) |
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
| CCheckpoint | `Render` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CCivilianPed | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CCivilianPed | `Destructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CCivilianPed | `ProcessControl` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CClothes | `GetTextureDependency` | [Resumen](../reversing/CClothes/validation-summary.json) |
| CClouds | `RenderBottomFromHeight` | [Resumen](../reversing/CClouds/validation-summary.json) |
| CColLine | `Set` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CColModelPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CColSphere | `IntersectEdge` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `IntersectPoint` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `IntersectRay` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `IntersectSphere` | [Resumen](../reversing/World43/validation-summary.json) |
| CColSphere | `Set` | [Resumen](../reversing/World43/validation-summary.json) |
| CColTrianglePlane | `GetNormal-Out` | [Resumen](../reversing/Core70/validation-summary.json) |
| CColTrianglePlane | `Set` | [Resumen](../reversing/Core70/validation-summary.json) |
| CCollisionPlugin | `PluginAttach` | [Resumen](../reversing/Core70/validation-summary.json) |
| CCollisionPlugin | `SetModelInfo` | [Resumen](../reversing/Core70/validation-summary.json) |
| CColourSet | `Constructor` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CColourSet | `Interpolate` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCompressedMatrixNotAligned | `CompressFromFullMatrix` | [Resumen](../reversing/Core70/validation-summary.json) |
| CCompressedMatrixNotAligned | `DecompressIntoFullMatrix` | [Resumen](../reversing/Core70/validation-summary.json) |
| CControllerState | `CheckForInput` | [Resumen](../reversing/Core70/validation-summary.json) |
| CControllerState | `Clear` | [Resumen](../reversing/Core70/validation-summary.json) |
| CConversationForPed | `IsPlayerInPositionForConversation` | [Resumen](../reversing/ConversationForPed/validation-summary.json) |
| CConversationForPed | `Update` | [Resumen](../reversing/ConversationForPed/validation-summary.json) |
| CConversationNode | `ClearRecursively` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CConversations | `DoneSettingUpConversation` | [Resumen](../reversing/CConversations/validation-summary.json) |
| CCopPed | `AddCriminalToKill` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `ClearCriminalsToKill` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `Constructor` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `Destructor` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `ProcessControl` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `RemoveCriminalToKill` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCopPed | `SetPartner` | [Resumen](../reversing/Data7/validation-summary.json) |
| CCoverPoint | `CanAccommodateAnotherPed` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
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
| CCurrentVehicle | `Display` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCurrentVehicle | `Process` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CCurves | `CalcCorrectedDist` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `CalcCurvePoint` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `CalcSpeedScaleFactor` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `CalcSpeedVariationInBend` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `DistForLineToCrossOtherLine` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCurves | `TestCurves` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CCustomBuildingPipeline | `CreateCustomObjPipe` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CreatePipe` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeAtomicSetup` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeMaterialSetup` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeReinstanceCB` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `CustomPipeRenderCB` | [Resumen](../reversing/Audio7/validation-summary.json) |
| CCustomBuildingPipeline | `DestroyPipe` | [Resumen](../reversing/Audio7/validation-summary.json) |
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
| CEntityScanner | `Clear` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntityScanner | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntityScanner | `Destructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntityScanner | `ScanForEntitiesInRange` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEntitySeekPosCalculatorFixedPos | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CEntitySeekPosCalculatorRadiusAngleOffset | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CEntitySeekPosCalculatorStandard | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CEntitySeekPosCalculatorXYOffset | `ComputeEntitySeekPos` | [Resumen](../reversing/Misc56/validation-summary.json) |
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
| CEventDanger | `AffectsPed` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDanger | `AffectsPedGroup` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDanger | `Constructor` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDanger | `GetSourceEntity` | [Resumen](../reversing/Events23/validation-summary.json) |
| CEventDontJoinPlayerGroup | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventEscalator | `AffectsPed` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CEventGlobalGroup | `AddEventsToGroup` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventGlobalGroup | `AddEventsToPed` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventGlobalGroup | `GetSoundLevel` | [Resumen](../reversing/Events52/validation-summary.json) |
| CEventGunShot | `AffectsPed` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `CloneEditable` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `Constructor` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `IsCriminalEvent` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShot | `TakesPriorityOver` | [Resumen](../reversing/Events43/validation-summary.json) |
| CEventGunShotWhizzedBy | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventGunShotWhizzedBy | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventHitByWaterCannon | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventHitByWaterCannon | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventInAir | `AffectsPed` | [Resumen](../reversing/Events70/validation-summary.json) |
| CEventInAir | `Constructor` | [Resumen](../reversing/Events70/validation-summary.json) |
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
| CFallingGlassPane | `Render` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CFallingGlassPane | `Update` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CFire | `Constructor` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `CreateFxSysForStrength` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `Extinguish` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `Initialise` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CFire | `ProcessFire` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CGamma | `Init` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CGamma | `SetGamma` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CGarage | `BuildRotatedDoorMatrix` | [Resumen](../reversing/Data23/validation-summary.json) |
| CGarage | `CloseThisGarage` | [Resumen](../reversing/Data23/validation-summary.json) |
| CGarage | `OpenThisGarage` | [Resumen](../reversing/Data23/validation-summary.json) |
| CGarage | `StoreAndRemoveCarsForThisHideOut` | [Resumen](../reversing/Data23/validation-summary.json) |
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
| CLoadedCarGroup | `PickRandomCar` | [Resumen](../reversing/SmallBatch/validation-summary.json) |
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
| CPed | `CalculateNewVelocity` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `DoFootLanded` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `KillPedWithCar` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `MakeTyresMuddySectorList<CPtrListSingleLink<CPhysical*>>` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `PlayFootSteps` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPed | `PositionPedOutOfCollision` | [Resumen](../reversing/CPed/validation-summary.json) |
| CPedAttractorPedPlacer | `PlacePedAtEffect` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CPedAttractorPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPedClothesDesc | `Constructor` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `GetIsWearingBalaclava` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `HasVisibleNewHairCut` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `HasVisibleTattoo` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `Initialise` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `SetModel-modelHash` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedClothesDesc | `SetModel-modelName` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorChat | `AllocateDefaultTasks` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorChat | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | `AllocateDefaultTasks` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowLimited | `AllocateDefaultTasks` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorFollowLimited | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorRandom | `AllocateDefaultTasks` | [Resumen](../reversing/GroupTasksCompletion/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorRandom | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | `AllocateDefaultTasks` | [Resumen](../reversing/GroupTasksCompletion/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorStandStill | `AllocateDefaultTasks` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroupDefaultTaskAllocatorStandStill | `GetType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroups | `Process` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedGroups | `RemoveGroup` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CPedIntelligencePool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPedList | `BuildListFromGroup_NoLeader` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedList | `BuildListFromGroup_NotInCar_NoLeader` | [Resumen](../reversing/CPedList/validation-summary.json) |
| CPedList | `BuildListOfPedsOfPedType` | [Resumen](../reversing/CPedList/validation-summary.json) |
| CPedList | `Empty` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedList | `ExtractPedsWithGuns` | [Resumen](../reversing/Data7/validation-summary.json) |
| CPedList | `RemovePedsAttackingPedType` | [Resumen](../reversing/CPedList/validation-summary.json) |
| CPedList | `RemovePedsThatDontListenToPlayer` | [Resumen](../reversing/CPedList/validation-summary.json) |
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
| CPedStats | `GetPedStatType` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedStats | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedStats | `LoadPedStats` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedStats | `Shutdown` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPedTaskPair | `Flush` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPedToPlayerConversations | `Clear` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CPedToPlayerConversations | `EndConversation` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CPedToPlayerConversations | `Update` | [Resumen](../reversing/PedToPlayerConversations/validation-summary.json) |
| CPlaceName | `GetForMap` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CPlaceName | `Process` | [Resumen](../reversing/Misc70/validation-summary.json) |
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
| CPlantSurfPropMgr | `AllocSurfProperties` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlantSurfPropMgr | `GetSurfProperties` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlantSurfPropMgr | `Initialise` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlantSurfPropMgr | `LoadPlantsDat` | [Resumen](../reversing/Data23/validation-summary.json) |
| CPlayerInfo | `FindClosestCarSectorList` | [Resumen](../reversing/CPlayerInfo/validation-summary.json) |
| CPlayerInfo | `FindObjectToSteal` | [Resumen](../reversing/CPlayerInfo/validation-summary.json) |
| CPlayerInfo | `Process` | [Resumen](../reversing/CPlayerInfo/validation-summary.json) |
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
| CPtrNodeDoubleLinkPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CPtrNodeSingleLinkPool | `New` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CReferences | `Init` | [Resumen](../reversing/Events23/validation-summary.json) |
| CReferences | `ListSize` | [Resumen](../reversing/Events23/validation-summary.json) |
| CReferences | `PruneAllReferencesInWorld` | [Resumen](../reversing/Events23/validation-summary.json) |
| CReferences | `RemoveReferencesToPlayer` | [Resumen](../reversing/Events23/validation-summary.json) |
| CRegisteredCorona | `Update` | [Resumen](../reversing/Misc56/validation-summary.json) |
| CRegisteredMotionBlurStreak | `Render` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CRegisteredMotionBlurStreak | `Update` | [Resumen](../reversing/Misc70/validation-summary.json) |
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
| CSetPiece | `TryToGenerateCopCar` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CSetPiece | `TryToGenerateCopPed` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CSetPiece | `Update` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CSetPieces | `AddOne` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Init` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Load` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Save` | [Resumen](../reversing/World43/validation-summary.json) |
| CSetPieces | `Update` | [Resumen](../reversing/World43/validation-summary.json) |
| CShinyTexts | `Init` | [Resumen](../reversing/Render23/validation-summary.json) |
| CShinyTexts | `RegisterOne` | [Resumen](../reversing/Render23/validation-summary.json) |
| CShinyTexts | `Render` | [Resumen](../reversing/Render23/validation-summary.json) |
| CShinyTexts | `RenderOutGeometryBuffer` | [Resumen](../reversing/Render23/validation-summary.json) |
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
| CStaticShadow | `Free` | [Resumen](../reversing/Pools56/validation-summary.json) |
| CStencilShadowObject | `Constructor` | [Resumen](../reversing/World52/validation-summary.json) |
| CStencilShadowObject | `Destroy` | [Resumen](../reversing/World52/validation-summary.json) |
| CStencilShadowObject | `Destructor` | [Resumen](../reversing/World52/validation-summary.json) |
| CStreamingInfo | `AddToList` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `GetCdPosn` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `GetCdPosnAndSize` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `InList` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `Init` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `RemoveFromList` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStreamingInfo | `SetCdPosnAndSize` | [Resumen](../reversing/Data7/validation-summary.json) |
| CStuntJumpManager | `AddOne` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Init` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Load` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Save` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Shutdown` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `ShutdownForRestart` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CStuntJumpManager | `Update` | [Resumen](../reversing/Mgr7/validation-summary.json) |
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
| CTaskComplexBeStill | `Clone` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `CreateNextSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `Destructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexBeStill | `GetTaskType` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
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
| CTaskComplexDiveFromAttachedEntityAndGetUp | `ControlSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
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
| CTaskComplexEnterAnyCarAsDriver | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `ControlSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterAnyCarAsDriver | `GetTaskType` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexEnterCarAsDriver | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsDriver | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsDriver | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsPassenger | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsPassenger | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexEnterCarAsPassenger | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexFallToDeath | `CalcFall` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `Constructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `CreateNextSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFallToDeath | `MakeAbortable` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexFollowPedFootsteps | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexGangFollower | `SetCanEnterLeadersCar` | [Resumen](../reversing/GroupTasksCompletion/validation-summary.json) |
| CTaskComplexGangLeader | `GetTaskType` | [Resumen](../reversing/GangLeader/validation-summary.json) |
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
| CTaskComplexGoToPointAndStandStill | `Clone` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `Constructor` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `ControlSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `CreateFirstSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `CreateNextSubTask` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `GetTaskType` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStill | `GoToPoint` | [Resumen](../reversing/CTasks7a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `Clone` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `Constructor` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `ControlSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `MakeAbortable` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointAndStandStillTimed | `StopTimer` | [Resumen](../reversing/CTasks6a/validation-summary.json) |
| CTaskComplexGoToPointShooting | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexGoToPointShooting | `Constructor` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexGoToPointShooting | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
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
| CTaskComplexJump | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `CreateFirstSubTask` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `CreateNextSubTask` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `CreateSubTask` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexJump | `MakeAbortable` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexKillPedOnFoot | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
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
| CTaskComplexLeaveCarAndDie | `Clone` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexLeaveCarAndDie | `CreateFirstSubTask` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexLeaveCarAndDie | `GetTaskType` | [Resumen](../reversing/PedTasks52/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `Clone` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `ControlSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `CreateFirstSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `CreateNextSubTask` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | `MakeAbortable` | [Resumen](../reversing/CTasks43/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `Clone` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `Constructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `Destructor` | [Resumen](../reversing/CTasks19/validation-summary.json) |
| CTaskComplexOpenDriverDoor | `GetTaskType` | [Resumen](../reversing/CTasks19/validation-summary.json) |
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
| CTaskComplexWalkRoundObject | `Constructor` | [Resumen](../reversing/EventsTasks56/validation-summary.json) |
| CTaskComplexWander | `CreateNextSubTask` | [Resumen](../reversing/CTaskComplexWander/validation-summary.json) |
| CTaskComplexWanderFlee | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexWanderFlee | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskComplexWanderFlee | `ControlSubTask` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
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
| CTaskInteriorLieInBed | `Constructor` | [Resumen](../reversing/Mgr7/validation-summary.json) |
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
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `Clone` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `Constructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `Destructor` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `GetTaskType` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `MakeAbortable` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | `ProcessPed` | [Resumen](../reversing/CTasks6b/validation-summary.json) |
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
| CTaskSimpleClearLookAt | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `Destructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `GetTaskType` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleClearLookAt | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
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
| CTaskSimpleDuckToggle | `Clone` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `Constructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `Destructor` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `GetTaskType` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `MakeAbortable` | [Resumen](../reversing/STasks6a/validation-summary.json) |
| CTaskSimpleDuckToggle | `ProcessPed` | [Resumen](../reversing/STasks6a/validation-summary.json) |
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
| CTaskSimpleHurtPedWithCar | `Clone` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `Constructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `Destructor` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `GetTaskType` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `MakeAbortable` | [Resumen](../reversing/STasks6b/validation-summary.json) |
| CTaskSimpleHurtPedWithCar | `ProcessPed` | [Resumen](../reversing/STasks6b/validation-summary.json) |
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
| CTaskSimpleJetPack | `DropJetPack` | [Resumen](../reversing/CTaskSimpleJetPack/validation-summary.json) |
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
| CTaskSimpleRunNamedAnim | `Constructor-Anim` | [Resumen](../reversing/ValidationRegistry/namedanim-validation.json) |
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
| CTaskSimpleStealthKill | `Clone` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `Constructor` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `FinishAnimStealthKillCB` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `GetTaskType` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `MakeAbortable` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `ManageAnim` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleStealthKill | `ProcessPed` | [Resumen](../reversing/STasks7/validation-summary.json) |
| CTaskSimpleThrowControl | `Clone` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `Constructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `Destructor` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `GetTaskType` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `MakeAbortable` | [Resumen](../reversing/STasks6c/validation-summary.json) |
| CTaskSimpleThrowControl | `ProcessPed` | [Resumen](../reversing/STasks6c/validation-summary.json) |
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
| CTaskSimpleUseGun | `ProcessPed` | [Resumen](../reversing/CTaskSimpleUseGun/validation-summary.json) |
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
| CTheCarGenerators | `CreateCarGenerator` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Get` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Init` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Load` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Process` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `RemoveCarGenerators` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTheCarGenerators | `Save` | [Resumen](../reversing/Mgr7/validation-summary.json) |
| CTimeInfo | `FindOtherTimeModel` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `GetOtherTimeModel` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `GetTimeOff` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `GetTimeOn` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `SetOtherTimeModel` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeInfo | `SetTimes` | [Resumen](../reversing/Misc6/validation-summary.json) |
| CTimeModelInfo | `GetModelType` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CTimeModelInfo | `GetTimeInfo` | [Resumen](../reversing/Tasks70/validation-summary.json) |
| CUpsideDownCarCheck | `AddCarToCheck` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `HasCarBeenUpsideDownForAWhile` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `IsCarUpsideDown-vehicle` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `RemoveCarFromCheck` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUpsideDownCarCheck | `UpdateTimers` | [Resumen](../reversing/Events43/validation-summary.json) |
| CUserDisplay | `Init` | [Resumen](../reversing/Misc70/validation-summary.json) |
| CUserDisplay | `Process` | [Resumen](../reversing/Misc70/validation-summary.json) |
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
| CVehicleSaveStructure | `Construct` | [Resumen](../reversing/Core70/validation-summary.json) |
| CVehicleSaveStructure | `Extract` | [Resumen](../reversing/Core70/validation-summary.json) |
| CWaterCannon | `Init` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `PushPeds` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `Render` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `Update_NewInput` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannon | `Update_OncePerFrame` | [Resumen](../reversing/Fx43/validation-summary.json) |
| CWaterCannons | `Init` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWaterCannons | `Render` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWaterCannons | `Update` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWaterCannons | `UpdateOne` | [Resumen](../reversing/Misc8/validation-summary.json) |
| CWeaponModelInfo | `GetModelType` | [Resumen](../reversing/World52/validation-summary.json) |
| CWeaponModelInfo | `Init` | [Resumen](../reversing/World52/validation-summary.json) |
| CWeaponModelInfo | `SetClump` | [Resumen](../reversing/World52/validation-summary.json) |
| CWindModifiers | `FindWindModifier` | [Resumen](../reversing/Core70/validation-summary.json) |
| CWindModifiers | `RegisterOne` | [Resumen](../reversing/Core70/validation-summary.json) |
| C_PcSave | `DeleteSlot` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `GenerateGameFilename` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `PopulateSlotInfo` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `SaveSlot` | [Resumen](../reversing/PcSave/validation-summary.json) |
| C_PcSave | `SetSaveDirectory` | [Resumen](../reversing/PcSave/validation-summary.json) |
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
| FxManager_c | `DestroyAllFxSystems` | [Resumen](../reversing/FxManager/validation-summary.json) |
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
| IKChain_c | `SetupBones` | [Resumen](../reversing/ValidationRegistry/ik-validation.json) |
| Input | `AttachInputDevices` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `HandleKeyDown` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `HandleKeyUp` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `KeyboardHandler` | [Resumen](../reversing/Fx43/validation-summary.json) |
| Input | `PadHandler` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegCompressScreen` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegCompressScreenToBuffer` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegCompressScreenToFile` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegDecompressToRaster` | [Resumen](../reversing/Fx43/validation-summary.json) |
| JPegPlugin | `JPegDecompressToVramFromBuffer` | [Resumen](../reversing/Fx43/validation-summary.json) |
| ModelIndices | `InitModelIndices` | [Resumen](../reversing/Core70/validation-summary.json) |
| ModelIndices | `MatchAllModelStrings` | [Resumen](../reversing/Core70/validation-summary.json) |
| NodeNamePlugin | `GetFrameNodeName` | [Resumen](../reversing/World52/validation-summary.json) |
| NodeNamePlugin | `NodeNamePluginAttach` | [Resumen](../reversing/World52/validation-summary.json) |
| NodeNamePlugin | `SetFrameNodeName` | [Resumen](../reversing/World52/validation-summary.json) |
| PipelinePlugin | `GetPipelineID` | [Resumen](../reversing/World52/validation-summary.json) |
| PipelinePlugin | `PipelinePluginAttach` | [Resumen](../reversing/World52/validation-summary.json) |
| PipelinePlugin | `SetPipelineID` | [Resumen](../reversing/World52/validation-summary.json) |
| ProcSurfaceInfo_c | `AddObject` | [Resumen](../reversing/Misc8/validation-summary.json) |
| ProcSurfaceInfo_c | `AddObjects` | [Resumen](../reversing/Misc8/validation-summary.json) |
| ProcSurfaceInfo_c | `Exit` | [Resumen](../reversing/Misc8/validation-summary.json) |
| ProcSurfaceInfo_c | `Init` | [Resumen](../reversing/Misc8/validation-summary.json) |
| RenderBuffer | `ClearRenderBuffer` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RenderBuffer | `RenderStuffInBuffer` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RenderBuffer | `StartStoring` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RenderBuffer | `StopStoring` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RpAnimBlend | `RpAnimBlendClumpUpdateAnimations` | [Resumen](../reversing/ValidationRegistry/rpanimblend-validation.json) |
| RtAnim | `RtAnimInterpolatorSetCurrentAnim` | [Resumen](../reversing/Misc56/validation-summary.json) |
| RwCore | `RwCameraBeginUpdate` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RwCore | `RwCameraClear` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RwCore | `RwCameraEndUpdate` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| RwCore | `RwCameraShowRaster` | [Resumen](../reversing/RenderVideo8/validation-summary.json) |
| Securom | `SecuromDisplayInit` | [Resumen](../reversing/Misc70/validation-summary.json) |
| Securom | `SecuromStateDisplay` | [Resumen](../reversing/Misc70/validation-summary.json) |
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
| `CAEAudioHardware` | 54 | 0 | 0 | 54 |
| `CAEAudioUtility` | 11 | 0 | 11 | 0 |
| `CAEBankLoader` | 6 | 0 | 6 | 0 |
| `CAECollisionAudioEntity` | 17 | 0 | 0 | 17 |
| `CAECutsceneTrackManager` | 8 | 0 | 0 | 8 |
| `CAEDataStream` | 20 | 0 | 0 | 20 |
| `CAEDoorAudioEntity` | 6 | 0 | 6 | 0 |
| `CAEExplosionAudioEntity` | 3 | 0 | 3 | 0 |
| `CAEFireAudioEntity` | 7 | 0 | 7 | 0 |
| `CAEFrontendAudioEntity` | 4 | 0 | 0 | 4 |
| `CAEGlobalWeaponAudioEntity` | 6 | 0 | 6 | 0 |
| `CAEMFDecoder` | 10 | 0 | 0 | 10 |
| `CAEMP3BankLoader` | 13 | 0 | 0 | 13 |
| `CAEMP3TrackLoader` | 7 | 0 | 7 | 0 |
| `CAEPedAudioEntity` | 16 | 0 | 0 | 16 |
| `CAEPedSpeechAudioEntity` | 58 | 0 | 0 | 58 |
| `CAEPedWeaponAudioEntity` | 4 | 0 | 4 | 0 |
| `CAEPedlessSpeechAudioEntity` | 11 | 0 | 0 | 11 |
| `CAEPoliceScannerAudioEntity` | 13 | 0 | 0 | 13 |
| `CAERadioTrackManager` | 29 | 0 | 0 | 29 |
| `CAEScriptAudioEntity` | 8 | 0 | 0 | 8 |
| `CAESmoothFadeThread` | 10 | 0 | 0 | 10 |
| `CAESound` | 18 | 0 | 1 | 17 |
| `CAESoundManager` | 14 | 0 | 0 | 14 |
| `CAEStaticChannel` | 8 | 0 | 1 | 7 |
| `CAEStreamThread` | 14 | 0 | 0 | 14 |
| `CAEStreamTransformer` | 2 | 0 | 2 | 0 |
| `CAEStreamingChannel` | 21 | 0 | 0 | 21 |
| `CAEStreamingDecoder` | 2 | 0 | 2 | 0 |
| `CAETrackLoader` | 1 | 0 | 1 | 0 |
| `CAETwinLoopSoundEntity` | 7 | 0 | 7 | 0 |
| `CAEUserRadioTrackManager` | 15 | 0 | 0 | 15 |
| `CAEVehicleAudioEntity` | 102 | 0 | 0 | 102 |
| `CAEVorbisDecoder` | 13 | 0 | 0 | 13 |
| `CAEWMADecoder` | 11 | 0 | 0 | 11 |
| `CAEWaterCannonAudioEntity` | 7 | 0 | 7 | 0 |
| `CAEWaveDecoder` | 9 | 0 | 0 | 9 |
| `CAEWeaponAudioEntity` | 20 | 0 | 0 | 20 |
| `CAEWeatherAudioEntity` | 5 | 0 | 5 | 0 |
| `CAccidentManager` | 5 | 0 | 5 | 0 |
| `CAcquaintance` | 5 | 0 | 5 | 0 |
| `CActiveOccluder` | 2 | 0 | 2 | 0 |
| `CAnimBlendAssocGroup` | 15 | 0 | 0 | 15 |
| `CAnimBlendAssociation` | 20 | 0 | 0 | 20 |
| `CAnimBlendClumpData` | 6 | 0 | 6 | 0 |
| `CAnimBlendHierarchy` | 16 | 0 | 0 | 16 |
| `CAnimBlendNode` | 16 | 0 | 0 | 16 |
| `CAnimBlendSequence` | 14 | 0 | 0 | 14 |
| `CAnimBlendStaticAssociation` | 6 | 0 | 6 | 0 |
| `CAnimManager` | 31 | 0 | 0 | 31 |
| `CAnimatedBuilding` | 3 | 0 | 3 | 0 |
| `CAtomicModelInfo` | 9 | 0 | 0 | 9 |
| `CAudioEngine` | 82 | 0 | 0 | 82 |
| `CAudioZones` | 5 | 0 | 5 | 0 |
| `CAutomobile` | 99 | 0 | 0 | 99 |
| `CBaseModelInfo` | 34 | 0 | 1 | 33 |
| `CBike` | 22 | 0 | 0 | 22 |
| `CBirds` | 6 | 0 | 6 | 0 |
| `CBmx` | 7 | 0 | 0 | 7 |
| `CBoat` | 23 | 0 | 0 | 23 |
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
| `CCarAI` | 25 | 0 | 0 | 25 |
| `CCarCtrl` | 22 | 0 | 0 | 22 |
| `CCarEnterExit` | 27 | 0 | 0 | 27 |
| `CCarFXRenderer` | 9 | 0 | 0 | 9 |
| `CCarGenerator` | 8 | 0 | 0 | 8 |
| `CCheat` | 72 | 0 | 0 | 72 |
| `CCheckpoint` | 1 | 0 | 1 | 0 |
| `CCheckpoints` | 8 | 0 | 0 | 8 |
| `CCivilianPed` | 3 | 0 | 3 | 0 |
| `CClock` | 9 | 0 | 0 | 9 |
| `CClothes` | 11 | 0 | 1 | 10 |
| `CClothesBuilder` | 15 | 0 | 0 | 15 |
| `CClouds` | 21 | 0 | 1 | 20 |
| `CClumpModelInfo` | 24 | 0 | 0 | 24 |
| `CColAccel` | 11 | 0 | 0 | 11 |
| `CColLine` | 1 | 0 | 1 | 0 |
| `CColModel` | 9 | 0 | 0 | 9 |
| `CColModelPool` | 1 | 0 | 1 | 0 |
| `CColSphere` | 5 | 0 | 5 | 0 |
| `CColStore` | 23 | 0 | 0 | 23 |
| `CColTrianglePlane` | 2 | 0 | 2 | 0 |
| `CCollision` | 59 | 0 | 0 | 59 |
| `CCollisionData` | 8 | 0 | 0 | 8 |
| `CCollisionPlugin` | 2 | 0 | 2 | 0 |
| `CColourSet` | 2 | 0 | 2 | 0 |
| `CCompressedMatrixNotAligned` | 2 | 0 | 2 | 0 |
| `CControllerConfigManager` | 58 | 0 | 0 | 58 |
| `CControllerState` | 2 | 0 | 2 | 0 |
| `CConversationForPed` | 2 | 0 | 2 | 0 |
| `CConversationNode` | 1 | 0 | 1 | 0 |
| `CConversations` | 10 | 0 | 1 | 9 |
| `CCopPed` | 7 | 0 | 7 | 0 |
| `CCoronas` | 11 | 0 | 0 | 11 |
| `CCover` | 16 | 0 | 0 | 16 |
| `CCoverPoint` | 3 | 0 | 3 | 0 |
| `CCranes` | 5 | 0 | 5 | 0 |
| `CCredits` | 6 | 0 | 6 | 0 |
| `CCreepingFire` | 3 | 0 | 3 | 0 |
| `CCrime` | 2 | 0 | 2 | 0 |
| `CCullZones` | 21 | 0 | 0 | 21 |
| `CCurrentVehicle` | 2 | 0 | 2 | 0 |
| `CCurves` | 6 | 0 | 6 | 0 |
| `CCustomBuildingDNPipeline` | 14 | 0 | 0 | 14 |
| `CCustomBuildingPipeline` | 7 | 0 | 7 | 0 |
| `CCustomBuildingRenderer` | 3 | 0 | 0 | 3 |
| `CCustomCarEnvMapPipeline` | 37 | 0 | 0 | 37 |
| `CCustomCarPlateMgr` | 14 | 0 | 0 | 14 |
| `CCustomRoadsignMgr` | 9 | 0 | 0 | 9 |
| `CCutsceneMgr` | 35 | 0 | 0 | 35 |
| `CCutsceneObject` | 6 | 0 | 6 | 0 |
| `CDamageAtomicModelInfo` | 4 | 0 | 4 | 0 |
| `CDamageManager` | 27 | 0 | 0 | 27 |
| `CDarkel` | 19 | 0 | 0 | 19 |
| `CData` | 2 | 0 | 2 | 0 |
| `CDebug` | 1 | 0 | 1 | 0 |
| `CDecision` | 1 | 0 | 0 | 1 |
| `CDecisionMakerTypesFileLoader` | 7 | 0 | 7 | 0 |
| `CDirectory` | 10 | 0 | 0 | 10 |
| `CDoor` | 6 | 0 | 6 | 0 |
| `CDraw` | 2 | 0 | 2 | 0 |
| `CDummy` | 1 | 0 | 1 | 0 |
| `CDummyObject` | 2 | 0 | 2 | 0 |
| `CDummyPool` | 1 | 0 | 1 | 0 |
| `CEntity` | 70 | 0 | 0 | 70 |
| `CEntityScanner` | 4 | 0 | 4 | 0 |
| `CEntitySeekPosCalculatorFixedPos` | 1 | 0 | 1 | 0 |
| `CEntitySeekPosCalculatorRadiusAngleOffset` | 1 | 0 | 1 | 0 |
| `CEntitySeekPosCalculatorStandard` | 1 | 0 | 1 | 0 |
| `CEntitySeekPosCalculatorXYOffset` | 1 | 0 | 1 | 0 |
| `CEntryExit` | 11 | 0 | 0 | 11 |
| `CEntryExitManager` | 18 | 0 | 0 | 18 |
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
| `CEventDamage` | 20 | 0 | 0 | 20 |
| `CEventDanger` | 4 | 0 | 4 | 0 |
| `CEventDontJoinPlayerGroup` | 1 | 0 | 1 | 0 |
| `CEventEditableResponse` | 11 | 0 | 0 | 11 |
| `CEventEscalator` | 1 | 0 | 1 | 0 |
| `CEventGlobalGroup` | 3 | 0 | 3 | 0 |
| `CEventGroup` | 12 | 0 | 0 | 12 |
| `CEventGunShot` | 5 | 0 | 5 | 0 |
| `CEventGunShotWhizzedBy` | 2 | 0 | 2 | 0 |
| `CEventHandler` | 69 | 0 | 0 | 69 |
| `CEventHandlerHistory` | 10 | 0 | 0 | 10 |
| `CEventHitByWaterCannon` | 2 | 0 | 2 | 0 |
| `CEventInAir` | 2 | 0 | 2 | 0 |
| `CEventKnockOffBike` | 8 | 0 | 0 | 8 |
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
| `CEventScriptCommand` | 8 | 0 | 0 | 8 |
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
| `CExplosion` | 12 | 0 | 0 | 12 |
| `CFallingGlassPane` | 2 | 0 | 2 | 0 |
| `CFileLoader` | 47 | 0 | 0 | 47 |
| `CFileMgr` | 17 | 0 | 0 | 17 |
| `CFire` | 5 | 0 | 5 | 0 |
| `CFireManager` | 24 | 0 | 0 | 24 |
| `CFont` | 32 | 0 | 0 | 32 |
| `CFormation` | 1 | 0 | 0 | 1 |
| `CGame` | 22 | 0 | 0 | 22 |
| `CGameLogic` | 27 | 0 | 0 | 27 |
| `CGamma` | 2 | 0 | 2 | 0 |
| `CGangWars` | 32 | 0 | 0 | 32 |
| `CGangs` | 8 | 0 | 0 | 8 |
| `CGarage` | 4 | 0 | 4 | 0 |
| `CGarages` | 21 | 0 | 0 | 21 |
| `CGeneral` | 9 | 0 | 0 | 9 |
| `CGenericGameStorage` | 5 | 0 | 0 | 5 |
| `CGlass` | 19 | 0 | 0 | 19 |
| `CGrassRenderer` | 12 | 0 | 0 | 12 |
| `CGridRef` | 5 | 0 | 5 | 0 |
| `CGroupEventHandler` | 30 | 0 | 0 | 30 |
| `CHandObject` | 3 | 0 | 3 | 0 |
| `CHandShaker` | 3 | 0 | 3 | 0 |
| `CHeli` | 11 | 0 | 0 | 11 |
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
| `CMenuManager` | 61 | 0 | 0 | 61 |
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
| `CObject` | 48 | 0 | 0 | 48 |
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
| `CPed` | 145 | 0 | 6 | 139 |
| `CPedAttractor` | 19 | 0 | 0 | 19 |
| `CPedAttractorManager` | 14 | 0 | 0 | 14 |
| `CPedAttractorPedPlacer` | 1 | 0 | 1 | 0 |
| `CPedAttractorPool` | 1 | 0 | 1 | 0 |
| `CPedClothesDesc` | 7 | 0 | 7 | 0 |
| `CPedDamageResponseCalculator` | 2 | 0 | 0 | 2 |
| `CPedGeometryAnalyser` | 9 | 0 | 0 | 9 |
| `CPedGroup` | 10 | 0 | 0 | 10 |
| `CPedGroupDefaultTaskAllocatorChat` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorFollowAnyMeans` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorFollowLimited` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorRandom` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorSitInLeaderCar` | 2 | 0 | 2 | 0 |
| `CPedGroupDefaultTaskAllocatorStandStill` | 2 | 0 | 2 | 0 |
| `CPedGroupIntelligence` | 26 | 0 | 0 | 26 |
| `CPedGroupMembership` | 18 | 0 | 0 | 18 |
| `CPedGroupPlacer` | 1 | 0 | 0 | 1 |
| `CPedGroups` | 2 | 0 | 2 | 0 |
| `CPedIK` | 9 | 0 | 0 | 9 |
| `CPedIntelligence` | 52 | 0 | 0 | 52 |
| `CPedIntelligencePool` | 1 | 0 | 1 | 0 |
| `CPedList` | 7 | 0 | 7 | 0 |
| `CPedModelInfo` | 9 | 0 | 0 | 9 |
| `CPedPlacement` | 3 | 0 | 3 | 0 |
| `CPedPool` | 2 | 0 | 2 | 0 |
| `CPedScriptedTaskRecord` | 5 | 0 | 5 | 0 |
| `CPedScriptedTaskRecordData` | 8 | 0 | 0 | 8 |
| `CPedShelterAttractor` | 2 | 0 | 0 | 2 |
| `CPedStats` | 4 | 0 | 4 | 0 |
| `CPedTaskPair` | 1 | 0 | 1 | 0 |
| `CPedToPlayerConversations` | 3 | 0 | 3 | 0 |
| `CPedType` | 13 | 0 | 0 | 13 |
| `CPhysical` | 56 | 0 | 0 | 56 |
| `CPickup` | 12 | 0 | 0 | 12 |
| `CPickups` | 35 | 0 | 0 | 35 |
| `CPlaceName` | 2 | 0 | 2 | 0 |
| `CPlaceable` | 16 | 0 | 0 | 16 |
| `CPlane` | 14 | 0 | 0 | 14 |
| `CPlaneTrail` | 3 | 0 | 3 | 0 |
| `CPlaneTrails` | 4 | 0 | 4 | 0 |
| `CPlantColEntEntry` | 2 | 0 | 2 | 0 |
| `CPlantLocTri` | 3 | 0 | 3 | 0 |
| `CPlantMgr` | 19 | 0 | 0 | 19 |
| `CPlantSurfPropMgr` | 4 | 0 | 4 | 0 |
| `CPlayerInfo` | 28 | 0 | 3 | 25 |
| `CPlayerPed` | 51 | 0 | 0 | 51 |
| `CPlayerPedData` | 5 | 0 | 5 | 0 |
| `CPlayerSkin` | 4 | 0 | 4 | 0 |
| `CPointRoutePool` | 1 | 0 | 1 | 0 |
| `CPools` | 16 | 0 | 0 | 16 |
| `CPopCycle` | 15 | 0 | 0 | 15 |
| `CPopulation` | 64 | 0 | 0 | 64 |
| `CPostEffects` | 35 | 0 | 0 | 35 |
| `CProjectileInfo` | 1 | 0 | 0 | 1 |
| `CPtrNodeDoubleLinkPool` | 1 | 0 | 1 | 0 |
| `CPtrNodeSingleLinkPool` | 1 | 0 | 1 | 0 |
| `CQuadBike` | 11 | 0 | 0 | 11 |
| `CQuadTreeNode<T>` | 12 | 0 | 0 | 12 |
| `CRadar` | 63 | 0 | 0 | 63 |
| `CRealTimeShadow` | 8 | 0 | 0 | 8 |
| `CRealTimeShadowManager` | 5 | 0 | 0 | 5 |
| `CRect` | 8 | 0 | 0 | 8 |
| `CReferences` | 4 | 0 | 4 | 0 |
| `CRegisteredCorona` | 1 | 0 | 1 | 0 |
| `CRegisteredMotionBlurStreak` | 2 | 0 | 2 | 0 |
| `CRenderer` | 33 | 0 | 0 | 33 |
| `CReplay` | 47 | 0 | 0 | 47 |
| `CRestart` | 11 | 0 | 0 | 11 |
| `CRoadBlocks` | 7 | 0 | 0 | 7 |
| `CRope` | 5 | 0 | 0 | 5 |
| `CRopes` | 9 | 0 | 0 | 9 |
| `CRunningScript` | 24 | 0 | 0 | 24 |
| `CScriptResourceManager` | 4 | 0 | 4 | 0 |
| `CScripted2dEffects` | 5 | 0 | 5 | 0 |
| `CScriptedBrainTaskEntry` | 1 | 0 | 1 | 0 |
| `CScriptedBrainTaskStore` | 4 | 0 | 4 | 0 |
| `CScriptsForBrains` | 3 | 0 | 0 | 3 |
| `CSetPiece` | 3 | 0 | 3 | 0 |
| `CSetPieces` | 5 | 0 | 5 | 0 |
| `CShadowCamera` | 19 | 0 | 0 | 19 |
| `CShadows` | 21 | 0 | 0 | 21 |
| `CShinyTexts` | 4 | 0 | 4 | 0 |
| `CShopping` | 30 | 0 | 0 | 30 |
| `CShotInfo` | 5 | 0 | 5 | 0 |
| `CSimpleVariablesSaveStructure` | 2 | 0 | 2 | 0 |
| `CSkidmarks` | 7 | 0 | 7 | 0 |
| `CSpecialFX` | 7 | 0 | 7 | 0 |
| `CSpecialPlateHandler` | 4 | 0 | 4 | 0 |
| `CSprite` | 15 | 0 | 0 | 15 |
| `CSprite2d` | 33 | 0 | 0 | 33 |
| `CStaticShadow` | 1 | 0 | 1 | 0 |
| `CStats` | 32 | 0 | 0 | 32 |
| `CStencilShadowObject` | 3 | 0 | 3 | 0 |
| `CStencilShadows` | 9 | 0 | 0 | 9 |
| `CStreamedScripts` | 12 | 0 | 0 | 12 |
| `CStreaming` | 94 | 0 | 0 | 94 |
| `CStreamingInfo` | 7 | 0 | 7 | 0 |
| `CStuckCarCheck` | 8 | 0 | 0 | 8 |
| `CStuntJumpManager` | 7 | 0 | 7 | 0 |
| `CTagManager` | 19 | 0 | 0 | 19 |
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
| `CTaskComplexAvoidOtherPedWhileWandering` | 15 | 0 | 0 | 15 |
| `CTaskComplexBeCop` | 2 | 0 | 0 | 2 |
| `CTaskComplexBeInCouple` | 7 | 0 | 0 | 7 |
| `CTaskComplexBeInGroup` | 10 | 0 | 0 | 10 |
| `CTaskComplexBeStill` | 6 | 0 | 6 | 0 |
| `CTaskComplexCarDrive` | 11 | 0 | 0 | 11 |
| `CTaskComplexCarDriveMission` | 5 | 0 | 5 | 0 |
| `CTaskComplexCarDriveMissionFleeScene` | 4 | 0 | 4 | 0 |
| `CTaskComplexCarDriveMissionKillPed` | 4 | 0 | 4 | 0 |
| `CTaskComplexCarDriveWander` | 5 | 0 | 5 | 0 |
| `CTaskComplexCarSlowBeDraggedOut` | 11 | 0 | 0 | 11 |
| `CTaskComplexCarSlowBeDraggedOutAndStandUp` | 3 | 0 | 0 | 3 |
| `CTaskComplexChat` | 7 | 0 | 7 | 0 |
| `CTaskComplexClimb` | 4 | 0 | 4 | 0 |
| `CTaskComplexCopInCar` | 9 | 0 | 0 | 9 |
| `CTaskComplexCrossRoadLookAndAchieveHeading` | 8 | 0 | 0 | 8 |
| `CTaskComplexDestroyCar` | 8 | 0 | 0 | 8 |
| `CTaskComplexDestroyCarArmed` | 10 | 0 | 0 | 10 |
| `CTaskComplexDestroyCarMelee` | 10 | 0 | 0 | 10 |
| `CTaskComplexDie` | 5 | 0 | 5 | 0 |
| `CTaskComplexDieInCar` | 6 | 0 | 6 | 0 |
| `CTaskComplexDiveFromAttachedEntityAndGetUp` | 7 | 0 | 7 | 0 |
| `CTaskComplexDragPedFromCar` | 2 | 0 | 2 | 0 |
| `CTaskComplexDriveFireTruck` | 6 | 0 | 6 | 0 |
| `CTaskComplexDrivePointRoute` | 7 | 0 | 7 | 0 |
| `CTaskComplexDriveToPoint` | 5 | 0 | 0 | 5 |
| `CTaskComplexEnterAnyCarAsDriver` | 5 | 0 | 5 | 0 |
| `CTaskComplexEnterBoatAsDriver` | 8 | 0 | 0 | 8 |
| `CTaskComplexEnterCar` | 14 | 0 | 0 | 14 |
| `CTaskComplexEnterCarAsDriver` | 3 | 0 | 3 | 0 |
| `CTaskComplexEnterCarAsDriverTimed` | 9 | 0 | 0 | 9 |
| `CTaskComplexEnterCarAsPassenger` | 3 | 0 | 3 | 0 |
| `CTaskComplexEnterCarAsPassengerTimed` | 9 | 0 | 0 | 9 |
| `CTaskComplexEnterCarAsPassengerWait` | 4 | 0 | 0 | 4 |
| `CTaskComplexEvasiveCower` | 7 | 0 | 0 | 7 |
| `CTaskComplexEvasiveDiveAndGetUp` | 9 | 0 | 0 | 9 |
| `CTaskComplexEvasiveStep` | 7 | 0 | 0 | 7 |
| `CTaskComplexExtinguishFires` | 8 | 0 | 0 | 8 |
| `CTaskComplexFacial` | 9 | 0 | 0 | 9 |
| `CTaskComplexFallAndGetUp` | 9 | 0 | 0 | 9 |
| `CTaskComplexFallToDeath` | 6 | 0 | 6 | 0 |
| `CTaskComplexFleeAnyMeans` | 8 | 0 | 0 | 8 |
| `CTaskComplexFleeEntity` | 8 | 0 | 0 | 8 |
| `CTaskComplexFleePoint` | 11 | 0 | 0 | 11 |
| `CTaskComplexFleeShooting` | 8 | 0 | 0 | 8 |
| `CTaskComplexFollowLeaderAnyMeans` | 8 | 0 | 0 | 8 |
| `CTaskComplexFollowLeaderInFormation` | 1 | 0 | 0 | 1 |
| `CTaskComplexFollowNodeRoute` | 19 | 0 | 0 | 19 |
| `CTaskComplexFollowPedFootsteps` | 1 | 0 | 1 | 0 |
| `CTaskComplexFollowPointRoute` | 13 | 0 | 0 | 13 |
| `CTaskComplexGangFollower` | 1 | 0 | 1 | 0 |
| `CTaskComplexGangJoinRespond` | 2 | 0 | 0 | 2 |
| `CTaskComplexGangLeader` | 13 | 0 | 1 | 12 |
| `CTaskComplexGetOnBoatSeat` | 8 | 0 | 0 | 8 |
| `CTaskComplexGetUpAndStandStill` | 5 | 0 | 5 | 0 |
| `CTaskComplexGoToAttractor` | 2 | 0 | 0 | 2 |
| `CTaskComplexGoToBoatSteeringWheel` | 6 | 0 | 6 | 0 |
| `CTaskComplexGoToCarDoorAndStandStill` | 11 | 0 | 0 | 11 |
| `CTaskComplexGoToPointAiming` | 5 | 0 | 0 | 5 |
| `CTaskComplexGoToPointAndStandStill` | 7 | 0 | 7 | 0 |
| `CTaskComplexGoToPointAndStandStillAndAchieveHeading` | 3 | 0 | 0 | 3 |
| `CTaskComplexGoToPointAndStandStillTimed` | 6 | 0 | 6 | 0 |
| `CTaskComplexGoToPointAnyMeans` | 3 | 0 | 0 | 3 |
| `CTaskComplexGoToPointShooting` | 3 | 0 | 3 | 0 |
| `CTaskComplexGotoDoorAndOpen` | 3 | 0 | 0 | 3 |
| `CTaskComplexHitByGunResponse` | 8 | 0 | 0 | 8 |
| `CTaskComplexHitPedWithCar` | 2 | 0 | 0 | 2 |
| `CTaskComplexHitResponse` | 7 | 0 | 7 | 0 |
| `CTaskComplexInAirAndLand` | 4 | 0 | 4 | 0 |
| `CTaskComplexInWater` | 7 | 0 | 7 | 0 |
| `CTaskComplexInvestigateDeadPed` | 9 | 0 | 0 | 9 |
| `CTaskComplexInvestigateDisturbance` | 8 | 0 | 0 | 8 |
| `CTaskComplexJump` | 6 | 0 | 6 | 0 |
| `CTaskComplexKillCriminal` | 9 | 0 | 0 | 9 |
| `CTaskComplexKillPedFromBoat` | 2 | 0 | 0 | 2 |
| `CTaskComplexKillPedGroupOnFoot` | 9 | 0 | 0 | 9 |
| `CTaskComplexKillPedOnFoot` | 1 | 0 | 1 | 0 |
| `CTaskComplexKillPedOnFootArmed` | 9 | 0 | 0 | 9 |
| `CTaskComplexKillPedOnFootMelee` | 10 | 0 | 0 | 10 |
| `CTaskComplexKillPedOnFootStealth` | 7 | 0 | 7 | 0 |
| `CTaskComplexLeaveAnyCar` | 7 | 0 | 7 | 0 |
| `CTaskComplexLeaveBoat` | 8 | 0 | 0 | 8 |
| `CTaskComplexLeaveCarAndDie` | 3 | 0 | 3 | 0 |
| `CTaskComplexLeaveCarAndFlee` | 8 | 0 | 0 | 8 |
| `CTaskComplexLeaveCarAndWander` | 8 | 0 | 0 | 8 |
| `CTaskComplexLeaveCarAsPassengerWait` | 10 | 0 | 0 | 10 |
| `CTaskComplexMedicTreatInjuredPed` | 9 | 0 | 0 | 9 |
| `CTaskComplexMoveBackAndJump` | 8 | 0 | 0 | 8 |
| `CTaskComplexObserveTrafficLights` | 8 | 0 | 0 | 8 |
| `CTaskComplexObserveTrafficLightsAndAchieveHeading` | 5 | 0 | 5 | 0 |
| `CTaskComplexOnFire` | 9 | 0 | 0 | 9 |
| `CTaskComplexOpenDriverDoor` | 4 | 0 | 4 | 0 |
| `CTaskComplexOpenPassengerDoor` | 3 | 0 | 0 | 3 |
| `CTaskComplexPartner` | 1 | 0 | 1 | 0 |
| `CTaskComplexPartnerChat` | 1 | 0 | 1 | 0 |
| `CTaskComplexPartnerDeal` | 1 | 0 | 1 | 0 |
| `CTaskComplexPartnerGreet` | 7 | 0 | 7 | 0 |
| `CTaskComplexPartnerShove` | 1 | 0 | 1 | 0 |
| `CTaskComplexPassObject` | 9 | 0 | 0 | 9 |
| `CTaskComplexPlayHandSignalAnim` | 3 | 0 | 0 | 3 |
| `CTaskComplexPolicePursuit` | 13 | 0 | 0 | 13 |
| `CTaskComplexProstituteSolicit` | 7 | 0 | 0 | 7 |
| `CTaskComplexReactToGunAimedAt` | 8 | 0 | 0 | 8 |
| `CTaskComplexRoadRage` | 9 | 0 | 0 | 9 |
| `CTaskComplexScreamInCarThenLeave` | 9 | 0 | 0 | 9 |
| `CTaskComplexSeekCoverUntilTargetDead` | 10 | 0 | 0 | 10 |
| `CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>` | 9 | 0 | 0 | 9 |
| `CTaskComplexSeekEntityAiming` | 7 | 0 | 7 | 0 |
| `CTaskComplexSeekEntityAnyMeansXYOffset` | 5 | 0 | 5 | 0 |
| `CTaskComplexSeekEntityRadiusAngleOffset` | 9 | 0 | 0 | 9 |
| `CTaskComplexSeekEntityStandard` | 9 | 0 | 0 | 9 |
| `CTaskComplexSequence` | 13 | 0 | 0 | 13 |
| `CTaskComplexShuffleSeats` | 10 | 0 | 0 | 10 |
| `CTaskComplexSignalAtPed` | 4 | 0 | 0 | 4 |
| `CTaskComplexSitDownThenIdleThenStandUp` | 9 | 0 | 0 | 9 |
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
| `CTaskComplexUseClosestFreeScriptedAttractor` | 6 | 0 | 0 | 6 |
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
| `CTaskComplexWalkRoundBuildingAttempt` | 12 | 0 | 0 | 12 |
| `CTaskComplexWalkRoundCar` | 9 | 0 | 0 | 9 |
| `CTaskComplexWalkRoundFire` | 8 | 0 | 0 | 8 |
| `CTaskComplexWalkRoundObject` | 1 | 0 | 1 | 0 |
| `CTaskComplexWander` | 14 | 0 | 1 | 13 |
| `CTaskComplexWanderCop` | 9 | 0 | 0 | 9 |
| `CTaskComplexWanderCriminal` | 3 | 0 | 0 | 3 |
| `CTaskComplexWanderFlee` | 6 | 0 | 6 | 0 |
| `CTaskComplexWanderGang` | 4 | 0 | 4 | 0 |
| `CTaskComplexWanderMedic` | 4 | 0 | 4 | 0 |
| `CTaskComplexWanderProstitute` | 4 | 0 | 0 | 4 |
| `CTaskComplexWanderStandard` | 3 | 0 | 0 | 3 |
| `CTaskGangHasslePed` | 2 | 0 | 0 | 2 |
| `CTaskGangHassleVehicle` | 2 | 0 | 0 | 2 |
| `CTaskGoToVehicleAndLean` | 2 | 0 | 0 | 2 |
| `CTaskInteriorBeInHouse` | 8 | 0 | 0 | 8 |
| `CTaskInteriorBeInOffice` | 8 | 0 | 0 | 8 |
| `CTaskInteriorBeInShop` | 6 | 0 | 6 | 0 |
| `CTaskInteriorGoToInfo` | 7 | 0 | 7 | 0 |
| `CTaskInteriorLieInBed` | 7 | 0 | 7 | 0 |
| `CTaskInteriorShopKeeper` | 7 | 0 | 7 | 0 |
| `CTaskInteriorSitAtDesk` | 9 | 0 | 0 | 9 |
| `CTaskInteriorUseInfo` | 8 | 0 | 0 | 8 |
| `CTaskLeanOnVehicle` | 3 | 0 | 0 | 3 |
| `CTaskManager` | 18 | 0 | 0 | 18 |
| `CTaskPool` | 1 | 0 | 1 | 0 |
| `CTaskSequences` | 2 | 0 | 2 | 0 |
| `CTaskSimpleAchieveHeading` | 5 | 0 | 5 | 0 |
| `CTaskSimpleAffectSecondaryBehaviour` | 5 | 0 | 5 | 0 |
| `CTaskSimpleAnim` | 1 | 0 | 1 | 0 |
| `CTaskSimpleArrestPed` | 3 | 0 | 3 | 0 |
| `CTaskSimpleBeHit` | 8 | 0 | 0 | 8 |
| `CTaskSimpleBikeJacked` | 8 | 0 | 0 | 8 |
| `CTaskSimpleCarAlign` | 10 | 0 | 0 | 10 |
| `CTaskSimpleCarCloseDoorFromInside` | 10 | 0 | 0 | 10 |
| `CTaskSimpleCarCloseDoorFromOutside` | 10 | 0 | 0 | 10 |
| `CTaskSimpleCarDrive` | 7 | 0 | 0 | 7 |
| `CTaskSimpleCarDriveTimed` | 4 | 0 | 0 | 4 |
| `CTaskSimpleCarFallOut` | 9 | 0 | 0 | 9 |
| `CTaskSimpleCarGetIn` | 9 | 0 | 0 | 9 |
| `CTaskSimpleCarGetOut` | 10 | 0 | 0 | 10 |
| `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse` | 6 | 0 | 6 | 0 |
| `CTaskSimpleCarJumpOut` | 10 | 0 | 0 | 10 |
| `CTaskSimpleCarOpenDoorFromOutside` | 10 | 0 | 0 | 10 |
| `CTaskSimpleCarOpenLockedDoorFromOutside` | 9 | 0 | 0 | 9 |
| `CTaskSimpleCarSetPedInAsDriver` | 4 | 0 | 0 | 4 |
| `CTaskSimpleCarSetPedInAsPassenger` | 4 | 0 | 0 | 4 |
| `CTaskSimpleCarSetPedOut` | 2 | 0 | 0 | 2 |
| `CTaskSimpleCarSetPedSlowDraggedOut` | 6 | 0 | 6 | 0 |
| `CTaskSimpleCarSetTempAction` | 6 | 0 | 6 | 0 |
| `CTaskSimpleCarShuffle` | 9 | 0 | 0 | 9 |
| `CTaskSimpleCarSlowBeDraggedOut` | 9 | 0 | 0 | 9 |
| `CTaskSimpleCarSlowDragPedOut` | 10 | 0 | 0 | 10 |
| `CTaskSimpleCarWaitForDoorNotToBeInUse` | 7 | 0 | 7 | 0 |
| `CTaskSimpleCarWaitToSlowDown` | 6 | 0 | 6 | 0 |
| `CTaskSimpleChat` | 3 | 0 | 3 | 0 |
| `CTaskSimpleChoking` | 5 | 0 | 0 | 5 |
| `CTaskSimpleClearLookAt` | 6 | 0 | 6 | 0 |
| `CTaskSimpleClimb` | 15 | 0 | 0 | 15 |
| `CTaskSimpleCower` | 4 | 0 | 4 | 0 |
| `CTaskSimpleCreateCarAndGetIn` | 6 | 0 | 6 | 0 |
| `CTaskSimpleDie` | 7 | 0 | 7 | 0 |
| `CTaskSimpleDieInCar` | 5 | 0 | 5 | 0 |
| `CTaskSimpleDoHandSignal` | 6 | 0 | 6 | 0 |
| `CTaskSimpleDrown` | 1 | 0 | 1 | 0 |
| `CTaskSimpleDrownInCar` | 1 | 0 | 1 | 0 |
| `CTaskSimpleDuck` | 15 | 0 | 0 | 15 |
| `CTaskSimpleDuckToggle` | 6 | 0 | 6 | 0 |
| `CTaskSimpleEvasiveDive` | 8 | 0 | 0 | 8 |
| `CTaskSimpleEvasiveStep` | 8 | 0 | 0 | 8 |
| `CTaskSimpleFacial` | 5 | 0 | 5 | 0 |
| `CTaskSimpleFall` | 6 | 0 | 6 | 0 |
| `CTaskSimpleFightingControl` | 6 | 0 | 0 | 6 |
| `CTaskSimpleFinishBrain` | 2 | 0 | 2 | 0 |
| `CTaskSimpleGangDriveBy` | 1 | 0 | 0 | 1 |
| `CTaskSimpleGetUp` | 5 | 0 | 5 | 0 |
| `CTaskSimpleGiveCPR` | 6 | 0 | 6 | 0 |
| `CTaskSimpleGoTo` | 4 | 0 | 4 | 0 |
| `CTaskSimpleGoToPoint` | 6 | 0 | 6 | 0 |
| `CTaskSimpleGoToPointFine` | 8 | 0 | 0 | 8 |
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
| `CTaskSimpleHoldEntity` | 14 | 0 | 0 | 14 |
| `CTaskSimpleHurtPedWithCar` | 6 | 0 | 6 | 0 |
| `CTaskSimpleIKChain` | 9 | 0 | 0 | 9 |
| `CTaskSimpleIKLookAt` | 8 | 0 | 0 | 8 |
| `CTaskSimpleIKManager` | 8 | 0 | 0 | 8 |
| `CTaskSimpleIKPointArm` | 6 | 0 | 6 | 0 |
| `CTaskSimpleInAir` | 4 | 0 | 4 | 0 |
| `CTaskSimpleJetPack` | 14 | 0 | 1 | 13 |
| `CTaskSimpleJump` | 8 | 0 | 0 | 8 |
| `CTaskSimpleKillPedWithCar` | 7 | 0 | 7 | 0 |
| `CTaskSimpleLand` | 6 | 0 | 6 | 0 |
| `CTaskSimpleLeaveGroup` | 2 | 0 | 2 | 0 |
| `CTaskSimpleLookAbout` | 3 | 0 | 3 | 0 |
| `CTaskSimpleNone` | 4 | 0 | 4 | 0 |
| `CTaskSimplePause` | 4 | 0 | 4 | 0 |
| `CTaskSimplePickUpBike` | 9 | 0 | 0 | 9 |
| `CTaskSimplePickUpEntity` | 4 | 0 | 4 | 0 |
| `CTaskSimplePlayHandSignalAnim` | 2 | 0 | 2 | 0 |
| `CTaskSimplePlayerOnFire` | 6 | 0 | 6 | 0 |
| `CTaskSimplePlayerOnFoot` | 11 | 0 | 0 | 11 |
| `CTaskSimplePutDownEntity` | 3 | 0 | 3 | 0 |
| `CTaskSimpleRunAnim` | 5 | 0 | 5 | 0 |
| `CTaskSimpleRunNamedAnim` | 4 | 0 | 1 | 3 |
| `CTaskSimpleRunTimedAnim` | 7 | 0 | 7 | 0 |
| `CTaskSimpleSay` | 6 | 0 | 6 | 0 |
| `CTaskSimpleScratchHead` | 2 | 0 | 2 | 0 |
| `CTaskSimpleSetCharDecisionMaker` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetCharIgnoreWeaponRangeFlag` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetKindaStayInSamePlace` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetPedAsAutoDriver` | 6 | 0 | 6 | 0 |
| `CTaskSimpleSetStayInSamePlace` | 5 | 0 | 5 | 0 |
| `CTaskSimpleShakeFist` | 8 | 0 | 0 | 8 |
| `CTaskSimpleSitDown` | 8 | 0 | 0 | 8 |
| `CTaskSimpleSitIdle` | 7 | 0 | 7 | 0 |
| `CTaskSimpleStandStill` | 3 | 0 | 3 | 0 |
| `CTaskSimpleStandUp` | 8 | 0 | 0 | 8 |
| `CTaskSimpleStealthKill` | 7 | 0 | 7 | 0 |
| `CTaskSimpleSwim` | 11 | 0 | 0 | 11 |
| `CTaskSimpleThrowControl` | 6 | 0 | 6 | 0 |
| `CTaskSimpleThrowProjectile` | 9 | 0 | 0 | 9 |
| `CTaskSimpleTired` | 3 | 0 | 3 | 0 |
| `CTaskSimpleTogglePedThreatScanner` | 5 | 0 | 5 | 0 |
| `CTaskSimpleTurn180` | 6 | 0 | 6 | 0 |
| `CTaskSimpleUninterruptable` | 1 | 0 | 1 | 0 |
| `CTaskSimpleUseAtm` | 4 | 0 | 4 | 0 |
| `CTaskSimpleUseGun` | 19 | 0 | 1 | 18 |
| `CTaskSimpleWaitForBus` | 6 | 0 | 6 | 0 |
| `CTaskSimpleWaitUntilAreaCodesMatch` | 6 | 0 | 6 | 0 |
| `CTaskSimpleWaitUntilLeaderAreaCodesMatch` | 4 | 0 | 4 | 0 |
| `CTaskSimpleWaitUntilPedIsInCar` | 6 | 0 | 6 | 0 |
| `CTaskSimpleWaitUntilPedIsOutCar` | 6 | 0 | 6 | 0 |
| `CTaskTimer` | 1 | 0 | 1 | 0 |
| `CTaskUtilityLineUpPedWithCar` | 2 | 0 | 0 | 2 |
| `CTempColModels` | 2 | 0 | 2 | 0 |
| `CText` | 9 | 0 | 0 | 9 |
| `CTheCarGenerators` | 7 | 0 | 7 | 0 |
| `CTheScripts` | 69 | 0 | 0 | 69 |
| `CTheZones` | 21 | 0 | 0 | 21 |
| `CTimeCycle` | 20 | 0 | 0 | 20 |
| `CTimeInfo` | 6 | 0 | 6 | 0 |
| `CTimeModelInfo` | 2 | 0 | 2 | 0 |
| `CTimer` | 13 | 0 | 0 | 13 |
| `CTrafficLights` | 12 | 0 | 0 | 12 |
| `CTrailer` | 13 | 0 | 0 | 13 |
| `CTrain` | 20 | 0 | 0 | 20 |
| `CTxdStore` | 26 | 0 | 0 | 26 |
| `CUpsideDownCarCheck` | 5 | 0 | 5 | 0 |
| `CUserDisplay` | 2 | 0 | 2 | 0 |
| `CVehicle` | 142 | 0 | 0 | 142 |
| `CVehicleAnimGroup` | 5 | 0 | 5 | 0 |
| `CVehicleAnimGroupData` | 5 | 0 | 5 | 0 |
| `CVehicleModelInfo` | 75 | 0 | 0 | 75 |
| `CVehiclePool` | 1 | 0 | 1 | 0 |
| `CVehicleRecording` | 29 | 0 | 0 | 29 |
| `CVehicleSaveStructure` | 2 | 0 | 2 | 0 |
| `CVisibilityPlugins` | 84 | 0 | 0 | 84 |
| `CWanted` | 33 | 0 | 0 | 33 |
| `CWaterCannon` | 5 | 0 | 5 | 0 |
| `CWaterCannons` | 4 | 0 | 4 | 0 |
| `CWaterLevel` | 25 | 0 | 0 | 25 |
| `CWeapon` | 35 | 0 | 0 | 35 |
| `CWeaponEffects` | 9 | 0 | 0 | 9 |
| `CWeaponInfo` | 10 | 0 | 0 | 10 |
| `CWeaponModelInfo` | 3 | 0 | 3 | 0 |
| `CWeather` | 10 | 0 | 0 | 10 |
| `CWindModifiers` | 2 | 0 | 2 | 0 |
| `CWorld` | 94 | 0 | 0 | 94 |
| `C_PcSave` | 5 | 0 | 5 | 0 |
| `CdStream` | 9 | 0 | 0 | 9 |
| `Common` | 9 | 0 | 0 | 9 |
| `FurnitureGroup_c` | 6 | 0 | 6 | 0 |
| `FurnitureManager_c` | 7 | 0 | 7 | 0 |
| `FurnitureSubGroup_c` | 3 | 0 | 3 | 0 |
| `FxEmitterBP_c` | 2 | 0 | 0 | 2 |
| `FxEmitterPrt_c` | 1 | 0 | 1 | 0 |
| `FxInfoManager_c` | 4 | 0 | 4 | 0 |
| `FxManager_c` | 25 | 0 | 2 | 23 |
| `FxMemoryPool_c` | 5 | 0 | 5 | 0 |
| `FxPrimBP_c` | 2 | 0 | 2 | 0 |
| `FxSphere_c` | 4 | 0 | 4 | 0 |
| `FxSystemBP_c` | 10 | 0 | 0 | 10 |
| `FxSystem_c` | 31 | 0 | 0 | 31 |
| `Fx_c` | 8 | 0 | 0 | 8 |
| `Game` | 12 | 0 | 0 | 12 |
| `IKChainManager_c` | 17 | 0 | 0 | 17 |
| `IKChain_c` | 20 | 0 | 1 | 19 |
| `Input` | 5 | 0 | 5 | 0 |
| `InteriorGroup_c` | 2 | 0 | 0 | 2 |
| `InteriorManager_c` | 21 | 0 | 0 | 21 |
| `JPegPlugin` | 5 | 0 | 5 | 0 |
| `Light` | 27 | 0 | 0 | 27 |
| `ModelIndices` | 2 | 0 | 2 | 0 |
| `NodeNamePlugin` | 3 | 0 | 3 | 0 |
| `PipelinePlugin` | 3 | 0 | 3 | 0 |
| `ProcObjectMan_c` | 8 | 0 | 0 | 8 |
| `ProcSurfaceInfo_c` | 4 | 0 | 4 | 0 |
| `Ps` | 22 | 0 | 0 | 22 |
| `RenderBuffer` | 4 | 0 | 4 | 0 |
| `RpAnimBlend` | 43 | 0 | 1 | 42 |
| `Rs` | 27 | 0 | 0 | 27 |
| `RtAnim` | 1 | 0 | 1 | 0 |
| `RwCore` | 4 | 0 | 4 | 0 |
| `RwHelper` | 24 | 0 | 0 | 24 |
| `Securom` | 2 | 0 | 2 | 0 |
| `SurfaceInfos_c` | 47 | 0 | 0 | 47 |
| `VideoMode` | 4 | 0 | 4 | 0 |
| `VideoPlayer` | 6 | 0 | 6 | 0 |
| `WaterCreatureManager_c` | 7 | 0 | 7 | 0 |
| `WaterCreature_c` | 3 | 0 | 3 | 0 |
| `Win` | 6 | 0 | 5 | 1 |
| `cBuoyancy` | 9 | 0 | 0 | 9 |
| `cHandlingDataMgr` | 11 | 0 | 0 | 11 |
| `cTransmission` | 4 | 0 | 4 | 0 |

</details>

Regenerar desde la raíz: `python contrib/validation_inventory.py`. El informe no cambia hooks.csv ni acredita funciones por sí mismo.
