<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# StoredCar64: CStoredCar::StoreCar (0x449760) y CStoredCar::RestoreCar (0x447E40)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wg-3`, HEAD df26d6fe7 + fuente del lote 64). 12288 pares (StoreCar 4096, RestoreCar 8192), cero diferencias EXE/DLL, todas las compuertas
de ABI/ruta/restauración en ambos lados, registros x87 muertos comparados (`kCompareDeadX87Registers=true`) y salida de cada lado igual a la del oráculo independiente (eventos de los falsos, cambios de la arena,
cambios del estado real y, en RestoreCar, el vehículo devuelto en EAX); cobertura mínima cumplida (`minimums.json`). Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-storedcar64\20261020-161900-597988` (corrida anterior idéntica: `...\20261020-155735-747399`).
Tras activar los hooks se repitieron `Garage63` y `GarageUpdate63` con la fuente de este lote: ambos APROBADOS.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CStoredCar::StoreCar(CVehicle*)` | 0x449760 | Simple (nuevo `CStoredCar::InjectHooks`) | 4096 | 1 |
| `CStoredCar::RestoreCar()` | 0x447E40 | Simple | 8192 | 15 |

## Qué hacen (desensamblado)

- **StoreCar** (thiscall, ret 4): copia modelo (+0x22), posición (matriz +0x30, o placement +4 si no hay matriz) como palabras, avance (matriz +0x10/+0x14/+0x18) × 100.0f [0x858628] truncado con `_ftol2` y guardado como
  byte (+0x3C..0x3E), colores, radio, banderas de física empaquetadas (+0x10, relleno +0x11 a 0), tipo de bomba (solo si el tipo del vehículo +0x590 es 0 o 9), 15 mejoras, remapeo (`CVehicle::GetRemapIndex` 0x6D0B70) y nitro.
- **RestoreCar** (thiscall, devuelve el vehículo): `RequestModel(modelo, 8)`, `RequestVehicleUpgrade(mejora, 0)` por mejora; si el byte de estado `0x8E4CC8 + 20*modelo + 8` no es 1 o alguna mejora no está cargada devuelve 0.
  Si lo está, fija `ms_compsToUse` (0x8A6458/9), reserva (`operator new` 0x6E2D50) y construye el vehículo según `CModelInfo +0x3C` (tabla 0x448300, tipo − 1 sin signo: 1→0x99C/0x6C8D60, 2→0x9BC/0x6CE370,
  3→0xA18/0x6C4190, 4→0xA04/0x6C8E20, 5→0x7E8/0x6F2940, 9→0x814/0x6BF430 y 10→0x838/0x6BF820 (ambos con `+0x614 |= 0x10`), 11→0x9F4/0x6D03A0; el resto, incluidos 0, 6..8 y fuera de rango: coche 0x988/0x6B0A90 con tres argumentos),
  coloca la posición (`fld/fstp`), el avance (byte con signo × 0.01f [0x858C58]) y su perpendicular (y, −x, 0) más las filas up (0, 0, 1), `+0x36 = (+0x36 & 7) | 0x20`, y copia banderas, colores, radio, mejoras y nitro;
  llama a `SetupUpgradesAfterLoad` (0x6E3400) y `SetRemap` (0x6D0C00, byte con signo).

## Fuente (Garage.cpp, X87Math.cpp/.h, Garage.h, InjectHooksMain.cpp)

Las dos funciones salen de `Garage.cpp` como C++ puro (campos por desplazamiento con `GaField`; colaboradores por dirección). `CStoredCar` gana `static void InjectHooks()` (`RH_ScopedClass(CStoredCar)`, dos `RH_ScopedInstall`) y
`InjectHooksMain.cpp` la llama tras `CGarage::InjectHooks()`. Ayudantes x87 nuevos (bloque «Lote 64» al final de X87Math.cpp/.h): `StoredCarPackAxisCpp` (`_ftol2` de `eje * 100.0f` sin guardar a float, con
`Ftol2RegsDblCpp`, que reproduce los registros x87 muertos de `_ftol2`) y `StoredCarUnpackForwardCpp` (`fild` del byte × 0.01f con un redondeo a 24 bits y siembra de los registros muertos T-4..T-1 = y', z', y', −x' con `X87Plant*`).
El `fld/fstp` de la posición se reproduce con `GaX87QuietFloatBits` (un NaN señalizador sale silenciado). Nada de `__asm`/`naked`. Cambio compartido: ninguno; solo se añade `CStoredCar::InjectHooks` (hook nuevo) y el bloque al final
de X87Math. `source.patch` contiene SOLO estos cambios respecto de HEAD (`git apply --check` verificado).

