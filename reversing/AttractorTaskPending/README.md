<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# AttractorTaskPending

Clase: `CTaskComplexGoToAttractor`. Funciones: Clone, CreateFirstSubTask, CreateNextSubTask. Casos previstos: 1024 pares EXE/DLL.

Construcción de secuencias, teletransporte y refugios; globales y dependencias aislados. La primera ejecución comparó 1024 pares sin diferencias, pero el identificador de pareja duplicado impidió aceptar cobertura. Corregido; pendiente de repetir la ejecución.

La reconstrucción se contrasta con `combo.sh` y ensamblador completo almacenados en `reversing/TasksPendingEvidence`. Cuando Ghidra no reconoce una función, se conserva explícitamente esa limitación y el ensamblador es la referencia. Las funciones virtuales se alternan mediante los hooks y sus ranuras comprobadas; los getters usan el ABI de retorno por estructura.

El banco compara retorno, bytes relevantes de fixtures y trazas ordenadas. Usa fixtures propios, restauración de globals/hooks y cobertura positiva por rama; cada pareja tiene un identificador global. `PumpMessages` mantiene la ventana receptiva. No usa tareas ni peds reales como entradas.

`build_probe.py` declara los cpp/h dependientes para la huella de fuentes; `run_local.py` espera las comparaciones y el resumen final. Un resultado válido exige pares completos, igualdad, cobertura positiva y restauración. Los helpers sustituidos delimitan la equivalencia del controlador: no validan integración con IA, animaciones, pools o audio reales. Los artefactos de compilación y ejecución son la evidencia; reconstruida no significa validada.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 1.024 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-attractor-task-pending\20261007-195707-708603/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
