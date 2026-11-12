<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 885 grupos / 10136 funciones reconstruidas; 10136 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desviaciones conocidas del C++ reconstruido frente al EXE original

Fecha de elaboración: 08/11/2026. Árbol: rama `main`, commit `160c24b0a0`. Documento de solo lectura: no se compiló, no se ejecutó el juego y no se tocó ningún hook.

Objetivo: dejar por escrito, de forma verificable, **dónde el código C++ reconstruido no es idéntico al EXE de GTA SA o dónde la validación no lo garantiza**. Un hook con `validated=1` significa "existe evidencia histórica aprobada dentro de unos fixtures", no "idéntico al EXE para toda entrada" (véanse [CurrentStatus](CurrentStatus.md) y [ValidationInventory](ValidationInventory.md)).

El riesgo (bajo, medio, alto) es una **estimación de quien redacta**, no un resultado medido. Criterio: *alto* = comportamiento distinto y alcanzable en una partida normal, o validación ausente en una ruta común; *medio* = comportamiento distinto en rutas raras o validación con un hueco relevante; *bajo* = diferencia no observable o sin llamadores que la lean.

## Método y fuentes leídas

| Fuente | Qué se hizo |
| --- | --- |
| `reversing/validation-registry/parte-001..005.json` | Se cargaron las 9493 entradas y se analizó el campo `scope` (frases, recuentos por expresión regular), `tested_sources`, `dependency_review` y `evidence.pairs`. Los recuentos por expresión regular cuentan entradas cuyo texto menciona el tema; no son recuentos de diferencias. |
| `reversing/*/README.md` (965 ficheros) | Búsqueda de "desviaci", "diferencia conocida", "Fuera de los fixtures", "saltad", "sin banco", etc. y lectura de los casos citados. |
| `source/` | Búsqueda de `FIX_BUGS`, `notsa::IsFixBugs`, `NOTSA`, `NOTSA_UNREACHABLE`, `NOTSA_LOG_*`, `locked = true`, `//RH_Scoped...`, `plugin::Call*`, y comparación de las direcciones de `RH_Scoped*Install` con `docs/hooks.csv`. |
| `docs/hooks.csv`, `docs/hooks_notsa.csv`, `docs/JuegoExe/` | Recuentos por tipo, filas ausentes, estado del mapa de funciones del EXE. |
| Cálculo propio | Hash SHA-256 de cada fuente listada en `tested_sources` frente al fichero actual (se aceptó coincidencia también tras normalizar CRLF a LF). |

Límites de este documento: no se leyeron los 965 README completos ni los 847 `validation-summary.json`; las categorías con cientos de casos iguales se agrupan con recuento y ejemplos. Los README de lotes antiguos y los documentos `BinaryCoverageAudit.md` (revisión del 10/10) y `CurrentStatus.md` (cuerpo del 10/10) tienen cifras fechadas; donde se contradicen con `hooks.csv` actual se indica.

## 1. Resumen con cifras

