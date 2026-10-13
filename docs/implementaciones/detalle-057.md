<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/Enums/eGameState.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eGameState gs) | [source/game_sa/Enums/eGameState.h:16](../../source/game_sa/Enums/eGameState.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/Enums/eIKArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IKArmToIKSlot(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:11](../../source/game_sa/Enums/eIKArm.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IKArmToBoneTag(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:19](../../source/game_sa/Enums/eIKArm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/Enums/eModelID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelIDForCutScene(eModelID modelId) | [source/game_sa/Enums/eModelID.h:14868](../../source/game_sa/Enums/eModelID.h#L14868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/Enums/ePedType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGang(ePedType ptype) | [source/game_sa/Enums/ePedType.h:61](../../source/game_sa/Enums/ePedType.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypePlayer(ePedType pt) | [source/game_sa/Enums/ePedType.h:65](../../source/game_sa/Enums/ePedType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale(ePedType type) | [source/game_sa/Enums/ePedType.h:74](../../source/game_sa/Enums/ePedType.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAllGangPedTypes() | [source/game_sa/Enums/ePedType.h:83](../../source/game_sa/Enums/ePedType.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGangOfPedType(ePedType ptype) | [source/game_sa/Enums/ePedType.h:114](../../source/game_sa/Enums/ePedType.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/Enums/eRadioID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eRadioID v) | [source/game_sa/Enums/eRadioID.h:30](../../source/game_sa/Enums/eRadioID.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1127"></a>

## Globales y helpers: source/game_sa/Enums/eTaskType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eTaskType taskType) | [source/game_sa/Enums/eTaskType.h:578](../../source/game_sa/Enums/eTaskType.h#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/Enums/eWeaponType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsWeaponTypeProjectile(eWeaponType weaponType) | [source/game_sa/Enums/eWeaponType.h:82](../../source/game_sa/Enums/eWeaponType.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1129"></a>

## Globales y helpers: source/game_sa/Enums/eWeatherType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsExtraSunny(eWeatherType wt) | [source/game_sa/Enums/eWeatherType.h:39](../../source/game_sa/Enums/eWeatherType.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/Events/Event.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEvent | [source/game_sa/Events/Event.h:12](../../source/game_sa/Events/Event.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) | [source/game_sa/Events/Event.h:28](../../source/game_sa/Events/Event.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPedGroup(CPedGroup* pedGroup) | [source/game_sa/Events/Event.h:29](../../source/game_sa/Events/Event.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCriminalEvent() | [source/game_sa/Events/Event.h:30](../../source/game_sa/Events/Event.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasEditableResponse() const | [source/game_sa/Events/Event.h:32](../../source/game_sa/Events/Event.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const | [source/game_sa/Events/Event.h:33](../../source/game_sa/Events/Event.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& other) | [source/game_sa/Events/Event.h:34](../../source/game_sa/Events/Event.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() | [source/game_sa/Events/Event.h:35](../../source/game_sa/Events/Event.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoInformVehicleOccupants(CPed* ped) | [source/game_sa/Events/Event.h:36](../../source/game_sa/Events/Event.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValid(CPed* ped) | [source/game_sa/Events/Event.h:37](../../source/game_sa/Events/Event.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() | [source/game_sa/Events/Event.h:38](../../source/game_sa/Events/Event.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UnTick() | [source/game_sa/Events/Event.h:43](../../source/game_sa/Events/Event.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Tick() | [source/game_sa/Events/Event.h:44](../../source/game_sa/Events/Event.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| classof(const From* f) | [source/game_sa/Events/Event.h:46](../../source/game_sa/Events/Event.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventAcquaintancePed.h:10](../../source/game_sa/Events/EventAcquaintancePed.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventAcquaintancePed.h:18](../../source/game_sa/Events/EventAcquaintancePed.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventAcquaintancePed.h:21](../../source/game_sa/Events/EventAcquaintancePed.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventAcquaintancePed.h:23](../../source/game_sa/Events/EventAcquaintancePed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedDislike.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedDislike | [source/game_sa/Events/EventAcquaintancePedDislike.h:10](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedDislike.h:14](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedDislike.h:15](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedDislike.h:16](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHate.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedHate | [source/game_sa/Events/EventAcquaintancePedHate.h:10](../../source/game_sa/Events/EventAcquaintancePedHate.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedHate.h:14](../../source/game_sa/Events/EventAcquaintancePedHate.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedHate.h:15](../../source/game_sa/Events/EventAcquaintancePedHate.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedHate.h:16](../../source/game_sa/Events/EventAcquaintancePedHate.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedHateBadlyLit | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:10](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:19](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:20](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:22](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:23](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedLike.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedLike | [source/game_sa/Events/EventAcquaintancePedLike.h:10](../../source/game_sa/Events/EventAcquaintancePedLike.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedLike.h:14](../../source/game_sa/Events/EventAcquaintancePedLike.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedLike.h:15](../../source/game_sa/Events/EventAcquaintancePedLike.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedLike.h:16](../../source/game_sa/Events/EventAcquaintancePedLike.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedRespect.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedRespect | [source/game_sa/Events/EventAcquaintancePedRespect.h:10](../../source/game_sa/Events/EventAcquaintancePedRespect.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedRespect.h:14](../../source/game_sa/Events/EventAcquaintancePedRespect.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedRespect.h:15](../../source/game_sa/Events/EventAcquaintancePedRespect.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedRespect.h:16](../../source/game_sa/Events/EventAcquaintancePedRespect.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/Events/EventAreaCodes.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAreaCodes | [source/game_sa/Events/EventAreaCodes.h:10](../../source/game_sa/Events/EventAreaCodes.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAreaCodes.h:19](../../source/game_sa/Events/EventAreaCodes.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAreaCodes.h:20](../../source/game_sa/Events/EventAreaCodes.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventAreaCodes.h:21](../../source/game_sa/Events/EventAreaCodes.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventAreaCodes.h:22](../../source/game_sa/Events/EventAreaCodes.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/Events/EventAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventAttractor.h:8](../../source/game_sa/Events/EventAttractor.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAttractor.h:20](../../source/game_sa/Events/EventAttractor.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAttractor.h:21](../../source/game_sa/Events/EventAttractor.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventAttractor.h:22](../../source/game_sa/Events/EventAttractor.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/Events/EventBuildingCollision.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventBuildingCollision | [source/game_sa/Events/EventBuildingCollision.h:14](../../source/game_sa/Events/EventBuildingCollision.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventBuildingCollision.h:27](../../source/game_sa/Events/EventBuildingCollision.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventBuildingCollision.h:28](../../source/game_sa/Events/EventBuildingCollision.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventBuildingCollision.h:29](../../source/game_sa/Events/EventBuildingCollision.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventBuildingCollision.h:30](../../source/game_sa/Events/EventBuildingCollision.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventBuildingCollision.h:31](../../source/game_sa/Events/EventBuildingCollision.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventBuildingCollision.h:32](../../source/game_sa/Events/EventBuildingCollision.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/Events/EventCarUpsideDown.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventCarUpsideDown | [source/game_sa/Events/EventCarUpsideDown.h:7](../../source/game_sa/Events/EventCarUpsideDown.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventCarUpsideDown.h:15](../../source/game_sa/Events/EventCarUpsideDown.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventCarUpsideDown.h:16](../../source/game_sa/Events/EventCarUpsideDown.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventCarUpsideDown.h:17](../../source/game_sa/Events/EventCarUpsideDown.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventCarUpsideDown.h:18](../../source/game_sa/Events/EventCarUpsideDown.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
