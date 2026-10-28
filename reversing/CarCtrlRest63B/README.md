<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlRest63B (tanda 2): SetUpDriverAndPassengersForVehicle, ScanForPedDanger, SetCoordsOfScriptCar y FindMaximumSpeedForThisCarInTraffic

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-4`, HEAD df26d6fe7 + fuentes del lote 63 parte C, tandas 1 y 2). 16384 pares (4 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados, `fp_difference` vacío (el ST0 de `FindMaximumSpeedForThisCarInTraffic` forma parte del estado x87 comparado), cobertura por familia (16 x 256 por función) y cobertura de ramas medida sobre los eventos de los colaboradores.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlrest63b\20261020-201801-699060`.

Primera corrida (20:15): 3 de 4 funciones sin diferencias y **`SetCoordsOfScriptCar` con 495 de 4096 diferencias**: ver «Defectos hallados».

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CCarCtrl::SetUpDriverAndPassengersForVehicle` | 0x4217C0 | Simple (`RH_ScopedInstall`) | 4096 | `CVehicle::SetUpDriver` 0x6D1A50, `SetupPassenger` 0x6D1AA0, `CPed::GiveObjectToPedToHold` 0x5E4390, `UpdateStatEnteringVehicle` 0x5E01A0, `CModelInfo::IsCarModel` 0x4C5AA0, `CAnimManager::GetAnimationBlockIndex(const char*)` 0x4D3990 (todos con hook) y `rand` del CRT del EXE 0x821B1E (sin hook); `GetAnimFileIndex` (virtual) mediante un vtable falso de 48 ranuras en un objeto de CBaseModelInfo falso |
| `CCarCtrl::SetCoordsOfScriptCar` | 0x4342A0 | Simple | 4096 | `CWorld::FindGroundZForCoord` 0x569660, `CEntity::GetDistanceFromCentreOfMassToBaseOfModel` 0x536BE0, `CStuckCarCheck::ClearStuckFlagForCar` 0x463C40, `CTheScripts::ClearSpaceForMissionEntity` 0x486B00, `CAutomobile::PlaceOnRoadProperly` 0x6AF420, `CBike::PlaceOnRoadProperly` 0x6BEEB0, `CCarCtrl::JoinCarWithRoadAccordingToMission` 0x432CB0 (todos con hook) y los virtuales `SetIsStatic` (ranura 4) y `Teleport` (ranura 14) del vtable del vehículo (falso) |
| `CCarCtrl::ScanForPedDanger` | 0x42CE40 | Simple | 4096 | `FindPlayerVehicle` 0x56E0D0, `CWorld::ClearScanCodes` 0x563470 (con hook) y `SlowCarDownForPedsSectorList` 0x425440 (envoltura `plugin::Call`, sin hook) |
| `CCarCtrl::FindMaximumSpeedForThisCarInTraffic` | 0x434400 | Simple | 4096 | `ClearScanCodes` 0x563470 (con hook) y `SlowCarDownFor{Cars,Peds,Objects}SectorList` 0x432420 / 0x425440 / 0x42D4F0 (envolturas `plugin::Call`, sin hook) |

```
bash reversing/_handoff/bench_here.sh CarCtrlRest63B           # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/CarCtrlRest63B/build_probe.py
python -B reversing/CarCtrlRest63B/run_local.py
python -B reversing/CarCtrlRest63B/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -B reversing/CarCtrlRest63B/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63b/20261020-201801-699060"   # sólo lo ejecuta el coordinador
```
Filas de `docs/hooks.csv` (las añade el coordinador; todas tipo S): `CCarCtrl,<función>,<dirección>,1,0,1,S` para las cuatro direcciones de la tabla. `SCOPE_NOTE` de `register_hook.py` ya está rellenado.
Archivo de arranque en el clon: `carctrlrest63b-runtime.start`; salida `carctrlrest63b-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de la tanda 2 respecto a la tanda 1; aplicar después de `reversing/CarCtrlRest63/source.patch`; `git apply --check` contra el resultado de la tanda 1 correcto)

