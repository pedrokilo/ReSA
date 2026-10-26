<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CCustomCarEnvMapPipeline (lista de 34)

Revisado el 04/10/2026 contra `gta-sa.exe` 1.0 US: **37 funciones**, banco de **10.880 pares original/candidato** dentro de GTA. Pendiente de compilar y ejecutar.

| Grupo | Funciones |
|---|---|
| Pipeline | `RegisterPlugin`, `CreatePipe`, `DestroyPipe`, `CreateCustomObjPipe`, `CustomPipeInstanceCB`, `CustomPipeRenderCB`, `CustomPipeMaterialSetup`, `CustomPipeAtomicSetup`, `PreRenderUpdate` |
| Material de entorno | `GetFxEnvScaleX/Y`, `GetFxEnvTransSclX/Y`, `GetFxEnvShininess`, `SetFxEnvScale`, `SetFxEnvTransScl`, `SetFxEnvShininess`, `SetFxEnvTexture`, `SetCustomEnvMapPipeMaterialDataDefaults`, `pluginEnvMat*` (6) |
| Atomic de entorno | `AllocEnvMapPipeAtomicData`, `pluginEnvAtm*` (3) |
| Especular | `GetFxSpecSpecularity`, `GetFxSpecTexture`, `pluginSpecMat*` (6) |

**Diferencias reales corregidas:**
- Pools: se manejan como las plantillas de `CPool` del EXE (formato crudo): `New` sin `break` de depuración, `Delete` sin `assert`, creación con `new` escalar sin relleno 0xFD y liberación inline de `DestroyPipe` (solo con tamaño > 0, `operator delete` escalar, último libre = 0, globales a 0 después).
- `RegisterPlugin` y `CreatePipe` devuelven `RwBool` (los llamadores del EXE hacen `neg eax` sobre EAX completo).
- `CreateCustomObjPipe` desbloquea lo que devuelve `RxLockedPipeAddFragment`.
- `CustomPipeInstanceCB` pasa la cabecera en su sitio (`resEntry + 0x18`), no una copia local.
- `CustomPipeRenderCB`:
  - sin reflejos con los bits 0x6000 del atomic (la DLL usaba 0x2000 dos veces);
  - se quita el NOTSA "DarkVehiclesFix";
  - `SPECULARENABLE` se pone antes de `LOCALVIEWER`/`SPECULARMATERIALSOURCE`; el especular solo se satura si es > 1 (NaN se conserva);
  - matrices de textura globales 0xC02D78/0xC02D38 y material D3D9 global 0x8D12D8 escritos por partes, como el EXE;
  - UV de cámara `-((p - n·s)·(1/s))` y UV de onda con el desplazamiento acumulado del EXE (una sola corrección ±1, sentido con el vector *up*, límite de `at.y + at.x` con NaN = 0,1), `RwFrameGetLTM` dentro de cada cálculo;
  - reserva inline de los datos del atomic (UV = 0 si el pool está lleno);
  - factores de textura truncados con `_ftol2` y saturados con signo;
  - material con difuso `c · (d · 1/255)`, ambiente `(c · AmbientSaturated) · (a · 1/255)`, orden de estados del EXE y alfas sin tocar;
  - superficie final con las globales 0x8D131C/0x8D1328; número de mallas con signo.
- `CustomPipeMaterialSetup` pone a 0 el campo de flags antes de nada.
- Getters de entorno: 0 sin datos (no `UNREACHABLE`); brillo como byte **sin signo** · (1/255).
- Setters de entorno: duplican primero los datos falsos (0x5D9570) y convierten con `_ftol2` (64 bits, byte bajo); `SetFxEnvShininess` ya no acaba en `UNREACHABLE`.
- `SetFxEnvTexture`: si el pool está lleno el puntero queda nulo; lee la textura del primer pase de MatFX directamente; escribe los bytes 0x51 = 0x11 y 0x50 = 2.
- `SetCustomEnvMapPipeMaterialDataDefaults` no toca el byte de relleno +5.
- `pluginEnvMatStreamReadCB`/`WriteCB`: conversiones del EXE, comparación de 12 bytes con el falso, pool lleno → falso; usan el offset recibido.
- `pluginEnvAtmConstructorCB`/`CopyConstructorCB`: usan el offset recibido; la copia no pone valores por defecto y copia `sizeInObject` bytes.
- `pluginSpecMatDestructorCB` pone a nulo el puntero del plugin (la DLL limpiaba una copia local).
- `pluginSpecMatCopyConstructorCB` devuelve el objeto destino, no libera la textura anterior (se quita el NOTSA) y solo suma una referencia si hay textura.
- `pluginSpecMatStreamReadCB` admite el pool lleno; `pluginSpecMatStreamWriteCB` usa `strncpy` de 24 bytes y no exige textura.
- Fuera de la lista, también corregidas: `GetFxEnvTexture` (0x5D6F20) y `SetFxSpecTexture` (0x5D8B00, direccionamiento envolvente 0x11).

**Método:**
- Materiales, atomics, geometrías, clump, frames, LTM, luz, texturas, datos MatFX y una `RwResEntry` con hasta 4 mallas son falsos; los offsets de plugins se leen del EXE.
- Los tres pools se sustituyen por pools pequeños falsos (16/8/16 huecos, a veces llenos).
- Stubs trazados sin reservas: streams, texturas, registro de plugins, `RxPipeline*`, capacidades D3D9, efectos MatFX, `RwFrameGetLTM`, recorte, estados/TSS/texturas/luces/transformaciones/material/superficie, alfa por vértice, vaciado de estados, `fpRenderStateSet`, un dispositivo falso y `operator new/delete` (solo en `CreatePipe`/`DestroyPipe`, sobre una arena).
- Se guardan y restauran en cada ejecución: offsets, datos falsos 0xC02D18, pipeline y pools, `g_GameLight`, matrices, material global, intensidad especular, `AmbientSaturated`, cachés D3D9, `pDirect`, calidad de efectos, dispositivo y `fpRenderStateSet`.
- Fuera de los fixtures:
  - el byte +5 de `pluginEnvMatStreamReadCB` es basura de pila en el EXE: se limpia la pila antes de la llamada y el falso tiene +5 = 0 (la DLL toma el +5 del falso);
  - flujos con longitud distinta de 24/28 (búfer sin inicializar);
  - datos nulos donde el EXE se cae (escritura del stream, material nulo en `SetFxEnvTexture`, flags sin datos en el render);
  - pools de tamaño 0 o con último libre fuera de rango en `New`.

Repetir: `python reversing/EnvMap37/build_probe.py` y `python reversing/run.py --case envmap37`.
