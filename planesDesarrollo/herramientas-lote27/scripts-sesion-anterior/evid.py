import json, os, subprocess, pathlib, collections
p = pathlib.Path('reversing/CppOnlyMigration/approvals.json')
b = p.read_bytes()
bs = b'runs' + bytes([92, 92])
n = b.count(bs)
nb = b.replace(bs, b'runs/')
p.write_bytes(nb)
print('barras corregidas', n)
a = json.loads(nb.decode('utf-8'))
dirs = set()
BACK = chr(92)


def walk(x):
    if isinstance(x, str):
        if 'TwelveLevel6' in x and x.endswith('summary.json'):
            dirs.add(x.replace(BACK, '/').rsplit('/', 1)[0])
    elif isinstance(x, dict):
        for v in x.values():
            walk(v)
    elif isinstance(x, list):
        for v in x:
            walk(v)


walk(a)
print(len(dirs), 'directorios de evidencia referenciados')
others = [o for o in subprocess.run(['git', 'ls-files', '--others', '--exclude-standard'], capture_output=True, text=True).stdout.split('\n') if o]
sel = [o for o in others if any(o.startswith(d + '/') for d in dirs)]
missing = [d for d in dirs if not os.path.isdir(d)]
print('archivos seleccionados', len(sel), '| dirs inexistentes', missing[:5])
open(os.environ['TEMP'] + '/evid_sel.txt', 'w').write('\n'.join(sel))
print(collections.Counter(os.path.basename(o) for o in sel).most_common(8))
