"""Especificacion unica del banco Fn237A (lote 237A: CRunningScript::ProcessCommands1200To1299 y ProcessCommands1900To1999).

De aqui salen Gen.hpp (tablas de colaboradores falsos, escena y elecciones para el banco C++, via gen_gen.py) y el oraculo (emulacion del codigo ORIGINAL del EXE con Unicorn, emu_oracle.py).
Los colaboradores falsos y la escena son identicos en el EXE, en la DLL y en el oraculo por construccion (los tres leen estas tablas).
"""
# ------------------------------------------------------------------------------------------------ funciones del lote
NAMES = ('CRunningScript::ProcessCommands1200To1299', 'CRunningScript::ProcessCommands1900To1999')
ENTRIES = (0x48B590, 0x46B460)
BASES = (1200, 1900)

# ------------------------------------------------------------------------------------------------ arenas (A, B, C, D) y ventanas
ARENA_NAMES = 'ABCD'
ARENA_SIZE = (0x4000, 0x9000, 0x1000, 0x1000)   # D: canario (no se toca)
GUARD = 0x40

def A(off): return ('A', off)
def B(off): return ('B', off)
def C(off): return ('C', off)

# --- arena A: guion, variables, tablas virtuales, estructuras de colaboradores
SCRIPT = 0x100          # CRunningScript (0xE0)
VARS = 0x200            # 16 dwords de variables de script
VT_VEH = 0x300          # tabla virtual de vehiculos (128 ranuras)
VT_MI = 0x500           # tabla virtual de CModelInfo (32 ranuras)
VT_TASK = 0x580         # tabla virtual de tareas (32 ranuras)
TEXT = 0x600            # 4 textos de 0x40 bytes (CText::Get)
BEAT = 0x700            # tBeatInfo (0xB0)
WINFO = 0x800           # 2 x 0x40 (CWeaponInfo)
PAD = 0x900             # 2 x 0x200? (solo se escribe un byte en +0x119): 2 x 0x140
STRS = 0xB80            # 2 x 0x20 cadenas (GetNameTag)
BLOCK = 0xBC0           # 2 x 0x20 CAnimBlock (+0x10 IsLoaded)
TAGOBJ = 0xC00          # 1 x 0x40 entidad "marca"
POOLS = 0xC80           # 3 x 0x20 CPool (ped, vehiculo, objeto)
SLOTS = 0xCE0           # estados de ranura (3 x 4 bytes)
RWMAT = 0xD00           # 2 x 0x40 RwMatrix para CQuaternion::Set
PATHS = 0xD80           # reserva
FREE_A = 0x1000

# --- arena B: entidades y estructuras que cuelgan de ellas
PED_STRIDE, VEH_STRIDE, OBJ_STRIDE = 0x7C4, 0xA18, 0x19C
PED = 0x0               # 2 peds
VEH = 0x1000            # 2 vehiculos
OBJ = 0x3000            # 2 objetos
MAT = 0x3400            # 8 matrices CMatrix de 0x80: ped0, ped1, veh0, veh1, obj0, obj1, tag, extra
HAND = 0x3800           # 2 tHandlingData (0x100)
INTEL = 0x3A00          # 2 CPedIntelligence (0x100)
PDATA = 0x3C00          # 2 CPlayerPedData (0x100)
CLOTH = 0x3E00          # 2 CPedClothesDesc (0x100)
MINFO = 0x4000          # 3 CModelInfo (0x100)
TASKS = 0x4400          # 2 tareas primarias (0x40) + 2 subtareas (0x40)
FREE_B = 0x4800

# --- arena C: reservas de tareas (CTask::operator new): 16 ranuras de 0x100
SLOT_C = 0x100
NSLOT_C = 14

