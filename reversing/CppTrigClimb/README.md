# CppTrigClimb

Propuesta offline pendiente de ejecución y aprobación. Derivado de CTasks15 con tabla completa, conmutación histórica y semillas indexadas por f original. Selección: {"CTaskSimpleClimb::GetCameraStickModifier": 512, "CTaskSimpleClimb::GetCameraTargetPos": 512, "CTaskSimpleClimb::ProcessPed": 2048, "CTaskSimpleClimb::TestForClimb": 1024, "CTaskSimpleClimb::TestForStandUp": 256, "CTaskSimpleClimb::TestForVault": 256}; total 4608 pares.

Mantiene snapshots, trazas, gates switches/clean, fixtures y restauración del banco histórico; añade comprobación enabled/locked de todos los hooks. Registros laterales completos en todas las comparaciones, nunca placeholders nulos. No incorpora tolerancias ni descarta campos.

No hay gate físico FP108 ni contrato completo de flags/registros en el banco heredado. La migración requiere evidencia del objeto helper y equivalencia de callers compartida por coordinación, además de comparación dentro del juego. Colaboradores y exclusiones históricos documentados en CTasks15/README.md siguen vigentes; no prueba independencia transitiva.

Manifiesto fuentes históricas+TrigSimpleCpp+CMake+InjectHooks+RH. Los scripts apuntan exclusivamente al clon E:/JUEGOS/GTA San Andreas - CVehicle Test. No se modifica runtime_runner.py. Solo coordinación compila y ejecuta serialmente. README guardado en UTF-8 y verificado por lectura estricta; sin mojibake.
