"""Oraculo independiente (Python) de Fn238A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE (`dumpbin /disasm` sobre el gta-sa.exe canonico, desensamblado/),
lo que debe observarse de las dos funciones del lote 238A y lo compara con las filas de comparisons.jsonl (lado original y candidato).

  0 ExeProtectionStub::MD5Transform 0x4012E9: la transformacion MD5 (RFC 1321) escrita desde la formula (K[i] = floor(abs(sin(i+1)) * 2^32), rotaciones y orden de palabras del RFC), no desde el C++.
  1 CRunningScript::ProcessCommands1700To1799 0x496E00: despachador de los comandos de script 1700..1799. Cada comando se escribe como un programa sobre el modelo de memoria, en el orden de lecturas,
    escrituras y llamadas del EXE: eventos de los colaboradores falsos (id, ECX, argumentos, lo apuntado), escrituras en ScriptParams y en las globales, bytes de las arenas, valor de AL.
No usa ninguna constante ni rama del C++ de produccion: las constantes de solo lectura (flotantes de 0x85xxxx...) se leen del EXE canonico; las demas salen del desensamblado.
Los constructores de las tareas corren REALES: sus escrituras en la memoria de la tarea (arena D, ventana [D_TASK, D_TASK + 0x200)) y la lista de referencias de las entidades que reciben (campo +0x24 y
CReferences) no las modela el oraculo: solo se comprueba que EXE y DLL escriben lo mismo (comparacion par a par) y que el despachador entrega la tarea al colaborador con los argumentos correctos.
Uso:  python -B fixture_oracle.py <comparisons.jsonl>
"""
import json
import math
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from fixture_common import *  # noqa: F401,F403
from fixture_common import M32, GUARD, WIN, Mem, PS, is_ps, tag_of, f2b, b2f, s32, X87, rnd
import fixture_common as FC
import collabs

EXE = Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')
_exe_bytes = None


def exe_rd(addr, n=4):
    """datos de solo lectura del EXE canonico (la imagen se carga en 0x400000; solo secciones con datos en disco)"""
    global _exe_bytes, _secs
    if _exe_bytes is None:
        _exe_bytes = EXE.read_bytes()
        pe = struct.unpack_from('<I', _exe_bytes, 0x3C)[0]
        nsec = struct.unpack_from('<H', _exe_bytes, pe + 6)[0]
        opt = struct.unpack_from('<H', _exe_bytes, pe + 20)[0]
        base = struct.unpack_from('<I', _exe_bytes, pe + 24 + 28)[0]
        _secs = []
        for i in range(nsec):
            o = pe + 24 + opt + 40 * i
            vsz, va, rsz, raw = struct.unpack_from('<IIII', _exe_bytes, o + 8)
            _secs.append((base + va, max(vsz, rsz), raw, rsz))
    for va, sz, raw, rsz in _secs:
        if va <= addr < va + sz and addr - va + n <= rsz:
            return int.from_bytes(_exe_bytes[raw + addr - va: raw + addr - va + n], 'little')
    raise Bad('lectura de solo lectura fuera de las secciones: %x' % addr)


class Bad(Exception):
    """caso invalido: el modelo desreferencia memoria desconocida"""


NAMES = ['ExeProtectionStub::MD5Transform', 'CRunningScript::ProcessCommands1700To1799']
ENTRIES = (0x4012E9, 0x496E00)

# ----------------------------------------------------------------------------------------------- MD5 (RFC 1321)
MD5_S = [7, 12, 17, 22] * 4 + [5, 9, 14, 20] * 4 + [4, 11, 16, 23] * 4 + [6, 10, 15, 21] * 4
MD5_K = [int(abs(math.sin(i + 1)) * 2 ** 32) & M32 for i in range(64)]


def rol(x, c):
    x &= M32
    return ((x << c) | (x >> (32 - c))) & M32


def md5_transform(state, block):
    a, b, c, d = state
    for i in range(64):
        if i < 16:
            f, g = (b & c) | (~b & d), i
        elif i < 32:
            f, g = (d & b) | (~d & c), (5 * i + 1) % 16
        elif i < 48:
            f, g = b ^ c ^ d, (3 * i + 5) % 16
        else:
            f, g = c ^ (b | (~d & M32)), (7 * i) % 16
        f = (f + a + MD5_K[i] + block[g]) & M32
        a, d, c = d, c, b
        b = (b + rol(f, MD5_S[i])) & M32
    return [(state[0] + a) & M32, (state[1] + b) & M32, (state[2] + c) & M32, (state[3] + d) & M32]


# ----------------------------------------------------------------------------------------------- mundo del caso
DEG = 0.017453292  # solo para documentar: la constante se lee del EXE (0x8595EC)
SCRIPT_THING = dict(SEARCH_LIGHT=2, CHECKPOINT=3, FIRE=5, DECISION_MAKER=7, PED_GROUP=8)
MC_DM, MC_SEARCHLIGHT, MC_CHECKPOINT = 9, 0xB, 0xC

CP, CN, SP_, UCF, GPV, RTL, GPST = 0x464080, 0x464250, 0x464370, 0x4859D0, 0x464790, 0x463D50, 0x465C20
MCA, MCR, GNU, GAS = 0x4637E0, 0x4654B0, 0x483720, 0x4839A0
MISSION_CLEANUP, IKCHAIN, CAMERA, FIREMGR, PATHS, STUCK, LINKED, GROUPS = 0xA90850, 0xC15448, 0xB6F028, 0xB71F80, 0x96F050, 0xA90AB0, 0xB4E6D8, 0xC09920
SEQ_BASE = 0xC178F0


