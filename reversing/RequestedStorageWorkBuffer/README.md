<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 899 grupos / 10469 funciones reconstruidas; 10469 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedStorageWorkBuffer

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **banco preparado que nunca se ejecuto; no aprobado en esta ronda. Sus funciones las cubre `StorageFinal1`.**

Hallazgos de la repeticion: (1) `LiveComparison.hpp` no compilaba (`error C2400` en el `__asm` de `Call`: `size` es un operador reservado del ensamblador de MSVC); se sustituyo por una llamada cdecl en C++ puro (`reinterpret_cast<uint32(__cdecl*)(void*,int32)>`). (2) `run_local.py` exige `Diagnostics/required-ABI-proof.json` (revision manual del desensamblado compilado) que jamas existio; sin ella el banco no corre. (3) la constante `snapshot_ranges == 17692` se actualizo a `>= 2*hooks_snapshotted`. Cubre `CGenericGameStorage::SaveDataToWorkBuffer-org` (0x5D1270) y `LoadDataFromWorkBuffer-org` (0x5D1300), que ya estan validadas por `StorageFinal1` (1620 pares cada una, registro vigente).
