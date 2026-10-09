"""Offline reaudit of the approved ProcessPad run (20261009-011023-884156), then one scoped historical record for CPad::ProcessPad (dependency_review=null)."""
import gzip,hashlib,importlib.util,json,os,shutil,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1];RUN_ID='20261009-011023-884156';RUN=HERE/'runs'/RUN_ID
EXTERNAL=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
report=json.loads((RUN/'enriched-summary.json').read_text(encoding='utf-8'));manifest=report['manifest'];summary=json.loads((RUN/'summary.json').read_text(encoding='utf-8'))
TOTAL=17256
assert report['passed'] is True and report['natural_exit'] is True and report['exit_code']==0 and report['comparisons']==TOTAL and report['differences']==0 and report['model_mismatches']==0 and report['errors']==[]
assert report['saves_unchanged'] is True and report['configs_restored'] is True and report['return_eax_model_mismatches_original']==0 and report['sincos_one_ulp_tolerated_legs']==0
for key in ('passed','comparisons','differences','model_mismatches','errors','executed_probe_sha256','comparison_sha256','harness_sha256','saves_unchanged','configs_restored'):assert summary[key]==report[key],key
assert report['executed_probe_sha256'].startswith('d8807e7d') and report['original_exe_sha256'].startswith('72ae59e4') and report['original_exe_sha256']==manifest['exe_sha256']
for group in ('tested_sources','bank_sources'):
    for p,h in manifest[group].items():assert sha(ROOT/p)==h,p
assert sha(HERE/'artifacts/gta_reversed-pad-process-pad.asi')==manifest['asi']==report['executed_probe_sha256']
assert sha(HERE/'artifacts/gta_reversed-pad-process-pad.pdb')==manifest['pdb']
assert sha(EXTERNAL/'_baseline/gta_reversed-pad-process-pad-probe.pdb')==manifest['pdb']
proofpath=HERE/'Diagnostics/compiled-runtime-proof.json';proof=json.loads(proofpath.read_text(encoding='utf-8'))
assert proof['passed'] is True and proof['pdb_rsds_match'] is True and proof['event_call_paths_verified'] is True and proof['inline_getpad_and_float_paths_reviewed'] is True and proof['failures']==[] and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb'] and proof['pov_float_path']['equivalent_to_exe'] is True
assert sha(proofpath)==report['compiled_callpath_proof_sha256']
# Hashes of the preserved run against the external run directory.
SRC=EXTERNAL/'resultados-pad-process-pad'/RUN_ID
assert sha(SRC/'comparisons.jsonl')==sha(RUN/'comparisons.jsonl')==report['comparison_sha256']==report['comparison_storage']['uncompressed_sha256']
assert sha(SRC/'summary.json')==sha(RUN/'summary.json') and sha(SRC/'RESULTADO.txt')==sha(RUN/'RESULTADO.txt')
assert sha(RUN/'harness.jsonl')==report['harness_sha256'] and sha(RUN/'comparisons.jsonl.gz')==report['comparison_storage']['compressed_sha256']
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(4*1024*1024),b''):digest.update(block)
assert digest.hexdigest()==report['comparison_sha256']
# Rejected first attempt (622 differences) stays untouched, referenced by hash.
rejected=HERE/'runs/20261009-010223-123803/enriched-summary.json';rejected_report=json.loads(rejected.read_text(encoding='utf-8'));assert rejected_report['passed'] is False and rejected_report['differences']==622
# Independent offline reaudit of the full preserved payload with the frozen analyzer and model.
sys.path.insert(0,str(HERE))
spec=importlib.util.spec_from_file_location('process_pad_audit',HERE/'run_local.py');bank=importlib.util.module_from_spec(spec);spec.loader.exec_module(bank)
with tempfile.TemporaryDirectory() as temp:
    bank.runner.GAME=Path(temp);(Path(temp)/'preflight-pad-process-pad-harness.jsonl').write_bytes((RUN/'harness.jsonl').read_bytes())
    result=bank.analyze(RUN/'comparisons.jsonl')
