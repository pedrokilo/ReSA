<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1161"></a>

## Globales y helpers: source/game_sa/Events/EventInteriorUseInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventInteriorUseInfo | [source/game_sa/Events/EventInteriorUseInfo.h:10](../../source/game_sa/Events/EventInteriorUseInfo.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventInteriorUseInfo.h:21](../../source/game_sa/Events/EventInteriorUseInfo.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventInteriorUseInfo.h:22](../../source/game_sa/Events/EventInteriorUseInfo.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventInteriorUseInfo.h:23](../../source/game_sa/Events/EventInteriorUseInfo.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventInteriorUseInfo.h:24](../../source/game_sa/Events/EventInteriorUseInfo.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1162"></a>

## Globales y helpers: source/game_sa/Events/EventKnockOffBike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KnockOffScale(const CVector& v, float k) | [source/game_sa/Events/EventKnockOffBike.cpp:167](../../source/game_sa/Events/EventKnockOffBike.cpp#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KnockOffCombine(const CVector& up, float upK, const CVector& side, float sideK) | [source/game_sa/Events/EventKnockOffBike.cpp:174](../../source/game_sa/Events/EventKnockOffBike.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1163"></a>

## Globales y helpers: source/game_sa/Events/EventKnockOffBike.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventKnockOffBike | [source/game_sa/Events/EventKnockOffBike.h:17](../../source/game_sa/Events/EventKnockOffBike.h#L17) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventKnockOffBike.h:37](../../source/game_sa/Events/EventKnockOffBike.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventKnockOffBike.h:38](../../source/game_sa/Events/EventKnockOffBike.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventKnockOffBike.h:39](../../source/game_sa/Events/EventKnockOffBike.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventKnockOffBike.h:40](../../source/game_sa/Events/EventKnockOffBike.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventKnockOffBike.h:41](../../source/game_sa/Events/EventKnockOffBike.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCriminalEvent() override | [source/game_sa/Events/EventKnockOffBike.h:43](../../source/game_sa/Events/EventKnockOffBike.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1164"></a>

## Globales y helpers: source/game_sa/Events/EventLeaderEnteredCarAsDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderEnteredCarAsDriver | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:11](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:21](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:22](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:23](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:24](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:26](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:27](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventLeaderEnteredCarAsDriver.h:28](../../source/game_sa/Events/EventLeaderEnteredCarAsDriver.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1165"></a>

## Globales y helpers: source/game_sa/Events/EventLeaderEntryExit.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderEntryExit | [source/game_sa/Events/EventLeaderEntryExit.h:11](../../source/game_sa/Events/EventLeaderEntryExit.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventLeaderEntryExit(CPed* ped) | [source/game_sa/Events/EventLeaderEntryExit.h:16](../../source/game_sa/Events/EventLeaderEntryExit.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventLeaderEntryExit.h:19](../../source/game_sa/Events/EventLeaderEntryExit.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventLeaderEntryExit.h:20](../../source/game_sa/Events/EventLeaderEntryExit.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventLeaderEntryExit.h:21](../../source/game_sa/Events/EventLeaderEntryExit.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPedGroup(CPedGroup* pedGroup) override | [source/game_sa/Events/EventLeaderEntryExit.h:22](../../source/game_sa/Events/EventLeaderEntryExit.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventLeaderEntryExit.h:23](../../source/game_sa/Events/EventLeaderEntryExit.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanBeInterruptedBySameEvent() override | [source/game_sa/Events/EventLeaderEntryExit.h:24](../../source/game_sa/Events/EventLeaderEntryExit.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1166"></a>

## Globales y helpers: source/game_sa/Events/EventLeaderExitedCarAsDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderExitedCarAsDriver | [source/game_sa/Events/EventLeaderExitedCarAsDriver.h:9](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventLeaderExitedCarAsDriver.h:16](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventLeaderExitedCarAsDriver.h:17](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventLeaderExitedCarAsDriver.h:18](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventLeaderExitedCarAsDriver.h:19](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventLeaderExitedCarAsDriver.h:21](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventLeaderExitedCarAsDriver.h:22](../../source/game_sa/Events/EventLeaderExitedCarAsDriver.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1167"></a>

