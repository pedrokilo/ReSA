<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución de P3 (sustituir por C++ propio: lotes S1–S4)

Rama `vivo-p3` (desde `origin/main` 410d9f2d84, 21/11/2026). Herramientas: `contrib/codigo_vivo_cierre_p3.py` (vuelca el estado en [cierre.csv](cierre.csv), [cierre-p3.csv](cierre-p3.csv), [plan.csv](plan.csv), [plan-lotes.md](plan-lotes.md) y la columna `cierre` de [funciones-vivas.csv](funciones-vivas.csv))
y los bancos de `reversing/FnP3` (ver su [README](../../reversing/FnP3/README.md)). Decisión vigente del usuario: sin validar función por función dentro del juego; basta un banco fuera del juego y el humo.

## Resumen

| Lote | Funciones del plan | Sustituidas | Con enganche | Verificación | Estado |
| --- | ---: | ---: | ---: | --- | --- |
| S1 QuickTime/QTML | 3 | 3 | 3 | razonada (no banqueable) | sustituido |
| S2 controlador D3D9 (usercall) | 6 | 6 | 1 (`0x4CCE60`; las demás son usercall) | banco `bench_d3d`: 0 diferencias; negativa 15/15 | sustituido y verificado |
| S3 vectores x87 | 6 (+1: `0x85742B`) | 7 | 7 | banco `bench_p3`: 0 diferencias; negativa 8/8 | sustituido y verificado |
| S4 CRT numérico | 4 | 3 (la cuarta, `___updatetlocinfo` 0x825456, es de la rama del locale) | 1 (`0x82872C`) + puntero (`0x833605`) + equivalente sin enganche (`0x821B40`) | banco `bench_p3`: 0 diferencias; negativa 5/5 (+1 equivalente) | parcial (3 de 4) |
| **Total** | **19** | **19** (18 del plan + `0x85742B` extra; `0x825456` fuera) | | | |

## S1: puentes de QuickTime (0x818650, 0x818670, 0x81A3C0)

**Qué usaban.** Los tres puentes (`mov eax, identificador; jmp` al despachador de QuickTime por `0x8E2A0C`, precedidos del gancho `0x8E2A08`) servían a la **radio personal**: `InitializeQTML` y `TerminateQTML` (0x8194B0/0x8194C0 → 0x818650/0x818670) y
`SoundConverterFillBuffer` (0x81A3C0, desde el callback `CAEMFDecoder::FillCallback`). Sólo los abre el reproductor de pistas de usuario `.aac`, `.m4a` y `.mov` (`audioExtensionTypes` de 0x8CBB28; el `.mp3` pasa a WMA).

**Qué se hizo.** En la práctica **no los usa nadie**: el lote EXT1A retiró QuickTime (`CAEMFDecoder::InitLibrary` devuelve siempre `false`, nadie llama a `QtmlInitialize`/`QtmlTerminate` y `FillCallback` sólo la puede invocar el `SoundConverter` de QuickTime, que ya no se crea). Por eso se sustituyen por la
**implementación nula segura** (`QTMLRuntime.cpp`: `QtmlInitializeBridge`, `QtmlTerminateBridge`, `QtmlConverterFillBridge`, con enganche): responden lo mismo que el EXE sin QuickTime, el OSErr −2095 (`0xF7D1`, el valor de `0x8194D0`: `mov ax, 0F7D1h; ret`), sin cargar `QuickTime.qts` ni llamar a nada. `QtmlInitializeThunk` y
`AEMFDecoder.cpp` llaman a las funciones C++ directamente. **No se usa Media Foundation**: no hay ningún camino que pida decodificar con QuickTime (los formatos que sólo abría QuickTime siguen sin cargarse, como desde EXT1A).

**Equivalencia razonada** (no banqueable: el EXE necesita el despachador de QuickTime). Con QuickTime ausente, el EXE ejecuta el gancho `0x819700` (que resuelve los tres puntos de entrada y, al no encontrarlos, los deja en `0x8194D0`) y salta a `0x8194D0`, que devuelve −2095; los tres C++ devuelven −2095. La única diferencia es el camino con QuickTime instalado
(PC con QuickTime 7 y `QuickTime.qts` en el sistema), que ya estaba retirado en la fase anterior. `QtmlInitialize`, `QtmlTerminate`, `QtmlLoadLibrary` y `QtmlResolveEntryPoints` siguen existiendo (sin llamadores del juego) y no se tocaron.

## S2: controlador D3D9 (0x4CB730–0x4CCE60)

