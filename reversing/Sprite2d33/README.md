<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Sprites 2D (lista de 33)

Validadas el 04/10/2026 con **11.136 pares original/candidato dentro de GTA**, todos exactos: 33 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CSprite2d` (sprite) | Constructor, Destructor, `Delete`, `SetTexture` (nombre y nombre + máscara), `SetAddressing`, `SetAddressingUV`, `SetRenderState` |
| `CSprite2d` (dibujo con textura) | `Draw` (x/y/ancho/alto, `CRect`, `CRect` + 4 colores, 4 esquinas), `DrawWithBilinearOffset` |
| `CSprite2d` (búfer de vértices) | `SetRecipNearClip`, `InitPerFrame`, `IsVertexBufferEmpty`, `IsVertexBufferFull`, `RenderVertexBuffer` |
| `CSprite2d` (vértices) | `SetVertices` (`CRect` + 4 colores, 8 floats + 4 colores, `CRect` + 4 colores + UV, `iffCRGBA`, `ifCRGBA`, `RwD3D9Vertex`), `SetMaskVertices` |
| `CSprite2d` (dibujo sin textura) | `DrawRect` (1 y 4 colores), `DrawTxRect`, `DrawRectXLU`, `DrawAnyRect`, `Draw2DPolygon`, `DrawBarChart`, `DrawCircleAtNearClip` |

**Diferencias reales corregidas:**
- `IsVertexBufferFull`: el límite de índices es 0xFFA (4090), no 4096.
- `SetVertices` (`ifCRGBA`, 0x727920): el color avanza 4 bytes por vértice (array de `CRGBA`), no un único color.
- `DrawRectXLU`: vértices primero y después raster nulo, alfa, mezcla origen/destino y dibujo (antes: alfa y mezcla antes de `DrawRect`).
- `Draw2DPolygon`: modo de sombreado 1 (plano), no 0.
- `DrawCircleAtNearClip`:
  - paso entero `360 / angle` (`idiv`) convertido a float;
  - índices de la tabla de senos `trunc(i·paso·0x3C8EFA35·0x4222F983)` (+64 para el coseno) y el segundo vértice suma el paso escalado, sin `GetSinFast`/`GetCosFast`.
- `DrawBarChart` (FIX_BUGS retirado):
  - el progreso solo se recorta por debajo de 0 (sin tope en 100) y `currX = unclamped < endX ? unclamped : endX`;
  - barra extra: izquierda `max(currX − progressAdd, x − 1)` con comparación estricta (sin el 127 de FIX_BUGS);
  - borde: 2·alto·(1/448) y 2·ancho·(1/640) del EXE; borde inferior con arriba/abajo del EXE y borde derecho con izquierda/derecha invertidas como el EXE;
  - porcentaje: escala vertical `alto · 0,04` (no `/ 0,04`), conversiones con `_ftol2` de 64 bits y NaN alineado a la derecha.
- `Draw` (x/y/ancho/alto) y `DrawBarChart`: los rectángulos se rellenan campo a campo, sin el assert de rectángulo invertido del constructor de `CRect`.
- `SetVertices`/`SetMaskVertices` con número de vértices: acceso con `.data()` (sin el abort de `std::array` en depuración).

**Método:**
- Texturas y rasters falsos; `RwTextureRead`/`RwTextureDestroy` sustituidos en el EXE (los de la DLL son envoltorios).
- `fpRenderStateSet` y `fpIm2DRenderPrimitive`/`IndexedPrimitive`/`Triangle` de `RwEngineInstance` cambiados por sustitutos con traza (vértices con hash) solo durante la llamada.
- `CFont` (`SetWrapx`, `SetRightJustifyWrap`, `SetColor`, `SetEdge`, `SetFontStyle`, `SetScale`, `SetOrientation`, `PrintString`) y `AsciiToGxtChar` sustituidos en el EXE y en el cuerpo de la DLL, resuelto antes de escribir ningún parche; traza en array fijo sin reservas.
- `maVertices`, contadores del búfer, `NearScreenZ`, `RecipNearClip`, cámara de la escena (falsa), `RsGlobal.maximumWidth/Height` y `zBufferNear` fijados por caso y restaurados en cada ejecución; interruptor del hook comprobado tras `State()`.
- No cubierto: `angle = 0` en `DrawCircleAtNearClip` (división entera por cero en ambos), más de 8 vértices en `SetVertices`/`SetMaskVertices` (escribirían globales tras `maVertices`), `DrawWithBilinearOffset` sin textura (el EXE desreferencia nulo) y porcentajes con progreso ≥ 2^31 (desborda el búfer de 12 bytes del EXE).

Repetir: `python reversing/Sprite2d33/build_probe.py` y `python reversing/run.py --case sprite2d33`. Evidencia: [validation-summary.json](validation-summary.json).
