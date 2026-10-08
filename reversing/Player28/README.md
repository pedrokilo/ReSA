<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Información del jugador (lista de 28)

Validadas el 04/10/2026 con **7.552 pares original/candidato dentro de GTA**, todos exactos: 24 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPlayerInfo` | Constructor, `AddHealth`, `ArrestPlayer`, `BlowUpRCBuggy`, `Clear`, `DeletePlayerSkin`, `EvaluateCarPosition`, `GetPos_Hook`, `GetSpeed_Hook`, `GivePlayerParachute`, `IsPlayerInRemoteMode`, `IsRestartingAfterArrest`, `IsRestartingAfterDeath`, `IsRestartingAfterMissionFailed`, `KillPlayer`, `Load`, `LoadPlayerSkin`, `MakePlayerSafe`, `PlayerFailedCriticalMission`, `Save`, `SetLastTargetVehicle`, `SetPlayerSkin`, `StreamParachuteWeapon`, `WorkOutEnergyFromHunger` |

**Diferencias reales corregidas:**
- `EvaluateCarPosition`: `LimitRadianAngle` va inlinado sin el recorte a ±25 (`> PI` resta 2PI, `< -PI` suma 2PI) y la división por 2PI es un producto por 1/(2PI) (0x8594F0).
- `StreamParachuteWeapon`:
  - fuera del modo cooperativo, con el argumento a `false` solo libera el paracaídas (la DLL procesaba el vehículo y pedía el modelo);
  - el paso de tiempo es `ms_fTimeStep * 0.02f * 1000.0f` truncado (la DLL dividía por 50);
  - una altura NaN deja el temporizador a 0 (`> 50` en vez de `<= 50`).
- `AddHealth`: `fild` + `fadd`, el entero se suma sin redondearlo antes a `float`.
- `BlowUpRCBuggy`: el causante es `CWorld::Players[PlayerInFocus].m_pPed` (`FindPlayerPed` inlinado).
- `MakePlayerSafe`:
  - mismo orden de escrituras y llamadas que el EXE (`GetPadFromPlayer` primero, `SetAllCarsCanBeDamaged` entre `RemoveAllProjectiles` y `ExtinguishAllCarFiresInArea`);
  - `m_bCanBeDamaged` se escribe en los `CPlayerPedData` del peatón (`m_pPed->m_pPlayerData`), no en los del `CPlayerInfo`;
  - `m_fTimeCanRun` solo se escribe si es negativo.
- `WorkOutEnergyFromHunger`:
  - la guarda del estático es el bit 0 del dword 0xB9B8F4 (la DLL usaba el byte entero);
  - `s_LastHungryState` es un byte sin signo y las horas sin comer se comparan con signo de 16 bits (`jle`/`jg`);
  - la segunda comparación de la hora usa el valor leído al principio;
  - un músculo NaN va a la rama de salud.
- `Clear`: solo escribe el byte bajo de `m_nCrosshairActivated` (0x158).
- `SetPlayerSkin`: `strncpy` de 32 bytes como el EXE (se quita el `strcpy_s` NOTSA).

**Método:**
- Los `CPlayerInfo` son búferes falsos de 0x190 bytes; peatones, vehículos, matrices, `CWanted`, `CPlayerPedData` del peatón, mando y texturas son falsos con offsets crudos.
- `CWorld::Players` y el jugador en foco se sustituyen por el fixture solo durante cada ejecución y se restauran.
- Se simulan y trazan los callees (estadísticas, textos, mensajes, ayuda, `CDarkel`, `CGangWars`, `CRemote`, cooperativo, streaming, suelo de la cámara, armas, voces, mando, tareas, fuego, explosiones, proyectiles, repeticiones, referencias, texturas de la piel y el búfer de guardado).
- `BlowUpCar` se traza por el slot 41 (0xA4) de la vtable falsa.
- El constructor corre sobre un búfer relleno con 0xCD y se compara entero.
- `Save`: se enmascara el relleno sin inicializar de la estructura de 0x28 bytes (+7, +0x11..0x13 y +0x25).
- Se guardan y restauran `CWorld::Players`, el foco, `CTimer`, el reloj, el truco del hambre, los estáticos del hambre, `CPad::Pads`, los menús, la pantalla ancha, la cinemática, `CGameLogic` y el estado de carga del paracaídas.

Repetir: `python reversing/Player28/build_probe.py` y `python reversing/run.py --case player28`. Evidencia: [validation-summary.json](validation-summary.json).
