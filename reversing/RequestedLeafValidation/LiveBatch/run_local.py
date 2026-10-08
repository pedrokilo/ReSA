import json,shutil,sys,hashlib
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[2];sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
shared=runner.probe_configuration
def menu_configuration(data):return shared(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(path):
 if not runner.terminal_event(path):return None
 rows=[json.loads(s) for s in path.read_text().splitlines()];assert not any(r['event'] in ('error','assertion') for r in rows)
 pairs=[r for r in rows if r['event']=='comparison'];assert len(pairs)==1280 and all(r['equal'] for r in pairs)
 assert sum(r['function']=='CDecision::From' for r in pairs)==512
 assert sum(r['function']=='CTaskSimpleGangDriveBy::GetTaskType' for r in pairs)==768
 last=rows[-1];assert last['event']=='complete' and all(last[k] for k in ('state_restored','entries_restored','candidates_restored','tables_restored'))
 return {'passed':True,'pairs':len(pairs),'function_pairs':{'CDecision::From':512,'CTaskSimpleGangDriveBy::GetTaskType':768},'restoration':last}
if __name__=='__main__':
 manifest=json.loads((HERE/'artifacts/manifest.json').read_text(encoding='utf-8'))
 for p,h in manifest['tested_sources'].items():assert runner.digest(ROOT/p)==h
 for p,h in manifest['bank_sources'].items():assert runner.digest(HERE/p)==h
 for ext in ('asi','pdb'):
  source=HERE/f'artifacts/gta_reversed-requested-leaf.{ext}';assert runner.digest(source)==manifest[ext];target=runner.GAME/f'_baseline/gta_reversed-requested-leaf-probe.{ext}'
  if target.exists():shutil.copy2(target,target.with_name(target.name+'.'+runner.digest(target)))
  shutil.copy2(source,target)
 runner.probe_configuration=menu_configuration
 status=runner.run_case('requested-leaf',manifest['asi'],analyze,'CDecision::From / GangDriveBy::GetTaskType',timeout=180,stop_before_analysis=True,unhook='')
 if status==0:
  path=max((runner.GAME/'resultados-requested-leaf').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns);summary=json.loads(path.read_text());summary.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)),log_sha256=runner.digest(path.parent/'requested-leaf-runtime.jsonl') if (path.parent/'requested-leaf-runtime.jsonl').exists() else None,scope=manifest['fixture_scope']);(HERE/'validation-summary.json').write_text(json.dumps(summary,indent=2),encoding='utf-8')
 sys.exit(status)
