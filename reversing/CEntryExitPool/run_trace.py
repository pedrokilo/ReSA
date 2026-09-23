import json,sys,shutil,os,argparse
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import run_case,digest,terminal_event,GAME
parser=argparse.ArgumentParser();parser.add_argument('--native',action='store_true');args=parser.parse_args()
if args.native:os.environ['ENEX_NATIVE']='1'
else:os.environ.pop('ENEX_NATIVE',None)
manifest=json.loads((HERE/'artifacts/trace/trace-manifest.json').read_text())
for ext in ('asi','pdb'):
 src=HERE/f'artifacts/trace/gta_reversed-entryexit-trace-probe.{ext}'
 assert digest(src)==manifest[ext]
 shutil.copy2(src,GAME/f'_baseline/gta_reversed-entryexit-trace-probe.{ext}')
def analyze(path):
 if not terminal_event(path):return None
 rows=[json.loads(l) for l in path.read_text().splitlines()]
 if rows[-1]['event']!='complete':raise RuntimeError(str(rows[-1]))
 return dict(trace=rows[-1],scope='Diagnostico; no valida equivalencia')
sys.exit(run_case('entryexit-trace',manifest['asi'],analyze,'Diagnostico CEntryExit',timeout=180,stop_before_analysis=True,unhook=''))
