<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 996 grupos / 12438 funciones reconstruidas; 11156 con evidencia histórica y 1282 pendientes de validación en 142 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn205A: 32 funciones del lote 205A (validacion del lote reconstruido sin validar)

Estado: **APROBADO dentro del juego** (15/11/2026, worktree `wv-6` desde main 9acc2209e0 + cherry-pick de ea6cb846d0 + correcciones de fuente impuestas por el banco; plantillas `Fn176A`, `Fn173A` y `Fn171A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn205a/20261115-200707-027400` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn205A-20261115-200707-027400`, `comparisons.jsonl.gz`, 88,5 MB). 32 x 16384 = **524288 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `b1917fc1feed6cb4fd7ae1f7a409fc3d7ed65ddf1024bfd9ac87be9f0c1b2f2c`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El listado del lote atribuia las 32 direcciones a tres clases (CEventDamage, CTaskComplexWanderGang, CPedAttractor); las clases reales salen de las vtables, de los llamadores y del desensamblado (ver `reversing/PendienteValidar/205A.md`).

| # | Funcion real | Entrada | Tipo | Colaboradores falseados (mismo falso en el EXE y en la DLL) |
|---|---|---|---|---|
| 0-9 | destructor con borrado de `CEventSexyVehicle`, `PedToChase`, `PedToFlee`, `Attractor`, `VehicleToSteal`, `AcquaintancePed`, `VehicleThreat`, `KnockOffBike`, `GunAimedAt`, `ScriptCommand` | 0x4B6170 + 0x50 k | S | cuerpo 4AF0A0, 4AF1C0, 4AF2D0, 4AF400, 4AF700, 4AF890, 4AFBF0, 4AFE70, 4B07B0, 4B0A50; el borrado del pool (0xB744AC) corre real sobre un pool sintetico |
| 10 | `CTaskComplexWanderGang` destructor con borrado | 0x66F650 | S | cuerpo 66F670, `CTask::operator delete` 61A5B0 |
| 11 | `CTaskComplexWanderGang` cuerpo del destructor | 0x66F670 | S | `~CTaskComplex` 61A3D0 |
| 12 | `CTaskComplexAvoidOtherPedWhileWandering` destructor con borrado | 0x66F680 | S | cuerpo 66A1D0, delete 61A5B0 |
| 13 | `CTaskComplexAvoidEntity` destructor con borrado | 0x66F6A0 | S | cuerpo 66AAE0 (sin hook en esta base: la DLL lo llama por su direccion del EXE), delete 61A5B0 |
| 14 | `CTaskComplexAvoidEntity::ComputeSphere` | 0x66F6C0 | S | `CColSphere::Set` 40FD10 |
| 15 | `CTaskComplexAvoidEntity::ComputeRouteRoundSphere` | 0x66FD30 | S | ComputeSphere 66F6C0 (funcion 14), `ComputeRouteRoundSphere` 5F1890 |
| 16 | `CPedShelterAttractor::IsAtHeadOfQueue` | 0x5E9680 | V | ninguno |
| 17 | `CPedAttractor::operator delete` | 0x5EAFE0 | S | el pool de atractores (0xB744C4) corre real sobre un pool sintetico |
| 18, 20, 23 | `std::copy_backward` de 4 bytes (instancias A, B, C) | 0x5EB140, 0x5EB1B0, 0x5EB260 | S | ninguno |
| 19, 22 | envoltorios de `copy_backward` de 20 y de 12 bytes | 0x5EB170, 0x5EB220 | S | 5EAC50 / 5EACC0 (sin hook en esta base: llamados por su direccion del EXE) |
| 21 | `std::fill` de 12 bytes | 0x5EB1E0 | S | ninguno |
| 24-28 | `std::uninitialized_fill_n` de 4 (A, B, C), 20 y 12 bytes | 0x5EB2A0, 0x5EB2E0, 0x5EB310, 0x5EB340, 0x5EB390 | S | ninguno |
| 29 | `RemoveEffect<CPedAttractor>` | 0x5EB5F0 | S | `AbortPedTasks` 5EAF60 |
| 30 | `IsPedRegistered<CPedAttractor>` | 0x5EB640 | S | `IsRegisteredWithPed` 5EB4C0 |
| 31 | `IsPedRegisteredWithEffect<CPedAttractor>` | 0x5EB690 | S | `IsRegisteredWithPed` 5EB4C0 |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Las 32 filas pasan a `validated = 1`.

## Aplicacion del lote sobre main
El commit del lote (ea6cb846d0) iba apilado sobre otros lotes sin validar (189A, 194A, 203A, 204A). Solo se conserva lo del 205A: los conflictos de `PedAttractor.cpp`, `TaskComplexAvoidEntity.cpp`, `TaskComplexWanderGang.cpp/.h` y `docs/hooks.csv` se resolvieron dejando las filas y el codigo del 205A. Los colaboradores que en esta base no tienen hook (0x66AAE0 del lote 203A; 0x5EAC50 y 0x5EACC0 del 189A) se llaman por su direccion del EXE; cuando esos lotes se integren, la llamada pasara a su funcion de la DLL. `CTaskComplexAvoidEntity::InjectHooks` se registra en `InjectHooksMain.cpp`.

## Funciones corregidas por el banco
- **`ComputeRouteRoundSphere` (0x66FD30)**: la DLL copiaba `center.z` como float y la x87 silenciaba un NaN senalizante; el EXE copia el dword. Ahora `memcpy` de 4 bytes.
- **`ComputeSphere` (0x66F6C0)**: la DLL conservaba las sumas parciales de posicion en un registro x87 con exponente ampliado; el EXE guarda cada suma como float (`fld; fadd; fstp`), de modo que un desbordamiento a infinito en una suma intermedia no se recupera (143 de 16384 pares con diferencias en la primera corrida completa, todos con valores enormes). Las sumas son ahora `volatile float`.
- **`CMakeLists.txt`**: `TaskComplexAvoidEntity.cpp` se compila con `/O2 /Ob0 /Oi /arch:IA32 /fp:fast` por archivo (x87 de 24 bits de `ComputeSphere`; `#pragma warning(push, 0)` alrededor de `StdInc.h` por NOMINMAX).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook. Memoria global tocada y restaurada: punteros a los pools 0xB744AC y 0xB744C4 y la tabla de model infos 0xA9B0C8 (indices -16..32767). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los casos por el oraculo. Particularidades:
- Las copias B y C identicas (0x5EB1B0, 0x5EB260, 0x5EB310, 0x5EB390) tienen como destino de hook la entrada de la copia anterior; su candidato se prueba con el hook de la copia A encendido tambien (`Env::chain`).
- `ComputeSphere`: oraculo con aritmetica x87 de 24 bits exacta (fracciones); cada caso lleva como mucho un NaN de usuario (dos NaN de cargas distintas en una operacion son ambiguos) y 16 peatones, matrices, model infos con indices negativos y mayores de 20000.
- `fill_n`: el destino nulo solo se salta la primera vuelta (despues avanza a 4, 8, ...), por eso solo se prueba con n <= 1; valores aliasados dentro del destino se leen en cada vuelta.
Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn205A` (la corrida principal tarda ~40 min; se uso un envoltorio propio con timeouts largos y una sola sesion de candado para la corrida principal y las dos rondas negativas).

## Prueba negativa
`mutate.py` (28 mutaciones por ronda que cubren las 32 funciones: las tres copias de `copy_backward` de 4 bytes y las tres de `fill_n` de 4 bytes comparten funcion de la DLL; condicionadas a un puntero desalineado). **Las 32 funciones fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261115-204646-109460` (14799 de 65536 pares con diferencias) y ronda 2 `20261115-205139-667104` (7409 de 65536); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE (eventos, tareas, atractores, peatones, model infos), pools reales, `n` de `fill_n` con el bit alto puesto (inviable: el destino nulo avanza y escribiria en la pagina cero), cuentas enormes de los vectores de atractores y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): ningun banco antiguo cita las 32 direcciones (solo `docs/BinaryCoverageAudit`, documentacion). Los cambios de codigo compartido: `CPedAttractor::operator delete` (ahora en linea con `PoolFreeSlotRaw`, antes `CPool::Delete`), que usan los bancos de atractores y de partida que borran atractores.
