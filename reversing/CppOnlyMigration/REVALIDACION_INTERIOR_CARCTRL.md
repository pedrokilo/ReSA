<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fusión de `Interior_c` y `CarCtrl` en sus dos archivos (11/10/2026)

Cierra las dos excepciones que dejó la [auditoría](AUDITORIA_ARCHIVOS_AUXILIARES.md): `Interior_c` (con `InteriorCpp.cpp` e `InteriorShelfCpp.cpp`) y `CCarCtrl` (con `NativeStubs.cpp`). Política: [PROCESO.md](PROCESO.md).

## Qué cambió

- `Interior_c.cpp` contiene ahora `Office_FurnishCenter`, `Shop_FurnishCeiling`, `FurnishOffice` y `Shop_AddShelfInfo`. Se borraron `InteriorCpp.cpp` e `InteriorShelfCpp.cpp`.
- `CarCtrl.cpp` contiene `Native_00424100`. Se borró `NativeStubs.cpp`.
- Ambos archivos se compilan sin `/ZI` (con `/Zi`), sin comprobaciones en tiempo de ejecución, sin PCH ni unity (`Interior_c.cpp` además con `/arch:IA32`). **No** se compilan con `/O2` completo: las funciones `naked` de `Interior_c` son transcripciones del EXE que escriben EBX, ESI y EDI sin guardarlos, y con `/O2` sus llamadores en C++ guardan variables en esos registros y las pierden. El primer intento así dio una violación de acceso en `FurnishOffice` (banco `CppInteriorNext`).
- Sólo `Shop_AddShelfInfo` (x87 directo para el redondeo C1 del EXE) y `Native_00424100` (3 bytes) se optimizan, con `#pragma optimize` y `#pragma float_control`. Funciona porque ya no se usa `/ZI`; con `/ZI` y `/RTC1` el compilador ignora ese pragma.
- `std::bit_cast<int32>(counter)` pasó a `int32(counter)`: sin `/Ob2` el `bit_cast` quedaba como una llamada.

Cuerpos verificados en el objeto real: `Shop_AddShelfInfo` 114 bytes idénticos al objeto anterior, `Shop_FurnishCeiling` 16 bytes idénticos, `Native_00424100` = `32 C0 C3` (3 bytes, como el EXE, sin relocaciones).

## Revalidación de `Interior_c`

Las 54 funciones enganchadas de `Interior_c.cpp` tienen banco; todas se repitieron.

| Banco | Pares | Resultado |
| --- | ---: | --- |
| CppInteriorRegression1 (corrida 20261011-160419-986968, auditada) | 6144 | aprobado |
| CppInteriorProposal (20261011-153548-459480, auditada) | 6144 | aprobado |
| CppInteriorNext (20261011-154040-161197, auditada) | 6144 | aprobado |
| InteriorAbi | 2048 | aprobado |
| InteriorBase | 25600 | aprobado |
| InteriorHome | 9216 | aprobado |
| InteriorLifecycleCore | 6144 | aprobado |
| InteriorLifecycleRuntime | 4096 | aprobado |
| InteriorPlacement | 4096 | aprobado |
| InteriorSpatial | 7168 | aprobado |
| InteriorCommerce (corrida 20261011-211815-039666, tras corregir su instrumentación) | 16384 | aprobado |

**InteriorCommerce** fallaba en `FurnishOffice` (1024/1024) y `Shop_AddShelfInfo` (266/1024), también con el árbol anterior a la fusión. Causa: el banco interceptaba los colaboradores sólo en sus direcciones del EXE, pero el candidato de esas dos funciones es C++ (convertido el 10/10) y llama a las funciones de la DLL (`SetTilesStatus`, `GetRandomId`, `AddInteriorInfo`, `Office_FurnishEdges`, `Office_FurnishCenter`, `Shop_FurnishCeiling`), que no pasaban por la intercepción: en la traza del candidato faltaban esas llamadas. Solución: `LiveComparison.hpp` intercepta ahora también el punto de entrada de la DLL de cada uno de esos colaboradores con el mismo despachador que usa para el EXE (`patchDll`), sin saltarse la función bajo prueba. No se retiró ni relajó ninguna compuerta: la comparación de trazas, memoria y retorno sigue siendo exacta. Con el parche pasan los 16384 pares de las 16 funciones. Los colaboradores quedan sustituidos por el despachador durante la prueba, así que este banco no valida sus cuerpos.