| Medida | Valor | Fuente |
| --- | ---: | --- |
| Filas en `docs/hooks.csv` (todas `reversed=1`, `validated=1`, `locked=0`) | 9493 | `docs/hooks.csv` |
| de ellas tipo S (hook sobre la dirección del EXE) / tipo V (hook de ranura de vtable) | 7792 / 1701 | `docs/hooks.csv`; coincide con el registro (S 7792, V 1701) |
| Entradas con dirección `0x00000000` en el registro (casi todas V: la dirección no figura en el catálogo) | 1516 | registro |
| Registros separados en `docs/hooks_notsa.csv` (`validated=0`) | 4 | `docs/hooks_notsa.csv`, [HooksNOTSA](HooksNOTSA.md) |
| Entradas del registro con `dependency_review = null` (sin revisión de dependencias) | 9326 de 9493 | registro |
| "Vigencia acreditada" declarada por el proyecto | 0 | [ValidationInventory](ValidationInventory.md), línea 10 |
| Entradas cuyas fuentes de `tested_sources` **ya no coinciden** con el árbol actual (aun normalizando CRLF) | 7253 de 9493 (coinciden 2240) | cálculo propio |
| Entradas cuyo `scope` dice "Historical fixture approval of the archived build only. Current source hashes were intentionally not checked" | 237 | registro (p. ej. `GangFollower355`, `GeometryQueries355`, `InteriorCore355`) |
| Pares por función: mínimo / mediana / entradas con menos de 128 / con menos de 256 | 8 / 256 / 1342 / 2752 | registro (`evidence.pairs`) |
| Ficheros de evidencia distintos | 847 | registro |
| Funciones del EXE (región 0x401000..0x74FFFF) según el mapa | 11 804 | [JuegoExe/README](JuegoExe/README.md) |
| Reemplazadas por C++ | 9376 (79,4 %; 91,1 % de los bytes) | JuegoExe |
| Envoltura pendiente (la DLL aún llama al EXE) | 22 (2 de `CWeaponModelInfo`, 20 de `Win`) | `docs/JuegoExe/funciones.csv` |
| **Sin reemplazar** (A: llamada desde código ya reemplazado 676; B: método virtual 826; C: solo la llaman funciones aún no reemplazadas 495; D: sin llamadas ni vtable 409) | **2406** | JuegoExe |
| Llamadas indirectas sin resolver en el EXE | 6004 | [BinaryCoverageAudit](BinaryCoverageAudit.md) |
| Puntos de `plugin::Call*` / `CallAndReturn` que siguen en `source/game_sa` | 175 en 38 ficheros (p. ej. `AEMFDecoder.cpp` 40, `Garage.cpp` 19, `Collision.cpp` 15, `GangWars.cpp` 11) | `grep` en `source/game_sa` |
| Líneas `__asm` / `naked` en `source/game_sa` | 4, todas comentarios en `X87Math.*` | `grep` |
| Bloques `FIX_BUGS` / `notsa::IsFixBugs` en `source/` | 59 apariciones en 32 ficheros; `FIX_BUGS` está **definido** (`source/CMakeLists.txt`, línea 430) | `grep` |
| `NOTSA_UNREACHABLE` / `NOTSA_LOG_DEBUG/ERR/WARN/TRACE` en `source/game_sa` | 139 / 74, 24, 19, 3 | `grep` |

Nota sobre las dos bases de recuento: las 9493 filas del catálogo y las 9376 "reemplazadas" del mapa del EXE no son el mismo conjunto (el mapa cuenta entradas binarias; el catálogo incluye sobrecargas, ranuras virtuales sin dirección propia y varias filas por dirección). No se ha conciliado una con otra.

Lectura honesta: el 100 % del catálogo significa "9493 filas con alguna evidencia histórica aprobada". No significa 9493 funciones idénticas al EXE para toda entrada, ni que el ejecutable ya no sea necesario (2406 funciones del EXE no están reemplazadas y hay 6004 llamadas indirectas sin resolver).

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

Se comparó la dirección de cada `RH_Scoped*Install` (excluyendo `RH_ScopedVirtualClass` y los de ranura virtual, cuya dirección en el CSV es 0) con las direcciones del CSV. Quedan 19 sin fila:

| Dirección | Función | Fuente | Estado |
| --- | --- | --- | --- |
| 0x53F2D0 | `GetMouseSetUp` | `app/platform/win/WinInput.cpp:339` | Activa (`WinInput::InjectHooks` se llama desde `WinPlatform.cpp:171`). Sin fila: `Fn132A/README.md` lo dice ("el catálogo se regenera aparte"). Sin banco propio localizado. |
| 0x7487CF, 0x747020, 0x746D80 | `WinInput::Initialise`, `EnumDevicesCallback`, `diPadSetRanges` | `WinInput.cpp:334-338` | Activas; sin fila. |
| 0x7469A0 | `diMouseInit` | `WinInput.cpp:336` | `{.locked = true}`: "Can't be hooked because it fails with ACCESS DENIED and crashes". No se instala. |
| 0x7486A0, 0x745560 | `Win32_InitApplication`, `Win32_InitInstance` | `WinMain.cpp:539-540` | La segunda `{.locked = true}`. Solo con `!NOTSA_USE_SDL3`. |
| 0x746ED0 | `CPad::GetMouseState` | `Pad.cpp:137` | Solo con `!NOTSA_USE_SDL3`; sin fila. |
| 0x59ACF0, 0x59ADD0, 0x59BB60, 0x59AD20, 0x459350, 0x59A840, 0x59AA40, 0x59ADF0 | `CMatrix::Detach`, `CopyOnlyMatrix`, `Update`, `UpdateMatrix`, `Scale`, `ConvertToEulerAngles`, `ConvertFromEulerAngles`, `operator+=` | `Core/Matrix.cpp:43-57` | **Código muerto**: `CMatrix::InjectHooks()` está desactivada en `InjectHooksMain.cpp` ("Don't hook these, performance is bad", líneas 729-735); solo se instalan los grupos selectivos. Sin fila ni banco. |
| 0x40FDD0 | `CVector::Sum` | `Core/Vector.cpp:66` | Igual: `CVector::InjectHooks()` desactivada. |
| 0x43E110 | `CVector2D::operator=` | `Core/Vector2D.cpp:36` | Igual. |
| 0x675FC0 | `CTaskInteriorLieInBed::FinishAnimCB` | `TaskInteriorLieInBed.cpp:32` | Instalada con `RH_ScopedInstall` (S); el CSV tiene fila V con dirección 0 (línea 8166). Hay un comentario en `InjectHooksMain.cpp` (líneas 1128-1134) que explica la corrección de VMT a S. **Tipo del CSV desactualizado.** |

