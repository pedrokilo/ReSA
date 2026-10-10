<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Corrección del veredicto del runner

El replay `20261008-124241-603231` completó 201552 comparaciones y salió naturalmente con código 0, pero tuvo **590 diferencias**. Su `passed=true` original es un veredicto inválido: el runner sobrescribía el `passed=false` del analizador. Los resúmenes originales permanecen intactos; [la auditoría adicional](runs/20261008-124241-603231/verdict-audit.json) invalida exclusivamente esa aprobación.

El runner corregido conserva el veredicto explícito y los conteos, junto con la salida natural. Cuatro pruebas aisladas pasan, incluyendo el caso de 590 diferencias que debe rechazarse. El runner previo quedó archivado por su hash. La revisión de las 25 identidades nuevas vinculadas al registro no encontró evidencias con diferencias o mismatches positivos ni all_equal=false. Ninguna función CPad se registró a partir de este veredicto falso.
