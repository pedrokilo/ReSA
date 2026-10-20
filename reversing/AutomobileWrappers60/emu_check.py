"""Revision de la fase 2 SIN juego: ejecuta los BYTES REALES del EXE canonico (D:\\Grand Theft Auto San Andreas\\gta-sa.exe) en un emulador x86 (unicorn) sobre las filas
`input` de las fixtures (volcadas con dump_fixtures.exe) y compara lo observado con fixture_oracle.py. No sustituye al banco en el juego (la DLL no se emula), pero valida que el
oraculo (y por tanto la lectura del desensamblado en la que se basa el C++) coincide con lo que el EXE hace de verdad.

  python -I emu_check.py rows.jsonl [maximo-de-casos-por-funcion]

Colaboradores sustituidos por los mismos falsos que Core.hpp (traza identica): GetFirstObject, CVehicle::Render, GetColModel, RwMatrixTranslate, RwFrameUpdateObjects,
GetVehicleAppearance, DoHeadLightBeam, renderCallBack, RwRenderStateGet/Set, CMatrix::{Attach,SetScale,Rotate,RotateY,UpdateRW}, IsWater, IsSand, FxSystem_c::AddParticle y
_CIasin (0x821E70: asin de Python; el CRT usa TLS/errno que aqui no existen). Se ejecutan de verdad: GetWheelSize (0x6A06F0), GetWheelStatus (0x6C21B0), CVector 0x40FE30/0x40FE90,
FxPrtMult_c::FxPrtMult_c (0x4AB290), el destructor de CMatrix (0x59ACD0) y todo el x87 (fsin/fpatan/fcomp...) de las tres funciones.
"""
import json
import math
import random
import struct
import sys
from pathlib import Path

from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UcError
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP,
                               UC_X86_REG_EIP)

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

EXE = Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe')
M32 = 0xFFFFFFFF
STACK_TOP = 0x7F100000
SENTINEL = 0x7E000000
FAKE_PAGE = 0x7E100000          # direcciones "de funcion" para los punteros del motor RW
CAR = 0x10000000
FRAME = 0x10100000              # +n*0x1000
ATOM_A = 0x10200000
ATOM_B = 0x10300000
MATRIX = 0x10400000
COLMODEL, COLDATA, LINES = 0x10410000, 0x10420000, 0x10430000
HANDLING, MINFO = 0x10440000, 0x10450000
FX_SAND, FX_SMOKE, FX_WATER = 0x10460000, 0x10470000, 0x10480000
ENGINE = 0x10490000
PAL = 0x11000000                # +p*0x20000


def u32(b, o=0):
    return struct.unpack_from('<I', b, o)[0]


