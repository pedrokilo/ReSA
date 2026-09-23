"""Differential regression for named-animation task initialization and offsets."""
import argparse
import json
import shutil
import sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME,digest,run_case,terminal_event

def analyze(path, diagnose_baseline=False):
    if not terminal_event(path):return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    r=rows[-1]
    if r['event']!='complete' or not r['counters_valid'] or not r['state_restored']:
        raise RuntimeError(str(r))
    expected={'constructor':8192,'offset':512,'process':128}
    for event,count in expected.items():
        cases=[row for row in rows if row['event']==event]
        if len(cases)!=count or r[event+'_pairs']!=count:
            raise RuntimeError(f'Incomplete {event} evidence')
        if not diagnose_baseline and any(not row['equal'] for row in cases):
            raise RuntimeError(f'Unexpected {event} difference')
    if r['offset_differences']:
        raise RuntimeError(str(r))
    if diagnose_baseline:
        if not r['constructor_differences'] or not r['process_differences']:
            raise RuntimeError('Baseline did not reproduce initialization and position discrepancies')
    elif r['constructor_differences'] or r['process_differences']:
        raise RuntimeError(str(r))
    return dict(scope='Baseline discrepancy reproduction' if diagnose_baseline else 'Constructor and offset differential comparison',validation=r,log_sha256=digest(path),pairs=sum(expected.values()))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--diagnose-baseline',action='store_true')
    args=parser.parse_args()
    manifest=json.loads((HERE/'artifacts/probe/manifest.json').read_text())
    for ext in ('asi','pdb'):
        src=HERE/f'artifacts/probe/gta_reversed-namedanim-init-probe.{ext}'
        assert digest(src)==manifest[ext]
        shutil.copy2(src,GAME/f'_baseline/{src.name}')
    def check(path):
        result=analyze(path,args.diagnose_baseline)
        if result is not None:result['probe_sha256']=manifest['asi']
        return result
    return run_case('namedanim-init',manifest['asi'],check,'Inicializacion de animaciones por nombre',timeout=180,stop_before_analysis=True,unhook='')

if __name__=='__main__':sys.exit(main())
