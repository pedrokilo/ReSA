# Imagen de datos del gta-sa.exe incrustada en el ejecutable independiente (23/11/2026, rama `datos-imagen`, desde `d3ec2c9a03`)

Objetivo (opción A acordada con el usuario): que `gta_sa_standalone.exe` **ya no lea el `gta-sa.exe` original en ejecución**. Hasta ahora el callback TLS de `source/app/platform/win/StandaloneExe.cpp` abría el original (`--ruta-original`) y copiaba a sus direcciones fijas las secciones de datos (`.rdata` 308.224 B, `.data` 262.144 B, `_TEXT_HA` 68.608 B, `_rwdseg` 512 B, `.rsrc` 1.536 B = 641.024 B; el `.bss` es cero y no se copia). Decisión posterior del usuario: **los bytes se versionan en git** (arreglos C++), de modo que ni jugar ni compilar necesitan `gta-sa.exe`.

## 1. Diseño

* **Generador** `reversing/FnFase8/gen_imagen.py` (Python, se ejecutó UNA vez y se conserva para regenerar). Lee el `gta-sa.exe` 1.0 US (comprueba los 5.189.632 B), toma las secciones de datos (todas menos `.text` y `_rwcseg`, que son código y no se copian) y escribe `source/game_sa/DatosExe/Imagen/`:
  * `ImagenDatos00..15.cpp`: los bytes (`extern const uint8_t DatosNN[n]`), en trozos de 48 KiB, cada uno con su dirección (`VA 0x…`). Un trozo que es todo ceros no se emite (la reserva ya es cero): por eso son **640.512 B**, 512 menos que la medición (`_rwdseg`, íntegra a ceros).
  * `ImagenTabla.cpp`: las cabeceras PE originales (`g_CabeceraOriginal`, 0x400 B: DOS + NT + tabla de secciones, que el cargador usa para la IAT y para las protecciones finales) y la tabla `ImagenExe::kTrozos {va, tam, datos}`.
  * Todo va dentro de `#ifdef GTASA_STANDALONE_EXE`. `python -I reversing/FnFase8/gen_imagen.py --comprobar` verifica que los archivos versionados coinciden byte a byte con el original.
* **Direcciones idénticas**: se conserva la reserva `0x401000-0xCB1000` (`reserva_imagen.asm`, ceros) y el cargador copia cada trozo a su VA original (base `0x400000`, `/FIXED`, sin relocaciones). El código original (`0x401000-0x858000`) sigue siendo INT3 (`0xCC`), como antes.
* **Cargador** (`StandaloneExe.cpp`): `FaseCargar` ya no abre nada: `PrepararReserva` (comprueba la cabecera incrustada y deja la reserva en RWX) y `CopiarImagenIncrustada` (INT3 + trozos). `ResolverImports` y las protecciones finales leen la cabecera incrustada (`NtOriginal()`), no el archivo. La carpeta del juego por defecto es la del exe (`--directorio-juego` la sustituye). **Defecto: imagen incrustada.** `--ruta-original` solo se usa con `--imagen-externa` (modo de reserva: es el cargador anterior, útil para comprobar que la imagen incrustada se comporta igual); sin ese interruptor se ignora, de modo que los scripts viejos (`ejecutar_f8b.ps1`, `jugar_standalone.ps1`) que lo pasan siguen funcionando pero **ya no leen el original**.
* **Enlace**: `reversing/FnFase8/enlazar.cmd` compila los 17 archivos con `GTASA_STANDALONE_EXE` en `<salida>\imagen\*.obj` y los añade a `link.rsp`. No toca `gta-sa.exe` (ni lo menciona).
* **El ASI no la lleva**: `source/CMakeLists.txt` excluye `game_sa/DatosExe/Imagen/` del `GLOB_RECURSE` (`list(FILTER … EXCLUDE REGEX "^game_sa/DatosExe/Imagen/")`; comprobado con un script de CMake: 2.568 archivos antes, 2.551 después = 17 menos), y además los archivos están vacíos sin `GTASA_STANDALONE_EXE`.
* **rsrc**: la sección `.rsrc` original (1.536 B, icono/versión del original) se copia como los demás datos a `0x8B0000`; el `.rsrc` del propio exe independiente no cambia.

## 2. Tamaño

| | Antes | Después |
|---|---|---|
| `gta_sa_standalone.exe` (Debug, `/DEBUG`) | 49.581.056 B (el del usuario, 22/11 15:52) | **50.223.616 B** (+642.560 B ≈ +0,61 MiB: los 640.512 B de datos + cabeceras + tabla; la reserva de 9 MB ya estaba) |
| Repo (fuente nuevo) | 0 | 17 archivos, **1.981.939 B (≈1,89 MiB) en el árbol; ≈0,33 MiB comprimidos** en git (zlib nivel 6: 350.627 B) |
| Memoria en ejecución | — | +0,6 MiB (los arreglos viven en `.rdata` propio y se copian) |

## 3. Verificación

Se enlazó con los `.obj` del ASI Debug de `wv-int` (`build/source/gta_reversed.dir/Debug`, mismas fuentes que `d3ec2c9a03`; no se recompiló el ASI) más `StandaloneExe.obj` y los 17 `imagen\*.obj`: `link exit=0`, sin símbolos sin resolver (`reversing/FnFase8/salida/link.log`). Exe nuevo en `D:\Original\_fase8b_run_imagen\gta_sa_standalone.exe` (carpeta de pruebas propia, junctions a los datos del juego, **sin `gta-sa.exe` dentro y sin `--ruta-original` en la línea de órdenes**). El exe del usuario en `_fase8b_run` no se tocó.