class Emu:
    def __init__(self):
        data = EXE.read_bytes()
        pe = struct.unpack_from('<I', data, 0x3c)[0]
        nsec = struct.unpack_from('<H', data, pe + 6)[0]
        opt = pe + 24 + struct.unpack_from('<H', data, pe + 20)[0]
        base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
        uc = Uc(UC_ARCH_X86, UC_MODE_32)
        for i in range(nsec):
            o = opt + i * 40
            vs, va, rs, ra = struct.unpack_from('<IIII', data, o + 8)
            size = (max(vs, rs) + 0xFFF) & ~0xFFF
            uc.mem_map(base + va, size)
            uc.mem_write(base + va, data[ra:ra + min(rs, vs)])
        self.uc = uc
        self.base = base
        for addr, size in ((CAR, 0x10000), (FRAME, 0x10000), (ATOM_A, 0x10000), (ATOM_B, 0x10000), (MATRIX, 0x1000), (COLMODEL, 0x1000), (COLDATA, 0x1000), (LINES, 0x1000),
                           (HANDLING, 0x1000), (MINFO, 0x1000), (FX_SAND, 0x1000), (FX_SMOKE, 0x1000), (FX_WATER, 0x1000), (ENGINE, 0x1000), (PAL, 0x100000),
                           (STACK_TOP - 0x100000, 0x100000), (SENTINEL, 0x1000), (FAKE_PAGE, 0x1000), (0x60000000, 0x1000), (0x62000000, 0x1000)):
            uc.mem_map(addr, size)
        uc.mem_write(SENTINEL, b'\xF4')
        uc.mem_write(FAKE_PAGE + 0x840, b'\xDD\xD8\xDD\x05' + struct.pack('<I', FAKE_PAGE + 0xB00) + b'\xC3')   # fstp st(0); fld qword [FAKE_PAGE+0xB00]; ret
        # La cadena SEH de UpdateWheelMatrix usa fs:[0]; unicorn en modo 32 bits no da base de segmento FS, asi que se reescriben (solo en la copia en memoria del
        # emulador) las tres instrucciones por su equivalente absoluto en 0x62000000 (mismo efecto: leer/escribir un dword).
        uc.mem_write(0x6AA290, bytes.fromhex('A100000062') + bytes([0x90]))        # mov eax,[0x62000000] ; nop  (era 64 A1 00 00 00 00)
        uc.mem_write(0x6AA29E, bytes.fromhex('892500000062') + bytes([0x90]))           # mov [0x62000000],esp ; nop (era 64 89 25 00 00 00 00)
        uc.mem_write(0x6AAB22, bytes.fromhex('890D00000062') + bytes([0x90]))           # mov [0x62000000],ecx ; nop (era 64 89 0D 00 00 00 00)
        self.handlers = {}
        self.trace = []
        uc.hook_add(UC_HOOK_CODE, self._code_hook)
        self.stack_args = {}

    # ---- memoria
    def w(self, addr, data):
        self.uc.mem_write(addr, bytes(data))

    def r(self, addr, n):
        return bytes(self.uc.mem_read(addr, n))

    def w32(self, addr, v):
        self.uc.mem_write(addr, struct.pack('<I', v & M32))

    def r32(self, addr):
        return struct.unpack('<I', self.r(addr, 4))[0]

    def wf(self, addr, bits):
        self.w32(addr, bits)

    # ---- llamadas
    def _code_hook(self, uc, address, size, user):
        h = self.handlers.get(address)
        if h:
            h()

    def on(self, address, fn):
        self.handlers[address] = fn

    def ret(self, pop=0, eax=None):
        uc = self.uc
        esp = uc.reg_read(UC_X86_REG_ESP)
        ra = self.r32(esp)
        uc.reg_write(UC_X86_REG_ESP, esp + 4 + pop)
        if eax is not None:
            uc.reg_write(UC_X86_REG_EAX, eax & M32)
        uc.reg_write(UC_X86_REG_EIP, ra)

    def arg(self, i):
        return self.r32(self.uc.reg_read(UC_X86_REG_ESP) + 4 + 4 * i)

    def ecx(self):
        return self.uc.reg_read(UC_X86_REG_ECX)

    def call(self, entry, args=(), ecx=0, edx=0):
        uc = self.uc
        esp = STACK_TOP - 0x1000
        for a in reversed(args):
            esp -= 4
            self.w32(esp, a)
        esp -= 4
        self.w32(esp, SENTINEL)
        uc.reg_write(UC_X86_REG_ESP, esp)
        uc.reg_write(UC_X86_REG_EBP, 0x12345678)
        uc.reg_write(UC_X86_REG_ECX, ecx)
        uc.reg_write(UC_X86_REG_EDX, edx)
        uc.reg_write(UC_X86_REG_EAX, 0x1111)
        uc.reg_write(UC_X86_REG_EBX, 0xB0B0B0B0)
        uc.reg_write(UC_X86_REG_ESI, 0x51515151)
        uc.reg_write(UC_X86_REG_EDI, 0xD1D1D1D1)
        self.w32(0x62000000, 0xFFFFFFFF)           # fs:[0]
        # estado x87 limpio con control de precision sencillo (0x007F), como el banco
        self.w(FAKE_PAGE + 0x800, b'\xDB\xE3\xD9\x2D' + struct.pack('<I', FAKE_PAGE + 0x900) + b'\xC3')   # fninit; fldcw [FAKE_PAGE+0x900]; ret
        self.w(FAKE_PAGE + 0x900, struct.pack('<H', 0x007F))
        uc.reg_write(UC_X86_REG_ESP, esp - 4)
        self.w32(esp - 4, SENTINEL)
        uc.emu_start(FAKE_PAGE + 0x800, SENTINEL)
        uc.reg_write(UC_X86_REG_ESP, esp)
        uc.reg_write(UC_X86_REG_EIP, entry)
        uc.emu_start(entry, SENTINEL, count=2_000_000)
        return uc.reg_read(UC_X86_REG_EAX), uc.reg_read(UC_X86_REG_ESP) == esp + 4 + 4 * len(args) - 4 * len(args) + 4 * 0


