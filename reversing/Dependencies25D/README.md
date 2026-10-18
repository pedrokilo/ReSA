<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies25D (grupo g25d de BRIEF_21): 20 funciones de biblioteca RenderWare / controlador D3D9

PENDIENTE: banco preparado sin ejecutar dentro del juego. No constituye validacion hasta que `run_local.py` apruebe una corrida real. La evidencia previa sin juego (`offline/`: bytes REALES
del EXE en un proceso hijo frente al texto de la DLL, 20 480 pares, 0 diferencias, `run_local.py --analyze` aprobado y 14 errores sembrados detectados) no sustituye a la corrida.

## Funciones (las 20 son de biblioteca; ver `g25d-classification.md`)

| Funcion (catalogo) | Entrada | Convencion | Pares | Fuente |
| --- | --- | --- | --- | --- |
| RwCore::RwTextureGetMipmapping | 0x7F3550 | cdecl, 0 args, lee RwTextureGlobals+0x1C | 1024 | `rwcore.cpp` |
| RwCore::RwTextureGetAutoMipmapping | 0x7F3580 | cdecl, 0 args, lee RwTextureGlobals+0x20 | 1024 | `rwcore.cpp` |
| RwCore::RwTextureRegisterPlugin | 0x7F3BB0 | cdecl, 5 args -> 0x8084A0(&textureTKList, ...) | 1024 | `rwcore.cpp` |
| RwCore::RwTexDictionaryRegisterPlugin | 0x7F3C10 | cdecl, 5 args -> 0x8084A0(&texDictTKList, ...) | 1024 | `rwcore.cpp` |
| RwCore::RwTextureRasterGenerateMipmaps | 0x7F3CD0 | cdecl, 2 args, callback RwTextureGlobals+0x2C | 1024 | `rwcore.cpp` |
| RwCore::RwD3D9DynamicVertexBufferDestroy | 0x7F5AE0 | cdecl, 1 arg (void*), void | 1024 | `rwcore.cpp` |
| RwCore::RwD3D9DynamicVertexBufferUnlock | 0x7F5C90 | cdecl, 1 arg (void*), COM slot 12 | 1024 | `rwcore.cpp` |
| RwCore::_rwD3D9SetDepthStencilSurface | 0x7F5EF0 | cdecl, 1 arg (void*), void, COM slot 39 | 1024 | `rwcore.cpp` (+ `rwcore.h`, `WindowedMode.cpp`) |
| RwCore::_rwDeviceRegisterPlugin | 0x7F5F60 | cdecl, 0 args; el EXE es `jmp 0x4C9AB0` | 1024 | `rwcore.cpp` |
| RwCore::RwD3D9DeleteVertexShader | 0x7FAC90 | cdecl, 1 arg (void*), void, COM slot 2 | 1024 | `rwcore.cpp` |
| RwCore::RwD3D9CreatePixelShader | 0x7FACC0 | cdecl, 2 args (func, void**), COM slot 106 | 1024 | `rwcore.cpp` |
| RwCore::RwD3D9DeletePixelShader | 0x7FACF0 | cdecl, 1 arg (void*), void, COM slot 2 | 1024 | `rwcore.cpp` |
| RwCore::RwRasterGetCurrentContext | 0x7FAE60 | cdecl, 0 args, rasterStack[rasterSP] | 1024 | `rwcore.cpp` |
| RwCore::RwRasterRegisterPlugin | 0x7FB0B0 | cdecl, 5 args -> 0x8084A0(&rasterTKList, ...) | 1024 | `rwcore.cpp` |
| RwCore::RwRasterLockPalette | 0x7FB0E0 | cdecl, 2 args, stdFunc[23], ranura de salida = argumento `lockMode` | 1024 | `rwcore.cpp` |
| RwPlCore::RwFreeListCreate (catalogo previo: `_rwFreeListCreate`) | 0x801980 | cdecl, **4** args -> 0x8019B0(.., 1, NULL, hint) | 1024 | `rwplcore.cpp` |
| RwPlCore::RwFreeListCreateAndPreallocateSpace | 0x801B70 | cdecl, 6 args; el EXE es `jmp 0x8019B0` | 1024 | `rwplcore.cpp` |
| RwCore::RwRGBAToPixel | 0x803740 | cdecl, 2 args, stdFunc[2], ranura de salida = argumento `rasterFormat` | 1024 | `rwcore.cpp` |
| RwCore::RwTextureRegisterPluginStream | 0x804550 | cdecl, 4 args -> 0x8088E0(&textureTKList, ...) | 1024 | `rwcore.cpp` |
| RwCore::_rwObjectHasFrameReleaseFrame | 0x804F40 | cdecl, 1 arg (void*), void | 1024 | `rwcore.cpp` |

