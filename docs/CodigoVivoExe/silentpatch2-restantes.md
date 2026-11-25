# SilentPatch, segunda tanda: viñetas restantes (rama `silentpatch2-restantes`, 25/11/2026)

Fuente: SilentPatch para SA (MIT, Copyright (c) 2024 Adrian Zdanowicz «Silent»; atribución en [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Esta nota cubre las 10 viñetas de `CHANGELOG-SA.md` que no tenían fila en ningún CSV de [silentpatch-clasificacion](silentpatch-clasificacion/): L35, L70, L72, L73, L181, L182, L183, L207, L214 y L220. La clasificación por viñeta, con función de SilentPatch, función nuestra, opción y estado, está en [silentpatch-clasificacion/restantes.csv](silentpatch-clasificacion/restantes.csv); las desviaciones nuevas, en la sección 15.R de [DesviacionesConocidas.md](../DesviacionesConocidas.md).

## Opciones

Todas se apagan con `GTASA_SILENTPATCH=0` o una a una con `GTASA_SILENTPATCH_<NOMBRE>=0|1` / `SilentPatch_<Nombre>=0|1`. Con la opción apagada se ejecuta el código del EXE (o su decoder original).

| Opción | Por defecto | ¿Está en `SilentPatchSA.ini`? |
| --- | --- | --- |
| `AltF4Fix` | 1 | no |
| `MetricImperialConstantsFix` | 1 | no |
| `WaveProfilesFix` | 1 | no |
| `UserTracksFLAC` | 1 | no |

Ninguna cambia el ritmo del juego ni la jugabilidad, así que van activadas.

## Estado de las 10 viñetas

| L | Viñeta | Clase | Estado |
| --- | --- | --- | --- |
| 35 | Fallo del gimnasio | A | ya hecho en la primera tanda (`GymGlitchFixDate`, ver [silentpatch-scm.md](silentpatch-scm.md)); solo se añade la fila |
| 70 | Alt+F4 | A | implementado (`__MainWndProc`), prueba abajo |
| 72 | Constantes métrico-imperiales | A | implementado+compilado (`StatsMetersToFeet` en `Stats.cpp`) |
| 73 | Temporizadores del baile | A | ya hecho en la rama de gráficos (`DancingTimersFix`); solo documentado |
| 181 | MP3 sin QuickTime | C | ya resuelto por `sustituir-quicktime` (Media Foundation) |
| 182 | PCM WAVE con más perfiles | A | implementado (`CAEWaveDecoderEx`), prueba abajo |
| 183 | PCM WAVE con ID3 | A | implementado (`WaveExtParse`), prueba abajo |
| 207 | Quadruple Stunt Bonus | A | ya hecho en la primera tanda (`QuadrupleStuntBonus`); solo se añade la fila |
| 214 | Censura alemana Steam/RGL | C | no aplica: el standalone es el EXE 1.0 US |
| 220 | FLAC en User Tracks | A | implementado con Media Foundation, prueba abajo |

## Qué se hizo

### Alt+F4 (L70)

El EXE 1.0 US termina las ramas `WM_KEYDOWN`/`WM_SYSKEYDOWN`/`WM_KEYUP`/`WM_SYSKEYUP` de `__MainWndProc` (0x747EB0) con `return 0`, así que `DefWindowProc` nunca ve el mensaje de Alt+F4 y la ventana no se cierra. SilentPatch cambia el salto final de las ramas `SYS` (0x748220 y 0x74826A) a `DefWindowProc` (0x748446). En `source/app/platform/win/WndProc.cpp` las ramas `SYS` hacen `break` tras entregar la tecla al juego (la tecla sigue llegando como antes) y el mensaje acaba en `DefWindowProcA`, que lo convierte en `WM_SYSCOMMAND`/`SC_CLOSE` y luego `WM_CLOSE`, ya tratado (`PostQuitMessage`). Los `esi -> ebx` y el `lParam` en la pila de SilentPatch son detalles de registro del ensamblador y no hacen falta en C++.

### Constantes métrico-imperiales (L72)

SilentPatch cambia el valor de metros a pies de 3.3333333f (0x859F60, el valor del EXE; da un 1,6 % de pies de más) a 3,280839895 en dos referencias, 0x55942F y 0x55AA96. En nuestro código son los dos `Stw60MulCpp(valor, 0x40555555)` de `Stats.cpp` (`CStats::BuildStatLine`, formato `%.2fft`, y la línea de estadística de tipo 7 de `StatLineEntry`); ambos pasan ahora por `StatsMetersToFeet()`.

### WAV ampliado y con ID3 (L182, L183)

El decoder del EXE (`CAEWaveDecoder`, 0x5032B0) lee una cabecera fija de 44 bytes y solo acepta PCM estéreo de 16 bits; un chunk `LIST` o `id3 ` antes de `data` lo desalinea y uno después se reproduce como ruido. Se añadió `CAEWaveDecoderEx` (`source/game_sa/Audio/Loaders/AEWaveDecoderEx.{h,cpp}`; clase NOTSA que no existe en el EXE) con su núcleo sin dependencias del juego `WaveExtCore.h`:

* Recorre los chunks (alineados a 2 bytes) hasta `fmt ` y `data`, en cualquier orden, y acota `data` al tamaño real del archivo.
* Acepta PCM 8/16/24 bits, mono o estéreo, hasta 48 kHz, también `WAVE_FORMAT_EXTENSIBLE` con subformato PCM; rechaza float, ADPCM, más de 2 canales y más de 48 kHz.
* Acepta una etiqueta ID3v2 delante del `RIFF` (SilentPatch no lo hace).
* Salida: PCM estéreo de 16 bits a la frecuencia del archivo (8 bits: `(v-128)<<8`; 24 bits: los 16 bits altos; mono: se duplica). La duración, el cursor y la posición se calculan con la longitud real de las muestras; se conserva el descarte de pistas de menos de 7 s.
* `LoadUserTrack` solo lo usa si la opción está encendida y el archivo NO es el WAV canónico de 44 bytes (PCM estéreo de 16 bits, `fmt ` de 16 bytes seguido de `data`): ese sigue por `CAEWaveDecoder` del EXE sin cambios.

SilentPatch tiene un bucle sin salida si un archivo termina antes de encontrar `fmt `/`data`; aquí se devuelve fallo.

### FLAC (L220)

SilentPatch enlaza libFLAC (`FLACDecoderSA.cpp`). Aquí no hace falta: Media Foundation de Windows 10 o posterior lee FLAC de forma nativa y el standalone ya decodifica las pistas `.mp3`/`.aac`/`.m4a`/`.mov` con `CAEMFDecoder` (ver [../QuickTime/sustitucion-quicktime.md](../QuickTime/sustitucion-quicktime.md)). `GetAudioFileType` devuelve `AUDIO_FILE_TYPE_QUICKTIME` (el tipo de Media Foundation) para `.flac`, tras recorrer la tabla del EXE (que no se puede ampliar: son 8 entradas fijas en 0x8CBB28). Limitaciones: depende del decodificador FLAC de Windows (sin él la pista se descarta, como un `.aac` sin Media Foundation); la salida se remuestrea a 44,1 kHz estéreo de 16 bits como cualquier pista de Media Foundation (los FLAC de 44,1 kHz salen bit a bit; los de otra frecuencia salen remuestreados); no se acepta la extensión `.fla` que SilentPatch registra además de `.flac`.

### Sin cambios de código

* L35 y L207 (scripts): ya portados en la primera tanda en `source/game_sa/Scripts/SilentPatchScripts.cpp`, bajo `EnableScriptFixes`.
* L73 (temporizadores del baile): ya portado por la rama de gráficos como `DancingTimersFix` en `CAEAudioUtility::GetCurrentTimeInMS`.
* L181: `sustituir-quicktime` ya quitó QuickTime del standalone (y de la ASI: es el mismo código). Con Media Foundation ausente, `Initialise` pasa los `.mp3` al decoder WMA como el original.
* L214: la censura alemana solo existe en los ejecutables Steam y RGL (SilentPatch la quita únicamente en los «new binary»). En el EXE 1.0 US `CLocalisation::SetGermanGame` (0x56D1E0) no tiene llamadores (`MenuManager.cpp` solo llama a `SetNormalGame` y `SetFrenchGame`), `germanGame` solo se escribe en `Localisation.cpp`, y por tanto `GermanGame()` es siempre falso y `nastyGame` verdadero.

## Pruebas

Archivos de prueba: `reversing/FnQT/gen_wav_flac.py` genera 8 WAV (16/8/24 bits, mono/estéreo, 8 a 48 kHz, ID3 antes o después de `data`, ID3v2 al inicio, `WAVE_FORMAT_EXTENSIBLE`), dos WAV que deben rechazarse (float de 32 bits y 96 kHz) y tres FLAC «verbatim» válidos (con CRC-8 y CRC-16), todos de 8 s, y `esperado.txt` con el CRC32 del PCM estéreo de 16 bits que debe salir.

### Núcleo del WAV ampliado (`reversing/FnQT/wave_ext_probe.cpp`, proceso aparte, x86)

La sonda lee cada archivo con `WaveExtParse`/`WaveExtFill` (el mismo `WaveExtCore.h` que el juego) y escribe el CRC32 del PCM de salida.

| Archivo | Resultado | CRC32 y bytes de salida frente al esperado |
| --- | --- | --- |
| 16 bits estéreo 44,1 kHz canónico | aceptado | B852D21F, 1411200: igual |
| 16 bits estéreo 44,1 kHz, chunks `id3 ` y `LIST` detrás de `data` | aceptado, datos acotados a 1411200 | B852D21F, 1411200: igual (sin ruido del ID3 al final) |
| 16 bits estéreo 44,1 kHz con ID3v2 al inicio del archivo | aceptado, datos en 354 | B852D21F: igual |
| 16 bits mono 22,05 kHz | aceptado | 610B0D29, 705600: igual |
| 8 bits estéreo 32 kHz | aceptado | E1F168AB, 1024000: igual |
| 8 bits mono 8 kHz | aceptado | E94C8EAD, 256000: igual |
| 24 bits estéreo 48 kHz con chunk `id3 ` antes de `data` | aceptado, datos en 362 | 4D08B53C, 1536000: igual |
| 24 bits mono 44,1 kHz extensible | aceptado, datos en 68 | 820AC069, 1411200: igual |
| float de 32 bits; 16 bits a 96 kHz | rechazados | (sin salida) |

### FLAC con Media Foundation (`reversing/FnQT/mf_probe.cpp`, `CMfPcmReader`, el código de producción)

| Archivo | Resultado |
| --- | --- |
| FLAC 16 bits estéreo 44,1 kHz | 352800 fotogramas, 8000 ms, CRC32 B852D21F: igual bit a bit al PCM esperado |
| FLAC 8 bits estéreo 44,1 kHz | CRC32 C27441E6: igual bit a bit |
| FLAC 24 bits mono 48 kHz | 8000 ms; CRC32 A85A423E, distinto del que saldría sin remuestrear (C31A6547) porque Media Foundation lo convierte a 44,1 kHz estéreo de 16 bits (esperado) |

### En el juego (standalone con las modificaciones, `reversing/FnQT/ejecutar_spr.ps1` y `preparar_spr.ps1`)

Carpeta propia `D:\Original\_spr_run`, partida 2 copiada, pista de usuario registrada a mano en `sa-utrax.dat`/`sa-ufiles.dat` (tipo 2 = WAV, 4 = Media Foundation), CJ en un coche con radio, emisora `RADIO_USER_TRACKS` pedida por script (escenario `usertrack`). La evidencia es el log del propio juego (el pico de audio WASAPI no distingue nada: el juego tiene ruido de fondo, igual que se anotó en el documento de QuickTime). Se añadieron líneas de registro `NOTSA_LOG_INFO` (solo extensión, nunca la ruta del usuario) en `CAEWaveDecoderEx::Initialise` y `CAEMFDecoder::Initialise`.

| Caso | Opción | Resultado en el log del juego |
| --- | --- | --- |
| WAV 24 bits estéreo 48 kHz con chunk `id3 ` antes de `data` | `WaveProfilesFix=1` | `WAV ampliado aceptado: 2 canales, 24 bits, 48000 Hz, datos en 362 (2304000 bytes), 8000 ms` (la pista se carga tres veces durante la ejecución); vivo al final |
| el mismo | `WaveProfilesFix=0` | sin línea de `CAEWaveDecoderEx`: el decoder del EXE lo descarta en silencio (24 bits); vivo al final |
| WAV 16 bits estéreo con ID3 y LIST detrás de los datos | `WaveProfilesFix=1` | `WAV ampliado aceptado: 2 canales, 16 bits, 44100 Hz, datos en 44 (1411200 bytes), 8000 ms` (pasa por el decoder nuevo porque tiene chunks detrás de `data`); vivo al final |
| WAV 16 bits estéreo canónico, sin chunks extra | `WaveProfilesFix=1` | sin línea de `CAEWaveDecoderEx`: sigue por el decoder del EXE; vivo al final |
| FLAC 16 bits estéreo 44,1 kHz | `UserTracksFLAC` (el tipo se registró a mano) | `Media Foundation acepta la pista con extension '.flac' (8000 ms)`; en la sonda `mfcore.dll` y `mfperfhelper.dll` se cargan al sintonizar; vivo al final |

Extractos del log y de la sonda de las ejecuciones de B, C y FLAC: `reversing/FnQT/evidencia/spr-restantes/` (las carpetas completas están en `reversing/FnQT/ejecuciones/`, ignorada por git). Las dos primeras filas (WAV 24 bits con la opción encendida y apagada) se leyeron de una ronda anterior cuyas carpetas se borraron al cambiar el código (cambio de `IsCanonicalHeader` y líneas de registro); el decoder de esas dos filas no cambió.

Reservas: en algunas ejecuciones la sonda no consiguió entrar al coche (`NO se pudo entrar al coche`) y la emisora de pistas de usuario no llegó a sintonizarse: se repitieron. En la ejecución del FLAC la pista se cargó 9 veces seguidas en un segundo a los 25 s de la sintonización; no se pudo comparar con una pista MP3 de control porque las dos ejecuciones de control no llegaron a sintonizar las pistas de usuario, así que no se sabe si esa ráfaga es propia de la emisora de pistas de usuario o de FLAC. No se oyó ninguna pista (no hay forma de comprobarlo desde la sonda), ni se comprobó el cambio de pista al terminar, el bucle ni `SetCursor` con el decoder nuevo en el juego (sí está en el código y el núcleo se probó con los archivos).

### Alt+F4 (L70)

Escenario nuevo `altf4` de `reversing/FnJuego/sonda_juego.py`: publica con `PostMessage` `WM_SYSKEYDOWN` y `WM_SYSKEYUP` de F4 con el bit de contexto Alt en la ventana del juego (no toca el teclado real ni la ventana en primer plano) y mira si el proceso termina en 8 s. Standalone en `D:\Original\_juego_run`:

| Opción | Resultado |
| --- | --- |
| `AltF4Fix=1` (por defecto) | el proceso terminó 0,5 s después, con código de salida 0 (`ejecuciones/spr_altf4_on-20261125-092516`) |
| `AltF4Fix=0` | el proceso seguía vivo 8,1 s después (`ejecuciones/spr_altf4_off-20261125-092547`) |

### Ejecución del standalone de 150 s

`ejecutar_juego.ps1 -Escenario conduccion,armas,interiores -Limite 150` con el standalone de esta rama (todas las opciones por defecto): 75 latidos, todos con `excepciones=0` y `codigo-original-distinto=0`, vivo hasta la guardia de tiempo (código de salida 0xDEAD0002 de la guardia). La conducción no dio NaN. La sonda anotó una excepción de Python propia en el escenario `armas` (`TypeError: 'NoneType' object is not subscriptable`, error de la sonda, no del juego), así que `armas` e `interiores` no se ejercitaron en esa ejecución.

### Verificaciones del repositorio

`python contrib/audit_hook_catalog.py --strict`: OK. `python contrib/auditoria_datos/tamanos_objetos.py lint`: 0 fallos. Compilación con `compilar_over` de `AEWaveDecoderEx.cpp`, `AEMFDecoder.cpp`, `AEUserRadioTrackManager.cpp`, `Stats.cpp` y `WndProc.cpp`; enlace de la ASI y del standalone con los `.obj` de `wv-intuild`.

### Humo con la ASI de la rama

ASI enlazada con los `.obj` de `D:\Original\wv-intuild` más los de `reversing\FnFase8\over` de esta rama (`Stats`, `WndProc`, `AEUserRadioTrackManager`, `AEMFDecoder`, `AEWaveDecoderEx`), desde `D:\Original\wv-8eversing\FnBIS1` con `AGUA_ASI` y `cal_mal.args`, todas las opciones por defecto (encendidas): `prueba_ropero.py` VEREDICTO BIEN (puerta True, marcador True, 15 entidades y 12 marcadores) y `prueba_marcadores.py` VEREDICTO BIEN (12 marcadores en memoria y 12 que cambian en 1 s), a la primera. No se repitió con SilentPatch apagado (el cambio de código en la ASI está protegido por las opciones y ninguna de las tres rutas toca el ropero ni los marcadores).

### Qué no se verificó

* L72: el texto en pies de la pantalla de estadísticas no se ejercitó (solo compilado).
* El escaneo automático de pistas de usuario con `.flac` (`GetAudioFileType`): las pistas se registraron a mano con tipo 4.
* La escucha real de WAV ampliado o FLAC, el cambio de pista, el bucle y el cursor en el juego.
* L35, L207 y L73 se dan por hechas por la rama que las portó, sin repetir pruebas aquí.
* L214: razonado leyendo el código y los llamadores, sin ejecutar un EXE Steam/RGL.

### Defecto detectado en el árbol integrado

Tras la fusión, `reversing/FnJuego/sonda_juego.py` tenía la línea `ESCENARIOS = {...}` en la mitad del archivo, antes de las funciones `esc_entrada`, `esc_luna`, `esc_fps` y `esc_graficos` que la propia tabla nombra: la sonda fallaba con `NameError` al arrancar. En esta rama la línea se movió justo antes de `main()`.