- `CarCtrl.cpp`: las cuatro funciones en C++ (antes `plugin::Call*` hacia el EXE) y sus cuatro líneas de hook en `CCarCtrl::InjectHooks`.
- `X87Math.cpp/.h` (bloque «Lote 63: CCarCtrl (parte C, tanda 2)» al final, prefijo `Cc63b`; el archivo se compila `/O2 /Oi /arch:IA32 /fp:fast`): `Cc63bSectorCpp`, `Cc63bCruiseSpeedCpp`, `Cc63bTrafficSpeedCpp`,
  `Cc63bRandBelowEighthCpp` y `Cc63bAddFloatCpp`. El juego corre con MXCSR FTZ/DAZ (0x9F80): los productos y sumas que pueden dar o recibir denormales (`crucero * multiplicador`, `distancia + z`) van por x87, no por SSE.

## Revisión contra el EXE (re-desensamblado y contrastado instrucción a instrucción)

**SetUpDriverAndPassengersForVehicle (0x4217C0..0x4218F2).** `SetUpDriver(arg2, arg4, arg5)`; si `arg2` está en 14..23 (comparación con signo) y `rand() < 0x3FFF` (`jge` con signo; rand del EXE, 0x821B1E) el conductor `[veh+0x460]` (sin comprobar nulo)
recibe `GiveObjectToPedToHold(word [0x8CD72C], 1)`. Límite = `min(passengersNum, byte [veh+0x488])` (`jge` con signo); si `arg3 < límite`: `límite - arg3` intentos de `fild(rand) * 0x38000100 < 0.125f`
(`test ah,5/jp`: sólo menor, ordenado) que suman 1 cada uno y se acota a `límite`; si no, `límite`. `IsCarModel(movsx word [veh+0x22])`: si es verdadero, `GetAnimationBlockIndex("van")` (0x858EE0) **antes** del
`call [vtable+0x38]` (`GetAnimFileIndex`, sin comprobar el puntero de `ms_modelInfoPtrs[modelo]`) y, si son iguales y la cuenta >= 1, cuenta = 1. Después, para cada asiento `0..cuenta-1`: `SetupPassenger(asiento, arg2, arg4, arg5)`, si no es nulo
`UpdateStatEnteringVehicle` (0x5E01A0, llamada directa) y el mismo sorteo de gang con `GiveObjectToPedToHold(…, 1)`. Los bool de pila de los colaboradores llegan como dword con el byte bajo canónico (la DLL los normaliza; el EXE los reenvía tal cual).

**ScanForPedDanger (0x42CE40..0x42D0CD).** `FindPlayerVehicle(-1, 0)` primero; radio 44.0f (0x858FF8) si es el propio vehículo, 11.0f (0x858FBC) si no; bit 1 de `+0x3DB` (bHonkAtPed) guardado y restaurado al final (`xor/and 2/xor`);
caja `(x - r, y - r, r + x, r + y)` en float; sectores `ftol2(floor(v · 0.02f + 60.0f))` con `fst dword` intermedio: mínimo `<= 0 -> 0`, máximo `>= 0x77 -> 0x77` (cada uno sin acotar el otro lado); `AdvanceCurrentScanCode` en línea (`cmp word [0xB7CD78], 0xFFFF; jae -> 0x563470 + 1`);
bucle y exterior / x interior con índice `((y & 15) << 4) + (x & 15)` sobre 0xB992BC (lista de peatones = +4 del sector repetido de 12 bytes); `SlowCarDownForPedsSectorList(lista, veh, x1, y1, x2, y2, &velocidad, (float)byte[+0x3D0])`;
al final bit 13 de `+0x428` (bWarnedPeds) y el bit restaurado.

