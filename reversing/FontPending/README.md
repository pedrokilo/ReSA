<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CFont: funciones pendientes

Reconstrucción de PrintChar (0x718A10), FindSubFontCharacter (0x7192C0), RenderFontBuffer (0x719840) y ProcessCurrentString (0x71A220), cotejada con pseudocódigo REA y ensamblador del EXE 1.0 US en `combo/`.

El banco compara 6.144 pares con el hook conmutado: 2.048 PrintChar, 1.024 FindSubFontCharacter, 1.024 RenderFontBuffer y 2.048 ProcessCurrentString. Incluye estilos, texturas, símbolos, tokens, saltos, ajuste de línea y coordenadas límite/NaN. Las llamadas de dibujo y dependencias no incluidas se sustituyen en ambos lados; no certifica el resultado visual completo. Regiones globales, cadenas y dependencias se restauran al terminar.

Compilación: `python reversing/FontPending/build_probe.py` con CL=/MP6. Ejecución: `python reversing/FontPending/run_local.py`, con GTA cerrado. El manifiesto de la sonda y validation-summary.json vinculan fuentes, binarios y resultados; únicamente una ejecución aprobada se registra como validación.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 6.144 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-font-pending\20261007-201033-210407/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
