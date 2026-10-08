<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# ArmedNextPending



`CTaskComplexKillPedOnFootArmed::CreateNextSubTask` (0x62C190), vtable 0x86D918, ranura 8 de 11. Banco de 1024 pares con peds/tareas/cover/matriz propios. Compara retorno, bytes de estado y trazas de LOS, RNG, speech, selección y reserva de cover, duck y subtareas.



Estado: reconstrucción terminada; compilación y comparación pendientes. El ensamblador completo está en TasksPendingEvidence/62C190.full-asm.txt y la cola 62CCD5..62CCDF fue comprobada. REA/Ghidra no reconoció la función y el puente aislado de recuperación falló; no se afirma disponer de pseudo válido. Las coordenadas originales indeterminadas al fallar el helper de cover se estabilizan en ambos lados por fixture; esa rama no certifica comportamiento con memoria indeterminada real.



Referencias y CTask::new están sustituidos en ambos lados; CreateSubTask se aísla con traza. No usa peds/tareas del juego para las entradas. Restaura tiempo y hooks, bombea mensajes y exige cobertura positiva. Helpers sustituidos delimitan la equivalencia del controlador. Los artefactos de la cola coordinada acreditarán el resultado; no se afirma aprobación antes de ejecutarlo.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 1.024 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-armed-next-pending\20261007-200146-998852/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
