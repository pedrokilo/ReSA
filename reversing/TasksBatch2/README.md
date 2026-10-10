<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas 2: TurnToFaceEntityOrCoord, UseEffectRunning/Sprinting, UseClosestFreeScriptedAttractorRun/Sprint

Validadas el 02/10/2026 con **7.488 pares original/candidato dentro de GTA**, todos exactos: 19 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetHeading`: el original normaliza la dirección y usa `GetRadianAngleBetweenPoints(d.x, d.y, 0, 0)`, no `CVector::Heading`. El constructor por entidad deja `m_CoordsToFace` sin inicializar.
- `CTaskComplexUseEffectSprinting`: el constructor pone `PEDMOVE_SPRINT`; la DLL tenía `PEDMOVE_RUN` (error de copia).
- `CTaskComplexUseClosestFreeScriptedAttractorRun::Clone`: crea una tarea nueva sin copiar nada.

Repetir: `python reversing/TasksBatch2/build_probe.py` y `python reversing/run.py --case tasks2`. Evidencia: [validation-summary.json](validation-summary.json).
