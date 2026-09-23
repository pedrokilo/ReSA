"""Observe real startup mouse acquisition and 120 successful input updates."""
import json
import shutil
import sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME,digest,run_case,terminal_event

def analyze(path):
    if not terminal_event(path):return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    last=rows[-1]
    initial=[r for r in rows if r['event']=='initial_acquire' and not r['ok']]
    success=[r for r in rows if r['event'] in ('read','retry_read') and r['ok']]
    if (last['event']!='complete' or not initial or not success or last['delivered']!=120
        or last['read_successes']<120 or success[0]['ms']<initial[0]['ms']):
        raise RuntimeError(str(last))
    return dict(validation=last,first_failure=initial[0],first_success=success[0],
                log_sha256=digest(path),scope='Natural acquisition and delivered mouse states; no synthetic movement or clicks')

def main():
    manifest=json.loads((HERE/'artifacts/probe/manifest.json').read_text())
    for ext in ('asi','pdb'):
        src=HERE/f'artifacts/probe/gta_reversed-mouse-recovery-probe.{ext}'
        assert digest(src)==manifest[ext]
        shutil.copy2(src,GAME/'_baseline'/src.name)
    def check(path):
        result=analyze(path)
        if result is not None:result['probe_sha256']=manifest['asi']
        return result
    return run_case('mouse-recovery',manifest['asi'],check,'Recuperacion natural del raton',
                    timeout=180,stop_before_analysis=True,unhook='')

if __name__=='__main__':sys.exit(main())
