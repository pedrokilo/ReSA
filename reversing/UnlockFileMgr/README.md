<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CFileMgr: trabajo aplazado

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**No validado; trece candados conservados.** Este banco no forma parte del catálogo de pruebas aprobadas.

El borrador y sus adaptaciones se conservan en el [parche de la entrega parcial](../UnlockBatch2/deferred-filemgr-game.patch), con [manifiesto y copias locales](../UnlockBatch2/deferred-manifest.json). Las fuentes compilables vuelven al estado anterior. No reconstruir esta sonda sin recuperar y revisar primero el borrador.

Última ejecución fallida: resultados-unlock-file-mgr/20260923-131422-691585. LoadFile, caso 1: el original devuelve 0 y el candidato 256. La causa sigue pendiente. El relleno del buffer de SetDirMyDocuments se corrigió antes de esa pasada. No se modificaron los flags del CSV.

[Estado y alcance autorizado](../UnlockBatch2/README.md).
