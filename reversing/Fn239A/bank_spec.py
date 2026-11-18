"""Especificacion unica del banco Fn239A (lote 239A: CRunningScript::ProcessCommands800To899, despachador del EXE para los comandos de script 801..899).

De aqui salen GenTables.hpp / GenStubs.hpp (tablas de colaboradores falsos, escena y elecciones para el banco C++, via gen_gen.py) y el oraculo (emulacion del codigo ORIGINAL del EXE con Unicorn, emu_oracle.py).
Los colaboradores falsos y la escena son identicos en el EXE, en la DLL y en el oraculo por construccion (los tres leen estas tablas).
"""
import struct

# ------------------------------------------------------------------------------------------------ funciones del lote
NAMES = ('CRunningScript::ProcessCommands800To899',)
ENTRIES = (0x481300,)
BASES = (801,)
NCMD = 99                      # comandos 801..899
OOR = (800, 900, 0x7FFFFFFF, 0xFFFFFFFF, 0x80000000)   # fuera de rango: el EXE rechaza con -1 (0xFF) sin leer parametros

# ------------------------------------------------------------------------------------------------ arenas (A, B, C, D) y ventanas
ARENA_NAMES = 'ABCD'
ARENA_SIZE = (0x4000, 0x5000, 0x1000, 0x1000)   # D: canario (no se toca)
GUARD = 0x40

def A(off): return ('A', off)
def B(off): return ('B', off)
def C(off): return ('C', off)

# --- arena A: guion, tabla virtual de entidades, textos, estructuras de colaboradores
SCRIPT = 0x100          # CRunningScript (0xE0)
VT_VEH = 0x300          # tabla virtual de entidades (128 ranuras): ranura 14 = Teleport
TEXT = 0x600            # 4 textos de 0x40 bytes (CText::Get)
GROUPOBJ = 0x700        # un grupo de peds (0x40)
COLM = 0x800            # 3 x 0x40 CColModel (caja a +0 min, +0xC max)
POOLS = 0xC80           # 3 x 0x20 CPool (ped, vehiculo, objeto)
SLOTS = 0xCE0           # estados de ranura (3 x 4 bytes)
FREE_A = 0x1000

# --- arena B: entidades y estructuras que cuelgan de ellas
PED_STRIDE, VEH_STRIDE, OBJ_STRIDE = 0x7C4, 0xA18, 0x19C
PED = 0x0               # 2 peds
VEH = 0x1000            # 2 vehiculos
OBJ = 0x3000            # 2 objetos
MAT = 0x3400            # 8 matrices CMatrix de 0x80: ped0, ped1, veh0, veh1, obj0, obj1, extra, extra
INTEL = 0x3A00          # 2 CPedIntelligence (0x100)
MINFO = 0x4000          # 3 CModelInfo (0x100)
FREE_B = 0x4400

# --- arena C: reservas de tareas (CTask::operator new): 14 ranuras de 0x100
SLOT_C = 0x100
NSLOT_C = 14

# ------------------------------------------------------------------------------------------------ memoria global del EXE que toca la funcion (direccion, bytes)
GLOBALS = [
    (0xA43C78, 0x80),   # ScriptParams
    (0xB74490, 0x10),   # punteros a pools: ped, vehiculo, edificio, objeto
    (0xB7CD74, 0x04),   # PlayerInFocus
    (0xB7CD98, 0x320),  # Players[2]
    (0xC0EC24, 0x04),   # ms_nTotalMissionPeds
    (0xA9B0C8, 0x20),   # ms_modelInfoPtrs[0..7]
    (0xB7CB84, 0x04),   # CTimer::m_snTimeInMilliseconds (lo lee el constructor de CEventDamage)
    (0x96C008, 0x04),   # CGarages::RespraysAreFree (byte 0x96C009)
    (0x8CC380, 0x04),   # gPlayerPedVisible
    (0xA44B68, 0x04),   # NumberOfIntroTextLinesThisFrame (u16)
    (0xA913E8, 0x120),  # IntroTextLines[0..3] (0x44 bytes cada una)
    (0xA44B88, 0xE0),   # UsedObjectArray[0..7] (0x1C bytes)
    (0xA449E0, 0x50),   # InvisibilitySettingArray (20 entradas)
    (0xB6F080, 0x04),   # byte 0xB6F081: camara activa
    (0xB6F1A8, 0x04),   # modo de la camara 0
    (0xB6F3E0, 0x04),   # modo de la camara 1
    (0xB6F618, 0x04),   # modo de la camara 2
]

