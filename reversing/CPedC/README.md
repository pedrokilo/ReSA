<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
