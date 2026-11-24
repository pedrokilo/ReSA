# Auditoria de «dato en dos sitios», grupo G5 (`.bss` del juego), lotes DE05-31 a DE05-61 (24/11/2026, rama `datos-g5b`, base `48e53f065c`)

Alcance: los 1.211 objetos de `docs/DatosExe/plan-objetos.csv` de los lotes 31 a 61 (el otro agente del grupo hace 1 a 30). Defecto buscado: el mismo dato accesible por dos caminos distintos, el objeto propio `DatosExe::*` (memoria de la DLL/standalone) y la direccion del EXE (`StaticRef<T>(0x..)`, `reinterpret_cast`, ayudantes, `base + desplazamiento`), con un escritor y un lector separados. Herramientas en `reversing/FnDatos/`.

De los 1.211 objetos, **534 eran objetos propios** (`DatosExe::Bss` o `DatosExe::R5`, incluidas 12 de las 32 ranuras de `DataBssF`); los otros **677 son solo la direccion del EXE** (`StaticRef` sin objeto propio): ahi no hay segundo sitio por construccion, salvo que una estructura citada por direccion los cubra junto a un objeto propio (lo cubre el cruce de extensiones).

## 1. Metodo

### 1.1 Estatico ampliado (`estatico.py`, `estatico2.py`, `estatico3.py`, `estatico4.py`, `solape_ranuras.py`)

* Tabla de objetos propios a partir del `.map` de enlace del standalone (`objmap.py`: direccion del EXE = sufijo hexadecimal del nombre, direccion real en el proceso) en lugar de las cabeceras: incluye las **32 ranuras `b<DIR><T>()` de `DataBssF`** y los 46 objetos `DatosExe::R5` (los de `.bss`), que la auditoria anterior (`docs/CodigoVivoExe/auditoria-datos.md`, objetos con `extern T nombre; // 0x..`) **no incluia**. Tamano: `sizeof` compilado (2.152 objetos de la auditoria anterior) y, para las ranuras, `sizeof` del tipo medido con un archivo que provoca el error `char (*)[N]` (`szslots.cpp`, `sizes_slots.json`).
* `estatico.py`: toda cita por literal 0x8E4000-0xCA0000 en `source/` (fuera de `DatosExe/`) que cae dentro de la extension de un objeto propio, en cualquier forma (`StaticRef`, cast, ayudante, literal suelto).
* `estatico2.py`: sumas `0xBASE + 0xOFF` evaluadas y `0xBASE + indice` con objetos propios que empiezan cerca de la base.
* `estatico3.py`: literales `.bss` pasados como argumento a un ayudante o lambda (el caso `fadeElement(0xBAA404, ..)`, que accede a `base + 4/8/12` y que ninguna busqueda por literal ve).
* `estatico4.py`: constantes con nombre `NOMBRE = 0x..` (65 en `.bss`) usadas con aritmetica y objetos propios cerca.
* `solape_ranuras.py`: cruce de las citas con su ancho real (ensamblador, `uses.json` de la auditoria anterior) contra la extension de las 32 ranuras.

### 1.2 Dinamico en el standalone (`sonda_datos.py`, `ejecutar_datos.ps1`, `analizar_dyn.py`, `resumen_g5b.py`)

La memoria del EXE (0x8E4000-0xCA0000, 3,9 MB, la que ven `StaticRef` y los literales) se lee desde fuera del proceso cada 3 s mientras `sonda_juego.py` (reversing/FnJuego) ejecuta escenarios reales. Un byte distinto de cero dentro de la extension de un objeto propio demuestra un **escritor por direccion** (en el standalone no hay codigo vivo del juego, solo C++, RenderWare y CRT): si el objeto propio tiene lectores por nombre, es el defecto. Se vuelca ademas el contenido del objeto propio (direccion real segun el `.map`) y se anota si el objeto llego a escribirse (escritor por nombre). Esto sustituye a la comparacion con la ASI sobre el EXE original: en el standalone la direccion del EXE no la toca ningun codigo vivo del juego y la senal es inequivoca; en la ASI cualquier funcion sin sustituir tambien escribe esas direcciones y la divergencia no distingue escritor C++ de codigo vivo.

