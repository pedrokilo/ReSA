<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies21G (grupo G de BRIEF_21): GetFirstTexture, RpGeometryReplaceOldMaterialWithNewMaterial y CQuaternion::Slerp

PENDIENTE: banco preparado sin ejecutar dentro del juego. No constituye validacion hasta que `run_local.py` apruebe una corrida real. Las comprobaciones sin juego
descritas abajo (arnes aislado con los bytes reales del EXE) son evidencia previa, no sustituyen a la corrida.

## Funciones (las tres son del juego; ver classification en `gg-classification.md`)

| Funcion | Entrada | Convencion | Pares | Fuente |
| --- | --- | --- | --- | --- |
| RwHelper::GetFirstTexture | 0x734940 | cdecl, 1 argumento | 1024 | `rwtexdict.cpp` (hook en `RwHelper.cpp`) |
| RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial | 0x734DE0 | cdecl, 3 argumentos | 1536 | `RwHelper.cpp` |
| CQuaternion::Slerp (halftheta, sintheta_inv, t) | 0x59C300 | thiscall, `ret 0x14` | 4096 | `Quaternion.cpp` + `QuaternionSlerpCpp` en `X87Math.cpp` |

Total 6656 pares. El hook de cada una se apaga para el lado original (EXE) y se enciende para el candidato (DLL) con `OriginalDependencies499/HookRoute.hpp`.

## Que se compara en cada par (todo en bits; original == candidato)

- Memoria: el arena propio completo (con 16 bytes de guarda a cada lado). Los objetos son del banco (`Arenas.hpp`); no se toca ningun objeto del motor RenderWare ni del juego,
  asi que no hay estado global que restaurar salvo el x87/MXCSR y los bytes de las entradas parcheadas (vigilados con `Watch` tras cada llamada).
- Valor de retorno: EAX de GetFirstTexture; AL de ReplaceOld (el unico llamador, 0x6FDF81, ignora EAX y lo sobrescribe: `mov eax,edi`).
- ESP y EBX/ESI/EDI/EBP; semillas distintas en EAX/ECX/EDX antes de la llamada.
- x87 completo: FNSAVE de 108 bytes tras la llamada (TOP, FCW, FSW, FTW y los diez bytes de cada registro ST, incluidos los vacios) y MXCSR. Antes de cada llamada `PrimeX87()`
  deja en los ocho registros un residuo conocido, de modo que tambien se compara lo que cada lado deja o no en los registros muertos. `kCompareDeadX87Registers = true` (no se relaja).
  FIP se normaliza solo en Slerp (cada lado ejecuta su ultima instruccion x87 en otra direccion) y se comprueba aparte: la ultima instruccion es `fcomp m32` (rama theta == 0) o
  `fstp m32` (rama de calculo), en el EXE para el original y en el ASI para el candidato. En GetFirstTexture y ReplaceOld ninguna ruta ejecuta x87: FIP debe ser el del ultimo `fstp` de
  `PrimeX87` (en el ASI) y coincidir en ambos lados sin normalizar.
- ECX/EDX al volver: se registran (`original_residue`/`candidate_residue`) pero no se exigen: ningun llamador del EXE los lee (evidencia en `callers.dis.txt`, 11 llamadas
  desensambladas y seguidas por todas las ramas con `offline/resread.py`). En GetFirstTexture solo se lee EAX (el resultado); en ReplaceOld y Slerp no se lee ninguno.

### Colaboradores de ReplaceOld
`_rpMaterialListFindMaterialIndex` (0x74E420) y `RpMaterialDestroy` (0x74DA20) son de RenderWare. Se sustituyen en AMBOS lados por el mismo par de stubs trazados con `EntryPatch` en la
direccion del EXE y en la entrada del envoltorio de la DLL (resuelta con `Resolve()`, que salta thunks E9). El stub de FindIndex repite el recorrido hacia atras del EXE (`jle -> dec eax` para
`numMaterials <= 0`); el de Destroy resta una referencia sin liberar (el camino `refCount == 1` libera memoria del motor y no se ejecuta). La traza (tipo, argumentos como desplazamientos
en el arena) forma parte de la comparacion. Asi se prueba el orden y los argumentos de las llamadas, incluido el `materials[-1]` que se escribe cuando el material viejo no esta en la lista.

## Fixtures (Fixtures.hpp; fixture_oracle.py las repite)

- **Slerp (4096 = 32 x 16 x 8)**: theta (+-0, subnormales, minimo normal, pi/2-1ulp, pi/2, pi/2+1ulp, pi, 2pi, 100, +-FLT_MAX, +-inf, NaN con carga distinta y senalizantes de ambos signos,
  +-1e30, 1e-30...), t (0, 1, 0.5, negativos, >1, NaN, senalizante, +-inf, +-0, subnormal, +-1e30, FLT_MAX, 1-ulp), 1/sin (1, 0.25, +-0, NaN, inf, 1e30, subnormal) y 16 pares de
  cuaterniones (identidad/90 grados, normales, familia NaN con signos opuestos y misma carga, NaN x subnormal, subnormales, +-0, +-inf, +-FLT_MAX, +-1e30, enteros extremos como float,
  mezclas); destino propio, igual a `from` o igual a `to`. 256 casos con theta = +-0 (rama de copia), 512 con theta NaN, 256 con +-inf, 256 con theta subnormal.
