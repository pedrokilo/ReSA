# Reclasificación de las raíces por valor (huecos «muertos» que el EXE propio ejecuta)

Rama `vivo-raices` (desde `origin/main` 0366e8fabf, 21/11/2026). Análisis estático sobre el EXE canónico, el código fuente y los CSV; no se compiló el proyecto (ver §8). Generado por `contrib/huecos_p6.py` + `contrib/raices_valor_p6.py` (regenerable, §9). Base de la medida: [fase8-experimento.md](fase8-experimento.md) (rama `fase8-exp`).

## 1. Qué estaba mal

El experimento de la fase 8 construyó un EXE propio y vio que direcciones clasificadas como «muerto» (`huecos-p6.csv`: `citada-en-cpp`/`muerto`) o sin lista **se ejecutan**: `0x4CD820`, `0x750430`, `0x750460`, `0x750480`, `0x750AC0`, `0x7F98D0`, `0x7F99B0`. «Muerto» se decidía por falta de llamadores directos (`call`/`jmp`/inmediato en una función viva) y no vale para una dirección que el C++ **escribe como valor** (tabla estándar de rasters de D3D9, callbacks de `RwCamera`, `mov [reg+x], imm32` de los nodos de RenderWare): la llama el motor, por un puntero que el análisis no sigue. Además `citada-en-cpp` agrupaba cualquier cita, incluidos los comentarios, y la trataba igual: ni viva ni muerta, «documentar».

## 2. Regla nueva

Una dirección citada por el C++ **como VALOR** (asignación, inicializador, argumento, `reinterpret_cast`, llamada por dirección), no como comentario, es una **raíz viva** hasta demostrar lo contrario (`escanear_citas` de `contrib/raices_valor_p6.py`):

* un autómata separa en `source/` el **código**, los **comentarios** y las **cadenas**; cada literal `0x4xxxxx`–`0x8xxxxx` (sin carácter de identificador delante: `f0x57C330` es un nombre) se clasifica en `enganche` (línea `RH_Scoped*`), `llamada-directa` (`plugin::Call*<0x…>`), `valor` (todo lo demás en código), `libjpeg-cb` (`LIBJPEG_CB` o `JPegCompressPlugin`), `protector` (tablas de `ExeProtectionStub.cpp`), `isla-d3dx9` (los cuatro `D3dx*.cpp` que `ejecucion-v48.md` 2.4 ya trata como isla) y `comentario`;
* se cruza con el **código compilado**: [`fase8-experimento/dependencias_codigo.csv`](fase8-experimento/dependencias_codigo.csv) (direcciones del `.text` que los `.obj` del proyecto llevan como inmediato; confianza A/B/C). A/B cuentan como evidencia de valor; una C solo si hay literal de código en el C++ **y** el destino es un inicio de rutina plausible (alineado a 16 o tras `int3`/`nop`/`ret`) **y** la instrucción es `push`/`mov` (así `0x4CAE40`, entrada real de la tabla de rasters, sale de la clase «ruido» y `0x475441`, un identificador de temporizador, no);
* destino: **viva sin hook** (cola de reconstrucción); **viva enlazada** (libjpeg, libpng+zlib, D3DX9, CRT de MSVC: decisiones vigentes del brief); **viva de QuickTime** (se retira); **viva del protector** (tablas de `ExeProtectionStub.cpp`, se retiran con el archivo, V1); **cerrada por comentario** (solo la cita un comentario y ninguna otra entrada es viva); **cerrada por constante** (inmediato numérico); **cerrada isla D3DX9**. Nada se cierra por «no tiene llamadores».

La clase `citada-en-cpp` **desaparece**. Sigue sin verse una cita sin literal hexadecimal y no se distingue código no compilado (`#if 0`); en ambos casos el error es hacia «viva» o «sin ver», nunca hacia «muerta» con certeza (§9).

## 3. Cifras antes y después: piezas de huecos (914 piezas, 118.544 B)

