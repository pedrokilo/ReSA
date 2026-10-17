import re, sys
t = open(sys.argv[1]).read()
name = sys.argv[2]
m = re.search(re.escape(name) + r'[^\n]* PROC.*?ENDP', t, re.S)
if not m:
    print('no encontrado', name)
    sys.exit(1)
out = []
for l in m.group(0).split('\n'):
    ls = l.strip()
    if not ls or ls.startswith(';'):
        continue
    out.append(l.replace('\t', ' '))
print('\n'.join(out))
