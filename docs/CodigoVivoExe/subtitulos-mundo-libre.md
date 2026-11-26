# Subtítulos de las voces del mundo libre

Rama `subtitulos-voces`, 25/11/2026. Módulo `source/game_sa/FreeRoamSubtitles.h/.cpp` (NOTSA: no existe en el EXE). Desviación propia 15.F de
[DesviacionesConocidas.md](../DesviacionesConocidas.md); atribución en [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md).

Idea, lógica y textos de **Free Roam Subtitled** de Kaizo M, usados con su autorización (declarada por el usuario del proyecto). **Los textos no se redistribuyen**:
el módulo los lee en tiempo de ejecución de los `.fxt` de la instalación del usuario. El código es propio: se reconstruyó la lógica del script CLEO a partir de su
desensamblado (con las tablas de opcodes de Sanny Builder), no se copió el script ni ningún texto al repositorio.

## Qué hace

Cuando un peatón (o CJ) empieza a hablar en el mundo libre, el módulo busca el texto de esa voz en el `.fxt` y lo muestra como subtítulo con el sistema de mensajes del
propio juego (el mismo texto de abajo del HUD que usan los subtítulos de las misiones, con la misma fuente, color y posición). El subtítulo dura lo que dura la voz más
`ReadingTime` ms y, si `SaveToBrief=1`, queda guardado en el Brief del menú de pausa.

## Instalación de los FXT

