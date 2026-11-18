<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11652 con evidencia histórica y 789 pendientes de validación en 38 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn200A: 31 funciones del EXE (lote 200A)

Estado: **APROBADO dentro del juego** (16/11/2026, worktree `wv-5` desde origin/main 23938b22f1 + cherry-pick del commit 8e4d6caa2e del lote 200A + las tres correcciones del banco descritas abajo; plantilla `Fn194A` y `Fn191A`, con `Fn173A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn200a/20261116-082915-030451` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn200A-20261116-082915-030451`). 31 x 16384 = **507904 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `ca2edd9681b48527fd9754945046101284bd19dc825af16829dae2005e990e97`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 32 direcciones: 31 funciones nuevas con hook (commit 8e4d6caa2e) y `CTaskComplexFallAndStayDown::Clone` (0x67C270), que ya estaba enganchada y se valido en el banco Fn191A. No se salta ni se excluye ninguna (31 filas de `docs/hooks.csv` pasan a validated=1).

| # | Funcion (el listado decia) | Entrada | Tipo |
|---|---|---|---|
| 0 | `CBoundingBox` alias del constructor (CCam) | 0x5B2AB0 | S |
| 1 | `CTaskComplexStuckInAir` cuerpo del destructor | 0x67BA70 | S |
| 2 | `CTaskSimpleGetUp::Clone` (CTaskComplexStuckInAir) | 0x67C0C0 | V |
| 3 | `CTaskComplexGetUpAndStandStill::Clone` | 0x67C130 | V |
| 4 | `CTaskSimpleFall::Clone` | 0x67C180 | V |
| 5 | `CTaskComplexFallAndGetUp::Clone` | 0x67C200 | V |
| 6 | `CTaskComplexInAirAndLand::Clone` | 0x67C2E0 | V |
| 7 | `CTaskSimpleInAir::Clone` | 0x67C360 | V |
| 8 | `CTaskSimpleLand::Clone` | 0x67C3F0 | V |
| 9 | `CTaskComplexFallToDeath::Clone` | 0x67C480 | V |
| 10 | `CTaskSimpleGetUp` destructor con borrado | 0x67C750 | S |
| 11 | `CTrain` destructor con borrado | 0x6F6300 | S |
| 12 | `CTrain::GetDooorAngleOpenRatioU32` | 0x6F5930 | V |
| 13 | `CTrain::GetDooorAngleOpenRatio` | 0x6F59C0 | V |
| 14 | `CTrain::IsRoomForPedToLeaveCar` | 0x6F5EE0 | V |
| 15 | `CTrain::OpenDoor` | 0x6F6370 | V |
| 16 | `CTrain::Teleport` | 0x6F7190 | V |
| 17 | `CVector::operator/=(float)` (CPlaceable) | 0x411A30 | S |
| 18 | `CColPoint::GetNormalAddress` (CPlaceable) | 0x411BA0 | S |
| 19 | `CColPoint::SetSurfaceTypeA` (CPlaceable) | 0x411BF0 | S |
| 20 | salto a `Flush` de las listas simples de punteros (CPlaceable) | 0x43E1A0 | S |
| 21 | `CMatrixLinkList` constructor (CPlaceable) | 0x54F440 | S |
| 22 | `CPlaceable` cuerpo del destructor | 0x54F490 | S |
| 23 | `CPlaceable` destructor con borrado | 0x54F5D0 | S |
| 24-30 | vaciado de un bloque de dos punteros (siete copias identicas, codigo muerto) | 0x54FA10, 0x54FA60, 0x54FAB0, 0x54FB00, 0x54FB50, 0x54FBA0, 0x54FBF0 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`).

