"""One-shot historical registration after independently audited full native replay."""
import hashlib,json,os
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
run=HERE/'runs/shake/20261008-160945-068337'
audit=json.loads((run/'independent-corrected-audit.json').read_text());report=json.loads((run/'enriched-summary.json').read_text())
assert audit['passed'] is True and audit['pairs']==89600 and audit['differences']==0
assert audit['summary_sha256']==sha(run/'enriched-summary.json')
scope='89600 isolated StartShake cases: product 4 delays x 4 deadline relations x 8 uint32 clocks x 5 stored frequencies x 5 requested frequencies x 7 signed duration pairs x 4 frontend/cutscene gate states. Full guarded private pad/controllers, readonly globals, both inactive physical scratch arenas and FP state compared without byte normalization; exact switches and native helper gates. Negative durations and wrapped clocks are declared characterization inputs. No real controller, operating-system vibration, actor integration, helper or general dependency accreditation.'
summary=dict(passed=True,function='CPad::StartShake',address='0x0053F920',type='S',pairs=89600,differences=0,scope=scope,dependency_review=None,run=str(run.relative_to(ROOT)).replace('\\','/'),asi_sha256=report['manifest']['asi'],pdb_sha256=report['manifest']['pdb'],tested_sources=report['manifest']['tested_sources'],audit_path=str((run/'independent-corrected-audit.json').relative_to(ROOT)).replace('\\','/'),audit_sha256=sha(run/'independent-corrected-audit.json'),enriched_summary_sha256=sha(run/'enriched-summary.json'),harness_sha256=sha(run/'harness.jsonl'),wire_sha256=audit['wire_sha256'],canonical_sha256=audit['canonical_sha256'],baseline_transport_audit='reversing/RequestedPadMutationRemaining3Hex/Diagnostics/baseline-transport-audit.json',baseline_transport_audit_sha256=sha(HERE/'Diagnostics/baseline-transport-audit.json'),baseline_differences=17800,natural_exit0=True,all_ranges_flags_restored=True)
path=HERE/'validation-summary-shake.json';path.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);data=json.loads(registry.read_text(encoding='utf8'))
hook=dict(class_='CPad',fn_name='StartShake',address='0x0053F920',type='S');hook['class']=hook.pop('class_')
assert not any(x['hook']==hook for x in data['functions'])
assert len(data['functions'])==6843
data['functions'].append(dict(hook=hook,evidence=dict(path=str(path.relative_to(ROOT)).replace('\\','/'),sha256=sha(path),passed_path=['passed'],pairs=89600,original_exe_sha256='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'),tested_sources=report['manifest']['tested_sources'],source_provenance='Exact corrected compiled manifest, current mapped body/scratch proof, frozen strict oracle reexecuted offline, full original outputs independently matched preserved baseline; lossless Hex/Phase transport verified over all 89600 pairs.',dependency_review=None,scope=scope,review_reference='reversing/RequestedPadMutationRemaining3Hex/DESIGN.md'))
temp=registry.with_suffix('.small-startshake.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf8')
assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=6843,after_count=len(data['functions']),registry_sha256=sha(registry),summary_sha256=sha(path),source_pad_sha256=report['manifest']['tested_sources']['source/game_sa/Pad.cpp']),indent=2))
