<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEntryExit: TransitionFinished

> CEntryExit ya figuraba 11/11 y `TransitionFinished` ya estaba marcada `reversed=1` en `docs/hooks.csv` antes de esta tarea. Este informe aporta la comparación dentro del motor que faltaba, exigida por `MEJORAS_DEL_PROCESO.md` antes de confiar en ese indicador. No se encontraron diferencias de comportamiento en el alcance probado; no fue necesario corregir `source/game_sa/EntryExit.cpp`.
>
> **Ampliación (fase 2, misma fecha):** por pedido expreso del usuario de validar también la parte invasiva, se añadió una segunda fase que sí alcanza la transición `EXIT_ENTER_STATE_2` → `EXIT_ENTER_STATE_3` y ejecuta el bloque final completo (streaming real, teleporte real del jugador, población, scripts, `CGame::TidyUpMemory`) con el jugador real en la copia de pruebas. Ver la sección "Fase 2" más abajo. También 0 diferencias; tampoco hizo falta corregir código.

## Qué se comparó

`CEntryExit::TransitionFinished` (`0x004404A0`) es una máquina de estados (`EXIT_ENTER_STATE_0..3`) con dos ramas principales (`bFoodDateFlag || bUnknownPairing || ped->bInVehicle`, y su rama contraria) y una sección final enorme (streaming de escena, teleporte de peatón/vehículo, tienda, población, scripts, `CGame::TidyUpMemory`) que solo se alcanza cuando el estado pasa de `EXIT_ENTER_STATE_2` a `EXIT_ENTER_STATE_3` (`TheCamera.GetFading() == false`).

El banco fuerza `TheCamera.GetFading()` a observar siempre `true` durante la sonda (ver limitaciones), de modo que esa transición final y todo lo que depende de ella **nunca se alcanza**. El alcance cubierto son los cinco escenarios que devuelven `false` sin llegar a esa sección:

| Escenario | Rama | Estado inicial | Resultado esperado |
|---|---|---|---|
| 0 | `bFoodDateFlag`/`bUnknownPairing`/`ped->bInVehicle` | `EXIT_ENTER_STATE_0` | `false`; estado→2; evento de grupo si aplica; nombre de zona |
| 1 | igual que 0 | `EXIT_ENTER_STATE_2` (fading forzado `true`) | `false`; sin cambio de estado |
| 2 | rama contraria | `EXIT_ENTER_STATE_0` | `false`; estado→1; `CGame::currArea` = área del punto de aparición; `CEntryExitManager::SetAreaCodeForVisibleObjects()` real |
| 3 | rama contraria | `EXIT_ENTER_STATE_1`, sin tarea primaria | `false`; estado→2; `TheCamera.Fade`/`SetFadeColour` reales |
| 4 | rama contraria | `EXIT_ENTER_STATE_2` (fading forzado `true`) | `false`; sin cambio de estado |

Cada escenario se repite con 3 variantes de bandera disparadora (para los escenarios 0/1: `bFoodDateFlag`, `bUnknownPairing` o `ped->bInVehicle`, cada una por separado), 2 valores de `ms_bWarping`, 8 códigos de área (1..8, nunca `AREA_CODE_NORMAL_WORLD`/`NONE`) y 4 posiciones distintas del punto de salida: 5×3×2×8×4 = **960 pares originales/candidatos**, 192 por escenario.

## Qué se alterna y qué permanece real

El banco localiza el hook `CEntryExit`/`TransitionFinished` ya instalado y alterna `hook->State(false/true)` para llamar, en la misma dirección `0x4404A0`, primero al código máquina original real y después a la reconstrucción en C++, exactamente como `reversing/CTaskSimpleUseGun`. Un contador (`EnexProbe::calls`) inyectado dentro de `TransitionFinished` confirma que la llamada original nunca lo incrementa y la candidata lo incrementa exactamente una vez.

Dependencias reales alcanzadas sin controlar: `CPedGroups::GetPedsGroup` (siempre devuelve `nullptr` para el peatón sintético, nunca registrado en un grupo real), `CColStore::AddCollisionNeededAtPosn`, `CIplStore::AddIplsNeededAtPosn`, `CStreaming::AddModelsToRequestList`, `ThePaths.SetPathsNeededAtPosition`, `CRenderer::RequestObjectsInDirection` (vía `RequestObjectsInFrustum`) cuando `ms_bWarping` es verdadero, y `TheCamera.Fade`/`SetFadeColour` (solo fijan campos de estado de cámara/color de desvanecimiento, sin flujo de streaming ni audio).

