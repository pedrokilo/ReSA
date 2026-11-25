import sys, json, re, struct, collections
sys.path.insert(0, '.')
from g import *
import modes as M
from names2 import label

rows = json.load(open('../rows_main.json'))
out = []
w = out.append
w('# GInputSA 1.11: tablas de datos propias del mod\n')
w('Origen: analisis estatico de `GInputSA.asi` (base 0x10000000). Las direcciones son del .asi salvo que se diga lo contrario. Todo extraido leyendo el binario; no se copio codigo.\n')

# ---------- 1. GXT
fix = {'DRV3DRV3': ['DRV3_3', 'DRV3_H'], 'HM9_HM9_': ['HM9_24', 'HM9_49'], 'LOW_LOW_': ['LOW_37'], 'PL_HPL_H': ['PL_H1'],
       'RM4_RM4_': ['RM4_32'], 'RUN_RUN_': ['RUN_XX'], 'RY2_RY2_': ['RY2_68'], 'HOODHOOD': ['HOOD2B'], 'S6_3S6_3': ['S6_37'],
       'ZER2ZER2': ['ZER2_3', 'ZER2_4'], 'Z4_HZ4_H': ['Z4_H13', 'Z4_H14']}
ents = []
for l in open('../gxt_table.txt', encoding='utf8'):
    p = l.rstrip('\n').split('\t'); ents.append((p[1], p[2], p[3]))
for l in open('../gxt_table2.txt', encoding='utf8'):
    p = l.rstrip('\n').split('\t')
    k = p[1]
    if k in fix: k = fix[k].pop(0)
    ents.append((k, p[2], p[3]))
w('## 1. Tabla de correcciones GXT (ApplyGXTFixes): %d entradas\n' % len(ents))
w('Estructura de 0x28 bytes en 0x100384D0 (68 entradas estaticas) y 0x10038F70 (56 construidas por el inicializador estatico 0x10001280): clave GXT de hasta 8 caracteres, puntero al patron (regex de ECMAScript, con tokens `~k~~ACCION~` del juego), puntero al reemplazo (con tokens propios `~@NOMBRE~`, `~o~`, `~x~`, `~<>~`, `~ud~`, `~[u~`, `~[d~`, `~[x~`, `~[y~`, `~>~`, `~<~`, `~[~`, `~v~` y referencias `$1`). Las 9 ultimas (FEH_*, FET_*, RYD1_93) tienen reemplazo nulo: el segundo campo es el texto completo que sustituye al GXT original. Las claves `DRV3_*`, `HM9_*`, etc. se reconstruyeron con la lista de cadenas porque el lector solo cargaba 8 bytes.\n')
w('| # | clave | patron | reemplazo |\n|---|---|---|---|')
for n, (k, a, b) in enumerate(ents):
    a = a.replace('|', '\\|'); b = b.replace('|', '\\|')
    w('| %d | %s | `%s` | %s |' % (n, k, a, ('`%s`' % b) if b != '<0>' else '(texto completo: el patron es el nuevo texto)'))
w('')
w('Mecanismo: hook de `CText::Get` (0x6A0050) -> 0x10009BE0 -> 0x10009680: CRC32 de la clave en mayusculas; busqueda en la tabla ordenada (0x1003AE74, 124 pares CRC/puntero creados por 0x10009900 con comparador 0x10009520); solo si hay iconos de pad (0x1003AB63) y ApplyGXTFixes (0x1003AB2A). Clave especial `GPUTERR` = mensaje de error de texturas.\n')
w('Tokens propios de reemplazo vistos (se resuelven en `CFont::ParseToken` 0x10009040 / 0x10008E80): ~@PED_MOVE~, ~@GO_UPDOWN~, ~@GO_LEFTRIGHT~, ~@CAMERA_LEFT_RIGHT~, ~@CAMERA_UP_DOWN~, ~@VEHICLE_CHANGE_RADIO_STATION~, ~@SNATCH_PACKAGE~, ~@BMX_BUNNYHOP~, ~@HYDRA_TARGET~ (nombres en 0x10033xxx); los iconos de un solo caracter se mapean a sprites (ver seccion 5).\n')

