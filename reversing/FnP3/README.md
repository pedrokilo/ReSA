# FnP3: bancos fuera del juego de la parte P3 del codigo vivo (lotes S2, S3 y S4)

Rama `vivo-p3` (desde `origin/main` 410d9f2d84, 21/11/2026). Decision del usuario: no se valida funcion por funcion dentro del juego; basta un banco fuera del juego cuando el metodo ya existe, mas el humo.
Metodo: el de FnCRT1A..FnCRT4A. El `gta_sa.exe` original (solo lectura; copia identica en `D:\Original\_crt1a_work\gta_sa.exe`) se mapea en 0x400000 dentro de un proceso de prueba de 32 bits (`FnCRT1A/reserva_imagen.asm` reserva
el rango) y se llama por su direccion; el C++ propio se compila en el mismo proceso y se compara bit a bit. El juego no se ejecuta. Sin `__asm` en el proyecto: las plantillas de llamada (argumento en ST0, registros EDI/ESI/EAX) son
bytes en memoria ejecutable que solo existen en el banco.

## Bancos

| Banco | Compila | Cubre | Resultado |
| --- | --- | --- | --- |
| `bench_p3.cpp` (`build_bench.cmd <salida>`; `bench_p3.exe <EXE> --casos N --solo vec\|mat\|cw\|err\|ftol`) | `CrtMath.cpp` y `RwMathX87.cpp` tal cual | 0x857000, 0x85707F, 0x8570EF, 0x857160, 0x8572A0, 0x857404, 0x85742B; 0x821B40; 0x82872C y 0x833605 | `resultados/corrida_completa.txt`: 0 diferencias |
| `bench_d3d.cpp` + `bench_d3d_helpers.inc` (`build_bench_d3d.cmd <salida>`; `bench_d3d.exe <EXE> --casos N`) | `D3D9RasterDriver.cpp` tal cual, con cabeceras minimas (`shim/`) | 0x4CCE60 con sus ayudantes usercall 0x4CB7C0, 0x4CB9C0, 0x4CC5C0, 0x4CCD70 y 0x4CB730 | `resultados/corrida_d3d.txt`: 0 diferencias |

### bench_p3
* **Vectores y matrices**: x87 a 24 bits (`_controlfp_s(_PC_24)`), matrices y vectores de exponente en [-30, 30] (con ceros con signo, 1 y -1), cuentas de 1 a 6 puntos, destino aparte y en el sitio (destino == entrada, destino == A). Se compara el resultado
  (bytes completos del destino, incluidos los dwords que la rutina no debe escribir) y el valor devuelto (el EXE devuelve la **entrada**, no la salida). Limite: sin desbordes ni subnormales (la x87 lleva exponente extendido hasta el `fstp` final).
* **Palabra de control**: 5 mascaras de excepciones x 4 redondeos x 3 precisiones x 2 contenidos previos de 0xCAF000 (precondicion: `_clearfp`), comparando la palabra resultante, los dwords 0xCAF000 y 0xCAF004 y la restauracion; ademas 8.192 palabras guardadas
  arbitrarias (bits reservados y precision 01) para 0x85742B.
* **0x821B40 (`_ftol2`)**: el argumento va en ST0; se llama por una plantilla `fld qword [esp+4]; call; ret`. Bordes (±0,5, ±1,5, 2^31, 2^32, 2^63, ±inf, NaN, ±0), enteros, mitades, floats del juego y multiplos de 2^32 con parte baja a cero.
* **0x82872C**: se parchean en la copia mapeada la llamada a `_errno` (0x82B181) y a la comprobacion de cookie (0x82AAD4) para que usen utilidades del banco. Se recorren los 0x400 codigos (todos los de la tabla y los que no hacen nada), tres modos de matherr
  (el 0x833605 del EXE, uno del banco que trata el error y cambia el valor devuelto, y otro que no lo trata), tres disposiciones de punteros (distintos, los tres iguales como en `RaiseDomain`, arg1 == resultado) y valores aleatorios con NaN senalizador,
  infinitos y ceros. Se compara el resultado, errno y el `struct _exception` completo (tipo, nombre, arg1, arg2, valor devuelto).

### bench_d3d
Falsos del banco: `IDirect3D9` (CheckDeviceFormat, CheckDepthStencilMatch), `IDirect3DDevice9` (EvictManagedResources, CreateTexture, CreateCubeTexture, CreateDepthStencilSurface), `GetClientRect` (IAT 0x858304), las funciones de memoria del motor
(`RwEngineInstance` +0x134..+0x148) y `RwFreeListCreate` (llamada 0x801980 parcheada en la copia del EXE). Los resultados de los falsos salen de un hash de sus argumentos (exito, resultado positivo, no disponible, sin memoria de video), asi que cada
rama del C++ y del EXE ve las mismas respuestas. Corren tal cual en los dos lados 0x7FED70 (tamano valido), 0x730510 (cache de texturas), 0x4CBDE0..0x4CC020 (consultas de formato) y 0x4CB530 (lista de rasters).
Por caso (estado inicial por semilla: formatos, capacidades, profundidades, RECT de la ventana, listas de paletas y de rasters, basura en el raster y su extension) se compara: valor devuelto, bytes del raster y de la extension, la **traza completa de llamadas a los falsos**
(orden y argumentos), la lista de rasters, la pila de indices de paleta y los globales del controlador. Ademas, cada ayudante usercall se llama por separado con una plantilla en memoria ejecutable (EDI/ESI/EAX) y se compara contra su version C++,
con el bit de cubo y las banderas de mipmaps puestos (RasterCreate los pone a cero antes de crear, asi que el camino de cubo solo se alcanza llamando al ayudante).

## Prueba negativa
`neg_run.sh` (bench_p3: 14 mutaciones de una linea; 13 detectadas, la 14 es equivalente: floor en lugar de redondear al mas cercano en la emulacion de `_ftol2`, que la correccion posterior absorbe) y `neg_run_d3d.sh` (15 mutaciones: **15 detectadas**).
Salidas en `resultados/prueba_negativa.txt` y `resultados/prueba_negativa_d3d.txt`. Los NaN senalizadores no se detectan con una mutacion porque el compilador (/arch:IA32) copia los doubles con `fld/fstp` y los silencia igual: el silenciado de `X87Pass` (`MathError`) se mantiene explicito para no depender del codegen.

## Lo que NO se banquea
* **QuickTime (S1)**: implementacion nula (el EXE necesita el despachador de QuickTime, que no existe); se razona la equivalencia en `docs/CodigoVivoExe/ejecucion-p3.md`.
* Humo con el juego (ropero y agua) y compilacion del proyecto: ver `docs/CodigoVivoExe/ejecucion-p3.md`.
