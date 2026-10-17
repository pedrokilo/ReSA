<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1721"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKPointArm | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKPointArm::Destructor 0x00634240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(const char* purpose, eIKArm arm, CEntity* targetEntity, eBoneTag32 bone, RwV3d offsetPosn, float speed, int32 blendTime) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L43) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1722"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleInAir | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h#L15) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1723"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleJetPack | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h#L19) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1724"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:112](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1725"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleJump | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1726"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShakePadIfPlayerVehicle(CVehicle* car) | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1727"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleKillPedWithCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1728"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleLand | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1729"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleLeaveGroup | [source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1730"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunTimedAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1731"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleNone | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLeaveGroup::Destructor 0x00463560 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleNone::GetTaskType 0x004635C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleNone::Clone 0x00463570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPed(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleNone::MakeAbortable 0x004635D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1732"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleOnEscalator | [source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1733"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePause.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePause | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1734"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePickUpBike | [source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h:34](../../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h:35](../../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1735"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePickUpEntity | [source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, float fMovePedUntilAnimProgress) | [source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimplePickUpEntity::Constructor-2 0x006917B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1736"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayHandSignalAnim | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h:13](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1737"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFire | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimplePlayerOnFire() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimplePlayerOnFire(const CTaskSimplePlayerOnFire&) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h:27](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1738"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & ActiveWeaponOf(CPlayerPed* player) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:154](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyMatrixWithVectorX87(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:160](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterDirection(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:822](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L822) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterProject(CMatrix* matrix, float angle, float magnitude, bool roundTrig) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:830](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L830) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1739"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFoot | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h#L19) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h:33](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h:36](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h:60](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h#L60) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1740"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePutDownEntity | [source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
