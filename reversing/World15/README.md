<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ciclo de población y sprites (lista de 15)

Validadas el 02/10/2026 con **17.312 pares original/candidato dentro de GTA**, todos exactos: 30 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPopCycle` | `Initialise`, `FindNewPedType`, `GetCurrentPercOther_Peds`, `IsPedAppropriateForCurrentZone`, `IsPedInGroup`, `PedIsAcceptableInCurrentZone`, `PickARandomGroupOfOtherPeds`, `PickGangToCreateMembersOf`, `PickPedMIToStreamInForCurrentZone`, `PlayerKilledADealer`, `Update`, `UpdateAreaDodgyness`, `UpdateDealerStrengths`, `UpdateIsGangArea`, `UpdatePercentages` |
| `CSprite` | `Initialise`, `InitSpriteBuffer`, `FlushSpriteBuffer`, `CalcScreenCoors`, `CalcHorizonCoors`, `Set4Vertices2D` (CRect), `RenderOneXLUSprite`, `RenderOneXLUSprite_Triangle`, `RenderOneXLUSprite_Rotate_Aspect`, `RenderOneXLUSprite2D`, `RenderBufferedOneXLUSprite`, `RenderBufferedOneXLUSprite_Rotate_Aspect`, `RenderBufferedOneXLUSprite_Rotate_Dimension`, `RenderBufferedOneXLUSprite_Rotate_2Colours`, `RenderBufferedOneXLUSprite2D` |

**Diferencias reales corregidas:**
- `CPopCycle::Initialise`: abre `POPCYCLE.DAT` en modo `"rb"` y al final reinicia índice horario, día de la semana, zona actual y tipo de zona (-1). El reescalado del EXE (100/suma en la FPU, resto al último máximo) queda transcrito fuera de `FIX_BUGS`.
- `CPopCycle::FindNewPedType`:
  - sin zona no toca las salidas;
  - la tirada de comisaría es `(int)(rand/32768·100) < 70`;
  - camellos: escribe el tipo y el modelo inválido antes de buscar;
  - bandas: si `PickGangToCreateMembersOf` devuelve 0 el modelo queda inválido sin llamar a `ChooseGangOccupation`;
  - civiles: cualquier modelo negativo devuelve falso; el máximo se elige por parejas como el EXE.
- `CPopCycle::GetCurrentPercOther_Peds` devuelve un entero truncado; `IsPlayerOnAMission`/`FindPlayerPed` se consultan siempre y `FrenzyOnGoing` después.
- `CPopCycle::IsPedAppropriateForCurrentZone` recorre los 18 grupos y comprueba el porcentaje de cada uno (la DLL comprobaba un puntero, siempre cierto, en 33 grupos).
- `CPopCycle::PickARandomGroupOfOtherPeds`: tirada `rand/32768` (0x407180) y resta sin límite.
- `CPopCycle::PickGangToCreateMembersOf`:
  - truco: `7 - (int)(rand/32768·-8)`;
  - sin densidad devuelve 0;
  - fuerza relativa con recíprocos y primer máximo estrictamente mayor que 0.
- `CPopCycle::UpdateDealerStrengths`:
  - actúa cuando **cambia** el minuto (estaba invertido);
  - recorre `TotalNumberOfZoneInfos` (no las 380 entradas);
  - compara `rand/32767` (no `rand`) y pide el número aunque la fuerza sea ya 15.
- `CPopCycle::UpdateAreaDodgyness` acumula banda a banda con ×0,01.
- `CPopCycle::UpdatePercentages`:
  - camellos = **mínimo** de 0,1 y fuerza·0,01;
  - suma policías+bandas+camellos y normaliza con el recíproco; el resto se resta uno a uno;
  - número de cada tipo = (porcentaje·fracción)·0,01·máximo; los peatones «otros» usan el entero de `GetCurrentPercOther_Peds` sin volver a multiplicar por la tabla;
  - durante disturbios el máximo de peatones sube **al menos** a 20; `LaRiotsActiveHere` se llama 3 veces en el orden del EXE.
- `CPopCycle::Update`: un día fuera de 0..7 no toca el valor; `FindPlayerCentreOfWorld` recibe el jugador con el foco.
- `CSprite`:
  - el conmutador de Z del búfer es un dword;
  - `fsin`/`fcos` a precisión completa en las variantes rotadas (asm);
  - `CalcScreenCoors`/`CalcHorizonCoors` transforman con el orden de sumas de 0x59C890 y comparan con NaN como el EXE;
  - los sprites no bufferizados escriben en el array global 0xC4B8E0;
  - `RenderOneXLUSprite_Triangle` solo atenúa por debajo de z = 2,3 y no toca u/v;
  - `RenderOneXLUSprite2D` usa el alfa del argumento;
  - `RenderBufferedOneXLUSprite2D` usa `size.y` para el alto;
  - los rectángulos se rellenan sin el assert de rectángulo invertido.

**Método:**
- Las tablas de PopCycle, los contadores y grupos de `CPopulation`, los siguientes peatones a cargar, las zonas de información del mundo y los búferes de sprites se aplican desde una base fija en cada ejecución; se comparan las diferencias por dword frente al estado previo.
- Los porcentajes de grupo suman 100 en todas las filas y en el `POPCYCLE.DAT` simulado, así que los caminos `FIX_BUGS` de `Initialise` y `PickARandomGroupOfOtherPeds` dan lo mismo que el EXE.
- Se simulan y trazan `rand`, las elecciones de ocupación, guerras de bandas, frenesí, misión, `FindPlayerPed`, truco de zonas, disturbios, centro del mundo, `GetZoneInfo`, el archivo y el dispositivo RW (estados, primitivas y primitivas indexadas con su contenido).
- Se guardan y restauran la cámara, recortes, FOV, tamaño de pantalla, Z del dispositivo, reloj, tiempo, lluvia, área, trucos, `ms_line` y los punteros del dispositivo RW.
- Fuera de los fixtures (solo lectura): la tabla de traducción de grupos, los model infos de peatones y el estado de carga de streaming.

Repetir: `python reversing/World15/build_probe.py` y `python reversing/run.py --case world15`. Evidencia: [validation-summary.json](validation-summary.json).
