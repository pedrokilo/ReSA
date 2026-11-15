"""Oraculo independiente (Python) de Fn189A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 29 funciones del lote 189A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, dumpbin /disasm).
   0  0x407670  destructor con borrado de CDirectory: push esi; esi = ecx; call 5322D0 (~CDirectory: eax = [ecx]; si no es nulo y el byte [ecx+0Ch] no es cero: operator delete 8214BD(eax));
            test byte [esp+8],1; si el bit 0: operator delete 8214BD(esi); eax = esi; ret 4
   1  0x407800  IsObjectInCdImage(i): eax = i + i*4; ecx = [eax*4 + 8E4CCCh]; al = (ecx != 0); ret
   2  0x407A10  ForceLayerToRead(x): byte [9654C4h] = 1; dword [9654C8h] = x; ret
   3  0x407A30  SetLoadVehiclesInLoadScene(b): al = byte [esp+4]; byte [8A5A88h] = al; ret
   4  0x407AC0  RemoveUnusedModelsInLoadedList: ret
   5  0x407BE0  ImGonnaUseStreamingMemory: ret
   6  0x407BF0  IHaveUsedStreamingMemory: ret
   7  0x407D30  GetDefaultAmbulanceModel: eax = [0BA6718h]; eax = [eax*4 + 8A5AB4h]; ret
   8  0x407DC0  GetDefaultFireEngineModel: igual con 8A5AD4h
   9  0x407F20  LoadInitialVehicles: ret
  10  0x408230  CLinkList<CEntity*>::Insert(&dato): eax = [ecx+20h]; si eax == ecx+24h: eax = 0, ret 4; edx = [esp+4]; edx = [edx]; [eax] = edx; edx = [eax+8]; esi = [eax+4]; [edx+4] = esi;
            edx = [eax+4]; esi = [eax+8]; [edx+8] = esi; edx = [ecx+8]; [eax+8] = edx; edx = [ecx+8]; [edx+4] = eax; [eax+4] = ecx; [ecx+8] = eax; ret 4 (eax = el enlace)
  11  0x4082A0  CPool<CPed,CCopPed>::GetAt(i): edx = [ecx+4]; eax = i; si el bit 7 de byte [edx+eax] esta puesto: eax = 0; si no: eax = i*7C4h + [ecx]; ret 4
  12  0x4096C0  StartRenderEntities: dword [8E48A0h] = 9654F0h; ret
  13  0x40B490  SetMissionDoesntRequireSpecialChar(id): [esp+4] = id + 122h; jmp 409C90
  14  0x40C1C0  RemoveStreamedScriptModel(id): [esp+4] = id + 6676h; jmp 4089A0
  15  0x40C450  ProcessEntitiesInSectorList(lista, flags): ebp = [lista]; mientras no sea nulo: esi = [nodo]; ebp = [nodo+4]; ax = word [0B7CD78h]; si word [esi+2Ch] == ax: siguiente; word [esi+2Ch] = ax;
            eax = movsx word [esi+22h]; si byte [eax*20 + 8E4CD0h] == 1: siguiente; ecx = [esi+1Ch]; si ch & 4: siguiente; dl = byte [esi+2Fh]; si edx(zx) != [0B72914h] y dl != 0Dh: siguiente;
            si ecx & 80000h: siguiente; si !(cl & 80h): siguiente; edi = [eax*4 + 0A9B0C8h]; eax = [edi]->[+14h]() (ecx = edi); si eax != 0: si !52CEE0(byte [eax], byte [eax+1]): siguiente;
            si [edi+1Ch] != 0 y [esi+18h] == 0: [esi]->[+1Ch]() (ecx = esi); 4087E0(movsx word [esi+22h], flags); siguiente
  16  0x5B8340  constructor de CPool<CVehicleStructure>(n, nombre): edi = n; eax = 821195(n * 314h) (32 bits); [esi] = eax; eax = 821195(n); [esi+4] = eax; byte [esi+10h] = 1; [esi+8] = n; [esi+0Ch] = -1;
            para i en 0..n-1 (con signo): byte [[esi+4]+i] = 80h; eax = esi; ret 8
  17..20  std::fill 4 bytes (0x5EAAA0, 0x5EAAF0, 0x5EAB10: identicas) y 20 bytes (0x5EAAC0): para cada elemento de [primero, ultimo): copia el elemento desde [valor] (dword a dword, en orden)
  21  0x5EAB80  std::copy 20 bytes (out, primero, ultimo, destino): copia elemento a elemento hacia delante (dword a dword); [out] = final del destino; eax = out
  22  0x5EAC50  std::copy_backward 20 bytes (out, primero, ultimo, final): recorre desde el final (elemento a elemento, dword a dword hacia delante dentro del elemento); [out] = inicio del destino; eax = out
  23  0x5EACC0  igual con 12 bytes (tres dwords, cargas y almacenes intercalados)
  24..28  std::uninitialized_copy 4 bytes (0x5EAE10, 0x5EAE70, 0x5EAEE0), 20 bytes (0x5EAE40), 12 bytes (0x5EAEA0): (primero, ultimo, destino): por elemento: si el destino no es nulo copia; avanza ambos; eax = destino final

Los falsos anotan: [0x8214BD, argumento (etiqueta o crudo)]; [0x821195, tamano, dword [pool+0], dword [pool+4]]; [0x409C90 o 0x4089A0, argumento]; [0x52CEE0, encendido, apagado];
[0x4087E0, indice, banderas]; [0x40C4F0, ECX]; [0x40C4F1, ECX].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CDirectory::ScalarDeletingDestructor", "CStreaming::IsObjectInCdImage", "CStreaming::ForceLayerToRead", "CStreaming::SetLoadVehiclesInLoadScene", "CStreaming::RemoveUnusedModelsInLoadedList",
         "CStreaming::ImGonnaUseStreamingMemory", "CStreaming::IHaveUsedStreamingMemory", "CStreaming::GetDefaultAmbulanceModel", "CStreaming::GetDefaultFireEngineModel", "CStreaming::LoadInitialVehicles",
         "CLinkList<CEntity*>::Insert-Data", "CPedPool::GetAt", "CStreaming::StartRenderEntities", "CStreaming::SetMissionDoesntRequireSpecialChar", "CStreaming::RemoveStreamedScriptModel",
         "CStreaming::ProcessEntitiesInSectorList-Simple", "CPools::VehicleStructPoolConstructor", "CPedAttractor::SArrayFillStride4A", "CPedAttractor::SArrayFillStride20", "CPedAttractor::SArrayFillStride4B",
         "CPedAttractor::SArrayFillStride4C", "CPedAttractor::SArrayCopyStride20", "CPedAttractor::SArrayCopyBackwardStride20", "CPedAttractor::SArrayCopyBackwardStride12",
         "CPedAttractor::SArrayUninitCopyStride4A", "CPedAttractor::SArrayUninitCopyStride20", "CPedAttractor::SArrayUninitCopyStride4B", "CPedAttractor::SArrayUninitCopyStride12",
         "CPedAttractor::SArrayUninitCopyStride4C"]
ENTRIES = (0x407670, 0x407800, 0x407A10, 0x407A30, 0x407AC0, 0x407BE0, 0x407BF0, 0x407D30, 0x407DC0, 0x407F20, 0x408230, 0x4082A0, 0x4096C0, 0x40B490, 0x40C1C0, 0x40C450, 0x5B8340,
           0x5EAAA0, 0x5EAAC0, 0x5EAAF0, 0x5EAB10, 0x5EAB80, 0x5EAC50, 0x5EACC0, 0x5EAE10, 0x5EAE40, 0x5EAE70, 0x5EAEA0, 0x5EAEE0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(DIRDTOR, INCD, FORCE, SETVEH, NOP, LEVEL, LINKINS, PEDGETAT, STARTRENDER, MISSION, PROCESS, POOLCTOR, FILL4, FILL20, COPY20, COPYBACK20, COPYBACK12, UNIN4, UNIN20, UNIN12) = range(20)
KIND = [DIRDTOR, INCD, FORCE, SETVEH, NOP, NOP, NOP, LEVEL, LEVEL, NOP, LINKINS, PEDGETAT, STARTRENDER, MISSION, MISSION, PROCESS, POOLCTOR,
        FILL4, FILL20, FILL4, FILL4, COPY20, COPYBACK20, COPYBACK12, UNIN4, UNIN20, UNIN4, UNIN12, UNIN4]
STRIDE = {FILL4: 4, UNIN4: 4, COPYBACK12: 12, UNIN12: 12, FILL20: 20, COPY20: 20, COPYBACK20: 20, UNIN20: 20}
CD_BASE, INFO_BASE, INFO_TABLE, CURR_LEVEL, AMB_TABLE, FIRE_TABLE, TABLES = 0x8E4CCC, 0x8E4CC0, 0xA9B0C8, 0xBA6718, 0x8A5AB4, 0x8A5AD4, 0x8A5AB4
FORCE_BYTE, LOAD_VEH, RENDER_LINK, MARKER, ID_STACK, MEM_ID, MEM_USED_MGR, MEM_USED_BYTES = 0x9654C4, 0x8A5A88, 0x8E48A0, 0xA9B0B0, 0xC87C18, 0xC87B1C, 0xC87B34, 0x8E4CB4
SCAN_CODE, CURR_AREA, POOL_SINGLE = 0xB7CD78, 0xB72914, 0xB74484
CD_SLOTS = (0, 1, 7000, 26315)
MODEL_IDX = (-3, -2, -1, 0, 1, 5, 100, 20000)


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


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


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

    def __init__(self, c, stl):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        # las funciones de la STL rellenan la arena B siempre con datos aleatorios (los demas rellenos harian invisibles las copias)
        self.mem = tuple(Mem(0 if (stl and i == 1) else a[2], a[92 + i]) for i in range(4))
        self.glob = {}      # memoria global: direccion -> byte (el valor inicial es el que prepara el caso)
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
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
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def getg(self, addr, n):
        return sum(self.glob[addr + k] << (8 * k) for k in range(n))

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        """cambios de las regiones de la funcion respecto a la imagen preparada: [[direccion, byte]] en el orden de las regiones"""
        out = []
        for addr, n in order:
            for k in range(n):
                if self.glob.get(addr + k) != self.glob_ref.get(addr + k):
                    out.append([addr + k, self.glob[addr + k]])
        return out


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == DIRDTOR:
        _k += ['entries_null', 'entries_set', 'owns_zero', 'owns_nonzero', 'entries_freed', 'self_freed', 'self_kept']
    elif _kind == INCD:
        _k += ['cd_zero', 'cd_nonzero', 'alias', 'real', 'high_bits']
    elif _kind == FORCE:
        _k += ['byte_was_one', 'byte_was_other', 'arg_zero']
    elif _kind == SETVEH:
        _k += ['arg_bool', 'arg_nonbool', 'arg_low_zero']
    elif _kind == NOP:
        _k += ['noop']
    elif _kind == LEVEL:
        _k += ['real', 'alias', 'high_bits']
    elif _kind == LINKINS:
        _k += ['used0', 'used_many', 'free0', 'free_some']
    elif _kind == PEDGETAT:
        _k += ['slot_free', 'slot_used', 'storage_arena']
    elif _kind == STARTRENDER:
        _k += ['prev_other']
    elif _kind == MISSION:
        _k += ['arg_zero', 'arg_negative', 'arg_wraps']
    elif _kind == PROCESS:
        _k += ['empty_list', 'many_nodes', 'scan_skip', 'loaded_skip', 'dontdelete_skip', 'area_skip', 'area13', 'dontstream_skip', 'invisible_skip', 'time_null', 'time_out', 'time_in',
               'create_rw', 'rw_exists', 'requested', 'neg_model', 'rewrites_model', 'dup_visit']
    elif _kind == POOLCTOR:
        _k += ['cap_pos', 'cap_zero', 'cap_neg', 'cap_wraps']
    else:
        _k += ['n_zero', 'n_one', 'n_many', 'disjoint', 'overlap', 'unaligned']
        if _kind in (FILL4, FILL20):
            _k += ['value_inside', 'value_outside']
        if _kind in (UNIN4, UNIN20, UNIN12):
            _k += ['null_dest']
        if _kind in (COPY20, COPYBACK20, COPYBACK12):
            _k += ['shift_forward', 'shift_back']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c, k >= FILL4)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == DIRDTOR:
        entries = 0 if a[8] == 0 else (W.raw(1, a[9]) if a[8] == 1 else a[4])
        A.wr(o + 0, 4, entries); A.wr(o + 4, 4, a[5]); A.wr(o + 8, 4, a[6]); A.wr(o + 0xC, 1, a[7] & 0xFF)
        W.snap()
        owns = a[7] & 0xFF
        marks.append('entries_null' if entries == 0 else 'entries_set')
        marks.append('owns_zero' if owns == 0 else 'owns_nonzero')
        if entries != 0 and owns != 0:
            events.append([0x8214BD, W.tagv(entries)])
            marks.append('entries_freed')
        if a[10] & 1:
            events.append([0x8214BD, W.tagv(this)])
            marks.append('self_freed')
        else:
            marks.append('self_kept')
        ret = W.tagv(this)
    elif k == INCD:
        if a[4] == 0:
            B.wr(a[7], 4, a[6])
            marks += ['alias'] + (['high_bits'] if a[8] else [])
        else:
            slot = CD_SLOTS[a[5]]
            W.setg((INFO_BASE + slot * 20 + 0xC) & M32, 4, a[6])
            marks.append('real')
        gorder = [((INFO_BASE + s * 20 + 0xC) & M32, 4) for s in CD_SLOTS]
        W.snap()
        ret = 1 if a[6] != 0 else 0
        marks.append('cd_nonzero' if a[6] != 0 else 'cd_zero')
    elif k == FORCE:
        W.setg(FORCE_BYTE, 4, a[5]); W.setg(FORCE_BYTE + 4, 4, a[6])
        gorder = [(FORCE_BYTE, 8)]
        W.snap()
        W.glob[FORCE_BYTE] = 1
        W.setg(FORCE_BYTE + 4, 4, a[4])
        marks.append('byte_was_one' if a[5] & 0xFF == 1 else 'byte_was_other')
        if a[4] == 0:
            marks.append('arg_zero')
    elif k == SETVEH:
        W.setg(LOAD_VEH, 4, a[5])
        gorder = [(LOAD_VEH, 4)]
        W.snap()
        W.glob[LOAD_VEH] = a[4] & 0xFF
        low = a[4] & 0xFF
        marks.append('arg_low_zero' if low == 0 else ('arg_bool' if low == 1 else 'arg_nonbool'))
    elif k == NOP:
        W.setg(MARKER, 1, 0x5A)
        W.setg(MEM_USED_BYTES, 4, a[24])
        gorder = [(MARKER, 1), (ID_STACK, 0x48), (MEM_ID, 4), (MEM_USED_MGR, 4), (MEM_USED_BYTES, 4)]
        W.snap()
        marks.append('noop')
    elif k == LEVEL:
        table = AMB_TABLE if fn == 7 else FIRE_TABLE
        for i in range(24):
            W.setg(TABLES + 4 * i, 4, a[8 + i])
        if a[4] == 0:
            level = a[5]
            ret = a[8 + (table - TABLES) // 4 + level]
            marks.append('real')
        else:
            target = W.raw(1, a[6])
            B.wr(a[6], 4, a[32])
            level = ((((target - table) & M32) >> 2) & 0x3FFFFFFF) | (a[7] << 30)
            ret = a[32]
            marks.append('alias')
            if a[7]:
                marks.append('high_bits')
        W.setg(CURR_LEVEL, 4, level & M32)
        gorder = [(CURR_LEVEL, 4), (TABLES, 0x60)]
        W.snap()
    elif k == LINKINS:
        n_used, n_free = a[4], a[5]
        used, freed = [], []
        for i in range(n_used + n_free):
            node = W.raw(1, a[6] + 0x20 * a[8 + i])
            (used if i < n_used else freed).append(node)

        def chain(head, mids, tail):
            ch = [head] + mids + [tail]
            for i in range(len(ch)):
                if i > 0:
                    W.wr(ch[i] + 4, 4, ch[i - 1])
                if i + 1 < len(ch):
                    W.wr(ch[i] + 8, 4, ch[i + 1])
        chain(this, used, this + 0xC)
        chain(this + 0x18, freed, this + 0x24)
        data = W.raw(2, a[27])
        Cc.wr(a[27], 4, a[24])
        W.snap()
        marks.append('used0' if n_used == 0 else 'used1')
        if n_used >= 2:
            marks.append('used_many')
        marks.append('free0' if n_free == 0 else 'free_some')
        eax = W.rd(this + 0x20)
        if eax == this + 0x24:
            ret = 0
        else:
            W.wr(eax, 4, W.rd(data))
            edx = W.rd(eax + 8); esi = W.rd(eax + 4); W.wr(edx + 4, 4, esi)
            edx = W.rd(eax + 4); esi = W.rd(eax + 8); W.wr(edx + 8, 4, esi)
            edx = W.rd(this + 8); W.wr(eax + 8, 4, edx)
            edx = W.rd(this + 8); W.wr(edx + 4, 4, eax)
            W.wr(eax + 4, 4, this)
            W.wr(this + 8, 4, eax)
            ret = W.tagv(eax)
    elif k == PEDGETAT:
        storage = W.raw(2, a[5]) if a[4] == 1 else a[5]
        A.wr(o + 0, 4, storage)
        A.wr(o + 4, 4, W.raw(3, a[6]))
        idx = a[7]
        A.wr(o + 8, 4, a[9])
        D.wr(a[6] + idx, 1, a[8])
        W.snap()
        if a[4] == 1:
            marks.append('storage_arena')
        if a[8] & 0x80:
            ret = 0
            marks.append('slot_free')
        else:
            ret = W.tagv((storage + idx * 0x7C4) & M32)
            marks.append('slot_used')
    elif k == STARTRENDER:
        W.setg(RENDER_LINK, 4, a[4]); W.setg(RENDER_LINK + 4, 4, a[5])
        gorder = [(RENDER_LINK, 8)]
        W.snap()
        W.setg(RENDER_LINK, 4, 0x9654F0)
        if a[4] != 0x9654F0:
            marks.append('prev_other')
    elif k == MISSION:
        add = 0x122 if fn == 13 else 0x6676
        W.snap()
        events.append([0x409C90 if fn == 13 else 0x4089A0, (a[4] + add) & M32])
        if a[4] == 0:
            marks.append('arg_zero')
        if a[4] & 0x80000000:
            marks.append('arg_negative')
        if a[4] + add > M32:
            marks.append('arg_wraps')
    elif k == PROCESS:
        W.setg(SCAN_CODE, 4, a[15]); W.setg(CURR_AREA, 4, a[44])
        gorder = [(SCAN_CODE, 4), (CURR_AREA, 4), (POOL_SINGLE, 4)]
        for j in range(8):
            idx = MODEL_IDX[j] & M32
            W.setg((INFO_BASE + idx * 20 + 0x10) & M32, 4, a[36 + j])
            gorder.append(((INFO_BASE + idx * 20 + 0x10) & M32, 4))
            obj = a[47] + 0x40 * j
            W.setg((INFO_TABLE + 4 * idx) & M32, 4, W.raw(3, obj))
            gorder.append(((INFO_TABLE + 4 * idx) & M32, 4))
            D.wr(obj + 0, 4, W.raw(3, a[46]))
            D.wr(obj + 0x1C, 4, a[49 + j])
            D.wr(obj + 0x20, 4, W.raw(3, a[48] + 4 * j) if a[57 + j] else 0)
            if a[57 + j]:
                D.wr(a[48] + 4 * j, 4, a[65 + j])
        for e in range(4):
            base = a[7] + 0x40 * e
            Cc.wr(base + 0, 4, W.raw(3, a[45]))
            Cc.wr(base + 0x18, 4, a[32 + e])
            Cc.wr(base + 0x1C, 4, a[24 + e])
            Cc.wr(base + 0x22, 2, MODEL_IDX[a[20 + e]] & 0xFFFF)
            Cc.wr(base + 0x2C, 2, a[16 + e] & 0xFFFF)
            mode = a[28 + e]
            Cc.wr(base + 0x2F, 1, (a[44] & 0xFF) if mode == 0 else (13 if mode == 1 else ((a[44] + 1 + e * 7) & 0xFF)))
        n_nodes = a[4]
        for s in range(n_nodes):
            B.wr(a[6] + 8 * s, 4, W.raw(2, a[7] + 0x40 * a[8 + s]))
            B.wr(a[6] + 8 * s + 4, 4, W.raw(1, a[6] + 8 * (s + 1)) if s + 1 < n_nodes else 0)
        A.wr(o, 4, W.raw(1, a[6]) if n_nodes > 0 else 0)
        W.snap()
        if n_nodes == 0:
            marks.append('empty_list')
        if n_nodes >= 3:
            marks.append('many_nodes')
        cur = a[15] & 0xFFFF
        flags_arg = a[14]
        clock_call = 0
        node = W.rd(this)
        visited = set()
        while node:
            item = W.rd(node)
            nxt = W.rd(node + 4)
            if W.rd(item + 0x2C, 2) == cur:
                marks.append('scan_skip')
                if item in visited:
                    marks.append('dup_visit')
                node = nxt
                continue
            visited.add(item)
            W.wr(item + 0x2C, 2, cur)
            idx = s16(W.rd(item + 0x22, 2))
            if idx < 0:
                marks.append('neg_model')
            if W.getg((INFO_BASE + idx * 20 + 0x10) & M32, 1) == 1:
                marks.append('loaded_skip')
                node = nxt
                continue
            flags = W.rd(item + 0x1C, 4)
            if flags & 0x400:
                marks.append('dontdelete_skip')
                node = nxt
                continue
            area = W.rd(item + 0x2F, 1)
            if area != a[44] and area != 13:
                marks.append('area_skip')
                node = nxt
                continue
            if area == 13 and area != a[44]:
                marks.append('area13')
            if flags & 0x80000:
                marks.append('dontstream_skip')
                node = nxt
                continue
            if not (flags & 0x80):
                marks.append('invisible_skip')
                node = nxt
                continue
            mi = W.getg((INFO_TABLE + 4 * idx) & M32, 4)
            events.append([0x40C4F1, W.tagv(mi)])
            ti = W.rd(mi + 0x20)
            if ti != 0:
                events.append([0x52CEE0, W.rd(ti, 1), W.rd(ti + 1, 1)])
                bit = (a[73] >> (clock_call & 31)) & 1
                clock_call += 1
                if not bit:
                    marks.append('time_out')
                    node = nxt
                    continue
                marks.append('time_in')
            else:
                marks.append('time_null')
            if W.rd(mi + 0x1C) != 0 and W.rd(item + 0x18) == 0:
                events.append([0x40C4F0, W.tagv(item)])
                marks.append('create_rw')
                if a[75]:
                    W.wr(item + 0x22, 2, a[76] & 0xFFFF)
                    marks.append('rewrites_model')
            elif W.rd(mi + 0x1C) != 0:
                marks.append('rw_exists')
            events.append([0x4087E0, s16(W.rd(item + 0x22, 2)) & M32, flags_arg])
            marks.append('requested')
            node = nxt
    elif k == POOLCTOR:
        cap = s32(a[4])
        pre0, pre4 = A.rd(o, 4), A.rd(o + 4, 4)
        ret1 = W.raw(1, a[5]) if cap > 0 else a[8]
        ret2 = W.raw(2, a[6]) if cap > 0 else a[9]
        W.snap()
        events.append([0x821195, (cap * 0x314) & M32, pre0, pre4])
        A.wr(o + 0, 4, ret1)
        events.append([0x821195, cap & M32, ret1, pre4])
        A.wr(o + 4, 4, ret2)
        A.wr(o + 0x10, 1, 1); A.wr(o + 8, 4, cap & M32); A.wr(o + 0xC, 4, M32)
        for i in range(cap):
            W.wr(ret2 + i, 1, 0x80)
        ret = W.tagv(this)
        marks.append('cap_pos' if cap > 0 else ('cap_zero' if cap == 0 else 'cap_neg'))
        if cap * 0x314 > M32 or cap * 0x314 < 0:
            marks.append('cap_wraps')
    else:
        stride = STRIDE[k]
        n = a[4]
        first = W.raw(1, a[7])
        last = (first + n * stride) & M32
        dst = (first + s32(a[8]) * stride) & M32 if a[6] == 1 else (W.raw(2, a[9]) + a[10]) & M32
        if a[14] == 1:
            dst = 0
        marks.append('n_zero' if n == 0 else ('n_one' if n == 1 else 'n_many'))
        marks.append('overlap' if a[6] == 1 else 'disjoint')
        if a[5] or a[10]:
            marks.append('unaligned')
        words = stride // 4
        W.snap()

        def copy_el(src, dest):
            for w in range(words):
                W.wr(dest + 4 * w, 4, W.rd(src + 4 * w))
        if k in (FILL4, FILL20):
            value = (first + a[13] * stride) & M32 if (a[11] == 1 and n > 0) else W.raw(3, a[12])
            marks.append('value_inside' if (a[11] == 1 and n > 0) else 'value_outside')
            for i in range(n):
                copy_el(value, first + i * stride)
        elif k == COPY20:
            out = this
            for i in range(n):
                copy_el(first + i * stride, dst + i * stride)
            W.wr(out, 4, (dst + n * stride) & M32)
            ret = W.tagv(out)
            if a[6] == 1:
                marks.append('shift_forward' if s32(a[8]) > 0 else 'shift_back')
        elif k in (COPYBACK20, COPYBACK12):
            out = this
            l, d = last, (dst + n * stride) & M32
            for i in range(n):
                l = (l - stride) & M32
                d = (d - stride) & M32
                copy_el(l, d)
            W.wr(out, 4, d)
            ret = W.tagv(out)
            if a[6] == 1:
                marks.append('shift_forward' if s32(a[8]) > 0 else 'shift_back')
        else:
            d = dst
            if dst == 0:
                marks.append('null_dest')
            for i in range(n):
                if d != 0:
                    copy_el(first + i * stride, d)
                d = (d + stride) & M32
            ret = W.tagv(d)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


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
