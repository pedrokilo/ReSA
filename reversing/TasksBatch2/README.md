# Tareas 2: TurnToFaceEntityOrCoord, UseEffectRunning/Sprinting, UseClosestFreeScriptedAttractorRun/Sprint

Validadas el 02/10/2026 con **7.488 pares original/candidato dentro de GTA**, todos exactos: 19 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetHeading`: el original normaliza la dirección y usa `GetRadianAngleBetweenPoints(d.x, d.y, 0, 0)`, no `CVector::Heading`. El constructor por entidad deja `m_CoordsToFace` sin inicializar.
- `CTaskComplexUseEffectSprinting`: el constructor pone `PEDMOVE_SPRINT`; la DLL tenía `PEDMOVE_RUN` (error de copia).
- `CTaskComplexUseClosestFreeScriptedAttractorRun::Clone`: crea una tarea nueva sin copiar nada.

Repetir: `python reversing/TasksBatch2/build_probe.py` y `python reversing/run.py --case tasks2`. Evidencia: [validation-summary.json](validation-summary.json).
