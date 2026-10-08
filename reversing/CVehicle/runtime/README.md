<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prueba local de CVehicle

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Ejecuta PROBAR.cmd --case cvehicle en la carpeta principal. No necesita Codex abierto, conexion de red, API ni tokens. Mantiene el equipo encendido y cierra otras sesiones de GTA antes de iniciarla.

El ejecutor usa exclusivamente D:\JUEGOS\GTA San Andreas - CVehicle Test. Comprueba los hashes del ejecutable y del ASI instrumentado, conserva el ASI normal, activa temporalmente la prueba y abre el juego. Compara 60 pares de vehiculos y 3 casos adicionales con las funciones originales. Rechaza diferencias, errores, referencias residuales y resultados incompletos. Cierra su proceso tras terminar o alcanzar 120 segundos y restaura el ASI normal.

Informes: carpeta resultados-cvehicle dentro de la instalacion de pruebas. Cada ejecucion genera RESULTADO.txt y summary.json, ademas del registro cuando existe. No reintenta ni modifica codigo automaticamente. Ante un fallo, comparte RESULTADO.txt y summary.json para diagnosticarlo.

Verificado el 22/09/2026: ejecucion independiente aprobada, 60+3 comparaciones, 948 llamadas instrumentadas y cero diferencias de referencias. Tambien se verifico el rechazo de registros vacios, incompletos, duplicados, con assertion y con referencias alteradas.

Alcance: comparaciones de estructuras y contadores dentro del motor real; no certifica aspecto visual ni estabilidad general. Se desactivan Audio y Global/SurfaceInfos_c debido a fallos observados fuera del objetivo. La configuracion reducida con --unhook-except fallo durante el arranque y no se usa.

Los binarios de prueba permanecen locales en _baseline y no se publican. El ejecutor contiene las rutas de esta computadora. LiveComparison.hpp y RuntimeProbe.hpp documentan la instrumentacion empleada; el ASI normal no contiene el ejecutor dentro del juego.
