"""Comprueba fixture_oracle.py contra el lado EXE del arnes (bytes reales del EXE reubicados) y las fixtures de Fixtures.hpp contra su espejo en Python.
Uso (sin el juego): python -I cmp_o.py   (ejecuta cph30.exe 4096 0 d y cph30.exe 4096 0 o)"""
import struct
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import fixture_oracle as O

M32 = 0xFFFFFFFF


def table():
    out = []
    for i in range(256):
        v = O.mix((i * 7919 + 3) & M32)
        out.append(((v >> 3) & 0x007FFFFF) | (0x3F000000 if (v & 1) else 0xBF000000))
    return out


def run(args):
    r = subprocess.run([str(HERE / 'cph30.exe')] + args, capture_output=True, text=True, check=True)
    return r.stdout.splitlines()


def check_fixtures():
    lines = run(['4096', '0', 'd'])
    assert len(lines) == O.CASES
    for line in lines:
        t = line.split()
        c = int(t[1])
        f = O.make(c)
        head = [int(x) for x in t[2:12]]
        exp = [f['family'], f['hasVel'], f['createLocal'], f['mustCreate'], f['poolMode'], f['listMode'], f['flagsOther'], f['radiusKind'], f['dirKind'], f['fill']]
        assert head[:10] == exp, (c, head, exp)
        assert int(t[12], 16) == f["seed"], (c, t[13], f['seed'])
        rest = [int(x, 16) for x in t[14:]]
        want = f['emission'] + f['mult'] + f['wld'] + f['getRw'] + f['velOverride'] + f['velAdd'] + [f['timeSince'], f['brightness']]
        assert rest == want, (c, [i for i in range(len(want)) if rest[i] != want[i]][:5])
    print('fixtures: %d casos iguales a Fixtures.hpp' % len(lines))


def check_oracle():
    tab = table()
    lines = [l for l in run(['4096', '0', 'o']) if l.startswith('O ')]
    bad = 0
    unknown = 0
    for line in lines:
        head, tr = line.split(' T')
        t = head.split()
        c = int(t[1])
        base = int(t[2], 16)
        eax = int(t[3], 16)
        rn = int(t[4], 16)
        arena = bytes.fromhex(t[5])
        trace = [int(x, 16) for x in tr.split()]
        f = O.make(c)
        mem, otrace, oret, orand, stats, unk = O.run(f, tab, base)
        ok = True
        for i in range(O.K_ARENA):
            if i in unk:
                continue
            if mem[i] != arena[i]:
                ok = False
                break
        ok &= len(trace) == len(otrace) and all(b is None or a == b for a, b in zip(trace, otrace))
        ok &= eax == oret and rn == orand
        unknown += bool(unk) or any(b is None for b in otrace)
        if not ok:
            bad += 1
            if bad <= 5:
                print('DIF caso', c, 'eax', hex(eax), hex(oret), 'rand', hex(rn), hex(orand), 'trace', len(trace), len(otrace))
                for i in range(O.K_ARENA):
                    if i not in unk and mem[i] != arena[i]:
                        print('   off %03x exe=%02x oraculo=%02x' % (i, arena[i], mem[i]))
                        break
                for k, (a, b) in enumerate(zip(trace, otrace)):
                    if b is not None and a != b:
                        print('   traza[%d] exe=%x oraculo=%x' % (k, a, b))
                        break
    print('oraculo: %d casos, %d diferencias, %d con comodines' % (len(lines), bad, unknown))
    return bad


if __name__ == '__main__':
    check_fixtures()
    sys.exit(1 if check_oracle() else 0)
