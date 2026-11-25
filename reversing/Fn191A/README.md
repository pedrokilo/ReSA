<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn191A: 33 funciones del EXE (lote 191A)

Estado: **APROBADO dentro del juego** (15/11/2026, worktree `wv-5` desde origin/main 94bf1068ae + cherry-pick del commit d870fec83f del lote 191A + las tres correcciones del banco descritas abajo; plantilla `Fn173A`, con `Fn171A` y `Fn169A` para tareas y destructores). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn191a/20261115-180744-187721` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn191A-20261115-180744-187721`). 33 x 16384 = **540672 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `b859539c5d8b103c8b0d4ad04e812d53fc755986c4875eea893cd96229831096`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Las 31 direcciones reconstruidas del encargo mas `CTaskComplexFallAndStayDown::Clone` (0x67C270) y `CreateNextSubTask` (0x67CBE0), que el lote enganchaba para completar la clase: 33 filas de `docs/hooks.csv`. La direccion 0x619910 (`__usercall` con EAX/EDI/EBX) sigue saltada (no es reconstruible en C++ puro sin `naked`/`__asm`).

| # | Funcion | Entrada | Tipo |
|---|---|---|---|
| 0 | `CTaskComplexGotoDoorAndOpen::GetTaskType` | 0x66BB90 | V |
| 1 | `CTaskComplexGotoDoorAndOpen::MakeAbortable` | 0x66BC80 | V |
| 2 | funcion muerta `GoToEntryExitCenterTask` (tarea sin identificar con `CEntryExit*` en +0xC) | 0x66C280 | S |
| 3 | `CTaskSimpleSlideToCoord` constructor sin animacion | 0x66C3E0 | S |
| 4 | `CTaskSimpleSlideToCoord` constructor con animacion | 0x66C450 | S |
| 5 | `CTaskSimpleSlideToCoord` cuerpo del destructor | 0x66C4C0 | S |
| 6 | `RsCameraShowRaster` | 0x619440 | S |
| 7 | `RsSetPresetView` | 0x619780 | S |
| 8 | `RsDestroyPresetViews` | 0x6198C0 | S |
| 9 | `CPool<CEntryExit>` cuerpo del destructor (copia 1) | 0x43EEB0 | S |
| 10 | `CEntryExitManager::GetInSlot` | 0x43EF00 | S |
| 11 | `CEntryExitManager::SetEntryExitFlagWithName` | 0x43EF20 | S |
| 12 | `CEntryExitManager::SetEntryExitFlagWithIndex` | 0x43EF90 | S |
| 13 | `CPool<CEntryExit>` constructor | 0x43F720 | S |
| 14 | `CPool<CEntryExit>` cuerpo del destructor (copia 2) | 0x43F780 | S |
| 15 | `CPool<CEntryExit>::New` | 0x43F7D0 | S |
| 16 | `CPool<CEntryExit>` destructor con borrado | 0x43F950 | S |
| 17 | `CEntryExitManager::SetEnabledByName` | 0x43F9B0 | S |
| 18 | `CPad::IsDPadRightPressed` | 0x4410E0 | S |
| 19 | `CPhysical::SetVelocity` | 0x441130 | S |
| 20 | `CTaskComplexFallAndGetUp::GetTaskType` | 0x6786F0 | V |
| 21 | `CTaskComplexFallAndGetUp` cuerpo del destructor | 0x678790 | S |
| 22 | `CTaskComplexFallAndStayDown` constructor (animacion, grupo) | 0x6789D0 | S |
| 23 | `CTaskComplexFallAndStayDown::GetTaskType` | 0x678A00 | V |
| 24 | `CTaskComplexFallAndStayDown` constructor (direccion) | 0x678A10 | S |
| 25 | `CTaskComplexFallAndStayDown` cuerpo del destructor | 0x678A90 | S |
| 26 | `CTaskComplexFallAndStayDown::MakeAbortable` | 0x678AA0 | V |
| 27 | `CTaskComplexFallAndStayDown::CreateFirstSubTask` | 0x678B20 | V |
| 28 | `CTaskComplexFallAndStayDown::ControlSubTask` | 0x678BC0 | V |
| 29 | `CTaskComplexFallAndStayDown::CreateSubTask` | 0x678BD0 | S |
| 30 | `CTaskComplexFallAndStayDown` destructor con borrado | 0x67CBC0 | S |
| 31 | `CTaskComplexFallAndStayDown::Clone` (extra) | 0x67C270 | V |
| 32 | `CTaskComplexFallAndStayDown::CreateNextSubTask` (extra) | 0x67CBE0 | V |

