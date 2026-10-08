<!-- RESA-CURRENT-STATUS:BEGIN -->
> Consulte [el estado y alcance actual](../../docs/CurrentStatus.md).
<!-- RESA-CURRENT-STATUS:END -->

# Corrección del veredicto del runner

El replay `20261008-124241-603231` completó 201552 comparaciones y salió naturalmente con código 0, pero tuvo **590 diferencias**. Su `passed=true` original es un veredicto inválido: el runner sobrescribía el `passed=false` del analizador. Los resúmenes originales permanecen intactos; [la auditoría adicional](runs/20261008-124241-603231/verdict-audit.json) invalida exclusivamente esa aprobación.

El runner corregido conserva el veredicto explícito y los conteos, junto con la salida natural. Cuatro pruebas aisladas pasan, incluyendo el caso de 590 diferencias que debe rechazarse. El runner previo quedó archivado por su hash. La revisión de las 25 identidades nuevas vinculadas al registro no encontró evidencias con diferencias o mismatches positivos ni all_equal=false. Ninguna función CPad se registró a partir de este veredicto falso.