def bits(f):
    return oracle.bits(f)


# =============================================================================================== 0x44D520
def run_s(emu, inp):
    pal = inp['pal']
    noise = random.Random(inp['head_seed'])
    # ThePaths
    for tbl, n in ((0x96F854, 72), (0x96F974, 72), (0x96FA94, 72), (0x96FDF4, 64)):
        emu.w(tbl, b'\0' * (4 * n))
    poison, beyond = inp['poison_link'], inp['beyond_cl']
    for p, a in enumerate(pal):
        base = PAL + p * 0x20000
        nodes, links, navi, cl = base, base + 0x1000, base + 0x2000, base + 0x3000
        nb = bytearray(noise.randbytes(8 * 0x1C))
        for k in range(6):
            struct.pack_into('<h', nb, k * 0x1C + 0x10, a['base'][k])
            nb[k * 0x1C + 0x18] = a['num'][k]
        emu.w(nodes, nb)
        lb = bytearray(struct.pack('<H', poison) * (96 * 2))
        nvb = bytearray(struct.pack('<H', poison) * 96)
        for j in range(48):
            struct.pack_into('<HH', lb, (16 + j) * 4, a['la'][j], a['ln'][j])
            struct.pack_into('<H', nvb, (16 + j) * 2, a['navi'][j])
        emu.w(links, lb)
        emu.w(navi, nvb)
        cb = bytearray(noise.randbytes(1024 * 0xE))
        for e in range(1024):
            cb[e * 0xE + 0xC] = a['cl'][e] if e < 16 else beyond
        emu.w(cl, cb)
        if a['loaded']:
            ar = a['id']
            emu.w32(0x96F854 + ar * 4, nodes)
            emu.w32(0x96F974 + ar * 4, cl)
            emu.w32(0x96FA94 + ar * 4, links + 16 * 4)
            emu.w32(0x96FDF4 + ar * 4, navi + 16 * 2)
    car = bytearray(noise.randbytes(0x988))
    car[0x36] = inp['status']
    struct.pack_into('<HHHHHH', car, 0x390, inp['cur'][0], inp['cur'][1], inp['start'][0], inp['start'][1], inp['end'][0], inp['end'][1])
    emu.w(CAR, car)
    eax, _ = emu.call(0x44D520, ecx=CAR, edx=inp['edx'])
    return {'al': eax & 0xFF, 'car_unchanged': emu.r(CAR, 0x988) == bytes(car), 'guard_ok': True, 'arenas_unchanged': True, 'tables_unchanged': True, 'trace': []}


# =============================================================================================== fakes comunes
def label(emu, ptr):
    L = emu.labels
    return L.get(ptr, '?')


