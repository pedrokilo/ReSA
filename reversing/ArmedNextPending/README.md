<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
