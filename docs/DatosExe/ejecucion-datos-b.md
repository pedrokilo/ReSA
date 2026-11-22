# DatosExe, ronda B: alcanzabilidad del código escondido, lotes DE04-D/DE05 y inicializadores estáticos

Rama `datos-exe2` (desde `origin/main` 4a4a7b7328, 21–22/11/2026). Continúa [README.md](README.md) (rondas DE01–DE05-E5 y DE04-R/DE05-R). Scripts nuevos: `contrib/huecos_alcanzables.py`, `datos_exe_de05f.py` (+ `datos_exe_de05f_iter.py`), `datos_exe_de04d.py`, `datos_exe_retirar.py`, `datos_exe_inicializadores.py`, `datos_exe_staticinit.py`; cambios en `datos_exe.py`, `datos_exe_de04.py` y `datos_exe_de05.py` (opción `--lote`, variable `DATOSEXE_HUECOS`, escáner corregido).

## 0. Resumen

| Tarea | Resultado |
| --- | --- |
| Código escondido (2.421 huecos, 316.152 B) | **128 alcanzables (146.735 B)** y **2.293 no (169.417 B)**, con la referencia que lo alcanza o el motivo de que no (`huecos-alcanzables*.csv`). Lo alcanzable es sobre todo el despachador de comandos de script del EXE (9 huecos, ~100 KB, tabla `0x8A6168`), ranuras de vtables (se van con DE09), libjpeg y RenderWare. Reconstruido y enganchado: `0x508EB0` |
| DE05 restante / DE04-D | **306 objetos nuevos** (`.bss` 143 + 32 con constructor; `.data` 15 + 116 `ModelIndex`/estructuras) y 417 referencias reescritas, **78 objetos de `origin/main` retirados** por ser campos de estructuras, tocados por código que se ejecuta o apuntados desde tablas (defectos latentes de las rondas anteriores) |
| Inicializadores de `_initterm` | 175 filas fuera del CRT decodificadas con un emulador; `StaticInit.cpp` construye 74 objetos con su constructor C++ y registra su `atexit`; 25 no hacen nada, 56 pendientes con motivo |
| Medición | `docs/DatosExe/actual/` (§6) |
| Compilación y humo | build completo `EXIT=0`; ropero, agua y marcadores BIEN (§7) |

## 1. Qué código del EXE puede ejecutarse (`contrib/huecos_alcanzables.py`)

El escáner de datos (`datos_exe.py`) trataba como código vivo **todos** los huecos del `.text` (zonas que Ghidra no tiene como función) porque la bisección de DE05 había mostrado que uno leía `SwitchDefaultAddress` (0xA43F54). El criterio de raíces por valor de [../CodigoVivoExe/reclasificacion-raices-valor.md](../CodigoVivoExe/reclasificacion-raices-valor.md) se afinó en siete puntos; los cuatro primeros liberan datos, los otros tres son correcciones que evitaron romper el juego:

1. **Tablas de saltos de funciones muertas.** Un `jmp [tabla+reg*4]` solo hace vivos los destinos de su tabla si su función es viva (reemplazada o DESCARTAR ⇒ la tabla está muerta). Si el `jmp` está dentro de otro hueco, la tabla solo cuenta cuando ese hueco es alcanzable (punto fijo).
2. **Vtables.** Un puntero a un hueco dentro de una serie de punteros a código solo es raíz si código vivo o el C++ (como valor, no el gancho `RH_Scoped*`) cita la base de la serie.
3. **Texto que parece puntero** (270 palabras de cadenas ASCII/UTF-16 de `.rdata` con valor 0x4xxxxx–0x74xxxx) y falsos literales (`SDLWrapper.cpp`: 0x475441 es un identificador de temporizador).
4. **Ganchos y protector.** Una referencia a una dirección con gancho (`docs/hooks.csv`) no alcanza el cuerpo original; las tablas de `_TEXT_HA` son del protector.
5. **Ganchos inactivos** (evitó un cierre del juego). `FrameUpdateCallBackSkinned` (0x4D2B90) figura como *reemplazada*, pero su gancho se registra con `{.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS}` y esa constante vale 1: **el cuerpo original del EXE se ejecuta**, y con él los `usercall` que llama (0x4D1680, 0x4D1A50, 0x4D1DB0…, marcados DESCARTAR/CUBIERTA). `gpAnimBlendClump` (0xB4EA0C), convertido por el primer lote, hizo caer 0x4D2BA5 con ECX = 0. `datos_exe.py` devuelve ahora al código vivo toda función con `{.enabled = <distinto de true>}` y, hasta punto fijo, las DESCARTAR/CUBIERTA que esta llama (20 funciones).
6. **Punteros a datos guardados en otras tablas de datos** (evitó un cierre del juego). `WaterCreatureManager_c::ms_creatureInfo` guarda punteros a las ranuras de `ModelIndices`; convertidas, el desreferencia leía la copia del EXE (`CBaseModelInfo::IsSwayInWind1` con `ECX = 0x01C0FFFF`). `escanear_exe` recorre las palabras alineadas de `.rdata`/`.data` y cuenta como «dirección tomada» cualquier valor del rango de datos (se ignoran los ≥ 0x3F000000: floats o enteros grandes).
7. **Tablas que el C++ lee por valor** (evitó que el humo mostrara la pantalla de carga con el juego en marcha). Las ranuras de `s_OriginalCommandHandlerTable` (`StaticRef<…>(0x8A6168)`) son los despachadores originales de comandos de script que `ProcessOneCommand` llama cuando un comando no tiene manejador propio. El primer análisis las daba por muertas porque la única función del EXE que indexa la tabla (el `ProcessOneCommand` original) está reemplazada; pero **el C++ la lee y salta a esos huecos**. Es la causa real de `SwitchDefaultAddress`: el manejador SWITCH (0x4717AE) vive en el despachador 0x470A8C. Tras la corrección se retiraron 32 objetos más (entre ellos 0xA43F58–0xA43F60, 0xA4448B…, 0xB70152…, de lotes de `origin/main`).

