# GInputSA 1.11 (Silent): inventario de lo que contiene y de lo que podemos aprovechar

Fecha: 25/11/2026. Origen: `1470119658_ginputsa-1 (1).zip` del usuario (paquete de distribución: `GInputSA.asi` 241 664 B, `GInputSA.ini`, `ReadMe.txt`,
`GAME CONTROLS FULL LIST.txt`, listas de trucos HTML, texturas de botones). El usuario indica haber pedido autorización a Silent para usarlo.
Análisis **estático** (PE, importaciones, cadenas, direcciones del EXE referidas por el código); el binario no se ejecutó ni se redistribuye, y no se copió
código suyo: lo de abajo es el inventario de funciones para reimplementarlas en C++ propio (atribución en `THIRD_PARTY_NOTICES.md`).

## 1. El binario

- PE 32 bits, compilado con MSVC (CRT estático, sin símbolos), secciones `.text` 0x2A716, `.rdata`, `.data`, `.reloc`.
- Importa `XINPUT1_3.dll` **por ordinal** (2 funciones: `XInputGetState` y el ordinal 100, `XInputGetStateEx`/«Guide»), `KERNEL32` (76), `ADVAPI32` (registro, para
  localizar Steam). Exporta 2 funciones sin nombre (0xD650, 0xD660). `GetPrivateProfileIntW`: lee `GInputSA.ini` (relectura al volver de la pausa).
- Cadenas que delatan módulos: `steam.dll`, `steamclient.dll`, `SteamClient012`, `SteamFriends014`, `SteamUtils007`, `SteamTenfoot` (overlay de Steam con el botón
  Guide); `XInputGetExtended`, `xinput1_3` (se espera el `xinput1_3.dll` de SCP para botones sensibles a presión y acelerómetros SIXAXIS);
  `models\x360btns.txd`, `models\ps3btns.txd`, `models\sixaxis.txd`, `models\pcbtns.txd` y los nombres de textura (circle, cross, square, triangle, l1…r3, up/down/left/right,
  `dlr`, `dud`, `thumbl/thumbr` y sus variantes `x/xl/xr/y/yu/yd`, `sixaxis1..4`); mensaje de error si faltan las texturas.

## 2. Qué hace (de ReadMe, INI y cadenas) y estado en nuestro proyecto

| Función de GInput | Cómo lo hace (por lo visto) | Nuestro estado |
| --- | --- | --- |
| XInput completo, Start/Guide, gatillos analógicos | Sustituye DirectInput; ordinal 100 para Guide | Hecho en `Pad.cpp` (XInput propio, sticks, cruceta, botones). Guide/overlay de Steam: no aplica (sin Steam) |
| Vibración | `XInputSetState` desde las funciones de sacudida | Hecho (`CPad::StartShake…`, opción de menú de vibración); no medida con mando real |
| **Dos juegos de controles**: «Setup 1» (equivale a los selectables de PS2) y «Setup 2» (recreación de GTA IV) | `ControlsSet=1|2` por pad; hooks de todas las consultas de `CPad` | PS2 por defecto (pedido del usuario). **El esquema estilo GTA IV no existe**: pendiente |
| Trucos con el mando (secuencias PS2) | Hook de `CCheat::DoCheats`/`ApplyCheat` (0x439AF0 / 0x438370) y cadenas `CHEAT1`…`CHEAT8`; usa la lista `cheat_list_*.html` | **En curso** (rama `pad-cheats`): 87 secuencias, ver `trucos-mando.md` |
| Iconos de botones en las ayudas y textos | `CText::Get` (0x6A0050), `CFont::ParseToken` (0x718F00), `CHud::SetHelpMessage` (0x588BE0), `CControllerConfigManager::GetGxtStringOfCommandKeys` (0x5303D0); dibujo con `CSprite2d` y TXD de botones; ancho variable por textura | **En curso** (rama `pad-button-icons`) |
| Cambio automático teclado/mando por último dispositivo | Detección de entrada reciente | Parcial (`PadDiagnostico`/`Pad.cpp`); lo cubre la rama de iconos |
| **ApplyGXTFixes**: correcciones de ~120 textos GXT (ayudas de misiones y tutoriales) para que citen el botón correcto | Tabla clave→regex→reemplazo con tokens propios (`~@PED_MOVE~`, `~@CAMERA_UP_DOWN~`, `~@HYDRA_TARGET~`, `~[u~`, `~o~`…); claves vistas: DES5_*, DST8_*, SPAC_*, DRV3_*, HEI2/3_*, HM9_*, HYDHLP1-3, MAN2_*, RACES33, RM4_32, RY2_68, STE2_03, ZER2_3/4, Z4_H13/14, SYN7_20, HOOD2B, S6_37, GYM1_*, BARB_H1, BJ_H1-5, CLOTHA/B, CONS_*, FOOD_H, TATTA/B, WARDH1-3, WINCH_V, BOAT_*, CAS9_H2, DS1_*, DES5_9B… (≈120 claves en total) | Pendiente: la rama de iconos resuelve los tokens `~k~~ACCION~`; las correcciones por mensaje habrá que revisarlas con su informe |
| Texto nuevo para la ayuda del robo («ski mask… ~@PED_MOVE~ to sneak», clave RYD1_93) | Sustitución de GXT | Pendiente (un texto) |
| **ApplyMissionSpecificFixes**: parches de SCM para que misiones usen los controles IV y el minijuego de baile use botones en vez de flechas | Parcheo en memoria de los scripts `bball`, `gymls/gymsf/gymlv`, `dance`, `heist1`, `casino1/9`, `camera`, `gfdate`, `zero2/4`, `desert5`, `ryder2/3`, `driv2`, `dskool/bskool`, `lowrcon`, `bcour/bcesar2`, `cokec`, `bdisply`, `ambulan/firetru/copcar/taxiodd/burgjb/freight`… (tabla de nombres de script) | Parcial: la primera tanda de SilentPatch ya parchea `bball`, gimnasio y baile en `SilentPatchScripts.cpp`; los controles de dichas misiones con mando no verificados |
| `PlayStationButtons`, `MapPadOneToPadTwo`, `FreeAim` | Opciones | Iconos PS/Xbox: rama de iconos. Pad1→jugador 2 y apuntado libre: no existen |
| Opciones por pad: `Southpaw`, `SwapSticksDuringAiming`, `InvertLook`, `Invert/SwapLeft/RightAxes`, `Left/RightStickDeadzone` (24/27), `Left/RightStickSensitivity` (0-200: curva cúbica→lineal→raíz), `FaceButtonsSensitivity` (50), `DrivebyWithAnalog` | Postproceso del estado analógico por pad | Hay inversión de «look» (`bInvertLook4Pad`) y zonas muertas propias; **no existen** Southpaw, ejes invertidos/permutados, curva de sensibilidad configurable ni sensibilidad de botones |
| Co-op con dos pads / pad 1 → jugador 2 | `ReconcileTwoControllersInput` (0x53F530) | Hecho a medias (`Pad.cpp`); sin probar con dos mandos |
| Entrada ignorada con la ventana sin foco/minimizada (`DisableOnFocusLost`) | Comprobación de foco | Pendiente (comprobar) |
| Controles nuevos: mantener «cambiar emisora» recorre las pistas MP3; R3 hace saltar coches con hidráulica; mantener «cambiar cámara» activa la repetición; Start/Triángulo unificados en menús | Hooks de `CPad` | No existen (menús: ver `ControllerConfigManager`) |
| SIXAXIS (acelerómetros, DS3 con SCP): recarga, dirección de coche/moto/barco/helicóptero/avión, hidráulica, `SixaxisSensitivity=150` | Necesita el `xinput1_3.dll` de SCP | **Fuera de alcance** (driver de terceros, mando antiguo) |

