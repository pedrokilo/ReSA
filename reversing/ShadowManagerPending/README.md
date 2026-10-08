<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CRealTimeShadowManager: funciones pendientes

Reconstrucción de ReInit (0x706870) y GetRealTimeShadow (0x706970), cotejada con REA y ensamblador en `combo/`. GetRealTimeShadow devuelve un puntero nullable, reserva el slot cero para el jugador y selecciona el último slot libre para otras entidades. ReInit respeta el orden de destrucción, creación y asociación de rasters.

2.560 pares: 512 ReInit y 2.048 GetRealTimeShadow. Cámaras, rasters, entidades y dependencias se aíslan en ambos lados; se comparan llamadas, slots y campos y se restaura el estado. No certifica el dibujo de sombras ni las dependencias sustituidas. SetupForThisEntity conserva su llamada nativa como dependencia no validada por este banco.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. Manifiesto y resumen vinculan los resultados aprobados a las fuentes y binarios.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.560 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-shadow-manager-pending\20261007-200912-414909/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
