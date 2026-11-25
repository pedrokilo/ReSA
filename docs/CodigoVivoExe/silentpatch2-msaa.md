# SilentPatch, segunda tanda: opciones de Anti-Aliasing (MSAA) de 2x a 16x y costuras del mapa (rama `silentpatch2-msaa`, 25/11/2026)

Fuente: SilentPatch para SA (MIT, Copyright (c) 2024 Adrian Zdanowicz «Silent»; `CHANGELOG-SA.md` L66 y L155, `SilentPatchSA\SilentPatchSA.cpp` `namespace MSAAFixes` (líneas 1382-1417) y el bloque «Fixed MSAA options» (línea 7448); atribución en [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Se aplica **reconstruyendo el efecto en nuestras funciones C++**, no con parches de bytes. Clasificación por viñeta: [silentpatch-clasificacion/msaa.csv](silentpatch-clasificacion/msaa.csv). Desviaciones registradas en [DesviacionesConocidas.md](../DesviacionesConocidas.md), sección 15.M.

## Qué hace el EXE original y qué hace SilentPatch

El driver D3D9 de RenderWare (nuestra reconstrucción en `source/game_sa/RenderWare/rw/rwcore.cpp`) guarda el **número de muestras** como nivel: `_rwD3D9FindMSLevels` (0x7F6BF0) prueba `CheckDeviceMultiSampleType` con los tipos 2..16 y deja en 0x8E242C el **mayor tipo admitido** (con un NVIDIA moderno, 8); `RwD3D9EngineSetMultiSamplingLevels` (0x7F84F0) y `RwD3D9ChangeMultiSamplingLevels` (0x7F8A90) recortan el pedido a ese máximo y bajan de uno en uno hasta un tipo admitido; ese número va directo a `D3DPRESENT_PARAMETERS.MultiSampleType`. El menú (`CMenuManager::m_nPrefsAntialiasing`, offset 0xC8, es lo que se guarda en `gta_sa.set`; `m_nDisplayAntialiasing`, 0xCC, es el que se edita) usa ese mismo número como valor del ajuste, con un **tope fijo de 4** en las flechas (0x57D0DF y 0x57D126) y el texto `%d` de `valor - 1` (0x57A0E0): se ve «1», «2», «3» para 2, 3 y 4 muestras; el 3 no existe en las tarjetas actuales y el driver lo baja a 2 (de ahí «2x/2x/4x» del changelog). Además `_rwD3D9FindMSLevels` sustituye el máximo por el número de niveles de calidad *no enmascarables* si es mayor (parches `jbe -> jmp` de SilentPatch en 0x7F6C9B, 0x7F60C6 y 0x7F6683: en nuestra reconstrucción las tres copias son una sola función).

SilentPatch reinterpreta el campo como un **índice de nivel** (1 = apagado, 2 = 2x, 3 = 4x, 4 = 8x, 5 = 16x; muestras = `1 << (nivel - 1)`), calcula el tope del menú a partir del máximo de muestras del driver, convierte índice a muestras en las cuatro llamadas al driver (0x5744FD, 0x57D162, 0x57D2A6, 0x746350) y cambia el texto a `%ux` (`1 << (valor - 1)`).

## Qué hace nuestro código

- Opción `MSAAFix` (SilentPatchSA.ini no tiene clave para ella; siempre activa en SilentPatch): por defecto **encendida**; se apaga con `GTASA_SILENTPATCH=0`, `GTASA_SILENTPATCH_MSAAFIX=0` o `SilentPatch_MSAAFix=0` en `gta-reversed.ini`. Apagada, el código del EXE se conserva literal en cada sitio (ciclo 1-2-3-4 con tope fijo, texto `%d`, número de muestras directo).
- `source/extensions/SilentPatchMSAA.h` (solo cabecera, funciones `inline`): `Enabled()`, `MaxLevelFromSamples()` (mayor potencia de dos <= máximo del driver, tope 5), `ClampLevel()`, `SamplesFromLevel()` y `DriverSamples()` (indice -> muestras si la opción está encendida, el valor tal cual si no).
- `CMenuManager::ProcessPCMenuOptions` (`MENU_ACTION_ANTIALIASING`, `MenuManager_Process.cpp`): sin el tope 4; las flechas recorren 1..máximo de la tarjeta con vuelta; al aplicar, `RwD3D9ChangeMultiSamplingLevels(DriverSamples(valor))`; igual en «Restablecer valores por defecto» de la pantalla avanzada.
- `CMenuManager::DrawStandardMenus` (`MenuManager_Draw.cpp`): texto `%ux` (`2x`, `4x`, `8x`, `16x`), «No» para el nivel 1.
- `CMenuManager::Initialise` (`MenuManager.cpp`): `RwD3D9ChangeMultiSamplingLevels(DriverSamples(prefs))`.
- `psSelectDevice` (`WinPs.cpp`): acota el valor guardado a 1..máximo de la tarjeta (los niveles ya se consultaron al abrir el motor), copia el valor acotado a `m_nDisplayAntialiasing` y llama a `RwD3D9EngineSetMultiSamplingLevels(DriverSamples(prefs))`.
- `_rwD3D9FindMSLevels` (`rwcore.cpp`): con la opción encendida el máximo no lo sustituye el número de niveles no enmascarables (los tres parches `jbe -> jmp`).
- **Compatibilidad con `gta_sa.set`**: el formato no cambia (el mismo `int32`). Un valor guardado por el EXE o por la opción apagada (1, 2, 3 o 4 muestras) se lee como índice: el 2 sigue siendo 2x, pero el 3 (que era «3x», en la práctica 2x) pasa a 4x y el 4 (4x) pasa a 8x, igual que en SilentPatch. Un valor mayor que lo que admite la tarjeta (o fuera de 1..5) se acota al máximo (ver prueba de acotado).
- Desviación respecto a SilentPatch: el tope del menú usa la mayor potencia de dos <= máximo del driver (`std::bit_width`), no `_BitScanForward` (bit más bajo, que con un máximo no potencia de dos, p. ej. 6, daría 2x en vez de 4x). Con los máximos habituales (2, 4, 8, 16) coincide.

## Pruebas (standalone de esta rama, sonda `msaa`, `reversing/FnJuego/sonda_juego.py`)

GPU usada: **NVIDIA GeForce RTX 3050 Laptop GPU** (controlador `nvldumd.dll`, vendedor 0x10DE; línea `Adaptador de vídeo D3D9` de `log.log` en cada ejecución, y `nvldumd.dll`/`nvd3dum.dll` cargados). Hay también una Intel Xe en el portátil (`igd12umd32.dll` cargado) pero el standalone exporta `NvOptimusEnablement` y el dispositivo D3D9 es el de NVIDIA. Niveles admitidos por esa tarjeta (leídos de las variables del driver, 0x8E242C y siguientes): máximo **8** muestras (2, 4 y 8; el tipo 3 no se admite: con la opción apagada el «3» pide 3 y el driver lo baja a 2); máximo no enmascarable 5. **16x no lo admite esta GPU**, así que el índice 5 (16x) no se pudo ejercitar: «implementado, no ejercitado» (la lógica de `MaxLevelFromSamples` lo cubre para un máximo de 16).

Cada ejecución se hizo con `reversing/FnJuego/ejecutar_juego.ps1` (candado `D:\Original\gta_lock` tomado por el propio script, carpeta `D:\Original\_juego_run`), 1024x768 en ventana; el menú se maneja con teclado real (SendInput): pausa -> Opciones -> Config. pantalla -> Avanzado -> Anti Aliasing; las capturas son de la **ventana completa con BitBlt desde el DC de la pantalla** (`CAPTUREBLT`, proceso DPI-aware, tamaño del cliente comparado con `RsGlobal` 0xC17044/0xC17048). `PrintWindow` (el de `captura.capturar`) devuelve un fotograma viejo después del `Reset` del dispositivo (primera tentativa: las capturas «de después» salieron idénticas byte a byte), por eso las pruebas de MSAA usan `foto_bb`.

### L66: opciones y niveles (ejecución `reversing/FnJuego/ejecuciones/msaa_on-20261125-123150`, opción encendida)

- Valor guardado de partida: 2 (la copia de pruebas lo había dejado ahí). Ciclo con la flecha derecha (`CICLO del ajuste` en `sonda_juego.log`): **2, 3, 4, 1, 2** (1 = «No», 2 = «2X», 3 = «4X», 4 = «8X»); flecha izquierda desde el inicial (2): 1; el tope es el 4 (8x) = máximo del driver.
- Texto del ajuste: capturas `aa_ciclo*_valor*.png` (p. ej. `aa_ciclo1_valor3.png` muestra «4X»).
- Cada nivel aplicado con Intro (`NIVEL n aplicado` en el log; variables leídas de la memoria del proceso, `sel` = muestras elegidas, `present` = `D3DPRESENT_PARAMETERS.MultiSampleType` en 0xC9C050 tras el `Reset`):

| Nivel del menú | Texto | `prefs` | muestras elegidas (0x8E2430) | tipo de multimuestreo en los parámetros de presentación |
| --- | --- | --- | --- | --- |
| 1 | No | 1 | 1 | 0 (sin MSAA) |
| 2 | 2X | 2 | 2 | 2 |
| 3 | 4X | 3 | 4 | 4 |
| 4 | 8X | 4 | 8 | 8 |

- El juego sigue vivo tras cada `Reset`.
- Comparación de bordes (capturas `aa_n{1,2,3,4}_mundo.png` y `_mapa.png` de la misma ejecución, recorte al área de dibujo 1024x768; métricas `METRICA` del log, número de colores distintos en la región = más colores intermedios = más suavizado): mundo 3067 (sin AA), 3672 (2x), 5325 (4x), 7791 (8x) colores; los pares de vecinos «duros» (salto de luminancia > 80) bajan 248, 225, 211, 204; mapa del menú 48807, 49829, 50712, 50971 colores y pares duros 21570, 17639, 17170, 17172. A ojo, el recorte ampliado de CJ muestra los escalones del borde del gorro y la espalda sin AA y bordes suaves con 8x (recortes `recorte_cj_sinAA_x5.png` y `recorte_cj_8x_x5.png` en la carpeta de la ejecución, generados con `reversing/FnJuego/recorte.py`; `margen.py` y `difmapa.py` miden los márgenes del mapa y la diferencia entre dos capturas). La escena no es perfectamente estática (peds/cámara), por eso las métricas del mundo son orientativas; la serie es monótona con la opción encendida.
- **Opción apagada** (`GTASA_SILENTPATCH_MSAAFIX=0`, ejecución `msaa_off-20261125-124607`): valor guardado 4 (4x del EXE); ciclo **4, 1, 2, 3, 4** (tope 4, igual que el EXE, texto «1», «2», «3»), niveles: 1 -> 1 muestra, 2 -> 2, 3 -> **muestras elegidas 2 / tipo presente 2** (el 3x pedido se baja a 2: el «2x/2x/4x» del changelog), 4 -> 4; mismo comportamiento que el EXE.
- La ventana del standalone crece a 1433x1075 de cliente tras cada `SetVideoMode` (dibuja en la esquina de 1024x768): **ocurre también con la opción apagada** (`msaa_off`) y con cualquier ajuste de este menú, no lo causa MSAAFix; no se tocó. Por eso las métricas recortan al área de `RsGlobal`.

### Acotado del valor guardado (ejecución `msaa_clamp9`, `ejecutar_msaa.ps1 -PrefsAA 9`)

Con el campo `m_nPrefsAntialiasing` de `gta_sa.set` forzado a 9 antes de arrancar (el script copia el archivo, lo parchea, y lo restaura al terminar): resultado (`msaa_clamp9-20261125-130506/sonda_juego.log`, línea `ESTADO TRAS ARRANCAR`): `prefs` 4, `display` 4, muestras elegidas 8, tipo de multimuestreo presente 8 (el 9 se acota a 4 = 8x, el máximo de la tarjeta); en el menú el ajuste aparece en 8x (`display en el menu 4`). El script dejó `gta_sa.set` como estaba antes de la ejecución.

### L155: costuras de 1 píxel en el mapa con AA (carpetas `msaa_on-20261125-123150`, `msaa_seams0-20261125-125337`, `msaa_seams0_aaoff-20261125-125703`)

En la pantalla del mapa (pausa -> Mapa), con **MSAA 8x y `MapSeamsFix=0`** (`GTASA_SILENTPATCH_MAPSEAMSFIX=0`) hay costuras reales: la fila superior de la pantalla (y = 0) tiene **837 píxeles no negros** (el margen negro de arriba debería ser negro en todas las filas) y las columnas 48-59 del margen izquierdo tienen un píxel distinto del fondo; con `MapSeamsFix=1` (por defecto) **no hay ninguno** en los niveles 1x, 2x, 4x y 8x (`aa_n{1..4}_mapa.png`: márgenes superior e izquierdo completamente negros). Con AA apagado y `MapSeamsFix=0` tampoco hay costura (la captura es idéntica píxel a píxel a la de `MapSeamsFix=1` con AA apagado), es decir, el defecto solo existe con AA, como dice el changelog.

## Humo (ASI de esta rama sobre el EXE original) y ejecución de 120 s

Compilación y enlace: los 5 archivos cambiados con `reversing/FnFase8/compilar_over.cmd` (bajo `D:\Original\compile_lock`), standalone con `reversing/FnFase8/enlazar.cmd` usando como tercer argumento los `.obj` de `D:\Original\wv-intuild\source\gta_reversed.dir\Debug` (1197 objetos, 5 de `over`; se copiaron a `reversing/FnFase8/lib` las tres `.lib` de `wv-int` porque el árbol esparso no las trae), ASI con el patrón de `_build_INT1_urgente.sh` adaptado (`_enlazar_asi_sp7b.py`, mismos `.obj` más los 5 de `over`).

- `python contrib/audit_hook_catalog.py --strict`: **OK**. `python contrib/auditoria_datos/tamanos_objetos.py lint`: **OK** (2134 objetos, 0 fallos).
- Humo con la ASI de esta rama sobre el EXE original (`AGUA_ASI=D:\Original\wv-sp7beversing\FnFase8si\gta_reversed.asi`, desde `wv-8eversing\FnBIS1`): `python prueba_ropero.py cal_mal.args sp7b_ropero` -> `puerta True marcador True`, **VEREDICTO BIEN**; `python prueba_marcadores.py cal_mal.args sp7b_marcadores` -> `marcadores_en_memoria 12 marcadores_que_cambian_en_1s 12`, **VEREDICTO BIEN** (salidas en `D:\Original\_humo_sp7b_ropero.log` y `_humo_sp7b_marc.log`).
- Ejecución del standalone de 120 s: `reversing/FnJuego/ejecuciones/msaa_120s_d-20261125-132035` (escenario `espera`: CJ parado en el mundo, 143 s de ejecución, `excepciones=0` en el último latido de `fase8_arranque.log`, proceso vivo hasta el final, peds 28-33 y vehículos 13-16 en los pools). **Otras cinco ejecuciones largas de la misma tanda (`msaa_120s`, `_b`, `_c`, `msaa_seams0_aaoff`, `msaa_clamp9`) mostraron `excepciones=1` desde el arranque (2,3 s)**: una excepción 0xC0000005 dentro de la ventana de arranque en `GetVideoMemInfo` -> `d3d9` -> `d3d9on12` -> `igd12um32xel!...` -> `FreeLibrary` -> `ntdll!RtlImageNtHeaderEx` (el controlador de Intel de la gráfica integrada sondea un módulo; la captura el propio controlador y el juego sigue). Es intermitente y no depende de MSAA ni de esta rama: las ejecuciones cortas `msaa_mio_a/b` y `msaa_main_a/b`/`msaa_base_main` (esta rama y el standalone de `wv-int`) dieron 0, y los registros de otras ramas ya la traen (`base_fps-20261125-061947/log.log`, `gr_on-*`, `luna3_off-*`, entre otros). Se repitió hasta tener una de 120 s con 0.

## Qué no se pudo verificar

- 16x: la GPU de pruebas (NVIDIA RTX 3050 Laptop) solo admite hasta 8x; el índice 5 se acota al máximo por código (`ClampLevel`) pero no se ejercitó con hardware que lo admita.
- GPU Intel/AMD (otros máximos y la diferencia de `NONMASKABLE`): no probadas.
- Pantalla completa exclusiva: solo se probó en ventana (los parámetros de presentación del modo exclusivo usan el mismo `MultiSampleType`).
- Espejos con AA (L64, otra rama) y reflejos: no comprobados aquí.
- Las capturas son comparaciones entre ejecuciones con peds en movimiento; las métricas de bordes son orientativas y la prueba visual la dan los recortes y el tipo de multimuestreo de los parámetros de presentación.
