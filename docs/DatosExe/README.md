# Datos del EXE: medición y plan de sustitución

Rama `datos-exe` (desde `origin/main` 4d39c9866b, 21/11/2026). Objetivo del proyecto: un ejecutable de GTA San Andreas que no cargue el `gta-sa.exe` original. El código del juego ya está en C++; lo que falta son los **datos** del EXE (`.rdata`, `.data`, `.bss`, IAT) que el C++ lee o escribe por dirección fija (`StaticRef<T>(0x…)`, `*(T*)0x…`, tablas, vtables) y los que todavía usa el código vivo del propio EXE.

Todo lo de esta carpeta sale de `contrib/datos_exe.py` (medición, estática, no ejecuta el juego), `contrib/datos_exe_plan.py` (plan) y `contrib/datos_exe_lote.py` (aplica un lote). El EXE medido es el canónico (SHA256 `72ae59e4…ecc9b`, `D:\Grand Theft Auto San Andreas\gta-sa.exe`).

Archivos: [plan-lotes.md](plan-lotes.md) · [plan-lotes.csv](plan-lotes.csv) · [plan-objetos.csv](plan-objetos.csv) · [objetos.csv](objetos.csv) (un objeto por dirección) · [referencias-dll.csv](referencias-dll.csv) (cada referencia del C++) · [por-seccion.csv](por-seccion.csv) · [por-seccion-origen.csv](por-seccion-origen.csv) · [por-clase.csv](por-clase.csv) · [resumen.json](resumen.json) · [lotes/](lotes/) (lo ya aplicado).

## 1. Método

1. **Lado DLL.** Se recorre `source/` (sin comentarios ni literales de texto) y cada literal hexadecimal de 6 a 8 cifras entre `0x858000` y `0xCB0FFF` es una referencia a un dato del EXE. Se clasifica por forma (`StaticRef`, `cast`, `reinterpret_cast`, ayudante tipo `f(0x…)`/`LoadWord`, vtable de `RH_Scoped…`, parche, literal) y por tipo declarado. El tamaño del objeto sale del tipo (`tam_origen=tipo`), del ancho de acceso del EXE (`acceso`) o, si no hay ninguno, del hueco hasta el siguiente objeto con tope de 64 bytes (`hueco`, cota).
2. **Lado EXE.** Con capstone se desensambla cada una de las 19.169 funciones de `docs/MemoryMap/binary_function_ranges.csv` y se recogen operandos de memoria absolutos (con o sin índice) e inmediatos (`push offset`, `mov r, offset`, `lea`) que caen en esas secciones. Una función es **viva** si no está reemplazada por C++ (`reemplazada` en los `funciones.csv` del juego y de RenderWare); hay 5.569 vivas (RenderWare no reconstruible, enlazadas, CRT y libpng/zlib/D3DX9).
3. **Naturaleza y origen.** Naturaleza: `d` constante (.rdata), `a` inicializado (.data), `c` estado (.bss). Origen: `juego`, `renderware` (RW y D3D), `crt` (CRT, x87, bibliotecas) por archivo de la DLL o, si solo lo lee el EXE, por la región de la función que lo lee.

Cotas y límites (importantes): no se ven los accesos con registro base (`[esi+0x…]`), ni los punteros guardados dentro de otras tablas de datos, ni los accesos dinámicos de CLEO/mods. Los tamaños `hueco` subestiman las tablas grandes (hay 330 objetos con referencia de la DLL y hueco de más de 64 bytes, unos 1,2 MB como máximo, que necesitan su tipo). La medición no certifica independencia; indica por dónde empezar.

## 2. Cifras medidas (antes de cualquier lote)

### 2.1 Referencias de la DLL

**8.767 referencias** a **5.276 direcciones** distintas del EXE en 826 archivos propietarios (clases). Por forma: `StaticRef` 5.071, `literal` 2.172, `ayudante` 625, `hook_vtable` 469, `reinterpret_cast` 381, `cast` 46, `parche` 3. Por origen de los objetos con referencia: juego 4.492, RenderWare/D3D 619, CRT/x87/bibliotecas 165.