class World:
    def __init__(self, c):
        self.arena = tuple(c.get('arena') or (0x30000000, 0x30100000, 0x30200000, 0x30300000))
        f = c['fill']
        self.mem = tuple(Mem(f[0], f[1 + i] & M32) for i in range(4))
        self.g = {}   # globales: byte -> valor
        self.gref = {}

    def tagv(self, v):
        v &= M32
        if is_ps(v):
            return tag_of((v >> 16) & 0xFF, v & 0xFFFF)
        return v

    def put(self, dest, off, width, val):
        if dest == 4:
            for k in range(width):
                self.g[off + k] = (val >> (8 * k)) & 0xFF
        else:
            self.mem[dest].wr(off, width, val)

    def snap(self):
        for m in self.mem:
            m.snap()
        self.gref = dict(self.g)

    def grd(self, addr, n=4):
        v = 0
        for k in range(n):
            b = self.g.get(addr + k)
            if b is None:
                raise Bad('global sin fijar %x' % (addr + k))
            v |= b << (8 * k)
        return v

    def gwr(self, addr, n, v):
        for k in range(n):
            self.g[addr + k] = (v >> (8 * k)) & 0xFF

    def rdt(self, tag, off, n=4):
        """lee de la arena a la que apunta la etiqueta (arena, desplazamiento)"""
        if not (0x10000000 <= tag < 0x14000000):
            raise Bad('desreferencia de una etiqueta invalida %x' % tag)
        return self.mem[(tag >> 20) & 0xF].rd((tag & 0xFFFFF) - GUARD + off, n)

    def wrt(self, tag, off, n, v):
        self.mem[(tag >> 20) & 0xF].wr((tag & 0xFFFFF) - GUARD + off, n, v)


def f32(x):
    return b2f(f2b(x))


class Ctx:
    def __init__(self, c):
        self.c = c
        self.W = World(c)
        self.events = []
        self.marks = []
        self.count = {}
        self.rets = {}
        for i in range(0, len(c['r']), 3):
            self.rets[(c['r'][i] & M32, c['r'][i + 1] & M32)] = c['r'][i + 2] & M32
        self.sp = [x & M32 for x in c['sp']]
        self.tx = [x & M32 for x in c['tx']]
        self.fuzzy = False
        self.S = None
        self.P = [0] * 32

    # ---- ayudas de memoria
    def ptr(self, v):
        return self.W.tagv(v)

    def gread(self, addr, n=4):
        return self.W.grd(addr, n)

    # ---- eventos
    def nextn(self, cid):
        n = self.count.get(cid, 0)
        self.count[cid] = n + 1
        return n

    def ret(self, cid, n):
        v = self.rets.get((cid, n), 0)
        return v

    def call(self, cid, ecx=None, args=(), derefs=()):
        """un colaborador falso: anota el evento y devuelve el valor programado (los punteros programados se dan como etiquetas)"""
        spec = collabs.BY_ID[cid]
        masks = collabs.MASK.get(cid, {})
        ev = [cid]
        if spec[2] == 'T':
            ev.append(ecx & M32)
        for i, a in enumerate(args):
            a &= M32
            ev.append(a & masks[i] if i in masks else a)
        for d in derefs:
            ev += [x & M32 for x in d]
        n = self.nextn(cid)
        extra = []
        eff = spec[7]
        if eff == 'SP':
            extra = [self.P[i] for i in range(args[0])]
        self.events.append(ev + extra)
        v = self.rets.get((cid, n), 0)
        if is_ps(v):
            v = self.W.tagv(v)
        if eff == 'CP':
            for i in range(args[0]):
                self.P[i] = self.sp[i]
        return v, n

    # colaboradores de uso frecuente
    def cp(self, k):
        self.call(CP, self.S, [k])

    def st(self, k):
        self.call(SP_, self.S, [k])

    def ucf(self, b):
        self.call(UCF, self.S, [1 if b else 0])

    def gas(self, handle, typ):
        return s32(self.call(GAS, None, [handle, typ])[0])

    def gnu(self, idx, typ):
        return self.call(GNU, None, [idx, typ])[0]

    def pool_at(self, kind, ref):
        """CPool::GetAtRef (real): ranura = ref >> 8 (aritmetico); coincide el byte de estado completo con el byte bajo de la referencia"""
        hdr, slots, stride, arena, base = {'ped': (FC.A_PEDPOOL, FC.A_PEDSLOT, FC.PED_STRIDE, 1, FC.B_PED), 'veh': (FC.A_VEHPOOL, FC.A_VEHSLOT, FC.VEH_STRIDE, 2, FC.C_VEH),
                                          'obj': (FC.A_OBJPOOL, FC.A_OBJSLOT, FC.OBJ_STRIDE, 2, FC.C_OBJ)}[kind]
        cap = {'ped': FC.NPED, 'veh': FC.NVEH, 'obj': FC.NOBJ}[kind]
        idx = s32(ref) >> 8
        if not 0 <= idx < cap:
            raise Bad('referencia fuera de rango')
        if self.W.mem[0].rd(slots + idx, 1) != (ref & 0xFF):
            raise Bad('referencia de ranura distinta')
        return tag_of(arena, base + idx * stride)

    def ped(self, ref):
        return self.pool_at('ped', ref)

    def veh(self, ref):
        return self.pool_at('veh', ref)

    def obj(self, ref):
        return self.pool_at('obj', ref)

    def rd(self, tag, off, n=4):
        return self.W.rdt(tag, off, n)

    def wr(self, tag, off, n, v):
        self.W.wrt(tag, off, n, v)

    def fl(self, bits):
        return X87.of(bits)

    def mulf(self, a, b):
        return X87.tobits(X87.mul(X87.of(a), X87.of(b)))

    def deg(self, bits):
        k = exe_rd(0x8595EC)
        return X87.tobits(X87.mul(X87.of(bits), X87.of(k)))

    def ptask(self, size):
        """CTask::operator new: devuelve el bloque que programa el caso (arena D); `size` es el argumento"""
        v, n = self.call(0x61A5A0, None, [size])
        return v

    def give(self, handle, task, cmd):
        self.call(GPST, self.S, [handle, task, cmd])

    def ged_sprint(self):
        pass


# ----------------------------------------------------------------------------------------------- comandos
CMD = {}
PARAMS = {}   # comando -> (CollectParameters(k), tipos de los parametros)
RETS = {}     # comando -> [(id del colaborador, tipo del valor programado)]; llamadas 0..2


def cmd(num, k, kinds, rets=()):
    def deco(fn):
        CMD[num] = fn
        PARAMS[num] = (k, kinds)
        RETS[num] = list(rets)
        return fn
    return deco


def sz(X):
    pass


@cmd(1701, 2, ['hdl', 'i'], [(0x61A5A0, 'task')])
def c1701(X):
    X.cp(2)
    X.fuzzy = True
    t = X.ptask(0x24)
    X.give(X.P[0], t, 1701)
    return 0