# ------------------------------------------------------------------------------------------------ rellenos aleatorios por caso: (arena, desplazamiento, bytes[, 'f'])
FILLS = [
    ('A', TEXT, 0x100), ('A', COLM, 0xC0, 'f'), ('A', SCRIPT, 0xE0),
    ('B', PED, 2 * PED_STRIDE), ('B', VEH, 2 * VEH_STRIDE), ('B', OBJ, 2 * OBJ_STRIDE),
]
for _i in range(2):   # posiciones propias de las entidades (cuando m_matrix es nulo): floats sanos
    FILLS += [('B', PED + PED_STRIDE * _i + 4, 12, 'f'), ('B', VEH + VEH_STRIDE * _i + 4, 12, 'f'), ('B', OBJ + OBJ_STRIDE * _i + 4, 12, 'f')]
FILLS += [('B', MAT, 0x400, 'f'), ('B', MINFO, 0x300)]
FILLS += [('G', a, n) for a, n in GLOBALS]
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
P('G', 0xA43C78 + 4 * 31, 4, 0xBA9C0239)
# guion
P('A', SCRIPT + 0x10 + 0x04, 4, A(0x2F00))      # m_IP (valor numerico solo; no se desreferencia)
# tabla virtual de entidades: Teleport (ranura 14, +0x38)
P('A', VT_VEH + 4 * 14, 4, ('F', 'Teleport'))
for i in range(2):
    ped, veh, obj = ent_addr('ped', i)[1], ent_addr('veh', i)[1], ent_addr('obj', i)[1]
    P('B', ped + 0x00, 4, A(VT_VEH)); P('B', ped + 0x14, 4, B(MAT + 0x80 * i)); P('B', ped + 0x36, 1, 3)
    P('B', ped + 0x47C, 4, B(INTEL + 0x100 * i)); P('B', ped + 0x58C, 4, B(VEH + VEH_STRIDE * i))
    P('B', veh + 0x00, 4, A(VT_VEH)); P('B', veh + 0x14, 4, B(MAT + 0x80 * (2 + i))); P('B', veh + 0x36, 1, 2)
    P('B', obj + 0x00, 4, A(VT_VEH)); P('B', obj + 0x14, 4, B(MAT + 0x80 * (4 + i))); P('B', obj + 0x36, 1, 4)
    P('G', 0xB7CD98 + 0x190 * i + 0x00, 4, B(ped))
for q in range(8):
    P('G', 0xA9B0C8 + 4 * q, 4, B(MINFO + 0x100 * (q % 3)))
for k in range(3):
    P('B', MINFO + 0x100 * k + 0x14, 4, A(COLM + 0x40 * k))