Única dependencia con restauración explícita: `CEntryExitManager::SetAreaCodeForVisibleObjects()` (escenario 2) reasigna el área de las entidades reales que estén en el frustum de la cámara y compartan el valor controlado de `CGame::currArea`. El banco deja que la función real se ejecute (no se intercepta), pero reinicia `ms_numVisibleEntities` a 0 antes de cada llamada y, inmediatamente después, recorre `ms_visibleEntityList` y restaura `SetAreaCode(AREA_CODE_NORMAL_WORLD)` en cada entidad tocada — válido porque la propia condición de coincidencia de la función garantiza que ese era su valor antes de la llamada. En la ejecución aprobada se tocaron y restauraron **7 entidades reales** por cada una de las 192×2 = 384 llamadas (original+candidata) del escenario 2.

Globales reales guardados antes del banco y restaurados al final, byte a byte: `CEntryExitManager::ms_exitEnterState`, `CGame::currArea`, `CEntryExit::ms_spawnPoint`, `CEntryExit::ms_bWarping`, `CRenderer::m_loadingPriority`, `TheCamera.m_bFading`, `CEntryExitManager::ms_numVisibleEntities`, `CEntryExitManager::ms_oldAreaCode` y las 32 entradas de `ms_visibleEntityList`.

El peatón y el `CEntryExit` de cada pasada son estructuras sintéticas independientes (`Fixture`, dos instancias `a`/`b` por par, memoria propia, nunca en los pools reales). El peatón solo expone campos leídos en este alcance (`bInVehicle`, una `CPedIntelligence` vacía enlazada mediante un `using CPed::m_pIntelligence;` en una clase derivada, ya que el campo es `protected`) y permanece sin tareas (`m_aPrimaryTasks` en cero), por lo que `GetEntityThatThisPedIsHolding()` y `GetTaskManager().GetTaskPrimary(...)` siempre devuelven `nullptr` de forma segura.

## Resultado

**960/960 pares iguales**, cero diferencias. Se compararon: el valor de retorno, `CEntryExitManager::ms_exitEnterState` resultante, `CGame::currArea` resultante, `CRenderer::m_loadingPriority` resultante y el número de entidades tocadas por `SetAreaCodeForVisibleObjects`. Se exigió independencia de las dos estructuras sintéticas y bytes de entrada sin cambios (peatón y `CEntryExit` son de solo lectura en este alcance). El contador de llamadas candidata confirmó 0 durante el código original y 1 durante la reconstrucción, en los 960 pares.

No fue necesario modificar `source/game_sa/EntryExit.cpp`: la implementación existente coincide con el original en el alcance probado.

## Fase 2: bloque invasivo con el jugador real

Por pedido expreso del usuario ("validalo aunque sea invasivo"), se amplió la misma sonda (`LiveComparison.hpp`, tras restaurar todos los globales de la fase 1) para alcanzar de verdad la transición a `EXIT_ENTER_STATE_3` y el bloque final completo, usando el **jugador real** (`FindPlayerPed()`) en vez del peatón sintético — `ped->AsPlayer()->m_pEnex` solo es válido sobre un `CPlayerPed` auténtico.

Para minimizar el desplazamiento real, el `CEntryExit` sintético usado como destino de "salida" siempre apunta a `AREA_CODE_NORMAL_WORLD` y a la posición/orientación **actuales** del propio jugador: `Teleport()`/`SetHeading()` lo devuelven exactamente a donde ya estaba. La fase solo se ejecuta si el jugador está a pie, en el mundo normal, sin nada sostenido y sin `m_pEnex` previo; si no se cumple, se omite y se reporta explícitamente en vez de forzarse. En la ejecución aprobada la precondición se cumplió (`heavy_precondition.safe_to_run = true`).

Dos subcasos, alternando `hook->State(false/true)` igual que la fase 1:

- **Ligero (`bFoodDateFlag=true`), 8 pares**: alcanza el prefijo común de `EXIT_ENTER_STATE_3` (`SetAreaCode`, `m_pEnex`, `CEntryExitManager::AddEntryExitToStack`, `bCanExitCar`, el flag de `CPad`, `CClothes::RebuildPlayerIfNeeded`) y retorna antes de streaming/teleporte/scripts. Repetido por ser barato y seguro.
- **Completo (`bFoodDateFlag=false`), 1 par**: ejecuta el bloque entero de verdad — `CColStore`/`CIplStore`/`ThePaths`, streaming de escena (`CStreaming::LoadScene`/`LoadAllRequestedModels`), `ped->Teleport`, `CAudioZones::Update`, `CWaterLevel::FindNearestWaterAndItsFlow`, `CGarages::CloseHideOutGaragesBeforeSave`, `g_interiorMan`, `CPopulation::RemoveAllRandomPeds`/`GenerateAmbientPeds`/`ManageAllPopulation`, `CTheScripts::ClearSpaceForMissionEntity`/`Process`, `TheCamera.Fade(1.f, FADE_OUT)`, `CGame::TidyUpMemory` — una sola vez por lado, dado lo invasivo que es repetirlo.

Tras cada llamada se restauran, usando las rutas normales del motor (nunca escritura cruda de memoria): posición (`Teleport`), cabeceo (`SetHeading`), `m_fAimingRotation`, área (`SetAreaCode`), `m_pEnex`, `bCanExitCar`, el flag `CPad::bPlayerOnInteriorTransition`, la pila de entry/exits completa, y se cancela el `Fade(..., FADE_OUT)` recién iniciado. Subsistemas alcanzados que **no** se restauran ni se comparan bit a bit (quedan documentados como dependencias reales, no controladas): `CPopulation`, `CTheScripts`, `CGarages`, `CWaterLevel`, `CAudioZones`, `g_interiorMan`, el streaming.

**Resultado de la fase 2: 9/9 pares iguales** (8 ligeros + 1 completo), cero diferencias. Se comparó: retorno, área resultante, si quedó `m_pEnex`, `bCanExitCar`, el flag de `CPad`, la posición de la pila de entry/exits y el desplazamiento de posición del jugador (0 en todos los casos tras restaurar). El contador de llamadas confirmó 0/1 original/candidata en los 9 pares. Tras la fase, se verificó `player_restored = true`: posición, área, `m_pEnex`, `bInVehicle` y la pila de entry/exits del jugador real coinciden exactamente con los valores previos a la sonda. No fue necesario corregir código.

## Qué NO se validó (limitaciones explícitas)

- **`ped->GetEntityThatThisPedIsHolding()` devolviendo una entidad real**: ni el peatón sintético de la fase 1 ni el jugador real de la fase 2 (se exige explícitamente que no sostenga nada) ejercen la línea `entity->SetAreaCode(...)` al principio de la función.
- **`CPedGroups::GetPedsGroup(ped)` devolviendo un grupo real** (evento `CEventLeaderEntryExit` hacia la inteligencia del grupo): el peatón sintético nunca pertenece a un grupo registrado.
- **`GetEntryExitToDisplayNameOf()` devolviendo un `CEntryExit` no nulo** (y por tanto `CHud::SetZoneName`/`TheText.Get`): se mantuvo `m_szName` vacío en toda la sonda para evitar depender de claves de texto del juego.
- **La sub-rama de `EXIT_ENTER_STATE_1` donde la tarea primaria es `TASK_COMPLEX_GOTO_DOOR_AND_OPEN`**: solo se probó el caso sin tarea primaria (ambas rutas devuelven `false`, pero solo la ausencia de tarea se ejerció).
- **El valor exacto que `TheCamera.Fade`/`SetFadeColour` escriben** (duración, marca de tiempo de `CTimer::GetTimeInMS()`, color): no se comparó bit a bit porque la marca de tiempo real difiere entre la llamada original y la candidata, que ocurren con microsegundos de diferencia; solo se exige que el flujo de control llegue a la misma transición de estado.
- **Vehículos, guardado/carga, misiones o estabilidad general**: no se tocaron. La fase 2 sí movió al jugador real a pie dentro del bloque final de verdad, pero restauró su posición/área/estado inmediatamente después (ver sección "Fase 2"); las partidas se verificaron sin cambios antes y después de ambas fases.
- **`CPedGroups::GetPedsGroup(ped)` con el jugador en un grupo real, un peatón que el jugador sostenga, un `GetEntryExitToDisplayNameOf()` no nulo, o la rama de `EXIT_ENTER_STATE_1` con tarea `TASK_COMPLEX_GOTO_DOOR_AND_OPEN`, dentro del bloque de la fase 2**: la precondición de la fase 2 exige jugador sin nada sostenido y sin agrupamiento comprobado; estas combinaciones tampoco se ejercieron ahí.

