# DatosExe, ronda B: liberar datos con alcanzabilidad del código escondido, inicializadores estáticos y código en huecos

Rama `datos-exe2` (desde `origin/main` 4a4a7b7328, 21/11/2026). Continúa [README.md](README.md) (rondas DE01–DE05-E5 y DE04-R/DE05-R). Todo lo medido es estático salvo lo indicado en §7 (compilación y humo). Scripts nuevos: `contrib/huecos_alcanzables.py`, `contrib/datos_exe_de05f.py`, `contrib/datos_exe_de04d.py`, `contrib/datos_exe_inicializadores.py`, `contrib/datos_exe_staticinit.py`; cambios en `contrib/datos_exe.py`, `datos_exe_de04.py` y `datos_exe_de05.py` (opción `--lote`, variable `DATOSEXE_HUECOS`).

## 1. El problema de fondo: qué código del EXE puede ejecutarse

La ronda anterior descubrió código del EXE escondido en los huecos del `.text` (zonas que Ghidra no tiene como función): 2.260 huecos «sin referencia directa» (257.550 B) y 161 «código con referencia sin función» (58.602 B) en la región del juego. Por prudencia el escáner de datos (`datos_exe.py`) trataba **todos** esos huecos como código vivo, y cualquier dato que tocaran quedaba fuera de los lotes (la bisección de DE05 había mostrado que un hueco leía `SwitchDefaultAddress`, 0xA43F54).

`contrib/huecos_alcanzables.py` decide cuáles de esos huecos se pueden ejecutar de verdad con el criterio de raíces por valor de [../CodigoVivoExe/reclasificacion-raices-valor.md](../CodigoVivoExe/reclasificacion-raices-valor.md), afinado en seis puntos que eran la causa de casi todo el «código vivo» aparente:

1. **Tablas de saltos de funciones muertas.** Un `jmp [tabla+reg*4]` solo hace vivos los destinos de su tabla si su función es viva. Las tablas de los despachadores de comandos de script, de los `switch` de `CTask*` y demás cuelgan de funciones **reemplazadas por C++** o DESCARTAR: sus destinos (los cuerpos de los `case`, que son los huecos grandes de 15 y 17 KB) están muertos. Si el `jmp` está dentro de otro hueco, la tabla solo cuenta cuando ese hueco es alcanzable (propagación por punto fijo).
2. **Vtables.** Un puntero a un hueco dentro de una serie de punteros a código (vtable o tabla de callbacks) solo es raíz si algún código vivo o el C++ (como valor, no el gancho `RH_Scoped*`) cita la base de esa serie; sin constructor vivo la vtable está muerta.
3. **Texto que parece puntero.** Cadenas ASCII/UTF-16 de `.rdata` cuyas palabras valen 0x4xxxxx–0x74xxxx (270 palabras) no son punteros.
4. **Gancheadas, protector y falsos literales.** Una referencia a una dirección con gancho (`docs/hooks.csv`) no alcanza el cuerpo original; las tablas de `_TEXT_HA` son del protector (descartado); `0x475441` de `SDLWrapper.cpp` es un identificador de temporizador, no una dirección.

5. **Ganchos inactivos.** El primer humo de la ronda (ropero) cerró el juego con una violación de acceso en `0x4D2BA5`: `gpAnimBlendClump` (0xB4EA0C, `CAnimBlendClumpData*`) se había convertido en variable propia (DE05-G4) porque `FrameUpdateCallBackSkinned` (0x4D2B90) figura como *reemplazada*, pero su gancho está registrado con `{.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS}` y esa constante vale 1: **el cuerpo original del EXE se ejecuta** (y los usercall que llama, 0x4D1680, 0x4D1A50, 0x4D1DB0…, que `funciones-vivas.csv` marcaba DESCARTAR/CUBIERTA). `datos_exe.py` ahora devuelve al código vivo toda función con `RH_Scoped*Install(…, {.enabled = <distinto de true>})` y, hasta punto fijo, las funciones DESCARTAR/CUBIERTA que esta llama (20 funciones: 0x4CFC50, 0x4CFD90, 0x4CFE60, 0x4D0000, 0x4D06C0, 0x4D08D0, 0x4D1610, 0x4D1680, 0x4D1A50, 0x4D1DB0, 0x4D2100, 0x4D2450 y el resto del grupo de animaciones). `contrib/datos_exe_retirar.py` aplica el escáner corregido a los 1.832 objetos que define DatosExe: **solo `gpAnimBlendClump` quedaba tocado por código vivo**; se retiró (`lotes/DE05R2.csv`) y vuelve a ser `StaticRef`. Por la misma razón el reanálisis de huecos (`huecos_alcanzables.py`) usa el mismo conjunto de funciones vivas.

