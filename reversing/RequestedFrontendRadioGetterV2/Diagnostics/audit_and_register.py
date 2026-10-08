"""Offline complete getter audit and one historical registry addition."""
import hashlib,json,os,shutil,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1];sys.path.insert(0,str(HERE))
import run_local as bank
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
run=HERE/'runs/20261008-163517-153924';report=json.loads((run/'enriched-summary.json').read_text());manifest=report['manifest']
assert report['passed'] is True and report['pairs']==2048 and report['differences']==0 and report['natural_exit'] is True and report['exit_code']==0
assert all(report[k] is True for k in ('state_restored','floating_environment_restored','harness_restored','saves_unchanged','configs_restored'))
for path,h in json.loads((HERE/'prepared-freeze.json').read_text()).items():assert sha(ROOT/path)==h
for path,h in manifest['bank_sources'].items():assert sha(ROOT/path)==h
assert manifest['asi']=='faf7459ac2c4a9d5748fac9c76787de52163aceced0b2bec14ee53a212a0891b'
proof=json.loads((HERE/'Diagnostics/constructor-proof.json').read_text());assert proof['reviewed'] is True and proof['default_constructor_has_no_external_side_effects'] is True and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
assert report['constructor_proof_sha256']==sha(HERE/'Diagnostics/constructor-proof.json')
with tempfile.TemporaryDirectory() as temp:
    bank.runner.GAME=Path(temp);shutil.copy2(run/'harness.jsonl',Path(temp)/'preflight-frontend-radio-v2-harness.jsonl');analyzed=bank.analyze(run/'comparisons.jsonl')
assert analyzed['passed'] is True and analyzed['log_sha256']==report['log_sha256'] and analyzed['harness_sha256']==report['harness_sha256']
rows=[json.loads(v) for v in (run/'comparisons.jsonl').read_text().splitlines()];counts=[0]*16
truth=[0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1]
for row in rows[2:-1]:
    seed=row['case'];counts[seed%16]+=1
    for side in ('original','candidate'):assert row[side]['raw_al']==truth[seed%16]
assert counts==[128]*16
h=[json.loads(x) for x in (run/'harness.jsonl').read_text().splitlines()];assert h[-1]['snapshot_ranges']==17692 and h[-1]['bank_fp_unchanged'] is True and h[-1]['initial_fenv']==h[-1]['bank_fenv'] and h[-1]['initial_mxcsr']==h[-1]['bank_mxcsr']
audit=dict(passed=True,pairs=2048,differences=0,independent_truth_table_counts=counts,strict_full524byte_arena_and18byte_timer_model=True,valid_int16_active_values=[0,1],return_contract='AL only; upper EAX unspecified',exact_gates=True,perleg_FP_payload_matched=True,external_FP_restored=True,global_ranges_restored=17692,natural_exit0=True,summary_sha256=sha(run/'enriched-summary.json'),log_sha256=sha(run/'comparisons.jsonl'),harness_sha256=sha(run/'harness.jsonl'),constructor_proof_sha256=sha(HERE/'Diagnostics/constructor-proof.json'))
auditpath=HERE/'Diagnostics/independent-replay-audit.json';auditpath.write_text(json.dumps(audit,indent=2)+'\n',encoding='utf8')
scope='2048 getter pairs: sixteen valid boolean combinations of user/code pause and the two int16 active fields (only0/1), each under128 guard patterns. Native AL contract compared without upper-EAX or invalid active-state demands. Independent truth table, full524-byte guarded typed fixture/18-byte timer model, per-leg FP payload and exact target switching; all17692 hooks/tables/flags, timer and external FP restored. Typed default constructor path PDB reviewed, null EntityRef excludes external registration. No sound assets/manager, audio playback, constructor/destructor or current dependency accreditation.'
summary=dict(passed=True,pairs=2048,differences=0,function='CAEFrontendAudioEntity::IsRadioTuneSoundActive',address='0x004DD480',type='S',scope=scope,dependency_review=None,manifest=manifest,run=run.relative_to(ROOT).as_posix(),audit_path=auditpath.relative_to(ROOT).as_posix(),audit_sha256=sha(auditpath),**{k:audit[k] for k in ('summary_sha256','log_sha256','harness_sha256','constructor_proof_sha256')})
path=HERE/'validation-summary.json';path.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);data=json.loads(registry.read_text(encoding='utf8'));assert len(data['functions'])==6858
hook=dict(fn_name='IsRadioTuneSoundActive',address='0x004DD480',type='S');hook['class']='CAEFrontendAudioEntity';assert not any(x['hook']==hook for x in data['functions'])
data['functions'].append(dict(hook=hook,evidence=dict(path=path.relative_to(ROOT).as_posix(),sha256=sha(path),passed_path=['passed'],pairs=2048,original_exe_sha256='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'),tested_sources=manifest['tested_sources'],source_provenance='Exact compiled manifest/matching PDB default fixture path reviewed; fresh canonical EXE bytes/fullassembly and isolated native replay independently checked via complete byte model/truth table.',dependency_review=None,scope=scope,review_reference='reversing/RequestedFrontendRadioGetterV2/corpus-plan.json'))
temp=registry.with_suffix('.small-frontend-radio.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=6858,after_count=len(data['functions']),registry_sha256=sha(registry),summary_sha256=sha(path),audit_sha256=sha(auditpath),compiled_source_sha256=manifest['tested_sources']['source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp']),indent=2))