Driver `reversing/FnFase8/probar_imagen.py` (candado `gta_lock`, mismo calendario de capturas del cliente completo de la ventana a t = 20, 60, 120, 180 y 240 s desde el lanzamiento, límite 255 s). La ventana mide **1024x768** (no 800x600).

| Ejecución | Arranque | Resultado |
|---|---|---|
| `imagen` (nuevo, 1.ª vez) | log: «imagen INCRUSTADA», 16 trozos, 640.512 B | `gGameState` 9, 5.997 fotogramas, `codigo-original-distinto=0 hits=0 excepciones=0`, CSV de código original vacío (solo cabecera), sale por la guardia de tiempo (`0xDEAD0002`). Disco frío: a t=20 s aún en la tarjeta del título |
| `imagen2` (nuevo, repetida) | igual | `gGameState` 9, 6.130 fotogramas, 0/0/0 |
| `control` (mismo exe, `--imagen-externa --ruta-original`, lee el original) | «EXTERNA» | `gGameState` 9, 0/0/0 |
| `actual` (exe del usuario en `_fase8b_run`) | modo estricto con ruta | `gGameState` 9, 6.138 fotogramas, 0/0/0 |

Capturas (cada una mirada con la herramienta de imagen, ventana completa):

* **`imagen2` vs `control`, t=20**: la misma escena de la maleta marrón en la cinta del aeropuerto (rojo 984 px en ambas, idéntico al del exe actual); t=60: el mismo fotograma de la escena del policía apuntando a CJ en el suelo junto al coche patrulla (medias RGB 83,4/89,2/84,7 frente a 83,5/89,4/85,0), texturas correctas, nada negro; t=240: CJ con control en el callejón (camiseta blanca, vaqueros, flecha azul, HUD con reloj, dinero 350 y radar) en `imagen2`. En `control` t=240 aparece «Truco activado» con CJ en bicicleta y 430.579 $: es una entrada de teclado ajena (no la envía el driver) que no apareció en las otras ejecuciones, no una diferencia del cargador.
* **`imagen` (1.ª vez, disco frío)**: título «Aeropuerto internacional Francis, Liberty City, 1992.» (t=20), taxi desde arriba (60), policía junto al coche (120), calle desde lo alto (180) y CJ con control a t=240: el mismo guion con ~20 s de retraso por la carga en frío, que se vio en el repetido.
* **`actual` (exe del usuario) NO es comparable por tiempo**: a t=60 ya está en juego con el tutorial («Usa ARRIBA, ABAJO, IZQUIERDA y DERECHA para mover a Carl») y a t=120/240 CJ de perfil. Ese exe se compiló el 22/11 15:52, antes que los `.obj` que enlacé, y no sé qué cambios de otras ramas incluye (la apertura corre mucho más rápido); el `control` (mi build con el cargador antiguo) se comporta igual que el nuevo, lo que aísla el cambio de cargador como inocuo. No se probó el mismo código que el del usuario.

«No abre gta-sa.exe»: la carpeta de pruebas no lo contiene, no se pasó ruta y el código de lectura solo existe tras `--imagen-externa` (sin él `g.archivo` ni se reserva). **No** se midió con Process Monitor (no está instalado) ni con ETW, y no se renombró el original del usuario: la evidencia es de construcción y de ejecución en carpeta sin original, no un trazado de `CreateFile`. Compilar/enlazar con el original ausente: ningún paso de `enlazar.cmd` lo referencia (solo `gen_imagen.py`, que ya no hace falta salvo para regenerar), pero no se renombró el original para probarlo.

ASI intacto (copia del `gta_reversed.asi` de `wv-int` del 23/11 18:11, que otro agente estaba recompilando; NO es un ASI de esta rama): `prueba_marcadores.py cal_mal.args` BIEN (15 entidades, 12 marcadores, los 12 cambian en 1 s); `prueba_ropero.py cal_mal.args` **MAL la 1.ª vez** (0 entidades, 10 marcadores, `stream_loadstate` 1: la firma conocida de carrera de streaming) y **BIEN la 2.ª** (15 entidades, 13 marcadores, puerta y marcador). Esas pruebas usan el `gta_sa.exe` original con el ASI, no el independiente, y esta rama no cambia código del ASI (solo el filtro de CMake), así que no son una prueba de esta rama; el MAL intermitente es del ASI/streaming de `wv-int`, no se investigó. Un primer intento falló por el candado `gta_lock` tomado por otro agente.

## 4. Lo que NO se verificó

* Zonas sin jugar: solo la apertura (cinemática) y el primer control de CJ en el callejón, ~4 min; ni ciudad, ni misiones, ni interiores, ni agua, ni guardado/carga con el exe independiente.
* El ASI de esta rama no se compiló (el único cambio es el filtro de CMake, comprobado por separado); las pruebas del ASI se hicieron con el ASI de `wv-int`.
* Más de una medida de rendimiento: la copia añade ~0,6 MiB de `memcpy` al arranque (insignificante), no se midió el tiempo de arranque más allá de que `gGameState` 9 llega a los ~10-13 s como antes.

## 5. Cómo regenerar / comprobar

```
python -I reversing/FnFase8/gen_imagen.py                # regenera (necesita D:\Grand Theft Auto San Andreas\gta-sa.exe o --original)
python -I reversing/FnFase8/gen_imagen.py --comprobar    # compara lo versionado con el original
reversing\FnFase8\enlazar.cmd <repo> <salida> <obj del ASI Debug>
```

Si se regenera, hay que borrar `<salida>\imagen\*.obj` (el script de enlace no recompila objetos existentes).
