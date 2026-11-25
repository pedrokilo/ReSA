<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desviaciones conocidas del C++ reconstruido frente al EXE original

> **Evidencia pesada archivada:** los volcados completos de los bancos (`comparisons.jsonl.*`, etc.) ya no están en el árbol de git; se archivaron fuera del repositorio con manifiesto de hashes. Cómo restaurarlos: [ARCHIVO_LEEME.md](../reversing/CppTwelveLevel6Final/ARCHIVO_LEEME.md).

Revisión de estado del 24/11/2026: ver la sección 14 (cifras recontadas y desviaciones nuevas del ejecutable independiente). El resto conserva el texto del 16/11/2026.

Fecha de elaboración: 16/11/2026 (revisión de la de 08/11/2026, commit `160c24b0a0`). Árbol: rama `main`, commit `3e6fdb5f89`. Documento de solo lectura: no se compiló, no se ejecutó el juego y no se tocó ningún hook.

Objetivo: dejar por escrito, de forma verificable, **dónde el código C++ reconstruido no es idéntico al EXE de GTA SA o dónde la validación no lo garantiza**. Un hook con `validated=1` significa "existe evidencia histórica aprobada dentro de unos fixtures", no "idéntico al EXE para toda entrada" (véanse [CurrentStatus](CurrentStatus.md) y [ValidationInventory](ValidationInventory.md)). Un hook con `validated=0` (1282 filas a esta fecha) es código integrado **sin banco**: solo revisado contra el desensamblado.

Qué cambió respecto de la revisión del 08/11:

- Los lotes 189A a 207A pasaron por banco par a par (EXE contra DLL, 16384 pares por función, cero diferencias en la corrida aprobada). Los defectos que esos bancos encontraron y se corrigieron salen de la lista de abiertos y están en la sección 11 (Resueltas).
- Los lotes 208A a 239A (código del juego y bibliotecas enlazadas en el EXE) y R1A a R34A (RenderWare, D3D9, D3DX9, zlib) están integrados **sin validar**. Sus desviaciones deliberadas están en la sección 8 (A).
- Lo que los bancos de 189A–207A **no cubren** está en la sección 9 (B).
- Las funciones que no se pueden reconstruir en C++ puro están en la sección 10 (C).
- Las secciones 3.3, 6.3 y 7 se actualizaron con el estado actual; el resto de secciones conserva el análisis del 08/11 y lo dice cuando una cifra no se recontó.

Entradas de esta revisión (filas de tabla numeradas o con función y dirección):

| Bloque | Entradas |
| --- | ---: |
| Desviaciones vigentes del 08/11 que se mantienen (secciones 2.1, 2.2, 2.3, 2.4, 2.5, 3.2, 3.3, 7.1) | 54 |
| Nuevas, sección 8 (A): convenciones comunes 6, código del juego 11, RenderWare/D3D 17 | 34 |
| **Desviaciones vigentes en total** | **88** |
| Nuevas, sección 9 (B): límites de validación de 189A–207A, agrupados | 11 |
| Sección 10 (C): 7 categorías de motivo (118 funciones del juego y 54 de RenderWare/D3D en los CSV) y 10 ayudantes locales sin hook | 17 |
| Resueltas, sección 11: defectos corregidos por los bancos 30, entradas del 08/11 cerradas 10 | 40 |
| Notas de calidad de la validación (secciones 4, 5, 6.1, 6.2): equivalencias aceptadas 6, colaboradores 5, dominios excluidos 13 y 6 (no se cuentan como desviaciones) | 30 |

El riesgo (bajo, medio, alto) es una **estimación de quien redacta**, no un resultado medido. Criterio: *alto* = comportamiento distinto y alcanzable en una partida normal, o validación ausente en una ruta común; *medio* = comportamiento distinto en rutas raras o validación con un hueco relevante; *bajo* = diferencia no observable o sin llamadores que la lean.

## Método y fuentes leídas

| Fuente | Qué se hizo |
| --- | --- |
| `reversing/validation-registry/parte-001..005.json` | Se cargaron las 9493 entradas y se analizó el campo `scope` (frases, recuentos por expresión regular), `tested_sources`, `dependency_review` y `evidence.pairs`. Los recuentos por expresión regular cuentan entradas cuyo texto menciona el tema; no son recuentos de diferencias. |
| `reversing/*/README.md` (965 ficheros) | Búsqueda de "desviaci", "diferencia conocida", "Fuera de los fixtures", "saltad", "sin banco", etc. y lectura de los casos citados. |
| `source/` | Búsqueda de `FIX_BUGS`, `notsa::IsFixBugs`, `NOTSA`, `NOTSA_UNREACHABLE`, `NOTSA_LOG_*`, `locked = true`, `//RH_Scoped...`, `plugin::Call*`, y comparación de las direcciones de `RH_Scoped*Install` con `docs/hooks.csv`. |
| `docs/hooks.csv`, `docs/hooks_notsa.csv`, `docs/JuegoExe/` | Recuentos por tipo, filas ausentes, estado del mapa de funciones del EXE. |
| Cálculo propio | Hash SHA-256 de cada fuente listada en `tested_sources` frente al fichero actual (se aceptó coincidencia también tras normalizar CRLF a LF). Solo en la revisión del 08/11; no se repitió el 16/11. |
| `reversing/PendienteValidar/*.md` (informes de 208A–239A y R1A–R34A; el de 208A se leyó del commit `5e22c62e00`, ya no está en el árbol) | Lectura de las secciones de saltadas, notas y riesgos y búsqueda de "divergencia", "deliberada", "difiere", "cuelga", "sin inicializar", "inalcanzable", "no se reproduce", "a diferencia del EXE" en las tablas de funciones. Sección 8. |
| `reversing/Fn189A` … `Fn207A` (`README.md`, `scope_note.txt`) | Secciones "Fuera de los fixtures", "Correcciones", "Acotados del banco", "Notas de alcance". Secciones 9 y 11. |
| `docs/hooks.csv` (12 438 filas), `docs/JuegoExe/excluidas.csv` (118), `docs/RenderWareExe/no-reconstruibles.csv` (54), `docs/JuegoExe/funciones.csv`, `docs/RenderWareExe/README.md` | Recuentos del 16/11 y comprobación de qué direcciones tienen ya fila. |

Límites de este documento: no se leyeron los 965 README completos ni los 847 `validation-summary.json`; las categorías con cientos de casos iguales se agrupan con recuento y ejemplos. De los informes de `PendienteValidar` se leyeron las secciones de saltadas, notas y riesgos y las filas de tabla que contienen las palabras clave; no se releyó cada fila de cada tabla (hay 1282 funciones sin validar). Los README de lotes antiguos y el cuerpo de `CurrentStatus.md` (10/10) tienen cifras fechadas; donde se contradicen con `hooks.csv` actual se indica. El análisis del registro de validación (secciones 1, 5, 6.1 y 6.2) es del 08/11 y no se recontó.

## 1. Resumen con cifras

| Medida | Valor | Fuente |
| --- | ---: | --- |
| Filas en `docs/hooks.csv` (todas `reversed=1`, `locked=0`) | 12 438 en 996 grupos (08/11: 9493) | `docs/hooks.csv` |
| de ellas `validated=1` (evidencia histórica) / `validated=0` (sin banco: lotes 208A–239A y R1A–R34A) | 11 156 / 1282 en 142 grupos | `docs/hooks.csv` |
| tipo S (hook sobre la dirección del EXE) / tipo V (hook de ranura de vtable) | 10 282 / 2156; las 1282 sin validar son 1272 S y 10 V | `docs/hooks.csv` |
| Grupos con más filas sin validar | `RwCore` 182, `RpWorld` 172, `RwPlCore` 118, `RwD3D9` 115, `CPools` 51, `CPedAttractor` 34, `RpSkin` 33, `RpMatFX` 28 | `docs/hooks.csv` |
| Lotes con banco par a par (189A–207A) | 19 lotes, 16384 pares por función, cero diferencias en la corrida aprobada (15 y 16/11/2026) | `reversing/Fn189A` … `Fn207A` |
| Entradas con dirección `0x00000000` en el registro (casi todas V: la dirección no figura en el catálogo) | 1516 | registro |
| Registros separados en `docs/hooks_notsa.csv` (`validated=0`) | 4 | `docs/hooks_notsa.csv`, [HooksNOTSA](HooksNOTSA.md) |
| Entradas del registro con `dependency_review = null` (sin revisión de dependencias), corte 08/11 | 9326 de 9493 | registro |
| "Vigencia acreditada" declarada por el proyecto | 0 | [ValidationInventory](ValidationInventory.md), línea 10 |
| Entradas cuyas fuentes de `tested_sources` **ya no coinciden** con el árbol (aun normalizando CRLF), corte 08/11 | 7253 de 9493 (coinciden 2240) | cálculo propio |
| Entradas cuyo `scope` dice "Historical fixture approval of the archived build only. Current source hashes were intentionally not checked" | 237 | registro (p. ej. `GangFollower355`, `GeometryQueries355`, `InteriorCore355`) |
| Pares por función: mínimo / mediana / entradas con menos de 128 / con menos de 256 | 8 / 256 / 1342 / 2752 | registro (`evidence.pairs`) |
| Ficheros de evidencia distintos | 847 | registro |
| Funciones del EXE, región del juego 0x401000..0x74FFFF, según el mapa | 11 686 (08/11: 11 804) | [JuegoExe/README](JuegoExe/README.md) |
| Reemplazadas por C++ | 11 683 (100,0 %) (08/11: 9376, 79,4 %) | JuegoExe |
| Envoltura pendiente (la DLL aún llama al EXE) | 1 (0x4F39C0, lote 208A) (08/11: 22) | `docs/JuegoExe/funciones.csv` |
| **Sin reemplazar** en el mapa (C: 0x4541E0; D: 0x453F40) | **2** (08/11: 2406). Las dos y la envoltura son de 208A: el mapa se genera del catálogo y va por detrás de las filas de 208A (ya enganchadas, `validated=0`) | JuegoExe |
| Funciones del juego excluidas por no ser reconstruibles en C++ puro | 118 filas, 28 587 bytes | [excluidas.csv](JuegoExe/excluidas.csv); sección 10 |
| Región RenderWare/D3D 0x750000..0x81FFFF | 1889 funciones: 724 reemplazadas (38,3 %), 54 no reconstruibles documentadas, 1111 de bibliotecas de terceros (se enlaza la original), 0 envolturas pendientes | [RenderWareExe/README](RenderWareExe/README.md); sección 10 |
| Llamadas indirectas sin resolver en el EXE | 6004 (dato del 10/10, no recontado) | [BinaryCoverageAudit](BinaryCoverageAudit.md) |
| Puntos `plugin::Call*` en `source/game_sa` | 237 en 51 ficheros con la expresión `plugin::Call` (08/11: 175 en 38 con otro criterio; no comparable). Mayores: `AEMFDecoder.cpp` 38, `PedAttractor.cpp` 26, `AEUserRadioTrackManager.cpp` 23, `Garage.cpp` 19, `Collision.cpp` 15, `D3D9RasterFormat.cpp` 12, `GangWars.cpp` 11 | `grep` en `source/game_sa` |
| Líneas `__asm` / `naked` en `source/game_sa` | 5, todas comentarios (`X87Math.*`, `d3dx9math.cpp`) | `grep` |
| Bloques `FIX_BUGS` / `notsa::IsFixBugs` en `source/` | 58 apariciones en 31 ficheros; `FIX_BUGS` está **definido** (`source/CMakeLists.txt`, línea 479) | `grep` |
| `NOTSA_UNREACHABLE` en `source/game_sa` | 139 (las trazas `NOTSA_LOG_*` no se recontaron; 08/11: 120) | `grep` |

Nota sobre las dos bases de recuento: las 12 438 filas del catálogo (que incluyen RenderWare y D3D) y las 11 683 "reemplazadas" del mapa del juego no son el mismo conjunto (el mapa cuenta entradas binarias; el catálogo incluye sobrecargas, ranuras virtuales sin dirección propia y varias filas por dirección). No se ha conciliado una con otra.

Lectura honesta: el 100 % de reemplazo del mapa del juego significa que cada función del EXE tiene un hook, no que sea idéntica. De las 12 438 filas, 11 156 tienen evidencia histórica aprobada (vigencia acreditada: 0) y **1282 no tienen ningún banco**. Tampoco significa que el ejecutable ya no sea necesario: quedan 237 puntos `plugin::Call*`, 6004 llamadas indirectas sin resolver (dato del 10/10), 118 funciones del juego y 54 de RenderWare/D3D que no se pueden escribir en C++ puro, y 1111 funciones de bibliotecas de terceros (D3DX9, libpng/zlib, QuickTime) que se enlazan y no se reconstruyen.

---

## 2. Desviaciones de comportamiento declaradas (la DLL hace algo distinto del EXE)

### 2.1 Desviaciones limitadas certificadas como "fuera de la equivalencia" (`Pending355`)

Fuente: `reversing/Pending355/resolutions.json` (`limited_deviations`) y `reversing/Pending355/README.md`. El EXE lee pila sin inicializar; el C++ usa un valor definido. Esas rutas **no están certificadas** como equivalentes.

| Función | Dirección | Qué difiere | Riesgo | Referencia |
| --- | --- | --- | --- | --- |
| `CPlane::VehicleDamage`, ruta explosivo a humo | 0x006CC4B0 | El EXE lee `[esp+14h]` sin inicializar (`fld` en 0x6CC915, `fmul` 0x6CC919) y multiplica; el C++ usa intensidad de daño cero definida. Ruta excluida de las comparaciones. Choque a humo y explosivo a `BlowUpCar` sí se ejercitan por separado. | Bajo (efecto de partículas; el valor del EXE es indeterminado) | `resolutions.json`; `reversing/PlaneDamage355/README.md` línea 24 |
| `CCamera::CamControl`, vehículo tipo BIKE, escaleras activas, zona hallada y `m_pToGarageWeAreIn == nullptr` | 0x00527FA0 | El EXE lee dos punteros de pila (`[ESP+0x74]`, `[ESP+0x78]`) que esa ruta nunca escribe y los desreferencia si no son cero (0x528800, 0x52887A). El C++ los inicializa a `nullptr` y calcula una cámara fija definida. | Bajo a medio (cámara en un caso raro; comportamiento del EXE indeterminado) | `resolutions.json`; `reversing/World355Evidence/CamControl.limitations.json` |
| `CCamera::CamControl`, cámara fija de garaje con `FindDoors` que devuelve primera puerta nula y segunda válida | 0x00527FA0 | El EXE consume un vector alternativo `stack[0x40..0x48]` no escrito en esa ruta (vehículo 0x528764..; ped 0x5295D5..). El C++ usa elecciones definidas (target como fuente, dummy de la segunda puerta). | Bajo a medio | `resolutions.json` |

`CamControl` tiene fila `S` con `validated=1` en `hooks.csv` (línea 2913). Esa fila no refleja estas dos rutas excluidas. `CPlane::VehicleDamage` es V con dirección 0.

### 2.2 `FIX_BUGS` activos en el código instalado

`FIX_BUGS` está definido en la compilación, por lo que los bloques `#ifdef FIX_BUGS` corrigen errores del EXE: **por diseño no son idénticos al original**. En muchos lotes los desarrolladores quitaron el arreglo para igualar al EXE antes de validar (p. ej. `ControllerConfigManager.cpp`, `Radar.cpp`, `Weapon.cpp`, `Population.cpp`, `Streaming.cpp:3172`, `CTasks10a`, `TaskWave2A`: ver sus README); la lista siguiente son los que **siguen activos** en el árbol. No se comprobó, función por función, si cada una está enganchada y validada con el arreglo dentro del banco.

