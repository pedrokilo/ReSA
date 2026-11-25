import sys, re, json, collections
sys.path.insert(0, '.')
rows = json.load(open('../draft_rows.json'))
C_FULL = 'confirmado (leido el reemplazo)'
C_AUTO = 'confirmado parcial (campos y ramas por Setup extraidos del reemplazo; cuerpo no leido linea a linea)'
C_CTX = 'por contexto'
FX = 'XInput/lectura del mando'; FM = 'mapeo de botones y Setup 1/2'; FV = 'vibracion'; FC = 'trucos desde el pad'
FI = 'iconos de botones y ayudas/GXT'; FS = 'dibujado de sprites de botones'; FK = 'cambio teclado/mando'
FSC = 'scripts parcheados (ApplyMissionSpecificFixes)'; F2 = 'co-op/dos pads'; FF = 'foco/minimizado'
FST = 'Steam overlay/Guide'; FSX = 'SIXAXIS/SCP'; FMN = 'menus y pantallas de ayuda'; FO = 'otros'
SP = 'Southpaw (0x1003ACE8[pad]) y pad activo (objeto+0x7D)'
T = {
0x10002450: ("Sustituye CPad::ReconcileTwoControllersInput (co-op): en vez de combinar teclado y pad mezcla, campo a campo, dos CControllerState (sticks: el de mayor magnitud con el mismo signo; botones y gatillos: el mayor; Start/Select/L3/R3...) y devuelve el resultado; en el caso de Guide pendiente avisa al overlay.", F2 + '; ' + FST, C_AUTO),
0x10002790: ("AimWeaponLeftRight: devuelve LeftStickX o RightStickX segun " + SP + " (intercambia sticks al apuntar); 0 si DisablePlayerControls.", FM, C_AUTO),
0x10002850: ("AimWeaponUpDown: igual con los ejes Y (LeftStickY/RightStickY segun Southpaw y SwapSticksDuringAiming).", FM, C_AUTO),
0x10002920: ("LookAroundUpDown: eje vertical de mirar (RSY o LSY segun Setup y Southpaw).", FM, C_AUTO),
0x100029c0: ("LookAroundLeftRight: eje horizontal de mirar (RSX o LSX segun Setup y Southpaw).", FM, C_AUTO),
0x10002a50: ("CPad::GetDisplayVitalStats: L1 muestra estadisticas; en Setup 2 solo cuando el jugador no esta en vehiculo ni apuntando (consulta CPed +0x47C y +0x71C); respeta bDisablePlayerDisplayVitalStats.", FM, C_FULL),
0x10002ae0: ("GetLookLeft usado por el jetpack: L2 sin R2 (Setup 1) o L1 sin R1 (Setup 2).", FM, C_AUTO),
0x10002b30: ("GetLookRight usado por el jetpack: R2 sin L2 (Setup 1) o R1 sin L1 (Setup 2).", FM, C_AUTO),
0x10002b80: ("GetAccelerate sustituido en jetpack/BMX-IA: devuelve siempre el estado de Cross (ignora el Setup).", FM, C_FULL),
0x10002ba0: ("GetBrake sustituido en jetpack: devuelve el estado de Square.", FM, C_FULL),
0x10002bc0: ("GetEnterTargeting a pie (PlayerOnFoot): R1 (Setup 1) o L2 (Setup 2) con flanco New/Old; ademas llama a punteros guardados del juego (FindPlayerPed y GetWeaponInfo) para el apuntado automatico.", FM, C_AUTO),
0x10002d20: ("CCamera::IsItTimeForNewCamera: lee un eje (LeftStickX o RightStickX segun Southpaw) en lugar de llamar a GetPad (el valor llega por AX).", FM, C_FULL),
0x10002d50: ("CPad::IsRadioTrackSkipPressed: usa NewState/OldState.m_bRadioTrackSkip y el temporizador de pulsacion larga de la cruceta (0x1003AB60) para saltar de pista MP3.", FM, C_FULL),
0x10002d80: ("SniperZoomIn: Setup 1 Square/R2; Setup 2 un eje de stick (RSY/LSY) con sensibilidad propia (0x1003ACA4).", FM, C_AUTO),
0x10002e50: ("SniperZoomOut: Setup 1 Cross/L2; Setup 2 un eje de stick (RSY/LSY).", FM, C_AUTO),
0x10002f10: ("GetForceCameraBehindPlayer: L1 (Setup 1) o R1 (Setup 2).", FM, C_AUTO),
0x10002f40: ("GetTarget en CVehicle::GetPlaneWeaponFiringStatus: R1 (Setup 1) o Square (Setup 2).", FM, C_AUTO),
0x10002f70: ("GetEnterTargeting en GetPlaneWeaponFiringStatus: R1 (Setup 1) o Square (Setup 2) con flanco.", FM, C_AUTO),
0x10002fb0: ("Salto BMX (ProcessBunnyHop): tras DisablePlayerControls usa L1 (Setup 1) o Square (Setup 2) con flanco; ademas parchea EN CALIENTE el byte 0x6C0736 (0x1C al pulsar, 0x08 al soltar) para cambiar la condicion de salto.", FM, C_FULL),
0x100030a0: ("Avion/helicoptero: devuelve true si L3/R3 (R3 = ShockButtonR; con Southpaw se intercambian) -> sustituye la comprobacion de ShockButtonR/OldShockButtonR de CPlane (cambio de vista/tren de aterrizaje).", FM, C_FULL),
0x10003110: ("WeaponJustDown en nado (CTaskSimpleSwim): flanco de Circle.", FM, C_AUTO),
0x10003170: ("CarGunJustDown: Setup 1 Circle (y L1 si no se desactivo el fuego con L1); Setup 2 solo Circle.", FM, C_AUTO),
0x100031d0: ("CarGunJustDown en avion: Setup 1 Circle/L1; Setup 2 Circle o Cross (flanco).", FM, C_AUTO),
0x10003240: ("CarGunJustDown en avion con intercambio por pad: Setup 2 acepta Cross y Circle.", FM, C_AUTO),
0x100032f0: ("GetCarGunFired: Setup 1 Circle/L1; Setup 2 Circle.", FM, C_AUTO),
0x10003340: ("GetCarGunFired en avion: Setup 1 Circle/L1; Setup 2 Circle o Cross.", FM, C_AUTO),
0x10003390: ("LastStationJustUp (emisora anterior): Setup 1 cruceta Abajo, Setup 2 cruceta Izquierda; con temporizador de pulsacion larga (pista MP3).", FM, C_AUTO),
0x100033f0: ("NextStationJustUp (emisora siguiente): Setup 1 cruceta Arriba, Setup 2 cruceta Derecha.", FM, C_AUTO),
0x10003450: ("WeaponJustDown a pie: Circle/L1 (y R2) segun Setup y bDisablePlayerFireWeapon*, con GetWeaponInfo del juego (puntero guardado) para armas especiales.", FM, C_AUTO),
0x10003590: ("GetWeapon a pie: igual que el anterior sin flanco.", FM, C_AUTO),
0x10003770: ("GetWeapon (PlayerOnFoot): envoltura que consulta 0x10003590 y la rutina 0x100036b0 (recarga/arma secundaria) devolviendo el boton Cross en casos especiales.", FM, C_AUTO),
0x10003820: ("CCamera::CamControl: 'cambiar camara' = flanco de bajada de Select con el temporizador de pulsacion larga 0x1003AB30 (>1500 ms = modo repeticion).", FM, C_FULL),
0x10003850: ("GetCameraCycleUp: Select con flanco y temporizador (replay con pulsacion larga).", FM, C_FULL),
0x10003890: ("sub_5404F0 (GetCameraCycleDown): devuelve siempre 0 (todo el cambio de camara va por GetCameraCycleUp).", FM, C_FULL),
0x100038a0: ("GetTarget: R1 (Setup 1) o L2 (Setup 2).", FM, C_FULL),
0x100038d0: ("ShiftTargetRightJustDown: R2 (Setup 1) o R1 (Setup 2), con flanco.", FM, C_AUTO),
0x10003920: ("ShiftTargetLeftJustDown: L2 (Setup 1) o L1 (Setup 2), con flanco.", FM, C_AUTO),
0x10003970: ("GetEnterTargeting: R1 (Setup 1) o L2 (Setup 2), con flanco.", FM, C_AUTO),
0x100039b0: ("CycleWeaponRightJustDown: R2 (Setup 1) o cruceta Derecha (Setup 2); respeta bDisablePlayerCycleWeapon.", FM, C_AUTO),
0x10003a10: ("CycleWeaponLeftJustDown: L2 (Setup 1) o cruceta Izquierda (Setup 2).", FM, C_AUTO),
0x10003a70: ("GetHorn: L3 (R3 con Southpaw y pad activo).", FM, C_FULL),
0x10003ad0: ("HornJustDown: flanco de L3 (R3 con Southpaw y pad activo).", FM, C_FULL),
0x10003b40: ("GetLookLeft: L2 sin R2 (Setup 1) o L1 sin R1 (Setup 2).", FM, C_FULL),
0x10003b80: ("GetLookRight: R2 sin L2 (Setup 1) o R1 sin L1 (Setup 2).", FM, C_AUTO),
0x10003bc0: ("GetLookBehindForPed: R3 (L3 con Southpaw).", FM, C_FULL),
0x10003c20: ("GetLookBehindForCar: R2 y L2 a la vez (Setup 1) o R1 y L1 (Setup 2).", FM, C_FULL),
0x10003c60: ("GetAccelerate: Cross (Setup 1) o R2 analogico (Setup 2).", FM, C_FULL),
0x10003c90: ("GetBrake: Square (Setup 1) o L2 analogico (Setup 2).", FM, C_FULL),
0x10003cc0: ("GetHandBrake: R1 (Setup 1) o Cross (Setup 2).", FM, C_FULL),
0x10003cf0: ("GetPedWalkLeftRight: LeftStickX (RightStickX con Southpaw y pad activo; LSX si el teclado es el dispositivo).", FM, C_FULL),
0x10003d40: ("GetPedWalkUpDown: LeftStickY (RightStickY con Southpaw).", FM, C_FULL),
0x10003d90: ("GetSteeringLeftRight: stick (RSX/LSX segun Southpaw), mas direccion por giroscopio SIXAXIS segun tipo de vehiculo (0x1003AB14 + comprobacion 0x10004BA0); recorta a +-127 y escribe el buffer de direccion del CPad (+0x60/+0x74).", FM + '; ' + FSX, C_FULL),
0x10003ea0: ("GetSteeringUpDown: igual para el eje vertical (inclinacion SIXAXIS en moto/avion/helicoptero).", FM + '; ' + FSX, C_AUTO),
0x10003f80: ("GetCarGunLeftRight: stick derecho (izquierdo con Southpaw).", FM, C_AUTO),
0x10003fd0: ("GetCarGunUpDown: stick derecho (izquierdo con Southpaw).", FM, C_AUTO),
0x10004020: ("Hidraulica (CAutomobile::HydraulicControl): GetCarGunLeftRight con inversion de ejes por pad (0x1003AB48/0x1003AB4A) y Southpaw.", FM, C_AUTO),
0x100040d0: ("Hidraulica: GetCarGunUpDown con inversion de eje y Southpaw.", FM, C_AUTO),
0x10004190: ("DuckJustDown: flanco de L3 (R3 con Southpaw); respeta bDisablePlayerDuck.", FM, C_AUTO),
0x10004200: ("GetDuck en PlayerControlFighter: L3 (R3 con Southpaw).", FM, C_AUTO),
0x10004260: ("ConversationNoJustDown: cruceta Izquierda (Setup 1) o cruceta Abajo / Cross (Setup 2).", FM, C_AUTO),
0x100042b0: ("ConversationYesJustDown: cruceta Derecha (Setup 1) o cruceta Arriba / Cross (Setup 2).", FM, C_AUTO),
0x10004300: ("ConversationNoJustDown (tarea de proxeneta): Izquierda (Setup 1) o Abajo (Setup 2).", FM, C_AUTO),
0x10004340: ("ConversationYesJustDown (tarea de proxeneta): Derecha (Setup 1) o Arriba (Setup 2).", FM, C_AUTO),
0x10004380: ("GetGroupControlForward: cruceta Arriba (Setup 1) o Cross+Arriba (Setup 2).", FM, C_AUTO),
0x100043c0: ("GetGroupControlBack: cruceta Abajo (Setup 1) o Cross+Abajo (Setup 2).", FM, C_AUTO),
0x10004400: ("Sustituye la llamada a CPad::Update dentro de CPad::UpdatePads: llama al Update original (puntero guardado), refresca la bandera de SIXAXIS por vehiculo del pad 0/1 (0x1003AB14/0x1003AE50 usando CPed+0x594/+0x38C), decide el flag 0x1003AB37 (FreeAim/raton en 3a persona) con FindPlayerPed y el arma activa, y evalua Start para abrir la pausa.", FX + '; ' + FK, C_FULL),
0x100045b0: ("Sustituye la llamada a CPad::UpdateMouse dentro de UpdatePads: llama al original, comprueba actividad de teclado/raton (0x1003AD28) y, si la hay, marca el pad como inactivo: Mode del CPad a 0, flag +0x7D a 0 y fuerza la bandera de raton; implementa el cambio automatico teclado/mando.", FK, C_FULL),
0x10004660: ("CPad::StopShaking(pad): pone ShakeFreq y ShakeDur a 0 y llama a XInputSetState(pad,0,0) (apaga la vibracion).", FV, C_FULL),
0x100046d0: ("CPad::StopPadsShaking: para los 2 pads: limpia ShakeFreq/ShakeDur y llama a XInputSetState con vibracion 0.", FV, C_FULL),
0x10004750: ("CPad::GetHydraulicJump: R3 o L3 (Southpaw) segun pad activo e inversion; salto de hidraulica con el stick.", FM, C_AUTO),
0x10004800: ("GetLookRight en helicoptero/avion: devuelve un float con el giro de camara usando L1/R1 o L2/R2 segun Setup y flechas, Select para la camara.", FM, C_AUTO),
0x10004950: ("Sustituye la llamada a CCheat::DoCheats en CGame::Process: llama al original y despues convierte el boton de pad pulsado (0x1003AB10/0x1003AB34) en letra (T,C,X,S,U,D,L,R,1..4), mantiene un historial de 12 caracteres (0x1003AB1C), calcula el hash y lo busca en la tabla de 90 hashes PS2 (0x10032F00); si coincide activa o desactiva el truco (ApplyCheat/m_aCheatsActive guardados) y muestra el texto GXT CHEAT1/CHEAT8 como ayuda.", FC, C_FULL),
0x10005b20: ("Sustituye CRunningScript::GetPadState: clasifica el script en ejecucion por su nombre (CRunningScript+8) en codigos 1-7 y para cada boton de script (0-20) devuelve el campo de CPad segun Setup 1/2 y la clasificacion (misiones con controles propios).", FSC + '; ' + FM, C_FULL),
0x100060f0: ("COMMAND_LOAD_TEXTURE_DICTIONARY (0x390): si el script se llama 'bdisply' y ControlsSet/pad estan activos devuelve un sprite de la tabla de 4 entradas (0x10033634) tras activar el TXD 'GInput'; si no activa el TXD 'script'.", FSC + '; ' + FS, C_FULL),
0x10006180: ("Comando de script (rango 1900-1999) usado por el script 'dance': escribe en ScriptParams[0..2] el Old.Start, el eje del stick y el boton Old.Square (0x63/0x64 = flechas) cuando el pad esta activo.", FSC, C_FULL),
0x10006200: ("Segundo comando de script del baile (script 'dance'): igual con Old.Cross y Old.L3.", FSC, C_FULL),
0x100063c0: ("CMenuManager::HasLanguageChanged (comando de script): devuelve true cuando cambia TempKeyState.R1 (byte +0x85) o la bandera de iconos PS (0x1003AB63); sirve para forzar el refresco de textos con iconos.", FI, C_FULL),
0x10006e10: ("GetAnaloguePadUp: true si LeftStickY < -10 con el estado anterior >= -5 (cruceta analogica).", FM, C_FULL),
0x10006e30: ("GetAnaloguePadDown: analogico hacia abajo con histeresis.", FM, C_AUTO),
0x10006e50: ("GetAnaloguePadLeft: analogico izquierda con histeresis.", FM, C_AUTO),
0x10006e70: ("GetAnaloguePadRight: analogico derecha con histeresis.", FM, C_AUTO),
0x10007d70: ("Sustituye la llamada a CPad::ProcessPad? (la segunda de UpdatePads, 0x746A10): llama a la rutina de lectura 0x10007550 para el pad 0 y el 1 (XInputGetState + conversion a CControllerState en TempJoyState, zonas muertas, sensibilidad, curvas, SIXAXIS, vibracion, Setup).", FX, C_FULL),
0x10007da0: ("Sustituye la llamada a CPad::UpdatePads en Game::FrontendIdle y en CGame::Process: en la primera ejecucion carga los TXD de botones y crea los sprites; despues llama al UpdatePads original.", FS + '; ' + FX, C_AUTO),
0x10007e50: ("Desde CFont::Initialise (+0x139): tras crear la fuente carga models\\x360btns.txd, ps3btns.txd y sixaxis.txd con CTxdStore (FindTxdSlot/AddTxdSlot/LoadTxd/PushCurrentTxd/SetCurrentTxd) y asigna los sprites de boton; si faltan, mensaje de error y cierre.", FS, C_AUTO),
0x10008320: ("Desde CFont::Shutdown (+0x2B): descarga sprites y TXD de botones (CSprite2d::Delete y RemoveTxdSlot) antes de terminar el sistema de fuentes.", FS, C_AUTO),
0x10009040: ("Sustituye CFont::ParseToken: interpreta los tokens de boton (~<~ ~>~ ~u~ ~d~ ~x~ ~o~ ~[x~ ~[y~ ~<>~ ~ud~ ~@NOMBRE~ ...), escribe el indice de sprite en la tabla PS2Symbol del juego, fija el ancho segun la tabla 0x1003ABF0 (PS3/Xbox) y para los demas tokens delega en el ParseToken original (puntero guardado).", FI + '; ' + FS, C_FULL),
0x100093f0: ("COMMAND_GET_POSITION_OF_ANALOGUE_STICKS (0x494): en los scripts 'dance' y 'lowgame' devuelve cruceta/botones como sticks; en el resto lee LeftStick/RightStick con Southpaw e inversion por pad.", FSC + '; ' + FM, C_AUTO),
0x100099c0: ("CFont::GetStringWidth: al encontrar un token '~' llama al nuevo ParseToken para sumar el ancho del icono de boton.", FI + '; ' + FS, C_AUTO),
0x10009a00: ("CFont::ProcessCurrentString: avanza segun el ancho del ultimo icono de boton (0x1003AE60 = 3 o 4 caracteres).", FI, C_AUTO),
0x10009a10: ("CMenuManager::PrintMap: en el mapa sustituye la consulta de la tecla 'zoom/leyenda' (GetPad+call) por 'pad activo y L1 pulsado'.", FMN, C_FULL),
0x10009a30: ("CMenuManager::AdditionalOptionInput: sustituye la lectura de CPad (boton del mapa) por 'pad activo y boton equivalente'.", FMN, C_CTX),
0x10009a60: ("CMenuManager::AdditionalOptionInput: sustituye la consulta por Select (New/Old) del pad.", FMN, C_AUTO),
0x10009a90: ("Comando COMMAND_GET_CONTROLLER_MODE (0x293): devuelve CPad::Mode del pad 0 (via selector de pad 0x10002440) a ScriptParams.", FSC + '; ' + FM, C_FULL),
0x10009ab0: ("CMenuManager::CheckForMenuClosing: cierra el menu con Start (flanco) o con Triangle (Setup 1) / Circle (Setup 2), ademas de ESC (punteros a NewKeyState.esc y OldKeyState.esc).", FMN, C_FULL),
0x10009b20: ("Input::KeyboardHandler (antes de HandleKeyDown): llama a ConfigPad::SetActive(false) del pad 0 (0x10015050) para registrar que la ultima entrada fue del teclado.", FK, C_FULL),
0x10009b40: ("CMessages::InsertPlayerControlKeysInString: filtra por tecla (0x2E, 0x11, 0x25) o por el flag de iconos de pad (0x1003AB63); en modo pad salta a 0x100087B0 (texto de boton), si no delega en GetGxtStringOfCommandKeys (0x1003B080).", FI, C_FULL),
0x10009b70: ("CReplay::Update: la comprobacion de tecla de repeticion (GetIsKeyboardKeyJustDown) tambien acepta el pad (Select con temporizador).", FK + '; ' + FM, C_AUTO),
0x10009ba0: ("CReplay::Update: segunda comprobacion (salir de repeticion) con pad.", FK + '; ' + FM, C_AUTO),
0x10009c30: ("CBike::ProcessControlInputs: GetAccelerate de moto con analogico/Setup.", FM, C_AUTO),
0x10009c50: ("CBike::ProcessControlInputs: GetHandBrake de moto.", FM, C_AUTO),
0x10009c70: ("Game::RenderEffects: sustituye la llamada a CPlayerPed::DrawTriangleForMouseRecruitPed (se omite o se condiciona al flag de raton/FreeAim).", FO, C_CTX),
}
S = {
0x47a649: ("COMMAND_IS_PC_USING_JOYPAD (0xA4B): el operando de 'mov al,[0xBA6818]' (m_ControlMethod) se cambia por la direccion leida en 0x50A851, es decir CCamera::m_bUseMouse3rdPerson (0xB6EC2E).", FK, C_FULL),
0x47a655: ("Mismo comando: 'jne' -> 'je' (invierte el resultado: 'usa pad' cuando 0xB6EC2E vale 0).", FK, C_FULL),
0x48a460: ("Comando de script (ProcessCommands1100To1199+0x140): lee el byte propio 0x1003AB37 en lugar de CCamera::m_bUseMouse3rdPerson (0xB6EC2E).", FK, C_FULL),
0x4840cf: ("Salto incondicional a 0x4840D9 que descarta el codigo original que seguia a la llamada sustituida en 0x4840B2.", FSC, C_FULL),
0x48af19: ("NOP de 6 bytes: elimina 'mov [0xA43C84],edx' (escritura de ScriptParams[3]) tras GET_POSITION_OF_ANALOGUE_STICKS, porque lo escribe el reemplazo.", FSC, C_FULL),
0x50f304: ("CCam::Process_1rstPersonPedOnPC: 'fmul [0x859AAC]' pasa a multiplicar por una variable propia (0x1003ACA4, sensibilidad de la vista con stick).", FM, C_FULL),
0x50f338: ("Idem (segundo fmul) en Process_1rstPersonPedOnPC.", FM, C_FULL),
0x510873: ("CCam::Process_M16_1stPerson: fmul [0x859AAC] -> variable propia 0x1003ACA4.", FM, C_FULL),
0x5108fb: ("Idem (segundo fmul) en Process_M16_1stPerson.", FM, C_FULL),
0x51d7c5: ("Continuacion del parche de IsItTimeForNewCamera: guarda EAX (valor del stick) en [esp+0x1C] en lugar de ECX.", FM, C_FULL),
0x5303e1: ("CControllerConfigManager::GetGxtStringOfCommandKeys+0x11: 'je' -> 'jmp' (siempre rama de texto), para que GInput pueda controlar el texto desde el hook de la funcion.", FI, C_FULL),
0x530b00: ("CControllerConfigManager::InitDefaultControlConfigJoyPad: el cuerpo se sustituye por 'ret 4' (anula la configuracion por defecto del joypad del juego).", FM, C_FULL),
0x540062: ("CPad::GetHandBrake+0x22 (caso del switch de Mode): lectura [ecx+0x1E] (Triangle) pasa a [ecx+0x20] (Cross). Probablemente codigo muerto: la entrada de la funcion ya salta a GInput; sirve de compatibilidad si otro mod restaura la entrada.", FM, C_CTX),
0x540071: ("CPad::GetHandBrake+0x31 (tabla de saltos del switch): una entrada del jump table cambia a 0x540062. Mismo comentario que el anterior.", FM, C_CTX),
0x541c98: ("CPad::Update+0x58: NOP de 5 bytes sobre la llamada a CControllerState::Clear (no se borra el estado cada frame; lo gestiona GInput).", FX, C_FULL),
0x541ce2: ("CPad::Update+0xa2: reescribe 'cmp DisablePlayerControls,0; movzx eax,Mode...' por 'mov ecx,ebx; call GetHorn(0x10003A70); jmp +' (la bocina/L3 en Update se evalua con el reemplazo).", FM, C_FULL),
0x5744a3: ("CMenuManager::Constructor+0x153: NOP de 4 bytes sobre 'mov byte [esi+0x20],1' (no fuerza el ajuste de vibracion a 1; lo pone el INI).", FV, C_FULL),
0x575139: ("CMenuManager::PrintMap+0x9: sustituye 'movzx eax,[edi+0xEA]; push; call GetPad' por llamada a 0x10009A10 con salto condicional.", FMN, C_FULL),
0x576c89: ("CMenuManager::CheckForMenuClosing+0x119: 'push 1' -> 'jmp +0x11' (salta la rama que cerraba el menu).", FMN, C_FULL),
0x576e77: ("CMenuManager::CheckForMenuClosing: misma rama saltada con 'jmp +0x11'.", FMN, C_FULL),
0x578876: ("CMenuManager::AdditionalOptionInput (+0x14A7): sustituye 'movzx; push; call GetPad' por llamada a 0x10009A30.", FMN, C_FULL),
0x578b8e: ("CMenuManager::AdditionalOptionInput (+0x17BF): igual, llamada a 0x10009A30.", FMN, C_FULL),
0x57cbc2: ("CMenuManager::LoadSettings+0x2D2: 'mov dl,[edi+0xD0]' (m_ControlMethod) -> 'lea edx,[edi+0xD0]'.", FK, C_FULL),
0x57cbcc: ("CMenuManager::LoadSettings+0x2DC: 'test dl,dl; sete cl' -> 'mov cl,1; mov byte [edx],0': al cargar ajustes fuerza m_ControlMethod=0 (teclado+raton) y m_bUseMouse3rdPerson=1; el pad lo gestiona GInput aparte.", FK, C_FULL),
0x57f21d: ("CMenuManager::RedefineScreenUserInput+0x2CD: 'cmp [eax+0x1E]' <-> 'cmp [eax+0x4E]' (Triangle New <-> Old) conmutado por 0x1000D2A0 segun [0x1003AB2C].", FMN, C_FULL),
0x57f224: ("RedefineScreenUserInput+0x2D4: 'cmp [eax+0x4E]' <-> 'cmp [eax+0x1E]'.", FMN, C_FULL),
0x57f737: ("CMenuManager::DrawControllerSetupScreen+0x437: NOP de la llamada a CFont::PrintString (no se dibuja el texto de mapeo original; lo sustituye GInput).", FMN, C_FULL),
0x577493: ("CMenuManager::AdditionalOptionInput+0xC3: 'cmp [eax+0x22]' (Circle New) <-> 'cmp [eax+0x52]' (Old) conmutado por 0x1000D2A0.", FMN, C_FULL),
0x57749a: ("AdditionalOptionInput+0xCA: idem sobre [eax+0x52].", FMN, C_FULL),
0x580358: ("CMenuManager::UserInput+0x5E8: 'cmp [eax+0x1E]' <-> 'cmp [eax+0x4E]' conmutado por 0x1000D2A0.", FMN, C_FULL),
0x58035f: ("CMenuManager::UserInput+0x5EF: 'cmp [eax+0x4E]' <-> 'cmp [eax+0x1E]'.", FMN, C_FULL),
0x5ba7c2: ("CFont::Initialise+0x132: se descarta la carga del TXD de fuentes original y se salta a 0x10007E50 (pila: add esp,0x18/pop esi/add esp,4), que hace la carga propia.", FS, C_FULL),
0x5d1cd5: ("CSimpleVariablesSaveStructure::Construct+0x155: 'mov cx,[eax+0x10A]' (CPad::Mode del pad 0) -> 'xor ecx,ecx; nop' (no guarda el Mode en la partida).", FM, C_FULL),
0x5d1f6e: ("CSimpleVariablesSaveStructure::Extract+0xCE: NOP de 7 bytes sobre 'mov [eax+0x10A],cx' (no restaura el Mode al cargar).", FM, C_FULL),
0x6cb329: ("CPlane::ProcessControlInputs+0x559: NOP de 5 bytes sobre la comparacion original de ShockButtonR (la hace 0x100030A0).", FM, C_FULL),
0x6cb33e: ("CPlane::ProcessControlInputs+0x56E: 'jne' -> 'je' para la nueva logica de 0x100030A0.", FM, C_FULL),
0x7189d9: ("CFont::Shutdown+0x29: salta a 0x10008320 (libera sprites de boton) en lugar del codigo original.", FS, C_FULL),
0x718a96: ("CFont::PrintChar+0x86: 'fmul [0x85F100]' -> 'fmul [0x1003B084]' (ancho variable del icono de boton).", FS, C_FULL),
0x718ade: ("CFont::PrintChar+0xCE: tabla de sprites 'ButtonSprite' (0xC71AD8) -> tabla propia 0x1003AD60.", FS, C_FULL),
0x719a53: ("CFont::RenderFontBuffer+0x213: 'fmul [0x85F100]' -> '[0x1003B084]'.", FS, C_FULL),
0x744a33: ("Input::AttachInputDevices+0x13: NOP de 5 bytes sobre la llamada a Rs::RsInputDeviceAttach (no se inicializa el DirectInput de pad del juego).", FX, C_FULL),
0x67ee04: ("CTaskSimpleJetPack::ProcessControlInput: 'fmul [0x858B1C]' -> '[0x1003ACC4]' (sensibilidad de giro del jetpack con stick).", FM, C_FULL),
0x67ee4e: ("Idem (segundo fmul) del jetpack.", FM, C_FULL),
0x6c4b24: ("CHeli: sustituye 'call GetLookRight; movzx ecx,al; mov [esp+0x38],ecx...' por 'call 0x10004800; fstp [esi+0x98C]; jmp' (guarda el float de mirar directamente).", FM, C_FULL),
0x6cb1fe: ("CPlane::ProcessControlInputs: sustituye GetLookRight por 0x10004800 con 'jmp 0x6CB241'.", FM, C_FULL),
}
REDIR = {0x60b65e, 0x60cc9f, 0x60d350, 0x685a65, 0x685cb5, 0x686905, 0x6869b0, 0x686c64, 0x686ce6, 0x48a460}

