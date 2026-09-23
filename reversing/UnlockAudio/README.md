# Desbloqueo de CAEAudioEnvironment

**9.216 comparaciones exactas aprobadas con Audio activo**: 8.192 del vector y 1.024 del adaptador Placeable. [Resumen verificable](validation-summary.json).

El original recibe un buffer de salida y no garantiza devolverlo en EAX. Se corrigieron ambos adaptadores de ABI, manteniendo las funciones públicas que devuelven CVector. También se corrigieron la posición de cámara y la precisión/orden de las operaciones x87.

Casos con entrada nula, entrada y salida compartidas, distintos modos de cámara, jugador ausente, matrices opcionales y llamadas nativas/C++. Se comparan los doce bytes del resultado y los contadores de cada implementación. Se restauran cámara, hooks, ASI e INI; partidas comprobadas por hash. Solo SurfaceInfos excluido.

No cubre reproducción o mezcla de sonido, todas las configuraciones de coma flotante ni estabilidad general.

Ejecución autónoma: `python reversing/UnlockAudio/run_local.py`.