Limite: la vigilancia detecta **escritores** por direccion en el codigo ejercitado; un lector por direccion de un objeto escrito solo por nombre no deja huella en memoria (se busca con el estatico).

## 2. Hallazgos reales

Todos demostrados con evidencia independiente (memoria del proceso y/o lectura del codigo). Los tres ultimos fueron hallados tambien, en paralelo, por el otro agente del grupo (`datos-g5a`; el 3 y el 4 caen en sus lotes): las correcciones de `Interior_c.cpp` y de los comandos de script son textualmente las suyas para que git las fusione sin conflicto.

| # | Dato (lote) | Escritor | Lector | Evidencia | Efecto | Correccion |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `bC17738` (`std::array<float,2>`) y `bC17780` (`std::array<uint8,2>`) (DE05-49) | `CTaskSimpleFight::LoadMeleeData` por direccion (filas de la tabla de combos `m_aComboData`, 0xC170D0 + fila x 0x88, que cubre 0xC17738 y 0xC17780) | `TaskSimpleUseGun.cpp:620-622` por nombre | dinamico: memoria del EXE en 0xC17738 = `f0ee6e3e f0ee6e3e` (0,2333 f) y en 0xC17780 = `01 01`, objeto propio = ceros (`evidencia/d2_datos_dyn.json`); estatico: la tabla se recorre con `FightTableAddress + (combo - 4) * sizeof(CMeleeInfo)` (constante con nombre: no la ve la busqueda por literal) | la pistolwhip evaluaba el golpe con tiempo de golpe 0 y vector de golpe 0 (golpe en el primer fotograma, desplazamiento equivocado) | `TaskSimpleUseGun.cpp` lee `StaticRef<std::array<float,2>>(0xC17738)` y `StaticRef<std::array<uint8,2>>(0xC17780)`; objetos propios retirados |
| 2 | `sl_C18D50` = `bC18D50<CPointRoute>()` (DE05-44) | FLUSH_ROUTE (1494) y EXTEND_ROUTE (1495) en `RunningScriptCommands1400To1499.cpp`, que citaban `StaticRef<CPointRoute>(0xC18D50)` | `CTaskComplexDrivePointRoute` en 1700-1799:341 y 2000-2099:328 (por la ranura propia) | estatico: cita exacta de la direccion de una ranura F (`estatico.py`, `solape_ranuras.py`); no se ejercita dinamicamente (solo misiones) | las tareas de ruta de coche leian una ruta vacia | `ScriptPointRoute` = ranura propia (como los otros dos archivos) |
| 3 | `sl_BAD3EC` = `bBAD3EC<TList_c<FurnitureEntity_c>>()` (DE05-28, lotes del otro agente) | `FurnitureManager_c` (instancia `g_furnitureMan` en 0xBAB380, 0x2078 bytes: su ultimo miembro `m_FurnitureList` esta en 0xBAD3EC) se construye y rellena por direccion | `Interior_c::PlaceObject/PlaceFurniture/Unfurnish` por la ranura | dinamico: memoria del EXE en 0xBAD3EC = `dcd3ba00 ecb3ba00 00020000` (lista con 512 elementos), ranura = ceros; estatico: `StaticRef<FurnitureManager_c>(0xBAB380)` mide 8.312 bytes por ensamblador y cubre la ranura | `GetNumItems() < 1` siempre: no se colocaba ningun mueble en interiores amueblados proceduralmente | las tres citas usan `StaticRef<TList_c<FurnitureEntity_c>>(0xBAD3EC)` |
| 4 | `bBAA408..bBAA410`, `bBAA428..bBAA430`, `bBAA438..bBAA440` (DE05-26, lotes del otro agente) | `CHud::DrawPlayerInfo::fadeElement(0xBAA404 / 0xBAA424 / 0xBAA434, ..)` accede a `base + 4/8/12` | `Hud::Initialise`, `DrawFadeState`, `DrawWeaponIcon/DrawAmmo` por nombre (`m_WeaponFadeTimer`...) | dinamico: `bBAA410` = 1 / 0x1c, `bBAA430` = `ffc99a3b`, `bBAA440` en la memoria del EXE con el objeto a cero; estatico: `estatico3.py` (literal como argumento de ayudante) | fundidos de arma, dinero y energia con estados inconsistentes (estado y ultimo valor en un sitio, temporizadores en otro) | `Hud.h`: los cuatro campos de cada uno de los tres bloques son `StaticRef<uint32>` (el otro agente lo resuelve por nombres en `Hud.cpp`; ambas son compatibles) |