1. Copiar `English.fxt` y `Spanish.fxt` del mod a `<carpeta del juego>\cleo\cleo_text\` (o a otra carpeta y apuntarla con `FreeRoamSubtitlesDir=`).
2. Opcional: `FreeRoamSub.ini` del mod en `<carpeta del juego>\cleo\` (se lee como último recurso; ver «Opciones»).
3. No hace falta nada más: sin los ficheros el módulo escribe una línea en `log.log` (`no se pudo cargar ...`) y no muestra nada (reintenta cada 10 s por si se instalan en caliente).

Idioma: con el juego en **español** (`FrontEndMenuManager.m_nTextLanguage == 4`) se usa `Spanish.fxt`; con **cualquier otro idioma** (inglés, francés, alemán, italiano) se usa
`English.fxt`, igual que el script. No hay reserva a otro fichero (un juego en español sin `Spanish.fxt` no muestra subtítulos aunque exista `English.fxt`).

## Opciones

| Opción | Valor por defecto | Rango | Fuente (la primera que exista gana) |
|---|---|---|---|
| Interruptor general | 0 (**desactivado**; se activa con `FreeRoamSubtitles=1` o `GTASA_FREE_ROAM_SUBS=1`) | 0/1 | `GTASA_FREE_ROAM_SUBS` > `FreeRoamSubtitles=` en `gta-reversed.ini` |
| `SpainSpanish` | 1 | 0/1 (≠0 = sí) | `GTASA_FRS_SPAINSPANISH` > `gta-reversed.ini` > `cleo\FreeRoamSub.ini` |
| `ChicanoSlangs` | 1 | 0/1 | `GTASA_FRS_CHICANOSLANGS` > idem |
| `SaveToBrief` | 1 | 0/1 | `GTASA_FRS_SAVETOBRIEF` > idem |
| `ReadingTime` | 200 ms | 0–2000 (se recorta) | `GTASA_FRS_READINGTIME` > idem |
| `FreeRoamSubtitlesDir` | `cleo\cleo_text` | ruta | `GTASA_FRS_DIR` > `gta-reversed.ini` |

Los nombres y valores por defecto de las cuatro opciones de Kaizo son **exactamente** los de su `FreeRoamSub.ini` (`SpainSpanish=1`, `ChicanoSlangs=1`, `SaveToBrief=1`,
`ReadingTime=200`). En `gta-reversed.ini` basta una línea `Clave=valor` en cualquier sección (como el resto de opciones del proyecto); también se entiende el formato del mod
(`[Subtitles]`). Comentarios con `;`. Cualquier valor numérico distinto de 0 es «sí» (como el script); un valor no numérico se ignora.

**Apagado en caliente:** las opciones se vuelven a leer (como mucho cada 2 s) cada vez que empieza una voz, así que editar el ini surte efecto en la voz siguiente sin reiniciar
ni pasar por la pausa. (El script usa el truco `SUBS` para recargar; aquí no hace falta.) Los cambios de las variables de entorno solo se ven al reiniciar.

**Opción apagada = comportamiento del EXE:** `OnPedSpeechStart` y `Tick` salen al principio; no se carga ningún fichero ni se escribe ningún mensaje.

**Decisión sobre el valor por defecto del interruptor general: DESACTIVADO (el usuario lo pidió el 26/11/2026; el agente lo había dejado activado).** El usuario pidió que sea opcional y que las opciones tengan los valores por defecto del ini de
Kaizo. Con el módulo activado y sin los `.fxt` instalados no ocurre nada visible (solo una línea de log), por lo que activarlo no cambia el juego de quien no los tenga; quien
quiera el comportamiento del EXE puede apagarlo con `FreeRoamSubtitles=0`. Para cambiar el valor por defecto basta cambiar `Config::Enabled` en `FreeRoamSubtitles.h`.

## Lógica del script reconstruida

Resumen del script `FreeRoamSub_FXT.cs` (31 121 B, ≈3250 instrucciones). Es un solo hilo CLEO (`SUBS`) que cada fotograma recorre los peatones. Lo que hace y cómo lo trata
este módulo:

| Parte del script | Qué hace | Aquí |
|---|---|---|
| Lectura del ini (`0x2BF7`) | `SpainSpanish`, `ChicanoSlangs`, `SaveToBrief`, `ReadingTime` de `cleo\FreeRoamSub.ini`, sección `Subtitles`; `ReadingTime` recortado a 0..2000 | Igual (además entorno y `gta-reversed.ini`) |
| Idioma (`current_language`, `new_language_set`) | idioma 4 = español → claves M/P/S; resto → C/E; recarga al cambiar de idioma | Igual; el idioma se consulta en cada voz (cambio de idioma sin recarga) |
| Detección del hablante | sondeo por fotograma de `CPed+0x294` (`m_pedSpeech`): `m_SoundID` +0xA4, `m_BankID` +0xA6, `m_LastGCtx` +0xB0, `m_Sound` +0xA0, y de `s_bAPlayerSpeaking` (0xB613E5) | **Por gancho** en `CAEPedSpeechAudioEntity::PlayLoadedSound` (0x4E5CD0): recibe banco, sonido, contexto, ped y hueco sin sondear la memoria |
| Final de la voz | el script lo infiere de que `m_Sound` deja de ser no nulo | El hueco de voz (`s_PedSpeechSlots[slot]`) deja de ser de ese hablante o de estar en `PLAYING`; se comprueba una vez por fotograma en `Tick` (sin gancho de final, ver «Por qué hay un `Tick`») |
| Contextos (`m_LastGCtx`) | descarta `>= 339` (dolor: `CTX_GLOBAL_PAIN_START`) | Igual. Además descarta `-1` (voz de script: `AddScriptSayEvent`), ver «Decisiones» |
| Hablantes | recorre todos los peatones; ignora tipos 0/1 (jugadores) como hablantes ajenos y trata aparte la voz del propio jugador (se subtitula siempre, sin distancia) | Igual: `PED_TYPE_PLAYER1/2` = voz propia, sin distancia |
| Radio de audición | por banco de sonido: 10 m por defecto; 20 m bancos 411, 416–420 (novias); 35 m bancos 412, 413, 422–428 (empleados de bar/barbería/tiendas); 40 m banco 644 (César). `locate_char_distance_to_char` (CLEO+ 0EE4) entre el jugador y el hablante | Igual (`RadiusForBank`), distancia 3D al jugador más cercano (en dos jugadores, el más cercano de los dos) |
| Parejas charlando | si un peatón a ≤10 m del jugador hace `TASK_COMPLEX_PARTNER_CHAT` (1204) o `TASK_COMPLEX_PARTNER` (256), solo se subtitula a quien está en esas tareas (borra el texto de los demás) | Igual (`SuppressedByPartnerChat`) |
| Clave del texto | `<prefijo><banco %03d><sonido+1 %03d>`: p. ej. `S506011`; si banco y sonido valen 0 o el valor es negativo, sin texto. Español: `M` (si `ChicanoSlangs`), `P` (si `SpainSpanish`), `S`; resto: `C` (si `ChicanoSlangs`), `E`. Se prueba cada prefijo en orden y gana el primero que existe | Igual (`KeyPrefixes`, `MakeKey`). El script consulta `CText::Get` (los `.fxt` los añade CLEO); aquí hay una tabla propia |
| Mostrar | por fotograma, `show_formatted_text_lowpriority '%s' time 1` (`PRINT` en cola con 1 ms, repetido cada fotograma mientras habla el hablante) o, con `SaveToBrief`, `show_text_lowpriority GXT "SAVBnn" time 1 flag 4` (`00BB`) sobre 20 etiquetas GXT dinámicas `SAVB00..19`/`SAVC00..19` en anillo, con texto persistente porque `CMessages::PreviousBriefs` guarda punteros | Un solo `CMessages::AddMessage2(texto, tiempo, 0, saveToBrief, instantáneo)` por voz; el texto se copia a un anillo de 20 búferes persistentes (160 bytes) |
| Tiempo de lectura | `19@ = ahora + ReadingTime`; mientras no vence, si no habla nadie vuelve a mostrar el último texto | `Tick` fija `VisibleTime = transcurrido + ReadingTime` al acabar la voz |
| Brief | el mismo texto no se guarda dos veces en menos de 1000 ms (compara con `strcmp` de `msvcrt`) | Igual |
| Varios hablantes | gana el último procesado del recorrido (orden del pool) | Gana el que empieza más reciente; si acaba el que se muestra y otro sigue hablando, se muestra el otro (`Tick`) |
| Mensajes del juego | `text_priority_displayed` (076F) para no pisar | `Show` no pisa el hueco 0 de `CMessages::BriefMessages` si lo ocupa otro mensaje, y cede el sitio si el juego encola uno detrás |
| `remove_text`/`add_text_label` (GF_*, DEAL*, PROS_*, SHOPNO, AMMUC/D, TALK_1, MAN3_33, GW_YRS) | borra los subtítulos propios del juego de citas, camellos, prostitutas y tiendas y los **sustituye por etiquetas vacías** para que solo se vea el texto del `.fxt` | **No se hace** (decisión): el juego conserva sus subtítulos; si ya hay uno en pantalla, el módulo no muestra el suyo |
| Parches de memoria | `0x58C271` (`je`→`jne` en `CHud::DrawSubtitles`, para dibujar subtítulos con el cartel WASTED/BUSTED) y `NOP` del `call AddMessageJumpQ` de `TALK_1` en `0x66696A` | No se portan (fuera de alcance; cambian comportamiento del juego) |
| Policía (`0xB6D..`, `0xB61D..`) y voz sin peatón del banco 421 | subtítulos del escáner de la policía («Tenemos un 10-… en <zona>», lista de ~160 zonas y tipos de vehículo) y de la voz `SPC_FA_POL_ANN` (`s_...` en 0xB6DB6C) | **No implementado** (el encargo era voces de peatones) |
| N2PD (pantalla dividida), tabla `SPEECH` de otro script | integración con mods de dos jugadores | No aplica |

### Por qué hay un `Tick`

El inicio de la voz se registra con un gancho, sin sondeo. El final no tiene un único punto: `UpdateParameters(-1)`, `StopCurrentSpeech`, `Terminate` y `Reset` lo producen, y
engancharlos todos añadiría cuatro hunks a una clase muy tocada. En cambio, el hueco de voz (`s_PedSpeechSlots`) refleja todos los finales a la vez (se vacía con `Reset()`), así
que `Tick` (una llamada al principio de `CMessages::Process`, ya una vez por fotograma de juego) lo comprueba: recorre como mucho 6 registros. `Tick` también es necesario para
mantener vivo el mensaje, ceder el sitio si el juego encola uno y fijar el tiempo de lectura. Con nada activo, `Tick` sale en tres comparaciones.

### Mapa de claves

Los `.fxt` son líneas `CLAVE texto` terminadas en CRLF, con claves de 7 caracteres y los textos en la codificación de fuente del juego (por eso salen bien las tildes y `¿ ¡`).

| Prefijo | Fichero | Contenido (según los ficheros instalados por el usuario) |
|---|---|---|
| `E` | `English.fxt` | texto en inglés de la voz (≈54 000 líneas, bancos 0–999, sonido 001–400) |
| `C` | `English.fxt` | variante con jerga (≈3 200 líneas) que sustituye a `E` cuando `ChicanoSlangs=1` |
| `S` | `Spanish.fxt` | español neutro (≈54 000 líneas) |
| `P` | `Spanish.fxt` | español de España (≈22 500 líneas) que sustituye a `S` cuando `SpainSpanish=1` |
| `M` | `Spanish.fxt` | español con las palabras extranjeras que dicen los peatones (ese, trucha, carnal…; ≈3 200 líneas) que sustituye a `P`/`S` cuando `ChicanoSlangs=1` |

Resto de la clave: `<banco 3 cifras><sonido+1 3 cifras>` (p. ej. banco 506, sonido 10 → `…506011`). El banco es `CAEPedSpeechAudioEntity::m_BankID` (`eSoundBank`) y el sonido
`m_SoundID`. Hay una clave duplicada en `English.fxt` (gana la primera). Las líneas largas llegan a 102 caracteres; el módulo copia hasta 159.

Memoria: el fichero entero en un bloque y un índice ordenado de 12 bytes por línea; `English.fxt` ≈ 2,6 MB y `Spanish.fxt` ≈ 3,8 MB en memoria (sin contar el otro: solo se carga el
del idioma en uso, la primera vez que una voz lo necesita; carga de 16–50 ms). Dentro del presupuesto de 5 MB.

## Decisiones propias (lo que no sale del script)

* **Voces de script (`m_LastGCtx == -1`) no se subtitulan.** El script de Kaizo sí las subtitula, pero en misiones y diálogos el juego ya muestra su subtítulo y un mensaje
  propio en cola retrasaría el del juego. Se prefiere conservar lo que hace el juego (requisito del encargo).
* **Los subtítulos del juego no se sustituyen** (`GF_*`, `DEAL*`, `PROS_*`, tiendas): se muestra el del juego y nunca el del módulo a la vez (ver tabla).
* **Un mensaje por voz** en lugar de uno por fotograma, con un anillo de 20 textos persistentes.
* **Hablante más reciente / el más cercano al acabar el actual**, en lugar del último del recorrido del pool (orden arbitrario).
* **Gancho de inicio + comprobación del hueco** en lugar de sondear la memoria del peatón.
* **Interruptor general y apagado en caliente por ini** (no existen en el script).
* Sin reserva a `English.fxt` en un juego en español sin `Spanish.fxt`.

## Pruebas

* **Banco en proceso** (`GTASA_FRS_SELFTEST=1`, `logs\free_roam_subs_selftest.log`): valores por defecto de Kaizo, orden de prefijos en las 6 combinaciones de idioma/opciones, formato
  de clave y casos límite (banco/sonido nulos o negativos), recorte de `ReadingTime`, radios por banco, lectura del ini (estilo Kaizo, valores no numéricos, interruptor y carpeta),
  tabla `.fxt` sintética (CRLF/LF, claves duplicadas o inválidas, la primera gana) y de 60 000 líneas (16–31 ms, 3,6 MB), y carga de los `.fxt` reales si están instalados. 0 fallos.
* **Banco en juego** (`GTASA_FRS_SELFTEST=2`, `logs\free_roam_subs_juego.log`, lanzador `reversing/FnJuego/ejecutar_frs.ps1` y capturas de ventana completa DPI-aware
  `frs_captura.py`): CJ y un peatón creado por `CPopulation::AddPed` hablan por el camino real del juego (`CPed::Say` → `AddSayEvent` → `PlayLoadedSound`): subtítulo correcto,
  duración = voz + `ReadingTime`, peatón a 14 m descartado, dos hablantes a la vez, Brief y opciones. Evidencia en `reversing/FnJuego/ejecuciones/`:
  * `on3-*` (opciones por defecto, español): `free_roam_subs_juego.log` y capturas (`cap_01`: CJ, `cap_07`: peatón a 4 m, `cap_21`: dos hablantes, `cap_06`: sin subtítulo después). Voz de 1,0–2,7 s;
    el subtítulo se retira ≈ 270 ms después de acabar la voz (`ReadingTime`=200 + un fotograma). Peatón a 14 m: `DESCARTADA fuera de rango`.
  * `off-*` (`GTASA_FREE_ROAM_SUBS=0`): el peatón habla (`Say` = suena) y la captura `cap_03_suena_a.png` no muestra subtítulo; el log no tiene `VOZ`/`MOSTRADO`.
  * `opts-*` (`GTASA_FRS_LANGUAGE=0 GTASA_FRS_CHICANOSLANGS=0 GTASA_FRS_READINGTIME=2000 GTASA_FRS_SAVETOBRIEF=0`): textos en inglés con claves `E…`, subtítulo visible 3,7 s (voz 1,65 s + 2 s),
    Brief vacío (con `SaveToBrief=1`, `on4-*`, quedan 4 entradas en `CMessages::PreviousBriefs`).
  * Ejecución del standalone de 120 s sin banco de pruebas (`r120-*`): `excepciones=0`.

## Qué queda fuera o sin verificar

Ver el informe del commit: policía y voz del banco 421, patches de `DrawSubtitles` y `TALK_1`, N2PD; conversaciones de misión y voces de script no se subtitulan; peatones sin texto
en el FXT (se registran como `SIN TEXTO` en el banco en juego); no se midió el rendimiento más allá de que `Tick` es de coste nulo sin voces y de la carga perezosa de la tabla.