# ---------- 2. Setup 1 vs 2
w('## 2. Mapeo de botones por juego de controles (Mode 0 = Setup 1, Mode 1 = Setup 2 estilo GTA IV)\n')
w('`CPad::Mode` vale ControlsSet-1 (tabla 0x1003AB2C por pad); lo escribe `ConfigPad::SetActive` (0x10015050) al activar el pad y se pone a 0 al volver al teclado. Cada reemplazo bifurca por Mode. Columnas: campos de `CControllerState` leidos (New = estado actual, Old = anterior) en cada rama, extraidos automaticamente del codigo del reemplazo; la columna "significado" es la lectura del reemplazo (ver el CSV). Los nombres de campos siguen `source/game_sa/ControllerState.h` (L1=LeftShoulder1, R1=RightShoulder1, L3=ShockButtonL, R3=ShockButtonR). Contraste: `GAME CONTROLS FULL LIST.txt` del paquete concuerda en las filas comprobadas (acelerar Cross / R2, freno Square / L2, freno de mano R1 / Cross, mirar atras L2+R2 / L1+R1, objetivo R1 / L2).\n')
w('| funcion del juego | reemplazo .asi | Setup 1 (Mode 0) | Setup 2 (Mode 1) | significado |\n|---|---|---|---|---|')
seen = set()
desc = {}
for r in rows:
    try: dest = int(r[5], 16) if r[5] else None
    except: dest = None
    if dest and dest not in desc: desc[dest] = (r[7], r[8])
for dest in sorted(desc):
    try: m = M.modes(dest)
    except Exception: m = None
    if not m or (not m[0] and not m[1]): continue
    fn = desc[dest][0] or ''
    w('| %s | 0x%08X | %s | %s | %s |' % (fn, dest, ', '.join(m[0]), ', '.join(m[1]), desc[dest][1][:110].replace('|', '/')))
w('')
w('Funciones con Southpaw / pad activo (sin rama por Mode, usan 0x1003ACE8[pad] y objeto+0x7D): GetPedWalkLeftRight/UpDown (LSX/LSY <-> RSX/RSY), GetCarGunLeftRight/UpDown, AimWeapon*, LookAround*, GetHorn/HornJustDown, GetLookBehindForPed, DuckJustDown, GetDuck (L3 <-> R3), GetSteering* (con direccion SIXAXIS).\n')