| Sección | VA | Tamaño | Objetos con ref. DLL | Bytes (cota) | Bytes de tamaño fiable | Objetos solo EXE | Compartidos con código vivo |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| .rdata (d, constantes) | 0x858000 | 311.296 | 989 | 25.438 | 1.886 | 5.085 | 82 |
| .data inicializado (a) | 0x8A4000 | 262.144 | 1.287 | 16.575 | 11.330 | 1.390 | 27 |
| .bss (c, estado) | 0x8E4000 | 3.907.584 | 2.830 | 69.938 | 53.622 | 2.936 | 74 |
| _TEXT_HA | 0xC9E000 | 69.632 | 170 | 797 | 669 | 30 | 19 |
| _rwdseg / .rsrc | 0xCAF000 | 8.192 | 0 | 0 | 0 | 2 | 0 |

(El `.data` ocupa 4.169.728 bytes en memoria pero solo 262.144 están inicializados en el archivo; el resto es `.bss`.) La IAT está al inicio de `.rdata` (0x858000, 888 bytes) con el directorio de importación en 0x8A2004.

Cruce sección × origen (objetos con referencia de la DLL / bytes cota): `.rdata` juego 883/23.937, RW 36/465, CRT 70/1.036; `.data` juego 1.071/18.288, RW 157/1.329, CRT 59/729; `.bss` juego 2.368/71.542, RW 426/2.507, CRT 36/145 (ver [por-seccion-origen.csv](por-seccion-origen.csv)).

Clases con más referencias ([por-clase.csv](por-clase.csv)): Cam 921, rwcore 687, Hud 438, Camera 298, ExeProtectionStub 259, rpworld 236, X87Math 220, Vehicle 202, Bike 201, Plane 181, PostEffects 171, ModelIndices 164.

### 2.2 Datos que aún usa el código vivo del EXE

1.896 objetos (36.239 bytes cota) los leen funciones vivas: sobre todo RenderWare/D3D (`.rdata` 714 objetos y 19.595 bytes, `.data` 357 y 7.101) y CRT/bibliotecas (`.rdata` 270, `.data` 125, `.bss` 139). El **código del juego casi no comparte datos con código vivo**: solo 20 objetos con referencia de la DLL (202 bytes) de origen juego los leen también funciones vivas, y 224 objetos los escribe código vivo. Es decir, la inmensa mayoría de los datos del juego (G4, G5) ya solo los toca C++.

Objetos sin referencia de la DLL: 4.080 objetos de `.rdata` (41.959 bytes cota), 919 de `.data` y 2.750 de `.bss` solo los leen funciones ya reemplazadas por C++; son candidatos a huérfanos (cadenas y constantes que el C++ ya sustituyó por literales propios). No se pueden dar por sobrantes sin comprobar accesos con registro base.

### 2.3 Composición byte a byte (cota)

`.rdata` (311.296 bytes): IAT 888; objetos con ref. de la DLL 25.370; objetos solo del EXE 65.191; cadenas ASCII sin otra clasificación 35.387; relleno de ceros 42.211; sin clasificar 142.249 (tablas indexadas, vtables y RTTI, constantes `double`/SSE sueltas). `.data` inicializado (262.144): objetos con ref. de la DLL 16.562; solo EXE 17.601; cadenas 18.686; ceros 23.716; sin clasificar 185.579 (tablas de animación, nombres de modelos, handling, armas, RTTI).

### 2.4 Clasificación pedida (a/b/c/d)

| Clase | Qué es | Dónde está | Objetos con ref. DLL |
| --- | --- | --- | ---: |
| (a) datos de código del juego | tablas y globales inicializados del juego | .data inicializado, origen juego | 1.071 |
| (b) RenderWare, D3D, CRT, bibliotecas | cualquier sección, origen renderware o crt | .rdata/.data/.bss + IAT | 784 (619 + 165) |
| (c) estado en ejecución | variables sin valor inicial | .bss | 2.830 (2.368 del juego) |
| (d) constantes | .rdata puro | .rdata, origen juego | 883 |

## 3. Qué se puede sustituir y cómo

La forma segura de sustituir un dato depende de quién lo toca:

