# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1181"></a>

## Globales y helpers: source/game_sa/Events/EventPedCollisionWithPlayer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedCollisionWithPlayer | [source/game_sa/Events/EventPedCollisionWithPlayer.h:11](../../source/game_sa/Events/EventPedCollisionWithPlayer.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedCollisionWithPlayer.h:16](../../source/game_sa/Events/EventPedCollisionWithPlayer.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPedCollisionWithPlayer.h:17](../../source/game_sa/Events/EventPedCollisionWithPlayer.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1182"></a>

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

<a id="grupo-1183"></a>

## Globales y helpers: source/game_sa/Events/EventPedToChase.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedToChase | [source/game_sa/Events/EventPedToChase.h:8](../../source/game_sa/Events/EventPedToChase.h#L8) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedToChase.h:16](../../source/game_sa/Events/EventPedToChase.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPedToChase.h:17](../../source/game_sa/Events/EventPedToChase.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPedToChase.h:18](../../source/game_sa/Events/EventPedToChase.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPedToChase.h:19](../../source/game_sa/Events/EventPedToChase.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventPedToChase.h:20](../../source/game_sa/Events/EventPedToChase.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1184"></a>

## Globales y helpers: source/game_sa/Events/EventPedToFlee.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPedToFlee | [source/game_sa/Events/EventPedToFlee.h:6](../../source/game_sa/Events/EventPedToFlee.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPedToFlee.h:14](../../source/game_sa/Events/EventPedToFlee.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPedToFlee.h:15](../../source/game_sa/Events/EventPedToFlee.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPedToFlee.h:16](../../source/game_sa/Events/EventPedToFlee.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPedToFlee.h:17](../../source/game_sa/Events/EventPedToFlee.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventPedToFlee.h:18](../../source/game_sa/Events/EventPedToFlee.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1185"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCollisionWithPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCollisionWithPed | [source/game_sa/Events/EventPlayerCollisionWithPed.h:14](../../source/game_sa/Events/EventPlayerCollisionWithPed.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPlayerCollisionWithPed.h:19](../../source/game_sa/Events/EventPlayerCollisionWithPed.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPlayerCollisionWithPed.h:20](../../source/game_sa/Events/EventPlayerCollisionWithPed.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1186"></a>

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