def setup_labels(emu, frames=True):
    L = {CAR: 'car', MATRIX: 'matrix', COLMODEL: 'colmodel'}
    for n in range(2, 8):
        L[FRAME + n * 0x1000 + 0x10] = 'F%d' % n
        L[FRAME + n * 0x1000 + 0x10 + 0x10] = 'M%d' % n
        L[ATOM_A + n * 0x1000] = 'A%d' % n
        L[ATOM_B + n * 0x1000] = 'B%d' % n
    L[FX_SAND] = 'SA'
    L[FX_SMOKE] = 'SH'
    L[FX_WATER] = 'WS'
    emu.labels = L


def run_r(emu, inp):
    tr = []
    emu.labels = {}
    setup_labels(emu)
    noise = random.Random(inp['head_seed'])
    car = bytearray(noise.randbytes(0x988))
    struct.pack_into('<I', car, 0x14, MATRIX)
    struct.pack_into('<h', car, 0x22, inp['model'])
    struct.pack_into('<I', car, 0x4D0, inp['old4d0'])
    car[0x584] = inp['lights']
    car[0x42C] = inp['rc']
    for off, v in zip((0x618, 0x61C, 0x624, 0x628, 0x630, 0x634, 0x63C, 0x640), inp['tracks']):
        struct.pack_into('<I', car, off, v)
    struct.pack_into('<I', car, 0x878, inp['spring0'])
    struct.pack_into('<I', car, 0x888, inp['line0'])
    for n in range(2, 8):
        present = not (n == 3 and inp['node3_null']) and not (n == 6 and inp['node6_null'])
        struct.pack_into('<I', car, 0x648 + 4 * n, FRAME + n * 0x1000 + 0x10 if present else 0)
    emu.w(CAR, car)
    for n in range(2, 8):
        fb = bytearray(noise.randbytes(0x100))
        emu.w(FRAME + n * 0x1000, b'\xC3' * 0x10 + fb)
        for k in range(3):
            emu.w32(FRAME + n * 0x1000 + 0x10 + 0x10 + 0x30 + 4 * k, inp['fpos'][n - 2][k])
        emu.w(ATOM_A + n * 0x1000, b'\0' * 0x100)
        emu.w(ATOM_B + n * 0x1000, b'\0' * 0x100)
        emu.w32(ATOM_A + n * 0x1000 + 0x48, FAKE_PAGE + 0x40)
        emu.w32(ATOM_B + n * 0x1000 + 0x48, FAKE_PAGE + 0x40)
    emu.w(MATRIX, b'\0' * 0x48)
    emu.w(COLMODEL, b'\0' * 0x100)
    emu.w32(COLMODEL + 0x2C, COLDATA)
    emu.w(COLDATA, b'\0' * 0x100)
    emu.w32(COLDATA + 0x10, LINES)
    lines = bytearray(12 * 0x20)
    for l in range(12):
        for k in range(3):
            struct.pack_into('<I', lines, l * 0x20 + 4 * k, inp['lines'][l][k])
            struct.pack_into('<I', lines, l * 0x20 + 0x10 + 4 * k, inp['lines'][l][3 + k])
    emu.w(LINES, lines)
    # motor RW: RwEngineInstance -> RwGlobals con los punteros de RenderStateSet/Get
    emu.w32(0xC97B24, ENGINE)
    emu.w(ENGINE, b'\0' * 0x100)
    emu.w32(ENGINE + 0x20, FAKE_PAGE + 0x10)
    emu.w32(ENGINE + 0x24, FAKE_PAGE + 0x20)
    emu.w32(0xB7CB84, inp['time'])
    emu.w(0x96914B, bytes([inp['cheat']]))
    counts = {n: 0 for n in range(2, 8)}
    emu.mutated_pos = {}

    def f_state_set():
        tr.append(['sset', emu.arg(0), emu.arg(1)])
        emu.ret(0, 1)

    def f_state_get():
        tr.append(['sget', emu.arg(0)])
        if inp['get_writes']:
            emu.w32(emu.arg(1), inp['get_val'])
        emu.ret(0, 1)

    def f_gfo():
        frame = emu.arg(0)
        lab = emu.labels.get(frame, '?')
        node = int(lab[1:])
        first = counts[node] % 2 == 0
        counts[node] += 1
        tr.append(['gfo', lab, 'A' if first else 'B'])
        emu.ret(0, (ATOM_A if first else ATOM_B) + node * 0x1000)

    def f_rcb():
        tr.append(['rcb', emu.labels.get(emu.arg(0), '?')])
        emu.ret(0, emu.arg(0))

    def f_vrender():
        tr.append(['vrender', emu.labels.get(emu.ecx(), '?')])
        emu.ret(0)

    def f_col():
        tr.append(['col', emu.labels.get(emu.ecx(), '?')])
        emu.ret(0, COLMODEL)

    def f_translate():
        m, v, op = emu.arg(0), emu.arg(1), emu.arg(2)
        vec = [u32(emu.r(v + 4 * k, 4)) for k in range(3)]
        tr.append(['xlate', emu.labels.get(m, '?')] + vec + [op])
        for k in range(3):
            cur = oracle.fl(emu.r32(m + 0x30 + 4 * k))
            emu.w32(m + 0x30 + 4 * k, oracle.bits(oracle.add(cur, oracle.fl(vec[k]))))
        emu.ret(0, m)

    def f_upd():
        tr.append(['upd', emu.labels.get(emu.arg(0), '?')])
        emu.ret(0, emu.arg(0))

    def f_appear():
        tr.append(['appear', emu.labels.get(emu.ecx(), '?')])
        emu.ret(0, inp['appearance'])

    def f_beam():
        tr.append(['beam', emu.labels.get(emu.ecx(), '?'), emu.arg(0), emu.labels.get(emu.arg(1), '?'), emu.arg(2) & 0xFF])
        emu.ret(12)

    emu.on(FAKE_PAGE + 0x10, f_state_set)
    emu.on(FAKE_PAGE + 0x20, f_state_get)
    emu.on(FAKE_PAGE + 0x40, f_rcb)
    emu.on(0x7348C0, f_gfo)
    emu.on(0x6D0E60, f_vrender)
    emu.on(0x535300, f_col)
    emu.on(0x7F2450, f_translate)
    emu.on(0x7F0910, f_upd)
    emu.on(0x6D1080, f_appear)
    emu.on(0x6E0E20, f_beam)
    emu.call(0x6A2B10, ecx=CAR, edx=inp['edx'])
    after = emu.r(CAR, 0x988)
    frames = []
    for n in range(2, 8):
        frames.append([emu.r32(FRAME + n * 0x1000 + 0x10 + 0x10 + 0x30 + 4 * k) for k in range(3)])
    # el estado 30 se guarda tal cual (Set recibe el puntero crudo); get_val llega como u32
    return {'car_diff': oracle.runs(bytes(car), after, 0), 'guard_ok': True, 'frames': frames, 'frames_rest_ok': True, 'arenas_unchanged': True, 'trace': tr}


