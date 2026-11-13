<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10395 funciones reconstruidas; 10395 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn177A: 26 funciones diminutas del EXE (lote 177A)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-3` desde main 7406ace9cf + el lote 177A (commit f15b84ec32 de `reconstruccion-sin-validar`, sin validar hasta ahora); plantillas `Fn173A` y `Fn171A`, con `Surface47` para `surface.dat` en memoria). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn177a/20261112-161914-490775` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn177A-20261112-161914-490775`, `comparisons.jsonl.gz` de 62,3 MB). 26 x 16384 = **425984 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `cb717da963360c09c911a1dce39cb20bbb09d69d8fb14186ea696270a7abec4e`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo del lote (`reversing/PendienteValidar/177A.md` en el commit del lote) listaba 26 direcciones. Ninguna se salta. Las clases del listado eran en su mayoria incorrectas: los nombres reales salen del desensamblado, de las vtables y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CStats::GetFullFavoriteRadioStationList` | 0x558F90 | 6 | S |
| 1 | `SurfaceInfos_c::LoadAdhesiveLimits` (CStats) | 0x55D0E0 | 314 | S |
| 2 | `CEventCreatePartnerTask::Constructor` (CPedGeometryAnalyser) | 0x5F6190 | 108 | S |
| 3 | `CEventCreatePartnerTask` cuerpo del destructor | 0x5F6200 | 90 | S |
| 4 | `CEventCreatePartnerTask::Clone` | 0x5F6280 | 110 | V |
| 5 | `CEventCreatePartnerTask` destructor con borrado | 0x5F6300 | 30 | S |
| 6 | `CTaskSimpleDrown::GetTaskType` (213) (CTaskComplexDieInCar) | 0x62FF00 | 5 | V |
| 7 | `CTaskSimpleDrown` cuerpo del destructor | 0x62FF10 | 11 | S |
| 8 | `CTaskSimpleDrownInCar::Constructor` | 0x62FF20 | 18 | S |
| 9 | `CTaskSimpleDrownInCar::GetTaskType` (216) | 0x62FF40 | 6 | V |
| 10 | `CTaskSimpleDrownInCar::MakeAbortable` | 0x62FF50 | 5 | V |
| 11 | `CTaskSimpleDrownInCar` cuerpo del destructor | 0x62FF60 | 11 | S |
| 12 | `CTaskSimpleDrown` destructor con borrado | 0x6378D0 | 30 | S |
| 13 | `CTaskSimpleDrownInCar` destructor con borrado | 0x6378F0 | 30 | S |
| 14 | `CTaskComplexDie` destructor con borrado | 0x637910 | 30 | S |
| 15 | `CTaskSimpleDead` destructor con borrado | 0x637930 | 30 | S |
| 16 | `CTaskSimpleTired` destructor con borrado | 0x637950 | 30 | S |
| 17 | `CTaskSimpleTired::ProcessPed` (CTaskComplexDieInCar) | 0x637970 | 96 | V |
| 18 | `CTaskSimpleSitDown` destructor con borrado | 0x6379D0 | 30 | S |
| 19 | `CTaskComplexDragPedFromCar::Constructor` (...OpenPassengerDoor) | 0x640430 | 131 | S |
| 20 | `CTaskComplexDragPedFromCar::GetTaskType` (703) | 0x6404C0 | 6 | V |
| 21 | `CTaskComplexDragPedFromCar` cuerpo del destructor | 0x6404D0 | 90 | S |
| 22 | `CTaskComplexDragPedFromCar::Clone` | 0x643950 | 100 | V |
| 23 | `CTaskGoToVehicleAndLean::GetTaskType` (1216) | 0x660ED0 | 6 | V |
| 24 | `CTaskGoToVehicleAndLean::CalcTargetPos` | 0x664770 | 295 | S |
| 25 | `CTaskLeanOnVehicle` destructor con borrado (CTaskGoToVehicleAndLean) | 0x6648A0 | 30 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Las 26 filas de `docs/hooks.csv` quedan con validated=1 (sin `sync_docs`/`hooks2md`).

