"""Oraculo independiente del banco FxInfoLoad28: FxInfoManager_c::Load (0x5C0B70).

Se modela desde el DESENSAMBLADO del EXE (reversing/_handoff/dis.sh 0x5C0B70 0x5C16E8), no desde el C++ de la DLL, a partir de la fila `input` que registra el banco. Re-analiza el
TEXTO del flujo con su propio modelo de fgets y de sscanf, asi que no depende del generador de Fixtures.hpp.

FxInfoManager_c::Load(FILESTREAM file, int32 version): thiscall (ECX = this), 2 argumentos en pila, `ret 8` (0x5C16E6). Marco de 0x188 bytes: linea[256] en esp+0x98 y campo[128] en esp+0x18
(relativos a la pila tras los push de esi/edi), indice del bucle en esp+0x10 y la bandera de tiempo en esp+0x14.
  5C0B8F  ReadLine(file, line, 256) (4AB250 -> jmp 5389B0 = fgets) ; descartada (cabecera)
  5C0BA2  ReadLine(file, line, 256) ; sscanf(line, "%s %d" (0x86A6AC), field, &this->m_nNumInfos)     [sscanf = 0x8220AD, CRT estatico del EXE]
  5C0BC5  ecx = n << 2 ; [this+8] = 0xFF ; [this+9] = 0xFF ; this->m_pInfos = GetMem(0xA9AF2C)(ecx, 4) (4A9CA0, thiscall, ret 8)
  5C0BE0  si n <= 0 (signed) salta al final ; si no, bucle i = 0..n-1 (n se RELEE de [ebp] en cada vuelta: `cmp eax,ecx ; jl`):
  5C0C00    flag = 1 ; ReadLine(file, line, 256) ; sscanf(line, "%s" (0x85C714), field)
  5C0C35    compara `field` con la tabla de 32 nombres (repe cmps, longitud con NUL) -> push tipo ; si ninguno coincide (tras el ultimo, 0x8001) salta a 5C1676 sin reservar ni cargar
            tipos 0x2xxx, 0x4xxx y 0x8001: `fild version ; fcomp 0.7 (double) ; jb` : si version >= 0.7 (version >= 1) ReadLine + sscanf(line, "%s %d", field, &flag)
  5C163B    info = AddFxInfo(tipo) (4A7B00, thiscall, ret 4) -> m_pInfos[i] ; info->Load(file, version) (vtable +4, thiscall, ret 8) ; ReadLine(file, line, 256) (linea de cierre, SIN sscanf)
  5C1676    byte [m_pInfos[i]+6] = (u8)flag ; t = word [m_pInfos[i]+4] ;
            si t & 0x1000: nada ; si no, si t & 0x2000: si [this+8] == 0xFF -> [this+8] = (u8)i ; si no, si t & 0xC000: si [this+9] == 0xFF -> [this+9] = (u8)i
  5C16C6  si [this+9] == 0xFF -> [this+9] = (u8)n ; si [this+8] == 0xFF -> [this+8] = [this+9] ; ret 8
Para un nombre desconocido el EXE usa el puntero que YA hubiera en m_pInfos[i] (memoria de la arena sin inicializar): el banco pre-rellena TODAS las ranuras con un FxInfo "basura"
(`junk`) cuyo tipo (`junk_type`) fija el reparto de offsets y cuyo byte 6 recibe la bandera 1.

Modelo de fgets(buf, 256, f): hasta 255 caracteres o hasta '\\n' inclusive; EOF -> el fixture es invalido (buffer sin tocar = basura de pila).
Modelo de sscanf: "%s" = secuencia sin blancos (blancos: ' \\t\\n\\v\\f\\r'; si no hay ninguna no se escribe nada y el resultado es EOF); "%s %d": tras la palabra, %d salta blancos, acepta
[+-]digitos y lee el prefijo decimal (0x10 -> 0, 3.9 -> 3, 1e3 -> 1); sin digitos no escribe. Valores de |v| >= 2^31 (o mas de 18 digitos) se marcan DESCONOCIDOS (el EXE y la DLL usan el
MISMO sscanf, asi que la igualdad EXE == DLL sigue siendo estricta; el oraculo solo no predice ese byte).
"""
import json
import sys

