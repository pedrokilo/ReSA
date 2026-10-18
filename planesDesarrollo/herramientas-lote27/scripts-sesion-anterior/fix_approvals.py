import json, os, pathlib, re

p = pathlib.Path('reversing/CppOnlyMigration/approvals.json')
t = p.read_bytes().decode('utf-8')
BS = chr(92)
valid = '"' + BS + '/bfnrtu'
bad = [i for i, c in enumerate(t) if c == BS and (i + 1 >= len(t) or t[i + 1] not in valid)]
# una barra invertida valida escapada ("\\") se salta: contar pares
bad = []
i = 0
while i < len(t):
    if t[i] == BS:
        nxt = t[i + 1] if i + 1 < len(t) else ''
        if nxt in valid:
            i += 2
            continue
        bad.append(i)
    i += 1
print('escapes invalidos:', len(bad))
if bad:
    print(repr(t[bad[0] - 30:bad[0] + 30]))
fixed = t.replace('/' + BS, '/')
# volver a comprobar
bad2 = []
i = 0
while i < len(fixed):
    if fixed[i] == BS:
        nxt = fixed[i + 1] if i + 1 < len(fixed) else ''
        if nxt in valid:
            i += 2
            continue
        bad2.append(i)
    i += 1
print('tras reparar:', len(bad2))
a = json.loads(fixed)
print('entradas', len(a))
miss = [k for k, v in a.items() if 'audit' in v and not os.path.exists(v['audit'])]
print('audit inexistentes:', len(miss), miss[:3])
if not bad2:
    p.write_bytes(fixed.encode('utf-8'))
    print('escrito')
