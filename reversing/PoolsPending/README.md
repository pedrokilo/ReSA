<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CPools: guardado de entidades

ReconstrucciÃ³n de SaveObjectPool (0x5D4940), SavePedPool (0x5D4B40) y SaveVehiclePool (0x5D4800), cotejada con el EXE mediante `combo/`.

3.072 pares, 1.024 por funciÃ³n, con pools privados y sustituciones simÃ©tricas de escritura, datos de objetos y guardado virtual. Comprueba ocupaciÃ³n, selecciÃ³n, referencias, modelos con extensiÃ³n de signo, orden y contenido de las escrituras. Los buffers completos se resumen mediante FNV1a64 para mantener el informe compacto. No certifica archivos de partidas completos ni las funciones de guardado sustituidas.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. Manifiesto y resumen de validaciÃ³n vinculan los resultados aprobados a las fuentes y binarios; el banco restaura pools y estado.

REA no produjo pseudocódigo válido para SaveVehiclePool (0x5D4800); se reconstruyó desde el ensamblador completo del combo y se comprobó mediante el banco diferencial.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 3.072 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pools-pending\20261007-201151-743253/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