# ---------- 3. INI
w('## 3. Opciones del INI -> variables del .asi (lector 0x10009CE0)\n')
w('Seccion `[GInput]` (globales) y `[Pad1]`/`[Pad2]` (por pad, indice 0/1). Lectura con `GetPrivateProfileIntW`; el INI es `GInputSA.ini` junto al .asi (el nombre se forma sustituyendo la extension, 0x1000D4B5). Se relee al volver de la pausa.\n')
w('| clave | seccion | variable / efecto |\n|---|---|---|')
ini = [
('Enable (DllMain)', 'GInput', 'si 0, el .asi no instala ningun parche (0x1000D50F)'),
('Vibration', 'GInput', 'byte FrontEndMenuManager+0x20 (puntero 0x1003AE18)'),
('PlayStationButtons', 'GInput', 'bandera de iconos PS (0x1003AB63 via SetActive)'),
('ApplyMissionSpecificFixes', 'GInput', '0x1003AB5F (activa GetPadState por script, comandos dance/bdisply)'),
('ApplyGXTFixes', 'GInput', '0x1003AB2A (activa la tabla de la seccion 1)'),
('CheatsFromPad', 'GInput', '0x1003AB5B (activa el hook de DoCheats)'),
('DisableOnFocusLost', 'GInput', '0x1003AB33 (WM_KILLFOCUS)'),
('MapPadOneToPadTwo', 'GInput', '0x1003AB5A (pad 1 -> jugador 2; usado por UpdatePads/StopShaking)'),
('FreeAim', 'GInput', '0x1003AB5E (apuntado libre; decide 0x1003AB37 en 0x10004400)'),
('ControlsSet', 'PadN', 'palabra 0x1003AB2C[pad] = valor-1 (Mode)'),
('SwapSticksDuringAiming', 'PadN', 'byte 0x1003ACC8[pad]'),
('InvertLook', 'PadN', 'byte 0x1003AC94[pad] (= valor==0)'),
('InvertLeftXAxis / InvertLeftYAxis / SwapLeftAxes', 'PadN', 'bytes 0x1003AB80 / 0x1003ABD8 / 0x1003ABC0 [pad]'),
('InvertRightXAxis / InvertRightYAxis / SwapRightAxes', 'PadN', 'bytes 0x1003AB64 / 0x1003ABBC / 0x1003ABDC [pad]'),
('Southpaw', 'PadN', 'byte 0x1003ACE8[pad]'),
('FaceButtonsSensitivity', 'PadN', 'entero 0x1003AB90[pad] y float 0x1003AE58[pad] (= valor/255 o 0x1003ae58 por defecto 17/..)'),
('LeftStickDeadzone', 'PadN', 'float 0x1003ABD0[pad] = valor*0.01'),
('LeftStickSensitivity', 'PadN', 'float 0x1003AC8C[pad] = valor*0.01'),
('RightStickDeadzone', 'PadN', 'float 0x1003AB84[pad]'),
('RightStickSensitivity', 'PadN', 'float 0x1003ABC4[pad]'),
('SixaxisSensitivity', 'PadN', 'float 0x1003ACB8[pad] = valor*0.01'),
('SixaxisReloading / Hydraulics / CarSteering / BikeSteering / BoatSteering / HeliSteering / PlaneSteering', 'PadN', 'bytes 0x1003ACB4 / 0x1003ACA0 / 0x1003ACCC / 0x1003ACC0 / 0x1003ACAC / 0x1003AC9C / 0x1003AC98 [pad]'),
('EnableSixaxisHack', 'GInput', 'activa el parche de la DLL de SCP (0x1000A600)'),
('GuideLaunchesOverlay', 'GInput', 'byte del primer objeto de pad (+0)'),
]
for a, b, c in ini: w('| %s | %s | %s |' % (a, b, c))
w('')
w('Curva de los sticks (leida en 0x10006EE0 y 0x10007550): valor/32768 -> zona muerta `(|v|-dz)/(1-dz)` con signo -> curva: con sensibilidad s = 0x1003AC8C (o 0x1003ABC4 para el derecho), si 1 > s se usa una rama con `pow` (0x10017846) mezclada con el valor lineal, si no una rama polinomica `v*v*(…)` de tercer grado; despues inversion/permutacion de ejes por las banderas de arriba. Los coeficientes exactos no se transcribieron (dos ramas; hacen falta pruebas numericas para fijarlos). Constantes del .rdata: 0.01 (0x100347EC), 255 (0x10034834), 25500 (0x1003483C), 17.0 (0x10034820), 20/30/35/50 (0x10034824..30).\n')

# ---------- 4. scripts
w('## 4. Scripts del juego tratados (ApplyMissionSpecificFixes)\n')
w('GInput no parchea el SCM en memoria con tablas de bytes: sustituye `CRunningScript::GetPadState` (0x485B10) y varios comandos (0x390, 0x494, 0x293, y dos del rango 1900-1999) y comprueba el nombre del script (CRunningScript+8, `strnicmp` de 8 bytes). Clasificacion de GetPadState (0x10005B20; codigo en `bl`):\n')
w('| codigo | scripts |\n|---|---|')
w('| 1 | r3, bbchal, taxiodd, ambulan, firetru, copcar, burgjb, freight, pimp, bball, ryder2 |')
w('| 2 | dskool, bskool, zero2, zero4, desert5, casino1 |')
w('| 3 | casino9, ryder3 |')
w('| 4 | driv2 |')
w('| 5 | camera |')
w('| 6 | gfdate, heist1, gymls, gymsf, gymlv, bcesar2, cokec, lowrcon |')
w('| 7 | bcour |')
w('| (especial) | boat: comprueba [script+0x14] antes de clasificar (rama en 0x10005D28) |')
w('')
w('Otros scripts con tratamiento propio: `dance` (comandos 0x10006180/0x10006200 y GET_POSITION_OF_ANALOGUE_STICKS 0x100093F0), `lowgame` (0x100093F0), `bdisply` (LOAD_TEXTURE_DICTIONARY 0x100060F0, tabla de 4 sprites en 0x10033634). La comprobacion `ApplyMissionSpecificFixes` (0x1003AB5F) y el pad activo (objeto+0x7D) condicionan todos. Los codigos 1-7 se traducen a que boton de pad devuelve cada boton de script (0-20) en el cuerpo de 0x10005B20 (switch en 0x1000609C; lectura completa pendiente).\n')

