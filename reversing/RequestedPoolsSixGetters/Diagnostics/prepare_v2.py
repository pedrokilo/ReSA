"""Archive exact V1 and prepare two-phase restoration only; no build/game."""
import hashlib,json,shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3];old=ROOT/'reversing/RequestedPoolsSixGetters';new=ROOT/'reversing/RequestedPoolsSixGettersV2'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
freeze=json.loads((old/'prepared-freeze.json').read_text());assert all(sha(ROOT/p)==h for p,h in freeze.items());new.mkdir(exist_ok=False)
archive=new/'Diagnostics/original-v1';archive.mkdir(parents=True)
for p in freeze:shutil.copy2(ROOT/p,archive/Path(p).name)
shutil.copy2(old/'prepared-freeze.json',archive/'prepared-freeze.json')
shutil.copytree(old/'runs',archive/'runs');shutil.copy2(old/'artifacts/manifest.json',archive/'manifest.json')
for p in freeze:
    rel=Path(p).relative_to(old.relative_to(ROOT));dest=new/rel;dest.parent.mkdir(parents=True,exist_ok=True)
    if dest.suffix in ('.py','.hpp'):
        text=(ROOT/p).read_text().replace('RequestedPoolsSixGetters','RequestedPoolsSixGettersV2').replace('requested-pools-six-getters','requested-pools-six-getters-v2').replace('preflight-pools-six-getters','preflight-pools-six-getters-v2').replace('gta_reversed-pools-six-getters','gta_reversed-pools-six-getters-v2')
        dest.write_text(text)
    else:shutil.copy2(ROOT/p,dest)
p=new/'LiveComparison.hpp';s=p.read_text();needle='restored&=nativeBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52)&&dllBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);}'
assert s.count(needle)==1
s=s.replace(needle,'}\n    // Restore both overlapping entries before checking every full 52-byte snapshot.\n    for(int k=0;k<2;++k){restored&=nativeBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52)&&dllBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);}')
p.write_text(s)
for name in ('test_model.py','inspect_constructor_graph.py','prove_borrowed_constructor.py'):
    s=(old/'Diagnostics'/name).read_text().replace('RequestedPoolsSixGetters','RequestedPoolsSixGettersV2').replace('requested-pools-six-getters','requested-pools-six-getters-v2').replace('preflight-pools-six-getters','preflight-pools-six-getters-v2').replace('gta_reversed-pools-six-getters','gta_reversed-pools-six-getters-v2');(new/'Diagnostics'/name).write_text(s)
revision=dict(reason='52-byte Ref snapshots overlap the neighbouring Get entry at +48; compare only after restoring both hooks.',source_functional_changes=False,corpus_pairs=3072,original_freeze_sha256=sha(old/'prepared-freeze.json'),old_run='20261008-165717-944081',old_failures=0,old_comparison_accepted=3072,old_family_restore_false=3,approval=False)
(new/'Diagnostics/revision.json').write_text(json.dumps(revision,indent=2))
f={p.relative_to(ROOT).as_posix():sha(p)for p in [new/Path(p).relative_to(old.relative_to(ROOT))for p in freeze]};(new/'prepared-freeze.json').write_text(json.dumps(f,indent=2))
print(json.dumps(dict(frozen=len(f),freeze_sha256=sha(new/'prepared-freeze.json'))))