6. **Punteros a datos guardados en otras tablas de datos.** El segundo humo (agua) cayó en `CBaseModelInfo::IsSwayInWind1` desde `WaterCreature_c::Init`: `WaterCreatureManager_c::ms_creatureInfo` (en `.data`) guarda **punteros a las ranuras de `ModelIndices`** (`m_pModelId`), y esas ranuras se habían convertido (DE04-D) en variables propias, de modo que el desreferencia leía la copia vieja del EXE. Es el límite que avisaba la sección 1 del README («punteros guardados dentro de otras tablas»). `escanear_exe` ahora recorre las palabras alineadas de `.rdata`/`.data` y cuenta como «dirección tomada» cualquier valor que caiga en el rango de datos (clave `0x100000000 + dirección de la palabra`; se ignoran valores ≥ 0x3F000000). Aplicado a los 1.831 objetos que define DatosExe aparecieron **13**: 7 `ModelIndex` (la tabla de criaturas), 3 objetos de lotes que ya estaban en `origin/main` (`0xB70000` bool de E1, `0xB700B4` uint32 de E2 y el array `0xC3FE08` de E5, cuyo final apunta una palabra de `.data`) y 3 que leen las pruebas de humo (`0x8D37D4`, `0x8D37E0`, `0x8D37E4`). Los 13 se retiraron (`lotes/DE05R2.csv`, junto con `gpAnimBlendClump`); los generadores (`datos_exe_de04/05/04d/05f.py`) usan ya el escáner corregido y no los vuelven a seleccionar.

Resultado sobre los 2.421 huecos del juego (316.152 B), en [huecos-alcanzables.csv](huecos-alcanzables.csv) (uno por hueco, con las referencias que lo alcanzan o el motivo de que no) y [huecos-alcanzables-resumen.csv](huecos-alcanzables-resumen.csv):

| Clase | Huecos | Bytes | Qué es |
| --- | ---: | ---: | --- |
| INALCANZABLE, código sin entrada | 1.864 | 234.259 | decodifica como código pero nada vivo apunta dentro: cuerpos de `case`, restos y funciones del dispatcher reemplazado, copias de plantillas |
| INALCANZABLE, dato | 456 | 48.519 | bytes que no decodifican (tablas de índices, cadenas, relleno con datos) sin raíz |
| ALCANZABLE (todas las clases) | 101 | 33.374 | ver abajo |

De los 101 alcanzables salen 194 entradas concretas ([huecos-alcanzables-entradas.csv](huecos-alcanzables-entradas.csv): dirección, tamaño por flujo de control, instrucciones, saltos fuera/indirectos, llamadas, origen y **destino**):

| Destino | Entradas | Tratamiento |
| --- | ---: | --- |
| vtable de clase C++ (94) y vtable cuyo constructor no se localiza (25, casi todas funciones de 3–17 bytes de las tablas de un hueco de libjpeg: sobrestimadas) | 119 | son las ranuras triviales (`GetEventType`, `GetLifeTime`, `mov al,1; ret`…) de las vtables de ~35 clases `CEvent*` y de unas `CTask*`, escritas por constructores C++ (`EventDeath.cpp:60`, `EventSexyVehicle.cpp:27`…). Las clases C++ ya tienen esos métodos virtuales; solo se ejecuta el código del EXE mientras el objeto lleve la vtable del EXE. **Desaparecen con DE09** (vtable del compilador); no se reconstruyen como funciones sueltas |
| libjpeg (enlazada) | 61 | decisión vigente: libjpeg se enlaza (hueco del `JPegCompressPlugin`, 0x5C1000–0x5CFFFF) |
| RenderWare | 11 | otro agente (0x74xxxx y `rwcore_dds.cpp`: 0x4CBEE0 `DdsCheckCubeTextureFormat` lo cita `rwcore_dds.cpp:98`) |
| llamada C++ por dirección | 1 | `0x508EB0`, **reconstruida en esta ronda** (§5) |
| solo desde otro hueco alcanzable | 2 | cuelgan de los anteriores |

