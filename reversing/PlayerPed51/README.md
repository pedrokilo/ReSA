<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Peatón del jugador (lista de 34)

Banco preparado el 04/10/2026: **11.648 pares original/candidato dentro de GTA** sobre 50 de las 51 funciones de `CPlayerPed` (pendiente de compilar y ejecutar).

| Clase | Funciones |
|---|---|
| `CPlayerPed` | `AnnoyPlayerPed`, `Busted`, `CanIKReachThisTarget`, `CanPlayerStartMission`, `CheatWantedLevel`, `Clear3rdPersonMouseTarget`, `ClearAdrenaline`, `ClearWeaponTarget`, `DeactivatePlayerPed`, `DisbandPlayerGroup`, `DoStuffToGoOnFire`, `DoesPlayerWantNewWeapon`, `DoesTargetHaveToBeBroken`, `EvaluateTarget`, `FindPedToAttack`, `FindTargetPriority`, `ForceGroupToAlwaysFollow`, `ForceGroupToNeverFollow`, `GetButtonSprintResults`, `GetPadFromPlayer`, `GetPlayerInfoForThisPlayerPed`, `GetWantedLevel`, `GetWeaponRadiusOnScreen`, `HandlePlayerBreath`, `HandleSprintEnergy`, `IsHidden`, `LOSBlockedBetweenPeds`, `Load` (vtable 25), `MakeChangesForNewWeapon` (tipo y slot), `MakeGroupRespondToPlayerTakingDamage`, `MakePlayerGroupDisappear`, `MakePlayerGroupReappear`, `PedCanBeTargettedVehicleWise`, `PickWeaponAllowedFor2Player`, `PlayerHasJustAttackedSomeone`, `ProcessControl` (vtable 10), `ProcessPlayerWeapon`, `ReApplyMoveAnims`, `ReactivatePlayerPed`, `RemovePlayerPed`, `ResetPlayerBreath`, `ResetSprintEnergy`, `Save` (vtable 24), `SetMoveAnim` (vtable 23), `SetPlayerMoveBlendRatio`, `SetWantedLevel`, `SetWantedLevelNoDrop`, `TellGroupToStartFollowingPlayer`, `UpdateCameraWeaponModes` |

La vtable 0x86D168 tiene 26 entradas: `ProcessControl` en el slot 10, `SetMoveAnim` en el 23, `Save` en el 24 y `Load` en el 25. `CPed` engancha sus propias `SetMoveAnim`/`Save`/`Load` en su vtable (0x86C358) con funciones distintas: no hay doble enganche.

**Diferencias reales corregidas:**
- `ProcessControl` reescrita desde el EXE:
  - pone el bit 8 de 0x478 (la DLL ponía `bCheckColAboveHead`, bit 6) y sale si `CPed::ProcessControl` aplazó el control (`m_bWasPostponed`); la DLL usaba `bCanPointGunAtTarget` y seguía;
  - minigun: frena el giro también con tarea sin animación o fuera del bucle; el sonido con munición pide `tiempo >= inicio del bucle`;
  - objetivo peatón: cabeza solo si está vivo, con disparo instantáneo y dentro del rango; si no, torso (la DLL no ponía hueso fuera de rango);
  - objetivo vehículo: su salud × 0.001 colorea la marca; la marca y su color van dentro de la rama no aplazada;
  - distancia de la marca en orden x, z, y; colores con `_ftol2`;
  - mirada: `atan2` con `fpatan`, el desempate entre direcciones con NaN y 999999 comparado por bits; temporizador = `(uint32)(paso en ms) × 5`;
  - velocidad 2D (la DLL usaba 3D) y NaN reinicia el temporizador de quieto;
  - agua: los límites usan la z sin el 1.5 añadido y NaN calcula el porcentaje;
  - `FindDistanceToNearestMember` dos veces y aleatorios del EXE.
