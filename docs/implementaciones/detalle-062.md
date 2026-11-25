<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1221"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/MonsterTruck.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote125AMonsterTruckDestructorBody() | [source/game_sa/Entity/Vehicle/MonsterTruck.cpp:343](../../source/game_sa/Entity/Vehicle/MonsterTruck.cpp#L343) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1222"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Plane.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlaneHeadingMatrixExe(CVehicle* self, void* /*edx*/, float heading) | [source/game_sa/Entity/Vehicle/Plane.cpp:58](../../source/game_sa/Entity/Vehicle/Plane.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlane::SetHeadingMatrix 0x006C4130 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneDestructorBodyExe(CPlane* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Plane.cpp:69](../../source/game_sa/Entity/Vehicle/Plane.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| PlaneGenerationHeading355(CVehicle* vehicle, float heading) | [source/game_sa/Entity/Vehicle/Plane.cpp:544](../../source/game_sa/Entity/Vehicle/Plane.cpp#L544) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlane::DoPlaneGenerationAndRemoval 0x006CD2F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationVelocity355(float heading, bool helicopter) | [source/game_sa/Entity/Vehicle/Plane.cpp:558](../../source/game_sa/Entity/Vehicle/Plane.cpp#L558) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationSelect355(int32 aircraftCount) | [source/game_sa/Entity/Vehicle/Plane.cpp:563](../../source/game_sa/Entity/Vehicle/Plane.cpp#L563) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneGenerationEngine355(CVehicle* vehicle) | [source/game_sa/Entity/Vehicle/Plane.cpp:608](../../source/game_sa/Entity/Vehicle/Plane.cpp#L608) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneQuaternionToMatrix(const CQuaternion& quaternion, RwMatrix* matrix) | [source/game_sa/Entity/Vehicle/Plane.cpp:672](../../source/game_sa/Entity/Vehicle/Plane.cpp#L672) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CQuaternion::Get-matrix 0x0059C080 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneTransformPoint(const CMatrix* matrix, const CVector& point) | [source/game_sa/Entity/Vehicle/Plane.cpp:677](../../source/game_sa/Entity/Vehicle/Plane.cpp#L677) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1223"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/QuadBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| QuadMultiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:34](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:43](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMax(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:52](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuadMin(float a, float b) | [source/game_sa/Entity/Vehicle/QuadBike.cpp:53](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote141AQuadBikeDestructorBody() | [source/game_sa/Entity/Vehicle/QuadBike.cpp:68](../../source/game_sa/Entity/Vehicle/QuadBike.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1224"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Trailer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Trailer.cpp:42](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Vehicle/Trailer.cpp:51](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeadingX87(const CPlaceable* placeable) | [source/game_sa/Entity/Vehicle/Trailer.cpp:65](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SteerTowardsTractor(const CPlaceable* tractor, const CPlaceable* self) | [source/game_sa/Entity/Vehicle/Trailer.cpp:75](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrailerDestructorBody(CTrailer* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Trailer.cpp:87](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTrailer::Destructor 0x006CED10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote140ATrailerDestructorBody() | [source/game_sa/Entity/Vehicle/Trailer.cpp:104](../../source/game_sa/Entity/Vehicle/Trailer.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1225"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Train.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTrainBaseDestructorCall(CTrain* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Train.cpp:56](../../source/game_sa/Entity/Vehicle/Train.cpp#L56) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CVehicle::Destructor 0x006E2B40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrainSetRotateAddPos(CMatrix& mat, const CVector& rot, const CVector& saved) | [source/game_sa/Entity/Vehicle/Train.cpp:80](../../source/game_sa/Entity/Vehicle/Train.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QuietThroughX87(float value) | [source/game_sa/Entity/Vehicle/Train.cpp:89](../../source/game_sa/Entity/Vehicle/Train.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote174ABody_CTrainBaseDestructor() | [source/game_sa/Entity/Vehicle/Train.cpp:98](../../source/game_sa/Entity/Vehicle/Train.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NodeDistanceFromStart(const CTrainNode& node) | [source/game_sa/Entity/Vehicle/Train.cpp:1098](../../source/game_sa/Entity/Vehicle/Train.cpp#L1098) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1226"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Vehicle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehEFtol(float f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:96](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:99](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEMatVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:108](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEInvVec(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:117](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEDot(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:126](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehECross(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:131](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehEPow(float base, float exponent) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:140](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehESinCos(float angle, float& outSin, float& outCos) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:145](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise2D(CVector2D& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:151](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormalise(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:163](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehENormaliseAndMag(CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:176](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehicleSetEngineOnExe(CVehicle* self, void* /*edx*/, uint8 on) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:193](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubProcessControlCollisionCheck(CVehicle* /*self*/, void* /*edx*/, bool /*applySpeed*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:210](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L210) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupSuspensionLines 0x006D6380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubProcessControlInputs(CVehicle* /*self*/, void* /*edx*/, uint8 /*playerNum*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:211](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::ProcessControlCollisionCheck 0x006D6270 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubGetComponentWorldPosition(CVehicle* /*self*/, void* /*edx*/, int32 /*componentId*/, CVector& /*outPos*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:212](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::ProcessControlInputs 0x006D6280 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsComponentPresent(CVehicle* /*self*/, void* /*edx*/, int32 /*componentId*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:213](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::GetComponentWorldPosition 0x006D6290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorReady(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:214](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsComponentPresent 0x006D62A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorFullyOpen(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:215](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorReady 0x006D62B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorClosed(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:216](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorFullyOpen 0x006D62C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorMissing(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:217](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorClosed 0x006D62D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorReadyU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:218](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorMissing 0x006D62E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorFullyOpenU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:219](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorReadyU32 0x006D62F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorClosedU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:220](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorFullyOpenU32 0x006D6300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorMissingU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:221](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorClosedU32 0x006D6310 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubRemoveRefsToVehicle(CVehicle* /*self*/, void* /*edx*/, CEntity* /*entity*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:222](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L222) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorMissingU32 0x006D6320 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubBlowUpCar(CVehicle* /*self*/, void* /*edx*/, CEntity* /*damager*/, bool /*bHideExplosion*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:223](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L223) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::RemoveRefsToVehicle 0x006D6330 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetUpWheelColModel(CVehicle* /*self*/, void* /*edx*/, CColModel* /*wheelCol*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:224](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L224) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::BlowUpCar 0x006D6340 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubBurstTyre(CVehicle* /*self*/, void* /*edx*/, uint8 /*tyreComponentId*/, bool /*bPhysicalEffect*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:225](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetUpWheelColModel 0x006D6350 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsRoomForPedToLeaveCar(CVehicle* /*self*/, void* /*edx*/, uint32 /*doorId*/, CVector* /*arg1*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:226](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L226) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::BurstTyre 0x006D6360 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetupSuspensionLines(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:227](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L227) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsRoomForPedToLeaveCar 0x006D6370 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubFix(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:228](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L228) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupSuspensionLines 0x006D6380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetupDamageAfterLoad(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:229](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L229) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::Fix 0x006D6390 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubDoBurstAndSoftGroundRatios(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:230](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupDamageAfterLoad 0x006D63A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubPlayCarHorn(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:231](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L231) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::DoBurstAndSoftGroundRatios 0x006D63B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubGetDooorAngleOpenRatio(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:232](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L232) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::PlayCarHorn 0x006D63C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubFindWheelWidth(CVehicle* /*self*/, void* /*edx*/, bool /*bRear*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:233](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L233) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::GetDooorAngleOpenRatio 0x006D56B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehIsHandbrakeOnExe(const CVehicle* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:249](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsHandbrakeOn 0x004F4CA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehIsAlarmActiveExe(const CVehicle* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:252](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L252) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsAlarmActive 0x004F4CE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehTowTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1383](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1383) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehDMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1499](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1499) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2259](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2259) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Object 0x006D2690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2272](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2272) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Frame 0x006D26D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2718](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2718) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Object 0x006D33B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2744](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2744) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Frame 0x006D3420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetupUpgradeAtomicRendering(RpAtomic* atomic, bool setAlphaFlag) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2794](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2794) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FlcAt(CVehicle& v, size_t offset) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4399](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4399) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcParityEven(unsigned int value) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4402](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4402) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcApplyAxisTurn(FlcFrame& f, const float& d, const float* axis, const float* lever) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4429](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4429) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcClampVector(FlcX87& fp, float* vec, const float& thresholdSq, const float& limit) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4452](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4452) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcTail(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4478](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4478) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:38 (0x00B7CB5C) |
| FlcRouteAirship(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4544](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4544) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:38 (0x00B7CB5C) |
| FlcRouteWing(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4618](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4618) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:38 (0x00B7CB5C) |
| FlcRouteRotor(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:5603](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L5603) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:38 (0x00B7CB5C) |
| VehicleHeadLightEffect(CVehicle& vehicle, bool inlinedCopy, int32 lightId, CMatrix& vehicleMatrix, uint8 isRight, uint8 disabledOrAlarm) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8532](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8532) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:50 (0x00B6F028) |
| CVehNormalise2DInline(float x, float y) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8665](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8665) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1227"></a>

## Globales y helpers: source/game_sa/EntityScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScannerSector(float position, float range, bool useEnd) | [source/game_sa/EntityScanner.cpp:60](../../source/game_sa/EntityScanner.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1228"></a>

## Globales y helpers: source/game_sa/EntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) | [source/game_sa/EntryExit.cpp:36](../../source/game_sa/EntryExit.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitGetPositionRelativeToOutsideWorldRegs(CEntryExit* self, uint32 entryEdx, CVector* outPos) | [source/game_sa/EntryExit.cpp:45](../../source/game_sa/EntryExit.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEntryExit::GetPositionRelativeToOutsideWorld 0x0043EA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EntryExitGetPositionExe(CEntryExit* self, void* /*edx*/, CVector* out) | [source/game_sa/EntryExit.cpp:57](../../source/game_sa/EntryExit.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExit::GetPosition 0x0043E090 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1229"></a>

## Globales y helpers: source/game_sa/EntryExitManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitPoolFlushBody(uint8* pool) | [source/game_sa/EntryExitManager.cpp:44](../../source/game_sa/EntryExitManager.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitPoolDestructorBodyExe(void* self, void* /*edx*/) | [source/game_sa/EntryExitManager.cpp:58](../../source/game_sa/EntryExitManager.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExitManager::PoolDestructorBody 0x0043EEB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitPoolDestructorBodyCopyExe(void* self, void* /*edx*/) | [source/game_sa/EntryExitManager.cpp:63](../../source/game_sa/EntryExitManager.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExitManager::PoolDestructorBodyCopy 0x0043F780 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EntryExitPoolScalarDeletingDestructorExe(void* self, void* /*edx*/, uint8 flags) | [source/game_sa/EntryExitManager.cpp:69](../../source/game_sa/EntryExitManager.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExitManager::PoolScalarDeletingDestructor 0x0043F950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * EntryExitInSlot(int32 slot) | [source/game_sa/EntryExitManager.cpp:124](../../source/game_sa/EntryExitManager.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawArea(const eAreaCodesS8& area) | [source/game_sa/EntryExitManager.cpp:193](../../source/game_sa/EntryExitManager.cpp#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CurrArea() | [source/game_sa/EntryExitManager.cpp:197](../../source/game_sa/EntryExitManager.cpp#L197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:23 (0x00B72914) |

<a id="grupo-1230"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1231"></a>

## Globales y helpers: source/game_sa/Enums/eGameState.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eGameState gs) | [source/game_sa/Enums/eGameState.h:16](../../source/game_sa/Enums/eGameState.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1232"></a>

## Globales y helpers: source/game_sa/Enums/eIKArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IKArmToIKSlot(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:11](../../source/game_sa/Enums/eIKArm.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IKArmToBoneTag(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:19](../../source/game_sa/Enums/eIKArm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1233"></a>

## Globales y helpers: source/game_sa/Enums/eModelID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelIDForCutScene(eModelID modelId) | [source/game_sa/Enums/eModelID.h:14868](../../source/game_sa/Enums/eModelID.h#L14868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1234"></a>

## Globales y helpers: source/game_sa/Enums/ePedType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGang(ePedType ptype) | [source/game_sa/Enums/ePedType.h:61](../../source/game_sa/Enums/ePedType.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypePlayer(ePedType pt) | [source/game_sa/Enums/ePedType.h:65](../../source/game_sa/Enums/ePedType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale(ePedType type) | [source/game_sa/Enums/ePedType.h:74](../../source/game_sa/Enums/ePedType.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAllGangPedTypes() | [source/game_sa/Enums/ePedType.h:83](../../source/game_sa/Enums/ePedType.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGangOfPedType(ePedType ptype) | [source/game_sa/Enums/ePedType.h:114](../../source/game_sa/Enums/ePedType.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1235"></a>

## Globales y helpers: source/game_sa/Enums/eRadioID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eRadioID v) | [source/game_sa/Enums/eRadioID.h:30](../../source/game_sa/Enums/eRadioID.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1236"></a>

## Globales y helpers: source/game_sa/Enums/eTaskType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eTaskType taskType) | [source/game_sa/Enums/eTaskType.h:578](../../source/game_sa/Enums/eTaskType.h#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1237"></a>

## Globales y helpers: source/game_sa/Enums/eWeaponType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsWeaponTypeProjectile(eWeaponType weaponType) | [source/game_sa/Enums/eWeaponType.h:82](../../source/game_sa/Enums/eWeaponType.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1238"></a>

## Globales y helpers: source/game_sa/Enums/eWeatherType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsExtraSunny(eWeatherType wt) | [source/game_sa/Enums/eWeatherType.h:39](../../source/game_sa/Enums/eWeatherType.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1239"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHate.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HateDestructorBody(CEventAcquaintancePedHate* self, void* /*edx*/) | [source/game_sa/Events/EventAcquaintancePedHate.cpp:29](../../source/game_sa/Events/EventAcquaintancePedHate.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAcquaintancePedHate::DestructorBody 0x00420F30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1240"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HateBadlyLitDestructorBody(CEventAcquaintancePedHateBadlyLit* self, void* /*edx*/) | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp:29](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAcquaintancePedHateBadlyLit::DestructorBody 0x005FF2C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