`RasterCreate` (0x4CCE60, `cdecl (dispositivo, raster, banderas)`, entrada `rasterCreate` de la tabla de dispositivo) tiene ahora cuerpo C++ y enganche; sus cuatro ayudantes usercall son funciones C++ locales con los registros como parámetros (`D3D9RasterDriver.cpp`, espacio `RasterCreation`):

| EXE | Registros | C++ |
| --- | --- | --- |
| 0x4CB7C0 | pila = raster, EDI = extensión | `RasterCreateTexture` |
| 0x4CB9C0 | pila = raster, ESI = extensión | `RasterCreateRenderTargetTexture` |
| 0x4CC5C0 | EAX = banderas, EDI = raster | `RasterCheckFormat` |
| 0x4CCD70 | ESI = extensión, EDI = raster | `RasterCreateDepthSurface` |
| 0x4CB730 | ESI = índice | `PushFreePaletteIndex` (ya existía en C++) |

Sus únicos llamadores del EXE eran el propio 0x4CCE60 (y, para 0x4CB730, `RasterDestroy` 0x4CBB00, ya C++), así que quedan **muertos**; sin enganche posible (usercall). `D3D9RasterFormat.cpp` llama a `RasterCreateCpp` en lugar de `plugin::Call(0x4CCE60)`. La tabla `_rwD3D9DeviceSystemStandards` (rwcore.cpp) sigue escribiendo la dirección 0x4CCE60 (salta al enganche), igual que el resto de entradas de esa tabla.

Detalles del EXE reproducidos: la extensión se limpia (`+0`, `+4`, `+8`, `+9`, `+0xA`, `+0xB = 0xFF`, `+0xC`, `+0x18`) antes de `RasterCheckFormat`, de modo que la rama de cubo sólo se alcanza llamando al ayudante (`ext[9]` se pone a cero en `RasterCreate`); `0x7FED70` y `D3DResourceSystem::CreateTexture` (0x730510) se llaman como en el EXE;
la lista de rasters se actualiza con `RasterListPush` (0x4CB530) salvo en el camino de paleta de `0x4CB7C0`, que la inlinea (misma reserva, mismo orden); el reintento de la textura de destino de render con el formato del buffer trasero quita el uso 1.

**Verificación**: banco `bench_d3d` (fuera del juego, falsos de `IDirect3D9`/`IDirect3DDevice9`/memoria del motor, comparación de la traza completa de llamadas): 400.000 casos de `RasterCreate` y 100.000 por ayudante, **0 diferencias**; cobertura de `CreateTexture`, cubo, superficie de profundidad, `EvictManagedResources`, `GetClientRect`, `malloc`/`realloc` de la pila de índices y reservas de la lista libre. Prueba negativa: 15 mutaciones, 15 detectadas.

## S3: callbacks x87 de `_rwcseg` (0x857000–0x857404)

`RwMathX87.{h,cpp}` (clase nueva): `TransformPoints` (0x857000), `TransformVectors` (0x85707F), `TransformPoint` (0x8570EF), `TransformVector` (0x857160), `MatrixMultiply` (0x8572A0), `SetFpuControlWord` (0x857404) y `RestoreFpuControlWord` (0x85742B, no estaba en el plan: la citaba `rwcore.cpp` con `fldcw`; se escribe con `_controlfp_s` convirtiendo la palabra cruda).
`rwcore.cpp` (peticion `FINALIZESTART` 17) instala ahora las funciones C++ en `_rwMatrixSetMultFn`/`_rwVectorSetMultFn` y llama a `SetFpuControlWord`/`RestoreFpuControlWord`.

* **Aritmética**: la x87 a 24 bits redondea cada `fmul`/`fadd` a float; se usan los intrínsecos SSE escalares `mulss`/`addss` en el **mismo orden** que el EXE: columna j = `((x·b0j + y·b1j) + z·b2j) [+ b3j]`. Las filas de A se leen antes de escribir (destino == A funciona, como en el EXE). Los de vectores devuelven la **entrada**.
  Sólo hay diferencia posible en el rango subnormal/desbordado (la x87 lleva exponente extendido hasta el `fstp`).
* `0x857404` guarda la palabra anterior (dword completo) en 0xCAF004, pone precisión de 24 bits y todas las excepciones enmascaradas (`| 0x3F`) y guarda la nueva en 0xCAF000; `_controlfp_s` toca también MXCSR y se restaura (`fldcw` sólo cambia la x87).
* **Verificación**: `bench_p3` (fuera del juego, x87 a 24 bits): 400.000 casos de cada rutina de vectores y de matrices (con destino en el sitio) y 8.312 de la palabra de control, **0 diferencias**. Prueba negativa 8/8.

