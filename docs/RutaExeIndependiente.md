<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ruta hacia un exe que corra sin el original

Documento de planificación (01/11/2026). Las cifras de catálogo e imports están medidas; los costes y plazos son estimaciones y están marcados como tales. Los datos de `librw` salen del experimento en `D:\Original\librw-test` (no versionado).

## Actualización del 24/11/2026: estado medido del ejecutable independiente

Este bloque resume lo ocurrido entre el 22 y el 24/11/2026. Cada hecho cita el documento donde está la medición; aquí no se repite nada que ese documento no diga. Las cifras del catálogo son de `docs/hooks.csv` el 24/11/2026 (`contrib/sync_docs.py`, `contrib/inventario_comandos_script.py`, `contrib/pending_wrappers.py`, `contrib/audit_cpp_assembly.py`): 13.511 entradas, todas `reversed=1`, 1.007 grupos, 11.800 `validated=1` (la validación histórica sigue sin vigencia acreditada), 0 envolturas pendientes y 0 apariciones de `__asm` o `naked` en `source/game_sa`. Que el catálogo esté al 100 % **no** significa que el ejecutable independiente esté terminado ni que sea equivalente al original.

### Lo que se hizo y lo que se midió

| Tema | Hecho (documento) | Lo que NO está verificado |
| --- | --- | --- |
| Arranque sin `gta-sa.exe` | El independiente compila y enlaza en un clon limpio sin símbolos sin resolver; una ejecución de 334 s se mantuvo en estado de juego 9 durante más de 5 minutos, con 0 funciones de código original ejecutadas y 0 accesos a un `gta-sa.exe` según el espía de `ntdll`; el control con un señuelo prueba que el espía sí lo vería ([sin-original.md](CodigoVivoExe/sin-original.md)). | Ropero y marcadores (sus sondas son del ASI), zonas más allá del callejón inicial, guardado y carga. El espía no ve una enumeración de directorio ni procesos hijos. |
| Tráfico y «dato en dos sitios» | Sin tráfico en el independiente: `GenerateOneRandomCar` y `ChooseModel` leían por dirección lo que el C++ escribía en objetos propios. Tras corregirlo hay 10-12 coches aleatorios frente a 0-1 antes; con el ASI y el código original de tráfico, 4-7 ([ejecucion-trafico-nubes.md](CodigoVivoExe/ejecucion-trafico-nubes.md)). Barrido de todo `source/`: 11 correcciones más de la misma clase (9 trucos, modos de cámara, helicóptero, mira, radio, luces de niebla...) ([auditoria-datos.md](CodigoVivoExe/auditoria-datos.md)). | El independiente genera más peatones (28-34) que el control (6-9), sin causa explicada. Los efectos de las 11 correcciones están deducidos leyendo el código; ninguno se midió en ejecución. |
| Auditorías de datos G2-G4, G5, G6-G8 | G2-G4: 2 fallos reales (`MI_HYDRAULICS`, 0x8CD76C, y `m_bWaterFogScript`, 0x8D37D5), corregidos sin verlos en juego ([ejecucion-datos-g2g4.md](DatosExe/ejecucion-datos-g2g4.md)). G5a: 3 fallos (fundidos del HUD, lista libre de muebles de interiores, ruta de scripts) ([auditoria-g5a.md](DatosExe/auditoria-g5a.md)). G5b: esos mismos y los combos de melee (`bC17738`, `bC17780`) ([auditoria-g5b.md](DatosExe/auditoria-g5b.md)). G6-G8: 2.157 objetos propios comparados con su dirección (535 difieren en el independiente, 1 con uso por nombre y por literal) y la semilla de `rand` del EXE, que `GameInit` nunca sembraba ([ejecucion-datos-g6g8.md](DatosExe/ejecucion-datos-g6g8.md)). | G5a: de 585 objetos, entre unos 100 y 290 nunca cambiaron en las ejecuciones; G5b: 244 de 534 no se ejercitaron (garajes, guerras de bandas, repetición, guardado, muchas misiones, audio concreto): solo vale el análisis estático. En G2-G4 no se condujo un coche durante el registro. La semilla de `rand` se corrigió por desensamblado y lectura; no se midió `holdrand` en ejecución. |
| Mando XInput y pruebas de juego | `ProcessPad` lee el mando por XInput y, sin mando, queda el camino de DirectInput (commit `ed39933019`). La sonda de juego probó en el independiente conducción, armas (ratón, teclado y mando), interiores, trucos y cámara con un `xinput1_4.dll` falso (commit `1c9f33fcea`; evidencia en `reversing/FnJuego/evidencia/`: A2, B2, D3, E7, F5, G6, H8, K9, L10, M11, N12, P13, xi1). Entrar en el gimnasio de Ganton caía (código 0xC0000005, ejecución C2) porque `TASK_KILL_CHAR_ON_FOOT` y `_WHILE_DUCKING` recibían un `CPed&` y no el manejador entero; corregido (commit `96f1c36f61`), y en la ejecución P13 entraron y salieron CARLS, GYM1, CLOTHGP, AMMUN1 y BARBERS, con el juego vivo y sin NaN. | **No hay informe escrito de estas pruebas en `main`**: lo anterior sale de los commits y de los `resumen.json` de la evidencia. Mando real (solo se usó el DLL falso) y misión 1 completa. |
| Comparación con el original | Casa de CJ y marcadores iguales; exterior despejado y nublado con diferencia de color medio del canal verde menor o igual que 3 en 31 de 36 pares; espejo del ropero distinto ([ejecucion-comparacion-original.md](CodigoVivoExe/ejecucion-comparacion-original.md)). | Una captura aislada del independiente salió más oscura (`s_int1`, factor 0,85) y no se reprodujo ni se explicó. Solo 800x600; no se probó 640x480 ni 1024x768. |
| Espejo del ropero | La causa era la GPU: Optimus mandaba el independiente a la Intel y el original a la NVIDIA, y el pase del espejo usa un render target sin MSAA con un z-buffer con 4x MSAA que cada controlador resuelve distinto. `StandaloneExe.cpp` exporta `NvOptimusEnablement` y `AmdPowerXpressRequestHighPerformance`; la diferencia del recorte del espejo baja de 35,75 a 2,03 ([ejecucion-espejo-ropero.md](CodigoVivoExe/ejecucion-espejo-ropero.md)). | No se demostró por qué cada controlador responde distinto; no se probó en AMD, sin GPU NVIDIA ni con MSAA en 0. Otros espejos (estadio, suelos) sin probar. |
| Lluvia, gamma y carga | La lluvia no es un defecto: es ruido del juego (rayos) con el mismo reparto en los dos. La gamma de `s_int1` no viene del juego ni de la rampa de hardware. La carga de `gGameState` 8 a 9 tarda 1,9-2,2 s en el independiente frente a 11,5 s en el original y 8,7 s en el ASI, con el mismo estado de streaming ([ejecucion-lluvia-gamma-carga.md](CodigoVivoExe/ejecucion-lluvia-gamma-carga.md)). | Por qué el independiente carga más rápido: no se identificó el mecanismo. Contenido de las colisiones por modelo sin comparar. |
| Nubes | Sin diferencia real de cielo ni de nubes frente al original (100 escenas); a pie las nubes esponjosas no se dibujan tampoco en el original ([ejecucion-nubes-original.md](CodigoVivoExe/ejecucion-nubes-original.md)). | Vehículo y avión, climas 1, 3, 5, 6, 9 y superiores, nubes esponjosas dibujadas de verdad, rayos. El `SendInput` del ratón se comporta distinto en el original y en el independiente y no se investigó. |
| QuickTime a Media Foundation | `CAEMFDecoder` usa `CMfPcmReader` (Media Foundation); el PCM coincide con el de QuickTime a ±1 LSB en 2 MP3, no es idéntico bit a bit ([sustitucion-quicktime.md](QuickTime/sustitucion-quicktime.md)). | Audio de pistas de usuario `.aac`, `.m4a` y `.mov`; cambio de pista, bucle y 48 kHz; la escucha real (el pico WASAPI no distingue las ejecuciones). |
| DirectDraw a D3D9 | `GetVideoMemInfo` con `GetAvailableTextureMem` y `GetDXVersion` sin DDRAW; el independiente ya no importa `DDRAW.dll` ([directdraw-sustitucion.md](RenderWareExe/directdraw-sustitucion.md)). | Pantalla completa exclusiva. La lista de modos del original es de 52 y la del independiente y del ASI de 35 (sin los R5G6B5), sin causa investigada. |
| CRT y x87 | Las 6 envolturas del montón del CRT pasan a `ExeCrt::*`; ramas x87 de `floor` y `ceil` sustituidas con banco sin diferencias; `RwMathX87` sin diferencias en 400.000 casos por sección; envolturas pendientes: 0 ([ejecucion-sustitucion-crt-rw-x87.md](CodigoVivoExe/ejecucion-sustitucion-crt-rw-x87.md)). | `pow` con la x87 no es bit a bit (se usa SSE2) y no se midió la diferencia; montón con `__active_heap != 1`; fugas en sesiones largas (la partida larga duró 5,5 minutos); ninguna de las funciones nuevas está registrada como validada. |

