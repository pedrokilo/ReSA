# Comandos de script (opcodes)

Inventario de los comandos de SCM del juego (`docs/ComandosScript.csv`, generado por `contrib/inventario_comandos_script.py`).
No forman parte de `hooks.csv`: el EXE los atiende dentro de despachadores por centenas (`ProcessCommandsNNNtoMMM`) y no tienen
una funcion con direccion propia. Los contadores de `ReversedClasses.md` **no los incluyen**.

## Resumen

| Estado | Comandos | Significado |
| --- | ---: | --- |
| reescrito | 668 | manejador C++ propio; **sin banco de validacion contra el EXE** |
| nop | 282 | el comando no hace nada, como en el original |
| no implementado | 713 | el propio juego no lo implementa |
| original | 1163 | sin manejador propio: pasa por el despachador de su centena |
| **Total** | **2826** | |

De los 1163 comandos *original*: despachador reescrito en `hooks.csv` = 184; despachador del EXE = 794; sin despachador identificado = 185.

## Por categoria (archivo de `Scripts/Commands`)

| Categoria | Reescritos | Nop | No impl. | Original |
| --- | ---: | ---: | ---: | ---: |
| (sin manejador propio) | 0 | 0 | 0 | 1163 |
| Basic | 161 | 0 | 31 | 0 |
| Camera | 43 | 0 | 16 | 0 |
| Character | 170 | 0 | 160 | 0 |
| Clock | 5 | 0 | 0 | 0 |
| Conversation | 11 | 0 | 0 | 0 |
| Draw | 8 | 0 | 2 | 0 |
| Game | 36 | 9 | 116 | 0 |
| Math | 7 | 0 | 0 | 0 |
| Mission | 4 | 0 | 2 | 0 |
| Object | 27 | 0 | 74 | 0 |
| Pad | 5 | 0 | 0 | 0 |
| Path | 6 | 0 | 0 | 0 |
| Ped | 4 | 0 | 0 | 0 |
| Player | 69 | 29 | 96 | 0 |
| Script | 13 | 0 | 182 | 0 |
| Sequence | 1 | 0 | 0 | 0 |
| Stat | 10 | 0 | 1 | 0 |
| Text | 53 | 2 | 12 | 0 |
| Unused | 1 | 242 | 0 | 0 |
| Vehicle | 21 | 0 | 12 | 0 |
| Zone | 13 | 0 | 9 | 0 |

## Despachadores del EXE que atienden comandos sin manejador propio

| Despachador | Comandos | Reescrito en hooks.csv | Validado |
| --- | ---: | --- | --- |
| `0x00466DE0` | 14 | no | no |
| `0x00469390` | 6 | no | no |
| `0x0046B460` | 59 | si | si |
| `0x0046D050` | 60 | no | no |
| `0x00470A90` | 44 | no | no |
| `0x00472310` | 55 | no | no |
| `0x00474900` | 78 | no | no |
| `0x004762D0` | 58 | no | no |
| `0x00478000` | 79 | no | no |
| `0x00479DA0` | 17 | no | no |
| `0x0047A760` | 45 | no | no |
| `0x0047C100` | 27 | no | no |
| `0x0047D210` | 22 | no | no |
| `0x0047E090` | 23 | no | no |
| `0x0047F370` | 10 | no | no |
| `0x0047FA30` | 24 | no | no |
| `0x00481300` | 18 | si | si |
| `0x00483BD0` | 32 | no | no |
| `0x00489500` | 27 | no | no |
| `0x0048A320` | 21 | no | no |
| `0x0048B590` | 38 | si | si |
| `0x0048CDD0` | 19 | no | no |
| `0x0048EAA0` | 47 | no | no |
| `0x00490DB0` | 38 | no | no |
| `0x00493FE0` | 48 | no | no |
| `0x00496E00` | 69 | si | si |

## Comandos reescritos (sin validar por banco)