Objetos propios que quedaron **muertos y solapados con datos reales del EXE** (nadie los lee por nombre; trampas para el futuro):

* Retirados aqui (mis lotes): `bA9B76C` (DE05-35, entrada 425 de `ms_modelInfoPtrs`; la auditoria anterior arreglo `Plane.cpp` pero dejo el objeto), `bC17738`, `bC17780` (hallazgo 1) y `bC4032C` (DE05-40, duplicado de `DatosExe::R5::vC4032C`, el mismo static local de `PostEffects`).
* Sin retirar (lotes del otro agente o fuera de G5; recomendado): `bB61364` y `bB62C1C` (DE05-1/2: citas `ScopedStaticRef` y del historial de radio por direccion), `bC0B184`, `bBAADC0` (DE05-10), `bC0F074`, `bC0F240` (`m_CarGroups`, DE05-14), `sl_BAD3EC` (hallazgo 3), `bBAA408..bBAA440` (hallazgo 4), `b96913F`, `v96917A..E`, `v969180` (arreglo `m_aCheatsActive`, DE05-5/14/21/36), y 17 pares de constantes `v85xxxx`/`k85xxxx` duplicados de `.rdata` (mismo valor, inocuos).

## 3. Cobertura

### 3.1 Estatico

* Las 32 ranuras de `DataBssF` y los 46 objetos de `DatosExe::R5` no estaban en la auditoria anterior; con ellos aparecen los hallazgos 2 y 3 (ranuras) y el solape de `bC4032C` (R5).
* 28 citas por literal dentro de objetos propios `.bss` (`estatico.py`): todas revisadas; ademas de los hallazgos, son coherentes (los dos lados usan la direccion y el objeto propio no lo lee nadie: `bB61364`, `bB62C1C`, `bC0B184`, `bC0F074/bC0F240`, `bBAADC0`, `bC97C34`; `bA444C4` solo en un mensaje de diagnostico de `StandaloneExe`) o adyacencia (ranuras de 16 bytes por alineacion, `PS2Symbol`/`gFontData`, `subGroupStore`, `MenuInUse`...). Sumas literal+literal (`estatico2.py`): 0. Bases con indice: 16 revisadas (`CarCtrl.cpp:399/400`, `Cam.cpp:609`, `Hud.cpp:1000/1002`, `InteriorGroup_c.cpp:162/291/294`: el indice no alcanza el objeto cercano). Bases literales pasadas a ayudantes: 124 revisadas, el unico defecto es `fadeElement`. Constantes con nombre: 65, 3 con objetos cerca, todas coherentes.
* Tamano: ningun objeto de los lotes difiere del tamano que el plan calcula por tipo o por acceso (0 de 524 con tamano conocido).

### 3.2 Dinamico (standalone, `ejecuciones/d2`, `d4`; evidencia en `evidencia/`)

