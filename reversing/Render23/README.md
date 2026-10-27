<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Render, escaleras, puente y piel (lista de 23)

Validadas el 02/10/2026 con **9.216 pares original/candidato dentro de GTA**, todos exactos: 28 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CBrightLights`, `CShinyTexts` | `Init`, `RegisterOne`, `Render`, `RenderOutGeometryBuffer` |
| `CMotionBlurStreaks` | `Init`, `RegisterStreak`, `Render`, `Update` |
| `CPlaneTrails` | `Init`, `RegisterPoint`, `Render`, `Update` |
| `CEscalators` | `AddOne`, `Init`, `Shutdown`, `Update` |
| `CBridge` | `FindBridgeEntities`, `Init`, `ShouldLightsBeFlashing`, `Update` |
| `CPlayerSkin` | `GetSkinTexture`, `Initialise`, `RenderFrontendSkinEdit`, `Shutdown` |

**Diferencias reales corregidas:**
- Contadores del búfer temporal (0xC4B950/0xC4B954): el EXE los usa como dwords con signo; `common.h` los declara `uint16`. `CBrightLights` y `CShinyTexts` usan referencias locales `int32` (posible problema en otras clases).
- `CBrightLights::Render` (el EXE inlinea `tBrightLight::Render`):
  - un `rand()` por luz antes del switch de color, `(rand()&0xFF)·0,2` truncado;
  - alfa con la constante 0x3D888889 en vez de `invLerp`;
  - vértices en el orden de sumas del EXE y `fsin`/`fcos` x87 para las luces pequeñas;
  - colores fuera del conjunto válido no hacen nada (sin assert).
- `RegisterOne` (luces y textos brillantes): distancia en el orden del EXE, guardada antes de comparar; NaN se rechaza (`!(d <= máx)`); cuenta con signo.
- `CShinyTexts::Render`: alfa 0xFF (no 12) y solo se dibujan los textos con la bandera exactamente a 1.
- `CMotionBlurStreaks`:
  - `Render`: el color de niebla usa los componentes `uint16` sin truncar a byte;
  - `RegisterStreak`: al tomar un hueco nuevo también limpia `m_acRenderHistory[2]`.
- `CPlaneTrails`:
  - `Render`: factor de color 1/256 (no 1/255) y la cadena de mínimos del EXE;
  - `Update`: ángulo con la constante 0x38490FDB, `fsin`/`fcos` x87, el tercer avión usa cos para X y sin para Y;
  - las coronas solo se ven de noche (`h > después || h < antes`): `ClockHoursInRange(22,7)` nunca era cierto;
  - `CPlaneTrail::Update` recibe la posición ya calculada.
- `CPlayerSkin`:
  - `RenderFrontendSkinEdit` usa las variables globales del EXE (0xC3F04C, 0xC3F048) y luz ambiente 0,65 (no 255);
  - `GetSkinTexture` usa `sprintf` (no `sprintf_s`, que rellena `gString` en debug) y `GetAt(slot)`: `GetAtRef` trataba el slot como referencia y devolvía nulo.

**Método:**
- Cámara, pool de TXD, `Scene.m_pRwCamera` y `gpPlayerClump` falsos.
- Estados de render, Im3D, `rand`, coronas, escaleras, estelas y las funciones de RW/TXD/BMP simulados y trazados; nunca se abre un archivo.
- La ruta de muchas luces pequeñas lee más allá de `aBrightLights` como el EXE; esa memoria se llena con datos falsos y se restaura.
- No cubierto: el vaciado dentro del bucle de `CShinyTexts::Render` (necesita más de 670 textos y el array tiene 32) y el caso en que el EXE se cae en `GetSkinTexture` (slot libre).

Repetir: `python reversing/Render23/build_probe.py` y `python reversing/run.py --case render23`. Evidencia: [validation-summary.json](validation-summary.json).
