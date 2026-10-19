"""Oraculo independiente del banco CBDNMaterialSetup30: CCustomBuildingDNPipeline::CustomPipeMaterialSetup (0x5D7120).

Se modela desde el DESENSAMBLADO del EXE (bash reversing/_handoff/dis.sh 0x5D7120 0x5D71C0), no desde el C++ de la DLL, a partir de la fila `input` del banco.

cdecl, `ret` normal. Argumentos: material (esi) y data (no se usa). Es el callback de RpGeometryForAllMaterials (0x74C790), que solo lee EAX (`test eax,eax`).
  5D7126  dword [material+10h] = 0
  5D712D  eax = RpMatFXMaterialGetEffects(material) (0x812140: puntero MatFX en [material+[C9AB74]]; nulo -> 0; si no [ptr+30h])
  5D7135  si eax != 2 salta a 5D7169
  5D713A  eax = SetFxEnvTexture((material + [8D12C4])) (0x5D9570; devuelve el puntero de entorno, o nulo)
  5D714A  si nulo salta a 5D7169
  5D714E  edx = [[C9AB74] + material] (datos MatFX) ; ecx = [edx+4] (textura del primer pase) ; [eax+8] = ecx
  5D715A  si ecx != 0: byte [ecx+51h] = 11h ; byte [ecx+50h] = 2
  5D7169  ecx = [material + [8D12C4]] (SE VUELVE A LEER la ranura) ; edx = 0 ; si ecx == 0 salta a 5D71A6
  5D7177  fild byte [ecx+4] ; fmul dword [859A3C] (1/255) ; fcomp dword [858B50] (0.0) ; si igual (C3) salta a 5D71A6
  5D7196  si [ecx+8] == 0 salta a 5D71A6 ; edx = 1
  5D71A6  [material+10h] = ([material+10h] & 0xFFFFFFF8) | edx   (la palabra se RELEE)
  5D71B1  eax = material ; ret
El falso de SetFxEnvTexture (colaborador sustituido en ambos lados) tiene siete modos (Core.hpp):
  0 devuelve *ranura sin tocarla ; 1 *ranura = envA, devuelve envA ; 2 *ranura = nulo, devuelve nulo ; 3 *ranura = envB, devuelve envB ;
  4 devuelve envA sin tocar la ranura ; 5 *ranura = envA y devuelve nulo ; 6 escribe `flags_word` en [material+10h] y devuelve *ranura.
Traza del falso: [1, env_off, etiqueta de *ranura antes, 0].
"""
import json
import struct
import sys

NAME = 'CCustomBuildingDNPipeline::CustomPipeMaterialSetup'
NAMES = [NAME]
CASES = {NAME: 5120}
# Constantes de disposicion (DEBEN coincidir con Fixtures.hpp; la autoprueba cruzada fixture_dump.cpp lo verifica)
K = dict(kMatGuard=0x40, kMatAt=0x40, kMatWin=0x80, kMatArena=0x100, kEnvGuard=0x20, kEnvAt=0x20, kEnvSize=0x10, kEnvArena=0x50, kTexGuard=0x20, kTexAt=0x20, kTexSize=0x60,
         kTexArena=0xA0, kMfxGuard=0x20, kMfxAt=0x20, kMfxSize=0x40, kMfxArena=0x80, kGuardByte=0xA7, kCases=5120, kGroups=10)
LBL_NULL, LBL_ENVA, LBL_ENVB, LBL_TEX0, LBL_TEX1, LBL_MFX = 0, 1, 2, 3, 4, 5
M32 = 0xFFFFFFFF


class Fail(Exception):
    pass


def _check(inp):
    if inp['env_off'] % 4 or inp['mfx_off'] % 4 or inp['env_off'] + 4 > K['kMatWin'] or inp['mfx_off'] + 4 > K['kMatWin']:
        raise Fail('desplazamientos de plugin fuera de la ventana')
    if inp['env_off'] < 0x14 or inp['mfx_off'] < 0x14 or abs(inp['env_off'] - inp['mfx_off']) < 4:
        raise Fail('desplazamientos de plugin solapados con las banderas o entre si')
    if inp['mfx_tex'] not in (LBL_NULL, LBL_TEX0, LBL_TEX1) or inp['env_pre'] not in (LBL_NULL, LBL_ENVA, LBL_ENVB) or inp['mode'] > 6:
        raise Fail('etiqueta o modo invalido')
    if inp['effects'] == 2 and not inp['mfx_present']:
        raise Fail('efectos == 2 sin datos de MatFX')


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('Funcion inesperada ' + function)
    try:
        return _expected(inp, case)
    except Fail as e:
        raise RuntimeError('Fixture invalido en el caso %d: %s' % (case, e))


