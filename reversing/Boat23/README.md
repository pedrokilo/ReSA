<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Barcos (CBoat, lista 22)

Validadas el 03/10/2026 con **12.288 pares original/candidato dentro de GTA**, todos exactos: 23 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CBoat` | Constructor, Destructor, `SetModelIndex`, `SetupModelNodes`, `ProcessControl`, `ProcessControlInputs`, `PreRender`, `Render`, `Teleport`, `BlowUpCar`, `GetComponentWorldPosition`, `IsComponentPresent`, `ProcessOpenDoor`, `DebugCode`, `DisplayHandlingData`, `ModifyHandlingValue`, `PruneWakeTrail`, `AddWakePoint`, `FillBoatList`, `IsSectorAffectedByWake`, `IsVertexAffectedByWake`, `CheckForSkippingCalculations`, `GetBoatAtomicObjectCB` |

**Diferencias reales corregidas:**
- Globales: `apFrameWakeGeneratingBoats` y los contadores del wake eran copias de la DLL; ahora son las del EXE (0xC27994, 0xC279A4, 0xC279AC).
- Constructor:
  - índice del handling = byte bajo de `m_nHandlingId`;
  - puerta con eje Z (no Y) y ángulos pi/10 y 2pi/10 exactos;
  - rumbo bloqueado -9999,99 (0xC61C3FF6), no -10000.
- `ProcessControl`:
  - constantes del casco quemado 0x871248/0x871244;
  - nivel buscado leído de `FindPlayerPed()->m_pPlayerData` y con signo;
  - daño multiplicado por 1/6 y 1/12 (no dividido);
  - fuego 1 m más alto;
  - `m_BlowUpTimer` suma los ms truncados (`_ftol2`), no `timestep * 20`;
  - `m_bHasHitWall` leído antes de `CPhysical::ProcessControl`;
  - el Skimmer no mira el truco de barcos voladores y compara la velocidad² con 0,05 en double;
  - rumbo bloqueado: resta con el `fpatan` sin redondear y -9999,99 en todas partes.
- `ProcessControlInputs`:
  - suavizado del freno y 1/255 del EXE (no `std::lerp` ni `/255`);
  - lógica ratón/teclado del EXE (sin dirección y con ratón como última entrada no se toca nada);
  - `pow` del EXE (`_CIpow`) y cuadrado con signo con -0 → +0.
- `PreRender`:
  - escala de la hélice 32/2pi y 2pi exactos, alpha sin truncar a byte;
  - orden de las hélices fija/móvil por par;
  - `m_Scan += timestep * 0,02`;
  - Marquis: se copia la posición antes de `SetRotate` y se usa `Multiply3x3` con el orden del EXE;
  - átomo inicializado a nulo; con el sistema de partículas creado no se comprueba la matriz.
- `Render`:
  - el truco de coche invisible solo omite los dibujos (render states y vértices se escriben igual);
  - u/v solo para los modelos con zona;
  - Speeder con y = +0,06 y Reefer con el par trasero primero.
- `BlowUpCar`: flag 0x4000, átomo inicializado y reiniciado antes de la segunda búsqueda, sin objeto si el pool no da hueco, flotabilidad 0x3DDA740F.
- `PruneWakeTrail` no escribe el contador si los 32 puntos siguen vivos; `IsSectorAffectedByWake` termina en el primer hueco vacío; `CheckForSkippingCalculations` usa max(vértice, contador-1) y decrementa el contador.
- `FillBoatList`: pool del último slot al primero, dirección de cámara normalizada en 2D, distancias al cuadrado y 999999,875.
- `DebugCode` sigue solo si DPadLeft se acaba de pulsar; `AddWakePoint` trunca la intensidad a 64 bits; `DisplayHandlingData` con `sprintf`.

**Método:**
- Barco falso (búfer 0xCD de `sizeof(CBoat)` con offsets crudos del EXE), vtable de trampas (huecos 24 y 41 trazados) y llamada por la dirección del EXE o por el hueco de la vtable 0x8721A0.
- Constructor y destructor de `CVehicle` y constructor de `CObject` de la DLL localizados en el código de la DLL y simulados junto con los del EXE.
- Los stubs no reservan memoria ni escriben al log: guardan tipo y valores crudos en un array fijo y el json (con nombres de punteros) se construye al volver la función probada.
- Se simulan y trazan la base de `CVehicle`/`CPhysical`, FX, RW, Im3D, render states, audio, IA, mundo, cámara, explosiones, eventos, referencias, pads, jugador, rand, `CDoor::Process`, `ProcessBoatControl` y `FlyingControl` (cBuoyancy no se ejecuta).
- `FillBoatList` usa un pool falso de vehículos (puntero 0xB74494 cambiado solo durante la llamada).
- Se guardan y restauran reloj, trucos, ratón, pads, KeepWaterOut, lista de barcos, contadores del wake, model infos, contador de objetos temporales y el render state del motor.
- Fuera de los fixtures: nodos nulos donde el EXE no comprueba (`GetComponentWorldPosition`, hélices con partículas, objeto del truco invisible), `Teleport` sin matriz con reinicio de orientación, NaN.

Repetir: `python reversing/Boat23/build_probe.py` y `python reversing/run.py --case boat23`. Evidencia: [validation-summary.json](validation-summary.json).
