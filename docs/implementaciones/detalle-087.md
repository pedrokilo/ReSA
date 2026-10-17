<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1721"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHurtPedWithCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1722"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1723"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKChain | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(char* name, eBoneTag32 effectorBoneTag, RwV3d effectorVec, eBoneTag32 pivotBoneTag, CEntity* a6, eBoneTag32 offsetBoneTag, RwV3d offsetPos, float speed, int32 time, int32 blendTime) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h#L38) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKChain(UninitTag tag) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1724"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1725"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKLookAt | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h:11](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKLookAt::Destructor- 0x00633EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(char* name, CEntity* lookAtEntity, int32 time, eBoneTag32 pedBoneID, RwV3d lookAtOffset, uint8 useTorso, float fSpeed, int32 blendTime, int32 priority) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.h#L38) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1726"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKManager | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h:12](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleIKManager() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h:20](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetTaskAtSlot(eIKChainSlot slot) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h:35](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.h#L42) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1727"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1728"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKPointArm | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleIKPointArm::Destructor 0x00634240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(const char* purpose, eIKArm arm, CEntity* targetEntity, eBoneTag32 bone, RwV3d offsetPosn, float speed, int32 blendTime) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.h#L43) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1729"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleInAir | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h#L15) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1730"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleJetPack | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h#L19) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1731"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:112](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1732"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleJump | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1733"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShakePadIfPlayerVehicle(CVehicle* car) | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1734"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleKillPedWithCar | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:9](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:21](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:22](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1735"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleLand | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h:24](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1736"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleLeaveGroup | [source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1737"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunTimedAnim | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h:8](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInterruptable(CPed const* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h:19](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1738"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleNone | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:5](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLeaveGroup::Destructor 0x00463560 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleNone::GetTaskType 0x004635C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:16](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleNone::Clone 0x00463570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPed(CPed* ped) override | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h:17](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleNone::MakeAbortable 0x004635D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1739"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleOnEscalator | [source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h:10](../../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h:18](../../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1740"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePause.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePause | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.h:6](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.h:14](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTaskType() const override | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.h:15](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
