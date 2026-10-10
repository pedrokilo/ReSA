<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CRope pendientes

UpdateWeightInRope 0x5561B0 reconstruido desde combo completo de REA en `combo/5561B0.txt`, con límite de longitud, colocación lineal, seis iteraciones de restricciones y velocidades. Banco de 1024 pares: anclajes0..31, estirada/corta, longitudes y timestep positivos diversos. Compara retorno, todos los bytes del objeto y salida, y trazas de normalización sustituida EXE+DLL. Comprueba hook, actividad positiva y restauración de timestep+hooks.

Update 0x557530 reconstruido completo desde `combo/557530.txt`: movimiento, restricciones, control de grúas, cargas, matrices, escaneo de los tres pools y limpieza. Otros1024pares con cámara, entidades, modelos/colisiones y pools ficticios. Stubs EXE+DLL de referencias, posición, matrices/transformación, velocidad, normalización, controles, RNG, mundo, audio y release/remove. Actividad positiva y hashes de todos los bytes de entidades, matrices, pools, objeto y13regiones globales. El benchmark restablece esas regiones y los hooks.

Total2048pares. Ninguna función aprobada hasta compilación+runtime del agente principal.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.048 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-rope-pending\20261007-200726-600449/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