Conclusión: de los 316 KB escondidos, **283 KB no se ejecutan** (descartados con el motivo de la columna `ejemplos`/`ref_*` del CSV) y de los 33 KB alcanzables solo una función clara de la región del juego dependía de una llamada por dirección (se reconstruyó); el resto es libjpeg enlazado, RenderWare (otro agente) y ranuras de vtables que se van con DE09.

Cota y aviso: el análisis es estático y se equivoca hacia «alcanzable». Si alguna vez se diera por muerto algo que corre, el síntoma sería un dato propio que el EXE no ve; la red de seguridad es el humo y la variable `DATOSEXE_HUECOS`: el escáner trata como vivos **todos** los huecos salvo con `DATOSEXE_HUECOS=alcanzables` (los lotes de esta ronda se generaron con esa opción; el medidor `docs/DatosExe/actual/` sigue siendo conservador).

## 2. Lotes aplicados

Todos con el escáner refinado; mismas reglas de seguridad que DE04/DE05 (referencias solo `StaticRef`, un tipo, ningún vecino dentro de la extensión, ninguna función no reemplazada ni hueco alcanzable la toca, sin las direcciones del humo).

| Lote | Contenido | Objetos | Refs | Script |
| --- | --- | ---: | ---: | --- |
| DE05-G1..G5 | `.bss` de los tipos de E1..E5 que solo quedaban fuera por el código «vivo» de huecos muertos (enteros, float, punteros, arrays, CVector…) | 185 | 223 | `datos_exe_de05.py E<n> --lote G<n>` |
| DE04-XA / XB | `.data` float y otros escalares liberados igual (incluye varios de DE04-R) | 27 | 35 | `datos_exe_de04.py <A\|B> --lote X<A\|B>` |
| DE04-D | `.data`: `ModelIndex` (119, `ModelIndices.cpp`), `CVector`, `RwRGBA`, `RwRGBAReal`, `int`, `RwInt32` con los bytes iniciales exactos del EXE | 124 | 124 | `datos_exe_de04d.py` |
| DE05-F | `.bss` de tipos con nombre propio (estructuras, `std::array<T,N>` de clases, `CMatrix`, `CRect`, `D3DMATRIX`, punteros a pools…) como almacenes con constructor | 32 | 35 | `datos_exe_de05f.py` |
| **Total** | | **368** | **417** | |

**DE05-F, diseño.** Cada objeto es un almacén en cero (`Slot<T>`) que `ConstructF()` construye con `std::construct_at` (como el `_initterm` del EXE) y cuyo destructor, si no es trivial, se registra con `ExeCrt::Onexit` (el atexit del CRT del EXE; en el ejecutable independiente será el CRT propio). Se accede con `DatosExe::Bss::b<DIR><T>()` (plantilla de función sin definición en la cabecera, especializada en `DataBssF.cpp`), de modo que la cabecera **no necesita ningún tipo** y se incluye al final de los `#include` del consumidor. `ConstructF()` se ejecuta al cargar la DLL (inicializador estático del propio archivo). Los `static_assert` de tamaño (contra el hueco hasta el siguiente objeto conocido) descartaron 19 de los 51 candidatos iniciales (`_f_iter.py`: genera, compila el único archivo y excluye los que fallan; lista en `lotes/DE05F.excluir.txt`).

El medidor ya no cuenta como dependencia del EXE los archivos de `source/game_sa/DatosExe/` (sus direcciones solo van en comentarios y mensajes de `static_assert`).

### Pendiente de DE05 (cifras sobre el árbol actual)

Quedan objetos de `.bss` sin convertir por motivos que no son de tipo: ~370 con la referencia en una cabecera (una `static inline auto& x = StaticRef<T>(…)` dentro de una clase exige que el tipo esté completo antes del `extern`), ~380 con formas distintas de `StaticRef` (`reinterpret_cast`, ayudantes `LoadWord`, literales en ganchos), ~50 con tipos mezclados y ~16 con código del EXE que sí puede ejecutarse. Los tipos con destructor no trivial ya entran por DE05-F (ver `contrib/datos_exe_de05f.py estadisticas`); el resto es trabajo de reescritura de consumidores archivo a archivo.

## 3. Inicializadores estáticos de `_initterm` ([inicializadores.csv](inicializadores.csv), [inicializadores-estado.csv](inicializadores-estado.csv))

