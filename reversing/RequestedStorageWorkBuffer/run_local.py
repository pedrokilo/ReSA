"""Root-only in-buffer replay; strict complete-byte oracle, no filesystem semantics credit."""
import hashlib,json,re,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
PREFIX='requested-storage-work-buffer';NAMES=['SaveDataToWorkBuffer-org','LoadDataFromWorkBuffer-org'];ADDRESSES=['0x005D1270','0x005D1300']
base_configuration=runner.probe_configuration;runner.probe_configuration=lambda d:base_configuration(d).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def strict_json(text):
    def pairs(items):
        d={}
        for k,v in items:
            if k in d:raise ValueError('duplicate key')
            d[k]=v
        return d
    return json.loads(text,object_pairs_hook=pairs,parse_constant=lambda x:(_ for _ in()).throw(ValueError('nonfinite')))
def hx(s,n):
    assert type(s)is str and len(s)==2*n and re.fullmatch('[0-9a-f]*',s);return bytes.fromhex(s)
def expected_leg(start,fn,seed):
    pattern=seed%8;size=[-1,0,1,2,3,4,7,16][(seed//8)%8];pos=[0,1,2,3,16,51168,51176,51184][(seed//64)%8];failed=seed//512!=0
    work=bytearray(b'\xa5'*16+bytes((pattern*37+i*13)&255 for i in range(51200))+b'\xa5'*16);caller=bytearray(b'\xd3'*16+bytes((pattern*53+i*29)&255 for i in range(256))+b'\xd3'*16)
    globals_=bytearray(hx(start['globals'],24));sg=bytearray(hx(start['size_globals'],32));globals_[:4]=start['work_address'].to_bytes(4,'little');globals_[4:8]=pos.to_bytes(4,'little',signed=True);globals_[20]=int(failed);sg[16:20]=(51200).to_bytes(4,'little')
    if not failed and size>0:
        if fn:caller[16:16+size]=work[16+pos:16+pos+size]
        else:work[16+pos:16+pos+size]=caller[16:16+size]
        globals_[4:8]=(pos+size).to_bytes(4,'little',signed=True)
    return dict(returned_al=int(not failed),switch_exact=True,work=work.hex(),caller=caller.hex(),globals=globals_.hex(),size_globals=sg.hex(),fp=start['fp'])
def analyze(path):
    if not runner.terminal_event(path):return None
    with path.open()as f:
        start=strict_json(next(f));assert set(start)=={'event','pairs','work_address','caller_address','globals','size_globals','fp'} and start['event']=='start' and start['pairs']==2048
        for field in ('work_address','caller_address'):assert type(start[field])is int and 0<start[field]<=0xffffffff
        assert set(start['fp'])=={'rounding','exceptions','mxcsr'} and all(type(x)is int for x in start['fp'].values())
        for fn in range(2):
            for seed in range(1024):
                r=strict_json(next(f));pattern=seed%8;size=[-1,0,1,2,3,4,7,16][(seed//8)%8];pos=[0,1,2,3,16,51168,51176,51184][(seed//64)%8];failed=seed//512!=0
                assert set(r)=={'event','fn','pair','case','pattern','size','position','prior_failed','equal','accepted','original','candidate'}
                assert (r['event'],r['fn'],r['pair'],r['case'],r['pattern'],r['size'],r['position'],r['prior_failed'])==('comparison',fn,fn*1024+seed,seed,pattern,size,pos,failed)
                assert type(r['prior_failed'])is bool and r['equal']is True and r['accepted']is True
                model=expected_leg(start,fn,seed);assert r['original']==r['candidate']==model
                for leg in (r['original'],r['candidate']):
                    assert type(leg['returned_al'])is int and type(leg['switch_exact'])is bool and all(type(v)is int for v in leg['fp'].values())
                    hx(leg['work'],51232);hx(leg['caller'],288);hx(leg['globals'],24);hx(leg['size_globals'],32)
        terminal=strict_json(next(f));assert terminal==dict(event='complete',pairs=2048,failures=0,passed=True,state_restored=True);assert not f.read()
    hpath=runner.GAME/'preflight-storage-work-buffer-harness.jsonl'
    if not runner.terminal_event(hpath):return None
    h=[strict_json(s)for s in hpath.read_text().splitlines()];assert [r['event']for r in h]==['preflight_entered','before_task_bank','task_bank_returned','complete']
    assert h[-1]['snapshot_ranges']==17692 and h[-1]['changed_ranges']==[] and all(h[-1][k]is True for k in ('state_restored','entries_restored','flags_restored','floating_environment_restored','timer_unchanged','bank_fp_unchanged'))
    return dict(passed=True,pairs=2048,pairs_per_function={n:1024 for n in NAMES},differences=0,all_equal=True,state_restored=True,harness_restored=True,floating_environment_restored=True,full_buffer_model_checked=True,log_sha256=runner.digest(path),harness_sha256=runner.digest(hpath))
def main():
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text());proofpath=HERE/'Diagnostics/required-ABI-proof.json';proof=json.loads(proofpath.read_text());assert proof['reviewed']is True and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h,p
    for p,h in json.loads((HERE/'prepared-freeze.json').read_text()).items():assert runner.digest(ROOT/p)==h,p
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-storage-work-buffer.{ext}';assert runner.digest(source)==manifest[ext];target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'StorageWorkBuffer2048',timeout=600,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent;dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if(run/name).exists():shutil.copy2(run/name,dest/name)
    shutil.copy2(runner.GAME/'preflight-storage-work-buffer-harness.jsonl',dest/'harness.jsonl');report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,ABI_proof_sha256=runner.digest(proofpath),runner_sha256=runner.digest(Path(__file__)))
    if report.get('natural_exit')is not True or report.get('exit_code')!=0:report['passed']=False;status=1
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));return status
if __name__=='__main__':raise SystemExit(main())
