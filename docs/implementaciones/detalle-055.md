<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1081"></a>

## Globales y helpers: source/app/platform/win/WindowedMode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DXCHECK(HRESULT hr) | [source/app/platform/win/WindowedMode.cpp:20](../../source/app/platform/win/WindowedMode.cpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1082"></a>

## Globales y helpers: source/app/platform/win/WndProc.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectHooksWndProcStuff() | [source/app/platform/win/WndProc.cpp:303](../../source/app/platform/win/WndProc.cpp#L303) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1083"></a>

## Globales y helpers: source/game_sa/3dMarker.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarker.cpp:40](../../source/game_sa/3dMarker.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetAtomicForMarkerType(e3dMarkerType type) | [source/game_sa/3dMarker.cpp:54](../../source/game_sa/3dMarker.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pRpClumpArray: source/game_sa/3dMarkers.h:61 (0x00C7C6DC) |

<a id="grupo-1084"></a>

## Globales y helpers: source/game_sa/3dMarkers.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeStdSize(float sz, float dist) | [source/game_sa/3dMarkers.cpp:39](../../source/game_sa/3dMarkers.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarkers.cpp:56](../../source/game_sa/3dMarkers.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1085"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendAssociation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendAssociationDestructorBodyExe(CAnimBlendAssociation* self, void* /*edx*/) | [source/game_sa/Animation/AnimBlendAssociation.cpp:37](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CAnimBlendAssociationScalarDeletingDestructor(CAnimBlendAssociation* self, void* /*edx*/, uint8 flags) | [source/game_sa/Animation/AnimBlendAssociation.cpp:52](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAnimBlendAssociation::DestructorBody 0x004CECF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1086"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendStaticAssociation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendStaticAssociationDestructorBodyExe(CAnimBlendStaticAssociation* self, void* /*edx*/) | [source/game_sa/Animation/AnimBlendStaticAssociation.cpp:27](../../source/game_sa/Animation/AnimBlendStaticAssociation.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1087"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RoundSectionSize(uint32 size) | [source/game_sa/Animation/AnimManager.cpp:672](../../source/game_sa/Animation/AnimManager.cpp#L672) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewAnimSequences(int32 n) | [source/game_sa/Animation/AnimManager.cpp:677](../../source/game_sa/Animation/AnimManager.cpp#L677) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FloatToKeyFrameWord(float v) | [source/game_sa/Animation/AnimManager.cpp:691](../../source/game_sa/Animation/AnimManager.cpp#L691) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1088"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsClumpSkinned(RpClump *clump) | [source/game_sa/Animation/AnimManager.h:129](../../source/game_sa/Animation/AnimManager.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1089"></a>

## Globales y helpers: source/game_sa/AttractorScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorSectorCell(float coordinate, bool useEnd) | [source/game_sa/AttractorScanner.cpp:78](../../source/game_sa/AttractorScanner.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1090"></a>

## Globales y helpers: source/game_sa/Attractors/PedAtmAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedATMAttractorConstructor(CPedATMAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedAtmAttractor.cpp:27](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedATMAttractorDestructorBody(CPedATMAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedAtmAttractor.cpp:32](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedATMAttractor::DestructorBody 0x005EE530 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedATMAttractorScalarDeletingDestructor(CPedATMAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedAtmAttractor.cpp:42](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedATMAttractor::ScalarDeletingDestructor 0x005EE4E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1091"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SArrayRawSize(const void* self, int32 stride) | [source/game_sa/Attractors/PedAttractor.cpp:256](../../source/game_sa/Attractors/PedAttractor.cpp#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SArrayPushBack4(SArrayRaw&lt;uint32&gt;* self, const uint32* x) | [source/game_sa/Attractors/PedAttractor.cpp:611](../../source/game_sa/Attractors/PedAttractor.cpp#L611) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SArrayInsertN4(void* vector, uint32* pos, uint32 count, const uint32* value) | [source/game_sa/Attractors/PedAttractor.cpp:668](../../source/game_sa/Attractors/PedAttractor.cpp#L668) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedAttractorListPushBack(void* list, CPedAttractor* const* item) | [source/game_sa/Attractors/PedAttractor.cpp:673](../../source/game_sa/Attractors/PedAttractor.cpp#L673) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1092"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractorPedPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:34](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:43](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1093"></a>

