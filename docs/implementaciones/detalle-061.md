<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1201"></a>

## Globales y helpers: source/game_sa/Events/EventStuckInAir.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventStuckInAir | [source/game_sa/Events/EventStuckInAir.h:9](../../source/game_sa/Events/EventStuckInAir.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventStuckInAir.h:17](../../source/game_sa/Events/EventStuckInAir.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventStuckInAir.h:19](../../source/game_sa/Events/EventStuckInAir.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventStuckInAir.h:20](../../source/game_sa/Events/EventStuckInAir.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventStuckInAir.h:23](../../source/game_sa/Events/EventStuckInAir.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1202"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleCollision.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventVehicleCollision | [source/game_sa/Events/EventVehicleCollision.h:15](../../source/game_sa/Events/EventVehicleCollision.h#L15) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleCollision.h:32](../../source/game_sa/Events/EventVehicleCollision.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventVehicleCollision.h:33](../../source/game_sa/Events/EventVehicleCollision.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventVehicleCollision.h:34](../../source/game_sa/Events/EventVehicleCollision.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventVehicleCollision.h:35](../../source/game_sa/Events/EventVehicleCollision.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventVehicleCollision.h:37](../../source/game_sa/Events/EventVehicleCollision.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1203"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamage.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventVehicleDamage.h:9](../../source/game_sa/Events/EventVehicleDamage.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventVehicleDamage.h:21](../../source/game_sa/Events/EventVehicleDamage.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventVehicleDamage.h:22](../../source/game_sa/Events/EventVehicleDamage.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventVehicleDamage.h:27](../../source/game_sa/Events/EventVehicleDamage.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1204"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageCollision.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventVehicleDamageCollision | [source/game_sa/Events/EventVehicleDamageCollision.h:5](../../source/game_sa/Events/EventVehicleDamageCollision.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleDamageCollision.h:10](../../source/game_sa/Events/EventVehicleDamageCollision.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1205"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageWeapon.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventVehicleDamageWeapon | [source/game_sa/Events/EventVehicleDamageWeapon.h:5](../../source/game_sa/Events/EventVehicleDamageWeapon.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleDamageWeapon.h:10](../../source/game_sa/Events/EventVehicleDamageWeapon.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1206"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDied.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventVehicleDied | [source/game_sa/Events/EventVehicleDied.h:7](../../source/game_sa/Events/EventVehicleDied.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleDied.h:16](../../source/game_sa/Events/EventVehicleDied.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventVehicleDied.h:17](../../source/game_sa/Events/EventVehicleDied.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventVehicleDied.h:18](../../source/game_sa/Events/EventVehicleDied.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventVehicleDied.h:19](../../source/game_sa/Events/EventVehicleDied.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1207"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleHitAndRun.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventVehicleHitAndRun | [source/game_sa/Events/EventVehicleHitAndRun.h:9](../../source/game_sa/Events/EventVehicleHitAndRun.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleHitAndRun.h:18](../../source/game_sa/Events/EventVehicleHitAndRun.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventVehicleHitAndRun.h:19](../../source/game_sa/Events/EventVehicleHitAndRun.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventVehicleHitAndRun.h:20](../../source/game_sa/Events/EventVehicleHitAndRun.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventVehicleHitAndRun.h:21](../../source/game_sa/Events/EventVehicleHitAndRun.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventVehicleHitAndRun.h:22](../../source/game_sa/Events/EventVehicleHitAndRun.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCriminalEvent() override | [source/game_sa/Events/EventVehicleHitAndRun.h:23](../../source/game_sa/Events/EventVehicleHitAndRun.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventVehicleHitAndRun.h:25](../../source/game_sa/Events/EventVehicleHitAndRun.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1208"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleOnFire.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventVehicleOnFire.h:7](../../source/game_sa/Events/EventVehicleOnFire.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleOnFire.h:17](../../source/game_sa/Events/EventVehicleOnFire.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventVehicleOnFire.h:18](../../source/game_sa/Events/EventVehicleOnFire.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventVehicleOnFire.h:19](../../source/game_sa/Events/EventVehicleOnFire.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventVehicleOnFire.h:21](../../source/game_sa/Events/EventVehicleOnFire.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1209"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleThreat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventVehicleThreat.h:6](../../source/game_sa/Events/EventVehicleThreat.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleThreat.h:14](../../source/game_sa/Events/EventVehicleThreat.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventVehicleThreat.h:15](../../source/game_sa/Events/EventVehicleThreat.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventVehicleThreat.h:16](../../source/game_sa/Events/EventVehicleThreat.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventVehicleThreat.h:18](../../source/game_sa/Events/EventVehicleThreat.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1210"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleToSteal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventVehicleToSteal | [source/game_sa/Events/EventVehicleToSteal.h:10](../../source/game_sa/Events/EventVehicleToSteal.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventVehicleToSteal.h:18](../../source/game_sa/Events/EventVehicleToSteal.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventVehicleToSteal.h:19](../../source/game_sa/Events/EventVehicleToSteal.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventVehicleToSteal.h:20](../../source/game_sa/Events/EventVehicleToSteal.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventVehicleToSteal.h:21](../../source/game_sa/Events/EventVehicleToSteal.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1211"></a>

