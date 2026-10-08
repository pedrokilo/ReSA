import json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-group-placer-corrected'
base_analyze=analyzer({'CPedGroupPlacer::PlaceGroup':1536})
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(base_analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-group-placer-corrected.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],base_analyze,'Group dispatcher1536',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('Preserved',dest,'passed',report['passed']);return status
if __name__=='__main__':raise SystemExit(main())
