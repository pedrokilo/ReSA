# Cuarta ronda del experimento de independencia (22/11/2026, rama `ronda4-indep`)

Base: `integracion-fase7b` 391cb7108d (skin, `SubmitNoLight`, vertex shader `0x75F0B0`, init estática sin `StandaloneInit.inc`, arreglo de vtables, cielo, auditoría de script). **No se cambió ningún `.cpp` del juego**: solo herramientas (`reversing/FnFase8`, `reversing/FnIND0A/analizar.py`) y datos. Método de siempre: el `gta_sa.exe` original solo como archivo de datos (`.text` = INT3, modo estricto), `.obj` de `wv-int` enlazados con `reversing/FnFase8/enlazar.cmd` (`enlazar_r4.sh`, 25 s, `EXIT=0`), carpeta de pruebas `D:\Original\_fase8b_run`, candado `gta_lock`. Registros en `reversing/FnFase8/ejecuciones/r4a-*` a `r4d-*` (y `r4-*.png`).

## 1. Ejecuciones (todas partida nueva, `SaveGameToLoad=0`, FastLoader sin logo, sin `--shims`)

| Intento | Qué | Resultado |
| --- | --- | --- |
| `r4a` (330 s) | observador `observar_jugador.py --entrada --modo tecla` | `gGameState=9` a los ~35 s; apertura de ~107 s de juego; **control libre a los 200 s** (`CPad::DisablePlayerControls` 160, 32, 0; terminan los scripts `a_cont`/`intro`); con W pulsado CJ pasa de (2239,3; -1261,9) a (2261,5; -1260,7) en ~4 s. 0 originales, 0 excepciones. Capturas `r4-cj-libre-antes.png` y `r4-cj-libre-despues.png` |
| `r4b` (300 s) | `cielo_r4.py` + `datos_copiados_r4.py` | capturas en el horizonte y con palanca derecha arriba (sin efecto: cámara de script); medición de datos (sec. 4) |
| `r4c` (300 s) | **`--censo`** con `salida\retn.bin` | `fase8_codigo_original.csv` vacío: 0 funciones originales llamadas |
| `r4d` (330 s) | `cobertura_r4.py`: INTRO (subir a la bici), pedalear, ESC (menú de pausa), bajar, ESC | la bici, los peatones y el menú de pausa funcionan sin original (`r4-bici-peds.png`, `r4-menu-pausa.png`) |

Latidos finales: 7.300-8.100 fotogramas en 300-330 s (~25 fps en Debug), `Present` = `EndScene`, 4,9-5,5 M llamadas de dibujo, `codigo-original-distinto=0 hits=0 excepciones=0` en las cuatro. A los 5 min: 2-12 peatones y 6-10 coches de 110.

### 1.1 Estado visual (capturas revisadas con la herramienta de imagen)

* **Se ve**: callejón y calle de Grove Street, casas de estuco azul, palmeras y árboles con follaje, CJ (camiseta blanca, vaqueros) con skin deformado correctamente, peatones lejanos, la bici BMX (roja o amarilla según la partida), HUD (dinero, arma, radar), subtítulos y ayudas en español, menú de pausa con fondo del cartel de Vinewood.
* **Cielo**: azul claro arriba y naranja cálido en el horizonte, que cambia con la hora (más azul en capturas posteriores). Ya no es blanco. **Sol y nubes: no verificados** (la cámara de la apertura es de script y no se movió; hace falta una sonda de memoria de `CClouds::Render`/`CCoronas` o una cámara libre). Sin comparar con el ASI.
* **Anomalías vistas**: (1) `Pool.h:207 Falló la reserva para el tipo "class CEntryExit"` una vez por partida (a los ~2 min), sin consecuencias visibles; sin comparar con el ASI. (2) Una línea `Línea de timecyc incorrecta (20 de 51 campos)` (antes 184): es la línea de R\* ya conocida. (3) El vídeo del logo (`Logo.mpg`) sigue sin reproducirse y se evita con `NoLogo`.

## 2. INT3 hallados

**Ninguno.** No hay función original ejecutada en ninguna de las cuatro ejecuciones (más de 21 min de juego en total), así que no había nada que reconstruir (0 de las 5 iteraciones permitidas). Lo que sigue sin ejercitarse: sec. 3.2 y 5.

## 3. Censo

### 3.1 Estático (`scan_obj.py` + `analizar.py`, 1.182 `.obj`, 47.988 referencias)

| | Ronda 3 (hooks.csv) | Ronda 4, con `docs/hooks.csv` | Ronda 4, con ganchos del código fuente |
| --- | ---: | ---: | ---: |
| Funciones del cierre | 652 | 673 | **521** |
| Bytes | 299.480 | 296.965 | **264.358** |
| RenderWare | 447 / 179.052 B | 447 / 173.647 B | **323 / 145.505 B** |
| CRT | 136 / 102.869 B | 157 / 105.759 B | **129 / 101.294 B** |
| Juego | 69 / 17.559 B | 69 / 17.559 B | **69 / 17.559 B** |
| Llamadas indirectas sin resolver / funciones de la IAT | 612 / - | 424 / 52 | **379 / 48** |
| Direcciones citadas sin gancho (A / B / ruido C) | 340 (107/124/109) | 344 (107/125/112) | **223 (49/63/111)** |