## Repetir

Desde la raíz del repositorio:

```powershell
python reversing/CEntryExitTransition/build_probe.py
# copiar artifacts/gta_reversed-enextransition-probe.{asi,pdb} a
# "D:/JUEGOS/GTA San Andreas - CVehicle Test/_baseline/"
python reversing/run.py --case enextransition
```

Disponible también en `PROBAR.cmd --case enextransition` y en `--all`. [run_local.py](run_local.py) usa [runtime_runner.py](../runtime_runner.py): respalda/restaura `scripts/gta_reversed.{asi,pdb}` y ambos `gta-reversed.ini`, verifica hashes del ejecutable y de la sonda, y comprueba las partidas antes y después.

[build_probe.py](build_probe.py) instrumenta temporalmente `source/game_sa/EntryExit.cpp` (contador de llamadas, `Probe.hpp`) y `source/game_sa/Game.cpp` (bucle de comparación dentro de `CGame::Process()`, `LiveComparison.hpp`) y restaura ambas fuentes en `finally`, incluso si la compilación falla. Recompilar exige actualizar `PROBE_HASH` en `run_local.py` e instalar el nuevo binario de pruebas.

## Evidencia

**Fase 1 (sin fase 2, histórica):** log `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-enextransition/20261001-125022-681196/comparisons.jsonl` (SHA-256 `806c86bdcb96c59a1948136fbab27f6b1ac184ec503c9170c18a0882ae0c6519`); sonda de esa pasada SHA-256 `4d577377c2e2933ff41227b7aa8a15fa8e459813fbb9850d7212d9fe021b30d9` / PDB `0d3a4ffbb3c14169c623ca1e51f501752b2d34c001973bf469921fe39a433a17`.

**Fase 1+2 (pasada final, la que cuenta como evidencia vigente):**

- Log completo: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-enextransition/20261001-132428-655341/comparisons.jsonl` (SHA-256 `909b67706f6d2457e89f16e5421decc6e2f6371206a03a7ba0fc3540d173d734`).
- Resumen: [validation-summary.json](validation-summary.json) y `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-enextransition/20261001-132428-655341/summary.json` (`heavy_attempted: true`, `heavy_pairs: 9`, `heavy_player_restored: true`).
- Ejecutable de la copia de pruebas verificado: SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b` (coincide con el original analizado).
- Sonda archivada (fase 1+2): `artifacts/gta_reversed-enextransition-probe.asi` SHA-256 `48207c1f405c8448e41d28897a6bfbf44afd0e11f6257c3e09096a11acf7bbb3`; PDB SHA-256 `416fb6d0d7845300095f926c098ccab58751c91db94a133c5bfab825ccb5957a`. `PROBE_HASH` en `run_local.py` apunta a este hash.
- ASI normal instalado (sin instrumentación), solo en la copia de pruebas: SHA-256 `34801e37752db66f73e01c8b79e7f5c1cf1d27b7a7866116c3557699287e36e5`; PDB SHA-256 `bb28eda79b1e2d8d17ec56522852b48a4d353450e30cba6861fd55f80668825e` (sin cambios respecto a la fase 1; se reinstaló tras la fase 2).
- Respaldo del ASI/PDB anteriores a la fase 1: `D:/JUEGOS/GTA San Andreas - CVehicle Test/_baseline/before-enextransition-20261001-125342` (ASI SHA-256 `cfb7ccd82ed7b4a289574699fa3a736c195f346e6abde04cd19d9333482949b9`, PDB SHA-256 `876716f9244e88fa45b38442ba041e16412c3442049bf9fd5692c8e0cbab01d4`).
- Partidas (`GTASAsf2.b` en `userfiles` y en `Documents\GTA San Andreas User Files`) verificadas sin cambios antes/después de ambas pasadas. Instalación personal (`E:\JUEGOS\GTA San Andreas`) no se tocó en ningún momento.

El CSV y `docs/ReversedClasses.md` no cambiaron: `CEntryExit,TransitionFinished,0x004404A0,1,0,0,S` ya estaba así y sigue siéndolo; esta tarea solo aporta la comparación dentro del motor que faltaba para sostener ese indicador, dentro del alcance descrito arriba.