| Fichero y línea | Efecto frente al EXE | Riesgo | Notas |
| --- | --- | --- | --- |
| `Collision/Collision.cpp:1493` | Rechaza (devuelve `false`) una línea casi paralela al plano (`abs(plLnMag) < 1e-6`); el EXE divide y propaga NaN/Inf. | Medio | Está en cálculo de colisión de ruedas (contacto/distancia de toque según el comentario del código). Cambia resultados en un caso límite. |
| `Core/OctTree.cpp:81` | Devuelve `false` si `treeElement` es nulo (el EXE desreferencia). | Bajo | |
| `Draw.cpp:90` | Rectángulo de desvanecimiento con esquinas `{-5,-5,W+5,H+5}` en vez de `{-5,H+5,W+5,-5}`. | Bajo | Mismo cubrimiento visual esperado; no es idéntico en datos. |
| `Entity/Vehicle/Automobile.cpp:1918`, `Bike.cpp:828` | `ProcessEntityCollision` sale con 0 si falta `GetColData` de cualquiera de las dos entidades (marcador `FIX_BUGS@CAutomobile::ProcessEntityCollision:1`). | Bajo a medio | Filas V con dirección 0. El README `AutomobileFinal4` / `BikeWave4` declara que los bancos son por pares en el juego completo; no se verificó si el banco cubre `GetColData == nullptr`. |
| `Entity/Vehicle/Boat.cpp:357` | Alfa de la estela `*= (ptIdx+1)/3` en vez de `ptIdx/3` para los primeros puntos. | Bajo (visual) | `CBoat::AddWakePoint` etc. están validados (`Boat23`); no se comprobó si cubren esta rama. |
| `TheZones.cpp:87` | `GetZoneWasVisited` con `clamp` y otra indexación. Es una función NOTSA (no existe en el EXE con ese nombre). | Bajo | |
| `PathFind.cpp:2174` | `else if (outNext2)` en vez de `else` (evita escribir por puntero nulo). | Bajo | |
| `ModelInfoAccelerator.cpp:81` | Si falla la apertura para escritura, registra y cierra un handle nulo. | Bajo | |
| `Streaming.cpp:1181` | `ms_pExtraObjectsDir->AddItem(entry, img)` recuerda de qué CD proviene. | Bajo a medio | Añade información que el EXE no guarda. |
| `ObjectData.cpp:78/86` | Salta comentarios `#` y lee la cadena con tamaño. | Bajo | |
| `Font.cpp:200/203` | `SafeRemoveTxdSlot` comprueba si existe el slot (el EXE llama a `RemoveTxdSlot(-1)`). | Bajo | Declarado fuera de fixtures en `RadioFont1` (`CFont::Shutdown`). |
| `Fx/FxPrimBP.cpp:43` | Destruye la tercera textura una vez (el EXE la destruye dos). | Bajo | |
| `Frontend/MenuManager_Process.cpp:135,378,409,517-541,596,645`, `MenuManager_Input.cpp:975,1113` | Límites de arrays, flecha izquierda que retrocede, contador de 8 bits corregido, entrada de ratón enviada a `WndProc`. | Medio | Menús y entrada; `MenuMgrB` lista diferencias conocidas por `FIX_BUGS` (véase 2.4). |
| `app/app_camera.cpp:65,87`, `app/app.cpp:140`, `VideoPlayer.cpp:126,197`, `VideoModeSelectDialog.cpp:62` | Comportamiento de modo ventana, aspecto y vídeo (capa de plataforma NOTSA). | Bajo (plataforma) | Rama NOTSA de `CameraSize` declarada fuera de fixtures en `Fn133A`. |

### 2.3 Marcadores `NOTSA` que alteran el comportamiento frente al EXE

Hay 543 comentarios `// NOTSA` / `NOTSA:` en `source/game_sa` y `source/app`; la mayoría son ayudantes o firmas cómodas (sin efecto). Los siguientes cambian lo que se ejecuta, verificado leyendo el código citado:

| Ubicación | Qué difiere | Riesgo |
| --- | --- | --- |
| `Collision/Collision.cpp:940` | Línea de longitud cero: evita `0/0`. | Bajo |
| `Shadows.cpp:649` | Comprobación de cero para evitar división por cero. | Bajo |
| `Population.cpp:1779` | Opción nueva `ePedType::NONE` para ignorar el tipo. | Bajo |
| `Scripts/Commands/TextCommands.cpp:39` | Tiempo `int32` a `uint32`. | Bajo |
| `Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp:129` | Comprueba datos de jugador nulos ("el EXE no comprueba"). | Bajo |
| `Renderer.cpp:652` | Compara el callback con `RenderEntity` **y** con la dirección 0x732B40 para convivir con hooks reversibles. | Bajo |
| `IplStore.cpp:817` | "Inverted conditional" sobre `CColAccel::isCacheLoading()`; no se verificó la equivalencia con el EXE. | Medio (no verificado) |
| `PedType.cpp:43,79` | Almacenamiento propio de copia de seguridad para `MayhemCheat` (NOTSA). | Bajo |
| `Audio/Managers/AEUserRadioTrackManager.cpp:99,122,206,366,515,629` | Variantes UTF-8 de hilo y funciones, no poner a nulo/doble liberación, fallo también con handle nulo, no filtrar memoria. La tabla con FLAC está detrás de `USERTRACK_FLAC_SUPPORT`, que no se define en ningún fichero buscado. | Medio (rutas de pistas de usuario) |
| `Events/EventDamage.cpp:939` | Versión NOTSA de 0x4ADD60 que devuelve el resultado en vez de escribirlo por referencia. | Bajo |
| `Tasks/.../TaskSimpleCarGetOut.cpp:125`, `TaskSimpleCarOpenDoorFromOutside.cpp:85`, `TaskSimpleCarSlowDragPedOut.cpp:101` | Firmas con valor de retorno (envolturas NOTSA sobre el cuerpo original). | Bajo |
| `Fx/FxInfoManager.cpp:129-131` | Tres `case` de `FX_INFO_*` comentados (no se crean esos tipos). | Medio (no verificado si alguna vez se usan) |
| `NOTSA_UNREACHABLE` (139 usos; 36 en `Scripts/Commands/CharacterCommands.cpp`, 5 en `TheScripts.cpp`, 5 en `ControllerConfigManager.cpp`, 4 en `MenuManager_Draw.cpp`, 3 en `TaskInteriorLieInBed.cpp`) | El EXE ejecuta algo (o cae) donde la DLL aborta o da por inalcanzable; ruta no equivalente por construcción. | Medio |
| `NOTSA_LOG_*` (120 usos en `source/game_sa`) | La DLL escribe trazas (spdlog); el EXE no. Sin efecto en el estado, pero p. ej. `AudioFinal1` cita `NOTSA_LOG_ERR("Failed initializing MP3 module!")`. | Bajo |

Dentro del `scope` del registro, 302 entradas mencionan NOTSA (p. ej. 54 de `CAEScriptAudioEntity/AudioWave2`: "`CAEStreamingChannel::InitialiseSilence` con `CreateSoundBuffer` fallando NO se prueba (la DLL escribe `NOTSA_LOG_WARN`, el EXE no hace nada)"; 44 de `AEWeatherAudioEntity`: "libjpeg simulada en ambos lados (la DLL usa libjpeg-turbo RGBA, NOTSA)").

### 2.4 Desviaciones declaradas en los README de los bancos

| Banco | Función o ámbito | Qué difiere | Riesgo |
| --- | --- | --- | --- |
| `reversing/Fx43/README.md` | `CAESoundManager::PlaySound` | `ClientVariable` es 0 por defecto en la DLL; el EXE usa -1 (el README dice que `Service` ya no lo usa). | Bajo |
| `Fx43`, `AEWeatherAudioEntity` (AudioWave2) | Carga JPEG | La DLL usa libjpeg-turbo con `JCS_EXT_RGBA`; el EXE usa `JCS_RGB`. El banco normaliza el espacio de color a "rgba". | Bajo |
| `reversing/CPedD/README.md` línea 69 | `CPed::Constructor`, subobjetos de audio (0x138, 0x294, 0x394) | `CAEPedSpeechAudioEntity()` pone `m_PedAudioType = PED_TYPE_UNK (-1)`; el EXE `PED_TYPE_GEN (0)` (`Initialise` lo reescribe luego). Comparación de 0x138..0x43C **informativa** en el constructor (no cuenta como fallo). | Bajo a medio |
| `reversing/MenuMgrA/README.md` línea 53 | `DrawBackground`, `PrintMap` | Con `USE_BUILD_INFORMATION` la DLL llama a `DrawBuildInfo` (el EXE no); `PrintMap` pasa `true` donde el EXE pasa 3 (no observable). | Bajo |
| `reversing/MenuMgrB/README.md` línea 37 | `CheckSliderMovement`, `CheckForMenuClosing`, `CentreMousePointer` | La DLL escribe también `m_fMouseAccelVertical`; llama `InitialiseMouse(false)` donde el EXE `true`; no hace nada si la app no está en primer plano. | Bajo a medio |
| `reversing/MenuMgrD/README.md` línea 20 | Texto de estadísticas | La DLL convierte GXT a UTF-8, el EXE no ("desviación intencionada"); `FIX_BUGS` no se cubre. | Bajo a medio |
| `reversing/CEntC/README.md` línea 26 | `CreateEffects` | Comprueba `GetInSlot(...) != nullptr` (el EXE desreferencia). | Bajo |
| `reversing/CPedB/README.md` línea 28 | `Render`, `RemoveBodyPart` | `RenderBigHead` / `RenderThinBody` (trucos) y `NOTSA_LOG_DEBUG`; el banco aborta si hay trucos activos. | Bajo |
| `reversing/CVehD/README.md` línea 29 | `ProcessOpenDoor` y otros | Tiempos de animación de puerta no monótonos y grupos ROLLOUT/ROLLDOOR sin tiempos: el EXE usa memoria sin inicializar; la DLL usa `NOTSA_UNREACHABLE`. | Medio |
| `reversing/Render14/README.md` línea 37 | `SetName`, `SetNumFrames` | Bugfix activo de `SetName` (borra el bit de bone tag) y NOTSA de `SetNumFrames` (`m_bUsingExternalMemory = frameData != nullptr`), fuera de fixtures. | Medio |
| `reversing/CTasks10a/README.md` línea 41 | `CTaskComplexDestroyCarArmed::Constructor/Clone` | El EXE registra la referencia del vehículo en la dirección del parámetro de la pila (referencia colgante); con `FIX_BUGS` la DLL registra el campo +0x10. El banco **normaliza** esa referencia antes de comparar. | Bajo a medio |
| `reversing/Fx8/README.md` línea 57 | `CTaskInteriorLieInBed` creadas por `UseInfo` | `m_bRghtHandSide` (+0x10) se enmascara: `FIX_BUGS` lo escribe, el EXE no. | Bajo |
| `reversing/CppAnimAttractorStudy/README.md` línea 29 | Animación (comparación de theta con cero) | El EXE usa `FCOM`, el compilador `FUCOMP`; solo cambian las banderas sticky de excepción con NaN silencioso. "Los bancos no lo detectan." | Bajo |
| `reversing/CamControl355`, `PlaneDamage355` | Ver 2.1 | | |
| `reversing/AudioEngineFinal4/README.md` línea 18 | `CAudioEngine::Initialise` | Documenta diferencias NOTSA ya corregidas (orden de las inicializaciones estáticas, llamada extra a `CAEWaterCannonAudioEntity::StaticInitialise`). Estado actual no re-verificado aquí. | Bajo |

### 2.5 Compilación: aritmética de coma flotante

El EXE usa x87 (24 bits de mantisa, exponente ampliado); la DLL compila con SSE salvo en un conjunto reducido de ficheros con `/arch:IA32 /fp:fast` (`source/CMakeLists.txt` líneas 55 a 125: `D3DResourceSystem.cpp`, `D3DTextureBuffer.cpp`, `CarCtrl.cpp`, `AttractorScanner.cpp`, cuatro `FxInterpInfo*`, `Vector.cpp`, `X87Math.cpp`). Para el resto, la equivalencia numérica depende de ayudantes de `X87Math.*` o de que los bancos no encuentren diferencias. Casos documentados:

- `Fn132A/README.md`: `CDecision::Choices::Pick` convierte `rand` con `fild` en el EXE y con `cvtsi2ss` en la DLL; "iguales hasta 2^24" y el `rand` del EXE llega a 32767; los fixtures no cubren valores mayores. Riesgo bajo.
- 46 entradas del registro mencionan SSE/`cvtsi2ss` en su `scope`; 488 mencionan NaN señalizantes fuera de los fixtures o "sin NaN señalizante".

Riesgo global de esta categoría: **medio y no medido**; el proyecto lo reduce con bancos par a par con palabra de control de 24 bits, pero cada banco solo cubre sus fixtures.

---

## 3. Clasificación del catálogo y de los hooks

### 3.1 Destructores virtuales catalogados como S

76 filas `ScalarDeletingDestructor` y 338 filas `Destructor` (no scalar-deleting) son tipo S; ninguna es V. Ejemplo: `CAEStreamingDecoder,ScalarDeletingDestructor,0x004F2860,...,S`; `CRealTimeShadow,...,0x00706850`; las 13 de `Fn126A` (`CTaskComplexEnterCarAsDriverTimed` 0x640590, etc.).

Motivo documentado en el `scope` de `Fn126A` y en `Fn130A`: "`RH_ScopedVMTInstall` no puede nombrar la ranura del destructor". El hook S parchea la entrada del EXE; las vtables del EXE apuntan a esa dirección, así que los objetos construidos por el EXE pasan por el C++. Qué no cubre: objetos reales construidos por el EXE y `this` nulo o crudo ("Fuera de los fixtures"). Diferencia de catálogo, no de comportamiento. **Riesgo bajo**. Referencias: `reversing/Fn126A/validation-summary.json`, `reversing/Fn130A/README.md` línea 24.

### 3.2 Envolturas NOTSA que no existen en el EXE

| Envoltura | Qué es | Riesgo |
| --- | --- | --- |
| `CPedPlacement::FindZCoorForPed(CVector pos)` devuelve `std::pair<CVector,bool>` (`source/game_sa/PedPlacement.cpp:72`) | No existe en el EXE. Llama a la versión hookeada `FindZCoorForPed(CVector*)` en 0x616920 (fila `FindZCoorForPed-Ptr`, validada en `Fn133A`, 16384 pares). Se usa en `Ped.cpp:1776,1808`, `PedAttractorPedPlacer.cpp:67`, `RoadBlocks.cpp:350`, `TaskComplexGoToAttractor.cpp:70`. La envoltura en sí no tiene banco; es trivial pero no equivale a un original. | Bajo |
| `EventDamage.cpp:939`, tres `Task*` con firma de retorno (2.3) | Envolturas NOTSA | Bajo |

### 3.3 Hooks con código activo y sin fila en `docs/hooks.csv`

Revisión del 16/11: de las 19 direcciones que el 08/11 no tenían fila, 11 ya la tienen (véase la sección 11, Resueltas). Quedan 8, comprobadas contra `docs/hooks.csv` y el código (`RH_Scoped*Install`):

| Dirección | Función | Fuente | Estado |
| --- | --- | --- | --- |
| 0x7487CF, 0x747020, 0x746D80 | `WinInput::Initialise`, `EnumDevicesCallback`, `diPadSetRanges` | `app/platform/win/WinInput.cpp:360-363` | Instaladas con `RH_ScopedGlobalInstall`; sin fila. El propio archivo (líneas 151-154) avisa de que con SDL3 (la compilación del proyecto) 0x747020 y 0x746D80 son código del EXE. |
| 0x7469A0 | `diMouseInit` | `WinInput.cpp:354` y `:364` | `{.locked = true}` solo congela el interruptor de la GUI: el hook SE INSTALA activo (el comentario "fails with ACCESS DENIED and crashes" es heredado del proyecto original). Lote S2Aresto (21/11/2026): comparado con el EXE (7469A0..746A0C) el cuerpo propio hace lo mismo (CreateDevice con GUID_SysMouse, SetDataFormat, SetCooperativeLevel con ventana y 5 = FOREGROUND\|EXCLUSIVE o 6 = FOREGROUND\|NONEXCLUSIVE, Acquire con resultado ignorado; se detiene en el primer HRESULT negativo igual que el EXE); `WIN_FCHECK` solo registra el error y vuelve, sin abortar, de modo que un ACCESS DENIED (0x80070005) real de DirectInput da el mismo comportamiento que el EXE. Fila añadida a `hooks.csv` (locked=1, validated=0). Sin comprobar en juego. |
| 0x7486A0, 0x745560 | `Win32_InitApplication`, `Win32_InitInstance` | `WinMain.cpp` (0x745560 en la línea 120) | La segunda `{.locked = true}`. Solo con `!NOTSA_USE_SDL3`. Sin fila. Véase además A-J4 (icono). |
| 0x746ED0 | `CPad::GetMouseState` | `Pad.cpp:137` | Solo con `!NOTSA_USE_SDL3`; sin fila. |
| 0x675FC0 | `CTaskInteriorLieInBed::FinishAnimCB` | `TaskInteriorLieInBed.cpp:34` | Instalada con `RH_ScopedInstall` (S); el CSV sigue con la fila V con dirección 0 (línea 8239) y ninguna fila en 0x675FC0. **Tipo del CSV desactualizado.** |