Standalone de este arbol (`gta_sa_standalone.exe` enlazado con los `.obj` del ASI Debug de `wv-int` y los `over\` de este arbol, mas los cambios ya en `main` posteriores a esos `.obj`, entre ellos la correccion del gimnasio), partida 2 copiada en `_juego_run`, bajo `gta_lock`. d4 (con el exe corregido): escenarios `interiores` (casa de CJ, gimnasio, tienda de ropa, Ammu-Nation, barberia: entrada y salida de cada una), `extra` (menu de pausa, clima por truco, 25 punetazos, salud a 0 con muerte y reaparicion en el hospital), `spawn` (dos vehiculos por truco) y `conduccion` (acelerar, frenar, marcha atras, giros, freno de mano): 315 s de vigilancia, sin caidas ni excepciones (`UNHANDLED` = 0, `codigo-original-distinto=0`, ~330 s de latidos). d2 (con el exe anterior a la correccion del gimnasio): conduccion, armas, camara, trucos (trafico rosa/negro, trafico reducido, armas, buscado) e interiores hasta el gimnasio.

Objetos de los lotes 31-61 tras las correcciones (`evidencia/objetos_g5b_lotes31-61.csv`; 1.211 objetos del plan):

| Estado | Objetos |
| --- | ---: |
| Solo direccion del EXE (sin objeto propio; sin segundo sitio por construccion) | 680 |
| Objeto propio, **verificado dinamicamente** (escrito por nombre en la partida y memoria del EXE intacta) | 287 |
| Objeto propio **no ejercitado** (ni el objeto ni el EXE se escribieron: solo estatico) | 244 |
| Objeto propio con memoria del EXE escrita sin explicacion | 0 |

(Antes de retirar los 4 muertos eran 677 solo-direccion y 534 propios; en d2 tres de los propios tenian el EXE escrito: `bA9B76C`, `bC17738`, `bC17780`.) Los no ejercitados se concentran en Replay (33), PathFind/Pad, Stats, Radar/PostEffects, trenes, guardado/carga, misiones y script, y zonas de juego que la sonda no visita.

## 4. Pruebas de las correcciones

* Compilacion: `compilar_over.cmd` de `TaskSimpleUseGun.cpp`, `Hud.cpp` (con `Hud.h`), `Interior_c.cpp`, `RunningScriptCommands1400To1499.cpp`, `DataBssE3/E4/E5.cpp`, `rwcore.cpp` y `CharacterCommands.cpp` (estos dos ya en `main`, posteriores a los `.obj` de `wv-int`); enlace del standalone (`link exit=0`) y de la ASI (a mano con los `.obj` de `wv-int` mas los `over\`, `reversing/FnDatos/link_asi.py`; sin la tabla de exportaciones, que no usa nadie).
* ASI sobre el EXE original (`AGUA_ASI` = ASI propia, SHA256 `06ae4674...5cdb`): `prueba_ropero.py cal_mal.args`: **BIEN** (puerta y marcador, `evidencia/humo_ropero.txt`); `prueba_marcadores.py cal_mal.args`: **BIEN** (12 marcadores en memoria, 12 cambian en 1 s, `evidencia/marcadores_BIEN.txt`; se lanzo con `humo_marcadores_g5b.py`, que reserva `gta_lock` y pasa `-SinCandado` porque con 4 agentes esperando el candado las pruebas se pisaban al lanzarse a la vez).
* Standalone ~315 s sin caidas (d4).
* `python contrib/audit_hook_catalog.py --strict`: OK.

## 5. Lo que no cubre

* Lectores por direccion de un objeto escrito solo por nombre en codigo no ejercitado (solo estatico), y escritores por direccion en codigo no ejercitado (guardado/carga, misiones de script, replays, trenes, mapa y estadisticas a fondo, otras zonas).
* La comparacion con la ASI sobre el EXE original no se hizo: en la ASI cualquier funcion sin sustituir escribe tambien las direcciones del EXE y la divergencia no distingue ese caso del defecto buscado; el standalone no tiene codigo vivo del juego y la senal es directa. Los objetos que comparte el codigo vivo de RenderWare/CRT con el C++ salen de la clasificacion del plan, no de este trabajo.
* Los 680 objetos solo-direccion no se comprobaron dinamicamente (no hay segundo sitio salvo solape con una estructura citada por direccion, que cubre el cruce estatico).
* No se ejecuto `prueba_agua` (excluida por el encargo). No se hizo push ni merge.
