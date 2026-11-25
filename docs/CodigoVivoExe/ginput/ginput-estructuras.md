# GInputSA 1.11: estructuras del juego que lee o escribe

Analisis estatico del .asi (offsets leidos del codigo; nombres cruzados con `source/game_sa/*.h` y `docs/MemoryMap/globals.csv`). "R" = lee, "W" = escribe.

## CPad (sizeof 0x134; base `CPad::Pads` 0xB73458; el indice se calcula con (this-base)/0x134 mediante la multiplicacion 0x3531DEC1)
| offset | campo | uso en GInput |
|---|---|---|
| +0x00..0x2F | NewState (CControllerState: LSX 0, LSY 2, RSX 4, RSY 6, L1 8, L2 0xA, R1 0xC, R2 0xE, Up 0x10, Down 0x12, Left 0x14, Right 0x16, Start 0x18, Select 0x1A, Square 0x1C, Triangle 0x1E, Cross 0x20, Circle 0x22, L3 0x24, R3 0x26, m_bChatIndicated 0x28, m_bPedWalk 0x2A, m_bVehicleMouseLook 0x2C, m_bRadioTrackSkip 0x2E) | R en todos los getters sustituidos; W en `ReconcileTwoControllersInput` (0x10002450) y en la lectura de pads |
| +0x30..0x5F | OldState (mismos campos, +0x30) | R para flancos (JustDown) y para el cambio de camara/menus; los parches de menu conmutan cmp [eax+0x1E/0x22] <-> [eax+0x4E/0x52] |
| +0x60..0x73, +0x74 | SteeringLeftRightBuffer (10 int16) y DrunkDrivingBufferUsed | W en GetSteeringLeftRight/UpDown (0x10003D90/0x10003EA0) |
| +0x78..0xA7 | PCTempKeyState | R/W: 0x10004660/0x100046D0 lo ponen a 0 al parar vibracion; +0x85 (campo R1 en offset temp) se compara en 0x100063C0 |
| +0xA8..0xD7 | PCTempJoyState | W por la lectura XInput (0x10007550) y R por los comandos de baile (0x10006180: LSX) |
| +0xD8..0x107 | PCTempMouseState | R (0xFC aparece como L3 de este bloque en 0x10003450: probable falso positivo, es un campo de otra estructura) |
| +0x10A | Mode (0 = Setup 1, 1 = Setup 2) | W: ConfigPad::SetActive (0x10015050) = ControlsSet-1 al activar el pad, 0 al volver al teclado; R: todos los getters; **no se guarda en la partida** (parche 0x5D1CD5/0x5D1F6E) |
| +0x10C | ShakeDur | R/W: vibracion (0x10007518, 0x10007D27 suman; StopShaking pone 0) |
| +0x10E | DisablePlayerControls | R en todos los getters (primer test) |
| +0x110 | ShakeFreq | W 0 en StopShaking/StopPadsShaking |
| +0x118..0x11F | bApplyBrakes, bDisablePlayerEnterCar, Duck(0x11A), FireWeapon(0x11B), FireWeaponWithL1(0x11C), CycleWeapon(0x11D), Jump, DisplayVitalStats(0x11F) | R: DuckJustDown, CarGun*, CycleWeapon*, WeaponJustDown, GetDisplayVitalStats |
| +0x120..0x130 | LastTimeTouched, AverageWeapon, AverageEntries, NoShakeBeforeThis, NoShakeFreq | R/W en 0x10007550 (vibracion con tiempos) |
Otros CPad estaticos: `NewKeyState.esc` 0xB733A8 y `OldKeyState.esc` 0xB73138 (R en menus), `bInvertLook4Pad` 0xB73402, `NewMouseControllerState` 0xB73418. Funciones con CALL reemplazado: ver CSV (CPad::Update, UpdateMouse, UpdatePads, GetPad, StopShaking, StopPadsShaking...).

## Objeto de pad propio de GInput (clase con vtable 0x10033660; 2 instancias en 0x1003ACEC[2]; exportadas por 0x1000D650 y 0x1000D660)
+0x04 indice de pad; +0x10..0x18 contadores; **+0x7D = pad activo (ultimo dispositivo usado es el mando)**; +0x14/+0x16 ejes crudos del stick izquierdo leidos en 0x10006EE0; vtable +0x10 registra un callback (se registra 0x1000D2A0 para reaplicar los parches de menu al releer el INI). Arrays por pad: Southpaw 0x1003ACE8, Mode 0x1003AB2C, inversiones 0x1003AB64..ABDC, zonas muertas 0x1003ABD0/AB84, sensibilidad 0x1003AC8C/ABC4/AB90/AE58, SIXAXIS 0x1003AC98..ACCC y 0x1003ACB8, entrada por giroscopio 0x1003AB48/0x1003AB4A (pares de int16).

