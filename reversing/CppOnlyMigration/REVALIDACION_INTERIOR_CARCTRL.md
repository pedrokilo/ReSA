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
| **InteriorCommerce** | 16384 | **falla en 2 funciones, también antes de la fusión** |

**InteriorCommerce** falla `FurnishOffice` (1024/1024) y `Shop_AddShelfInfo` (266/1024), exactamente igual con el árbol anterior a la fusión (evidencia en `CppInteriorBaseline`). El banco histórico espera, en las trazas, las llamadas al EXE de la versión antigua de esas dos funciones, que ya se habían convertido a C++ el 10/10; las valida `CppInteriorProposal`/`CppInteriorNext`. Las otras 14 funciones del banco pasan. Conviene retirar esas dos del banco histórico o derivar uno nuevo; no se hizo.

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
| **StreamingFinal4** | 20992 | **`CStreaming::Shutdown` falla 64/64, también antes del cambio** |

**StreamingFinal4:** la única función que difiere es `CStreaming::Shutdown`, y falla idéntico (64/64) con el árbol de HEAD, anterior a los cambios de `CarCtrl.cpp` e `Interior_c.cpp` (evidencia en `CppStreamingBaseline`). Es un fallo preexistente de ese banco o de esa función, no de esta fusión; las demás funciones del banco pasan. No se investigó su causa.

## Cómo se ejecutó y qué límites tiene

- Los bancos históricos se lanzaron con `herramientas/rerun_hist.py` (envoltorio `hist_wrap.py`): redirige el juego al clon de pruebas en memoria, guarda el resultado en `CppInteriorUnified`, `CppCarCtrlUnified`, `CppInteriorBaseline` y `CppStreamingBaseline`, y restaura con `git checkout` los archivos de evidencia que cada banco reescribe. `PopulationFactory` fija el hash de la sonda aprobada (`PROBE_HASH`): el envoltorio lo reapunta al hash de la sonda recién compilada y copia la sonda a `_baseline`, sin tocar ninguna otra compuerta.
- `CppSequence1` y `CppSequence2` no tenían script de auditoría en el repositorio: se auditaron con `herramientas/audit_generico.py`, que comprueba pares, igualdad, no nulos, compuertas booleanas, restauración y hashes, pero no registros crudos.
- Los bancos de Interior se corrieron antes del cambio de `CarCtrl.cpp` y los de CarCtrl después de la fusión de Interior; el árbol final contiene ambos cambios, que están en archivos distintos. No se repitió una pasada conjunta.
- El x87, NaN, infinitos y registros crudos conservan los límites que ya tenían los bancos.
