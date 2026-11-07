<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 06/11/2026 (Ciudad de México):** 848 grupos / 9272 funciones reconstruidas; 9272 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1101"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Bike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BikeSteerAngleTanExe(float degrees) | [source/game_sa/Entity/Vehicle/Bike.cpp:98](../../source/game_sa/Entity/Vehicle/Bike.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderSmooth355(float base,float step,float previous,float target) | [source/game_sa/Entity/Vehicle/Bike.cpp:533](../../source/game_sa/Entity/Vehicle/Bike.cpp#L533) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderWeights355(float lateral,float forward,float available,float* lateralOut,float* forwardOut) | [source/game_sa/Entity/Vehicle/Bike.cpp:536](../../source/game_sa/Entity/Vehicle/Bike.cpp#L536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderJitter355(float speed,int randomValue) | [source/game_sa/Entity/Vehicle/Bike.cpp:539](../../source/game_sa/Entity/Vehicle/Bike.cpp#L539) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderForwardLimit355(float lean,float target) | [source/game_sa/Entity/Vehicle/Bike.cpp:542](../../source/game_sa/Entity/Vehicle/Bike.cpp#L542) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeControlBlend355(float oldValue, int32 input, float step, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:690](../../source/game_sa/Entity/Vehicle/Bike.cpp#L690) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeControlProductAdd355(float oldValue, float input, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:694](../../source/game_sa/Entity/Vehicle/Bike.cpp#L694) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeInputSteer355(float degrees, float steer) | [source/game_sa/Entity/Vehicle/Bike.cpp:698](../../source/game_sa/Entity/Vehicle/Bike.cpp#L698) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeForwardSpeed355(const CVector* speed, const CVector* forward) | [source/game_sa/Entity/Vehicle/Bike.cpp:702](../../source/game_sa/Entity/Vehicle/Bike.cpp#L702) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMouseDecay355(float* value) | [source/game_sa/Entity/Vehicle/Bike.cpp:706](../../source/game_sa/Entity/Vehicle/Bike.cpp#L706) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeInputSpeedCap355(CVector* speed) | [source/game_sa/Entity/Vehicle/Bike.cpp:710](../../source/game_sa/Entity/Vehicle/Bike.cpp#L710) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeCopyColPointExe(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Vehicle/Bike.cpp:799](../../source/game_sa/Entity/Vehicle/Bike.cpp#L799) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMultiply3x3Exe(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Bike.cpp:810](../../source/game_sa/Entity/Vehicle/Bike.cpp#L810) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSqMagExe(const CVector& v) | [source/game_sa/Entity/Vehicle/Bike.cpp:956](../../source/game_sa/Entity/Vehicle/Bike.cpp#L956) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeDotZYXExe(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Bike.cpp:959](../../source/game_sa/Entity/Vehicle/Bike.cpp#L959) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMultiply3x3VMExe(const CVector& v, const CMatrix& m) | [source/game_sa/Entity/Vehicle/Bike.cpp:962](../../source/game_sa/Entity/Vehicle/Bike.cpp#L962) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeCrossExe(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Bike.cpp:971](../../source/game_sa/Entity/Vehicle/Bike.cpp#L971) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMoveHandBone355(RwMatrix* bone, const CVector& delta, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:1983](../../source/game_sa/Entity/Vehicle/Bike.cpp#L1983) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRoadOrientation355(CMatrix* matrix, float rear, float front, float backX, float by, float bz, float fx, float fy, float fz) | [source/game_sa/Entity/Vehicle/Bike.cpp:2038](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2038) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderChassisHeight355(float lean, float height) | [source/game_sa/Entity/Vehicle/Bike.cpp:2134](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderContact355(CVector& out, const CVector& point, const CVector& right, float lean, float height) | [source/game_sa/Entity/Vehicle/Bike.cpp:2137](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderTrig355(float angle, float multiplier, float& sineOut, float& cosineOut) | [source/game_sa/Entity/Vehicle/Bike.cpp:2140](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderSine355(float angle, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:2144](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderForkAngle355(float height, float original, float forkLength) | [source/game_sa/Entity/Vehicle/Bike.cpp:2147](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeFramePosition355(RwFrame* frame, RwFrame* stop) | [source/game_sa/Entity/Vehicle/Bike.cpp:2376](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2376) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSetupEnd355(CColLine* line, float upper, float lower, float wheel) | [source/game_sa/Entity/Vehicle/Bike.cpp:2385](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2385) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSetupHeight355(float force, float suspensionLength, float wheel, float startZ) | [source/game_sa/Entity/Vehicle/Bike.cpp:2389](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2389) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSetupSphere355(CColSphere* sphere,float limit) | [source/game_sa/Entity/Vehicle/Bike.cpp:2394](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2394) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1102"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Bmx.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BmxMax(float a, float b) | [source/game_sa/Entity/Vehicle/Bmx.cpp:41](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BmxMultiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Bmx.cpp:46](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BmxNeedsNewAnim(const CAnimBlendAssociation* a) | [source/game_sa/Entity/Vehicle/Bmx.cpp:55](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1103"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Boat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BoatMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Boat.cpp:63](../../source/game_sa/Entity/Vehicle/Boat.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1104"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Heli.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HeliExeRand() | [source/game_sa/Entity/Vehicle/Heli.cpp:56](../../source/game_sa/Entity/Vehicle/Heli.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & HeliField(void* object, size_t offset) | [source/game_sa/Entity/Vehicle/Heli.cpp:62](../../source/game_sa/Entity/Vehicle/Heli.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindSwatPositionRelativeToHeliExeAbi(CHeli* self, uint32 entryEdx, CVector* result, int32 swatNumber) | [source/game_sa/Entity/Vehicle/Heli.cpp:70](../../source/game_sa/Entity/Vehicle/Heli.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliIsFarFromPlayer(const CVector& player, const CVector& heli) | [source/game_sa/Entity/Vehicle/Heli.cpp:724](../../source/game_sa/Entity/Vehicle/Heli.cpp#L724) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliQuietNan(float value) | [source/game_sa/Entity/Vehicle/Heli.cpp:1142](../../source/game_sa/Entity/Vehicle/Heli.cpp#L1142) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1105"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Plane.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlaneGenerationHeading355(CVehicle* vehicle, float heading) | [source/game_sa/Entity/Vehicle/Plane.cpp:483](../../source/game_sa/Entity/Vehicle/Plane.cpp#L483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlane::DoPlaneGenerationAndRemoval 0x006CD2F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationVelocity355(float heading, bool helicopter) | [source/game_sa/Entity/Vehicle/Plane.cpp:497](../../source/game_sa/Entity/Vehicle/Plane.cpp#L497) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationSelect355(int32 aircraftCount) | [source/game_sa/Entity/Vehicle/Plane.cpp:502](../../source/game_sa/Entity/Vehicle/Plane.cpp#L502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GenPlane_LastTimeGenerated: source/game_sa/Entity/Vehicle/Plane.h:76 (0x00C1CAE0); Posible dato fijo GenPlane_ModelIndex: source/game_sa/Entity/Vehicle/Plane.h:74 (0x00C1CAD8); Posible dato fijo GenPlane_Status: source/game_sa/Entity/Vehicle/Plane.h:75 (0x00C1CADC) |
| PlaneGenerationEngine355(CVehicle* vehicle) | [source/game_sa/Entity/Vehicle/Plane.cpp:547](../../source/game_sa/Entity/Vehicle/Plane.cpp#L547) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneQuaternionToMatrix(const CQuaternion& quaternion, RwMatrix* matrix) | [source/game_sa/Entity/Vehicle/Plane.cpp:610](../../source/game_sa/Entity/Vehicle/Plane.cpp#L610) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CQuaternion::Get-matrix 0x0059C080 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneTransformPoint(const CMatrix* matrix, const CVector& point) | [source/game_sa/Entity/Vehicle/Plane.cpp:615](../../source/game_sa/Entity/Vehicle/Plane.cpp#L615) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1106"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/QuadBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| QuadMultiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:32](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:41](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMax(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:50](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMin(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:51](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1107"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Trailer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Trailer.cpp:42](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Vehicle/Trailer.cpp:51](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeadingX87(const CPlaceable* placeable) | [source/game_sa/Entity/Vehicle/Trailer.cpp:65](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SteerTowardsTractor(const CPlaceable* tractor, const CPlaceable* self) | [source/game_sa/Entity/Vehicle/Trailer.cpp:75](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1108"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Train.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NodeDistanceFromStart(const CTrainNode& node) | [source/game_sa/Entity/Vehicle/Train.cpp:908](../../source/game_sa/Entity/Vehicle/Train.cpp#L908) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1109"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Vehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehEFtol(float f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:82](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:85](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:94](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEInvVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:103](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEDot(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:112](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehECross(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:117](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEPow(float base, float exponent) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:126](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehESinCos(float angle, float& outSin, float& outCos) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:131](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise2D(CVector2D& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:137](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:149](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormaliseAndMag(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:162](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehTowTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1248](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehDMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1364](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1364) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2121](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Object 0x006D2690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2134](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Frame 0x006D26D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetCompAlphaCB(RpMaterial* material, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2203](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2203) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2577](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2577) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Object 0x006D33B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2603](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2603) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Frame 0x006D3420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetupUpgradeAtomicRendering(RpAtomic* atomic, bool setAlphaFlag) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2652](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2652) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FlcAt(CVehicle& v, size_t offset) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4220](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcParityEven(unsigned int value) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4223](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4223) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcApplyAxisTurn(FlcFrame& f, const float& d, const float* axis, const float* lever) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4250](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4250) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcClampVector(FlcX87& fp, float* vec, const float& thresholdSq, const float& limit) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4273](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4273) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcTail(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4299](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4299) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteAirship(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4365](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4365) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteWing(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4439](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4439) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteRotor(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:5424](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L5424) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| VehicleHeadLightEffect(CVehicle& vehicle, bool inlinedCopy, int32 lightId, CMatrix& vehicleMatrix, uint8 isRight, uint8 disabledOrAlarm) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8217](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| CVehNormalise2DInline(float x, float y) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8350](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8350) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1110"></a>

## Globales y helpers: source/game_sa/EntityScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScannerSector(float position, float range, bool useEnd) | [source/game_sa/EntityScanner.cpp:60](../../source/game_sa/EntityScanner.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1111"></a>

## Globales y helpers: source/game_sa/EntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) | [source/game_sa/EntryExit.cpp:36](../../source/game_sa/EntryExit.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitGetPositionRelativeToOutsideWorldRegs(CEntryExit* self, uint32 entryEdx, CVector* outPos) | [source/game_sa/EntryExit.cpp:44](../../source/game_sa/EntryExit.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTeleportPointValid(const CVector& origin, const CVector& target) | [source/game_sa/EntryExit.cpp:123](../../source/game_sa/EntryExit.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1112"></a>

## Globales y helpers: source/game_sa/EntryExitManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawArea(const eAreaCodesS8& area) | [source/game_sa/EntryExitManager.cpp:93](../../source/game_sa/EntryExitManager.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CurrArea() | [source/game_sa/EntryExitManager.cpp:97](../../source/game_sa/EntryExitManager.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1113"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1114"></a>

## Globales y helpers: source/game_sa/Enums/eGameState.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eGameState gs) | [source/game_sa/Enums/eGameState.h:16](../../source/game_sa/Enums/eGameState.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1115"></a>

## Globales y helpers: source/game_sa/Enums/eIKArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IKArmToIKSlot(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:11](../../source/game_sa/Enums/eIKArm.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IKArmToBoneTag(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:19](../../source/game_sa/Enums/eIKArm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1116"></a>

## Globales y helpers: source/game_sa/Enums/eModelID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelIDForCutScene(eModelID modelId) | [source/game_sa/Enums/eModelID.h:14868](../../source/game_sa/Enums/eModelID.h#L14868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1117"></a>

## Globales y helpers: source/game_sa/Enums/ePedType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGang(ePedType ptype) | [source/game_sa/Enums/ePedType.h:61](../../source/game_sa/Enums/ePedType.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypePlayer(ePedType pt) | [source/game_sa/Enums/ePedType.h:65](../../source/game_sa/Enums/ePedType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale(ePedType type) | [source/game_sa/Enums/ePedType.h:74](../../source/game_sa/Enums/ePedType.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAllGangPedTypes() | [source/game_sa/Enums/ePedType.h:83](../../source/game_sa/Enums/ePedType.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGangOfPedType(ePedType ptype) | [source/game_sa/Enums/ePedType.h:114](../../source/game_sa/Enums/ePedType.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1118"></a>

## Globales y helpers: source/game_sa/Enums/eRadioID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eRadioID v) | [source/game_sa/Enums/eRadioID.h:30](../../source/game_sa/Enums/eRadioID.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1119"></a>

## Globales y helpers: source/game_sa/Enums/eTaskType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eTaskType taskType) | [source/game_sa/Enums/eTaskType.h:578](../../source/game_sa/Enums/eTaskType.h#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1120"></a>

## Globales y helpers: source/game_sa/Enums/eWeaponType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsWeaponTypeProjectile(eWeaponType weaponType) | [source/game_sa/Enums/eWeaponType.h:82](../../source/game_sa/Enums/eWeaponType.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
