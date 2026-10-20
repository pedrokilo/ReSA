"""Oraculo/clasificador del banco KillPedOnFoot62 (CTaskComplexKillPedOnFoot: ControlSubTask 0x626260, CreateFirstSubTask 0x62B490, CreateNextSubTask 0x62B150, CreateSubTask 0x625E70, MakeAbortable 0x625E40).

La VERDAD del banco es el EXE (par EXE/DLL fila a fila). Este modulo hace tres cosas independientes del C++ de la DLL:
  1. `selftest()`: comprueba contra los BYTES del EXE canonico los hechos del desensamblado en que se apoyan el banco y el README (primeros bytes de cada funcion, constantes 0.0 / 0.1 / 5.0,
     tamanos pedidos a operator new, mascara de bit de 0x474) y las constantes de arena de Fixtures.hpp (suma a mano frente a la salida del C++ compilado, via fixture_mirror.py).
  2. `classify()`: clasifica cada par por su comportamiento OBSERVADO (eventos de los falsos en orden, retorno, cambios en el mundo) para medir la cobertura de las fixtures (run_local.py).
  3. `expected()` (fixture_model.py): modelo del desensamblado de las CINCO funciones (nombres de eventos en orden, tamano de la reserva, contextos de Say, clase del retorno y ramas alcanzadas);
     run_local.py lo compara con lo observado en AMBOS lados.
"""
import struct
import sys
from pathlib import Path

EXE = Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe')
NAMES = ['CTaskComplexKillPedOnFoot::ControlSubTask', 'CTaskComplexKillPedOnFoot::CreateFirstSubTask', 'CTaskComplexKillPedOnFoot::CreateNextSubTask',
         'CTaskComplexKillPedOnFoot::CreateSubTask', 'CTaskComplexKillPedOnFoot::MakeAbortable']
CASES_BY_NAME = dict(zip(NAMES, (2048, 768, 768, 1024, 512)))
CASES = sum(CASES_BY_NAME.values())
ENTRY = dict(zip(NAMES, (0x626260, 0x62B490, 0x62B150, 0x625E70, 0x625E40)))

T_NONE, T_PAUSE, T_STAND, T_DRAG, T_LEAVECAR, T_DRIVE, T_MELEE, T_ARMED, T_DESTROY, T_SIGNAL, T_FINISHED = 0xC8, 0xCA, 0xCB, 0x2BF, 0x2C0, 0x2CF, 0x3E9, 0x3EA, 0x3EB, 0x4BA, 0x516

# Arenas: guarda inicial 0x20 + por region (redondeo a 16 + guarda 0x20). Suma a mano, independiente de Fixtures.hpp.
REGION_BYTES = [0x38, 0x7A0, 0x7A0, 0x48, 0x48, 0x5A0, 0x5A0, 0x80, 0x30, 0x40, 0x30, 0x40, 0x40, 0x40, 0x40, 0x70, 0x40, 0x40]
REGION_TAGS = ['T', 'D', 'G', 'M', 'N', 'V', 'X', 'J', 'WP', 'PD', 'WT', 'CP', 'CT', 'GR', 'S', 'A', 'O', 'L']
R_THIS, R_PED, R_SUB, R_ALLOC = 0, 1, 14, 15


def region_offsets():
    o, out = 0x20, []
    for b in REGION_BYTES:
        out.append(o)
        o += ((b + 15) & ~15) + 0x20
    return out, o


class Pe:
    def __init__(self, path=EXE):
        d = path.read_bytes()
        pe = struct.unpack_from('<I', d, 0x3C)[0]
        n = struct.unpack_from('<H', d, pe + 6)[0]
        opt = struct.unpack_from('<H', d, pe + 20)[0]
        self.base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
        self.d = d
        self.secs = []
        o = pe + 24 + opt
        for _ in range(n):
            vs, va, rs, rp = struct.unpack_from('<IIII', d, o + 8)
            self.secs.append((va, max(vs, rs), rp))
            o += 40

    def read(self, va, n):
        for sva, size, rp in self.secs:
            if sva <= va - self.base < sva + size:
                at = rp + va - self.base - sva
                return self.d[at:at + n]
        raise KeyError(hex(va))


