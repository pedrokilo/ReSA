"""Oraculo independiente del banco CBDNRenderCB30: CCustomBuildingDNPipeline::CustomPipeRenderCB (0x5D6480).

Se modela desde el DESENSAMBLADO del EXE (bash reversing/_handoff/dis.sh 0x5D6480 0x5D6750), no desde el C++ de la DLL, a partir de la fila `input` del banco.

Callback de dibujo de la tuberia (cdecl, void, `ret` normal): (RwResEntry* entry, void* object, uint8 type, uint32 flags). Cabecera = entry + 0x18; mallas = cabecera + 0x40 (0x24 bytes cada una).
  5D6495  _rwD3D9EnableClippingIfNeeded(object, (u8)type)                      (0x756D90)
  5D64A4  RwD3D9GetRenderState(0x89 = D3DRS_LIGHTING, &lighting)               (0x7FC320; DWORD completo)
  5D64B0  si lighting == 0 y !(flags & 8) [byte bajo]: sinLuz = 1:
            RwD3D9SetTexture(0, 0) (0x7FDE70) ; SetRenderState(0x3C, 0xFF000000) (0x7FC2D0) ; SetTSS(0,1,3) ; SetTSS(0,3,3) ; SetTSS(0,4,3) ; SetTSS(0,6,3)   (0x7FC340)
          si no sinLuz = 0
  5D6518  si cabecera[+0x20 - 0x18 = +8] (indexBuffer) != 0: SetIndices(ib) (0x7FA1C0)
  5D6529  _rwD3D9SetStreams(&cabecera[+0x10], cabecera[+0x30] useOffsets) (0x7FA090) ; SetVertexDeclaration(cabecera[+0x34]) (0x7F9F70)
  5D653D  n = cabecera[+4] CON SIGNO (jle): bucle n veces (n <= 0: nada); edi = mallas, +0x24 por vuelta:
    5D6550  mat = malla[+8] ; bit = byte [mat+0x10] & 1 ; env = *(mat + [8D12C4])   (se lee SIEMPRE, se usa solo con bit)
            SetTSS(1,1,1) ; SetTSS(2,1,1)
    5D657B  si bit:
            fpRenderStateSet(2 = TEXTUREADDRESS, 1 = WRAP)  (llamada por la tabla del motor: [[C97B24]+0x20])
            [C02C28] = (float)(int8)env[0] * 0.125 ; [C02C64] = 1.0 ; [C02C50] = 1.0 ; [C02C3C] = (float)(int8)env[1] * 0.125     (diagonal de la matriz 0xC02C28)
            RwD3D9SetTexture(env[+8], 1)
            v = _ftol2( ((float)env[4] * 1/255) * 254.0 ) [float de 24 bits en cada operacion] ; si v > 255 (con signo): 255 ; color = 0xFF000000 | v<<16 | v<<8 | v
            SetRenderState(0x3C, color) ; SetTSS(1,1,25) (1,26,1) (1,2,2) (1,3,0) (1,4,3) (1,5,2) (1,6,1) (1,0xB,0x10001) (1,0x18,0) (2,1,1)
    5D669E  _rwD3D9RenderStateVertexAlphaEnable(malla[+0xC] != 0 ? 1 : (byte [mat+7] != 0xFF ? 1 : 0))   (0x7FE0A0)
    5D66C2  si sinLuz: 0x5DA640(cabecera, malla)
            si no: si lighting != 0: RwD3D9SetSurfaceProperties(mat+0xC, mat+4, flags) (0x7FC4D0) ; 0x5DA6A0(cabecera, malla, flags, [mat+0])
  5D6718  SetTSS(1,1,1) ; SetTSS(1,4,1) ; SetTSS(1,0xB,1) ; SetTSS(1,0x18,0) ; ret
Formato de la traza (kind, a, b, c, d):
  1 [objeto, tipo]  2 [estado]  3 [textura cruda, etapa]  4 [estado, valor]  5 [etapa, tipo, valor]  6 [indexBuffer]  7 [desplazamiento de los flujos en la cabecera, useOffsets]
  8 [declaracion]  9 [estado, valor] (fpRenderStateSet)  10 [habilitar]  11 [indice de material, desplazamiento propiedades | desplazamiento color << 16, banderas]
  12 [desplazamiento de la cabecera en el entry, indice de malla]  13 [desplazamiento de la cabecera, indice de malla, banderas, textura cruda]
"""
import struct
import sys