Total 20 480 pares. El hook de cada una se apaga para el lado original (EXE) y se enciende para el candidato (DLL) con `OriginalDependencies499/HookRoute.hpp`
(`RH_ScopedGlobalInstall` desde `RwCoreInjectHooks25d()` y `RwPlCoreInjectHooks25d()`; clases `RwCore` y `RwPlCore`, categoria `Renderware`). Las 20 filas del catalogo estan en `g25d-hooks.csv.txt` (reversed=1, locked=0, validated=0).

## Mundo falso y estado global (Scenes.hpp)

Todo vive en un arena propio del banco (`kMem` = 0x300 bytes con 16 de guarda a cada lado): el motor RenderWare falso (`RwGlobals` con su tabla `stdFunc`, todas las ranuras a una trampa salvo
la 2 `RGBTOPIXEL` y la 23 `RASTERLOCKPALETTE`), las regiones de modulo (textura: +0x1C/+0x20/+0x2C; raster: pila de 10 contextos + puntero de pila +0x28), la lista de buferes dinamicos de
vertices, los objetos COM falsos (bufer, sombreadores, dispositivo; una sola tabla virtual de 128 ranuras con trampas y cuatro metodos reales: 2 `Release`, 12 `Unlock`, 39
`SetDepthStencilSurface`, 106 `CreatePixelShader`) y la basura determinista por caso. Ningun objeto real del motor se toca. Los **seis globales del EXE** que estas funciones leen o escriben se
apuntan al arena durante cada llamada y se restauran justo despues, siempre ANTES de escribir en el registro (`Write` bombea mensajes de Windows):

| Global | Direccion | Lo usan |
| --- | --- | --- |
| `RwEngineInstance` | 0xC97B24 | GetMipmapping, GetAutoMipmapping, GenerateMipmaps, RasterGetCurrentContext, RasterLockPalette, RGBAToPixel |
| `textureModule.globalsOffset` | 0xC97B4C | GetMipmapping, GetAutoMipmapping, GenerateMipmaps |
| `rasterModule.globalsOffset` | 0xC980D8 | RasterGetCurrentContext |
| cabeza de la lista de buferes dinamicos D3D9 | 0xC97B8C | DynamicVertexBufferDestroy |
| superficie de profundidad actual | 0xC9808C | _rwD3D9SetDepthStencilSurface (lee y escribe) |
| dispositivo D3D9 (`_RwD3DDevice`) | 0xC97C28 | _rwD3D9SetDepthStencilSurface, CreatePixelShader |

Escribe: `_rwD3D9SetDepthStencilSurface` (0xC9808C), `RwD3D9DynamicVertexBufferDestroy` (dos palabras del nodo) y `_rwObjectHasFrameReleaseFrame` (dos enlaces vecinos); el resto solo lee.

## Colaboradores del EXE sustituidos (en AMBOS lados, mismo falso)

`0x8084A0` (`_rwPluginRegistryAddPlugin`), `0x8088E0` (`_rwPluginRegistryAddPluginStream`), `0x8019B0` (`RwFreeListCreateAndPreallocateSpace` real) y `0x4C9AB0` (registro del plugin de
raster D3D9). Las cuatro son codigo del EXE sin hook que la DLL llama por direccion (los envoltorios `_rwPluginRegistry*` de `rwplcore.cpp` y las llamadas por puntero de `rwcore.cpp`/`rwplcore.cpp`),
asi que basta un `EntryPatch` en la direccion del EXE (no hay cuerpo de DLL que parchear); se comprueba al empezar que ninguna es un salto a la DLL. Cada falso anota en la traza sus argumentos
(los punteros del arena como desplazamientos, el resto crudos) y devuelve el valor de la fixture. Asi se prueba el registro que se pasa, el orden y la forma de los argumentos, el `1`/`NULL` fijo de
`RwFreeListCreate` y el retorno, sin ejecutar el colaborador (que no se valida aqui: ver `g25d-classification.md`, apartado de limites).

