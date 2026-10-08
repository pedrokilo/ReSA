import copy,hashlib,importlib.util,json,tempfile,traceback
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];OLD=HERE.with_name('RequestedPoolsSixGetters');RUN=OLD/'runs/20261008-165717-944081'
spec=importlib.util.spec_from_file_location('oldbank',OLD/'run_local.py');bank=importlib.util.module_from_spec(spec);spec.loader.exec_module(bank)
rows=[json.loads(s)for s in(RUN/'comparisons.jsonl').read_text().splitlines()];pairs=[r for r in rows if r['event']=='comparison'];assert len(pairs)==3072 and all(r['accepted']is True and r['equal']is True for r in pairs)
restores=[r for r in rows if r['event']=='family_restore'];assert len(restores)==3 and all(r['state_restored']is False for r in restores)
assert rows[-1]==dict(event='error',failures=0,pairs=3072,passed=False,state_restored=True)
with tempfile.TemporaryDirectory()as temp:
    root=Path(temp);bank.runner.GAME=root;(root/'preflight-pools-six-getters-harness.jsonl').write_bytes((RUN/'harness.jsonl').read_bytes())
    path=root/'model-only.jsonl';projected=copy.deepcopy(rows)
    # Explicit diagnostic projection: never used as a runtime approval.
    for r in projected:
        if r['event']=='family_restore':r['state_restored']=True
    projected[-1]=dict(event='complete',pairs=3072,failures=0,passed=True,state_restored=True)
    path.write_text('\n'.join(map(json.dumps,projected))+'\n');model=bank.analyze(path);assert model['pairs']==3072
    path.write_bytes((RUN/'comparisons.jsonl').read_bytes())
    try:bank.analyze(path)
    except AssertionError:failure=traceback.format_exc()
    else:raise AssertionError('original failure accepted')
# Tiny overlapping-entry model reproduces the ordering defect independently.
before=bytearray(range(100));mutated=bytearray(before);mutated[48:53]=b'\xe9\xaa\xbb\xcc\xdd'
ref_snapshot=bytes(before[:52]);get_snapshot=bytes(before[48:100]);mutated[:5]=before[:5]
assert bytes(mutated[:52])!=ref_snapshot
mutated[48:53]=before[48:53];assert bytes(mutated[:52])==ref_snapshot and bytes(mutated[48:100])==get_snapshot
out=dict(approval=False,original_pairs=3072,original_accepted=3072,original_failures=0,original_family_restore_false=3,final_hook_flags_restored=True,diagnostic_projected_model_pairs=model['pairs'],projection_fields=['family_restore.state_restored','terminal'],traceback=failure,overlap_bytes=4,full_snapshot_bytes=52,two_phase_model_passed=True,raw_log_sha256=hashlib.sha256((RUN/'comparisons.jsonl').read_bytes()).hexdigest())
(HERE/'Diagnostics/v1-failure-audit.json').write_text(json.dumps(out,indent=2));print(json.dumps({k:v for k,v in out.items()if k!='traceback'}))