def _expected(inp, case):
    _check(inp)
    st = {}
    env = {LBL_ENVA: dict(img=bytearray.fromhex(inp['env_a']), tex=inp['env_a_tex']), LBL_ENVB: dict(img=bytearray.fromhex(inp['env_b']), tex=inp['env_b_tex'])}
    tex = {LBL_TEX0: bytearray.fromhex(inp['tex0']), LBL_TEX1: bytearray.fromhex(inp['tex1'])}
    slot = inp['env_pre']
    flags = 0                                                    # 5D7126
    trace = []
    eff = inp['effects'] if inp['mfx_present'] else 0            # RpMatFXMaterialGetEffects
    st['eff2'] = eff == 2
    st['eff_not2'] = eff != 2
    st['eff_other_nonzero'] = eff not in (0, 2)
    st['mfx_null'] = not inp['mfx_present']
    st['eff_low_bits_2'] = eff != 2 and (eff & 0xFF) == 2
    if eff == 2:
        trace.append([1, inp['env_off'], slot, 0])
        mode = inp['mode']
        st['mode%d' % mode] = True
        before = slot
        if mode == 0:
            ret = slot
        elif mode == 1:
            slot = ret = LBL_ENVA
        elif mode == 2:
            slot = ret = LBL_NULL
        elif mode == 3:
            slot = ret = LBL_ENVB
        elif mode == 4:
            ret = LBL_ENVA
        elif mode == 5:
            slot = LBL_ENVA
            ret = LBL_NULL
        else:
            flags = inp['flags_word']
            ret = slot
        st['ret_null'] = ret == LBL_NULL
        st['ret_ne_slot'] = ret != slot
        st['slot_changed'] = slot != before
        if ret != LBL_NULL:
            t = inp['mfx_tex']                                    # [[C9AB74]+material] + 4
            env[ret]['tex'] = t
            st['tex_null'] = t == LBL_NULL
            if t != LBL_NULL:
                tex[t][0x51] = 0x11
                tex[t][0x50] = 2
                st['tex_written'] = True
    has = 0
    st['slot_final_null'] = slot == LBL_NULL
    if slot != LBL_NULL:
        e = env[slot]
        shin_nz = e['img'][4] != 0
        tex_nz = e['tex'] != 0
        st['shin0'] = not shin_nz
        st['tex_zero'] = not tex_nz
        if shin_nz and tex_nz:
            has = 1
    st['has1'] = has == 1
    st['has0'] = has == 0
    flags = ((flags & 0xFFFFFFF8) | has) & M32                   # 5D71A6: se relee la palabra
    st['flags_hi_kept'] = (flags & 0xFFFFFFF8) != 0
    mat = bytearray.fromhex(inp['mat'])
    want = {
        'ret_ok': True, 'flags': flags, 'env_slot': slot, 'mfx_slot': LBL_MFX if inp['mfx_present'] else LBL_NULL,
        'mat': mat.hex(), 'mat_guard_ok': True,
        'env_a': env[LBL_ENVA]['img'].hex(), 'env_a_tex': env[LBL_ENVA]['tex'], 'env_a_guard_ok': True,
        'env_b': env[LBL_ENVB]['img'].hex(), 'env_b_tex': env[LBL_ENVB]['tex'], 'env_b_guard_ok': True,
        'tex0': tex[LBL_TEX0].hex(), 'tex0_guard_ok': True, 'tex1': tex[LBL_TEX1].hex(), 'tex1_guard_ok': True,
        'mfx': inp['mfx'], 'mfx_tex': inp['mfx_tex'], 'mfx_eff': inp['effects'], 'mfx_guard_ok': True,
        'trace': trace,
    }
    return want, st


def compare(obs, want):
    return [k for k, w in want.items() if obs.get(k) != w]