## S4: CRT numérico (0x821B40, 0x82872C, 0x833605; `___updatetlocinfo` 0x825456 no se toca)

* **0x82872C** → `ExeCrt::MathError` (`CrtMath.cpp`, con enganche): el manejador de errores de la biblioteca matemática (`floor`, `ceil`, `pow`…). Tabla de 24 códigos (tipo, nombre y camino: genérico con `ERANGE`, de dominio con `EDOM` con/sin copia de arg1, de subdesbordamiento sin errno, y 0x1A → resultado 1,0), `struct _exception` de 0x20 bytes,
  llamada a `matherr` por el puntero 0x8E3128, errno por `ErrnoPtr` (0x82B181 ya C++). Los dobles pasan por la x87: un NaN señalizador sale silencioso. `CrtMath.cpp` y `CrtPow.cpp` la llaman sin dirección.
* **0x833605** (`xor eax, eax; ret`, 3 bytes pegados a la función siguiente: no admite salto) → `ExeCrt::DefaultMatherr`; `CrtMathInjectHooks` sustituye el puntero 0x8E3128 si sigue valiendo 0x833605.
* **0x821B40** (`_ftol2`: argumento en ST0, resultado en EDX:EAX; 633 llamadas del EXE, **ninguna** desde código vivo: las tres de funciones vivas están en 0x5D84C0, 0x5D8590 y 0x709CF0, ya cerradas en V2) → `ExeCrt::FloatToInt64`, equivalente exacto (redondeo al más cercano de `fistp`, corrección con el resto a 24 bits,
  indefinido `0x8000000000000000` para NaN, ∞ y |x| ≥ 2^63). **Sin enganche** (no se puede expresar la convención en C++ puro); el resto del C++ propio ya lo expresa en línea y lo cita en comentarios.
* **Verificación**: `bench_p3`: 400.000 casos de `_ftol2` y 400.001 de `MathError` (todos los códigos 0–0x3FF, tres modos de `matherr`, tres disposiciones de punteros, NaN señalizador/∞/cero), comparando resultado, errno y el `struct _exception`: **0 diferencias**. Negativa 5/5 (+1 mutación equivalente).

## Comprobaciones

* Compilación completa (`_build_VIVOP3.sh`, candado `compile_lock`): ver la sección «Resultados» más abajo.
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 archivos con ensamblador o `naked`.
* Filas nuevas en `docs/hooks.csv` (clases `RwMathX87`, `RwD3D9RasterDriver`, `QTMLRuntime`, `ExeCrt`; 12 enganches, `validated=0`).

## Reproducir

```
reversing\FnP3\build_bench.cmd <salida>        & <salida>\bench_p3.exe <gta_sa.exe> --casos 400000
reversing\FnP3\build_bench_d3d.cmd <salida>    & <salida>\bench_d3d.exe <gta_sa.exe> --casos 400000
sh reversing/FnP3/neg_run.sh ; sh reversing/FnP3/neg_run_d3d.sh
python -X utf8 contrib/codigo_vivo_cierre_p3.py
```

## Resultados

* **Compilación** (`_build_VIVOP3.sh`, build completo de la DLL en un worktree nuevo): `EXIT=0`, sin advertencias en los archivos tocados.
* **Humo** con `gta_reversed.asi` de esta rama (`cal_mal.args`, etiqueta `p3`): `prueba_ropero.py` → **BIEN** (puerta 2877 y marcador presentes); `prueba_agua.py` → **BIEN** (brillo medio 50,4, bajo el agua). El primer intento de `prueba_agua.py` terminó a los 5 s con
  `FALLÓ (hr=0x80070005, acceso denegado) en PSGLOBAL(diMouse)->Acquire()` (entrada del escritorio ocupada por otra sesión, ajeno al código); repetido, BIEN.
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 archivos con ensamblador o `naked`.
* `contrib/sync_docs.py` no se pudo ejecutar entero: `hooks2md` aborta con «hooks.csv validated=1 (11801) no coincide con evidencia aprobada (9168)», un desajuste que ya trae `origin/main` (no lo causan estas filas, todas `validated=0`). Se revirtieron los cambios colaterales de ese intento.
  `ReversedClasses.md` y los demás derivados quedan sin regenerar; las 12 filas nuevas de `hooks.csv` van a mano. `docs/hooks.csv` puede dar conflicto al integrar con las otras ramas.