Consecuencia: `docs/JuegoExe/funciones.csv` se genera desde el catálogo y no desde las instalaciones reales, así que puede contar como "Falta" una función ya enganchada (hoy solo ocurre con las tres direcciones de 208A, sección 1). **Riesgo: bajo para el comportamiento; bajo para la fiabilidad del recuento.**

### 3.4 Hooks desactivados o bloqueados deliberadamente

- Clases enteras con `InjectHooks()` comentado (`InjectHooksMain.cpp:891-894`): `CVector`, `CVector2D`, `CMatrix` (con instalaciones selectivas: hoy `InjectHooksExactMath`, `ExactOperators`, `MultiplyVector`, `AttachAssign`, `ExactOps`, `Lote99A` y `Lote162A`, líneas 897-909), `CPtrList`, `CPtrNodeDoubleLink`, `CPtrListDoubleLink`, `CPtrNodeSingleLink`, `CPtrListSingleLink` (`InjectHooksMain.cpp:760-766`; Fn131A los reactivó puntualmente con `Flush` en 0x552400 y 0x552470) y unas 30 clases de tareas y eventos (`InjectHooksMain.cpp` líneas 1160-1380 y 1517-1553 aprox., p. ej. `CTaskComplexKillAllThreats`, `CTaskSimpleHailTaxi`, `CTaskSimpleAbseil`). Para las clases de tareas, que su `InjectHooks` esté comentado indica que esas funciones no están reemplazadas; no se comprobó cuántas funciones del catálogo corresponden a esas líneas.
- `source/` contiene 109 líneas `//RH_Scoped...` comentadas. Ejemplo: `Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h:42` `//RH_ScopedInstall(Get, 0x5F7200, { .enabled = false, .locked = true }); // Spoils <ecx>, can't hook`.
- Hooks con `locked = true` instalados en el código: `diMouseInit`, `Win32_InitInstance`. El CSV tiene 0 filas bloqueadas ([CurrentStatus](CurrentStatus.md): "Bloqueos reales pendientes: 0"), coherente salvo por esos dos que están fuera del CSV.
- `Win::NOTSA_WinMain` (0x748710) y dos hooks `WindowedModeHooks`: registrados aparte en `hooks_notsa.csv` con `validated=0`; el cuerpo adaptado NOTSA no se presenta como fiel. `source/app/platform/win/WindowedMode.cpp` ya no contiene `__asm` ni `naked` (el parche 0x7F6781 se sustituyó por `_rwD3D9System`, 0x7F5F70, en C++; ver `reversing/FnD3D9Device/README.md`). **Riesgo bajo** (capa de modo ventana, no código del juego).

### 3.5 Nombres erróneos en el listado de pendientes

12 README de lotes registran 32 casos en que el listado de pendientes atribuía la dirección a una clase equivocada ("el listado decía X"; p. ej. 0x5DE920 era `CAEPedAudioEntity` y no `CGangs`; 0x563130 es `CSector`, no `CWanted`; `Fn126A`: 13 destructores de tareas). Los hooks se registraron con la clase real, pero los catálogos antiguos pueden conservar nombres viejos. **Riesgo bajo**.

---

## 4. Equivalencias aceptadas en los bancos (no son "idéntico", son "indistinguible por el banco")

| Caso | Qué se acepta | Riesgo | Referencia |
| --- | --- | --- | --- |
| `GangFollower355` (`CTaskComplexGangFollower::Destructor`, 0x65EBB0) | Tras el destructor, el primer dword del objeto lleva la vtable de `CTask` del EXE (0x86D48C) o la de la DLL (p. ej. 0x706cc0b4 en la corrida); el banco las compara como **equivalentes** (`VmtIdentity355` reconoce ambas). Justificación del README: el objeto está muerto y el valor no se lee. Arreglo solo de banco; producción sin tocar. Aprobado `20261108-231817-248056`, 4352 pares. | Bajo (el valor lo lee quien inspeccione memoria liberada) | `reversing/GangFollower355/README.md` línea 37 |
| Registros volátiles EAX/ECX/EDX tras la llamada | 449 entradas del registro dicen que se registran como **informativos** salvo en llamadores del EXE que sí los leen (se compara solo lo que lee algún llamador desensamblado). | Bajo a medio (un llamador no desensamblado podría leerlos; no se verificó) | `scope` de `Fn130A`, `Fn132A`, `CCarCtrl`, `CHud`, etc. |
| Relleno y bytes sin inicializar | "Padding and uninitialised bytes masked" (322 entradas); matriz temporal de `CHandObject` enmascarada (`CAnimatedBuilding`, 42 entradas). | Bajo | registro |
| Vtables de la DLL frente al EXE | Aparte de `GangFollower355`, `Audio13`, `Dependencies21D`, `Fn90A`, `ForcePedOutVt71`, `HeliPre75`, `Helpers65B/C`, `Helpers66D`, `TaskWave2A`, `VehCtor62` mencionan comparación de vtables o equivalencias de vtable en su README (no se leyó cada una). | Bajo | READMEs citados |
| `CPedD` constructor | Región de audio 0x138..0x43C comparada solo de forma informativa (2.4). | Bajo a medio | |
| `Fn130A` | `CopyFrom`, `LoadingScreenLoadingFile`, etc.: `sprintf_s` sustituido por `sprintf` porque el original no rellenaba con 0xFE; el banco compara contra la CRT de la DLL. Fuera de fixtures: "la exactitud de la CRT frente a la de la DLL en cadenas no ASCII más allá de las probadas". | Bajo | `Fn130A/README.md` |

## 5. Colaboradores falseados y falsos "idénticos"

La técnica dominante de los bancos es sustituir colaboradores por "falsos idénticos" en la dirección del EXE y en el cuerpo de la DLL, y comparar solo el cuerpo propio. Esto es válido para la función probada, pero **no valida los colaboradores ni la integración**.

| Dato | Valor |
| --- | ---: |
| Entradas del registro que mencionan falsos, dobles o dependencias simuladas | 7761 de 9493 |
| "dependencias simuladas y trazadas en el EXE y en la DLL" | 5102 |
| "Traced doubles get NO credit" / "Los dobles con traza NO tienen crédito" | 813 + 277 |
| README de bancos que mencionan "falso idéntico" | 163 |
| `Equivalencia limitada a los fixtures documentados; colaboraciones aisladas simétricamente no acreditan su implementación...` | 354 |

Colaboradores que los README dicen explícitamente **sin banco propio**, "no validados aquí" o validados solo mediante falsos:

- `CVector::Normalise` (`Fn96A/README.md`: `CVector::InjectHooks` apagada, "casi todas sin banco"; luego se hookeó `Normalise` 0x59C910 por `InjectHooksExactMath`).
- `CMatrix::SetTranslate` 0x59AF40 (`HeliGen74`; luego en `InjectHooksExactOps`), `RwMatrixDestroy` 0x7F2A20 y `RwMatrixUpdate` 0x7F18A0 (`MatrixOps76`: RenderWare, fuera del catálogo).
- `CCamera::IsSphereVisible(origen, radio)` 0x420D40: "sin hook en la tabla y sin banco propio" (`CarPolice75`).
- `RsDestroyPresetViews`, `CMemoryMgr::Free` y `isprint` del CRT (`PresetLoad74`: "el banco NO las valida"; la configuración regional tampoco).
- `CAnimManager::RemoveAnimBlockRef` (`PlayerAnim80D`), `FxSystem_c::EnablePrim`, `FindPlayerPed`, `FindPlayerVehicle` (`FxWheel77`: "sin banco propio en este lote").
- `Idle` y `FrontendIdle` (`Sys12`: "sin banco"); posteriormente `HookUnlockGame` los cubre con 2560 pares ([CurrentStatus](CurrentStatus.md)), así que el README de `Sys12` está desfasado en este punto.
- Pruebas con RenderWare y D3D sustituidos por trazas: `GfxWave2`, `HudCamFinal2` ("drawing through a fake RW render-state table"), `FxPost1` (`CPostEffects::Render`: rama de guardar fotos con E/S real fuera del banco).

**Riesgo medio**: la integración de la función con colaboradores reales solo se ha validado en parte; el propio proyecto lo declara ("no integración universal").

## 6. Cobertura parcial y dominios excluidos

### 6.1 El EXE falla, se cuelga o lee basura en esa entrada: se excluye del dominio

1075 entradas del registro mencionan casos que el EXE no maneja (lectura de pila sin inicializar, desreferencia nula, bucle infinito) y que se excluyen. Es el criterio correcto para comparar, pero significa que **el comportamiento del C++ en esos casos nunca se ha comparado** (a veces el C++ añade comprobaciones: son desviaciones por construcción). Ejemplos verificados:

| Ejemplo | Dónde |
| --- | --- |
| `CStreaming::AddEntity` y otras: "los casos en que el EXE lee pila sin inicializar quedan fuera del dominio" (94 entradas) | `scope` de `CStreaming AddEntity` |
| `CPostEffects::CCTV`/`Render`: sin NaN señalizante, a lo sumo un operando NaN, sin entradas que hagan leer locales sin inicializar ni bucle infinito; dominio excluye `height >= 448` (el EXE no termina) | `FxPost1` |
| `CPickups::AddToCollectedPickupsArray` y banco `PickWeap1` | `scope`: "fixtures do not make the EXE read uninitialised locals" |
| `CWaterLevel::AddWaterLevelQuad`: mínimo Y 1 o 3, configuración distinta de 0/1, líneas malformadas, bote sin matriz excluidos | `WaterWave4` |
| `CPlayerPed::ProcessGroupBehaviour`, `KeepAreaAroundPlayerClear`, `ProcessAnimGroups`, `JoinGroupFlags`: datos del jugador nulos, tarea nula, tabla de manejo sin comprobar el índice | `scope` ("no cubre ... el EXE se cae") |
| `CMemoryHeap::IntegrityCheck`: `memId` fuera de [-N, N] (el EXE escribe fuera del arreglo local) | `scope` |
| `CTimer::GetMillisecondTime`, `CMissionCleanup::Process`, `CGarage::InitDoorsAtStart`, `CPathFind::FindNodeClosestToCoors` | `scope` ("el EXE desreferencia sin comprobar") |
| `CAEMFDecoder::OpenSoundMedia`: el EXE usa un ECX de entrada como handle sin inicializar en la ruta de error | `scope` |
| `CGenericGameStorage::SaveDataToWorkBuffer-org`: bloques `ReportError` con búfer sin inicializar; la rama NOTSA de `GenericLoad` (comprobación de versión) no se ejercita | `scope` |
| `CRoadBlocks::Init`: fallo al abrir el fichero NO probado (la DLL aborta, el EXE desreferencia NULL) | `RoadScriptWave3` |
| `CPools::Initialise`: fallo de `new[]` no cubierto; tipo de objeto de la DLL no probado más allá del paso del elemento | `scope` |
| `CAudioEngine::AttachMissionAudioToObject`: eventos de misión por encima de 0xFFFF e ids de emisora fuera de int8 | `scope` (82 entradas) |
| `CVehicleModelInfo::AddRemap`: `CountCompsInRule` con negativos (bucle infinito en el EXE), nibbles de regla 5..15, `GetMaximumNumberOfPassengersFromNumberOfDoors(-1)` | `scope` (75 entradas) |

**Riesgo bajo a medio** en conjunto: son rutas que el EXE no recorre bien. Riesgo medio donde el C++ añade comprobación y el EXE tenía un comportamiento (aunque sea erróneo) que el juego original dependía de ver, p. ej. el nulo de un `CPed` del jugador.

### 6.2 Ramas, funciones o escenarios que el banco no cubre

"Fuera de los fixtures" aparece en el `scope` de 522 entradas (ES) y "outside the fixture/bank" en 142 (EN). Agrupados por categoría de texto (el recuento es de entradas que repiten el mismo bloque):

| Texto del `scope` | Entradas | Ejemplo de función |
| --- | ---: | --- |
| Objetos reales construidos por el EXE, `this` nulo o crudo, registros volátiles, destructores base falseados | 56 | `CTaskComplexEnterCarAsDriverTimed::ScalarDeletingDestructor` (`Fn126A`) |
| Exactitud de los colaboradores falseados y registros volátiles | 31 + 20 | `CEventDraggedOutCar::Constructor`, `CEventDontJoinPlayerGroup::Destructor` |
| NaN señalizantes más allá de los patrones del banco | 25 | `CEventAcquaintancePed::Destructor` |
| Índices, cuentas y rumbos fuera de rango | 18 | `CBoundingBox::IsPointWithin`, `CMatrix`, `TransformVectors` |
| Punteros nulos o inválidos, ranuras de arma, bytes de bandera no booleanos | 17 + 17 + 16 | `CMBlur::MotionBlurRender`, `CSetPiece::GetSpawnCoord1`, `CTaskComplexEnterCar::ComputeTargetDoorOppositeToFlag` |
| Pools con cursor fuera de rango (la DLL tiene `assert`) | 17 | `ComputeTargetDoorOppositeToFlag` |

Casos concretos de funciones o ramas **fuera de banco** y cómo se resolvieron:

- `NOT covered by the bank and NOT registered` (`CamHud1`, `PickWeap1`): `CHud::Draw`, `DrawRadar`, `CWeapon::FireSniper`, `DoBulletImpact`, `GenerateDamageEvent`, `CPickups::Update`, etc. Hoy `CHud::Draw`, `CWeapon::FireSniper` y `DoBulletImpact` tienen fila `S` y validación posterior (`HudCamFinal2`, `WeaponFinal2`), con colaboradores doblados.
- Ejecuciones rechazadas con diferencias y "no registradas": `CWaterLevel::AddWaterLevelQuad` (254/1024), `RenderFlatWaterRectangle` (472/1024), `SplitWaterTriangleAlongXLine` (258/512), `CRunningScript::SetCharCoordinates` (109/1024) y `CShadows::RenderStoredShadows` (318/512). Las cinco tienen fila en `hooks.csv` hoy, registradas por incrementos posteriores (`WaterWave4`, `RoadScriptWave3 validation-summary-b`, `ShadowsWave3 validation-summary-b`); en el caso de `AddWaterLevelQuad` la causa era del fixture, y en `SetCharCoordinates`/`RenderStoredShadows` del código fuente corregido.
- `CPostEffects::Render`: rama de guardado de fotos (0x705301..0x7053E3, E/S real) fuera del banco.
- `CAEStreamingChannel::InitialiseSilence` con `CreateSoundBuffer` fallando; `CAEAudioHardware::Initialise` con `m_nUserTracksScanState == IN_PROGRESS` (bucle `Sleep`).
- `CHud::DrawWeaponIcon`: slot de TXD libre NO probado. `CPlayerPed::SetupPlayerPed`: `playerId = -1`.
- `CFont::Shutdown`: la rama en que `SafeRemoveTxdSlot` (FIX_BUGS) omite `RemoveTxdSlot(-1)`, que el EXE ejecuta.

**Riesgo medio** para las que fueron registradas con colaboradores doblados y rutas de E/S o de fallo sin cubrir.

### 6.3 Validación histórica: vigencia cero

- 7253 de 9493 entradas tienen fuentes cambiadas desde la prueba (cálculo propio con SHA-256; hay diferencias legítimas, p. ej. `source/InjectHooksMain.cpp` en 1024 entradas, `X87Math.*` en 646, `source/CMakeLists.txt` en 427). Eso es lo esperado con un árbol que sigue cambiando; no prueba que las funciones hayan cambiado de comportamiento, pero **la evidencia ya no corresponde al código actual**. El lote 134R (commit `49ff2a9e89`) repitió bancos afectados por un hook nuevo; no se ha repetido todo.
- 9326 entradas no tienen `dependency_review`.
- 237 entradas son aprobaciones de una compilación archivada con "current source hashes were intentionally not checked; this is not final replay approval" (familias `*355`).
- Cola de repetición histórica: `reversing/CppTwelveLevel6Final/REANUDAR.md` (16/10/2026) indica 54 bancos aprobados de 148 en la regresión final de las oleadas 1 y 2 al detenerse; `Pending355/README.md` indica que al detener el trabajo (08/10/2026) 74 de 177 bancos cumplían la verificación de manifiestos completos. Son datos fechados; no se verificó el estado actual de la regresión.
- Cobertura numérica: 1342 funciones tienen menos de 128 pares y la mediana es 256.
- Actualización del 16/11: los 19 lotes 189A a 207A (unas 590 funciones) tienen banco par a par con 16384 pares por función (el 08/11 la mediana del registro era 256), compuertas de ABI, ruta, SEH y restauración, oráculo Python independiente y prueba negativa (mutaciones que deben fallar) en `Fn202A`. Esa evidencia **no entró en el registro de validación** de 08/11 (que sigue con 9493 entradas); vive en `reversing/Fn189A` … `Fn207A` y en `reversing/CppTwelveLevel6Final/runs/`. 1282 filas (lotes 208A–239A y R1A–R34A) siguen sin banco. Sus límites están en la sección 9.

