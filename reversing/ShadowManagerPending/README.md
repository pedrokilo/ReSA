# CRealTimeShadowManager: funciones pendientes

Reconstrucción de ReInit (0x706870) y GetRealTimeShadow (0x706970), cotejada con REA y ensamblador en `combo/`. GetRealTimeShadow devuelve un puntero nullable, reserva el slot cero para el jugador y selecciona el último slot libre para otras entidades. ReInit respeta el orden de destrucción, creación y asociación de rasters.

2.560 pares: 512 ReInit y 2.048 GetRealTimeShadow. Cámaras, rasters, entidades y dependencias se aíslan en ambos lados; se comparan llamadas, slots y campos y se restaura el estado. No certifica el dibujo de sombras ni las dependencias sustituidas. SetupForThisEntity conserva su llamada nativa como dependencia no validada por este banco.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. Manifiesto y resumen vinculan los resultados aprobados a las fuentes y binarios.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.560 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-shadow-manager-pending\20261007-200912-414909/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
