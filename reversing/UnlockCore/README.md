<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