## Diferencias que encontro el banco (y que se corrigieron en la fuente)
Tres funciones del lote difirieron del EXE en el banco (primeras pruebas con 256 casos por funcion); se corrigio la fuente y el banco completo quedo en cero diferencias:
- **`CEventCreatePartnerTask::Constructor` (0x5F6190)**: el envoltorio llamaba al constructor generado por el compilador, que dejaba la vtable de la DLL en `[this]` (el EXE deja 0x86C6D0), expandia en linea el constructor de `CEvent` (sin pasar por el cuerpo enganchado 0x4ABFC0: la DLL no llamaba al colaborador) y ponia a cero `m_nTimeActive` y `m_bValid` por su cuenta. Ahora llama a `CEvent::Constructor()` (hook 0x4ABFC0; `friend` en `Event.h`), escribe el tipo, el BYTE bajo, la vtable 0x86C6D0, el companero, la distancia y registra la referencia; sin expansion en linea (lo llama `Clone`).
- **`CTaskComplexDragPedFromCar::Constructor` (0x640430)**: igual: el constructor generado expandia el de `CTaskComplexEnterCar` (la DLL escribia todos sus campos y no llamaba a 0x63A220) y dejaba la vtable de la DLL. Ahora es el miembro `Constructor(ped, tiempo)` (hook 0x640430) que llama al cuerpo enganchado `CTaskComplexEnterCar::Constructor` (0x63A220, ahora `noinline` y con `friend`), deja la vtable 0x86EB6C, el peaton con `RegisterReference` y el tiempo; `Clone` lo llama.
- **`CTaskGoToVehicleAndLean::CalcTargetPos` (0x664770)**: con matriz, el producto escalar se calculaba en float SSE: las restas se redondeaban a float (un delta fuera del rango del float daba infinito donde la x87 tiene exponente ampliado) y los denormales se aplanaban a cero (el caso del banco: `right.x` denormal, delta 1.0: el EXE da "adelante", la DLL "detras"). Ahora las tres restas, los productos y las sumas de ambas ramas (con matriz y por rumbo) van en dos ayudantes de `X87Math.cpp` (`Hp177ALeanSideAheadMatrixCpp` y `Hp177ALeanSideAheadHeadingCpp`, que reciben las posiciones y no los delta).
- Incidencia del propio banco: la primera corrida completa (425984 pares) dio cero diferencias EXE/DLL pero el oraculo discrepo en un solo par (un dword crudo aleatorio de la fixture caia dentro del rango de la pila y `Tag()` lo etiquetaba como puntero a la pila). `Tag()` ya no mira la pila; solo `TagStk()` (el vector local de `CMatrix * CVector`) lo hace. Se repitio la corrida completa con el banco corregido (la que se registra).
- `sscanf` de `LoadAdhesiveLimits` (CRT de la DLL frente al del EXE): sin diferencias con los textos de las fixtures; la funcion no se toco.