- `DisbandPlayerGroup`: `DISBAND_ONE` (la DLL decía `ATTACK_SINGLE`) y `RemoveAllFollowers` siempre.
- `MakePlayerGroupDisappear`: quita `bUsesCollision` (la DLL quitaba `bCollisionProcessed`).
- `TellGroupToStartFollowingPlayer`: evento local `CEventPlayerCommandToGroupAttack` y evento nuevo `CEventPlayerCommandToGroupGather` (vtables 0x86C710/0x86D120).
- `IsHidden`: solo fuera del vehículo.
- `DoesPlayerWantNewWeapon`: el jetpack no devuelve `false` (el EXE evalúa `bAimWithArm && !bAimWithArm`).
- `UpdateCameraWeaponModes`: el TEC9 también usa el modo M16.
- `GetWeaponRadiusOnScreen`: habilidad actual, cuerpo a cuerpo por tipo de disparo, `(0.5c + 1) × (rango × precisión)` y NaN a 0.2.
- `FindTargetPriority`: todas las comprobaciones (y llamadas) en el orden del EXE.
- `AnnoyPlayerPed`: comparaciones con signo.
- `HandleSprintEnergy`: `max` de 0x420800 (NaN se conserva) y `× 0.5`.
- `HandlePlayerBreath`: el recorte a 0 conserva NaN y -0; daño con `_ftol2`.
- `MakeChangesForNewWeapon`: habilidad actual, mínimo con signo del cargador (`movsx`) y `|= 9` en la animación de disparo (la DLL hacía `|= 0`).
- `LOSBlockedBetweenPeds`: el destino es el cuello o la posición de `entity2` (la DLL usaba `entity1`).
- `DoesTargetHaveToBeBroken`: multiplicador antes de la habilidad y distancia en orden z, y, x.
- `SetPlayerMoveBlendRatio`: solo recorta si es mayor que 2 (NaN se conserva).
- `FindPedToAttack`: recorre el pool del último al primero (la DLL tenía un bucle infinito), frente de `m_mCameraMatrix` (0xB6F9AC), distancia inicial 99999.9 y `× 0.2`.
- `EvaluateTarget`: distancia en orden z, y, x, NaN sale, `LimitRadianAngle` inlinado sin recorte y × 57.29578.
- `Load`/`Save`: solo el byte bajo del nivel de búsqueda y del arma elegida; ropa con `rep movs`.
- `RemovePlayerPed`, `DeactivatePlayerPed`, `ReactivatePlayerPed`, `GetPlayerInfoForThisPlayerPed`: `CWorld::Players[id]` directo.
- `GetPadFromPlayer`: sin `assert`, devuelve `nullptr` para otros tipos.

**Método:**
- El jugador es un búfer falso de 0x800 bytes: `CPlayerPedData`, `CWanted`, estadísticas, ropa, inteligencia, tareas, armas, vehículos, objetos, edificio, asociaciones de animación, clump (tipo 2) y modelo de colisión son falsos con offsets crudos.
- `CWorld::Players`, el foco, el grupo del jugador (`CPedGroups::ms_groups`), el pool de peatones (`FindPedToAttack`), la cámara, los mandos, el tiempo, los trucos y las globales tocadas se guardan y se restauran en cada ejecución.
- Callees simulados y trazados: estadísticas, `CWanted`, `CWorld`, luz, animaciones, habilidades, cámara, mirilla, referencias, grupos, eventos (`new`/`delete` del pool en un búfer fijo), tareas, mando, audio, `CMBlur`, agua, aleatorios, daño, búfer de guardado y `CPed::Load/Save/ProcessControl` (cuerpos de la DLL leídos de la vtable de `CPed`).
- Los punteros a la pila (eventos locales, referencias) se enmascaran como `stack`; en `Save` se enmascaran los bytes sin inicializar del EXE (+5..+7 y +0x81..+0x83).
- No se valida `SetupPlayerPed`: construye un `CPlayerPed` real (pools, audio, inteligencia y grupos).

Repetir: `python reversing/PlayerPed51/build_probe.py` y `python reversing/run.py --case playerped51`.
