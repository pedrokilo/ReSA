<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de atractores y matrículas (lista de 14)

Validadas el 02/10/2026 con **11.904 pares original/candidato dentro de GTA**, todos exactos: 28 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPedAttractorManager` | `BroadcastArrival`, `BroadcastDeparture`, `DeRegisterPed`, `GetPedAttractorManager`, `GetPedUsingEffect`, `GetRelevantAttractor`, `HasEmptySlot`, `HasQueueTailArrivedAtSlot`, `IsApproachable`, `IsPedRegisteredWithEffect` (peatón y efecto), `RegisterPedWithAttractor`, `RemoveEffect`, `RestoreStuffFromMem` |
| `CCustomCarPlateMgr` | `Initialise`, `Shutdown`, `GeneratePlateText`, `GetMapRegionPlateDesign`, `CreatePlateTexture`, `RenderLicenseplateTextToRaster`, `SetupMaterialPlatebackTexture`, `SetupMaterialPlateTexture`, `SetupClump`, `SetupClumpAfterVehicleUpgrade`, `MaterialSetCarplateTextureCB`, `MaterialUpgradeSetCarplateTextureCB`, `AtomicSetCarplateTextureCB`, `AtomicUpgradeSetCarplateTextureCB` |

**Diferencias reales corregidas:**
- `SArray` (vector de MSVC del EXE):
  - `erase` usaba `move_backward` y escribía delante del elemento borrado; ahora desplaza hacia delante;
  - el crecimiento es capacidad + capacidad / 2 (mínimo tamaño + 1), como `vector::_Insert_n` (0x5EDA50);
  - reserva y libera con `operator new` / `operator delete` (no `new[]` / `delete[]`) y no libera punteros nulos;
  - el constructor no escribe el campo del allocator (0x5EDF10).
- `CPedAttractorManager`:
  - `BroadcastDeparture` y `DeRegisterPed` borran el atractor (destructor virtual) cuando se queda sin peatones, además de quitarlo de la lista; `BroadcastDeparture` va por la vtable;
  - `GetRelevantAttractor` busca primero en los cajeros y luego en los asientos;
  - `IsApproachable`: dos productos escalares separados con el orden de sumas del EXE y, en vez de una distancia mínima, la dirección 2D normalizada hacia el peatón debe dar un producto > 0,25 con la dirección del efecto; matrices con `FromMultiply`/`Multiply3x3` del EXE;
  - `RegisterPedWithAttractor`, `HasEmptySlot` y `HasQueueTailArrivedAtSlot` comprueban el tipo de efecto en lugar del `assert` (nulo / false / true según el EXE); si el pool está lleno se añade un nulo a la lista y se devuelve nulo (antes `notsa::cast` hacía assert con el nulo);
  - los tipos de atractor fuera de 0..9 devuelven el valor fijo del EXE en lugar de `NOTSA_UNREACHABLE`;
  - `GetPedAttractorManager` crea el gestor con `operator new` en 0xC09840 (antes un estático de la DLL);
  - `RestoreStuffFromMem` estaba sin hacer: vacía las 10 listas y las rellena con el pool, del último slot al primero.
- `CCustomCarPlateMgr`:
  - `GeneratePlateText`: letras A..W y cifras 0..8 con la fórmula del EXE (`rand() / 32768 * -23` y `-9`) y longitud con signo;
  - `MaterialUpgradeSetCarplateTextureCB` compara con `"carplate"` (la DLL tenía `"careplate"`);
  - `Shutdown` llama a `CTxdStore::RemoveTxd`, no a `RemoveTxdSlot`;
  - `GetMapRegionPlateDesign` devuelve 0 para cualquier otra región; `SetupClumpAfterVehicleUpgrade` y `SetupMaterialPlateTexture` sin `assert` con material nulo.
- `CPedPizzaAttractor` (constructor 0x5EE6E0, usado por `RegisterPedWithAttractor`): 5 peatones como máximo y radio de llegada 0,2 (la DLL tenía 1 y 0,125).

**Método:**
- El gestor corre sobre un gestor falso y un pool de atractores falso (20 slots de 0xC4 colocado en 0xB744C4) con atractores de vtable propia (`GetType`, destructor y `BroadcastDeparture` trazados).
- `operator new`/`delete` (y las versiones de arrays) van a una arena fija solo durante la llamada; los atractores nuevos se construyen de verdad en el pool falso y se comparan por tipo, efecto, entidad, listas, modo y script.
- Las primitivas RW y `CTxdStore` se simulan con texturas, rasters, materiales, geometrías, atomics y clumps falsos; las iteraciones llaman de verdad a la callback.
- La corrección reversible NOTSA `CCustomCarPlateMgr_GeneratePlateText_MissingLettersAndDigits` (activa por defecto, letras A..Z y cifras 0..9) se desactiva durante el banco y se restaura al final.
- Se guardan y restauran 0xB744C4, 0xC09840, las globales 0xC3EF5C..0xC3EF84 y `CWeather::WeatherRegion`.
- Fuera de los fixtures:
  - `pCharsetTex` nulo fuera de `Shutdown` (el EXE lee su raster sin comprobar);
  - efecto nulo en `GetPedUsingEffect` y `RemoveEffect` (el EXE se cae);
  - el valor de retorno de `SetupMaterialPlateTexture` y `SetupMaterialPlatebackTexture` (void en la DLL; los llamadores del EXE lo ignoran).

Repetir: `python reversing/Mix14/build_probe.py` y `python reversing/run.py --case mix14`. Evidencia: [validation-summary.json](validation-summary.json).