# ------------------------------------------------------------------------------------------------ memoria global del EXE que toca la funcion (direccion, bytes)
GLOBALS = [
    (0xA43C78, 0x80),   # ScriptParams
    (0xB74490, 0x10),   # punteros a pools: ped, vehiculo, edificio, objeto
    (0xB7CD74, 0x04),   # PlayerInFocus
    (0xB7CD98, 0x320),  # Players[2]
    (0xB72914, 0x04),   # CGame::currArea
    (0xC1CC00, 0x04),   # bDisableRemoteDetonationOnContact (byte +1)
    (0x8A6458, 0x04),   # ms_compsToUse
    (0x8CDEE4, 0x04),   # MaximumWantedLevel
    (0x8D237C, 0x04),   # dword del 1953
    (0x8D37D4, 0x04),   # m_bWaterFogScript (byte 0x8D37D5)
    (0xB76898, 0x04),   # PlayerControlsCrane
    (0xBB4A70, 0x04),   # bArea51SamSiteDisabled (byte 0xBB4A72)
    (0xC0EC24, 0x04),   # ms_nTotalMissionPeds
    (0xA44488, 0x28),   # bAddNextMessageToPreviousBriefs (0xA4448B), bDisplayNonMiniGameHelpMessages (0xA444A7), bMiniGameInProgress (0xA444A8)
    (0xA9B0C8, 0x20),   # ms_modelInfoPtrs[0..7]
    (0xBA3778, 0xA0),   # NavigationZoneArray[-1..3]
    (0xBA1DE0, 0x80),   # ZoneInfoArray
    (0xB7CB84, 0x04),   # CTimer::m_snTimeInMilliseconds (lo lee el constructor de tareas con temporizador)
    (0xC1A7F0, 0x04),   # BriefMessages[0].Text
    (0xA9A7D8, 0x18),   # CShopping::ms_shopLoaded
]

# zonas: NavigationZoneArray en 0xBA3798, 0x20 bytes por zona, m_ZoneInfoIndex en +0x1C (16 bits)

# ------------------------------------------------------------------------------------------------ rellenos aleatorios por caso: (arena, desplazamiento, bytes)
FILLS = [
    ('A', VARS, 0x40), ('A', TEXT, 0x100), ('A', BEAT, 0xB0), ('A', WINFO, 0x80), ('A', PAD, 0x280), ('A', STRS, 0x40), ('A', BLOCK, 0x40), ('A', TAGOBJ, 0x40), ('A', RWMAT, 0x80, 'f'),
    ('A', SCRIPT, 0xE0),
    ('B', PED, 2 * PED_STRIDE), ('B', VEH, 2 * VEH_STRIDE), ('B', OBJ, 2 * OBJ_STRIDE), ('B', MAT, 0x400, 'f'), ('B', HAND, 0x200), ('B', INTEL, 0x200), ('B', PDATA, 0x200), ('B', CLOTH, 0x200),
    ('B', MINFO, 0x300), ('B', TASKS, 0x100),
    ('G', 0xA43C78, 0x80), ('G', 0xB74490, 0x10), ('G', 0xB7CD74, 4), ('G', 0xB7CD98, 0x320), ('G', 0xB72914, 4), ('G', 0xC1CC00, 4), ('G', 0x8A6458, 4), ('G', 0x8CDEE4, 4), ('G', 0x8D237C, 4),
    ('G', 0x8D37D4, 4), ('G', 0xB76898, 4), ('G', 0xBB4A70, 4), ('G', 0xC0EC24, 4), ('G', 0xA44488, 0x28), ('G', 0xA9B0C8, 0x20), ('G', 0xBA3778, 0xA0), ('G', 0xBA1DE0, 0x80), ('G', 0xC1A7F0, 4), ('G', 0xA9A7D8, 0x18),
]
# float-fill: conjunto de floats sanos para matrices (sin denormales ni infinitos: el EXE usa x87 y la DLL SSE con vaciado a cero)
FSET = [0x00000000, 0x3F800000, 0xBF800000, 0x3F000000, 0xBF000000, 0x3E800000, 0x40000000, 0xC0000000, 0x41200000, 0xC1200000, 0x42C80000, 0x3A83126F, 0xBA83126F, 0x3F333333, 0xBF333333, 0x3F59999A]
# La arena C (reservas de tareas) y la D (canario) se rellenan con ceros y 0xCD

