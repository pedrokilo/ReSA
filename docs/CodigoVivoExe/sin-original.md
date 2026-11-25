<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prueba de que el independiente compila, enlaza y ejecuta SIN el gta-sa.exe original (24/11/2026, rama `sin-original`, desde `689c9c1f1d`)

Todo se midió en un clon limpio (`git clone --shared` de `wv-int`, `D:\Original\wv-limpio`, borrado al terminar). Herramientas nuevas en `reversing/FnSinOriginal/`.

## 1. Auditoría estática (quién nombra el original)

| Sitio | Uso | Fase |
|---|---|---|
| `source/app/platform/win/StandaloneExe.cpp` (`--imagen-externa`, `--ruta-original`, `GTASA_ORIGINAL_EXE`, `CreateFileA(g.ruta)`) | cargador antiguo de respaldo; **solo se ejecuta con `--imagen-externa`**. El defecto es la imagen incrustada. El código sigue enlazado en el exe (3 cadenas: mensajes del cargador antiguo) | ejecución, solo si se pide |
| `reversing/FnFase8/gen_imagen.py` (`--original`, por defecto `D:\Grand Theft Auto San Andreas\gta-sa.exe`) | genera/comprueba `source/game_sa/DatosExe/Imagen/*` | solo regeneración/diagnóstico; no está en el enlace |
| `ejecutar_f8b.ps1`, `ejecutar_crtb.ps1`, `ejecutar_scrb.ps1`, `ejecutar_traf.ps1`, `render_correr.ps1`, `probar_raton.py`, `repro_barra_fuente.py`, `FnBIS1/jugar.ps1` | pasan `--ruta-original` en la línea de órdenes; sin `--imagen-externa` el exe la **ignora** | ejecución (inofensivo) |
| `datos_copiados_r4.py`, `dis.py`, `dis_rango.py`, `gen_tabla_ret.py` | leen el original para estudiarlo | solo diagnóstico |
| `CMakeLists.txt`, `cmake/`, `source/CMakeLists.txt`, `enlazar.cmd`, `compilar_over.cmd`, `cl.rsp`, `link.rsp` | **ninguna referencia** (solo comentarios en `source/CMakeLists.txt` y `enlazar.cmd`) | build |
| resto de `source/` | solo comentarios («generado desde gta_sa.exe»), cadenas de datos del juego | — |

Binario: en `gta_sa_standalone.exe` las únicas cadenas `gta_sa.exe`/`ruta-original`/`imagen-externa`/`GTASA_ORIGINAL_EXE` son los 3 mensajes del cargador de respaldo (no hay ninguna ruta del original ni `Grand Theft Auto San Andreas`); no hay `gta-sa.exe` en UTF-16.

## 2. Hallazgo corregido: `cl.rsp` dependía de otro árbol

`reversing/FnFase8/cl.rsp` (versionado) llevaba **54 rutas absolutas a `D:\Original\wv-fase8c`** (un árbol que ya no existe): en un clon limpio `cl` no encontraba las cabeceras. Corrección: el `.rsp` usa los marcadores `@RAIZ@`/`@RAIZ_FS@` y `enlazar.cmd`/`compilar_over.cmd` lo expanden con la raíz del repo en uso (`expandir_rsp.ps1`). No tiene relación con el original, pero impedía reconstruir desde un clon.

## 3. Compilación y enlace sin el original

Clon limpio + junction de solo lectura al `conan-cache` de `wv-int` + `.obj` del ASI Debug y las tres `.lib` propias ya compiladas de `wv-int` (artefactos de build, no se recompilaron los 1185 `.cpp`). `enlazar.cmd` compila `StandaloneExe.cpp` y los 17 `Imagen*.cpp` con `GTASA_STANDALONE_EXE` y enlaza: `link exit=0`, sin símbolos sin resolver. Además `compilar_over.cmd` compiló `Clock.cpp` en el clon.

Trazado (sin administrador: ETW denegado) con `espia_archivos.dll`, inyectada antes del punto de entrada en `link.exe`/`cl.exe` de 32 bits: `ejecuciones/traza-enlace/` — enlace 2676 accesos, compilación 313 y 173; **0 a `gta-sa.exe`/`gta_sa.exe` y 0 bajo `Grand Theft Auto`**.

## 4. Ejecución sin el original

Carpeta `D:\Original\_sin_original_run`: junctions a anim/audio/data/models/movies/text, `eax.dll`, `ogg.dll`, `vorbis*.dll`, `stream.ini`, `userfiles` y `gta-reversed.ini`; el único `.exe` es el independiente. Sin `--imagen-externa` ni `--ruta-original`. Resultado en `reversing/FnSinOriginal/ejecuciones/sinorig1-20261124-003126/`:

* 165 latidos del log del exe (cada 2 s, luego cada 20 s): `estado-juego=9` en 162; los 3 primeros (0, 8, 8) son del arranque (<5 s). Último latido a 329,7 s: frame 8082, `codigo-original-distinto=0 hits=0 excepciones=0`. Sale por la guardia de tiempo (`0xDEAD0002`, `--limite 330`), duración 334 s: **se mantuvo vivo y en estado 9 más de 5 minutos**.
* Capturas a t=20, 60, 120, 180, 240, 300 s (ventana 800x600, mirados t=120 y t=300): calle de Ganton al atardecer con semáforos y coche, y CJ (camiseta blanca, vaqueros) en el callejón con la bici; texturas correctas.
* Espía: 1896 accesos (NtCreateFile 712, NtOpenFile 509, NtQueryAttributesFile 598, NtQueryFullAttributesFile 77); **0 con `gta-sa.exe`/`gta_sa.exe`**; ningún `.exe` tocado salvo el propio `gta_sa_standalone.exe` (14, el cargador/D3D lo consultan) y sondas de DLL del sistema en la carpeta (`oculusmedium.exe` inexistente, etc.).
* Control con señuelo: mismo exe con `--imagen-externa --ruta-original D:\Original\_sin_original_decoy\gta-sa.exe` (archivo de texto de 31 B): el espía registra `NtCreateFile ...\_sin_original_decoy\gta-sa.exe` a los 15 ms y el exe aborta (`tamano 31 distinto de 5189632`, exit 4). Es decir, el espía SÍ ve al exe real abrir un `gta-sa.exe` cuando se le pide.
* NO medido: ropero y marcadores (las pruebas `FnBIS1` son del ASI y no hay equivalente para el independiente), ni zonas más allá del callejón inicial, ni guardado/carga.

## 5. Alcance y límites del espía

* Engancha en `ntdll` `NtCreateFile`, `NtOpenFile`, `NtQueryAttributesFile`, `NtQueryFullAttributesFile` (el original solo puede leerse tras uno de ellos). **No ve** una enumeración de directorio (`FindFirstFile`) que solo busque el nombre, ni accesos desde otros procesos hijos.
* Se inyecta en el punto de ruptura inicial del cargador (antes de los callbacks TLS): el primer acceso registrado es el propio `CreateFile` del log del callback TLS. Se pierden solo las cargas de DLL estáticas del cargador anterior a ese punto (DLL del sistema).
* Control positivo (`control_positivo.py`): un señuelo llamado `gta-sa.exe` abierto por un proceso espiado SÍ aparece en el log; y el mismo exe con `--imagen-externa --ruta-original <señuelo>` (ver `ejecuciones/control-externa-*`).
* Los accesos a datos del juego aparecen como `[raiz=\\?\D:\Grand Theft Auto San Andreas\...]` porque las junctions resuelven a ese árbol: son lecturas de datos (GXT, models, data), ningún `.exe`.