### Pendientes reales (no hechos)

- Misión 1 completa, mando real, mapa grande, guardar y cargar ([ejecucion-cargar-partidas.md](CodigoVivoExe/ejecucion-cargar-partidas.md): guardar y recargar no terminó).
- Objetos propios de datos no ejercitados: 244 en G5b y unos 100 a 290 en G5a (ver arriba).
- `CdStream`: sin decidir (rama `carga-guardar-2`, sin documento en `main`).
- Caída al cerrar el juego en `CEntity::CleanUpOldReference` (`Entity.cpp:1811`): previa a los cambios de CRT; la base 48e53f065c cae igual ([ejecucion-sustitucion-crt-rw-x87.md](CodigoVivoExe/ejecucion-sustitucion-crt-rw-x87.md)).
- Caída de `BuildShadowVolume` en la ASI: dato aportado por el responsable del proyecto; no hay informe en el repositorio.
- Ratón por `SendInput` del independiente ([ejecucion-nubes-original.md](CodigoVivoExe/ejecucion-nubes-original.md), sección 6).
- Pantalla completa exclusiva y lista de modos 52 frente a 35 ([directdraw-sustitucion.md](RenderWareExe/directdraw-sustitucion.md)).
- Audio de pistas de usuario `.aac`, `.m4a` y `.mov` ([sustitucion-quicktime.md](QuickTime/sustitucion-quicktime.md)).
- Una nota de método sobre las cifras de la región RenderWare: siete funciones (seis puentes de QuickTime atribuidos por vecindad a `RpMatFX` y el thunk `DirectDrawCreateExThunk`) dejaron de contar como «reemplazadas» porque su C++ se retiró al sustituir QuickTime y DirectDraw. Ahora figuran como sustituidas en [no-reconstruibles.csv](RenderWareExe/no-reconstruibles.csv); no es un retroceso del trabajo.