**Riesgo medio**: es la mayor limitación de la garantía. El 100 % es histórico.

---

## 7. Funciones saltadas por complejas y funciones sin reemplazar

### 7.1 Saltadas documentadas (revisión del 08/11, actualizada)

| Lote | Función | Motivo | Estado a 16/11 |
| --- | --- | --- | --- |
| `Fn105A` | 0x4AB250 (5 bytes, `jmp 0x5389B0` a `CFileMgr::ReadLine`) | Trampolín de un salto; "enganchar un trampolín de un salto no aporta nada" | Sin hook propio; los 59 llamadores por 0x4AB250 acaban en `ReadLine` con hook (vigente) |
| `Fn130A` | 0x5DE920 (destructor de `CAEPedAudioEntity`) y 0x563130 (destructor de `CSector`) | Marco SEH y colaboradores sin hook | Resueltas en `Fn131A` (65536 pares, cero diferencias); véase sección 11 |
| `Fn132A` | 0x53F2D0 `GetMouseSetUp` | Ya tenía hook en código, no en el CSV | Resuelta: fila `Input,GetMouseSetUpExe` en `hooks.csv`; sección 11 |
| `Fn132A` | 0x747300 (`getDvdGamePath`) | Cinco IAT y `repe cmpsb` | Resuelta: `Win,getDvdGamePath`, `validated=1`, instalada en `WinMain.cpp:559`; sección 11 |

### 7.2 Sin reemplazar en el mapa del EXE

El 08/11 había 2406 funciones sin reemplazar y 22 envolturas pendientes. A 16/11 el mapa del juego da 11 683 de 11 686 funciones reemplazadas: faltan 0x4541E0 (C) y 0x453F40 (D), y 0x4F39C0 figura como envoltura. Las tres son del lote 208A (`CPhoneInfo` y `UserTracksLegacyString::Reallocate`) y **ya tienen hook** con `validated=0`; el mapa va por detrás del catálogo. Las 22 envolturas del 08/11 (`CWeaponModelInfo` 0x4CC970 y 0x4CD250, y 20 de `Win` entre 0x749330 y 0x74FC70) se resolvieron con los lotes 224A, 225A y 226A–229A: eran código de RenderWare y del controlador D3D9, hoy en `rwcore.cpp` y `rpworld.cpp`.

Lo que sigue sin existir como C++ equivalente son las funciones de la sección 10 (118 del juego, 54 de RenderWare/D3D) y los 237 puntos `plugin::Call*`. El mapa es una **cota inferior** (6004 llamadas indirectas sin resolver, dato del 10/10) y 0x4F39C0, 0x4CC970 y 0x4CD250 (todas con fila) muestran que se genera del catálogo (3.3).

**Riesgo alto** (para la meta de independencia del EXE, no para el comportamiento actual): el juego sigue llamando al EXE por esas funciones no reconstruibles y por los 237 puntos `plugin::Call*`.

---

## 8. Desviaciones deliberadas del código reconstruido sin validar (lotes 208A–239A y R1A–R34A)

Estas 1282 filas (`validated=0` en `docs/hooks.csv`) están revisadas contra el desensamblado, pero **no tienen banco**: ninguna diferencia de esta sección ha sido medida, solo declarada por quien reconstruyó. Fuente de cada entrada: el informe `reversing/PendienteValidar/<lote>.md` (el de 208A, en el commit `5e22c62e00`). Riesgo: estimación, mismo criterio que arriba.

### 8.1 Convenciones comunes a los lotes sin validar

Valen para casi todas las funciones de 208A–239A y R1A–R34A; no se repiten en cada fila.

| ID | Qué difiere del EXE | Lotes y ejemplos | Riesgo |
| --- | --- | --- | --- |
| A-G1 | Marcos SEH del EXE que solo desenrollan se omiten. En los constructores y destructores no cambia el estado; donde la función lanza, el desenrollado no libera los temporales. | 209A `vector::_Xlen` 0x4F41B0 y 219A instancia B 0x5ECAB0: la DLL "no vuelve" tras lanzar `length_error` y pierde (fuga) la cadena y el objeto temporales; camino casi inalcanzable (solo con `n > max_size`). 210A–221A (constructores de FxInfo, atractores, tareas). Excepción: 208A `UserTracksLegacyString::Reallocate` 0x4F39C0 usa `try/catch` de C++ con los dos niveles del original. | Bajo |
| A-G2 | Los objetos construidos por la DLL llevan la **vtable de la DLL**, no la del EXE. Código del EXE que compare el vptr por dirección vería otro valor. | 210A `CTaskSimpleNone` (EXE 0x859CFC) en 0x670B00; 216A y 218A todos los `FxInfo*_c`; 220A y 221A atractores de peatones (0x5EE960 y siguientes, p. ej. EXE 0x86C5E4). | Bajo |
| A-G3 | Destructores con borrado y constructores usan `::operator delete`/`new` de la DLL o por dirección del EXE; el EXE usa el CRT 0x8214BD / 0x82119A. Mismo almacén según los informes, no comprobado por banco. | 217A (destructores con borrado de `FxInfo*_c`), 218A, 220A, 221A. | Bajo |
| A-G4 | Los valores residuales de EAX/ECX/EDX (y ESI) que deja el EXE al volver **no se reproducen**; los informes afirman que ningún llamador revisado los lee. En R25A no se desensamblaron los llamadores para comprobarlo. | 205A (STL de `SArray`), 218A, 221A (0x5EEA20, `ExeProtectionStub` 0x401FBB y 0x4024C0), 231A (envolturas `usercall` de libjpeg), R25A, R26A (0x7F7540 deja basura en EAX). | Bajo a medio |
| A-G5 | Las llamadas entre funciones reconstruidas del mismo lote van directas a C++ y **no pasan por la dirección del EXE**; un banco que mida llamadas a esas direcciones desde dentro no las ve. Mismo efecto observable en memoria según los informes. | R24A (`RwFrame*` recursivas 0x7F0050, 0x7F0210, 0x7F0830), R25A (`RwTexDictionaryDestroy` a `RwTextureDestroy`, etc.), R26A, R27A, R33A. | Bajo |
| A-G6 | Para llamadores del EXE aún no reconstruidos se conserva la copia del EXE; los llamadores reconstruidos usan un ayudante C++ sin hook. La función existe dos veces (EXE y C++), sin garantía de igualdad. Lista en la sección 10. | 208A, 211A, 214A–215A, 224A, 231A–236A, 203A (0x5A42B0). | Bajo a medio |

### 8.2 Código del juego (208A–239A)

| ID | Función | Dirección | Qué difiere o no está garantizado | Riesgo | Origen |
| --- | --- | --- | --- | --- | --- |
| A-J1 | `SurfaceInfos_c::LoadSurfaceInfos` / `LoadSurfaceAudioInfos` | 0x55EB90 / 0x55F2B0 | La DLL usa su `sscanf`, no el 0x8220AD del EXE; sin límite de longitud de campo (no probar cadenas más largas que los búferes). Los bits de +0x94/+0x98 se comprobaron uno a uno contra el EXE, sin banco. | Bajo | 208A |
| A-J2 | `CPlayerPedData::operator=` | 0x45C4B0 | Los bits de +0x34 distintos de 9 y 10–12 dependen en el EXE de ESI sin inicializar del llamador (0x45DEF0, ESI = `CPlayerInfo`); la DLL los deriva de `this - 4`. No copia el relleno +0x22/+0x23, +0x87, +0x8F. | Bajo a medio | 214A |
| A-J3 | `Win32_InitApplication` | 0x7486A0 | El EXE carga el icono con `LoadIconA(hInstance, MAKEINTRESOURCE(0x64))`; el C++ usa `IDI_MAIN_ICON` = 1042. Código preexistente, no se tocó. Solo con `!NOTSA_USE_SDL3`. | Bajo | 226A |
| A-J4 | `FxInterpInfo32_c/Float_c/U255_c/255_c::Allocate` y los constructores de `FxInfo*_c` de 216A | `Allocate` NOTSA; ctores 0x4A5980 … 0x4A6A00 | 215A cambia `Allocate` para escribir **solo el byte bajo** de `m_nCount` (+6 del interp), como el EXE, y `Load` compara `int8`; el informe 216A sigue diciendo "escribe int16 en +0xE/+0xF". Informes contradictorios: confirmar en el código. | Bajo | 215A, 216A |
| A-J5 | `CAutomobile::BurstTyre`, `DoNitroEffect`, `DoHeliDustEffect`, `SetUpWheelColModel` | 0x6A32B0, 0x6A3BD0, 0x6B0690, 0x6A3060 | C++ preexistente sin hook que **no era fiel** (fuerza `(derecha*aleatorio)*masa`, condición del segundo escape invertida, `DoHeliDustEffect` con otro orden de sonda y cámara). Reescrito a mano contra el desensamblado; sin banco. | Medio | 211A |
| A-J6 | `CCustomCarEnvMapPipeline`: `CreateEnvMapPool`/`DestroyEnvMapPool`, `pluginEnvMatStreamReadCB`/`WriteCB` | refactor sobre 0x5D8BE0, 0x5D8C50 | Código ya validado que 211A refactorizó para no duplicar. Mismo comportamiento según el informe; "el banco de EnvMap viejo habría que repetirlo". | Bajo a medio | 211A |
| A-J7 | `CRunningScript::ProcessCommands1700To1799`, comandos con tarea por script (1701, 1704, 1712, 1722, 1723, 1730, 1735, 1761–1764) | 0x496E00 | Con el pool de tareas lleno el EXE se salta el constructor y entrega una tarea nula; el C++ **no reproduce** ese caso (comparar solo con pool no lleno). | Bajo | 238A |
| A-J8 | `CRunningScript::ProcessCommands1700To1799`, comando 1752 `CREATE_MISSION_TRAIN`; comando 1732 | 0x496E00 | 1752: el EXE no inicializa el puntero de salida; el C++ lo pone a `nullptr`. 1732: `SetEntityBlip` recibe 5 argumentos en el EXE (el 5.º, `this+8`, sin uso) y 4 en el C++. | Bajo | 238A |
| A-J9 | `CRunningScript::ProcessCommands1200To1299`, comando 1283 `CREATE_SWAT_ROPE` | 0x48B590 | El EXE pasa a `ClearSpaceForMissionEntity` un `CVector` local de la pila **que ese caso nunca escribe** (indeterminado); el C++ pasa un vector a cero. | Bajo a medio | 237A |
| A-J10 | `CRunningScript::ProcessCommands1900To1999`, comandos 1987–1991 (cuaterniones; 1988 y 1990 con `RwMatrix` local) | 0x46B460 | `RwMatrix` local sin inicializar salvo lo que escribe `CQuaternion::Get`; el EXE tiene basura en el resto; el C++ la deja sin inicializar y `SetPosn` la pisa. | Bajo | 237A |
| A-J11 | `ExeProtectionStub::*` (MD5 0x4012E9, `ResolveKernelImports` 0x401FBB, `InitCallbackTable` 0x4024C0 y siguientes) | bloque 0x401000–0x403DFF | Solo se ejecutan en el arranque del EXE, antes de que cargue la DLL: el hook nunca se ejercita en una partida. El EXE deja EAX/ESI con el último puntero (no reproducido). Datos en BSS rellenados en tiempo de ejecución. El informe pide decidir si se quieren enganchadas (el parche de entrada modifica esas rutinas). | Bajo (sin efecto en partida) | 221A, 238A |
| A-J12 | Separador de la línea de comandos 0x746480 (`__usercall`, EAX = cadena) | 0x746480; llamadora 0x748710 (`NOTSA_WinMain`) | **Sustituido por el CRT**: `WinMain.cpp` usa `__argv`/`__argc` y no reproduce el separador. Diferencias con el CRT (medidas con 56 692 líneas; el oráculo coincide con el código del EXE emulado en todas): el original no trata barras invertidas ni comillas dobles seguidas (el CRT sí: `\"` es una comilla literal, de modo que `-path "C:\x\" -vms` funde `-vms` en el argumento anterior); conserva las comillas intermedias (`-v"m"s` queda igual, el CRT da `-vms`) y solo borra una comilla final; trata como separador cualquier byte <= 0x20 (CR, LF, controles), el CRT solo espacio y tab; una cadena solo de comillas (`""`, `"`) cuenta como argumento pero su ranura queda **sin inicializar** (basura del heap; el CRT da una cadena vacía); no hay trato especial del primer token. El EXE recorre `i = 1..argc-1`; `WinMain.cpp` recorre `i = 0..argc-1` (incluye el nombre del programa). Efecto real: el único consumidor es `rsPreInitCommandLine`, que solo compara con `-vms` (`rsCOMMANDLINE` acaba en `rsFILELOAD`, sin manejador). En las 105 líneas de uso real (`-windowed`, `-width`, `-height`, `-vms`, rutas entre comillas, tabuladores) el resultado es idéntico; el efecto sobre `-vms` solo aparece con barra final antes de la comilla de cierre y `-vms` detrás, con comillas dentro de `-vms` o con CR/LF/controles. Sin cambio de código. | Muy bajo | G3A, [reversing/FnG3A](../reversing/FnG3A/README.md) |

Nota: A-J7 y A-J8 son del lote 238A (`ProcessCommands1700To1799`); A-J9 y A-J10 del 237A. `ProcessCommands800To899` (0x481300, 239A, 8957 bytes) no declara divergencias: reproduce las rutas sin comprobar nulo del EXE (resultado de `GetAtRef` sin comprobar; índices de `CWorld::Players` y de `UsedObjectArray` sin acotar).

### 8.3 RenderWare, D3D9, D3DX9, zlib y libjpeg (R1A–R34A)