M32 = 0xFFFFFFFF
NAME = 'FxInfoManager_c::Load'
NAMES = [NAME]
CASES = {NAME: 4608}
MGR = 0x14
KGUARD = 0x40
MAX_SLOTS = 320
POOL = 330
STREAM_MAX = 0x10000 - 0x40
WS = ' \t\n\v\f\r'
TYPES = {
    'FX_INFO_EMRATE_DATA:': 0x1001, 'FX_INFO_EMSIZE_DATA:': 0x1004, 'FX_INFO_EMSPEED_DATA:': 0x1008, 'FX_INFO_EMDIR_DATA:': 0x1010, 'FX_INFO_EMANGLE_DATA:': 0x1020,
    'FX_INFO_EMLIFE_DATA:': 0x1040, 'FX_INFO_EMPOS_DATA:': 0x1080, 'FX_INFO_EMWEATHER_DATA:': 0x1100, 'FX_INFO_EMROTATION_DATA:': 0x1200,
    'FX_INFO_NOISE_DATA:': 0x2001, 'FX_INFO_FORCE_DATA:': 0x2002, 'FX_INFO_FRICTION_DATA:': 0x2004, 'FX_INFO_ATTRACTPT_DATA:': 0x2008, 'FX_INFO_ATTRACTLINE_DATA:': 0x2010,
    'FX_INFO_GROUNDCOLLIDE_DATA:': 0x2020, 'FX_INFO_WIND_DATA:': 0x2040, 'FX_INFO_JITTER_DATA:': 0x2080, 'FX_INFO_ROTSPEED_DATA:': 0x2100, 'FX_INFO_FLOAT_DATA:': 0x2200,
    'FX_INFO_UNDERWATER_DATA:': 0x2400,
    'FX_INFO_COLOUR_DATA:': 0x4001, 'FX_INFO_SIZE_DATA:': 0x4002, 'FX_INFO_SPRITERECT_DATA:': 0x4004, 'FX_INFO_HEATHAZE_DATA:': 0x4008, 'FX_INFO_TRAIL_DATA:': 0x4010,
    'FX_INFO_FLAT_DATA:': 0x4020, 'FX_INFO_DIR_DATA:': 0x4040, 'FX_INFO_ANIMTEX_DATA:': 0x4080, 'FX_INFO_COLOURRANGE_DATA:': 0x4100, 'FX_INFO_SELFLIT_DATA:': 0x4200,
    'FX_INFO_COLOURBRIGHT_DATA:': 0x4400,
    'FX_INFO_SMOKE_DATA:': 0x8001,
}
assert len(TYPES) == 32


class Fail(RuntimeError):
    """Fixture que depende de comportamiento indefinido del EXE (o inconsistente con la fila `input`)."""


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def le32(v):
    return (v & M32).to_bytes(4, 'little')


def scan_s(line):
    """sscanf(line, "%s", field): devuelve (palabra, resto) o None si no hay palabra."""
    i = 0
    while i < len(line) and line[i] in WS:
        i += 1
    if i == len(line):
        return None
    j = i
    while j < len(line) and line[j] not in WS:
        j += 1
    return line[i:j], line[j:]


def scan_s_d(line):
    """sscanf(line, "%s %d", field, &v): devuelve (asignado, sens, valor). `field` siempre se escribe si hay palabra."""
    r = scan_s(line)
    if r is None:
        return (False, False, 0)
    rest = r[1]
    i = 0
    while i < len(rest) and rest[i] in WS:
        i += 1
    sign = 1
    if i < len(rest) and rest[i] in '+-':
        sign = -1 if rest[i] == '-' else 1
        i += 1
    j = i
    while j < len(rest) and rest[j] in '0123456789':
        j += 1
    if j == i:
        return (False, False, 0)
    digits = rest[i:j]
    v = sign * int(digits)
    sens = len(digits.lstrip('0')) > 18 or abs(v) >= 1 << 31
    return (True, sens, 0 if sens else v)


class Sim:
    def __init__(self, inp):
        self.s = inp['stream']
        if len(self.s) + 1 > STREAM_MAX:
            raise Fail('flujo demasiado largo')
        self.pos = 0
        self.trace = []
        self.chunks = 0

    def readline(self, size=256):
        if self.pos >= len(self.s):
            raise Fail('EOF en el flujo (fgets dejaria el buffer sin tocar)')
        take = min(size - 1, len(self.s) - self.pos)
        k = self.s.find('\n', self.pos, self.pos + take)
        if k >= 0:
            take = k - self.pos + 1
        piece = self.s[self.pos:self.pos + take]
        before = self.pos
        self.pos += take
        self.trace.append([1, before, self.pos, size | (1 << 16) | (1 << 17)])
        if not piece.endswith('\n'):
            self.chunks += 1
        return piece


