<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas 2: TurnToFaceEntityOrCoord, UseEffectRunning/Sprinting, UseClosestFreeScriptedAttractorRun/Sprint

Validadas el 02/10/2026 con **7.488 pares original/candidato dentro de GTA**, todos exactos: 19 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetHeading`: el original normaliza la dirección y usa `GetRadianAngleBetweenPoints(d.x, d.y, 0, 0)`, no `CVector::Heading`. El constructor por entidad deja `m_CoordsToFace` sin inicializar.
- `CTaskComplexUseEffectSprinting`: el constructor pone `PEDMOVE_SPRINT`; la DLL tenía `PEDMOVE_RUN` (error de copia).
- `CTaskComplexUseClosestFreeScriptedAttractorRun::Clone`: crea una tarea nueva sin copiar nada.

Repetir: `python reversing/TasksBatch2/build_probe.py` y `python reversing/run.py --case tasks2`. Evidencia: [validation-summary.json](validation-summary.json).
