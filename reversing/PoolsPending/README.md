# CPools: guardado de entidades

ReconstrucciÃ³n de SaveObjectPool (0x5D4940), SavePedPool (0x5D4B40) y SaveVehiclePool (0x5D4800), cotejada con el EXE mediante `combo/`.

3.072 pares, 1.024 por funciÃ³n, con pools privados y sustituciones simÃ©tricas de escritura, datos de objetos y guardado virtual. Comprueba ocupaciÃ³n, selecciÃ³n, referencias, modelos con extensiÃ³n de signo, orden y contenido de las escrituras. Los buffers completos se resumen mediante FNV1a64 para mantener el informe compacto. No certifica archivos de partidas completos ni las funciones de guardado sustituidas.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. Manifiesto y resumen de validaciÃ³n vinculan los resultados aprobados a las fuentes y binarios; el banco restaura pools y estado.

REA no produjo pseudocódigo válido para SaveVehiclePool (0x5D4800); se reconstruyó desde el ensamblador completo del combo y se comprobó mediante el banco diferencial.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 3.072 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pools-pending\20261007-201151-743253/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
