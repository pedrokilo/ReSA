# Diferencias por escena de los volcados de memoria (CColourSet actual, gamma, CMenuManager) entre dos ejecuciones.
# Uso: python -I memdiff.py <dir_a> <dir_b> [escena ...]   (a = original, b = standalone o ASI)
import sys, os, json, struct
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui); sys.path.insert(0, os.path.join(aqui, '..', 'FnTraf'))
import nubes_matriz as N
CAMPOS = sorted(((o, n, t) for n, (o, t) in N.campos().items()))
def nombre(off):
    prev = None
    for o, n, t in CAMPOS:
        if o <= off: prev = (o, n, t)
    return prev
def cargar(d):
    r = {}
    for l in open(os.path.join(d, 'resultados.jsonl'), encoding='utf-8'):
        j = json.loads(l)
        if 'escena' in j and 'mem' in j: r[j['escena']] = j['mem']
    return r
def val(b, o, t):
    f = {'float': '<f', 'uint16': '<H', 'int16': '<h', 'int32': '<i', 'uint32': '<I', 'uint8': '<B', 'int8': '<b'}[t]; sz = struct.calcsize(f)
    return struct.unpack_from(f, b, o)[0]
def main():
    A, B = cargar(sys.argv[1]), cargar(sys.argv[2]); sel = sys.argv[3:] or sorted(set(A) & set(B))
    for esc in sel:
        if esc not in A or esc not in B: continue
        print('==', esc)
        for k in ('timecycle', 'gamma', 'menu'):
            a, b = bytes.fromhex(A[esc][k]), bytes.fromhex(B[esc][k]); dif = [i for i in range(min(len(a), len(b))) if a[i] != b[i]]
            print(' ', k, 'bytes distintos:', len(dif))
            if k == 'timecycle':
                vistos = set()
                for i in dif:
                    c = nombre(i)
                    if not c or c[1] in vistos: continue
                    vistos.add(c[1]); print('    %s @+0x%X orig=%s otro=%s' % (c[1], c[0], round(val(a, c[0], c[2]), 3), round(val(b, c[0], c[2]), 3)))
            elif dif: print('    offsets', [hex(i) for i in dif[:24]], 'orig', [a[i] for i in dif[:12]], 'otro', [b[i] for i in dif[:12]])
if __name__ == '__main__': main()