## CControllerConfigManager
- `InitDefaultControlConfigJoyPad` (0x530B00): sustituido por `ret 4` (el juego ya no carga el mapa por defecto del joypad).
- `GetGxtStringOfCommandKeys` (0x5303D0): `je`->`jmp` en +0x11 (0x5303E1) y su direccion se guarda en 0x1003B080 para llamarla desde 0x10009B40 (via `CMessages::InsertPlayerControlKeysInString` 0x69E259: tokens ~k~ filtrados por tecla 0x2E/0x11/0x25).
- `GetIsKeyboardKeyJustDown` (0x52E450) guardada en 0x1003AD5C; los dos CALL de `CReplay::Update` (0x4605AA/0x460604) pasan a 0x10009B70/0x10009BA0.

## CMenuManager (FrontEndMenuManager 0xBA6748)
R/W: +0x20 vibracion (W desde el INI; el constructor ya no la fuerza a 1, parche 0x5744A3), +0xD0 m_ControlMethod (LoadSettings lo fuerza a 0 y pone m_bUseMouse3rdPerson=1), +0xEA (campo leido en PrintMap/AdditionalOptionInput, reemplazado), +0x85 (R, flag de idioma en 0x100063C0). Memoria escrita en +0x1FD2 (0x48 bytes, 3 registros FEC_RED/FEC_MOU/FET_DEF...), +0x201A (0x90 bytes a 0), +0x20B3=0, +0x2277=1, +0x161B=2. `CheckForMenuClosing` (0x576B9F) cierra con Start/Triangle/Circle; `DrawControllerSetupScreen` (0x57F737) no imprime el texto de controles.

## CHud / CText / CFont / CSprite2d / CTxdStore
- `CHud::SetHelpMessage` 0x588BE0: se guarda y se llama (0x1003AD1C) para mostrar CHEAT1/CHEAT8; `CHud::Draw`+0x144 (0x58FC24): `GetDisplayVitalStats` -> 0x10002A50.
- `CText::Get` 0x6A0050: JMP encadenado diferido (ver CSV); `TheText` 0xC1B340 se guarda en 0x1003AD30; GXT `CHEAT1/CHEAT8/GPUTERR`; tabla de correcciones en `ginput-tablas.md`.
- `CFont`: `ParseToken` 0x718F00 (3 CALL en RenderFontBuffer/PrintSegment/ProcessCurrentString -> 0x10009040; original guardado en 0x1003ABE0); `GetStringWidth`+0xA1 y `ProcessCurrentString`+0x116 llaman a 0x100099C0/0x10009A00; `PrintChar`+0x86, `RenderFontBuffer`+0x213: `fmul [0x85F100]` -> `[0x1003B084]`; `PrintChar`+0xCE: `ButtonSprite` (0xC71AD8) -> tabla propia 0x1003AD60; `PS2Symbol` 0xC71A54 (tabla de simbolos) leida via 0x1003AE1C; `Initialise`+0x132/+0x139 y `Shutdown`+0x29 cargan/liberan los TXD de botones.
- `CSprite2d::Delete` 0x727240, `SetTexture` 0x7272B0 y `CTxdStore::{AddTxdSlot 0x731C80, RemoveTxdSlot 0x731CD0, FindTxdSlot 0x731850, AddRef 0x731A00, LoadTxd 0x7320B0, PushCurrentTxd 0x7316A0, PopCurrentTxd 0x7316B0, SetCurrentTxd 0x7319C0}` se llaman por puntero guardado.

## CCheat
`DoCheats` 0x439AF0 (CALL en CGame::Process 0x53BFB8 -> 0x10004950; original 0x1003AD20), `ApplyCheat` 0x438370 (0x1003AD38), `m_aCheatsActive` 0x969130 (0x1003AD34), 0x96918C (0x1003AD24), `AddToCheatString` operando 0xB5F851 (0x1003AD10), `CStats::IncrementStat` 0x55C180. Hashes en `ginput-tablas.md` seccion 6.

## CPed / CPlayerPed / CVehicle (solo lecturas)
`FindPlayerPed` 0x56E210 y `FindPlayerVehicle` 0x56E0D0 por puntero; offsets de CPed leidos: +0x47C (objeto consultado por 0x10014BE0/BC10/BC0, estado de vehiculo/apuntado), +0x594, +0x38C bit 17, +0x5A0 (m_aWeapons, tamano 0x1C), +0x718 (slot activo), +0x71C. Sus nombres no se confirmaron con `Ped.h`.

## Otros datos del juego
- `CCamera::m_bUseMouse3rdPerson` 0xB6EC2E: 10 lecturas redirigidas a 0x1003AB37; el comando 0xA4B lo lee invertido; GInput lo escribe a traves de 0x1003AE0C (0x10004400, 0x10015050).
- Constantes flotantes del juego redirigidas: 0x859AAC (vista en 1a persona), 0x858B1C (jetpack), 0x85F100 (ancho de icono).
- `CTimer`/radio 0xB7CB84 (leido por 0x1003AE14).
- Script: `CRunningScript` +8 nombre (strnicmp de 8), +0x14; `ScriptParams` 0xA43C78 (0x1003AE20).
- Ventana: WinMain 0x74872D, WndProc 0x748454 (DefWindowProcA IAT 0x8582A4), `SystemParametersInfoA` y `GetCommandLineA` (IAT) solo aparecen como contexto.
