"""Offline reaudit of the approved Hex V2 replay, then three scoped historical records (dependency_review=null)."""
import gzip,hashlib,importlib.util,json,os,shutil,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1];RUN=HERE/'runs/20261008-230025-789765'
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
report=json.loads((RUN/'enriched-summary.json').read_text(encoding='utf-8'));manifest=report['manifest']
assert report['passed'] is True and report['natural_exit'] is True and report['exit_code']==0 and report['pairs']==13952 and report['differences']==0 and report['model_mismatches']==0
assert report['all_equal'] is True and report['state_restored'] is True and report['harness_restored'] is True and report['saves_unchanged'] is True and report['configs_restored'] is True and report['validation_errors']==[]
assert report['pairs_per_function']=={'CPad::ProcessPCSpecificStuff':1024,'CPad::StartShake_Distance':10240,'CPad::StartShake_Train':2688}
for group in ('tested_sources','bank_sources'):
    for p,h in manifest[group].items():assert sha(ROOT/p)==h,p
for ext in ('asi','pdb'):assert sha(HERE/f'artifacts/gta_reversed-pad-distance-train-nop-hex-v2.{ext}')==manifest[ext]
assert report['executed_probe_sha256']==manifest['asi'] and report['original_exe_sha256']==manifest['exe_sha256']
assert sha(HERE/'artifacts/train-compiled-abi.json')==manifest['train_abi_proof_sha256']
assert sha(RUN/'comparisons.jsonl')==report['log_sha256']==report['comparison_storage']['uncompressed_sha256'] and sha(RUN/'harness.jsonl')==report['harness_sha256']
assert sha(RUN/'comparisons.jsonl.gz')==report['comparison_storage']['compressed_sha256']
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(4*1024*1024),b''):digest.update(block)
assert digest.hexdigest()==report['log_sha256']
# Rejected baselines stay untouched; they are only referenced by hash.
rejected_disk_full=HERE/'runs/20261008-170728-931505/enriched-summary.json';rejected_hex_v1=ROOT/'reversing/RequestedPadDistanceTrainNopHex/runs/20261008-164703-580513/enriched-summary.json'
assert rejected_disk_full.is_file() and rejected_hex_v1.is_file()
sys.path.insert(0,str(HERE))
spec=importlib.util.spec_from_file_location('distance_train_nop_hex_v2_audit',HERE/'run_local.py');bank=importlib.util.module_from_spec(spec);spec.loader.exec_module(bank)
with tempfile.TemporaryDirectory() as temp:
    bank.runner.GAME=Path(temp);(Path(temp)/'preflight-pad-distance-train-nop-hex-v2-harness.jsonl').write_bytes((RUN/'harness.jsonl').read_bytes())
    result=bank.analyze(RUN/'comparisons.jsonl')
assert result and result['passed'] and result['pairs']==13952 and result['differences']==0 and result['model_mismatches']==0 and result['validation_errors']==[]
assert result['pairs_per_function']==report['pairs_per_function'] and result['log_sha256']==report['log_sha256'] and result['harness_sha256']==report['harness_sha256']
final=bank.diagnostics['terminal_bank_record'];assert final['x87_environment_before']==final['x87_environment_after'] and len(final['x87_environment_before'])==56 and final['mxcsr_before']==final['mxcsr_after'] and final['mxcsr_after']&0x1f80==0x1f80
assert bank.diagnostics['terminal_harness_record']['snapshot_ranges']==17692 and bank.diagnostics['terminal_harness_record']['changed_ranges']==[]
audit=dict(result,summary_sha256=sha(RUN/'enriched-summary.json'),manifest_head=manifest['head'],asi_sha256=manifest['asi'],pdb_sha256=manifest['pdb'],train_abi_proof_sha256=manifest['train_abi_proof_sha256'],natural_exit=True,exit_code=0,exact_snapshot_ranges=17692,x87_environment_bytes_before_equal_after=28,mxcsr=final['mxcsr_after'],independent_oracle='oracle.py (byte-identical to revision2) re-executed offline over the preserved full payload',external_audit_verdict='APTO (declared by the requester; no audit file exists in the repository)',rejected_attempts={rel(rejected_disk_full):sha(rejected_disk_full),rel(rejected_hex_v1):sha(rejected_hex_v1)})
diag=HERE/'Diagnostics';diag.mkdir(exist_ok=True)
audit_path=diag/'independent-replay-audit.json';assert not audit_path.exists();audit_path.write_text(json.dumps(audit,indent=2)+'\n',encoding='utf-8')
scope=('13952 isolated CPU pairs, three CPad targets before graphics: ProcessPCSpecificStuff 0x0053FB40 (NOP) 1024, StartShake_Distance 0x0053F9A0 10240, StartShake_Train 0x0053FA70 2688; zero differences, zero oracle mismatches, natural exit 0, harness/state/saves/configs restored (17692 snapshot ranges). '
 'Limits: finite inputs only (no NaN/Inf) under x87 PC24 round-nearest with masked exceptions and MXCSR 0x1f80; FindPlayerVehicle replaced by a stub, so only call multiplicity and arguments are checked and the helper gets no credit; no resources or gameplay; the camera uses a borrowed private matrix that is restored; '
 'the NOP only proves "no effects"; the x87 environment is verified per bank (28-byte FNSTENV image before equals after), not per row. Historical evidence approved with explicit limits; no current dependency accreditation.')
summary=dict(passed=True,pairs=13952,differences=0,functions=report['pairs_per_function'],scope=scope,dependency_review=None,run=rel(RUN),audit_path=rel(audit_path),audit_sha256=sha(audit_path),manifest=manifest,summary_sha256=sha(RUN/'enriched-summary.json'),harness_sha256=report['harness_sha256'],log_sha256=report['log_sha256'],comparison_storage=report['comparison_storage'],natural_exit=True,exit_code=0,x87_environment_restored=True,
 rejected_attempts={rel(rejected_disk_full):sha(rejected_disk_full),rel(rejected_hex_v1):sha(rejected_hex_v1)})
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-pad-distance-train-nop-hex-v2.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);assert n==6867
entries=[('ProcessPCSpecificStuff','0x0053FB40',1024),('StartShake_Distance','0x0053F9A0',10240),('StartShake_Train','0x0053FA70',2688)]
for name,address,count in entries:
    hook={'class':'CPad','fn_name':name,'address':address,'type':'S'}
    assert not any(x['hook']==hook for x in data['functions']) and not any(x['hook']['class']=='CPad' and x['hook']['fn_name']==name for x in data['functions'])
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['passed'],pairs=count,original_exe_sha256=report['original_exe_sha256']),tested_sources=manifest['tested_sources'],source_provenance='Exact compiled Hex V2 manifest, matching ASI/PDB and Train RET8 ABI proof; frozen strict oracle reexecuted offline over the full preserved payload; 28-byte x87 environment restored by bank. Prior disk-full attempt and Hex V1 baseline preserved as rejected.',dependency_review=None,scope=scope,review_reference='reversing/RequestedPadDistanceTrainNopHexV2/README.md'))
temp=registry.with_suffix('.distance-train-nop.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp),audit_sha256=sha(audit_path)),indent=2))