* **Constante de `.rdata`** (valor que nadie escribe): se define en un `.cpp` propio con el valor extraído del EXE y se declara `extern const` en un `.h` propio; los consumidores se reapuntan. Es seguro aunque código vivo del EXE siga leyendo su copia (dos copias iguales de un valor inmutable). No cambia el código de los consumidores: la constante se carga de memoria como antes (no `constexpr`, para no variar el redondeo x87 ni el plegado de constantes).
* **Variable de `.data` o `.bss` del juego sin lector vivo** (G4, G5): se define en C++ con su valor inicial; solo es seguro cuando **todas** las funciones que la tocan son C++. La medición dice que es el caso de casi todos; el riesgo son los accesos que el escáner no ve (registro base, punteros guardados en otras tablas) y otras herramientas que escriben esas direcciones (CLEO, parches de ventana).
* **Datos compartidos con código vivo** (G6, RenderWare, CRT): no se pueden mover sin reconstruir antes el código vivo, o hay que parchear cada acceso del EXE. Van con la reconstrucción de RenderWare/CRT (plan 240A–389A y ramas `crt4a-locale`, `d3d9-device`).
* **vtables** (G9): los 469 `RH_Scoped…` apuntan a la vtable del EXE para enganchar sus ranuras; en el ejecutable independiente las vtables las genera el compilador y el enganche desaparece. No se copian.
* **Protección del EXE** (G10, `_TEXT_HA`, `ExeProtectionStub`): descartada por decisión del proyecto (código del protector).

Reglas: sin `__asm`/`naked`; un par `.h`/`.cpp` propio por grupo (`source/game_sa/DatosExe/`); sin duplicados (la misma dirección no se define dos veces); los valores iniciales salen siempre del EXE canónico con el script, nunca a mano; sin banco por función (decisión del propietario): compilación normal más las dos pruebas de humo.

## 4. Plan por partes y lotes

Detalle en [plan-lotes.md](plan-lotes.md) y [plan-lotes.csv](plan-lotes.csv). Son 10 grupos y 135 lotes de hasta 40 objetos u 8 KB (DE01 es un lote único por haberse hecho entero).

| Parte | Grupo | Contenido | Objetos | Riesgo | Estado |
| --- | --- | --- | ---: | --- | --- |
| 1 | G1 · DE01 | Constantes escalares `.rdata` (float, double, int32, uint32) con `StaticRef` tipado | 154 | bajo | **hecho** |
| 1 | G2 · DE02 | Constantes `.rdata` por otras vías (ayudante `f(0x…)`, `LoadWord`, `reinterpret_cast`, literal) | 224 | bajo | **hecho** (185 constantes) |
| 1 | G3 · DE03 | Tablas y cadenas `.rdata` del juego (más de 8 bytes) | 36 | bajo | **hecho** (cadenas y GUID; 33 eran vtables → G9) |
| 2 | G4 · DE04 | `.data` inicializado del juego sin lector vivo (tablas mutables) | 1.065 | medio | **hecho** parcial (539 objetos en A, B y C) |
| 3 | G5 · DE05 | `.bss` del juego sin lector vivo (estado) | 2.354 | medio | pendiente |
| 4 | G6 · DE06 | Datos del juego que aún lee código vivo | 20 | alto | tras cortar el lector |
| 5 | G7 · DE07 | RenderWare y D3D | 619 | alto | con el plan 240A–389A |
| 5 | G8 · DE08 | CRT, x87 y bibliotecas | 165 | alto | con el plan de CRT |
| 6 | G9 · DE09 | vtables enganchadas | 469 | alto | al retirar los hooks |
| 6 | G10 · DE10 | Protección del EXE | 170 | descartado | no se sustituye |

Orden de trabajo y razones: primero lo inmutable (partes 1) porque es seguro aunque haya lectores vivos; después `.data` (parte 2) porque trae valores iniciales que hay que extraer y verificar tipo a tipo; después `.bss` (parte 3), la más grande y la que más depende de tener el tipo correcto (los 180 objetos `.bss` con hueco grande son los que primero necesitan tipo); RenderWare, CRT y vtables van al final porque dependen de reconstruir el código vivo. Dentro de cada grupo los lotes se ordenan por propietario (clase) para que un lote toque pocos archivos.

Por lote: (1) `contrib/datos_exe.py` para tener la medición actual; (2) `contrib/datos_exe_lote.py <lote> --aplicar` genera las definiciones con los valores del EXE y reescribe las referencias; (3) compilar con el candado; (4) pruebas de humo (`prueba_ropero.py`, `prueba_agua.py`); (5) volver a medir y comprobar que las referencias del lote bajaron a cero.

### Riesgos

