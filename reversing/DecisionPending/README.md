<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CDecision: Set y SetDefault

Reconstrucción de Set (0x600570) y SetDefault (0x600530), comprobada contra pseudocódigo REA y ensamblador en `combo/`. Se corrigió la disposición de los booleanos a seis pares consecutivos y se conserva la conversión x87 a entero seguida de truncamiento al byte, incluido el resultado para NaN y desbordamientos.

El banco compara 3.072 pares: 1.024 SetDefault y 2.048 Set. Comprueba todos los bytes del objeto y que los argumentos permanecen intactos, con valores negativos, fracciones y valores fuera de rango. No necesita sustituir operaciones de la clase.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. El manifiesto y validation-summary.json vinculan fuentes, binarios y resultados aprobados.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 3.072 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-decision-pending\20261007-195600-386515/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
