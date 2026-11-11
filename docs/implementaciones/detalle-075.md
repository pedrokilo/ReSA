<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 884 grupos / 10050 funciones reconstruidas; 10050 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1481"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitHeadDestructorBodyExe(CTaskSimpleHitHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:26 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:27 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:30 (0x004D1390) |

<a id="grupo-1482"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldEntityDestructorBody(CTaskSimpleHoldEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L55) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskSimpleHoldEntity() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1483"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1484"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKLookAtDestructorBody(CTaskSimpleIKLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKLookAt::Destructor- 0x00633EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1485"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKManagerDestructorBody(CTaskSimpleIKManager* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKManager::Destructor 0x00633830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleIKManager() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1486"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKPointArmDestructorBody(CTaskSimpleIKPointArm* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKPointArm::Destructor 0x00634240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKPointArm() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1487"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleInAirDestructorBodyExe(CTaskSimpleInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:26 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:27 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:30 (0x004D1390) |

<a id="grupo-1488"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleJetPackDestructorBody(CTaskSimpleJetPack* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp#L60) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleJetPack::Destructor 0x0067E600 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleJetPack() | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1489"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SimpleJumpDestructorBodyExe(CTaskSimpleJump* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:148](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1490"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShakePadIfPlayerVehicle(CVehicle* car) | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1491"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleLandDestructorBody(CTaskSimpleLand* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAnimBlendAssociation::SetFinishCallback 0x004CEBE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote148ABody_CTaskSimpleLand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1492"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LookAboutDestructorBody(CTaskSimpleLookAbout* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLookAbout::Destructor 0x0048EA90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1493"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SimpleNoneDestructorBody(CTaskSimpleNone* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLeaveGroup::Destructor 0x00463560 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleNoneDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1494"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PauseDestructorBody(CTaskSimplePause* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePause::Destructor 0x0048E8A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1495"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall HandSignalConstructorBody(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/, uint32 animId, uint32 blendBits, uint32 fatHands, uint32 holdLastFrame) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePlayHandSignalAnim::Constructor 0x0061AE50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1496"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFireDestructorBody(CTaskSimplePlayerOnFire* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimplePlayerOnFire::Destructor 0x006335B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimplePlayerOnFire() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1497"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFootDestructorBody(CTaskSimplePlayerOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L43) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimplePlayerOnFoot::Destructor 0x006857D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskSimplePlayerOnFoot() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveWeaponOf(CPlayerPed* player) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:174](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyMatrixWithVectorX87(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:180](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterDirection(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:842](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L842) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterProject(CMatrix* matrix, float angle, float magnitude, bool roundTrig) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:850](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L850) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1498"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunAnimDestructorBody(CTaskSimpleRunAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleRunAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1499"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunNamedAnimDestructorBody(CTaskSimpleRunNamedAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleRunNamedAnim::Destructor 0x0061BF10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleRunNamedAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp:50](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1500"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScratchHeadBaseDestructorCall(CTaskSimpleScratchHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScratchHeadDestructorBody(CTaskSimpleScratchHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleScratchHead::Destructor 0x0048E930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147AScratchHeadBaseDestructorCall() | [source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
