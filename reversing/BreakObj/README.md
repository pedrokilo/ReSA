<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BreakObject_c

Validadas el 02/10/2026 con **10.752 pares original/candidato dentro de GTA**, todos exactos: las 9 funciones de `BreakObject_c` con `validated=1`.

**Diferencias reales corregidas:**
- `SetBreakInfo`: `bJustFaces` se compara con `== 1`, la memoria se reserva con `operator new` (no `new[]`) y la vida de los grupos es `256 + GetRandomIntInRange(0, 32)`.
- `Exit`: sin grupos no libera nada; usa `operator delete` y solo sobre punteros no nulos.
- `SetGroupData`: el original genera los números en línea con `rand * (1/32767)` y sus propias sumas, por ejemplo `(rango·r + vel) + mín`. El eje se normaliza con `RwV3dNormalize`.
- `DoCollisionResponse`: las mismas fórmulas en línea, más `RwV3dNormalize`/`RwV3dLength` y la asociación exacta. Las partículas usan `(r + pos) - 0,5` y `r·0,3 - 0,15`. La cantidad de chispas sale de `RwV3dLength(velocidad)`.
- `Update`:
  - la gravedad es `ts * 0.008` (no `/125`) y el ángulo es `a * 57,295776 * ts * 0,05`;
  - el eje se normaliza con `RwV3dNormalize`;
  - el `acos` es el del CRT del juego: `fpatan(sqrt((1+x)(1-x)), x)`, conservando la precisión del FPU (24 bits en el juego).

- `Init`: copia en crudo los bytes de `bJustFaces` y `m_nSparksOnImpact` (sin normalizar a `bool`). La Z del origen es `(pos.z + min.z) + 0,25`, el centro de colisión es `(max - min)·0,5 + min` y el punto se transforma con el orden de sumas de 0x59C890.
- `Render`: sin grupos sale sin llamar a `RenderEnd`.

**Método:**
- Objeto, grupos, información de rotura y texturas propios.
- `operator new/delete` del EXE se sustituyen, solo durante la llamada, por un asignador lineal. La traza se guarda sin reservar memoria, porque si no se recursa en el `new` falso.
- Se simulan y se trazan `RwTextureDestroy`, `IsVisionFXActive`, `AddParticle`, `AddSparks` y el RNG.
- Se ignoran los bytes de relleno de `RwMatrix`, porque `RwMatrixRotate` copia una matriz temporal de pila sin inicializar.
- Los tipos de grupo se limitan a 0..2.

`Init` usa un objeto, atomic, geometría con plugin de rotura, modelo y colisión falsos. `Render` usa `RenderBegin/End/AddTri` simulados (EXE y DLL).

Repetir: `python reversing/BreakObj/build_probe.py` y `python reversing/run.py --case breakobj`. Evidencia: [validation-summary.json](validation-summary.json).