# ------------------------------------------------------------------------------------------------ elecciones por caso (cada una se sortea independientemente): (arena|'G', desp, bytes, [valores], comando|0)
CHOICES = []
def CH(a, off, size, vals, cmd=0): CHOICES.append((a, off, size, vals, cmd))
SMALL = [0xFFFFFFFE, 0xFFFFFFFF, 0, 1, 2, 3, 0x7FFFFFFF, 0x80000000]
CH('G', 0xB7CD74, 4, [0, 1])
for i in range(2):
    ped, veh, obj = ent_addr('ped', i)[1], ent_addr('veh', i)[1], ent_addr('obj', i)[1]
    CH('B', ped + 0x14, 4, [B(MAT + 0x80 * i), B(MAT + 0x80 * i), 0])
    CH('B', veh + 0x14, 4, [B(MAT + 0x80 * (2 + i)), B(MAT + 0x80 * (2 + i)), 0])
    # (la matriz de un objeto no se anula: SLIDE_OBJECT con comprobacion de colision copia *m_matrix sin comprobar nulo)
    for e in (ped, veh, obj):
        CH('B', e + 0x2E, 1, [0, 0, 1, 7])
    CH('B', ped + 0x484, 1, [2, 2, 2, 2, 0, 1])          # creado por (2 = mision)
    CH('B', ped + 0x46D, 1, [0, 1, 0, 1, 0xFE])          # bit 8 de +0x46C: en vehiculo
    CH('B', ped + 0x58C, 4, [0, B(VEH), B(VEH + VEH_STRIDE)])
    CH('B', ped + 0x71C, 4, [0, B(OBJ), B(OBJ + OBJ_STRIDE)])
    CH('B', ped + 0x548, 4, [0x41200000, 0x42C80000, 0x0, 0xC1200000, 0x3F000000, 0x437F0000])
    CH('B', veh + 0x22, 2, [0, 1, 2, 3])
    CH('B', veh + 0x428, 1, [0, 0, 2, 4, 0, 1])
    CH('B', obj + 0x22, 2, [0, 1, 2, 3, 4, 5, 6, 7])
    for j in range(3):   # posiciones de los objetos: pocos valores para que ROTATE/SLIDE lleguen al destino
        CH('B', MAT + 0x80 * (4 + i) + 0x30 + 4 * j, 4, [0x00000000, 0x3F800000])
for k in range(3):
    CH('B', MINFO + 0x100 * k + 0x1C, 4, [0, 1, 0x1234])   # objeto RenderWare (nulo o no)
    CH('B', MINFO + 0x100 * k + 0x28, 4, [4, 5, 6, 3, 4])  # tipo de ped
for k in range(20):
    CH('G', 0xA449E0 + 4 * k, 4, [0, 0, B(OBJ), B(OBJ + OBJ_STRIDE), B(PED), B(VEH)])
CH('G', 0xA44B68, 4, [0, 1, 2, 3])
CH('G', 0xB6F080, 4, [0x000, 0x100, 0x200, 0x100])
for a in (0xB6F1A8, 0xB6F3E0, 0xB6F618):
    CH('G', a, 4, [4, 18, 0, 3, 4, 18])
CH('A', SCRIPT + 0xC6, 1, [0, 1])
CH('A', SLOTS + 4 * 1 + 0, 1, [0x03, 0x83], 807)   # GET_RANDOM_CAR: ranura de vehiculo libre (bit 7) o no
CH('A', SLOTS + 4 * 1 + 1, 1, [0x04, 0x84], 807)