def selftest(arena_dump=None):
    pe = Pe()
    assert pe.read(0x625E40, 6).hex() == '8b4424045681', pe.read(0x625E40, 6).hex()
    assert pe.read(0x62B150, 5).hex() == '83ec0c5356', pe.read(0x62B150, 5).hex()
    assert pe.read(0x62B490, 5).hex() == '83ec085356', pe.read(0x62B490, 5).hex()
    assert pe.read(0x626260, 2).hex() == '6aff'
    # constantes de coma flotante que usan Control (0.0, 0.1), First (0.0, 0.1, 5.0) y Next (5.0)
    for va, want in ((0x858B50, 0.0), (0x858B1C, struct.unpack('<f', struct.pack('<f', 0.1))[0]), (0x858C80, 5.0)):
        assert struct.unpack('<f', pe.read(va, 4))[0] == want, hex(va)
    # CreateSubTask: tamanos pedidos a operator new (push imm8; call 0x61A5A0) en las ramas del desensamblado
    code = pe.read(0x625E70, 0x3E0)
    sizes = []
    for i in range(len(code) - 7):
        if code[i] == 0x6A and code[i + 2] == 0xE8:
            tgt = (0x625E70 + i + 2 + 5 + struct.unpack_from('<i', code, i + 3)[0]) & 0xFFFFFFFF
            if tgt == 0x61A5A0:
                sizes.append(code[i + 1])
    # (la reserva de 0x18 de CTaskComplexSignalAtPed no es adyacente: `push 18h; mov edi,eax; call`)
    assert sorted(sizes) == sorted([0x18, 0x18, 0x8, 0x20, 0x34, 0x54, 0x70, 0x5C, 0x38, 0x20]), [hex(s) for s in sizes]
    # MakeAbortable: and dword [eax+474h], 0FFBFFFFFh; ControlSubTask: or dword [ebp+474h], 400000h
    assert pe.read(0x625E45, 10).hex() == '81a074040000ffffbfff', pe.read(0x625E45, 10).hex()
    assert pe.read(0x626281, 10).hex() == '81ad74040000000040' or pe.read(0x626281, 10)[:2] == b'\x81\x8d', pe.read(0x626281, 10).hex()
    off, total = region_offsets()
    if arena_dump is not None:
        assert arena_dump['arena'] == total, (arena_dump['arena'], total)
        assert arena_dump['offsets'] == off, (arena_dump['offsets'], off)
        assert arena_dump['cases'] == CASES, arena_dump['cases']
        assert arena_dump['route_cases'] == list(CASES_BY_NAME.values())
    return 'ok'


def classify(fn, inp, obs):
    """Claves de cobertura (True si se observa) de un par, a partir de lo observado en un lado."""
    ev = obs['events']
    k = {}
    short = fn.split('::')[-1]
    for e in ev:
        k['ev_' + e[0]] = True
        k[short + ':ev_' + e[0]] = True
        if e[0] == 'new':
            k['new_%02x' % e[1]] = True
            k[short + ':new_%02x' % e[1]] = True
        if e[0] == 'say':
            k['say_%x' % e[2]] = True
        if e[0] == 'abort':
            k[short + ':abort_prio%d' % min(e[3], 3)] = True
    if 'alloc_type' in obs:
        k['alloc_type_%x' % obs['alloc_type']] = True
        k[short + ':alloc_type_%x' % obs['alloc_type']] = True
    ret = obs['ret']
    k[short + ':ret_null'] = ret == 0
    k[short + ':ret_alloc'] = isinstance(ret, str) and ret.startswith('A+')
    k[short + ':ret_sub'] = isinstance(ret, str) and ret.startswith('S+')
    k[short + ':ret_other'] = not (ret == 0 or (isinstance(ret, str) and ret[:2] in ('A+', 'S+')))
    regs = {}
    for r, o, v in obs['changes']:
        regs.setdefault(r, []).append(o)
    k[short + ':wrote_this'] = R_THIS in regs
    k[short + ':wrote_ped'] = R_PED in regs
    k[short + ':wrote_sub'] = R_SUB in regs
    k[short + ':wrote_alloc'] = R_ALLOC in regs
    k[short + ':wrote_guard'] = any(r >= len(REGION_BYTES) or o >= REGION_BYTES[r] for r, o, v in obs['changes'])
    return {a: True for a, b in k.items() if b is True}


def expected(fn, inp, case):
    """Modelo del desensamblado (fixture_model.py): (esperado, ramas alcanzadas)."""
    import fixture_model
    return fixture_model.expected(fn, inp)


def observed_view(fn, obs):
    """Lo observado en el mismo formato que el modelo."""
    ret = obs['ret']
    if isinstance(ret, str):
        ret = 'alloc' if ret.startswith('A+') else 'sub' if ret.startswith('S+') else ret
    elif ret == 0 and not fn.endswith('MakeAbortable'):
        ret = None
    ev = obs['events']
    return {'names': [e[0] for e in ev], 'ret': ret, 'say': [e[2] for e in ev if e[0] == 'say'], 'new': [e[1] for e in ev if e[0] == 'new']}


if __name__ == '__main__':
    print(selftest())