## Diferencias que encontro el banco (corregidas en la fuente)
La corrida con todos los pares dio diferencias en una sola funcion, `CTrain::OpenDoor` (0x6F6370): 3888 de 16384 pares, por tres motivos que se solapan (las otras 30 funciones salieron limpias a la primera, salvo el aborto del fixture descrito al final). Dos eran defectos reales de la fuente respecto del EXE y se corrigieron:
- **Suma de la posicion guardada tras `SetRotate` (denormales), 621 pares**: el EXE hace `fld pos; fadd guardada; fstp pos` con la x87 (un denormal se conserva), y la DLL sumaba con el SSE del juego, que tiene FTZ activo en MXCSR y aplanaba a cero el denormal de la posicion guardada (0x00000001). Corregido con `TrainAddPosCpp` (X87Math, `float(double + double)` con la x87 de 24 bits): `TrainSetRotateAddPos` suma ya con la x87.
- **Angulo de la puerta con NaN senalizante, 196 pares**: el EXE hace `fld [angulo]; fstp [orientacion + eje * 4]` (la x87 silencia el NaN al cargarlo, bit 22) y la DLL copiaba el float tal cual. Corregido con `QuietThroughX87` en `Train.cpp`.
- **Fallo del fixture (no de la fuente), 3516 pares**: la DLL pasa a `RwFrameForAllObjects` su propio puntero de `CVehicleModelInfo::ClearAtomicFlagCB` / `SetAtomicFlagCB` (el EXE pasa 0x4C7BB0 / 0x4C7B90); el falso las normaliza a la direccion del EXE comparando con el puntero que da `&CVehicleModelInfo::ClearAtomicFlagCB` (la direccion de la funcion de la DLL no es observable por el juego).
Ademas, la primera corrida del banco aborto en `CTrain` (destructor con borrado) por el `assert` de `CPool::Delete` de la DLL contra un borrado doble o un indice fuera del pool; es una comprobacion NOTSA (el EXE no la tiene) que el juego real no alcanza (siempre borra un vehiculo vivo), asi que el fixture usa solo huecos usados dentro del pool (indices 0..63).

Tambien se ajusto la fusion con main: `docs/hooks.csv` (filas del lote anadidas al final, sin duplicados) y `X87Math.cpp`/`X87Math.h` (el marcador de conflicto dejaba sin cerrar la funcion `Pf194ClosestDistSqCpp` del lote 194A).

