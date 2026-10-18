<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# FxSystem_c: funciones pendientes

ReconstrucciÃ³n de ambas sobrecargas AddParticle (0x4AA440 y 0x4AA540), Update (0x4AAF70) y UpdateBoundingBoxCB (0x4AA9A0), cotejada con REA y ensamblador en `combo/`.

El banco compara 5.120 pares: 1.024 por sobrecarga, 1.024 del callback y 2.048 Update. Comprueba calidad/RNG, primitivas habilitadas, posiciones y matrices, reproducciÃ³n, tiempos, visibilidad y cajas. Usa primitivas y dependencias aisladas en ambos lados; no certifica RenderWare ni efectos visibles. El orden de las sobrecargas de FxPrim_c reproduce los slots del EXE.

Compilar build_probe.py con CL=/MP6 y ejecutar run_local.py con GTA cerrado. Los resultados aprobados se registran junto con los hashes del manifiesto; el estado global se restaura.

REA no produjo pseudocódigo válido para AddParticle con matriz (0x4AA540) ni UpdateBoundingBoxCB (0x4AA9A0); esas dos reconstrucciones se apoyan en el ensamblador completo del combo y la comparación diferencial.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 5.120 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fx-system-pending\20261007-201105-632762/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
