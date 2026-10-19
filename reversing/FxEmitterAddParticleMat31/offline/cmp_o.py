"""Comprueba fixture_oracle.py contra el lado EXE del arnes (bytes reales del EXE reubicados) y las fixtures de Fixtures.hpp contra su espejo en Python.
Uso (sin el juego): python -I cmp_o.py   (ejecuta cphaddmat31.exe 4096 0 d y cphaddmat31.exe 4096 0 o)"""
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import fixture_oracle as O


def run(args):
    r = subprocess.run([str(HERE / 'cphaddmat31.exe')] + args, capture_output=True, text=True, check=True)
    return r.stdout.splitlines()


def check_fixtures():
    lines = [l for l in run(['4096', '0', 'd']) if l.startswith('A ')]
    assert len(lines) == O.CASES
    for line in lines:
        t = line.split()
        c = int(t[1])
        f = O.make(c)
        head = [int(x) for x in t[2:9]]
        assert head == [f['family'], f['useConst'], f['createLocal'], f['parent'], f['particleMode'], f['fill'], 0], (c, head)
        rest = [int(x, 16) for x in t[9:]]
        want = f['mat'] + f['vel'] + [f['timeSince'], f['rotZ'], f['brightness']] + f['mult'] + [f['length']] + f['emission'] + [v for m in f['matInit'] for v in m] + f['parentMat'] + f['getRw']
        assert rest == want, (c, [i for i in range(len(want)) if rest[i] != want[i]][:5])
    print('fixtures: %d casos iguales a Fixtures.hpp' % len(lines))


def check_oracle():
    lines = [l for l in run(['4096', '0', 'o']) if l.startswith('O ')]
    bad = 0
    for line in lines:
        head, tr = line.split(' T')
        t = head.split()
        c = int(t[1])
        base = int(t[2], 16)
        arena = bytes.fromhex(t[3])
        trace = [int(x, 16) for x in tr.split()]
        f = O.make(c)
        mem, otrace, stats = O.run(f, base)
        if bytes(mem) != arena or trace != otrace:
            bad += 1
            if bad <= 5:
                print('DIF caso', c, 'traza', len(trace), len(otrace))
                for i in range(O.K_ARENA):
                    if mem[i] != arena[i]:
                        print('   off %03x exe=%02x oraculo=%02x' % (i, arena[i], mem[i]))
                        break
                for k, (a, b) in enumerate(zip(trace, otrace)):
                    if a != b:
                        print('   traza[%d] exe=%x oraculo=%x' % (k, a, b))
                        break
    print('oraculo: %d casos, %d diferencias' % (len(lines), bad))
    return bad


if __name__ == '__main__':
    check_fixtures()
    sys.exit(1 if check_oracle() else 0)