| Destino | Antes (piezas / B) | Después (piezas / B) |
| --- | ---: | ---: |
| reconstruida | 297 / 55.138 | 297 / 55.138 |
| viva-sin-hook | 0 / 0 | 24 / 9.323 |
| viva-enlazada | 0 / 0 | 55 / 8.261 |
| viva-quicktime | 0 / 0 | 1 / 108 |
| viva-protector | 0 / 0 | 8 / 2.531 |
| crt-enlazar | 5 / 729 | 5 / 729 |
| d3dx9-enlazar | 2 / 1.188 | 2 / 1.188 |
| pendiente-dato | 4 / 2.118 | 0 / 0 |
| citada-en-cpp | 167 / 20.389 | 0 / 0 |
| cerrada-comentario | 0 / 0 | 83 / 2.284 |
| muerta-por-reemplazo | 94 / 27.381 | 94 / 27.381 |
| destructor-atexit | 134 / 1.657 | 134 / 1.657 |
| sin-referencia-real | 211 / 9.944 | 211 / 9.944 |
| **Total** | 914 / 118.544 | 914 / 118.544 |

Las **147 piezas `citada-en-cpp`/`muerto` (19.342 B)** del encargo se reparten así:

| Nuevo destino | Piezas | Bytes |
| --- | ---: | ---: |
| cerrada-comentario | 64 | 1.581 |
| viva-enlazada | 54 | 7.917 |
| viva-sin-hook | 20 | 7.205 |
| viva-protector | 8 | 2.531 |
| viva-quicktime | 1 | 108 |

Las 94 piezas `muerta-por-reemplazo` no cambian (muerta-por-reemplazo 94): ninguna tiene cita de código ni inmediato compilado, solo llamadores que son funciones reemplazadas (su cuerpo ya no corre).

Huecos cerrados (`cierre-p6.csv`): antes 269 de 270 → después **259 de 270**. Un hueco con una pieza «viva sin hook» no está cerrado: su pieza está en la cola.

## 4. Las 379 direcciones de código citadas por el C++ sin gancho (fase 8)

Antes: 119 de confianza A, 147 B y 113 «ruido» C. Después, cada una cae en una clase (la columna «Direcciones» suma 379); «Objetos» agrupa las que comparten pieza de hueco:

| Clase | Direcciones | Objetos | Bytes |
| --- | ---: | ---: | ---: |
| viva sin hook | 50 | 47 | 23.483 |
| viva, enlazada (libjpeg) | 58 | 58 | 11.371 |
| viva, enlazada (libpng/zlib) | 104 | 104 | 9.670 |
| viva, enlazada (CRT) | 21 | 21 | 1.813 |
| viva, QuickTime (se retira) | 1 | 1 | 108 |
| viva, del protector | 0 | 8 | 2.531 |
| cerrada (isla D3DX9) | 33 | 33 | 0 |
| cerrada (comentario) | 0 | 83 | 2.284 |
| cerrada (constante) | 107 | 107 | 0 |
| enganchada desde la medida | 5 | 5 | 73 |
| **Total** | 379 | 467 | 51.333 |

«Enganchada desde la medida» = ya tiene fila en `docs/hooks.csv` (la medida es anterior a integrar `vivo-crtf8`). Los «ruido C» que eran reales: `0x004CAE40` (entradas de la tabla estándar de rasters); el resto son constantes (`imul`, `add`, `cmp`, desplazamientos de tabla, un identificador de temporizador `0x475441`, máscaras `0x7FFFFF`/`0x800000`, colores).

## 5. Cola de reconstrucción: «viva sin hook»

**47 objetos, 23.483 B.** Completa en [`cola-raices-valor-p6.csv`](cola-raices-valor-p6.csv) (todas las clases en [`raices-valor-p6.csv`](raices-valor-p6.csv)). Orden: primero lo **medido** en la fase 8 (se ejecuta en los dos primeros segundos), luego lo que comparte rutina de registro con una medida (mismo arranque), luego el resto; dentro de cada nivel por tamaño descendente. Los tamaños marcados `*` son una estimación (barrido lineal hasta el primer `ret`/`jmp` sin saltos pendientes; no sigue tablas de saltos). `RECONSTRUIR` = escribir C++ y engancharlo.

