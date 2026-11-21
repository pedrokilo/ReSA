# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1081"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractorPedPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:34](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:43](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1082"></a>

## Globales y helpers: source/game_sa/Attractors/PedPizzaAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedPizzaAttractorConstructor(CPedPizzaAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:27](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedPizzaAttractorDestructorBody(CPedPizzaAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:32](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedPizzaAttractor::DestructorBody 0x005EE7B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedPizzaAttractorScalarDeletingDestructor(CPedPizzaAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:42](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedPizzaAttractor::ScalarDeletingDestructor 0x005EE760 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1083"></a>

## Globales y helpers: source/game_sa/Attractors/PedSeatAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedSeatAttractorConstructor(CPedSeatAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedSeatAttractor.cpp:27](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedSeatAttractorDestructorBody(CPedSeatAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedSeatAttractor.cpp:32](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSeatAttractor::DestructorBody 0x005EE600 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedSeatAttractorScalarDeletingDestructor(CPedSeatAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedSeatAttractor.cpp:42](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSeatAttractor::ScalarDeletingDestructor 0x005EE5B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1084"></a>

## Globales y helpers: source/game_sa/Attractors/PedShelterAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedShelterAttractorConstructor(CPedShelterAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedShelterAttractor.cpp:145](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedShelterAttractorDestructorBody(CPedShelterAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedShelterAttractor.cpp:150](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedShelterAttractor::DestructorBody 0x005EE880 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedShelterAttractorScalarDeletingDestructor(CPedShelterAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedShelterAttractor.cpp:160](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedShelterAttractor::ScalarDeletingDestructor 0x005EE830 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1085"></a>

## Globales y helpers: source/game_sa/Attractors/PedStopAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedStopAttractorConstructor(CPedStopAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedStopAttractor.cpp:27](../../source/game_sa/Attractors/PedStopAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedStopAttractorDestructorBody(CPedStopAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedStopAttractor.cpp:32](../../source/game_sa/Attractors/PedStopAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStopAttractor::DestructorBody 0x005EE6D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedStopAttractorScalarDeletingDestructor(CPedStopAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedStopAttractor.cpp:42](../../source/game_sa/Attractors/PedStopAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStopAttractor::ScalarDeletingDestructor 0x005EE680 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1086"></a>

## Globales y helpers: source/game_sa/Attractors/PedTriggerScriptAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedTriggerScriptAttractorConstructor(CPedTriggerScriptAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:27](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedTriggerScriptAttractorDestructorBody(CPedTriggerScriptAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:32](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedTriggerScriptAttractor::DestructorBody 0x005EE950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedTriggerScriptAttractorScalarDeletingDestructor(CPedTriggerScriptAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:42](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedTriggerScriptAttractor::ScalarDeletingDestructor 0x005EE900 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1087"></a>

## Globales y helpers: source/game_sa/Audio/AESmoothFadeThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DsVolumeToDb(int32 dsVolume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:31](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DbToDsVolume(float volume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:36](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1088"></a>

## Globales y helpers: source/game_sa/Audio/AEStreamThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChannelPlayTime(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:30](../../source/game_sa/Audio/AEStreamThread.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ChannelLength(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:34](../../source/game_sa/Audio/AEStreamThread.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1089"></a>

## Globales y helpers: source/game_sa/Audio/AudioEngine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SExt(E v) | [source/game_sa/Audio/AudioEngine.cpp:37](../../source/game_sa/Audio/AudioEngine.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CallMgr(M method, A... args) | [source/game_sa/Audio/AudioEngine.cpp:39](../../source/game_sa/Audio/AudioEngine.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AERadioTrackManager: source/game_sa/Audio/Managers/AERadioTrackManager.cpp:30 (0x008CB6F8) |

<a id="grupo-1090"></a>

