<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueos de geometría, recursos y memoria

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**7.968 pares exactos aprobados**, con nueve hooks habilitados y sin bloqueo desde el arranque. [Resumen verificable](validation-summary.json).

| Función | Pares |
| --- | ---: |
| CCover::FindDirFromVector | 4.096 |
| CCover::FindVectorFromDir | 256 |
| CEntryExitManager::GetPositionRelativeToOutsideWorld | 1.024 |
| CDecisionMakerTypesFileLoader::UnloadDecisionMaker | 1.024 |
| CEntryInfoNode::operator new / delete | 512 + 512 |
| CCurves::TestCurves | 16 |
| CVehicleRecording::Render | 16 |
| CEntryInfoNodePool::New | 512 |

Ocho candados retirados tras contrastar el original. La novena función es una dependencia: la entrada del pool estaba registrada como GetAt, pero corresponde a New. Su política de cursor, referencias y liberación se conserva en el pool de EntryInfoNode sin alterar el comportamiento de todos los pools.

Se corrigieron la ABI uint8 y el cálculo x87 de CCover, la llamada thiscall de RemoveDecisionMaker y un hook duplicado de VehicleRecording::Render que impedía conmutarlo. TestCurves y Render son retornos vacíos en el original; las pruebas matemáticas anteriores de curvas se conservan como RunLocalTests.

El banco compara resultados, bytes, tablas y contadores, y restaura el estado usado. Audio y SurfaceInfos están excluidos. No cubre agotamiento de memoria, todos los modos de coma flotante, interiores completos ni estabilidad general. Los fallos de arranque anteriores quedaron archivados y no cuentan como aprobados.

Ejecución autónoma: `python reversing/UnlockCore/run_local.py`. Las fuentes de instrumentación se restauran tras compilar; el ejecutor repone ASI, PDB y ambos INI, verificando las partidas por hash.