## 3. Direcciones del EXE que el binario referencia (344)

Se extrajeron los operandos inmediatos/absolutos del código (`direcciones_exe.json` junto al paquete; no se versiona). 73 caen en funciones ya catalogadas en `docs/hooks.csv`:
las consultas de `CPad` (`GetSteeringLeftRight/UpDown`, `GetCarGunUpDown/LeftRight`, `GetPedWalkLeftRight/UpDown`, `GetLookLeft/Right`, `GetLookBehind*`, `GetHorn`, `HornJustDown`,
`GetCarGunFired`, `CarGunJustDown`, `GetHandBrake`, `GetBrake`, `GetWeapon`, `WeaponJustDown`, `GetAccelerate`, `GetCameraCycleUp`, `GetTarget`, `GetEnterTargeting`, `DuckJustDown`,
`ShiftTargetLeft/RightJustDown`, `GetAnaloguePadUp/Down/Left/Right`, `GetForceCameraBehindPlayer`, `SniperZoomIn/Out`, `LookAroundLeftRight/UpDown`, `AimWeaponLeftRight/UpDown`,
`ConversationYes/NoJustDown`), `CPad::Update`/`UpdateMouse`/`StopShaking`/`StopPadsShaking`/`ReconcileTwoControllersInput`, `CControllerConfigManager::InitDefaultControlConfigJoyPad`/
`GetGxtStringOfCommandKeys`/`GetIsKeyboardKeyJustDown`, `CCheat::DoCheats`/`ApplyCheat`, `CRunningScript::GetPadState`, `CHud::SetHelpMessage`, `CText::Get`, `CFont::ParseToken`,
`CMenuManager::MessageScreen`, `CTxdStore::*` (carga de TXD), `CSprite2d::Delete/SetTexture`, `CWeaponInfo::GetWeaponInfo`, `CPlayerPed::DrawTriangleForMouseRecruitPed`,
`CPed::GetWeaponSkill`, `CPedIntelligence::GetTaskJetPack`, `CStats::IncrementStat`, `App::DoRWStuffEndOfFrame`, `Input::HandleKeyDown`, `FindPlayerPed/Vehicle`.
Las 271 restantes son direcciones de datos y de instrucciones parcheadas dentro de esas rutinas (sin nombre en nuestras tablas todavía).

## 4. Qué se puede portar y cómo (propuesta de orden)

1. Hecho/en curso: trucos con el mando; iconos de botones (+ selección PS/Xbox).
2. Opciones de entrada por pad (Southpaw, ejes invertidos/permutados, zonas muertas y curva de sensibilidad 0-200, sensibilidad de botones): módulo propio de postproceso en `Pad.cpp`, opciones en `gta-reversed.ini` (`Pad1.*`/`Pad2.*`).
3. Juego de controles «estilo GTA IV» (ControlsSet=2): requiere definir el mapa acción→botón (ver `GAME CONTROLS FULL LIST.txt`, «Setup 2»), ampliar `ControllerConfigManager` y los textos de ayuda; es lo más grande.
4. Controles nuevos (MP3, salto hidráulico con R3, repetición con «cambiar cámara», Start/Triángulo en menús) y `DisableOnFocusLost`.
5. Tabla de correcciones de GXT por mensaje y el texto nuevo del robo, tras ver el alcance de la rama de iconos.
6. Fuera de alcance: SIXAXIS/SCP, overlay de Steam.

No se ha desensamblado el cuerpo de las funciones (no hace falta para estas propuestas: el comportamiento está documentado en `ReadMe.txt`, el INI y la lista de controles).
