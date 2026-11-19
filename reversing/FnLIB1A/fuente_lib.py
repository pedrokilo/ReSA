"""Tabla de funciones de la biblioteca compilada (zlib 1.1.3 + libpng 1.0.12) a partir de los .obj y .asm."""
import struct, re, os, glob, collections
def coff_funcs(path):
    d = open(path, 'rb').read()
    mach, nsec, ts, symptr, nsym, optsz, ch = struct.unpack_from('<HHIIIHH', d, 0)
    secs = []
    for i in range(nsec):
        o = 20 + optsz + i*40
        name = d[o:o+8].rstrip(b'\0').decode(errors='replace')
        vsz, va, rsz, rptr = struct.unpack_from('<IIII', d, o+8)
        flags = struct.unpack_from('<I', d, o+36)[0]
        secs.append((name, rsz, rptr, flags))
    strtab = d[symptr+nsym*18:]
    out = {}
    i = 0
    while i < nsym:
        o = symptr + i*18
        nm = d[o:o+8]
        if nm[:4] == b'\0\0\0\0':
            off = struct.unpack_from('<I', nm, 4)[0]
            name = strtab[off:strtab.index(b'\0', off)].decode()
        else:
            name = nm.rstrip(b'\0').decode()
        val, sn, typ, sc, na = struct.unpack_from('<IhHBB', d, o+8)
        if typ == 0x20 and sn > 0 and sc in (2, 3):
            out[name] = (sn-1, val)
        i += 1 + na
    res = {}
    for name, (si, val) in out.items():
        sname, rsz, rptr, flags = secs[si]
        res[name] = dict(size=rsz, section=sname, bytes=d[rptr:rptr+rsz], static=False)
    return res
def parse_asm(path):
    txt = open(path, encoding='latin-1').read().splitlines()
    strs = {}
    lab = None
    for l in txt:
        m = re.match(r"(\?\?_C@\S+|\$SG\d+)\s+DB\s+(.*)", l)
        if m:
            lab = m.group(1); strs[lab] = ''; body = m.group(2)
        elif lab and re.match(r"\s+DB\s+", l):
            body = l.split('DB', 1)[1]
        else:
            lab = None; continue
        i = 0
        while i < len(body):
            c = body[i]
            if c == "'":
                j = i+1; q = ''
                while j < len(body):
                    if body[j] == "'":
                        if body[j+1:j+2] == "'": q += "'"; j += 2; continue
                        break
                    q += body[j]; j += 1
                strs[lab] += q; i = j+1
            elif c == ';': break
            else:
                m2 = re.match(r"([0-9][0-9a-fA-F]*)H", body[i:])
                if m2:
                    v = int(m2.group(1), 16)
                    if v: strs[lab] += chr(v)
                    i += len(m2.group(0))
                else: i += 1
    funcs = {}
    cur = None
    for l in txt:
        m = re.match(r"(\S+)\s+PROC", l)
        if m:
            cur = m.group(1); funcs[cur] = dict(calls=[], strings=[], imms=[], n=0, x87=0, ind=0); continue
        if re.match(r"(\S+)\s+ENDP", l): cur = None; continue
        if cur:
            f = funcs[cur]
            if re.match(r"\s+\w+\s", l) or re.match(r"\s+ret", l): f['n'] += 1
            if re.match(r"\s+f[a-z0-9]+\s", l): f['x87'] += 1
            m = re.match(r"\s+call\s+(?:DWORD PTR\s+)?(\S+)", l)
            if m: f['calls'].append(m.group(1))
            for s in re.findall(r"OFFSET (\?\?_C@\S+|\$SG\d+)", l):
                f['strings'].append(strs.get(s, ''))
    return funcs
def load(outdir):
    allf = {}
    for obj in sorted(glob.glob(os.path.join(outdir, '*.obj'))):
        base = os.path.splitext(os.path.basename(obj))[0]
        cf = coff_funcs(obj)
        af = parse_asm(os.path.join(outdir, base + '.asm'))
        for name, c in cf.items():
            a = af.get(name, {})
            c.update(calls=a.get('calls', []), strings=a.get('strings', []), file=base, x87=a.get('x87', 0), ncall=len(a.get('calls', [])))
            allf[(base, name)] = c
    return allf
if __name__ == '__main__':
    import sys
    f = load(sys.argv[1])
    print(len(f), sum(x['size'] for x in f.values()))