### Bloqueo de la documentación corregido (24/11/2026)

`contrib/hooks2md.py` y `contrib/sync_docs.py` se negaban a correr porque el número de filas `validated=1` de `hooks.csv` (11.791 en `main`) no coincidía con las evidencias aprobadas del registro (11.800 contadas en este árbol). Causas medidas:

1. **Seis filas de D3DX9** (`D3DXFloat16To32Array`, `D3DXVec3TransformCoord`, `D3DXVec3TransformNormal`, `D3DXMatrixMultiply`, `D3DXMatrixTranspose`, `D3DXMatrixMultiplyTranspose`) tenían evidencia válida en `reversing/validation-registry/parte-006.json` (hash del resumen correcto y `covered_functions` en verdadero) pero `hooks.csv` no se había sincronizado: bastó `python contrib/sync_validated_flags.py`.
2. **Tres filas** (`CBaseModelInfo::SwaysInWind`, `FxManager_c::DestroyAllFxSystems` y `FxManager_c::Update`) tenían registrado el hash de un `validation-summary.json` guardado con saltos de línea **mixtos** (parte CRLF y parte LF). Git guarda esos archivos con LF, así que en un clon limpio el hash no coincide y esas filas contaban como no aprobadas (y como aprobadas en una copia de trabajo con CRLF: por eso las cifras variaban de un árbol a otro). El contenido es idéntico salvo los saltos de línea (comprobado byte a byte contra la copia que sí coincidía con el hash). Corrección: el registro guarda ahora el hash del texto normalizado a LF en esas dos evidencias, y `contrib/validation_registry.py` acepta, además del hash exacto y del CRLF, el normalizado a LF. No se falseó ninguna validación: si el contenido hubiera cambiado, la fila seguiría en 0.
3. `contrib/sync_validated_flags.py` escribe el CSV antes de comprobar la coincidencia, y la opción `--update-source-status` de `contrib/hooks2md.py` y `contrib/sync_docs.py` reescribe los comentarios de estado de unos 59 archivos de `source/` (solo comentarios: los bloques `GENERATED HOOK STATUS` con las cuentas por clase). Esos archivos y `source/reversiblehooks/HookStats.generated.hpp` (que escribe `contrib/sync_readme_counts.py`) **no** se regeneraron en este trabajo, para no tocar la compilación; se regeneran con `python contrib/sync_docs.py --update-source-status` y `python contrib/sync_readme_counts.py`.