Resultado ([huecos-alcanzables.csv](huecos-alcanzables.csv), un hueco por fila con sus referencias; [huecos-alcanzables-resumen.csv](huecos-alcanzables-resumen.csv)):

| Clase | Huecos | Bytes |
| --- | ---: | ---: |
| INALCANZABLE, código sin entrada (cuerpos de `case` de dispatchers reemplazados, copias de plantillas, restos) | 1.841 | 165.371 |
| INALCANZABLE, dato (índices, cadenas, relleno con datos) | 452 | 4.046 |
| **ALCANZABLE** (interior de un hueco 123, entrada propia 5) | 128 | 146.735 |

De los 128 alcanzables salen 253 entradas ([huecos-alcanzables-entradas.csv](huecos-alcanzables-entradas.csv): dirección, tamaño por flujo de control, instrucciones, saltos, llamadas, vtable y ranura, cita del C++ y **destino**):

| Destino | Entradas | Tratamiento |
| --- | ---: | --- |
| despachador de comandos de script del EXE (huecos 0x465E56, 0x470A8C, 0x4748F5, 0x477F89, 0x479D6E, 0x47E08E, 0x483BCC, 0x48EA95, 0x493FD2) | 30 (~115 KB) | **sigue ejecutándose**: son los `CRunningScript::ProcessCommands<N>To<N+99>` sin reescribir. Reconstruirlos es el lote de comandos de script (cada uno ~10–17 KB, un `switch` de 100 comandos); no se hizo en esta ronda por tamaño |
| vtable de clase C++ (101) y vtable sin constructor localizado (43; funciones de 3–17 bytes de las tablas de un hueco de libjpeg, sobrestimadas) | 144 | ranuras triviales (`GetEventType`, `GetLifeTime`, `mov al,1; ret`…) de las vtables de ~35 clases `CEvent*` y de unas `CTask*`, escritas por constructores C++ (`EventDeath.cpp:60`, `EventSexyVehicle.cpp:27`…). Esas clases ya tienen los métodos virtuales en C++; solo se ejecuta el código del EXE mientras el objeto lleve la vtable del EXE. **Desaparecen con DE09** |
| libjpeg (enlazada) | 61 | decisión vigente (hueco del `JPegCompressPlugin`, 0x5C1000–0x5CFFFF) |
| RenderWare | 11 | otro agente (0x74xxxx y `rwcore_dds.cpp:98` → 0x4CBEE0 `DdsCheckCubeTextureFormat`) |
| solo desde otro hueco alcanzable | 7 | cuelgan de los anteriores |
| llamada C++ por dirección | 0 | `0x508EB0` **reconstruida** (§5) |

Cota y aviso: el análisis es estático y se equivoca hacia «alcanzable»; si diera por muerto algo que corre, el síntoma sería un dato propio que el EXE no ve. La red de seguridad es el humo y la variable `DATOSEXE_HUECOS`: sin ella el escáner trata todos los huecos como vivos (conservador, lo que usa `docs/DatosExe/actual/`); los lotes de esta ronda se generaron con `DATOSEXE_HUECOS=alcanzables`.

## 2. Lotes aplicados

Reglas de seguridad de DE04/DE05 más las correcciones 5–7 y una nueva: un objeto cuya dirección cae **dentro de la extensión de otro referenciado por el C++** (campos de `TheCamera`, 0xB6F028, 0xD78 bytes; tamaños por `VALIDATE_SIZE` en `datos_exe.tam_clase`) no es independiente. Objetos definidos al final: 1.740 (`origin/main` 1.512).

