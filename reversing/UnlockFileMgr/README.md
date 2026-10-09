<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CFileMgr: trabajo aplazado

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**No validado; trece candados conservados.** Este banco no forma parte del catálogo de pruebas aprobadas.

El borrador y sus adaptaciones se conservan en el [parche de la entrega parcial](../UnlockBatch2/deferred-filemgr-game.patch), con [manifiesto y copias locales](../UnlockBatch2/deferred-manifest.json). Las fuentes compilables vuelven al estado anterior. No reconstruir esta sonda sin recuperar y revisar primero el borrador.

Última ejecución fallida: resultados-unlock-file-mgr/20260923-131422-691585. LoadFile, caso 1: el original devuelve 0 y el candidato 256. La causa sigue pendiente. El relleno del buffer de SetDirMyDocuments se corrigió antes de esa pasada. No se modificaron los flags del CSV.

[Estado y alcance autorizado](../UnlockBatch2/README.md).
