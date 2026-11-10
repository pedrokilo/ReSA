"""Oraculo independiente (Python) de Fn147A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 26 funciones del lote 147A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x4AB980  CEventGroup::Destructor (cuerpo)   push 0; mov [ecx],85AAB0h; call 4AB370; ret       (4AB370 = CEventGroup::Flush(bool), thiscall `ret 4`)
   1  0x4AB9B0  CEventGlobalGroup::Destructor (cuerpo)   identico (la unica llamada del EXE es el destructor con borrado de CEventGlobalGroup, 4ABAA0)
   2  0x4ABA20  CEventGroup (vtable 0x85AAB0, ranura 0) destructor con borrado   push esi; mov esi,ecx; push 0; mov [esi],85AAB0h; call 4AB370; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
   3  0x5E9800  mov eax,[ecx+4]; test eax,eax; jne +; ret; mov ecx,[ecx+8]; sub ecx,eax; mov eax,66666667h; imul ecx; sar edx,3; mov eax,edx; shr eax,1Fh; add edx,eax; mov eax,edx; ret      (size() / 20)
   4  0x5E9850  idem con 2AAAAAABh y sar edx,1                                                                                                                                           (size() / 12)
   5  0x66DFA0  CTaskComplexGoToPointShooting::Destructor (cuerpo)  jmp 668870   (~CTaskComplexGoToPointAiming; la clase no escribe su vtable)
   6  0x66DF80  CTaskComplexGoToPointShooting (vtable 0x86FE2C) destructor con borrado: plantilla de 30 bytes con cuerpo 66DFA0 y CTask::operator delete 61A5B0
   7  0x66DFB0  CTaskComplexGoToPointAndStandStillAndAchieveHeading (vtable 0x86FE58): cuerpo 668D40
   8  0x463640  CTaskSimpleFinishBrain::Constructor  push esi; mov esi,ecx; call 61A390; mov [esi],859D20h; mov eax,esi; pop esi; ret
   9  0x5F67E0  CTaskSimpleWaitUntilLeaderAreaCodesMatch::Destructor (cuerpo)  mov [ecx],86C7E8h; jmp 6355E0
  10  0x632C60  CTaskComplexSequence::GetTaskType (vtable 0x86E200, ranura 4)  mov eax,0F4h; ret
  11  0x638A80  CTaskComplexBeInGroup (vtable 0x86E22C): cuerpo 632EA0
  12  0x635490  CTaskComplexUseSequence::GetTaskType (vtable 0x86E518, ranura 4)  mov eax,113h; ret
  13  0x6397A0  CTaskSimpleWaitUntilAreaCodesMatch (vtable 0x86E544): cuerpo 6355E0
  14  0x6397C0  CTaskComplexUseScriptedBrain (vtable 0x86E568): cuerpo 635900
  15  0x4AB240  jmp 538900 (CFileMgr::OpenFile, cdecl(path, modo))
  16  0x4AB250  jmp 5389B0 (CFileMgr::ReadLine, cdecl(archivo, buffer, num); `setne al`)
  17  0x4AB260  jmp 5389D0 (CFileMgr::CloseFile, cdecl(archivo))
  18  0x4AB2D0  FxPrtMult_c::SetUp  mov eax,[esp+4]; mov edx,[esp+8]; mov [ecx],eax; ... siete dwords de pila a [ecx+0],[ecx+4],...,[ecx+18h]; ret 1Ch
  19  0x492EF0  CTaskComplexDiveFromAttachedEntityAndGetUp (vtable 0x85A358): cuerpo 492F10
  20  0x492F60  CEntitySeekPosCalculator (copia del cuerpo)  mov [ecx],859DC4h; ret
  21  0x6579F0  CTaskComplexWalkRoundFire (vtable 0x86F3E8): cuerpo 655780
  22  0x48DEA0  CTaskSimpleRunAnim::GetTaskType (vtable 0x86D528, ranura 4)  movzx eax,word [ecx+1Ch]; ret
  23  0x48E930  CTaskSimpleScratchHead::Destructor (cuerpo)  jmp 61A6F0 (~CTaskSimpleAnim)
  24  0x48E910  CTaskSimpleScratchHead (vtable 0x85A100): cuerpo 48E930
  25  0x48E940  CTaskSimpleUseAtm (vtable 0x85A134): cuerpo 48E960

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos y constructor base; Flush ademas el argumento) y dejan un dword del caso en [ECX]; operator delete anota
[direccion, puntero (etiqueta), dword de [puntero]]: prueba que se borra DESPUES de destruir. Las funciones de CFileMgr anotan sus argumentos crudos. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CEventGroup::Destructor', 'CEventGlobalGroup::Destructor', 'CEventGroup::ScalarDeletingDestructor', 'SArraySizeStride20', 'SArraySizeStride12',
         'CTaskComplexGoToPointShooting::Destructor', 'CTaskComplexGoToPointShooting::ScalarDeletingDestructor', 'CTaskComplexGoToPointAndStandStillAndAchieveHeading::ScalarDeletingDestructor',
         'CTaskSimpleFinishBrain::Constructor', 'CTaskSimpleWaitUntilLeaderAreaCodesMatch::Destructor', 'CTaskComplexSequence::GetTaskType', 'CTaskComplexBeInGroup::ScalarDeletingDestructor',
         'CTaskComplexUseSequence::GetTaskType', 'CTaskSimpleWaitUntilAreaCodesMatch::ScalarDeletingDestructor', 'CTaskComplexUseScriptedBrain::ScalarDeletingDestructor',
         'FxSystem_c::OpenFileThunk', 'FxSystem_c::ReadLineThunk', 'FxSystem_c::CloseFileThunk', 'FxPrtMult_c::SetUp',
         'CTaskComplexDiveFromAttachedEntityAndGetUp::ScalarDeletingDestructor', 'CEntitySeekPosCalculator::DestructorCopy', 'CTaskComplexWalkRoundFire::ScalarDeletingDestructor',
         'CTaskSimpleRunAnim::GetTaskType', 'CTaskSimpleScratchHead::Destructor', 'CTaskSimpleScratchHead::ScalarDeletingDestructor', 'CTaskSimpleUseAtm::ScalarDeletingDestructor']
ENTRIES = (0x4AB980, 0x4AB9B0, 0x4ABA20, 0x5E9800, 0x5E9850, 0x66DFA0, 0x66DF80, 0x66DFB0, 0x463640, 0x5F67E0, 0x632C60, 0x638A80, 0x635490, 0x6397A0, 0x6397C0, 0x4AB240, 0x4AB250, 0x4AB260,
           0x4AB2D0, 0x492EF0, 0x492F60, 0x6579F0, 0x48DEA0, 0x48E930, 0x48E910, 0x48E940)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
FLUSH = 0x4AB370
CRT_DELETE = 0x8214BD
TASK_DELETE = 0x61A5B0

# forma de cada funcion
BODY, SDD, SDDEV, SIZE, CONST, WORD, OPEN, READ, CLOSE, CTOR, SETUP = 'body sdd sddev size const word open read close ctor setup'.split()
KIND = [BODY, BODY, SDDEV, SIZE, SIZE, BODY, SDD, SDD, CTOR, BODY, CONST, SDD, CONST, SDD, SDD, OPEN, READ, CLOSE, SETUP, SDD, BODY, SDD, WORD, BODY, SDD, SDD]
# BODY / SDD / CTOR: direccion (EXE) del colaborador que llama la funcion (cuerpo del destructor, Flush, constructor base)
CALLEE = {0: FLUSH, 1: FLUSH, 2: FLUSH, 5: 0x668870, 6: 0x66DFA0, 7: 0x668D40, 8: 0x61A390, 9: 0x6355E0, 11: 0x632EA0, 13: 0x6355E0, 14: 0x635900, 19: 0x492F10, 21: 0x655780, 23: 0x61A6F0, 24: 0x48E930, 25: 0x48E960}
# vtable que el cuerpo deja en [this] ANTES de llamar al colaborador (cuerpos con escritura previa)
VT_BEFORE = {0: 0x85AAB0, 1: 0x85AAB0, 2: 0x85AAB0, 9: 0x86C7E8}
# vtable que deja DESPUES (constructor) o sola (copia del cuerpo sin llamadas)
VT_AFTER = {8: 0x859D20, 20: 0x859DC4}
CONSTANT = {10: 0xF4, 12: 0x113}
STRIDE = {3: 20, 4: 12}


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


def sdiv_trunc(d, n):
    """division con signo truncada hacia cero de una palabra de 32 bits"""
    d &= M32
    if d & 0x80000000:
        d -= 1 << 32
    q = abs(d) // n
    return (-q if d < 0 else q) & M32


KEYS = {}
for _fn in range(26):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (BODY, SDD, SDDEV, CTOR):
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other']
        if _kind in (SDD, SDDEV):
            _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == SIZE:
        _k += ['first_zero', 'first_nonzero', 'diff_negative', 'diff_positive', 'diff_exact', 'diff_inexact', 'diff_zero']
    elif _kind == WORD:
        _k += ['this_aligned', 'this_unaligned', 'word_high_set', 'word_high_clear']
    elif _kind == CONST:
        _k += ['this_aligned', 'this_unaligned']
    elif _kind in (OPEN, READ, CLOSE):
        _k += ['ret_zero', 'ret_nonzero', 'arg_zero', 'arg_nonzero']
        if _kind == READ:
            _k += ['ret_low_byte_zero', 'ret_high_bits']
    elif _kind == SETUP:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'arg_nan', 'arg_snan', 'arg_inf', 'arg_zero', 'arg_negative']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if KIND[fn] in (BODY, SDD, SDDEV, CTOR, WORD, CONST, SETUP):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
        span = {SETUP: 0x1C, WORD: 0x20}.get(KIND[fn], 4)
        if a[0] + span > WIN:
            marks.append('spill')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret = [], None
    o = a[0]
    if k in (BODY, SDD, SDDEV, CTOR) and fn != 20:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        callee = CALLEE[fn]
        if fn in VT_BEFORE:
            A.wr(o, 4, VT_BEFORE[fn])
        # el colaborador anota [direccion, this, dword de [this] al entrar] (Flush ademas el argumento, que el EXE empuja como 0) y deja el dword del caso en [this]
        dword_in = A.rd(o, 4)
        if callee == FLUSH:
            events.append([callee, tag(0, o), dword_in, 0])
        else:
            events.append([callee, tag(0, o), dword_in])
        A.wr(o, 4, a[5])
        if k in (SDD, SDDEV):
            flags = a[6] & 0xFF                                  # solo el byte bajo de la ranura de pila
            if flags & 1:
                events.append([CRT_DELETE if k == SDDEV else TASK_DELETE, tag(0, o), a[5]])   # operator delete ve lo que dejo el colaborador
            ret = tag(0, o)
            marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
            marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        if k == CTOR:
            A.wr(o, 4, VT_AFTER[fn])
            ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn == 20:                                    # `mov [ecx],859DC4h; ret`: sin colaboradores
        A.wr(o, 4, a[3]); A.snap(); B.snap(); C.snap()
        A.wr(o, 4, VT_AFTER[fn])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == SIZE:
        A.wr(o + 4, 4, a[3]); A.wr(o + 8, 4, a[4])
        A.snap(); B.snap(); C.snap()
        first, last = a[3], a[4]
        ret = 0 if first == 0 else sdiv_trunc((last - first) & M32, STRIDE[fn])
        marks.append('first_zero' if first == 0 else 'first_nonzero')
        if first:
            d = (last - first) & M32
            marks.append('diff_zero' if d == 0 else 'diff_negative' if d & 0x80000000 else 'diff_positive')
            marks.append('diff_exact' if (d if not d & 0x80000000 else (1 << 32) - d) % STRIDE[fn] == 0 else 'diff_inexact')
    elif k == CONST:
        A.wr(o, 4, a[3]); A.snap(); B.snap(); C.snap()
        ret = CONSTANT[fn]
    elif k == WORD:
        A.wr(o + 0x1C, 4, a[3]); A.snap(); B.snap(); C.snap()
        ret = A.rd(o + 0x1C, 2)                                   # movzx eax,word [ecx+1Ch]
        marks.append('word_high_set' if ret & 0x8000 else 'word_high_clear')
    elif k in (OPEN, READ, CLOSE):
        A.snap(); B.snap(); C.snap()
        if k == OPEN:
            events.append([0x538900, a[3], a[4]])
            ret = a[6]
        elif k == READ:
            events.append([0x5389B0, a[3], a[4], a[5]])
            ret = a[6] & 0xFF                                     # setne al: el valor es un bool
            marks.append('ret_low_byte_zero' if (a[6] & 0xFF) == 0 else 'ret_high_bits')
        else:
            events.append([0x5389D0, a[3]])
            ret = a[6]
        marks.append('ret_zero' if a[6] == 0 else 'ret_nonzero')
        marks.append('arg_zero' if a[3] == 0 else 'arg_nonzero')
    elif k == SETUP:
        A.snap(); B.snap(); C.snap()
        for i in range(7):
            A.wr(o + 4 * i, 4, a[3 + i])
            w = a[3 + i]
            e, m = (w >> 23) & 0xFF, w & 0x7FFFFF
            if e == 0xFF and m:
                marks.append('arg_nan')
                if not m & 0x400000:
                    marks.append('arg_snan')
            elif e == 0xFF:
                marks.append('arg_inf')
            if w & 0x7FFFFFFF == 0:
                marks.append('arg_zero')
            if w & 0x80000000:
                marks.append('arg_negative')
        ret = None
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