* **Escritura donde se creía constante.** `CTheScripts::DbgFlag` (0x859CF8, bool) está en `.rdata` pero el C++ lo escribe; se excluyen `bool` y `char` de DE01 (también un `char` que se usa como `char*`). Antes de cada lote de `.data`/`.bss` hay que comprobar escrituras desde el EXE vivo (`escribe_exe` en `objetos.csv`).
* **Parches externos.** Mods y CLEO que modifican constantes del EXE (FOV, aspecto) no verán las copias propias; es esperable en un ejecutable independiente, pero cambia el comportamiento con el EXE original cargado.
* **Redondeo y x87.** Una constante que pasa de leerse de memoria a `constexpr` puede cambiar el plegado; por eso se definen en un `.cpp` aparte con `extern const`.
* **Tamaños.** Los 330 objetos de tamaño desconocido y los arrays indexados exigen leer el tipo en el código antes de definirlos; un tamaño corto corrompe el vecino.
* **Compartidos.** Mover una variable que también toca código vivo desincroniza las dos copias: nunca se mueve sin reconstruir antes el lector.
* **Accesos invisibles.** El escáner no ve registro base ni punteros guardados; la medición es una cota inferior de lo que depende del EXE.

## 5. Lotes aplicados

### DE01 · constantes escalares de `.rdata` (hecho)

154 constantes (float, double, int32, uint32) con 221 referencias en 29 archivos reescritas de `StaticRef<T>(0x…)` a `DatosExe::Rdata::k<DIR>`. Definiciones en `source/game_sa/DatosExe/RdataConstantes.cpp` y `.h` con los valores extraídos del EXE (`std::bit_cast` de los bytes exactos). Lista en [lotes/DE01.csv](lotes/DE01.csv). Tras el lote la DLL tiene 8.546 referencias a 5.124 direcciones (de 8.767 y 5.276): 221 y 152 menos; dos objetos del lote conservan una referencia por otra vía. Compilación completa correcta (`bin\Debug\gta_reversed.asi`, build-project.ps1 con el candado) y pruebas de humo en BIEN con esa DLL: `prueba_ropero.py` (puerta y marcador, 15 entidades y 12 marcadores) y `prueba_agua.py` (UnderWaterness 1,0, brillo medio 44,9).

### DE02 · constantes de `.rdata` por ayudante, cast o literal (hecho)

185 constantes (180 float, 3 double, 2 uint32) y 913 referencias en 26 archivos. Cubre `ExeFloat(0x…)`, `RadarExeFloat`, `CamExeFloat`, `InstantHitConstant`, los lambdas locales `f(0x…)` y `constant(0x…)` (solo en archivos donde todas sus definiciones devuelven `float`, verificado por el script), `*reinterpret_cast<T*>(0x…)`, `*(T*)0x…` y los `StaticRef` que DE01 dejó por tipos mixtos. Definiciones en `RdataAyudantes.h/.cpp` (sufijos `_d`, `_i`, `_u`). Script: `contrib/datos_exe_de02.py`. Compiló a la primera.

### DE03 · cadenas, claves GXT y GUID de `.rdata` (hecho)

30 cadenas y 2 GUID en `RdataCadenas.h/.cpp`: 13 cadenas leídas con `reinterpret_cast<const char*>` (HandsUp, sunbathe, CODEAIR, JPEGMEM, "GTA: San Andreas"…), las 16 claves GXT `GA_*` de `Garage.cpp` (`GaKey` y `GaMessage` pasan a recibir `const char*`), `PedEvent.txt` de `DecisionMakerTypes.cpp` y los GUID de EAX de `AEAudioChannel.cpp` y `AEAudioHardware.cpp`. Script: `contrib/datos_exe_de03.py`. Quedan sin convertir tres usos de direcciones de cadena que se pasan a funciones del EXE o se ignoran: los nombres de animación del teléfono (`NewPhoneAnimTask`, parámetro ignorado por el EXE), `ThrowRuntimeError` de `CrtException.cpp` (grupo CRT) y los literales de vtable.

**Hallazgo sobre G3:** de los 36 objetos del grupo, 33 eran vtables del EXE que la DLL escribe en `this` al construir objetos (`*reinterpret_cast<uint32*>(self) = 0x85A788;`, cientos de sitios). No son datos copiables: son punteros a código del EXE y desaparecen cuando las vtables las genera el compilador. Se reasignan al grupo G9 (DE09), que es mucho más grande que los 469 `RH_ScopedVirtualClass`.

### DE04 · `.data` inicializado del juego (hecho en tres lotes)

Variables propias en `DataInicializadaA/B/C.h/.cpp` (namespace `DatosExe::Data`, nombre `v<DIR>`, no `const`: son estado vivo) con el valor inicial del EXE. Script: `contrib/datos_exe_de04.py <A|B|C>`. Un objeto solo se convierte si es seguro con las cotas de la medición: todas las referencias de la DLL a esa dirección son `StaticRef` con el mismo tipo (si no habría dos copias), ninguna otra dirección referenciada cae dentro de su extensión, ninguna función viva del EXE la lee, la escribe ni le toma la dirección, y el tipo es primitivo o array de primitivos. Se excluyen las direcciones que usan las pruebas de humo (ForegroundApp 0x8D621C, 0x8D514C).

