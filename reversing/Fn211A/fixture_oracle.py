"""Oraculo independiente (Python) de Fn211A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 211A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`; ver tambien README.md).
   0  0x6A3060  CAutomobile::SetUpWheelColModel(wheelCol): mi = tabla[modelo]; cm = GetColModel(this) (real); modelos 0x213, 0x214 y 0x23B devuelven 0 sin tocar nada; si no: caja (24 bytes) y esfera (16 bytes desde +0x18) de cm a
            wheelCol; para cada rueda LF, LB, RF, RB (y LM, RM si ambos nodos existen) Attach (real) de la matriz del fotograma (+0x10) y CColSphere::Set (real): radio = tamano * 0,5 (LF y RF delantero mi+0x40; las demas trasero
            mi+0x44), centro = posicion (+0x30 de la matriz), material 0x43, pieza 0xD, 0xF, 0xE, 0x10, 0xF, 0x10, iluminacion 0xFF; numero de esferas (word en colData) 6 o 4; devuelve 1
   1  0x6A32B0  CAutomobile::BurstTyre(componente, fisica): modelo 0x1B0, byte +0x42B con el bit 7 o bandera 0x20000000 en +0x40: 0; rueda = {13: 0, 14: 2, 15: 1, 16: 3} o el byte tal cual; byte de estado de la rueda ([this+0x5A5+rueda], GetWheelStatus real) distinto de 0: 0;
            SetWheelStatus(rueda, 1); IncrementStat(0x81, 1.0); AddAudioEvent(0x5A, 0.0); si (byte +0x36 & 0xF8) == 0x10: SwitchVehicleToRealPhysics(this); con fisica: r1 = aleatorio(-0,03, 0,03) * masa (float); fuerza = r1 * derecha;
            ApplyMoveForce; r2 * masa de giro; ApplyTurnForce(r2 * derecha, adelante); [+0xB8] = 0; devuelve 1
   2  0x6A3BD0  CAutomobile::DoNitroEffect(potencia): escape = vs[+0x48]; con bTouchingWater (+0x40 bit 27): 0x59C890 y GetWaterLevel(x, y, z, &nivel, 1, 0); sumergido = nivel >= z (ordenado); segundo escape (x * -1) solo con el
            doble escape (handling +0xCC bit 13) y bTouchingWater; fx0 (+0x978): si nulo y no sumergido y matriz: CreateFxSystem("nitro"), SetLocalParticles(1), Play; si no nulo: SetConstTime(1, |potencia|), GetPlayStatus ==
            0 y sumergido: Stop; si no, GetPlayStatus == 1 y no sumergido: Play; fx1 igual con el segundo escape (la creacion mira el primer escape)
   3  0x6A6280  xor eax,eax; ret
   4  0x6A6290  ret
   5  0x6A6590  CAutomobile::SetModelIndex(indice): 0x6D6A40(this, indice); 25 dwords a cero desde +0x648; 0x4C5440([this+0x18], this+0x648)
   6  0x6B0690  CAutomobile::DoHeliDustEffect(timeConstMult, fxMaxZMult): sigue solo con subtipo 3 y rotor > 0,1125 (ordenado) o subtipo 4; si no, mata el polvo (Kill, [+0x580] = 0, [+0x984] = 0); sonda vertical, IsSand, GetWaterLevel;
            distancia a la camara < 2500 y altura < maximo (ordenados) o mata; evento de peligro cada 20 fotogramas; crea el polvo si falta; constante de tiempo; SetOffsetPos; EnablePrim 0, 1, 2
   7  0x6B1330  CAutomobile destructor con borrado: 0x6A61E0(this); con el bit 0 del BYTE de banderas 0x6E2D90(this); devuelve this
   8  0x5D8980  IsEnvironmentMappingSupported: caps = 0x7FAD20(); MaxTextureBlendStages (+0x94, con signo) > 1, MaxSimultaneousTextures (+0x98, sin signo) >= 2, TextureOpCaps (+0x90) bits 13 y 24, ShadeCaps (+0x38) bit 9: 1, si no 0
   9  0x5D8BE0  EnvMatDataFromBuffer(buf, out): out[i] = byte bajo de _ftol2(buf[i] * 8) (i = 0..3), out[4] = byte bajo de _ftol2(buf[4] * 255); dword +8 = dword +0x14 del bufer; word +6 = 0; no escribe +5; devuelve la textura
  10  0x5D8C50  EnvMatDataToBuffer(in, buf): buf[i] = (float)(int8)in[i] * 0,125 (i = 0..3), buf[4] = (float)(uint8)in[4] * (1/255); dword +0x14 = dword +8; devuelve la textura
  11-13 0x5D9060 0x5D91B0 0x5D9300  CPool::CPool(tamano, nombre): [this] = new(tamano * elem); [this+4] = new(tamano); [this+0x10] = 1; [this+8] = tamano; [this+0xC] = -1; con tamano > 0 (con signo): flags a 0x80
  14-19 CPool::~CPool: con [this+8] > 0 (con signo): si el byte +0x10 no es cero: delete([this]); delete([this+4]); [this], [this+4], [this+8], [this+0xC] = 0
  20  0x5D9450  CPool destructor con borrado: cuerpo y, con el bit 0 del BYTE de banderas, delete(this); devuelve this
  21-23 CPool::New: ultimo libre + 1 (una vuelta como maximo); flag = ((flag & 0x7F) + 1) & 0x7F; devuelve objetos + indice * elem o nulo
  24  0x5D9180  CPool::Delete(obj): indice = (obj - objetos) / 12 con signo; flags[indice] |= 0x80; si indice < ultimo libre (con signo): ultimo libre = indice

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp). Los punteros se expresan como etiquetas de arena (indice, desplazamiento) o crudos; la pila del llamador es 0x7F000000.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CAutomobile::SetUpWheelColModel", "CAutomobile::BurstTyre", "CAutomobile::DoNitroEffect", "CAutomobile::GetRideAnimData", "CAutomobile::FlagToDestroyWhenNextProcessed",
         "CAutomobile::SetModelIndex", "CAutomobile::DoHeliDustEffect", "CAutomobile::ScalarDeletingDestructor", "CCustomCarEnvMapPipeline::IsEnvironmentMappingSupported",
         "CCustomCarEnvMapPipeline::EnvMatDataFromBuffer", "CCustomCarEnvMapPipeline::EnvMatDataToBuffer", "CCustomCarEnvMapPipeline::PoolConstructorMat",
         "CCustomCarEnvMapPipeline::PoolConstructorAtm", "CCustomCarEnvMapPipeline::PoolConstructorSpec", "CCustomCarEnvMapPipeline::PoolDestructorA", "CCustomCarEnvMapPipeline::PoolDestructorB",
         "CCustomCarEnvMapPipeline::PoolDestructorC", "CCustomCarEnvMapPipeline::PoolDestructorD", "CCustomCarEnvMapPipeline::PoolDestructorE", "CCustomCarEnvMapPipeline::PoolDestructorF",
         "CCustomCarEnvMapPipeline::PoolScalarDeletingDestructor", "CCustomCarEnvMapPipeline::PoolNewMat", "CCustomCarEnvMapPipeline::PoolNewAtm", "CCustomCarEnvMapPipeline::PoolNewSpec",
         "CCustomCarEnvMapPipeline::PoolDeleteMat"]
ENTRIES = (0x6A3060, 0x6A32B0, 0x6A3BD0, 0x6A6280, 0x6A6290, 0x6A6590, 0x6B0690, 0x6B1330, 0x5D8980, 0x5D8BE0, 0x5D8C50, 0x5D9060, 0x5D91B0, 0x5D9300,
           0x5D8F70, 0x5D8FC0, 0x5D9010, 0x5D90C0, 0x5D9210, 0x5D9360, 0x5D9450, 0x5D9110, 0x5D9260, 0x5D93B0, 0x5D9180)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
STACK = 0x7F000000
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SETUP, BURST, NITRO, RIDE, FLAGD, SETMODEL, HELI, SDD, ENVSUPP, FROMBUF, TOBUF, POOLCTOR, POOLDTOR, POOLSDD, POOLNEW, POOLDEL = \
    'setup burst nitro ride flagd setmodel heli sdd envsupp frombuf tobuf poolctor pooldtor poolsdd poolnew pooldel'.split()
KIND = [SETUP, BURST, NITRO, RIDE, FLAGD, SETMODEL, HELI, SDD, ENVSUPP, FROMBUF, TOBUF, POOLCTOR, POOLCTOR, POOLCTOR, POOLDTOR, POOLDTOR, POOLDTOR, POOLDTOR, POOLDTOR, POOLDTOR,
        POOLSDD, POOLNEW, POOLNEW, POOLNEW, POOLDEL]
ELEM = {11: 12, 12: 12, 13: 8, 21: 12, 22: 12, 23: 8}
MODEL_LO, MODEL_COUNT = 0x190, 0xB0


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


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def bits32(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def quiet(b):
    return (b | 0x400000) if is_nan_bits(b) else b


def x24(v):
    """redondea un double al valor de 24 bits de mantisa mas cercano (par en el empate), exponente sin limite: el resultado de una operacion x87 con control de precision de 24 bits"""
    if v == 0 or math.isnan(v) or math.isinf(v):
        return v
    m, e = math.frexp(v)
    m24 = round(m * 16777216.0) / 16777216.0      # round() de Python: redondeo al par en el empate
    return math.ldexp(m24, e)


def st32(v):
    """fstp dword: bits del float mas cercano (par en el empate, denormales, desborde a infinito)"""
    if math.isnan(v):
        return 0xFFC00000
    try:
        return struct.unpack('<I', struct.pack('<f', v))[0]
    except OverflowError:
        return 0xFF800000 if v < 0 else 0x7F800000


def sdiv(a, b):
    if b == 0:
        if a == 0 or math.isnan(a):
            return float('nan')
        return math.copysign(float('inf'), a) * (math.copysign(1.0, b))
    return a / b


def mul_bits(abits, bbits):
    """producto de dos floats por fld/fmul/fstp con 24 bits: bits del resultado almacenado (un solo NaN con carga: el resultado es ese NaN silenciado; inf * 0: el indefinido)"""
    if is_nan_bits(abits) and is_nan_bits(bbits):
        raise ValueError('dos NaN')
    if is_nan_bits(abits):
        return quiet(abits)
    if is_nan_bits(bbits):
        return quiet(bbits)
    a, b = f32(abits), f32(bbits)
    if (math.isinf(a) and b == 0) or (a == 0 and math.isinf(b)):
        return 0xFFC00000
    return st32(x24(a * b))


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba)"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo (0 y la pila del llamador no cambian)"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()

    def ptr(self, kind, arena, off):
        """puntero de un caso: 0 nulo, 1 dentro de la arena, 2 crudo (el desplazamiento tal cual)"""
        return 0 if kind == 0 else (self.raw(arena, off) if kind == 1 else off & M32)


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == SETUP:
        _k += ['special_model', 'six_spheres', 'four_spheres', 'lm_only', 'rm_only', 'size_nan', 'size_inf', 'size_denormal', 'size_negative', 'size_zero']
    elif _kind == BURST:
        _k += ['rhino', 'tyres_dont_burst', 'scorched', 'comp_lf', 'comp_rf', 'comp_rl', 'comp_rr', 'comp_other', 'already_burst', 'burst', 'simple_status', 'not_simple', 'physical', 'not_physical',
               'nan_in', 'inf_in', 'denormal_in']
    elif _kind == NITRO:
        _k += ['touching', 'not_touching', 'double', 'single', 'fx0_null', 'fx0_set', 'fx1_null', 'fx1_set', 'submerged1', 'submerged2', 'create0', 'create1', 'stop0', 'play0', 'stop1', 'play1',
               'level_nan', 'level_equal', 'rw_null', 'rw_set', 'power_nan', 'power_negative']
    elif _kind == SETMODEL:
        _k += ['negative_index', 'marks_differ', 'marks_same']
    elif _kind == HELI:
        _k += ['heli_ok', 'heli_slow', 'plane', 'other_type', 'rotor_nan', 'rotor_edge', 'kill_with_dust', 'kill_without_dust', 'matrix', 'no_matrix', 'cam_matrix', 'cam_global', 'rc_model', 'hydra', 'far_camera',
               'too_high', 'danger', 'danger_frame', 'under_water', 'sand', 'ground', 'created', 'create_failed', 'timeconst_up', 'timeconst_down', 'timeconst_same', 'timeconst_nan', 'done', 'nan_position']
    elif _kind in (SDD, POOLSDD):
        _k += ['delete', 'no_delete']
        if _kind == POOLSDD:
            _k += ['size_positive', 'size_not_positive', 'owner', 'not_owner']
    elif _kind == ENVSUPP:
        _k += ['supported', 'unsupported', 'stages_low', 'stages_negative', 'simul_low', 'no_blend', 'no_madd', 'no_shade']
    elif _kind == FROMBUF:
        _k += ['nan_in', 'inf_in', 'big_in', 'negative_in', 'zero_in', 'frac_in']
    elif _kind == TOBUF:
        _k += ['negative_byte', 'high_byte', 'zero_byte']
    elif _kind == POOLCTOR:
        _k += ['size_positive', 'size_zero', 'size_negative', 'new_null', 'new_set', 'size_big']
    elif _kind == POOLDTOR:
        _k += ['size_positive', 'size_not_positive', 'owner', 'not_owner']
    elif _kind == POOLNEW:
        _k += ['found', 'wrapped', 'null', 'negative_cursor', 'first_slot', 'last_slot']
    elif _kind == POOLDEL:
        _k += ['lowers_cursor', 'keeps_cursor', 'negative_index', 'remainder', 'was_free', 'was_used']
    KEYS[_fn] = _k


def trunc_low8(bits, scale):
    """byte bajo de _ftol2(x * escala): producto con 24 bits, entero truncado de 64 bits; NaN, infinito o fuera de rango: 0 (el entero indefinido)"""
    v = f32(bits)
    if math.isnan(v) or math.isinf(v):
        return 0
    p = x24(v * scale)
    if math.isinf(p) or math.isnan(p):
        return 0
    t = int(p)
    if not (-(1 << 63) <= t < (1 << 63)):
        return 0
    return t & 0xFF


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
    tg = W.tagv
    A.wr(o, 4, a[3])
    if k == SETUP:
        A.wr(o + 0x22, 2, a[4]); A.wr(o + 0x36, 1, a[5])
        slot = [0x65C, 0x664, 0x650, 0x658, 0x660, 0x654]
        for kk in range(6):
            frame = a[7] + kk * 0x60
            present = kk < 4 or ((a[6] & 1) if kk == 4 else (a[6] & 2))
            A.wr(o + slot[kk], 4, W.raw(1, frame) if present else 0)
            B.wr(frame + 0x40, 4, a[8 + 3 * kk]); B.wr(frame + 0x44, 4, a[9 + 3 * kk]); B.wr(frame + 0x48, 4, a[10 + 3 * kk])
        D.wr(a[28] + 0x14, 4, W.raw(3, a[29])); D.wr(a[28] + 0x40, 4, a[26]); D.wr(a[28] + 0x44, 4, a[27])
        Cc.wr(a[30] + 0x2C, 4, W.raw(2, a[31])); Cc.wr(a[31] + 8, 4, W.raw(2, a[32]))
        wheel = a[30]
        W.snap()
        special = (a[4] & 0xFFFF) in (0x213, 0x214, 0x23B)
        if special:
            ret = 0
            marks.append('special_model')
        else:
            cm = a[29]
            for off in range(0, 0x28, 4):
                Cc.wr(wheel + off, 4, D.rd(cm + off, 4))
            front, rear = a[26], a[27]
            piece = [0xD, 0xF, 0xE, 0x10, 0xF, 0x10]
            size = [front, rear, front, rear, rear, rear]
            count = 6 if (a[6] & 3) == 3 else 4
            sph = a[32]
            for kk in range(count):
                s = sph + kk * 0x14
                frame = a[7] + kk * 0x60
                for j in range(3):
                    Cc.wr(s + 4 * j, 4, a[8 + 3 * kk + j])
                sb = size[kk]
                Cc.wr(s + 0xC, 4, quiet(sb) if is_nan_bits(sb) else bits32(f32(sb) * 0.5))
                Cc.wr(s + 0x10, 1, 0x43); Cc.wr(s + 0x11, 1, piece[kk]); Cc.wr(s + 0x12, 1, 0xFF)
            Cc.wr(a[31], 2, count)
            ret = 1
            marks.append('six_spheres' if count == 6 else 'four_spheres')
            if (a[6] & 3) == 1:
                marks.append('lm_only')
            if (a[6] & 3) == 2:
                marks.append('rm_only')
            for sb in (front, rear):
                v = f32(sb)
                if is_nan_bits(sb):
                    marks.append('size_nan')
                if math.isinf(v):
                    marks.append('size_inf')
                if 0 < (sb & 0x7FFFFFFF) < 0x800000:
                    marks.append('size_denormal')
                if (sb & 0x80000000) and (sb & 0x7FFFFFFF):
                    marks.append('size_negative')
                if (sb & 0x7FFFFFFF) == 0:
                    marks.append('size_zero')
    elif k == BURST:
        A.wr(o + 0x22, 2, a[4]); A.wr(o + 0x42B, 1, a[5]); A.wr(o + 0x40, 4, a[6]); A.wr(o + 0x36, 1, a[9])
        A.wr(o + 0x8C, 4, a[13]); A.wr(o + 0x90, 4, a[14]); A.wr(o + 0xB8, 1, 0xA5)
        A.wr(o + 0x14, 4, W.raw(1, a[15]))
        for i in range(3):
            B.wr(a[15] + 4 * i, 4, a[16 + i]); B.wr(a[15] + 0x10 + 4 * i, 4, a[19 + i])
        comp = a[7] & 0xFF
        wheel = {13: 0, 14: 2, 15: 1, 16: 3}.get(comp, comp)
        A.wr(o + 0x5A5 + wheel, 1, a[10])
        W.snap()
        marks.append({13: 'comp_lf', 14: 'comp_rf', 15: 'comp_rl', 16: 'comp_rr'}.get(comp, 'comp_other'))
        for v in a[11:15] + a[16:22]:
            if is_nan_bits(v):
                marks.append('nan_in')
            if math.isinf(f32(v)):
                marks.append('inf_in')
            if 0 < (v & 0x7FFFFFFF) < 0x800000:
                marks.append('denormal_in')
        if (a[4] & 0xFFFF) == 0x1B0:
            ret = 0
            marks.append('rhino')
        elif a[5] & 0x80:
            ret = 0
            marks.append('tyres_dont_burst')
        elif a[6] & 0x20000000:
            ret = 0
            marks.append('scorched')
        else:
            dm = this + 0x5A0
            if (a[10] & 0xFF) != 0:
                ret = 0
                marks.append('already_burst')
            else:
                marks.append('burst')
                events.append([0x6C21A0, tg(dm), wheel, 1])
                events.append([0x55C180, 0x81, 0x3F800000])
                events.append([0x4F6420, tg(this + 0x138), 0x5A, 0])
                if (a[9] & 0xF8) == 0x10:
                    events.append([0x423FC0, tg(this)])
                    marks.append('simple_status')
                else:
                    marks.append('not_simple')
                if (a[8] & 0xFF) != 0:
                    marks.append('physical')
                    lo, hi = 0xBCF5C28F, 0x3CF5C28F
                    r1 = quiet(a[11])
                    events.append([0x41BD90, lo, hi])
                    s1 = mul_bits(r1, a[13])
                    mv = [mul_bits(s1, a[16 + i]) for i in range(3)]
                    events.append([0x5429F0, tg(this)] + mv)
                    r2 = quiet(a[12])
                    events.append([0x41BD90, lo, hi])
                    s2 = mul_bits(r2, a[14])
                    tv = [mul_bits(s2, a[16 + i]) for i in range(3)]
                    events.append([0x542A50, tg(this)] + tv + [a[19], a[20], a[21]])
                    A.wr(o + 0xB8, 1, 0)
                else:
                    marks.append('not_physical')
                ret = 1
    elif k == NITRO:
        A.wr(o + 0x22, 2, a[4]); A.wr(o + 0x40, 4, a[5])
        A.wr(o + 0x384, 4, W.raw(2, a[6])); Cc.wr(a[6] + 0xCC, 4, a[7])
        A.wr(o + 0x18, 4, W.raw(1, a[9]) if a[8] == 1 else 0)
        if a[8] == 1:
            B.wr(a[9] + 4, 4, a[10])
        A.wr(o + 0x978, 4, W.ptr(a[11], 3, a[12])); A.wr(o + 0x97C, 4, W.ptr(a[13], 3, a[14]))
        matrix = W.raw(1, (a[9] + 0x40) & 0xFFC) if a[15] == 1 else (a[10] | 1)
        A.wr(o + 0x14, 4, matrix)
        D.wr(a[16] + 0x5C, 4, W.raw(3, a[17]))
        D.wr(a[17] + 0x48, 4, a[18]); D.wr(a[17] + 0x4C, 4, a[19]); D.wr(a[17] + 0x50, 4, a[20])
        creates = [W.ptr(a[30], 3, a[31]), W.ptr(a[30], 3, a[31] + 0x40)]
        W.snap()
        touching = bool(a[5] & 0x8000000)
        double = bool(a[7] & 0x2000)
        marks.append('touching' if touching else 'not_touching')
        marks.append('double' if double else 'single')
        x, y, z = a[18], a[19], a[20]
        pos0 = [x, y, z]
        water_i = [0]
        status_i = [0]
        create_i = [0]

        def water(px, py, pz):
            i = water_i[0]
            water_i[0] += 1
            events.append([0x6EB690, px, py, pz, STACK, 1, 0])
            if a[22 + i] & 0xFF:
                lv = f32(a[24 + i])
                if math.isnan(lv):
                    marks.append('level_nan')
                if a[24 + i] == pz:
                    marks.append('level_equal')
                return lv >= f32(pz)
            return False

        def status():
            i = status_i[0]
            status_i[0] += 1
            return a[26 + (i & 3)]
        sub1 = sub2 = False
        if touching:
            events.append([0x59C890, tg(matrix), x, y, z])
            sub1 = water(x, y, z)
        pos1 = [x if is_nan_bits(x) else x ^ 0x80000000, y, z]
        if double and touching:
            events.append([0x59C890, tg(matrix), pos1[0], pos1[1], pos1[2]])
            sub2 = water(pos1[0], pos1[1], pos1[2])
        if sub1:
            marks.append('submerged1')
        if sub2:
            marks.append('submerged2')
        rw = a[8] == 1
        marks.append('rw_set' if rw else 'rw_null')
        edi = (a[10] + 0x10) & M32 if rw else 0
        fabsp = quiet(a[21]) & 0x7FFFFFFF
        if is_nan_bits(a[21]):
            marks.append('power_nan')
        if a[21] & 0x80000000:
            marks.append('power_negative')

        def create(pos):
            i = create_i[0]
            create_i[0] += 1
            events.append([0x4A9BE0, 0xA9AE80, fnv(b'nitro'), pos[0], pos[1], pos[2], tg(edi), 1])
            return creates[i if i < 2 else 1]
        fx0 = W.ptr(a[11], 3, a[12])
        marks.append('fx0_null' if fx0 == 0 else 'fx0_set')
        if fx0 == 0:
            if not sub1 and edi != 0:
                new = create(pos0)
                marks.append('create0')
                A.wr(o + 0x978, 4, new)
                if new != 0:
                    events.append([0x4AA910, tg(new), 1])
                    events.append([0x4AA2F0, tg(new)])
        else:
            events.append([0x4AA6C0, tg(fx0), 1, fabsp])
            events.append([0x4AA900, tg(fx0)])
            s1 = status()
            if s1 == 0 and sub1:
                events.append([0x4AA390, tg(fx0)])
                marks.append('stop0')
            else:
                events.append([0x4AA900, tg(fx0)])
                s2 = status()
                if s2 == 1 and not sub1:
                    events.append([0x4AA2F0, tg(fx0)])
                    marks.append('play0')
        if double:
            fx1 = W.ptr(a[13], 3, a[14])
            marks.append('fx1_null' if fx1 == 0 else 'fx1_set')
            if fx1 == 0:
                if not sub1 and edi != 0:
                    new = create(pos1)
                    marks.append('create1')
                    A.wr(o + 0x97C, 4, new)
                    if new != 0:
                        events.append([0x4AA910, tg(new), 1])
                        events.append([0x4AA2F0, tg(new)])
            else:
                events.append([0x4AA6C0, tg(fx1), 1, fabsp])
                events.append([0x4AA900, tg(fx1)])
                s1 = status()
                if s1 == 0 and sub2:
                    events.append([0x4AA390, tg(fx1)])
                    marks.append('stop1')
                else:
                    events.append([0x4AA900, tg(fx1)])
                    s2 = status()
                    if s2 == 1 and not sub2:
                        events.append([0x4AA2F0, tg(fx1)])
                        marks.append('play1')
    elif k == RIDE:
        W.snap()
        ret = 0
    elif k == FLAGD:
        W.snap()
    elif k == SETMODEL:
        A.wr(o + 0x18, 4, a[5])
        for i in range(25):
            A.wr(o + 0x648 + 4 * i, 4, 0xA5A5A500 + i)
        W.snap()
        events.append([0x6D6A40, tg(this), a[4]])
        events.append([0x4C5440, tg(a[5]), tg(this + 0x648)])
        for i in range(25):
            A.wr(o + 0x648 + 4 * i, 4, 0)
        A.wr(o + 0x648 + 4 * a[8], 4, a[9])
        marks.append('negative_index' if a[4] & 0x80000000 else 'nonneg_index')
        marks.append('marks_same' if a[6] == a[8] else 'marks_differ')
    elif k == HELI:
        A.wr(o + 0x594, 4, a[4]); A.wr(o + 0x84C, 4, a[5]); A.wr(o + 0x22, 2, a[11])
        dust0 = W.ptr(a[12], 3, a[33])
        A.wr(o + 0x580, 4, dust0); A.wr(o + 0x984, 4, a[13])
        if a[6] == 1:
            A.wr(o + 0x14, 4, W.raw(1, a[7]))
            B.wr(a[7] + 0x30, 4, a[8]); B.wr(a[7] + 0x34, 4, a[9]); B.wr(a[7] + 0x38, 4, a[10])
        else:
            A.wr(o + 0x14, 4, 0)
            A.wr(o + 4, 4, a[8]); A.wr(o + 8, 4, a[9]); A.wr(o + 0xC, 4, a[10])
        if a[14] == 1:
            Cc.wr(a[15] + 0x30, 4, a[19]); Cc.wr(a[15] + 0x34, 4, a[20]); Cc.wr(a[15] + 0x38, 4, a[21])
        creates = W.ptr(a[31], 3, a[33] + 0x40)
        group = W.ptr(a[32], 3, a[34])
        W.snap()
        marks.append('matrix' if a[6] == 1 else 'no_matrix')
        marks.append('cam_matrix' if a[14] == 1 else 'cam_global')
        rotor = f32(a[5])
        if is_nan_bits(a[5]):
            marks.append('rotor_nan')
        if a[5] in (0x3DE66666, 0x3DE66667, 0x3DE66668):
            marks.append('rotor_edge')
        heli = a[4] == 3 and rotor > f32(0x3DE66667)
        plane = a[4] == 4
        marks.append('plane' if plane else ('heli_ok' if heli else ('heli_slow' if a[4] == 3 else 'other_type')))
        if (a[11] & 0xFFFF) in (0x1F5, 0x1D1):
            marks.append('rc_model')
        if (a[11] & 0xFFFF) == 0x208:
            marks.append('hydra')
        if any(is_nan_bits(v) for v in a[8:11]):
            marks.append('nan_position')

        def kill():
            if dust0 != 0:
                events.append([0x4AA3F0, tg(dust0)])
                A.wr(o + 0x580, 4, 0)
                A.wr(o + 0x984, 4, 0)
                marks.append('kill_with_dust')
            else:
                marks.append('kill_without_dust')
        if not (heli or plane):
            kill()
        else:
            px, py, pz = a[8], a[9], a[10]
            events.append([0x5674E0, px, py, pz, 0xC47A0000, STACK, STACK, 1, 0, 0, 0, 0, 0, 0])
            gz = f32(a[24]); gzb = a[24]
            surf = a[25] & 0xFF
            events.append([0x55E6F0, 0xB79538, surf])
            sand = (a[26] & 0xFF) != 0
            events.append([0x6EB690, px, py, pz, STACK, 0, 0])
            under = False
            if a[27] & 0xFF:
                lvl = f32(a[28])
                if lvl > gz:
                    gz, gzb = lvl, a[28]
                    under = True
            base = 3.0 if (a[11] & 0xFFFF) in (0x1F5, 0x1D1) else 30.0
            m2 = a[30]
            if is_nan_bits(m2):
                maxz = f32(quiet(m2))
            else:
                maxz = f32(st32(x24(base * f32(m2)))) if not (math.isinf(f32(m2)) and base == 0) else float('nan')
            camb = (a[19], a[20], a[21]) if a[14] == 1 else (a[16], a[17], a[18])
            z, y, x = f32(pz), f32(py), f32(px)
            dz = x24(z - f32(camb[2])); dy = x24(y - f32(camb[1])); dx = x24(x - f32(camb[0]))
            s = x24(x24(dx * dx) + x24(dy * dy))
            dist = x24(s + x24(dz * dz))
            if not (dist < 2500.0):
                marks.append('far_camera')
                kill()
            else:
                d = x24(z - gz)
                if not (d < maxz):
                    marks.append('too_high')
                    kill()
                else:
                    if a[22] % 20 == 0:
                        marks.append('danger_frame')
                        rad = 30.0 if (a[11] & 0xFFFF) == 0x208 else 20.0
                        if d < rad:
                            marks.append('danger')
                            events.append([0x571B70, tg(this), STACK])
                            events.append([0x4ABA50])
                            events.append([0x4AB420, tg(group), STACK, 0, 0, 0, 1, 0xC8, 0xFFFF, tg(this), bits32(rad)])
                            events.append([0x571A00, tg(this), STACK])
                    dust = dust0
                    if dust == 0:
                        events.append([0x4A9BE0, 0xA9AE80, fnv(b'heli_dust'), 0, 0, 0, 0, 1])
                        dust = creates
                        A.wr(o + 0x580, 4, dust)
                        if dust != 0:
                            events.append([0x4AA910, tg(dust), 1])
                            events.append([0x4AA2F0, tg(dust)])
                            marks.append('created')
                        else:
                            marks.append('create_failed')
                    if dust != 0:
                        mult = f32(a[29])
                        t1 = x24(maxz - d)
                        t2 = x24(sdiv(t1, maxz))
                        t3 = x24(t2 * mult)
                        sqv = x24(t3 * t3)
                        sqb = st32(sqv)
                        sq = f32(sqb)
                        cur = f32(a[13])
                        curb = a[13]
                        if sq > cur:
                            x = x24(cur + f32(0x3D23D70A))
                            curb = st32(x) if x < sq else sqb
                            marks.append('timeconst_up')
                        elif sq < cur:
                            x = x24(cur - f32(0x3D23D70A))
                            curb = st32(x) if x > sq else sqb
                            marks.append('timeconst_down')
                        else:
                            marks.append('timeconst_nan' if (math.isnan(sq) or math.isnan(cur)) else 'timeconst_same')
                        A.wr(o + 0x984, 4, curb)
                        events.append([0x4AA6C0, tg(dust), 1, curb])
                        events.append([0x4AA660, tg(dust), px, py, gzb])
                        if under:
                            marks.append('under_water')
                            prim = [0, 0, 1]
                        elif sand:
                            marks.append('sand')
                            prim = [0, 1, 0]
                        else:
                            marks.append('ground')
                            prim = [1, 0, 0]
                        if under:
                            events.extend([[0x4AA610, tg(dust), 0, 0], [0x4AA610, tg(dust), 1, 0], [0x4AA610, tg(dust), 2, 1]])
                        elif sand:
                            events.extend([[0x4AA610, tg(dust), 0, 0], [0x4AA610, tg(dust), 1, 1], [0x4AA610, tg(dust), 2, 0]])
                        else:
                            events.extend([[0x4AA610, tg(dust), 0, 1], [0x4AA610, tg(dust), 1, 0], [0x4AA610, tg(dust), 2, 0]])
                        marks.append('done')
    elif k == SDD:
        W.snap()
        events.append([0x6A61E0, tg(this), a[3]])
        A.wr(o, 4, a[5])
        if a[4] & 1:
            events.append([0x6E2D90, tg(this)])
            marks.append('delete')
        else:
            marks.append('no_delete')
        ret = tg(this)
    elif k == ENVSUPP:
        D.wr(a[8] + 0x38, 4, a[4]); D.wr(a[8] + 0x90, 4, a[5]); D.wr(a[8] + 0x94, 4, a[6]); D.wr(a[8] + 0x98, 4, a[7])
        W.snap()
        events.append([0x7FAD20])
        stages = a[6] - (1 << 32) if a[6] >= 1 << 31 else a[6]
        c1 = stages > 1
        c2 = a[7] >= 2
        c3 = bool((a[5] >> 13) & 1)
        c4 = bool((a[5] >> 24) & 1)
        c5 = bool((a[4] >> 9) & 1)
        ret = 1 if (c1 and c2 and c3 and c4 and c5) else 0
        marks.append('supported' if ret else 'unsupported')
        if not c1:
            marks.append('stages_negative' if stages < 0 else 'stages_low')
        if not c2:
            marks.append('simul_low')
        if not c3:
            marks.append('no_blend')
        if not c4:
            marks.append('no_madd')
        if not c5:
            marks.append('no_shade')
    elif k == FROMBUF:
        for i in range(5):
            B.wr(a[10] + 4 * i, 4, a[4 + i])
        B.wr(a[10] + 0x14, 4, a[9])
        W.snap()
        for i in range(4):
            A.wr(o + i, 1, trunc_low8(a[4 + i], 8.0))
        A.wr(o + 4, 1, trunc_low8(a[8], 255.0))
        A.wr(o + 8, 4, a[9])
        A.wr(o + 6, 2, 0)
        ret = tg(a[9])
        for i in range(5):
            v = f32(a[4 + i])
            if is_nan_bits(a[4 + i]):
                marks.append('nan_in')
            if math.isinf(v):
                marks.append('inf_in')
            if not math.isnan(v) and not math.isinf(v) and abs(v) * (255.0 if i == 4 else 8.0) >= 128:
                marks.append('big_in')
            if v < 0:
                marks.append('negative_in')
            if v == 0:
                marks.append('zero_in')
            if not math.isnan(v) and not math.isinf(v) and v != int(v):
                marks.append('frac_in')
    elif k == TOBUF:
        for i in range(5):
            B.wr(a[10] + i, 1, a[4 + i])
        B.wr(a[10] + 8, 4, a[9])
        W.snap()
        for i in range(4):
            b = a[4 + i] & 0xFF
            sb = b - 256 if b >= 128 else b
            A.wr(o + 4 * i, 4, bits32(sb * 0.125))
        b4 = a[8] & 0xFF
        A.wr(o + 0x10, 4, st32(x24(b4 * f32(0x3B808081))))
        A.wr(o + 0x14, 4, a[9])
        ret = tg(a[9])
        if any((a[4 + i] & 0xFF) >= 0x80 for i in range(4)):
            marks.append('negative_byte')
        if any((a[4 + i] & 0xFF) >= 0xF0 for i in range(5)):
            marks.append('high_byte')
        if any((a[4 + i] & 0xFF) == 0 for i in range(5)):
            marks.append('zero_byte')
    elif k == POOLCTOR:
        size = a[4]
        elem = ELEM[fn]
        news = [W.ptr(a[5], 2, a[6]), W.raw(3, a[7])]
        W.snap()
        events.append([0x82119A, (size * elem) & M32])
        events.append([0x82119A, size])
        A.wr(o + 0, 4, news[0]); A.wr(o + 4, 4, news[1]); A.wr(o + 0x10, 1, 1); A.wr(o + 8, 4, size); A.wr(o + 0xC, 4, 0xFFFFFFFF)
        ssize = size - (1 << 32) if size >= 1 << 31 else size
        if ssize > 0:
            for i in range(ssize):
                D.wr(a[7] + i, 1, 0x80)
            marks.append('size_positive')
            if size >= 0x800:
                marks.append('size_big')
        elif ssize == 0:
            marks.append('size_zero')
        else:
            marks.append('size_negative')
        marks.append('new_null' if a[5] == 0 else 'new_set')
        ret = tg(this)
    elif k in (POOLDTOR, POOLSDD):
        A.wr(o + 0, 4, W.ptr(a[4], 2, a[5])); A.wr(o + 4, 4, W.ptr(a[6], 3, a[7]))
        A.wr(o + 8, 4, a[8]); A.wr(o + 0xC, 4, a[9]); A.wr(o + 0x10, 1, a[10])
        W.snap()
        size = a[8] - (1 << 32) if a[8] >= 1 << 31 else a[8]
        objects, flags = A.rd(o, 4), A.rd(o + 4, 4)
        if size > 0:
            marks.append('size_positive')
            if a[10] & 0xFF:
                events.append([0x8214BD, tg(objects)])
                events.append([0x8214BD, tg(flags)])
                marks.append('owner')
            else:
                marks.append('not_owner')
            A.wr(o, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        else:
            marks.append('size_not_positive')
            if a[10] & 0xFF:
                marks.append('owner')
            else:
                marks.append('not_owner')
        if k == POOLSDD:
            if a[11] & 1:
                events.append([0x8214BD, tg(this)])
                marks.append('delete')
            else:
                marks.append('no_delete')
            ret = tg(this)
        else:
            marks = [m for m in marks if m not in ('delete', 'no_delete')]
    elif k == POOLNEW:
        elem = ELEM[fn]
        size = a[4]
        A.wr(o + 0, 4, W.raw(2, a[6])); A.wr(o + 4, 4, W.raw(3, a[7]))
        A.wr(o + 8, 4, size); A.wr(o + 0xC, 4, a[5])
        for i in range(size):
            low = rnd(a[37], 0x500 + i) & 0x7F
            if a[9] == 0:
                free = rnd(a[37], 0x900 + i) % 100 < a[8]
            elif a[9] == 1:
                free = True
            elif a[9] == 2:
                free = False
            else:
                free = i == a[10]
            D.wr(a[7] + i, 1, low | (0x80 if free else 0))
        W.snap()
        last = a[5] - (1 << 32) if a[5] >= 1 << 31 else a[5]
        wrapped = False
        found = None
        start = last
        while True:
            last += 1
            if last == size:
                last = 0
                if wrapped:
                    break
                wrapped = True
            if D.rd(a[7] + last, 1) & 0x80:
                found = last
                break
        A.wr(o + 0xC, 4, last & M32)
        if found is None:
            ret = 0
            marks.append('null')
        else:
            old = D.rd(a[7] + found, 1)
            D.wr(a[7] + found, 1, (((old & 0x7F) + 1) & 0x7F))
            ret = tg((W.raw(2, a[6]) + found * elem) & M32)
            marks.append('found')
            if wrapped:
                marks.append('wrapped')
            if found == 0:
                marks.append('first_slot')
            if found == size - 1:
                marks.append('last_slot')
        if start < 0:
            marks.append('negative_cursor')
    elif k == POOLDEL:
        idx = a[6] - (1 << 32) if a[6] >= 1 << 31 else a[6]
        objects = W.raw(2, a[8])
        A.wr(o + 0, 4, objects); A.wr(o + 4, 4, W.raw(3, a[9]))
        A.wr(o + 8, 4, a[4]); A.wr(o + 0xC, 4, a[5])
        old = D.rd(a[9] + idx, 1)
        D.wr(a[9] + idx, 1, (old & 0x7F) | (0x80 if a[10] else 0))
        arg = (objects + idx * 12 + a[7]) & M32
        W.snap()
        diff = (arg - objects) & M32
        sdiff = diff - (1 << 32) if diff >= 1 << 31 else diff
        index = int(sdiff / 12)
        index = (abs(sdiff) // 12) * (1 if sdiff >= 0 else -1)
        D.wr(a[9] + index, 1, D.rd(a[9] + index, 1) | 0x80)
        last = a[5] - (1 << 32) if a[5] >= 1 << 31 else a[5]
        if index < last:
            A.wr(o + 0xC, 4, index & M32)
            marks.append('lowers_cursor')
        else:
            marks.append('keeps_cursor')
        if index < 0:
            marks.append('negative_index')
        if a[7] % 12:
            marks.append('remainder')
        marks.append('was_free' if a[10] else 'was_used')
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
