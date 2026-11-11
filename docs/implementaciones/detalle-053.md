<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 884 grupos / 10050 funciones reconstruidas; 10050 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1041"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendAssociation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendAssociationDestructorBodyExe(CAnimBlendAssociation* self, void* /*edx*/) | [source/game_sa/Animation/AnimBlendAssociation.cpp:28](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CAnimBlendAssociationScalarDeletingDestructor(CAnimBlendAssociation* self, void* /*edx*/, uint8 flags) | [source/game_sa/Animation/AnimBlendAssociation.cpp:43](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAnimBlendAssociation::DestructorBody 0x004CECF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1042"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RoundSectionSize(uint32 size) | [source/game_sa/Animation/AnimManager.cpp:653](../../source/game_sa/Animation/AnimManager.cpp#L653) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewAnimSequences(int32 n) | [source/game_sa/Animation/AnimManager.cpp:658](../../source/game_sa/Animation/AnimManager.cpp#L658) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FloatToKeyFrameWord(float v) | [source/game_sa/Animation/AnimManager.cpp:672](../../source/game_sa/Animation/AnimManager.cpp#L672) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1043"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsClumpSkinned(RpClump *clump) | [source/game_sa/Animation/AnimManager.h:127](../../source/game_sa/Animation/AnimManager.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1044"></a>

## Globales y helpers: source/game_sa/AttractorScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorSectorCell(float coordinate, bool useEnd) | [source/game_sa/AttractorScanner.cpp:59](../../source/game_sa/AttractorScanner.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1045"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractorPedPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:34](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:43](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1046"></a>

## Globales y helpers: source/game_sa/Audio/AESmoothFadeThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DsVolumeToDb(int32 dsVolume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:31](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DbToDsVolume(float volume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:36](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1047"></a>

## Globales y helpers: source/game_sa/Audio/AEStreamThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChannelPlayTime(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:30](../../source/game_sa/Audio/AEStreamThread.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ChannelLength(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:34](../../source/game_sa/Audio/AEStreamThread.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1048"></a>

## Globales y helpers: source/game_sa/Audio/AudioEngine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SExt(E v) | [source/game_sa/Audio/AudioEngine.cpp:37](../../source/game_sa/Audio/AudioEngine.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CallMgr(M method, A... args) | [source/game_sa/Audio/AudioEngine.cpp:39](../../source/game_sa/Audio/AudioEngine.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AERadioTrackManager: source/game_sa/Audio/Managers/AERadioTrackManager.cpp:30 (0x008CB6F8) |

<a id="grupo-1049"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearCollisionSoundEntry(tCollisionSound& entry) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:38](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Status: source/game_sa/Darkel.h:34 (0x0096A704) |
| GetLoopingCollisionMagnitude(const CVector& v, const CVector& t, bool zFirst) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:49](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEntityModel(const CEntity* entity, ModelIndex mi) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:60](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1050"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddCosMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:31](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddSinMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:36](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1051"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEPedAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedShirtLogarithmicVolume(float value, float baseVolume) | [source/game_sa/Audio/Entities/AEPedAudioEntity.cpp:758](../../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp#L758) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1052"></a>

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

<a id="grupo-1053"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetVehicleAudioSettings(eModelID model) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:283](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1054"></a>

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
| PlayGenericSound( CAEVehicleAudioEntity* self, CAESound*& s, eSoundBankSlot slot, eSoundID& currSfx, eSoundID newSfx, float speed, float volume, float rollOff ) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:2060](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L2060) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftDistantSoundVolume(const CPhysical* v, float baseVol) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3483](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftCameraPoVFactor(const CPhysical* p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3496](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3496) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3501](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3501) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCRotorSpeedFactor(float p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3512](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3512) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPropSpeedFactor(float propSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3628](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3628) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePropSpeed(const CPlane* plane, float gas, float brake) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3637](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3637) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePlanePropFreq(CAEVehicleAudioEntity::tVehicleParams& vp, float ac) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3644](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3644) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeClamp01(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3663](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3663) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropSpeedFactor(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3668](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3668) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropOrientation(const CEntity* e, float& R, float& Q) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3674](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3674) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropCamFactor(const CVector& entityPos, const CEntity* vehicle) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3685](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3685) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropDistantVolume(const CVector& entityPos, float pf) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3694](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3694) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliVolFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3925](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3925) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliFreqFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3929](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3929) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliRotorSpeed(const CVehicle* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3934](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3934) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliTilt(const CEntity* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3938](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3938) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliDamageFreq(const CVehicle* v, float f0) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3943](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3943) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorSpeed(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3953](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3953) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVolumeFactorForHeliRotor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3958](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3958) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorFreqFactor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3969](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3969) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliForwardTilt(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3980](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3980) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4381](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4381) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeed(const CTrain* train) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4506](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4506) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainAbsClamp01(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4518](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4518) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeedFactor(const CTrain* train, float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4525](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4525) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainDummyAcFactor(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4535](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4535) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessBicycleAudio(CAEVehicleAudioEntity& self, CAEVehicleAudioEntity::tVehicleParams& params, bool player) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5018](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5018) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2GetFreqForIdle_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, float ratio) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5468](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5468) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3DriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, float revs, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5480](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5480) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3NonDriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5484](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5484) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehAudioPlayerVolume_KeepEcx(const CAEVehicleAudioEntity* self, uint32 /*entryEdx*/, CAEVehicleAudioEntity::tVehicleParams* vp, CAEVehicleAudioEntity::eVehicleEngineSoundType st) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5491](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1055"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(CAEVehicleAudioEntity::eAEState s) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:881](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L881) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(CAEVehicleAudioEntity::eAircraftSoundType s) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:900](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L900) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(CAEVehicleAudioEntity::eCarEngineSoundType st) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:914](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L914) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(eAERadioType rt) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:933](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L933) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(eAEVehicleSoundType v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:944](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L944) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1056"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DefaultVolumeInt(eAudioEvents event) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:48](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FadeWeatherSoundOut(CAESound* sound) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:53](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1057"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleDoorType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleDoorType v) | [source/game_sa/Audio/Enums/eAEVehicleDoorType.h:14](../../source/game_sa/Audio/Enums/eAEVehicleDoorType.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1058"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleHornType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleHornType v) | [source/game_sa/Audio/Enums/eAEVehicleHornType.h:25](../../source/game_sa/Audio/Enums/eAEVehicleHornType.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1059"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eBassSetting.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eBassSetting v) | [source/game_sa/Audio/Enums/eBassSetting.h:14](../../source/game_sa/Audio/Enums/eBassSetting.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1060"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBank.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBank v) | [source/game_sa/Audio/Enums/eSoundBank.h:740](../../source/game_sa/Audio/Enums/eSoundBank.h#L740) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