| Lote | Contenido | Objetos | Referencias | Bytes |
| --- | --- | ---: | ---: | ---: |
| DE04-A | escalares `float` | 335 | 379 | 1.340 |
| DE04-B | otros escalares (int32, uint32, int16, uint8, bool, double) | 175 | 229 | 515 |
| DE04-C | arrays de primitivos (`std::array<T,N>` y `T[N]`) | 29 | 29 | 6.830 |

Quedan fuera de DE04 (por criterio): 313 objetos con formas distintas de `StaticRef`, 147 de tamaño desconocido, 125 con lector vivo, 27 de tipos mixtos y los de tipo estructura (`CRGBA`, `CVector`, `RwRGBA`): necesitan inicialización de agregados por tipo.

### DE05 · `.bss` del juego (hecho en cinco lotes, más una retirada)

Variables propias en `DataBssE1..E5.h/.cpp` (namespace `DatosExe::Bss`, `b<DIR>`, a cero como el `.bss`). Script: `contrib/datos_exe_de05.py <E1..E5>` (mismo criterio que DE04; tipos permitidos: primitivos y alias `Rw*`, punteros a clase declarada en `source/` o primitivo, arrays de primitivos, `CVector`, `CVector2D`, `CRGBA`, `RwRGBA`; se excluyen enumerados, clases con constructor y alias de plantilla).

| Lote | Contenido | Objetos | Estado |
| --- | --- | ---: | --- |
| E1 | enteros pequeños y bool | 350 → 308 | 42 retirados (DE05-R) |
| E2 | enteros de 32/64 bits | 346 | hecho |
| E3 | float y double | 174 | hecho |
| E4 | punteros | 114 | hecho |
| E5 | arrays y estructuras triviales | 66 | hecho |

Se excluyen las direcciones que usan las pruebas de humo y un margen alrededor (jugador 0xB7CD74/0xB7CD98, estado de juego 0xC8D4C0, área 0xB72914, colores 0xB7C4A0, UnderWaterness 0xC8132C, ForegroundApp 0x8D621C, pools 0xB74498…, 39 direcciones y 2 rangos; lista en el script).

### Hallazgo: código del EXE que la medición no veía (DE04-R y DE05-R)

Un primer E2 hizo fallar el ropero (posición NaN, sin entidades). La bisección automática (`D:\Original\_biseccion.py`, 13 compilaciones) lo aisló en `CTheScripts::SwitchDefaultAddress` (0xA43F54). La causa: el manejador SWITCH del EXE (0x4717AE) la lee desde **un hueco de 15 KB del `.text` «sin referencia directa»** que Ghidra no tiene como función y que el escáner por función no recorría. `docs/CodigoVivoExe/huecos.csv` lista 15.636 huecos, 2.260 solo en el juego, con 257 KB «sin referencia directa» además de 58 KB de «código con referencia sin función».

Corrección: `datos_exe.py` ahora (a) no cuenta como vivas las funciones DESCARTAR y CUBIERTA de `docs/CodigoVivoExe/funciones-vivas.csv` (como pidió el coordinador) y (b) desensambla también los huecos con código (con referencia, sin referencia directa e inicializadores estáticos) y los trata como código vivo, de forma conservadora (los bytes de datos mal decodificados dan falsos positivos, que solo excluyen objetos de más). Con el escáner corregido se detectaron **35 objetos de `.data` (DE04 A y B) y 42 de `.bss` (E1)** ya convertidos que ese código toca; se devolvieron a `StaticRef` (`lotes/DE04R.csv`, `lotes/DE05R.csv`) y se comprobó con compilación y humo. DE04 y DE05-E1 de `origin/main` incluyen esos 77 objetos: conviene integrar los commits DE04-R/DE05-R.

### Prueba de humo de marcadores

Aviso del coordinador (marcadores 3D que no aparecían con mi ASI) comprobado: falsa alarma. Nuevo `reversing/FnBIS1/prueba_marcadores.py` (en wv-8; no modifica los existentes): reutiliza el circuito del ropero y exige marcadores C3dMarkers en memoria, uno junto al marcador del ropero y que al menos uno cambie entre dos lecturas separadas 1 s (se dibuja cada frame). Resultado con mi ASI de DE05-E3 (sha 4be00ef78e6a): 12 marcadores, 12 cambian en 1 s, BIEN. Los píxeles rojos no sirven de criterio: con la cámara del ropero el marcador queda fuera de plano también con la DLL de main (rojo = 0 en ambas). Forma parte de `_humo_datos.sh` desde DE05-E4.

