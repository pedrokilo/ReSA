# CDecision: Set y SetDefault

Reconstrucción de Set (0x600570) y SetDefault (0x600530), comprobada contra pseudocódigo REA y ensamblador en `combo/`. Se corrigió la disposición de los booleanos a seis pares consecutivos y se conserva la conversión x87 a entero seguida de truncamiento al byte, incluido el resultado para NaN y desbordamientos.

El banco compara 3.072 pares: 1.024 SetDefault y 2.048 Set. Comprueba todos los bytes del objeto y que los argumentos permanecen intactos, con valores negativos, fracciones y valores fuera de rango. No necesita sustituir operaciones de la clase.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. El manifiesto y validation-summary.json vinculan fuentes, binarios y resultados aprobados.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 3.072 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-decision-pending\20261007-195600-386515/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
