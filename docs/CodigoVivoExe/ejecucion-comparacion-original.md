<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prueba de humo estándar: standalone contra el juego ORIGINAL puro (24/11/2026)

Rama `comparacion-original` (worktree `D:\Original\wv-comp`, base `integracion-fase7b`). Herramientas en `reversing/FnComparar/`, evidencia en `reversing/FnComparar/evidencia/` (28 MB). Standalone probado: `D:\Original\wv-int\reversing\FnFase8\salida\gta_sa_standalone.exe` (enlazado 23/11 23:21; es el binario que había, no se recompiló). Original: `gta_sa.exe` de la copia de pruebas (SHA256 `72ae59e4…ecc9b`).

## 1. Resumen honesto

* **Interior de la casa de CJ y marcadores: iguales.** Mismos conjuntos de entidades (44 en la casa, 15 en el ropero, mismos modelos, mismo estado RenderWare y mismas texturas), mismos pools (9426 edificios, 97 objetos, 1 ped, 0 coches), mismos marcadores (10 y 12), color medio idéntico en la casa (diferencia 0,0 por canal) y puerta 2877 del ropero con objeto RenderWare cargado en los dos.
* **Exterior con clima despejado (0, 1) y nublado (4): iguales** salvo un par: 31 de 36 con diferencia de color medio del canal verde ≤ 3, otros cuatro entre 4,6 y 11,0 (mediana por clima 0,9 a 1,6). Cielo, palmeras, edificios, vallas, textos del HUD, reloj, radar y sombras coinciden; solo cambian coches y peatones aleatorios (ruido esperado) y la animación de CJ.
* **Diferencia real 1 (reproducible, no corregida): el espejo del ropero.** En el original el espejo muestra el reflejo oscuro de la habitación; en el standalone muestra un panel claro con marcos anidados. Reproducido en dos ejecuciones del standalone (`s_int2`, `s_int3`: +2,5 de color medio, 4,3 de diferencia absoluta; en bloques de 100x100 píxeles la zona del espejo difiere 23-36, el resto de la imagen 0,2-6). El original con el ASI completo (`a_int2`) da el espejo idéntico al original (diferencia media 0,0). Por tanto la causa no está en el C++ de `CMirrors` (que corre igual en el ASI) sino en lo que solo cambia en el standalone (capa RenderWare/D3D9 elevada: render a textura de cámara, o datos propios `DatosExe` frente a lecturas por dirección). **No se identificó la función exacta ni se corrigió**: faltó tiempo para biseccionar dentro del standalone (no admite `--unhook`).
* **Diferencia 2 (intermitente, sin explicar): lluvia (clima 8).** 5 de 12 pares con |diferencia| > 10 y signo mixto (std más oscuro en 2, más claro en 3; el peor, `p3_h18_c08`: 130/136/123 original frente a 173/176/162 standalone). El `CTimeCycle` (volcado de 256 bytes) coincide entre los dos en esas escenas salvo diferencias de interpolación del reloj de 2 % o menos, y la intensidad de lluvia leída es la misma, así que no es el reloj ni el ciclo de color. Sospecha no comprobada: destello de relámpago u otra variable de clima que el standalone guarda en objetos propios (`CWeather::LightningFlash` es `DatosExe::Bss::bC812CC`, no legible por dirección en el standalone). No hay medida que lo confirme.
* **Un par claro (`p4_h06_c01`, amanecer despejado) difiere -9/-11/-12** con el mismo reloj; no investigado (podría ser un destello de sol; sin dato).
* **Carga:** el standalone pasa de `gGameState` 8 a 9 en 2,2 s; el original en 11,5 s; el ASI (FastLoader) en 8,6 s. Tiempo total hasta `gGameState = 9` desde el inicio del proceso: standalone 4,2 s, original 51 s (21 s de logos/intro más el menú por teclado), ASI 13,3 s. No son comparables como arranque (el original recorre logos y menú); la carga 8→9 sí lo es y el standalone es unas 5 veces más rápida con las mismas entidades al final (sin explicar; no se midió qué omite).
* **FPS:** 24,4-25,2 en los dos (limitador de fotogramas del juego a 25), 48 escenas.
* **Log del standalone:** solo avisos esperados: el error/aviso de partida de otra versión (forzada por `LoadSavesWithMismatchingVersion`) y un aviso de `Automobile.cpp:6983` ("Intentando dañar el componente 11 de blistac"). Sin otros errores.

