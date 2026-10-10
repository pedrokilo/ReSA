<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
