<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11770 con evidencia histórica y 671 pendientes de validación en 24 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1061"></a>

## Globales y helpers: source/game_sa/3dMarkers.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeStdSize(float sz, float dist) | [source/game_sa/3dMarkers.cpp:38](../../source/game_sa/3dMarkers.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarkers.cpp:55](../../source/game_sa/3dMarkers.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1062"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendAssociation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendAssociationDestructorBodyExe(CAnimBlendAssociation* self, void* /*edx*/) | [source/game_sa/Animation/AnimBlendAssociation.cpp:28](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CAnimBlendAssociationScalarDeletingDestructor(CAnimBlendAssociation* self, void* /*edx*/, uint8 flags) | [source/game_sa/Animation/AnimBlendAssociation.cpp:43](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAnimBlendAssociation::DestructorBody 0x004CECF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1063"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendStaticAssociation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendStaticAssociationDestructorBodyExe(CAnimBlendStaticAssociation* self, void* /*edx*/) | [source/game_sa/Animation/AnimBlendStaticAssociation.cpp:27](../../source/game_sa/Animation/AnimBlendStaticAssociation.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1064"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RoundSectionSize(uint32 size) | [source/game_sa/Animation/AnimManager.cpp:671](../../source/game_sa/Animation/AnimManager.cpp#L671) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewAnimSequences(int32 n) | [source/game_sa/Animation/AnimManager.cpp:676](../../source/game_sa/Animation/AnimManager.cpp#L676) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FloatToKeyFrameWord(float v) | [source/game_sa/Animation/AnimManager.cpp:690](../../source/game_sa/Animation/AnimManager.cpp#L690) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1065"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsClumpSkinned(RpClump *clump) | [source/game_sa/Animation/AnimManager.h:127](../../source/game_sa/Animation/AnimManager.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1066"></a>

## Globales y helpers: source/game_sa/AttractorScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorSectorCell(float coordinate, bool useEnd) | [source/game_sa/AttractorScanner.cpp:77](../../source/game_sa/AttractorScanner.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1067"></a>

## Globales y helpers: source/game_sa/Attractors/PedAtmAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedATMAttractorConstructor(CPedATMAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedAtmAttractor.cpp:27](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedATMAttractorDestructorBody(CPedATMAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedAtmAttractor.cpp:32](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedATMAttractor::DestructorBody 0x005EE530 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedATMAttractorScalarDeletingDestructor(CPedATMAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedAtmAttractor.cpp:42](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedATMAttractor::ScalarDeletingDestructor 0x005EE4E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1068"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SArrayPushBack4(SArrayRaw&lt;uint32&gt;* self, const uint32* x) | [source/game_sa/Attractors/PedAttractor.cpp:589](../../source/game_sa/Attractors/PedAttractor.cpp#L589) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1069"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractorPedPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:34](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:43](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1070"></a>

## Globales y helpers: source/game_sa/Attractors/PedPizzaAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedPizzaAttractorConstructor(CPedPizzaAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:27](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedPizzaAttractorDestructorBody(CPedPizzaAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:32](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedPizzaAttractor::DestructorBody 0x005EE7B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedPizzaAttractorScalarDeletingDestructor(CPedPizzaAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:42](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedPizzaAttractor::ScalarDeletingDestructor 0x005EE760 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1071"></a>

## Globales y helpers: source/game_sa/Attractors/PedSeatAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedSeatAttractorConstructor(CPedSeatAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedSeatAttractor.cpp:27](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedSeatAttractorDestructorBody(CPedSeatAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedSeatAttractor.cpp:32](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSeatAttractor::DestructorBody 0x005EE600 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedSeatAttractorScalarDeletingDestructor(CPedSeatAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedSeatAttractor.cpp:42](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSeatAttractor::ScalarDeletingDestructor 0x005EE5B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1072"></a>

## Globales y helpers: source/game_sa/Attractors/PedShelterAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedShelterAttractorConstructor(CPedShelterAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedShelterAttractor.cpp:144](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedShelterAttractorDestructorBody(CPedShelterAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedShelterAttractor.cpp:149](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedShelterAttractor::DestructorBody 0x005EE880 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedShelterAttractorScalarDeletingDestructor(CPedShelterAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedShelterAttractor.cpp:159](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L159) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedShelterAttractor::ScalarDeletingDestructor 0x005EE830 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1073"></a>

## Globales y helpers: source/game_sa/Attractors/PedStopAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedStopAttractorConstructor(CPedStopAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedStopAttractor.cpp:27](../../source/game_sa/Attractors/PedStopAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedStopAttractorDestructorBody(CPedStopAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedStopAttractor.cpp:32](../../source/game_sa/Attractors/PedStopAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStopAttractor::DestructorBody 0x005EE6D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedStopAttractorScalarDeletingDestructor(CPedStopAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedStopAttractor.cpp:42](../../source/game_sa/Attractors/PedStopAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStopAttractor::ScalarDeletingDestructor 0x005EE680 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1074"></a>

## Globales y helpers: source/game_sa/Attractors/PedTriggerScriptAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedTriggerScriptAttractorConstructor(CPedTriggerScriptAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:27](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedTriggerScriptAttractorDestructorBody(CPedTriggerScriptAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:32](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedTriggerScriptAttractor::DestructorBody 0x005EE950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedTriggerScriptAttractorScalarDeletingDestructor(CPedTriggerScriptAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:42](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedTriggerScriptAttractor::ScalarDeletingDestructor 0x005EE900 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1075"></a>

## Globales y helpers: source/game_sa/Audio/AESmoothFadeThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DsVolumeToDb(int32 dsVolume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:31](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DbToDsVolume(float volume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:36](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1076"></a>

## Globales y helpers: source/game_sa/Audio/AEStreamThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChannelPlayTime(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:30](../../source/game_sa/Audio/AEStreamThread.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ChannelLength(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:34](../../source/game_sa/Audio/AEStreamThread.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1077"></a>

## Globales y helpers: source/game_sa/Audio/AudioEngine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SExt(E v) | [source/game_sa/Audio/AudioEngine.cpp:37](../../source/game_sa/Audio/AudioEngine.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CallMgr(M method, A... args) | [source/game_sa/Audio/AudioEngine.cpp:39](../../source/game_sa/Audio/AudioEngine.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AERadioTrackManager: source/game_sa/Audio/Managers/AERadioTrackManager.cpp:30 (0x008CB6F8) |

<a id="grupo-1078"></a>

## Globales y helpers: source/game_sa/Audio/AudioZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AudioZonesTextBufferClear(TextBufferRaw* self, void* /*edx*/) | [source/game_sa/Audio/AudioZones.cpp:59](../../source/game_sa/Audio/AudioZones.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAudioZones::TextBufferClear 0x00508A10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1079"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearCollisionSoundEntry(tCollisionSound& entry) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:38](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Status: source/game_sa/Darkel.h:34 (0x0096A704) |
| GetLoopingCollisionMagnitude(const CVector& v, const CVector& t, bool zFirst) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:49](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEntityModel(const CEntity* entity, ModelIndex mi) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:60](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1080"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddCosMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:31](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddSinMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:36](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
