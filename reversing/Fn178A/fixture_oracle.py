"""Oraculo independiente (Python) de Fn178A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 178A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, capstone; vtables leidas del propio EXE).
   0  0x4AFB50  CEventVehicleThreat::Constructor: marco SEH; esi = ecx; edx = 0; [esi+4] = edx (dword); [esi+8] = dl; [esi+0Ch] = 1; word [esi+0Eh] = 0C8h; word [esi+10h] = 0FFFFh; ecx = vehiculo (pila); [esi] = 85B298h;
            [esi+14h] = vehiculo; si no es nulo: push esi+14h; call 571B70 (ecx = vehiculo); eax = esi; ret 4
   1  0x4AFBF0  cuerpo del destructor: [esi] = 85B298h; ecx = [esi+14h]; si no es nulo: push esi+14h; call 571A00; [esi] = 85AB38h; ret
   2  0x5984C0  InteriorManager_c::Constructor: vector de 8 con 821D30 (stride 794h, constructor 5921D0: [e+0] = [e+4] = 0 (4A8DB0), [e+5Ch..+64h] = 0 (4A8DD0), word [e+3ECh] = word [e+3F0h] = 0FFFFh); 4A8DD0 en +3CA0h (3 dwords
            a cero); vector de 8 con 821D30 (stride 0BCh, constructor 597FE0 = 4A8DB0: [g+0] = [g+4] = 0) desde +3CACh; 4A8DD0 en +428Ch y en +4298h; [+4740h] = [+474Ch] = 49742400h; [+4748h] = [+4744h] = 0C9742400h; eax = this; ret
   3  0x598580  InteriorManager_c::Destructor: 4A8DE0 (+4298h, +428Ch), vector destructor 821E02 (8 de 0BCh con 597FF0), 4A8DE0 (+3CA0h), 821E02 (8 de 794h con 591360): todos vacios; ret
   4  0x59CA10  CReplay::ApplyPanelDamageToCar (cdecl, 3 de pila, `ret`): ebx = bandera, esi = coche, edi = paneles; panel 0: si (estado & 0Fh) != (paneles & 0Fh): estado = (estado & ~0Fh) | (paneles & 0Fh); push bandera; push 0;
            ecx = coche; call 6B1480. Paneles 1 (mascara 0F0h, 6B1480 con 1), 4 (0F0000h, 6B1480 con 4): si ((estado ^ paneles) & m): estado = ((estado ^ paneles) & m) ^ estado; llamada. Paneles 2 (0F00h) y 3 (0F000h): igual sin llamada.
            Paneles 5 (0F00000h) y 6 (0F000000h): igual con 6B1350. Cada panel relee [esi+5B4h] tras la llamada anterior.
   5  0x59CBA0  mov [ecx],866DB0h; ret
   6  0x59CD50  destructor con borrado: push esi; esi = ecx; call 59CBA0; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; eax = esi; pop esi; ret 4
   7  0x5DE2C0  idem con 5DE0D0 y delete 5E4760
   8  0x5DE2E0  push esi; esi = ecx; call 532540; [esi] = 86C198h; eax = esi; pop esi; ret
   9  0x5DE300  mov [ecx],86C198h; jmp 532560
  10  0x5DE310  destructor con borrado: cuerpo 5DE300, delete 5326F0
  11  0x5DE340  CEmergencyPed::Constructor: marco SEH; push tipo; call 5E8030 (ecx = this); [esi] = 86C200h; push modelo; ecx = esi; call 5E4880; push esi; ecx = esi+294h; call 4E68D0; eax = esi; ret 8
  12  0x5DE3B0  mov [ecx],86C200h; jmp 5E8620
  13  0x5DE3E0  destructor con borrado: cuerpo 5DE3B0, delete 5E4760
  14  0x5DE400  ProcessControl: call 5E8CD0; si bit 6 de [esi+1Ch] o [esi+530h] == 37h: ret; ecx = esi + 5A0h + 1Ch * movsx(byte [esi+718h]); push 0; call 73DB40; si [esi+530h] == 36h: ret; (releer slot) push 0; call 73DB40;
            eax = [esi+598h] - 12h: si es cero ret; dec eax; si no es cero ret; ecx = esi; jmp 5E3960
  15  0x5FF2C0  mov [ecx],86CB10h; jmp 4AF890
  16  0x5FF2D0  mov eax,3Fh; ret
  17  0x5FF2E0  CloneEditable: marco SEH; push 28h; call 4B5620; add esp,4; si eax == 0: eax = 0; ret; edx = [esi+18h]; ecx = esi+1Ch; push ecx (punto); ecx = [esi+14h]; push edx; push ecx; ecx = eax; call 5FF250; eax = el del constructor; ret
  18  0x5FF350  mov al,1; ret
  19  0x5FF360  destructor con borrado: cuerpo 5FF2C0, delete 4B5630
  20  0x6C9160  CPlane cuerpo del destructor: [esi] = 871948h; si [esi+9D4h]: edi = 0; call 6D3F30 (ecx = esi); si eax <= 0 salta a delete[]; bucle: ecx = [[esi+9D4h] + edi*4]; si no es nulo: call 4AA3F0; ecx = [[esi+9D4h] + edi*4]
            (relee la tabla y la ranura); push ranura; ecx = 0A9AE80h; call 4A9810; edi++; call 6D3F30 (ecx = esi); si edi < eax (con signo) repite; push [esi+9D4h] (releido); call 8214AE (delete[] CRT); [esi+9D4h] = 0;
            4 particulas de [esi+9E8h]: si no es nula: call 4AA3F0 (ecx = particula); [ptr] = 0; humo [esi+9F8h]: igual; ecx = esi+138h; call 4FB8C0; ecx = esi; call 6A61E0; ret
  21  0x6CADB0  destructor con borrado: cuerpo 6C9160, delete 6E2D90
  22  0x6CDC30  CQuadBike cuerpo del destructor: [esi] = 871AE8h; ecx = esi+98Ch; call 6D0B30; ecx = esi; call 6A61E0; ret

Los falsos anotan: cuerpos y constructores [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (los de un argumento anaden el argumento etiquetado antes del dword; el de tres, ped, tiempo y punto); operator delete
[direccion, puntero (etiqueta), dword del puntero]; delete[] [direccion, puntero (etiqueta)]; operator new [direccion, tamano]; RegisterReference y CleanUpOldReference [direccion, ECX, referencia, dword de [referencia - 14h]];
SetPanelDamage y SetBumperDamage [direccion, ECX, panel, bandera, estado de paneles al entrar]; SetModelIndex [direccion, ECX, modelo, dword de [ECX]]; Initialise [direccion, ECX, ped]; ProcessControl de CPed [direccion, ECX];
CWeapon::Update [direccion, ECX (arma), propietario]; GetPlaneNumGuns, Kill y los de un solo ECX [direccion, ECX]; DestroyFxSystem [direccion, ECX (g_fxMan), sistema].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CEventVehicleThreat::Constructor", "CEventVehicleThreat::DestructorBody", "InteriorManager_c::Constructor", "InteriorManager_c::Destructor", "CReplay::ApplyPanelDamageToCar",
         "CWinsockLink::BaseDestructorBody", "CWinsockLink::ScalarDeletingDestructor", "CCopPed::ScalarDeletingDestructor", "CDummyPed::Constructor", "CDummyPed::DestructorBody",
         "CDummyPed::ScalarDeletingDestructor", "CEmergencyPed::Constructor", "CEmergencyPed::DestructorBody", "CEmergencyPed::ScalarDeletingDestructor", "CEmergencyPed::ProcessControl",
         "CEventAcquaintancePedHateBadlyLit::DestructorBody", "CEventAcquaintancePedHateBadlyLit::GetEventType", "CEventAcquaintancePedHateBadlyLit::CloneEditable",
         "CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent", "CEventAcquaintancePedHateBadlyLit::ScalarDeletingDestructor", "CPlane::DestructorBody",
         "CPlane::ScalarDeletingDestructor", "CQuadBike::DestructorBody"]
ENTRIES = (0x4AFB50, 0x4AFBF0, 0x5984C0, 0x598580, 0x59CA10, 0x59CBA0, 0x59CD50, 0x5DE2C0, 0x5DE2E0, 0x5DE300, 0x5DE310, 0x5DE340, 0x5DE3B0, 0x5DE3E0, 0x5DE400,
           0x5FF2C0, 0x5FF2D0, 0x5FF2E0, 0x5FF350, 0x5FF360, 0x6C9160, 0x6CADB0, 0x6CDC30)
GUARD = 0x40
WINS = (0x5000, 0x1000, 0x1000, 0x1000)
SIZES = tuple(GUARD + w + GUARD for w in WINS)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

VTCTOR, VTDTOR, IMCTOR, IMDTOR, REPLAY, WSBASE, SDD, DUMMYCTOR, BODYJMP, EMERCTOR, EMERPC, CONSTTYPE, HBLCLONE, CONSTBOOL, PLANEBODY, QUADBODY = \
    'vtctor vtdtor imctor imdtor replay wsbase sdd dummyctor bodyjmp emerctor emerpc consttype hblclone constbool planebody quadbody'.split()
KIND = [VTCTOR, VTDTOR, IMCTOR, IMDTOR, REPLAY, WSBASE, SDD, SDD, DUMMYCTOR, BODYJMP, SDD, EMERCTOR, BODYJMP, SDD, EMERPC, BODYJMP, CONSTTYPE, HBLCLONE, CONSTBOOL, SDD, PLANEBODY, SDD, QUADBODY]
SDD_BODY = {6: 0x59CBA0, 7: 0x5DE0D0, 10: 0x5DE300, 13: 0x5DE3B0, 19: 0x5FF2C0, 21: 0x6C9160}
SDD_DEL = {6: 0x8214BD, 7: 0x5E4760, 10: 0x5326F0, 13: 0x5E4760, 19: 0x4B5630, 21: 0x6E2D90}
BODYJMP_VT = {9: (0x86C198, 0x532560), 12: (0x86C200, 0x5E8620), 15: (0x86CB10, 0x4AF890)}
G_FXMAN = 0xA9AE80


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


def s8(v):
    v &= 0xFF
    return v - 0x100 if v >= 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 0x80000000 else v


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[60 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + SIZES[i]:
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
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + SIZES[i]:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind in (VTCTOR, VTDTOR):
        _k += ['veh_null', 'veh_arena', 'veh_raw']
    elif _kind == REPLAY:
        _k += ['panel0', 'panel1', 'panel2_nocall', 'panel3_nocall', 'panel4', 'panel5', 'panel6', 'no_calls', 'flag0', 'flag1', 'scribble_on', 'scribble_off', 'all_calls']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == EMERPC:
        _k += ['postponed', 'not_postponed', 'dead', 'die', 'type_medic', 'type_fireman', 'type_other', 'slot_negative', 'slot_positive', 'state_changed', 'slot_changed', 'type_changed', 'one_update', 'two_updates']
    elif _kind == HBLCLONE:
        _k += ['mem_null', 'mem_set']
    elif _kind == PLANEBODY:
        _k += ['table_null', 'table_set', 'guns_positive', 'guns_not_positive', 'gun_killed', 'gun_empty', 'scribble_slot', 'scribble_table', 'jets_set', 'jets_null', 'smoke_set', 'smoke_null', 'table_changed']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k in (VTCTOR, VTDTOR):
        veh = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        marks.append('veh_null' if a[4] == 0 else ('veh_arena' if a[4] == 1 else 'veh_raw'))
        if k == VTCTOR:
            W.snap()
            A.wr(o + 4, 4, 0); A.wr(o + 8, 1, 0); A.wr(o + 0xC, 1, 1); A.wr(o + 0xE, 2, 200); A.wr(o + 0x10, 2, 0xFFFF)
            A.wr(o, 4, 0x85B298)
            A.wr(o + 0x14, 4, veh)
            if veh:
                events.append([0x571B70, W.tagv(veh), W.tagv(this + 0x14), 0x85B298])
            ret = W.tagv(this)
        else:
            A.wr(o + 0x14, 4, veh)
            W.snap()
            A.wr(o, 4, 0x85B298)
            if veh:
                events.append([0x571A00, W.tagv(veh), W.tagv(this + 0x14), 0x85B298])
            A.wr(o, 4, 0x85AB38)
    elif k == IMCTOR:
        W.snap()
        for i in range(8):
            b = o + i * 0x794
            A.wr(b, 4, 0); A.wr(b + 4, 4, 0)
            for d in (0x5C, 0x60, 0x64):
                A.wr(b + d, 4, 0)
            A.wr(b + 0x3EC, 2, 0xFFFF); A.wr(b + 0x3F0, 2, 0xFFFF)
        for base in (0x3CA0, 0x428C, 0x4298):
            for d in (0, 4, 8):
                A.wr(o + base + d, 4, 0)
        for g in range(8):
            A.wr(o + 0x3CAC + g * 0xBC, 4, 0); A.wr(o + 0x3CAC + g * 0xBC + 4, 4, 0)
        A.wr(o + 0x4740, 4, 0x49742400); A.wr(o + 0x4744, 4, 0xC9742400); A.wr(o + 0x4748, 4, 0xC9742400); A.wr(o + 0x474C, 4, 0x49742400)
        ret = W.tagv(this)
    elif k in (IMDTOR, CONSTTYPE, CONSTBOOL):
        W.snap()
        if k == CONSTTYPE:
            ret = 0x3F
        elif k == CONSTBOOL:
            ret = 1
    elif k == REPLAY:
        car = W.raw(1, a[7])
        B.wr(a[7] + 0x5B4, 4, a[5])
        W.snap()
        status, panels, flag, scr = a[5], a[4], a[6], a[8:15]
        calls = [0]

        def call(addr, panel):
            nonlocal status
            events.append([addr, W.tagv(car), panel, flag, status])
            if calls[0] < 7:
                status ^= scr[calls[0]]
            calls[0] += 1

        cur = status
        if (cur & 0xF) != (panels & 0xF):
            status = (cur & 0xFFFFFFF0) | (panels & 0xF)
            marks.append('panel0')
            call(0x6B1480, 0)
        for mask, pnl, addr, nm in ((0xF0, 1, 0x6B1480, 'panel1'), (0xF00, 2, 0, 'panel2_nocall'), (0xF000, 3, 0, 'panel3_nocall'), (0xF0000, 4, 0x6B1480, 'panel4'),
                                    (0xF00000, 5, 0x6B1350, 'panel5'), (0xF000000, 6, 0x6B1350, 'panel6')):
            cur = status
            if (cur ^ panels) & mask:
                status = (((cur ^ panels) & mask) ^ cur) & M32
                marks.append(nm)
                if addr:
                    call(addr, pnl)
        B.wr(a[7] + 0x5B4, 4, status)
        marks.append('flag1' if flag else 'flag0')
        if not calls[0]:
            marks.append('no_calls')
        if calls[0] == 7 - 2 or calls[0] == 5:
            marks.append('all_calls')
        marks.append('scribble_on' if any(scr) else 'scribble_off')
    elif k == WSBASE:
        W.snap()
        A.wr(o, 4, 0x866DB0)
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([SDD_DEL[fn], W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == DUMMYCTOR:
        W.snap()
        events.append([0x532540, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, 0x86C198)
        ret = W.tagv(this)
    elif k == BODYJMP:
        W.snap()
        vt, target = BODYJMP_VT[fn]
        A.wr(o, 4, vt)
        events.append([target, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == EMERCTOR:
        W.snap()
        events.append([0x5E8030, W.tagv(this), W.tagv(a[4]), A.rd(o, 4)])
        A.wr(o, 4, a[7])
        A.wr(o, 4, 0x86C200)
        events.append([0x5E4880, W.tagv(this), W.tagv(a[5]), A.rd(o, 4)])
        events.append([0x4E68D0, W.tagv(this + 0x294), W.tagv(this)])
        ret = W.tagv(this)
    elif k == EMERPC:
        W.snap()
        events.append([0x5E8CD0, W.tagv(this)])
        A.wr(o + 0x1C, 4, a[4]); A.wr(o + 0x530, 4, a[5]); A.wr(o + 0x598, 4, a[6]); A.wr(o + 0x718, 1, a[7] & 0xFF)

        def weapon():
            return (this + 0x5A0 + 0x1C * s8(A.rd(o + 0x718, 1))) & M32

        marks.append('slot_negative' if s8(a[7]) < 0 else 'slot_positive')
        if (A.rd(o + 0x1C, 4) >> 6) & 1:
            marks.append('postponed')
        else:
            marks.append('not_postponed')
            if A.rd(o + 0x530, 4) == 0x37:
                marks.append('dead')
            else:
                events.append([0x73DB40, W.tagv(weapon()), 0])
                mask = a[9] & 15
                if mask & 1:
                    A.wr(o + 0x530, 4, a[8]); marks.append('state_changed')
                if mask & 2:
                    A.wr(o + 0x718, 1, a[10] & 0xFF); marks.append('slot_changed')
                if A.rd(o + 0x530, 4) == 0x36:
                    marks.append('die'); marks.append('one_update')
                else:
                    marks.append('two_updates')
                    events.append([0x73DB40, W.tagv(weapon()), 0])
                    if mask & 4:
                        A.wr(o + 0x598, 4, a[11]); marks.append('type_changed')
                    if mask & 8:
                        A.wr(o + 0x718, 1, a[12] & 0xFF); marks.append('slot_changed')
                    t = A.rd(o + 0x598, 4)
                    if t == 0x12:
                        marks.append('type_medic')
                    elif t == 0x13:
                        marks.append('type_fireman')
                        events.append([0x5E3960, W.tagv(this)])
                    else:
                        marks.append('type_other')
    elif k == HBLCLONE:
        A.wr(o + 0x14, 4, a[4]); A.wr(o + 0x18, 4, a[5])
        W.snap()
        events.append([0x4B5620, 0x28])
        if a[6] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[7])
            events.append([0x5FF250, W.tagv(mem), W.tagv(a[4]), a[5], W.tagv(this + 0x1C), B.rd(a[7], 4)])
            B.wr(a[7], 4, a[9])
            ret = W.tagv(a[8])
    elif k == PLANEBODY:
        table = 0 if a[4] == 0 else W.raw(1, a[5])
        alt = W.raw(2, a[14])
        A.wr(o + 0x9D4, 4, table)
        for i in range(8):
            B.wr(a[5] + 4 * i, 4, a[6 + i]); Cc.wr(a[14] + 4 * i, 4, a[15 + i])
        for i in range(4):
            A.wr(o + 0x9E8 + 4 * i, 4, a[33 + i])
        A.wr(o + 0x9F8, 4, a[37])
        W.snap()
        A.wr(o, 4, 0x871948)
        cnt = {'num': 0, 'kill': 0}

        def numguns():
            v = a[23 + min(cnt['num'], 9)]
            cnt['num'] += 1
            events.append([0x6D3F30, W.tagv(this)])
            return s32(v)

        def kill(fx):
            events.append([0x4AA3F0, W.tagv(fx)])
            kk = cnt['kill']
            cnt['kill'] += 1
            if kk < 4:
                what, slot = a[38 + kk] & 0xFF, (a[38 + kk] >> 8) & 7
                tbl = A.rd(o + 0x9D4, 4)
                if what == 1 and tbl:
                    W.wr(tbl + 4 * slot, 4, a[42 + kk]); marks.append('scribble_slot')
                elif what == 2:
                    A.wr(o + 0x9D4, 4, alt); marks.append('scribble_table')

        if A.rd(o + 0x9D4, 4):
            marks.append('table_set')
            n = numguns()
            if n > 0:
                marks.append('guns_positive')
                i = 0
                while True:
                    tbl = A.rd(o + 0x9D4, 4)
                    fx = W.rd(tbl + 4 * i, 4)
                    if fx:
                        marks.append('gun_killed')
                        kill(fx)
                        tbl = A.rd(o + 0x9D4, 4)
                        events.append([0x4A9810, G_FXMAN, W.tagv(W.rd(tbl + 4 * i, 4))])
                    else:
                        marks.append('gun_empty')
                    i += 1
                    n = numguns()
                    if not i < n:
                        break
            else:
                marks.append('guns_not_positive')
            tbl = A.rd(o + 0x9D4, 4)
            if tbl != table:
                marks.append('table_changed')
            events.append([0x8213AE, W.tagv(tbl)])
            A.wr(o + 0x9D4, 4, 0)
        else:
            marks.append('table_null')
        jets = False
        for i in range(4):
            p = A.rd(o + 0x9E8 + 4 * i, 4)
            if p:
                jets = True
                kill(p)
                A.wr(o + 0x9E8 + 4 * i, 4, 0)
        marks.append('jets_set' if jets else 'jets_null')
        p = A.rd(o + 0x9F8, 4)
        if p:
            marks.append('smoke_set')
            kill(p)
            A.wr(o + 0x9F8, 4, 0)
        else:
            marks.append('smoke_null')
        events.append([0x4FB8C0, W.tagv(this + 0x138)])
        events.append([0x6A61E0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[46])
    elif k == QUADBODY:
        W.snap()
        A.wr(o, 4, 0x871AE8)
        events.append([0x6D0B30, W.tagv(this + 0x98C)])
        events.append([0x6A61E0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
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
