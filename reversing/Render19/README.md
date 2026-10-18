<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cámara de sombras y cristales (lista de 19)

Validadas el 03/10/2026 con **9.920 pares original/candidato dentro de GTA**, todos exactos: 37 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CShadowCamera` | Constructor, Destructor, `Create`, `Destroy`, `SetFrustum`, `SetLight`, `SetCenter`, `InvertRaster`, `GetRwRenderRaster`, `GetRwRenderTexture`, `DrawOutlineBorder`, `Update` (clump y atomic), `MakeGradientRaster`, `RasterResample`, `RasterBlur`, `DrawBlurryRaster2`, `atomicQuickRender`, `Im2DRenderQuad` |
| `CGlass` | `Init`, `HasGlassBeenShatteredAtCoors`, `CarWindscreenShatters`, `WasGlassHitByBullet`, `WindowRespondsToCollision`, `GeneratePanesForWindow`, `Update`, `Render`, `FindWindowSectorList`, `RenderReflectionPolys`, `RenderShatteredPolys`, `RenderHiLightPolys`, `AskForObjectToBeRenderedInGlass`, `FindFreePane`, `WindowRespondsToSoftCollision`, `BreakGlassPhysically`, `WindowRespondsToExplosion`, `IsObjectGlass` |

**Diferencias reales corregidas:**
- `CShadowCamera`:
  - `Destroy` desengancha la cámara (no el frame) y limpia el raster de la cámara antes de destruirlo;
  - `SetFrustum`: plano cercano `radio · 0,001` (multiplicación, no división);
  - `InvertRaster` desactiva antes el Z-test; `DrawOutlineBorder` deja `VERTEXALPHA` a TRUE y devuelve el raster leído al principio;
  - `Create` no comprueba el frame: lo engancha tal cual y valida el padre de la cámara;
  - `atomicQuickRender` guarda y llama al `AtomicDefaultRenderCallBack` del EXE (0x7491C0), y `Update` (clump) pasa la función del EXE (0x705620) y llama a `GetFirstAtomic` después de `RwCameraBeginUpdate`;
  - `MakeGradientRaster` devuelve el raster (también si falla `BeginUpdate`), usa los enteros globales 0x8D5218/0x8D521C y repite el entero truncado en los cuatro bytes del color;
  - `RasterResample` devuelve el raster de la cámara si falla `BeginUpdate`; `RasterBlur` calcula la UV como `(0,5 ± 0,5) · (1/ancho)`.
- `CGlass`:
  - hay 45 paneles (no 44) y 5 piezas en `PanePolyPositions` (no 4); `Init` divide entre 3 (no 4) con el orden de sumas del EXE;
  - `HasGlassBeenShatteredAtCoors` devuelve el bit 0x20 (no 0x10), sectores con `· 0,02` y la misma función para objetos y dummies; `FindWindowSectorList` recorre los nodos a mano con el orden de sumas z, y, x;
  - `CarWindscreenShatters` usa los dos primeros triángulos de cristal (no el siguiente), `right = normal × arriba` (estaba invertido), comprueba los planos, suena `AE_GLASS_BREAK_FAST` y sigue los órdenes de sumas de 0x59C790/0x59C890;
  - `GeneratePanesForWindow`: secciones `max(tamaño + 0,75, 1)` y el límite a 1 no sube 0; desplazamientos `índice · longitud / secciones`; `(1/módulo · v) · tamaño`; aleatorios por 0,0015 y 0,002 con X e Y intercambiados; tiempo retrasado en float;
  - `WindowRespondsToCollision` y `BreakGlassPhysically`: colisión del model info, Z mínima/máxima por parejas y vértice más lejano con raíz (empates al primero); NaN de intensidad va a `DELAYED`;
  - `BreakGlassPhysically` recorre el pool al revés, pasa el punto con la traspuesta, sigue con el resto de objetos al marcar el primer golpe, suena `AE_GLASS_BREAK_FAST` y marca el bit 0x20;
  - `Render`: color de niebla con los componentes `uint16` sin recortar; `WindowRespondsToExplosion`: módulo z, y, x y NaN no rompe nada.

**Método:**
- Cámara, frames, luz, rasters, textura, clump, atomics y geometrías de RW son falsos; estados de render e Im2D se sustituyen en `RwEngineInstance->dOpenDevice` y `zBufferNear` se fija por caso.
- `RwCameraBeginUpdate`/`EndUpdate`/`Clear` y `GetFirstAtomic` se simulan en el EXE y en la DLL (no son envoltorios); el resto de primitivas RW, en el EXE.
- Objetos, vehículo, model infos (huecos 19990–19993), modelos de colisión, nodos y el pool de objetos son falsos; los sectores del mundo se vacían solo en `HasGlassBeenShatteredAtCoors`.
- Se simulan y trazan `rand`, el audio de cristales, el suelo, `CalculateTrianglePlanes`, `TestSphereTriangle`, `ClearScanCodes`, `CFallingGlassPane::Update/Render` e Im3D; las UV que el EXE deja sin escribir no se comparan.
- Se guardan y restauran paneles, centros, contadores, entidades a renderizar, `PanePolyPositions`, texturas de cristal, código de búsqueda, tiempo, cielo y los enteros del degradado.
- No cubierto:
  - `CGlass::CalcAlphaWithNormal`: toda la función queda bajo `FIX_BUGS` (luz normalizada y `pow`); la versión exacta del EXE está en la rama sin `FIX_BUGS`;
  - `RasterBlur` con 0 pasadas (NOTSA de la DLL: devuelve el raster de la cámara).

Repetir: `python reversing/Render19/build_probe.py` y `python reversing/run.py --case render19`. Evidencia: [validation-summary.json](validation-summary.json).