def expected_load(inp, case):
    sim = Sim(inp)
    stats = {}
    pre = bytes.fromhex(inp['pre'])
    n = s32(int.from_bytes(pre[0:4], 'little'))
    version = s32(inp['version'])
    plan_body, plan_retype = inp['plan_body'], inp['plan_retype']
    junk_type, junk_b6 = inp['junk_type'] & 0xFFFF, inp['junk_b6'] & 0xFF
    sim.readline()
    line = sim.readline()
    assigned, sens, val = scan_s_d(line)
    if assigned:
        if sens:
            return None, {'unknown_count': True}
        n = val
    else:
        stats['num_unparsed'] = True
    if n > MAX_SLOTS:
        raise Fail('contador > %d' % MAX_SLOTS)
    mv, rd = 0xFF, 0xFF
    sim.trace.append([2, (n * 4) & M32, 4, 1])
    objs, slots = [], []
    junk_used = False
    time_read = False
    for i in range(max(n, 0)):
        name = scan_s(sim.readline())
        if name is None:
            raise Fail('linea de nombre en blanco (field sin inicializar)')
        word = name[0]
        if len(word) > 127:
            raise Fail('nombre demasiado largo para field[128]')
        tm, tm_sens = 1, False
        if word in TYPES:
            typ = TYPES[word]
            if (typ & 0xF000) > 0x1000:
                if version >= 1:
                    a, sn, v = scan_s_d(sim.readline())
                    time_read = True
                    if a:
                        tm, tm_sens = (None, True) if sn else (v & 0xFF, False)
                    else:
                        stats['tm_unparsed'] = True
                else:
                    stats['time_skipped_version'] = True
            ordn = len(objs)
            if ordn >= POOL:
                raise Fail('pool agotado')
            sim.trace.append([3, 1, typ, ordn])
            body = plan_body[ordn] if ordn < len(plan_body) else 0
            retype = plan_retype[ordn] if ordn < len(plan_retype) else 0
            sim.trace.append([4, ordn, version & M32, 1 | (typ << 1)])
            for _ in range(body):
                sim.readline()
            cur = retype if retype else typ
            if retype and retype != typ:
                stats['retyped'] = True
            sim.readline()
            objs.append([cur, tm, True])
            slots.append(ordn)
            t = cur
            orig = typ
            if tm_sens:
                stats['tm_sens'] = True
            elif tm not in (0, 1):
                stats['tm_weird'] = True
        else:
            stats['unknown_name'] = True
            junk_used = True
            slots.append(0x10000)
            t = junk_type
            orig = t
        mv0, rd0 = mv, rd
        for tt, mark in ((t, True), (orig, False)):
            m2, r2 = mv0, rd0
            if tt & 0x1000:
                pass
            elif tt & 0x2000:
                if m2 == 0xFF:
                    m2 = i & 0xFF
            elif tt & 0xC000:
                if r2 == 0xFF:
                    r2 = i & 0xFF
            if mark:
                mv, rd = m2, r2
            elif (m2, r2) != (mv, rd):
                stats['retype_changes_offsets'] = True
        if t & 0x1000 and t & 0xE000:
            stats['both_bits'] = True
    if time_read:
        stats['time_read'] = True
    if rd == 0xFF:
        rd = n & 0xFF
        stats['render_from_count'] = True
    if mv == 0xFF:
        mv = rd
        stats['move_from_render'] = True
    mgr = bytearray(pre[:MGR])
    mgr[0:4] = le32(n)
    mgr[4:8] = b'\0\0\0\0'
    mgr[8] = mv
    mgr[9] = rd
    if n <= 0:
        stats['n_le0'] = True
    if n == 0:
        stats['n0'] = True
    if n < 0:
        stats['n_neg'] = True
    if n >= 128:
        stats['n_ge128'] = True
    if n >= 256:
        stats['n_ge256'] = True
    if junk_used and junk_type & 0xE000 and not junk_type & 0x1000:
        stats['junk_offsets'] = True
    if mv >= 128 and mv != 0xFF:
        stats['mv_neg'] = True
    if sim.chunks:
        stats['long_lines'] = True
    if any(x > 0 for x in plan_body[:len(objs)]):
        stats['body_lines'] = True
    if version < 1:
        stats['version_lt1'] = True
    stats['objs_%d' % min(len(objs), 8)] = True
    want = {
        'mgr': bytes(mgr).hex(), 'pinfos': KGUARD, 'guard_ok': True,
        'slots': slots[:MAX_SLOTS], 'ptr_outside_ok': True,
        'objs': objs, 'pool_outside_ok': True,
        'junk': [junk_type, 1 if junk_used else junk_b6, True],
        'stream_ok': True, 'cursor': sim.pos,
        'trace_len': len(sim.trace), 'trace_hash': trace_hash(sim.trace), 'trace_head': sim.trace[:24], 'trace_tail': sim.trace[-8:], '_trace': sim.trace,
    }
    return want, stats


