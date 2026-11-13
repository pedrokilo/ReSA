<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 903 grupos / 10577 funciones reconstruidas; 10577 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn187A: 19 funciones del EXE (lote 187A)

Estado: **APROBADO dentro del juego** (13/11/2026, worktree `wv-5` desde main d88acb8e1d + el commit del lote 187A (cherry-pick de 82c5c4b808 de `reconstruccion-sin-validar`) + las correcciones de fuente de abajo; plantillas `Fn184A`, `Fn173A` y `Fn171A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn187a/20261113-033656-499994` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn187A-20261113-033656-499994`, `comparisons.jsonl.gz`). 19 x 16384 = **311296 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `ed5281a966b1c89a0e3767932d500f6d3dc0eadf9715c374ac9f6775d4042a93`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Detalle de las funciones y riesgos: `reversing/PendienteValidar/187A.md` (commit del lote). Las clases del listado eran en su mayoria incorrectas: los nombres reales salen del desensamblado y las vtables.

| # | Funcion real | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0-2 | `CEventAttractor::GetEventType`, `GetEventPriority`, `GetLifeTime` | 0x4AF3D0, 0x4AF3F0, 0x4AF3E0 | 6, 3, 3 | V |
| 3 | `CEventVehicleToSteal::Clone` | 0x4B74B0 | 97 | V |
| 4 | `CEventVehicleThreat::CloneEditable` | 0x4B7520 | 97 | V |
| 5 | `CEventKnockOffBike::Clone` | 0x4B7590 | 154 | V |
| 6 | `CEventGunAimedAt::CloneEditable` | 0x4B7630 | 97 | V |
| 7 | `CEventInAir::Clone` | 0x4B76A0 | 33 | V |
| 8 | `CPlayerPed` destructor con borrado | 0x60A9A0 | 30 | S |
| 9 | `CPlayerPed::CPlayerPed(playerId, groupCreated)` | 0x60D5B0 | 465 | S |
| 10 | funcion vacia `EmptyStub60F8C0` | 0x60F8C0 | 1 | S |
| 11 | `CTaskSimpleDuckWhileShotsWhizzing` constructor | 0x692680 | 85 | S |
| 12 | `GetTaskType` | 0x6926E0 | 6 | V |
| 13 | cuerpo del destructor | 0x6926F0 | 11 | S |
| 14 | `MakeAbortable` | 0x692700 | 167 | V |
| 15 | `Clone` | 0x692D60 | 99 | V |
| 16 | `CTaskSimpleShakeFist` destructor con borrado | 0x692DD0 | 30 | S |
| 17 | `CTaskSimpleDuckWhileShotsWhizzing` destructor con borrado | 0x693B80 | 30 | S |
| 18 | `ProcessPed` | 0x694640 | 68 | V |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Las 19 filas quedan con validated=1 (sin `sync_docs`/`hooks2md`).

## Correcciones de fuente hechas al validar
Todas tras leer el desensamblado del EXE y de la DLL (capstone sobre el ASI), antes de la corrida final (la corrida de 64 casos ya pasaba las funciones 0-8 sin diferencias):
- **`CEventInAir::Clone`**: el EXE construye en linea (+4 = 0, BYTE +8 = 0, vtable 0x85B3C0) y no llama al constructor; el constructor de C++ dejaba la vtable de la DLL. Ahora escribe esos tres campos.
- **`CPlayerPed::CPlayerPed`** y **`CTaskSimpleDuckWhileShotsWhizzing::CTaskSimpleDuckWhileShotsWhizzing`**: dejan la vtable del EXE (0x86D168 y 0x870BEC) como el original (antes la de la DLL).
- **`CTaskSimpleDuckWhileShotsWhizzing` cuerpo del destructor (0x6926F0)**: llama al envoltorio enganchado `CTaskSimpleDuck::Destructor` (0x692030) y no al destructor de C++ directo (no enganchable); `friend` en `TaskSimpleDuck.h` y las dos funciones de borrado pasan a ser miembros estaticos privados de la clase.
- **`CTaskSimpleDuckWhileShotsWhizzing::ProcessPed`**: el EXE llama a 0x4CEBC0 (`SetDeleteCallback`), no a 0x4CEBE0 (`SetFinishCallback`) como decia la nota del lote; corregido.
- **`CEventKnockOffBike::Clone`**: el EXE empuja el tipo de golpe con `movsx` (dword extendido con signo); la DLL lo empujaba con `movzx`. El parametro del envoltorio `Constructor` pasa a `int32` (solo cuenta el byte bajo) y `Clone` lo pasa extendido con signo.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook; el pool de eventos (`CEventPool::New` en linea en el EXE, 0x4B5570, frente a `CEvent::operator new` 0x4B5620 en la DLL) lleva un falso en cada sitio con el mismo evento; en el constructor de CPlayerPed el falso de `CPed::CPed` va en la direccion del EXE y en el destino de la llamada de la DLL, y los de `CPedGroups::AddGroup` y `CStats::GetFatAndMuscleModifier` en la funcion de la clase (el hook de la DLL es otra funcion). El constructor base de CTask (0x61A390) corre real. Memoria global tocada y restaurada: puntero del pool 0xB744AC, hora 0xB7CB84, datos de jugadores 0xB7CD9C (id 0..3), byte +0x2D0 de grupos 0xC09BF0 (indices 0..7) e indice 0xC0BC10, rellenados con un patron propio y comparados con `gchanges`. La salud del constructor pasa por un falso que devuelve el float en ST0 (sin NaN senalizadores). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 311296 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn187A` (con un envoltorio de una sola sesion del candado para la corrida completa y las rondas negativas).

## Prueba negativa
`mutate.py` (19 mutaciones por ronda, condicionadas a `this` desalineado; ronda 1 cualquier desalineo, ronda 2 el de 2 bytes; la de `EmptyStub60F8C0` es una funcion desnuda de prueba que escribe en [ECX]). **Las 19 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261113-035838-616229` con 12799 de 38912 pares con diferencias y ronda 2 con 4691 de 38912; el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y manifiesto principal regenerado.

## Fuera de los fixtures
`this` nulo o crudo, pool de eventos lleno o corrupto, objetos reales construidos por el EXE (jugador completo, tareas y animaciones reales), id de jugador fuera de 0..3, indices de grupo fuera de 0..7, argumentos booleanos con bits altos (el EXE los reenvia en bruto), NaN senalizadores como salud, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x60D5B0 -> `GeoFinal1`, `PoolsWave4` (comparacion en vivo del constructor de CPlayerPed: **repetir**); 0x60F8C0 -> `HookUnlockGame`; 0x6926E0 -> `HooksActivados`. El resto de direcciones no aparece citado. Conviene repetir ademas los bancos que usan `CEventKnockOffBike` (cambio de firma del envoltorio del constructor) y los que crean `CPlayerPed` o `CTaskSimpleDuckWhileShotsWhizzing` (vtable del EXE).