## Diferencias que encontro el banco (corregidas en la fuente)
La primera corrida (33 x 16384 pares) dio diferencias en tres funciones; las tres eran defectos reales respecto del EXE o del fixture:
- **`CTaskSimpleSlideToCoord` constructor con animacion (0x66C450), 1765 pares**: el constructor de `CTaskSimpleRunNamedAnim` (0x61A990) del EXE escribe `m_pAnimHierarchy` (+0x3C) SIEMPRE tras las dos copias de cadena (nulo si `GetAnimationBlock` devuelve nulo), mientras la DLL solo la asignaba con bloque: una cadena de grupo de 0x14 o mas caracteres pisa +0x3C y la DLL lo dejaba con bytes de texto. Corregido en `TaskSimpleRunNamedAnim.cpp` (`else m_pAnimHierarchy = nullptr`).
- **`RsSetPresetView` (0x619780), 291 pares**: el EXE niega `rotX` con `fld; fchs; fstp`, y la x87 convierte un NaN senalizante en silencioso al cargarlo; la DLL cambiaba solo el signo con SSE y conservaba el NaN senalizante. Corregido con `NegateThroughX87` (bits) en `preset_view.cpp`.
- **`CTaskComplexGotoDoorAndOpen::MakeAbortable` (0x66BC80), 2194 pares**: fallo del fixture, no de la fuente: el MakeAbortable falso de la subtarea devolvia un AL distinto de 0/1 (el `bool` de la DLL lo normaliza a 1; las funciones reales solo devuelven 0 o 1). El falso devuelve ahora 0 o 1 con el resto de EAX en basura.
Ademas, al armar el banco se vio que `PoolDestructorBody` y `PoolDestructorBodyCopy` (0x43EEB0 y 0x43F780) enganchaban la MISMA funcion de la DLL y `HookRoute` no resuelve el cuerpo de la segunda ruta: la copia tiene ahora su funcion propia (`EntryExitPoolDestructorBodyCopyExe`, mismo cuerpo). Y `InjectHooksMain.cpp` no incluia `TaskComplexFallAndStayDown.h` tras el cherry-pick (anadido).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (`F`): CTask::operator new 0x61A5A0 y delete 0x61A5B0, CAnimManager::GetAnimationBlock 0x4D3940 y GetAnimation 0x4D42F0, SetFinishCallback 0x4CEBE0 (el DefaultAnimCB de la DLL se normaliza al del EXE), psCameraShowRaster 0x745240, CMemoryMgr::Free 0x72F430; funciones del propio lote que llaman otras (`Rt`): 0x43EF20 (desde SetEnabledByName), 0x678A90 (desde el destructor con borrado) y 0x678BD0 (desde CreateNextSubTask). Falso solo en la direccion del EXE (la DLL los llama por ella, `E`): operator new[] 0x821195, delete[] 0x8213AE y delete 0x8214BD del CRT y las seis funciones RW de RsSetPresetView (0x7F10B0, 0x7F1010, 0x7F0E30, 0x7F0910, 0x7EE1D0, 0x7EE2A0). Las llamadas virtuales a la subtarea (destructor con borrado, GetTaskType, MakeAbortable) van a una vtable falsa de la arena C compartida por ambos lados. Memoria global tocada y restaurada: puntero del pool 0x96A7D8, lista/contador/vista actual 0xC1707C / 0xC17080 / 0x8D2E30 y la marca 0xA9B0B0.