## Revision contra el EXE (`dumpbin /disasm` sobre el PE)
Ver `reversing/PendienteValidar/200A.md` (retirado en el commit de validacion) y `fixture_oracle.py`, que lleva el desensamblado de las 31 funciones. Puntos que condicionan el banco:
- **Clone (2-9)**: `CTask::operator new` (0x61A5A0, falso identico en el EXE y en la DLL) y, si da memoria, constructor base 0x61A390 / 0x61A3B0 (reales, con el hook apagado) y los campos copiados (leidos ANTES de reservar). `CTaskSimpleInAir` y `CTaskComplexFallToDeath` llaman a los constructores 0x678CD0 / 0x679040 (reales, con las banderas recompuestas desde los bits de +24h y +21h); `CTaskSimpleLand` expande el constructor en linea (los bits 3..7 de +10h conservan la memoria del pool). Las vtables que escriben los constructores se anotan como clase (la del EXE o una de la DLL).
- **Destructores con borrado (10, 11, 23)**: cuerpo falso identico en ambos lados (0x677F80, 0x6F54D0) o expandido (CPlaceable), liberacion con el bit 0 del BYTE de banderas; `CVehicle::operator delete` (0x6E2D90, sin hook) corre real en el EXE y en la DLL es `CPool::Delete`, ambos sobre un pool de vehiculos sintetico en las arenas (0xB74494 apunta a el; indice = diferencia / 0xA18 con la constante magica 0x6572EC3).
- **Puertas de CTrain (12-14)**: `GetDooorAngleOpenRatioU32` salta por la ranura 30 de una vtable falsa en la arena C (todas las demas ranuras caen en un falso distinto que el oraculo no espera); el resultado sale por ST0 (la x87 silencia un NaN senalizante al cargarlo). `GetDooorAngleOpenRatio` lee la puerta como dword sin comprobar el limite (la DLL como `eDoors` de 8 bits: el fixture solo usa codigos de 0 a 255).
- **OpenDoor (15)**: solo existen las puertas 2 y 3 (la funcion reescribe `componentId`); la cadena de `CMatrix` ligada al marco (0x59C050, 0x59BD10, 0x59BBB0, 0x59AD70, 0x59ACD0), `CDoor::IsClosed` (0x6F4800) y el constructor y destructor de `CEventSoundQuiet` (0x5E05B0, 0x5DEA00, 0x4AC450, 0x4AC480) corren reales con el hook apagado; son falsos identicos `CDoor::Open` (0x6F4790; deja el angulo del caso), `CMatrix::SetRotate` y `SetTranslate` (0x59B120, 0x59AF40), `RegisterReference` / `CleanUpOldReference` (0x571B70, 0x571A00), `GetEventGlobalGroup` y `CEventGroup::Add` (0x4ABA50, 0x4AB420, que anota los campos del evento) y, solo en la direccion del EXE, `RwFrameForAllObjects` (0x7F1200) y `RwMatrixUpdate` (0x7F18A0). El x87 de la rama del Streak (modelo 570) y la suma de la posicion guardada se comparan contra un modelo exacto de fracciones (24 bits, exponente ampliado); el ped es nulo o un objeto de la arena C (con o sin matriz).
- **Teleport (16)**: `CWorld::Remove` / `Add` y `SetOrientation` falsos; posicion a la matriz o a la compacta; velocidades a cero.
- **`CVector::operator/=` (17)**: `r = 1.0 / d` se queda en el registro x87 (24 bits, exponente ampliado) para las tres multiplicaciones: el oraculo lo modela con fracciones exactas (denormales, infinitos, NaN, divisor cero).
- **CColPoint (18, 19)**, salto a `Flush` (20), constructor de `CMatrixLinkList` (21), `CPlaceable` (22, 23) y los siete vaciados de bloque (24-30, `operator delete[]` 0x8213AE falso solo en la direccion del EXE; contador con signo, byte "es dueno").

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook y solo en la direccion del EXE de los que la DLL llama por ella (CRT y RenderWare). Memoria global tocada y restaurada: marca del banco 0xA9B0B0, reloj 0xB7CB84, contador de matrices 0xB74238 y puntero del pool de vehiculos 0xB74494 (comparada con `gchanges`). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 507904 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn200A` (con timeouts largos en `run_local.py`: 9000 s).

## Prueba negativa
`mutate.py` (25 mutaciones por ronda: las siete copias de `ArrayPairReset` comparten el cuerpo; todas condicionadas a la marca 0x5A en 0xA9B0B0, que solo pone el banco). **Las 31 funciones fallan en ambas rondas** (`negative-runs/`, 2048 casos por funcion): ronda 1 `20261116-090840-748869` (45322 de 63488 pares con diferencias; minimo 152/2048 en `GetDooorAngleOpenRatioU32`) y ronda 2 `20261116-091318-295253` (47642 de 63488; minimo 472/2048 en `OpenDoor`); el lado EXE coincide con el oraculo en todos los pares. Incidencia: la primera tanda de las dos rondas, lanzada en la misma sesion de candado que la corrida principal, no llego a ejecutar el juego (el registro de 1 GB de la corrida principal seguia retenido: WinError 32 al moverlo); se repitieron en otra sesion de candado borrando antes ese registro. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y manifiesto de la sonda regenerado con los hashes de las fuentes buenas.

## Fuera de los fixtures
`this` nulo o crudo, codigos de puerta distintos de 0..255 en `GetDooorAngleOpenRatio` y `OpenDoor` (la DLL usa `eDoors` de 8 bits), ejes de puerta distintos de 0..2 (el EXE escribiria fuera de la orientacion), `ped` crudo (distinto de nulo o de un objeto valido) en `OpenDoor`, borrados dobles y hueco fuera del pool en el destructor con borrado de `CTrain` (assert de la DLL), pools y vtables reales construidos por el EXE, la matriz y el evento reales del EXE con referencias reales (se falsean `RegisterReference` y `CleanUpOldReference`), objetos RenderWare reales, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x67BA70 -> `Fn128A` (banco del cuerpo del destructor de `CTaskComplexStuckInAir`, ahora con hook propio: **repetir**); 0x54F490 -> `CEntB` (README) y `Fn199A` (usa el cuerpo como colaborador: ahora tiene hook, **repetir**); 0x67C270 -> `Fn191A` (ya validada, sin cambios). Las demas direcciones no aparecen citadas (solo en la evidencia de auditoria de `Tasks355Evidence` / `VehicleAudio355Evidence` para 0x411A30 y 0x6F5EE0).
