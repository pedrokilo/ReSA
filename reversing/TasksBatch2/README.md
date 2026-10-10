<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas 2: TurnToFaceEntityOrCoord, UseEffectRunning/Sprinting, UseClosestFreeScriptedAttractorRun/Sprint

Validadas el 02/10/2026 con **7.488 pares original/candidato dentro de GTA**, todos exactos: 19 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetHeading`: el original normaliza la dirección y usa `GetRadianAngleBetweenPoints(d.x, d.y, 0, 0)`, no `CVector::Heading`. El constructor por entidad deja `m_CoordsToFace` sin inicializar.
- `CTaskComplexUseEffectSprinting`: el constructor pone `PEDMOVE_SPRINT`; la DLL tenía `PEDMOVE_RUN` (error de copia).
- `CTaskComplexUseClosestFreeScriptedAttractorRun::Clone`: crea una tarea nueva sin copiar nada.

Repetir: `python reversing/TasksBatch2/build_probe.py` y `python reversing/run.py --case tasks2`. Evidencia: [validation-summary.json](validation-summary.json).
