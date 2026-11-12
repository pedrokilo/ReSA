<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10344 funciones reconstruidas; 10344 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1161"></a>

## Globales y helpers: source/game_sa/EntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) | [source/game_sa/EntryExit.cpp:36](../../source/game_sa/EntryExit.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitGetPositionRelativeToOutsideWorldRegs(CEntryExit* self, uint32 entryEdx, CVector* outPos) | [source/game_sa/EntryExit.cpp:44](../../source/game_sa/EntryExit.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EntryExitGetPositionExe(CEntryExit* self, void* /*edx*/, CVector* out) | [source/game_sa/EntryExit.cpp:56](../../source/game_sa/EntryExit.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExit::GetPosition 0x0043E090 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1162"></a>

## Globales y helpers: source/game_sa/EntryExitManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawArea(const eAreaCodesS8& area) | [source/game_sa/EntryExitManager.cpp:93](../../source/game_sa/EntryExitManager.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CurrArea() | [source/game_sa/EntryExitManager.cpp:97](../../source/game_sa/EntryExitManager.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1163"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1164"></a>

## Globales y helpers: source/game_sa/Enums/eGameState.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eGameState gs) | [source/game_sa/Enums/eGameState.h:16](../../source/game_sa/Enums/eGameState.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1165"></a>

## Globales y helpers: source/game_sa/Enums/eIKArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IKArmToIKSlot(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:11](../../source/game_sa/Enums/eIKArm.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IKArmToBoneTag(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:19](../../source/game_sa/Enums/eIKArm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1166"></a>

## Globales y helpers: source/game_sa/Enums/eModelID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelIDForCutScene(eModelID modelId) | [source/game_sa/Enums/eModelID.h:14868](../../source/game_sa/Enums/eModelID.h#L14868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1167"></a>

## Globales y helpers: source/game_sa/Enums/ePedType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGang(ePedType ptype) | [source/game_sa/Enums/ePedType.h:61](../../source/game_sa/Enums/ePedType.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypePlayer(ePedType pt) | [source/game_sa/Enums/ePedType.h:65](../../source/game_sa/Enums/ePedType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale(ePedType type) | [source/game_sa/Enums/ePedType.h:74](../../source/game_sa/Enums/ePedType.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAllGangPedTypes() | [source/game_sa/Enums/ePedType.h:83](../../source/game_sa/Enums/ePedType.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGangOfPedType(ePedType ptype) | [source/game_sa/Enums/ePedType.h:114](../../source/game_sa/Enums/ePedType.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1168"></a>

## Globales y helpers: source/game_sa/Enums/eRadioID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eRadioID v) | [source/game_sa/Enums/eRadioID.h:30](../../source/game_sa/Enums/eRadioID.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1169"></a>

## Globales y helpers: source/game_sa/Enums/eTaskType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eTaskType taskType) | [source/game_sa/Enums/eTaskType.h:578](../../source/game_sa/Enums/eTaskType.h#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1170"></a>

## Globales y helpers: source/game_sa/Enums/eWeaponType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsWeaponTypeProjectile(eWeaponType weaponType) | [source/game_sa/Enums/eWeaponType.h:82](../../source/game_sa/Enums/eWeaponType.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1171"></a>

## Globales y helpers: source/game_sa/Enums/eWeatherType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsExtraSunny(eWeatherType wt) | [source/game_sa/Enums/eWeatherType.h:39](../../source/game_sa/Enums/eWeatherType.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1172"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HateBadlyLitDestructorBody(CEventAcquaintancePedHateBadlyLit* self, void* /*edx*/) | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp:29](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAcquaintancePedHateBadlyLit::DestructorBody 0x005FF2C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1173"></a>

## Globales y helpers: source/game_sa/Events/EventAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorDestructorBody(CEventAttractor* self, void* /*edx*/) | [source/game_sa/Events/EventAttractor.cpp:28](../../source/game_sa/Events/EventAttractor.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAttractor::Destructor 0x004AF400 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1174"></a>

## Globales y helpers: source/game_sa/Events/EventBuildingCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BuildingCollisionDestructorBody(CEventBuildingCollision* self, void* /*edx*/) | [source/game_sa/Events/EventBuildingCollision.cpp:26](../../source/game_sa/Events/EventBuildingCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventBuildingCollision::Destructor 0x004ACFF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1175"></a>

## Globales y helpers: source/game_sa/Events/EventDamage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeHitAnimDelayTimer(float delay) | [source/game_sa/Events/EventDamage.cpp:447](../../source/game_sa/Events/EventDamage.cpp#L447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1176"></a>

## Globales y helpers: source/game_sa/Events/EventDanger.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DangerDestructorBody(CEventDanger* self, void* /*edx*/) | [source/game_sa/Events/EventDanger.cpp:26](../../source/game_sa/Events/EventDanger.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDanger::Destructor 0x004B26A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1177"></a>

## Globales y helpers: source/game_sa/Events/EventDeadPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadPedDestructorBody(CEventDeadPed* self, void* /*edx*/) | [source/game_sa/Events/EventDeadPed.cpp:28](../../source/game_sa/Events/EventDeadPed.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1178"></a>

## Globales y helpers: source/game_sa/Events/EventDontJoinPlayerGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DontJoinPlayerGroupDestructorBody(CEventDontJoinPlayerGroup* self, void* /*edx*/) | [source/game_sa/Events/EventDontJoinPlayerGroup.cpp:30](../../source/game_sa/Events/EventDontJoinPlayerGroup.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDontJoinPlayerGroup::Destructor 0x00609140 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CEventDontJoinPlayerGroup() | [source/game_sa/Events/EventDontJoinPlayerGroup.cpp:50](../../source/game_sa/Events/EventDontJoinPlayerGroup.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1179"></a>

## Globales y helpers: source/game_sa/Events/EventDraggedOutCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DraggedOutCarDestructorBody(CEventDraggedOutCar* self, void* /*edx*/) | [source/game_sa/Events/EventDraggedOutCar.cpp:26](../../source/game_sa/Events/EventDraggedOutCar.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1180"></a>

## Globales y helpers: source/game_sa/Events/EventEscalator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EscalatorDestructorBody(CEventEscalator* self, void* /*edx*/) | [source/game_sa/Events/EventEscalator.cpp:30](../../source/game_sa/Events/EventEscalator.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CEventEscalator() | [source/game_sa/Events/EventEscalator.cpp:46](../../source/game_sa/Events/EventEscalator.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CEventOperatorDelete() | [source/game_sa/Events/EventEscalator.cpp:47](../../source/game_sa/Events/EventEscalator.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
