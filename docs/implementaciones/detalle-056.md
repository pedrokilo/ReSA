<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1101"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Bmx.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CBike | [source/game_sa/Entity/Vehicle/Bmx.h:27](../../source/game_sa/Entity/Vehicle/Bmx.h#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) | [source/game_sa/Entity/Vehicle/Bmx.h:62](../../source/game_sa/Entity/Vehicle/Bmx.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Entity/Vehicle/Bmx.h:63](../../source/game_sa/Entity/Vehicle/Bmx.h#L63) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1102"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Boat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BoatMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Boat.cpp:63](../../source/game_sa/Entity/Vehicle/Boat.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1103"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Boat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CVehicle | [source/game_sa/Entity/Vehicle/Boat.h:30](../../source/game_sa/Entity/Vehicle/Boat.h#L30) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroupId, uint32 animId, float currTime) override | [source/game_sa/Entity/Vehicle/Boat.h:107](../../source/game_sa/Entity/Vehicle/Boat.h#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1104"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Heli.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAutomobile | [source/game_sa/Entity/Vehicle/Heli.h:62](../../source/game_sa/Entity/Vehicle/Heli.h#L62) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetRopeId() | [source/game_sa/Entity/Vehicle/Heli.h:125](../../source/game_sa/Entity/Vehicle/Heli.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) | [source/game_sa/Entity/Vehicle/Heli.h:157](../../source/game_sa/Entity/Vehicle/Heli.h#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1105"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/MonsterTruck.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CMonsterTruck | [source/game_sa/Entity/Vehicle/MonsterTruck.h:39](../../source/game_sa/Entity/Vehicle/MonsterTruck.h#L39) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) | [source/game_sa/Entity/Vehicle/MonsterTruck.h:67](../../source/game_sa/Entity/Vehicle/MonsterTruck.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Entity/Vehicle/MonsterTruck.h:68](../../source/game_sa/Entity/Vehicle/MonsterTruck.h#L68) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1106"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Plane.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlaneGenerationHeading355(CVehicle* vehicle, float heading) | [source/game_sa/Entity/Vehicle/Plane.cpp:483](../../source/game_sa/Entity/Vehicle/Plane.cpp#L483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlane::DoPlaneGenerationAndRemoval 0x006CD2F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationVelocity355(float heading, bool helicopter) | [source/game_sa/Entity/Vehicle/Plane.cpp:497](../../source/game_sa/Entity/Vehicle/Plane.cpp#L497) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationSelect355(int32 aircraftCount) | [source/game_sa/Entity/Vehicle/Plane.cpp:502](../../source/game_sa/Entity/Vehicle/Plane.cpp#L502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationEngine355(CVehicle* vehicle) | [source/game_sa/Entity/Vehicle/Plane.cpp:547](../../source/game_sa/Entity/Vehicle/Plane.cpp#L547) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneQuaternionToMatrix(const CQuaternion& quaternion, RwMatrix* matrix) | [source/game_sa/Entity/Vehicle/Plane.cpp:611](../../source/game_sa/Entity/Vehicle/Plane.cpp#L611) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneTransformPoint(const CMatrix* matrix, const CVector& point) | [source/game_sa/Entity/Vehicle/Plane.cpp:616](../../source/game_sa/Entity/Vehicle/Plane.cpp#L616) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1107"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Plane.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAutomobile | [source/game_sa/Entity/Vehicle/Plane.h:40](../../source/game_sa/Entity/Vehicle/Plane.h#L40) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) | [source/game_sa/Entity/Vehicle/Plane.h:121](../../source/game_sa/Entity/Vehicle/Plane.h#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destroy() | [source/game_sa/Entity/Vehicle/Plane.h:122](../../source/game_sa/Entity/Vehicle/Plane.h#L122) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1108"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/QuadBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| QuadMultiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:32](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:41](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMax(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:50](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMin(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:51](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1109"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/QuadBike.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAutomobile | [source/game_sa/Entity/Vehicle/QuadBike.h:37](../../source/game_sa/Entity/Vehicle/QuadBike.h#L37) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1110"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Trailer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Trailer.cpp:42](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Vehicle/Trailer.cpp:51](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeadingX87(const CPlaceable* placeable) | [source/game_sa/Entity/Vehicle/Trailer.cpp:65](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SteerTowardsTractor(const CPlaceable* tractor, const CPlaceable* self) | [source/game_sa/Entity/Vehicle/Trailer.cpp:75](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1111"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Trailer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAutomobile | [source/game_sa/Entity/Vehicle/Trailer.h:41](../../source/game_sa/Entity/Vehicle/Trailer.h#L41) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) | [source/game_sa/Entity/Vehicle/Trailer.h:89](../../source/game_sa/Entity/Vehicle/Trailer.h#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1112"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Train.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TrainHitStuff(PtrListType& ptrList, CEntity* entity) | [source/game_sa/Entity/Vehicle/Train.cpp:436](../../source/game_sa/Entity/Vehicle/Train.cpp#L436) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NodeDistanceFromStart(const CTrainNode& node) | [source/game_sa/Entity/Vehicle/Train.cpp:908](../../source/game_sa/Entity/Vehicle/Train.cpp#L908) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1113"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Train.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CVehicle | [source/game_sa/Entity/Vehicle/Train.h:43](../../source/game_sa/Entity/Vehicle/Train.h#L43) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) | [source/game_sa/Entity/Vehicle/Train.h:137](../../source/game_sa/Entity/Vehicle/Train.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1114"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Vehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehEFtol(float f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:79](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:82](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:91](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEInvVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:100](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEDot(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:109](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehECross(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:114](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEPow(float base, float exponent) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:123](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehESinCos(float angle, float& outSin, float& outCos) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:128](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise2D(CVector2D& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:134](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:146](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormaliseAndMag(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:159](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L159) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehTowTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1244](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1244) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehDMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1360](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1360) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2117](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Object 0x006D2690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2130](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Frame 0x006D26D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetCompAlphaCB(RpMaterial* material, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2199](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2199) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveUpgradeCB(RpAtomic* atomic, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2533](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2533) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindUpgradeCB(RpAtomic* atomic, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2538](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2538) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2543](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2543) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Object 0x006D33B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2569](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2569) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Frame 0x006D3420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CopyObjectsCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2577](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2577) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CopyObjectsCB_TargetClump: source/game_sa/Entity/Vehicle/Vehicle.cpp:2576 (0x00C1CB58) |
| * FindReplacementUpgradeCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2591](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2591) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetupUpgradeAtomicRendering(RpAtomic* atomic, bool setAlphaFlag) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2608](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2608) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehNormalise2DInline(float x, float y) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4896](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4896) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValidModForVehicle(uint32 modelId, CVehicle* vehicle) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:5147](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L5147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1115"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Vehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPhysical | [source/game_sa/Entity/Vehicle/Vehicle.h:188](../../source/game_sa/Entity/Vehicle/Vehicle.h#L188) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlagToDestroyWhenNextProcessed() override | [source/game_sa/Entity/Vehicle/Vehicle.h:457](../../source/game_sa/Entity/Vehicle/Vehicle.h#L457) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessControlCollisionCheck(bool applySpeed) | [source/game_sa/Entity/Vehicle/Vehicle.h:459](../../source/game_sa/Entity/Vehicle/Vehicle.h#L459) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessControlInputs(uint8 playerNum) | [source/game_sa/Entity/Vehicle/Vehicle.h:460](../../source/game_sa/Entity/Vehicle/Vehicle.h#L460) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetComponentWorldPosition(int32 componentId, CVector& outPos) | [source/game_sa/Entity/Vehicle/Vehicle.h:462](../../source/game_sa/Entity/Vehicle/Vehicle.h#L462) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsComponentPresent(int32 componentId) const | [source/game_sa/Entity/Vehicle/Vehicle.h:464](../../source/game_sa/Entity/Vehicle/Vehicle.h#L464) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| OpenDoor(CPed* ped, int32 componentId, eDoors door, float doorOpenRatio, bool playSound) | [source/game_sa/Entity/Vehicle/Vehicle.h:465](../../source/game_sa/Entity/Vehicle/Vehicle.h#L465) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDooorAngleOpenRatioU32(uint32 door) | [source/game_sa/Entity/Vehicle/Vehicle.h:471](../../source/game_sa/Entity/Vehicle/Vehicle.h#L471) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDooorAngleOpenRatio(eDoors door) | [source/game_sa/Entity/Vehicle/Vehicle.h:472](../../source/game_sa/Entity/Vehicle/Vehicle.h#L472) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorReadyU32(uint32 door) | [source/game_sa/Entity/Vehicle/Vehicle.h:473](../../source/game_sa/Entity/Vehicle/Vehicle.h#L473) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorReady(eDoors door) | [source/game_sa/Entity/Vehicle/Vehicle.h:474](../../source/game_sa/Entity/Vehicle/Vehicle.h#L474) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorFullyOpenU32(uint32 door) | [source/game_sa/Entity/Vehicle/Vehicle.h:475](../../source/game_sa/Entity/Vehicle/Vehicle.h#L475) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorFullyOpen(eDoors door) | [source/game_sa/Entity/Vehicle/Vehicle.h:476](../../source/game_sa/Entity/Vehicle/Vehicle.h#L476) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorClosedU32(uint32 door) | [source/game_sa/Entity/Vehicle/Vehicle.h:477](../../source/game_sa/Entity/Vehicle/Vehicle.h#L477) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorClosed(eDoors door) | [source/game_sa/Entity/Vehicle/Vehicle.h:478](../../source/game_sa/Entity/Vehicle/Vehicle.h#L478) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorMissingU32(uint32 door) | [source/game_sa/Entity/Vehicle/Vehicle.h:479](../../source/game_sa/Entity/Vehicle/Vehicle.h#L479) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsDoorMissing(eDoors door) | [source/game_sa/Entity/Vehicle/Vehicle.h:480](../../source/game_sa/Entity/Vehicle/Vehicle.h#L480) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsOpenTopCar() | [source/game_sa/Entity/Vehicle/Vehicle.h:483](../../source/game_sa/Entity/Vehicle/Vehicle.h#L483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveRefsToVehicle(CEntity* entity) | [source/game_sa/Entity/Vehicle/Vehicle.h:485](../../source/game_sa/Entity/Vehicle/Vehicle.h#L485) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BlowUpCar(CEntity* damager, bool bHideExplosion) | [source/game_sa/Entity/Vehicle/Vehicle.h:486](../../source/game_sa/Entity/Vehicle/Vehicle.h#L486) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BlowUpCarCutSceneNoExtras(bool bNoCamShake, bool bNoSpawnFlyingComps, bool bDetachWheels, bool bExplosionSound) | [source/game_sa/Entity/Vehicle/Vehicle.h:487](../../source/game_sa/Entity/Vehicle/Vehicle.h#L487) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetUpWheelColModel(CColModel* wheelCol) | [source/game_sa/Entity/Vehicle/Vehicle.h:488](../../source/game_sa/Entity/Vehicle/Vehicle.h#L488) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) | [source/game_sa/Entity/Vehicle/Vehicle.h:490](../../source/game_sa/Entity/Vehicle/Vehicle.h#L490) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRoomForPedToLeaveCar(uint32 doorId, CVector* arg1) | [source/game_sa/Entity/Vehicle/Vehicle.h:491](../../source/game_sa/Entity/Vehicle/Vehicle.h#L491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetRideAnimData() | [source/game_sa/Entity/Vehicle/Vehicle.h:494](../../source/game_sa/Entity/Vehicle/Vehicle.h#L494) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetupSuspensionLines() | [source/game_sa/Entity/Vehicle/Vehicle.h:495](../../source/game_sa/Entity/Vehicle/Vehicle.h#L495) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddMovingCollisionSpeed(CVector& point) | [source/game_sa/Entity/Vehicle/Vehicle.h:496](../../source/game_sa/Entity/Vehicle/Vehicle.h#L496) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fix() | [source/game_sa/Entity/Vehicle/Vehicle.h:497](../../source/game_sa/Entity/Vehicle/Vehicle.h#L497) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetupDamageAfterLoad() | [source/game_sa/Entity/Vehicle/Vehicle.h:498](../../source/game_sa/Entity/Vehicle/Vehicle.h#L498) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoBurstAndSoftGroundRatios() | [source/game_sa/Entity/Vehicle/Vehicle.h:499](../../source/game_sa/Entity/Vehicle/Vehicle.h#L499) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayCarHorn() | [source/game_sa/Entity/Vehicle/Vehicle.h:501](../../source/game_sa/Entity/Vehicle/Vehicle.h#L501) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNumContactWheels() | [source/game_sa/Entity/Vehicle/Vehicle.h:502](../../source/game_sa/Entity/Vehicle/Vehicle.h#L502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehicleDamage(float damageIntensity, eVehicleCollisionComponent collisionComponent, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) | [source/game_sa/Entity/Vehicle/Vehicle.h:503](../../source/game_sa/Entity/Vehicle/Vehicle.h#L503) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTowLink(CVehicle* tractor, bool setMyPosToTowBar) | [source/game_sa/Entity/Vehicle/Vehicle.h:508](../../source/game_sa/Entity/Vehicle/Vehicle.h#L508) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BreakTowLink() | [source/game_sa/Entity/Vehicle/Vehicle.h:509](../../source/game_sa/Entity/Vehicle/Vehicle.h#L509) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindWheelWidth(bool bRear) | [source/game_sa/Entity/Vehicle/Vehicle.h:510](../../source/game_sa/Entity/Vehicle/Vehicle.h#L510) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetDriver() const | [source/game_sa/Entity/Vehicle/Vehicle.h:534](../../source/game_sa/Entity/Vehicle/Vehicle.h#L534) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHealth() const noexcept | [source/game_sa/Entity/Vehicle/Vehicle.h:675](../../source/game_sa/Entity/Vehicle/Vehicle.h#L675) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsAutomobile() const | [source/game_sa/Entity/Vehicle/Vehicle.h:683](../../source/game_sa/Entity/Vehicle/Vehicle.h#L683) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsMonsterTruck() const | [source/game_sa/Entity/Vehicle/Vehicle.h:684](../../source/game_sa/Entity/Vehicle/Vehicle.h#L684) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsQuad() const | [source/game_sa/Entity/Vehicle/Vehicle.h:685](../../source/game_sa/Entity/Vehicle/Vehicle.h#L685) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsHeli() const | [source/game_sa/Entity/Vehicle/Vehicle.h:686](../../source/game_sa/Entity/Vehicle/Vehicle.h#L686) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPlane() const | [source/game_sa/Entity/Vehicle/Vehicle.h:687](../../source/game_sa/Entity/Vehicle/Vehicle.h#L687) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBoat() const | [source/game_sa/Entity/Vehicle/Vehicle.h:688](../../source/game_sa/Entity/Vehicle/Vehicle.h#L688) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTrain() const | [source/game_sa/Entity/Vehicle/Vehicle.h:689](../../source/game_sa/Entity/Vehicle/Vehicle.h#L689) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFakeAircraft() const | [source/game_sa/Entity/Vehicle/Vehicle.h:690](../../source/game_sa/Entity/Vehicle/Vehicle.h#L690) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBike() const | [source/game_sa/Entity/Vehicle/Vehicle.h:691](../../source/game_sa/Entity/Vehicle/Vehicle.h#L691) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBMX() const | [source/game_sa/Entity/Vehicle/Vehicle.h:692](../../source/game_sa/Entity/Vehicle/Vehicle.h#L692) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTrailer() const | [source/game_sa/Entity/Vehicle/Vehicle.h:693](../../source/game_sa/Entity/Vehicle/Vehicle.h#L693) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubVehicleTypeValid() const | [source/game_sa/Entity/Vehicle/Vehicle.h:697](../../source/game_sa/Entity/Vehicle/Vehicle.h#L697) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubAutomobile() const | [source/game_sa/Entity/Vehicle/Vehicle.h:698](../../source/game_sa/Entity/Vehicle/Vehicle.h#L698) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubMonsterTruck() const | [source/game_sa/Entity/Vehicle/Vehicle.h:699](../../source/game_sa/Entity/Vehicle/Vehicle.h#L699) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubQuad() const | [source/game_sa/Entity/Vehicle/Vehicle.h:700](../../source/game_sa/Entity/Vehicle/Vehicle.h#L700) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubHeli() const | [source/game_sa/Entity/Vehicle/Vehicle.h:701](../../source/game_sa/Entity/Vehicle/Vehicle.h#L701) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubPlane() const | [source/game_sa/Entity/Vehicle/Vehicle.h:702](../../source/game_sa/Entity/Vehicle/Vehicle.h#L702) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubBoat() const | [source/game_sa/Entity/Vehicle/Vehicle.h:703](../../source/game_sa/Entity/Vehicle/Vehicle.h#L703) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubTrain() const | [source/game_sa/Entity/Vehicle/Vehicle.h:704](../../source/game_sa/Entity/Vehicle/Vehicle.h#L704) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubFakeAircraft() const | [source/game_sa/Entity/Vehicle/Vehicle.h:705](../../source/game_sa/Entity/Vehicle/Vehicle.h#L705) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubBike() const | [source/game_sa/Entity/Vehicle/Vehicle.h:706](../../source/game_sa/Entity/Vehicle/Vehicle.h#L706) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubBMX() const | [source/game_sa/Entity/Vehicle/Vehicle.h:707](../../source/game_sa/Entity/Vehicle/Vehicle.h#L707) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubTrailer() const | [source/game_sa/Entity/Vehicle/Vehicle.h:708](../../source/game_sa/Entity/Vehicle/Vehicle.h#L708) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubRoadVehicle() const | [source/game_sa/Entity/Vehicle/Vehicle.h:710](../../source/game_sa/Entity/Vehicle/Vehicle.h#L710) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSubFlyingVehicle() const | [source/game_sa/Entity/Vehicle/Vehicle.h:711](../../source/game_sa/Entity/Vehicle/Vehicle.h#L711) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTransportVehicle() const | [source/game_sa/Entity/Vehicle/Vehicle.h:714](../../source/game_sa/Entity/Vehicle/Vehicle.h#L714) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsAmphibiousHeli() const | [source/game_sa/Entity/Vehicle/Vehicle.h:715](../../source/game_sa/Entity/Vehicle/Vehicle.h#L715) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsConstructionVehicle() const | [source/game_sa/Entity/Vehicle/Vehicle.h:716](../../source/game_sa/Entity/Vehicle/Vehicle.h#L716) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRealBike() const | [source/game_sa/Entity/Vehicle/Vehicle.h:718](../../source/game_sa/Entity/Vehicle/Vehicle.h#L718) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRealHeli() const | [source/game_sa/Entity/Vehicle/Vehicle.h:719](../../source/game_sa/Entity/Vehicle/Vehicle.h#L719) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRealPlane()const | [source/game_sa/Entity/Vehicle/Vehicle.h:720](../../source/game_sa/Entity/Vehicle/Vehicle.h#L720) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRealBoat() const | [source/game_sa/Entity/Vehicle/Vehicle.h:721](../../source/game_sa/Entity/Vehicle/Vehicle.h#L721) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCreatedBy() const | [source/game_sa/Entity/Vehicle/Vehicle.h:723](../../source/game_sa/Entity/Vehicle/Vehicle.h#L723) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCreatedBy(eVehicleCreatedBy v) const | [source/game_sa/Entity/Vehicle/Vehicle.h:724](../../source/game_sa/Entity/Vehicle/Vehicle.h#L724) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsMissionVehicle() const | [source/game_sa/Entity/Vehicle/Vehicle.h:725](../../source/game_sa/Entity/Vehicle/Vehicle.h#L725) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanUpdateHornCounter() | [source/game_sa/Entity/Vehicle/Vehicle.h:728](../../source/game_sa/Entity/Vehicle/Vehicle.h#L728) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsPlane() | [source/game_sa/Entity/Vehicle/Vehicle.h:730](../../source/game_sa/Entity/Vehicle/Vehicle.h#L730) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsHeli() | [source/game_sa/Entity/Vehicle/Vehicle.h:731](../../source/game_sa/Entity/Vehicle/Vehicle.h#L731) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasDriver() const | [source/game_sa/Entity/Vehicle/Vehicle.h:752](../../source/game_sa/Entity/Vehicle/Vehicle.h#L752) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasPassengerAtSeat(int32 seat) const | [source/game_sa/Entity/Vehicle/Vehicle.h:753](../../source/game_sa/Entity/Vehicle/Vehicle.h#L753) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPassengers() const | [source/game_sa/Entity/Vehicle/Vehicle.h:754](../../source/game_sa/Entity/Vehicle/Vehicle.h#L754) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetMaxPassengerSeats() | [source/game_sa/Entity/Vehicle/Vehicle.h:755](../../source/game_sa/Entity/Vehicle/Vehicle.h#L755) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDefaultAirResistance() const | [source/game_sa/Entity/Vehicle/Vehicle.h:757](../../source/game_sa/Entity/Vehicle/Vehicle.h#L757) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTotallyUpsideDown() const | [source/game_sa/Entity/Vehicle/Vehicle.h:769](../../source/game_sa/Entity/Vehicle/Vehicle.h#L769) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasSpaceForAPassenger() const | [source/game_sa/Entity/Vehicle/Vehicle.h:772](../../source/game_sa/Entity/Vehicle/Vehicle.h#L772) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Constructor(eVehicleCreatedBy createdBy) | [source/game_sa/Entity/Vehicle/Vehicle.h:778](../../source/game_sa/Entity/Vehicle/Vehicle.h#L778) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Destructor() | [source/game_sa/Entity/Vehicle/Vehicle.h:783](../../source/game_sa/Entity/Vehicle/Vehicle.h#L783) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1116"></a>

## Globales y helpers: source/game_sa/EntityScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScannerSector(float position, float range, bool useEnd) | [source/game_sa/EntityScanner.cpp:60](../../source/game_sa/EntityScanner.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1117"></a>

## Globales y helpers: source/game_sa/EntityScanner.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntityScanner | [source/game_sa/EntityScanner.h:22](../../source/game_sa/EntityScanner.h#L22) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetTimer() | [source/game_sa/EntityScanner.h:42](../../source/game_sa/EntityScanner.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetClosestEntity() const | [source/game_sa/EntityScanner.h:48](../../source/game_sa/EntityScanner.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/EntityScanner.h:58](../../source/game_sa/EntityScanner.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/EntityScanner.h:63](../../source/game_sa/EntityScanner.h#L63) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1118"></a>

## Globales y helpers: source/game_sa/EntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) | [source/game_sa/EntryExit.cpp:36](../../source/game_sa/EntryExit.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitGetPositionRelativeToOutsideWorldRegs(CEntryExit* self, uint32 entryEdx, CVector* outPos) | [source/game_sa/EntryExit.cpp:44](../../source/game_sa/EntryExit.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTeleportPointValid(const CVector& origin, const CVector& target) | [source/game_sa/EntryExit.cpp:123](../../source/game_sa/EntryExit.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1119"></a>

## Globales y helpers: source/game_sa/EntryExitManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawArea(const eAreaCodesS8& area) | [source/game_sa/EntryExitManager.cpp:92](../../source/game_sa/EntryExitManager.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CurrArea() | [source/game_sa/EntryExitManager.cpp:96](../../source/game_sa/EntryExitManager.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1120"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
