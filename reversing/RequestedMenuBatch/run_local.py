import json,shutil,sys,os
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
from probe_case import analyzer
cases={'constructor':('small-pending',{'CPedDamageResponseCalculator::Constructor':1024}),'visibility':('interesting-visibility-pending',{'CInterestingEvents::InvalidateNonVisibleEvents':384}),'formation':('formation-cover-pending',{'CFormation::FindCoverPointsBehindBox':256}),'formation-expanded':('formation-cover-expanded',{'CFormation::FindCoverPointsBehindBox':4096}),'wander':('requested-wander-type',{'CTaskComplexWanderStandard::GetWanderType':1280})}
original_cfg=runner.probe_configuration
runner.probe_configuration=lambda data:original_cfg(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
if __name__=='__main__':
 name=sys.argv[1];prefix,expected=cases[name];manifest=json.loads((HERE/'artifacts/manifest.json').read_text(encoding='utf-8'))
 for p,h in manifest['tested_sources'].items():assert runner.digest(ROOT/p)==h
 for p,h in manifest['bank_sources'].items():assert runner.digest(ROOT/p)==h
 for ext in ('asi','pdb'):
  source=HERE/f'artifacts/gta_reversed-requested-menu.{ext}';assert runner.digest(source)==manifest[ext]
  immutable=HERE/f'artifacts/{manifest[ext]}.{ext}'
  if not immutable.exists():shutil.copy2(source,immutable)
  target=runner.GAME/f'_baseline/gta_reversed-{prefix}-probe.{ext}'
  assert target.resolve().is_relative_to((runner.GAME/'_baseline').resolve())
  if target.exists():
   backup=target.with_name(target.name+'.previous-'+runner.digest(target))
   if not backup.exists():target.replace(backup)
   else:target.unlink()
  try:os.link(immutable,target)
  except OSError:shutil.copy2(immutable,target)
 status=runner.run_case(prefix,manifest['asi'],analyzer(expected),name,timeout=180,stop_before_analysis=True,unhook='Win/Win/NOTSA_WinMain')
 run=max((runner.GAME/f'resultados-{prefix}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
 dest=HERE/'runs'/name/run.name;dest.mkdir(parents=True,exist_ok=True)
 for filename in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
  if (run/filename).exists():shutil.copy2(run/filename,dest/filename)
 report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)),log_sha256=runner.digest(dest/'comparisons.jsonl') if (dest/'comparisons.jsonl').exists() else None);(dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('PRESERVED',dest,'passed',report['passed']);sys.exit(status)