<a id="grupo-1187"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroupAttack.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCommandToGroupAttack | [source/game_sa/Events/EventPlayerCommandToGroupAttack.h:9](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.h#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPlayerCommandToGroupAttack(CPed* target) | [source/game_sa/Events/EventPlayerCommandToGroupAttack.h:13](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPlayerCommandToGroupAttack.h:16](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPlayerCommandToGroupAttack.h:17](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPlayerCommandToGroupAttack.h:19](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1188"></a>

## Globales y helpers: source/game_sa/Events/EventPlayerCommandToGroupGather.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCommandToGroupGather | [source/game_sa/Events/EventPlayerCommandToGroupGather.h:7](../../source/game_sa/Events/EventPlayerCommandToGroupGather.h#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPlayerCommandToGroupGather(CPed* target) | [source/game_sa/Events/EventPlayerCommandToGroupGather.h:11](../../source/game_sa/Events/EventPlayerCommandToGroupGather.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPlayerCommandToGroupGather.h:14](../../source/game_sa/Events/EventPlayerCommandToGroupGather.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPlayerCommandToGroupGather.h:15](../../source/game_sa/Events/EventPlayerCommandToGroupGather.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPlayerCommandToGroupGather.h:16](../../source/game_sa/Events/EventPlayerCommandToGroupGather.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1189"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialGetRunOver.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialGetRunOver | [source/game_sa/Events/EventPotentialGetRunOver.h:7](../../source/game_sa/Events/EventPotentialGetRunOver.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPotentialGetRunOver.h:15](../../source/game_sa/Events/EventPotentialGetRunOver.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPotentialGetRunOver.h:16](../../source/game_sa/Events/EventPotentialGetRunOver.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPotentialGetRunOver.h:17](../../source/game_sa/Events/EventPotentialGetRunOver.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventPotentialGetRunOver.h:19](../../source/game_sa/Events/EventPotentialGetRunOver.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventPotentialGetRunOver.h:20](../../source/game_sa/Events/EventPotentialGetRunOver.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPotentialGetRunOver.h:21](../../source/game_sa/Events/EventPotentialGetRunOver.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1190"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoFire.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoFire | [source/game_sa/Events/EventPotentialWalkIntoFire.h:20](../../source/game_sa/Events/EventPotentialWalkIntoFire.h#L20) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPotentialWalkIntoFire.h:31](../../source/game_sa/Events/EventPotentialWalkIntoFire.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPotentialWalkIntoFire.h:32](../../source/game_sa/Events/EventPotentialWalkIntoFire.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPotentialWalkIntoFire.h:33](../../source/game_sa/Events/EventPotentialWalkIntoFire.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPotentialWalkIntoFire.h:35](../../source/game_sa/Events/EventPotentialWalkIntoFire.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1191"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoObject.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoObject | [source/game_sa/Events/EventPotentialWalkIntoObject.h:20](../../source/game_sa/Events/EventPotentialWalkIntoObject.h#L20) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPotentialWalkIntoObject.h:29](../../source/game_sa/Events/EventPotentialWalkIntoObject.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventPotentialWalkIntoObject.h:30](../../source/game_sa/Events/EventPotentialWalkIntoObject.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPotentialWalkIntoObject.h:31](../../source/game_sa/Events/EventPotentialWalkIntoObject.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPotentialWalkIntoObject.h:32](../../source/game_sa/Events/EventPotentialWalkIntoObject.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPotentialWalkIntoObject.h:33](../../source/game_sa/Events/EventPotentialWalkIntoObject.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1192"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoPed | [source/game_sa/Events/EventPotentialWalkIntoPed.h:9](../../source/game_sa/Events/EventPotentialWalkIntoPed.h#L9) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPotentialWalkIntoPed.h:19](../../source/game_sa/Events/EventPotentialWalkIntoPed.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPotentialWalkIntoPed.h:20](../../source/game_sa/Events/EventPotentialWalkIntoPed.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPotentialWalkIntoPed.h:21](../../source/game_sa/Events/EventPotentialWalkIntoPed.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventPotentialWalkIntoPed.h:23](../../source/game_sa/Events/EventPotentialWalkIntoPed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPotentialWalkIntoPed.h:25](../../source/game_sa/Events/EventPotentialWalkIntoPed.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor(CPed* ped, const CVector& targetPoint, eMoveState moveState) | [source/game_sa/Events/EventPotentialWalkIntoPed.h:31](../../source/game_sa/Events/EventPotentialWalkIntoPed.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1193"></a>

## Globales y helpers: source/game_sa/Events/EventPotentialWalkIntoVehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoVehicle | [source/game_sa/Events/EventPotentialWalkIntoVehicle.h:20](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.h#L20) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPotentialWalkIntoVehicle.h:29](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TakesPriorityOver(const CEvent& refEvent) override | [source/game_sa/Events/EventPotentialWalkIntoVehicle.h:30](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventPotentialWalkIntoVehicle.h:31](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPotentialWalkIntoVehicle.h:32](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventPotentialWalkIntoVehicle.h:33](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1194"></a>

## Globales y helpers: source/game_sa/Events/EventRevived.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventRevived | [source/game_sa/Events/EventRevived.h:8](../../source/game_sa/Events/EventRevived.h#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventRevived.h:13](../../source/game_sa/Events/EventRevived.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventRevived.h:14](../../source/game_sa/Events/EventRevived.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventRevived.h:15](../../source/game_sa/Events/EventRevived.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventRevived.h:16](../../source/game_sa/Events/EventRevived.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1195"></a>

## Globales y helpers: source/game_sa/Events/EventScriptCommand.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventScriptCommand | [source/game_sa/Events/EventScriptCommand.h:7](../../source/game_sa/Events/EventScriptCommand.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventScriptCommand.h:20](../../source/game_sa/Events/EventScriptCommand.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventScriptCommand.h:22](../../source/game_sa/Events/EventScriptCommand.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventScriptCommand.h:23](../../source/game_sa/Events/EventScriptCommand.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1196"></a>

## Globales y helpers: source/game_sa/Events/EventScriptedAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventScriptedAttractor | [source/game_sa/Events/EventScriptedAttractor.h:10](../../source/game_sa/Events/EventScriptedAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventScriptedAttractor.h:19](../../source/game_sa/Events/EventScriptedAttractor.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventScriptedAttractor.h:20](../../source/game_sa/Events/EventScriptedAttractor.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1197"></a>

## Globales y helpers: source/game_sa/Events/EventSeenCop.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventAcquaintancePed | [source/game_sa/Events/EventSeenCop.h:10](../../source/game_sa/Events/EventSeenCop.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventSeenCop.h:14](../../source/game_sa/Events/EventSeenCop.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventSeenCop.h:15](../../source/game_sa/Events/EventSeenCop.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AffectsPed(CPed* ped) override | [source/game_sa/Events/EventSeenCop.h:16](../../source/game_sa/Events/EventSeenCop.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventSeenCop.h:17](../../source/game_sa/Events/EventSeenCop.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1198"></a>

## Globales y helpers: source/game_sa/Events/EventSeenPanickedPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventSeenPanickedPed.h:6](../../source/game_sa/Events/EventSeenPanickedPed.h#L6) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventSeenPanickedPed.h:14](../../source/game_sa/Events/EventSeenPanickedPed.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventSeenPanickedPed.h:15](../../source/game_sa/Events/EventSeenPanickedPed.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventSeenPanickedPed.h:16](../../source/game_sa/Events/EventSeenPanickedPed.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventSeenPanickedPed.h:18](../../source/game_sa/Events/EventSeenPanickedPed.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventSeenPanickedPed.h:19](../../source/game_sa/Events/EventSeenPanickedPed.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1199"></a>

## Globales y helpers: source/game_sa/Events/EventSexyPed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventEditableResponse | [source/game_sa/Events/EventSexyPed.h:10](../../source/game_sa/Events/EventSexyPed.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventSexyPed.h:19](../../source/game_sa/Events/EventSexyPed.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventSexyPed.h:20](../../source/game_sa/Events/EventSexyPed.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventSexyPed.h:21](../../source/game_sa/Events/EventSexyPed.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventSexyPed.h:23](../../source/game_sa/Events/EventSexyPed.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventSexyPed.h:24](../../source/game_sa/Events/EventSexyPed.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1200"></a>

## Globales y helpers: source/game_sa/Events/EventSexyVehicle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSexyVehicle | [source/game_sa/Events/EventSexyVehicle.h:7](../../source/game_sa/Events/EventSexyVehicle.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventSexyVehicle.h:15](../../source/game_sa/Events/EventSexyVehicle.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventSexyVehicle.h:16](../../source/game_sa/Events/EventSexyVehicle.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventSexyVehicle.h:17](../../source/game_sa/Events/EventSexyVehicle.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventSexyVehicle.h:18](../../source/game_sa/Events/EventSexyVehicle.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