def ent_addr(kind, i):
    return {'ped': ('B', PED + PED_STRIDE * i), 'veh': ('B', VEH + VEH_STRIDE * i), 'obj': ('B', OBJ + OBJ_STRIDE * i)}[kind]

# ------------------------------------------------------------------------------------------------ estructura constante (tras el relleno): (arena|'G', desplazamiento|direccion, bytes, valor)
# valor: entero, ('A', off) = direccion de la arena, o ('F', nombre) = direccion del falso virtual
STRUCT = []
def P(a, off, size, val): STRUCT.append((a, off, size, val))

POOL_STATE = {'ped': (0x05, 0x06), 'veh': (0x03, 0x04), 'obj': (0x09, 0x0A)}
POOL_ARRAYS = (('ped', 0, PED_STRIDE, PED), ('veh', 1, VEH_STRIDE, VEH), ('obj', 2, OBJ_STRIDE, OBJ))
for kind, k, stride, base in POOL_ARRAYS:
    pool = POOLS + 0x20 * k
    P('A', pool + 0x00, 4, B(base)); P('A', pool + 0x04, 4, A(SLOTS + 4 * k)); P('A', pool + 0x08, 4, 2); P('A', pool + 0x0C, 4, 0xFFFFFFFF); P('A', pool + 0x10, 4, 0)
    P('A', SLOTS + 4 * k + 0, 1, POOL_STATE[kind][0]); P('A', SLOTS + 4 * k + 1, 1, POOL_STATE[kind][1])
P('G', 0xB74490, 4, A(POOLS + 0x00)); P('G', 0xB74494, 4, A(POOLS + 0x20)); P('G', 0xB7449C, 4, A(POOLS + 0x40))
# marca del banco (la prueba negativa condiciona sus mutaciones a ella: el juego normal nunca deja ese valor en ScriptParams[31])
P('G', 0xA43C78 + 4 * 31, 4, 0xBA9C0237)
# guion
P('A', SCRIPT + 0x10 + 0x04, 4, A(0x2F00))      # m_IP (valor numerico solo; no se desreferencia)
P('A', SCRIPT + 0xC6, 1, 0);
# tablas virtuales: ranuras que llaman las funciones (se rellenan con los falsos virtuales)
P('A', VT_VEH + 4 * 44, 4, ('F', 'BurstTyre')); P('A', VT_VEH + 4 * 62, 4, ('F', 'BreakTowLink'))
P('A', VT_MI + 4 * 4, 4, ('F', 'GetModelType'))
P('A', VT_TASK + 4 * 2, 4, ('F', 'GetSubTask')); P('A', VT_TASK + 4 * 4, 4, ('F', 'GetTaskType'))
for i in range(2):
    ped, veh, obj = ent_addr('ped', i)[1], ent_addr('veh', i)[1], ent_addr('obj', i)[1]
    P('B', ped + 0x00, 4, A(VT_VEH)); P('B', ped + 0x14, 4, B(MAT + 0x80 * i)); P('B', ped + 0x36, 1, 3)
    P('B', ped + 0x47C, 4, B(INTEL + 0x100 * i)); P('B', ped + 0x480, 4, B(PDATA + 0x100 * i)); P('B', ped + 0x58C, 4, B(VEH + VEH_STRIDE * i))
    P('B', veh + 0x00, 4, A(VT_VEH)); P('B', veh + 0x14, 4, B(MAT + 0x80 * (2 + i))); P('B', veh + 0x36, 1, 2); P('B', veh + 0x384, 4, B(HAND + 0x100 * i))
    P('B', obj + 0x00, 4, A(VT_VEH)); P('B', obj + 0x14, 4, B(MAT + 0x80 * (4 + i))); P('B', obj + 0x36, 1, 4)
    P('B', INTEL + 0x100 * i + 0x10, 4, B(TASKS + 0x40 * i))
    P('B', TASKS + 0x40 * i, 4, A(VT_TASK)); P('B', TASKS + 0x80 + 0x40 * i, 4, A(VT_TASK))
    P('B', PDATA + 0x100 * i + 0x04, 4, B(CLOTH + 0x100 * i))
    P('G', 0xB7CD98 + 0x190 * i + 0x00, 4, B(ped)); P('G', 0xB7CD98 + 0x190 * i + 0x08, 4, B(CLOTH + 0x100 * i))
    P('B', MINFO + 0x100 * i + 0x00, 4, A(VT_MI))
    P('A', TAGOBJ + 0x00, 4, A(VT_VEH))
    P('A', PAD + 0x140 * i, 4, 0)
