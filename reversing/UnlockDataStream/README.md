<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEDataStream: seis hooks conmutables

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Estado: **6/6 desbloqueados**, validados dentro de GTA en la copia de pruebas. [Resumen verificable](validation-summary.json).

El bloqueo protegía una incompatibilidad real: el original guardaba un `HANDLE` de Windows y la reconstrucción un `FILE*` de otro runtime. Ahora ambos lados comparten `HANDLE`, apertura, lectura, posición y cierre. También se adaptó `Stat`; el constructor conserva el identificador sin inicializar hasta `Initialise`, como el original, y el destructor solo cierra cuando corresponde.

La pasada final aprobó **6.144 comparaciones**: constructor 256, destructor 256, apertura 512, lectura 2.048, posición 512, búsqueda 2.048 y `Stat` 512. Incluye archivos ausentes, límites, lectura cifrada, búsquedas fallidas, indicadores de apertura y buffers completos. Se comprobaron además las **64 combinaciones** de los seis hooks originales/reconstruidos durante un ciclo completo de uso. La primera pasada no suma cobertura adicional.

Audio estuvo activo; solo se excluyó SurfaceInfos. Los contadores prueban entrada real en el candidato y ausencia de esa entrada al comparar el original. Se restauraron hooks, estado, configuración y partidas; el ejecutor terminó automáticamente. Al terminar el banco se restauró el ASI que estaba instalado. La instalación normal posterior se documenta en la [entrega parcial](../UnlockBatch2/README.md).

Limitaciones: fixtures ASCII y operaciones acotadas. `CreateFileW` mantiene la ampliación UTF-8, sin afirmar equivalencia Unicode con `CreateFileA`. No se han probado todos los decodificadores, todos los métodos IStream ni una sesión prolongada. No garantiza estabilidad general del juego.

Reproducción: `build_probe.py`, `archive_probe.py` y `run_local.py`, desde la raíz del repositorio. La compilación restaura las fuentes instrumentadas incluso si falla; el ejecutor restaura el ASI/INI de pruebas. No hay otro PROBAR.cmd.
