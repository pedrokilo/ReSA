import json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
PREFIX='requested-wander-remaining'
EXPECTED={'CTaskComplexWanderStandard::Constructor':2048,'CTaskComplexWanderStandard::ScanForStuff':4096}
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(path):
    if not runner.terminal_event(path):return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    if rows[-1]['event']!='complete':raise RuntimeError('Bank failed; preserve all evidence')
    fixtures=[r for r in rows if r['event']=='fixture']
    if len(fixtures)!=1 or fixtures[0]['native_standard_vptr']!=0x85A200:raise RuntimeError('Missing named vtable fixture')
    own=fixtures[0]['dll_standard_vptr']
    counts={f:0 for f in EXPECTED};pairs=[r for r in rows if r['event']=='comparison']
    if len(pairs)!=6144 or [r['pair'] for r in pairs]!=list(range(6144)):raise RuntimeError('Unexpected corpus')
    for r in pairs:
        a,b=r['original'],r['candidate'];counts[r['function']]+=1
        if not (r['equal'] and r['accepted'] and r['payload_equal'] and a['named_vptr_matches'] and b['named_vptr_matches']):raise RuntimeError('Pair failed '+str(r['pair']))
        # Only the explicitly named first DWORD can relocate; never normalize padding or fields.
        if len(a['bytes'])!=56 or len(b['bytes'])!=56 or a['bytes'][4:]!=b['bytes'][4:]:raise RuntimeError('Payload mismatch')
        if int.from_bytes(bytes(a['bytes'][:4]),'little')!=0x85A200 or int.from_bytes(bytes(b['bytes'][:4]),'little')!=own:raise RuntimeError('Named vptr mismatch')
        if 'returned_this' in a:
            if not all(z['returned_this'] and z['gate'] for z in (a,b)):raise RuntimeError('Constructor ABI/gate failed')
        else:
            if a['trace']!=b['trace'] or a['trace']!=r['expected_trace']:raise RuntimeError('Helper trace mismatch')
            if not all(z['table_gate'] and z['entry_gate'] and z['model_matches'] and z['clock_unchanged'] for z in (a,b)):raise RuntimeError('Scan mode/model failed')
    if counts!=EXPECTED:raise RuntimeError('Unexpected function counts')
    end=rows[-1]
    for key in ('state_restored','entries_restored','tables_restored','helpers_restored','globals_restored','ped_fixture_unchanged','coverage'):
        if end[key] is not True:raise RuntimeError('Restoration/coverage failed '+key)
    return {'pairs':6144,'pairs_per_function':counts,'all_equal':True,'state_restored':True,'log_sha256':runner.digest(path),'comparison_scope':'full 52-byte payload plus exact named native/DLL Standard vptr mapping; helper traces independent model'}
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-wander-remaining.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'Wander remaining preflight6144',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('Preserved',dest,'passed',report['passed']);return status
if __name__=='__main__':raise SystemExit(main())
