<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPed grupo C: mirar, apuntar y animaciones (29 funciones)

Banco escrito pero **pendiente de compilar y ejecutar** (no hay evidencia de validación todavía; no existe `validation-summary.json`).

| Función | Dirección | Casos |
|---|---|---:|
| `CanUseTorsoWhenLooking` | 0x5DED90 | 256 |
| `ClearAimFlag` | 0x5DEF20 | 256 |
| `ClearAll` | 0x5E5320 | 512 |
| `ClearLook` | 0x5E3FF0 | 256 |
| `ClearLookFlag` | 0x5E1950 | 512 |
| `Look` | 0x5E7B20 | 1024 |
| `SetAimFlag-Entity` / `SetAimFlag-Heading` | 0x5DEED0 / 0x5E8830 | 256 / 512 |
| `SetLook-Entity` / `SetLook-Heading` | 0x5E7A60 / 0x5E79B0 | 512 / 512 |
| `SetLookFlag-Entity` / `SetLookFlag-Heading` | 0x5DEE40 / 0x5DEDC0 | 512 / 512 |
| `SetLookTimer` | 0x5DF8D0 | 256 |
| `TurnBody` | 0x5E4000 | 2048 |
| `ShoulderBoneRotation` | 0x5DF560 | 256 |
| `RestoreHeadingRate` / `RestoreHeadingRateCB` | 0x5DFD60 / 0x5DFD70 | 64 / 64 |
| `SetIdle` | 0x5E7980 | 256 |
| `StopNonPartialAnims` / `RestartNonPartialAnims` | 0x5DED10 / 0x5DED50 | 256 / 256 |
| `SetMoveAnim` (virtual, vtable 0x86C358, slot 23) | 0x5E4A00 | 1024 |
| `SetMoveAnimSpeed` / `SetMoveState` | 0x5DEC10 / 0x5DEC00 | 1024 / 64 |
| `GetWalkAnimSpeed` | 0x5E04B0 | 512 |
| `CalculateNewOrientation` | 0x5E52E0 | 256 |
| `WorkOutHeadingForMovingFirstPerson` | 0x5E1A00 | 1024 |
| `GetLocalDirection` | 0x5DEF60 | 2048 |
| `IsPlayingHandSignal` / `StopPlayingHandSignal` | 0x5E0460 / 0x5E0480 | 256 / 256 |

Total: 15.552 pares.

## Diferencias corregidas en la DLL (contra el EXE)

- `SetLookFlag(float)` y `SetLook(float)`: también limpian `bIsRestoringLook` (bit 3).
- `SetAimFlag(float)`: guarda el rumbo en `m_fLookDirection` (faltaba).
- `ClearLookFlag`: limpia `bShakeFist` (bit 11 de 0x470); la DLL limpiaba `bIsDrowning`.
- `ClearAll`: limpia `bKnockedUpIntoAir` y `bKnockedOffBike` (bits 4 y 31 de 0x470); la DLL limpiaba `bHitSteepSlope` y `bCrouchWhenScared`.
- `TurnBody`: reescrita. El rumbo limitado se corrige con ±2π según `m_fCurrentRotation` (esta no se corrige), se guarda en `m_fAimingRotation` y `m_fLookDirection`, el giro es `diferencia * 0,2f` (no `/ 5`) y la comparación usa `|dif| > 0,05f` con NaN como "alcanzado".
- `Look`: devuelve el `bool` de `TurnBody` (el EXE es un `jmp`); cambia `void Look()` a `bool Look()` en `Ped.h`.
- `SetMoveAnimSpeed`: orden `(semilla * 0,4f) * (1/32767)`.
- `SetMoveAnim`: con `m_nSwimmingMoveState == m_nMoveState` solo actúa con estado >= WALK (con signo) y pendiente fuera de [-0,01; 0,01]; el sprint con líder pide grupo y líder dos veces y compara `>= RUN` con signo.
- `StopNonPartialAnims` / `RestartNonPartialAnims`: recorren con primera/siguiente asociación (la primera comprueba que el clump tenga datos de animación).
- `GetLocalDirection`: transcripción x87 (fpatan completo, constantes π/4, 2π y 2/π del EXE, no 90°; solo reduce módulo 4 si el resultado es > 3).
- `GetWalkAnimSpeed`: frames con signo; sin traslación el EXE lee a +0x18 de un frame de 20 bytes (el siguiente al último).
- `WorkOutHeadingForMovingFirstPerson`: π/2 exacto 0x3FC90FDB (no `HALF_PI` de common.h).
- `ShoulderBoneRotation`: jerarquía con `GetAnimHierarchyFromSkinClump` (0x734A40) y matriz de hueso sin comprobar el rango (la DLL usaba la del frame y un assert).

## Método

- Un peatón falso (buffer del tamaño de `CPed` con patrón aleatorio; solo los punteros que el EXE desreferencia apuntan a fixtures falsos), entidades a mirar con o sin matriz, jugador, estadísticas, inteligencia con tareas falsas de vtable trazada (primaria, subtareas y secundarias), clump (tipo 2) con datos de animación, lista de asociaciones, jerarquía, secuencias y frames, y 6 matrices RW para los huesos. Nunca peatones reales ni el jugador.
- Se simulan y trazan en la dirección del EXE y en el símbolo de la DLL: `CEntity::RegisterReference/CleanUpOldReference`, `CAnimManager::BlendAnimation/GetAnimAssociation/UncompressAnimation`, `CPlaceable::SetOrientation`, `CPad::GetPedWalkLeftRight/UpDown`, `GetAnimHierarchyFromSkinClump`, `RpHAnimIDGetIndex`, `RpHAnimHierarchyGetMatrixArray`, `CPedGroups::IsInPlayersGroup/GetPedsGroup` y `CPedGroupMembership::GetLeader`.
- Se compara el retorno (floats por patrón de bits), el hash de todos los buffers y la traza. Se restauran el reloj y `CReplay::Mode`.

## No cubierto

- Entidad nula en `SetLook/SetLookFlag/SetAimFlag(Entity)`: el EXE registra la referencia con `this` nulo y se cuelga.
- Slot de arma > 12 en `SetAimFlag(float)` (el EXE usa el slot como byte con signo; la DLL, `uint8`) y valores de `m_nWeaponSkill` fuera de 0-3.
- Rutas con el jugador real (`IsPlayer` solo se prueba con tipos de peatón; `GetWeaponSkill` del jugador usa `CStats`).
- Rotaciones o ángulos que hagan que el bucle de `GetLocalDirection` no termine (±inf, muy negativos).
- Los matemáticos de `CMatrix` usados por `ShoulderBoneRotation` (`ConvertToEulerAngles`, `ConvertFromEulerAngles`, `Inverted`) no son transcripciones exactas del x87: si esa función difiere, el origen probable está ahí, no en `CPed`.

Repetir: `python reversing/CPedC/build_probe.py` y `python reversing/CPedC/run_local.py` (el caso `cpedc` hay que registrarlo en `reversing/cases.json` para `run.py`).
