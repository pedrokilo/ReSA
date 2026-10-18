import re, glob, os, json

os.chdir(r'D:\Original\gta-reversed-master')
t = open('source/game_sa/X87Math.cpp', encoding='utf-8').read()
h = open('source/game_sa/X87Math.h', encoding='utf-8').read()
decl = set(re.findall(r'\b(\w+)\s*\(', h))
defs = [(m.start(), m.group(1)) for m in re.finditer(
    r'^(?:static |__declspec\(noinline\) |inline |extern )*[\w:<>\*& ]+?\b(\w+)\s*\([^;{]*\)\s*(?:noexcept\s*)?\{', t, re.M)]
kw = {'if', 'while', 'switch', 'for', 'return', 'Push', 'sizeof'}
body = {}
for i, (s, n) in enumerate(defs):
    if n in kw:
        continue
    e = defs[i + 1][0] if i + 1 < len(defs) else len(t)
    body[n] = body.get(n, '') + t[s:e]
roots = {'Ftol2RegsCore', 'Ftol2RegsDblCpp', 'Ftol2RegsCpp'}
S = set(roots)
ch = True
while ch:
    ch = False
    for n, b in body.items():
        if n in S:
            continue
        if any(re.search(r'\b' + r + r'\b', b) for r in S):
            S.add(n)
            ch = True
exp = S & decl
print(len(S), 'alcanzan Ftol2;', len(exp), 'declarados en X87Math.h')
files = {}
for f in glob.glob('source/**/*.cpp', recursive=True):
    if f.endswith('X87Math.cpp'):
        continue
    s = open(f, encoding='utf-8', errors='replace').read()
    hit = [x for x in exp if re.search(r'\b' + x + r'\b', s)]
    if hit:
        files[f.replace(chr(92), '/')] = hit
for f, hh in sorted(files.items()):
    print(f, len(hh))
json.dump(files, open(os.environ['TEMP'] + '/ftol_files.json', 'w'))
