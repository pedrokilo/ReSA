"""archive_run.py <Banco> <carpeta de evidencia E:>: archiva summary.json, RESULTADO.txt y comparisons.jsonl.gz (.xz si >90 MB) en
reversing/CppTwelveLevel6Final/runs/<Banco>-<corrida> y borra los crudos propios (E: y runs/ del banco si pasan de 5 MB)."""
import gzip, lzma, shutil, sys, os
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
bank, src = sys.argv[1], Path(sys.argv[2])
RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / f'{bank}-{src.name}'
RUN.mkdir(exist_ok=True)
for n in ('summary.json', 'RESULTADO.txt'):
    if not (RUN / n).exists(): shutil.copyfile(src / n, RUN / n)
raw = src / 'comparisons.jsonl'
if not (RUN / 'comparisons.jsonl.gz').exists() and not (RUN / 'comparisons.jsonl.xz').exists() and raw.exists():
    with open(raw, 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g: shutil.copyfileobj(f, g)
    if (RUN / 'comparisons.jsonl.gz').stat().st_size > 90e6:
        with open(raw, 'rb') as f, lzma.open(RUN / 'comparisons.jsonl.xz', 'wb', preset=6) as g: shutil.copyfileobj(f, g)
        (RUN / 'comparisons.jsonl.gz').unlink()
h = ROOT / 'reversing' / bank / 'runs' / src.name / 'harness.jsonl'
if h.exists() and not (RUN / 'harness.jsonl').exists(): shutil.copyfile(h, RUN / 'harness.jsonl')
if (RUN / 'comparisons.jsonl.gz').exists() or (RUN / 'comparisons.jsonl.xz').exists():
    if raw.exists(): raw.unlink()
    b = ROOT / 'reversing' / bank / 'runs' / src.name / 'comparisons.jsonl'
    if b.exists() and b.stat().st_size > 5e6: b.unlink()
for p in src.glob('*.pdb'): p.unlink()
print('archivado', RUN, sorted(x.name for x in RUN.iterdir()))
