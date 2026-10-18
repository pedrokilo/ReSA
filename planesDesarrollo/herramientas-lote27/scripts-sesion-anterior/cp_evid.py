import gzip, json, os, pathlib, shutil, sys

F = pathlib.Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-cped-completion') / sys.argv[1]
D = pathlib.Path(r'D:\Original\gta-reversed-master\reversing\CppTwoLevel5Unified\runs') / ('CPedCompletion-' + sys.argv[1])
D.mkdir(parents=True, exist_ok=True)
for n in ('summary.json', 'RESULTADO.txt'):
    shutil.copy(F / n, D / n)
with open(F / 'comparisons.jsonl', 'rb') as f, open(D / 'comparisons.jsonl.gz', 'wb') as o, gzip.GzipFile(filename='', mode='wb', fileobj=o, mtime=0) as z:
    shutil.copyfileobj(f, z)
s = json.load(open(D / 'summary.json', encoding='utf-8'))
print({k: s.get(k) for k in ('passed', 'pairs', 'all_equal', 'max_error', 'tolerance', 'state_restored')}, os.path.getsize(D / 'comparisons.jsonl.gz'))