def tipo_row(r):
    O, N = r['O'], r['N']; a = r['addr']
    if a in (0x47a649,): return 'parche de bytes (operando de lectura reapuntado)'
    if N.startswith('nop'): return 'parche de bytes (NOP)'
    if a in REDIR: return 'parche de operando (0xB6EC2E -> variable propia 0x1003AB37)'
    if 'fmul' in N and '0x1003' in N: return 'parche de operando (fmul -> variable propia)'
    if 'ecx*4 + 0x1003ad60' in N: return 'parche de operando (tabla propia)'
    if O.startswith('call') and N.startswith('call 0x1000'): return 'gancho CALL (reescritura del rel32)'
    if N.startswith('jmp 0x1000'): return 'gancho JMP en la entrada de la funcion'
    if re.match(r'(mov ecx, (eax|ebx|esi); call|push ecx; mov ecx|push esi; call|mov ecx, esi; lea)', N) or 'call 0x1000' in N or 'jmp 0x1000' in N:
        return 'parche de codigo con CALL/JMP inyectado'
    return 'parche de bytes (reescritura de instrucciones)'

out = []
for r in rows:
    a = r['addr']
    if a in S: txt, feat, conf = S[a]
    elif a in REDIR: txt, feat, conf = ("Lee el byte propio 0x1003AB37 (modo apuntado libre/raton en 3a persona decidido por GInput) en lugar de CCamera::m_bUseMouse3rdPerson (0xB6EC2E).", FM + '; ' + FK, C_FULL)
    elif r['tgt'] in T: txt, feat, conf = T[r['tgt']]
    else: txt, feat, conf = ("(sin clasificar)", FO, C_CTX)
    tp = tipo_row(r)
    dest = ('0x%08X' % r['tgt']) if r['tgt'] else ''
    if r['tag'] == 'late1': ctxn = 'instalador diferido 0x10008294'
    elif r['tag'] == 'menu': ctxn = 'conmutable por 0x1000D2A0 (segun 0x1003AB2C)'
    else: ctxn = 'instalador 0x1000A740'
    out.append(['0x%08X' % a, '0x%08X' % r['end'], r['end'] - a + 1, tp, dest, r['ctx'], r['cname'], txt, feat, conf, ctxn, r['O'], r['N']])

