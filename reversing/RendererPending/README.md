<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
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
