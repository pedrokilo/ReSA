<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# App, CIniFile y CIplDefPool

Validadas el 02/10/2026 con **3.008 pares original/candidato dentro de GTA**, todos exactos: las 7 funciones de `App`, `CIniFile::LoadIniFile` y `CIplDefPool::New`. 9/9 con `validated=1`.

**Diferencias reales corregidas:**
- `DoRWStuffStartOfFrame`: faltaba `CFont::SetAlphaFade(255)`; la cámara solo limpia Z (la variante del horizonte también el stencil).
- `DefinedState`: el color de niebla se compone en bruto desde los valores de 16 bits con alfa 0xFF; la DLL lo daba con alfa 0.
- `PluginAttach`: el resultado de `RpAnisotPluginAttach` se ignora en el original (los siguientes plugins se registran igual).
- `CIplDefPool::New`: política original del pool (busca a partir del siguiente al último hueco y no rellena la memoria); usa `NewLegacy`, como `CEntryInfoNodePool`.

**Método:** las funciones de `App` son orquestación. Se interceptan todas sus dependencias (cámara, sprites, nubes, depuración, eventos, registro de plugins, estado de render y `CFileMgr`) y se compara la secuencia exacta de llamadas y argumentos, incluido el contenido de los temporales apuntados. No se ejecutan render ni registro reales. `CIplDefPool` se prueba sobre un pool propio.

Repetir: `python reversing/AppBatch/build_probe.py` y `python reversing/run.py --case app`. Evidencia: [validation-summary.json](validation-summary.json).