**SetCoordsOfScriptCar (0x4342A0..0x4343FC).** El manejador `CPools::GetVehicleRef` está inlinado: `índice = (veh - almacenamiento) / 0xA18` con signo (división por constante mágica; pool real en `[0xB74494]`, almacenamiento en `+0`, estados en `+4`), `manejador = estado[índice] + (índice << 8)`
sin comprobar que el puntero pertenezca al pool. `z <= -100.0f` (0x859014; con NaN no) -> `FindGroundZForCoord(x, y)`; `arg6` (byte bajo) -> `z += GetDistanceFromCentreOfMassToBaseOfModel()`; `SetIsStatic(false)` virtual (`call [vtable+0x10]`);
`ClearStuckFlagForCar` (this = 0xA90AB0); `Teleport(CVector(x, y, z), arg5)` virtual (`call [vtable+0x38]`, `arg5` empujado tal cual); si `+0x590 == 5` (barco): `ClearSpaceForMissionEntity(&pos, veh)` y `+0x3BB = 0`; si no,
automóvil/remolque (0 y 11) -> `CAutomobile::PlaceOnRoadProperly`, moto (9) -> `CBike::PlaceOnRoadProperly`, `ClearSpaceForMissionEntity`, `JoinCarWithRoadAccordingToMission` y `+0x3BB = 0`. `arg5` (`clearOrientation`) se reenvía; no hay otro uso.

**FindMaximumSpeedForThisCarInTraffic (0x434400..0x434783).** `+0x3B9` (estilo de conducción, byte) en {2, 3, 5} -> devuelve `byte[+0x3D0] · float[+0x3D4]` (x87, 24 bits). Si no: caja de ±14.0f (0x859030) con los mismos sectores y escaneo que arriba; `velocidad = crucero · multiplicador`;
para cada sector repetido: `SlowCarDownForCarsSectorList` (salvo estilo 6, que se relee en cada iteración), `...Peds` y `...Objects`, con `arg8 = crucero · multiplicador` **recalculado en cada llamada** (los falsos mutan el crucero y el multiplicador a mitad de recorrido
para comprobarlo) y `&velocidad` compartido; bWarnedPeds; estilos 0 y 4 devuelven `velocidad` y los demás `(crucero · multiplicador + velocidad) · 0.5f`.

Llamadores y registros de salida: los cuatro llamadores analizados (`residue_scan.py`) no leen EAX/ECX/EDX de salida de ninguna de las cuatro funciones; el ST0 de `FindMaximumSpeedForThisCarInTraffic` sí se compara.

## Defectos hallados

- **Producción (corregido):** la primera versión de `SetCoordsOfScriptCar` pasaba `arg5 != 0` (0/1) al `Teleport` virtual; el EXE empuja el dword de `arg5` tal cual (p. ej. 128 o 255) y el banco registra el byte bajo que recibe el colaborador (495 diferencias en las 16 familias). Ahora se
  pasa `std::bit_cast<bool>(arg5)` (el byte sin normalizar). Los llamadores reales empujan 0/1, pero la fuente reproduce el EXE para cualquier valor.
- Banco: ninguno en esta tanda.

## Suposiciones y riesgos

- `SlowCarDownFor{Cars,Peds,Objects}SectorList` son envolturas `plugin::Call` en este árbol; otro agente las reconstruye. Tras fusionar, `FindMaximumSpeedForThisCarInTraffic` y `ScanForPedDanger` las llamarán directamente en C++ (sin pasar por el EXE) y el banco, que sustituye la
  entrada del EXE, habrá que repetirlo (o ampliarlo con parches en el cuerpo de la DLL, que ya resuelve `DllBody`) para validar esa combinación.
- Las posiciones del vehículo se leen con `GetPosition()` (matriz nula -> +4, si no matriz+0x30); el señuelo en el otro sitio comprueba la elección.
- El pool de vehículos de `SetCoordsOfScriptCar` es falso (un `CPool` de 16 bytes con almacenamiento y tabla de estados en arenas, índice -8..100 y resto 0..0xA17); el cálculo del manejador no se prueba contra el pool real.
- `run_local.py` se calibró con la primera corrida y se fijó el `COVER` mínimo después de la corrida aprobada (el análisis del registro aprobado con ese `COVER` pasa: `python run_local.py --analyze <evidencia>/comparisons.jsonl`).