# ------------------------------------------------------------------------------------------------ colaboradores
# conv: 'T' thiscall (ECX + n pila, el callee limpia), 'S' stdcall (n pila, el callee limpia), 'C' cdecl (n pila, limpia el llamador)
# args (una letra por dword de pila): w palabra, f float (bits), B byte bajo, H 16 bits bajos, v puntero a 3 floats (se registran), u puntero a 2 floats, s cadena C (FNV), x ignorado,
#   L destino de etiqueta (el falso escribe), o puntero de salida (el falso escribe un dword), b puntero de salida booleano (puede ser nulo), O/N retorno oculto (3 floats / 1 dword; EAX = puntero),
#   h puntero a int16 de salida (el falso escribe un contador), P lista de punteros de salida (el falso escribe tantos punteros a entidades como el contador anterior),
#   X puntero a 6 floats (caja), M puntero a CMatrix (se registran 12 floats)
# ret: v nada, w EAX = respuesta, b 0/1, f float en ST0, n entero pequeno, o un conjunto de punteros o enteros (ver PTRSETS), t entero de COSAS DE GUION, c EAX = ECX (this)
# mode: 'F' falso en ambos lados, 'R' real (gancho apagado en EXE y DLL; se ejecuta el codigo de cada lado)
COLLABS = []
def K(name, exe, conv, args, ret, mode='F'): COLLABS.append(dict(name=name, exe=exe, conv=conv, args=args, ret=ret, mode=mode))
K('CollectParameters', 0x464080, 'T', 'w', 'v')
K('StoreParameters', 0x464370, 'T', 'w', 'v')
K('UpdateCompareFlag', 0x4859D0, 'T', 'B', 'v')
K('ReadTextLabelFromScript', 0x463D50, 'T', 'LB', 'v')
K('CollectNextParameterWithoutIncreasingPC', 0x464250, 'T', '', 'w')
K('AddEntityToList', 0x4637E0, 'T', 'ww', 'v')
K('RemoveEntityFromList', 0x4654B0, 'T', 'ww', 'v')
K('FindGroundZForCoord', 0x569660, 'C', 'ff', 'f')
K('TextGet', 0x6A0050, 'T', 's', 'o:TEXT')
K('SetCharCreatedBy', 0x5E47E0, 'T', 'w', 'v')
K('FlushImmediately', 0x601640, 'T', 'B', 'v')
K('TaskManagerSetTask', 0x681AF0, 'T', 'wwB', 'v')
K('ClearSpaceForMissionEntity', 0x486B00, 'C', 'vw', 'v')
K('SetPosn', 0x4241C0, 'T', 'v', 'v')
K('CleanUpOldReference', 0x571A00, 'T', 'w', 'v')
K('TaskNew', 0x61A5A0, 'C', 'w', 'o:TASKMEM')
# 801
# constructores y destructores de objetos locales (CPedDamageResponseCalculator, CEventDamage, CTaskSimpleCarSetPedInAsDriver): el constructor de la DLL es un simbolo propio (no se puede falsear
# por direccion), asi que corren REALES en los dos lados (gancho apagado); lo que tocan fuera del objeto va por colaboradores falsos (RegisterReference, CleanUpOldReference)
K('RegisterReference', 0x571B70, 'T', 'w', 'v')
K('EventAffectsPed', 0x4B35A0, 'T', 'w', 'b')
K('ComputeDamageResponse', 0x4B5AC0, 'T', 'wwB', 'v')
K('EventGroupAdd', 0x4AB420, 'T', 'wB', 'v')
# 805 806
K('StartScriptFire', 0x53A270, 'T', 'vwfBBB', 'w')
# 807
K('GetVehicleAppearance', 0x6D1080, 'T', '', 'o:APP')
K('CanBeDeleted', 0x6D1180, 'T', '', 'o:YES')
K('IsWithinArea', 0x54F200, 'T', 'ffff', 'o:YES')
K('SetVehicleCreatedBy', 0x6D5D70, 'T', 'w', 'v')
# 810 883
K('SetZoomFollowPed', 0x50C160, 'T', 'H', 'v')
K('SetZoomCamString', 0x50C1B0, 'T', 'H', 'v')
K('SetCameraBehind', 0x50BD40, 'T', '', 'v')
# 811
K('GetActualPickupIndex', 0x4552A0, 'C', 'w', 'w')
K('GenerateNewOne', 0x456F20, 'C', 'fffwBwwBw', 'w')
# 823
K('FindPlayerPed', 0x56E210, 'C', 'w', 'o:PEDN')
# 825
K('FindObjectsIntersectingCube', 0x568DD0, 'C', 'vvhwwBBBBB', 'v')
# 845 846
K('GetHeading', 0x441DB0, 'T', '', 'f')
K('SetRotateZ', 0x59B0E0, 'T', 'f', 'v')
K('MatrixTimesVector', 0x59C890, 'C', 'OMv', 'v')
K('FindObjectsIntersectingAngledCollisionBox', 0x568FF0, 'C', 'XMvffffhwwBBBBB', 'v')
K('SetHeading', 0x43E0C0, 'T', 'f', 'v')
K('UpdateRwMatrix', 0x446F90, 'T', '', 'v')
K('UpdateRwFrame', 0x532B00, 'T', '', 'v')
K('Teleport', None, 'T', 'fffB', 'v')
# 847
K('RemoveThisPed', 0x486240, 'C', 'w', 'v')
K('GetPedsGroup', 0x5F7E80, 'C', 'w', 'o:GROUP')
K('IsFollower', 0x5F69E0, 'T', 'w', 'b')
K('RemoveMember', 0x5FB210, 'T', 'w', 'v')
K('RemoveReferencesToDeletedObject', 0x565510, 'C', 'w', 'v')
# 848
K('SetStayInSamePlace', 0x481090, 'T', 'B', 'v')
# 854
K('TestForExplosionInArea', 0x736950, 'C', 'wffffff', 'b')
# 860
K('PlacePhysicalRelativeToOtherPhysical', 0x546DB0, 'C', 'wwfff', 'v')
# 861
K('SetObjectTargettable', 0x59F300, 'T', 'B', 'v')
# 863
K('IsPlayer', 0x5DF8F0, 'T', '', 'b')
K('FindPlayerSlotWithPedPointer', 0x563FA0, 'C', 'w', 'o:SLOT')
# 864 865
K('GetGarageNumberByName', 0x447680, 'C', 's', 'o:GARAGE')
K('OpenThisGarage', 0x447D50, 'T', '', 'v')
K('CloseThisGarage', 0x447D70, 'T', '', 'v')
# 866
K('GetDistanceFromCentreOfMassToBaseOfModel', 0x536BE0, 'T', '', 'f')
# 867
K('FindObjectsOfTypeInRange', 0x564C70, 'C', 'wvfBhwPBBBBB', 'v')
K('FindLodOfTypeInRange', 0x564ED0, 'C', 'wvfBhwP', 'v')
K('AddToInvisibilitySwapArray', 0x481200, 'C', 'wB', 'v')
# 868 870
K('OurPedCanSeeThisEntity', 0x5E1660, 'T', 'wB', 'b')
K('IsInvisibleOrRenderDamaged', 0x46A2F0, 'T', '', 'b')
# 874
K('CarSetPedInAsDriverProcessPed', 0x64B950, 'T', 'w', 'b')
# 877
K('AddBigMessageWithNumber', 0x69E5F0, 'C', 'wwHwwwwww', 'v')   # el estilo es un eMessageStyle de 16 bits (el EXE lo lee con movzx word)
# 886
K('ChooseCivilianOccupation', 0x612F90, 'C', 'wwwwwwwww', 'o:MODEL')
K('PedOperatorNew', 0x5E4720, 'C', 'w', 'o:PEDMEM')
K('CivilianPedCtor', 0x5DDB70, 'T', 'ww', 'c')
K('SetOrientation', 0x439A80, 'T', 'fff', 'v')
K('WorldAdd', 0x563220, 'C', 'w', 'v')
# reales (gancho apagado; el EXE corre su codigo y la DLL el suyo)
for nm, ad in (('CVehiclePoolGetAtRef', 0x4048E0), ('CPedPoolGetAtRef', 0x404910), ('ObjectPoolGetAt', 0x465040), ('CVehiclePoolGetRef', 0x424160), ('CPedPoolGetRef', 0x4442D0),
               ('PedDamageCalcCtor', 0x4AD3F0), ('EventDamageCtor', 0x4AD830), ('EventDamageDtor', 0x4AD960), ('PedDamageCalcDtor', 0x4AD420), ('CarSetPedInAsDriverCtor', 0x6470E0), ('CarSetPedInAsDriverDtor', 0x647170),
               ('CtorCRGBA', 0x7170C0), ('SetVelocity', 0x441130), ('MatrixAssign', 0x59BBC0), ('MatrixDestructor', 0x59ACD0), ('CtorTaskSimple', 0x61A390), ('CtorStandStill', 0x62F310)):
    K(nm, ad, '?', '', 'v', 'R')