| # | Dirección | B | Ejecución | Quién la cita | Tabla / estructura destino | Decisión |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | `0x004CD820` | 1.460 | medida (fase 8) | rwcore.cpp:6878 | rwcore.cpp:6878 `{ 0x0B, 0x4CB520 }, { 0x0C, 0x4CBD50 }, { 0x19, Fn(&D3D9RasterFormat::GetRasterMemorySize)` | RECONSTRUIR |
| 2 | `0x00750460` | 682 | medida (fase 8) | rpworld.cpp:8307; rpworld.cpp:8309 | rpworld.cpp:8309 `*reinterpret_cast<RwUInt32*>(camera + 0x1C) = 0x750460;` | RECONSTRUIR |
| 3 | `0x007F99B0`* | 621 | medida (fase 8) | rwcore.cpp:6875 | rwcore.cpp:6875 `{ 0x01, Fn(&RenderWare::_rwD3D9CameraBeginUpdate) }, { 0x0A, 0x7F98D0 }, { 0x15, Fn(&Rende` | RECONSTRUIR |
| 4 | `0x00750AC0` | 513 | medida (fase 8) | rpworld.cpp:8391 | rpworld.cpp:8391 `*reinterpret_cast<RwUInt32*>(light + 0x10) = 0x750AC0;` | RECONSTRUIR |
| 5 | `0x007F98D0`* | 223 | medida (fase 8) | rwcore.cpp:6875 | rwcore.cpp:6875 `{ 0x01, Fn(&RenderWare::_rwD3D9CameraBeginUpdate) }, { 0x0A, 0x7F98D0 }, { 0x15, Fn(&Rende` | RECONSTRUIR |
| 6 | `0x00750430` | 42 | medida (fase 8) | rpworld.cpp:8308 | rpworld.cpp:8308 `*reinterpret_cast<RwUInt32*>(camera + 0x18) = 0x750430;` | RECONSTRUIR |
| 7 | `0x004CD4D0` | 479 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6878 | rwcore.cpp:6878 `{ 0x0B, 0x4CB520 }, { 0x0C, 0x4CBD50 }, { 0x19, Fn(&D3D9RasterFormat::GetRasterMemorySize)` | RECONSTRUIR |
| 8 | `0x007FFF00` | 208 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6876 | rwcore.cpp:6876 `{ 0x06, Fn(&_rwD3D9RasterToImage) }, { 0x05, Fn(&RwD3D9RasterDriver::RasterDestroyCpp) }, ` | RECONSTRUIR |
| 9 | `0x004CBCB0`* | 131 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6878 | rwcore.cpp:6878 `{ 0x0B, 0x4CB520 }, { 0x0C, 0x4CBD50 }, { 0x19, Fn(&D3D9RasterFormat::GetRasterMemorySize)` | RECONSTRUIR |
| 10 | `0x004CA4E0`* | 88 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6877 | rwcore.cpp:6877 `{ 0x17, 0x4CA4E0 }, { 0x18, Fn(&D3D9RasterPaletteUnlockExe) }, { 0x0E, 0x4CB4E0 }, { 0x0D,` | RECONSTRUIR |
| 11 | `0x004CB4E0`* | 55 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6877 | rwcore.cpp:6877 `{ 0x17, 0x4CA4E0 }, { 0x18, Fn(&D3D9RasterPaletteUnlockExe) }, { 0x0E, 0x4CB4E0 }, { 0x0D,` | RECONSTRUIR |
| 12 | `0x004CBD50`* | 45 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6878 | rwcore.cpp:6878 `{ 0x0B, 0x4CB520 }, { 0x0C, 0x4CBD50 }, { 0x19, Fn(&D3D9RasterFormat::GetRasterMemorySize)` | RECONSTRUIR |
| 13 | `0x004CAE80`* | 24 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6877 | rwcore.cpp:6877 `{ 0x17, 0x4CA4E0 }, { 0x18, Fn(&D3D9RasterPaletteUnlockExe) }, { 0x0E, 0x4CB4E0 }, { 0x0D,` | RECONSTRUIR |
| 14 | `0x004CAE40`* | 21 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6877 | rwcore.cpp:6877 `{ 0x17, 0x4CA4E0 }, { 0x18, Fn(&D3D9RasterPaletteUnlockExe) }, { 0x0E, 0x4CB4E0 }, { 0x0D,` | RECONSTRUIR |
| 15 | `0x004CAE60`* | 21 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6877 | rwcore.cpp:6877 `{ 0x17, 0x4CA4E0 }, { 0x18, Fn(&D3D9RasterPaletteUnlockExe) }, { 0x0E, 0x4CB4E0 }, { 0x0D,` | RECONSTRUIR |
| 16 | `0x004CB4C0`* | 20 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6877 | rwcore.cpp:6877 `{ 0x17, 0x4CA4E0 }, { 0x18, Fn(&D3D9RasterPaletteUnlockExe) }, { 0x0E, 0x4CB4E0 }, { 0x0D,` | RECONSTRUIR |
| 17 | `0x004CBD40`* | 16 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6876 | rwcore.cpp:6876 `{ 0x06, Fn(&_rwD3D9RasterToImage) }, { 0x05, Fn(&RwD3D9RasterDriver::RasterDestroyCpp) }, ` | RECONSTRUIR |
| 18 | `0x004CB520`* | 15 | misma rutina de registro que una medida (arranque) | rwcore.cpp:6878 | rwcore.cpp:6878 `{ 0x0B, 0x4CB520 }, { 0x0C, 0x4CBD50 }, { 0x19, Fn(&D3D9RasterFormat::GetRasterMemorySize)` | RECONSTRUIR |
| 19 | `0x0075F0B0`* | 7.153 | sin medir | rpworld.cpp:9601 | rpworld.cpp:9601 `void* const shader = reinterpret_cast<CreateVertexShader>(0x75F0B0)(desc, dispatch);` | RECONSTRUIR |
| 20 | `0x0075A960`* | 2.933 | sin medir | rpworld_s30a31a.cpp:1417; rpworld_s30a31a.cpp:533 | rpworld_s30a31a.cpp:533 `return reinterpret_cast<RpMeshHeader*(__cdecl*)(RpBuildMesh*, void*, RwInt32)>(0x75A960)(b` | RECONSTRUIR |
| 21 | `0x007624A0` | 1.205 | sin medir | rpworld_s30a31a.cpp:1759; rpworld_s30a31a.cpp:1763 | rpworld_s30a31a.cpp:1759 `if (!reinterpret_cast<SectorWriteFn>(0x7624A0)(world->rootSector, stream, mutableWorld, fl` | RECONSTRUIR |
| 22 | `0x00756960`* | 1.044 | sin medir | rpworld_p6b.cpp:22 | rpworld_p6b.cpp:22 `return reinterpret_cast<ExeInstanceFn>(0x756960)(object, owner, type, ownerRef, meshHeader` | RECONSTRUIR |
| 23 | `0x00762F30` | 980 | sin medir | rpworld_s30a31a.cpp:1876 | rpworld_s30a31a.cpp:1876 `world->rootSector = reinterpret_cast<SectorReadFn>(0x762F30)(stream, &memoryCursor, world,` | RECONSTRUIR |
| 24 | `0x007F40F0` | 966 | sin medir | rwcore.cpp:13847 | rwcore.cpp:13847 `S20ModuleField<void*>(0xC97B4C, 0x14) = reinterpret_cast<void*>(0x7F40F0);` | RECONSTRUIR |
| 25 | `0x0074A770` | 669 | sin medir | rpworld_s30a31a.cpp:1025; rpworld_s30a31a.cpp:1132; rpworld_s30a31a.cp | rpworld_s30a31a.cpp:884 `inline RwBool BuildGeometryList(RpGeometryList* list, RpClump* clump) { return reinterpret` | RECONSTRUIR |
| 26 | `0x00763310` | 663 | sin medir | rpworld_s30a31a.cpp:1885 | rpworld_s30a31a.cpp:1885 `world->rootSector = reinterpret_cast<SectorReadFn>(0x763310)(stream, &memoryCursor, world,` | RECONSTRUIR |
| 27 | `0x007F5140` | 652 | sin medir | rwcore.cpp:13848 | rwcore.cpp:13848 `S20ModuleField<void*>(0xC97B4C, 0x2C) = reinterpret_cast<void*>(0x7F5140);` | RECONSTRUIR |
| 28 | `0x007507E0` | 453 | sin medir | rpworld.cpp:8346 | rpworld.cpp:8346 `*reinterpret_cast<RwUInt32*>(atomic + 0x10) = 0x7507E0;` | RECONSTRUIR |
| 29 | `0x007F53D0` | 292 | sin medir | rwcore.cpp:13846 | rwcore.cpp:13846 `S20ModuleField<void*>(0xC97B4C, 0x18) = reinterpret_cast<void*>(0x7F53D0);` | RECONSTRUIR |
| 30 | `0x00762040` | 257 | sin medir | rpworld.cpp:9742; rpworld.cpp:9749; rpworld_s30a31a.cpp:1671 | rpworld.cpp:9742 `children = reinterpret_cast<SubtreeSize>(0x762040)(plane->leftSubTree, world, flags) + 0x3` | RECONSTRUIR |
| 31 | `0x007F2930`* | 234 | sin medir | rwplcore.cpp:1466 | rwplcore.cpp:1466 `reinterpret_cast<void(__cdecl*)(const RwMatrix*, RwV3d*)>(0x7F2930)(matrix, unitAxis);` | RECONSTRUIR |
| 32 | `0x00751500` | 155 | sin medir | rpworld.cpp:8681 | rpworld.cpp:8681 `RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(static_cast<uintptr_t>(0x75` | RECONSTRUIR |
| 33 | `0x0075C290`* | 155 | sin medir | rpworld.cpp:8536 | rpworld.cpp:8536 `return reinterpret_cast<RpTriStripEdge*(__cdecl*)(RpTriStripEdge*, RpTriStripPolygon*, RwU` | RECONSTRUIR |
| 34 | `0x007515A0` | 132 | sin medir | rpworld.cpp:8682 | rpworld.cpp:8682 `RpClumpForAllLights(clump, reinterpret_cast<RpLightCallBack>(static_cast<uintptr_t>(0x7515` | RECONSTRUIR |
| 35 | `0x0074AEA0` | 119 | sin medir | rpworld_s30a31a.cpp:1142 | rpworld_s30a31a.cpp:1142 `if (!reinterpret_cast<ClumpLightWriteFn>(0x74AEA0)(ClumpListObject<RpLight>(node, 0xC924DC` | RECONSTRUIR |
| 36 | `0x0074AF20` | 119 | sin medir | rpworld_s30a31a.cpp:1152 | rpworld_s30a31a.cpp:1152 `if (!reinterpret_cast<ClumpCameraWriteFn>(0x74AF20)(ClumpListObject<RwCamera>(node, 0xC924` | RECONSTRUIR |
| 37 | `0x0074F590` | 118 | sin medir | rpworld_s30a31a.cpp:128 | rpworld_s30a31a.cpp:128 `RwCameraForAllSectorsInFrustum(static_cast<RwCamera*>(Globals()->curCamera), reinterpret_c` | RECONSTRUIR |
| 38 | `0x0074CE60`* | 112 | sin medir | rpworld.cpp:1929; rpworld_s30a31a.cpp:1195 | rpworld.cpp:1929 `const RwInt32 structureSize = reinterpret_cast<RwInt32(__cdecl*)(const RpGeometry*)>(0x74C` | RECONSTRUIR |
| 39 | `0x00762440` | 86 | sin medir | rpworld_s30a31a.cpp:1670 | rpworld_s30a31a.cpp:1670 `const SectorSizeFn atomicSectorSize = reinterpret_cast<SectorSizeFn>(0x762440);` | RECONSTRUIR |
| 40 | `0x00508EB0`* | 77 | sin medir | AudioZones.cpp:140 | AudioZones.cpp:140 `plugin::CallMethod<0x508EB0, BugstarReportRaw*, const char*, uint32>(self, data, length);` | RECONSTRUIR |
| 41 | `0x004CBEE0`* | 51 | sin medir | rwcore_dds.cpp:98 | rwcore_dds.cpp:98 `return reinterpret_cast<int32(__cdecl*)(uint32)>(0x4CBEE0)(format) != 0;` | RECONSTRUIR |
| 42 | `0x0074B000` | 45 | sin medir | rpworld_s30a31a.cpp:1047 | rpworld_s30a31a.cpp:1047 `RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(0x74B000), &total);` | RECONSTRUIR |
| 43 | `0x00751630` | 45 | sin medir | rpworld.cpp:8683 | rpworld.cpp:8683 `RpClumpForAllCameras(clump, reinterpret_cast<RwCameraCallBack>(static_cast<uintptr_t>(0x75` | RECONSTRUIR |
| 44 | `0x0074AFA0` | 40 | sin medir | rpworld_s30a31a.cpp:1048 | rpworld_s30a31a.cpp:1048 `RpClumpForAllLights(clump, reinterpret_cast<RpLightCallBack>(0x74AFA0), &total);` | RECONSTRUIR |
| 45 | `0x0074AFD0` | 40 | sin medir | rpworld_s30a31a.cpp:1049 | rpworld_s30a31a.cpp:1049 `RpClumpForAllCameras(clump, reinterpret_cast<RwCameraCallBack>(0x74AFD0), &total);` | RECONSTRUIR |
| 46 | `0x0074AE40` | 14 | sin medir | rpworld_s30a31a.cpp:1072 | rpworld_s30a31a.cpp:1072 `RpClumpForAllLights(clump, reinterpret_cast<RpLightCallBack>(0x74AE40), &lightCount);` | RECONSTRUIR |
| 47 | `0x00856BC0` | 10 | sin medir | SpecialFX.cpp:118 | SpecialFX.cpp:118 `reinterpret_cast<int32(__cdecl*)(void(__cdecl*)())>(0x821D1E)(reinterpret_cast<void(__cdec` | RECONSTRUIR |

