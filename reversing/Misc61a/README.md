# Clases sueltas (lote misc61a): scripts de cerebros, grupos de peatones, clima y cuerdas

Banco escrito el 05/10/2026 (sin ejecutar todavía): **20.992 pares original/candidato dentro de GTA** previstos para 42 funciones de 4 clases.

| Clase | Funciones | Casos |
|---|---|---|
| `CScriptsForBrains` | `GetIndexOfScriptBrainWithThisName`, `HasAttractorScriptBrainWithThisNameLoaded`, `Init`, `IsObjectWithinBrainActivationRange`, `StartAttractorScriptBrainWithThisName`, `StartOrRequestNewStreamedScriptBrain` | 3.840 |
| `CPedGroup` | `Constructor`, `Destructor`, `FindDistanceToFurthestMember`, `FindDistanceToNearestMember`, `Flush`, `GetClosestGroupPed`, `IsAnyoneUsingCar`, `PlayerGaveCommand_Attack`, `PlayerGaveCommand_Gather`, `Process`, `RemoveAllFollowers`, `Teleport` | 5.120 |
| `CWeather` | `AddRain`, `AddSandStormParticles`, `FindWeatherTypesList`, `ForceWeather`, `ForceWeatherNow`, `ForecastWeather`, `Init`, `IsRainy`, `ReleaseWeather`, `RenderRainStreaks`, `SetWeatherToAppropriateTypeNow`, `Update`, `UpdateInTunnelness` | 6.912 |
| `CRopes` | `CreateRopeForSwatPed`, `FindCoorsAlongRope`, `FindPickupHeight`, `FindRope`, `Init`, `IsCarriedByRope`, `RegisterRope`, `Render`, `SetSpeedOfTopNode`, `Shutdown`, `Update` | 5.120 |

**Reconstruidas desde el EXE en este lote** (antes `reversed=false`): `CScriptsForBrains::Init`, `IsObjectWithinBrainActivationRange`, `StartOrRequestNewStreamedScriptBrain`; `CPedGroup::FindDistanceToFurthestMember`, `PlayerGaveCommand_Gather`; `CWeather::AddRain`, `ForecastWeather`, `Update`; `CRopes::RegisterRope`, `FindCoorsAlongRope`.

**Diferencias reales corregidas:**
- `CScriptsForBrains::Init`: escribe campo a campo (los bytes de relleno +5..+7 no se tocan); el radio es 5.0 (0x859D9C).
- `CPedGroup`:
  - `Constructor`/`Destructor` propios: el EXE solo escribe unos campos y la DLL ponía a cero con `{}` los demás (los miembros ahora van en uniones anónimas y no se construyen solos); el destructor vacía primero la inteligencia y luego los miembros.
  - `Teleport`: sin evento actual se sigue (la DLL salía); el tipo se mira en el evento interno (`m_event`, +0x10), no en el de grupo; el desplazamiento de la formación se elige por hueco (0..6), no por orden entre los presentes.
  - `PlayerGaveCommand_Attack`: habla el jugador (`playerPed`), no el objetivo.
  - `IsAnyoneUsingCar`: se comprueban por separado las tareas de entrar como conductor (701) y como pasajero (700) (la DLL solo miraba la primera que existiera) y se quitó el `assert`.
  - `GetClosestGroupPed`: suma `(dx² + dz²) + dy²` (no `x² + y² + z²`), umbral inicial 1e8 y la distancia se escribe siempre que haya puntero (1e8 si no hay nadie).
  - `FindDistanceToNearestMember`: bucle propio (suma `(dz² + dx²) + dy²`); el puntero de salida se escribe con cada seguidor y queda el último, no el más cercano.
  - `RemoveAllFollowers`: bucle propio de `RemoveMember(i)` por hueco (0..6).
- `CWeather`:
  - `FindWeatherTypesList`: las tablas son de bytes (`int8`), no de `int16` (el EXE las lee con `movsx byte`); se devuelven y usan como `int8`.
  - `AddSandStormParticles`: `((rand · k · 40) + x) − 20`, no `x + (rand · k · 40 − 20)`.
  - `RenderRainStreaks`: reescrita desde el EXE (fuerza `(64 − FogReduction) · (int)(Rain · 110) / 64` con signo y sin bajar si es igual, rayas con viento y `Rain`, vértices con color `0xD2D2E6`, caída `rand · 0.09 + 0.01`, pérdida de fuerza de 2 a 4 con `GetRandomIntInRange`, reserva de los cuatro arreglos en el orden Y, X, Z, fuerza).
  - `UpdateInTunnelness`: puntos del túnel con sus guardas en 0xC8143C, objetivo `dist · 0.01` (no `/ 100`), `fsin` x87 sin matriz y paso con `|d| < paso` / `d < 0`.
  - `Update`/`AddRain`: nuevas, con las constantes y tablas del EXE (x87 a 24 bits, `fsin`/`fcos` completos).
- `CRopes`:
  - `CreateRopeForSwatPed`: el contador avanza siempre (módulo 256) y se devuelve el id (el EXE compara con signo el 0/1 de `RegisterRope`, nunca negativo); la DLL devolvía -1 si «tenía éxito» y no avanzaba.
  - `RegisterRope` y `FindCoorsAlongRope`: reconstruidas (actualización de cuerda existente con bucle do-while 0..segmentos, tipos 3/4/5-7/otros, zigzag o cuerda colgante).

**Método:**
- Los scripts de cerebros (1.400 bytes), el grupo (0x2D4), los peatones (10, con matriz o posición, vtable falsa, inteligencia y árbol de tareas), las entidades de las cuerdas y los vehículos son buferes propios.
- Los datos estáticos reales que usan las funciones (array de cuerdas, variables de `CWeather`, tablas de estado del time cycle, cámara, reloj, replay, cargas de los modelos de scripts, punteros de modelo 19990..19999, `g_fx`) se guardan y se restauran en cada ejecución.
- Se simulan y trazan: RNG, `operator new[]` (solo durante `RenderRainStreaks`), `operator new/delete` de eventos, estados de render e Im3D, `CamNoRain`/`PlayerNoRain`, `FindPlayer*`, `ProcessVerticalLine`, `FxSystem_c::AddParticle`, audio del clima, `CPad::StartShake`, `CPed::Say`, `AddEvent`/`SetTask`/`Flush`/`Process` de la inteligencia, `RemoveMember`, `CFormation::*`, `StartNewStreamedScriptBrain`, `RequestModel`, `CRope::*` y `RegisterReference`.
- En `Update` también se simulan `AddRain` y `UpdateInTunnelness` (se prueban por separado).
- Se comparan retorno, traza completa (orden y argumentos), buferes de las clases (palabras con punteros traducidos), variables de clima, buferes temporales, cuerdas y número de llamadas al RNG.

**Fuera del banco (y por qué):**
- La rama de `CPedGroup::Teleport` que baja a los seguidores del coche con `CTaskSimpleCarSetPedOut`: su `ProcessPed` real no se puede ejecutar con peatones falsos. Los casos se construyen para que esa rama no se dispare (seguidores en coche solo si son de misión o están muertos, o el líder va en coche).
- `RegisterRope` con más de 31 segmentos (el EXE escribe fuera del array de 32 segmentos).
- `WeatherTypeInList` negativo o muy grande (el EXE lee fuera de las tablas).

Repetir: `python reversing/Misc61a/build_probe.py` y `python reversing/run.py --case misc61a`.
