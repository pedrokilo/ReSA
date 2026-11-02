"""Elevador de CCarCtrl::GenerateOneRandomCar (0x430050..0x431F3A) a C++ sin ensamblador.

Lee el desensamblado limpio (ins.txt, salida de dumpbin) y las tablas de salto del EXE, sigue estaticamente ESP y la profundidad de la pila x87 de cada instruccion
y emite una funcion C++ con `goto`, registros enteros como variables locales, un marco de pila `F` (para que los punteros a locales sean validos) y la pila x87 como
variables double (cada operacion pasa por los ayudantes Veh83D* de X87Math: mantisa de 24 bits, exponente ampliado).

Uso: python lift.py analyze      (analisis de pila y llamadas, sin emitir)
     python lift.py emit out.inl
"""
import re
import struct
import sys
from collections import OrderedDict

HERE = __import__('os').path.dirname(__import__('os').path.abspath(__file__))
EXE = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'

# ------------------------------------------------------------------------------------------------ lectura del EXE (tablas de salto)
_pe = None


def pe():
    global _pe
    if _pe is None:
        data = open(EXE, 'rb').read()
        e_lfanew = struct.unpack_from('<I', data, 0x3C)[0]
        nsec = struct.unpack_from('<H', data, e_lfanew + 6)[0]
        optsz = struct.unpack_from('<H', data, e_lfanew + 20)[0]
        base = struct.unpack_from('<I', data, e_lfanew + 24 + 28)[0]
        secs = []
        off = e_lfanew + 24 + optsz
        for i in range(nsec):
            name, vsz, va, rsz, roff = struct.unpack_from('<8sIIII', data, off + 40 * i)
            secs.append((va, max(vsz, rsz), roff))
        _pe = (data, base, secs)
    return _pe


def read_va(va, n):
    data, base, secs = pe()
    rva = va - base
    for sva, ssz, roff in secs:
        if sva <= rva < sva + ssz:
            return data[roff + rva - sva: roff + rva - sva + n]
    raise ValueError(hex(va))


# ------------------------------------------------------------------------------------------------ analisis sintactico
REG32 = ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi']
REG16 = {'ax': 'eax', 'cx': 'ecx', 'dx': 'edx', 'bx': 'ebx', 'bp': 'ebp', 'si': 'esi', 'di': 'edi'}
REG8L = {'al': 'eax', 'cl': 'ecx', 'dl': 'edx', 'bl': 'ebx'}
REG8H = {'ah': 'eax', 'ch': 'ecx', 'dh': 'edx', 'bh': 'ebx'}


def num(s):
    s = s.strip()
    if s.endswith('h'):
        return int(s[:-1], 16)
    return int(s, 10)


class Op:
    def __init__(self, text):
        self.text = text.strip()
        t = self.text
        self.kind = None
        m = re.match(r'^(byte|word|dword|qword) ptr (ds:)?\[(.*)\]$', t)
        m2 = re.match(r'^(ds:)?\[(.*)\]$', t)
        if m:
            self.kind = 'mem'
            self.size = {'byte': 1, 'word': 2, 'dword': 4, 'qword': 8}[m.group(1)]
            self.parse_addr(m.group(3))
        elif m2:
            self.kind = 'mem'
            self.size = 4
            self.parse_addr(m2.group(2))
        elif t in REG32 or t in REG16 or t in REG8L or t in REG8H:
            self.kind = 'reg'
            self.reg = t
        elif re.match(r'^st(\(\d\))?$', t):
            self.kind = 'st'
            self.idx = int(t[3]) if len(t) > 2 else 0
        elif re.match(r'^-?[0-9A-F]+h?$', t):
            self.kind = 'imm'
            self.val = num(t) & 0xFFFFFFFF
        else:
            raise ValueError('operando: ' + t)

    def parse_addr(self, e):
        self.base = None
        self.index = None
        self.scale = 1
        self.disp = 0
        for part in re.split(r'(?=[+-])', e):
            if not part:
                continue
            sign = -1 if part[0] == '-' else 1
            p = part.lstrip('+-')
            if '*' in p:
                r, s = p.split('*')
                self.index = r
                self.scale = int(s)
            elif p in REG32:
                if self.base is None:
                    self.base = p
                else:
                    self.index = p
            else:
                self.disp += sign * num(p)
        self.disp &= 0xFFFFFFFF


class Ins:
    def __init__(self, addr, text):
        self.addr = addr
        self.text = text
        parts = text.split(' ', 1)
        self.mn = parts[0]
        self.ops = []
        self.target = None
        self.indirect = None
        rest = parts[1] if len(parts) > 1 else ''
        if self.mn in ('call', 'jmp') or self.mn.startswith('j'):
            if re.match(r'^[0-9A-F]{8}$', rest):
                self.target = int(rest, 16)
            else:
                self.indirect = Op(rest)
        elif rest:
            self.ops = [Op(x) for x in re.split(r',(?![^\[]*\])', rest)]


def load():
    L = [l.rstrip('\n') for l in open(HERE + '/ins.txt')]
    ins = []
    for l in L:
        a, r = l.split(': ', 1)
        ins.append(Ins(int(a, 16), r))
    return ins