Las 7 direcciones medidas (`0x4CD820`, `0x750430`, `0x750460`, `0x750480`, `0x750AC0`, `0x7F98D0`, `0x7F99B0`) están dentro de objetos de la cola (7 de 7): `0x750480` va dentro de la pieza `0x750460` (682 B) y `0x750AC0` es la segunda entrada de la pieza de `LightWorldPluginConstruct`. «Ejecución» solo es una medida en las siete; el resto es una prioridad por vecindad (misma rutina de registro), no una medida.

## 6. Efecto sobre las cifras de lo que falta

* **Cierre por llamadas del código original (fase 8)**: 691 funciones / 309.142 B, 266 raíces. De los 47 objetos de la cola, **45 ya eran raíces de ese cierre** (22.170 B) y **2 no estaban** (`0x4CAE40` (21 B), `0x856BC0` (10 B)): el cierre medido era correcto; lo que estaba mal era la **lista de pendientes** (P6 los daba por «muertos» o «citados»). Con la cola, el cierre sube a 693 funciones / 309.173 B.
* **P6**: de «269 de 270 huecos cerrados» a **11 huecos abiertos** (con piezas «viva sin hook»). Lo enlazado (libjpeg 8.261 B en piezas) y lo del protector (2.531 B) no suma a lo que falta reconstruir.
* **Por reconstruir con hook** (toda la cola, no solo P6): 47 objetos, 23.483 B; **18** se ejecutan al arrancar o comparten rutina de registro con una medida (4.664 B); 8 tienen ≤ 30 B.
* **Cierres nuevos con motivo**: 83 piezas (2.284 B) solo citadas en comentarios (antes `citada-en-cpp`), 107 direcciones del escaneo de `.obj` que son constantes y 33 citas de la isla D3DX9; la clase `citada-en-cpp` ya no existe.
* **Plan**: nueva parte P7 en `plan.csv`/`plan-lotes.md` (lotes R1–R3 = la cola por temas; `contrib/codigo_vivo_plan.py`), y H1–H6 con su estado real (antes `pendiente` sin recalcular).