P('B', MINFO + 0x200, 4, A(VT_MI))

# ------------------------------------------------------------------------------------------------ elecciones por caso (cada una se sortea independientemente): (arena|'G', desp, bytes, [valores])
CHOICES = []
def CH(a, off, size, vals): CHOICES.append((a, off, size, vals))
SMALL = [0xFFFFFFFE, 0xFFFFFFFF, 0, 1, 2, 3, 0x7FFFFFFF, 0x80000000]
for i in range(4): CH('A', VARS + 4 * i, 4, SMALL)
CH('G', 0xB7CD74, 4, [0, 1])
for i in range(2):
    ped, veh, obj = ent_addr('ped', i)[1], ent_addr('veh', i)[1], ent_addr('obj', i)[1]
    CH('B', veh + 0x4C4, 4, [0, B(VEH), B(VEH + VEH_STRIDE)])
    CH('B', veh + 0x4C8, 4, [0, B(VEH), B(VEH + VEH_STRIDE)])
    CH('B', veh + 0x590, 4, [0, 9, 5, 2, 1, 0, 9])
    CH('B', veh + 0x3BA, 1, [0x39, 0x3A, 0x33, 0, 5])
    CH('B', veh + 0x41C, 4, [0, B(PED), B(VEH + VEH_STRIDE)])
    CH('B', HAND + 0x100 * i + 0xCC, 4, [0x2000000, 0, 0x2000001, 0xFFFFFFFF])
    CH('B', TASKS + 0x40 * i + 0x10, 4, [0, 1, 7, 0xFFFFFFFF, 63])
    CH('B', TASKS + 0x80 + 0x40 * i + 0x10, 4, [0, 3, 12])
    CH('B', PDATA + 0x100 * i + 0x38, 4, [0, 1, 7, 9])
    CH('B', MINFO + 0x100 * i + 0x24, 4, [0, 0x1000, 0x2000])
    CH('B', ped + 0x548, 4, [0x41200000, 0x42C80000, 0x0, 0xC1200000, 0x3F000000, 0x437F0000])
    CH('B', obj + 0xB9, 1, [0, 1, 2])
for q in range(4, 8): CH('G', 0xA9B0C8 + 4 * q, 4, [0, B(MINFO + 0x100 * (q % 3)), 0])
CH('G', 0xA9B0C8 + 0, 4, [0, B(MINFO)]); CH('G', 0xA9B0C8 + 4, 4, [0, B(MINFO + 0x100)]); CH('G', 0xA9B0C8 + 8, 4, [B(MINFO + 0x200), 0]); CH('G', 0xA9B0C8 + 12, 4, [B(MINFO), B(MINFO + 0x100)])
for z in range(-1, 4): CH('G', 0xBA3798 + 0x20 * z + 0x1C, 2, [0, 1, 2, 3])
CH('A', BEAT + 0xA0, 1, [0, 1, 1]); CH('A', BEAT + 0xA8, 4, [0, 1, 5, 9])
for k in range(20): CH('A', BEAT + 8 * k, 4, [0, 1, 0xFFFFFFFF, 5, 0xFFFFFFFB, 100, 0xFFFFFF9C])
CH('A', TAGOBJ + 0x14, 4, [0, B(MAT + 0x80 * 6)])
CH('G', 0xC1A7F0, 4, [0, 0x10000000])
CH('G', 0xB7CB84, 4, [0, 0x0001E240, 0x16134AED, 0xFFFFFF00, 0x7FFFFFF0, 0x00100000])   # el temporizador de juego (lo lee el constructor de tareas con temporizador)
CH('G', 0xA44488, 4, [0x01000000, 0x01000000, 0x01000000, 0x80]); CH('G', 0xA444A4, 4, [0, 0x01000000, 0x80000000, 0x01010000]); CH('G', 0xA444A8, 4, [0, 1])
CH('G', 0xC1CC00, 4, [0, 0x100, 0x10100])
CH('A', SCRIPT + 0xC6, 1, [0, 1]); CH('A', SCRIPT + 0xC8, 1, [0, 1])
for k in range(10): CH('B', CLOTH + 0x28 + 4 * k, 4, [0, 0xFFFFFFFF, 'KEY:100'])   # KEY:100 = clave de la primera etiqueta que devolvera ReadTextLabelFromScript
CH('G', 0xA9A7D8, 4, [0x706D6F63, 0x4C4F4842, 0x61626364, 0x0])
CH('A', STRS, 4, [0x706D6F63, 0x4C4F4842, 0x61626364, 0x0])
CH('A', STRS + 0x20, 4, [0x706D6F63, 0x4C4F4842, 0x61626364, 0x0])

