"""Oraculo independiente (Python) de Fn167A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 7 funciones del lote 167A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x4C5750  CClumpModelInfo cuerpo del destructor: mov dword [ecx],85BB9Ch; ret
   1  0x4C5760  CWeaponModelInfo cuerpo del constructor: push esi; mov esi,ecx; call 4C4A60; mov dword [esi],85BD78h; mov eax,esi; pop esi; ret
   2  0x4DC2A0  CAEDataStream::Close: mov eax,[ecx+4]; push eax; mov byte [ecx+0Ch],0; call [858078h] (CloseHandle); ret   (EAX = el de CloseHandle)
   3  0x4DC550  CAEDataStream destructor con borrado: push esi; mov esi,ecx; mov al,[esi+0Ch]; test al,al; mov dword [esi],85F0BCh; je +; push [esi+4]; mov byte [esi+0Ch],0; call [858078h];
             mov eax,[esi+8]; test eax,eax; je +; push eax; call 8214BD; add esp,4; mov dword [esi+8],0; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
   4  0x6889F0  CTaskSimpleSwim::GetTaskType (vtable 0x870920, ranura 4): mov eax,518h
   5  0x68B680  CTaskSimpleArrestPed::GetTaskType (vtable 0x870984, ranura 4): mov eax,44Ch
   6  0x68B690  CTaskSimpleArrestPed cuerpo del destructor: [this] = 870984h; si [this+10h]: push 0; push 4D1390h; call 4CEBE0 (ecx = animacion); si [this+8]: push this+8; call 571A00 (ecx = ped);
             si [this+10h] y [this+8] (releidos): ecx = ped; call 5DF8F0; si AL == 0: si bit 8 de [ped+46Ch] y [ped+530h] == 3Fh: push 32h; call 5E4500 (ecx = ped); ecx = this; call 61A3A0

Los falsos anotan: ~CTask y el constructor base [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; CloseHandle [0x858078, HANDLE, dword de [this+0Ch] en la llamada];
operator delete [0x8214BD, puntero (etiqueta), dword del puntero]; SetFinishCallback [direccion, ECX, funcion (0x4D1390), datos]; CleanUpOldReference [direccion, ECX, argumento];
IsPlayer [direccion, ECX]; SetPedState [direccion, ECX, estado]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas, y solo AL de IsPlayer.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CClumpModelInfo::DestructorBody", "CWeaponModelInfo::ConstructorBody", "CAEDataStream::Close", "CAEDataStream::ScalarDeletingDestructor", "CTaskSimpleSwim::GetTaskType",
         "CTaskSimpleArrestPed::GetTaskType", "CTaskSimpleArrestPed::DestructorBody"]
ENTRIES = (0x4C5750, 0x4C5760, 0x4DC2A0, 0x4DC550, 0x6889F0, 0x68B680, 0x68B690)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SETFINISH, CLEANUP, ISPLAYER, SETPEDSTATE, CTASKDTOR, BASECTOR = 0x4CEBE0, 0x571A00, 0x5DF8F0, 0x5E4500, 0x61A3A0, 0x4C4A60
CLOSEHANDLE, CRTDELETE = 0x858078, 0x8214BD
DEFAULT_CB = 0x4D1390

# forma de cada funcion
CONST, VOID, CTORBODY, CLOSE, AESDD, ARRESTBODY = 'const void ctorbody close aesdd arrestbody'.split()
KIND = [VOID, CTORBODY, CLOSE, AESDD, CONST, CONST, ARRESTBODY]
CONSTS = {4: 0x518, 5: 0x44C}
VOID_VT = {0: 0x85BB9C}
CTOR_VT = {1: 0x85BD78}
PED_FLAGS, PED_STATE = 0x46C, 0x530


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
for _fn in range(7):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind in (VOID, CTORBODY):
        _k += ['prev_zero', 'prev_other']
    elif _kind == CLOSE:
        _k += ['byte_set', 'byte_clear', 'high_bytes', 'result_zero', 'result_nonzero', 'handle_zero', 'handle_other']
    elif _kind == AESDD:
        _k += ['open', 'closed', 'open_high_bytes', 'name_null', 'name_set', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'open_and_name', 'closed_and_no_name']
    elif _kind == ARRESTBODY:
        _k += ['ped_null', 'ped_in_b', 'ped_raw', 'assoc_null', 'assoc_in_c', 'assoc_raw', 'is_player', 'not_player', 'in_vehicle', 'not_in_vehicle', 'state_arrested', 'state_other', 'set_state_called',
               'both_set', 'assoc_only', 'ped_only', 'neither', 'player_al_zero_upper_set']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    return ['kind%d' % kind, 'fill%d' % fill, 'this_unaligned' if a[0] % 4 else 'this_aligned']


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
    elif k == VOID:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, VOID_VT[fn])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CTORBODY:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([BASECTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, CTOR_VT[fn])
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLOSE:
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, a[5]); A.wr(o + 0xC, 4, a[6])
        W.snap()
        A.wr(o + 0xC, 1, 0)                                 # el byte se limpia ANTES de llamar a CloseHandle
        events.append([CLOSEHANDLE, a[4], A.rd(o + 0xC, 4)])
        ret = a[7]
        marks.append('byte_set' if a[6] & 0xFF else 'byte_clear')
        if a[6] >> 8:
            marks.append('high_bytes')
        marks.append('result_zero' if a[7] == 0 else 'result_nonzero')
        marks.append('handle_zero' if a[4] == 0 else 'handle_other')
    elif k == AESDD:
        name = 0 if a[5] == 0 else W.raw(1, a[6])
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, name); A.wr(o + 0xC, 4, a[7])
        W.snap()
        A.wr(o, 4, 0x85F0BC)
        is_open = (a[7] & 0xFF) != 0
        if is_open:
            A.wr(o + 0xC, 1, 0)
            events.append([CLOSEHANDLE, a[4], A.rd(o + 0xC, 4)])
            marks.append('open')
            if a[7] >> 8:
                marks.append('open_high_bytes')
        else:
            marks.append('closed')
        if name:
            events.append([CRTDELETE, W.tagv(name), W.rd(name, 4)])
            A.wr(o + 8, 4, 0)
            marks.append('name_set')
        else:
            marks.append('name_null')
        marks.append('open_and_name' if is_open and name else ('closed_and_no_name' if not is_open and not name else 'mixed'))
        flags = a[9] & 0xFF                                 # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([CRTDELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[9] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == ARRESTBODY:
        ped = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        assoc = 0 if a[7] == 0 else (W.raw(2, a[8]) if a[7] == 1 else a[9])
        A.wr(o, 4, a[3]); A.wr(o + 8, 4, ped); A.wr(o + 0xC, 4, a[19]); A.wr(o + 0x10, 4, assoc)
        if a[4] == 1:
            B.wr(a[5] + PED_FLAGS, 4, a[10]); B.wr(a[5] + PED_STATE, 4, a[11])
        W.snap()
        A.wr(o, 4, 0x870984)
        marks.append('ped_null' if a[4] == 0 else ('ped_in_b' if a[4] == 1 else 'ped_raw'))
        marks.append('assoc_null' if a[7] == 0 else ('assoc_in_c' if a[7] == 1 else 'assoc_raw'))
        if assoc:
            events.append([SETFINISH, W.tagv(assoc), DEFAULT_CB, 0])
        if ped:
            events.append([CLEANUP, W.tagv(ped), W.tagv((this + 8) & M32)])
        marks.append('both_set' if ped and assoc else ('assoc_only' if assoc else ('ped_only' if ped else 'neither')))
        if assoc and ped:
            player_al = a[16] & 0xFF
            if a[4] == 2:
                player_al = (a[16] | 1) & 0xFF            # con el ped crudo el banco fuerza AL != 0 (no se puede leer su memoria)
            events.append([ISPLAYER, W.tagv(ped)])
            marks.append('is_player' if player_al else 'not_player')
            if (a[16] & 0xFF) == 0 and (a[16] >> 8):
                marks.append('player_al_zero_upper_set')
            if not player_al:
                flags = W.rd((ped + PED_FLAGS) & M32, 4)
                state = W.rd((ped + PED_STATE) & M32, 4)
                marks.append('in_vehicle' if (flags >> 8) & 1 else 'not_in_vehicle')
                marks.append('state_arrested' if state == 0x3F else 'state_other')
                if (flags >> 8) & 1 and state == 0x3F:
                    events.append([SETPEDSTATE, W.tagv(ped), 0x32])
                    marks.append('set_state_called')
        events.append([CTASKDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[17])
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
