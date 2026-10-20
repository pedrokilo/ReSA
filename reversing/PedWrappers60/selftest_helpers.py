"""Autoprueba de los ayudantes x87 PedCtl* de source/game_sa/X87Math.cpp frente al modelo del oraculo (fixture_oracle.py), sin el juego: compila X87Math.cpp con sus opciones
(/O2 /Oi /arch:IA32 /fp:fast) junto con selftest_helpers.cpp, ejecuta unas decenas de miles de entradas (aleatorias y de borde) con el FCW de 24 bits y compara bit a bit.
El pow del CRT de Python y el del programa pueden diferir de los del EXE; el banco compara EXE == DLL aparte. Uso:  python -I selftest_helpers.py   (imprime «ok»).
"""
import random
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import fixture_oracle as o  # noqa: E402

VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')
EDGES = [0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x3A83126F, 0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000,
         0x3C23D70A, 0x3C23D70B, 0x3C23D709, 0x3DB851EC, 0x3DB851ED, 0x3DB851EB, 0x43AF0000, 0x3E800000, 0x447A0000, 0x461C4000, 0x40A00000, 0x40D55555, 0x40555555, 0x3FC00000, 0x7F000000, 0x3E4CCCCD]


def rnd(rng):
    r = rng.random()
    if r < 0.15:
        return rng.choice(EDGES)
    if r < 0.8:
        return (rng.getrandbits(1) << 31) | (rng.randrange(0x60, 0xA0) << 23) | rng.getrandbits(23)
    return rng.getrandbits(32)


def build():
    out = Path(tempfile.mkdtemp(prefix='pedwrappers60-helpers-'))
    bat = out / 'b.bat'
    gs = ROOT / 'source/game_sa'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /std:c++latest /EHsc /O2 /Oi /arch:IA32 /fp:fast /MD /I"%s" /Fe:"%s" /Fo:"%s\\\\" "%s" "%s" >"%s" 2>&1\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', gs, out / 't.exe', out, HERE / 'selftest_helpers.cpp', gs / 'X87Math.cpp', out / 'log.txt'), encoding='ascii')
    res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True)
    if not (out / 't.exe').exists():
        raise RuntimeError('No se pudo compilar: ' + (out / 'log.txt').read_text(errors='replace')[-3000:])
    return out / 't.exe'


def run(exe, lines):
    res = subprocess.run([str(exe)], input='\n'.join(lines) + '\n', capture_output=True, text=True)
    return res.stdout.split('\n')[:len(lines)]


def main():
    exe = build()
    rng = random.Random(1234)
    lines, want = [], []
    N = 6000
    fp = o.Fp
    for _ in range(N):
        v = [rnd(rng) for _ in range(3)]
        lim = rnd(rng)
        lines.append('sqmag %x %x %x %x' % (v[0], v[1], v[2], lim))
        want.append('1' if o.lt(o.sumsq(*[fp.bits(x) for x in v]), fp.bits(lim)) else '0')
        z, lm = rnd(rng), rnd(rng)
        lines.append('raised %x %x' % (z, lm))
        want.append('1' if o.lt(o.fadd(fp.bits(z), o.f32c(1.5)), fp.bits(lm)) else '0')
        p = [rnd(rng) for _ in range(3)]
        c = [rnd(rng) for _ in range(3)]
        lines.append('near %x %x %x %x %x %x' % (p[0], p[1], p[2], c[0], c[1], c[2]))
        d = [o.f32(o.fsub(fp.bits(p[i]), fp.bits(c[i]))) for i in range(3)]
        want.append('1' if o.lt(o.sumsq(*d), o.f32c(2500.0)) else '0')
        r1, r2, r3 = rng.randrange(32768), rng.randrange(32768), rng.randrange(32768)
        pos = [rnd(rng) for _ in range(3)]
        lines.append('spawn %x %x %x %x %x %x' % (r1, r2, r3, pos[0], pos[1], pos[2]))
        size = o.fadd(o.fmul(fp.of(r1 & 0x7F), fp.bits(0x3AC49BA6)), fp.bits(0x3E19999A))
        x = o.fadd(o.fmul(fp.of((r2 & 0x7F) - 0x40), fp.bits(0x3BE56042)), fp.bits(pos[0]))
        y = o.fadd(o.fmul(fp.of((r3 & 0x7F) - 0x40), fp.bits(0x3BE56042)), fp.bits(pos[1]))
        zz = o.fadd(fp.bits(pos[2]), o.f32c(1.0))
        want.append('%08x %08x %08x %08x' % (size.to_bits(), x.to_bits(), y.to_bits(), zz.to_bits()))
        ts = rnd(rng)
        lines.append('slow %x %x %x %x' % (v[0], v[1], v[2], ts))
        sq = o.sumsq(*[fp.bits(x_) for x_ in v])
        vv = o.fmul(o.fmul(sq, fp.bits(ts)), fp.bits(ts))
        slow = not (o.gt(vv, o.f32c(0.09)) or o.eq(vv, o.f32c(0.09)))
        want.append('1' if slow else '0')
        mode = rng.randrange(3)
        lines.append('steps %x %x %x %x %x' % (v[0], v[1], v[2], ts, mode))
        dist = o.fmul(o.fsqrt(sq), fp.bits(ts))
        if mode == 0:
            steps = o.ftol_low(o.fceil(o.fmul(dist, o.f32c(5.0))))
        elif mode == 1:
            sc = o.fmul(dist, o.f32c(6.6666665))
            steps = 4 if o.lt(o.fceil(sc), o.f32c(4.0)) else o.ftol_low(o.fceil(o.f32(sc)))
        else:
            sc = o.fmul(dist, o.f32c(3.3333333))
            steps = 2 if o.lt(o.fceil(sc), o.f32c(2.0)) else o.ftol_low(o.fceil(o.f32(sc)))
        want.append('%08x' % steps)
        lines.append('elapsed %x' % ts)
        want.append('%08x' % o.ftol_low(o.fmul(o.fmul(fp.bits(ts), o.f32c(0.02)), o.f32c(1000.0))))
        # decay: solo con paso finito y positivo razonable (el pow de Python es el del CRT)
        st = (rng.randrange(0x70, 0x84) << 23) | rng.getrandbits(23) if rng.random() < 0.9 else rnd(rng)
        lines.append('decay %x %x %x %x' % (v[0], v[1], v[2], st))
        fac = o.decay_factor(fp.bits(st))
        want.append('%08x %08x %08x' % tuple(o.fmul(fp.bits(v[i]), fac).to_bits() for i in range(3)))
    got = run(exe, lines)
    bad = 0
    for i, (g, w, l) in enumerate(zip(got, want, lines)):
        if l.startswith('decay'):
            gv, wv = [int(t, 16) for t in g.split()], [int(t, 16) for t in w.split()]
            ok = all(a == b or (a & 0x7FFFFFFF) > 0x7F800000 and (b & 0x7FFFFFFF) > 0x7F800000 or (abs(a - b) <= 2 and (a ^ b) < 0x80000000) for a, b in zip(gv, wv))
        else:
            ok = g.strip() == w
        if not ok:
            bad += 1
            if bad <= 12:
                print('DIFF', l, '| exe-helper:', g, '| oraculo:', w)
    if bad:
        raise SystemExit('%d diferencias de %d' % (bad, len(lines)))
    print('ok (%d comparaciones)' % len(lines))


if __name__ == '__main__':
    main()
