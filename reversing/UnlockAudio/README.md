<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de CAEAudioEnvironment

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**9.216 comparaciones exactas aprobadas con Audio activo**: 8.192 del vector y 1.024 del adaptador Placeable. [Resumen verificable](validation-summary.json).

El original recibe un buffer de salida y no garantiza devolverlo en EAX. Se corrigieron ambos adaptadores de ABI, manteniendo las funciones públicas que devuelven CVector. También se corrigieron la posición de cámara y la precisión/orden de las operaciones x87.

Casos con entrada nula, entrada y salida compartidas, distintos modos de cámara, jugador ausente, matrices opcionales y llamadas nativas/C++. Se comparan los doce bytes del resultado y los contadores de cada implementación. Se restauran cámara, hooks, ASI e INI; partidas comprobadas por hash. Solo SurfaceInfos excluido.

No cubre reproducción o mezcla de sonido, todas las configuraciones de coma flotante ni estabilidad general.

Ejecución autónoma: `python reversing/UnlockAudio/run_local.py`.