## 7. Descartes P1/V1–V8 cerrados que dependían de una pieza viva

Se recalculó el alcance por punto fijo de `contrib/codigo_vivo_cierre_v48.py` (modos estricto + amplio, sobre las 4.804 funciones con decisión DESCARTAR) con la cola (47 direcciones) como raíces extra. El alcance base ya tomaba como raíz todo literal de código del C++ (`escanear_fuente` quita los comentarios) y `codigo_vivo_cierre_v48.py` lee ahora además `cola-raices-valor-p6.csv`; por tanto el cierre de V4–V8 **no dependía de la clase «muerto» de P6** y no cambia.

* Nodos alcanzados: base 2728, con la cola 2728.
* **Descartes cerrados que pasan a estar alcanzados con la cola: 0**. No se reabre ninguno.
* Prueba pesimista, si además fueran raíz las piezas «viva, del protector»: 0 descartes más.
* Prueba pesimista, si además fueran raíz las piezas «viva, enlazada (libjpeg)»: 0 descartes más.
* Prueba pesimista, si además fueran raíz las piezas «cerrada (comentario)»: 3 descartes más (`0x00745920` V2-2, `0x00745CC0` V2-2, `0x00745DC0` V2-2).

| Lote | Funciones DESCARTAR alcanzadas por la base / muertas |
| --- | --- |
| V1 | muerta 11, alcanzadas 7 |
| V2-1 | muerta 30, alcanzadas 15 |
| V2-2 | muerta 38, alcanzadas 2 |
| V3-1 | muerta 588, alcanzadas 12 |
| V3-2 | muerta 600 |
| V3-3 | muerta 600 |
| V3-4 | muerta 535, alcanzadas 65 |
| V3-5 | muerta 600 |
| V3-6 | muerta 504 |
| V4-1 | muerta 250 |
| V4-2 | muerta 250 |
| V4-3 | muerta 250 |
| V4-4 | muerta 182 |
| V5-1 | muerta 63, alcanzadas 7 |
| V5-2 | muerta 66, alcanzadas 4 |
| V5-3 | muerta 57, alcanzadas 6 |
| V6-1 | muerta 31 |
| V7-1 | muerta 13, alcanzadas 4 |
| V8-1 | muerta 14 |