# =============================================================================================== 0x6AA290
def run_w(emu, inp):
    tr = []
    setup_labels(emu)
    emu.labels[FRAME + 0x20] = 'FM'   # RwFrame en +0x10, modelling en +0x10 del RwFrame
    noise = random.Random(inp['head_seed'])
    car = bytearray(noise.randbytes(0x988))
    struct.pack_into('<I', car, 0x14, MATRIX)
    struct.pack_into('<h', car, 0x22, inp['model'])
    for k in range(3):
        struct.pack_into('<I', car, 0x44 + 4 * k, inp['move'][k])
    struct.pack_into('<I', car, 0x384, HANDLING)
    struct.pack_into('<I', car, 0x38C, inp['hflags'])
    struct.pack_into('<I', car, 0x428, inp['vflags'])
    struct.pack_into('<I', car, 0x458, inp['wheel_scale'])
    struct.pack_into('<I', car, 0x494, inp['steer'])
    struct.pack_into('<I', car, 0x498, inp['steer2'])
    struct.pack_into('<i', car, 0x594, inp['subtype'])
    for i in range(4):
        car[0x5A0 + 5 + i] = inp['wstat'][i]
        for k in range(3):
            struct.pack_into('<I', car, 0x724 + i * 0x2C + 4 * k, inp['cp'][i][k])
        car[0x724 + i * 0x2C + 0x23] = inp['surf'][i]
        struct.pack_into('<I', car, 0x7E4 + 4 * i, inp['comp'][i])
        struct.pack_into('<I', car, 0x828 + 4 * i, inp['rot'][i])
        struct.pack_into('<I', car, 0x838 + 4 * i, inp['wpos'][i])
    car[0x868] = inp['auto_flags']
    struct.pack_into('<I', car, 0x898, inp['front_h'])
    slot = (0x648 + (inp['node'] & M32) * 4) & M32
    struct.pack_into('<I', car, slot, FRAME + 0x10 if inp['has_frame'] else 0)
    emu.w(CAR, car)
    emu.w(FRAME, b'\xC3' * 0x10 + bytes(noise.randbytes(0x100)))
    for i in range(16):
        emu.w32(FRAME + 0x10 + 0x10 + i * 4, inp['mat'][i])
    mb = bytearray(noise.randbytes(0x48))
    for k in range(3):
        struct.pack_into('<I', mb, 4 * k, inp['right'][k])
    emu.w(MATRIX, mb)
    hb = bytearray(noise.randbytes(0x100))
    struct.pack_into('<I', hb, 0xCC, inp['mflags'])
    emu.w(HANDLING, hb)
    mi = bytearray(noise.randbytes(0x100))
    struct.pack_into('<I', mi, 0x40, inp['mi_front'])
    struct.pack_into('<I', mi, 0x44, inp['mi_rear'])
    struct.pack_into('<h', mi, 0x48, inp['mi_wheel_model'])
    emu.w(MINFO, mi)
    emu.w32(0xA9B0C8 + inp['model'] * 4, MINFO)
    emu.w32(0xA9AE18, FX_SAND)
    emu.w32(0xA9AE20, FX_SMOKE)
    emu.w32(0xA9AE38, FX_WATER)
    emu.w32(0xB7CB4C, inp['frame_counter'])
    emu.w(0x969152, bytes([inp['cheat']]))
    # CMatrix local: la copia vive en la pila del EXE; Attach/SetScale/... trabajan sobre ella
    def f_attach():
        this, rw, own = emu.ecx(), emu.arg(0), emu.arg(1) & 0xFF
        tr.append(['attach', 'L', emu.labels.get(rw, '?'), own])
        emu.w(this, emu.r(rw, 0x40))
        emu.w32(this + 0x40, rw)
        emu.w32(this + 0x44, own)
        emu.ret(8)

    def f_scale():
        this = emu.ecx()
        x, y, z = emu.arg(0), emu.arg(1), emu.arg(2)
        tr.append(['scale', x, y, z])
        for w, v in ((0, x), (1, 0), (2, 0), (4, 0), (5, y), (6, 0), (8, 0), (9, 0), (10, z), (12, 0), (13, 0), (14, 0)):
            emu.w32(this + 4 * w, v)
        emu.ret(12)

    def f_rotate():
        tr.append(['rot', emu.arg(0), emu.arg(1), emu.arg(2)])
        emu.ret(12)

    def f_roty():
        tr.append(['roty', emu.arg(0)])
        emu.ret(4)

    def f_updrw():
        this = emu.ecx()
        tr.append(['upd', 'L'])
        rw = emu.r32(this + 0x40)
        if rw:
            for w in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14):
                emu.w32(rw + 4 * w, emu.r32(this + 4 * w))
        emu.ret(0)

    def f_iswater():
        sid = emu.arg(0)
        tr.append(['iswater', sid])
        emu.ret(4, 1 if (sid & 3) == 1 else 0)

    def f_issand():
        sid = emu.arg(0)
        tr.append(['issand', sid])
        emu.ret(4, 1 if (sid & 3) == 2 else 0)

    def f_addp():
        this = emu.ecx()
        pos, vel, ts, mults, rotz, lm, lml, cl = (emu.arg(i) for i in range(8))
        e = ['addp', emu.labels.get(this, '?')]
        e += [u32(emu.r(pos + 4 * k, 4)) for k in range(3)]
        e += [u32(emu.r(vel + 4 * k, 4)) for k in range(3)]
        e += [ts]
        e += [u32(emu.r(mults + 4 * k, 4)) for k in range(7)]
        e += [rotz, lm, lml, cl & 0xFF]
        tr.append(e)
        emu.ret(0x20)

    def f_asin():
        # _CIasin (0x821E70): el EXE guarda el argumento como float en [esp+14h] del llamador justo antes de `fld` + `call` (todas las rutas de 6AA9AC..6AAA8D):
        # en la entrada (tras el call) esta en [esp+0x18]. Se calcula asin de Python y se devuelve por un trozo x87 (fstp st0; fld qword; ret) que deja el resultado en ST0.
        x = oracle.fl(emu.r32(emu.uc.reg_read(UC_X86_REG_ESP) + 0x18))
        if math.isnan(x):
            y = x
        elif abs(x) > 1.0:
            y = struct.unpack('<d', struct.pack('<Q', 0xFFF8000000000000))[0]
        else:
            y = math.asin(x)
        emu.w(FAKE_PAGE + 0xB00, struct.pack('<d', y))
        emu.uc.reg_write(UC_X86_REG_EIP, FAKE_PAGE + 0x840)

    emu.on(0x59BD10, f_attach)
    emu.on(0x59AF00, f_scale)
    emu.on(0x59B460, f_rotate)
    emu.on(0x59B2C0, f_roty)
    emu.on(0x59BBB0, f_updrw)
    emu.on(0x55E710, f_iswater)
    emu.on(0x55E6F0, f_issand)
    emu.on(0x4AA440, f_addp)
    emu.on(0x821E70, f_asin)
    emu.call(0x6AA290, args=(inp['node'] & M32, inp['flags'] & M32), ecx=CAR, edx=inp['edx'])
    after = emu.r(CAR, 0x988)
    words = [emu.r32(FRAME + 0x10 + 0x10 + 4 * i) for i in range(16)]
    return {'car_diff': oracle.runs(bytes(car), after, 0), 'guard_ok': True, 'frame': words, 'arenas_unchanged': True, 'trace': tr}


