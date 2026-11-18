<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1181"></a>

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
| VehStubProcessControlCollisionCheck(CVehicle* /*self*/, void* /*edx*/, bool /*applySpeed*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:196](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupSuspensionLines 0x006D6380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubProcessControlInputs(CVehicle* /*self*/, void* /*edx*/, uint8 /*playerNum*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:197](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::ProcessControlCollisionCheck 0x006D6270 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubGetComponentWorldPosition(CVehicle* /*self*/, void* /*edx*/, int32 /*componentId*/, CVector& /*outPos*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:198](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L198) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::ProcessControlInputs 0x006D6280 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsComponentPresent(CVehicle* /*self*/, void* /*edx*/, int32 /*componentId*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:199](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L199) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::GetComponentWorldPosition 0x006D6290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorReady(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:200](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsComponentPresent 0x006D62A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorFullyOpen(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:201](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorReady 0x006D62B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorClosed(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:202](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorFullyOpen 0x006D62C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorMissing(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:203](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L203) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorClosed 0x006D62D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorReadyU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:204](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorMissing 0x006D62E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorFullyOpenU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:205](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorReadyU32 0x006D62F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorClosedU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:206](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorFullyOpenU32 0x006D6300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsDoorMissingU32(CVehicle* /*self*/, void* /*edx*/, uint32 /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:207](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorClosedU32 0x006D6310 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubRemoveRefsToVehicle(CVehicle* /*self*/, void* /*edx*/, CEntity* /*entity*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:208](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L208) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsDoorMissingU32 0x006D6320 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubBlowUpCar(CVehicle* /*self*/, void* /*edx*/, CEntity* /*damager*/, bool /*bHideExplosion*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:209](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L209) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::RemoveRefsToVehicle 0x006D6330 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetUpWheelColModel(CVehicle* /*self*/, void* /*edx*/, CColModel* /*wheelCol*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:210](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L210) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::BlowUpCar 0x006D6340 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubBurstTyre(CVehicle* /*self*/, void* /*edx*/, uint8 /*tyreComponentId*/, bool /*bPhysicalEffect*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:211](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetUpWheelColModel 0x006D6350 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubIsRoomForPedToLeaveCar(CVehicle* /*self*/, void* /*edx*/, uint32 /*doorId*/, CVector* /*arg1*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:212](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::BurstTyre 0x006D6360 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetupSuspensionLines(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:213](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsRoomForPedToLeaveCar 0x006D6370 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubFix(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:214](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupSuspensionLines 0x006D6380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubSetupDamageAfterLoad(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:215](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::Fix 0x006D6390 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubDoBurstAndSoftGroundRatios(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:216](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::SetupDamageAfterLoad 0x006D63A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubPlayCarHorn(CVehicle* /*self*/, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:217](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::DoBurstAndSoftGroundRatios 0x006D63B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubGetDooorAngleOpenRatio(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:218](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::PlayCarHorn 0x006D63C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehStubFindWheelWidth(CVehicle* /*self*/, void* /*edx*/, bool /*bRear*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:219](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::GetDooorAngleOpenRatio 0x006D56B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehIsHandbrakeOnExe(const CVehicle* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:235](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L235) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsHandbrakeOn 0x004F4CA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehIsAlarmActiveExe(const CVehicle* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:238](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L238) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicle::IsAlarmActive 0x004F4CE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehTowTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1367](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1367) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CVehDMatPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:1483](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L1483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2240](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2240) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Object 0x006D2690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetVehicleAtomicVisibilityCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2253](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2253) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::SetVehicleAtomicVisibilityCB-Frame 0x006D26D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SetCompAlphaCB(RpMaterial* material, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2322](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2322) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2696](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2696) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Object 0x006D33B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RemoveObjectsCB(RwFrame* frame, void* data) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2722](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2722) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicle::RemoveObjectsCB-Frame 0x006D3420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetupUpgradeAtomicRendering(RpAtomic* atomic, bool setAlphaFlag) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:2771](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L2771) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FlcAt(CVehicle& v, size_t offset) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4339](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4339) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcParityEven(unsigned int value) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4342](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4342) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcApplyAxisTurn(FlcFrame& f, const float& d, const float* axis, const float* lever) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4369](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4369) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcClampVector(FlcX87& fp, float* vec, const float& thresholdSq, const float& limit) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4392](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4392) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlcTail(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4418](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4418) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteAirship(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4484](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4484) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteWing(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:4558](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L4558) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FlcRouteRotor(FlcFrame& f) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:5543](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L5543) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| VehicleHeadLightEffect(CVehicle& vehicle, bool inlinedCopy, int32 lightId, CMatrix& vehicleMatrix, uint8 isRight, uint8 disabledOrAlarm) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8336](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8336) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| CVehNormalise2DInline(float x, float y) | [source/game_sa/Entity/Vehicle/Vehicle.cpp:8469](../../source/game_sa/Entity/Vehicle/Vehicle.cpp#L8469) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1182"></a>

## Globales y helpers: source/game_sa/EntityScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScannerSector(float position, float range, bool useEnd) | [source/game_sa/EntityScanner.cpp:60](../../source/game_sa/EntityScanner.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1183"></a>

## Globales y helpers: source/game_sa/EntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) | [source/game_sa/EntryExit.cpp:36](../../source/game_sa/EntryExit.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitGetPositionRelativeToOutsideWorldRegs(CEntryExit* self, uint32 entryEdx, CVector* outPos) | [source/game_sa/EntryExit.cpp:44](../../source/game_sa/EntryExit.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall EntryExitGetPositionExe(CEntryExit* self, void* /*edx*/, CVector* out) | [source/game_sa/EntryExit.cpp:56](../../source/game_sa/EntryExit.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntryExit::GetPosition 0x0043E090 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1184"></a>

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

<a id="grupo-1185"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1186"></a>

## Globales y helpers: source/game_sa/Enums/eGameState.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eGameState gs) | [source/game_sa/Enums/eGameState.h:16](../../source/game_sa/Enums/eGameState.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1187"></a>

## Globales y helpers: source/game_sa/Enums/eIKArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IKArmToIKSlot(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:11](../../source/game_sa/Enums/eIKArm.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IKArmToBoneTag(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:19](../../source/game_sa/Enums/eIKArm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1188"></a>

## Globales y helpers: source/game_sa/Enums/eModelID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelIDForCutScene(eModelID modelId) | [source/game_sa/Enums/eModelID.h:14868](../../source/game_sa/Enums/eModelID.h#L14868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1189"></a>

## Globales y helpers: source/game_sa/Enums/ePedType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGang(ePedType ptype) | [source/game_sa/Enums/ePedType.h:61](../../source/game_sa/Enums/ePedType.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypePlayer(ePedType pt) | [source/game_sa/Enums/ePedType.h:65](../../source/game_sa/Enums/ePedType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale(ePedType type) | [source/game_sa/Enums/ePedType.h:74](../../source/game_sa/Enums/ePedType.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAllGangPedTypes() | [source/game_sa/Enums/ePedType.h:83](../../source/game_sa/Enums/ePedType.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGangOfPedType(ePedType ptype) | [source/game_sa/Enums/ePedType.h:114](../../source/game_sa/Enums/ePedType.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1190"></a>

## Globales y helpers: source/game_sa/Enums/eRadioID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eRadioID v) | [source/game_sa/Enums/eRadioID.h:30](../../source/game_sa/Enums/eRadioID.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1191"></a>

## Globales y helpers: source/game_sa/Enums/eTaskType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eTaskType taskType) | [source/game_sa/Enums/eTaskType.h:578](../../source/game_sa/Enums/eTaskType.h#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1192"></a>

## Globales y helpers: source/game_sa/Enums/eWeaponType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsWeaponTypeProjectile(eWeaponType weaponType) | [source/game_sa/Enums/eWeaponType.h:82](../../source/game_sa/Enums/eWeaponType.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1193"></a>

## Globales y helpers: source/game_sa/Enums/eWeatherType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsExtraSunny(eWeatherType wt) | [source/game_sa/Enums/eWeatherType.h:39](../../source/game_sa/Enums/eWeatherType.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1194"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHate.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HateDestructorBody(CEventAcquaintancePedHate* self, void* /*edx*/) | [source/game_sa/Events/EventAcquaintancePedHate.cpp:29](../../source/game_sa/Events/EventAcquaintancePedHate.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAcquaintancePedHate::DestructorBody 0x00420F30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1195"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HateBadlyLitDestructorBody(CEventAcquaintancePedHateBadlyLit* self, void* /*edx*/) | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp:29](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAcquaintancePedHateBadlyLit::DestructorBody 0x005FF2C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1196"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedLike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LikeDestructorBody(CEventAcquaintancePedLike* self, void* /*edx*/) | [source/game_sa/Events/EventAcquaintancePedLike.cpp:29](../../source/game_sa/Events/EventAcquaintancePedLike.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventAcquaintancePedLike::DestructorBody 0x005FF0F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1197"></a>

## Globales y helpers: source/game_sa/Events/EventBuildingCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BuildingCollisionDestructorBody(CEventBuildingCollision* self, void* /*edx*/) | [source/game_sa/Events/EventBuildingCollision.cpp:26](../../source/game_sa/Events/EventBuildingCollision.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventBuildingCollision::Destructor 0x004ACFF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1198"></a>

## Globales y helpers: source/game_sa/Events/EventDamage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeHitAnimDelayTimer(float delay) | [source/game_sa/Events/EventDamage.cpp:459](../../source/game_sa/Events/EventDamage.cpp#L459) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1199"></a>

## Globales y helpers: source/game_sa/Events/EventDanger.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DangerDestructorBody(CEventDanger* self, void* /*edx*/) | [source/game_sa/Events/EventDanger.cpp:26](../../source/game_sa/Events/EventDanger.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventDanger::Destructor 0x004B26A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1200"></a>

## Globales y helpers: source/game_sa/Events/EventDeadPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadPedDestructorBody(CEventDeadPed* self, void* /*edx*/) | [source/game_sa/Events/EventDeadPed.cpp:28](../../source/game_sa/Events/EventDeadPed.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