**Colaboradores reales**: las cadenas de constructores y destructores base (0x61A390, 0x61A3B0, 0x61A6C0, 0x61A990, 0x667CD0, 0x61A6F0, 0x61A3A0, 0x61A3D0) corren reales en el EXE (hook apagado, comprobado) y en la DLL son los constructores y destructores de C++ de las clases base: no se pueden falsear en la DLL (no llaman a los cuerpos enganchados), asi que se compara el objeto resultante byte a byte (estos colaboradores constan como `real`). **vtables**: la vtable que dejan los constructores, destructores y objetos nuevos (A+0, o la del objeto creado) es la del EXE en el lado original y una vtable de C++ de la DLL en el candidato por diseno (los hooks parchean la vtable del EXE); se omiten de los cambios de bytes y se comparan como clase (`vptrs`: "vt" si vale la del EXE esperada o una direccion dentro del ASI; cualquier otro valor se anota crudo y falla). El oraculo exige "vt" donde la funcion debe escribir una vtable.

16 familias de 1024 casos por funcion (posicion del objeto: alineado, desalineado, al borde, cerca del borde/inicio; relleno aleatorio/ceros/unos/0xCD), con NaN senalizantes/silenciosos, infinitos, denormales, cadenas de nombres con desbordamiento del objeto, capacidades de pool no positivas, prioridades y tipos de tarea de todas las ramas. `python -I fixture_selfcheck.py` pasa los 540672 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn191A` (se uso un envoltorio con timeouts de 9500 s).

## Prueba negativa
`mutate.py` (33 mutaciones por ronda, condicionadas a la marca 0x5A en 0xA9B0B0, que solo pone el banco). **Las 33 fallan en ambas rondas** (`negative-runs/`; 2048 casos por funcion): ronda 1 `20261115-194831-764160` con 53175 de 67584 pares con diferencias y ronda 2 `20261115-200158-884646` con 46521; en ninguna funcion hubo cero diferencias y el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y manifiesto de la sonda conservado (el de la corrida principal).

## Fuera de los fixtures
`this` nulo o crudo; `SetEntryExitFlagWithIndex` con slot libre (puntero nulo + 0x30: fallo de acceso identico en ambos); capacidades de pool enormes (el EXE escribiria fuera de memoria); `_strnicmp` con bytes no ASCII; cadenas de animacion que desborden el objeto de 0x80 bytes; listas de vistas preestablecidas truncadas (el EXE y la DLL leen +0xC de un nulo); objetos reales construidos por el EXE (vtables del EXE en la subtarea) y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros):
- 0x66C3E0 y 0x66C450: `Fn115A` (README), `Misc61` (`source.patch`). 0x619440: `AppBatch`, `GameIdle`, `Loading` y `HookUnlockGame` (bancos de partida que pasan por RsCameraShowRaster: **repetir**). 0x619780 y 0x619910 (saltada): `Helpers66A`; 0x6198C0: `PresetLoad74` (**repetir**: RsSetPresetView y RsDestroyPresetViews cambiaron).
- 0x43F720: `EnExInit` (README; el banco del constructor del pool se sustituye por este). 0x441130: `PlaneGeneration355` y `VehicleAudio355Evidence` (como colaborador de la generacion de aviones). 0x678790: `Fn154A` (banco del destructor con borrado de CTaskComplexFallAndGetUp, que sustituia el cuerpo 0x678790: ahora tiene hook: **repetir**). 0x678A00: `HooksActivados` (README).
- Sin citas en bancos: 0x66BB90, 0x66BC80, 0x66C280, 0x66C4C0, 0x43EEB0, 0x43EF00, 0x43EF20, 0x43EF90, 0x43F780, 0x43F7D0, 0x43F950, 0x43F9B0, 0x4410E0, 0x6786F0, 0x6789D0, 0x678A10, 0x678A90, 0x678AA0, 0x678B20, 0x678BC0, 0x678BD0, 0x67CBC0, 0x67C270, 0x67CBE0.
- La correccion de `CTaskSimpleRunNamedAnim` (0x61A990, siempre escribir `m_pAnimHierarchy`) toca codigo compartido: bancos que citan 0x61A990: `CTasks61c`, `FallenPeds` (**repetir**).