def trace_hash(trace):
    """FNV-1a 64 sobre kind,a,b,c de cada evento (cuatro u32 little endian), igual que Core.hpp::Push."""
    h = 1469598103934665603
    for ev in trace:
        for w in ev:
            for b in (w & M32).to_bytes(4, 'little'):
                h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return '%016x' % h


def expected(function, inp, case):
    if function != NAME:
        raise Fail('funcion desconocida %s' % function)
    want, stats = expected_load(inp, case)
    if want is None:
        raise Fail('contador desconocido (desbordamiento de %d)')
    return want, stats


def compare(obs, want):
    """Lista de claves en las que lo observado difiere de lo esperado (None en un byte de tiempo = desconocido)."""
    bad = []
    for key, w in want.items():
        if key.startswith('_'):
            continue
        o = obs.get(key)
        if key == 'objs':
            if not isinstance(o, list) or len(o) != len(w) or any(len(a) != 3 or a[0] != b[0] or (b[1] is not None and a[1] != b[1]) or a[2] != b[2] for a, b in zip(o, w)):
                bad.append(key)
        elif o != w:
            bad.append(key)
    return bad


# ---------------------------------------------------------------------------------------------------- autoprueba
def _inp(stream, version=1, pre_n=0, plan_body=(), plan_retype=(), junk_type=0x2001, junk_b6=0x33):
    pre = bytearray(range(0x80, 0x80 + MGR))
    pre[0:4] = le32(pre_n)
    return dict(kind=0, sub=0, stream=stream, version=version, pre=bytes(pre).hex(), junk_type=junk_type, junk_b6=junk_b6, plan_body=list(plan_body), plan_retype=list(plan_retype),
                eax=0, edx=0, stack=0)