# conjuntos de punteros o enteros que devuelven los falsos (respuesta -> elemento); None = nulo
def ptrsets():
    ped = lambda i: B(PED + PED_STRIDE * i); veh = lambda i: B(VEH + VEH_STRIDE * i); obj = lambda i: B(OBJ + OBJ_STRIDE * i)
    return {
        'TEXT': [A(TEXT), A(TEXT + 0x40), A(TEXT + 0x80), A(TEXT + 0xC0)],
        'PEDN': [ped(0), ped(1)],
        'PEDMEM': [ped(0), ped(1)],
        'ENTL': [ped(0), ped(1), veh(0), veh(1), obj(0), obj(1)],
        'GROUP': [None, A(GROUPOBJ), A(GROUPOBJ), A(GROUPOBJ)],
        'MODEL': [0, 1, 2, 3, 4, 5, 6, 7],
        'SLOT': [0, 1, 0, 1, 1, 0, 0, 1],
        'GARAGE': [0xFFFFFFFF, 0, 1, 2, 63, 0xFFFF, 7, 3],
        'APP': [1, 2, 1, 2, 1, 2, 0, 5],
        'YES': [1, 1, 1, 1, 1, 1, 0, 1],
        'TASKMEM': ['TASK'],   # tratado aparte (reserva de la arena C)
    }
