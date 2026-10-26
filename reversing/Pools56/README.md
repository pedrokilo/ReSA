<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Pools y clases sueltas (lista de 56)

Validadas el 02/10/2026 con **13.056 pares original/candidato dentro de GTA**, todos exactos: 23 funciones con `validated=1`.

| Funciones | Clases |
|---|---|
| `New` (14) | `CBuildingPool`, `CColModelPool`, `CDummyPool`, `CEntryInfoNodePool`, `CEventPool`, `CNodeRoutePool`, `CPedAttractorPool`, `CPedIntelligencePool`, `CPointRoutePool`, `CPtrNodeDoubleLinkPool`, `CPtrNodeSingleLinkPool`, `CTaskAllocatorPool`, `CTaskPool`, `CVehiclePool` |
| Sueltas (9) | `CDummy::IsDummyPointerValid`, `CEntryInfoList::Flush`, `CPedTaskPair::Flush`, `CStaticShadow::Free`, `CConversationNode::ClearRecursively`, `CTaskTimer::IsOutOfTime`, `CPathNode::GetPosition`, `CColLine::Set`, `FxEmitterPrt_c::operator new[]` |

**Diferencias reales corregidas:**
- Los 14 `New`:
  - el original busca desde `m_nFirstFree + 1`, da una sola vuelta (si el pool está lleno deja `m_nFirstFree = 0` y devuelve nulo);
  - incrementa el ID de 7 bits;
  - no escribe en el objeto (la DLL lo rellenaba con 0xCD).
  
  Ahora cada pool usa `NewLegacy`.
- `CPedAttractorPool`: los slots del pool del EXE son de 0xC4 bytes y la DLL usaba `sizeof(CPedAttractor)` (0x8C), así que indexaba mal el pool real. Ahora el almacenamiento es de 0xC4.
- `CDummy::IsDummyPointerValid`: el original solo comprueba el rango del índice (con su `<=` desfasado) y no mira el mapa de ocupación.
- `CStaticShadow::Free` pone a cero el grupo de polígonos y el ID siempre, no solo cuando había polígonos.

**Método:**
- Pools falsos con mapa de bytes aleatorio (lleno, vacío, un hueco, IDs en 0x7F); nunca los pools reales.
- Para `IsDummyPointerValid`, `CEntryInfoList::Flush` y `ClearRecursively` se sustituyen temporalmente el pool o el array globales, y se restauran.
- `FxMemoryPool_c::GetMem` solo se simula durante la llamada.

Repetir: `python reversing/Pools56/build_probe.py` y `python reversing/run.py --case pools56`. Evidencia: [validation-summary.json](validation-summary.json).
