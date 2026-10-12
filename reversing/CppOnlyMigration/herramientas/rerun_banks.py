import subprocess,sys,os,re,json,time
ROOT=r'D:\Original\gta-reversed-master'
PY=ROOT+r'\reversing\CVehicle\tooling\Scripts\python.exe'
if not os.path.exists(PY): PY=sys.executable
env=dict(os.environ,CMAKE_BUILD_PARALLEL_LEVEL='2',_CL_='-MP2')
banks=sys.argv[1:]
res={}
for b in banks:
    t0=time.time(); r={'bank':b}
    def run(cmd,**k):
        return subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',**k)
    p=run([PY,'-B',f'reversing/{b}/build_probe.py'])
    r['build']=p.returncode
    if p.returncode: r['err']=(p.stdout+p.stderr)[-400:]; res[b]=r; print(json.dumps(r),flush=True); continue
    p=run([PY,'-B',f'reversing/{b}/run_local.py'])
    out=p.stdout+p.stderr
    r['run']=p.returncode; r['head']=out.strip().splitlines()[0] if out.strip() else ''
    m=re.search(r'Informe: (.+)',out)
    if not m: r['err']=out[-400:]; res[b]=r; print(json.dumps(r),flush=True); continue
    folder=m.group(1).strip(); r['folder']=folder
    if p.returncode==0:
        ap=f'reversing/{b}/audit/audit_run.py' if os.path.exists(ROOT+'/'+f'reversing/{b}/audit/audit_run.py') else f'reversing/{b}/audit_run.py'
        a=run([PY,'-B',ap,b,folder,'--archive'])
        r['audit']=a.returncode
        txt=a.stdout
        for k in ('pairs','differences','null_comparison_records','state_restored','saves_unchanged','configs_restored','hook_routes_verified'):
            mm=re.search(r'"%s": ([^,\n]+)'%k,txt)
            if mm: r[k]=mm.group(1)
        if a.returncode: r['err']=(a.stdout+a.stderr)[-500:]
    r['secs']=round(time.time()-t0)
    res[b]=r; print(json.dumps(r),flush=True)
json.dump(res,open(os.environ.get('TEMP','.')+r'\rerun_banks_result.json','w'),indent=1)