@cmd(1703, 9, ['ped', 'veh', 'f', 'f', 'f', 'i', 'fa', 'fa', 'i'])
def c1703(X):
    X.cp(9)
    vl = X.deg(X.P[6])
    hl = X.deg(X.P[7])
    bike = X.veh(X.P[1])
    ped = X.ped(X.P[0])
    X.call(0x5E7E60, ped, [bike, X.P[2], X.P[3], X.P[4], X.P[5], vl, hl, X.P[8]])
    return 0


@cmd(1704, 5, ['hdl', 'ped', 't', 'f', 'fa'], [(0x61A5A0, 'task')])
def c1704(X):
    X.cp(5)
    X.fuzzy = True
    ang = X.deg(X.P[4])
    target = X.ped(X.P[1])
    t = X.ptask(0x54)
    X.give(X.P[0], t, 1704)
    # el EXE copia la global 0x8D237C a la tarea y la pone a 6
    X.W.gwr(0x8D237C, 4, 6)
    return 0


@cmd(1705, 5, ['pedm1', 'f', 'f', 'f', 'tl'], [(0x61A5A0, 'task')])
def c1705(X):
    X.cp(5)
    d = s32(X.P[4])
    if d == -1:
        d = 20000
    elif d == -2:
        d = 999999
    if s32(X.P[0]) != -1:
        ped = X.ped(X.P[0])
        # LookAt(this = g_ikChainMan, texto 0x85A3F0, ped, nullptr, duracion, bone -1, &pos (pila), false, 0.25f, 500, 6, true)
        X.call(0x618970, IKCHAIN, [0x85A3F0, ped, 0, d & M32, 0xFFFFFFFF, 0x7F000000, 0, f2b(0.25), 500, 6, 1], [(X.P[1], X.P[2], X.P[3])])
    else:
        X.fuzzy = True
        t = X.ptask(0x30)
        idx = X.gread(0x8D2E98)
        X.call(0x632D10, SEQ_BASE + ((idx << 6) & M32), [t])
    return 0


@cmd(1706, 1, ['i'])
def c1706(X):
    X.cp(1)
    X.ucf(False)
    return 0


@cmd(1707, 2, ['ped', 'b'])
def c1707(X):
    X.cp(2)
    ped = X.ped(X.P[0])
    if X.P[1] != 0:
        X.call(0x5E6530, ped, [])
        return 0
    X.call(0x5E6550, ped, [])
    if (X.rd(ped, 0x46C) >> 8) & 1:
        X.call(0x5E6370, ped, [0])
    return 0


@cmd(1708, 1, ['ped'])
def c1708(X):
    X.cp(1)
    ped = X.ped(X.P[0])
    sp = ped + 0x44
    if (X.rd(ped, 0x46C) >> 8) & 1:
        v = X.W.tagv(X.rd(ped, 0x58C))
        if v:
            sp = v + 0x44
    x, y, z = (X.fl(X.rd(sp, o)) for o in (0, 4, 8))
    s = X87.add(X87.add(X87.mul(x, x), X87.mul(y, y)), X87.mul(z, z))
    m = X87.sqrt(s)
    X.P[0] = X87.tobits(X87.mul(m, X87.of(exe_rd(0x858B40))))
    X.st(1)
    return 0


@cmd(1709, 2, ['grp', 'dm'], [(GAS, 'thing')])
def c1709(X):
    X.cp(2)
    gi = X.gas(X.P[0], 8)
    if gi < 0 or gi >= 8:
        return 0
    dm = -1 & M32
    if X.P[1] != M32:
        dm = X.gas(X.P[1], 7) & M32
    X.call(0x5F7340, GROUPS + 0x30 + gi * 0x2D4, [dm])
    return 0


@cmd(1710, 1, ['i'], [(0x607D30, 'int'), (0x483720, 'int'), (CN, 'int'), (GAS, 'thing')])
def c1710(X):
    X.cp(1)
    # GetGrpDMName(indice, buffer de 128 en la pila)
    X.call(0x600880, None, [X.P[0], 0x7F000000])
    nxt = X.call(CN, X.S, [])[0]
    X.gas(nxt, 7)
    dm = X.call(0x607D30, None, [0x7F000000, 1, X.rd(X.S, 0xC6, 1)])[0]
    X.P[0] = X.gnu(dm, 7)
    X.st(1)
    if X.rd(X.S, 0xC6, 1):
        X.call(MCA, MISSION_CLEANUP, [X.P[0], MC_DM])
    return 0


@cmd(1711, 2, ['pl', 'b'])
def c1711(X):
    X.cp(2)
    pinfo = 0xB7CD98 + X.P[0] * 0x190
    ped = X.W.tagv(X.gread(pinfo))
    pd = X.W.tagv(X.rd(ped, 0x480))
    X.wr(pd, 0x84, 1, 1 if X.P[1] != 0 else 0)
    return 0


@cmd(1712, 2, ['hdl', 'dur'], [(0x61A5A0, 'task')])
def c1712(X):
    X.cp(2)
    X.fuzzy = True
    t = X.ptask(0x14)
    X.give(X.P[0], t, 1712)
    return 0


def fx(v):
    return v & M32


@cmd(1713, 8, ['f'] * 8, [(0x493000, 'thing'), (CN, 'int'), (GAS, 'thing')])
def c1713(X):
    X.cp(8)
    nxt = X.call(CN, X.S, [])[0]
    X.gas(nxt, 2)
    P = X.P
    sl = X.call(0x493000, None, [P[0], P[1], P[2], 0, P[3], P[4], P[5], P[6], P[7]])[0]
    P[0] = sl
    if X.rd(X.S, 0xC6, 1):
        X.call(MCA, MISSION_CLEANUP, [P[0], MC_SEARCHLIGHT])
    X.st(1)
    return 0


@cmd(1714, 1, ['th'], [(GAS, 'thing')])
def c1714(X):
    X.cp(1)
    if X.gas(X.P[0], 2) < 0:
        return 0
    X.call(0x493160, None, [X.P[0]])
    if X.rd(X.S, 0xC6, 1):
        X.call(MCR, MISSION_CLEANUP, [X.P[0], MC_SEARCHLIGHT])
    return 0


@cmd(1715, 1, ['th'], [(GAS, 'thing')])
def c1715(X):
    X.cp(1)
    ok = X.gas(X.P[0], 2) >= 0
    X.ucf(ok)
    return 0


@cmd(1716, 8, ['th', 'f', 'f', 'f', 'f', 'f', 'f', 'f'])
def c1716(X):
    X.cp(8)
    P = X.P
    X.call(0x493360, None, [P[0], P[1], P[2], P[3], P[4], P[5], P[6], P[7]])
    return 0


