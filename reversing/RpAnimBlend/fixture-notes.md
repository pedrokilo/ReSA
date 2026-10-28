<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Restricciones del banco

El primer intento entró en el callback original fuera de pantalla sin una sola secuencia válida, pero con extracción de velocidad activada. El propio callback original presupone una asociación válida y falló en 0x4D2E30. No se contabilizó como una comparación aprobada.

Se excluye extracción de velocidad cuando todas las jerarquías carecen de secuencias. También se excluyen clumps sin skin, fuera de pantalla y con extracción: el callback original fuera de pantalla interpreta el destino como datos de skin. Los casos sin skin fuera de pantalla se prueban sin extracción. Las variantes comprimidas con extracción usan Z, como espera el original.

Estos casos artificiales inválidos no son evidencia de un defecto del candidato ni se ocultan dentro de un resultado aprobado. El banco se restringe a estados coherentes con el contrato de los callbacks originales.

Un segundo ensayo confirmó que el callback skinned visible también requiere al menos una secuencia válida (fallo original en 0x4D2C62). El caso de jerarquía sin secuencias ahora se mezcla con dos asociaciones válidas; así se prueba que UpdateAnimations la omita sin violar la precondición del callback. La lista completamente vacía se comprueba por separado.

La revisión de independencia detectó que RpClumpClone deja los atomics apuntando a la jerarquía anterior. La sonda ahora sigue el patrón de CClumpModelInfo::CreateInstance: reasigna los atomics a la jerarquía clonada y crea su interpolador de animación. Exige direcciones de pose distintas entre original, candidato y jugador, entradas idénticas y pose del jugador intacta al terminar. El primer resultado de 652 comparaciones se descarta como validación final por esta falta de independencia.
