"""Resume las diferencias de estado x87 de una corrida de rehearsal.exe (jsonl): bits distintos de FSW y registros muertos distintos (T-n), agrupados por la traza del EXE.
uso: python analyze_diff.py corrida.jsonl [maximo de casos a imprimir]"""
import sys, json, collections
NAMES = {0: 'IE', 1: 'DE', 2: 'ZE', 3: 'OE', 4: 'UE', 5: 'PE', 6: 'SF', 7: 'ES', 8: 'C0', 9: 'C1', 10: 'C2', 14: 'C3', 15: 'B'}
def bits(a, b):
    d = a ^ b
    return [NAMES.get(i, 'bit%d' % i) for i in range(16) if d >> i & 1 and not (11 <= i <= 13)]
def regs(a, b):
    out = []
    for k in range(8):
        if a[k * 20:(k + 1) * 20] != b[k * 20:(k + 1) * 20]:
            out.append('T-%d' % (8 - k))      # ST(k) = T-(8-k)
    return out
def sig(trace):
    names = [e[0] for e in trace]
    return ','.join(names[:12])
rows = [json.loads(l) for l in open(sys.argv[1])]
limit = int(sys.argv[2]) if len(sys.argv) > 2 else 12
groups = collections.Counter(); shown = 0
fsw_bits = collections.Counter(); reg_slots = collections.Counter()
for r in rows:
    if r['event'] != 'comparison' or r['equal']:
        continue
    o, c = r['original']['x87'], r['candidate']['x87']
    fb = bits(o['fsw'], c['fsw']); rg = regs(o['raw_registers'], c['raw_registers'])
    for b in fb: fsw_bits[b] += 1
    for s in rg: reg_slots[s] += 1
    key = (tuple(fb), tuple(rg))
    groups[key] += 1
    if shown < limit and (fb or rg):
        shown += 1
        p = r['profile']
        print('caso %d  fsw:%s  regs:%s  escenario=%s/%s  EXE fsw=%04x DLL fsw=%04x' % (r['case'], fb, rg, p['scenario'], p['directed'], o['fsw'], c['fsw']))
        print('    traza EXE:', sig(r['original']['trace']))
print('bits FSW distintos:', dict(fsw_bits))
print('registros muertos distintos:', dict(reg_slots))
print('grupos:')
for k, v in groups.most_common(15): print('  ', v, k)