## 2. Método

**Mismo escenario, mismos ajustes.** Los dos se ejecutan con una copia de la partida 2 y de `gta_sa.set` de `E:\JUEGOS\GTA San Andreas - CVehicle Test\userfiles` (copiada otra vez en cada ejecución). Ventana de **800x600 de cliente en ambos** (el original muestra los logos y el menú a 640x480 y la partida a 800x600; se supone que el modo sale del `gta_sa.set` copiado, campo no verificado; no se probó 640x480 ni 1024x768). Carpetas: `D:\Original\_comparar\orig` y `\std`, con junctions de solo lectura a `anim audio data models movies text` de la copia de pruebas.

**Original puro.** `gta_sa.exe` sin `gta_reversed.asi` ni `gta-reversed.ini`. Lleva `dinput8.dll` y en `scripts\` solo dos calzas que no son código del proyecto:
* `III.VC.SA.WindowedMode.asi` (ThirteenAG, el de `D:\Grand Theft Auto San Andreas\Windowed Mode`): sin ventana, el original corre a pantalla completa exclusiva.
* `redir_docs.asi` (`reversing/FnComparar/shim/redir_docs.cpp`, escrito aquí): parchea la IAT de `RegQueryValueExA` (0x85800C) para que la carpeta "Personal" apunte a `_comparar\orig\docs`. Sin esto el EXE lee y escribe las partidas y `gta_sa.set` reales de Mis documentos (OneDrive). No toca ninguna función del juego.

El original carga la partida 2 sin más (empíricamente): la comprobación de versión del guardado (`LoadSavesWithMismatchingVersion`) es un añadido NOTSA del proyecto; en el desensamblado de `GenericLoad` (0x5D17B0) la llamada a `GetCurrentVersionNumber` (0x5D18E6) no va seguida de ninguna comparación. El arranque se hace con teclado (`SendInput` con códigos de barrido): Enter en los logos y, en el menú, Enter, Abajo, Enter, Abajo, Enter, Enter (cada tecla se repite si el estado del `CMenuManager` no cambia).

**Standalone.** FastLoader ranura 2, `--shims` (como los otros lanzadores), carpeta propia.

**Control EXE original + ASI (`asi`).** `gta_reversed.asi` de `wv-int\bin\Debug` (23/11 23:13), ini en la raíz y en `scripts\` (sin la copia en `scripts\` carga una partida nueva: error del primer intento `a_int1`, descartado).

**Puntos y condiciones.** (1) casa de CJ (partida cargada sin tocar, y el ropero pisando el marcador CHANGER hasta área 14), (2) (2652,-1720), (3) (2672,-1652), (4) (2658,-1434) con las alturas de `sonda_traf.py`. Hora 6/12/18/0 por memoria (`CClock` 0xB70153/0xB70152) y clima 0/1/4/8 (`CWeather::ForcedWeatherType` 0xC81318), 4 s de asentamiento, 14 s tras teletransportar. La cámara se fija escribiendo los ángulos de las tres `CCam` de `TheCamera` (`m_fHorizontalAngle` +0xBC, `m_fVerticalAngle` +0xAC, ...) en un hilo a 50 Hz que sigue activo durante la captura; objetivo yaw 90° (mirando a +Y), pitch pedido +0,10 (el juego lo deja en z = -0,05 en los dos). `orient_ok` está en `resultados.jsonl`; en las 48 escenas finales fue verdadero en los dos.

**Captura.** Cliente completo de la ventana, proceso DPI-aware, `BitBlt` de la pantalla con la ventana al frente y `TOPMOST`, tamaño validado contra `GetClientRect` (800x600 en todas; `valida: true`). `PrintWindow` devuelve negro con el D3D9 ventaneado del original (0,3 de media en la mayoría de fotogramas), por eso se usa `BitBlt` en los dos; en el standalone `PrintWindow` y `BitBlt` coinciden (p. ej. 61,9/56,0/42,5 y 61,9/55,9/42,4), que valida las coordenadas. PNG propio sin dependencias (`png_util.py`; PIL no está instalado).

**Medidas por memoria** (por escena en `resultados.jsonl`): pools de peds, vehículos, edificios, objetos y dummies (usados, con RW cargado, a menos de 120 m), `NumRandomCars`, marcadores (pool 0xC7DD58), reloj, clima, fotogramas por segundo (`CTimer::m_FrameCounter` 0xB7CB4C en intervalos de 2-3 s), `gGameState` con marca de tiempo, y volcados crudos de `CTimeCycle::m_CurrentColours` (0xB7C4A0), gamma (0xC92134) y `CMenuManager` (0xBA6748). Entidades a 12 m (interior) y 60 m (exterior) con modelo, estado RW y nombres de textura en `*_entidades.json`.

## 3. Interior (casa de CJ y ropero)

Original frente a standalone `s_int3` (`evidencia/int_*.png`, a tamaño completo; original a la izquierda).

| escena | dif. media RGB (std-orig) | dif. abs. media | media orig | media std | cielo orig / std | veh o/s | peds o/s | rnd o/s | fps o/s |
|---|---|---:|---|---|---|---|---|---|---|
| casa_h12 | 0/0/0 | 2.3 | 70/64/47 | 70/64/47 | 85/79/60 / 85/78/60 | 0/0 | 1/1 | 0/0 | 25/25 |
| casa_hsave | 0/0/0 | 1.7 | 62/56/42 | 62/56/42 | 75/69/55 / 75/69/54 | 0/0 | 1/1 | 0/0 | 25/25 |
| ropero | 3/2/2 | 4.3 | 110/93/79 | 113/95/81 | 125/118/105 / 129/121/108 | 0/0 | 1/1 | 0/0 | 25/25 |

Descripción de lo que se ve (igual en los dos salvo lo indicado): `casa_hsave` (reloj 00:13): CJ de espaldas con gorra verde en la cocina del salón de Grove Street, marco de madera de la puerta con luz amarilla de la flecha de misión, botella y mesa a la izquierda, cocina a la derecha, HUD con puño, barra de salud, `$999999999` y radar. `casa_h12`: igual con hora 12:xx y algo más de luz. `ropero` (reloj 12:17): CJ delante del vestidor, puerta de madera con marcador rojo, dos pufs, tabla de surf, radio y cómoda; el espejo central es la única zona distinta (sección 1, diferencia real 1; recorte ampliado en `ropero_espejo_orig_asi_std.png`: izquierda original, centro ASI, derecha standalone).

Control con el ASI completo contra el original (`evidencia/int_asi_*.png`, el panel derecho es el ASI):

| escena | dif. media RGB (std-orig) | dif. abs. media | media orig | media std | cielo orig / std | veh o/s | peds o/s | rnd o/s | fps o/s |
|---|---|---:|---|---|---|---|---|---|---|
| casa_h12 | 0/0/0 | 1.9 | 70/64/47 | 70/64/47 | 85/79/60 / 85/78/60 | 0/0 | 1/1 | 0/0 | 25/25 |
| casa_hsave | 0/0/0 | 1.6 | 62/56/42 | 62/56/42 | 75/69/55 / 75/69/54 | 0/0 | 1/1 | 0/0 | 25/25 |
| ropero | 0/0/0 | 1.9 | 110/93/79 | 110/93/78 | 125/118/105 / 125/118/105 | 0/0 | 1/1 | 0/0 | 25/25 |

Nota sobre la ejecución `s_int1` (primera del standalone): salió ~13 de color medio **más oscura** en toda la escena 3D (0,85 de factor, sin tocar el HUD; `evidencia/int_s1_oscura_*.png`). No se repitió en `s_int2` ni en `s_int3`. Sin explicar; sospecha de estado de gamma/pantalla heredado de la ejecución de otro lanzador anterior (el volcado de gamma no se hacía todavía). Se deja constancia porque una medida de brillo aislada del standalone puede engañar.

## 4. Exterior: 48 pares

Original (`o_ext4` para climas 0, 1 y `o_ext6` para 4, 8) frente a standalone (`s_ext1` y `s_ext6`). `evidencia/ext_*.png`: lado a lado con el original a la izquierda; tamaño completo si la diferencia absoluta media pasa de 25, a media escala en el resto. La columna de diferencia es standalone menos original (media del cliente completo). `veh/peds/rnd` son vehículos y peds usados en los pools y `NumRandomCars`.

Resumen por clima (12 pares cada uno, diferencia de color medio del canal verde): despejado 0: mediana 0,9, máx. 4,6, 11 pares ≤ 3; soleado 1: mediana 1,6, máx. 11,0 (`p4_h06_c01`), 9 ≤ 3; nublado 4: mediana 1,0, máx. 3,1, 11 ≤ 3; lluvia 8: mediana 2,4, máx. 40,9, 7 ≤ 3 y 5 con más de 10.

| escena | dif. media RGB (std-orig) | dif. abs. media | media orig | media std | cielo orig / std | veh o/s | peds o/s | rnd o/s | fps o/s |
|---|---|---:|---|---|---|---|---|---|---|
| p2_h00_c00 | 0/-1/0 | 8.0 | 41/51/40 | 41/51/39 | 20/50/45 / 19/48/44 | 14/13 | 32/30 | 10/9 | 25/25 |
| p2_h00_c01 | 0/0/0 | 7.4 | 33/46/42 | 33/47/42 | 24/36/38 / 23/36/37 | 13/13 | 26/25 | 9/9 | 25/25 |
| p2_h00_c04 | 0/1/0 | 3.4 | 46/51/40 | 46/52/40 | 30/38/36 / 30/38/36 | 14/15 | 31/28 | 10/11 | 25/25 |
| p2_h00_c08 | 12/14/14 | 14.9 | 61/62/48 | 73/76/62 | 50/52/42 / 66/69/60 | 13/13 | 24/19 | 9/9 | 25/25 |
| p2_h06_c00 | 0/0/0 | 17.8 | 104/94/74 | 104/94/74 | 174/180/163 / 172/180/163 | 13/12 | 19/16 | 9/8 | 25/25 |
| p2_h06_c01 | 0/0/0 | 19.5 | 103/92/70 | 103/92/70 | 172/179/161 / 171/179/161 | 13/13 | 27/30 | 9/9 | 25/25 |
| p2_h06_c04 | 0/0/-1 | 4.0 | 55/58/45 | 54/58/44 | 46/54/48 / 44/52/46 | 13/13 | 17/16 | 9/9 | 25/25 |
| p2_h06_c08 | -20/-23/-25 | 26.9 | 77/84/73 | 57/61/48 | 78/87/81 / 45/49/40 | 13/12 | 28/24 | 9/8 | 25/25 |
| p2_h12_c00 | -1/-1/0 | 16.7 | 66/82/85 | 65/82/85 | 90/151/179 / 89/151/180 | 16/16 | 37/33 | 12/12 | 25/25 |
| p2_h12_c01 | -1/-1/-1 | 17.7 | 63/96/86 | 62/94/85 | 72/176/180 / 72/176/180 | 16/16 | 36/32 | 12/12 | 25/25 |
| p2_h12_c04 | -1/-1/-1 | 6.7 | 109/116/112 | 108/115/111 | 174/195/192 / 173/194/191 | 16/16 | 37/32 | 12/11 | 25/25 |
| p2_h12_c08 | 2/2/3 | 13.3 | 98/102/97 | 100/104/100 | 126/131/127 / 131/136/133 | 16/13 | 30/23 | 12/9 | 25/25 |
| p2_h18_c00 | -1/0/0 | 17.4 | 65/84/87 | 64/84/87 | 91/153/182 / 91/153/182 | 16/16 | 37/33 | 12/12 | 25/25 |
| p2_h18_c01 | 0/0/0 | 18.8 | 75/98/85 | 75/98/84 | 104/184/178 / 105/184/178 | 16/16 | 34/32 | 12/12 | 25/25 |
| p2_h18_c04 | 0/0/0 | 7.0 | 100/108/106 | 100/108/106 | 160/183/186 / 162/183/186 | 15/16 | 36/31 | 12/12 | 25/25 |
| p2_h18_c08 | 0/0/0 | 10.9 | 88/99/89 | 88/99/90 | 114/125/116 / 115/126/118 | 15/16 | 27/24 | 11/12 | 25/25 |
| p3_h00_c00 | 2/2/1 | 10.7 | 63/65/48 | 65/66/49 | 34/57/46 / 34/58/47 | 15/16 | 19/24 | 9/10 | 25/25 |
| p3_h00_c01 | 8/7/10 | 13.4 | 48/60/53 | 57/66/63 | 30/49/47 / 29/50/48 | 15/17 | 21/22 | 9/11 | 25/24 |
| p3_h00_c04 | -5/-3/-2 | 8.4 | 71/68/51 | 66/66/49 | 42/53/43 / 43/53/43 | 16/15 | 24/21 | 10/9 | 25/25 |
| p3_h00_c08 | 12/10/7 | 18.7 | 72/73/57 | 84/83/64 | 68/75/61 / 69/76/62 | 16/14 | 18/20 | 10/8 | 25/25 |
| p3_h06_c00 | 7/5/3 | 12.6 | 109/91/67 | 116/95/70 | 136/141/125 / 133/141/124 | 16/15 | 21/21 | 10/9 | 25/25 |
| p3_h06_c01 | 4/3/2 | 11.4 | 111/90/63 | 114/93/65 | 134/140/122 / 132/140/121 | 15/17 | 20/26 | 9/11 | 25/24 |
| p3_h06_c04 | -2/-1/0 | 7.6 | 77/74/55 | 74/73/54 | 56/66/53 / 56/66/53 | 14/17 | 20/23 | 8/11 | 25/25 |
| p3_h06_c08 | 0/1/2 | 11.2 | 76/77/58 | 76/78/60 | 54/63/50 / 59/68/55 | 16/17 | 24/22 | 10/11 | 25/25 |
| p3_h12_c00 | -1/-2/-4 | 12.7 | 124/118/104 | 122/116/100 | 111/146/155 / 108/145/152 | 15/18 | 24/30 | 9/12 | 25/25 |
| p3_h12_c01 | -3/-6/-6 | 14.4 | 122/132/110 | 119/126/104 | 93/160/152 / 91/158/151 | 17/17 | 25/26 | 11/11 | 25/25 |
| p3_h12_c04 | 1/2/3 | 10.7 | 149/147/140 | 150/149/143 | 168/185/181 / 168/185/182 | 16/18 | 27/29 | 10/12 | 25/25 |
| p3_h12_c08 | -4/-3/-2 | 14.9 | 136/133/124 | 132/131/122 | 130/138/132 / 124/131/125 | 17/15 | 24/21 | 11/9 | 25/25 |
| p3_h18_c00 | 0/-1/-4 | 12.2 | 119/116/106 | 120/115/102 | 105/141/156 / 103/140/154 | 16/15 | 27/27 | 9/9 | 25/25 |
| p3_h18_c01 | -1/-2/-3 | 12.5 | 126/129/106 | 124/126/103 | 116/167/152 / 114/166/150 | 17/17 | 24/29 | 11/11 | 25/25 |
| p3_h18_c04 | 2/3/4 | 10.2 | 139/137/132 | 141/140/136 | 157/174/174 / 158/174/175 | 16/17 | 28/26 | 10/11 | 25/24 |
| p3_h18_c08 | 43/41/39 | 51.4 | 130/136/123 | 173/176/162 | 117/131/121 / 185/200/187 | 16/17 | 23/26 | 10/11 | 25/25 |
| p4_h00_c00 | -2/-2/-2 | 6.3 | 46/50/39 | 44/48/37 | 59/62/49 / 57/61/47 | 14/13 | 32/32 | 13/12 | 25/25 |
| p4_h00_c01 | 0/0/0 | 4.3 | 30/46/44 | 30/46/44 | 39/57/54 / 40/57/53 | 13/11 | 30/29 | 12/10 | 25/25 |
| p4_h00_c04 | -1/-1/-1 | 6.7 | 47/54/42 | 46/53/41 | 62/67/51 / 61/64/48 | 13/10 | 25/29 | 12/9 | 25/25 |
| p4_h00_c08 | 0/0/0 | 10.2 | 60/66/53 | 60/65/52 | 78/80/63 / 74/75/58 | 12/11 | 21/28 | 11/10 | 25/24 |
| p4_h06_c00 | 1/1/1 | 6.1 | 70/58/41 | 71/59/42 | 109/92/70 / 109/91/70 | 12/11 | 26/24 | 11/11 | 25/25 |
| p4_h06_c01 | -9/-11/-12 | 14.6 | 69/56/35 | 60/45/24 | 109/89/63 / 84/59/32 | 14/12 | 31/32 | 13/11 | 25/25 |
| p4_h06_c04 | 3/3/3 | 7.3 | 52/58/44 | 55/61/47 | 72/73/56 / 72/74/56 | 13/12 | 24/23 | 12/11 | 25/25 |
| p4_h06_c08 | 2/2/2 | 9.6 | 58/66/54 | 60/68/56 | 72/77/61 / 74/79/64 | 12/11 | 23/32 | 11/10 | 25/25 |
| p4_h12_c00 | -1/-1/-1 | 7.4 | 92/84/70 | 90/83/69 | 128/124/113 / 127/124/112 | 12/12 | 30/29 | 11/11 | 25/25 |
| p4_h12_c01 | -1/-1/-1 | 7.9 | 93/92/70 | 92/91/70 | 126/134/109 / 126/134/109 | 13/11 | 29/30 | 12/10 | 24/24 |
| p4_h12_c04 | -1/-1/-1 | 9.9 | 115/115/107 | 114/114/106 | 165/162/152 / 165/162/152 | 13/12 | 28/29 | 12/11 | 25/25 |
| p4_h12_c08 | -16/-17/-20 | 23.2 | 124/127/122 | 108/110/102 | 168/167/161 / 144/142/131 | 12/10 | 23/29 | 11/9 | 25/25 |
| p4_h18_c00 | -1/-1/0 | 7.1 | 88/83/72 | 86/82/72 | 121/120/115 / 121/120/115 | 13/12 | 34/32 | 12/11 | 25/25 |
| p4_h18_c01 | -1/-2/-1 | 7.1 | 97/92/70 | 96/91/69 | 135/136/108 / 135/135/108 | 13/12 | 30/33 | 12/11 | 24/25 |
| p4_h18_c04 | 0/0/0 | 7.7 | 108/107/100 | 108/107/101 | 156/152/145 / 156/153/145 | 13/10 | 30/29 | 11/9 | 25/25 |
| p4_h18_c08 | -1/-1/-2 | 11.1 | 101/110/99 | 100/108/97 | 138/142/128 / 137/141/126 | 12/12 | 22/29 | 11/11 | 25/25 |

Conteos medios (48 escenas): vehículos 14,4 original frente a 14,0 standalone; peds 26,8 frente a 26,9; `NumRandomCars` 10,7 frente a 10,4. Es decir, el tráfico y los peatones del standalone corregido (sin tráfico antes del arreglo de `fix-trafico-nubes`) ya son del mismo orden que los del original; las diferencias de cada escena son aleatorias.

Descripción de pares revisados a ojo:
* `p2_h12_c00` (carretera junto a la casa, mediodía despejado): cielo azul liso sin nubes en los dos, palmeras, paso elevado, valla metálica en primer plano con la misma sombra, calle de adoquín; en el original dos coches antiguos y un sedán a la izquierda, en el standalone un sedán negro y peatones al fondo (ruido); reloj 12:02 frente a 12:03.
* `p4_h18_c01` (plaza con edificios de ladrillo, tarde): mismos edificios, escalera, toldos, pancarta, semáforos y sombras largas; cielo cian liso; coches y peatones distintos (ruido); la animación de brazos de CJ difiere (fase de animación).
* `p2_h00_c00` (noche): farola con halo, cielo verde oscuro, mismos edificios y vallas; coches distintos.
* `p4_h12_c08` (lluvia, mediodía): mismo encuadre y edificios, gotas de lluvia visibles en los dos; el original más velado y claro (cielo 168/167/161) y el standalone más gris (144/142/131). Esa es la diferencia 2.
* `p3_h18_c08`: el standalone es claramente más claro y neblinoso (cielo 185/200/187) que el original (cielo 117/131/121), y CJ está en otra postura.
* Ni el original ni el standalone muestran formas de nubes en el cielo con esta cámara (11-34 grados sobre el horizonte según el punto; pitch real -0,05, el cielo ocupa poco): coincide con lo medido en `ejecucion-trafico-nubes.md`. No se reproduce una diferencia de nubes.
* Texturas que faltan, objetos o sombras: en ningún par exterior se ve una textura blanca o magenta, un edificio ausente ni una sombra que falte en uno solo. A 60 m de cada punto los conjuntos de modelos coinciden salvo el estado de carga (`rw` verdadero/falso del mismo modelo, 39-59 entidades), que depende del instante de streaming; no es una diferencia fiable.

## 5. Diferencias reales y lo que falta

| # | diferencia | evidencia | culpable |
|---|---|---|---|
| 1 | espejo del ropero (reflejo distinto) | `int_ropero.png`, `ropero_espejo_orig_asi_std.png` (recorte ampliado: original, ASI, standalone), `int_asi_ropero.png` (ASI = original), `s_int2/s_int3` | capa propia del standalone (no `CMirrors` C++, que en el ASI da lo mismo); sospecha: render a textura de cámara (`RwCameraSetRaster` con `rwRASTERTYPECAMERATEXTURE`) o `DatosExe` de `CMirrors` leídos por dirección; **no confirmado, sin corregir** |
| 2 | lluvia: 5 de 12 pares con más de 10 de diferencia, signo mixto | tabla exterior, `CTimeCycle` igual | sin identificar (sospecha: relámpago/variable de clima en `DatosExe`) |
| 3 | un amanecer despejado (`p4_h06_c01`) | -9/-11/-12 | sin identificar |
| 4 | carga 8→9 cinco veces más rápida | `ARRANQUE` en `comparar.log` | sin analizar |

No se tocó código del juego ni se hizo ningún arreglo en la rama (solo herramientas y documentación). Pendiente para quien continúe: (a) biseccionar el espejo dentro del standalone o comparar `RwCameraBeginUpdate`/`SetRaster` con textura de cámara contra el EXE; leer `CMirrors::pBuffer/TypeOfMirror` en el log del propio proceso (`TRAF:`-like), (b) repetir el exterior del original dos veces para tener la línea base de ruido de la lluvia (solo se midió el ruido del interior original contra sí mismo: diferencia absoluta 0,2-0,4, frente a 1,6-2,4 del standalone con el mismo color medio, que es animación de la flecha y de CJ), (c) registrar `CWeather::LightningFlash` desde dentro del proceso del standalone.

## 6. Límites y observaciones

* El "original puro" lleva dos calzas de terceros/propias (ventana y redirección de Mis documentos); el código del juego es el original.
* Resolución 800x600, no 640x480 ni 1024x768; el original corre a 25 fotogramas por segundo por el limitador, así que el FPS no discrimina rendimiento.
* La lectura de `CWeather` cov/fog/extra (0xC81304/0xC81300/0xC812F8) dio 0 en las dos (direcciones posiblemente erróneas); `old/new/rain` sí son coherentes.
* Hubo tres ejecuciones del original descartadas por defectos de la sonda, conservadas solo como registro en `ejecuciones/`: orientación por ratón no convergente (`descartada_o_ext1…`), cuelgue de la ventana del original tras salir de casa (`descartada_o_ext2…`, la ventana quedó "no responde" y se añadió un vigilante de 40 s) y tecla de menú perdida (`o_ext3`, ahora el menú comprueba que el estado cambie). El cuelgue no se volvió a ver en las dos ejecuciones siguientes; causa no determinada.
* Candado `D:\Original\gta_lock`: respetado; otros agentes (`juego`, `sin_original`, `bis1`) lo ocuparon varios ratos y se esperó. Al descartar ejecuciones propias se cerraron solo procesos de este lanzamiento; en una de ellas se pasó una lista de PID sin comprobar uno por uno y uno de los números lo reutilizó después un proceso de otro agente (creado a las 01:56:29, posterior al cierre); no hay indicio de que se cerrara un proceso ajeno, pero la lista no se verificó antes.
* No se tocó `D:\Original\_fase8b_run`, ni las partidas reales (el calce redirige las partidas del original a una copia), ni se usó `prueba_agua`.

## 7. Reproducir

```
# original puro
pwsh -File reversing\FnComparar\ejecutar_comparar.ps1 -Modo orig -Etiqueta o_ext -Limite 1500 -Sonda comparar.py -SondaArgs "--fase exterior"
# standalone
pwsh -File reversing\FnComparar\ejecutar_comparar.ps1 -Modo std  -Etiqueta s_ext -Limite 1500 -Sonda comparar.py -SondaArgs "--fase exterior"
# control EXE original + ASI (opcional -AsiArgs "--unhook=Global/CMirrors")
pwsh -File reversing\FnComparar\ejecutar_comparar.ps1 -Modo asi  -Etiqueta a_int -Limite 300 -Sonda comparar.py -SondaArgs "--fase interior"
# pares, compuestas y tabla
python -I reversing\FnComparar\analizar.py --orig <dirs orig> --std <dirs std> --salida reversing\FnComparar\evidencia --nombre ext --completa-si-dif 25
python -I reversing\FnComparar\memdiff.py <dir orig> <dir std> <escena>      # diferencias de CTimeCycle/gamma/menu
```

Archivos: `ejecutar_comparar.ps1` (lanzador con candado), `comparar.py` (sonda), `teclas.py` (teclado), `capt2.py` (BitBlt), `png_util.py`, `analizar.py`, `memdiff.py`, `shim/redir_docs.cpp` y `compilar_shim.cmd`, `explorar.py` (exploración del menú). Las capturas completas de cada ejecución están en `reversing/FnComparar/ejecuciones/` (no versionadas, 50 MB).
