<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de interiores (lista de 21)

Validadas el 03/10/2026 con **10.112 pares original/candidato dentro de GTA**, todos exactos: 21 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `InteriorManager_c` | `ActivatePeds`, `AddSameGroupEffectInfos`, `AreAnimsLoaded`, `Exit`, `FindStealableObjectId` (entidad y datos), `GetBoundingBox`, `GetInteriorFromPool`, `GetPedsInterior`, `GetPedsInteriorGroup`, `GetVectorsInterior`, `GetVisibleEffects`, `HasInteriorHadStealDataSetup`, `Init`, `IsGroupActive`, `IsInteriorEffectVisible`, `PruneVisibleEffects`, `ReturnInteriorToPool`, `SetEntryExitPtr`, `SetStealableObjectStolen`, `Update` |

**Diferencias reales corregidas:**
- `SetStealableObjectStolen` guarda el byte del argumento tal cual (`mov byte ptr`); la DLL lo convertía en `bool` y escribía 1.
- `Update`:
  - sin tarea activa no se buscan efectos (antes sí); `GetActiveTask` se llama dos veces;
  - grupo y efecto se asocian por `m_groupId` (+0xD), no por `m_id`;
  - el identificador del interior es `ftol((z·y)·x)` (orden del EXE, truncado a 64 bits);
  - si el pool de interiores se vacía el bucle continúa.
- `GetVisibleEffects`:
  - el filtro de duplicados estaba invertido (guardaba sólo los repetidos);
  - no sale al llenarse el buffer y con máximo 0 también busca;
  - sólo escribe entidad, efecto, índice, número y `IsCulled`; `AddSameGroupEffectInfos` en orden ascendente.
- `AddSameGroupEffectInfos`: sin la salida NOTSA con 7 efectos; límite de 8 sin cortar el bucle.
- `PruneVisibleEffects`: ordenación por intercambio del EXE (no estable), distancias en orden ascendente, NaN se descarta, contadores con signo.
- `IsInteriorEffectVisible`: caja alineada en el mundo con las 8 esquinas transformadas (no la caja local), límites inclusivos y las tres comprobaciones siempre.
- `GetPedsInterior` busca sólo en el grupo del peatón; `GetVectorsInterior` y `GetBoundingBox` recorren los 8 huecos; `GetBoundingBox` pasa antes por `GetFurnitureEntity`.
- `SetEntryExitPtr` guarda siempre el puntero y el rectángulo (sólo el reinicio depende de que cambie).
- `Init` devuelve `true`, usa el pool del global y deja `m_TimeLastPickupsGenerated = -180000`.
- `IsGroupActive` (movsx), `GetPedsInteriorGroup`, `HasInteriorHadStealDataSetup` y `FindStealableObjectId` con contadores con signo; `AreAnimsLoaded` lee el bloque sin límites y compara con 1.
- `Interior_c::IsPtInside` (llamada al EXE): el sesgo va por valor (`ret 10h`); por referencia desplazaba la pila.

**Método:**
- Se trabaja sobre el `g_interiorMan` real (0xBAF670) con listas, grupos, interiores y objetos generados por caso; se guarda y restaura en cada ejecución junto con `g_furnitureMan`, `CGame::currArea` y cuatro entradas de `ms_modelInfoPtrs`.
- Entidades, modelos, colisiones, efectos 2DFX, peatones, inteligencia, tarea y entradas/salidas son falsos.
- Se simulan y trazan `InteriorGroup_c` (Init/Update/AddInterior/Setup/Exit), `Interior_c` (Init/IsPtInside/GetFurnitureEntity/GetBoundingBox), `FurnitureManager_c::Exit/LoadFurniture`, `CWorld::FindObjectsInRange`, `Get2dEffect`, `GetAnimationBlockIndex` y `FindPlayerPed`; nunca se crean objetos ni peatones.
- En `Update` los pares entidad/grupo activos son únicos para que el pool de grupos nunca se agote (el EXE no lo comprueba).

Repetir: `python reversing/Interior21/build_probe.py` y `python reversing/run.py --case interior21`. Evidencia: [validation-summary.json](validation-summary.json).
