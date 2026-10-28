<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
