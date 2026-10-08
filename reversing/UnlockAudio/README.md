<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de CAEAudioEnvironment

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**9.216 comparaciones exactas aprobadas con Audio activo**: 8.192 del vector y 1.024 del adaptador Placeable. [Resumen verificable](validation-summary.json).

El original recibe un buffer de salida y no garantiza devolverlo en EAX. Se corrigieron ambos adaptadores de ABI, manteniendo las funciones públicas que devuelven CVector. También se corrigieron la posición de cámara y la precisión/orden de las operaciones x87.

Casos con entrada nula, entrada y salida compartidas, distintos modos de cámara, jugador ausente, matrices opcionales y llamadas nativas/C++. Se comparan los doce bytes del resultado y los contadores de cada implementación. Se restauran cámara, hooks, ASI e INI; partidas comprobadas por hash. Solo SurfaceInfos excluido.

No cubre reproducción o mezcla de sonido, todas las configuraciones de coma flotante ni estabilidad general.

Ejecución autónoma: `python reversing/UnlockAudio/run_local.py`.