PTRSETS = ptrsets()

LABELS = ['', 'A', 'SHOP01', 'ABCDEFG', 'LBL_ONE', 'COMP', 'IPL_X', 'SEC_Y']
THINGS = [0xFFFFFFFF, 0, 1, 5, 63, 64, 100, 7, 0xFFFFFFFE]
ZONES = [0xFFFFFFFF, 0, 1, 2, 3, 0xFFFFFFFF, 1, 2]
NODES = [0xFFFFFFFF, 0x00010002, 0x00030004, 0xFFFFFFFF, 0x00000001, 0x00050006]
COUNTS = [0, 1, 2, 3, 4, 0xFFFF]   # contadores int16 de los falsos de busqueda en el mundo (0xFFFF = -1)
FLOATS = [0x00000000, 0x3F800000, 0xBF800000, 0x40490FDB, 0xC0490FDB, 0x42B40000, 0x43B40000, 0xC2C80000, 0x41200000, 0x3DCCCCCD, 0x461C4000, 0xC61C4000, 0x3F000000, 0x40000000, 0xC0000000, 0x3FC90FDB]

# ------------------------------------------------------------------------------------------------ tablas de parametros que se calculan aqui (el banco C++ las lee de GenTables.hpp)
def f32(x): return struct.unpack('<f', struct.pack('<f', x))[0]
def bits(x): return struct.unpack('<I', struct.pack('<f', x))[0]
def fl(b): return struct.unpack('<f', struct.pack('<I', b))[0]
def _headings():
    out = []
    for b in FLOATS:
        p = f32(fl(b) * 57.2957763671875)            # GetHeading * 57.2957763671875 (fmul de 24 bits = producto float)
        if p < 0.0:
            p = f32(p + 360.0)
        out.append(bits(p))
    return out
# angulos de destino de ROTATE_OBJECT: grados que el rumbo falso puede tener (para llegar a la igualdad), y valores sueltos
ANGLES = _headings()[:8] + [bits(f32(x)) for x in (0.0, 90.0, 180.0, 270.0, 360.0, 45.0, 0.1, 359.9)]
SPEEDS = [bits(f32(x)) for x in (0.0, 1.0, 5.0, 90.0, 0.5, 1000.0, 360.0, 20.0)]

# ------------------------------------------------------------------------------------------------ calendario de comandos: cada caso n usa SCHEDULE[n % len(SCHEDULE)]
# (los comandos con cadenas largas de colaboradores se repiten para que cada colaborador se llame muchas veces; el largo es impar y coprimo con las 16 familias)
WEIGHT = {801: 2, 874: 2, 807: 4, 810: 2, 811: 2, 825: 2, 845: 4, 846: 4, 847: 6, 861: 2, 863: 2, 867: 6, 886: 2}
SCHEDULE = [BASES[0] + k for k in range(NCMD)]
for _cmd, _w in sorted(WEIGHT.items()):
    SCHEDULE += [_cmd] * (_w - 1)
SCHEDULE += list(OOR)