## Globales y helpers: source/game_sa/Events/GroupEventHandler.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedInPlayersGroup(CPedGroup* pg, CPed* p) | [source/game_sa/Events/GroupEventHandler.cpp:111](../../source/game_sa/Events/GroupEventHandler.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MaybeAdjustTaskOfGroupThreatEvent(const CEventEditableResponse& e, CPedGroup* pg, CPed* srcPed) | [source/game_sa/Events/GroupEventHandler.cpp:124](../../source/game_sa/Events/GroupEventHandler.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PickPartnerFollower(CPedGroup* pg, CPed* other) | [source/game_sa/Events/GroupEventHandler.cpp:786](../../source/game_sa/Events/GroupEventHandler.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerLookAts(CPedGroup* pg, CPed* closest, const char* purpose) | [source/game_sa/Events/GroupEventHandler.cpp:827](../../source/game_sa/Events/GroupEventHandler.cpp#L827) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1212"></a>

## Globales y helpers: source/game_sa/Explosion.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| U32ToFloat(uint32 value) | [source/game_sa/Explosion.cpp:56](../../source/game_sa/Explosion.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoesNeedToVehProcessBombTimer(eExplosionType type) | [source/game_sa/Explosion.cpp:171](../../source/game_sa/Explosion.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExplosionRand01() | [source/game_sa/Explosion.cpp:184](../../source/game_sa/Explosion.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1213"></a>

## Globales y helpers: source/game_sa/FileLoader.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeCIacos(float x) | [source/game_sa/FileLoader.cpp:1066](../../source/game_sa/FileLoader.cpp#L1066) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AddTextureCB(RwTexture* texture, void* dict) | [source/game_sa/FileLoader.cpp:2452](../../source/game_sa/FileLoader.cpp#L2452) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneAtomicToClumpCB(RpAtomic* atomic, void* data) | [source/game_sa/FileLoader.cpp:2464](../../source/game_sa/FileLoader.cpp#L2464) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFilename(const char* filepath) | [source/game_sa/FileLoader.cpp:2474](../../source/game_sa/FileLoader.cpp#L2474) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LoadingScreenLoadingFile(const char* str) | [source/game_sa/FileLoader.cpp:2480](../../source/game_sa/FileLoader.cpp#L2480) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gString: source/game_sa/common.h:63 (0x00B71670) |

<a id="grupo-1214"></a>

## Globales y helpers: source/game_sa/FileMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| createDirectory(const wchar_t* path) | [source/game_sa/FileMgr.cpp:39](../../source/game_sa/FileMgr.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1215"></a>

## Globales y helpers: source/game_sa/Fire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FireStartBurnTime(uint32 burnTime) | [source/game_sa/Fire.cpp:94](../../source/game_sa/Fire.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FireMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Fire.cpp:292](../../source/game_sa/Fire.cpp#L292) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FireDistanceZYX(const CVector& a, const CVector& b) | [source/game_sa/Fire.cpp:301](../../source/game_sa/Fire.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1216"></a>