# ---------- 5. iconos / texturas
w('## 5. Texturas y tokens de iconos\n')
def cs(va):
    try: return rd(va, 64).split(b'\0')[0].decode('latin1')
    except Exception: return ''
w('TXD que carga (0x10007E50): `models\\x360btns.txd`, `models\\ps3btns.txd`, `models\\sixaxis.txd` (y `pcbtns.txd` aparece como cadena). Nombres de textura referenciados (0x100335FC-0x10033760): ' + ', '.join(sorted({cs(v) for v in range(0x100335fc, 0x10033790, 4) if cs(v).isidentifier() and len(cs(v)) <= 12 and cs(v).islower()})) + '.')
w('Tokens que interpreta `ParseToken` (0x10009040): `~<~ ~>~ ~u~ ~d~ ~x~ ~o~ ~v~ ~[x~ ~[y~ ~[u~ ~[d~ ~<>~ ~ud~ ~@NOMBRE~` y el selector de sprite 1..4 de SIXAXIS (`~K~`, etc. segun la tabla de saltos 0x100092AC/0x1000930C/0x10009370/0x100093D4). El ancho del icono (tabla 0x1003ABF0, indexada por sprite 0..0x22) se pone en 0x1003B084 y sustituye a la constante 0x85F100 de `CFont::PrintChar`/`RenderFontBuffer`.\n')

# ---------- 6. trucos
hs = [u32(0x10032f00 + 4 * k) for k in range(0x5a)]
w('## 6. Trucos desde el pad\n')
w('Tabla de 90 hashes (CRC32 con la tabla estandar 0x10033100, de la cadena de letras T,C,X,S,U,D,L,R,1,2,3,4 en mayusculas; la cadena es la secuencia ya normalizada) en 0x10032F00; el orden es el de `CCheat::m_aCheatHashKeys` del juego (el indice se pasa a ApplyCheat). Texto de aviso: GXT `CHEAT1` (activado) y `CHEAT8` (desactivado) (0x10032DFC/0x10032E04).\n')
w('```\n' + ' '.join('%08X' % h for h in hs) + '\n```\n')