Las «alcanzadas» son las que ya figuraban abiertas o condicionadas en `cierre-p1.csv` (llamadores vivos conservados; p. ej. los ayudantes `usercall` de rasters de V7, que **sí** están vivos: `ejecucion-v48.md` §3). Ninguna se reabre por este cambio. Las tablas de `ExeProtectionStub.cpp` (piezas `viva-protector`) alcanzan funciones del protector (V1): es lo ya conocido, se retira con el archivo.

**Las 40 funciones «condicionadas a un hueco o tabla por clasificar (P6)»** de `cierre-p1.csv` (V1, V2, E1) se resuelven por la pieza que las llama (columna `motivo_raices_valor`):

| Resultado | Funciones |
| --- | ---: |
| cerrada | 16 |
| condicionada (enlace de una biblioteca (libjpeg/libpng/D3DX9/CRT)) | 11 |
| condicionada (enlace de libpng/zlib) | 10 |
| condicionada (retirada del protector) | 1 |
| condicionada (enlace de libjpeg) | 1 |
| condicionada (enlace de CRT) | 1 |

## 8. Piezas pequeñas y claras (≤ 30 B) de la cola

No se reconstruyó ninguna en esta rama: exige compilar el worktree (build completo, un solo hilo y bajo el candado compartido) y el humo de ropero + agua, que espera a que no haya otro `gta_sa.exe`. Quedan **listas para el lote R3/R1** (cuerpo ya desensamblado; ninguna toca memoria que no sea la indicada):

