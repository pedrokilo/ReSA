# GInputSA 1.11: tablas de datos propias del mod

Origen: analisis estatico de `GInputSA.asi` (base 0x10000000). Las direcciones son del .asi salvo que se diga lo contrario. Todo extraido leyendo el binario; no se copio codigo.

## 1. Tabla de correcciones GXT (ApplyGXTFixes): 124 entradas

Estructura de 0x28 bytes en 0x100384D0 (68 entradas estaticas) y 0x10038F70 (56 construidas por el inicializador estatico 0x10001280): clave GXT de hasta 8 caracteres, puntero al patron (regex de ECMAScript, con tokens `~k~~ACCION~` del juego), puntero al reemplazo (con tokens propios `~@NOMBRE~`, `~o~`, `~x~`, `~<>~`, `~ud~`, `~[u~`, `~[d~`, `~[x~`, `~[y~`, `~>~`, `~<~`, `~[~`, `~v~` y referencias `$1`). Las 9 ultimas (FEH_*, FET_*, RYD1_93) tienen reemplazo nulo: el segundo campo es el texto completo que sustituye al GXT original. Las claves `DRV3_*`, `HM9_*`, etc. se reconstruyeron con la lista de cadenas porque el lector solo cargaba 8 bytes.

| # | clave | patron | reemplazo |
|---|---|---|---|
| 0 | HELPMO | `~k~~GO_FORWARD~.+~k~~GO_BACK~.+~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@PED_MOVE~` |
| 1 | HELP27 | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~@GO_UPDOWN~` |
| 2 | INTRO2C | `~k~~VEHICLE_ACCELERATE~` | `~x~` |
| 3 | INTRO2F | `~k~~VEHICLE_ACCELERATE~` | `~x~` |
| 4 | IE20 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 5 | IE21 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 6 | JCK_HLP | `~k~~PED_FIREWEAPON~` | `~o~` |
| 7 | MEL_HLP | `~k~~PED_FIREWEAPON~` | `~o~` |
| 8 | MODH1 | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| 9 | MODH2 | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| 10 | MODH3 | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| 11 | MODH5 | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~ud~` |
| 12 | NITROH1 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 13 | AMMU_H | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 14 | AMMU_H2 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 15 | BARB_H1 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 16 | BJ_H1 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 17 | BJ_H2 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 18 | BJ_H3 | `~k~~PED_FIREWEAPON~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~o~$1~v~` |
| 19 | BJ_H5 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 20 | CINCAM | `~k~~VEHICLE_TURRETLEFT~` | `~@CAMERA_LEFT_RIGHT~` |
| 21 | CLOTHA | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 22 | CLOTHB | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 23 | CONS_1 | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~[x~` |
| 24 | CONS_1B | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~[x~` |
| 25 | CONS_2 | `~k~~GO_BACK~(.+)~k~~GO_FORWARD~` | `~[u~$1~[d~` |
| 26 | CONS_2B | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~[y~` |
| 27 | FOOD_H | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| 28 | FOOD_H2 | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| 29 | GA_6 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 30 | GA_7 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 31 | GF_0020 | `~k~~GO_FORWARD~.+~k~~GO_BACK~(.+)~k~~PED_FIREWEAPON~` | `~[y~$1~o~` |
| 32 | GF_0043 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 33 | GF_RAD | `~k~~VEHICLE_RADIO_STATION_UP~.+~k~~VEHICLE_RADIO_STATION_DOWN~` | `~@VEHICLE_CHANGE_RADIO_STATION~` |
| 34 | GYM1_36 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 35 | GYM1_37 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 36 | GYM1_41 | `~k~~CONVERSATION_NO~.+~k~~CONVERSATION_YES~` | `~<>~` |
| 37 | GYM1_43 | `~k~~CONVERSATION_NO~.+~k~~CONVERSATION_YES~` | `~<>~` |
| 38 | GYM1_60 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 39 | GYM1_71 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 40 | HELP32 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 41 | QUAR_1 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 42 | REMOH1 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 43 | ROUHELP | `~k~~PED_FIREWEAPON~` | `~o~` |
| 44 | SKIP_1 | `~k~~CONVERSATION_YES~` | `~>~` |
| 45 | STR_02 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 46 | TATTA | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 47 | TATTB | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 48 | TRAINS | `~k~~VEHICLE_ACCELERATE~` | `~x~` |
| 49 | WARDH1 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 50 | WARDH2 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 51 | WARDH3 | `~k~~GO_FORWARD~.+~k~~GO_BACK~` | `~ud~` |
| 52 | WINCH_V | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~(.+)~k~~PED_FIREWEAPON~` | `~@CAMERA_UP_DOWN~$1~o~` |
| 53 | WOFHD01 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 54 | WZI2_A5 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 55 | BCOU_84 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 56 | BCOU_9 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 57 | BOAT_72 | `~<~~>~` | `~<>~` |
| 58 | BOAT_B4 | `~k~~VEHICLE_STEERLEFT~ and ~k~~VEHICLE_STEERRIGHT~` | `~@GO_LEFTRIGHT~` |
| 59 | BS_X03 | `~<~~>~` | `~<>~` |
| 60 | CAS9_H2 | `~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~` |
| 61 | CAT4_45 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 62 | CSH3_52 | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| 63 | DS1_33 | `~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~` | `~@GO_UPDOWN~` |
| 64 | DS1_60 | `~k~~GO_LEFT~` | `~<~` |
| 65 | DS1_61 | `~k~~GO_RIGHT~` | `~>~` |
| 66 | DS1_62 | `~<~~>~` | `~<>~` |
| 67 | DES5_9B | `~k~~PED_FIREWEAPON~` | `~o~` |
| 68 | DES5_CB | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| 69 | DES5_CC | `~k~~PED_FIREWEAPON~` | `~o~` |
| 70 | DES5_CE | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| 71 | DES5_CI | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~@GO_LEFTRIGHT~` |
| 72 | DES5_FK | `~<~~>~` | `~<>~` |
| 73 | DES5_FI | `~k~~GO_LEFT~` | `~<~` |
| 74 | DES5_FJ | `~k~~GO_RIGHT~` | `~>~` |
| 75 | DST8_30 | `~k~~GO_RIGHT~` | `~k~~PED_CYCLE_TARGET_RIGHT~` |
| 76 | DST8_31 | `~k~~GO_LEFT~` | `~k~~PED_CYCLE_TARGET_LEFT~` |
| 77 | DST8_32 | `~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~` |
| 78 | DST8_37 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 79 | SPAC_10 | `~<~~u~~d~~>~` | `~[~` |
| 80 | SPAC_11 | `~<~~>~(.+)~k~~PED_FIREWEAPON~` | `~[x~$1~o~` |
| 81 | SPAC_13 | `~u~~d~` | `~[y~` |
| 82 | DRV3_3 | `~k~~VEHICLE_FIREWEAPON_ALT~` | `~@SNATCH_PACKAGE~` |
| 83 | DRV3_H | `~k~~VEHICLE_FIREWEAPON_ALT~` | `~@SNATCH_PACKAGE~` |
| 84 | SPRAY_1 | `~k~~PED_FIREWEAPON~.+~k~~VEHICLE_FIREWEAPON_ALT~` | `~o~` |
| 85 | HEI2_11 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 86 | HEI3_25 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 87 | HM9_24 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 88 | HM9_49 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 89 | INTRO2D | `~k~~VEHICLE_FIREWEAPON_ALT~` | `~@BMX_BUNNYHOP~` |
| 90 | LOW_37 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 91 | HYDHLP1 | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| 92 | HYDHLP2 | `~k~~VEHICLE_HANDBRAKE~` | `~@HYDRA_TARGET~` |
| 93 | HYDHLP3 | `~k~~VEHICLE_HANDBRAKE~` | `~@HYDRA_TARGET~` |
| 94 | MAN2_70 | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| 95 | MAN2_71 | `~k~~VEHICLE_HANDBRAKE~` | `~@HYDRA_TARGET~` |
| 96 | MAN2_98 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 97 | PL_H1 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 98 | QUAR_H1 | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| 99 | RACES33 | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| 100 | RM4_32 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 101 | RUN_XX | `~k~~GO_LEFT~.+~k~~GO_RIGHT~` | `~<>~` |
| 102 | RY2_68 | `~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~` | `~@CAMERA_UP_DOWN~` |
| 103 | STE2_03 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 104 | ST4_H03 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 105 | STE5_03 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 106 | STP2_10 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 107 | STP2_11 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 108 | HOOD2B | `~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~` | `~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~` |
| 109 | S6_37 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 110 | SYN7_20 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 111 | ZER2_3 | `~k~~VEHICLE_STEERDOWN~.+~k~~VEHICLE_STEERUP~` | `~@GO_UPDOWN~` |
| 112 | ZER2_4 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 113 | Z4_H13 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 114 | Z4_H14 | `~k~~PED_FIREWEAPON~` | `~o~` |
| 115 | FEH_MPB | `~ud~ - MOVE UP/DOWN~n~~x~ - TOGGLE OPTION` | (texto completo: el patron es el nuevo texto) |
| 116 | FET_CIG | `BACKSPACE - CLEAR~n~~x~ - CHANGE` | (texto completo: el patron es el nuevo texto) |
| 117 | FET_APP | `~x~ - APPLY NEW SETTING` | (texto completo: el patron es el nuevo texto) |
| 118 | FEH_SNC | `~x~ - SCAN USER TRACKS` | (texto completo: el patron es el nuevo texto) |
| 119 | FEH_JMP | `~x~ - ENTER MENU` | (texto completo: el patron es el nuevo texto) |
| 120 | FEH_BPO | `~x~ - BACK` | (texto completo: el patron es el nuevo texto) |
| 121 | FET_MIG | `~<>~ - ADJUST` | (texto completo: el patron es el nuevo texto) |
| 122 | FEH_SSA | `~ud~ - MOVE~n~S - SAVE TO FILE` | (texto completo: el patron es el nuevo texto) |
| 123 | RYD1_93 | `When wearing the ski mask gently push ~@PED_MOVE~ to sneak.` | (texto completo: el patron es el nuevo texto) |

Mecanismo: hook de `CText::Get` (0x6A0050) -> 0x10009BE0 -> 0x10009680: CRC32 de la clave en mayusculas; busqueda en la tabla ordenada (0x1003AE74, 124 pares CRC/puntero creados por 0x10009900 con comparador 0x10009520); solo si hay iconos de pad (0x1003AB63) y ApplyGXTFixes (0x1003AB2A). Clave especial `GPUTERR` = mensaje de error de texturas.

Tokens propios de reemplazo vistos (se resuelven en `CFont::ParseToken` 0x10009040 / 0x10008E80): ~@PED_MOVE~, ~@GO_UPDOWN~, ~@GO_LEFTRIGHT~, ~@CAMERA_LEFT_RIGHT~, ~@CAMERA_UP_DOWN~, ~@VEHICLE_CHANGE_RADIO_STATION~, ~@SNATCH_PACKAGE~, ~@BMX_BUNNYHOP~, ~@HYDRA_TARGET~ (nombres en 0x10033xxx); los iconos de un solo caracter se mapean a sprites (ver seccion 5).

## 2. Mapeo de botones por juego de controles (Mode 0 = Setup 1, Mode 1 = Setup 2 estilo GTA IV)

`CPad::Mode` vale ControlsSet-1 (tabla 0x1003AB2C por pad); lo escribe `ConfigPad::SetActive` (0x10015050) al activar el pad y se pone a 0 al volver al teclado. Cada reemplazo bifurca por Mode. Columnas: campos de `CControllerState` leidos (New = estado actual, Old = anterior) en cada rama, extraidos automaticamente del codigo del reemplazo; la columna "significado" es la lectura del reemplazo (ver el CSV). Los nombres de campos siguen `source/game_sa/ControllerState.h` (L1=LeftShoulder1, R1=RightShoulder1, L3=ShockButtonL, R3=ShockButtonR). Contraste: `GAME CONTROLS FULL LIST.txt` del paquete concuerda en las filas comprobadas (acelerar Cross / R2, freno Square / L2, freno de mano R1 / Cross, mirar atras L2+R2 / L1+R1, objetivo R1 / L2).

| funcion del juego | reemplazo .asi | Setup 1 (Mode 0) | Setup 2 (Mode 1) | significado |
|---|---|---|---|---|
| CPad::GetLookLeft | 0x10002AE0 | New.L2, New.R2 | New.L1, New.R1 | GetLookLeft usado por el jetpack: L2 sin R2 (Setup 1) o L1 sin R1 (Setup 2). |
| CPad::GetLookRight | 0x10002B30 | New.R2, New.L2 | New.R1, New.L1 | GetLookRight usado por el jetpack: R2 sin L2 (Setup 1) o R1 sin L1 (Setup 2). |
| CPad::GetEnterTargeting | 0x10002BC0 | New.R1, Old.R1 | New.L2, Old.L2 | GetEnterTargeting a pie (PlayerOnFoot): R1 (Setup 1) o L2 (Setup 2) con flanco New/Old; ademas llama a puntero |
|  | 0x10002D80 | New.Square, New.R2 | New.RSY, New.LSY | SniperZoomIn: Setup 1 Square/R2; Setup 2 un eje de stick (RSY/LSY) con sensibilidad propia (0x1003ACA4). |
|  | 0x10002E50 | New.Cross, New.L2 | New.RSY, New.LSY | SniperZoomOut: Setup 1 Cross/L2; Setup 2 un eje de stick (RSY/LSY). |
|  | 0x10002F10 | New.L1 | New.R1 | GetForceCameraBehindPlayer: L1 (Setup 1) o R1 (Setup 2). |
| CPad::GetTarget | 0x10002F40 | New.R1 | New.Square | GetTarget en CVehicle::GetPlaneWeaponFiringStatus: R1 (Setup 1) o Square (Setup 2). |
| CPad::GetEnterTargeting | 0x10002F70 | New.R1, Old.R1 | New.Square, Old.Square | GetEnterTargeting en GetPlaneWeaponFiringStatus: R1 (Setup 1) o Square (Setup 2) con flanco. |
|  | 0x10002FB0 | New.L1, Old.L1 |  | Salto BMX (ProcessBunnyHop): tras DisablePlayerControls usa L1 (Setup 1) o Square (Setup 2) con flanco; ademas |
|  | 0x10003170 | New.Circle, Old.Circle, bDisFireL1, New.L1, Old.L1 | New.Circle, Old.Circle | CarGunJustDown: Setup 1 Circle (y L1 si no se desactivo el fuego con L1); Setup 2 solo Circle. |
| CPad::CarGunJustDown | 0x100031D0 | New.Circle, Old.Circle, bDisFireL1, New.L1, Old.L1 | New.Circle, Old.Circle, New.Cross, Old.Cross | CarGunJustDown en avion: Setup 1 Circle/L1; Setup 2 Circle o Cross (flanco). |
| CPad::CarGunJustDown | 0x10003240 | New.Circle, Old.Circle, bDisFireL1, New.L1, Old.L1 | New.Circle, Old.Circle, New.Cross, Old.Cross | CarGunJustDown en avion con intercambio por pad: Setup 2 acepta Cross y Circle. |
|  | 0x100032F0 | New.Circle, bDisFireL1, New.L1 | New.Circle | GetCarGunFired: Setup 1 Circle/L1; Setup 2 Circle. |
| CPad::GetCarGunFired | 0x10003340 | New.Circle, bDisFireL1, New.L1 | New.Circle, New.Cross | GetCarGunFired en avion: Setup 1 Circle/L1; Setup 2 Circle o Cross. |
| CPad::LastStationJustUp | 0x10003390 | New.Down, Old.Down | New.Left, Old.Left | LastStationJustUp (emisora anterior): Setup 1 cruceta Abajo, Setup 2 cruceta Izquierda; con temporizador de pu |
| CPad::NextStationJustUp | 0x100033F0 | New.Up, Old.Up | New.Right, Old.Right | NextStationJustUp (emisora siguiente): Setup 1 cruceta Arriba, Setup 2 cruceta Derecha. |
|  | 0x10003450 | bDisFireL1, TempMouse.L3, New.Circle, Old.Circle, New.L1, Old.L1 |  | WeaponJustDown a pie: Circle/L1 (y R2) segun Setup y bDisablePlayerFireWeapon*, con GetWeaponInfo del juego (p |
|  | 0x10003590 | bDisFireL1, TempMouse.L3, New.L1, New.Circle |  | GetWeapon a pie: igual que el anterior sin flanco. |
|  | 0x100038A0 | New.R1 | New.L2 | GetTarget: R1 (Setup 1) o L2 (Setup 2). |
|  | 0x100038D0 | New.R2, Old.R2, New.L2, Old.L2 | New.R1, Old.R1 | ShiftTargetRightJustDown: R2 (Setup 1) o R1 (Setup 2), con flanco. |
|  | 0x10003920 | New.L2, Old.L2, New.R2, Old.R2 | New.L1, Old.L1 | ShiftTargetLeftJustDown: L2 (Setup 1) o L1 (Setup 2), con flanco. |
|  | 0x10003970 | New.R1, Old.R1 | New.L2, Old.L2 | GetEnterTargeting: R1 (Setup 1) o L2 (Setup 2), con flanco. |
| CPad::CycleWeaponRightJustDown | 0x100039B0 | New.R2, Old.R2, New.L2, Old.L2 | New.Right, Old.Right | CycleWeaponRightJustDown: R2 (Setup 1) o cruceta Derecha (Setup 2); respeta bDisablePlayerCycleWeapon. |
| CPad::CycleWeaponLeftJustDown | 0x10003A10 | New.L2, Old.L2, New.R2, Old.R2 | New.Left, Old.Left | CycleWeaponLeftJustDown: L2 (Setup 1) o cruceta Izquierda (Setup 2). |
|  | 0x10003B40 | New.L2, New.R2 | New.L1, New.R1 | GetLookLeft: L2 sin R2 (Setup 1) o L1 sin R1 (Setup 2). |
|  | 0x10003B80 | New.R2, New.L2 | New.R1, New.L1 | GetLookRight: R2 sin L2 (Setup 1) o R1 sin L1 (Setup 2). |
|  | 0x10003C20 | New.R2, New.L2 | New.R1, New.L1 | GetLookBehindForCar: R2 y L2 a la vez (Setup 1) o R1 y L1 (Setup 2). |
|  | 0x10003C60 | New.Cross | New.R2 | GetAccelerate: Cross (Setup 1) o R2 analogico (Setup 2). |
|  | 0x10003C90 | New.Square | New.L2 | GetBrake: Square (Setup 1) o L2 analogico (Setup 2). |
|  | 0x10003CC0 | New.R1 | New.Cross | GetHandBrake: R1 (Setup 1) o Cross (Setup 2). |
|  | 0x10004260 | New.Left, Old.Left | New.Cross, New.Down, Old.Down | ConversationNoJustDown: cruceta Izquierda (Setup 1) o cruceta Abajo / Cross (Setup 2). |
|  | 0x100042B0 | New.Right, Old.Right | New.Cross, New.Up, Old.Up | ConversationYesJustDown: cruceta Derecha (Setup 1) o cruceta Arriba / Cross (Setup 2). |
| CPad::ConversationNoJustDown | 0x10004300 | New.Left, Old.Left | New.Down, Old.Down | ConversationNoJustDown (tarea de proxeneta): Izquierda (Setup 1) o Abajo (Setup 2). |
| CPad::ConversationYesJustDown | 0x10004340 | New.Right, Old.Right | New.Up, Old.Up | ConversationYesJustDown (tarea de proxeneta): Derecha (Setup 1) o Arriba (Setup 2). |
| CPad::GetGroupControlForward | 0x10004380 | New.Up | New.Cross, New.Up | GetGroupControlForward: cruceta Arriba (Setup 1) o Cross+Arriba (Setup 2). |
| CPad::GetGroupControlBack | 0x100043C0 | New.Down | New.Cross, New.Down | GetGroupControlBack: cruceta Abajo (Setup 1) o Cross+Abajo (Setup 2). |
| CPad::GetLookRight | 0x10004800 | New.R2, New.L2 | New.R1, New.L1 | CHeli: sustituye 'call GetLookRight; movzx ecx,al; mov [esp+0x38],ecx...' por 'call 0x10004800; fstp [esi+0x98 |

Funciones con Southpaw / pad activo (sin rama por Mode, usan 0x1003ACE8[pad] y objeto+0x7D): GetPedWalkLeftRight/UpDown (LSX/LSY <-> RSX/RSY), GetCarGunLeftRight/UpDown, AimWeapon*, LookAround*, GetHorn/HornJustDown, GetLookBehindForPed, DuckJustDown, GetDuck (L3 <-> R3), GetSteering* (con direccion SIXAXIS).

## 3. Opciones del INI -> variables del .asi (lector 0x10009CE0)

Seccion `[GInput]` (globales) y `[Pad1]`/`[Pad2]` (por pad, indice 0/1). Lectura con `GetPrivateProfileIntW`; el INI es `GInputSA.ini` junto al .asi (el nombre se forma sustituyendo la extension, 0x1000D4B5). Se relee al volver de la pausa.

| clave | seccion | variable / efecto |
|---|---|---|
| Enable (DllMain) | GInput | si 0, el .asi no instala ningun parche (0x1000D50F) |
| Vibration | GInput | byte FrontEndMenuManager+0x20 (puntero 0x1003AE18) |
| PlayStationButtons | GInput | bandera de iconos PS (0x1003AB63 via SetActive) |
| ApplyMissionSpecificFixes | GInput | 0x1003AB5F (activa GetPadState por script, comandos dance/bdisply) |
| ApplyGXTFixes | GInput | 0x1003AB2A (activa la tabla de la seccion 1) |
| CheatsFromPad | GInput | 0x1003AB5B (activa el hook de DoCheats) |
| DisableOnFocusLost | GInput | 0x1003AB33 (WM_KILLFOCUS) |
| MapPadOneToPadTwo | GInput | 0x1003AB5A (pad 1 -> jugador 2; usado por UpdatePads/StopShaking) |
| FreeAim | GInput | 0x1003AB5E (apuntado libre; decide 0x1003AB37 en 0x10004400) |
| ControlsSet | PadN | palabra 0x1003AB2C[pad] = valor-1 (Mode) |
| SwapSticksDuringAiming | PadN | byte 0x1003ACC8[pad] |
| InvertLook | PadN | byte 0x1003AC94[pad] (= valor==0) |
| InvertLeftXAxis / InvertLeftYAxis / SwapLeftAxes | PadN | bytes 0x1003AB80 / 0x1003ABD8 / 0x1003ABC0 [pad] |
| InvertRightXAxis / InvertRightYAxis / SwapRightAxes | PadN | bytes 0x1003AB64 / 0x1003ABBC / 0x1003ABDC [pad] |
| Southpaw | PadN | byte 0x1003ACE8[pad] |
| FaceButtonsSensitivity | PadN | entero 0x1003AB90[pad] y float 0x1003AE58[pad] (= valor/255 o 0x1003ae58 por defecto 17/..) |
| LeftStickDeadzone | PadN | float 0x1003ABD0[pad] = valor*0.01 |
| LeftStickSensitivity | PadN | float 0x1003AC8C[pad] = valor*0.01 |
| RightStickDeadzone | PadN | float 0x1003AB84[pad] |
| RightStickSensitivity | PadN | float 0x1003ABC4[pad] |
| SixaxisSensitivity | PadN | float 0x1003ACB8[pad] = valor*0.01 |
| SixaxisReloading / Hydraulics / CarSteering / BikeSteering / BoatSteering / HeliSteering / PlaneSteering | PadN | bytes 0x1003ACB4 / 0x1003ACA0 / 0x1003ACCC / 0x1003ACC0 / 0x1003ACAC / 0x1003AC9C / 0x1003AC98 [pad] |
| EnableSixaxisHack | GInput | activa el parche de la DLL de SCP (0x1000A600) |
| GuideLaunchesOverlay | GInput | byte del primer objeto de pad (+0) |

Curva de los sticks (rutina estandar de XInput 0x10006EA0..0x100074A5; la ruta SCP 0x10007550 es analoga): valor/32767 -> zona muerta `(|v|-dz)/(1-dz)` con signo -> curva SOLO en el eje X de cada stick: con s <= 1 `s*x+(1-s)*x^3`, con s > 1 `(s-1)*cbrt(x)+(2-s)*x` (0x10017846 es una `cbrt`, no `pow`) -> inversion/tope +-1/permutacion -> x127 truncado. Correccion del catalogo previo: la rama con raiz cubica es la de s > 1 (no la de 1 > s). Coeficientes, tabla de valores y verificacion en `../lectura-mando-ginput.md`. Constantes del .rdata: 0.01 (0x100347EC), 255 (0x10034834), 25500 (0x1003483C), 17.0 (0x10034820), 20/30/35/50 (0x10034824..30), 1/32767 (0x10034800), 127 (0x10034818), 0.75 (0x10034808, tope de zona muerta), 2.0 (0x10034814, tope de sensibilidad), 1.1333333 (0x1003480C, gatillos).

## 4. Scripts del juego tratados (ApplyMissionSpecificFixes)

GInput no parchea el SCM en memoria con tablas de bytes: sustituye `CRunningScript::GetPadState` (0x485B10) y varios comandos (0x390, 0x494, 0x293, y dos del rango 1900-1999) y comprueba el nombre del script (CRunningScript+8, `strnicmp` de 8 bytes). Clasificacion de GetPadState (0x10005B20; codigo en `bl`):

| codigo | scripts |
|---|---|
| 1 | r3, bbchal, taxiodd, ambulan, firetru, copcar, burgjb, freight, pimp, bball, ryder2 |
| 2 | dskool, bskool, zero2, zero4, desert5, casino1 |
| 3 | casino9, ryder3 |
| 4 | driv2 |
| 5 | camera |
| 6 | gfdate, heist1, gymls, gymsf, gymlv, bcesar2, cokec, lowrcon |
| 7 | bcour |
| (especial) | boat: comprueba [script+0x14] antes de clasificar (rama en 0x10005D28) |

Otros scripts con tratamiento propio: `dance` (comandos 0x10006180/0x10006200 y GET_POSITION_OF_ANALOGUE_STICKS 0x100093F0), `lowgame` (0x100093F0), `bdisply` (LOAD_TEXTURE_DICTIONARY 0x100060F0, tabla de 4 sprites en 0x10033634). La comprobacion `ApplyMissionSpecificFixes` (0x1003AB5F) y el pad activo (objeto+0x7D) condicionan todos. Los codigos 1-7 se traducen a que boton de pad devuelve cada boton de script (0-20) en el cuerpo de 0x10005B20 (switch en 0x1000609C; lectura completa pendiente).

## 5. Texturas y tokens de iconos

TXD que carga (0x10007E50): `models\x360btns.txd`, `models\ps3btns.txd`, `models\sixaxis.txd` (y `pcbtns.txd` aparece como cadena). Nombres de textura referenciados (0x100335FC-0x10033760): ace, ame, ath, bl, blx, blxl, blxr, bly, blyu, circle, cross, dance, dlr, down, dud, e, l1, l2, l3, le, left, ngle, pm, pt, r1, r2, re, right, s, script, square, t, team, terface, thumbl, thumblx, thumblxl, thumblxr, thumbly, thumblyu, triangle, txd, up, ut, xd, ði, ðj.
Tokens que interpreta `ParseToken` (0x10009040): `~<~ ~>~ ~u~ ~d~ ~x~ ~o~ ~v~ ~[x~ ~[y~ ~[u~ ~[d~ ~<>~ ~ud~ ~@NOMBRE~` y el selector de sprite 1..4 de SIXAXIS (`~K~`, etc. segun la tabla de saltos 0x100092AC/0x1000930C/0x10009370/0x100093D4). El ancho del icono (tabla 0x1003ABF0, indexada por sprite 0..0x22) se pone en 0x1003B084 y sustituye a la constante 0x85F100 de `CFont::PrintChar`/`RenderFontBuffer`.

## 6. Trucos desde el pad

Tabla de 90 hashes (CRC32 con la tabla estandar 0x10033100, de la cadena de letras T,C,X,S,U,D,L,R,1,2,3,4 en mayusculas; la cadena es la secuencia ya normalizada) en 0x10032F00; el orden es el de `CCheat::m_aCheatHashKeys` del juego (el indice se pasa a ApplyCheat). Texto de aviso: GXT `CHEAT1` (activado) y `CHEAT8` (desactivado) (0x10032DFC/0x10032E04).

```
518BAD1E 867974FB 80750205 207BE5A5 97F19EA4 4DCF6EE5 D8F6171B A44D2630 D2331E02 AE882F29 8F590254 6A171AC9 B4EDE50D CBC466A6 7F9F9BE0 F0031628 2A849202 9454290D E6A1EEEE E5BB7DE4 B1865C87 D9606CB7 B533F81F E3ECA2DD F8027C81 1B184F2B 09656C54 62B594B1 C9B3A8FB 53EE1352 BAC116E1 8FA0B3BF 9C57938A 7620F788 FD27F498 56E4BB70 9A5F52B5 49BA4900 208B6A19 8644854E 51E41320 E89BF194 C3CC20CF 9BC98FB7 21953B77 9180B0BD 12D165A9 9C2E6D4E 7EB898F4 825A0D49 103FB71C 9A6CFBB3 2A379A4D A6F8D057 688F9533 7640E454 C3359459 E1D4D60D D354C163 4E6E459B ED0309C6 F0C6272D C31EC24F F3EFA85F B40E31E5 331804CD 44A95F0C C58051F4 F5FFEC07 CB32F7AE F9BC0421 F9205C67 F24C377C 646273B4 20DF2C9A CDE25320 7F816CFC A3CFE345 83018331 F35B86E2 532EC19E 77C91C9B E2512B4E BC4E2D7A 62F26615 F96D57D3 D5791CE1 EB433433 FA760386 ACE1A711
```

## 7. Versiones de ejecutable reconocidas (resolutor 0x10005210 y similares)

Se comprueba un dword en el EXE cargado: VA 0x458D21 == 0x3539F633 -> tipo 3; VA 0x42457C == 0x94BF -> tipo 0 (1.0 US); 0x4245BC -> tipo 0 con bandera EU; 0x4252FC -> tipo 1 (1.01 US); 0x42533C -> tipo 1 (EU); 0x45EC4A -> tipo 2. `DllMain` solo instala los parches si es tipo 0 (US o EU 1.0): las direcciones fijas del instalador son las de 1.0 US; para EU solo cambian las dos de WinMain/WndProc (0x1000E230, 0x1000E290). El resto de tipos solo se usan para resolver punteros (funciones/datos) en las rutinas 0x1000xxxx.

## 8. Punteros a funciones y datos del juego resueltos por GInput (inicializadores 0x10001000-0x10002000)

| variable del .asi | resolutor / direccion US | uso |
|---|---|---|
| 0x1003AD2C | CWeaponInfo::GetWeaponInfo 0x743C60 | se llama desde 0x10004400/0x10003450 (informacion del arma) |
| 0x1003AD38 | CCheat::ApplyCheat 0x438370 | aplicar truco |
| 0x1003AD18 | operando 0x53FB7B = CPad::Pads (0xB73458) | base de los CPad (indice = (pad-base)/0x134) |
| 0x1003AD14 | CPad::Update 0x541C40 | llamada original en 0x10004400 |
| 0x1003AD08 | CPad::UpdateMouse 0x53F3C0 | llamada original en 0x100045B0 |
| 0x1003AD20 | CCheat::DoCheats 0x439AF0 | llamada original en 0x10004950 |
| 0x1003AD1C | CHud::SetHelpMessage 0x588BE0 | mostrar CHEAT1/CHEAT8 |
| 0x1003AD3C | CStats::IncrementStat 0x55C180 | contar trucos usados |
| 0x1003AD10 | operando 0x438481 = byte 0xB5F851 | bandera leida por DoCheats |
| 0x1003AD34 | operando 0x438532 = CCheat::m_aCheatsActive (0x969130) | estado de cada truco |
| 0x1003AD24 | operando 0x438578 = 0x96918C (m_bHasPlayerCheated) |  |
| 0x1003AD30 | operando 0x438539 = TheText (0xC1B340) | obtener textos GXT |
| 0x1003AD48 | CText::Get 0x6A0050 | texto original |
| 0x1003AD50 | CMenuManager::MessageScreen 0x579330 |  |
| 0x1003AE1C | operando 0x7190ED = 0xC71A54 (PS2Symbol) | tabla de simbolos de CFont |
| 0x1003AE14 | operando 0x4242D1 = 0xB7CB84 | temporizador (m_snTimeInMilliseconds / radio) |
| 0x1003AE18 | operando 0x4054DC = FrontEndMenuManager (0xBA6748) | vibracion (+0x20) |
| 0x1003AE0C | operando 0x50A851 = 0xB6EC2E (m_bUseMouse3rdPerson) | bandera de raton 3a persona |
| 0x1003AE24 | App::DoRWStuffEndOfFrame 0x53D840 |  |
| 0x1003AE28 | CWorld::FindPlayerPed 0x56E210 | jugador |
| 0x1003AE04 | CWorld::FindPlayerVehicle 0x56E0D0 | vehiculo del jugador |
| 0x1003AD58 | operando 0x52DE28 = 0xB733A8 (NewKeyState.esc) | menu: ESC |
| 0x1003AE00 | operando 0x52E505 = 0xB73138 (OldKeyState.esc) |  |
| 0x1003AE10 | CMenuManager::CheckForMenuClosing+0x3F | continuacion tras el hook 0x576B9F |
| 0x1003AE08 | Input::HandleKeyDown 0x743DF0 |  |
| 0x1003AD5C | CControllerConfigManager::GetIsKeyboardKeyJustDown 0x52E450 |  |
| 0x1003AE2C | CPlayerPed::DrawTriangleForMouseRecruitPed 0x60BA80 |  |
| 0x1003B080 | CControllerConfigManager::GetGxtStringOfCommandKeys 0x5303D0 |  |
| 0x1003ABE0 | CFont::ParseToken 0x718F00 | ParseToken original |
| 0x1003ABCC/ABE8/ACF8/AB9C/ABA0/ABE4/ACF4/ACFC | CTxdStore: AddTxdSlot 0x731C80, RemoveTxdSlot 0x731CD0, FindTxdSlot 0x731850, AddRef 0x731A00, LoadTxd 0x7320B0, PopCurrentTxd 0x7316B0, PushCurrentTxd 0x7316A0, SetCurrentTxd 0x7319C0 | carga de TXD de botones |
| 0x1003AE44/AE48 | CSprite2d::Delete 0x727240 / SetTexture 0x7272B0 |  |
| 0x1003AE20 | ScriptParams (0xA43C78) | devolver valores a scripts |
