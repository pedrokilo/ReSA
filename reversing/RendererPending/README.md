<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CCustomBuildingRenderer: funciones pendientes

Revisión y activación de AtomicSetup (0x5D7F00), IsCBPCPipelineAttached (0x5D7F40), Update (0x5D8050) y UpdateDayNightBalanceParam (0x5D7F80), contrastadas con el EXE en `combo/`.

6.144 pares: 1.024 AtomicSetup, 1.024 comprobación de pipeline, 2.048 Update y 2.048 balance día/noche. Comprueba datos de geometría, selección de pipeline, contador circular, horas y precisión del balance. RenderWare y otras dependencias se aíslan de igual manera en ambos lados; no certifica el resultado gráfico completo. Se restaura el estado global.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. Manifiesto y validation-summary.json vinculan resultados aprobados, fuentes y binarios.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 6.144 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-renderer-pending\20261007-200833-500053/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
