# Solapes entre las citas por direccion (uses.json + anchos por ensamblador, de la auditoria estatica) y las 32 ranuras F (DataBssF), que la auditoria anterior no incluia como objetos.
import json, re, glob, collections, bisect, pickle, sys, os
sys.path.insert(0, r'D:\Original\sim_out\aud2'); os.chdir(r'D:\Original\sim_out\aud2')
from parse_asm import parse
uses = json.load(open('uses.json')); tsz = json.load(open('tsizes.json'))
asz = collections.defaultdict(set)
for f in ['sz_all.asm'] + glob.glob('asm/*.asm'):
    for a, s, t, l in parse(f):
        if a and s: asz[a].add(s)
def width(u):
    k = u['kind']; bt = u.get('bt')
    if k in ('StaticRef', 'ScopedStaticRef') and u['a'] in asz: return max(asz[u['a']]), 'asm'
    if bt and bt in tsz: return tsz[bt], 'type'
    if k.startswith('fn:') and k[3:] in ('K', 'constant', 'Legible'): return 8, 'fn'
    return 4, 'guess'
SL = {0x8E4004: 4, 0x8E4008: 4, 0x8E48AC: 36, 0x965560: 4, 0x96A7D8: 4, 0x96EA98: 32, 0x96EF90: 64, 0x96EFD0: 64, 0xA8B430: 21504, 0xA951F8: 1408, 0xB5F87C: 4, 0xB610F0: 8, 0xB6131C: 4,
      0xB73C98: 1408, 0xBA82D8: 8, 0xBAA350: 4, 0xBAD3EC: 12, 0xBBCDC8: 32, 0xBD0B74: 4, 0xBD0B78: 204800, 0xC02C28: 64, 0xC102A0: 1408, 0xC18D50: 100, 0xC1C26C: 4, 0xC1C270: 4,
      0xC1CB30: 4, 0xC1CB34: 4, 0xC1CD98: 20, 0xC6A170: 4, 0xC718B0: 420, 0xC80434: 16, 0xC80444: 16}
for u in uses:
    if u['f'].startswith('game_sa/DatosExe/'): continue
    w, src = width(u); a = u['a']
    for sa, ss in SL.items():
        if a < sa + ss and sa < a + w:
            print('0x%X (w=%d %s, %s) %s:%d  ~ ranura 0x%X (%d)  | %s' % (a, w, src, u['kind'], u['f'], u['l'], sa, ss, u['code'][:110]))
