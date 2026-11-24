"""Compara dos PCM s16 estereo (referencia QuickTime y Media Foundation) sin numpy: busca el desfase en fotogramas y mide la diferencia."""
import sys, array, math
def load(p):
    a = array.array('h'); a.frombytes(open(p, 'rb').read()); return a[0::2], a[1::2]   # canal izquierdo, derecho
(al, ar), (bl, br) = load(sys.argv[1]), load(sys.argv[2])   # a = referencia (QuickTime), b = candidato (MF)
print("fotogramas ref=%d cand=%d" % (len(al), len(bl)))
W, S = 1500, 30000
seg = bl[S:S+W]
best = None
for lag in range(-2500, 2501):
    s = S + lag
    if s < 0 or s + W > len(al): continue
    d = sum(abs(x - y) for x, y in zip(al[s:s+W], seg))
    if best is None or d < best[0]: best = (d, lag)
lag = best[1]
print("mejor desfase (ref[i+lag] = cand[i]): %d  |dif| medio en la ventana: %.3f" % (lag, best[0] / W))
n = min(len(al) - max(lag, 0), len(bl) - max(-lag, 0)) - 2000
oa = max(lag, 0); ob = max(-lag, 0)
sd = 0.0; ss = 0.0; mx = 0; eq = 0; sad = 0
for ch_a, ch_b in ((al, bl), (ar, br)):
    for x, y in zip(ch_a[oa:oa+n], ch_b[ob:ob+n]):
        d = x - y; sd += d*d; ss += x*x; sad += abs(d); mx = max(mx, abs(d)); eq += (d == 0)
cnt = 2*n
print("tramo: %d fotogramas; |dif| medio=%.3f maximo=%d; muestras iguales=%.2f%%; RMS dif=%.3f; RMS senal=%.1f; SNR=%.1f dB" % (n, sad/cnt, mx, 100.0*eq/cnt, math.sqrt(sd/cnt), math.sqrt(ss/cnt), 10*math.log10(ss/max(sd, 1e-9))))
