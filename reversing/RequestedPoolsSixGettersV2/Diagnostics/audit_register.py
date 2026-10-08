"""Offline independent acceptance audit, then explicitly authorized six historical records."""
import hashlib,importlib.util,json,os,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3];HERE=Path(__file__).resolve().parents[1];RUN=HERE/'runs/20261008-170325-898320'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
report=json.loads((RUN/'enriched-summary.json').read_text());manifest=report['manifest'];assert report['passed']is True and report['natural_exit']is True and report['exit_code']==0 and report['pairs']==3072 and report['differences']==0 and report['saves_unchanged']is True and report['configs_restored']is True
for group in ('tested_sources','bank_sources'):
    for p,h in manifest[group].items():assert sha(ROOT/p)==h,p
for p,h in json.loads((HERE/'prepared-freeze.json').read_text()).items():assert sha(ROOT/p)==h,p
proofpath=HERE/'Diagnostics/borrowed-constructor-proof.json';proof=json.loads(proofpath.read_text());assert proof['reviewed']is True and proof['no_actor_or_heap_lifecycle_calls']is True and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
for ext in ('asi','pdb'):assert sha(HERE/f'artifacts/gta_reversed-pools-six-getters-v2.{ext}')==manifest[ext]
spec=importlib.util.spec_from_file_location('pools_audit',HERE/'run_local.py');bank=importlib.util.module_from_spec(spec);spec.loader.exec_module(bank)
with tempfile.TemporaryDirectory()as temp:
    bank.runner.GAME=Path(temp);(bank.runner.GAME/'preflight-pools-six-getters-v2-harness.jsonl').write_bytes((RUN/'harness.jsonl').read_bytes());result=bank.analyze(RUN/'comparisons.jsonl')
assert result['pairs']==3072 and result['differences']==0
rows=[json.loads(s)for s in(RUN/'comparisons.jsonl').read_text().splitlines()];pairs=[r for r in rows if r['event']=='comparison'];assert len({(r['family'],r['fn'],r['case'])for r in pairs})==3072
assert all(r['accepted']is True and r['equal']is True and r['original']==r['candidate']for r in pairs)
h=[json.loads(s)for s in(RUN/'harness.jsonl').read_text().splitlines()];assert h[-1]['snapshot_ranges']==17692 and h[-1]['bank_fp_unchanged']is True and h[-1]['initial_fenv']==h[-1]['bank_fenv'] and h[-1]['initial_mxcsr']==h[-1]['bank_mxcsr']
audit=dict(result,summary_sha256=sha(RUN/'enriched-summary.json'),manifest=manifest,borrowed_constructor_proof_sha256=sha(proofpath),frozen_sha256=sha(HERE/'prepared-freeze.json'),natural_exit=True,exit_code=0,unique_cases=3072,exact_snapshot_ranges=17692,actor_lifetime_or_heap_credit=False)
ap=HERE/'Diagnostics/independent-replay-audit.json';ap.write_text(json.dumps(audit,indent=2)+'\n')
scope='Six CPU pool address/handle wrappers over actual typed nonowning 20-byte pool headers and two borrowed raw slots. In-bounds indices0/1; active seven-bit IDs0..127; aligned slot addresses for GetRef; exact matched/stale-ID/free-bit mismatch handles for Get. Full uint32 raw returns/pointer identity, full storage/state/header/68-byte pool globals, guards, FP, gates and52-byte target entry restoration checked. All17692 hook/table ranges/flags, timer and external FP restored; natural exit0. No invalid-index/unaligned/free-stored-slot behavior, actor lifetime, heap, borrowed-constructor/helper, fullclass or current dependency accreditation.'
summary=dict(passed=True,pairs=3072,differences=0,functions=result['pairs_per_function'],scope=scope,dependency_review=None,run=RUN.relative_to(ROOT).as_posix(),audit_path=ap.relative_to(ROOT).as_posix(),audit_sha256=sha(ap),manifest=manifest,summary_sha256=audit['summary_sha256'],harness_sha256=audit['harness_sha256'],log_sha256=audit['log_sha256'],borrowed_constructor_proof_sha256=sha(proofpath))
sp=HERE/'validation-summary.json';sp.write_text(json.dumps(summary,indent=2)+'\n')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);data=json.loads(registry.read_text(encoding='utf8'));assert len(data['functions'])==6861
entries=[('GetPedRef','0x0054FF60',256),('GetPed','0x0054FF90',768),('GetVehicleRef','0x0054FFC0',256),('GetVehicle','0x0054FFF0',768),('GetObjectRef','0x00550020',256),('GetObject','0x00550050',768)]
for name,address,count in entries:
    hook=dict(class_='CPools');hook={'class':'CPools','fn_name':name,'address':address,'type':'S'};assert not any(x['hook']==hook for x in data['functions'])
    data['functions'].append(dict(hook=hook,evidence=dict(path=sp.relative_to(ROOT).as_posix(),sha256=sha(sp),passed_path=['passed'],pairs=count,original_exe_sha256=report['original_exe_sha256']),tested_sources=manifest['tested_sources'],source_provenance='Fresh native fullasm and matching compiled PDB borrowed-constructor/uint32 call proof; independently rerun full raw byte and pointer/handle oracle for complete3072 replay.',dependency_review=None,scope=scope,review_reference='reversing/RequestedPoolsSixGettersV2/corpus-plan.json'))
temp=registry.with_suffix('.small-pools-six.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before=6861,after=6867,registry_sha256=sha(registry),summary_sha256=sha(sp),audit_sha256=sha(ap),source_sha256=manifest['tested_sources']['source/game_sa/Pools.cpp']),indent=2))