NAME = 'CCustomBuildingDNPipeline::CustomPipeRenderCB'
NAMES = [NAME]
CASES = {NAME: 6144}
# Constantes de disposicion (DEBEN coincidir con Fixtures.hpp; la autoprueba cruzada fixture_dump.cpp lo verifica)
K = dict(kMaxMeshes=4, kEntryHdr=0x18, kHeaderSize=0x40, kMeshSize=0x24, kResSize=0xE8, kResGuard=0x40, kResAt=0x40, kResArena=0x168, kNumMats=4, kMatWin=0x80, kMatGuard=0x20,
         kMatAt=0x20, kMatArena=0xC0, kNumEnvs=4, kEnvSize=0x10, kEnvGuard=0x20, kEnvAt=0x20, kEnvArena=0x50, kObjSize=0x20, kObjGuard=0x20, kObjAt=0x20, kObjArena=0x60,
         kObjProbeOffset=0x10, kMtxSize=64, kGuardByte=0xA7, kBogusBase=0xC0DE0000, kEnvSlotNull=0, kEnvSlotBase=1, kCases=6144, kGroups=12)
OBJ_RAW = 0x0BADF00D                  # valor crudo del objeto cuando obj_kind == 2 (Core.hpp usa el mismo)
M32 = 0xFFFFFFFF
F_1_255 = struct.unpack('<f', struct.pack('<I', 0x3B808081))[0]     # [859A3C]
F_254 = 254.0                                                       # [86BE90]
F_0125 = 0.125                                                      # [858C48]


class Fail(Exception):
    pass


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def s8(b):
    return b - 256 if b >= 128 else b


def dw(buf, off):
    return struct.unpack_from('<I', buf, off)[0]


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('Funcion inesperada ' + function)
    try:
        return _expected(inp, case)
    except Fail as e:
        raise RuntimeError('Fixture invalido en el caso %d: %s' % (case, e))