## Globales y helpers: source/game_sa/Audio/AudioZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AudioZonesTextBufferClear(TextBufferRaw* self, void* /*edx*/) | [source/game_sa/Audio/AudioZones.cpp:62](../../source/game_sa/Audio/AudioZones.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAudioZones::TextBufferClear 0x00508A10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AudioZonesTextBufferAddInt(TextBufferRaw* self, void* /*edx*/, int32 value) | [source/game_sa/Audio/AudioZones.cpp:85](../../source/game_sa/Audio/AudioZones.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: CAudioZones::TextBufferAddInt 0x00508B80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AudioZonesTextBufferAddTyped(TextBufferRaw* self, void* /*edx*/, const char* key, const char* type) | [source/game_sa/Audio/AudioZones.cpp:93](../../source/game_sa/Audio/AudioZones.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: CAudioZones::TextBufferAddTyped 0x00508D30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1091"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearCollisionSoundEntry(tCollisionSound& entry) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:38](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Status: source/game_sa/Darkel.h:34 (0x0096A704) |
| GetLoopingCollisionMagnitude(const CVector& v, const CVector& t, bool zFirst) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:49](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEntityModel(const CEntity* entity, ModelIndex mi) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:60](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1092"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddCosMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:31](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddSinMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:36](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1093"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEPedAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedShirtLogarithmicVolume(float value, float baseVolume) | [source/game_sa/Audio/Entities/AEPedAudioEntity.cpp:758](../../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp#L758) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1094"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ExeSpeechContextEntry(int16 idx) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:120](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindExeSpeechContext(int16 gCtx) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:125](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeVoiceBank(const tExeSpeechTypeTables& t, int16 voice) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:146](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ExeRange(const tExeSpeechTypeTables& t, int16 sCtx, int16 voice) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:151](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ExeGenRange(int16 sCtx, int16 voice) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:156](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SpeechOf(CPed* ped) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:161](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & SpeechSlot(int32 idx) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:165](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo s_PedSpeechSlots: source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h:170 (0x00B61C38) |
| * GetFacialTask(CEntity* entity) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:170](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1095"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TwinLoopSoundEntityDestructorExe(CAETwinLoopSoundEntity* self, void* /*edx*/) | [source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp:29](../../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1096"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetVehicleAudioSettings(eModelID model) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:283](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1097"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AudioLog10ScaledExe(float input, float factor) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:36](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSurfaceAudioGrass(eSurfaceType surface) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:57](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| IsSurfaceWet(eSurfaceType surface) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:61](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| GracefullyStopSound(CAESound*& sound) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:65](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StopAndForgetSound(CAESound*& sound) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:72](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNumOfContactWheels(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:78](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLerp(float lo, float hi, float t) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:91](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeInlinePiecewise5(float x, const float (&p)\[5\]\[2\]) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:98](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2Clamp01Exe(float r) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:117](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedalToRaw(float v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:122](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PlayVehicleSound(CAESoundManager::tSoundPlayParams p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:129](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AESoundManager: source/game_sa/Audio/Managers/AESoundManager.cpp:29 (0x00B62CB0) |
| PlayGenericSound( CAEVehicleAudioEntity* self, CAESound*& s, eSoundBankSlot slot, eSoundID& currSfx, eSoundID newSfx, float speed, float volume, float rollOff ) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:2061](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L2061) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftDistantSoundVolume(const CPhysical* v, float baseVol) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3484](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3484) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftCameraPoVFactor(const CPhysical* p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3497](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3497) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3502](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCRotorSpeedFactor(float p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3513](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3513) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPropSpeedFactor(float propSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3629](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3629) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePropSpeed(const CPlane* plane, float gas, float brake) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3638](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3638) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePlanePropFreq(CAEVehicleAudioEntity::tVehicleParams& vp, float ac) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3645](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3645) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeClamp01(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3664](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3664) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropSpeedFactor(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3669](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3669) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropOrientation(const CEntity* e, float& R, float& Q) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3675](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3675) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropCamFactor(const CVector& entityPos, const CEntity* vehicle) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3686](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3686) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropDistantVolume(const CVector& entityPos, float pf) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3695](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3695) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliVolFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3926](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3926) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliFreqFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3930](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3930) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliRotorSpeed(const CVehicle* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3935](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3935) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliTilt(const CEntity* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3939](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3939) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliDamageFreq(const CVehicle* v, float f0) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3944](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3944) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorSpeed(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3954](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3954) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVolumeFactorForHeliRotor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3959](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3959) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorFreqFactor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3970](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3970) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliForwardTilt(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3981](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3981) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4382](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4382) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeed(const CTrain* train) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4507](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4507) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainAbsClamp01(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4519](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4519) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeedFactor(const CTrain* train, float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4526](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4526) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainDummyAcFactor(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4536](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessBicycleAudio(CAEVehicleAudioEntity& self, CAEVehicleAudioEntity::tVehicleParams& params, bool player) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5020](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5020) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2GetFreqForIdle_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, float ratio) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5470](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5470) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3DriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, float revs, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5482](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5482) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3NonDriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5487](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5487) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAEVehicleAudioEntity::GetVehicleNonDriveWheelSkidValue 0x004F6000 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehAudioPlayerVolume_KeepEcx(const CAEVehicleAudioEntity* self, uint32 /*entryEdx*/, CAEVehicleAudioEntity::tVehicleParams* vp, CAEVehicleAudioEntity::eVehicleEngineSoundType st) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5495](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5495) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAEVehicleAudioEntity::GetVolForPlayerEngineSound 0x004F5D00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1098"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(CAEVehicleAudioEntity::eAEState s) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:881](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L881) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(CAEVehicleAudioEntity::eAircraftSoundType s) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:900](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L900) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(CAEVehicleAudioEntity::eCarEngineSoundType st) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:914](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L914) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(eAERadioType rt) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:933](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L933) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(eAEVehicleSoundType v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:944](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L944) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1099"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DefaultVolumeInt(eAudioEvents event) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:48](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FadeWeatherSoundOut(CAESound* sound) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:53](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall WeatherAudioEntityConstructorBody(CAEWeatherAudioEntity* self, void* /*edx*/) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:68](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1100"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleDoorType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleDoorType v) | [source/game_sa/Audio/Enums/eAEVehicleDoorType.h:14](../../source/game_sa/Audio/Enums/eAEVehicleDoorType.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