## Registros

Llamadores de StoreCar (0x4499C8, 0x449B18, 0x44A4BB): ninguno lee EAX/ECX/EDX. Llamadores de RestoreCar (0x44856A, 0x4485DB): solo leen EAX (el vehículo; se compara como ficha simbólica); ECX/EDX solo se registran
(`kCompareVolatileRegisters=false`).

## Banco

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con solo ese hook encendido. Los colaboradores son falsos idénticos en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL cuando tienen hook)
que registran EN ORDEN sus argumentos (punteros como fichas `S`, `V0/V1`, `M0/M1`) y devuelven lo que dicta el caso. Cada caso es una lista de escrituras previas sobre un relleno determinista de la arena (CStoredCar, 2 vehículos de
0xA18 bytes, 2 matrices, 4 CModelInfo falsos con guardas) más el estado real que lee/escribe RestoreCar (ms_compsToUse, byte de estado y puntero de CModelInfo de cuatro modelos de prueba 5000..5003), que se fija por caso y se restaura.
Se comparan: eventos, cambios de toda la arena (región `X` = cualquier cambio inesperado), cambios del estado real, EAX y las compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 con FCW/TOP/etiquetas y los 8 registros incluidos
los muertos; se ignoran las banderas de FSW, FIP/FDP y las del MXCSR), ruta del hook, entradas de los colaboradores intactas y restauración.
`fixture_oracle.py` modela las funciones desde el desensamblado con aritmética x87 a 24 bits en fracciones exactas; `python -I fixture_oracle.py` ejecuta la autoprueba.
Las fixtures cubren: NaN (silenciosos y señalizadores), infinitos, subnormales, −0, desbordes de `eje*100` (≥ 2^63), vecinos por ±1/±2 ulp de k/100, los 8 bits de banderas, tipos de bomba 0/9/otros, 0..11, 12, 0xFFFFFFFF y 100 como tipo
de vehículo, fallos de estado y de mejoras en cada posición, constructor que devuelve otro vehículo (V1) y mejoras negativas.

Defecto hallado por el banco: **uno**, en la fuente recién escrita (omitidos `+0x36`, y las filas up de la matriz (0, 0, 1) en RestoreCar); corregido en la primera iteración. Después 0 diferencias en 12288 pares.

## Suposiciones y límites (qué NO se validó)

- Todos los colaboradores son falsos: no se validan sus cuerpos ni la integración real con el streaming, el pool de vehículos ni los constructores.
- No se prueban los caminos en que el EXE desreferencia sin comprobar: matriz nula del vehículo en `StoreCar` (`fld [matriz+0x10]`), reserva nula de `operator new` en `RestoreCar` (el EXE se cae al leer `[vehiculo+0x14]`, o en
  `or [0+0x614]` en bicis) y matriz nula del vehículo nuevo. Por eso las ramas de posición en el placement (+4) de ambas funciones quedan sin cubrir (en el código están escritas igual que en el EXE).
- Solo se usan cuatro índices de modelo de prueba (5000..5003) con CModelInfo falsos; la lectura del tipo es `+0x3C` sin comprobar el puntero, como el EXE.
- FSW (banderas de excepción y C0..C3) no se compara (como en GarageUpdate63); sí FCW, TOP, etiquetas y todos los registros x87.
- Los modos de redondeo/precisión distintos de los del juego (24 bits, al más cercano) no se prueban.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh StoredCar64     # compila la sonda y corre en el juego (candado global)
python -I reversing/StoredCar64/fixture_oracle.py                                           # autoprueba del oráculo
python -B reversing/StoredCar64/run_local.py --analyze <comparisons.jsonl>
python reversing/StoredCar64/register_hook.py <carpeta-de-evidencia>                        # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```
Fila necesaria en docs/hooks.csv (CStoredCar no tiene filas): `CStoredCar,StoreCar,0x00449760,1,0,1,S` y `CStoredCar,RestoreCar,0x00447E40,1,0,1,S`.

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
