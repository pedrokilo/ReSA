"""Oraculo independiente (Python) de Fn145A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 145A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x502BF0  CAEWMADecoder::Shutdown (cdecl, `ret`):  mov eax,[0B6BAD8h]; test eax,eax; je fin; mov eax,[0B6BAD4h]; push eax; call [858110h]; mov [0B6BAD8h],0; fin: ret
                (0xB6BAD8 = WMCreateSyncReader, 0xB6BAD4 = manejador de wmvcore.dll; la casilla de IAT 0x858110 es FreeLibrary: stdcall, un argumento; el manejador NO se anula)
   1  0x502C40  CAEWMADecoder (vtable 0x862D90, ranura 6, destructor con borrado)  push esi; mov esi,ecx; call 502760; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
 2..21 destructores con borrado de 20 clases de tareas (ranura 0 de su vtable; misma plantilla de 30 bytes con `call <cuerpo>` y `call 61A5B0` = CTask::operator delete):
        0x64C930  CTaskComplexCarSlowBeDraggedOutAndStandUp (vtable 0x86EF80)  cuerpo 0x648690
        0x64C950  CTaskSimpleBikeJacked (vtable 0x86EFAC)  cuerpo 0x648C40
        0x65D0C0  CTaskComplexWanderFlee (vtable 0x86F690)  cuerpo 0x65B370
        0x65D0E0  CTaskComplexFleePoint (vtable 0x86F6CC)  cuerpo 0x65B410
        0x671610  CTaskComplexFollowPointRoute (vtable 0x870078)  cuerpo 0x66E050
        0x671730  CTaskComplexFollowNodeRoute (vtable 0x8700A8)  cuerpo 0x66EB70
        0x638160  CTaskComplexHitByGunResponse (vtable 0x86E06C)  cuerpo 0x631E30
        0x638180  CTaskComplexSunbathe (vtable 0x86E0AC)  cuerpo 0x632050
        0x6275E0  CTaskSimpleGangDriveBy (vtable 0x86D944)  cuerpo 0x6218C0
        0x628780  CTaskComplexDestroyCar (vtable 0x86D968)  cuerpo 0x621CB0
        0x639310  CTaskSimplePlayerOnFire (vtable 0x86E334)  cuerpo 0x6335B0
        0x639330  CTaskSimpleIKManager (vtable 0x86E358)  cuerpo 0x633830
        0x664A80  CTaskComplexGangJoinRespond (vtable 0x86FB5C)  cuerpo 0x661720
        0x664AA0  CTaskComplexProstituteSolicit (vtable 0x86FB88)  cuerpo 0x661AF0
        0x638900  CTaskComplexWaitForDryWeather (vtable 0x86E160)  cuerpo 0x632830
        0x638920  CTaskComplexWaitForBus (vtable 0x86E18C)  cuerpo 0x632930
        0x638F00  CTaskComplexAttractorPartnerWait (vtable 0x86E284)  cuerpo 0x633290
        0x639F90  CTaskComplexUseSequence (vtable 0x86E518)  cuerpo 0x6396C0
        0x48E700  CTaskComplexWanderCriminal (vtable 0x85A23C)  cuerpo 0x48E720
        0x670400  CTaskComplexGoToAttractor (vtable 0x86FF3C)  cuerpo 0x66B6A0

En los destructores con borrado el falso del cuerpo deja el dword del caso en [ECX] y devuelve un EAX que no es `this`; el falso de operator delete anota el dword de [argumento] al entrar (= lo que dejo
el cuerpo: prueba que se borra DESPUES de destruir). Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas. En Shutdown el FreeLibrary falso (casilla de la IAT) anota el manejador y devuelve un BOOL
del caso que la funcion no usa; solo se llama si WMCreateSyncReader no es nulo, y despues WMCreateSyncReader queda a cero (sus 4 bytes) y el manejador igual.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CAEWMADecoder::Shutdown', 'CAEWMADecoder::ScalarDeletingDestructor', 'CTaskComplexCarSlowBeDraggedOutAndStandUp::ScalarDeletingDestructor', 'CTaskSimpleBikeJacked::ScalarDeletingDestructor', 'CTaskComplexWanderFlee::ScalarDeletingDestructor', 'CTaskComplexFleePoint::ScalarDeletingDestructor', 'CTaskComplexFollowPointRoute::ScalarDeletingDestructor', 'CTaskComplexFollowNodeRoute::ScalarDeletingDestructor', 'CTaskComplexHitByGunResponse::ScalarDeletingDestructor', 'CTaskComplexSunbathe::ScalarDeletingDestructor', 'CTaskSimpleGangDriveBy::ScalarDeletingDestructor', 'CTaskComplexDestroyCar::ScalarDeletingDestructor', 'CTaskSimplePlayerOnFire::ScalarDeletingDestructor', 'CTaskSimpleIKManager::ScalarDeletingDestructor', 'CTaskComplexGangJoinRespond::ScalarDeletingDestructor', 'CTaskComplexProstituteSolicit::ScalarDeletingDestructor', 'CTaskComplexWaitForDryWeather::ScalarDeletingDestructor', 'CTaskComplexWaitForBus::ScalarDeletingDestructor', 'CTaskComplexAttractorPartnerWait::ScalarDeletingDestructor', 'CTaskComplexUseSequence::ScalarDeletingDestructor', 'CTaskComplexWanderCriminal::ScalarDeletingDestructor', 'CTaskComplexGoToAttractor::ScalarDeletingDestructor']
ENTRIES = (0x502BF0, 0x502C40, 0x64C930, 0x64C950, 0x65D0C0, 0x65D0E0, 0x671610, 0x671730, 0x638160, 0x638180, 0x6275E0, 0x628780, 0x639310, 0x639330, 0x664A80, 0x664AA0, 0x638900, 0x638920, 0x638F00, 0x639F90, 0x48E700, 0x670400)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD


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


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytearray(out[:nbytes])


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return 0x10000000 + (arena << 20) + GUARD + off


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




# ------------------------------------------------------------------------------------------------ tablas leidas del desensamblado
BODIES = [5252960, 6588048, 6589504, 6665072, 6665232, 6742096, 6744944, 6495792, 6496336, 6428864, 6429872, 6501808, 6502448, 6690592, 6691568, 6498352, 6498608, 6501008, 6526656, 4777760, 6731424]          # idx 1..21: cuerpo del destructor
DELETES = [0x8214BD] + [0x61A5B0] * 20
FREELIB = 0x858110
SPAN = 4


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if fn >= 1:
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
        if a[0] + SPAN > WIN:
            marks.append('spill')
    return marks


KEYS = {}
for _fn in range(22):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    if _fn == 0:
        _k += ['reader_zero', 'reader_nonzero', 'handle_zero', 'handle_ffff', 'handle_other', 'free_ret_zero', 'free_ret_other']
    else:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'prev_zero', 'prev_other']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, gchanges, ret = [], [], None
    if fn == 0:
        handle, reader = a[3], a[4]
        A.snap(); B.snap(); C.snap()
        if reader != 0:
            events.append([FREELIB, handle])                  # FreeLibrary(manejador) por la IAT
            for k in range(4):                                # WMCreateSyncReader = 0: cambian los bytes distintos de cero (region 0: 0xB6BAD4, 8 bytes)
                if (reader >> (8 * k)) & 0xFF:
                    gchanges.append([0, 4 + k, 0])
        marks.append('reader_nonzero' if reader else 'reader_zero')
        marks.append('handle_zero' if handle == 0 else 'handle_ffff' if handle == M32 else 'handle_other')
        marks.append('free_ret_zero' if a[5] == 0 else 'free_ret_other')
    else:
        o = a[0]
        body, delete = BODIES[fn - 1], DELETES[fn - 1]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events.append([body, tag(0, o), a[3]])
        A.wr(o, 4, a[5])                                    # el falso del cuerpo deja su dword en [this]
        flags = a[6] & 0xFF                                 # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([delete, tag(0, o), a[5]])        # operator delete ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gchanges, 'ret': ret, 'marks': marks}


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


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