| Id | Comando | Manejador | Riesgo | Fuente |
| --- | --- | --- | --- | --- |
| 0x0000 | `COMMAND_NOP` | `Nop` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:393 |
| 0x0001 | `COMMAND_WAIT` | `Wait` | alto | source/game_sa/Scripts/Commands/BasicCommands.cpp:392 |
| 0x0002 | `COMMAND_GOTO` | `GoTo` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:379 |
| 0x0003 | `COMMAND_SHAKE_CAM` | `ShakeCam` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:312 |
| 0x0004 | `COMMAND_SET_VAR_INT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:344 |
| 0x0005 | `COMMAND_SET_VAR_FLOAT` | `(AssignTo<float, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:346 |
| 0x0006 | `COMMAND_SET_LVAR_INT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:345 |
| 0x0007 | `COMMAND_SET_LVAR_FLOAT` | `(AssignTo<float, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:347 |
| 0x0008 | `COMMAND_ADD_VAL_TO_INT_VAR` | `AddInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:304 |
| 0x0009 | `COMMAND_ADD_VAL_TO_FLOAT_VAR` | `AddInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:309 |
| 0x000A | `COMMAND_ADD_VAL_TO_INT_LVAR` | `AddInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:302 |
| 0x000B | `COMMAND_ADD_VAL_TO_FLOAT_LVAR` | `AddInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:305 |
| 0x000C | `COMMAND_SUB_VAL_FROM_INT_VAR` | `SubInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:290 |
| 0x000D | `COMMAND_SUB_VAL_FROM_FLOAT_VAR` | `SubInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:295 |
| 0x000E | `COMMAND_SUB_VAL_FROM_INT_LVAR` | `SubInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:289 |
| 0x000F | `COMMAND_SUB_VAL_FROM_FLOAT_LVAR` | `SubInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:293 |
| 0x0010 | `COMMAND_MULT_INT_VAR_BY_VAL` | `MultInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:314 |
| 0x0011 | `COMMAND_MULT_FLOAT_VAR_BY_VAL` | `MultInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:321 |
| 0x0012 | `COMMAND_MULT_INT_LVAR_BY_VAL` | `MultInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:315 |
| 0x0013 | `COMMAND_MULT_FLOAT_LVAR_BY_VAL` | `MultInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:318 |
| 0x0014 | `COMMAND_DIV_INT_VAR_BY_VAL` | `DivInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:328 |
| 0x0015 | `COMMAND_DIV_FLOAT_VAR_BY_VAL` | `DivInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:335 |
| 0x0016 | `COMMAND_DIV_INT_LVAR_BY_VAL` | `DivInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:326 |
| 0x0017 | `COMMAND_DIV_FLOAT_LVAR_BY_VAL` | `DivInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:336 |
| 0x0018 | `COMMAND_IS_INT_VAR_GREATER_THAN_NUMBER` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:261 |
| 0x0019 | `COMMAND_IS_INT_LVAR_GREATER_THAN_NUMBER` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:254 |
| 0x001A | `COMMAND_IS_NUMBER_GREATER_THAN_INT_VAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:265 |
| 0x001B | `COMMAND_IS_NUMBER_GREATER_THAN_INT_LVAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:264 |
| 0x001C | `COMMAND_IS_INT_VAR_GREATER_THAN_INT_VAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:259 |
| 0x001D | `COMMAND_IS_INT_LVAR_GREATER_THAN_INT_LVAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:258 |
| 0x001E | `COMMAND_IS_INT_VAR_GREATER_THAN_INT_LVAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:256 |
| 0x001F | `COMMAND_IS_INT_LVAR_GREATER_THAN_INT_VAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:260 |
| 0x0020 | `COMMAND_IS_FLOAT_VAR_GREATER_THAN_NUMBER` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:268 |
| 0x0021 | `COMMAND_IS_FLOAT_LVAR_GREATER_THAN_NUMBER` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:270 |
| 0x0022 | `COMMAND_IS_NUMBER_GREATER_THAN_FLOAT_VAR` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:272 |
| 0x0023 | `COMMAND_IS_NUMBER_GREATER_THAN_FLOAT_LVAR` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:269 |
| 0x0024 | `COMMAND_IS_FLOAT_VAR_GREATER_THAN_FLOAT_VAR` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:266 |
| 0x0025 | `COMMAND_IS_FLOAT_LVAR_GREATER_THAN_FLOAT_LVAR` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:271 |
| 0x0026 | `COMMAND_IS_FLOAT_VAR_GREATER_THAN_FLOAT_LVAR` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:273 |
| 0x0027 | `COMMAND_IS_FLOAT_LVAR_GREATER_THAN_FLOAT_VAR` | `IsGreater<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:267 |
| 0x0028 | `COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_NUMBER` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:233 |
| 0x0029 | `COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_NUMBER` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:242 |
| 0x002A | `COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_INT_VAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:237 |
| 0x002B | `COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_INT_LVAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:245 |
| 0x002C | `COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_INT_VAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:238 |
| 0x002D | `COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_INT_LVAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:234 |
| 0x002E | `COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_INT_LVAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:241 |
| 0x002F | `COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_INT_VAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:239 |
| 0x0030 | `COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_NUMBER` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:251 |
| 0x0031 | `COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_NUMBER` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:247 |
| 0x0032 | `COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_FLOAT_VAR` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:246 |
| 0x0033 | `COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_FLOAT_LVAR` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:252 |
| 0x0034 | `COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_FLOAT_VAR` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:248 |
| 0x0035 | `COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_FLOAT_LVAR` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:250 |
| 0x0036 | `COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_FLOAT_LVAR` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:244 |
| 0x0037 | `COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_FLOAT_VAR` | `IsGreaterEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:249 |
| 0x0038 | `COMMAND_IS_INT_VAR_EQUAL_TO_NUMBER` | `IsEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:276 |
| 0x0039 | `COMMAND_IS_INT_LVAR_EQUAL_TO_NUMBER` | `IsEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:277 |
| 0x003A | `COMMAND_IS_INT_VAR_EQUAL_TO_INT_VAR` | `IsEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:278 |
| 0x003B | `COMMAND_IS_INT_LVAR_EQUAL_TO_INT_LVAR` | `IsEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:279 |
| 0x003C | `COMMAND_IS_INT_VAR_EQUAL_TO_INT_LVAR` | `IsEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:275 |
| 0x0042 | `COMMAND_IS_FLOAT_VAR_EQUAL_TO_NUMBER` | `IsEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:281 |
| 0x0043 | `COMMAND_IS_FLOAT_LVAR_EQUAL_TO_NUMBER` | `IsEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:282 |
| 0x0044 | `COMMAND_IS_FLOAT_VAR_EQUAL_TO_FLOAT_VAR` | `IsEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:284 |
| 0x0045 | `COMMAND_IS_FLOAT_LVAR_EQUAL_TO_FLOAT_LVAR` | `IsEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:283 |
| 0x0046 | `COMMAND_IS_FLOAT_VAR_EQUAL_TO_FLOAT_LVAR` | `IsEqual<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:280 |
| 0x004D | `COMMAND_GOTO_IF_FALSE` | `GoToIfFalse` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:380 |
| 0x004E | `COMMAND_TERMINATE_THIS_SCRIPT` | `TerminateThisScript` | alto | source/game_sa/Scripts/Commands/BasicCommands.cpp:388 |
| 0x004F | `COMMAND_START_NEW_SCRIPT` | `StartNewScript` | alto | source/game_sa/Scripts/Commands/BasicCommands.cpp:389 |
| 0x0050 | `COMMAND_GOSUB` | `GoToSub` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:381 |
| 0x0051 | `COMMAND_RETURN` | `Return` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:384 |
| 0x0053 | `COMMAND_CREATE_PLAYER` | `CreatePlayer` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:464 |
| 0x0054 | `COMMAND_GET_PLAYER_COORDINATES` | `GetPlayerCoordinates` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:561 |
| 0x0056 | `COMMAND_IS_PLAYER_IN_AREA_2D` | `IsPlayerInArea2D` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:563 |
| 0x0057 | `COMMAND_IS_PLAYER_IN_AREA_3D` | `IsPlayerInArea3D` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:564 |
| 0x0058 | `COMMAND_ADD_INT_VAR_TO_INT_VAR` | `AddInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:300 |
| 0x0059 | `COMMAND_ADD_FLOAT_VAR_TO_FLOAT_VAR` | `AddInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:306 |
| 0x005A | `COMMAND_ADD_INT_LVAR_TO_INT_LVAR` | `AddInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:301 |
| 0x005B | `COMMAND_ADD_FLOAT_LVAR_TO_FLOAT_LVAR` | `AddInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:307 |
| 0x005C | `COMMAND_ADD_INT_VAR_TO_INT_LVAR` | `AddInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:299 |
| 0x005D | `COMMAND_ADD_FLOAT_VAR_TO_FLOAT_LVAR` | `AddInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:308 |
| 0x005E | `COMMAND_ADD_INT_LVAR_TO_INT_VAR` | `AddInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:303 |
| 0x005F | `COMMAND_ADD_FLOAT_LVAR_TO_FLOAT_VAR` | `AddInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:310 |
| 0x0060 | `COMMAND_SUB_INT_VAR_FROM_INT_VAR` | `SubInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:291 |
| 0x0061 | `COMMAND_SUB_FLOAT_VAR_FROM_FLOAT_VAR` | `SubInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:296 |
| 0x0062 | `COMMAND_SUB_INT_LVAR_FROM_INT_LVAR` | `SubInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:286 |
| 0x0063 | `COMMAND_SUB_FLOAT_LVAR_FROM_FLOAT_LVAR` | `SubInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:294 |
| 0x0064 | `COMMAND_SUB_INT_VAR_FROM_INT_LVAR` | `SubInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:287 |
| 0x0065 | `COMMAND_SUB_FLOAT_VAR_FROM_FLOAT_LVAR` | `SubInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:292 |
| 0x0066 | `COMMAND_SUB_INT_LVAR_FROM_INT_VAR` | `SubInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:288 |
| 0x0067 | `COMMAND_SUB_FLOAT_LVAR_FROM_FLOAT_VAR` | `SubInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:297 |
| 0x0068 | `COMMAND_MULT_INT_VAR_BY_INT_VAR` | `MultInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:313 |
| 0x0069 | `COMMAND_MULT_FLOAT_VAR_BY_FLOAT_VAR` | `MultInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:322 |
| 0x006A | `COMMAND_MULT_INT_LVAR_BY_INT_LVAR` | `MultInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:316 |
| 0x006B | `COMMAND_MULT_FLOAT_LVAR_BY_FLOAT_LVAR` | `MultInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:323 |
| 0x006C | `COMMAND_MULT_INT_VAR_BY_INT_LVAR` | `MultInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:317 |
| 0x006D | `COMMAND_MULT_FLOAT_VAR_BY_FLOAT_LVAR` | `MultInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:320 |
| 0x006E | `COMMAND_MULT_INT_LVAR_BY_INT_VAR` | `MultInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:312 |
| 0x006F | `COMMAND_MULT_FLOAT_LVAR_BY_FLOAT_VAR` | `MultInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:319 |
| 0x0070 | `COMMAND_DIV_INT_VAR_BY_INT_VAR` | `DivInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:325 |
| 0x0071 | `COMMAND_DIV_FLOAT_VAR_BY_FLOAT_VAR` | `DivInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:331 |
| 0x0072 | `COMMAND_DIV_INT_LVAR_BY_INT_LVAR` | `DivInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:329 |
| 0x0073 | `COMMAND_DIV_FLOAT_LVAR_BY_FLOAT_LVAR` | `DivInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:333 |
| 0x0074 | `COMMAND_DIV_INT_VAR_BY_INT_LVAR` | `DivInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:327 |
| 0x0075 | `COMMAND_DIV_FLOAT_VAR_BY_FLOAT_LVAR` | `DivInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:334 |
| 0x0076 | `COMMAND_DIV_INT_LVAR_BY_INT_VAR` | `DivInPlace<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:330 |
| 0x0077 | `COMMAND_DIV_FLOAT_LVAR_BY_FLOAT_VAR` | `DivInPlace<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:332 |
| 0x0078 | `COMMAND_ADD_TIMED_VAL_TO_FLOAT_VAR` | `AddTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:367 |
| 0x0079 | `COMMAND_ADD_TIMED_VAL_TO_FLOAT_LVAR` | `AddTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:368 |
| 0x007A | `COMMAND_ADD_TIMED_FLOAT_VAR_TO_FLOAT_VAR` | `AddTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:369 |
| 0x007B | `COMMAND_ADD_TIMED_FLOAT_LVAR_TO_FLOAT_LVAR` | `AddTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:371 |
| 0x007C | `COMMAND_ADD_TIMED_FLOAT_VAR_TO_FLOAT_LVAR` | `AddTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:372 |
| 0x007D | `COMMAND_ADD_TIMED_FLOAT_LVAR_TO_FLOAT_VAR` | `AddTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:370 |
| 0x007E | `COMMAND_SUB_TIMED_VAL_FROM_FLOAT_VAR` | `SubTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:364 |
| 0x007F | `COMMAND_SUB_TIMED_VAL_FROM_FLOAT_LVAR` | `SubTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:366 |
| 0x0080 | `COMMAND_SUB_TIMED_FLOAT_VAR_FROM_FLOAT_VAR` | `SubTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:365 |
| 0x0081 | `COMMAND_SUB_TIMED_FLOAT_LVAR_FROM_FLOAT_LVAR` | `SubTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:362 |
| 0x0082 | `COMMAND_SUB_TIMED_FLOAT_VAR_FROM_FLOAT_LVAR` | `SubTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:361 |
| 0x0083 | `COMMAND_SUB_TIMED_FLOAT_LVAR_FROM_FLOAT_VAR` | `SubTimedStore` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:363 |
| 0x0084 | `COMMAND_SET_VAR_INT_TO_VAR_INT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:339 |
| 0x0085 | `COMMAND_SET_LVAR_INT_TO_LVAR_INT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:343 |
| 0x0086 | `COMMAND_SET_VAR_FLOAT_TO_VAR_FLOAT` | `(AssignTo<float, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:349 |
| 0x0087 | `COMMAND_SET_LVAR_FLOAT_TO_LVAR_FLOAT` | `(AssignTo<float, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:350 |
| 0x0088 | `COMMAND_SET_VAR_FLOAT_TO_LVAR_FLOAT` | `(AssignTo<float, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:351 |
| 0x0089 | `COMMAND_SET_LVAR_FLOAT_TO_VAR_FLOAT` | `(AssignTo<float, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:348 |
| 0x008A | `COMMAND_SET_VAR_INT_TO_LVAR_INT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:338 |
| 0x008B | `COMMAND_SET_LVAR_INT_TO_VAR_INT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:342 |
| 0x008C | `COMMAND_CSET_VAR_INT_TO_VAR_FLOAT` | `(AssignTo<int32, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:354 |
| 0x008D | `COMMAND_CSET_VAR_FLOAT_TO_VAR_INT` | `(AssignTo<float, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:356 |
| 0x008E | `COMMAND_CSET_LVAR_INT_TO_VAR_FLOAT` | `(AssignTo<int32, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:355 |
| 0x008F | `COMMAND_CSET_LVAR_FLOAT_TO_VAR_INT` | `(AssignTo<float, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:358 |
| 0x0090 | `COMMAND_CSET_VAR_INT_TO_LVAR_FLOAT` | `(AssignTo<int32, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:352 |
| 0x0091 | `COMMAND_CSET_VAR_FLOAT_TO_LVAR_INT` | `(AssignTo<float, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:359 |
| 0x0092 | `COMMAND_CSET_LVAR_INT_TO_LVAR_FLOAT` | `(AssignTo<int32, float>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:353 |
| 0x0093 | `COMMAND_CSET_LVAR_FLOAT_TO_LVAR_INT` | `(AssignTo<float, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:357 |
| 0x0094 | `COMMAND_ABS_VAR_INT` | `AbsStore<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:374 |
| 0x0095 | `COMMAND_ABS_LVAR_INT` | `AbsStore<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:375 |
| 0x0096 | `COMMAND_ABS_VAR_FLOAT` | `AbsStore<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:376 |
| 0x0097 | `COMMAND_ABS_LVAR_FLOAT` | `AbsStore<float>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:377 |
| 0x0099 | `COMMAND_GENERATE_RANDOM_INT` | `GenerateRandomInt` | bajo | source/game_sa/Scripts/Commands/MathCommands.cpp:45 |
| 0x00A0 | `COMMAND_GET_CHAR_COORDINATES` | `GetCharCoordinates` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4095 |
| 0x00A1 | `COMMAND_SET_CHAR_COORDINATES` | `SetCharCoordinates` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4096 |
| 0x00A3 | `COMMAND_IS_CHAR_IN_AREA_2D` | `IsCharInArea2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4097 |
| 0x00A4 | `COMMAND_IS_CHAR_IN_AREA_3D` | `IsCharInArea3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4098 |
| 0x00AE | `COMMAND_SET_CAR_DRIVING_STYLE` | `SetCarDrivingStyle` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:148 |
| 0x00BA | `COMMAND_PRINT_BIG` | `PrintBig` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:316 |
| 0x00BB | `COMMAND_PRINT` | `Print` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:317 |
| 0x00BC | `COMMAND_PRINT_NOW` | `PrintNow` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:318 |
| 0x00BE | `COMMAND_CLEAR_PRINTS` | `ClearPrints` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:320 |
| 0x00BF | `COMMAND_GET_TIME_OF_DAY` | `GetTimeOfDay` | alto | source/game_sa/Scripts/Commands/ClockCommands.cpp:39 |
| 0x00C0 | `COMMAND_SET_TIME_OF_DAY` | `SetTimeOfDay` | medio | source/game_sa/Scripts/Commands/ClockCommands.cpp:40 |
| 0x00C1 | `COMMAND_GET_MINUTES_TO_TIME_OF_DAY` | `GetMinutesToTimeOfDay` | alto | source/game_sa/Scripts/Commands/ClockCommands.cpp:41 |
| 0x00C2 | `COMMAND_IS_POINT_ON_SCREEN` | `IsPointOnScreen` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:311 |
| 0x00C3 | `COMMAND_DEBUG_ON` | `DebugOn` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:390 |
| 0x00C4 | `COMMAND_DEBUG_OFF` | `DebugOff` | medio | source/game_sa/Scripts/Commands/BasicCommands.cpp:391 |
| 0x00C5 | `COMMAND_RETURN_TRUE` | `ReturnTrue` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:382 |
| 0x00C6 | `COMMAND_RETURN_FALSE` | `ReturnFalse` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:383 |
| 0x00D6 | `COMMAND_ANDOR` | `AndOr` | alto | source/game_sa/Scripts/Commands/BasicCommands.cpp:386 |
| 0x00D7 | `COMMAND_LAUNCH_MISSION` | `LaunchMission` | medio | source/game_sa/Scripts/Commands/MissionCommands.cpp:84 |
| 0x00D9 | `COMMAND_STORE_CAR_CHAR_IS_IN` | `StoreCarCharIsIn` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4099 |
| 0x00DB | `COMMAND_IS_CHAR_IN_CAR` | `IsCharInCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4100 |
| 0x00DD | `COMMAND_IS_CHAR_IN_MODEL` | `IsCharInModel` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4101 |
| 0x00DF | `COMMAND_IS_CHAR_IN_ANY_CAR` | `IsCharInAnyCar` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4102 |
| 0x00E1 | `COMMAND_IS_BUTTON_PRESSED` | `IsButtonPressed` | alto | source/game_sa/Scripts/Commands/PadCommands.cpp:39 |
| 0x00EC | `COMMAND_LOCATE_CHAR_ANY_MEANS_2D` | `LocateCharAnyMeans2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4103 |
| 0x00ED | `COMMAND_LOCATE_CHAR_ON_FOOT_2D` | `LocateCharOnFoot2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4104 |
| 0x00EE | `COMMAND_LOCATE_CHAR_IN_CAR_2D` | `LocateCharInCar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4105 |
| 0x00EF | `COMMAND_LOCATE_STOPPED_CHAR_ANY_MEANS_2D` | `LocateStoppedCharAnyMeans2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4106 |
| 0x00F0 | `COMMAND_LOCATE_STOPPED_CHAR_ON_FOOT_2D` | `LocateStoppedCharOnFoot2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4107 |
| 0x00F1 | `COMMAND_LOCATE_STOPPED_CHAR_IN_CAR_2D` | `LocateStoppedCharInCar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4108 |
| 0x00F2 | `COMMAND_LOCATE_CHAR_ANY_MEANS_CHAR_2D` | `LocateCharAnyMeansChar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4115 |
| 0x00F3 | `COMMAND_LOCATE_CHAR_ON_FOOT_CHAR_2D` | `LocateCharOnFootChar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4116 |
| 0x00F4 | `COMMAND_LOCATE_CHAR_IN_CAR_CHAR_2D` | `LocateCharInCarChar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4117 |
| 0x00FE | `COMMAND_LOCATE_CHAR_ANY_MEANS_3D` | `LocateCharAnyMeans3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4109 |
| 0x00FF | `COMMAND_LOCATE_CHAR_ON_FOOT_3D` | `LocateCharOnFoot3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4110 |
| 0x0100 | `COMMAND_LOCATE_CHAR_IN_CAR_3D` | `LocateCharInCar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4111 |
| 0x0101 | `COMMAND_LOCATE_STOPPED_CHAR_ANY_MEANS_3D` | `LocateStoppedCharAnyMeans3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4112 |
| 0x0102 | `COMMAND_LOCATE_STOPPED_CHAR_ON_FOOT_3D` | `LocateStoppedCharOnFoot3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4113 |
| 0x0103 | `COMMAND_LOCATE_STOPPED_CHAR_IN_CAR_3D` | `LocateStoppedCharInCar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4114 |
| 0x0104 | `COMMAND_LOCATE_CHAR_ANY_MEANS_CHAR_3D` | `LocateCharAnyMeansChar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4118 |
| 0x0105 | `COMMAND_LOCATE_CHAR_ON_FOOT_CHAR_3D` | `LocateCharOnFootChar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4119 |
| 0x0106 | `COMMAND_LOCATE_CHAR_IN_CAR_CHAR_3D` | `LocateCharInCarChar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4120 |
| 0x0107 | `COMMAND_CREATE_OBJECT` | `CreateObject` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:195 |
| 0x0108 | `COMMAND_DELETE_OBJECT` | `RemoveObject` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:196 |
| 0x0109 | `COMMAND_ADD_SCORE` | `AddScore` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:465 |
| 0x010A | `COMMAND_IS_SCORE_GREATER` | `IsScoreGreater` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:466 |
| 0x010B | `COMMAND_STORE_SCORE` | `StoreScore` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:467 |
| 0x010D | `COMMAND_ALTER_WANTED_LEVEL` | `AlterPlayerWantedLevel` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:468 |
| 0x010E | `COMMAND_ALTER_WANTED_LEVEL_NO_DROP` | `AlterPlayerWantedLevelNoDrop` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:469 |
| 0x010F | `COMMAND_IS_WANTED_LEVEL_GREATER` | `IsWantedLevelGreater` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:470 |
| 0x0110 | `COMMAND_CLEAR_WANTED_LEVEL` | `ClearWantedLevel` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:471 |
| 0x0117 | `COMMAND_IS_PLAYER_DEAD` | `IsPlayerDead` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:472 |
| 0x0118 | `COMMAND_IS_CHAR_DEAD` | `IsCharDead` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4127 |
| 0x0122 | `COMMAND_IS_PLAYER_PRESSING_HORN` | `IsPlayerPressingHorn` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:473 |
| 0x0129 | `COMMAND_CREATE_CHAR_INSIDE_CAR` | `CreateCharInsideCar` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4128 |
| 0x014C | `COMMAND_SWITCH_CAR_GENERATOR` | `SwitchCarGenerator` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:145 |
| 0x0154 | `COMMAND_IS_CHAR_IN_ZONE` | `IsCharInZone` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4129 |
| 0x0158 | `COMMAND_POINT_CAMERA_AT_CAR` | `PointCameraAtCar` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:317 |
| 0x0159 | `COMMAND_POINT_CAMERA_AT_CHAR` | `PointCameraAtChar` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:318 |
| 0x015A | `COMMAND_RESTORE_CAMERA` | `RestoreCamera` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:319 |
| 0x015B | `COMMAND_SHAKE_PAD` | `ShakePad` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:217 |
| 0x015F | `COMMAND_SET_FIXED_CAMERA_POSITION` | `SetFixedCameraPosition` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:320 |
| 0x0160 | `COMMAND_POINT_CAMERA_AT_POINT` | `PointCameraAtPoint` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:321 |
| 0x016A | `COMMAND_DO_FADE` | `DoCamFade` | alto | source/game_sa/Scripts/Commands/CameraCommands.cpp:315 |
| 0x016F | `COMMAND_DRAW_SHADOW` | `DrawShadow` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:218 |
| 0x0172 | `COMMAND_GET_CHAR_HEADING` | `GetCharHeading` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4130 |
| 0x0173 | `COMMAND_SET_CHAR_HEADING` | `SetCharHeading` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4131 |
| 0x0176 | `COMMAND_GET_OBJECT_HEADING` | `GetObjectHeading` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:204 |
| 0x0177 | `COMMAND_SET_OBJECT_HEADING` | `SetObjectHeading` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:205 |
| 0x0178 | `COMMAND_IS_PLAYER_TOUCHING_OBJECT` | `IsPlayerTouchingObject` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:584 |
| 0x0179 | `COMMAND_IS_CHAR_TOUCHING_OBJECT` | `IsCharTouchingObject` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4132 |
| 0x017B | `COMMAND_SET_CHAR_AMMO` | `SetCharAmmo` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4133 |
| 0x0184 | `COMMAND_IS_CHAR_HEALTH_GREATER` | `IsCharHealthGreater` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4134 |
| 0x01B4 | `COMMAND_SET_PLAYER_CONTROL` | `SetPlayerControl` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:474 |
| 0x01B9 | `COMMAND_SET_CURRENT_CHAR_WEAPON` | `SetCurrentCharWeapon` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4233 |
| 0x01BB | `COMMAND_GET_OBJECT_COORDINATES` | `GetObjectCoordinates` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:200 |
| 0x01BC | `COMMAND_SET_OBJECT_COORDINATES` | `SetObjectCoordinates` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:201 |
| 0x01C2 | `COMMAND_MARK_CHAR_AS_NO_LONGER_NEEDED` | `MarkCharAsNoLongerNeeded` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4234 |
| 0x01C4 | `COMMAND_MARK_OBJECT_AS_NO_LONGER_NEEDED` | `MarkObjectNoLongerNeeded` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:198 |
| 0x01C7 | `COMMAND_DONT_REMOVE_OBJECT` | `DoNotRemoveObject` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:199 |
| 0x01E3 | `COMMAND_PRINT_WITH_NUMBER_BIG` | `(PrintWithNumbersBig<int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:338 |
| 0x01E4 | `COMMAND_PRINT_WITH_NUMBER` | `(PrintWithNumbers<int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:332 |
| 0x01E5 | `COMMAND_PRINT_WITH_NUMBER_NOW` | `(PrintWithNumbersNow<int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:326 |
| 0x01E7 | `COMMAND_SWITCH_ROADS_ON` | `SwitchRoadsOn` | medio | source/game_sa/Scripts/Commands/PathCommands.cpp:85 |
| 0x01E8 | `COMMAND_SWITCH_ROADS_OFF` | `SwitchRoadsOff` | medio | source/game_sa/Scripts/Commands/PathCommands.cpp:86 |
| 0x01F0 | `COMMAND_SET_MAX_WANTED_LEVEL` | `SetMaxWantedLevel` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:475 |
| 0x01F3 | `COMMAND_IS_CAR_IN_AIR_PROPER` | `IsCarInAirProper` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:156 |
| 0x01F5 | `COMMAND_GET_PLAYER_CHAR` | `GetPlayerChar` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:476 |
| 0x01F7 | `COMMAND_SET_POLICE_IGNORE_PLAYER` | `SetPoliceIgnorePlayer` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:477 |
| 0x0202 | `COMMAND_LOCATE_CHAR_ANY_MEANS_CAR_2D` | `LocateCharAnyMeansCar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4121 |
| 0x0203 | `COMMAND_LOCATE_CHAR_ON_FOOT_CAR_2D` | `LocateCharOnFootCar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4122 |
| 0x0204 | `COMMAND_LOCATE_CHAR_IN_CAR_CAR_2D` | `LocateCharInCarCar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4123 |
| 0x0205 | `COMMAND_LOCATE_CHAR_ANY_MEANS_CAR_3D` | `LocateCharAnyMeansCar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4124 |
| 0x0206 | `COMMAND_LOCATE_CHAR_ON_FOOT_CAR_3D` | `LocateCharOnFootCar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4125 |
| 0x0207 | `COMMAND_LOCATE_CHAR_IN_CAR_CAR_3D` | `LocateCharInCarCar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4126 |
| 0x0208 | `COMMAND_GENERATE_RANDOM_FLOAT_IN_RANGE` | `GenerateRandomFloatInRange` | medio | source/game_sa/Scripts/Commands/MathCommands.cpp:47 |
| 0x0209 | `COMMAND_GENERATE_RANDOM_INT_IN_RANGE` | `GenerateRandomIntInRange` | medio | source/game_sa/Scripts/Commands/MathCommands.cpp:46 |
| 0x0217 | `COMMAND_PRINT_BIG_Q` | `PrintBigQ` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:344 |
| 0x0221 | `COMMAND_APPLY_BRAKES_TO_PLAYERS_CAR` | `ApplyBrakesToPlayersCar` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:478 |
| 0x0223 | `COMMAND_SET_CHAR_HEALTH` | `SetCharHealth` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4135 |
| 0x0226 | `COMMAND_GET_CHAR_HEALTH` | `GetCharHealth` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4136 |
| 0x022A | `COMMAND_SWITCH_PED_ROADS_ON` | `SwitchPedRoadsOn` | medio | source/game_sa/Scripts/Commands/PathCommands.cpp:88 |
| 0x022B | `COMMAND_SWITCH_PED_ROADS_OFF` | `SwitchPedRoadsOff` | medio | source/game_sa/Scripts/Commands/PathCommands.cpp:89 |
| 0x023A | `COMMAND_IS_PLAYER_TOUCHING_OBJECT_ON_FOOT` | `IsPlayerTouchingObjectOnFoot` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:611 |
| 0x023B | `COMMAND_IS_CHAR_TOUCHING_OBJECT_ON_FOOT` | `IsCharTouchingObjectOnFoot` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4137 |
| 0x0241 | `COMMAND_IS_PLAYER_IN_REMOTE_MODE` | `IsPlayerInRemoteMode` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:479 |
| 0x0247 | `COMMAND_REQUEST_MODEL` | `LoadModel` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:210 |
| 0x0248 | `COMMAND_HAS_MODEL_LOADED` | `HasModelLoaded` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:211 |
| 0x0249 | `COMMAND_MARK_MODEL_AS_NO_LONGER_NEEDED` | `MarkModelNotNeeded` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:212 |
| 0x0250 | `COMMAND_DRAW_LIGHT` | `DrawLight` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:217 |
| 0x0253 | `COMMAND_STORE_CLOCK` | `StoreClock` | medio | source/game_sa/Scripts/Commands/ClockCommands.cpp:42 |
| 0x0254 | `COMMAND_RESTORE_CLOCK` | `RestoreClock` | medio | source/game_sa/Scripts/Commands/ClockCommands.cpp:43 |
| 0x0256 | `COMMAND_IS_PLAYER_PLAYING` | `IsPlayerPlaying` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:480 |
| 0x0297 | `COMMAND_RESET_NUM_OF_MODELS_KILLED_BY_PLAYER` | `ResetNumOfModelsKilledByPlayer` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:481 |
| 0x0298 | `COMMAND_GET_NUM_OF_MODELS_KILLED_BY_PLAYER` | `GetNumOfModelsKilledByPlayer` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:482 |
| 0x02A0 | `COMMAND_IS_CHAR_STOPPED` | `IsCharStopped` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4138 |
| 0x02A9 | `COMMAND_SET_CHAR_ONLY_DAMAGED_BY_PLAYER` | `SetCharOnlyDamagedByPlayer` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4139 |
| 0x02AB | `COMMAND_SET_CHAR_PROOFS` | `SetCharProofs` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4085 |
| 0x02AC | `COMMAND_SET_CAR_PROOFS` | `SetCarProofs` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:144 |
| 0x02C0 | `COMMAND_GET_CLOSEST_CHAR_NODE` | `GetClosestCharNode` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4140 |
| 0x02CB | `COMMAND_IS_CHAR_ON_SCREEN` | `IsCharOnScreen` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4141 |
| 0x02CE | `COMMAND_GET_GROUND_Z_FOR_3D_COORD` | `GetGroundZFor3DCoord` | alto | source/game_sa/Scripts/Commands/GameCommands.cpp:197 |
| 0x02CF | `COMMAND_START_SCRIPT_FIRE` | `StartScriptFire` | medio | source/game_sa/Scripts/Commands/MissionCommands.cpp:83 |
| 0x02D6 | `COMMAND_IS_CHAR_SHOOTING_IN_AREA` | `IsCharShootingInArea` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4142 |
| 0x02D8 | `COMMAND_IS_CURRENT_CHAR_WEAPON` | `IsCurrentCharWeapon` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4143 |
| 0x02DD | `COMMAND_GET_RANDOM_CHAR_IN_ZONE` | `GetRandomCharInZone` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4144 |
| 0x02E0 | `COMMAND_IS_CHAR_SHOOTING` | `IsCharShooting` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4145 |
| 0x02E2 | `COMMAND_SET_CHAR_ACCURACY` | `SetCharAccuracy` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4146 |
| 0x02E3 | `COMMAND_GET_CAR_SPEED` | `GetCarSpeed` | alto | source/game_sa/Scripts/Commands/VehicleCommands.cpp:147 |
| 0x02EB | `COMMAND_RESTORE_CAMERA_JUMPCUT` | `RestoreCameraJumpcut` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:322 |
| 0x02F2 | `COMMAND_IS_CHAR_MODEL` | `IsCharModel` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4147 |
| 0x02F6 | `COMMAND_SIN` | `Sin` | medio | source/game_sa/Scripts/Commands/MathCommands.cpp:48 |
| 0x02F7 | `COMMAND_COS` | `Cos` | medio | source/game_sa/Scripts/Commands/MathCommands.cpp:49 |
| 0x02FC | `COMMAND_PRINT_WITH_2_NUMBERS` | `(PrintWithNumbers<int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:333 |
| 0x02FD | `COMMAND_PRINT_WITH_2_NUMBERS_NOW` | `(PrintWithNumbersNow<int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:327 |
| 0x02FF | `COMMAND_PRINT_WITH_3_NUMBERS` | `(PrintWithNumbers<int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:334 |
| 0x0300 | `COMMAND_PRINT_WITH_3_NUMBERS_NOW` | `(PrintWithNumbersNow<int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:328 |
| 0x0302 | `COMMAND_PRINT_WITH_4_NUMBERS` | `(PrintWithNumbers<int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:335 |
| 0x0303 | `COMMAND_PRINT_WITH_4_NUMBERS_NOW` | `(PrintWithNumbersNow<int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:329 |
| 0x0305 | `COMMAND_PRINT_WITH_5_NUMBERS` | `(PrintWithNumbers<int32, int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:336 |
| 0x0306 | `COMMAND_PRINT_WITH_5_NUMBERS_NOW` | `(PrintWithNumbersNow<int32, int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:330 |
| 0x0308 | `COMMAND_PRINT_WITH_6_NUMBERS` | `(PrintWithNumbers<int32, int32, int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:337 |
| 0x0309 | `COMMAND_PRINT_WITH_6_NUMBERS_NOW` | `(PrintWithNumbersNow<int32, int32, int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:331 |
| 0x030C | `COMMAND_PLAYER_MADE_PROGRESS` | `PlayerMadeProgress` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:198 |
| 0x030E | `COMMAND_REGISTER_JUMP_DISTANCE` | `RegisterJumpDistance` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:239 |
| 0x030F | `COMMAND_REGISTER_JUMP_HEIGHT` | `RegisterJumpHeight` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:240 |
| 0x0311 | `COMMAND_REGISTER_JUMP_SPINS` | `RegisterJumpSpins` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:241 |
| 0x0312 | `COMMAND_REGISTER_JUMP_STUNT` | `RegisterJumpStunt` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:242 |
| 0x0313 | `COMMAND_REGISTER_UNIQUE_JUMP_FOUND` | `RegisterUniqueJumpFound` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:243 |
| 0x0314 | `COMMAND_SET_UNIQUE_JUMPS_TOTAL` | `SetUniqueJumpsTotal` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:244 |
| 0x0315 | `COMMAND_REGISTER_PASSENGER_DROPPED_OFF_TAXI` | `RegisterPassengerDroppedOffTaxi` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:245 |
| 0x0316 | `COMMAND_REGISTER_MONEY_MADE_TAXI` | `RegisterMoneyMadeTaxi` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:246 |
| 0x0317 | `COMMAND_REGISTER_MISSION_GIVEN` | `RegisterMissionGiven` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:199 |
| 0x031A | `COMMAND_REMOVE_ALL_SCRIPT_FIRES` | `RemoveAllScriptFires` | bajo | source/game_sa/Scripts/Commands/ScriptCommands.cpp:90 |
| 0x031B | `COMMAND_IS_FIRST_CAR_COLOUR` | `IsFirstCarColor` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:149 |
| 0x031C | `COMMAND_IS_SECOND_CAR_COLOUR` | `IsSecondCarColor` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:150 |
| 0x031D | `COMMAND_HAS_CHAR_BEEN_DAMAGED_BY_WEAPON` | `HasCharBeenDamagedByWeapon` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4148 |
| 0x0321 | `COMMAND_EXPLODE_CHAR_HEAD` | `ExplodeCharHead` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4149 |
| 0x0326 | `COMMAND_START_CHAR_FIRE` | `StartCharFire` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4150 |
| 0x032A | `COMMAND_SET_CAMERA_ZOOM` | `SetCameraZoom` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:323 |
| 0x0330 | `COMMAND_SET_PLAYER_NEVER_GETS_TIRED` | `SetPlayerNeverGetsTired` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:483 |
| 0x0331 | `COMMAND_SET_PLAYER_FAST_RELOAD` | `SetPlayerFastReload` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:484 |
| 0x0332 | `COMMAND_SET_CHAR_BLEEDING` | `SetCharBleeding` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4151 |
| 0x0337 | `COMMAND_SET_CHAR_VISIBLE` | `SetCharVisible` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4152 |
| 0x033E | `COMMAND_DISPLAY_TEXT` | `DisplayText` | bajo | source/game_sa/Scripts/Commands/TextCommands.cpp:322 |
| 0x033F | `COMMAND_SET_TEXT_SCALE` | `SetTextScale` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:349 |
| 0x0340 | `COMMAND_SET_TEXT_COLOUR` | `SetTextColour` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:350 |
| 0x0341 | `COMMAND_SET_TEXT_JUSTIFY` | `SetTextJustify` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:351 |
| 0x0342 | `COMMAND_SET_TEXT_CENTRE` | `SetTextCentre` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:352 |
| 0x0343 | `COMMAND_SET_TEXT_WRAPX` | `SetTextWrapX` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:353 |
| 0x0344 | `COMMAND_SET_TEXT_CENTRE_SIZE` | `SetTextCentreSize` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:354 |
| 0x0345 | `COMMAND_SET_TEXT_BACKGROUND` | `SetTextBackground` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:355 |
| 0x0348 | `COMMAND_SET_TEXT_PROPORTIONAL` | `SetTextProportional` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:356 |
| 0x0349 | `COMMAND_SET_TEXT_FONT` | `SetTextFont` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:357 |
| 0x0350 | `COMMAND_SET_CHAR_STAY_IN_SAME_PLACE` | `SetCharStayInSamePlace` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4154 |
| 0x0356 | `COMMAND_IS_EXPLOSION_IN_AREA` | `IsExplosionInArea` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:216 |
| 0x0362 | `COMMAND_WARP_CHAR_FROM_CAR_TO_COORD` | `WarpCharFromCarToCoord` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4155 |
| 0x0364 | `COMMAND_HAS_CHAR_SPOTTED_CHAR` | `HasCharSpottedChar` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4156 |
| 0x036A | `COMMAND_WARP_CHAR_INTO_CAR` | `WarpCharIntoCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4157 |
| 0x036D | `COMMAND_PRINT_WITH_2_NUMBERS_BIG` | `(PrintWithNumbersBig<int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:339 |
| 0x036E | `COMMAND_PRINT_WITH_3_NUMBERS_BIG` | `(PrintWithNumbersBig<int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:340 |
| 0x036F | `COMMAND_PRINT_WITH_4_NUMBERS_BIG` | `(PrintWithNumbersBig<int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:341 |
| 0x0370 | `COMMAND_PRINT_WITH_5_NUMBERS_BIG` | `(PrintWithNumbersBig<int32, int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:342 |
| 0x0371 | `COMMAND_PRINT_WITH_6_NUMBERS_BIG` | `(PrintWithNumbersBig<int32, int32, int32, int32, int32, int32>)` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:343 |
| 0x0373 | `COMMAND_SET_CAMERA_BEHIND_PLAYER` | `SetCameraBehindPlayer` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:324 |
| 0x0381 | `COMMAND_SET_OBJECT_VELOCITY` | `SetObjectVelocity` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:203 |
| 0x0382 | `COMMAND_SET_OBJECT_COLLISION` | `SetObjectCollision` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:207 |
| 0x038B | `COMMAND_LOAD_ALL_MODELS_NOW` | `LoadAllModelsNow` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:213 |
| 0x038D | `COMMAND_DRAW_SPRITE` | `DrawSprite` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:117 |
| 0x038E | `COMMAND_DRAW_RECT` | `DrawRect` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:122 |
| 0x038F | `COMMAND_LOAD_SPRITE` | `LoadSprite` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:119 |
| 0x0390 | `COMMAND_LOAD_TEXTURE_DICTIONARY` | `LoadTextureDictionary` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:120 |
| 0x0391 | `COMMAND_REMOVE_TEXTURE_DICTIONARY` | `RemoveTextureDictionary` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:121 |
| 0x0393 | `COMMAND_SET_CHAR_ANIM_SPEED` | `SetCharAnimSpeed` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4158 |
| 0x039E | `COMMAND_SET_CHAR_CANT_BE_DRAGGED_OUT` | `SetCharCantBeDraggedOut` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4159 |
| 0x03A3 | `COMMAND_IS_CHAR_MALE` | `IsCharMale` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4160 |
| 0x03A4 | `COMMAND_SCRIPT_NAME` | `ScriptName` | alto | source/game_sa/Scripts/Commands/MissionCommands.cpp:82 |
| 0x03BF | `COMMAND_SET_EVERYONE_IGNORE_PLAYER` | `SetEveryoneIgnorePlayer` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:485 |
| 0x03C0 | `COMMAND_STORE_CAR_CHAR_IS_IN_NO_SAVE` | `StoreCarCharIsInNoSave` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4161 |
| 0x03C8 | `COMMAND_SET_CAMERA_IN_FRONT_OF_PLAYER` | `SetCameraInFrontOfPlayer` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:325 |
| 0x03CA | `COMMAND_DOES_OBJECT_EXIST` | `DoesObjectExists` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:197 |
| 0x03CB | `COMMAND_LOAD_SCENE` | `LoadScene` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:91 |
| 0x03CC | `COMMAND_ADD_STUCK_CAR_CHECK` | `AddStuckCarCheck` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:158 |
| 0x03CD | `COMMAND_REMOVE_STUCK_CAR_CHECK` | `RemoveStuckCarCheck` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:159 |
| 0x03CE | `COMMAND_IS_CAR_STUCK` | `IsCarStuck` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:157 |
| 0x03CF | `COMMAND_LOAD_MISSION_AUDIO` | `LoadMissionAudio` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:93 |
| 0x03D1 | `COMMAND_PLAY_MISSION_AUDIO` | `PlayMissionAudio` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:97 |
| 0x03D5 | `COMMAND_CLEAR_THIS_PRINT` | `ClearThisPrint` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:347 |
| 0x03D6 | `COMMAND_CLEAR_THIS_BIG_PRINT` | `ClearThisBigPrint` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:348 |
| 0x03DE | `COMMAND_SET_PED_DENSITY_MULTIPLIER` | `SetPedDensityMultiplier` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:200 |
| 0x03E0 | `COMMAND_SET_TEXT_DRAW_BEFORE_FADE` | `SetTextDrawBeforeFade` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:358 |
| 0x03E3 | `COMMAND_SET_SPRITES_DRAW_BEFORE_FADE` | `SetSpritesDrawBeforeFade` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:123 |
| 0x03E4 | `COMMAND_SET_TEXT_RIGHT_JUSTIFY` | `SetTextRightJustify` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:359 |
| 0x03E5 | `COMMAND_PRINT_HELP` | `PrintHelp` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:313 |
| 0x03E6 | `COMMAND_CLEAR_HELP` | `ClearHelp` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:314 |
| 0x03E7 | `COMMAND_FLASH_HUD_OBJECT` | `FlashHudObject` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:315 |
| 0x03EB | `COMMAND_CLEAR_SMALL_PRINTS` | `ClearSmallPrints` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:312 |
| 0x03EE | `COMMAND_CAN_PLAYER_START_MISSION` | `CanPlayerStartMission` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:486 |
| 0x03EF | `COMMAND_MAKE_PLAYER_SAFE_FOR_CUTSCENE` | `MakePlayerSafeForCutscene` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:487 |
| 0x03F0 | `COMMAND_USE_TEXT_COMMANDS` | `UseTextCommands` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:321 |
| 0x03FE | `COMMAND_SET_CHAR_MONEY` | `SetCharMoney` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4162 |
| 0x0417 | `COMMAND_LOAD_AND_LAUNCH_MISSION_INTERNAL` | `LoadAndLaunchMissionInternal` | medio | source/game_sa/Scripts/Commands/MissionCommands.cpp:81 |
| 0x041A | `COMMAND_GET_AMMO_IN_CHAR_WEAPON` | `GetAmmoInCharWeapon` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4163 |
| 0x0430 | `COMMAND_WARP_CHAR_INTO_CAR_AS_PASSENGER` | `WarpCharIntoCarAsPassenger` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4164 |
| 0x0432 | `COMMAND_GET_CHAR_IN_CAR_PASSENGER_SEAT` | `GetCharInCarPassengerSeat` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4165 |
| 0x0433 | `COMMAND_SET_CHAR_IS_CHRIS_CRIMINAL` | `SetCharIsChrisCriminal` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4166 |
| 0x0445 | `COMMAND_ARE_ANY_CAR_CHEATS_ACTIVATED` | `AreAnyCarCheatsActivated` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:195 |
| 0x0446 | `COMMAND_SET_CHAR_SUFFERS_CRITICAL_HITS` | `SetCharSuffersCriticalHits` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4167 |
| 0x0448 | `COMMAND_IS_CHAR_SITTING_IN_CAR` | `IsCharSittingInCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4168 |
| 0x0449 | `COMMAND_IS_CHAR_SITTING_IN_ANY_CAR` | `IsCharSittingInAnyCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4169 |
| 0x044B | `COMMAND_IS_CHAR_ON_FOOT` | `IsCharOnFoot` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4170 |
| 0x0454 | `COMMAND_GET_DEBUG_CAMERA_COORDINATES` | `GetDebugCameraCoordinates` | alto | source/game_sa/Scripts/Commands/CameraCommands.cpp:327 |
| 0x0457 | `COMMAND_IS_PLAYER_TARGETTING_CHAR` | `IsPlayerTargettingChar` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:488 |
| 0x0458 | `COMMAND_IS_PLAYER_TARGETTING_OBJECT` | `IsPlayerTargettingObject` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:489 |
| 0x0459 | `COMMAND_TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME` | `TerminateAllScriptsWithThisName` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:89 |
| 0x045A | `COMMAND_DISPLAY_TEXT_WITH_NUMBER` | `(DisplayTextWithNumbers<int32>)` | bajo | source/game_sa/Scripts/Commands/TextCommands.cpp:324 |
| 0x045B | `COMMAND_DISPLAY_TEXT_WITH_2_NUMBERS` | `(DisplayTextWithNumbers<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/TextCommands.cpp:325 |
| 0x045C | `COMMAND_FAIL_CURRENT_MISSION` | `FailCurrentMission` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:196 |
| 0x0463 | `COMMAND_GET_DEBUG_CAMERA_POINT_AT` | `GetDebugCameraPointAt` | alto | source/game_sa/Scripts/Commands/CameraCommands.cpp:329 |
| 0x0464 | `COMMAND_ATTACH_CHAR_TO_CAR` | `AttachCharToCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4171 |
| 0x0465 | `COMMAND_DETACH_CHAR_FROM_CAR` | `DetachCharFromCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4172 |
| 0x0467 | `COMMAND_CLEAR_CHAR_LAST_WEAPON_DAMAGE` | `ClearCharLastWeaponDamage` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4173 |
| 0x046E | `COMMAND_GIVE_REMOTE_CONTROLLED_MODEL_TO_PLAYER` | `GiveRemoteControlledModelToPlayer` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:490 |
| 0x0470 | `COMMAND_GET_CURRENT_CHAR_WEAPON` | `GetCurrentCharWeapon` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4174 |
| 0x0471 | `COMMAND_LOCATE_CHAR_ANY_MEANS_OBJECT_2D` | `LocateCharAnyMeansObject2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4175 |
| 0x0472 | `COMMAND_LOCATE_CHAR_ON_FOOT_OBJECT_2D` | `LocateCharOnFootObject2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4176 |
| 0x0474 | `COMMAND_LOCATE_CHAR_ANY_MEANS_OBJECT_3D` | `LocateCharAnyMeansObject3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4177 |
| 0x0475 | `COMMAND_LOCATE_CHAR_ON_FOOT_OBJECT_3D` | `LocateCharOnFootObject3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4178 |
| 0x047A | `COMMAND_IS_CHAR_ON_ANY_BIKE` | `IsCharOnAnyBike` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4179 |
| 0x0480 | `COMMAND_CAN_CHAR_SEE_DEAD_CHAR` | `CanCharSeeDeadChar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4180 |
| 0x0488 | `COMMAND_IS_MODEL_AVAILABLE` | `IsModelAvailable` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:214 |
| 0x0489 | `COMMAND_SHUT_CHAR_UP` | `ShutCharUp` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4181 |
| 0x048F | `COMMAND_REMOVE_ALL_CHAR_WEAPONS` | `RemoveAllCharWeapons` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4182 |
| 0x0491 | `COMMAND_HAS_CHAR_GOT_WEAPON` | `HasCharGotWeapon` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4183 |
| 0x04A5 | `COMMAND_GET_DEAD_CHAR_PICKUP_COORDS` | `GetDeadCharPickupCoords` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4184 |
| 0x04A7 | `COMMAND_IS_CHAR_IN_ANY_BOAT` | `IsCharInAnyBoat` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4185 |
| 0x04A9 | `COMMAND_IS_CHAR_IN_ANY_HELI` | `IsCharInAnyHeli` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4186 |
| 0x04AB | `COMMAND_IS_CHAR_IN_ANY_PLANE` | `IsCharInAnyPlane` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4187 |
| 0x04AD | `COMMAND_IS_CHAR_IN_WATER` | `IsCharInWater` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4188 |
| 0x04AE | `COMMAND_SET_VAR_INT_TO_CONSTANT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:341 |
| 0x04AF | `COMMAND_SET_LVAR_INT_TO_CONSTANT` | `(AssignTo<int32, int32>)` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:340 |
| 0x04B0 | `COMMAND_IS_INT_VAR_GREATER_THAN_CONSTANT` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:262 |
| 0x04B1 | `COMMAND_IS_INT_LVAR_GREATER_THAN_CONSTANT` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:263 |
| 0x04B2 | `COMMAND_IS_CONSTANT_GREATER_THAN_INT_VAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:255 |
| 0x04B3 | `COMMAND_IS_CONSTANT_GREATER_THAN_INT_LVAR` | `IsGreater<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:257 |
| 0x04B4 | `COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_CONSTANT` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:240 |
| 0x04B5 | `COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_CONSTANT` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:243 |
| 0x04B6 | `COMMAND_IS_CONSTANT_GREATER_OR_EQUAL_TO_INT_VAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:235 |
| 0x04B7 | `COMMAND_IS_CONSTANT_GREATER_OR_EQUAL_TO_INT_LVAR` | `IsGreaterEqual<int32>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:236 |
| 0x04B8 | `COMMAND_GET_CHAR_WEAPON_IN_SLOT` | `GetCharWeaponInSlot` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4189 |
| 0x04C4 | `COMMAND_GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS` | `GetOffsetFromCharInWorldCoords` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4190 |
| 0x04C5 | `COMMAND_HAS_CHAR_BEEN_PHOTOGRAPHED` | `HasCharBeenPhotographed` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4191 |
| 0x04C8 | `COMMAND_IS_CHAR_IN_FLYING_VEHICLE` | `IsCharInFlyingVehicle` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4192 |
| 0x04D1 | `COMMAND_CLEAR_HELI_ORIENTATION` | `ClearHeliOrientation` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:142 |
| 0x04D7 | `COMMAND_FREEZE_CHAR_POSITION` | `FreezeCharPosition` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4193 |
| 0x04D8 | `COMMAND_SET_CHAR_DROWNS_IN_WATER` | `SetCharDrownsInWater` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4194 |
| 0x04DB | `COMMAND_REMOVE_RC_BUGGY` | `RemoveRCBuggy` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:143 |
| 0x04DD | `COMMAND_GET_CHAR_ARMOUR` | `GetCharArmour` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4195 |
| 0x04DE | `COMMAND_SET_CHAR_ARMOUR` | `SetCharArmour` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4196 |
| 0x04E3 | `COMMAND_SET_PLAYER_MOOD` | `SetPlayerMood` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:491 |
| 0x04F0 | `COMMAND_IS_CHAR_WAITING_FOR_WORLD_COLLISION` | `IsCharWaitingForWorldCollision` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4197 |
| 0x04F4 | `COMMAND_ATTACH_CHAR_TO_OBJECT` | `AttachCharToObject` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4198 |
| 0x04F8 | `COMMAND_ADD_SET_PIECE` | `CSetPieces::AddOne` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:216 |
| 0x0500 | `COMMAND_IS_PLAYER_WEARING` | `IsPlayerWearing` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:492 |
| 0x0501 | `COMMAND_SET_PLAYER_CAN_DO_DRIVE_BY` | `SetPlayerCanDoDriveBy` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:493 |
| 0x0509 | `COMMAND_GET_DISTANCE_BETWEEN_COORDS_2D` | `GetDistanceBetweenCoords2d` | medio | source/game_sa/Scripts/Commands/MathCommands.cpp:43 |
| 0x050A | `COMMAND_GET_DISTANCE_BETWEEN_COORDS_3D` | `GetDistanceBetweenCoords3d` | medio | source/game_sa/Scripts/Commands/MathCommands.cpp:44 |
| 0x050B | `COMMAND_POP_CAR_BOOT_USING_PHYSICS` | `PopCarBootUsingPhysics` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:152 |
| 0x0512 | `COMMAND_PRINT_HELP_FOREVER` | `PrintHelpForever` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:345 |
| 0x0513 | `COMMAND_PRINT_HELP_FOREVER_WITH_NUMBER` | `PrintHelpForeverWithNumber` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:346 |
| 0x051A | `COMMAND_HAS_CHAR_BEEN_DAMAGED_BY_CHAR` | `HasCharBeenDamagedByChar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4199 |
| 0x051B | `COMMAND_HAS_CHAR_BEEN_DAMAGED_BY_CAR` | `HasCharBeenDamagedByCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4200 |
| 0x0526 | `COMMAND_SET_CHAR_STAY_IN_CAR_WHEN_JACKED` | `SetCharStayInCarWhenJacked` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4201 |
| 0x052C | `COMMAND_SET_PLAYER_DRUNKENNESS` | `SetPlayerDrunkenness` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:494 |
| 0x0547 | `COMMAND_IS_CHAR_TOUCHING_VEHICLE` | `IsCharTouchingVehicle` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4202 |
| 0x054A | `COMMAND_SET_CHAR_CAN_BE_SHOT_IN_VEHICLE` | `SetCharCanBeShotInVehicle` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4203 |
| 0x054C | `COMMAND_LOAD_MISSION_TEXT` | `LoadMissionText` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:362 |
| 0x054E | `COMMAND_CLEAR_CHAR_LAST_DAMAGE_ENTITY` | `ClearCharLastDamageEntity` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4204 |
| 0x055D | `COMMAND_MAKE_PLAYER_FIRE_PROOF` | `MakePlayerFireProof` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:495 |
| 0x055E | `COMMAND_INCREASE_PLAYER_MAX_HEALTH` | `IncreasePlayerMaxHealth` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:496 |
| 0x055F | `COMMAND_INCREASE_PLAYER_MAX_ARMOUR` | `IncreasePlayerMaxArmour` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:497 |
| 0x0560 | `COMMAND_CREATE_RANDOM_CHAR_AS_DRIVER` | `CreateRandomCharAsDriver` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4205 |
| 0x0561 | `COMMAND_CREATE_RANDOM_CHAR_AS_PASSENGER` | `CreateRandomCharAsPassenger` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4206 |
| 0x0563 | `COMMAND_ENSURE_PLAYER_HAS_DRIVE_BY_WEAPON` | `EnsurePlayerHasDriveByWeapon` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:498 |
| 0x0568 | `COMMAND_SET_CHAR_NEVER_TARGETTED` | `SetCharNeverTargetted` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4207 |
| 0x056C | `COMMAND_IS_CHAR_IN_ANY_POLICE_VEHICLE` | `IsCharInAnyPoliceVehicle` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4208 |
| 0x056D | `COMMAND_DOES_CHAR_EXIST` | `DoesCharExist` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4209 |
| 0x0572 | `COMMAND_SET_ALL_TAXIS_HAVE_NITRO` | `SetAllTaxisHaveNitro` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:193 |
| 0x0575 | `COMMAND_FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION` | `FreezeCharPositionAndDontLoadCollision` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4210 |
| 0x057E | `COMMAND_SET_PLAYER_IS_IN_STADIUM` | `SetPlayerInStadium` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:101 |
| 0x0581 | `COMMAND_DISPLAY_RADAR` | `DisplayRadar` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:214 |
| 0x0582 | `COMMAND_REGISTER_BEST_POSITION` | `RegisterBestPosition` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:215 |
| 0x0583 | `COMMAND_IS_PLAYER_IN_INFO_ZONE` | `IsPlayerInInfoZone` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:499 |
| 0x0588 | `COMMAND_SET_LOAD_COLLISION_FOR_CHAR_FLAG` | `SetLoadCollisionForCharFlag` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4211 |
| 0x058A | `COMMAND_ADD_BIG_GUN_FLASH` | `AddBigGunFlash` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:218 |
| 0x0597 | `COMMAND_IS_CHAR_DUCKING` | `IsCharDucking` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4212 |
| 0x059F | `COMMAND_GET_OBJECT_VELOCITY` | `GetObjectVelocity` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:202 |
| 0x05A0 | `COMMAND_IS_DEBUG_CAMERA_ON` | `IsDebugCameraOn` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:334 |
| 0x05A9 | `COMMAND_SET_VAR_TEXT_LABEL` | `SetTextLabel<8>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:410 |
| 0x05AA | `COMMAND_SET_LVAR_TEXT_LABEL` | `SetTextLabel<8>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:411 |
| 0x05AD | `COMMAND_IS_VAR_TEXT_LABEL_EQUAL_TO_TEXT_LABEL` | `AreTextLabelsEqual` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:402 |
| 0x05AE | `COMMAND_IS_LVAR_TEXT_LABEL_EQUAL_TO_TEXT_LABEL` | `AreTextLabelsEqual` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:403 |
| 0x05E2 | `COMMAND_TASK_KILL_CHAR_ON_FOOT` | `TaskKillCharOnFoot` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4213 |
| 0x05F6 | `COMMAND_IS_CHAR_IN_ANGLED_AREA_2D` | `IsCharInAngledArea2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4214 |
| 0x05F8 | `COMMAND_IS_CHAR_IN_ANGLED_AREA_IN_CAR_2D` | `IsCharInAngledAreaInCar2D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4215 |
| 0x05FC | `COMMAND_IS_CHAR_IN_ANGLED_AREA_3D` | `IsCharInAngledArea3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4216 |
| 0x05FD | `COMMAND_IS_CHAR_IN_ANGLED_AREA_ON_FOOT_3D` | `IsCharInAngledAreaOnFoot3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4217 |
| 0x05FE | `COMMAND_IS_CHAR_IN_ANGLED_AREA_IN_CAR_3D` | `IsCharInAngledAreaInCar3D` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4218 |
| 0x0602 | `COMMAND_IS_CHAR_IN_TAXI` | `IsCharInTaxi` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4219 |
| 0x060A | `COMMAND_LOAD_CHAR_DECISION_MAKER` | `LoadCharDecisionMaker` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4220 |
| 0x060B | `COMMAND_SET_CHAR_DECISION_MAKER` | `SetCharDecisionMaker` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4221 |
| 0x060D | `COMMAND_SET_TEXT_DROPSHADOW` | `SetTextDropshadow` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:360 |
| 0x0611 | `COMMAND_IS_CHAR_PLAYING_ANIM` | `IsCharPlayingAnim` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4222 |
| 0x0612 | `COMMAND_SET_CHAR_ANIM_PLAYING_FLAG` | `SetCharAnimPlayingFlag` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4223 |
| 0x0613 | `COMMAND_GET_CHAR_ANIM_CURRENT_TIME` | `GetCharAnimCurrentTime` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4224 |
| 0x0614 | `COMMAND_SET_CHAR_ANIM_CURRENT_TIME` | `SetCharAnimCurrentTime` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4225 |
| 0x0619 | `COMMAND_SET_CHAR_COLLISION` | `SetCharCollision` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4226 |
| 0x061A | `COMMAND_GET_CHAR_ANIM_TOTAL_TIME` | `GetCharAnimTotalTime` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4227 |
| 0x0623 | `COMMAND_INCREMENT_INT_STAT` | `IncrementStat<int32>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:51 |
| 0x0624 | `COMMAND_INCREMENT_FLOAT_STAT` | `IncrementStat<float>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:52 |
| 0x0625 | `COMMAND_DECREMENT_INT_STAT` | `DecrementStat<int32>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:54 |
| 0x0626 | `COMMAND_DECREMENT_FLOAT_STAT` | `DecrementStat<float>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:55 |
| 0x0627 | `COMMAND_REGISTER_INT_STAT` | `RegisterStat<int32>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:57 |
| 0x0628 | `COMMAND_REGISTER_FLOAT_STAT` | `RegisterStat<float>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:58 |
| 0x0629 | `COMMAND_SET_INT_STAT` | `SetStat<int32>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:60 |
| 0x062A | `COMMAND_SET_FLOAT_STAT` | `SetStat<float>` | medio | source/game_sa/Scripts/Commands/StatCommands.cpp:61 |
| 0x0634 | `COMMAND_TASK_KILL_CHAR_ON_FOOT_WHILE_DUCKING` | `TaskKillCharOnFootWhileDucking` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4229 |
| 0x0639 | `COMMAND_TASK_TURN_CHAR_TO_FACE_CHAR` | `TaskTurnCharToFaceChar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4230 |
| 0x0642 | `COMMAND_IS_CHAR_AT_SCRIPTED_ATTRACTOR` | `IsCharAtScriptedAttractor` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4231 |
| 0x0643 | `COMMAND_SET_SEQUENCE_TO_REPEAT` | `SetSequenceToRepeat` | medio | source/game_sa/Scripts/Commands/SequenceCommands.cpp:24 |
| 0x0652 | `COMMAND_GET_INT_STAT` | `GetStat<int32>` | alto | source/game_sa/Scripts/Commands/StatCommands.cpp:63 |
| 0x0653 | `COMMAND_GET_FLOAT_STAT` | `GetStat<float>` | alto | source/game_sa/Scripts/Commands/StatCommands.cpp:64 |
| 0x0665 | `COMMAND_GET_CHAR_MODEL` | `GetCharModel` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4232 |
| 0x0679 | `COMMAND_ATTACH_CAMERA_TO_VEHICLE` | `AttachCameraToVehicle` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:335 |
| 0x067A | `COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_VEHICLE` | `AttachCameraToVehicleLookAtVehicle` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:313 |
| 0x067B | `COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_CHAR` | `AttachCameraToVehicleLookAtChar` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:336 |
| 0x067C | `COMMAND_ATTACH_CAMERA_TO_CHAR` | `AttachCameraToChar` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:337 |
| 0x067E | `COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_CHAR` | `AttachCameraToCharLookAtChar` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:338 |
| 0x068C | `COMMAND_IS_PLAYER_TARGETTING_ANYTHING` | `IsPlayerTargettingAnything` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:500 |
| 0x068D | `COMMAND_GET_ACTIVE_CAMERA_COORDINATES` | `GetActiveCameraCoordinates` | alto | source/game_sa/Scripts/Commands/CameraCommands.cpp:339 |
| 0x068E | `COMMAND_GET_ACTIVE_CAMERA_POINT_AT` | `GetActiveCameraPointAt` | alto | source/game_sa/Scripts/Commands/CameraCommands.cpp:340 |
| 0x06A7 | `COMMAND_ATTACH_CHAR_TO_BIKE` | `AttachCharToBike` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4238 |
| 0x06AC | `COMMAND_GET_CHAR_SPEED` | `GetCharSpeed` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4235 |
| 0x06AF | `COMMAND_DISABLE_PLAYER_SPRINT` | `DisablePlayerSprint` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:501 |
| 0x06B7 | `COMMAND_IS_CHAR_IN_SEARCHLIGHT` | `IsCharInSearchlight` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4236 |
| 0x06C9 | `COMMAND_REMOVE_CHAR_FROM_GROUP` | `RemoveCharFromGroup` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4237 |
| 0x06D1 | `COMMAND_SET_VAR_TEXT_LABEL16` | `SetTextLabel<16>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:412 |
| 0x06D2 | `COMMAND_SET_LVAR_TEXT_LABEL16` | `SetTextLabel<16>` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:413 |
| 0x06DF | `COMMAND_DELETE_PLAYER` | `DeletePlayer` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:502 |
| 0x06E0 | `COMMAND_SET_TWO_PLAYER_CAMERA_MODE` | `SetTwoPlayerCameraMode` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:503 |
| 0x06F1 | `COMMAND_LIMIT_TWO_PLAYER_DISTANCE` | `LimitTwoPlayerDistance` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:504 |
| 0x06F2 | `COMMAND_RELEASE_TWO_PLAYER_DISTANCE` | `ReleaseTwoPlayerDistance` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:505 |
| 0x06F3 | `COMMAND_SET_PLAYER_PLAYER_TARGETTING` | `SetPlayerPlayerTargetting` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:506 |
| 0x06FA | `COMMAND_SET_PLAYERS_CAN_BE_IN_SEPARATE_CARS` | `SetPlayersCanBeInSeparateCars` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:507 |
| 0x070D | `COMMAND_BUILD_PLAYER_MODEL` | `BuildPlayerModel` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:508 |
| 0x0717 | `COMMAND_START_SETTING_UP_CONVERSATION` | `StartSettingUpConversation` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:80 |
| 0x0719 | `COMMAND_FINISH_SETTING_UP_CONVERSATION` | `FinishSettingUpConversation` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:81 |
| 0x071A | `COMMAND_IS_CONVERSATION_AT_NODE` | `IsConversationAtNode` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:77 |
| 0x072F | `COMMAND_ADD_STUCK_CAR_CHECK_WITH_WARP` | `AddStuckCarCheckWithWarp` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:160 |
| 0x074B | `COMMAND_DRAW_SPRITE_WITH_ROTATION` | `DrawSpriteWithRotation` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:118 |
| 0x0767 | `COMMAND_SET_ZONE_POPULATION_TYPE` | `SetZonePopulationType` | bajo | source/game_sa/Scripts/Commands/ZoneCommands.cpp:128 |
| 0x076A | `COMMAND_SET_ZONE_DEALER_STRENGTH` | `SetZoneDealerStrength` | bajo | source/game_sa/Scripts/Commands/ZoneCommands.cpp:131 |
| 0x076B | `COMMAND_GET_ZONE_DEALER_STRENGTH` | `GetZoneDealerStrength` | medio | source/game_sa/Scripts/Commands/ZoneCommands.cpp:132 |
| 0x076C | `COMMAND_SET_ZONE_GANG_STRENGTH` | `SetZoneGangStrength` | bajo | source/game_sa/Scripts/Commands/ZoneCommands.cpp:133 |
| 0x076D | `COMMAND_GET_ZONE_GANG_STRENGTH` | `GetZoneGangStrength` | medio | source/game_sa/Scripts/Commands/ZoneCommands.cpp:134 |
| 0x0784 | `COMMAND_GIVE_PLAYER_CLOTHES` | `GivePlayerClothes` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:509 |
| 0x0792 | `COMMAND_CLEAR_CHAR_TASKS_IMMEDIATELY` | `ClearCharTasksImmediately` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4276 |
| 0x079D | `COMMAND_PLAYER_ENTERED_DOCK_CRANE` | `PlayerEnteredDockCrane` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:510 |
| 0x079E | `COMMAND_PLAYER_ENTERED_BUILDINGSITE_CRANE` | `PlayerEnteredBuildingsiteCrane` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:511 |
| 0x079F | `COMMAND_PLAYER_LEFT_CRANE` | `PlayerLeftCrane` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:512 |
| 0x07AF | `COMMAND_GET_PLAYER_GROUP` | `GetPlayerGroup` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:513 |
| 0x07B4 | `COMMAND_SET_PLAYER_GROUP_RECRUITMENT` | `SetPlayerGroupRecruitment` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:514 |
| 0x07CC | `COMMAND_SET_PLAYER_ENTER_CAR_BUTTON` | `SetPlayerEnterCarButton` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:515 |
| 0x07EE | `COMMAND_SET_CAR_ALWAYS_CREATE_SKIDS` | `SetCarAlwaysCreateSkids` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:163 |
| 0x07F1 | `COMMAND_IS_PLAYER_PERFORMING_WHEELIE` | `IsPlayerPerformingWheelie` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:516 |
| 0x07F2 | `COMMAND_IS_PLAYER_PERFORMING_STOPPIE` | `IsPlayerPerformingStoppie` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:517 |
| 0x07F9 | `COMMAND_PLAYER_ENTERED_QUARRY_CRANE` | `PlayerEnteredQuarryCrane` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:518 |
| 0x07FA | `COMMAND_PLAYER_ENTERED_LAS_VEGAS_CRANE` | `PlayerEnteredLasVegasCrane` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:519 |
| 0x07FC | `COMMAND_DISPLAY_TEXT_WITH_FLOAT` | `DisplayTextWithFloat` | bajo | source/game_sa/Scripts/Commands/TextCommands.cpp:323 |
| 0x0801 | `COMMAND_GET_CAMERA_FOV` | `GetCameraFov` | alto | source/game_sa/Scripts/Commands/CameraCommands.cpp:347 |
| 0x0811 | `COMMAND_GET_CAR_CHAR_IS_USING` | `GetCarCharIsUsing` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4239 |
| 0x081C | `COMMAND_SET_TEXT_EDGE` | `SetTextEdge` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:361 |
| 0x0822 | `COMMAND_SET_FIRST_PERSON_IN_CAR_CAMERA_MODE` | `SetFirstPersonInCarCameraMode` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:348 |
| 0x082A | `COMMAND_SET_PLAYER_DUCK_BUTTON` | `SetPlayerAbleToUseCrouch` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:520 |
| 0x0834 | `COMMAND_DO_CAMERA_BUMP` | `DoCameraBump` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:314 |
| 0x0836 | `COMMAND_SET_OBJECT_ANIM_SPEED` | `SetObjectAnimSpeed` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:222 |
| 0x0837 | `COMMAND_IS_OBJECT_PLAYING_ANIM` | `IsObjectPlayingAnim` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:223 |
| 0x0839 | `COMMAND_GET_OBJECT_ANIM_CURRENT_TIME` | `GetObjectAnimCurrentTime` | alto | source/game_sa/Scripts/Commands/ObjectCommands.cpp:224 |
| 0x083A | `COMMAND_SET_OBJECT_ANIM_CURRENT_TIME` | `SetObjectAnimCurrentTime` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:225 |
| 0x083C | `COMMAND_SET_CHAR_VELOCITY` | `SetCharVelocity` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4086 |
| 0x083D | `COMMAND_GET_CHAR_VELOCITY` | `GetCharVelocity` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4087 |
| 0x083E | `COMMAND_SET_CHAR_ROTATION` | `SetCharRotation` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4088 |
| 0x0844 | `COMMAND_IS_VAR_TEXT_LABEL_EMPTY` | `IsTextLabelEmpty` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:395 |
| 0x0845 | `COMMAND_IS_LVAR_TEXT_LABEL_EMPTY` | `IsTextLabelEmpty` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:396 |
| 0x0846 | `COMMAND_IS_VAR_TEXT_LABEL16_EMPTY` | `IsTextLabelEmpty` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:397 |
| 0x0847 | `COMMAND_IS_LVAR_TEXT_LABEL16_EMPTY` | `IsTextLabelEmpty` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:398 |
| 0x0856 | `COMMAND_SET_CHAR_ALLOWED_TO_DUCK` | `SetCharAllowedToDuck` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4089 |
| 0x0860 | `COMMAND_SET_CHAR_AREA_VISIBLE` | `SetCharAreaVisible` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4090 |
| 0x0874 | `COMMAND_SET_ZONE_POPULATION_RACE` | `SetZonePopulationRace` | bajo | source/game_sa/Scripts/Commands/ZoneCommands.cpp:138 |
| 0x087E | `COMMAND_SET_CHAR_DROPS_WEAPONS_WHEN_DEAD` | `SetCharDropsWeaponsWhenDead` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4091 |
| 0x087F | `COMMAND_SET_CHAR_NEVER_LEAVES_GROUP` | `SetCharNeverLeavesGroup` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4092 |
| 0x0881 | `COMMAND_SET_PLAYER_FIRE_BUTTON` | `SetPlayerFireButton` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:525 |
| 0x0883 | `COMMAND_ATTACH_FX_SYSTEM_TO_CHAR_BONE` | `AttachFxSystemToCharBone` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4093 |
| 0x0889 | `COMMAND_GET_DEAD_CHAR_COORDINATES` | `GetDeadCharCoordinates` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4094 |
| 0x089B | `COMMAND_IS_PLAYER_IN_POSITION_FOR_CONVERSATION` | `IsPlayerInPositionForConversation` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:84 |
| 0x089C | `COMMAND_ENABLE_CONVERSATION` | `EnableConversation` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:78 |
| 0x08A2 | `COMMAND_PLANE_ATTACK_PLAYER_USING_DOG_FIGHT` | `PlaneAttackPlayerUsingDogFight` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:161 |
| 0x08B3 | `COMMAND_SET_ZONE_FOR_GANG_WARS_TRAINING` | `SetZoneForGangWarsTraining` | medio | source/game_sa/Scripts/Commands/ZoneCommands.cpp:139 |
| 0x08CA | `COMMAND_INIT_ZONE_POPULATION_SETTINGS` | `InitZonePopulationSettings` | medio | source/game_sa/Scripts/Commands/ZoneCommands.cpp:140 |
| 0x08D2 | `COMMAND_SET_OBJECT_SCALE` | `SetObjectScale` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:206 |
| 0x08D3 | `COMMAND_GET_CURRENT_POPULATION_ZONE_TYPE` | `GetCurrentPopulationZoneType` | alto | source/game_sa/Scripts/Commands/ZoneCommands.cpp:141 |
| 0x08ED | `COMMAND_CLEAR_CONVERSATION_FOR_CHAR` | `ClearConversationForChar` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:79 |
| 0x08F9 | `COMMAND_IS_VAR_TEXT_LABEL16_EQUAL_TO_TEXT_LABEL` | `AreTextLabelsEqual` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:400 |
| 0x08FA | `COMMAND_IS_LVAR_TEXT_LABEL16_EQUAL_TO_TEXT_LABEL` | `AreTextLabelsEqual` | bajo | source/game_sa/Scripts/Commands/BasicCommands.cpp:401 |
| 0x0901 | `COMMAND_SET_PLAYER_JUMP_BUTTON` | `SetPlayerJumpButton` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:526 |
| 0x090C | `COMMAND_SET_SPECIFIC_ZONE_TO_TRIGGER_GANG_WAR` | `SetSpecificZoneToTriggerGangWar` | medio | source/game_sa/Scripts/Commands/ZoneCommands.cpp:143 |
| 0x090D | `COMMAND_CLEAR_SPECIFIC_ZONES_TO_TRIGGER_GANG_WAR` | `ClearSpecificZonesToTriggerGangWar` | medio | source/game_sa/Scripts/Commands/ZoneCommands.cpp:144 |
| 0x091A | `COMMAND_GET_LATEST_CONSOLE_COMMAND` | `GetLatestConsoleCommand` | medio | source/game_sa/Scripts/Commands/UnusedCommands.cpp:23 |
| 0x091D | `COMMAND_SWITCH_ROADS_BACK_TO_ORIGINAL` | `SwitchRoadsBackToOriginal` | medio | source/game_sa/Scripts/Commands/PathCommands.cpp:87 |
| 0x091E | `COMMAND_SWITCH_PED_ROADS_BACK_TO_ORIGINAL` | `SwitchPedRoadsBackToOriginal` | medio | source/game_sa/Scripts/Commands/PathCommands.cpp:90 |
| 0x0920 | `COMMAND_CAMERA_SET_VECTOR_TRACK` | `CameraSetVectorTrack` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:349 |
| 0x0922 | `COMMAND_CAMERA_SET_LERP_FOV` | `CameraSetLerpFov` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:351 |
| 0x0925 | `COMMAND_CAMERA_RESET_NEW_SCRIPTABLES` | `CameraResetNewScriptables` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:352 |
| 0x092F | `COMMAND_CAMERA_PERSIST_TRACK` | `CameraPersistTrack` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:353 |
| 0x0930 | `COMMAND_CAMERA_PERSIST_POS` | `CameraPersistPos` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:354 |
| 0x0931 | `COMMAND_CAMERA_PERSIST_FOV` | `CameraPersistFov` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:355 |
| 0x0933 | `COMMAND_CAMERA_IS_VECTOR_MOVE_RUNNING` | `CameraIsVectorMoveRunning` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:357 |
| 0x0934 | `COMMAND_CAMERA_IS_VECTOR_TRACK_RUNNING` | `CameraIsVectorTrackRunning` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:358 |
| 0x0936 | `COMMAND_CAMERA_SET_VECTOR_MOVE` | `CameraSetVectorMove` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:360 |
| 0x0937 | `COMMAND_DRAW_WINDOW` | `DrawWindow` | medio | source/game_sa/Scripts/Commands/DrawCommands.cpp:124 |
| 0x093D | `COMMAND_SET_CINEMA_CAMERA` | `SetCinemaCamera` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:361 |
| 0x0942 | `COMMAND_HAS_PLAYER_BOUGHT_ITEM` | `HasPlayerBoughtItem` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:527 |
| 0x0944 | `COMMAND_SET_CAMERA_IN_FRONT_OF_CHAR` | `SetCameraInFrontOfChar` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:363 |
| 0x0945 | `COMMAND_GET_PLAYER_MAX_ARMOUR` | `GetPlayerMaxArmour` | alto | source/game_sa/Scripts/Commands/PlayerCommands.cpp:521 |
| 0x094E | `COMMAND_DISABLE_CHAR_SPEECH` | `DisableCharSpeech` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4240 |
| 0x094F | `COMMAND_ENABLE_CHAR_SPEECH` | `EnableCharSpeech` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4241 |
| 0x099C | `COMMAND_CAMERA_SET_SHAKE_SIMULATION_SIMPLE` | `CameraSetShakeSimulationSimple` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:364 |
| 0x09A4 | `COMMAND_SET_UP_CONVERSATION_NODE_WITH_SPEECH` | `SetUpConversationNodeWithSpeech` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:83 |
| 0x09AA | `COMMAND_SET_UP_CONVERSATION_END_NODE_WITH_SPEECH` | `SetUpConversationEndNodeWithSpeech` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:85 |
| 0x09AD | `COMMAND_SET_PLAYER_IN_CAR_CAMERA_MODE` | `SetPlayerInCarCameraMode` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:365 |
| 0x09B5 | `COMMAND_SET_CHAR_SIGNAL_AFTER_KILL` | `SetCharSignalAfterKill` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4242 |
| 0x09B6 | `COMMAND_SET_CHAR_WANTED_BY_POLICE` | `SetCharWantedByPolice` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4243 |
| 0x09B7 | `COMMAND_SET_ZONE_NO_COPS` | `SetZoneNoCops` | bajo | source/game_sa/Scripts/Commands/ZoneCommands.cpp:145 |
| 0x09BA | `COMMAND_DISPLAY_ZONE_NAMES` | `DisplayZoneNames` | medio | source/game_sa/Scripts/Commands/ZoneCommands.cpp:146 |
| 0x09BC | `COMMAND_SET_CHAR_COORDINATES_DONT_WARP_GANG_NO_OFFSET` | `SetCharCoordinatesDontWarpGangNoOffset` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4244 |
| 0x09C1 | `COMMAND_ADD_NEXT_MESSAGE_TO_PREVIOUS_BRIEFS` | `AddNextMessageToPreviousBriefs` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:363 |
| 0x09C5 | `COMMAND_IS_CHAR_USING_MAP_ATTRACTOR` | `IsCharUsingMapAttractor` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4245 |
| 0x09C9 | `COMMAND_REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION` | `RemoveCharFromCarMaintainPosition` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4246 |
| 0x09CC | `COMMAND_DOES_OBJECT_HAVE_THIS_MODEL` | `DoesObjectHaveThisModel` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:208 |
| 0x09D5 | `COMMAND_SET_CHAR_SAY_CONTEXT_IMPORTANT` | `SetCharSayContextImportant` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4247 |
| 0x09D6 | `COMMAND_SET_CHAR_SAY_SCRIPT` | `SetCharSayScript` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4248 |
| 0x09DE | `COMMAND_IS_CHAR_GETTING_IN_TO_A_CAR` | `IsCharGettingInToACar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4249 |
| 0x09E8 | `COMMAND_GET_CHAR_AREA_VISIBLE` | `GetCharAreaVisible` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4250 |
| 0x09EC | `COMMAND_ALLOW_FIXED_CAMERA_COLLISION` | `AllowFixedCameraCollision` | bajo | source/game_sa/Scripts/Commands/CameraCommands.cpp:366 |
| 0x09ED | `COMMAND_HAS_CHAR_SPOTTED_CHAR_IN_FRONT` | `HasCharSpottedCharInFront` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4251 |
| 0x09EF | `COMMAND_SET_VEHICLE_CAMERA_TWEAK` | `SetVehicleCameraTweak` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:367 |
| 0x09F0 | `COMMAND_RESET_VEHICLE_CAMERA_TWEAK` | `ResetVehicleCameraTweak` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:368 |
| 0x09F1 | `COMMAND_REPORT_MISSION_AUDIO_EVENT_AT_CHAR` | `ReportMissionAudioEventAtChar` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:95 |
| 0x09F4 | `COMMAND_IGNORE_HEIGHT_DIFFERENCE_FOLLOWING_NODES` | `IgnoreHeightDifferenceFollowingNodes` | medio | source/game_sa/Scripts/Commands/PedCommands.cpp:33 |
| 0x09F5 | `COMMAND_SHUT_ALL_CHARS_UP` | `ShutAllCharsUp` | medio | source/game_sa/Scripts/Commands/PedCommands.cpp:34 |
| 0x09F6 | `COMMAND_SET_CHAR_GET_OUT_UPSIDE_DOWN_CAR` | `SetCharGetOutUpsideDownCar` | medio | source/game_sa/Scripts/Commands/PedCommands.cpp:35 |
| 0x09F7 | `COMMAND_REPORT_MISSION_AUDIO_EVENT_AT_CAR` | `ReportMissionAudioEventAtCar` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:96 |
| 0x09F8 | `COMMAND_DO_WEAPON_STUFF_AT_START_OF_2P_GAME` | `DoWeaponStuffAtStartOf2PlayerGame` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:213 |
| 0x09FA | `COMMAND_HAS_GAME_JUST_RETURNED_FROM_FRONTEND` | `HasGameJustReturnedFromFrontend` | medio | source/game_sa/Scripts/Commands/PadCommands.cpp:43 |
| 0x09FB | `COMMAND_GET_CURRENT_LANGUAGE` | `GetCurrentLanguage` | alto | source/game_sa/Scripts/Commands/GameCommands.cpp:212 |
| 0x09FF | `COMMAND_SET_RESPAWN_POINT_FOR_DURATION_OF_MISSION` | `SetRespawnPointForDurationOfMission` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:211 |
| 0x0A07 | `COMMAND_SKIP_TO_NEXT_ALLOWED_STATION` | `SkipToNextAllowedStation` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:153 |
| 0x0A09 | `COMMAND_SHUT_CHAR_UP_FOR_SCRIPTED_SPEECH` | `ShutCharUpForScriptedSpeech` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4252 |
| 0x0A0A | `COMMAND_ENABLE_DISABLED_ATTRACTORS_ON_OBJECT` | `EnableDisabledAttractorOnObject` | medio | source/game_sa/Scripts/Commands/ObjectCommands.cpp:220 |
| 0x0A0B | `COMMAND_LOAD_SCENE_IN_DIRECTION` | `LoadSceneInDirection` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:92 |
| 0x0A0E | `COMMAND_CLEAR_THIS_PRINT_BIG_NOW` | `ClearThisPrintBigNow` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:319 |
| 0x0A10 | `COMMAND_INCREMENT_INT_STAT_NO_MESSAGE` | `IncrementFloatStatNoMessage` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:208 |
| 0x0A12 | `COMMAND_GET_EXTRA_CAR_COLOURS` | `GetExtraCarColors` | alto | source/game_sa/Scripts/Commands/VehicleCommands.cpp:151 |
| 0x0A14 | `COMMAND_SET_NO_RESPRAYS` | `SetNoResprays` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:210 |
| 0x0A16 | `COMMAND_ATTACH_MISSION_AUDIO_TO_CAR` | `AttachMissionAudioToCar` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:94 |
| 0x0A17 | `COMMAND_SET_HAS_BEEN_OWNED_FOR_CAR_GENERATOR` | `SetHasBeenOwnedForCarGenerator` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:146 |
| 0x0A18 | `COMMAND_SET_UP_CONVERSATION_NODE_WITH_SCRIPTED_SPEECH` | `SetUpConversationNodeWithScriptedSpeech` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:86 |
| 0x0A19 | `COMMAND_SET_AREA_NAME` | `SetAreaName` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:204 |
| 0x0A1B | `COMMAND_IS_CHAR_TOUCHING_CHAR` | `IsCharTouchingChar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4253 |
| 0x0A1C | `COMMAND_DISABLE_HELI_AUDIO` | `DisableHeliAudio` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:155 |
| 0x0A1E | `COMMAND_TAKE_PHOTO` | `TakePhoto` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:209 |
| 0x0A1F | `COMMAND_INCREMENT_FLOAT_STAT_NO_MESSAGE` | `IncrementFloatStatNoMessage` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:207 |
| 0x0A20 | `COMMAND_SET_PLAYER_GROUP_TO_FOLLOW_ALWAYS` | `SetPlayerGroupToFollowAlways` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:522 |
| 0x0A24 | `COMMAND_SET_DISABLE_MILITARY_ZONES` | `SetDisableMilitaryZones` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:206 |
| 0x0A25 | `COMMAND_SET_CAMERA_POSITION_UNFIXED` | `SetCameraPositionUnfixed` | medio | source/game_sa/Scripts/Commands/CameraCommands.cpp:369 |
| 0x0A26 | `COMMAND_SET_RADIO_TO_PLAYERS_FAVOURITE_STATION` | `SetRadioToPlayersFavorite` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:205 |
| 0x0A27 | `COMMAND_SET_DEATH_WEAPONS_PERSIST` | `SetDeathWeaponPersist` | medio | source/game_sa/Scripts/Commands/PedCommands.cpp:32 |
| 0x0A28 | `COMMAND_SET_SWIM_SPEED` | `SetSwimSpeed` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:523 |
| 0x0A29 | `COMMAND_IS_PLAYER_CLIMBING` | `IsPlayerClimbing` | medio | source/game_sa/Scripts/Commands/PlayerCommands.cpp:524 |
| 0x0A2B | `COMMAND_IS_WIDESCREEN_ON_IN_OPTIONS` | `IsWidescreenOnInOptions` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:202 |
| 0x0A2C | `COMMAND_DRAW_SUBTITLES_BEFORE_FADE` | `DrawSubtitlesBeforeFade` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:100 |
| 0x0A2D | `COMMAND_DRAW_ODDJOB_TITLE_BEFORE_FADE` | `DrawOddJobTitleBeforeFade` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:99 |
| 0x0A2F | `COMMAND_SET_PHOTO_CAMERA_EFFECT` | `SetPhotoCameraEffect` | medio | source/game_sa/Scripts/Commands/ScriptCommands.cpp:98 |
| 0x0A32 | `COMMAND_IS_CHAR_ATTACHED_TO_ANY_CAR` | `IsCharAttachedToAnyCar` | medio | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4254 |
| 0x0A33 | `COMMAND_STORE_CAR_CHAR_IS_ATTACHED_TO_NO_SAVE` | `StoreCarCharIsAttachedToNoSave` | alto | source/game_sa/Scripts/Commands/CharacterCommands.cpp:4255 |
| 0x0A39 | `COMMAND_GET_PLAYER_IN_CAR_CAMERA_MODE` | `GetPlayerInCarCameraMode` | alto | source/game_sa/Scripts/Commands/CameraCommands.cpp:370 |
| 0x0A3C | `COMMAND_SET_UP_CONVERSATION_END_NODE_WITH_SCRIPTED_SPEECH` | `SetUpConversationEndNodeWithScriptedSpeech` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:87 |
| 0x0A3D | `COMMAND_ACTIVATE_PIMP_CHEAT` | `ActivatePimpCheat` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:194 |
| 0x0A43 | `COMMAND_GET_RID_OF_PLAYER_PROSTITUTE` | `GetRidOfPlayerProstitute` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:203 |
| 0x0A44 | `COMMAND_DISPLAY_NON_MINIGAME_HELP_MESSAGES` | `DisplayNonMiniGameHelpMessages` | medio | source/game_sa/Scripts/Commands/TextCommands.cpp:364 |
| 0x0A45 | `COMMAND_SET_RAILTRACK_RESISTANCE_MULT` | `SetRailTrackResistanceMult` | medio | source/game_sa/Scripts/Commands/VehicleCommands.cpp:154 |
| 0x0A47 | `COMMAND_FINISH_SETTING_UP_CONVERSATION_NO_SUBTITLES` | `FinishSettingUpConversationNoSubtitles` | medio | source/game_sa/Scripts/Commands/ConversationCommands.cpp:82 |
| 0x0A49 | `COMMAND_IS_XBOX_VERSION` | `IsXboxVersion` | bajo | source/game_sa/Scripts/Commands/GameCommands.cpp:219 |
| 0x0A4A | `COMMAND_GET_PC_MOUSE_MOVEMENT` | `GetPCMouseMovement` | alto | source/game_sa/Scripts/Commands/PadCommands.cpp:40 |
| 0x0A4B | `COMMAND_IS_PC_USING_JOYPAD` | `IsPCUsingJoyPad` | medio | source/game_sa/Scripts/Commands/PadCommands.cpp:41 |
| 0x0A4C | `COMMAND_IS_MOUSE_USING_VERTICAL_INVERSION` | `IsMouseUsingVerticalInversion` | medio | source/game_sa/Scripts/Commands/PadCommands.cpp:42 |
| 0x0A4D | `COMMAND_IS_JAPANESE_VERSION` | `IsJapaneseVersion` | bajo | source/game_sa/Scripts/Commands/GameCommands.cpp:201 |
| 0x0A50 | `COMMAND_DO_DEBUG_STUFF` | `DoDebugStuff` | medio | source/game_sa/Scripts/Commands/GameCommands.cpp:225 |

## Riesgo (Fase 0 del banco de comandos)

Clasificacion de los reescritos por analisis de texto del manejador C++ (`contrib/riesgo_comandos_script.py`, reproducible; columnas `riesgo`, `devuelve`, `mundo`, `motivo_riesgo` del CSV).
- **alto**: devuelve valor al script (variable, varias variables o control de flujo) y lee/escribe estado del mundo. Un error de tipo o de semantica cambia el flujo del script sin ruido (caso `GET_CHAR_AREA_VISIBLE`).
- **medio**: escribe estado del mundo sin devolver valor, condiciones (bandera) que leen el mundo, o valores calculados.
- **bajo**: puro o trivial (aritmetica, comparaciones, asignaciones de variables de script).

| Riesgo | Comandos |
| --- | ---: |
| alto | 121 |
| medio | 390 |
| bajo | 157 |

| Retorno / mundo | Comandos |
| --- | ---: |
| devuelve=no, mundo=escribe | 305 |
| devuelve=no, mundo=no | 96 |
| devuelve=valor, mundo=lee | 87 |
| devuelve=flag, mundo=lee | 71 |
| devuelve=flag, mundo=no | 62 |
| devuelve=no, mundo=lee | 11 |
| devuelve=multi, mundo=lee | 11 |
| devuelve=valor, mundo=escribe | 10 |
| devuelve=valor, mundo=no | 10 |
| devuelve=resultado, mundo=escribe | 4 |
| devuelve=?, mundo=? | 1 |

| Categoria | alto | medio | bajo |
| --- | ---: | ---: | ---: |
| Basic | 4 | 14 | 143 |
| Camera | 7 | 35 | 1 |
| Character | 81 | 89 | 0 |
| Clock | 2 | 3 | 0 |
| Conversation | 0 | 11 | 0 |
| Draw | 0 | 8 | 0 |
| Game | 2 | 32 | 2 |
| Math | 0 | 6 | 1 |
| Mission | 1 | 3 | 0 |
| Object | 8 | 19 | 0 |
| Pad | 2 | 3 | 0 |
| Path | 0 | 6 | 0 |
| Ped | 0 | 4 | 0 |
| Player | 9 | 60 | 0 |
| Script | 0 | 12 | 1 |
| Sequence | 0 | 1 | 0 |
| Stat | 2 | 8 | 0 |
| Text | 0 | 49 | 4 |
| Unused | 0 | 1 | 0 |
| Vehicle | 2 | 19 | 0 |
| Zone | 1 | 7 | 5 |

## Pendiente

- Banco por comando: ejecutar cada manejador reescrito y el original del EXE con los mismos parametros y comparar valores devueltos, variables escritas y efectos. `GTASA_CMD_BYPASS` (archivo con ids) permite volver al manejador original de cualquier comando para esas comparaciones o para bisecciones.
- Primer error hallado en este inventario: `GET_CHAR_AREA_VISIBLE` (0x09E8) devolvia un booleano en vez del id del interior (ropero de la casa de CJ).