Consecuencia: `docs/JuegoExe/funciones.csv` clasifica como "Falta" (A o D) algunos de esos EXE aunque el código los engancha o los tiene preparados (p. ej. 0x59ACF0 figura como A de `CMaths`, 0x53F2D0 como A de `CMouseControllerState`, 0x40FDD0 como D). El mapa se genera desde el catálogo, no desde las instalaciones reales. **Riesgo: bajo para el comportamiento; medio para la fiabilidad del recuento de "sin reemplazar".**

### 3.4 Hooks desactivados o bloqueados deliberadamente

- Clases enteras con `InjectHooks()` comentado: `CVector`, `CVector2D`, `CMatrix` (con instalaciones selectivas), `CPtrList`, `CPtrNodeDoubleLink`, `CPtrListDoubleLink`, `CPtrNodeSingleLink`, `CPtrListSingleLink` (`InjectHooksMain.cpp:760-766`; Fn131A los reactivó puntualmente con `Flush` en 0x552400 y 0x552470) y unas 30 clases de tareas y eventos (`InjectHooksMain.cpp` líneas 1160-1380 y 1517-1553 aprox., p. ej. `CTaskComplexKillAllThreats`, `CTaskSimpleHailTaxi`, `CTaskSimpleAbseil`). Para las clases de tareas, que su `InjectHooks` esté comentado indica que esas funciones no están reemplazadas; no se comprobó cuántas funciones del catálogo corresponden a esas líneas.
- `source/` contiene 109 líneas `//RH_Scoped...` comentadas. Ejemplo: `Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h:42` `//RH_ScopedInstall(Get, 0x5F7200, { .enabled = false, .locked = true }); // Spoils <ecx>, can't hook`.
- Hooks con `locked = true` instalados en el código: `diMouseInit`, `Win32_InitInstance`. El CSV tiene 0 filas bloqueadas ([CurrentStatus](CurrentStatus.md): "Bloqueos reales pendientes: 0"), coherente salvo por esos dos que están fuera del CSV.
- `Win::NOTSA_WinMain` (0x748710) y tres hooks `WindowedModeHooks`: registrados aparte en `hooks_notsa.csv` con `validated=0`; el cuerpo adaptado NOTSA no se presenta como fiel. `source/app/platform/win/WindowedMode.cpp` conserva `__asm` y `naked` como excepción documentada ([ImplementacionesDelJuego](ImplementacionesDelJuego.md)). **Riesgo bajo** (capa de modo ventana, no código del juego).

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

**Riesgo medio**: es la mayor limitación de la garantía. El 100 % es histórico.

---

## 7. Funciones saltadas por complejas y funciones sin reemplazar

### 7.1 Saltadas documentadas

| Lote | Función | Motivo | Estado |
| --- | --- | --- | --- |
| `Fn105A` | 0x4AB250 (5 bytes, `jmp 0x5389B0` a `CFileMgr::ReadLine`) | Trampolín de un salto; "enganchar un trampolín de un salto no aporta nada" | Sin hook propio; los 59 llamadores por 0x4AB250 acaban en `ReadLine` con hook |
| `Fn130A` | 0x5DE920 (destructor de `CAEPedAudioEntity`) y 0x563130 (destructor de `CSector`) | Marco SEH y colaboradores sin hook | Resueltas en `Fn131A` (65536 pares, cero diferencias) |
| `Fn132A` | 0x53F2D0 `GetMouseSetUp` | Ya tenía hook en código, no en el CSV (3.3) | Activa, sin fila ni banco localizado |
| `Fn132A` | 0x747300 (`getDvdGamePath`, `WinMain.cpp`) | Llama por IAT a `SetErrorMode`, `GetLogicalDriveStringsA`, `GetDriveTypeA`, `GetVolumeInformationA`, `lstrlen`, `lstrcpy`; resultado depende de las unidades del equipo; exige falsos de cinco IAT y el modelo de `repe cmpsb` | **Sin hook** (hay versión C++ sin enganchar) |

### 7.2 Sin reemplazar en el mapa del EXE

