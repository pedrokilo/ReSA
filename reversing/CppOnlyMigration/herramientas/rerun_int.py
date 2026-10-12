import subprocess,sys,os,re,json,shutil
from pathlib import Path
ROOT=Path(r'D:\Original\gta-reversed-master')
PY=str(ROOT/r'reversing\CVehicle\tooling\Scripts\python.exe')
if not os.path.exists(PY): PY=sys.executable
env=dict(os.environ,CMAKE_BUILD_PARALLEL_LEVEL='2',_CL_='-MP2')
GAME=r'E:/JUEGOS/GTA San Andreas - CVehicle Test'
def run(cmd): return subprocess.run(cmd,cwd=str(ROOT),env=env,capture_output=True,text=True,encoding='utf-8',errors='replace')
res={}
for bank,kind,prefix in [x for x in (('CppIntegerWorld4',4,'cppintegerworld4'),('CppIntegerWorld10',10,'cppintegerworld10')) if x[0] in sys.argv[1:]]:
    r={'bank':bank}
    p=run([PY,'-B',f'reversing/{bank}/build_probe.py']); r['build']=p.returncode
    if p.returncode: r['err']=(p.stdout+p.stderr)[-400:]; res[bank]=r; print(json.dumps(r),flush=True); continue
    p=run([PY,'-B',f'reversing/{bank}/run_local.py']); out=p.stdout+p.stderr
    r['run']=p.returncode; r['head']=out.strip().splitlines()[0] if out.strip() else ''
    m=re.search(r'Informe: (.+)',out)
    if not m or p.returncode: r['err']=out[-400:]; res[bank]=r; print(json.dumps(r),flush=True); continue
    folder=Path(m.group(1).strip()); ts=folder.name
    asi=ROOT/f'reversing/{bank}/artifacts/gta_reversed-{prefix}-probe.asi'
    common=['--kind',str(kind),'--manifest',str(ROOT/f'reversing/{bank}/artifacts/build-manifest.json'),'--asi',str(asi),'--repo',str(ROOT),'--bank',str(ROOT/f'reversing/{bank}'),'--runner-summary',str(folder/'summary.json'),'--game',GAME]
    a=run([PY,'-B','reversing/CppIntegerMinimalStudy/audit_runtime.py',str(folder/'comparisons.jsonl')]+common)
    r['audit']=a.returncode
    if a.returncode: r['err']=(a.stdout+a.stderr)[-600:]
    else:
        j=json.loads(a.stdout)
        for k in ('functions','pairs','differences','null_passes','state_restored'): r[k]=j.get(k)
        out_dir=ROOT/f'reversing/{bank}/runs/{ts}'
        z=run([PY,'-B','reversing/CppIntegerMinimalStudy/archive_evidence.py',str(folder/'comparisons.jsonl'),'--out',str(out_dir)]+common)
        r['archive']=z.returncode
        if z.returncode: r['err']=(z.stdout+z.stderr)[-600:]
        else: (out_dir/'independent-audit.json').write_text(a.stdout,encoding='utf-8')
    res[bank]=r; print(json.dumps(r),flush=True)
