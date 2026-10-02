"""Bounded in-engine comparison, new game only, with shared backup/restoration."""
import json, shutil, sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
import runtime_runner as runtime

def analyze(path):
    if not runtime.terminal_event(path):return None
    rows=[json.loads(line) for line in path.read_text().splitlines()]
    if any(r['event'] in ('error','assertion') for r in rows):
        raise RuntimeError('El banco informó un error; consultar comparisons.jsonl.')
    pairs=[r for r in rows if r['event']=='comparison']
    ends=[r for r in rows if r['event']=='complete']
    assert len(ends)==1 and ends[0]['state_restored']
    assert len(pairs)==6266==ends[0]['pairs']==ends[0]['expected']
    assert [r['pair'] for r in pairs]==list(range(6266))
    for r in pairs:
        assert r['equal'] and r['original']==r['candidate'] and r['original']['inputs_intact']
        assert r['original_candidate_calls']==0 and r['candidate_calls']==1
    return {'pairs':len(pairs),'all_equal':True,'state_restored':True,'log_sha256':runtime.digest(path)}

def main():
    folder=Path(json.loads((HERE/'latest-probe.json').read_text())['folder'])
    manifest=json.loads((folder/'manifest.json').read_text())
    for ext in ('asi','pdb'):
        source=folder/f'gta_reversed.{ext}'
        assert runtime.digest(source)==manifest[ext]
        shutil.copy2(source,runtime.GAME/f'_baseline/gta_reversed-ped-to-player-probe.{ext}')
    original=runtime.probe_configuration
    def new_game(data):
        text=original(data)
        assert b'SaveGameToLoad = 2\n' in text
        return text.replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = 0\n')
    runtime.probe_configuration=new_game
    code=runtime.run_case('ped-to-player',manifest['asi'],analyze,'CPedToPlayerConversations::Update',timeout=180,unhook='',wait_for_exit=True)
    if not code:
        result=max((runtime.GAME/'resultados-ped-to-player').glob('*/summary.json'),key=lambda p:p.stat().st_mtime)
        summary=json.loads(result.read_text())
        summary.update(build_manifest=str(folder/'manifest.json'),report=str(result),scope='Complete Update control flow with deterministic dependency providers; real engine references, geometry and task objects. No audio playback or combat simulation.')
        (HERE/'validation-summary.json').write_text(json.dumps(summary,indent=2),encoding='utf-8')
    return code

if __name__=='__main__':sys.exit(main())
