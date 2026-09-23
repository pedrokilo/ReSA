"""Compare renderer setup, pipeline recognition and clock updates in the engine."""
import json
import shutil
import sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME,digest,run_case,terminal_event
EXPECTED=[256,256,90496,4096,64]


def analyze(path):
    if not terminal_event(path):return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    last=rows[-1]
    if last['event']!='complete' or last['counts']!=EXPECTED or any(last['differences']):
        raise RuntimeError(str(last))
    if not all(last[k] for k in ('counters_valid','inputs_unchanged','player_unchanged','state_restored')):
        raise RuntimeError('State/counter verification failed')
    for fn,n in enumerate(EXPECTED):
        pairs=[r for r in rows if r['event']=='pair' and r['function']==fn]
        if len(pairs)!=n or {r['case'] for r in pairs}!=set(range(n)):
            raise RuntimeError('Incomplete or duplicate cases')
        for r in pairs:
            if (not r['equal'] or r['original']!=r['candidate'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1
                or r.get('returned_original')!=r.get('returned_candidate')):
                raise RuntimeError(f'Difference in {fn}/{r["case"]}')
    hooks=[r for r in rows if r['event']=='hook']
    if len(hooks)!=5 or {r['function'] for r in hooks}!=set(range(5)):
        raise RuntimeError('Missing hook state evidence')
    environment=[r for r in rows if r['event']=='environment']
    if len(environment)!=1 or not environment[0]['hooks_restored']:
        raise RuntimeError('Hook restoration not verified')
    if not all(r['enabled'] and not r['locked'] for r in hooks):
        raise RuntimeError('Final hooks must be enabled and unlocked from startup')
    return dict(pairs=sum(EXPECTED),pairs_per_function=EXPECTED,all_equal=True,
                hook_states=hooks,environment=environment[0],validation=last,log_sha256=digest(path),
                scope='Renderer orchestration; real private RenderWare fixtures with no materials; no drawing or full pipelines')


def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/probe/manifest.json').read_text())
    for ext in ('asi','pdb'):
        src=HERE/f'artifacts/probe/gta_reversed-custom-building-probe.{ext}'
        if digest(src)!=manifest[ext]:raise RuntimeError('Probe hash mismatch')
        shutil.copy2(src,GAME/'_baseline'/src.name)
    def check(path):
        r=analyze(path)
        if r is not None:r['probe_sha256']=manifest['asi']
        return r
    return run_case('custom-building',manifest['asi'],check,'CCustomBuildingRenderer',timeout=300,stop_before_analysis=True,unhook='')


if __name__=='__main__':sys.exit(main())
