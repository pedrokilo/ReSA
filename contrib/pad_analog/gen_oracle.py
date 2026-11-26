# -*- coding: utf-8 -*-
"""Genera los vectores de prueba del banco bench_core.cpp: entradas aleatorias y de borde evaluadas con la rutina de GInputSA.asi (ginput_read_oracle.py: instruccion a instruccion,
sin ejecutar el .asi). Uso: python gen_oracle.py <salida.txt> [semilla]"""
import random, struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ginput_read_oracle import run_xinput, run, DEFAULT


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def pct_to_float(pct, mx):
    v = f32(pct * 0.009999999776482582)
    return min(max(v, 0.0), mx)


def main(out, seed=12345):
    rnd = random.Random(seed)
    lines = []
    raws_edge = [0, 1, -1, 2, -2, 32767, -32768, -32767, 16384, -16384, 8000, -8000, 7864, 7865, 7866, -7864, -7865, 8847, 8848, 8849, 24575, -24575]
    dz_set = [0, 1, 10, 24, 27, 30, 50, 74, 75, 90]
    sens_set = [0, 10, 25, 50, 75, 99, 100, 101, 125, 150, 199, 200, 230]
    n = 0
    for i in range(6000):
        dzl, dzr = rnd.choice(dz_set), rnd.choice(dz_set)
        sl, sr = rnd.choice(sens_set), rnd.choice(sens_set)
        flags = rnd.getrandbits(6)

        def rv():
            c = rnd.random()
            if c < 0.25:
                return rnd.choice(raws_edge)
            if c < 0.45:
                return rnd.randint(-9000, 9000)
            return rnd.randint(-32768, 32767)
        raw = dict(lx=rv(), ly=rv(), rx=rv(), ry=rv(), lt=rnd.choice([0, 29, 30, 31, 32, 100, 254, 255, rnd.randint(0, 255)]), rt=rnd.randint(0, 255))
        cfg = dict(ldz=pct_to_float(dzl, 0.75), rdz=pct_to_float(dzr, 0.75), lsens=pct_to_float(sl, 2.0), rsens=pct_to_float(sr, 2.0),
                   invLX=flags & 1, invLY=(flags >> 1) & 1, swapL=(flags >> 2) & 1, invRX=(flags >> 3) & 1, invRY=(flags >> 4) & 1, swapR=(flags >> 5) & 1)
        o = run_xinput(0, raw, cfg)
        lines.append('S %d %d %d %d %d %d %d %d %d %d %d | %d %d %d %d %d %d' % (dzl, dzr, sl, sr, flags, raw['lx'], raw['ly'], raw['rx'], raw['ry'], raw['lt'], raw['rt'],
                                                                                o['lx'], o['ly'], o['rx'], o['ry'], o['l2'], o['r2']))
        n += 1
    # ruta SCP: botones analogicos y gatillos analogicos (0x10007550)
    for i in range(1500):
        val = rnd.choice([1, 10, 25, 50, 75, 99, 100, 101, 150, 200, rnd.randint(1, 120)])
        factor = max(f32(25500.0 / val), 255.0)
        b = f32(rnd.choice([0.0, 1.0, 0.5, 0.25, 0.01, rnd.random(), rnd.random()]))
        t = f32(rnd.choice([0.0, 1.0, 0.11764705926179886, 0.12, 0.5, rnd.random()]))
        o = run(0, dict(cross=b, sq=b, l1=b, l2=t, r2=t), dict(face=factor))
        lines.append('F %d %s %s | %d %d %d %d' % (val, struct.pack('<f', b).hex(), struct.pack('<f', t).hex(), o['cross'], o['square'], o['l1'], o['l2']))
        assert o['l2'] == o['r2']
    # factor 0 (INI FaceButtonsSensitivity=0 -> infinito) : cualquier pulsacion cuenta entera y 0*inf=NaN -> 0
    for b in (0.0, 0.001, 0.5, 1.0):
        o = run(0, dict(cross=b), dict(face=float('inf')))
        lines.append('I %s | %d' % (struct.pack('<f', b).hex(), o['cross']))
    open(out, 'w').write('\n'.join(lines) + '\n')
    print(len(lines), 'vectores ->', out)


if __name__ == '__main__':
    main(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else 12345)