def _expected(inp, case):
    res = bytearray.fromhex(inp['res'])
    mats = [bytearray.fromhex(h) for h in inp['mat']]
    envs = [bytearray.fromhex(h) for h in inp['env']]
    mtx = bytearray.fromhex(inp['mtx'])
    slots = inp['mat_slot']
    mesh_mat = inp['mesh_mat']
    if len(res) != K['kResSize'] or len(mats) != K['kNumMats'] or len(envs) != K['kNumEnvs'] or len(mtx) != K['kMtxSize'] or inp['env_off'] % 4 or inp['env_off'] + 4 > K['kMatWin']:
        raise Fail('imagenes de tamano inesperado')
    st = {}
    lighting = inp['lighting']
    flags = inp['flags']
    typ = inp['type_arg'] & 0xFF
    hdr = K['kEntryHdr']
    nm_raw = dw(res, hdr + 4)
    if nm_raw != inp['num_meshes']:
        raise Fail('numMeshes de la cabecera distinto del de la fila')
    nm = nm_raw - (1 << 32) if nm_raw >= 1 << 31 else nm_raw
    if nm > K['kMaxMeshes']:
        raise Fail('mas mallas de las que caben')
    ib = dw(res, hdr + 8)
    use_off = dw(res, hdr + 0x30)
    decl = dw(res, hdr + 0x34)
    obj = {0: K['kObjProbeOffset'], 1: 0, 2: OBJ_RAW}[inp['obj_kind']]
    tr = []

    def ev(k, a=0, b=0, c=0, d=0):
        tr.append([k, a, b, c, d])

    ev(1, obj, typ)
    ev(2, 0x89)
    nolight = lighting == 0 and (flags & 0xFF & 8) == 0
    st['nolight'] = nolight
    st['light_on'] = lighting != 0
    st['light_zero_prelit'] = lighting == 0 and not nolight
    st['light_low_byte_zero_nonzero'] = lighting != 0 and (lighting & 0xFF) == 0
    st['flags_hi_prelit_clear'] = (flags & 8) == 0 and (flags & ~0xFF) != 0
    st['type_garbage_hi'] = (inp['type_arg'] & ~0xFF) != 0
    st['obj_null'] = inp['obj_kind'] == 1
    st['obj_raw'] = inp['obj_kind'] == 2
    if nolight:
        ev(3, 0, 0)
        ev(4, 0x3C, 0xFF000000)
        for t in (1, 3, 4, 6):
            ev(5, 0, t, 3)
    st['ib'] = ib != 0
    st['no_ib'] = ib == 0
    st['decl_null'] = decl == 0
    if ib != 0:
        ev(6, ib)
    ev(7, 0x10, use_off)
    ev(8, decl)
    st['nm_le0'] = nm <= 0
    st['nm_neg'] = nm < 0
    st['nm_int_min'] = nm_raw == 0x80000000
    for n_ in range(1, 5):
        st['nm%d' % n_] = nm == n_
    cover_env = False
    for i in range(max(nm, 0)):
        mi = mesh_mat[i]
        mimg = mats[mi]
        mesh_off = hdr + K['kHeaderSize'] + i * K['kMeshSize']
        vertex_alpha = dw(res, mesh_off + 0xC)
        bit = mimg[0x10] & 1
        slot = slots[mi]
        ev(5, 1, 1, 1)
        ev(5, 2, 1, 1)
        if bit:
            e = slot - K['kEnvSlotBase']
            if not 0 <= e < K['kNumEnvs']:
                raise Fail('bit de entorno con ranura invalida')
            env = envs[e]
            cover_env = True
            sx, sy = s8(env[0]), s8(env[1])
            st['env_mesh'] = True
            st['sx_neg'] = sx < 0
            st['sy_neg'] = sy < 0
            ev(9, 2, 1)
            struct.pack_into('<f', mtx, 0x00, f32(sx * F_0125))
            struct.pack_into('<f', mtx, 0x3C, 1.0)
            struct.pack_into('<f', mtx, 0x28, 1.0)
            struct.pack_into('<f', mtx, 0x14, f32(sy * F_0125))
            ev(3, dw(env, 8), 1)
            p = f32(env[4] * F_1_255)               # fild + fmul (24 bits): un solo redondeo
            q = f32(p * F_254)                      # fmul (24 bits)
            v = int(q)                              # _ftol2: trunca hacia cero
            if v > 255:
                v = 255
            st['shin0'] = st.get('shin0', False) or env[4] == 0
            st['shin_max'] = st.get('shin_max', False) or env[4] == 0xFF
            color = 0xFF000000 | (v << 16) | (v << 8) | v
            ev(4, 0x3C, color)
            for t, val in ((1, 25), (26, 1), (2, 2), (3, 0), (4, 3), (5, 2), (6, 1), (0xB, 0x10001), (0x18, 0)):
                ev(5, 1, t, val)
            ev(5, 2, 1, 1)
        else:
            st['env_off_mesh'] = True
            st['env_off_slot_null'] = st.get('env_off_slot_null', False) or slot == 0
            st['env_off_slot_valid'] = st.get('env_off_slot_valid', False) or 1 <= slot <= 4
            st['env_off_slot_garbage'] = st.get('env_off_slot_garbage', False) or slot > 4
        alpha_byte = mimg[7]
        enable = 1 if vertex_alpha != 0 else (1 if alpha_byte != 0xFF else 0)
        st['alpha_vertex'] = st.get('alpha_vertex', False) or vertex_alpha != 0
        st['alpha_color'] = st.get('alpha_color', False) or (vertex_alpha == 0 and alpha_byte != 0xFF)
        st['alpha_opaque'] = st.get('alpha_opaque', False) or (vertex_alpha == 0 and alpha_byte == 0xFF)
        ev(10, enable)
        if nolight:
            ev(12, hdr, i)
        else:
            if lighting != 0:
                ev(11, mi, 0xC | (4 << 16), flags)
            ev(13, hdr, i, flags, dw(mimg, 0))
    st['cover_env'] = cover_env
    ev(5, 1, 1, 1)
    ev(5, 1, 4, 1)
    ev(5, 1, 0xB, 1)
    ev(5, 1, 0x18, 0)
    # contadores por caso (True si ocurre al menos una vez); los conteos por malla se hacen con las banderas de arriba
    n_env = sum(1 for e in tr if e[0] == 9)
    st['env_meshes_ge2'] = n_env >= 2
    st['surf_props'] = any(e[0] == 11 for e in tr)
    st['render_lighting'] = any(e[0] == 13 for e in tr)
    st['render_nolight'] = any(e[0] == 12 for e in tr)
    want = {'trace': tr, 'matrix': mtx.hex(), 'res_ok': True, 'mats_ok': True, 'envs_ok': True, 'obj_ok': True}
    return want, st