def _selftest():
    # sscanf
    assert scan_s_d('TIMEMODEPRT: 5\n') == (True, False, 5)
    assert scan_s_d('TIMEMODEPRT:5\n') == (False, False, 0)
    assert scan_s_d('X 0x10') == (True, False, 0)
    assert scan_s_d('X -7abc') == (True, False, -7)
    assert scan_s_d('X - 4') == (False, False, 0)
    assert scan_s_d('X 3000000000')[:2] == (True, True)
    assert scan_s_d('   \n') == (False, False, 0)
    assert scan_s('  abc def') == ('abc', ' def') and scan_s('  \n') is None
    # fgets: linea larga partida en 255 + resto
    sim = Sim(dict(stream='a' * 300 + '\nb\n'))
    assert len(sim.readline()) == 255 and sim.readline() == 'a' * 45 + '\n' and sim.readline() == 'b\n'
    # 1) vacio
    want, st = expected(NAME, _inp('h\nNUM_INFOS: 0\nFILLER\n'), 0)
    m = bytes.fromhex(want['mgr'])
    assert m[0:4] == b'\0\0\0\0' and m[8] == 0 and m[9] == 0 and want['_trace'][-1] == [2, 0, 4, 1] and want['cursor'] == len('h\nNUM_INFOS: 0\n')
    assert want['slots'] == [] and want['objs'] == []
    # 2) emision + movimiento + render con lineas de tiempo y cuerpo
    stream = ('h\nNUM_INFOS: 4\n'
              'FX_INFO_EMRATE_DATA:\n' 'b1\n' '-\n'
              'FX_INFO_NOISE_DATA:\n' 'TIMEMODEPRT: 0\n' 'b2\n' '-\n'
              'FX_INFO_COLOUR_DATA:\n' 'TIMEMODEPRT: 5\n' '-\n'
              'FX_INFO_SMOKE_DATA:\n' 'TIMEMODEPRT: 300\n' '-\n' 'FILLER 0\n')
    want, st = expected(NAME, _inp(stream, version=109, plan_body=[1, 1, 0, 0]), 0)
    m = bytes.fromhex(want['mgr'])
    assert m[8] == 1 and m[9] == 2 and m[0:4] == le32(4)
    assert want['objs'] == [[0x1001, 1, True], [0x2001, 0, True], [0x4001, 5, True], [0x8001, 300 & 0xFF, True]]
    assert want['slots'] == [0, 1, 2, 3] and want['_trace'][2] == [2, 16, 4, 1] and want['_trace'][3] == [1, 15, 36, 256 | 1 << 16 | 1 << 17]
    assert [t for t in want['_trace'] if t[0] == 4] == [[4, 0, 109, 1 | 0x1001 << 1], [4, 1, 109, 1 | 0x2001 << 1], [4, 2, 109, 1 | 0x4001 << 1], [4, 3, 109, 1 | 0x8001 << 1]]
    # 3) version 0: no se lee la linea de tiempo ; esa linea se consume como cuerpo/cierre
    stream = 'h\nNUM_INFOS: 1\nFX_INFO_FORCE_DATA:\nTIMEMODEPRT: 9\n-\nFILLER 0\nFILLER 1\n'
    want, st = expected(NAME, _inp(stream, version=0), 0)
    assert want['objs'] == [[0x2002, 1, True]] and st.get('time_skipped_version') and want['cursor'] == len('h\nNUM_INFOS: 1\nFX_INFO_FORCE_DATA:\nTIMEMODEPRT: 9\n')
    assert bytes.fromhex(want['mgr'])[8] == 0 and bytes.fromhex(want['mgr'])[9] == 1
    # 4) nombre desconocido: usa el FxInfo basura (tipo 0x4001 -> render en 0, byte 6 = 1)
    want, st = expected(NAME, _inp('h\nNUM_INFOS: 2\nFOO:\nFX_INFO_EMRATE_DATA:\n-\nF\n', junk_type=0x4001), 0)
    m = bytes.fromhex(want['mgr'])
    assert want['slots'] == [0x10000, 0] and want['junk'] == [0x4001, 1, True] and m[9] == 0 and m[8] == 0 and st['unknown_name'] and st['junk_offsets']
    # 5) NUM_INFOS ilegible: conserva el valor previo ; offsets en bytes
    want, st = expected(NAME, _inp('h\nNUM_INFOS:3\nFX_INFO_EMRATE_DATA:\n-\nFX_INFO_EMRATE_DATA:\n-\nFX_INFO_EMRATE_DATA:\n-\nF\n', pre_n=3), 0)
    m = bytes.fromhex(want['mgr'])
    assert m[0:4] == le32(3) and m[9] == 3 and m[8] == 3 and st['num_unparsed']
    # 6) retype: el Load deja un tipo con 0x1000 y 0x2000 -> no cambia offsets ; con 0x2000 -> movimiento
    want, st = expected(NAME, _inp('h\nNUM_INFOS: 2\nFX_INFO_EMRATE_DATA:\n-\nFX_INFO_EMSIZE_DATA:\n-\nF\n', plan_retype=[0x3000, 0x2000]), 0)
    m = bytes.fromhex(want['mgr'])
    assert m[8] == 1 and m[9] == 2 and st['retyped'] and st['retype_changes_offsets']
    # 7) n <= 0
    want, st = expected(NAME, _inp('h\nNUM_INFOS: -5\nF\n'), 0)
    m = bytes.fromhex(want['mgr'])
    assert m[0:4] == le32(-5) and m[9] == 0xFB and m[8] == 0xFB and want['_trace'][-1] == [2, (-20) & M32, 4, 1]
    # 8) n = 255 con todo emision: render = 0xFF = -1 (se queda en "sin asignar") y movimiento lo copia
    stream = 'h\nNUM_INFOS: 255\n' + 'FX_INFO_EMRATE_DATA:\n-\n' * 255 + 'F\n'
    want, st = expected(NAME, _inp(stream), 0)
    m = bytes.fromhex(want['mgr'])
    assert m[8] == 0xFF and m[9] == 0xFF
    stream = 'h\nNUM_INFOS: 256\n' + 'FX_INFO_EMRATE_DATA:\n-\n' * 256 + 'F\n'
    want, st = expected(NAME, _inp(stream), 0)
    assert bytes.fromhex(want['mgr'])[9] == 0 and bytes.fromhex(want['mgr'])[8] == 0
    # 9) nombre en blanco -> fixture invalido ; EOF -> invalido
    for bad in ('h\nNUM_INFOS: 1\n\nF\n', 'h\nNUM_INFOS: 1\n'):
        try:
            expected(NAME, _inp(bad), 0)
            raise AssertionError('debia rechazar')
        except Fail:
            pass
    # 10) desbordamiento de TIMEMODEPRT: byte desconocido
    want, st = expected(NAME, _inp('h\nNUM_INFOS: 1\nFX_INFO_NOISE_DATA:\nTIMEMODEPRT: 4294967296\n-\nF\n'), 0)
    assert want['objs'][0][1] is None and compare({'objs': [[0x2001, 77, True]]}, {'objs': want['objs']}) == []
    # 11) compare
    assert compare({'a': 1}, {'a': 2}) == ['a']
    print('ok')


if __name__ == '__main__':
    _selftest()