| ID | Función | Dirección | Qué difiere o no está garantizado | Riesgo | Origen |
| --- | --- | --- | --- | --- | --- |
| A-R1 | `RwMatrixScale`, `RwMatrixTranslate`, `RwMatrixRotateOneMinusCosineSine` | 0x7F22C0, 0x7F2450, 0x7F1D00 | Con combinación inválida (fuera de 0..2) el EXE desreferencia `NULL+0xC` y **se cuelga**; la DLL registra el error 0x80000003 y devuelve NULL. Sin llamadores con valores fuera de 0..2. | Bajo | R13A |
| A-R2 | `RwEngineGetCurrentSubSystem`, `GetNumVideoModes`, `GetCurrentVideoMode` | 0x7F2C60, 0x7F2CC0, 0x7F2D20 | Si el dispositivo devuelve TRUE sin escribir, el EXE devuelve una variable **sin inicializar**; la DLL devuelve 0. No ocurre con el controlador D3D9. | Bajo | R13A |
| A-R3 | `RwMatrixRotateOneMinusCosineSine`, `RwMatrixTransform` (rellenos +0x1C, +0x2C, +0x3C) | 0x7F1D00 | El EXE copia basura de pila a los rellenos; la DLL deja los suyos sin inicializar. Comparar solo ejes, posición y flags. | Bajo | R13A |
| A-R4 | `_rwD3D9ImageToRaster` | 0x801390 | Con un D3DFORMAT sin función de píxel el EXE hace `call 0`; la DLL **sale sin escribir**. | Bajo | R14A |
| A-R5 | `RwImageCopy` | 0x803950 | Con clave de profundidades no soportada el EXE desreferencia NULL (cuelgue); la DLL **devuelve NULL**. | Bajo | R30A |
| A-R6 | `RwImageApplyMask` | 0x802AF0 | Dos ramas muertas del EXE (resultado 0 de los copiadores) no se reproducen. Con falta de memoria sale sin error, como el EXE. | Bajo | R30A |
| A-R7 | `_rwD3D9RasterSetFromImageIndexed`, `_rwD3D9RasterSetFromImage` | 0x800C80, 0x8001E0 | Tabla de luminancia de 256 entradas a 0 (el EXE deja basura fuera de la paleta); la bandera "paleta ya bloqueada", sin inicializar en el EXE, es 0. Los fallos de bloqueo salen sin liberar la imagen reescalada (igual que el EXE). | Bajo | R29A |
| A-R8 | `_rwD3D9SetPresentParameters` y rutinas de modos de vídeo | 0x7F6CB0 | `GetWindowRect` con rectángulo a 0 y modos de pila sin inicializar a 0 (el EXE deja basura). | Bajo | R26A |
| A-R9 | `RpLightStreamRead` | 0x751F00 | Con `longitud > 24` el EXE desborda la pila; **no emulado**. Rama `version < 0x30300` (`-RwSqrt(1/(a*a+1))`) inalcanzable con versiones válidas. | Bajo | R16A |
| A-R10 | `_rwD3D9VSInvertRigidMatrix` | 0x764890 | Con salida == entrada difiere del EXE (orden de escrituras); los llamadores usan 0x8D73D0. | Bajo | R21A |
| A-R11 | `RwCameraSetNearClipPlane`, `RwCameraSetFarClipPlane`, `RwFrameRotate` (ángulo `float` por pila) | 0x7EE1D0, 0x7EE2A0, 0x7F1010 | Un NaN señalizante de entrada se convierte en silencioso en la x87 y no en el C++. Irrelevante en el juego. | Bajo | R23A, R24A |
| A-R12 | `RwD3D9CameraIsBBoxFullyInsideFrustum`, `RwD3D9CameraIsSphereFullyInsideFrustum`, `RwD3D9SetSurfaceProperties` | 0x7FAD90, 0x7FAD30, 0x7FC4D0 | Escritas en `float` con SSE, compiladas con las opciones normales (sin `/arch:IA32`): coinciden con la x87 de 24 bits salvo **denormales y desbordes**, que es la clase de diferencia que los bancos de 193A, 198A, 200A, 202A y 207A encontraron en código del juego (sección 11). El informe de R28A prevé moverlas a un ayudante de `X87Math.cpp` si el banco ve diferencias. | Medio | R25A, R28A |
| A-R13 | `D3DX9AcosPS` y variantes SSE; `D3DXFloat16To32Array_3DNow`, `D3DXVec2Transform*` | 0x7C2AAE, 0x7C203B, 0x7C2090 | Se enganchan aunque la CPU de pruebas no ejecute 3DNow!: en una máquina con 3DNow! el redondeo (`pfmul`/`pfadd`) podría diferir en la última cifra. En `AcosPS` el compilador puede conmutar operandos de una suma (solo cambia la carga útil de un NaN con dos operandos NaN). | Bajo | R4A |
| A-R14 | `_rwD3D9CPUGetDescription` y las cuatro pruebas de CPU | 0x80D680 y vecinas | Dependen de la CPU del equipo: solo comparables con el EXE en la misma máquina. Con cadena de marca devuelve NULL. | Bajo | R34A |
| A-R15 | `_rpMatFXD3D9DrawMesh` | 0x812C90 | R8A pasaba mal `BaseVertexIndex`/`MinVertexIndex`; R10A lo corrigió por revisión. Sin banco: debe revalidarse con mallas de `baseIndex` distinto de 0. | Bajo a medio | R8A, R10A |
| A-R16 | `WindowedModeHooks` (NOTSA, preexistente): `_rwD3D9CameraBeginUpdate`, `_rwD3D9CameraClear` (el parche `naked` `HookDirect3DDeviceReplacerSA`, 0x7F6781, se retiró al reconstruir 0x7F5F70 en C++); copia local `D3D9SetPresentParameters` | 0x7F8F20, 0x7F7730 | Registradas en `hooks_notsa.csv` con `validated=0`. `WindowedMode.cpp` avisa de que algo de `_rwD3D9CameraClear` rompe el suelo reflectante de la casa de CJ. La copia local de `D3D9SetPresentParameters` (con `flags &= ~rwVIDEOMODEEXCLUSIVE`) **no es la del EXE**; la de R26A sí. | Medio (modo ventana) | R25A–R27A |
| A-R17 | Funciones R-lotes que preservan **defectos del EXE** (no son desviaciones, pero no se protegen) | `_rpSkinInitialize` 0x7C8740, `_rpSkinD3D9CreateVertexBuffer` 0x7C9680, `RtBMPImageRead`, `RwCameraStreamRead` 0x808DE0, `_rwFrameListStreamRead`, `RwPalQuantInit` 0x80C470 | Desreferencia de skin NULL, `div` por stride 0, desbordes de pila con cabeceras BMP o cámaras malformadas, fugas de marcos, estadísticas sin inicializar: se copian tal cual. Un archivo malformado produce el mismo fallo que en el EXE. | Bajo | R3A, R32A, R33A |

Nota: varios nombres de RenderWare en R1A–R34A son **inferidos** (funciones internas del SDK sin símbolo público; los informes los marcan "inferido"); las direcciones son las verificables.

## 9. Límites de validación de los lotes 189A–207A (qué no cubren sus bancos)

Los 19 lotes (unas 590 funciones) pasaron banco par a par con cero diferencias. Esa evidencia vale **dentro de sus fixtures**. Fuente: sección "Fuera de los fixtures" (y "Acotados del banco", "Notas de alcance") de `reversing/Fn<lote>/README.md` y `scope_note.txt`. Son límites del banco, no defectos del código: se agrupan porque se repiten.

| ID | Qué no cubre el banco | Lotes | Ejemplos | Riesgo |
| --- | --- | --- | --- | --- |
| B1 | `this` nulo o crudo; objetos reales construidos por el EXE (vtables, eventos, tareas, peds, vehículos, model infos); la **exactitud de los colaboradores falseados**: se sustituyen por falsos idénticos en la dirección del EXE y en el cuerpo de la DLL, y lo único que se compara es la función probada. | 189A–207A (todos) | `Fn189A`, `Fn190A`, `Fn192A`, `Fn204A` ("la vtable de los objetos construidos: solo se comprueba que apunte al ASI o a la del EXE") | Medio |
| B2 | **Pools reales y asserts de las plantillas de la DLL**: ranuras fuera de capacidad, ocupadas o ya libres, borrados dobles, cursores fuera de rango. La plantilla `CPool` de la DLL tiene `assert` que el EXE no tiene; el fixture solo usa huecos válidos. | 189A (`CPool::GetAt`), 195A, 196A, 199A, 200A (huecos 0..63 en `CTrain`), 203A (`COctTree`, `CVehicleStructure`), 206A (pool de atractores) | `CPool<CVehicle,CHeli>::CreateAtRef` 0x6D5DD0; `CTrain` destructor; `CPedAttractor` destructor 0x5EC4B0 | Bajo a medio |
| B3 | **Aritmética extrema**: NaN señalizantes, denormales, magnitudes enormes o diminutas, ángulos de más de 2^14, 2^20 o 2^63, posiciones de más de 2^60, rangos de azar cuya conversión no cabe en 32 bits (`_ftol` frente a `cvttss2si`), anchuras de `CRect`/`CBox` cero o no finitas. En varias el oráculo no tiene modelo exacto y solo se compara EXE contra DLL. | 192A, 193A (dos NaN distintos en una misma resta), 197A, 198A, 199A, 201A (`VectorSubtract`, `Normalise` y ángulos falseados), 202A (`ConvertToEulerAngles` 838 filas y `ConvertFromEulerAngles` 2384 filas), 203A (radio sNaN), 207A | `CTrain::OpenDoor` 0x6F6370 (200A); `ConvertToEulerAngles`/`ConvertFromEulerAngles` (202A); `FindAttractorInRange` (203A) | Medio |
| B4 | **Entradas fuera de dominio**: bytes altos basura en argumentos `bool`/`byte` (207A), tipos de nodo distintos de 0 y 1, áreas de enlace fuera de 0..79, jugadores en foco distintos de 0 y 1 (197A), índices de modelo fuera de -3..7 (198A), códigos de puerta distintos de 0..255 y ejes distintos de 0..2 (200A), índices de radio fuera de 0..63 y atractores de tipo >= 10 (202A), contador de extras de `CVehicleStructure` por encima de 6 (203A), scripts brain fuera de rango (192A). | 192A, 197A, 198A, 200A, 202A, 203A, 207A | `FindNodeClosestToCoors`, `RecordNodesInCircle`, `FindNodePairClosestToCoors` (sin candidatos y máxima > 10000.0f) | Bajo a medio |
| B5 | **Estructuras corruptas o incoherentes**: listas con enlaces corruptos o más de 5 nodos (198A), `SArray` con el fin anterior al inicio (206A), ciclos o índices de hijo fuera del pool en `COctTree` (203A), cuentas enormes de vectores y `n` con el bit alto (205A), capacidades de pool enormes y cadenas de animación que desbordan el objeto de 0x80 bytes (191A), rutas de patrulla con nombres sin NUL o más de 12 nodos (195A), peatones sin matriz (194A, 204A), entidades crudas en índices 2 y 4 (204A), subtarea no nula en el destructor de `CTaskComplexSeekEntity<FixedPos>` (194A). | 191A, 194A, 195A, 198A, 203A, 204A, 205A, 206A | `CPatrolRoute::Reverse` 0x66D550; `SetEntryExitFlagWithIndex` con slot libre | Bajo |
| B6 | **Memoria agotada**: `operator new` nulo y pool de tareas lleno. El EXE y la DLL se comportan de forma distinta (el EXE desreferencia nulo; la DLL devuelve nulo o no reproduce el caso). | 194A, 198A (`CTask::operator new` nulo en `GO_TO_POINT`), 199A (`CTaskComplexKillPedOnFoot::Clone`: el EXE falla, la DLL devuelve nulo; tamaño que la DLL pasa a `CEvent::operator new`), 201A (rama 0x38B de `CreateSubTask`) | `CTaskComplexKillPedOnFoot::Clone`; `GetModelInfoIdFile` (reserva nula: el EXE escribe en [0]) | Bajo |
| B7 | **Objetos reales de RenderWare, Fx, DirectInput o ficheros**: listas de sombras reales (190A), `CObject` con sombra, lista de colisiones, LOD, referencias o matriz reales (192A), animaciones, peds, pads, fuegos y `FxManager_c` reales (193A), `CFileMgr` falseado y almacenes en su dirección real de 14 000 elementos (194A), `RegisterReference`/`CleanUpOldReference` falseados (200A), marcos nulos con `assert` en `RwFrameDestroy` y `clump` que no es `RwObject` válido (203A, 206A), DirectInput real y estado real de `CPad::UpdatePads` (196A), asociaciones, jerarquías y manos reales (202A). | 190A, 192A, 193A, 194A, 196A, 200A, 202A, 203A, 206A | `Input::InputShutdown` 0x747000; `CPad::StopPadsShaking` | Medio |
| B8 | **Dominios acotados por el propio banco** (no son defectos en el uso real): AL de falsos booleanos limitado a 0 o 1, EDX restaurado por los falsos, floats que no son NaN señalizantes (198A); `rand()` falseado en 0x821B1E y `CGeneral::GetRandomNumber`, segundo `GetTimeLeft` limitado (197A); destructor de la matriz local de `ScanEffect` sin falsear, etiqueta común del callback de fin de `StartAnim`, pool de eventos 0x4B5570 frente a 0x4B5620 (202A); punteros de callback normalizados a la dirección del EXE (200A). | 197A, 198A, 200A, 202A | | Bajo |
| B9 | **Archivos recompilados con `/arch:IA32 /fp:fast` por el banco de un lote que afectan a funciones de otro lote**: `TaskComplexReactToAttack.cpp` (de 193A) obligaba a repetir `Fn199A` en el pase final (repetido el 2026-11-18, 491.520 pares sin diferencias, con 0x620B00 y 0x54F490 ya con hook); `TaskComplexGoToPointAndStandStillX87.cpp` (198A) y el archivo de `CTaskInteriorSitInChair` (204A) se compilan aparte. | 193A, 198A, 204A | `CTaskComplexReactToAttack::MakeAbortable/CreateSubTask/ControlSubTask/Clone` (199A) | Bajo (`Fn199A` repetido el 2026-11-18) |
| B10 | **Dos hooks sobre una misma función de la DLL**: Robust Hook escribe en el enlace de la función un salto a la dirección del EXE del último hook apagado, así que el banco debe poner el mismo falso en las dos direcciones. Con el hook encendido el comportamiento no se probó para ambas rutas por separado. | 189A (STL de `SArray`, ya separado en instancias propias), 191A (`PoolDestructorBody` y copia), 195A (`GoToDestructorBody` 0x667A00 y 0x667D50) | `CTaskSimpleGoTo` destructor | Bajo |
| B11 | **Fuentes distintas de las probadas**: el banco se ejecutó sobre un cherry-pick del lote en una base concreta (`origin/main` 9acc2209e0, 94bf1068ae, 7bd5ae8b77, 23938b22f1 o 70b6d35891) con las correcciones del banco; el árbol actual es posterior (merge de hooks.csv, `X87Math.*` y `InjectHooksMain.cpp` en varios lotes). Vale lo mismo que la sección 6.3 (vigencia acreditada: 0). | 189A–207A | `Fn200A` (marcador de conflicto en `X87Math.cpp`) | Medio |

Cada lote declara además su lista concreta en `scope_note.txt`; las más específicas (por ejemplo `Fn207A`: `FindNodePairClosestToCoors` sin candidatos con máxima mayor que 10000.0f; `Fn200A`: `eDoors` de 8 bits) están recogidas arriba.

---

## 10. Funciones saltadas o excluidas por no ser reconstruibles en C++ puro

La política del proyecto es C++ puro (sin `__asm` ni `naked`). Estas funciones no se pueden escribir sin ellos, o no son funciones. Listas completas y verificables:

- Código del juego: [docs/JuegoExe/excluidas.csv](JuegoExe/excluidas.csv): **118 filas, 28 587 bytes** (dirección, bytes, módulo, motivo y lote).
- RenderWare, D3D9, D3DX9 y QuickTime: [docs/RenderWareExe/no-reconstruibles.csv](RenderWareExe/no-reconstruibles.csv): **54 filas**.

### 10.1 Resumen por motivo

| Motivo | Juego | RenderWare/D3D | Ejemplos (lote) |
| --- | ---: | ---: | --- |
| `__usercall` o registros de entrada/salida que C++ no puede leer ni fijar (EAX/ECX/EDX/ESI/EDI/EBX, XMM0, MM0) | 69 | 10 | 0x4D1680, 0x4D1A50, 0x4D1DB0 (214A), 0x4D2100, 0x4D2450, 0x4D27F0, 0x4D6450, 0x4D6510, 0x4D66A0 (215A), 0x4CAAE0–0x4CC0C0 (224A), 0x4CC5C0, 0x4CCD70, 0x4CCE60 (225A), libjpeg jchuff/jcmarker/jdhuff/jdinput/jdmainct/jdmarker/jdmaster/jmemmgr (230A–235A), 0x5D84C0, 0x5D8590 (211A), 0x5A42B0 (203A), D3DX9 con XMM0 y `ret 10h` (R4A), 0x7CB828, 0x7CB8B0 (R6A) |
| Salto puro (`jmp` a otra dirección) o puente sin cuerpo propio | 20 | 37 | 0x48DE60 (214A), 0x4A2BB0 (230A), 0x5CFA30 (235A), 0x5CFE00 (236A); QuickTime/QTML: 17 puentes (R9A) y 15 (R10A) con el identificador en EAX y `jmp [0x8E2A0C]`; carga retardada de DLL 0x817508, 0x818020, 0x818041 (R8A), 0x807C10, 0x807C2B (R14A) |
| Bloque de protección con convención propia (`pushad`, bandera de acarreo, `esp` guardado) | 18 | 0 | 0x4027CD (221A) y las de 222A (0x402A36 … 0x403D07). En total el módulo `ExeProtectionStub` tiene 17 filas en el CSV; las que usan solo registros de entrada están contadas en la fila de `__usercall`. Las demás funciones del bloque sí se reconstruyeron (A-J11) |
| Funclet de `catch(...)` o fragmento interno de otra función (sin prólogo propio) | 11 | 3 | 0x4F44B3 (209A), 0x4F3A3E, 0x4F3AAD (209A), 0x4F3A15 (208A), 0x5ED0A3, 0x5ED388 (219A), 0x5ED643, 0x5ED92A, 0x5EDBE3 (220A); 0x7CCF6C (R2A/R7A), 0x7CB929 (R6A), 0x7F6781 (R25A; cola de una rutina D3D9, ya enganchada por `WindowedMode.cpp`) |
| 3DNow! (`pfrcp`, `pfrsqrt`: aproximaciones definidas por el hardware, no reproducibles bit a bit) | 0 | 2 | 0x7C20C9, 0x7C290B (R4A) |
| Ya enganchadas por `WindowedMode.cpp` (NOTSA, no se rehacen) | 0 | 2 | 0x7F7730, 0x7F8F20 (R26A, R27A) |
| **Total** | **118** | **54** | |