def compare(obs, want):
    return [k for k, w in want.items() if obs.get(k) != w]


# ---------------------------------------------------------------------------------------------------- autoprueba
def _inp(**kw):
    res = bytearray(range(256))[:K['kResSize']]
    struct.pack_into('<I', res, K['kEntryHdr'] + 4, kw.pop('nm', 1))
    struct.pack_into('<I', res, K['kEntryHdr'] + 8, kw.pop('ib', 0))
    struct.pack_into('<I', res, K['kEntryHdr'] + 0x30, kw.pop('use', 1))
    struct.pack_into('<I', res, K['kEntryHdr'] + 0x34, kw.pop('decl', 0x1234))
    for m in range(4):
        struct.pack_into('<I', res, K['kEntryHdr'] + K['kHeaderSize'] + m * K['kMeshSize'] + 0xC, kw.pop('va%d' % m, 0))
    mats = []
    for i in range(4):
        m = bytearray(b'\x11' * 0x80)
        struct.pack_into('<I', m, 0, 0xB0000000 + i)
        m[7] = 0xFF
        m[0x10] = kw.pop('bit%d' % i, 0)
        mats.append(m.hex())
    envs = []
    for j in range(4):
        e = bytearray(b'\x22' * 16)
        e[0] = (0x80 if j == 1 else 0x08)
        e[1] = 0xFE
        e[4] = kw.pop('shin%d' % j, 255)
        struct.pack_into('<I', e, 8, 0xD0000000 + j)
        envs.append(e.hex())
    d = dict(group=0, n=0, env_off=0x28, num_meshes=struct.unpack_from('<I', res, K['kEntryHdr'] + 4)[0], lighting=1, flags=0, type_arg=0x1234AB01, obj_kind=0, res=res.hex(),
             mesh_mat=[0, 1, 2, 3], mat=mats, mat_slot=[0, 0, 0, 0], env=envs, mtx=(b'\x00' * 64).hex(), eax=0, ecx=0, edx=0, stack_seed=0)
    d.update(kw)
    return d


