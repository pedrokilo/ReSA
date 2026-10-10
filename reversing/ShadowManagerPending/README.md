<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