| Dirección | B | Equivalente C++ (cdecl) | Dónde se registra |
| --- | ---: | --- | --- |
| `0x004CAE40` | 21 | llama a un ayudante `usercall` aún sin reconstruir (0x4CAD90/0x4CAEA0) o a otro método: no es trivial | rwcore.cpp:6877 |
| `0x004CAE60` | 21 | llama a un ayudante `usercall` aún sin reconstruir (0x4CAD90/0x4CAEA0) o a otro método: no es trivial | rwcore.cpp:6877 |
| `0x004CAE80` | 24 | llama a un ayudante `usercall` aún sin reconstruir (0x4CAD90/0x4CAEA0) o a otro método: no es trivial | rwcore.cpp:6877 |
| `0x004CB4C0` | 20 | llama a un ayudante `usercall` aún sin reconstruir (0x4CAD90/0x4CAEA0) o a otro método: no es trivial | rwcore.cpp:6877 |
| `0x004CB520` | 15 | `int cb(void*, int valor) { *(int*)0xB4E9E4 = valor; return 1; }` (guarda en un global del EXE; manejador estándar de rasters) | rwcore.cpp:6878 |
| `0x004CBD40` | 16 | `int cb(int* out, int valor) { *out = valor; return 1; }` (manejador estándar de rasters) | rwcore.cpp:6876 |
| `0x0074AE40` | 14 | `void* cb(void* obj, int* n) { ++*n; return obj; }` (callback de `RpClumpForAllLights`/`ForAllCameras` que cuenta) | rpworld_s30a31a.cpp:1072 |
| `0x00856BC0` | 10 | `mov ecx, 0xC803E8; jmp 0x59ACD0`: destructor de un global de SpecialFX (`this` = 0xC803E8), registrado con `atexit`; 0x59ACD0 es un método aún por identificar; la pieza (10 B ≥ 5) admite enganche | SpecialFX.cpp:118 |