Resultado: 11.800 de 11.800 con `python contrib/sync_validated_flags.py`, y `python contrib/audit_hook_catalog.py --strict` en OK.

Desde aquí hacia abajo el documento es la planificación del 01/11 y la nota del 18/11: conserva las cifras de aquella fecha (por ejemplo, las tablas de dependencias y «Lo que depende hoy del exe») y no se han vuelto a medir.

## Actualización del 18/11/2026: cambio de plan para RenderWare

Este documento se escribió el 01/11/2026, cuando RenderWare no estaba reconstruido y `librw` era el atajo. Hoy el código del juego (0x401000–0x74FFFF) y RenderWare (724 de 724 funciones) están reconstruidos en C++ y en `main`; falta validarlos contra el EXE. Por eso:

- **RenderWare se queda con la reconstrucción propia.** Una vez validada al 100 %, es un RenderWare igual al del EXE: no hace falta escribir plugins, capa de compatibilidad ni pipelines nuevos para `librw`, ni volver a comprobar que se ve igual.
- **`librw` deja de ser el camino principal.** Queda solo como idea opcional para portabilidad (correr fuera de Windows o con OpenGL o Vulkan). Los pasos 2, 3 y 4 de la hoja de ruta original y la fila de RenderWare de las tablas de abajo quedan como contexto histórico.
- **Lo que sigue fuera de la reconstrucción:** las funciones de libpng, zlib y D3DX9 que se enlazan en lugar de reconstruirse (libpng y zlib son abiertas; D3DX9 es una DLL de Microsoft que hay que dejar disponible), las 54 funciones de RenderWare no reconstruibles, QuickTime/QTML (sustituir), DirectDraw (reescribir con D3D9/DXGI) y el CRT.
- **Las 118 funciones excluidas del juego** (51 `__usercall` cubiertas por llamadoras validadas, 17 `__usercall` de RenderWare que pasan al [plan de validación de RenderWare](RenderWareExe/plan-validacion.md), 1 separador de la línea de comandos sustituido por el CRT (lote G3A), 20 saltos puros, 18 bloques de la protección del EXE y 11 bloques que no son funciones; [excluidas.csv](JuegoExe/excluidas.csv)): cada una está cerrada con una categoría y una evidencia por inspección de bytes y de llamadoras ([reversing/FnG2A](../reversing/FnG2A/README.md), revalidable con `inspect_excluidas.py --check`); la última con trabajo pendiente, 0x746480 (separador de la línea de comandos, llamado solo por `NOTSA_WinMain`), se cerró en el lote G3A como `sustituida-por-crt` ([reversing/FnG3A](../reversing/FnG3A/README.md)): ninguna excluida del juego queda con trabajo pendiente. Se revisa la lista al cerrar el juego.

### Orden vigente

1. **Cerrar la validación del código del juego** (lotes 235A a 239A en curso; después un pase final de bancos viejos).
2. **Validar RenderWare** según [plan-validacion.md](RenderWareExe/plan-validacion.md) (652 entradas por validar).
3. **Cerrar las excluidas del juego** con el criterio de arriba.
4. **Sustituir lo externo:** QuickTime, DirectDraw, CRT.
5. **Cambio de método de validación.** Sin el EXE desaparece la comparación par a par: guardar salidas del EXE como referencia o comparar imágenes.
6. **Opcional:** `librw` como backend alternativo para portabilidad.

## Objetivo

Un ejecutable de San Andreas que no necesite `gta-sa.exe`: todo el código del juego reconstruido en C++ y las dependencias externas sustituidas o enlazadas.

## Lo que depende hoy del exe

### Dependencias externas (imports de `gta-sa.exe` 1.0 US)

| Dependencia | Imports | Plan |
|---|---|---|
| KERNEL32, USER32, ADVAPI32, GDI32, ole32, WS2_32, WINMM | 142, 48, 10, 5, 7, 4, 8 | Sistema operativo: nada que hacer |
| `vorbisfile.dll` | 11 | Biblioteca abierta, se enlaza tal cual |
| `d3d9.dll` | 7 | Lo trae Windows |
| `DINPUT8` | 7 | Sistema, o SDL3 como extra |
| `DSOUND` | 8 | Sistema, o OpenAL |
| `DDRAW` | 10 | Solo lista modos de vídeo; reescribir con D3D9/DXGI |
| `EAX.DLL` | 4 | Obsoleto (Creative); descartar o dejar sin efecto |