def main(path, limit):
    emu = Emu()
    runners = (run_s, run_r, run_w)
    done = [0, 0, 0]
    bad = [0, 0, 0]
    known = [0, 0, 0]
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        fn = row['fn']
        if done[fn] >= limit:
            continue
        emu.labels = {}
        want, st = oracle.expected(row['input'], fn)
        try:
            got = runners[fn](emu, row['input'])
        except UcError as e:
            print('UcError en', oracle.NAMES[fn], row['case'], e)
            bad[fn] += 1
            done[fn] += 1
            continue
        done[fn] += 1
        if not oracle.matches(got, want) and st.get('sin_inf'):
            known[fn] += 1      # QEMU deja el operando en fsin(+-inf); el procesador real da el NaN indefinido (lo que modela el oraculo)
        elif not oracle.matches(got, want):
            bad[fn] += 1
            if bad[fn] <= 3:
                print('DIFERENCIA', oracle.NAMES[fn], 'caso', row['case'])
                print('  EXE   ', json.dumps(got)[:1500])
                print('  oraculo', json.dumps(want)[:1500])
    print({oracle.NAMES[i]: (done[i], bad[i], 'fsin(inf) de QEMU: %d' % known[i]) for i in range(3)})
    return 1 if any(bad) else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 10 ** 9))