## 9. Cómo regenerar y límites

```
python -X utf8 contrib/huecos_p6.py --exe <gta_sa.exe>      # huecos-p6.csv (regla de valor), cierre-p6.csv, H1..H6 en plan.csv y plan-lotes.md
python -X utf8 contrib/raices_valor_p6.py --exe <gta_sa.exe> --cierres --aplicar --cache <carpeta>   # cola, raices-valor-p6.csv, este informe y la comprobacion de cierres
python -X utf8 contrib/codigo_vivo_cierre_v48.py --alcance --aplicar --cache hechos.pkl --cache-aristas aristas.pkl   # V4-V8 con la cola como raices
python -X utf8 contrib/codigo_vivo_plan.py                    # plan.csv / plan-lotes.md (lotes R1..R3); ejecutar huecos_p6.py despues para el estado de H1..H6
```

Límites: (1) `dependencias_codigo.csv` es el escaneo de `.obj` de la fase 8 (solo se regenera con las herramientas de esa rama); (2) una cita sin literal hexadecimal (base + desplazamiento, decimal) no se ve; (3) un literal en código no compilado (`#if 0`) cuenta como valor; (4) «cuánto se ejecuta» solo está medido para 7 direcciones, el resto es por rutina de registro compartida (el censo `--censo` de `reversing/FnFase8` exige enlazar y ejecutar el EXE propio y no se repitió aquí); (5) el tamaño de un tramo de hueco es una estimación; (6) los módulos enlazados salen de `docs/RenderWareExe/funciones.csv` (D3DX9 0x7652AE–0x7C4220, libpng+zlib 0x7CF600–0x7EB260), QuickTime 0x818430–0x81DAB0 y el CRT hasta 0x8568AB.