(Cifras del juego revisadas en el lote G2A con la columna `categoria` del CSV: 69 `__usercall` = 51 cubiertas por llamadoras validadas + 17 de RenderWare (rwtexdict y D3D9 raster) + 1 sustituida por el CRT (0x746480, lote G3A, ver A-J12); 20 saltos puros; 18 de la protección del EXE (las 17 de `ExeProtectionStub` y 0x746800); 11 que no son funciones. Evidencia por función en [reversing/FnG2A](../reversing/FnG2A/README.md). La columna de RenderWare/D3D sigue agrupada por texto del motivo.)

### 10.2 Reconstruidas como ayudante local sin hook (el EXE conserva la copia)

Estas no figuran en `excluidas.csv` como "no hechas" pero **no tienen hook propio**: la versión C++ solo se usa desde llamadores ya reconstruidos y la del EXE sigue activa para el resto (A-G6). Sin banco.

| Dirección | Función | Lote | Llamador reconstruido |
| --- | --- | --- | --- |
| 0x5C6430, 0x5C6600, 0x5C6680 | `InitialSetup`, `SelectScanParameters`, `PerScanSetup` (libjpeg, compresión) | 231A | `JinitCMasterControl`, `PrepareForPass` |
| 0x5C7010, 0x5C70D0, 0x5C8620 | `add_huff_table`, `std_huff_tables`, `output_pass_setup` | 232A | `jpeg_set_defaults`, `jpeg_start_decompress` |
| 0x5CA2A0, 0x5CA490, 0x5CA630, 0x5CA890 | `initial_setup`, `per_scan_setup`, `latch_quant_tables`, `alloc_funny_pointers` | 233A | `consume_markers`, `start_input_pass`, `jinit_d_main_controller` |
| 0x5CC950, 0x5CC9D0 | `prepare_range_limit_table`, `master_selection` | 234A | `jinit_master_decompress` |
| 0x5CFE20 | `default_decompress_parms` | 236A | `jpeg_consume_input` |
| 0x4D1680, 0x4D1A50, 0x4D1DB0, 0x4D2100, 0x4D2450, 0x4D27F0 | `FrameUpdateCallBack*` de RpAnimBlend (plantilla `FrameUpdateCallBackExeVelocity<...>`) | 214A, 215A | los callbacks enganchados |
| 0x4D6450, 0x4D6510, 0x4D66A0 | `RpAnimBlendClumpFillFrameArray` (rama skinned), `RpAnimBlendClumpInitSkinned`, `...NonSkinned` | 215A | 0x4D64A0, 0x4D6720 |
| 0x4CB560, 0x4CB730 | `RemoveRasterFromList`, `PushFreePaletteIndex` | 224A | 0x4CBB00 |
| 0x5D84C0, 0x5D8590 | `CalcEnvCamUV`, `CalcEnvWaveUV` | 211A | `CustomPipeRenderCB` 0x5D9900 |
| 0x72E080 … 0x72E360 | geometrías de clump en una fase | 208A (y 95C) | `RpClumpGtaStreamReadAtOnce` 0x72E3A0 |

---

## 11. Resueltas

### 11.1 Defectos reales que encontraron los bancos 189A–207A y se corrigieron en la fuente

Dejan de ser desviaciones: tras el arreglo la función pasó 16384 pares por función con cero diferencias (el código previo al arreglo **sí** difería del EXE). Fuente: `reversing/Fn<lote>/README.md`, secciones de correcciones. Direcciones tomadas del README o de `docs/hooks.csv`.

| Lote | Función | Dirección | Defecto corregido |
| --- | --- | --- | --- |
| 189A | `CStreaming::GetDefaultAmbulanceModel`, `GetDefaultFireEngineModel` | 0x407D30, 0x407DC0 | Leían solo el byte bajo de 0xBA6718; el EXE lee el DWORD |
| 189A | `ProcessEntitiesInSectorList` | 0x40C450 | `GetInfo` con `assert` y `operator[]` comprobado con índices negativos; `GetRwObject()` desreferenciaba el objeto RenderWare; comparación de `IsInCurrentArea` sin extensión con ceros |
| 191A | `CTaskSimpleSlideToCoord` constructor con animación | 0x66C450 | `m_pAnimHierarchy` (+0x3C) solo se asignaba con bloque; el EXE lo escribe siempre |
| 191A | `RsSetPresetView` | 0x619780 | Negación de `rotX` con SSE conservaba el NaN señalizante; el EXE lo silencia (`fld; fchs; fstp`) |
| 192A | `CObject` cuerpo del destructor | 0x59F660 | Comparación de `m_nModelIndex` con `MI_TRAINCROSSING1` sin la extensión con signo / sin signo del EXE |
| 193A | `CTaskComplexReactToAttack::CreateFirstSubTask` | 0x625C70 | Denormal aplanado por FTZ del SSE; archivo pasado a `/arch:IA32 /fp:fast` |
| 193A | `CTaskComplexExtinguishFireOnFoot::CreateNextSubTask` / `CreateFirstSubTask` | V (dirección 0 en el CSV) | Constructores no enganchados; ahora los cuerpos enganchados y su EAX |
| 194A | `CTaskComplexWaitForPed::ControlSubTask` | 0x6834E0 | Comparación de distancias en `float` (SSE) frente a la x87 con exponente ampliado; resta de vectores con denormales aplanados (75 de 16384 pares) |
| 194A | `CTaskComplexWander::GetDistSqOfClosestPathNodeToPed`, `GetWanderTaskByPedTypeId`; `CPedModelInfo` cuerpo del destructor | 0x669FF0, 0x673EF0, 0x4C62F0 | Preventivo o de enganche: aritmética a `X87Math`; constructores y destructor llamados por sus cuerpos enganchados |
| 196A | `CEventHealthLow::Constructor`, `CEventSeenCop::CloneEditable`, `CVehicle` destructor con borrado | 0x5FF440, 0x5FF3C0, 0x6E38A0 | Vtable de la DLL y cadena de constructores/destructor sin pasar por el cuerpo enganchado |
| 196A | `Input::InputShutdown` | 0x747000 | El hook estaba en `WinInput::InjectHooks`, que no se ejecuta con SDL3: **no se registraba**; pasado a `InjectPadHooks` |
| 196A | `CGangWarsSaveStructure::Construct` / `Extract` | 0x5D2620, 0x5D2740 | `std::array::operator[]` con índices fuera de la tabla de 380 disparaba asserts que el EXE no tiene |
| 197A | `CTaskComplexSunbathe::CreateSubTask` | 0x638290 | Subtareas con `new` de C++ en vez de los constructores enganchados y su EAX |
| 198A | `CTaskComplexWaitAtAttractor` y `CTaskComplexUseAttractor` destructores con borrado; `CTaskComplexPresentIDToCop` (clase entera); `CTaskComplexGoToPointAndStandStill::CreateSubTask` y `SelectMoveState` | 0x638590, 0x6385B0, 0x6591D0 y vecinas, 0x6682D0, 0x668250 | Constructores y destructores generados en vez de los cuerpos enganchados; x87: `dx*dx` de 3e38 desbordaba a infinito en `float` (24 pares de `SelectMoveState`) |
| 199A | `CCamera` cuerpo del destructor | 0x50A870 | Orden de la puesta a cero de +0x14 (el informe previo decía lo contrario) |
| 199A | `CTaskComplexReactToAttack::CreateSubTask` | 0x620C30 | Subtareas con `new T` y conversión de daño con `static_cast<int32>(float)` (daba 0x80000000 con \|x\| >= 2^31; el `_ftol` del EXE es de 64 bits) |
| 199A | `CPed` destructor con borrado | 0x5E8C80 | Llamaba a `~CPed()` directamente |
| 200A | `CTrain::OpenDoor` | 0x6F6370 | Suma de la posición guardada con SSE aplanaba denormales (621 pares); NaN señalizante de ángulo de puerta copiado sin silenciar (196 pares) |
| 201A | `CTaskComplexFollowLeaderInFormation::CreateSubTask`; `CEventInWater::Clone` | 0x6962A0, 0x4B7810 | Constructores directos y `operator-` en vez de `VectorSubtract`; NaN señalizante de la aceleración sin silenciar |
| 202A | `TASK_SIMPLE_ANIM_LOOPED_MIDDLE::ProcessPed` | 0x61BCB0 | Con tiempos denormales la DLL daba 0 (FTZ); dos ayudantes x87 |
| 203A | `COctTree` (`ReleaseChildren`, destructor con borrado, `CPool::Delete`) y `CVehicleStructure` (`operator delete`, destructor con borrado) | 0x5A73C0, 0x4C9580, 0x4C9840 | `CPool::Delete` de la DLL comprobaba con `assert` lo que el EXE no comprueba; nuevo `Core/PoolRaw.h` |
| 204A | `CPedAcquaintanceScanner::CreateAcquaintanceEvent` | 0x606BA0 | Rama de odio en grupo: `GetPedsGroup(entidad)` en vez de `GetPedsGroup(ped)`: el evento iba **al grupo equivocado** (diferencia de comportamiento real) |
| 204A | `CTaskInteriorSitInChair::ProcessPed` | 0x676D30 | Intermedios en `float` desbordaban con FLT_MAX; copias de vectores con NaN señalizante pasadas por la x87 |
| 205A | `CPedGeometryAnalyser::ComputeRouteRoundSphere`, `ComputeSphere` | 0x5F1890 | Copia de `center.z` como dword y suma parcial guardada como float |
| 206A | `CPedAttractor` destructor con borrado | 0x5EC4B0 | Llamaba al destructor de C++ en vez de al cuerpo enganchado 0x5EC410 |
| 206A | `SArray` vaciado (`SArrayTidyStride4`) | 0x5EC820 | `clear()` recorría el rango con `std::destroy`; con el fin anterior al inicio **hundía el juego** (0xC0000409) |
| 206A | `CTaskComplexUseMobilePhone::CreateNextSubTask` | 0x634A60 | Constructores de C++ en vez de los cuerpos enganchados 0x61A900 y 0x48E750 |
| 207A | `CPathFind::RecordNodesInCircle` | 0x44FB60 | Usaba el argumento 8 para saltar nodos marcados **y** para el bit de agua (el EXE usa el 9 para el agua): defecto real |
| 207A | Barrido y sonda de `CCarCtrl` | 0x421A50, 0x422E10 | Geometría de esquinas y de inicio de rayo con SSE (FTZ y otro NaN); ayudantes x87 |
| 207A | `CTaskComplexInWater` destructor con borrado; `CTaskComplexUseMobilePhone::CreateFirstSubTask`/`ControlSubTask` | 0x635100 (cuerpo), V | Destruía una subtarea cruda; constructor de `CTaskSimpleRunAnim` por el cuerpo enganchado |

Cuenta: 30 filas. No se listan los arreglos de enganche sin cambio de comportamiento (destructores con borrado de `CTaskSimpleChoking`, `CTaskSimpleBeHit` y `CTaskComplexKillPedOnFootStealth`/`Melee` en 193A; `CTaskComplexKillPedGroupOnFoot::Destructor` 0x6222B0 en 190A; `CEventAcquaintancePedHate::CloneEditable` 0x420F50 y `CTaskComplexKillAllThreats::CreateNextSubTask/CreateFirstSubTask` en 190A, que ahora pasan por los constructores enganchados): véase `Fn190A` y `Fn193A`.

### 11.2 Entradas del documento del 08/11 que ya no están abiertas

| Antes | Estado actual | Evidencia |
| --- | --- | --- |
| 3.3: `GetMouseSetUp` 0x53F2D0 sin fila | Fila `Input,GetMouseSetUpExe`, `validated=1` | `docs/hooks.csv` |
| 3.3: `CMatrix::Detach` 0x59ACF0, `CopyOnlyMatrix` 0x59ADD0, `Update` 0x59BB60, `UpdateMatrix` 0x59AD20, `Scale` 0x459350, `operator+=` 0x59ADF0 "código muerto" | Instaladas por `InjectHooksLote162A`, banco `Fn162A`, filas con `validated=1` | `Matrix.cpp:43,92-99`, `hooks.csv` |
| 3.3: `CMatrix::ConvertToEulerAngles` 0x59A840 y `ConvertFromEulerAngles` 0x59AA40 sin fila | Filas con `validated=1`; el banco `Fn202A` las ejerce | `hooks.csv`, `Fn202A` |
| 3.3: `CVector::Sum` 0x40FDD0 | Instalada por `InjectHooksExactOperators`, lote 153A, banco `Fn153A`; fila `validated=1` | `Vector.cpp:83,174` |
| 3.3: `CVector2D::operator=` 0x43E110 | Instalada por `InjectHooksLote162A`, banco `Fn162A`; fila `validated=1` | `Vector2D.cpp:36-45` |
| 7.1: 0x5DE920 (`CAEPedAudioEntity`) y 0x563130 (`CSector`) destructores saltados en `Fn130A` | Reconstruidos y validados en `Fn131A` (65536 pares, cero diferencias) | `Fn131A` |
| 7.1: 0x747300 `getDvdGamePath` "sin hook" | Instalada (`WinMain.cpp:559`); fila `Win,getDvdGamePath`, `validated=1` | `hooks.csv` |
| 7.2: 22 envolturas pendientes (`CWeaponModelInfo` 0x4CC970, 0x4CD250 y 20 de `Win`) | Hoy 1 envoltura (0x4F39C0, 208A); las 22 eran código de RenderWare y D3D9 y están en C++ | lotes 224A, 225A, 226A–229A; `hooks.csv` |
| 7.2: 2406 funciones del EXE sin reemplazar | 2 en el mapa (las dos de 208A, ya enganchadas) más las no reconstruibles de la sección 10 | `docs/JuegoExe/funciones.csv` |
| 7.2: sección "Omisiones concretas" de `BinaryCoverageAudit.md` desfasada | Auditoría archivada en `docs/archivo/BinaryCoverageAudit-2026-10-10.md`; `docs/BinaryCoverageAudit.md` es solo un aviso | `docs/archivo/` |

Cuenta: 10 filas.

## 12. Hallazgos de riesgo medio y alto

| # | Hallazgo | Riesgo | Sección |
| ---: | --- | --- | --- |
| 1 | 1282 funciones (lotes 208A–239A y R1A–R34A) integradas **sin ningún banco**, revisadas solo contra el desensamblado; 28 desviaciones o límites declarados en los informes (cuelgues del EXE sustituidos por NULL, variables sin inicializar puestas a 0, ramas omitidas, `float` en lugar de x87) | Medio (no medido) | 8 |
| 2 | Aritmética `float` con SSE sin x87 en RenderWare (frustum, propiedades de superficie): la clase de diferencia (denormales, desbordes) que los bancos 193A–207A encontraron y corrigieron en código del juego | Medio | 8.3 (A-R12) |
| 3 | Límites de los bancos 189A–207A: colaboradores falseados, objetos reales construidos por el EXE, pools con asserts, NaN y rangos extremos, memoria agotada, archivos recompilados que obligan a repetir bancos (`Fn199A`) | Medio | 9 |
| 4 | 118 funciones del juego y 54 de RenderWare/D3D no reconstruibles en C++ puro; 237 puntos `plugin::Call*`; 1111 funciones de bibliotecas de terceros se enlazan | Alto (independencia) | 10, 7.2 |
| 5 | La evidencia es histórica: 7253 de 9493 entradas con fuentes distintas de las probadas (corte 08/11), vigencia declarada 0; los bancos 189A–207A no están en el registro | Medio | 6.3 |
| 6 | 7761 entradas validadas con colaboradores falseados; integración real no cubierta | Medio | 5 |
| 7 | `FIX_BUGS` definido: 58 apariciones activas, no idénticas al EXE (p. ej. `Collision.cpp:1493`, menús, `ProcessEntityCollision` de coche y moto) | Medio | 2.2 |
| 8 | `CCamera::CamControl` (dos rutas) y `CPlane::VehicleDamage` explosivo a humo: comportamiento definido distinto del EXE (pila sin inicializar), no certificado | Bajo a medio | 2.1 |
| 9 | `NOTSA_UNREACHABLE` en 139 sitios, `FxInfoManager.cpp:129-131` con tres casos comentados, `IplStore.cpp:817` condicional invertido sin verificar | Medio | 2.3 |
| 10 | Modo ventana NOTSA: `_rwD3D9CameraClear` rompe el suelo reflectante de la casa de CJ; copia local de `D3D9SetPresentParameters` que no es la del EXE | Medio | 8.3 (A-R16) |
| 11 | 8 hooks activos o preparados sin fila en `hooks.csv` (WinInput, `Win32_Init*`, `CPad::GetMouseState`) y fila `FinishAnimCB` con tipo desactualizado | Bajo | 3.3 |
| 12 | Aritmética x87 frente a SSE en el resto del código: solo ~10 ficheros compilados con `/arch:IA32 /fp:fast` más los que añadieron los lotes 193A, 198A y 204A | Medio (no medido) | 2.5, 9 (B9) |
| 13 | Equivalencias de banco aceptadas (vtable de `CTask`, EAX/ECX/EDX informativos, relleno enmascarado) | Bajo a medio | 4 |
| 14 | 1075 entradas excluyen dominios donde el EXE falla; el C++ añade comprobaciones en ellos sin comparar (08/11) | Bajo a medio | 6.1 |
| 15 | 76 `ScalarDeletingDestructor` y 338 `Destructor` catalogados como S, no V (08/11) | Bajo | 3.1 |
| 16 | Banco de regresión final detenido (54 de 148 y 74 de 177 en cortes fechados) | Medio (fechado) | 6.3 |

