# RequestedStorageWorkBuffer

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **banco preparado que nunca se ejecuto; no aprobado en esta ronda. Sus funciones las cubre `StorageFinal1`.**

Hallazgos de la repeticion: (1) `LiveComparison.hpp` no compilaba (`error C2400` en el `__asm` de `Call`: `size` es un operador reservado del ensamblador de MSVC); se sustituyo por una llamada cdecl en C++ puro (`reinterpret_cast<uint32(__cdecl*)(void*,int32)>`). (2) `run_local.py` exige `Diagnostics/required-ABI-proof.json` (revision manual del desensamblado compilado) que jamas existio; sin ella el banco no corre. (3) la constante `snapshot_ranges == 17692` se actualizo a `>= 2*hooks_snapshotted`. Cubre `CGenericGameStorage::SaveDataToWorkBuffer-org` (0x5D1270) y `LoadDataFromWorkBuffer-org` (0x5D1300), que ya estan validadas por `StorageFinal1` (1620 pares cada una, registro vigente).
