<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de efectos FxManager_c (lista de 25)

Validadas el 04/10/2026 con **14.336 pares original/candidato dentro de GTA**, todos exactos: 23 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `FxManager_c` | Constructor, Destructor, `Init`, `Exit`, `LoadFxProject`, `UnloadFxProject`, `LoadFxSystemBP`, `FindFxSystemBP`, `CalcFrustumInfo`, `GetFrustumInfo`, `Render`, `ReturnParticle`, `GetParticle`, `FreeUpParticle`, `SetWindData`, `FxRwMatrixCreate`, `FxRwMatrixDestroy`, `ShouldCreate`, `CreateFxSystem` (1, 2, 3 y 4), `DestroyFxSystem` |

`DestroyAllFxSystems` y `Update` ya estaban validadas (banco [FxManager](../FxManager/README.md)).

**Diferencias reales corregidas:**
- `FxEmitterPrt_c` medía 0x44 bytes (heredaba dos veces de `ListItem_c`); en el EXE mide 0x3C (`m_MultColor` en 0x2C). `Init` y `UnloadFxProject` reservan ahora, como el EXE, 1000 × 0x3C + 4 bytes del pool FX global, con la cookie del recuento y solo los enlaces a cero.
- `Constructor`: ya no pone a cero el centro y el radio de la esfera del frustum (el EXE solo escribe `m_nNumPlanesPassed`).
- `CalcFrustumInfo`:
  - `fpatan` y `fsin` del x87 con las constantes del EXE (57.295826f y 0.017453279f, no las más cercanas);
  - longitud de la ventana con `fsqrt` en línea, no `RwV2dLength`;
  - ya no pone a cero `m_nNumPlanesPassed` de la esfera.
- El EXE usa en línea el gestor global `g_fxMan` aunque `this` sea otro:
  - `ShouldCreate` y `CreateFxSystem-3`: pila de matrices temporales;
  - `DestroyFxSystem`: lista libre de partículas.
- `DestroyFxSystem`: `m_nNumPrims` con signo (un valor negativo no recorre primitivas).
- `CreateFxSystem-1`: calidad comparada con signo en cadena (≥ 2 → 1.0, negativa → sin cambio).
- `LoadFxProject`:
  - manejador comprobado con signo (`<= 0` → false);
  - un único búfer de campo para la cabecera y las comparaciones (una línea vacía conserva el anterior).
- `LoadFxSystemBP` devuelve el blueprint creado.
- Fuera los `assert` que el EXE no tiene (`GetParticle` con tipo ≠ 0, `CreateFxSystem-2/4` sin blueprint) y el índice comprobado de `std::array` en `FxRwMatrixCreate`.

**Método:**
- Cada caso usa un `FxManager_c` falso o `g_fxMan` con su contenido sustituido (las dos variantes, para cubrir los usos en línea del gestor global); `g_fxMan`, la calidad de `g_fx`, el balance día/noche y `fpRenderStateSet` se restauran tras cada ejecución.
- Blueprints, primitivas, sistemas, partículas, esferas, matrices, cámara, pools FX y la memoria de `new FxSystem_c` son búferes falsos de dirección fija; listas bien formadas.
- Simulados con registro fijo (sin reservas): rand, `FxMemoryPool_c::Init/Exit/Optimise`, `RwMatrixCreate/Destroy`, `operator new/delete` (solo durante la llamada), `FxSystem_c::Init/Exit/SetRateMult`, `FxSystemBP_c::Load/Render/FreePrtFromSystem`, `CTxdStore`, `CFileMgr` (guion de líneas que reproduce el bucle del EXE) y el estado de render.
- Se comparan los bytes de ambos gestores, hashes de blueprints, primitivas, sistemas, partículas, matrices y pools, la traza y el valor devuelto.
- Fuera de los fixtures:
  - el `FxSystem_c` creado solo se compara hasta 0x7C: el constructor de la DLL (otra clase) inicializa más campos de `CAEFireAudioEntity`;
  - lista de sistemas vacía en `FreeUpParticle` (el EXE divide por cero);
  - rutas de proyecto de 128 bytes o más y cabeceras vacías (búfer sin inicializar).

Repetir: `python reversing/Fx25/build_probe.py` y `python reversing/run.py --case fx25`. Evidencia: [validation-summary.json](validation-summary.json).
