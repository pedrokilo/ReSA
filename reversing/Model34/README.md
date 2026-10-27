<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CBaseModelInfo (lista de 34)

Preparado el 04/10/2026: **13.280 pares original/candidato dentro de GTA** para las 33 funciones pendientes de `CBaseModelInfo` (9 por la vtable 0x85BB9C). `SwaysInWind` ya estaba validada (banco `CBaseModelInfo`).

| Grupo | Funciones |
|---|---|
| Virtuales (vtable 0x85BB9C, 15 slots) | `AsAtomicModelInfoPtr` (1), `AsDamageAtomicModelInfoPtr` (2), `AsLodAtomicModelInfoPtr` (3), `GetTimeInfo` (5), `Init` (6), `Shutdown` (7), `SetAnimFile` (12), `ConvertAnimFileIndex` (13), `GetAnimFileIndex` (14) |
| TXD y referencias | `SetTexDictionary`, `ClearTexDictionary`, `AddTexDictionaryRef`, `RemoveTexDictionaryRef`, `AddRef`, `RemoveRef` |
| Colisión y 2DFX | `SetColModel`, `DeleteCollisionModel`, `Init2dEffects`, `Get2dEffect`, `Add2dEffect` |
| Flags | `GetIsDrawLast`, `HasBeenPreRendered`, `HasComplexHierarchy`, `IsBackfaceCulled`, `IsLod`, `IsRoad`, `IsTagModel`, `IsBreakableStatuePart`, `SetHasBeenPreRendered`, `SetIsLod`, `SetOwnsColModel`, `IncreaseAlpha`, `SetBaseModelInfoFlags` |

**Diferencias reales corregidas:**
- Las consultas devuelven el bit enmascarado en EAX completo, no un `bool` en AL: `IsLod` 0x80, `IsRoad` 0x100, `HasComplexHierarchy` 0x200, `GetIsDrawLast` 2, `IsBackfaceCulled` 0x40, `HasBeenPreRendered` 1. `IsTagModel` e `IsBreakableStatuePart` devuelven 0/1 en EAX completo.
- `SetIsLod`, `SetOwnsColModel` y `SetHasBeenPreRendered` prueban el dword completo del argumento (`test eax,eax`). `SetHasBeenPreRendered` copiaba solo el bit 0 (un 2 lo apagaba).
- `SetTexDictionary` guarda el slot y compara solo sus 16 bits con -1 (`cmp ax,0FFFFh`) antes de llamar a `AddTxdSlot`.
- `Get2dEffect`:
  - el conteo es un int de 32 bits (la DLL lo truncaba a `uint8` y, si quedaba negativo, tomaba el store en vez de la geometría);
  - `GetRwModelType` se consulta antes de mirar el RwObject, y otra vez si no es atómico con objeto;
  - sin el `assert` de `GetRwObject`/`GetItemAtIndex`.
- `Add2dEffect` calcula el índice como `(efecto - store - 4) >> 6` (aritmético), como el original.

**Sin cambios (coinciden con el EXE):** las virtuales triviales, `Init`, `Shutdown`, `SetColModel` (el bucle del EXE equivale a tratar el modelo horario emparejado), `DeleteCollisionModel`, `Init2dEffects`, las referencias de TXD, `IncreaseAlpha` y `SetBaseModelInfoFlags` (la envoltura desnuda conserva EDX).

**Método:**
- Model infos falsos en buffers del banco, con una vtable falsa que traza `GetTimeInfo`, `DeleteRwObject` y `GetRwModelType` (con el estado del objeto en el momento de la llamada).
- Stubs trazados (EXE + DLL) de `CTxdStore::FindTxdSlot`, `AddTxdSlot`, `AddRef` y `RemoveRef`, `Get2DEffectAtomic` y `CColModel::operator delete`.
- El destructor de `CColModel` corre de verdad sobre un `CColModel` falso sin datos (`m_pColData` nulo).
- El plugin 2DFX, inline en el EXE, se rellena en una geometría falsa en el offset de 0xC3A1E0.
- `SetColModel` usa un slot libre de `ms_modelInfoPtrs` para el modelo emparejado; se restaura en cada ejecución.
- `SetBaseModelInfoFlags` se llama con EDX centinela y se compara EDX a la salida.
- Fuera de los fixtures:
  - camino de geometría con plugin o geometría nulos (el EXE lee memoria nula);
  - modelo emparejado con índice no válido.

Repetir: `python reversing/Model34/build_probe.py` y `python reversing/run.py --case model34`.