# ------------------------------------------------------------------------------------------------ tabla de llamadas
# direccion: (nombre, convencion, argumentos en pila (dwords), dwords que limpia el llamado, retorno)
# convencion: 'c' cdecl, 't' thiscall (ECX = this). retorno: 'v' nada, 'i' EAX, 'f' ST0 (float), 'b' AL (bool)
CALLS = {
    0x407180: ('GetRandomNumberInRange_int', 'c', 2, 0, 'i'),
    0x4082C0: ('NormaliseAndMag', 't', 0, 0, 'f'),
    0x4087E0: ('RequestModel', 'c', 2, 0, 'v'),
    0x40BA70: ('PossiblyStreamCarOutAfterCreation', 'c', 1, 0, 'v'),
    0x40FE30: ('VectorAdd', 'c', 3, 0, 'i'),
    0x40FE60: ('VectorSub', 'c', 3, 0, 'i'),
    0x40FEC0: ('VectorScale', 'c', 3, 0, 'i'),
    0x4119D0: ('Fn4119D0', 'c', 3, 0, 'i'),
    0x41BD90: ('GetRandomNumberInRange_float', 'c', 2, 0, 'f'),
    0x41BE60: ('GetWantedLevel', 't', 0, 0, 'i'),
    0x41C070: ('AddPoliceCarOccupants', 'c', 2, 0, 'v'),
    0x41C9D0: ('FindPoliceCarSpeedForWantedLevel', 'c', 0, 0, 'b'),
    0x41CA40: ('FindPoliceBoatSpeedForWantedLevel', 'c', 0, 0, 'b'),
    0x41CA50: ('FindPoliceCarMaxSpeed', 'c', 0, 0, 'b'),
    0x41CAA0: ('FindPoliceCarMaxSpeedFor', 'c', 1, 0, 'b'),
    0x41CCB0: ('Fn41CCB0', 't', 1, 1, 'i'),
    0x420800: ('Fn420800', 'c', 2, 0, 'f'),
    0x420920: ('Fn420920', 't', 1, 1, 'b'),
    0x420950: ('Fn420950', 't', 1, 1, 'b'),
    0x420980: ('Fn420980', 't', 1, 1, 'b'),
    0x420A10: ('CPathNode_GetPosition', 't', 1, 1, 'i'),
    0x420A60: ('Fn420A60', 't', 1, 1, 'i'),
    0x421120: ('Fn421120', 't', 0, 0, 'v'),
    0x421440: ('GetNewVehicleDependingOnCarModel', 'c', 2, 0, 'i'),
    0x4217C0: ('SetUpDriverAndPassengersForVehicle', 'c', 6, 0, 'v'),
    0x421980: ('ChoosePoliceCarModel', 'c', 1, 0, 'i'),
    0x424130: ('FindSpeedMultiplierWithSpeedFromNodes', 'c', 1, 0, 'f'),
    0x4241C0: ('SetPosn', 't', 1, 1, 'v'),
    0x424210: ('GenerateCarCreationCoors2', 'c', 15, 0, 'b'),
    0x424CE0: ('ChooseModel', 'c', 1, 0, 'i'),
    0x42C2B0: ('CreatePoliceChase', 'c', 3, 0, 'b'),
    0x42C740: ('CreateConvoy', 'c', 2, 0, 'b'),
    0x42DE80: ('PickNextNodeRandomly', 'c', 1, 0, 'v'),
    0x43C710: ('CalcSpeedScaleFactor', 'c', 6, 0, 'f'),
    0x43C900: ('CalcCurvePoint', 'c', 8, 0, 'v'),
    0x441C10: ('LaRiotsActiveHere', 'c', 0, 0, 'b'),
    0x443AC0: ('GangWarFightingGoingOn', 'c', 0, 0, 'b'),
    0x44DB00: ('Fn44DB00', 't', 0, 0, 'f'),
    0x4C5A70: ('IsBoatModel', 'c', 1, 0, 'b'),
    0x4C8500: ('ChooseVehicleColour', 't', 5, 5, 'v'),
    0x534540: ('GetIsOnScreen', 't', 0, 0, 'b'),
    0x563220: ('CWorldAdd', 'c', 1, 0, 'v'),
    0x5674E0: ('ProcessVerticalLine', 'c', 11, 0, 'b'),
    0x568B80: ('FindObjectsKindaColliding', 'c', 11, 0, 'v'),
    0x56E090: ('FindPlayerCoors', 'c', 1, 0, 'i'),
    0x56E0D0: ('FindPlayerVehicle', 'c', 2, 0, 'i'),
    0x56E210: ('FindPlayerPed_a', 'c', 1, 0, 'i'),
    0x56E230: ('FindPlayerWanted', 'c', 1, 0, 'i'),
    0x56E250: ('FindPlayerCentreOfWorld', 'c', 1, 0, 'i'),
    0x572400: ('GetZoneInfo', 'c', 2, 0, 'i'),
    0x59C910: ('MatrixSetIdentity', 't', 0, 0, 'v'),
    0x611B60: ('FindCarMultiplierMotorway', 'c', 0, 0, 'f'),
    0x611E90: ('PickLeastUsedModel', 't', 1, 1, 'i'),
    0x6A2530: ('Fn6A2530', 't', 1, 1, 'v'),
    0x6D1080: ('GetVehicleAppearance', 't', 0, 0, 'i'),
    0x6D2330: ('ChangeLawEnforcerState', 't', 1, 1, 'v'),
    0x6E3290: ('AddVehicleUpgrade', 't', 1, 1, 'v'),
    0x6EB690: ('GetWaterLevel', 'c', 6, 0, 'b'),
    0x72DD80: ('FewerCars', 'c', 0, 0, 'b'),
    0x732B00: ('SetClumpAlpha', 'c', 2, 0, 'v'),
    0x821B1E: ('rand', 'c', 0, 0, 'i'),
    0x821B40: ('ftol', 'c', 0, 0, 'i'),
}