## Revision contra el EXE (`dumpbin /disasm` sobre el PE)
- **0**: `mov eax,0B78E58h; ret` (ECX y EDX intactos: el oraculo lo exige en ambos lados).
- **1**: `SetDir("")` (0x5387D0), `OpenFile("data\\surface.dat","rb")` (0x538900), `LoadLine` (0x536F80) en bucle y `CloseFile` (0x5389D0); sscanf real (0x8220AD); la fila N trae N + 1 valores, se escriben en `[N][j]` y `[j][N]` sin limite de filas; un valor que empieza por `-` vale 0.
- **2-5**: constructor (`ret 10h`, SEH que solo desenrolla): ctor de `CEvent` 0x4ABFC0, tipo, BYTE bajo del tercer argumento, vtable 0x86C6D0, companero, float como dword, `RegisterReference` 0x571B70 si no es nulo; cuerpo del destructor: vtable, `CleanUpOldReference` 0x571A00 y 0x4AC010; `Clone`: `operator new(0x1C)` 0x4B5620 y el constructor; destructor con borrado: cuerpo y `operator delete` 0x4B5630 con el bit 0.
- **6, 9, 20, 23**: `mov eax,imm; ret`. **10**: `xor al,al; ret 0Ch`.
- **7, 11**: `mov [ecx],vtable; jmp` al cuerpo base (0x62FB40 y 0x61A3A0). **8**: base 0x61A390 y vtable 0x86DE54.
- **12-16, 18, 25**: `push esi; mov esi,ecx; call cuerpo; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4` con los cuerpos 0x62FF10, 0x62FF60, 0x6300C0, 0x6305F0, 0x630F60, 0x631070 y 0x661030.
- **17**: sin animacion llama a `StartAnim` 0x630F70; sin arrancar devuelve 0; con +0x15 reinicia el inicio con el reloj 0xB7CB84; si el reloj (sin signo) >= inicio + duracion llama a la ranura 6 de la vtable con (ped, 0, 0) y devuelve 1.
- **19, 21, 22**: constructor (`ret 8`): base 0x63A220(0,0,0,1,0), vtable 0x86EB6C, peaton en +0x50 con `RegisterReference`, tiempo en +0x20; cuerpo del destructor: vtable, `CleanUpOldReference` y 0x63DFA0; `Clone`: `operator new(0x54)` 0x61A5A0 y el constructor (peaton, tiempo).
- **24**: x87 a 24 bits con exponente ampliado: tres restas que quedan en la pila, producto escalar `(rz*dz + ry*dy) + rx*dx` y `fcomp 0.0f`; derecha de la matriz del vehiculo o `(fcos, fsin, 0)` del rumbo; local = (max.x + 0.5f o min.x - 0.5f, 0, 0); `CMatrix * CVector` 0x59C890 con la matriz sin comprobar nulo; retorno oculto `ret 8`.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (ver `LiveComparison.hpp`). CalcTargetPos: `CMatrix * CVector` se falsea en la direccion del EXE (cdecl: resultado, matriz, vector) y, en la DLL, en el ayudante `Hp65MatrixTimesPointCpp` (cdecl: matriz, vector, resultado) que llama la funcion: mismo evento y mismo resultado; el hook de 0x59C890 queda apagado. LoadAdhesiveLimits: SetDir, OpenFile, LoadLine y CloseFile falsos (con las lineas de `surface.dat` del caso: comentarios, vacias, valores `-`, `x`, con sufijo, tabuladores, de 0 a 8 filas); sscanf real en ambos lados (fracciones binarias exactas y los valores reales de surface.dat). Tired::ProcessPed usa una vtable falsa cuya ranura 6 (MakeAbortable) anota sus argumentos. Memoria global tocada y restaurada: reloj 0xB7CB84, tabla de model infos 0xA9B0C8 y lista de emisoras 0xB78E58 (solo la marca de la prueba negativa). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 425984 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn177A`.

El oraculo modela la x87 de CalcTargetPos con aritmetica exacta (24 bits, exponente ampliado, NaN e infinitos); el seno y el coseno de la x87 se calculan con 80 digitos decimales y se redondean a 64 bits. Las fixtures de rumbo se limitan a |h| < 1024 (donde la reduccion de la x87 es exacta de sobra) y a los casos que el oraculo conoce exactos (cero con signo, denormales, infinitos, NaN y |h| >= 2^64, donde `fcos`/`fsin` dejan el operando intacto).

## Prueba negativa
`mutate.py` (26 mutaciones por ronda, condicionadas a `this` desalineado o, en la funcion sin `this`, a la marca 0x5A5A5A5A en la lista de emisoras). **Las 26 fallan en ambas rondas** (`negative-runs/`, 2048 casos por funcion): ronda 1 `20261112-164705-694871` (19496 de 53248 pares con diferencias) y ronda 2 `20261112-165044-861567` (9182 de 53248); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas). Las sondas mutadas se compilaron aparte y se ejecutaron en el mismo turno del candado del juego que la corrida principal.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE, peds, vehiculos, model infos y matrices reales, rumbos con 1024 <= |h| < 2^64 en CalcTargetPos, lineas de surface.dat sin espacio tras el nombre o con textos que difieran en el CRT del EXE y el de la DLL (`inf`, `nan`, hexadecimales), ficheros reales (CFileMgr), y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x55D0E0 -> `Surface47` (**repetir**: su `Init` ejecuta `LoadAdhesiveLimits`); 0x637930 -> `CTaskDead`; 0x637970 -> `Fn144A`; 0x640430 -> `EvHB`, `KillPedOnFoot62`; 0x6404D0 -> `Fn165A`; 0x664770 -> `VehicleLean355`, `Tasks355Evidence` (**repetir**: usan `CalcTargetPos`); por nombre de clase `CEventCreatePartnerTask` -> `EvHA`, `EvHB`, `GangEvent1`. Ademas, cambios de codigo compartido de este lote: constructor y `Clone` de `CEventCreatePartnerTask` y de `CTaskComplexDragPedFromCar` (ahora con vtable del EXE y colaboradores enganchados), `CTaskComplexEnterCar::Constructor` sin expansion en linea, `Event.h` (friend) y los ayudantes x87 de `CalcTargetPos` en `X87Math.cpp`. El resto de direcciones no aparece citado.