# filas extra (modificaciones no capturadas por el instalador estatico)
EX = [
 ['0x0074872E', '0x00074872E', 4, 'gancho CALL (rel32 del CALL 0x7468E0 en WinMain)', '0x1000A660', 'Win::NOTSA_WinMain+0x1d', '0x7468E0 (primer CALL de WinMain)',
  'Primer parche: desvia el CALL de WinMain a 0x1000A660 (guarda el destino original en 0x1003AE6C). Cuando WinMain lo ejecuta, 0x1000A660 hace el parche diferido de CText::Get (para encadenar con otros mods) y luego sigue.', FO + '; ' + FI, 'confirmado (leido el reemplazo)', 'instalador 0x1000A740 (via 0x1000E230)', '', ''],
 ['0x006A0050', '0x006A0054', 5, 'gancho JMP en la entrada (diferido y encadenado)', '0x10009BE0', 'CText::Get', 'CText::Get',
  'Si 0x6A0050 ya empieza por 0xE9 (otro mod ya ha enganchado) reescribe el rel32 de ese JMP (0x6A0051) para ir a 0x10009C10 y guarda el destino previo (0x1003AE68); si no, escribe E9 + rel32 a 0x10009BE0 y guarda 0x6A0055. El reemplazo (0x10009BE0/0x10009C10) guarda la clave (0x1003AE70), ejecuta el prologo original y llama a 0x10009680: calcula el CRC32 (mayusculas) de la clave; si es el de "GPUTERR" devuelve el texto de error de texturas de GInput (0x100341B8); si hay iconos de pad activos (0x1003AB63) y ApplyGXTFixes (0x1003AB2A), busca el CRC en la tabla ordenada 0x1003AE74 (124 entradas, la crea 0x10009900) y aplica el regex->reemplazo sobre el texto del juego (tabla en ginput-tablas.md seccion 1).', FI, 'confirmado (leido instalador y 0x10009BE0/0x10009C10/0x10009680 hasta la busqueda; el motor regex posterior no se leyo)', 'diferido 0x1000A660', '', ''],
 ['0x00748454', '0x00748457', 4, 'parche de operando (IAT): call [DefWindowProcA] -> call [var propia]', '0x1000A5D0', 'Win::__MainWndProc+0x5a4', 'USER32!DefWindowProcA (IAT 0x8582A4)',
  'Cambia el operando de "call dword ptr [0x8582A4]" por 0x100384B8 (variable que contiene 0x1000A5D0). El reemplazo trata WM_SETFOCUS(7) y WM_KILLFOCUS(8): fija 0x100384BC y llama a 0x1000A4B0 (soltar/recuperar pads y vibracion); despues salta al DefWindowProcA original. Es el unico hook de API de GInput.', FF, 'confirmado (leido el reemplazo)', 'instalador 0x1000D260', '', ''],
 ['0x007484A4', '0x007484A7', 4, 'variante EU del anterior (no aplicada en US)', '0x1000A5D0', 'Win::__MainWndProc+0x5f4', 'idem', 'Variante de 0x1000E290 para el EXE 1.0 EU (bandera 0x1003AB02): solo cambia la direccion del CALL.', FF, 'por contexto (no se aplica con el EXE US)', 'instalador 0x1000D260', '', ''],
 ['0x006C0736', '0x006C0736', 1, 'parche de bytes EN CALIENTE (1 byte)', '', 'CBmx::ProcessBunnyHop+0x1a6', 'CBmx::ProcessBunnyHop',
  'Desde 0x10002FB0 escribe 0x1C (al pulsar salto BMX) o 0x08 (al soltar) en el byte del codigo del juego, con VirtualProtect cada vez, y recuerda el estado en 0x1003AB00.', FM, 'confirmado (leido el reemplazo)', 'runtime desde 0x10002FB0', '', ''],
 ['0x00BA6748+0x1FD2', '0x00BA6748+0x2019', 0x48, 'escritura de datos globales (memcpy 0x48 bytes)', '', 'FrontEndMenuManager (tabla de pantallas, +0x1FD2)', 'CMenuManager::m_aScreens (entrada de la pantalla de mando)',
  'Copia 3 registros de menu (FEC_RED, FEC_MOU, FET_DEF...) de 0x10033098 en FrontEndMenuManager+0x1FD2; antes pone a 0 el bloque +0x201A (0x90 bytes) y escribe +0x20B3=0, +0x2277=1, +0x161B=2. La direccion de FrontEndMenuManager se lee del operando de la instruccion en 0x576B38.', FMN, 'confirmado (leido el instalador; el significado exacto de cada campo es contextual)', 'instalador 0x1000A740 (bloque 0x1000C73E)', '', ''],
 ['0x1003AE?? (vibracion)', '', 4, 'escritura de datos globales (INI -> campo del juego)', '', 'FrontEndMenuManager+0x20', 'CMenuManager::m_PrefsUseVibration',
  'Al leer GInputSA.ini, "Vibration" se escribe en el byte FrontEndMenuManager+0x20 (puntero 0x1003AE18) y "PlayStationButtons" en la bandera propia; la opcion del menu del juego y el INI quedan sincronizadas.', FV, 'confirmado (leido el lector de INI 0x10009CE0 en las lineas citadas)', 'runtime desde 0x10009CE0', '', ''],
 ['0x1000A600 (SCP)', '', 4, 'reemplazo de puntero de funcion en la DLL xinput1_3 de SCP', '0x10009CC0', 'xinput1_3.dll (SCP Virtual Bus)', 'XInputGetExtended (SCP)',
  'Si GetModuleHandle("xinput1_3") tiene la palabra 0x3C7 en +0x108A, toma el puntero en +0x108C y, en la tabla de ese objeto, reemplaza el puntero de funcion en +0x58 por 0x10009CC0 (guardando el original en 0x1003AE64) para poder interceptar los datos de presion de botones/acelerometros del DS3.', FSX, 'confirmado (leido el parcheador)', 'instalador 0x1000A600 (solo si la INI activa SixaxisHack)', '', ''],
]
for e in EX: out.append(e)
for k, o in enumerate(out, 1): o.insert(0, k)
json.dump(out, open('../rows_main.json', 'w'))
print(len(out))
print(collections.Counter(o[4] for o in out))
print(collections.Counter(o[10] for o in out))
print([o[1] for o in out if o[8] == '(sin clasificar)'])
