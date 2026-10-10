<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
