"""Root-only replay; strict CPU address/handle model, no actor lifetime claims."""
import json,re,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
PREFIX='requested-pools-six-getters'
NAMES=[['GetPedRef','GetPed'],['GetVehicleRef','GetVehicle'],['GetObjectRef','GetObject']];STRIDES=[0x7C4,0xA18,0x19C];SLOTS=[12,16,24]
base_configuration=runner.probe_configuration
runner.probe_configuration=lambda data:base_configuration(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def strict_json(text):
    def pairs(items):
        d={}
        for k,v in items:
            if k in d:raise ValueError('Duplicate key')
            d[k]=v
        return d
    return json.loads(text,object_pairs_hook=pairs,parse_constant=lambda x:(_ for _ in ()).throw(ValueError('Nonfinite JSON')))
def hex_bytes(value,n):
    assert type(value) is str and len(value)==2*n and re.fullmatch('[0-9a-f]*',value);return bytes.fromhex(value)
def analyze(path):
    if not runner.terminal_event(path):return None
    rows=[strict_json(s)for s in path.read_text().splitlines()];assert rows[0]['event']=='start' and rows[0]['cases']==3072 and rows[0]['functions']==6
    cursor=1;pair=0;counts={}
    for family,stride in enumerate(STRIDES):
        fixture=rows[cursor];cursor+=1;assert fixture['event']=='fixture' and fixture['family']==family and fixture['stride']==stride and fixture['capacity']==2
        assert set(fixture)=={'event','family','stride','capacity','storage_address','state_address','pool_address','storage','states','header','globals','fp'}
        for field in ('storage_address','state_address','pool_address'):assert type(fixture[field]) is int and 0<fixture[field]<=0xffffffff
        storage=hex_bytes(fixture['storage'],2*stride+32);states=hex_bytes(fixture['states'],34);header=hex_bytes(fixture['header'],52);globals_=hex_bytes(fixture['globals'],68)
        assert storage==b'\xa5'*16+b'\xfd'*(2*stride)+b'\xa5'*16 and states==b'\xd3'*16+b'\x80\x80'+b'\xd3'*16
        assert header[:16]==header[-16:]==b'\xb6'*16
        assert header[16:20]==fixture['storage_address'].to_bytes(4,'little') and header[20:24]==fixture['state_address'].to_bytes(4,'little')
        assert header[24:28]==(2).to_bytes(4,'little') and header[28:32]==b'\xff'*4 and header[32:34]==b'\0\0'
        assert int.from_bytes(globals_[SLOTS[family]:SLOTS[family]+4],'little')==fixture['pool_address']
        assert set(fixture['fp'])=={'rounding','exceptions','mxcsr'} and all(type(v)is int for v in fixture['fp'].values())
        for fn,total in enumerate((256,768)):
            name='CPools::'+NAMES[family][fn];counts[name]=total
            for seed in range(total):
                row=rows[cursor];cursor+=1;index=seed%2;id_=(seed//2)%128;variant=seed//256 if fn else 0;tag=[id_,id_^1,id_^128][variant]
                address=fixture['storage_address']+index*stride;argument=((index<<8)|tag) if fn else address;expected=(address if tag==id_ else 0) if fn else (index<<8)|id_
                assert set(row)=={'event','function','family','fn','pair','case','index','id','variant','argument','expected','equal','accepted','original','candidate'}
                assert (row['event'],row['function'],row['family'],row['fn'],row['pair'],row['case'],row['index'],row['id'],row['variant'],row['argument'],row['expected'])==('comparison',name,family,fn,pair,seed,index,id_,variant,argument,expected)
                expected_storage=storage[:16]+bytes((seed*47+j*29)&255 for j in range(2*stride))+storage[-16:]
                expected_states=bytearray(states);expected_states[16]=(id_+17)&127;expected_states[17]=(id_+31)&127;expected_states[16+index]=id_
                leg=dict(returned=expected,switch_exact=True,storage_unchanged=True,states_unchanged=True,header_unchanged=True,globals_unchanged=True,storage=expected_storage.hex(),states=expected_states.hex(),header=header.hex(),globals=globals_.hex(),fp=fixture['fp'])
                assert row['original']==row['candidate']==leg and row['equal'] is True and row['accepted'] is True
                pair+=1
        restore=rows[cursor];cursor+=1;assert restore==dict(event='family_restore',family=family,state_restored=True)
    assert cursor==len(rows)-1 and rows[-1]==dict(event='complete',pairs=3072,failures=0,passed=True,state_restored=True)
    harness=runner.GAME/'preflight-pools-six-getters-harness.jsonl'
    if not runner.terminal_event(harness):return None
    h=[strict_json(s)for s in harness.read_text().splitlines()];assert [r['event']for r in h]==['preflight_entered','before_task_bank','task_bank_returned','complete']
    assert h[-1]['snapshot_ranges']>=17692 and h[-1]['changed_ranges']==[] and all(h[-1][k]is True for k in ('state_restored','entries_restored','flags_restored','floating_environment_restored','timer_unchanged'))
    return dict(passed=True,pairs=3072,pairs_per_function=counts,differences=0,all_equal=True,state_restored=True,harness_restored=True,floating_environment_restored=True,full_borrowed_pool_arena_model_checked=True,log_sha256=runner.digest(path),harness_sha256=runner.digest(harness))
def main():
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text());proofpath=HERE/'Diagnostics/borrowed-constructor-proof.json';proof=json.loads(proofpath.read_text())
    assert proof['reviewed']is True and proof['no_actor_or_heap_lifecycle_calls']is True and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    for p,h in json.loads((HERE/'prepared-freeze.json').read_text()).items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-pools-six-getters.{ext}';assert runner.digest(source)==manifest[ext];target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'PoolsSixGetters3072',timeout=240,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent;dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    shutil.copy2(runner.GAME/'preflight-pools-six-getters-harness.jsonl',dest/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,borrowed_constructor_proof_sha256=runner.digest(proofpath),runner_sha256=runner.digest(Path(__file__)))
    if report.get('natural_exit')is not True or report.get('exit_code')!=0:report['passed']=False;status=1
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));return status
if __name__=='__main__':raise SystemExit(main())