# ------------------------------------------------------------------------------------------------ analisis de flujo: ESP y pila x87
JUMP_TABLES = {0x431F3C: 4, 0x431F4C: 4, 0x431F5C: 3}


def table_targets(base):
    n = JUMP_TABLES[base]
    return [struct.unpack('<I', read_va(base + 4 * i, 4))[0] for i in range(n)]


def x87_effect(i):
    """(delta de profundidad) de una instruccion x87"""
    mn = i.mn
    ops = i.ops
    if mn in ('fld', 'fild'):
        return +1
    if mn in ('fstp', 'fcomp', 'faddp', 'fsubp', 'fmulp', 'fdivp', 'fsubrp', 'fdivrp'):
        return -1
    if mn == 'fcompp':
        return -2
    return 0


def analyze(ins):
    by_addr = OrderedDict((i.addr, k) for k, i in enumerate(ins))
    n = len(ins)
    delta = [None] * n
    xd = [None] * n
    labels = set()
    work = [(0, 0, 0)]
    problems = []
    def visit(k, d, x):
        if delta[k] is None:
            delta[k] = d
            xd[k] = x
            work.append((k, d, x))
        else:
            if delta[k] != d:
                problems.append('ESP inconsistente en %08X: %d vs %d' % (ins[k].addr, delta[k], d))
            if xd[k] != x:
                problems.append('x87 inconsistente en %08X: %d vs %d' % (ins[k].addr, xd[k], x))
    delta[0] = 0
    xd[0] = 0
    while work:
        k, d, x = work.pop()
        i = ins[k]
        mn = i.mn
        nd = d
        nx = x + x87_effect(i)
        succ = []
        if mn == 'push':
            nd -= 4
        elif mn == 'pop':
            nd += 4
        elif mn == 'sub' and i.ops[0].kind == 'reg' and i.ops[0].reg == 'esp':
            nd -= i.ops[1].val
        elif mn == 'add' and i.ops[0].kind == 'reg' and i.ops[0].reg == 'esp':
            nd += i.ops[1].val
        elif mn == 'call':
            if i.target is not None:
                info = CALLS.get(i.target)
                if info is None:
                    problems.append('llamada sin entrada: %08X en %08X' % (i.target, i.addr))
                else:
                    nd += 4 * info[3]
                    if info[4] == 'f':
                        nx += 1
                    if i.target == 0x821B40:
                        nx -= 1
            else:
                # llamada virtual: this en ECX, sin argumentos propios salvo el `push 1` del destructor (limpia el llamado)
                nd += 4 if i.addr in (0x431AAB, 0x431B33, 0x431F2E) else 0
                if i.indirect.disp == 0xD4:
                    nx += 1
        if mn == 'ret':
            if d != 0:
                problems.append('ret con ESP %d en %08X' % (d, i.addr))
            if x != 0:
                problems.append('ret con x87 %d en %08X' % (x, i.addr))
            continue
        if mn == 'jmp':
            if i.target is not None:
                visit(by_addr[i.target], nd, nx)
            else:
                for t in table_targets(i.indirect.disp):
                    visit(by_addr[t], nd, nx)
            continue
        if mn.startswith('j'):
            visit(by_addr[i.target], nd, nx)
        if k + 1 < n:
            visit(k + 1, nd, nx)
    return delta, xd, problems


if __name__ == '__main__':
    ins = load()
    delta, xd, problems = analyze(ins)
    cmd = sys.argv[1] if len(sys.argv) > 1 else 'analyze'
    if cmd == 'analyze':
        un = [ins[k].addr for k in range(len(ins)) if delta[k] is None]
        print('instrucciones', len(ins), 'sin alcanzar', len(un), ['%08X' % a for a in un[:20]])
        for p in problems:
            print('PROBLEMA', p)
        print('--- llamadas: direccion llamada nombre | dwords en pila (delta) y x87')
        for k, i in enumerate(ins):
            if i.mn == 'call' and delta[k] is not None:
                nm = CALLS.get(i.target, ('?',))[0] if i.target else 'virtual'
                print('%08X call %s %s depth=%d dwords x87=%d' % (i.addr, ('%08X' % i.target) if i.target else i.text, nm, -delta[k] // 4 - 0x12C // 4 - 4, xd[k]))