## Globales y helpers: source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeaderQuitEnteringCarAsDriver | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h:11](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h:18](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h:19](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h:20](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h:21](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h:22](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h:24](../../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1168"></a>

## Globales y helpers: source/game_sa/Events/EventLeanOnVehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLeanOnVehicle | [source/game_sa/Events/EventLeanOnVehicle.h:8](../../source/game_sa/Events/EventLeanOnVehicle.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventLeanOnVehicle.h:17](../../source/game_sa/Events/EventLeanOnVehicle.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventLeanOnVehicle.h:18](../../source/game_sa/Events/EventLeanOnVehicle.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventLeanOnVehicle.h:19](../../source/game_sa/Events/EventLeanOnVehicle.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventLeanOnVehicle.h:20](../../source/game_sa/Events/EventLeanOnVehicle.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventLeanOnVehicle.h:21](../../source/game_sa/Events/EventLeanOnVehicle.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1169"></a>

## Globales y helpers: source/game_sa/Events/EventLowAngerAtPlayer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventLowAngerAtPlayer | [source/game_sa/Events/EventLowAngerAtPlayer.h:8](../../source/game_sa/Events/EventLowAngerAtPlayer.h#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventLowAngerAtPlayer.h:12](../../source/game_sa/Events/EventLowAngerAtPlayer.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventLowAngerAtPlayer.h:13](../../source/game_sa/Events/EventLowAngerAtPlayer.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventLowAngerAtPlayer.h:14](../../source/game_sa/Events/EventLowAngerAtPlayer.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventLowAngerAtPlayer.h:15](../../source/game_sa/Events/EventLowAngerAtPlayer.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventLowAngerAtPlayer.h:16](../../source/game_sa/Events/EventLowAngerAtPlayer.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1170"></a>

## Globales y helpers: source/game_sa/Events/EventNewGangMember.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventNewGangMember | [source/game_sa/Events/EventNewGangMember.h:7](../../source/game_sa/Events/EventNewGangMember.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventNewGangMember.h:17](../../source/game_sa/Events/EventNewGangMember.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventNewGangMember.h:18](../../source/game_sa/Events/EventNewGangMember.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventNewGangMember.h:19](../../source/game_sa/Events/EventNewGangMember.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventNewGangMember.h:20](../../source/game_sa/Events/EventNewGangMember.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventNewGangMember.h:21](../../source/game_sa/Events/EventNewGangMember.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1171"></a>

## Globales y helpers: source/game_sa/Events/EventObjectCollision.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventObjectCollision | [source/game_sa/Events/EventObjectCollision.h:14](../../source/game_sa/Events/EventObjectCollision.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventObjectCollision.h:29](../../source/game_sa/Events/EventObjectCollision.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventObjectCollision.h:30](../../source/game_sa/Events/EventObjectCollision.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventObjectCollision.h:31](../../source/game_sa/Events/EventObjectCollision.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventObjectCollision.h:32](../../source/game_sa/Events/EventObjectCollision.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventObjectCollision.h:33](../../source/game_sa/Events/EventObjectCollision.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1172"></a>

## Globales y helpers: source/game_sa/Events/EventOnFire.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventOnFire | [source/game_sa/Events/EventOnFire.h:7](../../source/game_sa/Events/EventOnFire.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventOnFire.h:12](../../source/game_sa/Events/EventOnFire.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventOnFire.h:13](../../source/game_sa/Events/EventOnFire.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventOnFire.h:14](../../source/game_sa/Events/EventOnFire.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventOnFire.h:15](../../source/game_sa/Events/EventOnFire.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventOnFire.h:17](../../source/game_sa/Events/EventOnFire.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1173"></a>