@cmd(1717, 5, ['th', 'f', 'f', 'f', 'f'])
def c1717(X):
    X.cp(5)
    P = X.P
    X.call(0x493480, None, [P[0], P[1], P[2], P[3], P[4]])
    return 0


@cmd(1718, 3, ['th', 'ped', 'f'])
def c1718(X):
    X.cp(3)
    P = X.P
    ped = X.ped(P[1])
    X.call(0x493420, None, [P[0], ped, P[2]])
    return 0


@cmd(1719, 2, ['th', 'ped'], [(0x493900, 'bool')])
def c1719(X):
    X.cp(2)
    ped = X.ped(X.P[1])
    r = X.call(0x493900, None, [X.P[0], ped])[0]
    X.ucf((r & 0xFF) != 0)
    return 0


@cmd(1721, 0, [])
def c1721(X):
    X.ucf(X.gread(0xB5F84C) == 2)
    return 0


@cmd(1722, 4, ['hdl', 'f', 'f', 'f'], [(0x61A5A0, 'task')])
def c1722(X):
    X.cp(4)
    X.fuzzy = True
    t = X.ptask(0x28)
    X.give(X.P[0], t, 1722)
    return 0


@cmd(1723, 3, ['hdl', 'vehn', 'f'], [(0x61A5A0, 'task')])
def c1723(X):
    X.cp(3)
    X.fuzzy = True
    if s32(X.P[1]) >= 0:
        X.veh(X.P[1])
    t = X.ptask(0x2C)
    X.give(X.P[0], t, 1723)
    return 0


@cmd(1724, 7, ['f'] * 6 + ['i'])
def c1724(X):
    X.cp(7)
    P = X.P
    X.call(0x73AF00, None, [0x7F000000, 0x7F000000, P[6]], [(P[0], P[1], P[2]), (P[3], P[4], P[5])])
    return 0


@cmd(1725, 11, ['f'] * 6 + ['b'] * 5, [(0x56A490, 'bool')])
def c1725(X):
    X.cp(11)
    P = X.P
    r = X.call(0x56A490, None, [0x7F000000, 0x7F000000] + [1 if P[i] else 0 for i in range(6, 11)] + [0, 0], [(P[0], P[1], P[2]), (P[3], P[4], P[5])])[0]
    X.ucf((r & 0xFF) != 0)
    return 0


@cmd(1726, 1, ['veh'], [(0x6A6010, 'f')])
def c1726(X):
    X.cp(1)
    veh = X.veh(X.P[0])
    r = X.call(0x6A6010, veh, [])[0]
    X.P[0] = r
    X.st(1)
    return 0


@cmd(1727, 3, ['th', 'veh', 'f'])
def c1727(X):
    X.cp(3)
    P = X.P
    veh = X.veh(P[1])
    X.call(0x493420, None, [P[0], veh, P[2]])
    return 0


@cmd(1728, 2, ['th', 'veh'], [(0x493900, 'bool')])
def c1728(X):
    X.cp(2)
    veh = X.veh(X.P[1])
    r = X.call(0x493900, None, [X.P[0], veh])[0]
    X.ucf((r & 0xFF) != 0)
    return 0


@cmd(1729, 9, ['veh'] + ['f'] * 8, [(0x493000, 'thing'), (CN, 'int'), (GAS, 'thing')])
def c1729(X):
    X.cp(9)
    nxt = X.call(CN, X.S, [])[0]
    X.gas(nxt, 2)
    P = X.P
    veh = X.veh(P[0])
    sl = X.call(0x493000, None, [P[1], P[2], P[3], veh, P[4], P[5], P[6], P[7], P[8]])[0]
    P[0] = sl
    if X.rd(X.S, 0xC6, 1):
        X.call(MCA, MISSION_CLEANUP, [P[0], MC_SEARCHLIGHT])
    X.st(1)
    return 0


@cmd(1730, 11, ['hdl', 'f', 'f', 'f', 'i', 'f', 'f', 'pedn', 'f', 'f', 'f'], [(0x61A5A0, 'task')])
def c1730(X):
    X.cp(11)
    X.fuzzy = True
    if s32(X.P[7]) >= 0:
        X.ped(X.P[7])
    t = X.ptask(0x38)
    X.give(X.P[0], t, 1730)
    return 0


@cmd(1731, 4, ['f', 'f', 'f', 'f'], [(0x5397F0, 'int')])
def c1731(X):
    X.cp(4)
    P = X.P
    n = X.call(0x5397F0, FIREMGR, [0x7F000000, P[3]], [(P[0], P[1], P[2])])[0]
    X.P[0] = n
    X.st(1)
    return 0


@cmd(1732, 1, ['th'], [(GAS, 'thing'), (0x582870, 'int'), (CN, 'int'), (0x5839A0, 'int')])
def c1732(X):
    X.cp(1)
    X.gas(X.P[0], 2)
    nxt = X.call(CN, X.S, [])[0]
    X.call(0x582870, None, [nxt])
    blip = X.call(0x5839A0, None, [6, X.P[0], 0, 3])[0]
    X.call(0x583CC0, None, [blip, 3])
    X.P[0] = blip
    X.st(1)
    return 0


@cmd(1733, 1, ['veh'])
def c1733(X):
    X.cp(1)
    X.call(0x45A4A0, None, [X.veh(X.P[0])])
    return 0


@cmd(1735, 4, ['hdl', 'vehm1', 'i', 'i'], [(0x61A5A0, 'task')])
def c1735(X):
    X.cp(4)
    X.fuzzy = True
    if s32(X.P[1]) != -1:
        X.veh(X.P[1])
    t = X.ptask(0x68)
    X.give(X.P[0], t, 1735)
    return 0


@cmd(1736, 1, ['b'])
def c1736(X):
    X.cp(1)
    X.W.g[0xB72958] = 1 if X.P[0] else 0
    return 0


@cmd(1737, 1, ['ped'], [(0x5F7E80, 'grp'), (0x5F69C0, 'bool')])
def c1737(X):
    X.cp(1)
    ped = X.ped(X.P[0])
    grp = X.call(0x5F7E80, None, [ped])[0]
    if not grp:
        return 0
    gt = X.W.tagv(grp)
    if X.call(0x5F69C0, gt + 8, [ped])[0] & 0xFF:
        return 0
    X.call(0x5FB210, gt + 8, [ped])
    X.call(0x5FC7E0, gt, [])
    return 0