assert result and result['passed'] and result['comparisons']==TOTAL and result['differences']==0 and result['model_mismatches']==0 and result['errors']==[]
assert result['comparison_sha256']==report['comparison_sha256'] and result['harness_sha256']==report['harness_sha256']
for key in ('return_eax_differences_informational','x87_env_status_differences_informational','sincos_one_ulp_tolerated_legs','return_eax_model_mismatches_original'):assert result[key]==report[key],key
audit=dict(result,summary_sha256=sha(RUN/'enriched-summary.json'),manifest_head=manifest['head'],asi_sha256=manifest['asi'],pdb_sha256=manifest['pdb'],compiled_runtime_proof_sha256=sha(proofpath),natural_exit=True,exit_code=0,snapshot_ranges=17692,independent_oracle='model.py (independent rational model, frozen in the manifest) re-executed offline over the preserved full payload',rejected_attempts={rel(rejected):sha(rejected)})
diag=HERE/'Diagnostics';audit_path=diag/'independent-replay-audit.json';assert not audit_path.exists();audit_path.write_text(json.dumps(audit,indent=2)+'\n',encoding='utf-8')
pad_sha=manifest['tested_sources']['source/game_sa/Pad.cpp']
scope=('17256 isolated CPU pairs for CPad::ProcessPad (EXE 0x00746A10, cdecl), simple preflight-style pair bank (original with hook off vs candidate with hook on) plus an independent model; zero differences, zero model mismatches, errors [], natural exit 0, saves unchanged, configs restored, harness/state/entries/flags/tables restored (17692 snapshot ranges, no changed ranges). '
 'Corpus (scope.json, corpus-plan.json): 11 device routes, 4 side-effect flips, x87 PC24 and PC53, 64 menu combinations, 4 joystick masks, 12 curated vectors, 22 boundary values per axis (599/600/601, INT_MIN...), 41 POV values and 1536 pseudo-random vectors (documented LCG); 17280 rows minus 24 exact duplicates. '
 'Limits: private scripted DirectInput device (Poll, Acquire with the INPUTLOST retry, GetDeviceState 272 bytes), no real DirectInput/COM; RsPadEventHandler is traced (events 0x20, 0x20, 0x1F with the value of *param); GetPad and FTOL are native on the EXE leg, and in the compiled candidate GetPad is a real call to the DLL CPad::GetPad (not inlined) and the float to int conversion is an inline cvttss2si, so there is no credit for GetPad, FTOL, RsPadEventHandler, DirectInput/COM, ControlsManager, input integration, gameplay or current dependencies; '
 'finite stick floats only (no NaN; the compiled comiss deadzone test proceeds on NaN where the EXE skips, unreachable because sticks come from fild*0.0005); x87 PC24 and PC53, round-nearest, exceptions masked; padID other than 0/1 is outside the corpus (the EXE dereferences NULL at 0x746A3A while the source returns); bJoyJustInitialised only 0/1; menu bytes and flags canonical booleans. '
 'The EXE returns 0 or the failing HRESULT and the source is void: the EAX difference (16392 pairs) and the x87 status-word differences (15816 pairs) are informational and are not counted as differences; the original EAX matched the model in all pairs. '
 'Source corrections in Pad.cpp made before this run: (1) POV: signed fild + cmp/jge + fadd of the 2^32 float before the two fmul, as the EXE (0x746B32..0x746B44), verified in the compiled ASI (Diagnostics/compiled-runtime-proof.json passed=true); (2) the if (*pDiDevice) now wraps only the stick block (POV and Z/RZ): events 0x20/0x1F, GetPad and axis processing are unconditional as in the EXE (0x746B00 je 0x746BA6). A first run (20261009-010223-123803) found 622 differences (all with the device nulled) before correction (2) and is rejected, referenced by hash. '
 'The current Pad.cpp (SHA-256 '+pad_sha+') differs from the one tested in the earlier CPad records (StartShake_Distance/Train, ProcessPCSpecificStuff, UpdateMouse: 89badf36...) because of these ProcessPad corrections; the behaviour of those functions does not change and their records are untouched. '
 'The prepared manifest still says expected_pairs 17000 (stale label); the real corpus and the analyzer use 17256. Historical evidence approved with explicit limits; no current dependency accreditation.')
summary_out=dict(passed=True,pairs=TOTAL,differences=0,model_mismatches=0,functions={'CPad::ProcessPad':TOTAL},scope=scope,dependency_review=None,run=rel(RUN),audit_path=rel(audit_path),audit_sha256=sha(audit_path),manifest=manifest,summary_sha256=sha(RUN/'summary.json'),enriched_summary_sha256=sha(RUN/'enriched-summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),harness_sha256=report['harness_sha256'],log_sha256=report['comparison_sha256'],comparison_storage=report['comparison_storage'],compiled_runtime_proof=rel(proofpath),compiled_runtime_proof_sha256=sha(proofpath),natural_exit=True,exit_code=0,
 informational=dict(return_eax_differences=report['return_eax_differences_informational'],x87_env_status_differences=report['x87_env_status_differences_informational']),
 pad_cpp_sha256=pad_sha,pad_cpp_note='la fuente actual de Pad.cpp difiere de la probada en las corridas anteriores de CPad por las correcciones de ProcessPad (POV y puerta *pDiDevice); el comportamiento de esas funciones no cambia',
 source_corrections_before_run=['POV: signed fild + cmp/jge + fadd 2^32 float before both fmul (0x746B32..0x746B44), verified in the compiled ASI','if (*pDiDevice) wraps only the stick block; events 0x20/0x1F, GetPad and axis processing unconditional (0x746B00 je 0x746BA6)'],
 rejected_attempts={rel(rejected):sha(rejected)})
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(summary_out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-pad-process-pad.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions'])
hook={'class':'CPad','fn_name':'ProcessPad','address':'0x00746A10','type':'S'}
assert not any(x['hook']['class']=='CPad' and x['hook']['fn_name']=='ProcessPad' for x in data['functions'])
data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['passed'],pairs=TOTAL,original_exe_sha256=report['original_exe_sha256']),tested_sources=manifest['tested_sources'],
 source_provenance='Exact compiled ProcessPad manifest, matching ASI/PDB (RSDS) and static call-path/float-path proof (RsPadEventHandler thunk chain, DLL GetPad call, inline cvttss2si, POV fild/cmp/jge/fadd sequence); frozen analyzer and independent model reexecuted offline over the full preserved payload. A first run with 622 differences (device nulled) is preserved as rejected; Pad.cpp was corrected before the approved run.',dependency_review=None,scope=scope,review_reference='reversing/RequestedPadProcessPad/README.md'))
assert sha(registry)==before,'el registro cambio durante la preparacion; reintentar'
temp=registry.with_suffix('.process-pad.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions'])),indent=2),json.dumps(dict(registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp),audit_sha256=sha(audit_path)),indent=2))