## 13. Cómo actualizar este documento

1. Recontar con los mismos criterios: `python` sobre `reversing/validation-registry/parte-*.json` (campos `hook`, `scope`, `tested_sources`, `dependency_review`, `evidence.pairs`) para las secciones 1, 5, 6.1 y 6.2; recuento de `docs/hooks.csv` por `validated` y `type` para la sección 1.
2. Volver a comparar hashes de `tested_sources` con el árbol tras cada lote; las cifras de la sección 6.3 cambian con cada commit.
3. Regenerar el cruce de `RH_Scoped*Install` frente a `hooks.csv` (sección 3.3) tras añadir lotes.
4. Revisar `source/CMakeLists.txt` (línea 479) por si `FIX_BUGS` deja de definirse, lo que cambiaría la sección 2.2.
5. Al validar un lote de `reversing/PendienteValidar`, mover sus filas de la sección 8 a la 11.1 (si el banco corrigió algo) o borrarlas (si el banco confirma la desviación como equivalente), y pasar sus límites a la sección 9.
6. Al integrar un lote nuevo, leer en su informe las secciones de saltadas, riesgos y "divergencia", añadir las desviaciones a la sección 8 y las funciones no reconstruibles a `docs/JuegoExe/excluidas.csv` o `docs/RenderWareExe/no-reconstruibles.csv` (sección 10).

## 14. Revisión del 24/11/2026 (ejecutable independiente y sustituciones)

Esta sección se añadió el 24/11/2026 sin reescribir el resto: las secciones 1 a 13 conservan las cifras y el análisis del 16/11 (cada cifra lleva su fecha). Fuente de cada fila: el documento de ejecución citado. El riesgo sigue siendo una estimación de quien redacta, no una medida.

### 14.1 Cifras recontadas el 24/11/2026

| Medida | Valor | Cómo se midió |
| --- | ---: | --- |
| Filas de `docs/hooks.csv` | 13.511 en 1.007 grupos (tipo S 11.355, tipo V 2.156); todas `reversed=1`, 1 con `locked=1` | `contrib/sync_docs.py` y recuento directo del CSV |
| `validated=1` / `validated=0` | 11.800 / 1.711 (1.709 S y 2 V sin validar) | CSV tras `contrib/sync_validated_flags.py`; evidencia aprobada del registro: 11.800 |
| Vigencia acreditada | 0 | `contrib/validation_registry.py` (`current`) |
| Región del juego (0x401000-0x74FFFF) | 11.684 funciones, 11.684 reemplazadas; 150 excluidas | `contrib/game_function_map.py` |
| Región RenderWare/D3D (0x750000-0x81FFFF) | 1.888 funciones: 795 reemplazadas, 1.032 de libpng, zlib y D3DX9 (se enlaza la original), 61 no reconstruibles o sustituidas (7 de ellas, QuickTime y DirectDraw, nuevas), 0 envolturas pendientes | `contrib/rw_function_map.py` |
| Comandos de script | 2.826: 669 reescritos, 280 nop, 713 no implementados en el juego, 1.164 sin manejador propio | `contrib/inventario_comandos_script.py` |
| `__asm` o `naked` en `source/game_sa` | 0 apariciones léxicas fuera de comentarios y cadenas | `contrib/audit_cpp_assembly.py` |
| `FIX_BUGS` o `notsa::IsFixBugs` en `source/` | 58 apariciones en 31 archivos | `grep` |
| `NOTSA_UNREACHABLE` en `source/game_sa` | 141 | `grep` |
| `plugin::Call` en `source/game_sa` | 32 apariciones en 22 archivos (el 16/11 la tabla decía 237 en 51; no se comparó el método de recuento) | `grep` |

### 14.2 Desviaciones nuevas o cambiadas del 22 al 24/11

| # | Qué difiere del EXE | Riesgo (estimado) | Fuente |
| ---: | --- | --- | --- |
| 1 | `ExeCrt::Pow`: la rama x87 (`fyl2x`, `f2xm1`, `fscale`) no es reconstruible bit a bit en C++ puro y se sustituye por el núcleo SSE2: mismos dominios y errores, resultado correcto pero no idéntico al de la x87; no se midió la diferencia máxima | bajo | [ejecucion-sustitucion-crt-rw-x87.md](CodigoVivoExe/ejecucion-sustitucion-crt-rw-x87.md) |
| 2 | `ExeCrt::Floor` y `Ceil` sin SSE2: no reproducen la excepción de operación inválida con la x87 sin enmascarar y un NaN señalizador (con las excepciones enmascaradas el resultado coincide en 3 millones de casos) | bajo | ídem |
| 3 | Montón del CRT: se quitó el camino por dirección al EXE para `__active_heap != 1` (montón de bloques pequeños de Windows 9x), que `ExeCrt` no reproduce y el juego no alcanza | bajo | ídem |
| 4 | Pistas de usuario: `CAEMFDecoder` decodifica con Media Foundation; el PCM coincide con QuickTime a ±1 LSB (no bit a bit), el tiempo de reproducción cuenta fotogramas ya decodificados (el original, de origen leídos) y la duración de un MP3 CBR sin cabecera Xing es una estimación por tamaño (hasta 233 ms de diferencia en la prueba). `.aac`, `.m4a` y `.mov` sin probar | medio para esos formatos, bajo para MP3 | [sustitucion-quicktime.md](QuickTime/sustitucion-quicktime.md) |
| 5 | DirectDraw a D3D9: memoria de vídeo total y libre salen del mismo valor de `GetAvailableTextureMem` (el total difiere un 0,02 % en esta máquina) y el juego ya no comprueba que `DDRAW.DLL` exista | bajo | [directdraw-sustitucion.md](RenderWareExe/directdraw-sustitucion.md) |
| 6 | Lista de modos de vídeo: 52 entradas en el original y 35 en el independiente y el ASI (faltan los R5G6B5); causa no investigada | bajo | ídem |
| 7 | El espejo del ropero depende del controlador: el original sale oscuro con la NVIDIA por una combinación de render target sin MSAA y z-buffer con 4x MSAA que D3D9 no define; el independiente exporta `NvOptimusEnablement` para usar la misma GPU. En otra GPU el original también se vería como el independiente sobre Intel | bajo | [ejecucion-espejo-ropero.md](CodigoVivoExe/ejecucion-espejo-ropero.md) |
| 8 | Carga de `gGameState` 8 a 9: 1,9-2,2 s en el independiente frente a 11,5 s en el original y 8,7 s en el ASI, con el mismo estado de streaming; el mecanismo del ahorro no se identificó | bajo | [ejecucion-lluvia-gamma-carga.md](CodigoVivoExe/ejecucion-lluvia-gamma-carga.md) |
| 9 | Peatones: el independiente genera 28-34 frente a 6-9 del control con el ASI en los mismos puntos; sin explicar | bajo | [ejecucion-trafico-nubes.md](CodigoVivoExe/ejecucion-trafico-nubes.md) |
| 10 | Objetos propios de `DatosExe` declarados y sin lector por nombre que solapan datos reales del EXE (trampas latentes: `bBAD3EC`, los de la sección 3 de la auditoría G5a, 14 de `DE04R`): hoy nadie los lee | bajo | [auditoria-g5a.md](DatosExe/auditoria-g5a.md), [ejecucion-datos-g2g4.md](DatosExe/ejecucion-datos-g2g4.md) |
| 11 | Datos no ejercitados en el independiente: entre unos 100 y 290 objetos de G5a y 244 de G5b nunca cambiaron en las ejecuciones; solo cuenta el análisis estático | medio | [auditoria-g5a.md](DatosExe/auditoria-g5a.md), [auditoria-g5b.md](DatosExe/auditoria-g5b.md) |

### 14.3 Resueltas en esos días (dato en dos sitios y otros)

- `GenerateOneRandomCar` y `ChooseModel` leían por dirección datos que el C++ guardaba en objetos propios: sin tráfico aleatorio en el independiente ([ejecucion-trafico-nubes.md](CodigoVivoExe/ejecucion-trafico-nubes.md)).
- 11 sitios más de la misma clase en trucos, cámaras, helicóptero, mira, radio y luces de niebla ([auditoria-datos.md](CodigoVivoExe/auditoria-datos.md)); sus efectos están deducidos del código, no medidos en ejecución.
- `MI_HYDRAULICS` y `m_bWaterFogScript` ([ejecucion-datos-g2g4.md](DatosExe/ejecucion-datos-g2g4.md)); fundidos del HUD, lista libre de muebles de interiores y ruta de scripts ([auditoria-g5a.md](DatosExe/auditoria-g5a.md)); combos de melee ([auditoria-g5b.md](DatosExe/auditoria-g5b.md)).
- La semilla de `rand` del EXE no se sembraba en `GameInit`; ahora se siembran las dos (`ExeCrt::Srand` y `srand`); corregido por lectura del desensamblado, sin medir `holdrand` en ejecución ([ejecucion-datos-g6g8.md](DatosExe/ejecucion-datos-g6g8.md)).
- Entrar en el gimnasio de Ganton caía por pasar `-1` a `TASK_KILL_CHAR_ON_FOOT` y `_WHILE_DUCKING` como `CPed&`; ahora reciben el manejador entero (commit `96f1c36f61`; evidencia en `reversing/FnJuego/evidencia/`, sin informe escrito en `main`).
- El espejo del ropero (diferencia con el original por la GPU) y el ratón del independiente (SDL3 sustituía el registro de entrada en bruto de DirectInput; [ejecucion-raton.md](CodigoVivoExe/ejecucion-raton.md)).

## 15. Correcciones opcionales de SilentPatch (desviaciones deliberadas, 24-25/11/2026)

