import json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-group-placer-corrected-v2'
generic_analyze=analyzer({'CPedGroupPlacer::PlaceGroup':1536})
def base_analyze(path):
    report=generic_analyze(path)
    if report is None:return None
    import struct
    rows=[json.loads(line) for line in path.read_text().splitlines()]
    pairs=[v for v in rows if v['event']=='comparison']
    if {(v['allocator'],v['case']) for v in pairs}!={(a,n) for a in range(6) for n in range(256)}:raise RuntimeError('Six enum product coverage incomplete')
    summaries=[v for v in rows if v['event']=='function_summary']
    if len(summaries)!=1 or summaries[0]['cases']!=1536 or summaries[0]['failures']!=0 or summaries[0]['subset_passed'] is not True or summaries[0]['allocator4_candidate_replayed'] is not True:raise RuntimeError('Invalid function summary')
    terminal=rows[-1]
    if terminal.get('event')!='complete' or terminal.get('pairs')!=1536 or terminal.get('passed') is not True or terminal.get('state_restored') is not True:raise RuntimeError('Incomplete or unrestored terminal')
    for pair in pairs:
        a,n=pair['allocator'],pair['case'];offset=4*(n%64)
        origin=struct.pack('<fff',float(n%17)-8.,float(n%11)*.25,3.)
        arena=bytearray([205]*512);arena[offset:offset+12]=origin
        expected_trace=[] if a==4 else [dict(route='formation' if a<2 else 'chat' if a<4 else 'random',self_matches=True,type=7+n%10,count=1+n%5,origin_offset=offset,origin_bits=list(struct.unpack('<III',origin)),allocator=a)]
        if pair['accepted'] is not True or pair['equal'] is not True:raise RuntimeError('Rejected comparison')
        for side in ('original','candidate'):
            leg=pair[side]
            if any(leg[k] is not True for k in ('switched','helper_gates','payload_unchanged','raw_al_contract','route_contract')):raise RuntimeError('Gate or contract failed')
            if leg['al']!=(offset if a==4 else n&1) or leg['trace']!=expected_trace or leg['origin_bytes']!=list(arena) or leg['placer_bytes']!=[0]:raise RuntimeError('Explicit ABI/payload/trace mismatch')
    report.update(accepted_and_leg_contracts_checked=True,six_enum_product_checked=True,explicit_fixture_payload_and_trace_checked=True)
    return report
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(base_analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-group-placer-corrected-v2.{ext}';assert runner.digest(source)==manifest[ext]
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
