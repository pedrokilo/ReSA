<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prueba local de CVehicle

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Ejecuta PROBAR.cmd --case cvehicle en la carpeta principal. No necesita Codex abierto, conexion de red, API ni tokens. Mantiene el equipo encendido y cierra otras sesiones de GTA antes de iniciarla.

El ejecutor usa exclusivamente D:\JUEGOS\GTA San Andreas - CVehicle Test. Comprueba los hashes del ejecutable y del ASI instrumentado, conserva el ASI normal, activa temporalmente la prueba y abre el juego. Compara 60 pares de vehiculos y 3 casos adicionales con las funciones originales. Rechaza diferencias, errores, referencias residuales y resultados incompletos. Cierra su proceso tras terminar o alcanzar 120 segundos y restaura el ASI normal.

Informes: carpeta resultados-cvehicle dentro de la instalacion de pruebas. Cada ejecucion genera RESULTADO.txt y summary.json, ademas del registro cuando existe. No reintenta ni modifica codigo automaticamente. Ante un fallo, comparte RESULTADO.txt y summary.json para diagnosticarlo.

Verificado el 22/09/2026: ejecucion independiente aprobada, 60+3 comparaciones, 948 llamadas instrumentadas y cero diferencias de referencias. Tambien se verifico el rechazo de registros vacios, incompletos, duplicados, con assertion y con referencias alteradas.

Alcance: comparaciones de estructuras y contadores dentro del motor real; no certifica aspecto visual ni estabilidad general. Se desactivan Audio y Global/SurfaceInfos_c debido a fallos observados fuera del objetivo. La configuracion reducida con --unhook-except fallo durante el arranque y no se usa.

Los binarios de prueba permanecen locales en _baseline y no se publican. El ejecutor contiene las rutas de esta computadora. LiveComparison.hpp y RuntimeProbe.hpp documentan la instrumentacion empleada; el ASI normal no contiene el ejecutor dentro del juego.