## Globales y helpers: source/game_sa/FireManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FireDistSq3D(const CVector& point, const CVector& firePos) | [source/game_sa/FireManager.cpp:150](../../source/game_sa/FireManager.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampFireGenerations(int8 nGenerations) | [source/game_sa/FireManager.cpp:301](../../source/game_sa/FireManager.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gFireManager: source/game_sa/FireManager.h:81 (0x00B71F80) |
| FireRegisterCorona(uint32 id, uint8 red, uint8 green, uint8 blue, const CVector& pos, float coronaSize, eCoronaFlareType flare) | [source/game_sa/FireManager.cpp:395](../../source/game_sa/FireManager.cpp#L395) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1217"></a>

## Globales y helpers: source/game_sa/Font.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetScriptLetterSize(uint8 letterId) | [source/game_sa/Font.cpp:892](../../source/game_sa/Font.cpp#L892) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo IntroTextLines: source/game_sa/Scripts/TheScripts.h:388 (0x00A913E8); Posible dato fijo NumberOfIntroTextLinesThisFrame: source/game_sa/Scripts/TheScripts.h:389 (0x00A44B68); Posible dato fijo gFontData: source/game_sa/Font.cpp:34 (0x00C718B0) |

<a id="grupo-1218"></a>

## Globales y helpers: source/game_sa/Formation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CoverSubtract(float a, float b) | [source/game_sa/Formation.cpp:47](../../source/game_sa/Formation.cpp#L47) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CoverAdd(float a, float b) | [source/game_sa/Formation.cpp:56](../../source/game_sa/Formation.cpp#L56) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CoverCorner(const CMatrix* matrix, float x, float y) | [source/game_sa/Formation.cpp:65](../../source/game_sa/Formation.cpp#L65) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CoverDistance2D(const CVector& a, const CVector& b) | [source/game_sa/Formation.cpp:105](../../source/game_sa/Formation.cpp#L105) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CoverNormalise(CVector& value) | [source/game_sa/Formation.cpp:125](../../source/game_sa/Formation.cpp#L125) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CoverOffset(float position, float direction) | [source/game_sa/Formation.cpp:164](../../source/game_sa/Formation.cpp#L164) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GatherPoint(int32 index, int32 count, float heading, float radius, const CVector& pos) | [source/game_sa/Formation.cpp:221](../../source/game_sa/Formation.cpp#L221) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GatherCarFraction(int32 index, int32 count) | [source/game_sa/Formation.cpp:281](../../source/game_sa/Formation.cpp#L281) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GatherCarComponent(float position, float right, float lateralOffset, float forward, float forwardLength, float factor, bool positive, int32 component) | [source/game_sa/Formation.cpp:292](../../source/game_sa/Formation.cpp#L292) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FormationCentroid(const CVector* points, int32 count, bool destination) | [source/game_sa/Formation.cpp:355](../../source/game_sa/Formation.cpp#L355) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FormationMeanRadius(const CVector* points, int32 count, const CVector& center, FormationExtended* result) | [source/game_sa/Formation.cpp:411](../../source/game_sa/Formation.cpp#L411) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FormationRadiusScale(const FormationExtended& destination, const FormationExtended& ped, FormationExtended* result) | [source/game_sa/Formation.cpp:449](../../source/game_sa/Formation.cpp#L449) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 2 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FormationScalePosition(const CVector& point, const CVector& pedCenter, const CVector& destinationCenter, const FormationExtended& scale) | [source/game_sa/Formation.cpp:476](../../source/game_sa/Formation.cpp#L476) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| FormationVehicleIsStationary(const CVector& speed) | [source/game_sa/Formation.cpp:610](../../source/game_sa/Formation.cpp#L610) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1219"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Draw.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InlineStretchX(float a) | [source/game_sa/Frontend/MenuManager_Draw.cpp:78](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InlineStretchY(float a) | [source/game_sa/Frontend/MenuManager_Draw.cpp:79](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InlineScaleX(float at640, uint32 factorBits) | [source/game_sa/Frontend/MenuManager_Draw.cpp:83](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| InlineScaleY(float at448, uint32 factorBits) | [source/game_sa/Frontend/MenuManager_Draw.cpp:86](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1220"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Internal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsSaveSlot(eMenuEntryType slot) | [source/game_sa/Frontend/MenuManager_Internal.h:242](../../source/game_sa/Frontend/MenuManager_Internal.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
