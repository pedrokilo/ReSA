"""Read-only Markdown/frozen-epoch inventory. Writes diagnostics only; no prefix mutation."""
import hashlib,json,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3];OUT=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
files=subprocess.check_output(['rg','--files','--hidden','--no-ignore','-g','!.git','-g','!build*','-g','!thirdparty'],cwd=ROOT,text=True).splitlines()
visible=set(p.replace('\\','/') for p in subprocess.check_output(['rg','--files','--hidden','-g','!.git'],cwd=ROOT,text=True).splitlines())
md=sorted(p.replace('\\','/') for p in files if p.lower().endswith('.md'))
refs={};scanned=0

def visit(x,reference):
    if isinstance(x,dict):
        for k,v in x.items():
            if (k=='bank_sources' or (k in ('inputs','instruments','files') and ('freeze' in reference.lower() or 'frozen' in reference.lower()))) and isinstance(v,dict):
                for path,h in v.items():
                    if isinstance(h,str) and re.fullmatch('[0-9a-fA-F]{64}',h) and path.lower().endswith('.md'):
                        normalized=path.replace('\\','/')
                        if (k=='files' or not (ROOT/normalized).exists()) and (ROOT/Path(reference).parent/normalized).exists():normalized=(Path(reference).parent/normalized).as_posix()
                        refs.setdefault(normalized,[]).append({'reference':reference,'expected_sha256':h.lower()})
            visit(v,reference)
    elif isinstance(x,list):
        for value in x:visit(value,reference)
for name in files:
    if not name.lower().endswith('.json') or not name.replace('\\','/').startswith('reversing/'):continue
    p=ROOT/name
    try:data=json.loads(p.read_text(encoding='utf-8-sig'))
    except (ValueError,UnicodeError,OSError):continue
    scanned+=1;visit(data,name.replace('\\','/'))
frozen={};epochs={}
for path,entries in refs.items():
    p=ROOT/path
    if not p.exists():epochs[path]={'missing':True,'references':entries};continue
    actual=sha(p);matches=[x for x in entries if x['expected_sha256']==actual];older=[x for x in entries if x['expected_sha256']!=actual]
    if matches:frozen[path]={'actual_sha256':actual,'exact_references':matches}
    if older:epochs[path]={'actual_sha256':actual,'older_references':older,'policy':'Historical epoch only: never replace live bytes or rewrite recorded hashes; resolve archived mapping explicitly.'}
excluded={p:sha(ROOT/p) for p in md if any(part in ('artifacts','preparation-archive','instrumentation-archive','baseline','build','thirdparty','node_modules','.venv','venv','vendor','dependencies','_deps','runs','original-rev2-inputs','original-rev3-inputs','original-rev4-inputs','original-rev5-inputs') or part.startswith(('baseline-','build-','cmake-build','original-rev')) for part in Path(p).parts)}
result={'snapshot':{'historical':6843,'new_approvals':107,'pending':1603,'pending_groups':61,'current':0},'reference_commit':'ef0654e8','read_only_inventory':True,'all_markdown':md,'all_count':len(md),'json_scanned':scanned,'exact_frozen_markdown':frozen,'historical_epoch_references':epochs,'excluded_evidence_markdown':excluded,'eligible_current_markdown':[p for p in md if p in visible and p not in frozen and p not in excluded]}
(OUT/'documentation-plan-107.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf8')
print(json.dumps({'md':len(md),'eligible':len(result['eligible_current_markdown']),'frozen':len(frozen),'epoch_paths':len(epochs),'json_scanned':scanned,'new_bank_exceptions':[p for p in frozen if any(s in p for s in ('Cached','Phased','Hex','SourceProposals','ArrestRemaining','FxEmitterTwo','Dispatch','DistanceTrainNop'))]},indent=2))