Interruptor general `GTASA_SILENTPATCH=0|1` (o `SilentPatch=` en `gta-reversed.ini`; módulo `source/extensions/SilentPatchConfig.*`); por defecto encendido; con él apagado el comportamiento es el del EXE. Licencia y atribución: [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Esquema único de nombres: `GTASA_SILENTPATCH=0|1` / `SilentPatch=0|1`; por opción `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0|1` / `SilentPatch_<Nombre>=0|1` (cabecera de `SilentPatchConfig.h`). Está dividida en tres subsecciones (15.1 vehículos/peds/RenderWare, 15.2 gráficos/tiempo/streaming, 15.3 scripts/entrada/HUD/guardado); los arreglos que dos ramas portaron por separado quedaron fusionados en una sola implementación (ver notas).

### 15.1 RtAnim, vehículos, peds, jugador, fuego y pools

| # | Función (EXE) | Desviación | Opción propia | Riesgo |
| --- | --- | --- | --- | --- |
| 1 | `RtAnimInterpolatorSetCurrentAnim` (0x7CD5A0, hook existente) | Con el esquema RpAnimBlend no escribe los punteros `keyFrame1`/`keyFrame2` sobre `q.x`/`q.y` de cada fotograma interpolado (el EXE los escribe tras inicializar el fotograma a identidad con 0x4D60C0). Leídos como float, un puntero >= 0x3F800000 (por encima de ~1 GB; el exe es LARGE ADDRESS AWARE) vale 1 a 3e38 y los nodos que el blend no reescribe quedan con un cuaternión enorme: huesos deformados durante un fotograma. Es el «streaming memory bug» de SilentPatch. Nadie lee esas cabeceras en RpAnimBlend (no usa `AddAnimTime`). | `GTASA_SILENTPATCH_RTANIMINTERP=0` o `SilentPatch_RtAnimInterp=0` | bajo |
| 2 | CCheat::WeaponCheat2/3, NinjaCheat, ParachuteCheat | El cuchillo, la motosierra, la katana y el paracaídas de los trucos se dan con 1 de munición (el EXE: 0) para que un pickup no los sustituya | `SilentPatch_MeleeCheatAmmo=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 3 | CWeapon::TargetWeaponRangeMultiplier (0x73B380) | Un peatón a pie con `m_pVehicle` obsoleto ya no cuenta como «en coche» (el EXE multiplicaba el alcance x3 y la IA era demasiado precisa) | `SilentPatch_AIAccuracyFix=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 4 | CAutomobile::VehicleDamage (Securicar) | Se quita el x7 de daño de los Securicar cuando los golpea el jugador (resto de GTA III) | `SilentPatch_SecuricarDamage=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 5 | CGarages::StoreCarInNearestImpoundingGarage; CVehicle::~CVehicle; CCarCtrl::PossiblyRemoveVehicle | El impound solo guarda coches y motos; el vehículo destruido sale de apCarsToKeep; los vehículos se conservan cerca de cualquier garaje (no solo de los escondites) | `SilentPatch_ImpoundOnlyCarsAndBikes=0`, `SilentPatch_ImpoundRandomVehiclesFix=0`, `SilentPatch_ImpoundGaragesFix=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | medio (cambia qué vehículos se guardan) |
| 6 | CTaskSimpleUseGun::FireGun (0x61EB10) | Muzzle flash también con la última bala del cargador (se usa el resultado de Fire en lugar del estado FIRING) | `SilentPatch_LastBulletMuzzleFlash=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 7 | CFire::ProcessFire (bici ardiendo) | Una BMX en llamas prende a su conductor, no a CJ | `SilentPatch_BicycleFire=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 8 | CTaskComplexEnterCar::CreateSubTask; CVehicle::RemoveDriver; CTheScripts::RemoveThisPed; CTaskSimpleCarSetPedOut::ProcessPed; CTaskComplexCarSlowBeDraggedOut::CreateFirstSubTask | Robar el coche por el lado del pasajero ya no mata al conductor vivo; un coche destrozado ya no pasa a abandonado (explotaba dos veces) | `SilentPatch_StealCarFromPassengerSide=0`, `SilentPatch_VehicleExplodeTwice=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 9 | CAutomobile::NitrousControl (0x6A3EA0) | El nitro no se recarga más rápido al ir marcha atrás | `SilentPatch_NitrousReverseRecharge=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 10 | CTaskSimpleUseGun::SetMoveAnim; CTaskSimpleGunControl | Los policías ya no usan la postura de pistola de banda; la usan los dealers y los criminales | `SilentPatch_CopsGunStance=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo (cambia animación) |
| 11 | CRunningScript GENERATE_RANDOM_FLOAT / GENERATE_RANDOM_INT | El azar de los scripts es de 16 bits en lugar de 15 (rango 0-1 completo del float aleatorio) | `SilentPatch_Rand16bit=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | medio (cambia la secuencia aleatoria de las misiones) |
| 12 | CVehicle::GetVehicleLightsStatus (0x6D55C0) | Umbral de luces de tráfico 1/25000 (probabilidades de PS2) | `SilentPatch_LightsRandomness=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 13 | ::ChooseComponent (0x4C7FB0) | La regla de extras 4 (cualquiera) puede elegir también el extra 6 | `SilentPatch_Extra6Rule=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 14 | CFileLoader::LoadCarGenerator; CCheat | El Beagle mal colocado en Fort Carson (coordenadas exactas) pasa a ser una BMX; SEAROADER activa/desactiva «coches sobre el agua» | `SilentPatch_FortCarsonBeagle=0`, `SilentPatch_SeaRoaderCheat=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | medio (cambia contenido) |
| 15 | CRunningScript::GetCorrectPedModelIndexForEmergencyServiceType; CCarAI::BackToCruisingIfNoWantedLevel | LAPDM1 pedido con PEDTYPE_COP genera un motorista; los motoristas cancelan el drive-by al perder el nivel de búsqueda | `SilentPatch_BikerCopSpawn=0`, `SilentPatch_BikerCopsDriveByFix=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 16 | CPlane::FindPlaneCreationCoors; CStreaming::Init2; CGame::ReInitGameObjectVariables | La comprobación de colisión de la creación de aviones usa sectores; la lista de entidades en streaming pasa de 1000 a 1250 enlaces; se reinician variables de New Game (respray gratis, temporizadores de ambulancia/bomberos, contadores de stats, penalizaciones; implementado en `SilentPatchNewGame::BeforeReInit`, ver 15.3, con la misma opción) | `SilentPatch_PlaneCreationCoorsFix=0`, `SilentPatch_StreamedEntityList=0`, `SilentPatch_ResetVariablesOnNewGame=0` (o `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0`) | bajo |
| 17 | CPedDamageResponseCalculator::ComputeWillKillPed (0x4B3210) | Con el truco de invencibilidad, las balas del helicóptero de policía no hieren a CJ | `SilentPatch_TrueInvincibility=1` o `GTASA_SILENTPATCH_TRUEINVINCIBILITY=1` para ACTIVARLA (apagada por defecto, como en SilentPatchSA.ini) | bajo |

Verificación (24-25/11/2026, `GTASA_SILENTPATCH_SELFTEST=1`, log `logs/silentpatch_selftest.log`): con un `pFrames` ficticio 0x5A000000 el EXE (interruptor apagado) deja `q.x`/`q.y` = 9,007e15 y 9,008e15 en los 32 nodos; con el arreglo todos los fotogramas quedan en identidad exacta. Ver las cifras de partidas en `reversing/FnSkin/ejecuciones/sp1_*`. **Límite y causa real de los 0xCCCCCCCC (hallada con un guardián de escritura por hardware, 25/11/2026):** esta corrección no tocaba las palabras 0xCCCCCCCC de los nodos 6 y 7. Las escribía `CBike::ProcessRiderAnims` (jitter de la cabeza del motorista a velocidad > 0,3): la reconstrucción llamaba a `RwMatrixRotate` sobre el `KeyFrame` de la cabeza, pero la dirección 0x7EB7C0 del EXE es `RtQuatRotate` (gira `q`). `RwMatrixRotate` copiaba 64 bytes (con los rellenos 0xCC de un temporal) sobre los nodos 5-7. Corregido con `RtQuatRotate(&keyFrame->q, ...)`: 0 anomalías en 42.500 comprobaciones (antes ~4.000 apariciones de 0xCC en 4 min con motos). Es un defecto de la reconstrucción, no una desviación del EXE.

### 15.2 Gráficos, tiempo y streaming (24/11/2026)

Interruptor general `GTASA_SILENTPATCH=0|1` (o `SilentPatch=` en `gta-reversed.ini`; `source/extensions/SilentPatchConfig.*`); con él apagado todo es como el EXE. Licencia: [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Opción propia: `GTASA_SILENTPATCH_<NOMBRE>=0|1` o `SilentPatch_<Nombre>=0|1` (se leen una vez por proceso). Detalle: [silentpatch-graficos.md](CodigoVivoExe/silentpatch-graficos.md).

| # | Función | Desviación | Opción | Riesgo |
| --- | --- | --- | --- | --- |
| 1 | `CCoronas::Render` | Las coronas giran al acercar la cámara (recipz sin x20, ángulo = 20/z) | `CoronaRotation` | bajo |
| 2 | `CCoronas::DoSunAndMoon` | Sol más grande: distancia de far clip limitada a 600 (en el EXE solo con timecyc/seabed estándar) | `SunSizeHack` | bajo |
| 3 | `CShadows::RenderStaticShadows` / `RenderStoredShadows` | Prueba alfa desactivada (ALWAYS) al pintarlas, estado restaurado al salir | `StaticShadowAlphaFix` | bajo |
| 4 | `CTheScripts::DrawScriptSpritesAndRectangles` | Filtro bilineal en los sprites de script (estado restaurado) | `ScriptSpritesBilinear` | bajo |
| 5 | `Idle` (0x53E920) | Sin la espera de 14 ms entre fotogramas: 30 FPS reales en vez de 25 con el limitador | `NoFrameDelay` | medio (cambia el ritmo del juego respecto al original) |
| 6 | `CTimeCycle::Initialise` | Multiplicador direccional por defecto 1.0 y relectura de la línea rota de RAINY_COUNTRYSIDE | `TimecycMissingDataFix` | bajo |
| 7 | `CStreaming::Init` | Lista de entidades en streaming de 1000 a 1250 enlaces | `StreamedEntityList` (misma corrección que la fila 16 de 15.1; la rama de gráficos la llamaba `BiggerStreamedEntityList`; una sola implementación en `CStreaming::Init2`) | bajo |

### 15.3 Scripts, misiones, entrada, HUD, trucos, guardado y textos (rama `silentpatch-scm`)

Opcionales: todos se apagan con `GTASA_SILENTPATCH=0` (o `SilentPatch=0` en `gta-reversed.ini`) y cada uno con `GTASA_SILENTPATCH_<OPCION>=0` o `SilentPatch_<Opcion>=0` (esquema único, ver arriba). Detalle y estado de ejercitación en [CodigoVivoExe/silentpatch-scm.md](CodigoVivoExe/silentpatch-scm.md).

| Arreglo | Opción | Efecto frente al EXE original |
| --- | --- | --- |
| Gimnasio, baloncesto, Mountain Cloud Boys, Sweet's Girl, conos de las autoescuelas, Air Raid, Ice Cold Killa, Supply Lines, Beefy Baron, Quadruple Stunt Bonus | `EnableScriptFixes` | Se modifica el bytecode del SCM en memoria y `GET_CURRENT_DATE` de los scripts gym* |
| CREATE_BIRDS con coordenadas float | `FixCreateBirds` | Pájaros de Ice Cold Killa en su sitio |
| Reinicio de variables y de pickups/generadores/saltos de IPL al empezar Nueva Partida | `ResetVariablesOnNewGame`, `ReinitIplObjectsOnNewGame` | Contadores, respray gratis, mensaje "To stop Carl", penalización wasted/busted |
| Territorios fuera del mapa | `FixTerritoriesOutsideMap` | Fuera de la rejilla 10x10 la zona cuenta como desbloqueada |
| Num5 enlazable | `BindableNum5` | El menú de controles acepta Num5 |
| Cuadros de ayuda junto a "Mission Passed" | `HelpBoxWithMissionPassed` | El cuadro de ayuda ya no se oculta |
| Sensibilidad vertical del ratón = horizontal | `MouseVerticalSensitivityFix` | Cámaras que leían `m_fMouseAccelVertical` |
| Ratón gira el coche en el aire solo con "Steer with Mouse" | `MouseAirborneRequiresSteerOption` | Automóviles y quads |
| Munición 1 de cuchillo, motosierra, katana y paracaídas de los trucos | `MeleeCheatAmmo` | No los sustituye un pickup |
| Stats en kilogramos y nombres GANGS/CRIMES de stats.html | `FixStatsKilograms`, `FixStatsHtmlSectionNames` | Texto de estadísticas |
| Nombre de la emisora sin ajuste de línea por dibujos de script | `FixRadioStationNameWrap` | `SetCentreSize` |
| Unidades métricas/imperiales según la configuración regional | `Units` (-1 región, 0 métrico, 1 imperial) | Antes: imperial solo con idioma americano |

Notas de la fusión: los arreglos de biker cop (`BikerCopSpawn`), azar de 16 bits (`Rand16bit`), SEAROADER (`SeaRoaderCheat`) y Beagle de Fort Carson (`FortCarsonBeagle`), que la rama `silentpatch-scm` también había portado (con los nombres `FixBikerCopSpawn`, `ScriptRandomness16Bit`, `SearoaderCheat`, `FixFortCarsonBeagle`), quedan con la implementación y las opciones de 15.1. `FixCheatMeleeAmmo` pasó a llamarse `MeleeCheatAmmo` (misma opción que la de NinjaCheat/ParachuteCheat; aquí cubre además los conjuntos de armas 1-3). El reinicio de variables de New Game es el de `SilentPatchNewGame` (superconjunto: además reinicia `TimeNextMadDriverChaseCreated`).

### 15.4 Vehículos, segunda tanda (rama `silentpatch2-vehiculos`, 25/11/2026)

Todas se apagan con `GTASA_SILENTPATCH=0` y cada una con `GTASA_SILENTPATCH_<OPCIÓN EN MAYÚSCULAS>=0` o `SilentPatch_<Opción>=0` (esquema único). Sin clave en `SilentPatchSA.ini` salvo lo indicado: activadas. Los arreglos con datos por modelo (el «SVF» de SilentPatch) usan una tabla estática en `Entity/Vehicle/SilentPatchVehicles.cpp`. Detalle por viñeta y estado de prueba: [CodigoVivoExe/silentpatch2-vehiculos.md](CodigoVivoExe/silentpatch2-vehiculos.md).

| Función (EXE) | Desviación | Opción | Riesgo |
| --- | --- | --- | --- |
| `CDamageManager::FuckCarCompletely` (0x6C25D0) | La rueda perdida en una explosión se elige entre 4 (rango -4) y los paneles no se reparan al instante | `ExplodedCarWheels`, `ExplosionPanelsSwing` | bajo (cambia el azar: una llamada a rand igual, otro rango) |
| `CAutomobile::BlowUpCar`, `CHeli::BlowUpCar`, `CPlane::BlowUpCar` | Se separan y ocultan las ruedas que marca el DamageManager (SpawnFlyingComponent + ocultar solo si se ve) en lugar de la delantera izquierda | `ExplodedCarWheels` | bajo |
| `CAutomobile::Fix` (0x6A3440), `CPlane::Fix` (0x6CABB0) | Se restauran las matrices originales del modelo (Rumpo, Bandito), 3 paneles en lugar de 4 (grúa y tractor conservan el del gancho; Vortex el suyo), ruedas del Rhino | `FixKeepsComponents` | bajo |
| `CPhysical::SetDamagedPieceRecord` (0x5428C0) | Con distancia <= 0 usa la pieza B del punto de colisión (el segundo coche daña sus piezas correctas) | `CarCollisionDamagePieces` | bajo |
| `CAutomobile::PreRender` (0x6AAB50), `CPlane::PreRender` (0x6C94A0), `CVehicle::SetupRender` | Animaciones extra por modelo (Phoenix, Sweeper, Newsvan, alerones de Beagle y Stuntplane) y componentes del motor parados con el motor apagado | `ExtraVehicleAnimations`, `EngineComponentsOnlyWhenOn` | bajo |
| `CClumpModelInfo::FindFrameFromNameWithoutIdCB` (0x4C52F0) | Acepta erratas de jerarquía (`wheel_lm`, `taillights`, `misca`, `transmision`, `elevator`, `boat_moving`) | `HierarchyTypoFix` | bajo (cambia qué marcos se enlazan) |
| `CAutomobile::ProcessControl`, `CCam::Process_FollowCar_SA`, `CAutomobile::GetTowBarPos`, `CTrailer::GetTowBarPos` | Escalera del Firela, barra del Tug y remolques artict3 encadenables | `FirelaLadder`, `TugTowBar`, `DoubleTrailer` | bajo |
| `CAutomobile::PreRender`/`Render` (luces del Rhino) | El Rhino dibuja luces y haces | `RhinoLights` | bajo |
| `CPlane::PreRender` (Hydra) | Banderas del `RwMatrix` temporal de los propulsores a 0 | `JetThrustersFix` | bajo |
| `CVehicleModelInfo::PreprocessHierarchy` (0x4C8E60) | Hunter: la puerta izquierda deja de marcarse izquierda; ruedas traseras dobles también en remolques y sin marcar la matriz nueva como identidad | `HunterDoorRenderFix`, `DoubleRearWheels` | bajo |
| `CObject::Render` (0x59F180), `CAutomobile::SpawnFlyingComponent` | Piezas sueltas con el color del coche, luz direccional y sin culling | `DetachedPartsColour`, `DetachedPartsBothSides` | bajo |
| `CCustomCarEnvMapPipeline::CustomPipeRenderCB` (0x5D9900) | Vehículo destrozado con luz direccional apagada: sin brillo ni reflejos; la luz especular usa el índice D3D 8 | `BlownUpVehicleShine`, `VehicleLightsNotOverwritten` | bajo (visual) |
| `CVehicleModelInfo::SetClump` (0x4C95C0), `SetDirtTextures` (0x5D5DB0) | Se rellena la lista de materiales de suciedad (el EXE 1.0 no la rellenaba: los coches sucios no se limpiaban) y se recorren los 32 | `CleanDirt` | bajo |
| `CVehicle::CustomCarPlate_TextureCreate` (0x6D10E0), `SetupRender`, `ResetAfterRender`, `CVehicleModelInfo::SetCarCustomPlate` (0x4C9450), `CCustomCarPlateMgr::SetupClumpAfterVehicleUpgrade`, `CreatePlateTexture` | Matrícula propia por vehículo (también motos), diseño propio (5 % aleatorio), aplicada al dibujar; filtro bilineal. Consume `rand()` del juego al crear cada vehículo (el EXE solo al cargar el modelo) | `UniqueNumberPlates`, `BilinearNumberPlates` | medio (cambia la secuencia aleatoria y la memoria usada por vehículo) |
| `CVehicle::GetRemapIndex` (0x6D0B70) | Usa la pintura pendiente si todavía no se dibujó | `PaintjobRemapIndex` | bajo |
| `CVehicleModelInfo::SetEditableMaterialsCB` (0x4C8220), `CVehicle::DoHeadLightBeam` (0x6E0E20) | Se restauran las 3 propiedades de superficie de los faros y los 9 estados de dibujo del haz | `VehicleLightsStayLit`, `HeadlightBeamStateRestore` | bajo (visual) |
| `CHeli::Render`, `CPlane::Render`, `SetAtomicRendererCB_RealHeli/_Plane`, `RenderHeliRotorAlphaCB`, `RenderHeliTailRotorAlphaCB`, `RenderVehicleHiDetailAlphaCB_BigVehicle` | Rotores y hélices con desenfoque (alfa según velocidad), salvo Maverick y Skimmer (como `SilentPatchSA.ini`) | `RotorBlur` | medio (cambia el aspecto de todos los aviones y helicópteros) |
| `CCarGenerator::DoInternalProcessing` | Contador de generadores con número fijo de apariciones (antes nunca bajaba) y código de área 13 con Z >= 950 | `CargenCounterFix`, `CargenInteriorAreaCode` | medio (cambia cuántos coches aparecen y dónde) |
| `CRenderer::ConstructRenderList` (0x5556E0) | Con un RC, el jugador es el RC (LOD y streaming) | `RCVehicleStreaming` | bajo |
| `CPlane::DoPlaneGenerationAndRemoval` (helicóptero de policía) | No apunta al grupo del jugador durante misiones | `RiotHeliIgnoresPlayerGroup` | bajo |
| `CBike::ProcessRiderAnims` (0x6B7280), `CPed` (viento sobre la ropa), `CVehicle::ProcessDrivingAnims` (0x6DF4A0) | Manillar del cuatrimoto, viento al conducirlo, animación de cambiar de emisora solo con brazo derecho y cabeza (sin ella en el Kart) | `QuadbikeHandlebarAnims`, `QuadbikeWindAnims`, `RadioChangeAnim` | bajo |
| `CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor` (0x64FC10), `CCarEnterExit::GetPositionToOpenCarDoor` (0x64E740) | Subir a la moto por delante resta la Z; furgonetas: grupo de animación 13 | `BikeJumpOnFromFront`, `VanRearDoors` | bajo |
| `CColModel::AllocateData` (0x40F870) | El bloque de datos de colisión asignado con malloc inicializa banderas e información de sombras | `CollisionDataInit` | bajo |
