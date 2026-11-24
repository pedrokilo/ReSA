"""Desfase local entre referencia (QuickTime) y candidato (MF) a lo largo de la pista."""
import sys, array
def load(p):
    a = array.array('h'); a.frombytes(open(p,'rb').read()); return a[0::2]
a, b = load(sys.argv[1]), load(sys.argv[2])
W = 1000
for S in range(20000, len(b)-W-5000, 60000):
    seg = b[S:S+W]; best = None
    for lag in range(-6000, 6001, 1):
        s = S+lag
        if s < 0 or s+W > len(a): continue
        d = sum(abs(x-y) for x, y in zip(a[s:s+W:7], seg[::7]))
        if best is None or d < best[0]: best = (d, lag)
    print("pos %7d  desfase %5d  dif %.2f" % (S, best[1], best[0]/(W/7)))
