<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CTaskComplexWander — 14/14

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

CreateNextSubTask (0x674140) aprobó 10.240 comparaciones originales/candidatas dentro de GTA. Se compara el árbol de tareas creado, parámetros, flags, nodos y orden de llamadas; contadores independientes comprueban que el original no ejecuta el candidato. Peatones independientes y jugador real sin modificaciones. La copia de pruebas restaura binarios y verifica partidas automáticamente.

Correcciones: comparar el siguiente nodo con el nodo anterior guardado después de UpdateDir y antes de UpdatePathNodes; eliminar la transición de sentarse ausente en el original; conservar el orden de construcción de la secuencia y de las decisiones de tráfico.

Matriz: diez tipos de subtarea, ocho actualizaciones de nodos, 32 combinaciones de flags y cuatro combinaciones de semáforo/cruce. Incluye nodos inválidos, inversión de ruta, bloqueo, dirección 255, salida del coche, animación, radio y estado de movimiento. Asignaciones y constructores de tareas reales; se normalizan punteros y se comparan campos definidos, excluyendo padding.

Las actualizaciones de ruta, CreateFirstSubTask, coordenadas, orientación y consultas de tráfico son controladas. Las tareas resultantes no se ejecutan: este banco no demuestra navegación completa ni estabilidad general del juego. Audio y Global/SurfaceInfos_c están excluidos. 14/14 cuenta funciones registradas, no cobertura exhaustiva de toda la clase.

Repetir: `PROBAR.cmd --case wander` o `python reversing/run.py --case wander`. Reutiliza el runner compartido y termina automáticamente. `build_probe.py` genera instrumentación temporal y restaura fuentes en finally. Sonda archivada y hashes en validation-summary.json; --all repite bancos archivados, no una compilación conjunta actualizada.

Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-wander/20260922-211400-323524`. No commit ni push.