### Código enlazado dentro del exe

| Bloque | Tamaño (catálogo) | Plan |
|---|---|---|
| RenderWare SDK | 134 entradas | `librw` cubre la lectura de datos; faltan plugins de SA y el puente |
| Driver D3D9 de RenderWare | 58 entradas | Backend D3D9 de `librw` + pipelines de SA |
| QuickTime/QTML | 2 entradas en catálogo; unas 27 llamadas y 4 ayudantes (`0x4E7990`, `0x4E7A80`, `0x4E7B80`, `0x4E7CB0`) usados por `AEMFDecoder.cpp`; unas 180 funciones puente en `0x818000-0x81B000` (cuenta por patrón) | Sustituir por Media Foundation u otro decodificador de MP3 (radio personal) |
| CRT estático | 7 entradas | CRT del compilador; cuidado con los redondeos que el juego usa |
| Código del juego | 11.804 funciones en 0x401000-0x74FFFF (docs/JuegoExe): 8.905 reemplazadas (75,4 %), 33 envolturas pendientes y unas 2.866 sin reemplazar (grupos A-D, cota inferior por 6.004 llamadas indirectas sin resolver). El grupo A (1.007) es el piso para la independencia | Lotes de validación en curso |

## Lo que se midió de `librw`

- Compila como biblioteca estática x86 con VS 18.
- Abre 15331 de 15356 DFF y 4049 de 4049 TXD de SA (RW 3.6.0.3). Los 25 DFF restantes son diccionarios de animación UV (chunk 0x2B); el exe tampoco los lee como clump.
- Comparado con el RenderWare original del exe dentro del juego (15309 modelos, 19186 geometrías): 0 diferencias en recuentos, posiciones, normales, UVs, prelit, triángulos, mesh, materiales (color y propiedades), flags, frames, HAnim y skin.
- Única diferencia real: el exe pide más texturas (929 modelos, 560 sin `vehiclespecdot64`) por plugins de SA que `librw` salta sin leer (2dfx, Breakable y material especular; la atribución a Breakable es una inferencia).
- No comparado: MatFX, UVAnim, derechos de render, datos de los plugins de SA, nombres de textura por material (se sustituyeron por "no encontrada"), escritura de archivos.
- El backend D3D9 dibuja un coche y un peatón con skin de SA con texturas correctas. Falta el color de pintura, el filtrado de partes por nombre de frame, las animaciones y los reflejos.
- Adaptaciones de las pruebas: el plugin de colisión de coches (0x253F2FA) se renombró en la copia en memoria para el exe, porque su pool aún no existe en el menú; el hilo principal se suspendía durante cada lectura.

## Hoja de ruta propuesta

1. **Cerrar el código del juego** (lotes actuales). Hasta entonces el exe sigue haciendo falta.
2. **Capa de RenderWare propia.** Enlazar `librw`, registrar los plugins de SA (2dfx, Breakable, Specular y Environment Material, Extra Normals, Node Name, Pipeline Set, COL embebido) y verificarlos con los hashes de `librw-test`.
3. **Pipelines de SA** sobre el backend D3D9: coches (environment map, specular), skin de peatones, vegetación, cristales. Bloque de coste desconocido.
4. **Puentes `RwD3D9*` y del SDK** (58 + 134). Dependen del punto 3.
5. **QuickTime, DDraw y CRT.**
6. **Cambio de método de validación.** Sin el exe, la comparación par a par desaparece: guardar salidas del exe como referencia (como `exe_vals.csv`) o comparar imágenes.

## Riesgos sin resolver

- Paridad visual: reflejos, iluminación nocturna, colores dinámicos.
- Redondeos del CRT y del x87 donde el juego los usa.
- Tablas y globales que el código lee por dirección fija y hay que extraer del binario.
- Escritura de archivos con `librw` sin probar.

## Estimaciones (sin medir)

- Vista correcta de vehículo en la demo: de una a pocas horas.
- Peatones animados (ANP): de uno a dos días.
- Plugins de SA en `librw`: poco riesgo, verificables con hashes.
- Pipelines y puentes D3D9: no estimado.
