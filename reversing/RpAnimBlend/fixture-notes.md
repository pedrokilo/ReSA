<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Restricciones del banco

El primer intento entró en el callback original fuera de pantalla sin una sola secuencia válida, pero con extracción de velocidad activada. El propio callback original presupone una asociación válida y falló en 0x4D2E30. No se contabilizó como una comparación aprobada.

Se excluye extracción de velocidad cuando todas las jerarquías carecen de secuencias. También se excluyen clumps sin skin, fuera de pantalla y con extracción: el callback original fuera de pantalla interpreta el destino como datos de skin. Los casos sin skin fuera de pantalla se prueban sin extracción. Las variantes comprimidas con extracción usan Z, como espera el original.

Estos casos artificiales inválidos no son evidencia de un defecto del candidato ni se ocultan dentro de un resultado aprobado. El banco se restringe a estados coherentes con el contrato de los callbacks originales.

Un segundo ensayo confirmó que el callback skinned visible también requiere al menos una secuencia válida (fallo original en 0x4D2C62). El caso de jerarquía sin secuencias ahora se mezcla con dos asociaciones válidas; así se prueba que UpdateAnimations la omita sin violar la precondición del callback. La lista completamente vacía se comprueba por separado.

La revisión de independencia detectó que RpClumpClone deja los atomics apuntando a la jerarquía anterior. La sonda ahora sigue el patrón de CClumpModelInfo::CreateInstance: reasigna los atomics a la jerarquía clonada y crea su interpolador de animación. Exige direcciones de pose distintas entre original, candidato y jugador, entradas idénticas y pose del jugador intacta al terminar. El primer resultado de 652 comparaciones se descarta como validación final por esta falta de independencia.
