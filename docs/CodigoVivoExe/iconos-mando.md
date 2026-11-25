# Iconos del mando en las ayudas y textos

Desviación propia (no existe en el EXE; no depende de `GTASA_SILENTPATCH`), registrada en [DesviacionesConocidas.md](../DesviacionesConocidas.md), sección 15.B.
Idea y juegos de iconos de **GInputSA 1.11 de Silent** («Pad buttons in In-Game helps, replacing PC key names»); las texturas **no se redistribuyen**:
se cargan de la instalación del usuario. Atribución en [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md).

Código: `source/game_sa/PadButtonIcons.h/.cpp` (módulo propio) y hunks mínimos marcados `NOTSA` en `Messages.cpp` (`InsertPlayerControlKeysInString`),
`Font.cpp` (`GetStringWidth`, `Shutdown`), `Pad.cpp` (`ProcessPad`) y `extensions/SilentPatchConfig.*` (`OwnDeviationString`).

## Cómo instalar las texturas

Copiar a la carpeta `models` del juego (la misma donde está `pcbtns.txd`) los archivos del paquete GInputSA de Silent
(`Original(Reworked)(Incl.All TXD's)\models\`):

| Archivo | Juego de iconos |
| --- | --- |
| `models\ps3btns.txd` | botones PlayStation |
| `models\x360btns.txd` | botones Xbox |

Cada TXD tiene 30 texturas de 64×64 con los mismos nombres (`circle cross square triangle l1 l2 l3 r1 r2 r3 up down left right dlr dud thumb*`); el módulo usa
las 14 de botones. Si el archivo no existe no pasa nada: se escribe una línea en `logs\log.log` («Iconos del mando: no existe models\…») y los textos siguen con nombres de tecla,
exactamente como el EXE. Con la ASI sobre el EXE original basta lo mismo (el directorio de trabajo es la carpeta del juego). `sixaxis.txd` (acelerómetros, requiere el
controlador SCP) está fuera de alcance.

## Opciones

| Opción | Entorno | `gta-reversed.ini` | Valores | Por defecto |
| --- | --- | --- | --- | --- |
| Interruptor general | `GTASA_PAD_ICONS` | `PadIcons=` | `0` / `1` | `1`; `0` = comportamiento del EXE literal |
| Juego de iconos | `GTASA_PAD_BUTTONS` | `PadButtons=` | `ps` / `xbox` / `auto` | `ps` (el mapa de botones por defecto del proyecto es el de PS2) |

`auto` elige Xbox si el mando se anuncia como de Microsoft (el XInput del juego se registra como 0x045E:0x028E) o si el nombre de Windows del mando (`joyGetDevCaps`)
contiene «xbox»/«xinput»; si no, PlayStation. Un mando PlayStation emulado como Xbox (DS4Windows, Steam Input) se ve como Xbox: forzar `ps`.
El entorno gana al ini. Las opciones se leen una vez por proceso.

## Cómo funciona

1. Los textos de ayuda, mensajes y tutoriales llevan `~k~~NOMBRE_ACCION~` (p. ej. `Press ~k~~PED_ANSWER_PHONE~`). `CMessages::InsertPlayerControlKeysInString` lo sustituye por el
   texto de la tecla (`CControllerConfigManager::GetGxtStringOfCommandKeys`). Con iconos activos, antes se consulta `PadButtonIcons::WriteToken`: si el dispositivo activo es el mando
   y la acción tiene un botón de mando con icono, escribe la etiqueta de símbolo del botón (`~X~`, `~O~`, `~K~`…); si no, queda el texto del EXE (nunca texto vacío).
2. Las etiquetas de símbolo ya existían en `CFont::ParseToken` (`PS2Symbol`, `CFont::ButtonSprite[]`; las de PS2: `~u~ ~d~ ~<~ ~>~ ~x~ ~o~ ~q~ ~t~ ~k~=L1 ~m~=L2 ~j~=R1 ~v~=R2 ~a~=L3 ~c~=R3`).
   `CFont::PrintChar` dibuja el sprite en línea con la altura de la fuente (17 × alto de fuente, el mismo avance que `RenderFontBuffer`). En el PC solo estaban cargadas 4 flechas;
   `PadButtonIcons` carga el TXD elegido (`CTxdStore`, ranura `padbtn_ps` / `padbtn_xbox`) y pone sus texturas en `ButtonSprite[1..14]` la primera vez que hace falta un icono.
3. `CFont::GetStringWidth` no contaba el ancho de los símbolos (el EXE no los usa en PC); con iconos cargados se suma `17 × alto + contorno`, de modo que el ajuste de línea de las cajas de ayuda no se desborda.
4. Botón → icono (numeración de botones del juego, la del mando XInput del proyecto y la del mapa de PS2 por defecto): 1 círculo (B en Xbox), 2 cruz (A), 3 cuadrado (X), 4 triángulo (Y),
   5 L2 (LT), 6 R2 (RT), 7 L1 (LB), 8 R1 (RB), 10 L3, 11 R3, 13–16 cruceta. Select (9) y Start (12) no tienen icono en el TXD: texto. El botón es el que el jugador tenga asignado
   a esa acción en el mando (menú de controles), no uno fijo.
5. Último dispositivo: `PadButtonIcons::NoteInput` (llamado por `CPad::ProcessPad` de PAD1, con la lectura XInput o DirectInput ya normalizada) anota actividad del mando
   (cualquier botón, cruceta o stick a más de la mitad del rango) frente a teclado/ratón (cualquier tecla salvo ESC/Enter, que Start inyecta, movimiento del ratón ≥ 4 px o botón). Gana la actividad más reciente; sin actividad
   todavía decide el método de control del jugador. Sin lecturas de mando en los últimos 1,5 s no hay modo mando. El texto de una ayuda ya mostrada no cambia al cambiar de dispositivo (se resuelve al ponerla, como en el EXE).

## Qué viene del análisis de GInputSA.asi y qué es decisión propia

Análisis estático de `GInputSA.asi` (solo lectura; sin ejecutarlo; capstone/pefile; catálogo completo del otro agente en `D:\Original\_ginput_refnalisis\`: `ginput-modificaciones.csv`, `ginput-estructuras.md`, `ginput-tablas.md`). Solo se integra la lógica de iconos/ayudas/GXT sobre nuestro XInput y nuestro mapa PS2 (no se porta la capa XInput de GInput, ni SCP, ni Guide).

Del `.asi` (comportamiento reproducido con código propio):

- Los iconos se dibujan por el camino de símbolos de `CFont` (`ParseToken` redirigido a una función propia, `PrintChar` con tabla de sprites propia en lugar de `ButtonSprite`, `GetStringWidth` y `ProcessCurrentString` con ancho de símbolo propio): aquí `ParseToken` consulta primero `PadButtonIcons::ExtendedSymbol`, `PrintChar` pide `SpriteFor` y `GetStringWidth` suma `SymbolAdvance`.
- Los TXD `models60btns.txd` / `models\ps3btns.txd` con las texturas por nombre (`circle cross square triangle l1 l2 l3 r1 r2 r3 up down left right dlr dud thumbl thumblx/xl/xr/y/yu/yd thumbr…`); opción `PlayStationButtons` (aquí `PadButtons=ps|xbox|auto`).
- Etiquetas propias de icono: `~[~` `~]~` (stick izquierdo/derecho), `~[x~ ~[y~ ~]x~ ~]y~` (ejes), `~[<~ ~[>~ ~[u~ ~[d~` y las de `~]…~` (direcciones), `~ud~` (cruceta arriba/abajo), `~<>~` (izquierda/derecha); la correspondencia etiqueta → textura se deduce de los nombres (`dud`, `dlr`, `thumb*`) y de dónde se usan en la tabla: **no se ha cotejado con el código de 0x10008E80** (pendiente en el análisis del otro agente).
- Tabla de correcciones de textos del GXT («ApplyGXTFixes», 124 entradas: 68 estáticas en 0x100384D0 y 56 construidas en 0x10001280): clave GXT de hasta 8 caracteres → patrón regex (ECMAScript, con las etiquetas `~k~~ACCION~`) → sustituto con etiquetas de icono y `$1`; las 9 últimas (`FEH_*`, `FET_*`, `RYD1_93`) sustituyen el texto completo. Se aplica en `CText::Get` (0x6A0050, como el `.asi`) solo con el mando como dispositivo y los iconos cargados. La tabla completa está más abajo; claves con ortografía recuperada del inicializador (`DRV3_3`, `HM9_24`…) en las 56 construidas.
- Las acciones propias `~@NOMBRE~` (`PED_MOVE`, `BMX_HANDBRAKE`, `BMX_BUNNYHOP`, `CAMERA_LEFT_RIGHT`, `CAMERA_UP_DOWN`, `VEHICLE_CHANGE_RADIO_STATION`, `GO_LEFTRIGHT`, `GO_UPDOWN`, `SNATCH_PACKAGE`, `HYDRA_TARGET`) y el cambio teclado/mando por último dispositivo usado (`ConfigPad::SetActive`, byte +0x7D «pad activo»).
- Sin textura TXD el `.asi` cierra el juego con un mensaje; aquí (decisión del usuario/brief) el comportamiento es el del EXE y una línea de log.

Decisión propia (no sale del `.asi`):

- Detección del último dispositivo: actividad del mando (botón, cruceta o stick > 50 %) frente a teclado/ratón, sobre `CPad::ProcessPad`/`NewKeyState`/`NewMouseControllerState` de nuestro proyecto (el `.asi` usa su propia lectura).
- Mapa botón → icono: el del `CControllerConfigManager` del proyecto (mapa PS2 por defecto; 1 círculo … 16 cruceta izquierda).
- `~@NOMBRE~`: `PED_MOVE`, `GO_*`, `CAMERA_*` → iconos de stick; `VEHICLE_CHANGE_RADIO_STATION` → `~ud~` / `~<>~` si la emisora arriba/abajo están en la cruceta (si no, los dos botones asignados); `BMX_*`, `HYDRA_TARGET`, `SNATCH_PACKAGE` → el botón asignado a `VEHICLE_HANDBRAKE` / `VEHICLE_FIRE_WEAPON_ALT` (el `.asi` los resuelve con su propia configuración por Setup).
- Ancho de icono: avance fijo `17 × alto de fuente` (como el de los símbolos del juego) y sprite cuadrado; el `.asi` calcula un ancho variable por textura (constante 0x85F100 redirigida): no reproducido, las texturas de los TXD son todas 64×64.
- Ayuda ya mostrada: no cambia al cambiar de dispositivo.

## Qué cubre y qué no

Cubre todo texto que pase por `InsertPlayerControlKeysInString` (ayudas, incluidas las de misión y tutorial, mensajes grandes y breves, `PRINT_*` con teclas, ayuda del menú) y los textos de la tabla de sustituciones cuando se obtienen con `CText::Get`.
No cubre: el menú de controles (muestra los nombres de botón asignados, `JOY n`), la pantalla de controles de GInput, acciones sin botón de mando fuera de la tabla (movimiento y cámara con los sticks en textos que no estén en ella, `PED_LOOKBEHIND` y combinaciones: texto del EXE), `sixaxis.txd` y las opciones de teclado frente a mando de otros textos que el `.asi` trata en su `CMessages`.
Con teclado como último dispositivo los textos son los del EXE (si el método de control es «mando», el EXE ya escribe `JOY n`).

## Tabla de sustituciones de GXT (124 entradas)

| Clave GXT | Patrón (ECMAScript) | Sustituto |
| --- | --- | --- |
| `HELPMO` | `~k~~GO_FORWARD~.+~k~~GO_BACK~.+~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@PED_MOVE~` |
| `HELP27` | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~@GO_UPDOWN~` |
| `INTRO2C` | `~k~~VEHICLE_ACCELERATE~` | `~x~` |
| `INTRO2F` | `~k~~VEHICLE_ACCELERATE~` | `~x~` |
| `IE20` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `IE21` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `JCK_HLP` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `MEL_HLP` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `MODH1` | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| `MODH2` | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| `MODH3` | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| `MODH5` | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| `NITROH1` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `AMMU_H` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `AMMU_H2` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `BARB_H1` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `BJ_H1` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `BJ_H2` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `BJ_H3` | `~k~~PED_FIREWEAPON~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~o~$1~v~` |
| `BJ_H5` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `CINCAM` | `~k~~VEHICLE_TURRETLEFT~` | `~@CAMERA_LEFT_RIGHT~` |
| `CLOTHA` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `CLOTHB` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `CONS_1` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~[x~` |
| `CONS_1B` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~[x~` |
| `CONS_2` | `~k~~GO_BACK~(.+)~k~~GO_FORWARD~` | `~[u~$1~[d~` |
| `CONS_2B` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~[y~` |
| `FOOD_H` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| `FOOD_H2` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| `GA_6` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `GA_7` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `GF_0020` | `~k~~GO_FORWARD~.+~k~~GO_BACK~(.+)~k~~PED_FIREWEAPON~` | `~[y~$1~o~` |
| `GF_0043` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `GF_RAD` | `~k~~VEHICLE_RADIO_STATION_UP~.+~k~~VEHICLE_RADIO_STATION_DOWN~` | `~@VEHICLE_CHANGE_RADIO_STATION~` |
| `GYM1_36` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `GYM1_37` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `GYM1_41` | `~k~~CONVERSATION_NO~.+~k~~CONVERSATION_YES~` | `~<>~` |
| `GYM1_43` | `~k~~CONVERSATION_NO~.+~k~~CONVERSATION_YES~` | `~<>~` |
| `GYM1_60` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `GYM1_71` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `HELP32` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `QUAR_1` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `REMOH1` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `ROUHELP` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `SKIP_1` | `~k~~CONVERSATION_YES~` | `~>~` |
| `STR_02` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `TATTA` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `TATTB` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `TRAINS` | `~k~~VEHICLE_ACCELERATE~` | `~x~` |
| `WARDH1` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `WARDH2` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `WARDH3` | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| `WINCH_V` | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~(.+)~k~~PED_FIREWEAPON~` | `~@CAMERA_UP_DOWN~$1~o~` |
| `WOFHD01` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `WZI2_A5` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `BCOU_84` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `BCOU_9` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `BOAT_72` | `~<~~>~` | `~<>~` |
| `BOAT_B4` | `~k~~VEHICLE_STEERLEFT~ and ~k~~VEHICLE_STEERRIGHT~` | `~@GO_LEFTRIGHT~` |
| `BS_X03` | `~<~~>~` | `~<>~` |
| `CAS9_H2` | `~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~` |
| `CAT4_45` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `CSH3_52` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| `DS1_33` | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~@GO_UPDOWN~` |
| `DS1_60` | `~k~~GO_LEFT~` | `~<~` |
| `DS1_61` | `~k~~GO_RIGHT~` | `~>~` |
| `DS1_62` | `~<~~>~` | `~<>~` |
| `DES5_9B` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `DES5_CB` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| `DES5_CC` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `DES5_CE` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| `DES5_CI` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| `DES5_FK` | `~<~~>~` | `~<>~` |
| `DES5_FI` | `~k~~GO_LEFT~` | `~<~` |
| `DES5_FJ` | `~k~~GO_RIGHT~` | `~>~` |
| `DST8_30` | `~k~~GO_RIGHT~` | `~k~~PED_CYCLE_TARGET_RIGHT~` |
| `DST8_31` | `~k~~GO_LEFT~` | `~k~~PED_CYCLE_TARGET_LEFT~` |
| `DST8_32` | `~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~` |
| `DST8_37` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `SPAC_10` | `~<~~u~~d~~>~` | `~[~` |
| `SPAC_11` | `~<~~>~(.+)~k~~PED_FIREWEAPON~` | `~[x~$1~o~` |
| `SPAC_13` | `~u~~d~` | `~[y~` |
| `DRV3_3` | `~k~~VEHICLE_FIREWEAPON_ALT~` | `~@SNATCH_PACKAGE~` |
| `DRV3_H` | `~k~~VEHICLE_FIREWEAPON_ALT~` | `~@SNATCH_PACKAGE~` |
| `SPRAY_1` | `~k~~PED_FIREWEAPON~.+~k~~VEHICLE_FIREWEAPON_ALT~` | `~o~` |
| `HEI2_11` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `HEI3_25` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `HM9_24` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `HM9_49` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `INTRO2D` | `~k~~VEHICLE_FIREWEAPON_ALT~` | `~@BMX_BUNNYHOP~` |
| `LOW_37` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `HYDHLP1` | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| `HYDHLP2` | `~k~~VEHICLE_HANDBRAKE~` | `~@HYDRA_TARGET~` |
| `HYDHLP3` | `~k~~VEHICLE_HANDBRAKE~` | `~@HYDRA_TARGET~` |
| `MAN2_70` | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| `MAN2_71` | `~k~~VEHICLE_HANDBRAKE~` | `~@HYDRA_TARGET~` |
| `MAN2_98` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `PL_H1` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `QUAR_H1` | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| `RACES33` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| `RM4_32` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `RUN_XX` | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| `RY2_68` | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| `STE2_03` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `ST4_H03` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `STE5_03` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `STP2_10` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `STP2_11` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `HOOD2B` | `~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~` |
| `S6_37` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `SYN7_20` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `ZER2_3` | `~k~~VEHICLE_STEERDOWN~.+~k~~VEHICLE_STEERUP~` | `~@GO_UPDOWN~` |
| `ZER2_4` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `Z4_H13` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `Z4_H14` | `~k~~PED_FIREWEAPON~` | `~o~` |
| `FEH_MPB` | (texto completo) | `~ud~ - MOVE UP/DOWN~n~~x~ - TOGGLE OPTION` |
| `FET_CIG` | (texto completo) | `BACKSPACE - CLEAR~n~~x~ - CHANGE` |
| `FET_APP` | (texto completo) | `~x~ - APPLY NEW SETTING` |
| `FEH_SNC` | (texto completo) | `~x~ - SCAN USER TRACKS` |
| `FEH_JMP` | (texto completo) | `~x~ - ENTER MENU` |
| `FEH_BPO` | (texto completo) | `~x~ - BACK` |
| `FET_MIG` | (texto completo) | `~<>~ - ADJUST` |
| `FEH_SSA` | (texto completo) | `~ud~ - MOVE~n~S - SAVE TO FILE` |
| `RYD1_93` | (texto completo) | `When wearing the ski mask gently push ~@PED_MOVE~ to sneak.` |

## Pruebas

`reversing/FnJuego/sonda_juego.py` escenario `iconos` (con `GTASA_PAD_ICONS_DEMO=1`, ayuda de demostración cada 8 s con etiquetas `~k~~ACCION~`; solo pruebas) y
`ejecutar_juego.ps1 -XInput -Dest <carpeta con models propio>`. Resultados y capturas: `reversing/FnJuego/evidencia/iconos-mando/`.
