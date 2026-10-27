<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de la radio del usuario

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**224 comparaciones aprobadas con Audio activo**, ambos hooks habilitados y sin bloqueo desde el arranque. [Resumen verificable](validation-summary.json).

192 casos de WriteUserTracksFile y 32 ejecuciones del hilo, comparando las cuatro combinaciones original/reversado del escáner y del hilo. Incluye archivos únicos (registro duplicado), carpetas vacías, subcarpetas, límite de profundidad, extensiones, archivos ignorados, enlaces a archivos y carpetas. Cada hilo termina bajo un plazo de diez segundos.

Se corrigieron orden de argumentos, vector de punteros del ejecutable y propiedad de memoria entre CRT. El adaptador de enlaces también devuelve memoria compatible con los callers nativos. Se retiró un CoUninitialize duplicado; se corrigieron enlaces a carpetas, extensión LNK y terminador de la ruta.

La comparación normaliza separadores y componentes de ruta, y verifica límites de cada registro; no afirma igualdad binaria de los archivos. Se conserva la mejora existente de rutas UTF-8 y recarga inmediata del índice en el hilo reversado, comprobada aparte. El original no realiza esa recarga. Las 32 ejecuciones repiten las 16 combinaciones de carpeta/escáner para comprobar su reutilización.

Fixtures privados dentro de artifacts, sin leer música personal. Se restauran directorio de trabajo, globals, ASI y ambos INI; partidas conservadas por hash. Solo SurfaceInfos excluido. No cubre errores de disco, agotamiento de memoria, decodificación, todas las rutas Unicode, concurrencia con escaneos externos ni estabilidad general.

Ejecución autónoma: `python reversing/UnlockRadio/run_local.py` (requiere los fixtures privados). Para recrearlos: `powershell -NoProfile -ExecutionPolicy Bypass -File reversing/UnlockRadio/prepare_fixtures.ps1`. Solo se crean archivos bajo artifacts/fixtures; no contienen audio real ni acceden a música personal.
