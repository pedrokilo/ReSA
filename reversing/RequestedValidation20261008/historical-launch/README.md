<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Historical class launch

Selection: all catalog functions in each class must have validated=1. This selects 694 classes / 6136 functions. This is historical evidence, not current source/dependency accreditation or a new functional replay.

The explicit --historical-hooks argument selects exact class/function/address/type identities after injection and ordinary hook settings. Unknown hooks, including script-command hooks, are disabled. It temporarily unlocks hooks for selection, then locks every hook to preserve the profile. It exits before gameplay if the selection file is empty, the report cannot be written, the state disagrees, or any listed identity is missing. Launch without this argument retains normal behavior.

runtime-mappings.json records 14 catalog/runtime type discrepancies. Thirteen retain identical concrete addresses; the static FinishAnimCB callback address is verified against its source registration. The catalog itself was not changed.

runtime.tsv records actual hook states after selection. This is state verification, not a new EXE/C++ behavioral comparison. Initial failed selections were stopped by the startup guard; final report: 6136 enabled, 2313 disabled, 694 active classes, zero selection/state mismatches. EXE and ASI hashes are in launch-summary.json. Backup of the previously installed ASI is in backup/.

Debug build: _CL_=/MP6, outer build parallelism 1. No automatic save loading; existing test-copy configuration preserved. Only the CVehicle Test installation was used.
