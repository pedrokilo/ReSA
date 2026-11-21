# Anade a docs/hooks.csv las filas de los ganchos nuevos del lote CRT4A-locale (clase ExeCrt, validated=0, tipo S) tras la ultima fila ExeCrt.
import sys
rows = [
    ('UpdateTLocInfo', 0x825456), ('UpdateTLocInfoNoLock', 0x82524D), ('FreeTLocInfo', 0x825183), ('FreeLcTime', 0x82FBE7),
    ('FreeLconvNum', 0x82FE09), ('FreeLconvMon', 0x830068), ('IsctypeMt', 0x82AAE2), ('TolowerLk', 0x825C88), ('ToupperLk', 0x823945),
    ('CrtLCMapStringA', 0x82BB1E), ('CrtGetStringTypeA', 0x830FB8), ('Ansicp', 0x8344CA), ('Convertcp', 0x834513),
    ('WctombMt', 0x8334DC), ('Wctomb', 0x83353C), ('MbtowcMt', 0x83380E), ('Mbtowc', 0x8338CE),
    ('Toupper', 0x823A0E), ('Isalpha', 0x8226D6), ('Isupper', 0x822715), ('Islower', 0x82274F), ('Isalnum', 0x822876),
    ('Atol', 0x822506), ('AsciiStricmp', 0x82ABC0), ('AsciiStrnicmp', 0x836990), ('Stricmp', 0x8229B6), ('Strnicmp', 0x835AC7),
    ('Strlwr', 0x836C58), ('Strupr', 0x836FBA),
]
extra = sys.argv[1:]  # pares Nombre=0xDIR adicionales
for e in extra:
    n, a = e.split('=')
    rows.append((n, int(a, 16)))
p = r'D:\Original\wv-32\docs\hooks.csv'
raw = open(p, 'rb').read().decode('utf-8')
nl = '\r\n' if '\r\n' in raw else '\n'
lines = raw.split(nl)
have = {l.split(',')[2].lower() for l in lines if l.startswith('ExeCrt,')}
last = max(i for i, l in enumerate(lines) if l.startswith('ExeCrt,'))
new = []
for n, a in rows:
    key = '0x%08x' % a
    if key in have:
        continue
    new.append('ExeCrt,%s,0x%08X,1,0,0,S' % (n, a))
lines[last + 1:last + 1] = new
open(p, 'wb').write(nl.join(lines).encode('utf-8'))
print('filas nuevas:', len(new))
