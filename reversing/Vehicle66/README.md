<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Vehicle66: CVehicle::RemoveVehicleUpgrade (0x6DF930), AddUpgrade (0x6DFA20), AddVehicleUpgrade (0x6E3290), PossiblyDropFreeFallBombForPlayer (0x6E07E0), ProcessSirenAndHorn (0x6E0950)

Estado: **APROBADO dentro del juego** (21/10/2026, worktree `D:\Original\wu-2`, HEAD 5765c0b5f + Vehicle.cpp/Vehicle.h). 20480 pares (5 x 4096), cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-vehicle66\20261021-105337-699582`.

`source.patch`: solo Vehicle.cpp y Vehicle.h (`git apply --check` verificado contra HEAD).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| RemoveVehicleUpgrade | 0x006DF930 | CVehicle,RemoveVehicleUpgrade | S |
| AddUpgrade | 0x006DFA20 | CVehicle,AddUpgrade | S |
| AddVehicleUpgrade | 0x006E3290 | CVehicle,AddVehicleUpgrade | S |
| PossiblyDropFreeFallBombForPlayer | 0x006E07E0 | CVehicle,PossiblyDropFreeFallBombForPlayer | S |
| ProcessSirenAndHorn | 0x006E0950 | CVehicle,ProcessSirenAndHorn | S |

Ninguna es virtual.

Revision contra el EXE: las cinco son thiscall con `ret n`. AddVehicleUpgrade devuelve en EAX la mejora sustituida (`int32`; antes `void`): el llamador 0x4985DA hace `mov [0xA43C78],eax`; se exige EAX en el banco. Ningun llamador del EXE lee EAX/ECX/EDX de las otras cuatro (residue_scan.py). Orden de operaciones x87 de PossiblyDrop: x = pos.x + (+-ord.x), y = ord.y + pos.y, z = ord.z + pos.z, con la posicion leida antes de GetPlaneOrdnancePosition.
Discrepancia hallada por el banco: la DLL comprueba el tipo del RwObject en `GetRpClump()` (assert); el banco usa un clump falso de tipo rpCLUMP (defecto del banco, no de la funcion).
Limites: no se prueban denormales (MXCSR FTZ), dos NaN en una suma, indices de mejora fuera de 0..14 en AddUpgrade ni model infos fuera de los falsos instalados en ms_modelInfoPtrs[15..19].

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Vehicle66`; `python -I fixture_oracle.py`; `python -I fixture_mirror.py`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).

## 2026-10-26: AddReplacementUpgrade (0x6D3830) con hook desde el lote 72
Desde el lote 72 `CVehicle::AddReplacementUpgrade` tiene `RH_ScopedInstall` (banco ReplUpgrade72). El banco ya la sustituia por `FkAddRepl` en la direccion del EXE, pero `MakeCollab` ya devuelve el cuerpo de la DLL cuando encuentra el hook y lo parchea con el mismo falso; lo que fallaba era la compuerta de `run_local.py`, que esperaba `dll=false` para 0x6D3830. Cambio: 0x6D3830 (`_AREPL`) entra en `DLL_HOOKED` (colaboradores con hook activo, sustituidos por el mismo falso en el EXE y en el cuerpo de la DLL). Compuertas y cobertura sin relajar; `hooked` sigue exigido a false para los colaboradores. Sin cambios en produccion.