# ---------------------------------------------------------------------------------------------------- autoprueba
def _inp(**kw):
    d = dict(group=0, n=0, env_off=0x28, mfx_off=0x1C, effects=2, mfx_present=1, mfx_tex=LBL_TEX0, env_pre=LBL_NULL, mode=1, flags_word=0, pre_flags=0xFFFFFFFF,
             mat=(bytes(range(128))).hex(), env_a=(b'\x11' * 16).hex(), env_a_tex=0, env_b=(b'\x22' * 16).hex(), env_b_tex=0x99,
             tex0=(b'\x33' * 96).hex(), tex1=(b'\x44' * 96).hex(), mfx=(b'\x55' * 64).hex(), data=0, eax=0, ecx=0, edx=0, stack_seed=0)
    d.update(kw)
    return d


def _selftest():
    # modo 1 (envA con brillo != 0): la textura se copia; tex0 queda 2/0x11; bit 0 = 1
    ea = bytearray(b'\x11' * 16)
    ea[8:12] = bytes(4)
    w, s = expected(NAME, _inp(env_a=ea.hex()), 0)
    assert w['flags'] == 1 and w['env_slot'] == LBL_ENVA and w['env_a_tex'] == LBL_TEX0 and w['trace'] == [[1, 0x28, 0, 0]]
    t0 = bytearray(b'\x33' * 96)
    t0[0x51] = 0x11
    t0[0x50] = 2
    assert w['tex0'] == t0.hex() and w['tex1'] == (b'\x44' * 96).hex() and s['has1'] and s['tex_written']
    # brillo 0: bit 0 = 0
    ea[4] = 0
    w, s = expected(NAME, _inp(env_a=ea.hex()), 0)
    assert w['flags'] == 0 and s['shin0'] and s['has0']
    ea[4] = 1
    # textura nula: no hay bytes escritos y bit 0 = 0
    w, s = expected(NAME, _inp(env_a=ea.hex(), mfx_tex=LBL_NULL), 0)
    assert w['flags'] == 0 and w['tex0'] == (b'\x33' * 96).hex() and s['tex_null'] and s['tex_zero']
    # efectos != 2: no hay llamada; la ranura previa decide (envB con textura basura y brillo != 0)
    eb = bytearray(b'\x22' * 16)
    eb[8:12] = bytes(4)
    w, s = expected(NAME, _inp(effects=0x102, env_pre=LBL_ENVB, env_b=eb.hex()), 0)
    assert w['trace'] == [] and w['flags'] == 1 and s['eff_low_bits_2']
    w, s = expected(NAME, _inp(effects=7, mfx_present=0, env_pre=LBL_NULL), 0)
    assert w['trace'] == [] and w['flags'] == 0 and w['mfx_slot'] == LBL_NULL
    # modo 6: las banderas escritas por el falso conservan los bits 3..31
    w, s = expected(NAME, _inp(mode=6, flags_word=0xFFFFFFFF, env_pre=LBL_ENVB, env_b=eb.hex()), 0)
    assert w['flags'] == 0xFFFFFFF9, hex(w['flags'])
    w, s = expected(NAME, _inp(mode=6, flags_word=0xFFFFFFFF, env_pre=LBL_NULL), 0)
    assert w['flags'] == 0xFFFFFFF8
    # modo 4: devuelve envA pero la ranura sigue nula: la textura se escribe en envA y el bit 0 sale 0
    w, s = expected(NAME, _inp(mode=4, env_pre=LBL_NULL, env_a=ea.hex()), 0)
    assert w['flags'] == 0 and w['env_a_tex'] == LBL_TEX0 and w['env_slot'] == LBL_NULL and s['ret_ne_slot']
    # modo 5: la ranura queda en envA (textura previa nula en envA, brillo != 0) y no se escribe nada
    w, s = expected(NAME, _inp(mode=5, env_pre=LBL_NULL, env_a=ea.hex(), env_a_tex=0), 0)
    assert w['flags'] == 0 and w['env_slot'] == LBL_ENVA and w['env_a_tex'] == 0
    w, s = expected(NAME, _inp(mode=5, env_pre=LBL_NULL, env_a=ea.hex(), env_a_tex=0x77), 0)
    assert w['flags'] == 1
    # desplazamientos invalidos
    try:
        expected(NAME, _inp(env_off=0x10), 0)
        raise AssertionError('debia fallar')
    except RuntimeError:
        pass
    try:
        expected(NAME, _inp(effects=2, mfx_present=0), 0)
        raise AssertionError('debia fallar')
    except RuntimeError:
        pass
    print('ok')


if __name__ == '__main__':
    _selftest()
