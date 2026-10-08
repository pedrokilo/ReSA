<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas `CTaskComplexUse*` (lote 4)

Validadas el 02/10/2026 con **15.872 pares original/candidato dentro de GTA**, todos exactos: 33 funciones con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexUseScriptedAttractor` | 7 |
| `CTaskComplexUseScriptedBrain` | 8 |
| `CTaskComplexUseSequence` | 6 |
| `CTaskComplexUseSwatRope` | 7 |
| `CTaskComplexUseWaterCannon` | 5 |

**Diferencias reales corregidas:**
- `UseScriptedBrain`:
  - el constructor copia el nombre con `strcpy` sin tocar el resto del búfer;
  - `Clone` pasa el nombre de la tarea de origen (la DLL pasaba el suyo sin inicializar);
  - `MakeAbortable` comprueba que la tarea del comando no sea nula;
  - `CreateNextSubTask` termina con `TASK_SIMPLE_FINISH_BRAIN`;
  - `CreateFirstSubTask` espera con `CTaskSimplePause(2000)` si el cerebro no tiene tarea;
  - `ControlSubTask` usa prioridad URGENT y guarda la tarea actual antes de clonarla.
- `UseSwatRope`:
  - el indicador de helicóptero es solo el bit 0 del byte +0x14;
  - `MakeAbortable` no toca el evento con prioridad IMMEDIATE y exige salud estrictamente < 20;
  - `ControlSubTask` aborta cuando no encuentra el punto de la cuerda;
  - `ControlSubTask` usa el resultado de `FindZCoorForPed`, que la DLL descartaba (nunca bajaba por la cuerda).
- `UseWaterCannon::CreateNextSubTask` consulta el tipo de la subtarea como el original.
- `UseScriptedAttractor::CreateFirstSubTask` solo reconoce un puntero exactamente igual al inicio de un efecto (la DLL aceptaba punteros interiores).

**Método:**
- Peatón, helicóptero, vehículo e incendio en búferes propios.
- Tareas falsas con vtable trazada.
- Las secuencias globales se guardan y se restauran tras cada ejecución.
- El almacén de cerebros (0xC188F0) se rellena de verdad, porque el EXE lo consulta en línea.
- Se simulan `FindZCoorForPed`, `FindCoorsAlongRope`, `CPed::Say`, `FireTruckControl`, `CScriptedBrainTaskStore::Clear` y el RNG.

Repetir: `python reversing/TasksBatch4/build_probe.py` y `python reversing/run.py --case tasks4`. Evidencia: [validation-summary.json](validation-summary.json).
