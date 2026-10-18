<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Luces RW Light (lista de 27)

Validadas el 04/10/2026 con **7.936 pares original/candidato dentro de GTA**, todos exactos: 27 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `Light` | `ActivateDirectional`, `DeActivateDirectional`, `LightsCreate`, `LightsDestroy`, `LightsEnable`, `SetLightsWithTimeOfDayColour`, `WorldReplaceNormalLightsWithScorched`, `WorldReplaceScorchedLightsWithNormal`, `AddAnExtraDirectionalLight`, `RemoveExtraDirectionalLights`, `SetAmbientAndDirectionalColours`, `ReSetAmbientAndDirectionalColours`, `SetFlashyColours`, `SetFlashyColours_Mild`, `SetBrightMarkerColours`, `SetDirectionalColours`, `SetAmbientColoursToIndicateRoadGroup`, `SetFullAmbient`, `SetAmbientColours` (sin argumento y con color), `SetLightColoursForPedsCarsAndObjects`, `SetLightsForInfraredVisionHeatObjects`, `StoreAndSetLightsForInfraredVisionHeatObjects`, `RestoreLightsForInfraredVisionHeatObjects`, `SetLightsForInfraredVisionDefaultObjects`, `SetLightsForNightVision`, `GetDayNightBalance` |

**Diferencias reales corregidas:**
- `AddAnExtraDirectionalLight`:
  - la búsqueda de la luz más débil empieza en -1 (si ninguna es más débil no se sustituye nada) y solo recorre las 4 o 6 primeras, no las 6 siempre;
  - la fuerza es `(r > g ? r : g) > b ? … : b`, como el EXE (ceros con signo);
  - el alpha del color local queda sin inicializar.
- `SetLightColoursForPedsCarsAndObjects`: verde y azul suman su propio brillo (`m_BrightnessAddedToAmbientGreen/Blue`), no el del rojo.
- `SetAmbientColoursToIndicateRoadGroup`: índice `idx % 7` con signo (`cdq/idiv`), no `idx & 8`.
- `SetFlashyColours` y `SetFlashyColours_Mild`: copian solo rgb de `DirectionalLightColourForFrame` (no el alpha).
- `GetDayNightBalance`: minutos = `(horas*60 + minutos) + segundos*0,016666668` y las rampas multiplican por esa constante en vez de dividir por 60.
- `SetLightsWithTimeOfDayColour`: flags, rellenos y posición de la matriz quedan sin inicializar (antes a cero); sin `assert(world)`.
- `WorldReplaceNormalLightsWithScorched`: el alpha del color local queda sin inicializar.
- `LightsDestroy`: destruye la luz y después pone el puntero global a nulo.

**Método:**
- `pAmbient`, `pDirect` y `pExtraDirectionals` apuntan a 8 `RpLight` falsos con `RwFrame` falsos; nunca se tocan las luces reales del mundo ni el dispositivo.
- Stubs en el EXE (las funciones RW de la DLL son envoltorios): `RpLightCreate/Destroy/SetColor/SetRadius`, `RpWorldAddLight/RemoveLight`, `RwFrameCreate/Destroy/Rotate/Transform/UpdateObjects`, `_rwObjectHasFrameSetFrame` y `RwMatrixUpdate`. Registro fijo sin reservas; el json se arma después de la llamada.
- Se enmascaran los bytes de pila que el EXE no escribe: alpha de colores locales y flags/rellenos/pos de la matriz de `RwFrameTransform`.
- Se guardan y restauran los globales de luces (0xC8865C–0xC8870C), `m_CurrentColours`, `gfLaRiotsLightMult`, `CCoronas::LightsMult`, `LightningFlash`, la dirección del sol, el brillo añadido, el tiempo, el reloj, `currArea` y el color de visión infrarroja.
- Fixtures con ±0, valores iguales, dirección del sol vertical (Normalise pone x=1), índices negativos y horas/minutos fuera de rango.

Repetir: `python reversing/Light27/build_probe.py` y `python reversing/run.py --case light27`. Evidencia: [validation-summary.json](validation-summary.json).