@cmd(1738, 7, ['th', 'obj', 'obj', 'obj', 'f', 'f', 'f'])
def c1738(X):
    X.cp(7)
    P = X.P
    bulb = X.obj(P[3])
    housing = X.obj(P[2])
    tower = X.obj(P[1])
    X.call(0x4934F0, None, [P[0], tower, housing, bulb, P[4], P[5], P[6]])
    return 0


@cmd(1743, 1, ['i'])
def c1743(X):
    X.cp(1)
    return 0


@cmd(1744, 1, ['b'])
def c1744(X):
    X.cp(1)
    X.W.g[0x8A5B28] = 1 if X.P[0] else 0
    return 0


def _label16(X, vartype):
    dest = X.call(GPV, X.S, [vartype])[0]
    txt = b''.join(struct.pack('<I', w) for w in X.tx)
    X.call(RTL, X.S, [0x7F000000, 16])
    # strncpy(destino, texto, 16): copia hasta el primer NUL y rellena con ceros hasta 16 bytes
    n = txt.find(b'\0')
    n = 16 if n < 0 else n
    out = txt[:n] + b'\0' * (16 - n)
    for i in range(16):
        X.wr(X.W.tagv(dest), i, 1, out[i])
    return 0


@cmd(1745, 0, [], [(GPV, 'ptrA')])
def c1745(X):
    return _label16(X, 2)


@cmd(1746, 0, [], [(GPV, 'ptrA')])
def c1746(X):
    return _label16(X, 1)


@cmd(1749, 8, ['th', 'f', 'f', 'f', 'f', 'f', 'f', 'f'], [(0x4935A0, 'thing'), (CN, 'int'), (GAS, 'thing')])
def c1749(X):
    X.cp(8)
    nxt = X.call(CN, X.S, [])[0]
    X.gas(nxt, 3)
    P = X.P
    cp = X.call(0x4935A0, None, [P[1], P[2], P[3], P[4], P[5], P[6], P[7], P[0]])[0]
    P[0] = cp
    if X.rd(X.S, 0xC6, 1):
        X.call(MCA, MISSION_CLEANUP, [P[0], MC_CHECKPOINT])
    X.st(1)
    return 0


@cmd(1750, 1, ['th'], [(GAS, 'thing')])
def c1750(X):
    X.cp(1)
    if X.gas(X.P[0], 3) < 0:
        return 0
    X.call(0x4936C0, None, [X.P[0]])
    if X.rd(X.S, 0xC6, 1):
        X.call(MCR, MISSION_CLEANUP, [X.P[0], MC_CHECKPOINT])
    return 0


@cmd(1751, 1, ['b'])
def c1751(X):
    X.cp(1)
    X.call(0x6F5DB0, None, [0 if X.P[0] != 0 else 1])
    return 0


@cmd(1752, 5, ['i', 'f', 'f', 'f', 'b'], [(0x6F7550 | 0x01000000, 'trainptr'), (0x6F7550, 'int')])
def c1752(X):
    X.cp(5)
    P = X.P
    X.call(0x6F7550, None, [P[1], P[2], P[3], 1 if P[4] else 0, P[0], 0x7F000000, 0, 0xFFFFFFFF, 0xFFFFFFFF, 1])
    train = X.rets.get((0x6F7550 | 0x01000000, 0), 0)
    X.P[0] = X.veh_ref(train)
    X.st(1)
    return 0


def _veh_ref(X, ptr):
    """CVehiclePool::GetRef (real): ((ptr - almacen) / zancada) << 8 | estado de la ranura (division con signo)"""
    off = (X.W.tagv(ptr) & 0xFFFFF) - GUARD - FC.C_VEH
    idx = off // FC.VEH_STRIDE
    st = X.W.mem[0].rd(FC.A_VEHSLOT + idx, 1)
    return ((idx << 8) | st) & M32


Ctx.veh_ref = _veh_ref


@cmd(1753, 0, [])
def c1753(X):
    X.call(0x6F6A20, None, [])
    return 0


@cmd(1754, 0, [])
def c1754(X):
    X.call(0x6F6B60, None, [])
    return 0


@cmd(1755, 0, [])
def c1755(X):
    X.call(0x6F6AA0, None, [])
    return 0


@cmd(1756, 2, ['veh', 'f'])
def c1756(X):
    X.cp(2)
    veh = X.veh(X.P[0])
    X.call(0x6F5E20, None, [veh, X.P[1]])
    return 0


@cmd(1757, 2, ['veh', 'f'])
def c1757(X):
    X.cp(2)
    veh = X.veh(X.P[0])
    X.call(0x6F5E50, None, [veh, X.P[1]])
    return 0


@cmd(1758, 1, ['veh'], [(0x6F5E70, 'vehptr')])
def c1758(X):
    X.cp(1)
    veh = X.veh(X.P[0])
    cab = X.call(0x6F5E70, None, [veh])[0]
    X.P[0] = X.veh_ref(cab)
    X.st(1)
    return 0


@cmd(1759, 1, ['i'])
def c1759(X):
    X.cp(1)
    X.call(0x6094A0, None, [X.P[0]])
    return 0


@cmd(1760, 1, ['i'])
def c1760(X):
    X.cp(1)
    X.call(0x50C260, CAMERA, [])
    return 0


@cmd(1761, 6, ['hdl', 'vehn', 'vehn', 'i', 'f', 'i'], [(0x61A5A0, 'task')])
def c1761(X):
    X.cp(6)
    X.fuzzy = True
    if s32(X.P[1]) >= 0:
        X.veh(X.P[1])
    if s32(X.P[2]) >= 0:
        X.veh(X.P[2])
    t = X.ptask(0x2C)
    X.give(X.P[0], t, 1761)
    return 0


@cmd(1762, 4, ['hdl', 'obj', 'i', 'f'], [(0x61A5A0, 'task')])
def c1762(X):
    X.cp(4)
    X.fuzzy = True
    X.obj(X.P[1])
    t = X.ptask(0x4C)
    X.give(X.P[0], t, 1762)
    return 0


@cmd(1763, 2, ['hdl', 'b'], [(0x61A5A0, 'task')])
def c1763(X):
    X.cp(2)
    X.fuzzy = True
    t = X.ptask(0x20)
    X.give(X.P[0], t, 1763)
    return 0