## Globales y helpers: source/game_sa/Events/EventPassObject.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPassObject | [source/game_sa/Events/EventPassObject.h:7](../../source/game_sa/Events/EventPassObject.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPassObject.h:16](../../source/game_sa/Events/EventPassObject.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPassObject.h:17](../../source/game_sa/Events/EventPassObject.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPassObject.h:18](../../source/game_sa/Events/EventPassObject.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPassObject.h:19](../../source/game_sa/Events/EventPassObject.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventPassObject.h:20](../../source/game_sa/Events/EventPassObject.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1174"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedCollisionWithPed | [source/game_sa/Events/EventPedCollisionWithPed.h:8](../../source/game_sa/Events/EventPedCollisionWithPed.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedCollisionWithPed.h:23](../../source/game_sa/Events/EventPedCollisionWithPed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPedCollisionWithPed.h:25](../../source/game_sa/Events/EventPedCollisionWithPed.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPedCollisionWithPed.h:26](../../source/game_sa/Events/EventPedCollisionWithPed.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPedCollisionWithPed.h:27](../../source/game_sa/Events/EventPedCollisionWithPed.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVictimMoveState() const | [source/game_sa/Events/EventPedCollisionWithPed.h:29](../../source/game_sa/Events/EventPedCollisionWithPed.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetMoveState() const | [source/game_sa/Events/EventPedCollisionWithPed.h:30](../../source/game_sa/Events/EventPedCollisionWithPed.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1175"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPlayer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedCollisionWithPlayer | [source/game_sa/Events/EventPedCollisionWithPlayer.h:11](../../source/game_sa/Events/EventPedCollisionWithPlayer.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedCollisionWithPlayer.h:16](../../source/game_sa/Events/EventPedCollisionWithPlayer.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPedCollisionWithPlayer.h:17](../../source/game_sa/Events/EventPedCollisionWithPlayer.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1176"></a>

## Globales y helpers: source/game_sa/Events/EventPedEnteredMyVehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedEnteredMyVehicle | [source/game_sa/Events/EventPedEnteredMyVehicle.h:7](../../source/game_sa/Events/EventPedEnteredMyVehicle.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedEnteredMyVehicle.h:12](../../source/game_sa/Events/EventPedEnteredMyVehicle.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPedEnteredMyVehicle.h:13](../../source/game_sa/Events/EventPedEnteredMyVehicle.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPedEnteredMyVehicle.h:14](../../source/game_sa/Events/EventPedEnteredMyVehicle.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventPedEnteredMyVehicle.h:16](../../source/game_sa/Events/EventPedEnteredMyVehicle.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventPedEnteredMyVehicle.h:17](../../source/game_sa/Events/EventPedEnteredMyVehicle.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPedEnteredMyVehicle.h:18](../../source/game_sa/Events/EventPedEnteredMyVehicle.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1177"></a>

## Globales y helpers: source/game_sa/Events/EventPedToChase.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedToChase | [source/game_sa/Events/EventPedToChase.h:8](../../source/game_sa/Events/EventPedToChase.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedToChase.h:16](../../source/game_sa/Events/EventPedToChase.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPedToChase.h:17](../../source/game_sa/Events/EventPedToChase.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPedToChase.h:18](../../source/game_sa/Events/EventPedToChase.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPedToChase.h:19](../../source/game_sa/Events/EventPedToChase.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventPedToChase.h:20](../../source/game_sa/Events/EventPedToChase.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1178"></a>

## Globales y helpers: source/game_sa/Events/EventPedToFlee.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedToFlee | [source/game_sa/Events/EventPedToFlee.h:6](../../source/game_sa/Events/EventPedToFlee.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedToFlee.h:14](../../source/game_sa/Events/EventPedToFlee.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPedToFlee.h:15](../../source/game_sa/Events/EventPedToFlee.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPedToFlee.h:16](../../source/game_sa/Events/EventPedToFlee.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPedToFlee.h:17](../../source/game_sa/Events/EventPedToFlee.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventPedToFlee.h:18](../../source/game_sa/Events/EventPedToFlee.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1179"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCollisionWithPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCollisionWithPed | [source/game_sa/Events/EventPlayerCollisionWithPed.h:14](../../source/game_sa/Events/EventPlayerCollisionWithPed.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPlayerCollisionWithPed.h:19](../../source/game_sa/Events/EventPlayerCollisionWithPed.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPlayerCollisionWithPed.h:20](../../source/game_sa/Events/EventPlayerCollisionWithPed.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1180"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroup.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCommandToGroup | [source/game_sa/Events/EventPlayerCommandToGroup.h:14](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPlayerCommandToGroup.h:22](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPlayerCommandToGroup.h:26](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPlayerCommandToGroup.h:27](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPlayerCommandToGroup.h:28](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventPlayerCommandToGroup.h:29](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventPlayerCommandToGroup.h:31](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventPlayerCommandToGroup.h:32](../../source/game_sa/Events/EventPlayerCommandToGroup.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
