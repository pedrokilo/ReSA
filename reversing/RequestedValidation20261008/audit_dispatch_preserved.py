"""Read preserved evidence, write reviewed proposals; never run GTA or write registry."""
import argparse,collections,csv,gzip,hashlib,importlib.util,json,shutil,sys,tempfile
from pathlib import Path
if not __debug__:raise RuntimeError('Strict preserved audit requires assertions enabled')
ROOT=Path(__file__).resolve().parents[2];BANK=ROOT/'reversing/RequestedDispatchShadowFour'
def sha(path):
    with Path(path).open('rb') as stream:return hashlib.file_digest(stream,'sha256').hexdigest()

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--run',type=Path,required=True);args=parser.parse_args();run=args.run.resolve()
    report=json.loads((run/'enriched-summary.json').read_text());manifest=report['manifest']
    for key in ('passed','all_equal','natural_exit','saves_unchanged','configs_restored','state_restored','harness_restored'):assert report.get(key) is True,key
    assert report['exit_code']==0 and report['differences']==report['model_mismatches']==0
    assert report['executed_probe_sha256']==manifest['asi']==sha(BANK/'artifacts/gta_reversed-dispatch-shadow-four.asi')
    assert manifest['pdb']==sha(BANK/'artifacts/gta_reversed-dispatch-shadow-four.pdb')
    assert report['original_exe_sha256']==manifest['exe_sha256']=='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
    assert report['runner_sha256']==sha(BANK/'run_local.py')
    assert all(sha(ROOT/p)==v for label in ('tested_sources','bank_sources') for p,v in manifest[label].items())
    assert sha(ROOT/'source/app/platform/win/WinMain.cpp')==manifest['original_winmain_sha256']
    inventory={p.relative_to(ROOT).as_posix() for p in (ROOT/'source').rglob('*') if p.is_file() and p.suffix in ('.cpp','.h','.hpp','.inl','.inc') and p.relative_to(ROOT).as_posix()!='source/app/platform/win/WinMain.cpp'}
    assert inventory=={p for p in manifest['tested_sources'] if p.startswith('source/')}
    sys.path.insert(0,str(BANK));spec=importlib.util.spec_from_file_location('strict_dispatch_preserved',BANK/'run_local.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    original_game=module.runner.GAME
    with tempfile.TemporaryDirectory() as temp:
        temp=Path(temp);module.runner.GAME=temp;shutil.copy2(run/'harness.jsonl',temp/'preflight-dispatch-shadow-four-harness.jsonl')
        raw=run/'comparisons.jsonl'
        if not raw.exists():
            raw=temp/'comparisons.jsonl'
            with gzip.open(run/'comparisons.jsonl.gz','rb') as source,raw.open('wb') as target:shutil.copyfileobj(source,target)
        try:
            checked=module.analyze(raw);assert checked['passed'] and checked['pairs']==5120
            assert checked['log_sha256']==report['log_sha256'] and checked['harness_sha256']==report['harness_sha256']
            counts=collections.Counter();domains={key:collections.Counter() for key in module.KEYS}
            for line in raw.open(encoding='utf-8'):
                row=json.loads(line)
                if row.get('event')!='comparison':continue
                expected=module.expected_row(row);assert row['original']==row['candidate']==expected and row['equal'] is True
                key=row['function'];counts[key]+=1
                if key.startswith('CCustom'):domains[key][str((row['normal_status'],row['dn_status'],row['plugin_status']))]+=1
                else:domains[key][str((row['initialized'],row['shadow_null'],row['owner_null']))]+=1
            assert dict(counts)==module.EXPECTED
        finally:module.runner.GAME=original_game
    hooks=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')));scope=json.loads((BANK/'scope.json').read_text());out=ROOT/'reversing/RequestedValidation20261008/dispatch-audits'/run.name;out.mkdir(parents=True,exist_ok=False)
    proposals=[]
    for target in scope['targets']:
        key=target['class']+'::'+target['fn_name'];rows=[h for h in hooks if h['class']==target['class'] and h['fn_name']==target['fn_name'] and int(h['address'],16)==int(target['address'],16)];assert len(rows)==1
        reviewed=dict(passed=True,function=key,pairs=counts[key],aggregate_parent=run.relative_to(ROOT).as_posix(),parent_sha256=sha(run/'enriched-summary.json'),manifest=manifest,strict_preserved_reaudit=checked,domains=dict(domains[key]),approved_scope='Pregraphics CPU private fixtures; exact native traces, writes and canonical bool AL where applicable. Collaborator implementations, graphics integration, camera/gameplay/resource lifetimes excluded.',registry_written=False)
        path=out/(target['fn_name']+'-validation-summary.json');path.write_text(json.dumps(reviewed,indent=2)+'\n')
        proposals.append(dict(hook={k:rows[0][k] for k in ('class','fn_name','address','type')},evidence=dict(path=path.relative_to(ROOT).as_posix(),sha256=sha(path),passed_path=['passed'],pairs=counts[key],original_exe_sha256=manifest['exe_sha256']),tested_sources=manifest['tested_sources'],source_provenance='Exact compiled manifest '+manifest['asi']+'; independently reaudited preserved full byte/trace oracle and terminal restoration/normal exit.',dependency_review=None,scope=reviewed['approved_scope'],review_reference='reversing/RequestedDispatchShadowFour/README.md'))
    (out/'registration-proposals.json').write_text(json.dumps(dict(registry_written=False,requires_coordinator_authorization=True,functions=proposals),indent=2)+'\n');print(json.dumps(dict(audit_passed=True,pairs=dict(counts),output=out.as_posix(),registry_written=False)))
if __name__=='__main__':main()