| Lote | Contenido | Objetos | Script |
| --- | --- | ---: | --- |
| DE05-G1..G5 | `.bss` de los tipos de E1..E5 que solo quedaban fuera por el «código vivo» de huecos muertos | 143 | `datos_exe_de05.py E<n> --lote G<n>` |
| DE04-XA / XB | `.data` float y otros escalares liberados igual | 15 | `datos_exe_de04.py <A\|B> --lote X<A\|B>` |
| DE04-D | `.data`: `ModelIndex` (112, `ModelIndices.cpp`) y estructuras (`CVector`, `RwRGBA`, `RwRGBAReal`…), con los bytes iniciales exactos del EXE | 116 | `datos_exe_de04d.py` |
| DE05-F | `.bss` de tipos con nombre propio (estructuras, `std::array` de clases, `CMatrix`, `CRect`, `D3DMATRIX`, punteros a pools…) como almacenes con constructor | 32 | `datos_exe_de05f.py` |
| **Total nuevos** | | **306** | |
| Retirados de `origin/main` | campos de estructura mayor, apuntados desde tablas o tocados por código que se ejecuta | **78** | `datos_exe_retirar.py` |

**DE05-F, diseño.** Cada objeto es un almacén en cero (`Slot<T>`) que `ConstructF()` construye con `std::construct_at` (como el `_initterm`) y cuyo destructor, si no es trivial, se registra con `ExeCrt::Onexit` (el atexit del CRT del EXE). Se accede con `DatosExe::Bss::b<DIR><T>()`: plantilla de función **sin definición en la cabecera** (se especializa en `DataBssF.cpp`), de modo que la cabecera no necesita ningún tipo y se incluye al final de los `#include` del consumidor. `ConstructF()` corre al cargar la DLL. Los `static_assert` de tamaño (contra el hueco hasta el siguiente objeto conocido) descartaron 19 de 51 candidatos (`datos_exe_de05f_iter.py`: genera, compila el único archivo con MSBuild y excluye los que fallan; lista en `lotes/DE05F.excluir.txt`). Los consumidores en cabeceras (≈370 objetos de `.bss`) no se convierten: el `extern` necesita el tipo ya declarado.

Los generadores ya no redefinen un objeto que otro lote definió (solo lo declaran y reapuntan los consumidores que se habían quedado con `StaticRef`: había 16, lo que dejaba dos copias del mismo dato). `datos_exe_retirar.py` aplica el escáner corregido a todo lo definido y retira lo que no debe estar; la lista de la última pasada queda en `lotes/DE05R2.csv`.

### Pendiente de DE05 y DE04-D

Quedan por motivos que no son de tipo: ~370 objetos de `.bss` con la referencia en una **cabecera**, ~380 con formas distintas de `StaticRef` (`reinterpret_cast`, ayudantes `LoadWord`, literales en ganchos), ~50 con tipos mezclados y los que lee código que se ejecuta (despachadores de script, vtables). Con el despachador de comandos reescrito (§1) se liberarían las decenas de datos que hoy bloquean; es el siguiente paso de mayor rendimiento.

## 3. Inicializadores estáticos de `_initterm` ([inicializadores.csv](inicializadores.csv), [inicializadores-estado.csv](inicializadores-estado.csv))

`contrib/datos_exe_inicializadores.py` ejecuta con un emulador mínimo cada inicializador de [../CodigoVivoExe/fase8-inicializadores.csv](../CodigoVivoExe/fase8-inicializadores.csv) y anota sus efectos (llamadas con `ecx` y pila, escrituras con ancho y valor, bucles `dec/jne`, `rep stosd`, lecturas de `.data`); [inicializadores-escrituras.csv](inicializadores-escrituras.csv) trae las escrituras comprimidas. Los constructores y destructores se cruzan con `docs/hooks.csv` para saber si ya son C++.

`contrib/datos_exe_staticinit.py` genera `source/game_sa/DatosExe/StaticInit.cpp/.h`: `DatosExe::RunStaticInitializers()`, que en el ejecutable independiente construye cada objeto con `std::construct_at` sobre `StaticRef<Clase>(dir)`, registra su destructor con `ExeCrt::Onexit`, construye los vectores con `static_assert(sizeof(T) == tamaño del EXE)` y escribe los valores iniciales. **No se llama con la DLL sobre el EXE** (el `_initterm` del EXE ya corrió); compila siempre, así que un cambio de constructor lo detecta el compilador. Las dos constantes x87 que `CrtStaticInit.cpp` no cubría quedan escritas: `cos(π/8)` en `0xC18D48` y las tres restas del arma del Tiger en `0xC1C208..0xC1C210`. Falta que `StandaloneStartup` (`CrtStartup.cpp`, del agente del CRT) la llame tras `RunX87StaticInitializers()`.