@cmd(1764, 2, ['hdl', 'ped'], [(0x61A5A0, 'task')])
def c1764(X):
    X.cp(2)
    X.fuzzy = True
    X.ped(X.P[1])
    t = X.ptask(0x24)
    X.give(X.P[0], t, 1764)
    return 0


@cmd(1765, 2, ['veh', 'n'])
def c1765(X):
    X.cp(2)
    veh = X.veh(X.P[0])
    mi = X.W.tagv(X.gread(0xA9B0C8 + 4 * X.rd(veh, 0x22, 2)))
    v = X.rd(mi, 0x2D6 + 2 * s32(X.P[1]), 2)
    X.P[0] = (v - 0x10000 if v & 0x8000 else v) & M32
    X.st(1)
    return 0


def script_model(X, mid):
    mid = s32(mid)
    if mid >= 0:
        return mid
    return s32(X.gread(0xA44B88 + 0x1C * (-mid)))


@cmd(1766, 1, ['sm'])
def c1766(X):
    X.cp(1)
    m = script_model(X, X.P[0])
    mi = X.W.tagv(X.gread(0xA9B0C8 + 4 * m))
    flags = X.rd(mi, 0x12, 2)
    kind = (flags >> 10) & 0x1F
    if flags & 0x100:
        table = {1: 0x0B, 2: 0x0C, 12: 0x0E, 13: 0x0F, 19: 0x0D, 20: 0x10, 21: 0x10, 22: 0x10}
    else:
        table = {0: 0, 1: 1, 2: 1, 6: 2, 8: 3, 9: 3, 10: 4, 11: 5, 12: 6, 14: 7, 15: 8, 16: 9, 17: 10}
    if kind in table:
        X.P[0] = table[kind]
    X.st(1)
    return 0


@cmd(1767, 2, ['veh', 'sm'], [(0x6E3290, 'int')])
def c1767(X):
    X.cp(2)
    veh = X.veh(X.P[0])
    m = script_model(X, X.P[1])
    r = X.call(0x6E3290, veh, [m & M32])[0]
    X.P[0] = r
    X.st(1)
    return 0


@cmd(1768, 2, ['veh', 'sm'])
def c1768(X):
    X.cp(2)
    veh = X.veh(X.P[0])
    m = script_model(X, X.P[1])
    X.call(0x6DF930, veh, [m & M32])
    return 0


@cmd(1769, 1, ['sm'])
def c1769(X):
    X.cp(1)
    m = script_model(X, X.P[0])
    X.call(0x408C70, None, [m & M32, 0xC])
    return 0


@cmd(1770, 1, ['sm'], [(0x407820, 'bool')])
def c1770(X):
    X.cp(1)
    m = script_model(X, X.P[0])
    r = X.call(0x407820, None, [m & M32])[0]
    X.ucf((r & 0xFF) != 0)
    return 0


@cmd(1771, 1, ['sm'], [(0x4C74D0, 'int16')])
def c1771(X):
    X.cp(1)
    m = script_model(X, X.P[0])
    X.call(0x409C90, None, [m & M32])
    other = X.call(0x4C74D0, LINKED, [(m & 0xFFFF) | (0xFFFF0000 if m & 0x8000 else 0)])[0]
    other = s32((other & 0xFFFF) | (0xFFFF0000 if other & 0x8000 else 0))
    if other == -1:
        return 0
    X.call(0x409C90, None, [other & M32])
    return 0


@cmd(1772, 1, ['veh'], [(0x4C86B0, 'int')])
def c1772(X):
    X.cp(1)
    veh = X.veh(X.P[0])
    mi = X.W.tagv(X.gread(0xA9B0C8 + 4 * X.rd(veh, 0x22, 2)))
    r = X.call(0x4C86B0, mi, [])[0]
    X.P[0] = r
    X.st(1)
    return 0


@cmd(1773, 2, ['veh', 'i'])
def c1773(X):
    X.cp(2)
    remap = X.P[1]
    veh = X.veh(X.P[0])
    X.call(0x6D0C00, veh, [remap])
    return 0


@cmd(1774, 2, ['ped', 'grp'], [(GAS, 'thing'), (0x5F6A10, 'bool')])
def c1774(X):
    X.cp(2)
    ped = X.ped(X.P[0])
    gi = X.gas(X.P[1], 8)
    res = False
    if 0 <= gi < 8 and X.gread(0xC098E0 + gi, 1):
        res = (X.call(0x5F6A10, GROUPS + 8 + gi * 0x2D4, [ped])[0] & 0xFF) != 0
    X.ucf(res)
    return 0


@cmd(1775, 2, ['ped', 'grp'], [(GAS, 'thing'), (0x5F69C0, 'bool')])
def c1775(X):
    X.cp(2)
    ped = X.ped(X.P[0])
    gi = X.gas(X.P[1], 8)
    res = False
    if 0 <= gi < 8:
        res = (X.call(0x5F69C0, GROUPS + 8 + gi * 0x2D4, [ped])[0] & 0xFF) != 0
    X.ucf(res)
    return 0


@cmd(1776, 2, ['grp', 'f'], [(GAS, 'thing')])
def c1776(X):
    X.cp(2)
    gi = X.gas(X.P[0], 8)
    if 0 <= gi < 8:
        # SetSeparationRange: escribe el flotante en el campo +0x2C de la pertenencia (grupo + 8): global 0xC09920 + gi*0x2D4 + 8 + 0x2C
        X.W.gwr(0xC0994C + gi * 0x2D4, 4, X.P[1])
    return 0


@cmd(1777, 1, ['f'])
def c1777(X):
    X.cp(1)
    X.W.g[0x8A5E4A] = 1
    X.W.gwr(0x8A5E4C, 4, X.P[0])
    return 0


@cmd(1778, 0, [])
def c1778(X):
    X.W.g[0x8A5E4A] = 0
    return 0


@cmd(1779, 1, ['b'])
def c1779(X):
    X.cp(1)
    X.W.g[0x96A8B2] = 1 if X.P[0] == 0 else 0
    return 0