`docs/hooks.csv` va retrasado respecto al código (faltan, por ejemplo, `0x80E270`, `0x7FBD30`, `0x7FB650`, que sí están enganchadas con `RH_ScopedGlobalInstall`): da 152 funciones y 33 KB de falso pendiente. La tercera columna usa `reversing/FnFase8/ganchos_fuente.py` (13.964 direcciones de `RH_Scoped*` en `source/`; `docs/CodigoVivoExe/fase8-experimento/ganchos-fuente.csv`), que `analizar.py` suma si existe. La comparación honesta con la ronda 3 es la columna 2 (673 / 296.965: +21 funciones, -2,5 KB; el CRT propio añadió referencias); el estado real es la columna 3.

Por contenido (columna 3, `cierre_codigo-ronda4.csv`): D3DX9 303 funciones / 129.610 B (19 raíces, 2.290 B, citadas por `d3dx9math.obj` y `rpworld.obj`; las otras 284 son internas); CRT 130 / 101.303 B (**10 raíces, 1.776 B**; el resto cuelga de tres bloques de ~28 KB que el mapa trata como funciones, `0x821E8D`, `0x82218D` y `0x82239D`, y del montón de bloques pequeños); libjpeg 6b 67 / 16.938 B; RenderWare restante 20 / 15.895 B (15 raíces, 9.445 B); clump `0x74A770`/`0x74A880` 621 B. libpng + zlib **ya no cuenta** (64 funciones / 21.689 B de la columna 2 están enganchadas en el código). El cierre estático es una cota superior: no sabe qué se ejecuta.

Datos: `docs/CodigoVivoExe/fase8-experimento/resumen-ronda4.json`, `cierre_codigo-ronda4.csv`, `dependencias_codigo-ronda4.csv`.

### 3.2 Dinámico

**0 funciones originales ejecutadas** en partida nueva: carga, apertura de ~107 s (cinemática), ~90 s de control libre, subir a la bici y pedalear, caminar, menú de pausa y volver, y un censo (`--censo`: cada acierto contaría y devolvería 0): `fase8_codigo_original.csv` vacío. Antes: 1 (rondas 2-3: despachador de script, nodos Rx) y 0 con shims. No cubre: conducir un coche, armas, interiores, guardar/cargar, misiones posteriores, radio/audio (no medido), capturas JPEG, vídeo.

## 4. Datos del EXE que siguen copiándose

| Sección | Copiados | Idénticos al archivo tras 100 s | De ellos distintos de cero |
| --- | ---: | ---: | ---: |
| `.rdata` | 308.224 | 298.471 | 197.185 |
| `.data` (parte inicial) | 262.144 | 259.942 | 179.044 |
| `_TEXT_HA` | 68.608 | 68.608 | 50.974 |
| `_rwdseg` | 512 | 510 | 0 |
| `.rsrc` | 1.536 | 1.536 | 607 |
| **Total** | **641.024** | **629.067 (98,1 %)** | **427.810** |

Se **sigue copiando todo** (641.024 B, igual que en la ronda 1): solo 11.957 B (1,9 %) cambian por la ejecución propia o por `DatosExe` (parches de vtable, estado del juego). Los 427.810 B no nulos que no cambian son la cota superior de lo que aún se toma del original (incluye datos que nadie lee). Estáticamente, el código propio cita 3.192 direcciones de datos por dirección fija (31.113 referencias): **739 sin definición en C++** (`.bss` 294, `.data` inicializado 239, constantes 149, `_TEXT_HA` 37, cadenas 13, vtables 7) y 2.453 con definición. Los inicializadores de C++ van por `DatosExe::RunStaticInitializers`; los del CRT por `ExeCrt`. Herramienta: `reversing/FnFase8/datos_copiados_r4.py`.

## 5. Lista priorizada de lo que falta para la independencia

Cero rupturas hoy; lo que sigue es lo que haría falta para quitar el original por completo, de más a menos importante.

