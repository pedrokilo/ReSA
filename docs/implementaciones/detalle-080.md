<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1581"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HandsUpDestructorBody(CTaskSimpleHandsUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleHandsUp::Destructor 0x0048EA60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskSimpleHandsUp() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1582"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitHeadDestructorBodyExe(CTaskSimpleHitHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:34 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:33 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:26 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:31 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:30 (0x004D1390) |

<a id="grupo-1583"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:61](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:70](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskSimpleHoldEntity() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:91](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1584"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall IKChainDefaultConstructorBody(CTaskSimpleIKChain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleIKChain::ConstructorDefaultBody 0x0062EC10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1585"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKLookAtDestructorBody(CTaskSimpleIKLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKLookAt::Destructor- 0x00633EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1586"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKManagerDestructorBody(CTaskSimpleIKManager* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKManager::Destructor 0x00633830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleIKManager() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1587"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKPointArmDestructorBody(CTaskSimpleIKPointArm* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKPointArm::Destructor 0x00634240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKPointArm() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1588"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleInAirDestructorBodyExe(CTaskSimpleInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:34 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:33 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:26 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:31 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:30 (0x004D1390) |

<a id="grupo-1589"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleJetPackDestructorBody(CTaskSimpleJetPack* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp#L60) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleJetPack::Destructor 0x0067E600 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleJetPack() | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1590"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SimpleJumpDestructorBodyExe(CTaskSimpleJump* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:150](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1591"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleLandDestructorBody(CTaskSimpleLand* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAnimBlendAssociation::SetFinishCallback 0x004CEBE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote148ABody_CTaskSimpleLand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1592"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LookAboutDestructorBody(CTaskSimpleLookAbout* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLookAbout::Destructor 0x0048EA90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1593"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SimpleNoneDestructorBody(CTaskSimpleNone* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLeaveGroup::Destructor 0x00463560 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleNoneDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1594"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OnEscalatorDestructorBody(CTaskSimpleOnEscalator* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleOnEscalator::DestructorBody 0x004B8750 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote171ABody_CTaskSimpleOnEscalator() | [source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1595"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PauseDestructorBody(CTaskSimplePause* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePause::Destructor 0x0048E8A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1596"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall HandSignalConstructorBody(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/, uint32 animId, uint32 blendBits, uint32 fatHands, uint32 holdLastFrame) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePlayHandSignalAnim::Constructor 0x0061AE50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1597"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFireDestructorBody(CTaskSimplePlayerOnFire* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimplePlayerOnFire::Destructor 0x006335B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimplePlayerOnFire() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1598"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimplePlayerOnFootDestructorBody(CTaskSimplePlayerOnFoot* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L43) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimplePlayerOnFoot::Destructor 0x006857D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote160ABody_CTaskSimplePlayerOnFoot() | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveWeaponOf(CPlayerPed* player) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:174](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyMatrixWithVectorX87(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:180](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterDirection(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:842](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L842) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterProject(CMatrix* matrix, float angle, float magnitude, bool roundTrig) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:850](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L850) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1599"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleRunAnimDestructorBody(CTaskSimpleRunAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleRunAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1600"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LoopedMiddleDestructorBody(CTaskSimple* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.cpp:135](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleRunAnimLoopedMiddle::DestructorBody 0x0061BCA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
