<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-221"></a>

## CEventInAir

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventInAir::InjectHooks() | [source/game_sa/Events/EventInAir.cpp:23](../../source/game_sa/Events/EventInAir.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventInAir::Constructor() | [source/game_sa/Events/EventInAir.cpp:33](../../source/game_sa/Events/EventInAir.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventInAir::Constructor 0x004B0CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventInAir::AffectsPed(CPed* ped) | [source/game_sa/Events/EventInAir.cpp:40](../../source/game_sa/Events/EventInAir.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventInAir::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-222"></a>

## CEventInWater

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventInWater::CEventInWater(float acceleration) | [source/game_sa/Events/EventInWater.cpp:5](../../source/game_sa/Events/EventInWater.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventInWater::AffectsPed(CPed* ped) | [source/game_sa/Events/EventInWater.cpp:9](../../source/game_sa/Events/EventInWater.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventInWater::TakesPriorityOver(const CEvent& refEvent) | [source/game_sa/Events/EventInWater.cpp:16](../../source/game_sa/Events/EventInWater.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-223"></a>

## CEventInteriorUseInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventInteriorUseInfo::CEventInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction) | [source/game_sa/Events/EventInteriorUseInfo.cpp:5](../../source/game_sa/Events/EventInteriorUseInfo.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventInteriorUseInfo::IsValid(CPed* ped) | [source/game_sa/Events/EventInteriorUseInfo.cpp:12](../../source/game_sa/Events/EventInteriorUseInfo.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventInteriorUseInfo::AffectsPed(CPed* ped) | [source/game_sa/Events/EventInteriorUseInfo.cpp:18](../../source/game_sa/Events/EventInteriorUseInfo.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-224"></a>

## CEventKnockOffBike

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventKnockOffBike::InjectHooks() | [source/game_sa/Events/EventKnockOffBike.cpp:27](../../source/game_sa/Events/EventKnockOffBike.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::CEventKnockOffBike(CVehicle* vehicle, const CVector& moveSpeed, const CVector& collisionImpactVelocity, float damageIntensity, float a6, uint8 knockOffType, uint8 knockOffDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff) | [source/game_sa/Events/EventKnockOffBike.cpp:42](../../source/game_sa/Events/EventKnockOffBike.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::CEventKnockOffBike() | [source/game_sa/Events/EventKnockOffBike.cpp:60](../../source/game_sa/Events/EventKnockOffBike.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::~CEventKnockOffBike() | [source/game_sa/Events/EventKnockOffBike.cpp:73](../../source/game_sa/Events/EventKnockOffBike.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventKnockOffBike::Constructor(CVehicle* vehicle, CVector* moveSpeed, CVector* collisionImpactVelocity, float damageIntensity, float a6, uint8 knockOffType, uint8 knockOffDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff) | [source/game_sa/Events/EventKnockOffBike.cpp:79](../../source/game_sa/Events/EventKnockOffBike.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventKnockOffBike::Constructor() | [source/game_sa/Events/EventKnockOffBike.cpp:85](../../source/game_sa/Events/EventKnockOffBike.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::AffectsPed(CPed* ped) | [source/game_sa/Events/EventKnockOffBike.cpp:92](../../source/game_sa/Events/EventKnockOffBike.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventKnockOffBike::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::ReportCriminalEvent(CPed* ped) | [source/game_sa/Events/EventKnockOffBike.cpp:117](../../source/game_sa/Events/EventKnockOffBike.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventKnockOffBike::ReportCriminalEvent 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::From(const CEventKnockOffBike& right) | [source/game_sa/Events/EventKnockOffBike.cpp:126](../../source/game_sa/Events/EventKnockOffBike.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventKnockOffBike::From 0x004AFDD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::SetPedOutCar(CPed* ped) | [source/game_sa/Events/EventKnockOffBike.cpp:144](../../source/game_sa/Events/EventKnockOffBike.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventKnockOffBike::SetPedOutCar 0x004AFF60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::CalcForcesAndAnims(CPed* ped) | [source/game_sa/Events/EventKnockOffBike.cpp:182](../../source/game_sa/Events/EventKnockOffBike.cpp#L182) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventKnockOffBike::CalcForcesAndAnims 0x004B0020 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventKnockOffBike::SetPedSafePosition(CPed* ped) | [source/game_sa/Events/EventKnockOffBike.cpp:331](../../source/game_sa/Events/EventKnockOffBike.cpp#L331) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventKnockOffBike::SetPedSafePosition 0x004B4AC0 | 0 / 0 | Posible dato fijo pIgnoreEntity: source/game_sa/World.h:67 (0x00B7CD68) |

<a id="grupo-225"></a>

## CEventLeaderEnteredCarAsDriver

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderEnteredCarAsDriver::InjectHooks() | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp:23](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeaderEnteredCarAsDriver::CEventLeaderEnteredCarAsDriver(CVehicle* vehicle) | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp:33](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderEnteredCarAsDriver::Constructor 0x0048E1C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeaderEnteredCarAsDriver::~CEventLeaderEnteredCarAsDriver() | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp:39](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventLeaderEnteredCarAsDriver::Constructor(CVehicle* vehicle) | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp:45](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderEnteredCarAsDriver::Constructor 0x0048E1C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeaderEnteredCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup) | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp:52](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderEnteredCarAsDriver::AffectsPedGroup 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-226"></a>

## CEventLeaderEntryExit

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderEntryExit::InjectHooks() | [source/game_sa/Events/EventLeaderEntryExit.cpp:23](../../source/game_sa/Events/EventLeaderEntryExit.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventLeaderEntryExit::Constructor(CPed* ped) | [source/game_sa/Events/EventLeaderEntryExit.cpp:32](../../source/game_sa/Events/EventLeaderEntryExit.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderEntryExit::Constructor 0x0043E1C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-227"></a>

## CEventLeaderExitedCarAsDriver

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderExitedCarAsDriver::InjectHooks() | [source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp:23](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventLeaderExitedCarAsDriver::Constructor() | [source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp:33](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderExitedCarAsDriver::Constructor 0x004B8300 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeaderExitedCarAsDriver::AffectsPedGroup(CPedGroup* pg) | [source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp:40](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderExitedCarAsDriver::AffectsPedGroup 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-228"></a>

## CEventLeaderQuitEnteringCarAsDriver

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderQuitEnteringCarAsDriver::InjectHooks() | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp:23](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventLeaderQuitEnteringCarAsDriver::Constructor() | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp:33](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderQuitEnteringCarAsDriver::Constructor 0x0063A110 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeaderQuitEnteringCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup) | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp:40](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeaderQuitEnteringCarAsDriver::AffectsPedGroup 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-229"></a>

## CEventLeanOnVehicle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeanOnVehicle::InjectHooks() | [source/game_sa/Events/EventLeanOnVehicle.cpp:23](../../source/game_sa/Events/EventLeanOnVehicle.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventLeanOnVehicle::Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs) | [source/game_sa/Events/EventLeanOnVehicle.cpp:32](../../source/game_sa/Events/EventLeanOnVehicle.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeanOnVehicle::Constructor 0x0065DAF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeanOnVehicle::CEventLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs) | [source/game_sa/Events/EventLeanOnVehicle.cpp:35](../../source/game_sa/Events/EventLeanOnVehicle.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeanOnVehicle::Constructor 0x0065DAF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeanOnVehicle::~CEventLeanOnVehicle() | [source/game_sa/Events/EventLeanOnVehicle.cpp:41](../../source/game_sa/Events/EventLeanOnVehicle.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeanOnVehicle::IsValid(CPed* ped) | [source/game_sa/Events/EventLeanOnVehicle.cpp:46](../../source/game_sa/Events/EventLeanOnVehicle.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventLeanOnVehicle::IsValid 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-230"></a>

## CEventNewGangMember

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventNewGangMember::InjectHooks() | [source/game_sa/Events/EventNewGangMember.cpp:23](../../source/game_sa/Events/EventNewGangMember.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventNewGangMember::CEventNewGangMember(CPed* member) | [source/game_sa/Events/EventNewGangMember.cpp:32](../../source/game_sa/Events/EventNewGangMember.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventNewGangMember::Constructor 0x00608F70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventNewGangMember::~CEventNewGangMember() | [source/game_sa/Events/EventNewGangMember.cpp:38](../../source/game_sa/Events/EventNewGangMember.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventNewGangMember::Constructor(CPed* member) | [source/game_sa/Events/EventNewGangMember.cpp:44](../../source/game_sa/Events/EventNewGangMember.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventNewGangMember::Constructor 0x00608F70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-231"></a>

## CEventObjectCollision

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventObjectCollision::InjectHooks() | [source/game_sa/Events/EventObjectCollision.cpp:23](../../source/game_sa/Events/EventObjectCollision.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventObjectCollision::CEventObjectCollision(int16 pieceType, float damageIntensity, CObject* object, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState) | [source/game_sa/Events/EventObjectCollision.cpp:33](../../source/game_sa/Events/EventObjectCollision.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventObjectCollision::Constructor 0x004ACCF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventObjectCollision::~CEventObjectCollision() | [source/game_sa/Events/EventObjectCollision.cpp:44](../../source/game_sa/Events/EventObjectCollision.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventObjectCollision::Constructor(int16 pieceType, float damageIntensity, CObject* object, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState) | [source/game_sa/Events/EventObjectCollision.cpp:50](../../source/game_sa/Events/EventObjectCollision.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventObjectCollision::Constructor 0x004ACCF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventObjectCollision::AffectsPed(CPed* ped) | [source/game_sa/Events/EventObjectCollision.cpp:57](../../source/game_sa/Events/EventObjectCollision.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventObjectCollision::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-232"></a>

## CEventOnFire

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventOnFire::InjectHooks() | [source/game_sa/Events/EventOnFire.cpp:23](../../source/game_sa/Events/EventOnFire.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventOnFire::AffectsPed(CPed* ped) | [source/game_sa/Events/EventOnFire.cpp:30](../../source/game_sa/Events/EventOnFire.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventOnFire::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-233"></a>

## CEventPassObject

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPassObject::InjectHooks() | [source/game_sa/Events/EventPassObject.cpp:23](../../source/game_sa/Events/EventPassObject.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPassObject::Constructor(CPed* giver, bool dontPassObject) | [source/game_sa/Events/EventPassObject.cpp:32](../../source/game_sa/Events/EventPassObject.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPassObject::Constructor 0x0065DC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPassObject::CEventPassObject(CPed* giver, bool dontPassObject) | [source/game_sa/Events/EventPassObject.cpp:35](../../source/game_sa/Events/EventPassObject.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPassObject::Constructor 0x0065DC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPassObject::~CEventPassObject() | [source/game_sa/Events/EventPassObject.cpp:42](../../source/game_sa/Events/EventPassObject.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPassObject::IsValid(CPed* ped) | [source/game_sa/Events/EventPassObject.cpp:47](../../source/game_sa/Events/EventPassObject.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPassObject::IsValid 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-234"></a>

## CEventPedCollisionWithPed

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedCollisionWithPed::InjectHooks() | [source/game_sa/Events/EventPedCollisionWithPed.cpp:24](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedCollisionWithPed::CEventPedCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) | [source/game_sa/Events/EventPedCollisionWithPed.cpp:35](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPedCollisionWithPed::Constructor 0x004AC990 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedCollisionWithPed::~CEventPedCollisionWithPed() | [source/game_sa/Events/EventPedCollisionWithPed.cpp:47](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPedCollisionWithPed::Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) | [source/game_sa/Events/EventPedCollisionWithPed.cpp:53](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPedCollisionWithPed::Constructor 0x004AC990 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedCollisionWithPed::TakesPriorityOver(const CEvent& refEvent) | [source/game_sa/Events/EventPedCollisionWithPed.cpp:60](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPedCollisionWithPed::TakesPriorityOver 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedCollisionWithPed::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPedCollisionWithPed.cpp:68](../../source/game_sa/Events/EventPedCollisionWithPed.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPedCollisionWithPed::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-235"></a>

## CEventPedCollisionWithPlayer

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedCollisionWithPlayer::InjectHooks() | [source/game_sa/Events/EventPedCollisionWithPlayer.cpp:23](../../source/game_sa/Events/EventPedCollisionWithPlayer.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedCollisionWithPlayer::CEventPedCollisionWithPlayer(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) | [source/game_sa/Events/EventPedCollisionWithPlayer.cpp:32](../../source/game_sa/Events/EventPedCollisionWithPlayer.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPedCollisionWithPlayer::Constructor 0x005FED40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPedCollisionWithPlayer::Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) | [source/game_sa/Events/EventPedCollisionWithPlayer.cpp:38](../../source/game_sa/Events/EventPedCollisionWithPlayer.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPedCollisionWithPlayer::Constructor 0x005FED40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-236"></a>

## CEventPedEnteredMyVehicle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedEnteredMyVehicle::CEventPedEnteredMyVehicle(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor) | [source/game_sa/Events/EventPedEnteredMyVehicle.cpp:5](../../source/game_sa/Events/EventPedEnteredMyVehicle.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedEnteredMyVehicle::~CEventPedEnteredMyVehicle() | [source/game_sa/Events/EventPedEnteredMyVehicle.cpp:14](../../source/game_sa/Events/EventPedEnteredMyVehicle.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedEnteredMyVehicle::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPedEnteredMyVehicle.cpp:19](../../source/game_sa/Events/EventPedEnteredMyVehicle.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-237"></a>

## CEventPedToChase

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedToChase::CEventPedToChase(CPed* ped) | [source/game_sa/Events/EventPedToChase.cpp:6](../../source/game_sa/Events/EventPedToChase.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedToChase::~CEventPedToChase() | [source/game_sa/Events/EventPedToChase.cpp:12](../../source/game_sa/Events/EventPedToChase.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-238"></a>

## CEventPedToFlee

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedToFlee::CEventPedToFlee(CPed* ped) | [source/game_sa/Events/EventPedToFlee.cpp:6](../../source/game_sa/Events/EventPedToFlee.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPedToFlee::~CEventPedToFlee() | [source/game_sa/Events/EventPedToFlee.cpp:12](../../source/game_sa/Events/EventPedToFlee.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-239"></a>

## CEventPlayerCollisionWithPed

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCollisionWithPed::InjectHooks() | [source/game_sa/Events/EventPlayerCollisionWithPed.cpp:23](../../source/game_sa/Events/EventPlayerCollisionWithPed.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPlayerCollisionWithPed::CEventPlayerCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) | [source/game_sa/Events/EventPlayerCollisionWithPed.cpp:32](../../source/game_sa/Events/EventPlayerCollisionWithPed.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCollisionWithPed::Constructor 0x005FEE40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPlayerCollisionWithPed::Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) | [source/game_sa/Events/EventPlayerCollisionWithPed.cpp:38](../../source/game_sa/Events/EventPlayerCollisionWithPed.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCollisionWithPed::Constructor 0x005FEE40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-240"></a>

## CEventPlayerCommandToGroup

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCommandToGroup::InjectHooks() | [source/game_sa/Events/EventPlayerCommandToGroup.cpp:23](../../source/game_sa/Events/EventPlayerCommandToGroup.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPlayerCommandToGroup::CEventPlayerCommandToGroup(ePlayerGroupCommand command, CPed* target) | [source/game_sa/Events/EventPlayerCommandToGroup.cpp:33](../../source/game_sa/Events/EventPlayerCommandToGroup.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCommandToGroup::Constructor 0x004B23D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPlayerCommandToGroup::~CEventPlayerCommandToGroup() | [source/game_sa/Events/EventPlayerCommandToGroup.cpp:40](../../source/game_sa/Events/EventPlayerCommandToGroup.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPlayerCommandToGroup::Constructor(ePlayerGroupCommand command, CPed* target) | [source/game_sa/Events/EventPlayerCommandToGroup.cpp:46](../../source/game_sa/Events/EventPlayerCommandToGroup.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCommandToGroup::Constructor 0x004B23D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPlayerCommandToGroup::AffectsPedGroup(CPedGroup* pedGroup) | [source/game_sa/Events/EventPlayerCommandToGroup.cpp:53](../../source/game_sa/Events/EventPlayerCommandToGroup.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCommandToGroup::AffectsPedGroup 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
