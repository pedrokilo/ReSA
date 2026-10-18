import csv, glob, json, os, re

os.chdir(r'D:\Original\gta-reversed-master')
ids = json.load(open(os.environ['TEMP'] + '/lote25_ids.json'))
hooks = {(r['class'], r['fn_name']): r['address'] for r in csv.DictReader(open('docs/hooks.csv', encoding='utf-8-sig'))}
files = [f.replace(chr(92), '/') for f in glob.glob('source/**/*.cpp', recursive=True)]
texts = {}


def source_of(fn):
    pat = re.compile(r'RH_Scoped\w*\(\s*' + re.escape(fn) + r'\s*,')
    hits = []
    for f in files:
        if f not in texts:
            texts[f] = open(f, encoding='utf-8', errors='replace').read()
        if pat.search(texts[f]):
            hits.append(f)
    return hits[0] if hits else None


hook_lines = []
src_lines = []
missing = []
for bank, ident in ids:
    cls, fn = ident.split('::', 1)
    addr = hooks.get((cls, fn))
    if addr is None:
        missing.append(ident)
        continue
    src = source_of(fn)
    if src is None:
        missing.append(ident + ' (sin fuente)')
        src = 'source/game_sa/RenderWare/rw/rwcore.cpp'
    hook_lines.append(f"    ('{cls}', '{fn}', '{addr}', '{ident}', '{bank}'),")
    src_lines.append(f"    '{ident}': '{src}',")
print('sin fila/fuente:', missing)
p = r'reversing\CppTwelveLevel6Study\register_hooks_nuevos.py'
t = open(p, 'rb').read().decode('utf-8')
nl = '\r\n' if '\r\n' in t else '\n'
anchor = "    ('Input', 'diPadInit', '0x007485C0', 'WinInput::diPadInit', 'Dependencies24B')," + nl
assert anchor in t
t = t.replace(anchor, anchor + nl.join(hook_lines) + nl, 1)
anchor2 = "    'Win::__MainWndProc': 'source/app/platform/win/WndProc.cpp', 'WinInput::diPadInit': 'source/app/platform/win/WinInput.cpp'," + nl
assert anchor2 in t
t = t.replace(anchor2, anchor2 + nl.join(src_lines) + nl, 1)
open(p, 'wb').write(t.encode('utf-8'))
print(len(hook_lines), 'entradas añadidas')
