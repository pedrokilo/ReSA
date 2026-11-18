<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn209A: 29 funciones del EXE (lote 209A)

Estado: **APROBADO dentro del juego** (16/11/2026, worktree `wv-5` desde origin/main 0b394a15ba, sin cherry-pick; plantilla `Fn173A`, con `Fn171A` para el Clone V). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn209a/20261116-214910-302597` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn209A-20261116-214910-302597`). 29 x 16384 = **475136 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).

El lote ya estaba integrado en main con hooks y `validated=0` (`reversing/PendienteValidar/209A.md`). Solo faltaba el banco; el banco encontro **un defecto de la fuente**, corregido (ver abajo).

| # | Funcion | Entrada | Tipo |
|---|---|---|---|
| 0-6 | `std::fill`, `copy_backward`, `uninitialized_fill_n`, envoltorio de `uninitialized_copy`, `fill_n` que devuelve el final, dos vaciados de vector | 0x4F3AE0, 0x4F3B00, 0x4F3B30, 0x4F3CE0, 0x4F3D80, 0x4F3F30, 0x4F4030 | S |
| 7 | `vector<T*>::insert(pos, n, valor)` | 0x4F4320 | S |
| 8-13 | `std::string`: `_Tidy`, `_Grow`, `erase`, `assign(str,pos,n)`, `assign(char*,n)`, `string(const char*)` | 0x4F3BA0, 0x4F3BD0, 0x4F3DB0, 0x4F3E80, 0x4F3F60, 0x4F4170 | S |
| 14-21 | `std::runtime_error`: cuerpos del destructor (x2), `what`, destructores con borrado (x2), constructores (cadena y copia), `vector::_Xlen` | 0x4F3D10, 0x4F4130, 0x4F3D50, 0x4F3D60, 0x4F4110, 0x4F4090, 0x4F4230, 0x4F41B0 | S |
| 22, 23 | `CVehicle` bIsHandbrakeOn y alarma activa | 0x4F4CA0, 0x4F4CE0 | S |
| 24 | `CPatrolRoute` destructor con borrado | 0x66BA70 | S |
| 25-27 | `CTaskComplexFollowPatrolRoute` MakeAbortable, ControlSubTask, cuerpo del destructor | 0x66BA90, 0x66BB00, 0x6709F0 | S |
| 28 | `CTaskSimpleSlideToCoord::Clone` | 0x66D300 | V |

## Defecto encontrado y corregido
`SArrayTidyStride4` se enganchaba con **la misma funcion C++** en 0x4F3F30 y 0x4F4030 (y en otras nueve direcciones de 219A): RH no admite dos hooks hacia la misma funcion de la DLL y el cuerpo del segundo se resolvia a la entrada del EXE del primero (`body = 0x4F3F30` en el EXE para el hook de 0x4F4030). Se anaden instancias propias `SArrayTidyStride4Inst<1>` y `<2>` en `PedAttractor.cpp` que delegan en el mismo cuerpo. **Las otras nueve direcciones de 219A (0x5EC040, 0x5EC0B0, 0x5EC180, 0x5EC1F0, 0x5EC260, 0x5EC2D0, 0x5EC340, 0x5EC3B0, 0x5EC3E0) tienen el mismo defecto latente** (no son de este lote; el banco que las valide debe darles instancia propia).

## Defecto ajeno al lote
Con el main actual el juego muere al arrancar en `RwD3D9RasterDriver::RasterDestroy` (0x4CBB00, lote 224A sin validar) llamado con un raster nulo. El banco se corrio con `--unhook=Audio,Global/SurfaceInfos_c,Global/RwD3D9RasterDriver` (variable `FN209A_UNHOOK` de `run_local.py`).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; strings, vectores, runtime_error, tareas y vehiculos sinteticos. Colaboradores falseados con el mismo falso en la direccion del EXE y en el cuerpo de la DLL (o solo en la direccion del EXE si la DLL los llama por ella); `uninitialized_copy` 0x4F3940 corre real en insert. Las funciones que no vuelven (`_Xran`, `_Xlen`, `_CxxThrowException`, `vector::_Xlen`) terminan con una salida no local del falso (Gate restaura ESP/EBP y fs:[0]); se comprueba todo lo anterior al lanzamiento, no el desenrollado. Los valores de retorno `bool` se comparan solo en AL (todos los llamadores del EXE usan `test al,al`). Los punteros crudos nunca caen en la zona de la pila (la pila se etiqueta aparte). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn209A` (con los timeouts de `run_local.py` subidos a 9000 s).

## Prueba negativa
`mutate.py` (27 mutaciones por ronda, condicionadas a `this` o a un puntero de argumento desalineado a 4 bytes, que el juego real no produce). **Las 29 funciones fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261116-222106-084018` (20158 de 59392 pares con diferencias) y ronda 2 `20261116-221658-556053` (18667 de 59392); el lado EXE coincide con el oraculo en todos los pares. Incidencia: una primera ronda 1 tumbo el juego (una mutacion de `erase` producia una longitud negativa y llamaba al `_Xlen` real); se sustituyo por una mutacion no destructiva y se repitio. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
`Helpers65A` (0x4F3B30 y 0x4F4320: **repetir**), `Dependencies23A` (0x4F4CE0) y `RequestedValidation20261008` (0x4F4CA0 y 0x4F4CE0, segun el informe del lote) citan estas direcciones. El resto no aparece. El cambio de la correccion solo toca los dos hooks nuevos de vaciado: ningun banco anterior cita 0x4F3F30 ni 0x4F4030.