# ------------------------------------------------------------------------------------------------ colaboradores
# conv: 'T' thiscall (ECX + n pila, el callee limpia), 'S' stdcall (n pila, el callee limpia), 'C' cdecl (n pila, limpia el llamador)
# args (una letra por dword de pila): w palabra, f float (bits), B byte bajo, H 16 bits bajos, v puntero a 3 floats (se registran), u puntero a 2 floats, s cadena C (FNV), x ignorado,
#   L destino de etiqueta (el falso escribe), o puntero de salida (el falso escribe un dword), b puntero de salida booleano (puede ser nulo), O/N retorno oculto (3 floats / 1 dword; EAX = puntero)
# ret: v nada, w EAX = respuesta, b 0/1, f float en ST0, n entero pequeno, o un conjunto de punteros (ver PTRSETS), t entero de COSAS DE GUION
# mode: 'F' falso en ambos lados, 'R' real (gancho apagado en EXE y DLL; se ejecuta el codigo de cada lado)
COLLABS = []
def K(name, exe, conv, args, ret, mode='F'): COLLABS.append(dict(name=name, exe=exe, conv=conv, args=args, ret=ret, mode=mode))
K('CollectParameters', 0x464080, 'T', 'w', 'v')
K('GetPointerToScriptVariable', 0x464790, 'T', 'w', 'o:VAR')
K('StoreParameters', 0x464370, 'T', 'w', 'v')
K('UpdateCompareFlag', 0x4859D0, 'T', 'B', 'v')
K('ReadTextLabelFromScript', 0x463D50, 'T', 'LB', 'v')
K('CollectNextParameterWithoutIncreasingPC', 0x464250, 'T', '', 'w')
K('GetIndexOfGlobalVariable', 0x464700, 'T', '', 'w')
K('GivePedScriptedTask', 0x465C20, 'T', 'www', 'v')
K('LocateObjectCommand', 0x487D10, 'T', 'w', 'v')
K('ObjectInAreaCheckCommand', 0x489150, 'T', 'w', 'v')
K('RequestModel', 0x4087E0, 'C', 'ww', 'v')
K('RemoveBuildingsNotInArea', 0x4094B0, 'C', 'w', 'v')
K('SetMissionDoesntRequireAnim', 0x48B570, 'C', 'w', 'v')
K('RequestCollision', 0x410C00, 'C', 'uw', 'v')
K('FindNodePairClosestToCoors', 0x44FEE0, 'T', 'fffwoogffwww', 'v')
K('FindNodeCoorsForScript', 0x4505E0, 'T', 'Owb', 'v')
K('FindNthNodeClosestToCoors', 0x44F8C0, 'T', 'Nfffwwwwwwww', 'v')
K('ModelForWeapon', 0x454AC0, 'C', 'w', 'w')
K('RegisterScriptRoadBlock', 0x460DF0, 'C', 'ffffffB', 'v')
K('ClearScriptRoadBlocks', 0x460EC0, 'C', '', 'v')
K('AddEntityToList', 0x4637E0, 'T', 'ww', 'v')
K('FindGroundZForCoord', 0x569660, 'C', 'ff', 'f')
K('GetActualBlipArrayIndex', 0x582870, 'C', 'w', 'w')
K('SetShortRangeCoordBlip', 0x583920, 'C', 'wfffwww', 'w')
K('SetBlipSprite', 0x583D70, 'C', 'wB', 'v')   # el EXE guarda el sprite en un byte
K('SetBlipAlwaysDisplayInZoom', 0x583DB0, 'C', 'wB', 'v')
K('SetHelpMessage', 0x588BE0, 'C', 'wBBB', 'v')
K('SetHelpMessageWithNumber', 0x588E30, 'C', 'wwBB', 'v')
K('AddToPreviousBriefArray', 0x69DD50, 'C', 'wwwwwwww', 'v')
K('TextGet', 0x6A0050, 'T', 's', 'o:TEXT')
K('GetAnimationBlockIndex', 0x4D3990, 'C', 's', 'n')
K('GetAnimationBlock', 0x4D3940, 'C', 's', 'o:BLOCK')
K('AddToResourceManager', 0x4704B0, 'T', 'www', 'v')
K('RemoveFromResourceManager', 0x470510, 'T', 'www', 'b')
K('AddCounter', 0x44CDA0, 'T', 'wHsH', 'v')
K('SetCJMood', 0x4E3ED0, 'C', 'Hwwww', 'v')
K('AddOne', 0x499500, 'C', 'Bffffffffffff', 'v')
K('StartExtraColour', 0x55FEC0, 'C', 'wB', 'v')
K('StopExtraColour', 0x55FF20, 'C', 'B', 'v')
K('BlowUpRCBuggy', 0x56EA30, 'T', 'B', 'v')
K('SetHeliOrientation', 0x6A2450, 'T', 'f', 'v')
K('ClearHeliOrientation', 0x6A2460, 'T', '', 'v')
K('TellPlaneToGoToCoors', 0x6A2470, 'T', 'fffff', 'v')
K('PopBoot', 0x6AF910, 'T', '', 'v')
K('CloseAllDoors', 0x6A4520, 'T', '', 'v')
K('GetCarPitch', 0x6A6050, 'T', '', 'f')
K('GetVehicleAppearance', 0x6D1080, 'T', '', 'n')
K('InitWinch', 0x6D3B60, 'T', 'w', 'v')
K('ReleasePickedUpEntityWithWinch', 0x6D3CB0, 'T', '', 'v')
K('QueryPickedUpEntityWithWinch', 0x6D3CF0, 'T', '', 'o:ENT')
K('RemoveOneMissionTrain', 0x6F5DC0, 'C', 'w', 'v')
K('ReleaseOneMissionTrain', 0x6F5DF0, 'C', 'w', 'v')
K('FindCarriage', 0x6F5EB0, 'C', 'wB', 'o:VEHN')
K('SetNewTrainPosition', 0x6F7140, 'C', 'wfff', 'v')
K('SetCharCreatedBy', 0x5E47E0, 'T', 'w', 'v')
K('AttachPedToEntity', 0x5E7CB0, 'T', 'wfffHfw', 'v')
K('SetPedDecisionMakerType', 0x600B50, 'T', 'w', 'v')
K('FlushImmediately', 0x601640, 'T', 'B', 'v')
K('SetDefaultTaskAllocatorType', 0x5FBB70, 'T', 'w', 'v')
K('GetVacantSlot', 0x608540, 'C', '', 'n')
K('GetStatus', 0x608710, 'C', 'ww', 'n')
K('ScriptedTaskSet', 0x6083E0, 'T', 'www', 'v')
K('GetPedFlag', 0x608830, 'C', 'w', 'w')
K('ClearAsAcquaintance', 0x608980, 'T', 'ww', 'v')
K('ClearAcquaintances', 0x6089A0, 'T', 'w', 'v')
K('SetAsAcquaintance', 0x608DA0, 'T', 'ww', 'v')
K('AddPed', 0x612710, 'C', 'wwvB', 'o:PEDN')
K('TaskManagerSetTask', 0x681AF0, 'T', 'wwB', 'v')
K('ClearSpaceForMissionEntity', 0x486B00, 'C', 'xw', 'v')
K('CreateRopeForSwatPed', 0x558D10, 'C', 'v', 'w')
K('GetNameTag', 0x49ADA0, 'C', 'w', 'o:STR')
K('GetExtraInfo', 0x49ADE0, 'C', 'ww', 'w')
K('StoreClothesState', 0x49B200, 'C', '', 'v')
K('RestoreClothesState', 0x49B240, 'C', '', 'v')
K('Buy', 0x49BF70, 'C', 'ww', 'v')
K('GetNearestTag', 0x49D160, 'C', 'v', 'o:TAG')
K('GetNumFiresInArea', 0x539860, 'T', 'ffffff', 'w')
K('GetActualScriptThingIndex', 0x4839A0, 'C', 'ww', 't')
K('GetUniqueScriptThingIndex', 0x4810C0, 'C', 'ww', 'w')
K('IsEntityWithinAnySearchLight', 0x493960, 'C', 'wi', 'b')
K('GetWeaponInfo', 0x743C60, 'C', 'ww', 'o:WINFO')
K('GetBeatInfo', 0x5071B0, 'T', '', 'o:BEAT')
K('GetPad', 0x53FB70, 'C', 'w', 'o:PAD')
K('RequestRecordingFile', 0x45A020, 'C', 'w', 'v')
K('HasRecordingFileBeenLoaded', 0x45A060, 'C', 'w', 'b')
K('FindIplSlot', 0x404AC0, 'C', 's', 'n')
K('RequestIplAndIgnore', 0x405850, 'C', 'w', 'v')
K('RemoveIplAndIgnore', 0x405890, 'C', 'w', 'v')
K('RemoveIplWhenFarAway', 0x4058D0, 'C', 'w', 'v')
K('FindZoneByLabel', 0x572C40, 'C', 'sw', 'z')
K('CleanUpOldReference', 0x571A00, 'T', 'w', 'v')
K('RegisterReference', 0x571B70, 'T', 'w', 'v')
K('GetRopeHeight', 0x59F380, 'T', '', 'f')
K('SetRopeHeight', 0x59F3A0, 'T', 'f', 'v')
K('GetObjectCarriedWithRope', 0x59F3C0, 'T', '', 'o:ENT')
K('ReleaseObjectCarriedWithRope', 0x59F3E0, 'T', '', 'v')
K('SetTextureAndModel', 0x5A8050, 'T', 'www', 'v')
K('GetModellingMatrix', 0x46A2D0, 'T', '', 'o:RWMAT')
K('SetMatrix', 0x54F610, 'T', 'm', 'v')
K('SetPosn', 0x4241C0, 'T', 'v', 'v')
K('RegisterCorona', 0x6FC580, 'C', 'wwBBBBvffwwBBwfBfBfBB', 'v')
K('TaskNew', 0x61A5A0, 'C', 'w', 'o:TASKMEM')
# virtuales (no tienen direccion de entrada: se instalan en las tablas virtuales de la escena)
K('BurstTyre', None, 'T', 'BB', 'v')
K('BreakTowLink', None, 'T', '', 'v')
K('GetModelType', None, 'T', '', 'o:MT')
K('GetSubTask', None, 'T', '', 'o:SUBT')
K('GetTaskType', None, 'T', '', 'w')
# reales (gancho apagado; el EXE corre su codigo y la DLL el suyo)
for nm, ad in (('CVehiclePoolGetAtRef', 0x4048E0), ('CPedPoolGetAtRef', 0x404910), ('CVehiclePoolGetRef', 0x424160), ('CPedPoolGetRef', 0x4442D0), ('ObjectPoolGetAt', 0x465040), ('ObjectPoolGetRef', 0x465070),
               ('Magnitude', 0x4082C0), ('VectorTimesFloat', 0x40FEC0), ('VectorPlusEq', 0x411A00), ('Multiply3x3Exe', 0x59C790),
               ('QuaternionGet', 0x59C080), ('QuaternionSet', 0x59C3E0), ('MatrixConstructAttached', 0x59C050), ('MatrixDestructor', 0x59ACD0),
               ('GetUppercaseKey', 0x53CF30), ('MakeUpperCase', 0x718710),
               ('CtorDuckToggle', 0x62F690), ('CtorStandStill', 0x62F310), ('CtorSwatRope', 0x659410), ('CtorClimb', 0x46A630), ('CtorSeekEntityStandard', 0x46AC10),
               ('CtorUseSequence', 0x635450), ('CtorSeekEntityAiming', 0x694B90), ('CtorKillPedOnFoot', 0x620E30), ('CtorJetPack', 0x67B4E0), ('CtorSetCharDecisionMaker', 0x46A470),
               ('CtorGoPickUpEntity', 0x6919C0), ('CtorSequence', 0x632BD0), ('SequenceAddTask', 0x632D10), ('CtorGoToPointAndStandStill', 0x668120), ('CtorSlideToCoord', 0x66C3E0),
               ('VectorsDiffer', 0x509760), ('MatrixAttach', 0x59BD10), ('CtorTaskSimple', 0x61A390), ('CtorTaskComplex', 0x61A3B0), ('CtorTaskSimpleAnim', 0x61A6C0), ('CtorTaskComplexJump', 0x67A030)):
    K(nm, ad, '?', '', 'v', 'R')