## Globales y helpers: source/game_sa/Attractors/PedPizzaAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedPizzaAttractorConstructor(CPedPizzaAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:27](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedPizzaAttractorDestructorBody(CPedPizzaAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:32](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedPizzaAttractor::DestructorBody 0x005EE7B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedPizzaAttractorScalarDeletingDestructor(CPedPizzaAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedPizzaAttractor.cpp:42](../../source/game_sa/Attractors/PedPizzaAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedPizzaAttractor::ScalarDeletingDestructor 0x005EE760 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1094"></a>

## Globales y helpers: source/game_sa/Attractors/PedSeatAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedSeatAttractorConstructor(CPedSeatAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedSeatAttractor.cpp:27](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedSeatAttractorDestructorBody(CPedSeatAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedSeatAttractor.cpp:32](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSeatAttractor::DestructorBody 0x005EE600 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedSeatAttractorScalarDeletingDestructor(CPedSeatAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedSeatAttractor.cpp:42](../../source/game_sa/Attractors/PedSeatAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSeatAttractor::ScalarDeletingDestructor 0x005EE5B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1095"></a>

## Globales y helpers: source/game_sa/Attractors/PedShelterAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedShelterAttractorConstructor(CPedShelterAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedShelterAttractor.cpp:145](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedShelterAttractorDestructorBody(CPedShelterAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedShelterAttractor.cpp:150](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedShelterAttractor::DestructorBody 0x005EE880 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedShelterAttractorScalarDeletingDestructor(CPedShelterAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedShelterAttractor.cpp:160](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedShelterAttractor::ScalarDeletingDestructor 0x005EE830 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1096"></a>

## Globales y helpers: source/game_sa/Attractors/PedStopAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedStopAttractorConstructor(CPedStopAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedStopAttractor.cpp:27](../../source/game_sa/Attractors/PedStopAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedStopAttractorDestructorBody(CPedStopAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedStopAttractor.cpp:32](../../source/game_sa/Attractors/PedStopAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStopAttractor::DestructorBody 0x005EE6D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedStopAttractorScalarDeletingDestructor(CPedStopAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedStopAttractor.cpp:42](../../source/game_sa/Attractors/PedStopAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStopAttractor::ScalarDeletingDestructor 0x005EE680 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1097"></a>

## Globales y helpers: source/game_sa/Attractors/PedTriggerScriptAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedTriggerScriptAttractorConstructor(CPedTriggerScriptAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:27](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedTriggerScriptAttractorDestructorBody(CPedTriggerScriptAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:32](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedTriggerScriptAttractor::DestructorBody 0x005EE950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedTriggerScriptAttractorScalarDeletingDestructor(CPedTriggerScriptAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedTriggerScriptAttractor.cpp:42](../../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedTriggerScriptAttractor::ScalarDeletingDestructor 0x005EE900 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1098"></a>

## Globales y helpers: source/game_sa/Audio/AESmoothFadeThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DsVolumeToDb(int32 dsVolume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:31](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DbToDsVolume(float volume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:36](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1099"></a>

## Globales y helpers: source/game_sa/Audio/AEStreamThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChannelPlayTime(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:30](../../source/game_sa/Audio/AEStreamThread.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ChannelLength(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:34](../../source/game_sa/Audio/AEStreamThread.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1100"></a>

## Globales y helpers: source/game_sa/Audio/AudioEngine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SExt(E v) | [source/game_sa/Audio/AudioEngine.cpp:37](../../source/game_sa/Audio/AudioEngine.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CallMgr(M method, A... args) | [source/game_sa/Audio/AudioEngine.cpp:39](../../source/game_sa/Audio/AudioEngine.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AERadioTrackManager: source/game_sa/Audio/Managers/AERadioTrackManager.cpp:37 (0x008CB6F8) |
