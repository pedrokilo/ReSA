# FnS26A29A: lotes S26A, S27A, S28A y S29A del plan de sustitucion (oleada 2: RpWorld)

96 raices de RpWorld (RenderWare) reconstruidas en C++ puro y enganchadas (hook activo, `validated=0` en docs/hooks.csv, filas nuevas con tipo `S`). Los llamadores propios usan ahora la funcion propia en lugar de la direccion del EXE: las tres envolturas de RpWorldAddClump, los callbacks de plugin de RpWorldPluginAttach, de la tabla de plugins 0x509/0x50E/0x510/0x1F, de D3D9LightPluginAttach y de D3D9GeometryPluginAttach, los cuatro comparadores de qsort, la sincronizacion de RpLightCreate y el callback de restauracion de D3D9.

Archivos: `source/game_sa/RenderWare/rw/rpworld.cpp` (todo el codigo, al final del archivo; las envolturas por direccion se han quitado de su sitio), `rpworld.h` (`_rwD3D9VSGetWorldViewMatrix` ahora recibe la matriz destino, como el EXE), `source/InjectHooksMain.cpp` (`RpWorldInjectHooksS26A29A`). Politica: sin `__asm`, sin funciones duplicadas (los cuerpos repetidos del EXE llaman a ayudantes comunes: `ReadFixedChunkInfo`, `AtomicDestroyAllTies`, `ForAllLinksWithMarker`) y ninguna clase nueva.

Herramientas: `disx.py` (desensamblado con capstone; genera `dis_Sxx.txt`), `sitios.py` / `sitios.txt` (usos de las 96 direcciones en el codigo propio ANTES del cambio), `rd.py` (lee dwords del EXE, para tablas de saltos), `mapa.txt` (direccion -> funcion propia).

Verificacion: compilacion completa de Debug con build-project.ps1 (1 hilo, copia de _build_INT1.sh en D:\Original\_build_S26A.sh): EXIT=0, sin errores ni avisos nuevos. `python contrib/audit_hook_catalog.py --strict`: OK. NO se emulo contra el EXE (Unicorn) ni se ejecuto ningun banco ni el juego.

## Lista (direccion, tamano, funcion)

Ver `mapa.txt` para el nombre de la funcion propia de cada direccion; los tamanos estan en docs/ExeIndependiente/raices.csv.

| lote | raices | direcciones |
|---|---|---|
| S26A | 24 | 0x750E40..0x754B40 (plugins nativos 0x510, mundo de camara/atomo/clump/luz, RpWorldRemoveClump, registro de plugins de luz, modulo de luces, declaraciones de vertice) |
| S27A | 24 | 0x755BC0..0x75B500 (plugin 0x505 de luz D3D9, atenuacion, luces D3D9, comparadores, flags de uso, apertura/cierre del modulo de mallas, malla trivial, coste de tira por defecto) |
| S28A | 24 | 0x75BD80..0x764760 (generador de tiras rapido, metodo de tiras, tuberia por defecto, vertex shaders, tamano del flujo de un mundo, cabeceras de chunk, matrices del vertex shader) |
| S29A | 24 | 0x74BF40..0x764920 (instanciado, geometrias y listas de materiales, luces y clumps del mundo, ties, recorridos de sectores, cajas de sectores) |

## Notas y no comprobado
- No hay raices sin hacer: las 96 estan reconstruidas.
- Se conservan por direccion las rutinas del EXE que no son de estos lotes: 0x751500, 0x7515A0 y 0x751630 (callbacks de RpWorldRemoveClump), 0x75C290 (siguiente arista de tira; la usan RpTriStripMeshQuick y otros), 0x75F0B0 (crea un vertex shader), 0x762040 (tamano de un subarbol de sectores) y 0x755BF0 (destructor del plugin 0x505 de luz). Tambien el qsort del EXE (0x8247E0, ya enganchado por ExeCrt), 0x74EEC0 y los callbacks de camara/atomo/luz 0x750430, 0x750460, 0x750480, 0x7507E0 y 0x750AC0.
- 0x75BD80 (RpTriStripMeshQuick, 1283 bytes) y 0x75B500 (coste de tira) son transcripciones de flujo de control largas sin banco: los cubos de poligonos se reconstruyeron a partir del ensamblador (cuatro cubos por numero de vecinos, seis salidas por poligono, recolocacion de los vecinos a lo largo de la tira); si un modelo saliese con tiras distintas, empezar por aqui.
- 0x75EED0 (cache de vertex shaders): la busqueda binaria y la sustitucion con la cache llena (sin reordenar) siguen al EXE; la tabla puede quedar desordenada tras un reemplazo, igual que en el original.
- 0x763690: el formato antiguo (version < 0x34001) lee en un bloque de 64 palabras en lugar de las 13 del EXE (que desbordaria la pila con una longitud mayor que 0x30).
- 0x74F020: los tipos de plano se usan como desplazamiento en bytes del eje (0, 4 u 8), igual que el EXE; las palabras de los floats se copian como dwords.
- 0x74DA80: la rama del EXE para propiedades de superficie nulas (inalcanzable) se conserva como destruccion del clon (RpMaterialDestroy).
- 0x74C130: la ruta de fallo del desbloqueo es el cuerpo de RpGeometryDestroy.
