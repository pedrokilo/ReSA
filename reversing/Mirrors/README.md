<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMirrors

Validadas el 02/10/2026 con **6.272 pares original/candidato dentro de GTA**, todos exactos: las 8 funciones de `CMirrors` con `validated=1`.

**Diferencias reales corregidas:**
- `CreateBuffer`: con calidad media o superior, si falla el ráster de 1024×512 (o su z-buffer) el original cae a 512×256; la DLL se quedaba sin búferes.
- `BuildCamMatrix`: escribe (0,0,1) en `up`, normaliza `forward` y también `right` (la DLL no normalizaba `right`).
- `RenderMirrorBuffer`:
  - vértices en los búferes globales del EXE (Im2D en 0xC7C5A8, `TempBufferVertices`, índices copiados de 0x8D5E38);
  - el punto de distancia es (216, -42, 1003) con la suma `(z² + y²) + x²` (la DLL tenía las coordenadas cambiadas de orden);
  - el quad especial cambia `ZWRITE`, `VERTEXALPHA` y la mezcla antes de dibujarse.
- `BuildCameraMatrixForScreens`: milisegundos · 0,001 (no / 1000) y giro con la constante π/8192 del EXE.
- `BeforeConstructRenderList`:
  - recorre siempre las dos pantallas, con el centroide sumado en el orden del EXE;
  - normal = componentes · 0,01 (no / 100);
  - `MirrorFlags` con signo de 32 bits;
  - matriz de pantallas sin inicialización por valor.
- `TypeOfMirror` y `MirrorFlags` son palabras de 32 bits en el original (la DLL los declaraba de un byte).

**Método:**
- Se simulan y se trazan:
  - rásteres, calidad de efectos, estados de render;
  - Im2D (puntero de dispositivo) e Im3D, `DefinedState`;
  - una cámara RW falsa y la posición de `TheCamera` (matriz falsa);
  - `FindPlayerVehicle`, las zonas de espejo, `IsSphereVisible`, `DealWithMirrorBeforeConstructRenderList`;
  - el cambio de cámara para el espejo, `RwCameraClear`/`EndUpdate` (la DLL tiene implementaciones propias), `RsCameraBeginUpdate`, `RenderScene` y `RenderWeaponPedsForPC`.
- Se comparan el estado global de los espejos, los búferes temporales del EXE, las matrices y la traza.

Repetir: `python reversing/Mirrors/build_probe.py` y `python reversing/run.py --case mirrors`. Evidencia: [validation-summary.json](validation-summary.json).
