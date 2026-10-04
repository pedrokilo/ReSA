# Darkel y grafitis (lista de 19)

Validadas el 03/10/2026 con **17.792 pares original/candidato dentro de GTA**, todos exactos: 38 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CDarkel` | `Init`, `FrenzyOnGoing`, `ReadStatus`, `CalcFade`, `CheckDamagedWeaponType`, `QueryModelsKilledByPlayer`, `ResetModelsKilledByPlayer`, `FindTotalPedsKilledByPlayer`, `ThisPedShouldBeKilledForFrenzy`, `ThisVehicleShouldBeKilledForFrenzy`, `DrawMessages`, `RegisterKillNotByPlayer`, `RegisterKillByPlayer`, `RegisterCarBlownUpByPlayer`, `StartFrenzy`, `DealWithWeaponChangeAtEndOfFrenzy`, `Update`, `ResetOnPlayerDeath`, `FailKillFrenzy` |
| `CTagManager` | `Init`, `ShutdownForRestart`, `AddTag`, `FindTagDesc`, `IsTag`, `GetAlpha` (atómico y entidad), `SetAlpha` (atómico y entidad), `GetPercentageTagged`, `GetPercentageTaggedInArea`, `UpdateNumTagged`, `SetAlphaInArea`, `GetNearestTag`, `GetTagPos`, `SetupAtomic`, `RenderTagForPC`, `Save`, `Load` |

**Diferencias reales corregidas:**
- `CDarkel::DrawMessages`:
  - escalas con las constantes del EXE (ancho × 1/640, alto × 1/448), sin la corrección de aspecto de `SCREEN_SCALE_*`; la segunda línea es `alto·20 + alto·148`;
  - `FindPlayerPed(1)` va en línea (`CWorld::Players[1].m_pPed`);
  - `sprintf` en vez de `sprintf_s` (que rellena `gString` con 0xFE en debug).
- `CDarkel::StartFrenzy`:
  - la conversión `UZI_DRIVEBY → MICRO_UZI` mira el parámetro, no la global `WeaponType`;
  - comparación con signo contra `WEAPON_LAST_WEAPON`;
  - `FindPlayerPed()` en cada acceso, como el EXE; munición del cargador con comparación con signo.
- `CDarkel::DealWithWeaponChangeAtEndOfFrenzy`:
  - el límite se comprueba sobre el arma ya convertida (con `UZI_DRIVEBY` el EXE sí devuelve las armas);
  - faltaba restaurar `m_nChosenWeapon` con el slot del arma activa antes del frenesí;
  - `MakeChangesForNewWeapon` recibe el tipo del arma activa (0x60B460), no el slot (0x60D000);
  - `m_nRefCount` con signo y `FindPlayerPed()` en cada acceso.
- `CDarkel::FindTotalPedsKilledByPlayer`: suma solo los 400 primeros modelos (peatones), no 800.
- `CDarkel::RegisterKillByPlayer`:
  - una muerte cuenta como mucho una vez para el frenesí (la DLL podía contarla dos veces, p. ej. con explosiones);
  - orden de estadísticas del EXE (respeto antes que bandas) y sin comparar con el jugador antes de `IsMember`;
  - `CWorld::Players[0].m_pPed` en línea y tipo de peatón con signo.
- `CDarkel::RegisterKillNotByPlayer`: `FindPlayerPed()` se llama dos veces.
- `CDarkel::Update` y `FailKillFrenzy`: `SetMissionFailed`/misión superada en línea escribiendo `GameState` como byte (la DLL escribía un dword); `FindPlayerPed(1)` en línea.
- `CDarkel::ThisPedShouldBeKilledForFrenzy` / `ThisVehicleShouldBeKilledForFrenzy`: modelo extendido con signo (`movsx`).
- `CTagManager`:
  - `ShutdownForRestart` limpia solo las `ms_numTags` entradas;
  - `FindTagDesc`, `GetPercentageTaggedInArea`, `SetAlphaInArea` y `GetNearestTag` recorren de la última entrada a la primera (en `GetNearestTag` el empate lo gana el índice más alto);
  - `SetAlphaInArea` guarda el alfa aunque la etiqueta no tenga atómico;
  - `GetPercentageTagged` convierte sin signo y trunca con `_ftol2` (0 con NaN); `GetPercentageTaggedInArea` divide en coma flotante;
  - `RenderTagForPC`: alfa = byte bajo del valor de usuario × valor completo / 255 (dos lecturas);
  - `AddTag`, `GetAlpha`/`SetAlpha` de entidad sin `assert`; `AddTag` y `GetTagPos` sin comprobaciones de rango;
  - `Save` y `Load` devuelven `true`.

**Método:**
- Peatones, jugadores (con `CPlayerPedData`), vehículos, entidades, matrices, atómicos, geometrías, materiales y model infos son falsos con offsets crudos; los model infos falsos ocupan ranuras libres de `ms_modelInfoPtrs`.
- Stubs con traza: `FindPlayerPed`/`FindPlayerVehicle`, estadísticas, audio de interfaz, `IsMember`, guerras de bandas, `PopCycle`, ayuda y mensajes del HUD, fuentes, `TheText.Get`, `AsciiToGxtChar`, `GetYPosBasedOnHealth`, lógica de juego, funciones de armas del jugador, `AddRef`/`RemoveRef`, apariencia y policía de vehículos, valores de usuario de atómicos, `TriggerMessage` y el búfer de guardado (nunca ficheros reales).
- Se guardan y restauran en cada ejecución y al final: todas las globales de `CDarkel` (tabla de muertes, estado, modelos, armas interrumpidas), `GameState`, la tabla de grafitis con sus contadores y la tubería, `PedsKilledOfThisType`, `m_AllRandomPedsThisType`, `Players[0/1].m_pPed`, reloj, contador de fotogramas, modo de repetición, jugador en foco, resolución, pantalla ancha, `gString`/`gGxtString` y las ranuras de model infos usadas.
- Fuera de los fixtures:
  - `GetNearestTag` sin etiquetas o sin ninguna distancia válida (el EXE devuelve `ms_tagDesc[-1].Entity`; la DLL mantiene el BUGFIX que devuelve nulo);
  - `GetAlpha`/`SetAlpha` de entidad sin atómico y fuera de la tabla (el EXE se cae; la DLL conserva `GenericCrashing`);
  - armas o modelos negativos e índices de tabla fuera de rango.

Repetir: `python reversing/Game19/build_probe.py` y `python reversing/run.py --case game19`. Evidencia: [validation-summary.json](validation-summary.json).