# ---------- 7. versiones
w('## 7. Versiones de ejecutable reconocidas (resolutor 0x10005210 y similares)\n')
w('Se comprueba un dword en el EXE cargado: VA 0x458D21 == 0x3539F633 -> tipo 3; VA 0x42457C == 0x94BF -> tipo 0 (1.0 US); 0x4245BC -> tipo 0 con bandera EU; 0x4252FC -> tipo 1 (1.01 US); 0x42533C -> tipo 1 (EU); 0x45EC4A -> tipo 2. `DllMain` solo instala los parches si es tipo 0 (US o EU 1.0): las direcciones fijas del instalador son las de 1.0 US; para EU solo cambian las dos de WinMain/WndProc (0x1000E230, 0x1000E290). El resto de tipos solo se usan para resolver punteros (funciones/datos) en las rutinas 0x1000xxxx.\n')
w('## 8. Punteros a funciones y datos del juego resueltos por GInput (inicializadores 0x10001000-0x10002000)\n')
w('| variable del .asi | resolutor / direccion US | uso |\n|---|---|---|')
ptr = [
('0x1003AD2C', 'CWeaponInfo::GetWeaponInfo 0x743C60', 'se llama desde 0x10004400/0x10003450 (informacion del arma)'),
('0x1003AD38', 'CCheat::ApplyCheat 0x438370', 'aplicar truco'),
('0x1003AD18', 'operando 0x53FB7B = CPad::Pads (0xB73458)', 'base de los CPad (indice = (pad-base)/0x134)'),
('0x1003AD14', 'CPad::Update 0x541C40', 'llamada original en 0x10004400'),
('0x1003AD08', 'CPad::UpdateMouse 0x53F3C0', 'llamada original en 0x100045B0'),
('0x1003AD20', 'CCheat::DoCheats 0x439AF0', 'llamada original en 0x10004950'),
('0x1003AD1C', 'CHud::SetHelpMessage 0x588BE0', 'mostrar CHEAT1/CHEAT8'),
('0x1003AD3C', 'CStats::IncrementStat 0x55C180', 'contar trucos usados'),
('0x1003AD10', 'operando 0x438481 = byte 0xB5F851', 'bandera leida por DoCheats'),
('0x1003AD34', 'operando 0x438532 = CCheat::m_aCheatsActive (0x969130)', 'estado de cada truco'),
('0x1003AD24', 'operando 0x438578 = 0x96918C (m_bHasPlayerCheated)', ''),
('0x1003AD30', 'operando 0x438539 = TheText (0xC1B340)', 'obtener textos GXT'),
('0x1003AD48', 'CText::Get 0x6A0050', 'texto original'),
('0x1003AD50', 'CMenuManager::MessageScreen 0x579330', ''),
('0x1003AE1C', 'operando 0x7190ED = 0xC71A54 (PS2Symbol)', 'tabla de simbolos de CFont'),
('0x1003AE14', 'operando 0x4242D1 = 0xB7CB84', 'temporizador (m_snTimeInMilliseconds / radio)'),
('0x1003AE18', 'operando 0x4054DC = FrontEndMenuManager (0xBA6748)', 'vibracion (+0x20)'),
('0x1003AE0C', 'operando 0x50A851 = 0xB6EC2E (m_bUseMouse3rdPerson)', 'bandera de raton 3a persona'),
('0x1003AE24', 'App::DoRWStuffEndOfFrame 0x53D840', ''),
('0x1003AE28', 'CWorld::FindPlayerPed 0x56E210', 'jugador'),
('0x1003AE04', 'CWorld::FindPlayerVehicle 0x56E0D0', 'vehiculo del jugador'),
('0x1003AD58', 'operando 0x52DE28 = 0xB733A8 (NewKeyState.esc)', 'menu: ESC'),
('0x1003AE00', 'operando 0x52E505 = 0xB73138 (OldKeyState.esc)', ''),
('0x1003AE10', 'CMenuManager::CheckForMenuClosing+0x3F', 'continuacion tras el hook 0x576B9F'),
('0x1003AE08', 'Input::HandleKeyDown 0x743DF0', ''),
('0x1003AD5C', 'CControllerConfigManager::GetIsKeyboardKeyJustDown 0x52E450', ''),
('0x1003AE2C', 'CPlayerPed::DrawTriangleForMouseRecruitPed 0x60BA80', ''),
('0x1003B080', 'CControllerConfigManager::GetGxtStringOfCommandKeys 0x5303D0', ''),
('0x1003ABE0', 'CFont::ParseToken 0x718F00', 'ParseToken original'),
('0x1003ABCC/ABE8/ACF8/AB9C/ABA0/ABE4/ACF4/ACFC', 'CTxdStore: AddTxdSlot 0x731C80, RemoveTxdSlot 0x731CD0, FindTxdSlot 0x731850, AddRef 0x731A00, LoadTxd 0x7320B0, PopCurrentTxd 0x7316B0, PushCurrentTxd 0x7316A0, SetCurrentTxd 0x7319C0', 'carga de TXD de botones'),
('0x1003AE44/AE48', 'CSprite2d::Delete 0x727240 / SetTexture 0x7272B0', ''),
('0x1003AE20', 'ScriptParams (0xA43C78)', 'devolver valores a scripts'),
]
for a, b, c in ptr: w('| %s | %s | %s |' % (a, b, c))
open('../ginput-tablas.md', 'w', encoding='utf-8').write('\n'.join(out) + '\n')
print(len(out))