## 6. Cifras antes y después

| Medida | Antes (origin/main 4d39c9866b) | Después (DE01–DE05) | Diferencia |
| --- | ---: | ---: | ---: |
| Referencias de la DLL a datos del EXE | 8.767 | 5.930 | −2.837 |
| Direcciones distintas | 5.276 | 3.477 | −1.799 |
| `.rdata`: objetos con ref. de la DLL | 989 | 678 | −311 |
| `.data`: objetos con ref. de la DLL | 1.287 | 792 | −495 |
| `.bss`: objetos con ref. de la DLL | 2.830 | 1.837 | −993 |
| Funciones vivas (trozos) según el escáner | 5.569 | 3.708 | −1.861 (DESCARTAR/CUBIERTA fuera; los huecos entran como vivos) |
| Objetos con ref. de la DLL y lector vivo (compartidos) | `.rdata` 82, `.data` 27, `.bss` 74 | `.rdata` 86, `.data` 165, `.bss` 773 | sube porque el escáner nuevo es más estricto |

Los datos de `docs/DatosExe/actual/` son la medición del árbol actual con el escáner corregido.

## 7. Reproducir

```
python -X utf8 contrib/datos_exe.py [--salida carpeta]    # medición completa
python -X utf8 contrib/datos_exe_plan.py                  # plan-lotes.md/.csv, plan-objetos.csv
python -X utf8 contrib/datos_exe_lote.py DE01 [--aplicar] # lote DE01
python -X utf8 contrib/datos_exe_de02.py [--aplicar]      # lote DE02
python -X utf8 contrib/datos_exe_de03.py [--aplicar]      # lote DE03
python -X utf8 contrib/datos_exe_de04.py A|B|C [--aplicar]# lote DE04 (ya con el escáner corregido no repetir A/B sin revisar DE04R)
python -X utf8 contrib/datos_exe_de05.py E1..E5 [--aplicar] [--solo lista.txt]
```

Los CSV de la raíz de esta carpeta (`objetos.csv`, `referencias-dll.csv`, `por-*.csv`, `resumen.json`) son la medición **anterior** a los lotes (con el escáner antiguo); `actual/` es la posterior. Lo aplicado está en [lotes/](lotes/).

## 8. Siguiente

La ronda B (rama `datos-exe2`) está en [ejecucion-datos-b.md](ejecucion-datos-b.md): alcanzabilidad del código escondido (`huecos-alcanzables*.csv`), lotes DE05-G/F y DE04-X/D, inicializadores estáticos (`inicializadores*.csv`, `source/game_sa/DatosExe/StaticInit.cpp`) y las correcciones del escáner (huecos muertos, ganchos inactivos, punteros guardados en datos, tablas que el C++ lee por valor, campos de estructuras mayores). Queda:

* DE05: objetos de `.bss` con la referencia en una **cabecera** (se necesita un `extern` sin tipo, como el de DE05-F, pero la inclusión no puede ir al final del bloque de includes) y con formas distintas de `StaticRef` (`reinterpret_cast`, ayudantes, literales en ganchos): reescritura archivo a archivo.
* DE04-D con estructuras que `.data` compartía con cabeceras (`CRGBA`, `CVector`): mismo caso.
* Constructores estáticos pendientes (54 filas de `inicializadores-estado.csv`): dependen de reconstruir antes 28 constructores del EXE (0x441E00, 0x4470E0, 0x463750, 0x4C5CB0…).
* Despachadores originales de comandos de script (9 huecos, ~100 KB, tabla `0x8A6168`): siguen ejecutándose desde el EXE y bloquean datos; reescribirlos libera decenas de objetos.
* DE09 (vtables): al generarlas el compilador desaparecen las ~140 ranuras triviales de `CEvent*`/`CTask*` que siguen ejecutándose desde el EXE.
* Criterio de seguridad: siguen siendo invisibles los accesos con registro base dentro de funciones vivas; el humo (ropero, agua, marcadores) y la bisección son la red de seguridad. Los accesos desde datos y los ganchos inactivos ya se ven (ver ejecucion-datos-b.md §1).