@cmd(1781, 1, ['th'], [(GAS, 'thing'), (0x5397E0, 'fireptr')])
def c1781(X):
    X.cp(1)
    fi = X.gas(X.P[0], 5)
    if 0 <= fi < 0x3C:
        vec = X.call(0x5397E0, FIREMGR, [fi & 0xFFFF])[0]
        v = X.W.tagv(vec)
        x, y, z = (X.rd(v, o) for o in (0, 4, 8))
        X.P[1], X.P[2], X.P[0] = y, z, x
    else:
        X.P[1] = X.P[2] = X.P[0] = 0
    X.st(3)
    return 0


@cmd(1784, 4, ['f', 'f', 'fz', 'n3'], [(0x569660, 'f'), (0x44F8C0, 'node'), (0x4505E0, 'p'), (0x450320, 'f')])
def c1784(X):
    X.cp(4)
    x, y, z = X.P[0], X.P[1], X.P[2]
    zf = b2f(z)
    lim = b2f(exe_rd(0x859014))
    # fcom: `z <= -100.0f` es falso con NaN (test ah,41h / jp)
    if zf == zf and zf <= lim:
        z = X.call(0x569660, None, [x, y])[0]
    nth = (X.P[3] - 1) & M32
    nn = X.nextn(0x44F8C0)
    node = X.rets.get((0x44F8C0, nn), 0)
    X.events.append([0x44F8C0, PATHS, 0x7F000000, x, y, z, 0, 0x497423FE, 0, 1, nth, 0, 0, 0])
    n = X.nextn(0x4505E0)
    # FindNodeCoorsForScript(this, sret, nodo, &found): el falso escribe el CVector programado y el byte
    px = X.rets.get((0x4505E0 | 0x02000000, n), 0)
    py = X.rets.get((0x4505E0 | 0x03000000, n), 0)
    pz = X.rets.get((0x4505E0 | 0x04000000, n), 0)
    found = X.rets.get((0x4505E0 | 0x05000000, n), 0) & 0xFF
    X.events.append([0x4505E0, PATHS, 0x7F000000, node & M32, 0x7F000000])
    if found:
        X.P[2] = pz
        X.P[0] = px
        X.P[1] = py
        o = X.call(0x450320, PATHS, [node & M32])[0]
        X.P[3] = o
        res = True
    else:
        X.P[0] = X.P[1] = X.P[2] = X.P[3] = 0
        res = False
    X.st(4)
    X.ucf(res)
    return 0


@cmd(1786, 1, ['b'])
def c1786(X):
    X.cp(1)
    X.W.g[0x96A8B3] = 1 if X.P[0] != 0 else 0
    return 0


@cmd(1788, 1, ['veh'], [(0x463C70, 'bool')])
def c1788(X):
    X.cp(1)
    r = X.call(0x463C70, STUCK, [X.P[0]])[0]
    X.ucf((r & 0xFF) != 0)
    return 0


@cmd(1789, 2, ['veh', 'f'])
def c1789(X):
    X.cp(2)
    sp = X.P[1]
    veh = X.veh(X.P[0])
    X.call(0x459660, None, [veh, sp])
    return 0


@cmd(1791, 2, ['ped', 'fr'])
def c1791(X):
    X.cp(2)
    ped = X.ped(X.P[0])

    def pos(p):
        m = X.W.tagv(X.rd(p, 0x14))
        return [X.fl(X.rd(m + 0x30 if m else p + 4, o)) for o in (0, 4, 8)]
    px = pos(ped)
    rs = X87.mul(X.fl(X.P[1]), X.fl(X.P[1]))
    rs_b = X87.tobits(rs)
    rsf = X.fl(rs_b)
    found = False
    for i in range(FC.NPED - 1, -1, -1):
        if found:
            break
        if X.W.mem[0].rd(FC.A_PEDSLOT + i, 1) & 0x80:
            continue
        other = tag_of(1, FC.B_PED + i * FC.PED_STRIDE)
        if other == ped:
            continue
        o = pos(other)
        d = [X.fl(X87.tobits(X87.sub(o[k], px[k]))) for k in range(3)]
        sq = X87.add(X87.add(X87.mul(d[0], d[0]), X87.mul(d[1], d[1])), X87.mul(d[2], d[2]))
        # fcomp con test ah,5 / jp: estricto y falso con NaN
        if not isinstance(sq, Fraction) or not isinstance(rsf, Fraction):
            lt = float(sq) < float(rsf)
        else:
            lt = sq < rsf
        if lt:
            found = True
    X.ucf(found)
    return 0


@cmd(1793, 0, [])
def c1793(X):
    X.wr(X.S, 0xD8, 4, 0)
    return 0


def _rect(minx, maxy, maxx, miny):
    return [minx, maxy, maxx, miny]


def _minmax(a, b):
    fa, fb = b2f(a), b2f(b)
    if fa > fb:
        return b, a
    return a, b


@cmd(1794, 4, ['fn', 'fn', 'fn', 'fn'], [(0x49D0B0, 'int')])
def c1794(X):
    X.cp(4)
    x1, y1, x2, y2 = X.P[0], X.P[1], X.P[2], X.P[3]
    minx, maxx = _minmax(x1, x2)
    miny, maxy = _minmax(y1, y2)
    r = X.call(0x49D0B0, None, [0x7F000000], [_rect(minx, maxy, maxx, miny)])[0]
    X.P[0] = r
    X.st(1)
    return 0


@cmd(1795, 5, ['fn', 'fn', 'fn', 'fn', 'b'])
def c1795(X):
    X.cp(5)
    x1, y1, x2, y2 = X.P[0], X.P[1], X.P[2], X.P[3]
    minx, maxx = _minmax(x1, x2)
    miny, maxy = _minmax(y1, y2)
    X.call(0x49CFE0, None, [0x7F000000, 0xFF if X.P[4] != 0 else 0], [_rect(minx, maxy, maxx, miny)])
    return 0