def _selftest():
    # mallas = 0, sin luz (lighting 0 y sin PRELIT): solo el prologo, los flujos y el epilogo
    w, s = expected(NAME, _inp(nm=0, lighting=0), 0)
    kinds = [e[0] for e in w['trace']]
    assert kinds == [1, 2, 3, 4, 5, 5, 5, 5, 7, 8, 5, 5, 5, 5], kinds
    assert w['trace'][0] == [1, 0x10, 1, 0, 0] and w['trace'][8] == [7, 0x10, 1, 0, 0] and w['matrix'] == '00' * 64
    # con luz y un indice: SetIndices antes de los flujos
    w, s = expected(NAME, _inp(nm=0, lighting=1, ib=0x77), 0)
    assert [e[0] for e in w['trace']] == [1, 2, 6, 7, 8, 5, 5, 5, 5] and w['trace'][2] == [6, 0x77, 0, 0, 0]
    # PRELIT (bit 3 del byte bajo) evita el modo sin luz; bit 3 solo en el byte alto no
    w, s = expected(NAME, _inp(nm=0, lighting=0, flags=8), 0)
    assert not s['nolight'] and len(w['trace']) == 8
    w, s = expected(NAME, _inp(nm=0, lighting=0, flags=0x800), 0)
    assert s['nolight'] and s['flags_hi_prelit_clear'] is True and len(w['trace']) == 14
    # numero de mallas negativo o INT_MIN: ninguna vuelta
    w, s = expected(NAME, _inp(nm=0xFFFFFFFF), 0)
    assert s['nm_neg'] and len(w['trace']) == 8
    w, s = expected(NAME, _inp(nm=0x80000000), 0)
    assert s['nm_int_min'] and len(w['trace']) == 8
    # una malla, material 0 sin entorno, con luz: TSS, alfa (0), propiedades y dibujo con luz
    w, s = expected(NAME, _inp(nm=1, lighting=1, flags=0x108), 0)
    t = w['trace'][4:]
    assert t[:2] == [[5, 1, 1, 1, 0], [5, 2, 1, 1, 0]] and t[2] == [10, 0, 0, 0, 0] and t[3] == [11, 0, 0x4000C, 0x108, 0] and t[4] == [13, 0x18, 0, 0x108, 0xB0000000], t
    # sin luz y alfa de vertice: dibujo sin luz
    w, s = expected(NAME, _inp(nm=1, lighting=0, va0=0x100), 0)
    assert [e for e in w['trace'] if e[0] in (10, 12, 13)] == [[10, 1, 0, 0, 0], [12, 0x18, 0, 0, 0]]
    # con entorno: matriz, textura de la etapa 1, factor y TSS
    w, s = expected(NAME, _inp(nm=1, lighting=1, flags=8, bit0=1, mat_slot=[1, 0, 0, 0], shin0=0xFF), 0)
    # env 0: escala X 8 -> 1.0 ; escala Y 0xFE = -2 -> -0.25 ; brillo 255 -> 254
    mt = bytes.fromhex(w['matrix'])
    assert struct.unpack_from('<f', mt, 0)[0] == 1.0 and struct.unpack_from('<f', mt, 0x14)[0] == -0.25 and struct.unpack_from('<f', mt, 0x28)[0] == 1.0 and struct.unpack_from('<f', mt, 0x3C)[0] == 1.0
    colors = [e for e in w['trace'] if e[0] == 4 and e[1] == 0x3C]
    assert colors == [[4, 0x3C, 0xFFFEFEFE, 0, 0]], colors
    assert [e for e in w['trace'] if e[0] == 9] == [[9, 2, 1, 0, 0]] and [3, 0xD0000000, 1, 0, 0] in w['trace']
    # brillo 0: color 0xFF000000 ; brillo 1: 1/255*254 = 0.996 -> 0 ; brillo 2: 1 (2*254/255 = 1.99)
    for sh, v in ((0, 0), (1, 0), (2, 1), (128, 127), (254, 253), (255, 254)):
        w, s = expected(NAME, _inp(nm=1, lighting=1, flags=8, bit0=1, mat_slot=[1, 0, 0, 0], shin0=sh), 0)
        c = [e for e in w['trace'] if e[0] == 4][0][2]
        assert c == (0xFF000000 | (v << 16) | (v << 8) | v), (sh, hex(c))
    # el bit 0 limpio con ranura valida o basura no hace nada de entorno; el bit 0 con ranura basura es invalido
    w, s = expected(NAME, _inp(nm=1, lighting=1, flags=8, mat_slot=[0xC0DE0300, 0, 0, 0]), 0)
    assert not s.get('env_mesh') and w['matrix'] == '00' * 64
    try:
        expected(NAME, _inp(nm=1, bit0=1, mat_slot=[0, 0, 0, 0]), 0)
        raise AssertionError('debia fallar')
    except RuntimeError:
        pass
    # varias mallas: el orden de las TSS por malla y los indices de malla
    w, s = expected(NAME, _inp(nm=3, lighting=0, mesh_mat=[0, 0, 1, 2]), 0)
    assert [e[2] for e in w['trace'] if e[0] == 12] == [0, 1, 2]
    print('ok')


if __name__ == '__main__':
    _selftest()