1. **Imagen de datos (641.024 B)**: el EXE no arranca sin el `gta_sa.exe` del usuario como archivo. Definir en `DatosExe` los **739 objetos sin definición** y las vtables/constantes de `.rdata` (308 KB: vtables, tablas de punteros de RenderWare/D3D, cadenas) y el `.data` inicializado (262 KB), y copiar solo `.rsrc` (1,5 KB) o recrearlo. Es la dependencia más grande y la única que seguro se usa siempre.
2. **RpWorld: lectura/escritura de streams y mallas** (llamadas directas del C++ al EXE, 8,0 KB): `0x762F30` (980 B, `RpWorldStreamRead`), `0x763310` (663 B, `RpWorldStreamRead`), `0x7624A0` (672 B, `RpWorldStreamWrite`), `0x762740` (533 B), `0x75A960` (2.933 B, `RpBuildMesh`/tristrip), `0x74A880` (397 B, escribir átomo del clump), `0x74A770` (224 B). Salen al cargar o escribir mundos/clumps; no ejercitados.
3. **Módulo de texturas de rwcore** (tablas de punteros de `_rwTextureModuleOpen`, 1,9 KB): `0x7F40F0` (966 B), `0x7F5140` (652 B), `0x7F53D0` (292 B); además `0x7F2930` (234 B, `RwMatrixQueryRotate`).
4. **libjpeg 6b** (67 funciones, 16.938 B; raíces `0x5C3B70` 2.055 B, `0x5C9CE0` 1.067 B, `0x5C8B70` 659 B, etc., rango `0x5C2xxx-0x5CFxxx`): capturas JPEG y texturas JPEG. Ya existe la opción de CMake `GTASA_WITH_LIBJPEG` (apagada) con la libjpeg 6b enlazada en el standalone: activarla para `gta_sa_standalone` (configuración, no reconstrucción).
5. **D3DX9** (303 funciones / 129.610 B, 19 raíces de `d3dx9math.cpp` y `rpworld.cpp`): el ensamblador de shaders propio (`D3dxShaderAssembler`: `0x7725E3` 1.238 B, `0x76BA49`, `0x7716DD`, `0x76BA24`, `0x770835`) llama a código del D3DX9 estático del EXE. Mientras `D3DXAssembleShader` use `d3dx9_43.dll` (como hoy el generador de vertex shaders, que funciona) no se ejecuta; la solución es **no reconstruirlo**: enlazar `d3dx9.lib` y retirar esa vía.
6. **CRT (10 raíces, 1.776 B)**: `0x820A10` (626 B, usada por `_RwD3D9DDSTextureRead`), `0x824570` (466 B, referenciada por `NOTSA_WinMain`), `0x825A79` (346 B, `setlocale`), `0x824046` (129 B, `getenv`), `0x8212ED` (98 B, `operator new`), `0x8244D1` (37 B, `_amsg_exit`), `0x826B26` (26 B), `0x821397` (23 B), `0x825136` (22 B, `_controlfp`), `0x833605` (3 B). Con el CRT de MSVC enlazado la mayoría sobra; revisar quién las cita (varias son direcciones de ganchos del propio `ExeCrt`).
7. **QuickTime** (`QTMLRuntime`, `0x819700`, 108 B): retirar.
8. **Vídeo de logo/intro** (DirectShow + DDraw, hilo de `quartz`): no es código original; hoy se evita con `NoLogo`.
9. **No es código original pero conviene mirar**: `CEntryExit` (reserva del pool fallida), sol y nubes sin verificar, cobertura dinámica de conducción, armas y guardado (sin medir), y validación contra bancos de lo reconstruido recientemente (`SubmitNoLight`, skin y vertex shader tienen su validación; el resto solo humo).

## 6. Cómo reproducir

```
git worktree add --no-checkout -b ronda4-indep D:\Original\wv-r4 391cb7108d    # sparse checkout sin reversing/CppTwelveLevel6Final; junction reversing\CVehicle\conan-cache -> wv-int; lib\*.lib copiados de otro worktree
bash reversing/FnFase8/enlazar_r4.sh                                           # enlaza con los .obj de wv-int (EXIT=0); ajustar rutas si el worktree se llama distinto
pwsh -File reversing/FnFase8/ejecutar_f8b.ps1 -Etiqueta r4a -Limite 330 -Extra ""            # partida nueva
OBS_FILTRO=wv-r4 python reversing/FnFase8/observar_jugador.py r4a 320 --entrada --modo tecla  # en paralelo: control libre y entrada
python reversing/FnFase8/cobertura_r4.py r4d 300                               # bici + pausa; cielo_r4.py y datos_copiados_r4.py son análogos
python -I reversing/FnFase8/gen_tabla_ret.py "<gta_sa.exe>"                    # y -Extra:"--censo --tabla-ret ...\salida\retn.bin" para el censo
python -I reversing/FnFase8/ganchos_fuente.py . ; python -I reversing/FnIND0A/scan_obj.py --obj-dir <obj> --out reversing/FnIND0A/datos/refs_codigo.csv ; python -I reversing/FnIND0A/analizar.py --exe "<gta_sa.exe>"
```

Nota: `ejecutar_f8b.ps1` necesita `-Extra:"--censo ..."` (con dos puntos) cuando el valor empieza por `--`. `cl.rsp` (rutas de `wv-fase8c`) hay que reescribirlo para otro worktree con `sed`, sin commitearlo.
