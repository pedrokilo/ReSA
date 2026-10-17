<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
