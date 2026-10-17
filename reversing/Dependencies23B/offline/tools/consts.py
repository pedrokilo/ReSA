import re, struct, sys
exe = open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe', 'rb').read()
pe = struct.unpack_from('<I', exe, 0x3c)[0]
nsec = struct.unpack_from('<H', exe, pe + 6)[0]
opt = struct.unpack_from('<H', exe, pe + 20)[0]
base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
o = pe + 24 + opt
secs = []
for i in range(nsec):
    vs, va, rs, rp = struct.unpack_from('<IIII', exe, o + 8 + i * 40)
    secs.append((va, vs, rp, rs))
def off(a):
    r = a - base
    for va, vs, rp, rs in secs:
        if va <= r < va + max(vs, rs):
            return rp + r - va if r - va < rs else None
seen = {}
for l in open('fc.dis'):
    m = re.search(r'(dword|qword) ptr \[(0x[0-9a-f]+)\]', l)
    if m:
        a = int(m.group(2), 16)
        seen.setdefault(a, set()).add(m.group(1))
for a in sorted(seen):
    f = off(a)
    kinds = ','.join(sorted(seen[a]))
    if f is None:
        print('%06x %-12s (bss/global)' % (a, kinds))
        continue
    d = exe[f:f + 8]
    fl = struct.unpack('<f', d[:4])[0]
    db = struct.unpack('<d', d)[0]
    print('%06x %-12s dword=%08x float=%r  qword=%016x double=%r' % (a, kinds, struct.unpack('<I', d[:4])[0], fl, struct.unpack('<Q', d)[0], db))
