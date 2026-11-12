"""Oraculo independiente (Python) de Fn172A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 172A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de los bloques del monton (Fixtures.hpp /
LiveComparison.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; hardware de audio = objeto global 0xB5F8B8):
   0  0x4DBF10  esi = ecx; ecx = 0xB5F8B8; call 4D94D0 (DisableBassEq); eax = WORD [esi+4] (extension a cero); push 27h, 0, eax; call 4D9500 (SetChannelFlags: canal, 0, 0x27); ecx = WORD [esi+4] (a cero);
                push 0, 0xC0400000, 0, ecx; call 4D8870 (SetChannelVolume: canal, 0, -3.0f, 0); jmp 4D8F30 (StartTrackPlayback)
   1  0x4DBF60  si [0xB7CB49] o [0xB7CB48] != 0 -> 0.0f; si no, el byte [this+1] == 0 -> 1.0f, si no 0.0f; movsx ecx, WORD [this+4]; push f, 0, ecx; call 4D8960 (SetChannelFrequencyScalingFactor: canal, 0, f)
   2  0x4DC0E0  [this+0Ch] = movsx WORD arg1; [this+14h] = 0; [this+8] = 0; si el BYTE arg2 != 0: repetir { call 4D8F60 (GetTrackPlayTime); Service 4DBFB0(tiempo); call 4D9870 (Service del hardware) }
                mientras [this+8] != 2
   3  0x4DC190  call 4F1750 (Initialise) con ecx = this; eax = this
   4  0x4EFD30  vector constructor 821D30 de 300 elementos de 0x74 bytes desde this+4 con el constructor 446FE0 (mov eax,ecx; mov [eax+8],0): dword +8 de cada elemento a cero; eax = this
   5  0x4F0B10  constructor de la base 4D7890 (pDS, canal, frecuencia, bits): +4 = pDS; WORD +3Ah = canal; +3Ch, +40h, +4Bh = frecuencia; +4Fh = (bits_word >> 3) * frecuencia; WORD +55h = bits_word; +28h, +5Ch = 0;
                WORDs +57h = 0, +53h = 2, +49h = 1, +47h = 1; +34h = 0xC2C80000; +8, +0Ch = 0; BYTEs +38h, +44h, +45h = 0; luego +60h, +62h = 0 (bytes), vtable 85F3CCh en +0 y BYTE +8Ah = tercer argumento; eax = this
   6  0x4F0B50  [this] = 85F3CCh; si [this+8] != 0: dec [0xB5F898]; si [this+8] != 0: Release (ranura 2) de [this+8]; q = [this+0Ch]; [this+8] = 0; si q: Release(q); [this+0Ch] = 0;
                call 4D7910 (cuerpo de la base: la misma secuencia con vtable 85F03Ch)
   7  0x506B20  [this] = 862E54h; 4F2B80 sobre this+144h; 4F2B80 sobre this+9Ch; [this] = 859988h; 4EF660 sobre this+8
   8  0x506BA0  [this] = 862E58h; [this+19Ch] = 859988h; 4EF660 sobre this+1A4h; [this+114h] = 859988h; 4EF660 sobre this+11Ch; [this] = 859988h; 4EF660 sobre this+8
   9  0x506C30  [ecx] = 859988h; ecx += 8; jmp 4EF660          10  0x506C40  igual
  11  0x508930  (cadena {puntero, longitud}) new(len + n) [0x82119A]; a cero; copia len bytes de la cadena y n bytes de source; delete del bloque anterior [0x8214BD]; puntero = bloque; longitud = len + n
  12  0x508A10  si [this] != 0: delete [this]; [this] = 0; [this+4] = 0     13  0x509140  igual
  14  0x5B19D0  call 5A1D10 (ctor de CObject); [this] = 868A60h; dwords +184h..+198h (seis) = 0; byte36 = [this+36h]; flags = [this+1Ch]; [this+36h] = (byte36 & 7) | 0x10; flags = (flags & ~1) | 0x10000400; [this+13Ch] = 4 (byte);
                [this+17Ch] = [this+180h] = 0; [this+1Ch] = flags; call 5A0760 (SetIsStatic(0)); [this+1Ch] &= 0xF7FFFFFF; [this+12Ch] = 0x3F000000; eax = this
  15  0x5B1A80  [ecx] = 868A60h; jmp 59F660
  16  0x5B1A90  call 5D5B20(atomic); eax = atomic (cdecl, el llamador limpia)
  17  0x49C8F0  [this] = new[](count * 0x44); [this+4] = new[](count); BYTE [this+10h] = 1; [this+8] = count; [this+0Ch] = -1; si count > 0: estados[i] |= 0x80; estados[i] &= 0x80; eax = this
  18  0x49C950  si [this+8] > 0: si BYTE [this+10h] != 0: delete[] [this]; delete[] [this+4]; [this], [this+4], [this+8], [this+0Ch] = 0
  19  0x49C9A0  n = [this+8]; si n > 0: i = 0; repetir { s = [this+4]; s[i] |= 0x80; s = [this+4]; s[i] &= 0x80; n = [this+8]; i++ } mientras i < n
  20  0x49C9E0  size = [this+8]; w = 0; repetir { n = [this+0Ch] + 1; [this+0Ch] = n; si n == size: [this+0Ch] = 0; si w: eax = 0; w = 1; } hasta que [[this+4] + [this+0Ch]] & 0x80;
                s = [this+4]; i = [this+0Ch]; s[i] &= 0x7F; s[i] = (s[i] & 0x80) | ((s[i] + 1) & 0x7F); eax = [this+0Ch] * 0x44 + [this]
  21  0x438D60  health = float(BYTE [0xB7CEE7]); ped = FindPlayerPed(-1); [ped+540h] = health; v = FindPlayerVehicle(-1, 0) (llamada 1); si 0 termina; [FindPlayerVehicle()+4C0h] = 1000.0f; t = [FindPlayerVehicle()+590h];
                si t != 0: si [FindPlayerVehicle()+590h] != 9 termina; luego [FindPlayerVehicle()+590h] == 0: [FindPlayerVehicle()+8E4h] = 0 si no [FindPlayerVehicle()+7BCh] = 0; FindPlayerVehicle()->ranura 32h (Fix)
  22  0x43A6A0  eax = 1 si arg1 == 0x16 o 5, si no 0

Los falsos anotan: metodos del hardware [direccion, ECX, argumentos crudos]; GetTrackPlayTime [4D8F60, ECX] (devuelve tiempo ^ (llamada * 0x9E3779B1)); Service [4DBFB0, ECX, argumento] (pone el estado de +8: 2 en la vuelta k, otro antes);
4F2B80 / 4EF660 [direccion, ECX, dword de [ECX], dword del objeto contenedor] (dejan un centinela en [ECX]); CObject [5A1D10, ECX, +1Ch, +34h] y SetIsStatic [5A0760, ECX, arg, +1Ch, +34h, +13Ch, +17Ch, +184h]; cuerpo 59F660 [direccion, ECX,
dword de [ECX]]; Release [0xC0DE, objeto, +8 y +0Ch del canal, contador]; FindPlayerPed [56E210, arg]; FindPlayerVehicle [56E0D0, arg1, arg2]; Fix [0xF1F1, ECX, dword de [ECX]]; CustomCarPipeAtomicSetup [5D5B20, arg].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CAECutsceneTrackManager::StartTrackPlayback", "CAECutsceneTrackManager::CheckForPause", "CAECutsceneTrackManager::PreloadCutsceneTrack", "CAEStreamTransformer::Constructor", "CAESoundManager::Constructor",
         "CAEStaticChannel::Constructor", "CAEStaticChannel::DestructorBody", "CAEFrontendAudioEntity::DestructorBody", "CAEScriptAudioEntity::DestructorBody", "CAEAudioEntity::Destructor-4",
         "CAEAudioEntity::Destructor-5", "CAudioZones::TextBufferAppend", "CAudioZones::TextBufferClear", "CAudioZones::TextBufferClear-2", "CCutsceneObject::Constructor", "CCutsceneObject::DestructorBody",
         "CCutsceneObject::SetupCarPipeAtomicCB", "CStuntJumpManager::PoolConstructor", "CStuntJumpManager::PoolFlush", "CStuntJumpManager::PoolClear", "CStuntJumpManager::PoolNew", "CCheat::HealthCheat",
         "CCheat::IsFemalePedType"]
ENTRIES = (0x4DBF10, 0x4DBF60, 0x4DC0E0, 0x4DC190, 0x4EFD30, 0x4F0B10, 0x4F0B50, 0x506B20, 0x506BA0, 0x506C30, 0x506C40, 0x508930, 0x508A10, 0x509140, 0x5B19D0, 0x5B1A80, 0x5B1A90,
           0x49C8F0, 0x49C950, 0x49C9A0, 0x49C9E0, 0x438D60, 0x43A6A0)
GUARD = 0x40
WIN = 0x1000
WIN_BIG = 0x9000
VT_OFF = 0xE00
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
HW = 0xB5F8B8

(START, PAUSE, PRELOAD, XFORM, SOUNDMGR, CHANCTOR, CHANDTOR, FRONTDTOR, SCRIPTDTOR, AUDIODTOR, TEXTAPPEND, TEXTCLEAR, CUTCTOR, CUTDTOR, ATOMICCB,
 POOLCTOR, POOLFLUSH, POOLCLEAR, POOLNEW, HEALTH, FEMALE) = range(21)
KIND = [START, PAUSE, PRELOAD, XFORM, SOUNDMGR, CHANCTOR, CHANDTOR, FRONTDTOR, SCRIPTDTOR, AUDIODTOR, AUDIODTOR, TEXTAPPEND, TEXTCLEAR, TEXTCLEAR, CUTCTOR, CUTDTOR, ATOMICCB,
        POOLCTOR, POOLFLUSH, POOLCLEAR, POOLNEW, HEALTH, FEMALE]


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


def fill_bytes(n, seed):
    """FillWords: n bytes de palabras Rand(seed, i)"""
    out = bytearray()
    for i in range(0, n, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytes(out[:n])


def fnv(data, h=2166136261):
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


def win_a(fn):
    return WIN_BIG if fn == 4 else WIN


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed, win):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.win = win
        self.n = GUARD + win + GUARD
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

    def _changed(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append((i - GUARD, self.m[i]))
        return out

    def changes(self):
        return [[o, v] for o, v in self._changed()]

    def changes_compact(self):
        ch = self._changed()
        h = 2166136261
        for o, v in ch:
            h = fnv(struct.pack('<iB', o, v), h)
        return [len(ch), h]


class World:
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12], win_a(c['fn'])), Mem(a[2], a[13], WIN), Mem(a[2], a[14], WIN))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + self.mem[i].n:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        i, off = self.locate(addr & M32)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr & M32)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + self.mem[i].n:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


KEYS = {}
_COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
for _fn in range(23):
    _k = list(_COMMON)
    _kind = KIND[_fn]
    if _kind == START:
        _k += ['channel_low', 'channel_word_high_bit', 'channel_upper_bits']
    elif _kind == PAUSE:
        _k += ['user_pause', 'code_pause', 'paused_byte', 'unpaused', 'channel_negative', 'channel_positive']
    elif _kind == PRELOAD:
        _k += ['wait_no', 'wait_yes', 'laps_1', 'laps_2', 'laps_3', 'laps_4', 'laps_5', 'track_negative', 'track_positive', 'wait_high_bits_only']
    elif _kind == XFORM:
        _k += ['prev_zero', 'prev_other']
    elif _kind == CHANCTOR:
        _k += ['hw_zero', 'hw_one', 'hw_other', 'hw_high_bits_only', 'channel_high_bits', 'bits_high_bits', 'bits_zero', 'freq_zero', 'freq_high']
    elif _kind == CHANDTOR:
        _k += ['buf_null', 'buf_com', 'buf3d_null', 'buf3d_com', 'buf3d_same', 'counter_decremented', 'counter_wraps', 'both_com']
    elif _kind in (FRONTDTOR, SCRIPTDTOR, AUDIODTOR, CUTDTOR):
        _k += ['prev_zero', 'prev_other']
    elif _kind == TEXTAPPEND:
        _k += ['len_zero', 'n_zero', 'data_null', 'data_block', 'src_b', 'src_c', 'src_unaligned', 'len_unaligned', 'n_unaligned']
    elif _kind == TEXTCLEAR:
        _k += ['data_null', 'data_block', 'len_zero', 'len_nonzero']
    elif _kind == CUTCTOR:
        _k += ['xor_zero', 'xor_clears_bit27', 'xor_other', 'bit0_prev_set', 'bit0_prev_clear', 'bit27_prev_set']
    elif _kind == ATOMICCB:
        _k += ['atomic_zero', 'atomic_raw']
    elif _kind == POOLCTOR:
        _k += ['count_zero', 'count_one', 'count_small', 'count_large']
    elif _kind == POOLFLUSH:
        _k += ['size_positive', 'size_zero', 'size_negative', 'owns_set', 'owns_clear', 'owns_high_bits_only']
    elif _kind == POOLCLEAR:
        _k += ['size_positive', 'size_zero', 'size_negative', 'states_alias', 'states_in_b']
    elif _kind == POOLNEW:
        _k += ['found', 'none_free', 'wrapped', 'start_minus1', 'start_end', 'density0', 'density1', 'density2', 'density3', 'density4']
    elif _kind == HEALTH:
        _k += ['no_vehicle', 'auto_path', 'bike_path', 'other_type_exit', 'type_mix', 'health_zero', 'health_max']
    elif _kind == FEMALE:
        _k += ['id_22', 'id_5', 'id_other', 'id_high_bits']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    return ['kind%d' % kind, 'fill%d' % fill]


def sext16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret, heap, gch = [], None, None, []
    o = a[0]
    this = W.raw(0, o)
    compact = False
    if k == START:
        A.wr(o, 4, a[5]); A.wr(o + 4, 4, a[3])
        W.snap()
        ch = a[3] & 0xFFFF
        events += [[0x4D94D0, HW], [0x4D9500, HW, ch, 0, 0x27], [0x4D8870, HW, ch, 0, 0xC0400000, 0], [0x4D8F30, HW]]
        marks.append('channel_word_high_bit' if ch & 0x8000 else 'channel_low')
        if a[3] >> 16:
            marks.append('channel_upper_bits')
    elif k == PAUSE:
        A.wr(o, 4, a[4]); A.wr(o + 4, 4, a[3])
        W.snap()
        user, code, paused = a[5] & 0xFF, a[6] & 0xFF, (a[4] >> 8) & 0xFF
        f = 0 if (user or code or paused) else 0x3F800000
        ch = sext16(a[3]) & M32
        events.append([0x4D8960, HW, ch, 0, f])
        gch = ['%02x%02x' % (code, user)]
        if user:
            marks.append('user_pause')
        if code:
            marks.append('code_pause')
        if paused and not user and not code:
            marks.append('paused_byte')
        if f:
            marks.append('unpaused')
        marks.append('channel_negative' if sext16(a[3]) < 0 else 'channel_positive')
    elif k == PRELOAD:
        A.wr(o, 4, a[8]); A.wr(o + 4, 4, a[9]); A.wr(o + 8, 4, a[6]); A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 4, a[10]); A.wr(o + 0x14, 4, a[8] ^ a[9])
        W.snap()
        A.wr(o + 0xC, 4, sext16(a[4]) & M32)
        A.wr(o + 0x14, 4, 0)
        A.wr(o + 8, 4, 0)
        wait = a[5] & 0xFF
        if wait:
            marks.append('wait_yes')
            target = a[3]
            for j in range(1, target + 1):
                t = (a[7] ^ (((j - 1) * 0x9E3779B1) & M32)) & M32
                events += [[0x4D8F60, HW], [0x4DBFB0, W.tagv(this), t]]
                status = 2 if j == target else (a[6] + j * 7) % 11
                if j != target and status == 2:
                    status = 3
                A.wr(o + 8, 4, status)
                events.append([0x4D9870, HW])
            marks.append('laps_%d' % target)
        else:
            marks.append('wait_no')
            if a[5]:
                marks.append('wait_high_bits_only')
        marks.append('track_negative' if sext16(a[4]) < 0 else 'track_positive')
    elif k == XFORM:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([0x4F1750, W.tagv(this)])
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == SOUNDMGR:
        W.snap()
        for i in range(300):
            A.wr(o + 4 + 0x74 * i + 8, 4, 0)
        ret = W.tagv(this)
        compact = True
    elif k == CHANCTOR:
        W.snap()
        pds, ch, hw, sps, bps = a[3], a[4] & 0xFFFF, a[5] & 0xFF, a[6], a[7] & 0xFFFF
        A.wr(o + 4, 4, pds)
        A.wr(o + 0x3A, 2, ch)
        for off in (0x3C, 0x40, 0x4B):
            A.wr(o + off, 4, sps)
        A.wr(o + 0x4F, 4, ((bps >> 3) * sps) & M32)
        A.wr(o + 0x55, 2, bps)
        A.wr(o + 0x28, 4, 0); A.wr(o + 0x5C, 4, 0)
        A.wr(o + 0x57, 2, 0); A.wr(o + 0x53, 2, 2); A.wr(o + 0x49, 2, 1); A.wr(o + 0x47, 2, 1)
        A.wr(o + 0x34, 4, 0xC2C80000)
        A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        for off in (0x38, 0x44, 0x45, 0x60, 0x62):
            A.wr(o + off, 1, 0)
        A.wr(o, 4, 0x85F3CC)
        A.wr(o + 0x8A, 1, hw)
        ret = W.tagv(this)
        marks.append('hw_zero' if hw == 0 else 'hw_one' if hw == 1 else 'hw_other')
        if a[5] >> 8 and hw == 0:
            marks.append('hw_high_bits_only')
        if a[4] >> 16:
            marks.append('channel_high_bits')
        if a[7] >> 16:
            marks.append('bits_high_bits')
        if bps == 0:
            marks.append('bits_zero')
        marks.append('freq_zero' if sps == 0 else 'freq_high' if sps >= 0x80000000 else 'freq_mid')
    elif k == CHANDTOR:
        A.wr(o, 4, a[3])

        def place(mode, off):
            if mode == 0:
                return 0
            ar = 1 if mode == 1 else 2
            W.mem[ar].wr(off, 4, W.raw(ar, VT_OFF))
            return W.raw(ar, off)
        p1 = place(a[4], a[5])
        p2 = p1 if a[6] == 3 else place(a[6], a[7])
        A.wr(o + 8, 4, p1); A.wr(o + 0xC, 4, p2)
        W.snap()
        counter = a[10]

        def sequence(vt):
            nonlocal counter
            A.wr(o, 4, vt)
            if A.rd(o + 8, 4) != 0:
                counter = (counter - 1) & M32
            p = A.rd(o + 8, 4)
            if p:
                events.append([0xC0DE, W.tagv(p), A.rd(o + 8, 4), A.rd(o + 0xC, 4), counter])
            q = A.rd(o + 0xC, 4)
            A.wr(o + 8, 4, 0)
            if q:
                events.append([0xC0DE, W.tagv(q), A.rd(o + 8, 4), A.rd(o + 0xC, 4), counter])
            A.wr(o + 0xC, 4, 0)
        sequence(0x85F3CC)
        sequence(0x85F03C)
        gch = [counter.to_bytes(4, 'little').hex()]
        marks.append('buf_com' if p1 else 'buf_null')
        marks.append('buf3d_com' if p2 else 'buf3d_null')
        if a[6] == 3 and p1:
            marks.append('buf3d_same')
        if p1 and p2:
            marks.append('both_com')
        if p1:
            marks.append('counter_decremented')
            if a[10] == 0:
                marks.append('counter_wraps')
    elif k in (FRONTDTOR, SCRIPTDTOR, AUDIODTOR):
        A.wr(o, 4, a[3])
        W.snap()
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')

        def sub(addr, off, sentinel):
            events.append([addr, W.tagv((this + off) & M32), A.rd(o + off, 4), A.rd(o, 4)])
            A.wr(o + off, 4, sentinel)
        if k == FRONTDTOR:
            A.wr(o, 4, 0x862E54)
            sub(0x4F2B80, 0x144, 0x5E570001)
            sub(0x4F2B80, 0x9C, 0x5E570001)
            A.wr(o, 4, 0x859988)
            sub(0x4EF660, 8, 0x5E570002)
        elif k == SCRIPTDTOR:
            A.wr(o, 4, 0x862E58)
            A.wr(o + 0x19C, 4, 0x859988)
            sub(0x4EF660, 0x1A4, 0x5E570002)
            A.wr(o + 0x114, 4, 0x859988)
            sub(0x4EF660, 0x11C, 0x5E570002)
            A.wr(o, 4, 0x859988)
            sub(0x4EF660, 8, 0x5E570002)
        else:
            A.wr(o, 4, 0x859988)
            sub(0x4EF660, 8, 0x5E570002)
    elif k == TEXTAPPEND:
        length, n = a[3], a[4]
        A.wr(o, 4, 0x5A5A5A5A)
        A.wr(o + 4, 4, length)
        src_arena = 1 if a[6] == 0 else 2
        so = a[7]
        W.snap()
        old = fill_bytes(length, a[8])
        src = bytes(W.mem[src_arena].rd(so + i, 1) for i in range(n))
        data = old + src
        heap = {'len': length + n, 'data': data.hex()}
        A.wr(o, 4, 0xA5A5A5A5)
        A.wr(o + 4, 4, length + n)
        if length == 0:
            marks.append('len_zero')
        if n == 0:
            marks.append('n_zero')
        marks.append('data_null' if (length == 0 and a[5] == 1) else 'data_block')
        marks.append('src_b' if a[6] == 0 else 'src_c')
        if so % 4:
            marks.append('src_unaligned')
        if length % 4:
            marks.append('len_unaligned')
        if n % 4:
            marks.append('n_unaligned')
    elif k == TEXTCLEAR:
        A.wr(o, 4, 0x5A5A5A5A)
        A.wr(o + 4, 4, a[4])
        W.snap()
        A.wr(o, 4, 0)
        if a[3]:
            A.wr(o + 4, 4, 0)
        marks.append('data_block' if a[3] else 'data_null')
        marks.append('len_zero' if a[4] == 0 else 'len_nonzero')
    elif k == CUTCTOR:
        W.snap()
        events.append([0x5A1D10, W.tagv(this), A.rd(o + 0x1C, 4), A.rd(o + 0x34, 4)])
        A.wr(o, 4, a[4])
        A.wr(o, 4, 0x868A60)
        for off in (0x184, 0x188, 0x18C, 0x190, 0x194, 0x198):
            A.wr(o + off, 4, 0)
        byte36 = A.rd(o + 0x36, 1)
        flags = A.rd(o + 0x1C, 4)
        marks.append('bit0_prev_set' if flags & 1 else 'bit0_prev_clear')
        if flags & 0x08000000:
            marks.append('bit27_prev_set')
        A.wr(o + 0x36, 1, (byte36 & 7) | 0x10)
        flags = ((flags & ~1) | 0x10000400) & M32
        A.wr(o + 0x13C, 1, 4)
        A.wr(o + 0x17C, 4, 0)
        A.wr(o + 0x180, 4, 0)
        A.wr(o + 0x1C, 4, flags)
        events.append([0x5A0760, W.tagv(this), 0, A.rd(o + 0x1C, 4), A.rd(o + 0x34, 4), A.rd(o + 0x13C, 4), A.rd(o + 0x17C, 4), A.rd(o + 0x184, 4)])
        A.wr(o + 0x1C, 4, A.rd(o + 0x1C, 4) ^ a[5])
        A.wr(o + 0x1C, 4, A.rd(o + 0x1C, 4) & 0xF7FFFFFF)
        A.wr(o + 0x12C, 4, 0x3F000000)
        ret = W.tagv(this)
        marks.append('xor_zero' if a[5] == 0 else 'xor_clears_bit27' if a[5] == 0x08000000 else 'xor_other')
    elif k == CUTDTOR:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x868A60)
        events.append([0x59F660, W.tagv(this), 0x868A60])
        A.wr(o, 4, a[4])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == ATOMICCB:
        W.snap()
        events.append([0x5D5B20, a[3]])
        ret = a[3]
        marks.append('atomic_zero' if a[3] == 0 else 'atomic_raw')
    elif k == POOLCTOR:
        count = a[3]
        A.wr(o, 4, 0x5A5A5A5A); A.wr(o + 4, 4, 0x5A5A5A5A)
        W.snap()
        A.wr(o, 4, 0xA5A5A5A5); A.wr(o + 4, 4, 0xA5A5A5A5)
        A.wr(o + 8, 4, count)
        A.wr(o + 0xC, 4, M32)
        A.wr(o + 0x10, 1, 1)
        states = bytes([0x80]) * count
        heap = {'nonnull': [True, True], 'states_hash': fnv(states), 'states_head': states[:8].hex()}
        ret = W.tagv(this)
        marks.append('count_zero' if count == 0 else 'count_one' if count == 1 else 'count_small' if count < 256 else 'count_large')
    elif k == POOLFLUSH:
        A.wr(o, 4, 0x5A5A5A5A); A.wr(o + 4, 4, 0x5A5A5A5A)
        A.wr(o + 8, 4, a[3]); A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[4])
        W.snap()
        if s32(a[3]) > 0:
            for off in (0, 4, 8, 0xC):
                A.wr(o + off, 4, 0)
            marks.append('size_positive')
        else:
            marks.append('size_zero' if a[3] == 0 else 'size_negative')
        marks.append('owns_set' if a[4] & 0xFF else 'owns_clear')
        if (a[4] & 0xFF) == 0 and a[4]:
            marks.append('owns_high_bits_only')
    elif k == POOLCLEAR:
        states = W.raw(0, o + 8) if a[4] == 1 else W.raw(1, a[5])
        A.wr(o + 4, 4, states); A.wr(o + 8, 4, a[3])
        W.snap()
        size = A.rd(o + 8, 4)
        if s32(size) > 0:
            i = 0
            while True:
                s = A.rd(o + 4, 4)
                W.wr(s + i, 1, W.rd(s + i, 1) | 0x80)
                s = A.rd(o + 4, 4)
                W.wr(s + i, 1, W.rd(s + i, 1) & 0x80)
                size = A.rd(o + 8, 4)
                i += 1
                if not i < s32(size):
                    break
        marks.append('size_positive' if s32(a[3]) > 0 else 'size_zero' if a[3] == 0 else 'size_negative')
        marks.append('states_alias' if a[4] == 1 else 'states_in_b')
    elif k == POOLNEW:
        for i in range(a[3]):
            r7 = a[7]
            free = False if r7 == 0 else True if r7 == 1 else (rnd(a[9], i) % 4 == 0) if r7 == 2 else (rnd(a[9], i) % 4 != 0) if r7 == 3 else (i == a[10] % a[3])
            B.wr(a[5] + i, 1, (rnd(a[8], i) & 0x7F) | (0x80 if free else 0))
        A.wr(o, 4, a[6]); A.wr(o + 4, 4, W.raw(1, a[5])); A.wr(o + 8, 4, a[3]); A.wr(o + 0xC, 4, a[4])
        W.snap()
        size = A.rd(o + 8, 4)
        wrapped = False
        ret = None
        wrapped_marker = False
        while True:
            nxt = (A.rd(o + 0xC, 4) + 1) & M32
            A.wr(o + 0xC, 4, nxt)
            if nxt == size:
                A.wr(o + 0xC, 4, 0)
                if wrapped:
                    ret = 0
                    break
                wrapped = True
                wrapped_marker = True
            st = A.rd(o + 4, 4)
            idx = A.rd(o + 0xC, 4)
            if W.rd(st + idx, 1) & 0x80:
                break
        if ret is None:
            st = A.rd(o + 4, 4); idx = A.rd(o + 0xC, 4)
            W.wr(st + idx, 1, W.rd(st + idx, 1) & 0x7F)
            j = A.rd(o + 0xC, 4); st2 = A.rd(o + 4, 4)
            cur = W.rd(st2 + j, 1)
            inc = (cur + 1) & 0xFF
            W.wr(st2 + j, 1, cur ^ ((inc ^ cur) & 0x7F))
            ret = (A.rd(o + 0xC, 4) * 0x44 + A.rd(o, 4)) & M32
            marks.append('found')
        else:
            marks.append('none_free')
        if wrapped_marker:
            marks.append('wrapped')
        if a[4] == M32:
            marks.append('start_minus1')
        if a[4] == a[3] - 1:
            marks.append('start_end')
        marks.append('density%d' % a[7])
    elif k == HEALTH:
        max_health = a[3] & 0xFF
        ped = a[10]
        v1o, v2o = a[8], a[9]
        B.wr(v1o, 4, W.raw(1, VT_OFF)); B.wr(v1o + 0x590, 4, a[6])
        Cc.wr(v2o, 4, W.raw(2, VT_OFF)); Cc.wr(v2o + 0x590, 4, a[7])
        W.snap()
        v1, v2 = W.raw(1, v1o), W.raw(2, v2o)
        q = []
        for j in range(12):
            second = (a[4] & 1) if j == 0 else ((a[4] >> j) & 1)
            q.append(0 if (j == 0 and a[5] % 4 == 0) else (v2 if second else v1))
        idx = [0]

        def call():
            events.append([0x56E0D0, M32, 0])
            v = q[min(idx[0], 11)]
            idx[0] += 1
            return v

        def fix(v):
            events.append([0xF1F1, W.tagv(v), W.rd(v, 4)])
        events.append([0x56E210, M32])
        A.wr(ped + 0x540, 4, f32bits(float(max_health)))
        gch = ['%02x' % max_health]
        marks.append('health_zero' if max_health == 0 else 'health_max' if max_health == 255 else 'health_mid')
        v = call()
        if v == 0:
            marks.append('no_vehicle')
        else:
            v = call(); W.wr(v + 0x4C0, 4, 0x447A0000)
            v = call(); t = W.rd(v + 0x590, 4)
            done = False
            if t != 0:
                v = call()
                if W.rd(v + 0x590, 4) != 9:
                    done = True
                    marks.append('other_type_exit')
            if not done:
                v = call(); t3 = W.rd(v + 0x590, 4)
                if t3 == 0:
                    v = call(); W.wr(v + 0x8E4, 4, 0)
                    v = call(); fix(v)
                    marks.append('auto_path')
                else:
                    v = call(); W.wr(v + 0x7BC, 4, 0)
                    v = call(); fix(v)
                    marks.append('bike_path')
                if a[6] != a[7]:
                    marks.append('type_mix')
    elif k == FEMALE:
        W.snap()
        ret = 1 if a[3] in (0x16, 5) else 0
        marks.append('id_22' if a[3] == 0x16 else 'id_5' if a[3] == 5 else 'id_high_bits' if a[3] in (0x116, 0x105) else 'id_other')
    else:
        raise ValueError(k)
    out = {'events': events, 'changes': {'A': A.changes_compact() if compact else A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': gch, 'heap': heap, 'ret': ret, 'marks': marks}
    return out


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o.get('heap') != e['heap']:
        return 'heap: %s != %s' % (str(o.get('heap'))[:300], str(e['heap'])[:300])
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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) y AL en IsFemalePedType"""
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