## Que se compara en cada par (todo en bits; original == candidato == oraculo)

- Valor de retorno (EAX completo de 32 bits) salvo en las 5 funciones `void` (DynamicVertexBufferDestroy, _rwD3D9SetDepthStencilSurface, RwD3D9DeleteVertexShader, RwD3D9DeletePixelShader,
  _rwObjectHasFrameReleaseFrame): ningun llamador lee su EAX (evidencia en `g25d-classification.md`); el residuo se registra (`*_residue`) pero no se exige.
- Ventana del arena tras la llamada (de 8 a 0xC0 bytes segun la funcion) y que TODO el arena fuera de la ventana (y las guardas) quede intacto (`outside_unchanged`); traza de colaboradores y de metodos COM;
  numero de trampas (debe ser 0); los seis globales del motor tras la llamada (antes de restaurarlos); hash de todo el arena antes de la llamada (misma entrada en ambos lados).
- ABI: ESP restaurado y EBX/ESI/EDI/EBP conservados (`stack_restored`, `nonvolatile_preserved`), ruta (`route_ok`: el candidato salta a la DLL, el original no), EAX/ECX/EDX sembrados con valores por caso
  y seis argumentos de pila (los que la funcion no usa llevan `0xC0DE0000`).
- x87 completo (FNSAVE de 108 bytes con TOP, FCW, FSW, FTW y los diez bytes de cada registro ST, incluidos los vacios) y MXCSR. Antes de cada llamada `PrimeX87()` deja en los ocho registros un
  residuo conocido. `kCompareDeadX87Registers = true` (no se relaja). Ninguna de las 20 funciones ejecuta x87 (ni el EXE ni el C++ de la DLL): FIP debe ser el del ultimo `fstp` de `PrimeX87` (en el ASI)
  en ambos lados y coincidir sin normalizar. No hay helpers `X87Plant*`/`Ftol2RegsCore` que plantar (por eso no hay `g25d-x87.*`).

## Fixtures (Fixtures.hpp; fixture_oracle.py las repite) — 1024 por funcion

- **Texturas** (GetMipmapping/GetAutoMipmapping/GenerateMipmaps): 2 posiciones del motor falso x 3 de la region de modulo (cambia el `globalsOffset`), campos +0x1C/+0x20/+0x2C con los 32 enteros de estres
  (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x100, 0xDEADBEEF, bits de NaN/inf/FLT_MAX...), el resto de la region con basura distinta (detecta un desplazamiento equivocado); el callback devuelve 0 en 278 casos y
  un valor no nulo (1, 2, 0x100, 0x80000000, 0xFFFFFFFF, punteros...) en 746 (la funcion devuelve `!= 0`).
- **RasterGetCurrentContext**: `rasterSP` de -4 a 13 (568 en la pila, 456 fuera: lee el contexto anterior, el propio puntero de pila en la posicion 10 y la basura siguiente; el EXE no comprueba el rango), 3 x 2 posiciones.
- **Registro de plugins** (4 funciones): tamano, id y tres callbacks con enteros de estres y valores aleatorios; el falso devuelve valores con y sin bit alto.
- **DynamicVertexBufferDestroy**: lista de 0 a 6 nodos en una permutacion de 7 ranuras (6 permutaciones), buferes de un conjunto de 4 valores con repeticiones y el valor 0 (154 casos con mas de un nodo coincidente:
  solo el primero cambia; 81 con vertexBuffer == NULL coincidente), argumento presente, ausente (0x44440000) o 0xFFFFFFFF, lista vacia (147).
- **Objetos COM** (Unlock, Release x2): objeto en 2 posiciones, HRESULT de 16 valores (exito, S_FALSE, positivos, limites de signo, E_FAIL, D3DERR_INVALIDCALL, E_INVALIDARG, E_OUTOFMEMORY...), contadores propios (el falso los
  incrementa/decrementa: prueba que la llamada se hizo una sola vez y sobre el objeto correcto).
- **_rwD3D9SetDepthStencilSurface**: superficie actual y nueva entre {NULL, 0xA00010, 0xB00020, 0xFFFFFFFF} (256 casos iguales sin llamada, 768 con llamada), HRESULT de 16 valores, mutadores del falso
  (ninguno / reescribe el global de superficie = 252 / pone a NULL el puntero del dispositivo = 252: prueba que la funcion NO vuelve a escribir tras la llamada y que escribe ANTES de llamar).
