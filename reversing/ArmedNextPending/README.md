# ArmedNextPending



`CTaskComplexKillPedOnFootArmed::CreateNextSubTask` (0x62C190), vtable 0x86D918, ranura 8 de 11. Banco de 1024 pares con peds/tareas/cover/matriz propios. Compara retorno, bytes de estado y trazas de LOS, RNG, speech, selección y reserva de cover, duck y subtareas.



Estado: reconstrucción terminada; compilación y comparación pendientes. El ensamblador completo está en TasksPendingEvidence/62C190.full-asm.txt y la cola 62CCD5..62CCDF fue comprobada. REA/Ghidra no reconoció la función y el puente aislado de recuperación falló; no se afirma disponer de pseudo válido. Las coordenadas originales indeterminadas al fallar el helper de cover se estabilizan en ambos lados por fixture; esa rama no certifica comportamiento con memoria indeterminada real.



Referencias y CTask::new están sustituidos en ambos lados; CreateSubTask se aísla con traza. No usa peds/tareas del juego para las entradas. Restaura tiempo y hooks, bombea mensajes y exige cobertura positiva. Helpers sustituidos delimitan la equivalencia del controlador. Los artefactos de la cola coordinada acreditarán el resultado; no se afirma aprobación antes de ejecutarlo.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 1.024 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-armed-next-pending\20261007-200146-998852/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