| Estado (185 filas; 11 son del CRT y las lleva `StandaloneStartup`) | Filas | Detalle |
| --- | ---: | --- |
| generada | 93 | 53 constructores, 20 vectores de objetos, 1 destructor solo, 18 de valores iniciales (21 escrituras no nulas), 1 x87 (2 constantes) |
| nada que hacer | 25 | el funclet de `atexit` es un `ret` (objeto trivial) o solo había ceros sobre `.bss` |
| pendiente | 56 | 28 constructores y 23 constructores de elemento que aún son del EXE (0x441E00, 0x4470E0, 0x463750, 0x4C5CB0…), 3 destructores no de clase (`CPtrNodeDoubleLinkPool::Flush`, 0x552470) y 2 especiales |

Sin tocar: `Automobile.cpp:90` define `TIGER_GUN_POS(0, 0.5, 0.2)` con el comentario `0xC1C208`, pero el inicializador del EXE calcula `(0, 4.344, 0.674)` en esa dirección; conviene revisarlo.

## 4. Datos que siguen compartidos con código que se ejecuta

Siguen sin poder convertirse: lo que lee el despachador de comandos de script del EXE (§1), los `usercall` de animación revividos, RenderWare/D3D9 y el CRT (otros agentes), los objetos apuntados desde tablas de datos y los que leen las pruebas de humo (`HARNESS` de `datos_exe_de05.py`, incluido `C3dMarkers::m_aMarkerArray`).

## 5. Código reconstruido

`0x508EB0` (`BugstarWriteBlock`, 77 bytes, `thiscall` con bloque y longitud, `ret 8`): escribe en el informe de «bugstar» la longitud (4 bytes), el bloque y un cero final con la ranura de escritura de la E/S del informe. `AudioZones.cpp` lo llamaba con `plugin::CallMethod<0x508EB0>`; ahora es C++ con gancho (`RH_ScopedNamedGlobalInstall`, fila nueva en `docs/hooks.csv` y `docs/direcciones.csv`), de modo que el cuerpo original deja de ejecutarse. No hay ninguna otra función clara de la región del juego que el C++ llame por dirección desde un hueco.

## 6. Cifras antes y después

`contrib/datos_exe.py` (escáner conservador, solo código; `docs/DatosExe/actual/` es la medición del árbol actual). «Antes» = `origin/main` 4a4a7b7328 medido con el mismo script. Las 4.691 referencias de `CrtStaticInitTable.inc` (agente del CRT) están en ambas columnas. Los bytes no son comparables (ahora `datos_exe.tam_clase` da el tamaño de las clases).

| Medida | Antes | Después | Diferencia |
| --- | ---: | ---: | ---: |
| Referencias de la DLL a datos del EXE | 10.737 | 10.513 | −224 |
| Direcciones distintas | 5.839 | 5.606 | −233 |
| `.data`: objetos con ref. de la DLL | 887 | 762 | −125 |
| `.bss`: objetos con ref. de la DLL | 3.336 | 3.228 | −108 |
| `.rdata`: objetos con ref. de la DLL | 1.444 | 1.444 | 0 |
| Objetos definidos en DatosExe | 1.512 | 1.740 | +228 (+306 nuevos, −78 retirados) |
| Huecos del `.text` del juego que pueden ejecutarse | 2.421 (todos supuestos vivos) | 128 | −2.293 |
| Funciones vivas (trozos) | 3.708 | 3.724 | +16 (ganchos inactivos devueltos al código vivo) |

## 7. Verificación

* Compilación completa (`_build_DATOS2.sh`, Debug x86, candado compartido): `EXIT=0` en la versión final; once compilaciones durante la ronda.
* Humo con la DLL final (`prueba_ropero.py`, `prueba_agua.py`, `prueba_marcadores.py`, `cal_mal.args`): **ropero BIEN** (puerta y marcador; la captura muestra la escena del ropero), **agua BIEN** (`UnderWaterness` 1,0, brillo medio 49,5 frente a 50,0 con la DLL de `integracion-fase7b`), **marcadores BIEN** (3 marcadores que cambian en 1 s).
* El humo detectó **tres fallos reales** durante la ronda, todos por datos que el EXE leía y el análisis daba por libres (puntos 5, 6 y 7 de §1) y uno más por campos de `TheCamera`; el último (pantalla de carga fija con el juego en marcha) se aisló por bisección de lotes (G → G2 → mitad de G2) y era el despachador de comandos de script.
* Un criterio de humo más fiable que el veredicto es mirar la captura (`ropero_*.png`): con la DLL rota el veredicto seguía siendo BIEN porque la prueba lee la memoria del juego, no los píxeles.
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 `__asm`.
* No se conservaron las pruebas por banco de cada lote (decisión del propietario).
