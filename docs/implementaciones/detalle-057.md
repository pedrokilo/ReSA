# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

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

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/EntityScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScannerDistance(const CVector& a, const CVector& b) | [source/game_sa/EntityScanner.cpp:59](../../source/game_sa/EntityScanner.cpp#L59) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ScannerSector(float position, float delta) | [source/game_sa/EntityScanner.cpp:86](../../source/game_sa/EntityScanner.cpp#L86) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/EntityScanner.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntityScanner | [source/game_sa/EntityScanner.h:22](../../source/game_sa/EntityScanner.h#L22) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetTimer() | [source/game_sa/EntityScanner.h:42](../../source/game_sa/EntityScanner.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetClosestEntity() const | [source/game_sa/EntityScanner.h:48](../../source/game_sa/EntityScanner.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/EntityScanner.h:58](../../source/game_sa/EntityScanner.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/EntityScanner.h:63](../../source/game_sa/EntityScanner.h#L63) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/EntryExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntryExitGetPositionRelativeToOutsideWorld(CEntryExit* enex, void*, CVector* outPos) | [source/game_sa/EntryExit.cpp:35](../../source/game_sa/EntryExit.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntryExitGetPositionRelativeToOutsideWorldKeepEdx() | [source/game_sa/EntryExit.cpp:41](../../source/game_sa/EntryExit.cpp#L41) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| IsTeleportPointValid(const CVector& origin, const CVector& target) | [source/game_sa/EntryExit.cpp:120](../../source/game_sa/EntryExit.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/EntryExitManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawArea(const eAreaCodesS8& area) | [source/game_sa/EntryExitManager.cpp:91](../../source/game_sa/EntryExitManager.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CurrArea() | [source/game_sa/EntryExitManager.cpp:95](../../source/game_sa/EntryExitManager.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/Enums/eCarWheel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarWheelToCarPiece(eCarWheel i) | [source/game_sa/Enums/eCarWheel.cpp:6](../../source/game_sa/Enums/eCarWheel.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1127"></a>

## Globales y helpers: source/game_sa/Enums/eGameState.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eGameState gs) | [source/game_sa/Enums/eGameState.h:16](../../source/game_sa/Enums/eGameState.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/Enums/eIKArm.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IKArmToIKSlot(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:11](../../source/game_sa/Enums/eIKArm.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IKArmToBoneTag(eIKArm arm) | [source/game_sa/Enums/eIKArm.h:19](../../source/game_sa/Enums/eIKArm.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1129"></a>

## Globales y helpers: source/game_sa/Enums/eModelID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelIDForCutScene(eModelID modelId) | [source/game_sa/Enums/eModelID.h:14868](../../source/game_sa/Enums/eModelID.h#L14868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/Enums/ePedType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedTypeGang(ePedType ptype) | [source/game_sa/Enums/ePedType.h:61](../../source/game_sa/Enums/ePedType.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypePlayer(ePedType pt) | [source/game_sa/Enums/ePedType.h:65](../../source/game_sa/Enums/ePedType.h#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale(ePedType type) | [source/game_sa/Enums/ePedType.h:74](../../source/game_sa/Enums/ePedType.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAllGangPedTypes() | [source/game_sa/Enums/ePedType.h:83](../../source/game_sa/Enums/ePedType.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetGangOfPedType(ePedType ptype) | [source/game_sa/Enums/ePedType.h:114](../../source/game_sa/Enums/ePedType.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/Enums/eRadioID.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eRadioID v) | [source/game_sa/Enums/eRadioID.h:30](../../source/game_sa/Enums/eRadioID.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/Enums/eTaskType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eTaskType taskType) | [source/game_sa/Enums/eTaskType.h:578](../../source/game_sa/Enums/eTaskType.h#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

## Globales y helpers: source/game_sa/Enums/eWeaponType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsWeaponTypeProjectile(eWeaponType weaponType) | [source/game_sa/Enums/eWeaponType.h:82](../../source/game_sa/Enums/eWeaponType.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Enums/eWeatherType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsExtraSunny(eWeatherType wt) | [source/game_sa/Enums/eWeatherType.h:39](../../source/game_sa/Enums/eWeatherType.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/Events/Event.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEvent | [source/game_sa/Events/Event.h:12](../../source/game_sa/Events/Event.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) | [source/game_sa/Events/Event.h:28](../../source/game_sa/Events/Event.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPedGroup(CPedGroup* pedGroup) | [source/game_sa/Events/Event.h:29](../../source/game_sa/Events/Event.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCriminalEvent() | [source/game_sa/Events/Event.h:30](../../source/game_sa/Events/Event.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasEditableResponse() const | [source/game_sa/Events/Event.h:32](../../source/game_sa/Events/Event.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const | [source/game_sa/Events/Event.h:33](../../source/game_sa/Events/Event.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& other) | [source/game_sa/Events/Event.h:34](../../source/game_sa/Events/Event.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() | [source/game_sa/Events/Event.h:35](../../source/game_sa/Events/Event.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoInformVehicleOccupants(CPed* ped) | [source/game_sa/Events/Event.h:36](../../source/game_sa/Events/Event.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValid(CPed* ped) | [source/game_sa/Events/Event.h:37](../../source/game_sa/Events/Event.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() | [source/game_sa/Events/Event.h:38](../../source/game_sa/Events/Event.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UnTick() | [source/game_sa/Events/Event.h:43](../../source/game_sa/Events/Event.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Tick() | [source/game_sa/Events/Event.h:44](../../source/game_sa/Events/Event.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| classof(const From* f) | [source/game_sa/Events/Event.h:46](../../source/game_sa/Events/Event.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventAcquaintancePed.h:10](../../source/game_sa/Events/EventAcquaintancePed.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventAcquaintancePed.h:18](../../source/game_sa/Events/EventAcquaintancePed.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventAcquaintancePed.h:21](../../source/game_sa/Events/EventAcquaintancePed.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventAcquaintancePed.h:23](../../source/game_sa/Events/EventAcquaintancePed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedDislike.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedDislike | [source/game_sa/Events/EventAcquaintancePedDislike.h:10](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedDislike.h:14](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedDislike.h:15](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedDislike.h:16](../../source/game_sa/Events/EventAcquaintancePedDislike.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHate.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedHate | [source/game_sa/Events/EventAcquaintancePedHate.h:10](../../source/game_sa/Events/EventAcquaintancePedHate.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedHate.h:14](../../source/game_sa/Events/EventAcquaintancePedHate.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedHate.h:15](../../source/game_sa/Events/EventAcquaintancePedHate.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedHate.h:16](../../source/game_sa/Events/EventAcquaintancePedHate.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedHateBadlyLit | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:10](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:19](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:20](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:22](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h:23](../../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/Events/EventAcquaintancePedLike.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePedLike | [source/game_sa/Events/EventAcquaintancePedLike.h:10](../../source/game_sa/Events/EventAcquaintancePedLike.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventAcquaintancePedLike.h:14](../../source/game_sa/Events/EventAcquaintancePedLike.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventAcquaintancePedLike.h:15](../../source/game_sa/Events/EventAcquaintancePedLike.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventAcquaintancePedLike.h:16](../../source/game_sa/Events/EventAcquaintancePedLike.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