@cmd(1796, 4, ['veh', 'f', 'f', 'fz'], [(0x569660, 'f'), (0x536BE0, 'f'), (0x42F870, 'bool')])
def c1796(X):
    X.cp(4)
    veh = X.veh(X.P[0])
    x, y, z = X.P[1], X.P[2], X.P[3]
    zf = b2f(z)
    if zf == zf and zf <= b2f(exe_rd(0x859014)):
        z = X.call(0x569660, None, [x, y])[0]
    base = X.call(0x536BE0, veh, [])[0]
    zz = X87.tobits(X87.add(X.fl(base), X.fl(z)))
    ok = X.call(0x42F870, None, [veh, 0x7F000000, 0, 0], [(x, y, zz)])[0] & 0xFF
    mission = X.rd(veh, 0x3BA, 1)
    if mission not in (0x39, 0x3A):
        X.wr(veh, 0x3BA, 1, 9 if ok else 0x21)
    # SetStatus(STATUS_PHYSICS): byte +0x36 = (byte & 7) | 0x18 ... conserva los bits bajos y pone el estado 3 en los bits 3..7: (b & 7) | 0x18
    b = X.rd(veh, 0x36, 1)
    X.wr(veh, 0x36, 1, (b & 7) | 0x18)
    X.call(0x41BDD0, veh, [1])
    cr = X.rd(veh, 0x3D0, 1)
    X.wr(veh, 0x3D0, 1, max(cr, 1))
    X.wr(veh, 0x3AC, 4, X.gread(0xB7CB84))
    return 0


@cmd(1797, 2, ['veh', 'rec'])
def c1797(X):
    X.cp(2)
    veh = X.veh(X.P[0])
    X.call(0x45A980, None, [veh, X.P[1], 1, 0])
    return 0


@cmd(1798, 2, ['veh', 'f'])
def c1798(X):
    X.cp(2)
    d = X.P[1]
    veh = X.veh(X.P[0])
    X.call(0x459D10, None, [veh, d])
    return 0


@cmd(1799, 1, ['i'])
def c1799(X):
    X.cp(1)
    X.wr(X.S, 0xD8, 4, X.P[0])
    return 0


# sin cuerpo: 0 (OR_CONTINUE) o -1 (OR_INTERRUPT)
NOBODY0 = (1700, 1702, 1734, 1739, 1742, 1780, 1782, 1783, 1785, 1787, 1790, 1792)
NOBODYM1 = (1720, 1740, 1741, 1747, 1748)


# ----------------------------------------------------------------------------------------------- oraculo de un caso
D_TASK_WIN = (FC.D_TASK, FC.D_TASK + 0x200)


def expected(c):
    fn = c['fn']
    X = Ctx(c)
    W = X.W
    marks = ['fn%d' % fn, 'family%d' % c['family']]
    p = [x & M32 for x in c['p']]
    for i in range(0, len(p), 3):
        W.put(p[i] >> 8, p[i + 1], p[i] & 0xFF, p[i + 2])
    W.snap()
    args = [x & M32 for x in c['args']]
    ret = None
    if fn == 0:
        st_t, bl_t = W.tagv(args[0]), W.tagv(args[1])
        state = [W.rdt(st_t, 4 * i) for i in range(4)]
        block = [W.rdt(bl_t, 4 * i) for i in range(16)]
        new = md5_transform(state, block)
        for i in range(4):
            W.wrt(st_t, 4 * i, 4, new[i])
        marks.append('alias' if st_t == bl_t else ('overlap' if abs((st_t & 0xFFFFF) - (bl_t & 0xFFFFF)) < 64 and (st_t >> 20) == (bl_t >> 20) else 'plain'))
    else:
        X.S = W.tagv(c['ecx'] & M32)
        for i in range(32):
            X.P[i] = W.grd(0xA43C78 + 4 * i)
        num = s32(args[0])
        marks.append('cmd%d' % num)
        if 1700 <= num <= 1799:
            if num in CMD:
                CMD[num](X)
                ret = 0
            elif num in NOBODY0:
                ret = 0
            elif num in NOBODYM1:
                ret = 0xFF
            else:
                raise Bad('comando sin modelo %d' % num)
        else:
            ret = 0xFF
            marks.append('outside')
        for i in range(32):
            W.gwr(0xA43C78 + 4 * i, 4, X.P[i])
    gch = []
    if fn == 1:
        for at, n in FC.REGIONS:
            if n > 0x40:
                continue
            for k in range(n):
                a = at + k
                if W.g.get(a, W.gref.get(a)) != W.gref.get(a):
                    gch.append([a, W.g[a]])
    return {'events': X.events, 'changes': {'A': W.mem[0].changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes(), 'D': W.mem[3].changes()}, 'gchanges': gch, 'ret': ret,
            'threw': False, 'marks': marks, 'fuzzy': X.fuzzy}


def _ignored(arena, off, fuzzy):
    if not fuzzy:
        return False
    if arena == 'D' and D_TASK_WIN[0] <= off < D_TASK_WIN[1]:
        return True
    if arena == 'B' and (off % FC.PED_STRIDE) in range(0x24, 0x28) and off < FC.PED_STRIDE * FC.NPED:
        return True
    if arena == 'C':
        if off < FC.C_OBJ and (off % FC.VEH_STRIDE) in range(0x24, 0x28) and off < FC.VEH_STRIDE * FC.NVEH:
            return True
        if off >= FC.C_OBJ and ((off - FC.C_OBJ) % FC.OBJ_STRIDE) in range(0x24, 0x28):
            return True
    return False


def _filt(ch, arena, fuzzy):
    return [x for x in ch if not _ignored(arena, x[0], fuzzy)]


def check_side(c, o, e, side_row):
    fz = e.get('fuzzy')
    for ar in 'ABCD':
        if _filt(o['changes'][ar], ar, fz) != _filt(e['changes'][ar], ar, fz):
            return 'changes %s: %s != %s' % (ar, str(_filt(o['changes'][ar], ar, fz))[:300], str(_filt(e['changes'][ar], ar, fz))[:300])
    known = lambda a: any(at <= a < at + n for at, n in FC.REGIONS)
    og = [x for x in o['gchanges'] if not (fz and (0xBA1768 <= x[0] < 0xBA176C or not known(x[0])))]
    if og != e['gchanges']:
        return 'gchanges %s != %s' % (str(og)[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (str(o['events'])[:600], str(e['events'])[:600])
    if o.get('threw') != e['threw']:
        return 'threw'
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
    return None


KEYS = {0: ['alias', 'overlap', 'plain'], 1: []}
if __name__ == '__main__':
    bad = 0
    n = 0
    with open(sys.argv[1], encoding='utf-8') as fh:
        for line in fh:
            if not line.startswith('{"accepted":'):
                continue
            row = json.loads(line)
            n += 1
            r = check_row(row)
            if r:
                bad += 1
                if bad <= 10:
                    print(row['function'], row['case'], r)
    print(n, 'filas;', bad, 'con diferencias respecto del modelo')
    sys.exit(1 if bad else 0)
