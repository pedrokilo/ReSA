import json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-geometry-wrappers-five-v4'
NAMES=['ComputeEntityBoundingBoxCorners','ComputeEntityBoundingBoxCentreUncachedAll','ComputeEntityBoundingBoxPlanesUncachedAll','ComputeEntityBoundingBoxSegmentPlanes','ComputeEntityBoundingBoxSegmentPlanesUncachedAll']
generic_analyze=analyzer({'CPedGeometryAnalyser::'+n:1024 for n in NAMES})
def base_analyze(path):
    report=generic_analyze(path)
    if report is None:return None
    import struct
    rows=[json.loads(v) for v in path.read_text().splitlines()]
    fixture=[v for v in rows if v['event']=='fixture']
    if len(fixture)!=1 or fixture[0]['rand_calls']!=1 or fixture[0]['rand_controlled'] is not True or any(fixture[0][k] is not True for k in ('callsite_proven','general_private_gates','general_restored')):raise RuntimeError('Typed fixture uncontrolled')
    pairs=[v for v in rows if v['event']=='comparison']
    if {(v['wrapper'],v['case']) for v in pairs}!={(k,n) for k in range(5) for n in range(1024)}:raise RuntimeError('Wrapper product coverage incomplete')
    for v in pairs:
        k,n=v['wrapper'],v['case'];z=-0. if n%127==0 else float(n%101-50)*.25
        bits=lambda x:struct.unpack('<I',struct.pack('<f',x))[0]
        corners=b''.join(struct.pack('<fff',float(n%31-15)+i*.25,float(n%19-9)-i*.5,float(n%13-6)) for i in range(4))
        centre=struct.pack('<fff',float(n%17-8),float(n%23-11),float(n%11-5))
        expected=[dict(helper='corners',z_bits=bits(z),entity_identity=True,external_output=k==0,corners_payload=list(corners))]
        if k in (1,3,4):expected.append(dict(helper='centre',z_bits=bits(z),corners_identity=True,corners_payload=list(corners),external_output=k==1,centre_payload=list(struct.unpack('<III',centre))))
        if k==2:expected.append(dict(helper='planes',z_bits=bits(z),corners_identity=True,corners_payload=list(corners),external_outputs=True))
        if k in (3,4):expected.append(dict(helper='segment',corners_identity=True,centre_identity=True,corners_payload=list(corners),centre_payload=list(struct.unpack('<III',centre)),external_outputs=True))
        arena=bytearray([205]*160);arena[16:64]=bytes(48)
        if k==0:arena[16:64]=corners
        elif k==1:arena[16:28]=centre
        else:
            arena[16:64]=b''.join(struct.pack('<fff',float(n%29+i),float(n%7-i),float(i)) for i in range(4))
            arena[80:96]=b''.join(struct.pack('<f',float(n%41-i)*.125) for i in range(4))
        for side,index in [('original',0),('candidate',1)]:
            leg=v[side]
            if any(leg[a] is not True for a in ('entity_unchanged','gates','contracts','return_role')):raise RuntimeError('False leg contract')
            if leg['trace']!=expected or leg['arena']!=list(arena):raise RuntimeError('Explicit trace or arena mismatch')
            pointers=v['raw_pointer_audit'][index]['pointers'];produced={}
            for p in pointers:
                role=p['role'];kind=role.split('_')[0]
                if role.endswith('producer'):produced[kind]=p['address']
                elif p['address']!=produced.get(kind):raise RuntimeError('Raw temporary role identity mismatch')
            if k==4 and v['raw_pointer_audit'][index]['returned']!=produced.get('corners'):raise RuntimeError('Raw dangling return identity mismatch')
    if rows[-1].get('passed') is not True or rows[-1].get('pairs')!=5120 or rows[-1].get('rand_controlled') is not True:raise RuntimeError('Terminal incomplete')
    harness=runner.GAME/'preflight-geometry-wrappers-five-v4-harness.jsonl'
    if not runner.terminal_event(harness):return None
    h=[json.loads(x) for x in harness.read_text().splitlines()]
    if [r['event'] for r in h]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:raise RuntimeError('Incomplete global preflight')
    final=h[-1]
    if final['snapshot_ranges']<17692 or final['changed_ranges']!=[] or any(final[k] is not True for k in ('state_restored','entries_restored','flags_restored','floating_environment_restored','timer_unchanged')):raise RuntimeError('Global ranges/flags/timer/FP restoration failed')
    report.update(explicit_fullarena_trace_model_checked=True,pointer_producer_consumer_identity_checked=True,typed_constructor_rand_controlled=True,harness_restored=True,floating_environment_restored=True,timer_unchanged=True,harness_sha256=runner.digest(harness),bank_fp_unchanged=final['bank_fp_unchanged'])
    return report
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(base_analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    proof_path=HERE/'Diagnostics/required-callchain-proof.json';proof=json.loads(proof_path.read_text())
    assert proof['run_ready'] is True and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
    frozen=json.loads((HERE/'prepared-freeze.json').read_text())
    for p,h in frozen.items():assert runner.digest(ROOT/p)==h
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-geometry-wrappers-five-v4.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],base_analyze,'Geometry wrappers5120',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    shutil.copy2(runner.GAME/'preflight-geometry-wrappers-five-v4-harness.jsonl',dest/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    if report.get('natural_exit') is not True or report.get('exit_code')!=0:report['passed']=False;status=1
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('Preserved',dest,'passed',report['passed']);return status
if __name__=='__main__':raise SystemExit(main())