2406 funciones (676 A, 826 B, 495 C, 409 D) más 22 envolturas pendientes (2 `CWeaponModelInfo`: 0x4CC970, 0x4CD250; 20 de `Win` entre 0x749330 y 0x74FC70). Bloques con menor cobertura: 0x5C0000 (9 % reemplazado, 127 faltan, 27 874 bytes; sin módulos nombrados), 0x740000 (47 %), 0x4A0000 (52 %, `FxInfoManager_c` 96), 0x5E0000 (53 %, `CPedAttractor` 13), 0x4B0000 (56 %, eventos). Por clase: `CPools` 51, `CAEUserRadioTrackManager` 28, `FxInfoManager_c` 96, `CVehicle` 29, `CBike` 22 (véase `docs/JuegoExe/README.md`). El mapa es una **cota inferior** (6004 llamadas indirectas sin resolver) y se genera del catálogo (3.3), por lo que puede incluir algunas funciones ya enganchadas.

La sección "Omisiones concretas" de `BinaryCoverageAudit.md` (revisión del 10/10) está **desfasada**: `CPed::ProcessControl`, `SpecialEntityPreCollisionStuff`, `CAutomobile::PreRender` y `CTaskSimpleFight::ProcessPed` ya tienen fila V en `hooks.csv` (líneas 8802, 8803, 8814 y 8797).

**Riesgo alto** (para la meta de independencia del EXE, no para el comportamiento actual): el juego sigue llamando al EXE en esas 2406 + 22 funciones y en los 175 puntos `plugin::Call*`.

---

## 8. Hallazgos de riesgo medio y alto

| # | Hallazgo | Riesgo | Sección |
| ---: | --- | --- | --- |
| 1 | 2406 funciones del EXE sin reemplazar y 22 envolturas; 175 `plugin::Call*` activos; 6004 llamadas indirectas sin resolver | Alto (independencia) | 7.2 |
| 2 | La evidencia es histórica: 7253 de 9493 entradas con fuentes distintas de las probadas, 9326 sin revisión de dependencias, vigencia declarada 0 | Medio | 6.3 |
| 3 | 7761 entradas validadas con colaboradores falseados; integración real no cubierta | Medio | 5 |
| 4 | `FIX_BUGS` definido: 59 apariciones activas, no idénticas al EXE (p. ej. `Collision.cpp:1493`, menús, `ProcessEntityCollision` de coche y moto) | Medio | 2.2 |
| 5 | `CCamera::CamControl` (dos rutas) y `CPlane::VehicleDamage` explosivo a humo: comportamiento definido distinto del EXE (pila sin inicializar), no certificado | Bajo a medio | 2.1 |
| 6 | `NOTSA_UNREACHABLE` en 139 sitios, `FxInfoManager.cpp:129-131` con tres casos comentados, `IplStore.cpp:817` condicional invertido sin verificar | Medio | 2.3 |
| 7 | 19 hooks activos o preparados sin fila en `hooks.csv` (incluye `GetMouseSetUp`) y fila `FinishAnimCB` con tipo desactualizado; el mapa del EXE los cuenta como "Falta" | Bajo a medio | 3.3 |
| 8 | Aritmética x87 frente a SSE: solo ~10 ficheros compilados con `/arch:IA32 /fp:fast`; el resto depende de ayudantes y de los bancos | Medio (no medido) | 2.5 |
| 9 | Equivalencias de banco aceptadas (vtable de `CTask`, EAX/ECX/EDX informativos, relleno enmascarado) | Bajo a medio | 4 |
| 10 | 1075 entradas excluyen dominios donde el EXE falla; el C++ añade comprobaciones en ellos sin comparar | Bajo a medio | 6.1 |
| 11 | 76 `ScalarDeletingDestructor` y 338 `Destructor` catalogados como S, no V | Bajo | 3.1 |
| 12 | Banco de regresión final detenido (54 de 148 y 74 de 177 en cortes fechados) | Medio (fechado) | 6.3 |

## 9. Cómo actualizar este documento

1. Recontar con los mismos criterios: `python` sobre `reversing/validation-registry/parte-*.json` (campos `hook`, `scope`, `tested_sources`, `dependency_review`, `evidence.pairs`).
2. Volver a comparar hashes de `tested_sources` con el árbol tras cada lote; las cifras de la sección 6.3 cambian con cada commit.
3. Regenerar el cruce de `RH_Scoped*Install` frente a `hooks.csv` (sección 3.3) tras añadir lotes.
4. Revisar `source/CMakeLists.txt` (líneas 425-435) por si `FIX_BUGS` deja de definirse, lo que cambiaría la sección 2.2.
