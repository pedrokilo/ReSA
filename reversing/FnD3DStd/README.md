# FnD3DStd: bancos fuera del juego de los manejadores de raster de la tabla estandar de D3D9 (rama `vivo-d3dstd`)

Rama `vivo-d3dstd` (desde `origin/main` d911da633b, 21/11/2026). Decision del usuario: no se valida funcion por funcion dentro del juego; basta un banco fuera del juego cuando el metodo ya existe, mas el humo.
Metodo: el de `reversing/FnP3` (bench_d3d). El `gta_sa.exe` original (solo lectura; copia identica en `D:\Original\_crt1a_work\gta_sa.exe`) se mapea en 0x400000 dentro de un proceso de prueba de 32 bits (`FnCRT1A/reserva_imagen.asm` reserva el rango) y se llama por su direccion; el C++ propio se compila en el mismo proceso
y se compara. El juego no se ejecuta. Sin `__asm` en el proyecto (los dos bancos solo usan desvios `jmp rel32` en la copia mapeada del EXE).

## Bancos

| Banco | Compila | Cubre | Resultado |
| --- | --- | --- | --- |
| `bench_std.cpp` (`build_bench.cmd <salida>`; `bench_std.exe <EXE> --casos N [--solo K]`) | `D3D9RasterStandards.cpp` tal cual (cabeceras minimas en `shim/`) | los 12 manejadores 0x4CA4E0, 0x4CAE40/60/80, 0x4CB4C0/E0, 0x4CB520, 0x4CBCB0, 0x4CBD40/50, 0x4CD4D0 y 0x4CD820, con sus ayudantes usercall 0x4CAD90, 0x4CAEA0, 0x4CAAE0 y 0x4CA5A0 (todos se alcanzan por los manejadores, que son cdecl) | `resultados/corrida_std.txt` |
| `bench_inst.cpp` (`build_bench_inst.cmd <salida>`; `bench_inst.exe <EXE> --casos N`) | el texto de `D3D9InstanceObject` (`rpworld_p6b.cpp`), extraido tal cual por `extract_inst.py` | 0x756960 (instanciado generico de los nodos AllInOne de D3D9, 1.080 bytes) | `resultados/corrida_inst.txt` |

`desens.py` es una utilidad (capstone sobre el EXE mapeado por secciones PE) que se uso para desensamblar las 17 rutinas (`python desens.py <va hex> <bytes>`).

### Falsos del banco (los mismos para el EXE y para el C++)
Cada falso escribe en la **traza** su marca y sus argumentos (valores, nunca direcciones de pila) y devuelve resultados deterministas por hash de sus argumentos y de la semilla del caso, asi que cada rama del C++ y del EXE ve las mismas respuestas. En el EXE se desvian (salto en la entrada de la copia mapeada):
* **bench_std**: `_rwD3D9PixelToRGB/RGBToPixel`, `_rwD3D9SetRenderTarget`, `_rwD3D9SetDepthStencilSurface`, `RwD3D9SetRenderTarget`, `_rwD3D9RestoreDevice`, `_rwD3D9RWGet/SetRenderState`, `RwD3D9Get/SetRenderState`, `_rwD3D9RWSetRasterStage`, `_rwD3D9RenderStateVertexAlphaEnable/SrcBlend/DestBlend`,
  `_rwD3D9Im2DRenderPrimitive` (traza los 4 vertices completos), bloqueo/desbloqueo de rasters (0x4C9F90, 0x4CA290, `RwRasterLock/Unlock`, `LockPalette/UnlockPalette`, `GetNumLevels`), `RwRasterCreate/Destroy`, `RwTextureCreate/SetName/SetMaskName`, flujos (`RwStreamRead/Write/FindChunk`,
  `_rwStreamWriteVersionedChunkHeader`), `D3D9RasterFormat::GetRasterMemorySize/CreateRasterFromD3DFormat`, `CheckTextureFormat`/`Usage400`/`CubeFormatUsage400` y `D3DResourceSystem::CreateTexture`; y por vtables falsas los metodos COM de `IDirect3DDevice9` (CreateCubeTexture, UpdateSurface, GetRenderTargetData, StretchRect,
  ColorFill, Clear, SetViewport) y de textura, textura de cubo y superficie (GetLevelCount, GetLevelDesc, GetSurfaceLevel/GetCubeMapSurface, Release).
* **bench_inst**: `RwResourcesAllocateResEntry/FreeResEntry`, `RwD3D9IndexBufferCreate`, malloc/free del motor (`RwEngineInstance + 0x134/0x138`), el callback de instanciado, Lock/Unlock del bufer de indices (vtable falsa) y `qsort` del EXE (0x8247E0, necesita el CRT del EXE, que no esta inicializado en el banco: ordenacion
  por insercion del banco en los dos lados). `0x756710/0x756770/0x756830` (puras) corren del EXE en los dos lados.

Por caso (estado inicial por semilla: cadenas de rasters padre, tipos, profundidades, desplazamientos de 16 bits, extensiones con cubos, mipmaps, paletas y texturas falsas, capacidades del dispositivo; cabeceras de mallas y flujos) se compara: valor devuelto, la **arena completa** (rasters, extensiones, objetos COM, entradas de recursos,
cabeceras, datos de instancia), el bufer de pixeles y el de la paleta, el flujo escrito, el cursor de lectura, el raster destino actual y la **traza completa**. x87 a 24 bits (`_controlfp_s(_PC_24)`; las divisiones de coordenadas de textura son `fild`+`fidiv` en el EXE).

## Prueba negativa
`neg_run.sh` (bench_std: 43 mutaciones de una linea en `D3D9RasterStandards.cpp`) y `neg_run_inst.sh` (bench_inst: 21 mutaciones en `D3D9InstanceObject`). Salidas en `resultados/prueba_negativa.txt` y `resultados/prueba_negativa_inst.txt`. `ONLY="11 33" sh neg_run.sh` repite solo las mutaciones indicadas.
La primera pasada del bench_std dejo 4 sin detectar: una por un texto mal copiado (11, no se aplicaba) y tres porque el generador no producia el caso (33: version exacta 0x36003; 34: la mutacion apuntaba al texto equivocado; 35: capacidades de cubo con solo 0x20000); se corrigieron los generadores (versiones en los bordes, las cuatro combinaciones de capacidades)
y las mutaciones y ahora se detectan todas.

## Lo que se asume
* Los ayudantes usercall se banquean a traves de sus manejadores cdecl (el unico acceso desde el EXE son los manejadores); no hay plantillas de registro.
* `RasterBlit` con origen NULL devuelve 0; el banco no genera rasters con cadena de padres circular.
* 0x756830 (`_rpD3D9TriStripToTriList`) con menos de 3 indices da una vuelta gigante tambien en el EXE: el generador no produce mallas indexadas con menos de 3 indices.
* En 0x756960 el indice de la tabla de tipos primitivos (0x874FEC) se lee de un byte del motor que solo toma valores 0..6 con datos reales; el C++ lleva la tabla propia de 8 entradas (valores fuera de 0..7 darian 0 en lugar de leer mas alla de la tabla).
