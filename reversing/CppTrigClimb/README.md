<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppTrigClimb

Propuesta offline pendiente de ejecución y aprobación. Derivado de CTasks15 con tabla completa, conmutación histórica y semillas indexadas por f original. Selección: {"CTaskSimpleClimb::GetCameraStickModifier": 512, "CTaskSimpleClimb::GetCameraTargetPos": 512, "CTaskSimpleClimb::ProcessPed": 2048, "CTaskSimpleClimb::TestForClimb": 1024, "CTaskSimpleClimb::TestForStandUp": 256, "CTaskSimpleClimb::TestForVault": 256}; total 4608 pares.

Mantiene snapshots, trazas, gates switches/clean, fixtures y restauración del banco histórico; añade comprobación enabled/locked de todos los hooks. Registros laterales completos en todas las comparaciones, nunca placeholders nulos. No incorpora tolerancias ni descarta campos.

No hay gate físico FP108 ni contrato completo de flags/registros en el banco heredado. La migración requiere evidencia del objeto helper y equivalencia de callers compartida por coordinación, además de comparación dentro del juego. Colaboradores y exclusiones históricos documentados en CTasks15/README.md siguen vigentes; no prueba independencia transitiva.

Manifiesto fuentes históricas+TrigSimpleCpp+CMake+InjectHooks+RH. Los scripts apuntan exclusivamente al clon E:/JUEGOS/GTA San Andreas - CVehicle Test. No se modifica runtime_runner.py. Solo coordinación compila y ejecuta serialmente. README guardado en UTF-8 y verificado por lectura estricta; sin mojibake.
