# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1201"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Bmx.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BmxMax(float a, float b) | [source/game_sa/Entity/Vehicle/Bmx.cpp:41](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BmxMultiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Bmx.cpp:46](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BmxNeedsNewAnim(const CAnimBlendAssociation* a) | [source/game_sa/Entity/Vehicle/Bmx.cpp:55](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CBmxBaseDestructorCall(CBmx* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Bmx.cpp:64](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L64) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CBmxDestructorBodyExe(CBmx* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Bmx.cpp:66](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBmx::DestructorBody 0x006BF9D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote152ABody_CBmxBaseDestructor() | [source/game_sa/Entity/Vehicle/Bmx.cpp:85](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1202"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Boat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BoatMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Boat.cpp:64](../../source/game_sa/Entity/Vehicle/Boat.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1203"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Heli.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HeliExeRand() | [source/game_sa/Entity/Vehicle/Heli.cpp:59](../../source/game_sa/Entity/Vehicle/Heli.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & HeliField(void* object, size_t offset) | [source/game_sa/Entity/Vehicle/Heli.cpp:65](../../source/game_sa/Entity/Vehicle/Heli.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindSwatPositionRelativeToHeliExeAbi(CHeli* self, uint32 entryEdx, CVector* result, int32 swatNumber) | [source/game_sa/Entity/Vehicle/Heli.cpp:74](../../source/game_sa/Entity/Vehicle/Heli.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CHeli::FindSwatPositionRelativeToHeli 0x006C4760 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliReleaseGunflashFx(CHeli* heli) | [source/game_sa/Entity/Vehicle/Heli.cpp:84](../../source/game_sa/Entity/Vehicle/Heli.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| CHeliDestructorBodyExe(CHeli* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Heli.cpp:100](../../source/game_sa/Entity/Vehicle/Heli.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliIsFarFromPlayer(const CVector& player, const CVector& heli) | [source/game_sa/Entity/Vehicle/Heli.cpp:760](../../source/game_sa/Entity/Vehicle/Heli.cpp#L760) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliQuietNan(float value) | [source/game_sa/Entity/Vehicle/Heli.cpp:1178](../../source/game_sa/Entity/Vehicle/Heli.cpp#L1178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1204"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/MonsterTruck.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote125AMonsterTruckDestructorBody() | [source/game_sa/Entity/Vehicle/MonsterTruck.cpp:339](../../source/game_sa/Entity/Vehicle/MonsterTruck.cpp#L339) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1205"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Plane.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlaneHeadingMatrixExe(CVehicle* self, void* /*edx*/, float heading) | [source/game_sa/Entity/Vehicle/Plane.cpp:50](../../source/game_sa/Entity/Vehicle/Plane.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlane::SetHeadingMatrix 0x006C4130 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneDestructorBodyExe(CPlane* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Plane.cpp:61](../../source/game_sa/Entity/Vehicle/Plane.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| PlaneGenerationHeading355(CVehicle* vehicle, float heading) | [source/game_sa/Entity/Vehicle/Plane.cpp:536](../../source/game_sa/Entity/Vehicle/Plane.cpp#L536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlane::DoPlaneGenerationAndRemoval 0x006CD2F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationVelocity355(float heading, bool helicopter) | [source/game_sa/Entity/Vehicle/Plane.cpp:550](../../source/game_sa/Entity/Vehicle/Plane.cpp#L550) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationSelect355(int32 aircraftCount) | [source/game_sa/Entity/Vehicle/Plane.cpp:555](../../source/game_sa/Entity/Vehicle/Plane.cpp#L555) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GenPlane_LastTimeGenerated: source/game_sa/Entity/Vehicle/Plane.h:76 (0x00C1CAE0); Posible dato fijo GenPlane_ModelIndex: source/game_sa/Entity/Vehicle/Plane.h:74 (0x00C1CAD8); Posible dato fijo GenPlane_Status: source/game_sa/Entity/Vehicle/Plane.h:75 (0x00C1CADC) |
| PlaneGenerationEngine355(CVehicle* vehicle) | [source/game_sa/Entity/Vehicle/Plane.cpp:600](../../source/game_sa/Entity/Vehicle/Plane.cpp#L600) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneQuaternionToMatrix(const CQuaternion& quaternion, RwMatrix* matrix) | [source/game_sa/Entity/Vehicle/Plane.cpp:664](../../source/game_sa/Entity/Vehicle/Plane.cpp#L664) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CQuaternion::Get-matrix 0x0059C080 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneTransformPoint(const CMatrix* matrix, const CVector& point) | [source/game_sa/Entity/Vehicle/Plane.cpp:669](../../source/game_sa/Entity/Vehicle/Plane.cpp#L669) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1206"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/QuadBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| QuadMultiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:32](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:41](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMax(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:50](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMin(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:51](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote141AQuadBikeDestructorBody() | [source/game_sa/Entity/Vehicle/QuadBike.cpp:66](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1207"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Trailer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Trailer.cpp:42](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Vehicle/Trailer.cpp:51](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeadingX87(const CPlaceable* placeable) | [source/game_sa/Entity/Vehicle/Trailer.cpp:65](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SteerTowardsTractor(const CPlaceable* tractor, const CPlaceable* self) | [source/game_sa/Entity/Vehicle/Trailer.cpp:75](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrailerDestructorBody(CTrailer* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Trailer.cpp:87](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTrailer::Destructor 0x006CED10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote140ATrailerDestructorBody() | [source/game_sa/Entity/Vehicle/Trailer.cpp:104](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1208"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Train.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTrainBaseDestructorCall(CTrain* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Train.cpp:42](../../source/game_sa/Entity/Vehicle/Train.cpp#L42) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CVehicle::Destructor 0x006E2B40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrainSetRotateAddPos(CMatrix& mat, const CVector& rot, const CVector& saved) | [source/game_sa/Entity/Vehicle/Train.cpp:66](../../source/game_sa/Entity/Vehicle/Train.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuietThroughX87(float value) | [source/game_sa/Entity/Vehicle/Train.cpp:75](../../source/game_sa/Entity/Vehicle/Train.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote174ABody_CTrainBaseDestructor() | [source/game_sa/Entity/Vehicle/Train.cpp:84](../../source/game_sa/Entity/Vehicle/Train.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NodeDistanceFromStart(const CTrainNode& node) | [source/game_sa/Entity/Vehicle/Train.cpp:1087](../../source/game_sa/Entity/Vehicle/Train.cpp#L1087) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1209"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Vehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehEFtol(float f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:88](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:91](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:100](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEInvVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:109](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEDot(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:118](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehECross(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:123](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEPow(float base, float exponent) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:132](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehESinCos(float angle, float& outSin, float& outCos) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:137](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise2D(CVector2D& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:143](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:155](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L155) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormaliseAndMag(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:168](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubProcessControlCollisionCheck(CVehicle* /*self*/, void* /*edx*/, bool /*applySpeed*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:202](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupSuspensionLines 0x006D6380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubProcessControlInputs(CVehicle* /*self*/, void* /*edx*/, uint8 /*playerNum*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:203](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L203) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::ProcessControlCollisionCheck 0x006D6270 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubGetComponentWorldPosition(CVehicle* /*self*/, void* /*edx*/, int32 /*componentId*/, CVector& /*outPos*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:204](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::ProcessControlInputs 0x006D6280 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsComponentPresent(CVehicle* /*self*/, void* /*edx*/, int32 /*componentId*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:205](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::GetComponentWorldPosition 0x006D6290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorReady(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:206](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsComponentPresent 0x006D62A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorFullyOpen(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:207](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorReady 0x006D62B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorClosed(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:208](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L208) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorFullyOpen 0x006D62C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorMissing(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:209](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L209) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorClosed 0x006D62D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorReadyU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:210](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L210) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorMissing 0x006D62E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorFullyOpenU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:211](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorReadyU32 0x006D62F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorClosedU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:212](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorFullyOpenU32 0x006D6300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorMissingU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:213](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorClosedU32 0x006D6310 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubRemoveRefsToVehicle(CVehicle* /*self*/, void* /*edx*/, CEntity* /*entity*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:214](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorMissingU32 0x006D6320 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubBlowUpCar(CVehicle* /*self*/, void* /*edx*/, CEntity* /*damager*/, bool /*bHideExplosion*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:215](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::RemoveRefsToVehicle 0x006D6330 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetUpWheelColModel(CVehicle* /*self*/, void* /*edx*/, CColModel* /*wheelCol*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:216](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::BlowUpCar 0x006D6340 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubBurstTyre(CVehicle* /*self*/, void* /*edx*/, uint8 /*tyreComponentId*/, bool /*bPhysicalEffect*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:217](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetUpWheelColModel 0x006D6350 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsRoomForPedToLeaveCar(CVehicle* /*self*/, void* /*edx*/, uint32 /*doorId*/, CVector* /*arg1*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:218](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::BurstTyre 0x006D6360 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetupSuspensionLines(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:219](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsRoomForPedToLeaveCar 0x006D6370 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubFix(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:220](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupSuspensionLines 0x006D6380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetupDamageAfterLoad(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:221](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::Fix 0x006D6390 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubDoBurstAndSoftGroundRatios(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:222](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L222) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupDamageAfterLoad 0x006D63A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubPlayCarHorn(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:223](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L223) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::DoBurstAndSoftGroundRatios 0x006D63B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubGetDooorAngleOpenRatio(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:224](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L224) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::PlayCarHorn 0x006D63C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubFindWheelWidth(CVehicle* /*self*/, void* /*edx*/, bool /*bRear*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:225](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::GetDooorAngleOpenRatio 0x006D56B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehIsHandbrakeOnExe(const CVehicle* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:241](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L241) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsHandbrakeOn 0x004F4CA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehIsAlarmActiveExe(const CVehicle* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:244](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L244) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsAlarmActive 0x004F4CE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehTowTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1375](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1375) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehDMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1491](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2251](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2251) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Object 0x006D2690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2264](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2264) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Frame 0x006D26D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2710](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2710) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Object 0x006D33B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2736](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2736) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Frame 0x006D3420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetupUpgradeAtomicRendering(RpAtomic* atomic, bool setAlphaFlag) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2786](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FlcAt(CVehicle& v, size_t offset) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4391](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4391) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcParityEven(unsigned int value) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4394](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4394) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcApplyAxisTurn(FlcFrame& f, const float& d, const float* axis, const float* lever) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4421](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4421) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcClampVector(FlcX87& fp, float* vec, const float& thresholdSq, const float& limit) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4444](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4444) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcTail(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4470](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4470) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteAirship(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4536](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteWing(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4610](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4610) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteRotor(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:5595](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L5595) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| VehicleHeadLightEffect(CVehicle& vehicle, bool inlinedCopy, int32 lightId, CMatrix& vehicleMatrix, uint8 isRight, uint8 disabledOrAlarm) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8524](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8524) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| CVehNormalise2DInline(float x, float y) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8657](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8657) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1210"></a>

## Globales y helpers: source/game_sa/EntityScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScannerSector(float position, float range, bool useEnd) | [source/game_sa/EntityScanner.cpp:60](../../source/game_sa/EntityScanner.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1211"></a>

## Globales y helpers: source/game_sa/EntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) | [source/game_sa/EntryExit.cpp:36](../../source/game_sa/EntryExit.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitGetPositionRelativeToOutsideWorldRegs(CEntryExit* self, uint32 entryEdx, CVector* outPos) | [source/game_sa/EntryExit.cpp:45](../../source/game_sa/EntryExit.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEntryExit::GetPositionRelativeToOutsideWorld 0x0043EA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EntryExitGetPositionExe(CEntryExit* self, void* /*edx*/, CVector* out) | [source/game_sa/EntryExit.cpp:57](../../source/game_sa/EntryExit.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExit::GetPosition 0x0043E090 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1212"></a>

## Globales y helpers: source/game_sa/EntryExitManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitPoolFlushBody(uint8* pool) | [source/game_sa/EntryExitManager.cpp:41](../../source/game_sa/EntryExitManager.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitPoolDestructorBodyExe(void* self, void* /*edx*/) | [source/game_sa/EntryExitManager.cpp:55](../../source/game_sa/EntryExitManager.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExitManager::PoolDestructorBody 0x0043EEB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitPoolDestructorBodyCopyExe(void* self, void* /*edx*/) | [source/game_sa/EntryExitManager.cpp:60](../../source/game_sa/EntryExitManager.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExitManager::PoolDestructorBodyCopy 0x0043F780 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EntryExitPoolScalarDeletingDestructorExe(void* self, void* /*edx*/, uint8 flags) | [source/game_sa/EntryExitManager.cpp:66](../../source/game_sa/EntryExitManager.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExitManager::PoolScalarDeletingDestructor 0x0043F950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * EntryExitInSlot(int32 slot) | [source/game_sa/EntryExitManager.cpp:121](../../source/game_sa/EntryExitManager.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo mp_poolEntryExits: source/game_sa/EntryExitManager.cpp:27 (0x0096A7D8) |
| RawArea(const eAreaCodesS8& area) | [source/game_sa/EntryExitManager.cpp:190](../../source/game_sa/EntryExitManager.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CurrArea() | [source/game_sa/EntryExitManager.cpp:194](../../source/game_sa/EntryExitManager.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1213"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1214"></a>

## Globales y helpers: source/game_sa/Enums/eGameState.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eGameState gs) | [source/game_sa/Enums/eGameState.h:16](../../source/game_sa/Enums/eGameState.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1215"></a>

## Globales y helpers: source/game_sa/Enums/eIKArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IKArmToIKSlot(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:11](../../source/game_sa/Enums/eIKArm.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IKArmToBoneTag(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:19](../../source/game_sa/Enums/eIKArm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1216"></a>

## Globales y helpers: source/game_sa/Enums/eModelID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelIDForCutScene(eModelID modelId) | [source/game_sa/Enums/eModelID.h:14868](../../source/game_sa/Enums/eModelID.h#L14868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1217"></a>

## Globales y helpers: source/game_sa/Enums/ePedType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGang(ePedType ptype) | [source/game_sa/Enums/ePedType.h:61](../../source/game_sa/Enums/ePedType.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypePlayer(ePedType pt) | [source/game_sa/Enums/ePedType.h:65](../../source/game_sa/Enums/ePedType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale(ePedType type) | [source/game_sa/Enums/ePedType.h:74](../../source/game_sa/Enums/ePedType.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAllGangPedTypes() | [source/game_sa/Enums/ePedType.h:83](../../source/game_sa/Enums/ePedType.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGangOfPedType(ePedType ptype) | [source/game_sa/Enums/ePedType.h:114](../../source/game_sa/Enums/ePedType.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1218"></a>

## Globales y helpers: source/game_sa/Enums/eRadioID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eRadioID v) | [source/game_sa/Enums/eRadioID.h:30](../../source/game_sa/Enums/eRadioID.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1219"></a>

## Globales y helpers: source/game_sa/Enums/eTaskType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eTaskType taskType) | [source/game_sa/Enums/eTaskType.h:578](../../source/game_sa/Enums/eTaskType.h#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1220"></a>

## Globales y helpers: source/game_sa/Enums/eWeaponType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsWeaponTypeProjectile(eWeaponType weaponType) | [source/game_sa/Enums/eWeaponType.h:82](../../source/game_sa/Enums/eWeaponType.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