Una corrida de `CppInteriorRegression1` registró una aserción de `CShadowCamera::DrawOutlineBorder` (código de sombras en tiempo real, ajeno a `Interior_c`) y su auditor la rechazó; repetida, pasó limpia. Se anota como puntual.

## Revalidación de `CCarCtrl`

| Banco | Pares | Resultado |
| --- | ---: | --- |
| CppIntegerWorld10 (20261011-161211-340213, auditada) | 4096 | aprobado |
| CppSequence1 (20261011-161020-773879, auditor genérico) | 4352 | aprobado |
| CppSequence2 (20261011-161116-526305, auditor genérico) | 4352 | aprobado |
| CarCtrl22 | 10304 | aprobado |
| DependenciesCarCtrl2 | 4352 | aprobado |
| DependenciesCarCtrl4 | 2048 | aprobado |
| DependenciesVehicleLeaf5 | 5120 | aprobado |
| DependenciesWorld10 | 4096 | aprobado |
| DependenciesWorld14 | 2048 | aprobado |
| PopulationFactory | 18432 | aprobado |
| StreamingFinal4 (corrida 20261011-211139-086170, tras corregir su instrumentación) | 20992 | aprobado |

**StreamingFinal4** fallaba sólo en `CStreaming::Shutdown` (64/64), también con el árbol anterior. Causa: el banco intercepta `CMemoryMgr::FreeAlign` en la dirección del EXE (0x72F4F0), pero la `FreeAlign` de la DLL es ahora una transcripción `naked` que libera por su cuenta sin pasar por esa dirección, de modo que el candidato registraba un evento menos (`events` 3 contra 2 y 1 contra 0). Solución: `LiveComparison.hpp` pasa ahora la dirección de la función de la DLL (`&CMemoryMgr::FreeAlign`) a `AddStub`, igual que ya hacía con `CGeneral::GetRandomNumber`. No se tocó `Shutdown` ni ninguna compuerta. Pasan los 20992 pares. Como `FreeAlign` queda sustituida por el stub durante la prueba, este banco no valida su cuerpo.

## Cómo se ejecutó y qué límites tiene

- Los bancos históricos se lanzaron con `herramientas/rerun_hist.py` (envoltorio `hist_wrap.py`): redirige el juego al clon de pruebas en memoria, guarda el resultado en `CppInteriorUnified`, `CppCarCtrlUnified`, `CppInteriorBaseline` y `CppStreamingBaseline`, y restaura con `git checkout` los archivos de evidencia que cada banco reescribe. `PopulationFactory` fija el hash de la sonda aprobada (`PROBE_HASH`): el envoltorio lo reapunta al hash de la sonda recién compilada y copia la sonda a `_baseline`, sin tocar ninguna otra compuerta.
- `CppSequence1` y `CppSequence2` no tenían script de auditoría en el repositorio: se auditaron con `herramientas/audit_generico.py`, que comprueba pares, igualdad, no nulos, compuertas booleanas, restauración y hashes, pero no registros crudos.
- Los dos fallos que quedaban eran de la instrumentación de sus bancos (ver arriba); las correcciones están en `reversing/InteriorCommerce/LiveComparison.hpp` y `reversing/StreamingFinal4/LiveComparison.hpp`, y la evidencia en `CppInteriorCommerceFix` y `CppStreamingFix`.
- Los bancos de Interior se corrieron antes del cambio de `CarCtrl.cpp` y los de CarCtrl después de la fusión de Interior; el árbol final contiene ambos cambios, que están en archivos distintos. No se repitió una pasada conjunta.
- El x87, NaN, infinitos y registros crudos conservan los límites que ya tenían los bancos.
