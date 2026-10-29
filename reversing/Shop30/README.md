<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tiendas (lista de 30)

Validadas el 04/10/2026 con **9.024 pares original/candidato dentro de GTA**, todos exactos: 30 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CShopping` | `Buy`, `FindItem`, `FindSection`, `FindSectionInSection`, `GetExtraInfo`, `GetItemIndex`, `GetKey`, `GetNameTag`, `GetNextSection`, `GetPrice`, `GetPriceSectionFromName`, `HasPlayerBought`, `IncrementStat`, `IncrementStat2`, `Init`, `Load`, `LoadPrices`, `LoadShop`, `LoadStats`, `RemoveLoadedPrices`, `RemoveLoadedShop`, `RestoreClothesState`, `RestoreVehicleMods`, `Save`, `SetCurrentProperty`, `SetPlayerHasBought`, `ShutdownForRestart`, `StoreClothesState`, `StoreVehicleMods`, `UpdateStats` |

**Diferencias reales corregidas:**
- `FindItem`, `GetItemIndex` y `GetPriceSectionFromName` devuelven -1 si no encuentran nada (la DLL abortaba con `NOTSA_UNREACHABLE`); `GetPrice`, `GetNameTag`, `GetExtraInfo`, `HasPlayerBought`, `SetPlayerHasBought`, `UpdateStats` y `Buy` usan ese índice -1 como el EXE (`.data()[]`).
- `GetPrice`: `fild` del precio con signo × multiplicador del nivel y `_ftol2` (truncado de 64 bits).
- `IncrementStat2`: con -1 sale sin hacer nada (la DLL tenía un `assert`) y compara la estadística como dword completo (no `uint16`).
- `FindSection`, `GetNextSection`, `LoadPrices`, `LoadShop` y `LoadStats` usan `strtok`, `strncpy` y `strcpy` del EXE (sin variantes `_s`; `strncpy_s` fallaba con etiquetas de 8 caracteres).
- `LoadPrices`:
  - las líneas vacías o de comentario ya no cuentan como precio;
  - recorre todos los modificadores de precio (gana el último que coincide).
- `LoadShop`: `FindPlayerVehicle` solo se llama para `CarMods` y después de `GetKey`.
- `LoadStats`: `GetKey` está inline y en `CarPaintJobs` no escribe la clave (se repite la anterior); secciones desconocidas (-1) como el EXE.
- `Buy`:
  - el índice absoluto sale de la clave del precio encontrado;
  - `CarMods`: primero la estadística y después la mejora (modelos leídos de `ms_modelInfoPtrs`);
  - `Food`: primero `STAT_NUMBER_OF_MEALS_EATEN` y luego el presupuesto;
  - `Weapons`: `GetWeaponSlot` + `SetCurrentWeapon(slot)`.
- `RestoreClothesState`: copia la ropa guardada sobre la del jugador (la DLL solo cambiaba un puntero local).
- `StoreVehicleMods`/`RestoreVehicleMods`: `gComponentDamageState` son 25 bytes (la DLL usaba 20); solo se restauran estados `> 0` con signo y paneles/puertas llaman también a `SetPanelDamage`/`SetDoorDamage(…, true)`.
- `Save`/`Load`: devuelven `true` y el bucle de modificadores compara sin signo.
- Fuera de la lista (sin hook): `AddPriceModifier` (0x49BDD0, también actualiza el precio cargado) y `RemovePriceModifier` (0x49ACD0, el último ocupa el hueco) transcritas del EXE.

**Método:**
- `shopping.dat` se genera en memoria (secciones de precios y tiendas en orden aleatorio, comentarios, líneas vacías, sangrías, separadores y etiquetas largas); `OpenFile`/`CloseFile`/`LoadLine` se simulan con varios manejadores y un único búfer de línea como el real. Nunca se leen ficheros ni partidas reales.
- Peatones, `CPlayerPedData`, `CPedClothesDesc`, vehículo, `CWanted` y modelos falsos (entradas 19900..19963 de `ms_modelInfoPtrs`); `CWorld::Players` se sustituye por el fixture durante cada ejecución.
- Stubs con traza en un array fijo (sin reservas durante la llamada): estadísticas, `CTimer::Suspend/Resume`, `GetModelInfo` por nombre, `FindWeaponType`, bloques de animación, streaming, `FindPlayerVehicle/Wanted`, libertad condicional, ropa, armas, mejoras de vehículo, `CAutomobile::Fix*`/`Set*Damage`, `CDamageManager` (ruedas, paneles y puertas), `IsValidModForVehicle` y el búfer de guardado.
- Los cuerpos de la DLL se resuelven antes de parchear y nunca se parchea un stub; el cambio de hook se comprueba después de `State()`.
- Se guardan y restauran en cada ejecución y al final: todas las globales de `CShopping` (0xA97258..0xA9A890), `CWorld::Players`, jugador en foco, nivel actual y las entradas de modelos tocadas.
- Fuera de los fixtures: `LoadStats` con `CarPaintJobs` como primera sección con elementos (el EXE usa una clave sin inicializar), índices de estadística fuera de -1..5, tipos de ropa ≥ 18, secciones de precios ausentes cuando hay líneas detrás (el EXE haría `atoi(NULL)`) y precios ≥ 2^24.

Repetir: `python reversing/Shop30/build_probe.py` y `python reversing/run.py --case shop30`. Evidencia: [validation-summary.json](validation-summary.json).
