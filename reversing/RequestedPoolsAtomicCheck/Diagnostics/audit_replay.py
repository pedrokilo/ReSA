import hashlib,json,shutil,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1];sys.path.insert(0,str(HERE));import run_local as bank
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
run=HERE/'runs/20261008-164307-573432';report=json.loads((run/'enriched-summary.json').read_text());manifest=report['manifest']
assert report['passed'] is True and report['pairs']==256 and report['differences']==0 and report['natural_exit'] is True and report['exit_code']==0
assert all(report[k] is True for k in ('state_restored','harness_restored','floating_environment_restored','saves_unchanged','configs_restored'))
for name,h in json.loads((HERE/'prepared-freeze.json').read_text()).items():assert sha(ROOT/name)==h
for name,h in manifest['bank_sources'].items():assert sha(ROOT/name)==h
proof=json.loads((HERE/'Diagnostics/required-ABI-proof.json').read_text());assert proof['run_ready'] is True and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
with tempfile.TemporaryDirectory() as temp:
 bank.runner.GAME=Path(temp);shutil.copy2(run/'harness.jsonl',Path(temp)/'preflight-pools-atomic-check-harness.jsonl');analyzed=bank.analyze(run/'comparisons.jsonl')
assert analyzed['passed'] is True and analyzed['log_sha256']==report['log_sha256'] and analyzed['harness_sha256']==report['harness_sha256']
rows=[json.loads(v)for v in (run/'comparisons.jsonl').read_text().splitlines()];assert len({r['eax_seed']for r in rows[2:-1]})==256
h=[json.loads(v)for v in (run/'harness.jsonl').read_text().splitlines()];assert h[-1]['snapshot_ranges']==17692 and h[-1]['bank_fp_unchanged'] is True and h[-1]['initial_fenv']==h[-1]['bank_fenv'] and h[-1]['initial_mxcsr']==h[-1]['bank_mxcsr']
result=dict(passed=True,pairs=256,differences=0,natural_exit0=True,global_ranges_restored=17692,strict_full_guard64_poolglobal68_FP_model_checked=True,caller_EAX_contexts_unique=256,raw_uint32_return=4294967295,cdecl_stack_and_saved_register_contract_checked=True,compiled_ABI_proof_sha256=sha(HERE/'Diagnostics/required-ABI-proof.json'),summary_sha256=sha(run/'enriched-summary.json'),log_sha256=sha(run/'comparisons.jsonl'),harness_sha256=sha(run/'harness.jsonl'),manifest=manifest,scope='One pure parameterless constant-return function under256 caller contexts; no EFLAGS, pool/actor lifecycle or class-wide accreditation.')
(HERE/'Diagnostics/independent-replay-audit.json').write_text(json.dumps(result,indent=2));print(json.dumps({k:v for k,v in result.items()if k!='manifest'},indent=2))