- **CreatePixelShader**: puntero de funcion NULL (147) o aleatorio, 4 ranuras de salida, modos del falso (escribe si SUCCEEDED / siempre / nunca), HRESULT de 16 valores.
- **RasterLockPalette / RGBAToPixel**: retorno de la funcion estandar de 8 valores (0, 1, 2, 0xFFFFFFFF, 0x80000000, 0x100, 0xFF00, 0x7FFFFFFF), modos de escritura (siempre / nunca / solo si el retorno es no nulo) y
  modo/formato de 32 enteros de estres: prueba la ranura de salida compartida con el argumento (resultado = `lockMode`/`rasterFormat` si el falso no escribe).
- **FreeList** (2 funciones): cuatro argumentos de estres y dos de basura (los dos ultimos de RwFreeListCreate NO deben llegar al colaborador).
- **_rwDeviceRegisterPlugin**: retorno del colaborador de 32 enteros de estres.
- **_rwObjectHasFrameReleaseFrame**: 8 topologias de lista (normal, un solo elemento con centinela, `prev` o `next` apuntando al propio enlace, a `parent`, a la cabecera o a `prev`: el ultimo caso fuerza la relectura de
  `prev` tras la primera escritura) x 5 valores de `parent` (0 = sin marco: no hace nada; 1, 0xFFFFFFFF, 0x80000000, 0x12345678).

## Compuertas de run_local.py

Igualdad fila a fila (`probe_case.analyzer`), hooks registrados y cuerpos en el ASI, colaboradores sustituidos, todas las compuertas de ABI/x87/ruta en ambos lados, `fp_difference` vacio, fixture registrada ==
oraculo (incluidos globales de entrada, semillas y argumentos de pila), resultado de CADA lado == oraculo independiente (retorno, ventana final, traza, seis globales; el oraculo del arena de
`_rwObjectHasFrameReleaseFrame` simula la memoria con los aliasing), cero trampas, cobertura minima de cada clase de estres y final con restauracion. `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza un registro.

## Evidencia sin juego (offline/)

`offline/extract.py` extrae sin cambios el texto de las 22 funciones (20 + los dos envoltorios `_rwPluginRegistry*`) de `rwcore.cpp` y `rwplcore.cpp` a `rw_cpp.inc`; `offline/main.cpp` (compilado con `bld.bat`, opciones
del proyecto en Debug `/Od /RTC1`) mapea los bytes reales del EXE en un proceso hijo (`D:\Grand Theft Auto San Andreas\gta-sa.exe`), instala los cuatro falsos de los colaboradores y ejecuta las 20 x 1024 parejas EXE/C++
con las mismas escenas y compuertas de `Scenes.hpp` (la compuerta x87 solo existe dentro del juego). Resultado al entregar: 20 480 pares, 0 diferencias, `run_local.py --analyze` aprobado (`offline/rehearsal-summary.json`).
`offline/mutations.json` lista 14 errores sembrados a proposito en el C++ extraido (salida inicial a NULL en RasterLockPalette/RGBAToPixel, campo equivocado en GetAutoMipmapping, escritura del global tras la llamada en
SetDepthStencil, falta de `return` tras el primer nodo, relectura de enlaces, bloques a 0, `> 0` en lugar de `>= 0` en dos HRESULT, indice fijo en RasterGetCurrentContext, retorno 2 en GenerateMipmaps, registro y argumentos
permutados, funcion sin llamada): el arnes los detecta los 14 (de 64 a 1024 diferencias cada uno). El arnes sin juego NO sustituye al banco: no ejecuta el ASI ni el mecanismo RH.

## Ejecucion

```
python -B reversing/Dependencies25D/fixture_oracle.py                 # cobertura declarada de las fixtures
python -B reversing/Dependencies25D/build_probe.py
python -B reversing/Dependencies25D/run_local.py
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies25D
```
Sin juego (no toca `build\`): `reversing\Dependencies25D\offline\bld.bat` y despues `offline\obj\rw25d.exe 1024 salida.jsonl` y `python -B run_local.py --analyze salida.jsonl`.
Antes: aplicar `g25d.patch`, anadir las filas de `g25d-hooks.csv.txt` a docs/hooks.csv y compilar.