# conjuntos de punteros que devuelven los falsos (respuesta -> elemento); None = nulo
def ptrsets():
    ped = lambda i: B(PED + PED_STRIDE * i); veh = lambda i: B(VEH + VEH_STRIDE * i); obj = lambda i: B(OBJ + OBJ_STRIDE * i)
    return {
        'VAR': [A(VARS), A(VARS + 4), A(VARS + 8), A(VARS + 12)],
        'TEXT': [A(TEXT), A(TEXT + 0x40), A(TEXT + 0x80), A(TEXT + 0xC0)],
        'BLOCK': [A(BLOCK), A(BLOCK + 0x20)],
        'ENT': [None, ped(0), ped(1), veh(0), veh(1), obj(0), obj(1), None],
        'VEHN': [None, veh(0), veh(1), None],
        'PEDN': [ped(0), ped(1)],
        'STR': [A(STRS), A(STRS + 0x20)],
        'TAG': [None, A(TAGOBJ), A(TAGOBJ)],
        'WINFO': [A(WINFO), A(WINFO + 0x40)],
        'BEAT': [A(BEAT)],
        'PAD': [A(PAD), A(PAD + 0x140)],
        'RWMAT': [A(RWMAT), A(RWMAT + 0x40)],
        'SUBT': [None, B(TASKS + 0x80), B(TASKS + 0xC0)],
        'MT': [6, 6, 1, 0, 2, 6],
        'TASKMEM': ['TASK'],   # tratado aparte (reserva de la arena C)
    }
PTRSETS = ptrsets()

LABELS = ['', 'A', 'SHOP01', 'ABCDEFG', 'LBL_ONE', 'COMP', 'IPL_X', 'SEC_Y']
THINGS = [0xFFFFFFFF, 0, 1, 5, 63, 64, 100, 7, 0xFFFFFFFE]
ZONES = [0xFFFFFFFF, 0, 1, 2, 3, 0xFFFFFFFF, 1, 2]
NODES = [0xFFFFFFFF, 0x00010002, 0x00030004, 0xFFFFFFFF, 0x00000001, 0x00050006]   # area (16) | nodo (16) << 16
FLOATS = [0x00000000, 0x3F800000, 0xBF800000, 0x40490FDB, 0xC0490FDB, 0x42B40000, 0x43B40000, 0xC2C80000, 0x41200000, 0x3DCCCCCD, 0x461C4000, 0xC61C4000, 0x7F7FFFFF, 0x3F000000]