- **GetFirstTexture (1024)**: 0..8 texturas, diccionario con cuatro desplazamientos, seis permutaciones de las ranuras, cinco rellenos de basura y enlaces `prev` coherentes o corruptos.
- **ReplaceOld (1536)**: 0..8 mallas, 0..6 materiales, material viejo ausente de la lista (283), nuevo == viejo (427), repetidos en la lista, `firstMeshOffset` != 0 (383; el EXE lo ignora),
  refCount 0/1/2/3/5/0x7FFF/0xFFFF/0x8000 (el incremento de 16 bits da la vuelta), lista vacia (225), cero mallas (171).

## Compuertas de run_local.py

Igualdad fila a fila (`probe_case.analyzer`), hook registrado y cuerpo en el ASI, colaboradores sustituidos, todas las compuertas de ABI/x87/ruta en ambos lados, `fp_difference` vacio,
fixture registrada == oraculo, estado final de cada arena == oraculo independiente de Python (ReplaceOld y GetFirstTexture completos; en Slerp la rama de copia y que nada fuera del destino
cambie), retorno y traza == oraculo, cobertura minima de cada clase de estres y final con restauracion. `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza un registro.
`offline/synth_log.py` genera un registro sintetico (mismas claves que `LiveComparison.hpp`), comprueba que el analizador lo acepta y que rechaza 8 mutaciones.

## Ejecucion

```
python -B reversing/Dependencies21G/fixture_oracle.py                 # cobertura declarada de las fixtures
python -B reversing/Dependencies21G/build_probe.py                    # compila la sonda (CMake del proyecto; restaura Game.cpp)
python -B reversing/Dependencies21G/run_local.py                      # corre dentro del juego y analiza
python -B reversing/Dependencies21G/run_local.py --analyze <comparisons.jsonl>
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de salida> Dependencies21G
```
Antes: aplicar `gg.patch` y anexar `gg-x87.cpp.txt`/`gg-x87.h.txt` al final de `X87Math.cpp`/`X87Math.h`; compilar el proyecto real; `python -B contrib/audit_register_residue.py` (sin cambios).

## Evidencia previa sin juego (arnes aislado, carpeta `offline/`)

`offline/gen_exebytes.py` extrae del `gta-sa.exe` original los bytes de las funciones; los arneses los reubican en memoria propia (parchean las constantes absolutas y los CALL rel32 de los
colaboradores) y los ejecutan en un proceso aparte frente a la reconstruccion C++ (`build.sh`/`buildh.sh` usan el cl.exe de VS con las opciones de `source/CMakeLists.txt`):

| Arnes | Que ejecuta | Resultado |
| --- | --- | --- |
| `slerp_h.cpp` | bytes de 0x59C300 vs `QuaternionSlerpCpp` compilado desde el X87Math.cpp real + anexo (registros ST con residuo conocido, FCW 0x007F) | 12 000 000 de casos: salida, FCW, FSW (incluidas las banderas sticky), FTW y los 80 bytes de ST0..ST7 identicos; solapes out == from y out == to |
| `rw_h.cpp` | bytes de 0x734940 (con 0x734930 y 0x7F3730) y de 0x734DE0 (con los bytes reales de 0x74E420 y 0x74DA20 en el camino refCount >= 2) vs C++ | 1 500 000 de casos de cada uno (509 257 con reemplazo): memoria, retorno y registros no volatiles identicos |
| `rehearsal.cpp` | las 1024 + 1536 fixtures del banco (mismos arenas y mismos stubs trazados) | 0 diferencias; el volcado coincide byte a byte con `fixture_oracle.py` (`xcheck.py`) |
| `gates_test.cpp` | `GateThis5`, `PrimeX87`, `SaveFP` del banco con los bytes reales de Slerp | ESP y no volatiles correctos; FSW/FTW iguales |

Esto no sustituye a la corrida dentro del juego (que tambien ejercita el camino real del hook: EXE -> thunk -> `CQuaternion::Slerp` Debug -> `QuaternionSlerpCpp`).

## Limites

- ReplaceOld: el camino `RpMaterialDestroy` con `refCount == 1` libera memoria del motor y solo se prueba con el stub (se comprueba que se llama con ese material y en ese orden, no la liberacion).
  `firstMeshOffset` != 0 se prueba (el EXE lo ignora). `numMaterials` negativo y `numMeshes` > 8 quedan fuera de las fixtures. Un `oldMaterial` nulo o un `newMaterial` nulo no son parte
  del dominio (el EXE desreferencia ESI+0x18).
- GetFirstTexture: el EXE tambien lee `first->next` (en ForAllTextures) antes de llamar al callback; la reconstruccion no lo lee. Solo difiere si la memoria de la primera textura no es legible,
  caso en que la textura devuelta ya seria invalida.
- Slerp: el oraculo de valores en Python solo cubre la rama de copia (los bytes de `to`); el calculo x87 se decide por igualdad EXE/DLL con las compuertas anteriores.