`contrib/datos_exe_inicializadores.py` ejecuta con un emulador mínimo cada inicializador de [../CodigoVivoExe/fase8-inicializadores.csv](../CodigoVivoExe/fase8-inicializadores.csv) que depende de DatosExe y anota sus efectos (llamadas con `ecx` y pila, escrituras con ancho y valor, bucles `dec/jne`, `rep stosd`, lecturas de `.data`). Clasifica los 140 «constructor + atexit», las 35 asignaciones y las 2 constantes x87 raras; los constructores/destructores se cruzan con `docs/hooks.csv` para saber si ya son C++.

`contrib/datos_exe_staticinit.py` genera `source/game_sa/DatosExe/StaticInit.cpp` (y `.h`): `DatosExe::RunStaticInitializers()`, que en el ejecutable independiente construye cada objeto con `std::construct_at` sobre `StaticRef<Clase>(dir)`, registra su destructor con `ExeCrt::Onexit`, construye los vectores con `static_assert(sizeof(T)==tamaño del EXE)` y escribe los valores iniciales. **No se llama con la DLL sobre el EXE** (el `_initterm` del EXE ya corrió); compila siempre, de modo que un cambio de ctor lo detecta el compilador. Las dos constantes x87 que `CrtStaticInit.cpp` no cubría quedan escritas a mano: `cos(π/8)` en `0xC18D48` y las tres restas del arma del Tiger en `0xC1C208..0xC1C210`.

Estado (sobre 175 filas fuera del CRT; las 11 restantes son del CRT y las lleva `StandaloneStartup`):

| Estado | Filas | Detalle |
| --- | ---: | --- |
| generada | 93 | 53 constructores, 20 vectores de objetos, 1 destructor solo, 18 de valores iniciales (21 escrituras no nulas), 1 x87 (2 constantes) |
| nada que hacer | 25 | el funclet de `atexit` es un `ret` (objeto trivial) o solo había ceros sobre `.bss` |
| pendiente | 56 | 28 constructores y 23 constructores de elemento que aún son del EXE (clases sin reconstruir: 0x441E00, 0x4470E0, 0x463750, 0x4C5CB0…), 3 destructores no de clase (`CPtrNodeDoubleLinkPool::Flush`, 0x552470) y 2 especiales; lista con motivo en el CSV |

Observación sin tocar: `Automobile.cpp:90` define `TIGER_GUN_POS(0, 0.5, 0.2)` con el comentario `0xC1C208`, pero el inicializador del EXE calcula `(0, 4.344, 0.674)` en esa dirección; conviene revisarlo.

## 4. Datos que siguen compartidos con código que se ejecuta

Siguen sin poder convertirse (código del EXE no reemplazado ni hueco alcanzable que los lee o escribe, o referencias del C++ en formas que no son `StaticRef`): lo que usan RenderWare/D3D9 y el CRT (otros agentes), y los marcadores del humo (lista `HARNESS` de `datos_exe_de05.py`).

## 5. Código reconstruido

`0x508EB0` (`BugstarWriteBlock`, 77 bytes, `thiscall` con bloque y longitud, `ret 8`): escribe en el informe de «bugstar» la longitud (4 bytes), el bloque y un cero final con la ranura de escritura de la E/S del informe. `AudioZones.cpp` lo llamaba con `plugin::CallMethod<0x508EB0>`; ahora es C++ con gancho (`RH_ScopedNamedGlobalInstall`), de modo que el cuerpo original deja de ejecutarse.

## 6. Cifras antes y después

Medidas con `contrib/datos_exe.py` (mismo escáner conservador; `docs/DatosExe/actual/` es la medición del árbol actual). «Antes» = `origin/main` 4a4a7b7328; los datos incluyen los 4.691 literales de `CrtStaticInitTable.inc` (agente del CRT), idénticos antes y después.

| Medida | Antes | Después | Diferencia |
| --- | ---: | ---: | ---: |
| Referencias de la DLL a datos del EXE | 10.737 | 10.344 | −393 |
| Direcciones distintas | 5.839 | 5.488 | −351 |
| `.data`: objetos con ref. de la DLL | 887 | 744 | −143 |
| `.bss`: objetos con ref. de la DLL | 3.336 | 3.128 | −208 |
| `.rdata`: objetos con ref. de la DLL | 1.444 | 1.444 | 0 |
| Huecos del `.text` del juego que pueden ejecutarse | 2.421 (supuestos vivos) | 101 | −2.320 |

## 7. Verificación

Ver el final del archivo (se completa tras la compilación y el humo).
