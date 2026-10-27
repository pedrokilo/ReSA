<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helpers66C: CAEMFDecoder 0x4E7990 y 0x4E7A80, GetDXVersion 0x745650, RwTextureGtaStreamRead 0x730E60 y SkinGetBonePositionsToTable 0x735360

Estado: **APROBADO dentro del juego** (21/10/2026, worktree `D:\Original\wu-3`, HEAD 5765c0b5f). 20480 pares (5 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados,
`fp_difference` vacio y cobertura por familia (16 x 256 por funcion) y de ramas.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helpers66c\20261021-110237-738753`.

| Funcion | Direccion | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `AEMFOpenSoundMedia` (abrir pelicula y media de sonido) | 0x4E7990 | `CAEMFDecoder` / `OpenSoundMedia` / S (`RH_ScopedNamedGlobalInstall`) | QuickTime 0x818970, 0x81DAA0, 0x81B680, 0x81AB80, 0x81A9E0, 0x81BC60, 0x81A0E0 |
| `AEMFGetMediaSoundFormat` (formato de la media) | 0x4E7A80 | `CAEMFDecoder` / `GetMediaSoundFormat` / S | QuickTime 0x818DB0, 0x81A300, 0x819860, 0x81C600, 0x8192B0, 0x818F90, 0x818E70, 0x818D70, 0x819390, 0x818FB0, 0x819270 |
| `GetDXVersion` | 0x745650 | `Ps` / `GetDXVersion` / S (`RH_ScopedGlobalInstall`, en `WinPsInjectHooks`) | LoadLibraryA, GetProcAddress, FreeLibrary y OutputDebugStringA en las ranuras de importacion (EXE 0x858070, 0x85806C, 0x858110, 0x858118, y las de la DLL); DirectDrawCreateEx y Release falsos |
| `RwTextureGtaStreamRead` | 0x730E60 | `RwHelper` / `RwTextureGtaStreamRead` / S (`RH_ScopedGlobalInstall`, en `RwHelperInjectHooks`) | RwStreamFindChunk 0x7ED2D0, CTimer::GetCurrentTimeInCycles 0x561A80, GetCyclesPerMillisecond 0x561A40, RpAnisot 0x748F50/0x748F20/0x748F30 (con hook: se parchean EXE y DLL), ranura stdFunc[26] de RwEngineInstance |
| `SkinGetBonePositionsToTable` | 0x735360 | `RwHelper` / `SkinGetBonePositionsToTable` / S | RpClumpForAllAtomics 0x749B70, RpSkinGeometryGetSkin 0x7C7550, RpSkinGetNumBones 0x7C77E0, RpSkinGetSkinToBoneMatrices 0x7C7810, RwMatrixInvert 0x7F2070, RwV3dTransformPoints 0x7EDD90 |

Filas de `docs/hooks.csv` (las anade el coordinador; todas tipo S): `CAEMFDecoder,OpenSoundMedia,0x004E7990,1,0,1,S`, `CAEMFDecoder,GetMediaSoundFormat,0x004E7A80,1,0,1,S`, `Ps,GetDXVersion,0x00745650,1,0,1,S`,
`RwHelper,RwTextureGtaStreamRead,0x00730E60,1,0,1,S`, `RwHelper,SkinGetBonePositionsToTable,0x00735360,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh Helpers66C        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/Helpers66C/build_probe.py
python -B reversing/Helpers66C/run_local.py
python -B reversing/Helpers66C/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aun no existe o no esta completo
python -B reversing/Helpers66C/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers66c/20261021-110237-738753"   # sólo lo ejecuta el coordinador
```
No hay `fixture_oracle.py`: la referencia es el propio EXE (hook apagado). Las fixtures son C++ estandar en `Fixtures.hpp`.

## Fuente (`source.patch`: sólo estas cinco funciones respecto a HEAD; `git apply --check` contra los blobs de HEAD correcto)

- `Audio/Loaders/AEMFDecoder.cpp/.h`: `AEMFOpenSoundMedia` y `AEMFGetMediaSoundFormat` (libres, declaradas en el .h) con `RH_ScopedNamedGlobalInstall` en `CAEMFDecoder::InjectHooks`.
  `Initialise` (0x4E7CB0) sigue llamandolas por su direccion (con el hook, el salto del EXE va a la DLL): asi el banco MFDec, que las sustituye por falsos en la direccion del EXE, sigue siendo valido.
- `app/platform/win/WinPs.cpp`: `GetDXVersion` (estatica, como `GetVideoMemInfo`) + hook; `psInitialize` la sigue llamando por 0x745650 (banco Ps22).
- `RenderWare/rw/rwtexdict.cpp` + `RwHelper.cpp`: `RwTextureGtaStreamRead` en C++ + hook; `X87Math.cpp/.h` (bloque «Lote 66C», prefijo `Rt66`): `Rt66TextureReadStartCpp` / `Rt66TextureReadEndCpp` (cociente y media incremental
  en x87 de 24 bits con exponente extendido; el sumando 2^32 de los enteros con el bit alto se redondea a 24 bits como en el EXE).
- `RwHelper.cpp`: `SkinGetBonePositionsToTable` en C++ + hook. Los callbacks se pasan como direcciones del EXE (0x734810, 0x734A20), como el original.

## Revision contra el EXE (re-desensamblado y contrastado)

**0x4E7990.** `NativePathNameToFSSpec(nombre, FSSpec, 0)` (resultado ignorado) -> 0x81DAA0(FSSpec, FInfo) -> `OpenMovieFile(FSSpec, &refNum, 1)` -> `NewMovieFromFile(&movie, refNum, &resId, 0, 1, &cambiado)` y, SIEMPRE, `CloseMovieFile(refNum)`
antes de mirar el error de NewMovieFromFile -> `GetMovieIndTrackType(movie, 1, 'soun', 1)` (nula: -2009) -> `GetTrackMedia` (nula: -2008, pero igualmente escribe la media). Escribe `*outMedia` y `*outMovie` solo si hay pista. El resultado se compara con `test ax,ax`
(16 bits); el unico llamador (0x4E7CCA) solo mira AX. Los 16 bits altos de `refNum` (dword de pila sin inicializar en el EXE) no son observables: el banco los enmascara.

**0x4E7A80.** `NewHandle(0)` -> `GetMediaSampleDescription(media, 1, h)` -> `GetMoviesError` (AX). Sin error: segundo `NewHandle(0)`, `GetSoundDescriptionExtension(h, &ext, 'wave')`; si va bien: `GetHandleSize`, `HLock`, `NewPtr` -> `*outAtom`,
`MemError` (pasa a ser el resultado), `BlockMoveData(*ext, *outAtom, tam)` (relee `*outAtom`), `HUnlock`; si no, resultado 0. Luego rellena `info` releyendo el handle en cada campo (+0x28 = +4 de la descripcion, +4 = word +0x18 con signo, +0x3E = word +0x1A,
+8 = dword +0x20, +0x38 = word +0x1C). Al final `DisposeHandle(ext)` y `DisposeHandle(h)` si no son nulos. **Hueco de pila sin inicializar:** el EXE usa el dword de `push ecx` como handle de la extension; si `GetMoviesError` falla,
`DisposeHandle` recibe el ECX de entrada. El unico llamador (0x4E7CE4) llega con ECX = la media; el C++ lo reproduce usando `media`, y el banco entra con ECX = media.

**0x745650.** LoadLibraryA("DDRAW.DLL") -> GetProcAddress("DirectDrawCreateEx") -> `DirectDrawCreateEx(0, &obj, IID_IDirectDraw7 [0x859694], 0)` (HRESULT < 0 es fallo) -> `obj->Release()` (ranura 2) -> LoadLibraryA("D3D8.DLL") (0x700 si falla),
"dpnhpast.dll" (0x800; en ese caso FreeLibrary(NULL), DDRAW, D3D8), "D3D9.DLL" (descarga dpnhpast, DDRAW, D3D8; 0x801 si falla); con todo cargado FreeLibrary(D3D9) y 0x900. En cada fallo, FreeLibrary de lo cargado y despues OutputDebugStringA (texto con `\r\n`),
excepto 0x800/0x801 donde el aviso va despues de las descargas. Llamador (0x74754A): lee EAX completo; ECX/EDX se reescriben antes de usarse.

**0x730E60.** `RwStreamFindChunk(stream, 0x15, &len, &ver)`; hora (0x561A80) y divisor (0x561A40) -> cociente float (enteros sin signo via fild + 2^32); `stdFunc[26](stream, &tex, len)` (nulo: 0); con `gGameState == 8` (aunque la textura sea nula)
actualiza la media incremental (0xC87FEC = ((n * media) + (t/d - inicio)) / (n + 1), n en 0xC87FF0); textura nula: 0; filtro (byte bajo de +0x50) 1 -> 2 y 3 -> 4; con 0xC87FFC no nulo, anisotropia de la textura >= 1 (byte con signo) y
`g_fx.GetFxQuality() >= 2` (int con signo): `SetMaxAnisotropy(tex, GetMaxSupportedMaxAnisotropy())`. Devuelve la textura. Llamadores (0x731026, 0x7310E1, 0x731185 y RwTexDictionaryGtaStreamRead): solo EAX.

**0x735360.** Tabla nula: nada. `RpClumpForAllAtomics(clump, 0x734810, &atomo)`, `RpSkinGeometryGetSkin(atomo->geometry)` (sin comprobar nulo), `RpClumpForAllAtomics(clump, 0x734A20, &jerarquia)`, `tabla[0] = 0`, `RpSkinGetNumBones` (con signo); para i = 1..n-1:
copia la matriz i (0x40 bytes) -> `RwMatrixInvert` -> copia la matriz del padre (indice << 6) -> `RwV3dTransformPoints(&tabla[i], &inversa.pos, 1, &padre)`; banderas `pNodeInfo[i].flags` (nodo de 0x10 bytes): PUSH (2) apila el padre, POP (1) desapila, si no POP el padre pasa a ser i. Llamador (0x4D657A): no lee salidas.

## Defectos hallados

- Del banco (no de la funcion): `small` es una macro de Windows (`rpcndr.h`: `char`); el generador de banderas contaba el nodo 0, que el bucle nunca lee, y producia un POP sin PUSH (el EXE leia pila sin inicializar y reventaba). Ambos corregidos.
- Efecto en bancos que usaban estas direcciones: `Helpers65D` (0x730E60 pasa a ser colaborador con hook: par `{0x730E60, DllBody, true}`) y `RpAnim43` (0x735360 con cuerpo de la DLL) se actualizaron; ver el informe de entrega para su estado.

## Suposiciones y riesgos

- Los QuickTime reales no se ejecutan nunca. El hueco `push ecx` de 0x4E7A80 se modela con ECX = media (unico llamador); otro llamador con otro ECX diferiria solo en la ruta de error de `GetMoviesError`.
- `GetDXVersion` sustituye las APIs en las ranuras de importacion del EXE y de la DLL (otro hilo que las llame en ese instante pasa al original); la DLL debe importar LoadLibraryA/GetProcAddress/FreeLibrary/OutputDebugStringA por su IAT.
- `SkinGetBonePositionsToTable` no se prueba con POP sin PUSH previo, profundidad > 24 ni atomo nulo (el EXE lee pila sin inicializar o desreferencia nulo).
- `RwTextureGtaStreamRead` se prueba con la lectura nativa sustituida (puntero en `RwEngineInstance + 0xB0`); la hora y el divisor son falsos.
