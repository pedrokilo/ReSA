<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CTaskComplexKillCriminal — pendientes

Validación del banco aprobada el 2026-10-07: 2048 pares EXE/DLL, sin diferencias. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-criminal-pending/20261007-191825-623524/summary.json>); comparaciones: [comparisons.jsonl](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-criminal-pending/20261007-191825-623524/comparisons.jsonl>).

CreateFirstSubTask 0x68DC60 y ControlSubTask 0x68E950 reconstruidas con ensamblador íntegro y pseudocódigo REA recuperado para ambas funciones. La recuperación posterior de First cubre 0x68DC60..0x68E4EE y está conservada en `reversing/_handoff/ghidra/TasksPending.Recovered_68DC60.c`; el combo actualizado está en `reversing/CriminalPending/combo/68DC60.txt`. El ensamblador completo y su epílogo delimitan el cuerpo original. Tabla virtual 0x870A00: 11 entradas, First slot 9 y Control slot 10.

Banco de 2048 pares, 1024 por función. Fixtures de policías, criminal, compañero, pasajeros, jugador, wanted, inteligencia y vehículos; tareas actuales con vtable controlada. Stubs emparejados de referencias, armas, decisiones, eventos, RNG, selección de criminal, aborto, asignación de tareas y asignación de memoria. Constructores de dependencias se ejecutan sobre almacenamiento del banco; se compara todo su contenido normalizando únicamente la vtable al serializar. También se prueban asignaciones fallidas y reutilización de subtareas. Ningún actor real recibe eventos, armas o referencias.

Comparación de todos los bytes de la tarea, actores, vehículos, inteligencia y memoria asignada, retornos e historial de efectos. Hooks alternados antes de cada par y se exige actividad positiva por función. Se restauran hooks, ambient crime, tiempo y timestep. SOURCES incluye la clase y las dependencias de construcción y matemáticas.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.048 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-criminal-pending\20261007-195636-281114/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
