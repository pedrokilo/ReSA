<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
