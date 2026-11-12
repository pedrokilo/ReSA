"""Oraculo independiente (Python) de Fn168A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 168A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE: 0x86F878 CEventLeanOnVehicle, 0x86F8B8 CEventPassObject, 0x86D0A0 CEventNewGangMember, ranuras 0 dtor, 1 GetEventType,
2 GetEventPriority, 3 GetLifeTime, 4 Clone, 5 AffectsPed).
   0  0x65DB50  LeanOnVehicle::GetEventType: mov eax,5Ch                  1  0x65DB60  GetLifeTime: xor eax,eax
   2  0x65DB70  LeanOnVehicle::Clone: push 14h; call 4B5620 (CEvent::operator new; ecx=this guardado); si EAX != 0: push [esi+10h]; push [esi+0Ch]; ecx = EAX; call 65DAF0; (eax = el del ctor) ; si 0: eax = 0
   3  0x65DBE0  AffectsPed: mov al,1; ret 4           4  0x65DBF0  scalar deleting dtor: call 65DC10; si byte [esp+8] & 1: push this; call 4B5630; eax = this; ret 4
   5  0x65DC10  cuerpo del destructor (SEH): [this] = 86F878h; ecx = [this+0Ch]; si != 0: push this+0Ch; call 571A00 (ecx = [this+0Ch]); ecx = this; call 4AC010
   6  0x65DE10  LeanOnVehicle::GetEventPriority: mov eax,0Ch
   7  0x65DD50  PassObject::GetEventType: mov eax,59h                     8  0x65DD60  GetLifeTime: xor eax,eax
   9  0x65DD70  PassObject::Clone: igual que 2 pero push BYTE [esi+10h] extendido a cero; push [esi+0Ch]; call 65DC70
  10  0x65DDE0  AffectsPed: mov al,1; ret 4          11  0x65DDF0  scalar deleting dtor: call 65DCF0 (cuerpo, con hook); mismo borrado
  12  0x65DE20  PassObject::GetEventPriority: mov eax,0Bh
  13  0x608FD0  NewGangMember::GetEventType: mov eax,5Ah                  14  0x608FE0  GetLifeTime: xor eax,eax
  15  0x608FF0  NewGangMember::Clone: push 10h; call 4B5620; si EAX != 0: push [esi+0Ch]; ecx = EAX; call 608F70
  16  0x609050  AffectsPed: mov al,1; ret 4          17  0x609060  scalar deleting dtor: call 609080; mismo borrado
  18  0x609080  cuerpo del destructor (SEH): [this] = 86D0A0h; igual que 5
  19  0x65DE60  CEntitySeekPosCalculatorXYOffset ctor: edx = arg; eax = ecx; [eax] = 86F8F8h; esi = [edx]; [eax+4] = esi; esi = [edx+4]; [eax+8] = esi; edx = [edx+8]; [eax+0Ch] = edx; ret 4 (eax = this)
  20  0x65DE90  mov dword [ecx],859DC4h
  21  0x605300  CEventScanner ctor: [esi+4],[esi+8] = 0; bytes [esi+0Ch],[esi+0Dh] = 0; [esi+10h],[esi+14h] = 0; bytes [esi+18h],[esi+19h] = 0; ecx = esi+1Ch: byte [ecx] = 1, [ecx+4],[ecx+8] = 0, bytes [ecx+0Ch],[ecx+0Dh] = 0,
             [ecx+10h],[ecx+14h] = 0; call 5FFF90 (Clear); [esi+0ACh],[esi+0B0h] = 0; bytes +0B4h,+0B5h = 0; byte +0B8h = 1; bytes +0B9h,+0BAh = 0; [esi+0BCh],[esi+0C0h] = 0; bytes +0C4h,+0C5h = 0; [esi+0C8h],[esi+0CCh] = 0;
             bytes +0D0h,+0D1h = 0; edi = [0B7CB84h]; call 821B1E (rand); and eax,0FFFFh; fild; fmul [858B14h] (2^-15); fmul [859A90h] (-3000.0); call 821B40 (_ftol); [esi] = edi - eax; eax = esi
  22  0x6070F0  CopyDecisionMaker (thiscall, 3 dwords de pila): eax = arg1; si arg1 == -1: si arg2 == 0: arg1' = this+0CB50h, si no this+0E824h; si no: arg1' = arg1 * 99Ch + this + 4; jmp 607050 (AddDecisionMaker)

Los falsos anotan: ~CEvent y cuerpos de destructor [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; operator delete [0x4B5630, puntero (etiqueta), dword del puntero]; operator new [0x4B5620, tamano];
constructores [direccion, ECX (etiqueta), argumentos (el primero como etiqueta, el segundo dword completo)]; CleanUpOldReference [0x571A00, ECX (etiqueta), argumento (etiqueta)]; Clear [0x5FFF90, ECX, 6 dwords]; rand [0x821B1E];
AddDecisionMaker [0x607050, ECX, origen (etiqueta), tipo, byte bajo de mision]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CEventLeanOnVehicle::GetEventType", "CEventLeanOnVehicle::GetLifeTime", "CEventLeanOnVehicle::Clone", "CEventLeanOnVehicle::AffectsPed", "CEventLeanOnVehicle::ScalarDeletingDestructor",
         "CEventLeanOnVehicle::DestructorBody", "CEventLeanOnVehicle::GetEventPriority",
         "CEventPassObject::GetEventType", "CEventPassObject::GetLifeTime", "CEventPassObject::Clone", "CEventPassObject::AffectsPed", "CEventPassObject::ScalarDeletingDestructor",
         "CEventPassObject::GetEventPriority",
         "CEventNewGangMember::GetEventType", "CEventNewGangMember::GetLifeTime", "CEventNewGangMember::Clone", "CEventNewGangMember::AffectsPed", "CEventNewGangMember::ScalarDeletingDestructor",
         "CEventNewGangMember::DestructorBody",
         "CEntitySeekPosCalculatorXYOffset::Constructor", "CEntitySeekPosCalculator::DestructorCopy2", "CEventScanner::Constructor", "CDecisionMakerTypes::CopyDecisionMaker"]
ENTRIES = (0x65DB50, 0x65DB60, 0x65DB70, 0x65DBE0, 0x65DBF0, 0x65DC10, 0x65DE10,
           0x65DD50, 0x65DD60, 0x65DD70, 0x65DDE0, 0x65DDF0, 0x65DE20,
           0x608FD0, 0x608FE0, 0x608FF0, 0x609050, 0x609060, 0x609080,
           0x65DE60, 0x65DE90, 0x605300, 0x6070F0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
OPNEW, OPDELETE, CLEANUP, EVDTOR, CLEAR, RAND, ADDDM = 0x4B5620, 0x4B5630, 0x571A00, 0x4AC010, 0x5FFF90, 0x821B1E, 0x607050

CONST, CONSTAL, CLONE, SDD, EVBODY, XYCTOR, VTONLY, SCANNER, COPYDM = 'const constal clone sdd evbody xyctor vtonly scanner copydm'.split()
KIND = [CONST, CONST, CLONE, CONSTAL, SDD, EVBODY, CONST,
        CONST, CONST, CLONE, CONSTAL, SDD, CONST,
        CONST, CONST, CLONE, CONSTAL, SDD, EVBODY,
        XYCTOR, VTONLY, SCANNER, COPYDM]
CONSTS = {0: 0x5C, 1: 0, 6: 0xC, 7: 0x59, 8: 0, 12: 0xB, 13: 0x5A, 14: 0}
CLONE_INFO = {2: (0x65DAF0, 2, 0x14), 9: (0x65DC70, 2, 0x14), 15: (0x608F70, 1, 0x10)}      # constructor, argumentos, tamano
SDD_BODY = {4: 0x65DC10, 11: 0x65DCF0, 17: 0x609080}
EVBODY_VT = {5: 0x86F878, 18: 0x86D0A0}
VT_ONLY = {20: 0x859DC4}
XY_VT = 0x86F8F8


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


class World:
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14]))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


KEYS = {}
for _fn in range(23):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind == CLONE:
        _k += ['mem_null', 'mem_in_b', 'ctor_ret_mem', 'ctor_ret_mem4', 'ctor_ret_zero', 'ctor_ret_raw']
        if _fn == 9:
            _k += ['byte_zero', 'byte_nonzero', 'byte_high_bits']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'prev_zero', 'prev_other']
    elif _kind == EVBODY:
        _k += ['ref_null', 'ref_set', 'prev_zero', 'prev_other']
    elif _kind == XYCTOR:
        _k += ['src_b', 'src_a', 'src_overlap', 'src_adjacent', 'src_unaligned', 'src_aligned', 'snan_copied', 'negzero_copied', 'prev_zero', 'prev_other']
    elif _kind == VTONLY:
        _k += ['prev_zero', 'prev_other']
    elif _kind == SCANNER:
        _k += ['rand_zero', 'rand_high_bits', 'rand_low16_max', 'time_zero', 'time_high', 'time_wraps', 'product_rounded', 'delta_zero']
    elif _kind == COPYDM:
        _k += ['idx_minus1', 'idx_slot', 'idx_big', 'type_zero', 'type_nonzero', 'mission_zero', 'mission_nonzero', 'src_default_random', 'src_default_group', 'src_slot']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    return ['kind%d' % kind, 'fill%d' % fill, 'this_unaligned' if a[0] % 4 else 'this_aligned']


def f32(x):
    """redondeo a float de 32 bits (al mas cercano, par en empates), como cada operacion x87 a 24 bits de mantisa o cada operacion SSE de precision simple"""
    return struct.unpack('<f', struct.pack('<f', x))[0]


def scanner_delta(rand_value):
    """lo que resta el constructor del tiempo: ftol(((rand & 0xFFFF) * 2^-15) * -3000.0) con cada producto redondeado a float; devuelve (entero, producto exacto no representable?)"""
    x = float(rand_value & 0xFFFF)
    p1 = f32(x * 3.0517578125e-05)
    exact = p1 * -3000.0
    p2 = f32(exact)
    return int(p2), p2 != exact          # int() trunca hacia cero, como _ftol


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTS[fn]
    elif k == CONSTAL:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 1                                              # mov al,1: solo AL
    elif k == VTONLY:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, VT_ONLY[fn])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), a[3]])
        A.wr(o, 4, a[5])                                     # el falso del cuerpo deja gDtorWrite en [this]
        flags = a[6] & 0xFF                                  # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([OPDELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        ret = W.tagv(this)
    elif k == EVBODY:
        A.wr(o, 4, a[3])
        A.wr(o + 0xC, 4, a[6])
        W.snap()
        ref = a[6]
        if ref:
            events.append([CLEANUP, W.tagv(ref), W.tagv((this + 0xC) & M32)])
        events.append([EVDTOR, W.tagv(this), EVBODY_VT[fn]])      # ~CEvent ve la vtable que dejo el cuerpo
        A.wr(o, 4, a[5])
        marks.append('ref_set' if ref else 'ref_null')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLONE:
        ctor, nargs, size = CLONE_INFO[fn]
        A.wr(o + 0xC, 4, a[8])
        A.wr(o + 0x10, 4, a[9])
        mem = 0 if a[3] == 0 else W.raw(1, a[4])
        W.snap()
        events.append([OPNEW, size])
        marks.append('mem_null' if mem == 0 else 'mem_in_b')
        if mem == 0:
            ret = 0
        else:
            arg2 = a[9] & 0xFF if fn == 9 else a[9]                # PassObject: BYTE de +0x10 extendido a cero
            if nargs == 2:
                events.append([ctor, W.tagv(mem), W.tagv(a[8]), arg2])
            else:
                events.append([ctor, W.tagv(mem), W.tagv(a[8])])
            B.wr(a[4], 4, a[5])                                    # el falso del constructor deja gDtorWrite en [memoria]
            sel = a[7]
            ret_eax = (mem if sel == 0 else (mem + 4 if sel == 1 else (0 if sel == 2 else a[6]))) & M32
            ret = W.tagv(ret_eax)
            marks.append(['ctor_ret_mem', 'ctor_ret_mem4', 'ctor_ret_zero', 'ctor_ret_raw'][sel])
            if fn == 9:
                marks.append('byte_zero' if (a[9] & 0xFF) == 0 else 'byte_nonzero')
                if a[9] >> 8:
                    marks.append('byte_high_bits')
    elif k == XYCTOR:
        A.wr(o, 4, a[3])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        if a[4] == 0:
            B.wr(a[5], 4, a[6]); B.wr(a[5] + 4, 4, a[7]); B.wr(a[5] + 8, 4, a[8])
            src = W.raw(1, a[5])
            marks.append('src_b')
            marks.append('src_unaligned' if a[5] % 4 else 'src_aligned')
        else:
            delta = a[5] - (1 << 32) if a[5] & 0x80000000 else a[5]
            so = o + delta
            A.wr(so, 4, a[6]); A.wr(so + 4, 4, a[7]); A.wr(so + 8, 4, a[8])
            src = W.raw(0, so)
            marks.append('src_a')
            marks.append('src_overlap' if delta > -12 and delta < 16 else 'src_adjacent')
            marks.append('src_unaligned' if so % 4 else 'src_aligned')
        if any(v in (0x7FA00000, 0xFFA00001) for v in a[6:9]):
            marks.append('snan_copied')
        if 0x80000000 in a[6:9]:
            marks.append('negzero_copied')
        W.snap()
        A.wr(o, 4, XY_VT)                                          # la vtable se escribe ANTES de leer el origen (puede solaparse con el objeto)
        for i in range(3):
            A.wr(o + 4 + 4 * i, 4, W.rd((src + 4 * i) & M32, 4))
        ret = W.tagv(this)
    elif k == SCANNER:
        W.snap()
        for off, n in [(4, 4), (8, 4), (0xC, 1), (0xD, 1), (0x10, 4), (0x14, 4), (0x18, 1), (0x19, 1)]:
            A.wr(o + off, n, 0)
        A.wr(o + 0x1C, 1, 1)
        for off, n in [(0x20, 4), (0x24, 4), (0x28, 1), (0x29, 1), (0x2C, 4), (0x30, 4)]:
            A.wr(o + off, n, 0)
        events.append([CLEAR, W.tagv((this + 0x1C) & M32)] + [A.rd(o + 0x1C + 4 * i, 4) for i in range(6)])
        for off, n in [(0xAC, 4), (0xB0, 4), (0xB4, 1), (0xB5, 1), (0xB8, 1), (0xB9, 1), (0xBA, 1), (0xBC, 4), (0xC0, 4), (0xC4, 1), (0xC5, 1), (0xC8, 4), (0xCC, 4), (0xD0, 1), (0xD1, 1)]:
            A.wr(o + off, n, 1 if off == 0xB8 else 0)
        events.append([RAND])
        delta, rounded = scanner_delta(a[4])
        value = (a[3] - delta) & M32
        A.wr(o, 4, value)
        ret = W.tagv(this)
        marks.append('rand_zero' if (a[4] & 0xFFFF) == 0 else ('rand_low16_max' if (a[4] & 0xFFFF) == 0xFFFF else 'rand_mid'))
        if a[4] >> 16:
            marks.append('rand_high_bits')
        marks.append('time_zero' if a[3] == 0 else 'time_high' if a[3] >= 0x80000000 else 'time_low')
        if value < a[3]:
            marks.append('time_wraps')
        if rounded:
            marks.append('product_rounded')
        if delta == 0:
            marks.append('delta_zero')
    elif k == COPYDM:
        W.snap()
        idx, typ, mission = a[3], a[4], a[5]
        if idx == M32:
            src = (this + (0xCB50 if typ == 0 else 0xE824)) & M32
            marks.append('idx_minus1')
            marks.append('src_default_random' if typ == 0 else 'src_default_group')
        else:
            src = (this + 4 + ((idx * 0x99C) & M32)) & M32
            marks.append('idx_slot' if idx < 20 else 'idx_big')
            marks.append('src_slot')
        marks.append('type_zero' if typ == 0 else 'type_nonzero')
        marks.append('mission_zero' if (mission & 0xFF) == 0 else 'mission_nonzero')
        events.append([ADDDM, W.tagv(this), W.tagv(src), typ, mission & 0xFF])
        ret = a[6]
    else:
        raise ValueError(k)
    out = {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}
    return out


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado)"""
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
        why = check_residue(c, row[side + '_residue'])
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
