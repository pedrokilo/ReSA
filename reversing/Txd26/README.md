<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Almacén de diccionarios de texturas (lista de 25)

Validadas el 04/10/2026 con **8.064 pares original/candidato dentro de GTA**, todos exactos: 24 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTxdStore` | `Initialise`, `Shutdown`, `GameShutdown`, `StartLoadTxd`, `FinishLoadTxd`, `LoadTxd` (stream y fichero), `PushCurrentTxd`, `PopCurrentTxd`, `SetCurrentTxd`, `FindTxdSlot` (nombre y hash), `GetTxd`, `GetParentTxdSlot`, `Create`, `RemoveTxd`, `AddRef`, `RemoveRef`, `RemoveRefWithoutDelete`, `GetNumRefs`, `SetupTxdParent`, `TxdStoreFindCB`, `TxdStoreLoadCB`, `RemoveIfRefCountIsGreaterThanOne` |

**Diferencias reales corregidas:**
- `StartLoadTxd` y `LoadTxd(stream)` buscan siempre el chunk (`RwStreamFindChunk`) aunque el slot esté libre; la DLL devolvía falso sin llamarlo.
- Como el EXE, ninguna función comprueba el slot (el EXE desreferencia el `nullptr` de un slot libre):
  - `Create` llama siempre a `RwTexDictionaryCreate`;
  - `SetCurrentTxd`, `GetTxd`, `GetParentTxdSlot`, `GetNumRefs`, `AddRef`, `RemoveRef`, `RemoveRefWithoutDelete`, `RemoveTxd` y `FinishLoadTxd` ya no devuelven `nullptr`/-1/0 ni salen antes;
  - `SetupTxdParent` no comprueba el slot del padre.
- `Initialise` y `Shutdown` construyen y destruyen el pool como el original (0x731AD0 y en línea): `operator new`/`delete` simples, sin relleno 0xFD del almacenamiento, sin escribir el byte +0x11, con primer libre 0 al destruir y sin borrar el puntero global.
- `LoadTxd(fichero)` usa `sprintf`, no `sprintf_s`.

**Método:**
- Pool falso de `TxdDef` (8–64 slots; 5000 en `GameShutdown`, cuyo bucle recorre 5000 slots fijos) con ~1/2 de slots ocupados, padres -1 u ocupados y hashes de una lista de nombres; el puntero global del pool se cambia solo durante el caso y se restaura.
- Diccionarios y texturas falsos; el padre se guarda en el desplazamiento real del plugin (`ms_txdPluginOffset`), sin ciclos.
- Se guardan y restauran los globales de `CTxdStore`: pool, diccionario guardado, último slot encontrado y `defaultTxds`.
- Stubs trazados (sin reservas) en la dirección del EXE (los símbolos RW de la DLL son envoltorios):
  - `RwTexDictionaryGetCurrent/SetCurrent/FindNamedTexture/Create/Destroy/ForAllTextures`, `RwTextureDestroy`, `RwTexDictionaryRemoveTexture`;
  - `RwStreamFindChunk/Open/Close` y `RwTexDictionaryGtaStreamRead/1/2` (nunca se carga un TXD real);
  - `RwTextureSetFindCallBack/SetReadCallBack`, `CStreaming::RemoveModel` (EXE + DLL) y `operator new/delete` con un asignador lineal, solo en `Initialise` y `Shutdown`.
- Las retrollamadas se normalizan (dirección del EXE o símbolo de la DLL): con hooks son equivalentes.
- Slots libres solo donde el EXE no escribe (`StartLoadTxd`/`LoadTxd(stream)` sin chunk).

Repetir: `python reversing/Txd26/build_probe.py` y `python reversing/run.py --case txd26`. Evidencia: [validation-summary.json](validation-summary.json).
